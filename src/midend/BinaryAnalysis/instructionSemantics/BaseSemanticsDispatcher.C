#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#include <sage3basic.h>
#include <BaseSemanticsDispatcher.h>

#include <BaseSemanticsRiscOperators.h>
#include <BaseSemanticsState.h>
#include <RegisterStateGeneric.h>

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics2 {
namespace BaseSemantics {

StatePtr
Dispatcher::currentState() const {
    return operators ? operators->currentState() : StatePtr();
}

SValuePtr
Dispatcher::protoval() const {
    return operators ? operators->protoval() : SValuePtr();
}

SgAsmInstruction*
Dispatcher::currentInstruction() const {
    return operators ? operators->currentInstruction() : NULL;
}

SValuePtr
Dispatcher::undefined_(size_t nBits) const {
    ASSERT_not_null(operators);
    return operators->undefined_(nBits);
}

SValuePtr
Dispatcher::unspecified_(size_t nBits) const {
    ASSERT_not_null(operators);
    return operators->unspecified_(nBits);
}

SValuePtr
Dispatcher::number_(size_t nbits, uint64_t number) const {
    ASSERT_not_null(operators);
    return operators->number_(nbits, number);
}

void
Dispatcher::advanceInstructionPointer(SgAsmInstruction *insn) {
    RegisterDescriptor ipReg = instructionPointerRegister();
    size_t nBits = ipReg.nBits();
    BaseSemantics::SValuePtr ipValue;
    if (!autoResetInstructionPointer_ && operators->currentState() && operators->currentState()->registerState()) {
        BaseSemantics::RegisterStateGenericPtr grState =
            boost::dynamic_pointer_cast<BaseSemantics::RegisterStateGeneric>(operators->currentState()->registerState());
        if (grState && grState->is_partly_stored(ipReg))
            ipValue = operators->readRegister(ipReg);
    }
    if (!ipValue)
        ipValue = operators->number_(nBits, insn->get_address());
    ipValue = operators->add(ipValue, operators->number_(nBits, insn->get_size()));
    operators->writeRegister(ipReg, ipValue);
}

void
Dispatcher::addressWidth(size_t nBits) {
    ASSERT_require2(nBits==addrWidth_ || addrWidth_==0, "address width cannot be changed once it is set");
    addrWidth_ = nBits;
}

void
Dispatcher::processInstruction(SgAsmInstruction *insn)
{
    operators->startInstruction(insn);
    InsnProcessor *iproc = iproc_lookup(insn);
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
    operators->finishInstruction(insn);
}

InsnProcessor *
Dispatcher::iproc_lookup(SgAsmInstruction *insn)
{
    int key = iproc_key(insn);
    ASSERT_require(key>=0);
    return iproc_get(key);
}

void
Dispatcher::iproc_replace(SgAsmInstruction *insn, InsnProcessor *iproc)
{
    iproc_set(iproc_key(insn), iproc);
}

void
Dispatcher::iproc_set(int key, InsnProcessor *iproc)
{
    ASSERT_require(key>=0);
    if ((size_t)key>=iproc_table.size())
        iproc_table.resize(key+1, NULL);
    iproc_table[key] = iproc;
}

InsnProcessor *
Dispatcher::iproc_get(int key)
{
    if (key<0 || (size_t)key>=iproc_table.size())
        return NULL;
    return iproc_table[key];
}

RegisterDescriptor
Dispatcher::findRegister(const std::string &regname, size_t nbits/*=0*/, bool allowMissing) const
{
    const RegisterDictionary *regdict = get_register_dictionary();
    if (!regdict)
        throw Exception("no register dictionary", currentInstruction());

    const RegisterDescriptor reg = regdict->find(regname);
    if (!reg) {
        if (allowMissing)
            return reg;
        std::ostringstream ss;
        ss <<"Invalid register \"" <<regname <<"\" in dictionary \"" <<regdict->get_architecture_name() <<"\"";
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
    } t1(operators);
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
    } t1(operators);
    t1.traverse(e, preorder);
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
        RegisterDescriptor reg = rre->get_descriptor();
        retval = operators->readRegister(reg);
    } else if (SgAsmBinaryAdd *op = isSgAsmBinaryAdd(e)) {
        BaseSemantics::SValuePtr lhs = effectiveAddress(op->get_lhs(), nbits);
        BaseSemantics::SValuePtr rhs = effectiveAddress(op->get_rhs(), nbits);
        retval = operators->add(lhs, rhs);
    } else if (SgAsmBinaryMultiply *op = isSgAsmBinaryMultiply(e)) {
        BaseSemantics::SValuePtr lhs = effectiveAddress(op->get_lhs(), nbits);
        BaseSemantics::SValuePtr rhs = effectiveAddress(op->get_rhs(), nbits);
        retval = operators->unsignedMultiply(lhs, rhs);
    } else if (SgAsmIntegerValueExpression *ival = isSgAsmIntegerValueExpression(e)) {
        retval = operators->number_(ival->get_significantBits(), ival->get_value());
    }

    ASSERT_not_null(retval);
    if (retval->get_width() < nbits) {
        retval = operators->signExtend(retval, nbits);
    } else if (retval->get_width() > nbits) {
        retval = operators->extract(retval, 0, nbits);
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
        operators->writeRegister(re->get_descriptor(), value);
    } else if (SgAsmIndirectRegisterExpression *re = isSgAsmIndirectRegisterExpression(e)) {
        SValuePtr offset = operators->readRegister(re->get_offset());
        if (!offset->is_number()) {
            std::string offset_name = get_register_dictionary()->lookup(re->get_offset());
            offset_name = offset_name.empty() ? "" : "(" + offset_name + ") ";
            throw Exception("indirect register offset " + offset_name + "must have a concrete value", NULL);
        }
        size_t idx = (offset->get_number() + re->get_index()) % re->get_modulus();
        RegisterDescriptor reg = re->get_descriptor();
        reg.majorNumber(reg.majorNumber() + re->get_stride().majorNumber() * idx);
        reg.minorNumber(reg.minorNumber() + re->get_stride().minorNumber() * idx);
        reg.offset(reg.offset() + re->get_stride().offset() * idx);
        reg.nBits(reg.nBits() + re->get_stride().nBits() * idx);
        operators->writeRegister(reg, value);
    } else if (SgAsmMemoryReferenceExpression *mre = isSgAsmMemoryReferenceExpression(e)) {
        SValuePtr addr = effectiveAddress(mre, addr_nbits);
        ASSERT_require(0==addrWidth_ || addr->get_width()==addrWidth_);
        operators->writeMemory(segmentRegister(mre), addr, value, operators->boolean_(true));
    } else {
        ASSERT_not_implemented("[Robb P. Matzke 2014-10-07]");
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
        retval = operators->readRegister(re->get_descriptor());
    } else if (SgAsmIndirectRegisterExpression *re = isSgAsmIndirectRegisterExpression(e)) {
        SValuePtr offset = operators->readRegister(re->get_offset());
        if (!offset->is_number()) {
            std::string offset_name = get_register_dictionary()->lookup(re->get_offset());
            offset_name = offset_name.empty() ? "" : "(" + offset_name + ") ";
            throw Exception("indirect register offset " + offset_name + "must have a concrete value", NULL);
        }
        size_t idx = (offset->get_number() + re->get_index()) % re->get_modulus();
        RegisterDescriptor reg = re->get_descriptor();
        reg.majorNumber(reg.majorNumber() + re->get_stride().majorNumber() * idx);
        reg.minorNumber(reg.minorNumber() + re->get_stride().minorNumber() * idx);
        reg.offset(reg.offset() + re->get_stride().offset() * idx);
        reg.nBits(reg.nBits() + re->get_stride().nBits() * idx);
        retval = operators->readRegister(reg);
    } else if (SgAsmMemoryReferenceExpression *mre = isSgAsmMemoryReferenceExpression(e)) {
        BaseSemantics::SValuePtr addr = effectiveAddress(mre, addr_nbits);
        ASSERT_require(0==addrWidth_ || addr->get_width()==addrWidth_);
        BaseSemantics::SValuePtr dflt = undefined_(value_nbits);
        retval = operators->readMemory(segmentRegister(mre), addr, dflt, operators->boolean_(true));
    } else if (SgAsmValueExpression *ve = isSgAsmValueExpression(e)) {
        uint64_t val = SageInterface::getAsmSignedConstant(ve);
        retval = operators->number_(value_nbits, val);
    } else if (SgAsmBinaryAdd *sum = isSgAsmBinaryAdd(e)) {
        SgAsmExpression *lhs = sum->get_lhs();
        SgAsmExpression *rhs = sum->get_rhs();
        size_t nbits = std::max(lhs->get_nBits(), rhs->get_nBits());
        retval = operators->add(operators->signExtend(read(lhs, lhs->get_nBits(), addr_nbits), nbits),
                                operators->signExtend(read(rhs, rhs->get_nBits(), addr_nbits), nbits));
    } else if (SgAsmBinaryMultiply *product = isSgAsmBinaryMultiply(e)) {
        SgAsmExpression *lhs = product->get_lhs();
        SgAsmExpression *rhs = product->get_rhs();
        retval = operators->unsignedMultiply(read(lhs, lhs->get_nBits()), read(rhs, rhs->get_nBits()));
    } else {
        ASSERT_not_implemented(e->class_name());
    }

    // Make sure the return value is the requested width. The unsignedExtend() can expand or shrink values.
    ASSERT_not_null(retval);
    if (retval->get_width()!=value_nbits)
        retval = operators->unsignedExtend(retval, value_nbits);
    return retval;
}

} // namespace
} // namespace
} // namespace
} // namespace

#endif
