#include <featureTests.h>
#ifdef ROSE_ENABLE_ASM_A64

#include <sage3basic.h>
#include <DispatcherA64.h>

#include <Diagnostics.h>

using namespace Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics;
using namespace Rose::Diagnostics;

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics2 {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Functors that handle individual A64 instruction kinds
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace A64 {

// Base class for processing each instruction kind.
class P: public BaseSemantics::InsnProcessor {
public:
    using D = DispatcherA64*;
    using Ops = BaseSemantics::RiscOperators*;
    using I = SgAsmA64Instruction*;
    using A = const SgAsmExpressionPtrList&;

    virtual void p(D, Ops, I, A) = 0;

    virtual void process(const BaseSemantics::DispatcherPtr &dispatcher_, SgAsmInstruction *insn_) ROSE_OVERRIDE {
        DispatcherA64Ptr dispatcher = DispatcherA64::promote(dispatcher_);
        BaseSemantics::RiscOperatorsPtr operators = dispatcher->get_operators();
        SgAsmA64Instruction *insn = isSgAsmA64Instruction(insn_);
        ASSERT_not_null(insn);
        ASSERT_require(insn == operators->currentInstruction());
        dispatcher->advanceInstructionPointer(insn);    // branch instructions will reassign
        SgAsmExpressionPtrList &operands = insn->get_operandList()->get_operands();
        p(dispatcher.get(), operators.get(), insn, operands);
    }

    void assert_args(I insn, A args, size_t nargs) {
        if (args.size() != nargs) {
            std::string mesg = "instruction must have " + StringUtility::plural(nargs, "arguments");
            throw BaseSemantics::Exception(mesg, insn);
        }
    }
};

struct IP_ldr: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        d->preUpdate(args[1]);
        SValuePtr result = d->read(args[1]);
        SValuePtr extended = ops->signExtend(result, args[0]->get_nBits());
        d->write(args[0], extended);
        d->postUpdate(args[1]);
    }
};

} // namespace

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
DispatcherA64::initializeInsnDispatchTable() {
    iproc_set(ARM64_INS_LDR, new A64::IP_ldr);
}

void
DispatcherA64::initializeRegisterDescriptors() {
    if (regdict) {
        REG_PC = findRegister("pc", 64);
        REG_SP = findRegister("sp", 64);
        REG_LR = findRegister("lr", 64);
    }
}

void
DispatcherA64::initializeMemory() {
    if (BaseSemantics::StatePtr state = currentState()) {
        if (BaseSemantics::MemoryStatePtr memory = state->memoryState()) {
            switch (memory->get_byteOrder()) {
                case ByteOrder::ORDER_LSB:
                    break;
                case ByteOrder::ORDER_MSB:
                    mlog[WARN] <<"A64 memory state is using big-endian byte order\n";
                    break;
                case ByteOrder::ORDER_UNSPECIFIED:
                    memory->set_byteOrder(ByteOrder::ORDER_LSB);
                    break;
            }
        }
    }
}

RegisterDescriptor
DispatcherA64::instructionPointerRegister() const {
    return REG_PC;
}

RegisterDescriptor
DispatcherA64::stackPointerRegister() const {
    return REG_SP;
}

RegisterDescriptor
DispatcherA64::callReturnRegister() const {
    return REG_LR;
}

void
DispatcherA64::set_register_dictionary(const RegisterDictionary *regdict) {
    BaseSemantics::Dispatcher::set_register_dictionary(regdict);
    initializeRegisterDescriptors();
}

int
DispatcherA64::iproc_key(SgAsmInstruction *insn_) const {
    auto insn = isSgAsmA64Instruction(insn_);
    ASSERT_not_null(insn);
    return insn->get_kind();
}

void
DispatcherA64::preUpdate(SgAsmExpression *expr) {
    ASSERT_not_null(expr);

    struct T: AstSimpleProcessing {
        DispatcherA64 *d;

        T(DispatcherA64 *d)
            : d(d) {}

        void visit(SgNode *node_) override {
            if (auto node = isSgAsmBinaryAddPreupdate(node_)) {
                SValuePtr addend1 = d->read(node->get_lhs());
                SValuePtr addend2 = d->read(node->get_rhs());
                ASSERT_require(addend1->get_width() == addend2->get_width()); // this, or sign extend one of them
                SValuePtr rvalue = d->get_operators()->add(addend1, addend2);
                d->write(node->get_lhs(), rvalue);
            }
        }
    } t(this);
    t.traverse(expr, postorder);
}

void
DispatcherA64::postUpdate(SgAsmExpression *expr) {
    ASSERT_not_null(expr);

    struct T: AstSimpleProcessing {
        DispatcherA64 *d;

        T(DispatcherA64 *d)
            : d(d) {}

        void visit(SgNode *node_) override {
            if (auto node = isSgAsmBinaryAddPostupdate(node_)) {
                SValuePtr addend1 = d->read(node->get_lhs());
                SValuePtr addend2 = d->read(node->get_rhs());
                ASSERT_require(addend1->get_width() == addend2->get_width()); // this, or sign extend one of them
                SValuePtr rvalue = d->get_operators()->add(addend1, addend2);
                d->write(node->get_lhs(), rvalue);
            }
        }
    } t(this);
    t.traverse(expr, postorder);
}

} // namespace
} // namespace
} // namespace

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
BOOST_CLASS_EXPORT_IMPLEMENT(Rose::BinaryAnalysis::InstructionSemantics2::DispatcherA64);
#endif

#endif
