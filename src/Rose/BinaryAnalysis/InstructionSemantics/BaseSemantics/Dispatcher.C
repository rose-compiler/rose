#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/Dispatcher.h>

#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/Exception.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/RegisterStateGeneric.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/RiscOperators.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/State.h>
#include <Rose/BinaryAnalysis/RegisterDictionary.h>

#include <boost/scope_exit.hpp>

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics {
namespace BaseSemantics {

Dispatcher::Dispatcher()
    : addrWidth_(0), autoResetInstructionPointer_(true) {}

Dispatcher::Dispatcher(size_t addrWidth, const RegisterDictionary::Ptr &regs)
    : regdict(regs), addrWidth_(addrWidth), autoResetInstructionPointer_(true) {}

Dispatcher::Dispatcher(const RiscOperators::Ptr &ops, size_t addrWidth, const RegisterDictionary::Ptr &regs)
    : operators_(ops), regdict(regs), addrWidth_(addrWidth), autoResetInstructionPointer_(true) {
    ASSERT_not_null(operators_);
    ASSERT_not_null(regs);
}

Dispatcher::~Dispatcher() {
    for (InsnProcessors::iterator iter = iproc_table.begin(); iter != iproc_table.end(); ++iter)
        delete *iter;
}

void
Dispatcher::operators(const RiscOperatorsPtr &ops) {
    ASSERT_not_null(ops);
    operators_ = ops;
}

StatePtr
Dispatcher::currentState() const {
    return operators() ? operators()->currentState() : StatePtr();
}

SValuePtr
Dispatcher::protoval() const {
    return operators() ? operators()->protoval() : SValuePtr();
}

SgAsmInstruction*
Dispatcher::currentInstruction() const {
    return operators() ? operators()->currentInstruction() : NULL;
}

SValuePtr
Dispatcher::undefined_(size_t nBits) const {
    ASSERT_not_null(operators());
    return operators()->undefined_(nBits);
}

SValuePtr
Dispatcher::unspecified_(size_t nBits) const {
    ASSERT_not_null(operators());
    return operators()->unspecified_(nBits);
}

SValuePtr
Dispatcher::number_(size_t nbits, uint64_t number) const {
    ASSERT_not_null(operators());
    return operators()->number_(nbits, number);
}

RegisterDictionary::Ptr
Dispatcher::registerDictionary() const {
    return get_register_dictionary();
}

void
Dispatcher::registerDictionary(const RegisterDictionary::Ptr &rd) {
    set_register_dictionary(rd);
}

RegisterDictionary::Ptr
Dispatcher::get_register_dictionary() const {
    return regdict;
}

void
Dispatcher::set_register_dictionary(const RegisterDictionary::Ptr &rd) {
    regdict = rd;
}

void
Dispatcher::advanceInstructionPointer(SgAsmInstruction *insn) {
    operators()->comment("advancing instruction pointer");
    RegisterDescriptor ipReg = instructionPointerRegister();
    size_t nBits = ipReg.nBits();
    BaseSemantics::SValuePtr ipValue;
    if (!autoResetInstructionPointer_ && operators()->currentState() && operators()->currentState()->registerState()) {
        BaseSemantics::RegisterStateGenericPtr grState =
            boost::dynamic_pointer_cast<BaseSemantics::RegisterStateGeneric>(operators()->currentState()->registerState());
        if (grState && grState->is_partly_stored(ipReg))
            ipValue = operators()->readRegister(ipReg);
    }
    if (!ipValue)
        ipValue = operators()->number_(nBits, insn->get_address());
    ipValue = operators()->add(ipValue, operators()->number_(nBits, insn->get_size()));
    operators()->writeRegister(ipReg, ipValue);
}

void
Dispatcher::addressWidth(size_t nBits) {
    ASSERT_require2(nBits==addrWidth_ || addrWidth_==0, "address width cannot be changed once it is set");
    addrWidth_ = nBits;
}

void
Dispatcher::processInstruction(SgAsmInstruction *insn)
{
    operators()->startInstruction(insn);
    BOOST_SCOPE_EXIT(this_) {
        this_->operators()->currentInstruction(nullptr);
    } BOOST_SCOPE_EXIT_END;

    InsnProcessor *iproc = iprocLookup(insn);
    try {
        if (!iproc)
            throw Exception("no dispatch ability for \"" + insn->get_mnemonic() + "\" instruction", insn);
        iproc->process(shared_from_this(), insn);
    } catch (Exception &e) {
        // If the exception was thrown by something that didn't have an instruction available, then add the instruction
        if (!e.insn)
            e.insn = insn;
        e.insn->incrementSemanticFailure();
        throw e;
    }
    operators()->finishInstruction(insn);
}

InsnProcessor *
Dispatcher::iprocLookup(SgAsmInstruction *insn)
{
    int key = iprocKey(insn);
    ASSERT_require(key>=0);
    return iprocGet(key);
}

void
Dispatcher::iprocReplace(SgAsmInstruction *insn, InsnProcessor *iproc)
{
    iprocSet(iprocKey(insn), iproc);
}

void
Dispatcher::iprocSet(int key, InsnProcessor *iproc)
{
    ASSERT_require(key>=0);
    if ((size_t)key>=iproc_table.size())
        iproc_table.resize(key+1, NULL);
    iproc_table[key] = iproc;
}

InsnProcessor *
Dispatcher::iprocGet(int key)
{
    if (key<0 || (size_t)key>=iproc_table.size())
        return NULL;
    return iproc_table[key];
}

RegisterDescriptor
Dispatcher::findRegister(const std::string &regname, size_t nbits/*=0*/, bool allowMissing) const
{
    RegisterDictionary::Ptr regdict = registerDictionary();
    if (!regdict)
        throw Exception("no register dictionary", currentInstruction());

    const RegisterDescriptor reg = regdict->find(regname);
    if (!reg) {
        if (allowMissing)
            return reg;
        std::ostringstream ss;
        ss <<"Invalid register \"" <<regname <<"\" in dictionary \"" <<regdict->name() <<"\"";
        throw Exception(ss.str(), currentInstruction());
    }

    if (nbits>0 && reg.nBits()!=nbits) {
        std::ostringstream ss;
        ss <<"Invalid " <<nbits <<"-bit register: \"" <<regname <<"\" is "
           <<reg.nBits() <<" " <<(1==reg.nBits()?"byte":"bytes");
        throw Exception(ss.str(), currentInstruction());
    }
    return reg;
}

void
Dispatcher::initializeState(const StatePtr&) {}

void
Dispatcher::decrementRegisters(SgAsmExpression *e)
{
    struct T1: AstSimpleProcessing {
        RiscOperatorsPtr ops;
        T1(const RiscOperatorsPtr &ops): ops(ops) {}
        void visit(SgNode *node) {
            if (SgAsmRegisterReferenceExpression *rre = isSgAsmRegisterReferenceExpression(node)) {
                RegisterDescriptor reg = rre->get_descriptor();
                if (rre->get_adjustment() < 0) {
                    SValuePtr adj = ops->number_(64, (int64_t)rre->get_adjustment());
                    if (reg.nBits() <= 64) {
                        adj = ops->unsignedExtend(adj, reg.nBits());  // truncate
                    } else {
                        adj = ops->signExtend(adj, reg.nBits());      // extend
                    }
                    ops->writeRegister(reg, ops->add(ops->readRegister(reg), adj));
                }
            }
        }
    } t1(operators());
    t1.traverse(e, preorder);
}

void
Dispatcher::incrementRegisters(SgAsmExpression *e)
{
    struct T1: AstSimpleProcessing {
        RiscOperatorsPtr ops;
        T1(const RiscOperatorsPtr &ops): ops(ops) {}
        void visit(SgNode *node) {
            if (SgAsmRegisterReferenceExpression *rre = isSgAsmRegisterReferenceExpression(node)) {
                RegisterDescriptor reg = rre->get_descriptor();
                if (rre->get_adjustment() > 0) {
                    SValuePtr adj = ops->unsignedExtend(ops->number_(64, (int64_t)rre->get_adjustment()), reg.nBits());
                    ops->writeRegister(reg, ops->add(ops->readRegister(reg), adj));
                }
            }
        }
    } t1(operators());
    t1.traverse(e, preorder);
}

void
Dispatcher::preUpdate(SgAsmExpression *e, const BaseSemantics::SValuePtr &enabled) {
    struct T1: AstSimpleProcessing {
        Dispatcher *self;
        BaseSemantics::SValuePtr enabled;
        T1(Dispatcher *self, const BaseSemantics::SValuePtr &enabled)
            : self(self), enabled(enabled) {}
        void visit(SgNode *node) override {
            if (SgAsmBinaryPreupdate *op = isSgAsmBinaryPreupdate(node)) {
                if (enabled->isTrue()) {
                    // Definitely updating, therefore not necessary to read old value
                    BaseSemantics::SValuePtr newValue = self->read(op->get_rhs());
                    self->write(op->get_lhs(), newValue);
                } else if (!enabled->isConcrete() || enabled->toUnsigned().get() != 0) {
                    // Maybe updating (but not "definitely not updating")
                    BaseSemantics::SValuePtr oldValue = self->read(op->get_lhs());
                    BaseSemantics::SValuePtr newValue = self->read(op->get_rhs());
                    BaseSemantics::SValuePtr maybe = self->operators()->ite(enabled, newValue, oldValue);
                    self->write(op->get_lhs(), maybe);
                }
            }
        }
    } t1(this, enabled);
    t1.traverse(e, postorder);
}

void
Dispatcher::postUpdate(SgAsmExpression *e, const BaseSemantics::SValuePtr &enabled) {
    struct T1: AstSimpleProcessing {
        Dispatcher *self;
        BaseSemantics::SValuePtr enabled;
        T1(Dispatcher *self, const BaseSemantics::SValuePtr &enabled)
            : self(self), enabled(enabled) {}
        void visit(SgNode *node) override {
            if (SgAsmBinaryPostupdate *op = isSgAsmBinaryPostupdate(node)) {
                if (enabled->isTrue()) {
                    // Definitely updating, therefore not necessary to read old value
                    BaseSemantics::SValuePtr newValue = self->read(op->get_rhs());
                    self->write(op->get_lhs(), newValue);
                } else if (!enabled->isConcrete() || enabled->toUnsigned().get() != 0) {
                    // Maybe updating (but not "definitely not updating")
                    BaseSemantics::SValuePtr oldValue = self->read(op->get_lhs());
                    BaseSemantics::SValuePtr newValue = self->read(op->get_rhs());
                    BaseSemantics::SValuePtr maybe = self->operators()->ite(enabled, newValue, oldValue);
                    self->write(op->get_lhs(), maybe);
                }
            }
        }
    } t1(this, enabled);
    t1.traverse(e, postorder);
}
    
SValuePtr
Dispatcher::effectiveAddress(SgAsmExpression *e, size_t nbits/*=0*/)
{
    BaseSemantics::SValuePtr retval;
#if 1 // DEBUGGING [Robb P. Matzke 2015-08-04]
    ASSERT_always_require(retval==NULL);
#endif
    if (0==nbits)
        nbits = addressWidth();

    if (SgAsmMemoryReferenceExpression *mre = isSgAsmMemoryReferenceExpression(e)) {
        retval = effectiveAddress(mre->get_address(), nbits);
    } else if (SgAsmRegisterReferenceExpression *rre = isSgAsmRegisterReferenceExpression(e)) {
        retval = read(rre, nbits);
    } else if (SgAsmBinaryAdd *op = isSgAsmBinaryAdd(e)) {
        BaseSemantics::SValuePtr lhs = effectiveAddress(op->get_lhs(), nbits);
        BaseSemantics::SValuePtr rhs = effectiveAddress(op->get_rhs(), nbits);
        retval = operators()->add(lhs, rhs);
    } else if (SgAsmBinarySubtract *op = isSgAsmBinarySubtract(e)) {
        BaseSemantics::SValuePtr lhs = effectiveAddress(op->get_lhs(), nbits);
        BaseSemantics::SValuePtr rhs = effectiveAddress(op->get_rhs(), nbits);
        retval = operators()->subtract(lhs, rhs);
    } else if (SgAsmBinaryPreupdate *op = isSgAsmBinaryPreupdate(e)) {
        // The add operation is ignored here, but performed by preUpdate.
        retval = effectiveAddress(op->get_lhs(), nbits);
    } else if (SgAsmBinaryPostupdate *op = isSgAsmBinaryPostupdate(e)) {
        // The add operation is ignored here, but performed by postUpdate.
        retval = effectiveAddress(op->get_lhs(), nbits);
    } else if (SgAsmBinaryMultiply *op = isSgAsmBinaryMultiply(e)) {
        BaseSemantics::SValuePtr lhs = effectiveAddress(op->get_lhs(), nbits);
        BaseSemantics::SValuePtr rhs = effectiveAddress(op->get_rhs(), nbits);
        retval = operators()->unsignedMultiply(lhs, rhs);
    } else if (SgAsmIntegerValueExpression *ival = isSgAsmIntegerValueExpression(e)) {
        retval = operators()->number_(ival->get_significantBits(), ival->get_value());
    } else if (SgAsmUnaryUnsignedExtend *op = isSgAsmUnaryUnsignedExtend(e)) {
        BaseSemantics::SValuePtr lhs = effectiveAddress(op->get_operand(), op->get_operand()->get_nBits());
        retval = operators()->unsignedExtend(lhs, op->get_nBits());
    } else if (SgAsmUnarySignedExtend *op = isSgAsmUnarySignedExtend(e)) {
        BaseSemantics::SValuePtr lhs = effectiveAddress(op->get_operand(), op->get_operand()->get_nBits());
        retval = operators()->signExtend(lhs, op->get_nBits());
    } else if (SgAsmUnaryTruncate *op = isSgAsmUnaryTruncate(e)) {
        BaseSemantics::SValuePtr lhs = effectiveAddress(op->get_operand(), op->get_operand()->get_nBits());
        retval = operators()->unsignedExtend(lhs, op->get_nBits()); // yes, can be used for truncation
    } else if (SgAsmBinaryLsl *op = isSgAsmBinaryLsl(e)) {
        BaseSemantics::SValuePtr lhs = effectiveAddress(op->get_lhs(), nbits);
        BaseSemantics::SValuePtr rhs = effectiveAddress(op->get_rhs(), nbits);
        retval = operators()->shiftLeft(lhs, rhs);
    } else if (SgAsmBinaryLsr *op = isSgAsmBinaryLsr(e)) {
        BaseSemantics::SValuePtr lhs = effectiveAddress(op->get_lhs(), nbits);
        BaseSemantics::SValuePtr rhs = effectiveAddress(op->get_rhs(), nbits);
        retval = operators()->shiftRight(lhs, rhs);
    } else if (SgAsmBinaryAsr *op = isSgAsmBinaryAsr(e)) {
        BaseSemantics::SValuePtr lhs = effectiveAddress(op->get_lhs(), nbits);
        BaseSemantics::SValuePtr rhs = effectiveAddress(op->get_rhs(), nbits);
        retval = operators()->shiftRightArithmetic(lhs, rhs);
    } else if (SgAsmBinaryRor *op = isSgAsmBinaryRor(e)) {
        BaseSemantics::SValuePtr lhs = effectiveAddress(op->get_lhs(), nbits);
        BaseSemantics::SValuePtr rhs = effectiveAddress(op->get_rhs(), nbits);
        retval = operators()->rotateRight(lhs, rhs);
    } else if (SgAsmUnaryUnsignedExtend *op = isSgAsmUnaryUnsignedExtend(e)) {
        BaseSemantics::SValuePtr lhs = effectiveAddress(op->get_operand(), op->get_operand()->get_nBits());
        retval = operators()->unsignedExtend(lhs, op->get_nBits());
    }

    ASSERT_not_null(retval);
    if (retval->nBits() < nbits) {
        retval = operators()->signExtend(retval, nbits);
    } else if (retval->nBits() > nbits) {
        retval = operators()->extract(retval, 0, nbits);
    }
    return retval;
}

RegisterDescriptor
Dispatcher::segmentRegister(SgAsmMemoryReferenceExpression *mre)
{
    if (mre!=NULL && mre->get_segment()!=NULL) {
        if (SgAsmRegisterReferenceExpression *rre = isSgAsmRegisterReferenceExpression(mre->get_segment())) {
            return rre->get_descriptor();
        }
    }
    return RegisterDescriptor();
}

void
Dispatcher::write(SgAsmExpression *e, const SValuePtr &value, size_t addr_nbits/*=0*/)
{
    ASSERT_not_null(e);
    ASSERT_not_null(value);
    if (SgAsmDirectRegisterExpression *re = isSgAsmDirectRegisterExpression(e)) {
        // If the expression type is different than the register lvalue, we need to adjust what part of the register we're writing.
        RegisterDescriptor reg = re->get_descriptor();
        if (reg.nBits() > e->get_nBits())
            reg = RegisterDescriptor(reg.majorNumber(), reg.minorNumber(), reg.offset(), e->get_nBits());
        operators()->writeRegister(reg, value);
    } else if (SgAsmIndirectRegisterExpression *re = isSgAsmIndirectRegisterExpression(e)) {
        SValuePtr offset = operators()->readRegister(re->get_offset());
        if (!offset->isConcrete()) {
            std::string offset_name = registerDictionary()->lookup(re->get_offset());
            offset_name = offset_name.empty() ? "" : "(" + offset_name + ") ";
            throw Exception("indirect register offset " + offset_name + "must have a concrete value", NULL);
        }
        size_t idx = (offset->toUnsigned().get() + re->get_index()) % re->get_modulus();
        RegisterDescriptor reg = re->get_descriptor();
        reg.majorNumber(reg.majorNumber() + re->get_stride().majorNumber() * idx);
        reg.minorNumber(reg.minorNumber() + re->get_stride().minorNumber() * idx);
        reg.offset(reg.offset() + re->get_stride().offset() * idx);
        reg.nBits(reg.nBits() + re->get_stride().nBits() * idx);
        operators()->writeRegister(reg, value);
    } else if (SgAsmMemoryReferenceExpression *mre = isSgAsmMemoryReferenceExpression(e)) {
        SValuePtr addr = effectiveAddress(mre, addr_nbits);
        ASSERT_require(0==addrWidth_ || addr->nBits()==addrWidth_);
        operators()->writeMemory(segmentRegister(mre), addr, value, operators()->boolean_(true));
    } else {
        ASSERT_not_implemented(e->class_name());
    }
}

SValuePtr
Dispatcher::read(SgAsmExpression *e, size_t value_nbits/*=0*/, size_t addr_nbits/*=0*/)
{
    ASSERT_not_null(e);
    if (0 == value_nbits) {
        SgAsmType *expr_type = e->get_type();
        ASSERT_not_null(expr_type);
        value_nbits = expr_type->get_nBits();
        ASSERT_require(value_nbits != 0);
    }

    SValuePtr retval;
    if (SgAsmDirectRegisterExpression *re = isSgAsmDirectRegisterExpression(e)) {
        retval = operators()->readRegister(re->get_descriptor());
    } else if (SgAsmIndirectRegisterExpression *re = isSgAsmIndirectRegisterExpression(e)) {
        SValuePtr offset = operators()->readRegister(re->get_offset());
        if (!offset->isConcrete()) {
            std::string offset_name = registerDictionary()->lookup(re->get_offset());
            offset_name = offset_name.empty() ? "" : "(" + offset_name + ") ";
            throw Exception("indirect register offset " + offset_name + "must have a concrete value", NULL);
        }
        size_t idx = (offset->toUnsigned().get() + re->get_index()) % re->get_modulus();
        RegisterDescriptor reg = re->get_descriptor();
        reg.majorNumber(reg.majorNumber() + re->get_stride().majorNumber() * idx);
        reg.minorNumber(reg.minorNumber() + re->get_stride().minorNumber() * idx);
        reg.offset(reg.offset() + re->get_stride().offset() * idx);
        reg.nBits(reg.nBits() + re->get_stride().nBits() * idx);
        retval = operators()->readRegister(reg);
    } else if (SgAsmMemoryReferenceExpression *mre = isSgAsmMemoryReferenceExpression(e)) {
        BaseSemantics::SValuePtr addr = effectiveAddress(mre, addr_nbits);
        ASSERT_require(0==addrWidth_ || addr->nBits()==addrWidth_);
        BaseSemantics::SValuePtr dflt = undefined_(value_nbits);
        retval = operators()->readMemory(segmentRegister(mre), addr, dflt, operators()->boolean_(true));
    } else if (SgAsmValueExpression *ve = isSgAsmValueExpression(e)) {
        uint64_t val = SageInterface::getAsmSignedConstant(ve);
        retval = operators()->number_(value_nbits, val);
    } else if (SgAsmBinaryAdd *sum = isSgAsmBinaryAdd(e)) {
        SgAsmExpression *lhs = sum->get_lhs();
        SgAsmExpression *rhs = sum->get_rhs();
        size_t nbits = std::max(lhs->get_nBits(), rhs->get_nBits());
        retval = operators()->add(operators()->signExtend(read(lhs, lhs->get_nBits(), addr_nbits), nbits),
                                  operators()->signExtend(read(rhs, rhs->get_nBits(), addr_nbits), nbits));
    } else if (SgAsmBinarySubtract *diff = isSgAsmBinarySubtract(e)) {
        SgAsmExpression *lhs = diff->get_lhs();
        SgAsmExpression *rhs = diff->get_rhs();
        size_t nbits = std::max(lhs->get_nBits(), rhs->get_nBits());
        retval = operators()->subtract(operators()->signExtend(read(lhs, lhs->get_nBits(), addr_nbits), nbits),
                                       operators()->signExtend(read(rhs, rhs->get_nBits(), addr_nbits), nbits));
    } else if (SgAsmBinaryMultiply *product = isSgAsmBinaryMultiply(e)) {
        SgAsmExpression *lhs = product->get_lhs();
        SgAsmExpression *rhs = product->get_rhs();
        retval = operators()->unsignedMultiply(read(lhs, lhs->get_nBits()), read(rhs, rhs->get_nBits()));
    } else if (SgAsmUnaryTruncate *op = isSgAsmUnaryTruncate(e)) {
        BaseSemantics::SValuePtr operand = read(op->get_operand(), op->get_operand()->get_nBits(), addr_nbits);
        retval = operators()->unsignedExtend(operand, op->get_nBits());
    } else if (SgAsmBinaryLsr *op = isSgAsmBinaryLsr(e)) {
        BaseSemantics::SValuePtr lhs = read(op->get_lhs(), op->get_lhs()->get_nBits(), addr_nbits);
        BaseSemantics::SValuePtr rhs = read(op->get_rhs(), op->get_rhs()->get_nBits(), addr_nbits);
        retval = operators()->shiftRight(lhs, rhs);
    } else if (SgAsmBinaryAsr *op = isSgAsmBinaryAsr(e)) {
        BaseSemantics::SValuePtr lhs = read(op->get_lhs(), op->get_lhs()->get_nBits(), addr_nbits);
        BaseSemantics::SValuePtr rhs = read(op->get_rhs(), op->get_rhs()->get_nBits(), addr_nbits);
        retval = operators()->shiftRightArithmetic(lhs, rhs);
    } else if (SgAsmBinaryLsl *op = isSgAsmBinaryLsl(e)) {
        BaseSemantics::SValuePtr lhs = read(op->get_lhs(), op->get_lhs()->get_nBits(), addr_nbits);
        BaseSemantics::SValuePtr rhs = read(op->get_rhs(), op->get_rhs()->get_nBits(), addr_nbits);
        retval = operators()->shiftLeft(lhs, rhs);
    } else if (SgAsmBinaryRor *op = isSgAsmBinaryRor(e)) {
        BaseSemantics::SValuePtr lhs = read(op->get_lhs(), op->get_lhs()->get_nBits(), addr_nbits);
        BaseSemantics::SValuePtr rhs = read(op->get_rhs(), op->get_rhs()->get_nBits(), addr_nbits);
        retval = operators()->rotateRight(lhs, rhs);
    } else if (SgAsmUnaryUnsignedExtend *op = isSgAsmUnaryUnsignedExtend(e)) {
        BaseSemantics::SValuePtr lhs = effectiveAddress(op->get_operand(), op->get_operand()->get_nBits());
        retval = operators()->unsignedExtend(lhs, op->get_nBits());
    } else if (SgAsmUnarySignedExtend *op = isSgAsmUnarySignedExtend(e)) {
        BaseSemantics::SValuePtr lhs = effectiveAddress(op->get_operand(), op->get_operand()->get_nBits());
        retval = operators()->signExtend(lhs, op->get_nBits());
    } else if (SgAsmBinaryPostupdate *op = isSgAsmBinaryPostupdate(e)) {
        return read(op->get_lhs(), value_nbits, addr_nbits);
    } else if (SgAsmBinaryPreupdate *op = isSgAsmBinaryPreupdate(e)) {
        return read(op->get_lhs(), value_nbits, addr_nbits);
    } else {
        ASSERT_not_implemented(e->class_name());
    }

    // Make sure the return value is the requested width. The unsignedExtend() can expand or shrink values.
    ASSERT_not_null(retval);
    if (retval->nBits()!=value_nbits)
        retval = operators()->unsignedExtend(retval, value_nbits);
    return retval;
}

} // namespace
} // namespace
} // namespace
} // namespace

BOOST_CLASS_EXPORT_IMPLEMENT(Rose::BinaryAnalysis::InstructionSemantics::BaseSemantics::Dispatcher);

#endif
