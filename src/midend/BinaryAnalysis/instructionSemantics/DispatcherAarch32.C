#include <featureTests.h>
#ifdef ROSE_ENABLE_ASM_AARCH32
#include <sage3basic.h>
#include <DispatcherAarch32.h>

using namespace Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics;
using namespace Rose::Diagnostics;

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics2 {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Functors that handle individual A32/T32 instruction kinds
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Aarch32 {

// Base class for processing each instruction kind.
class P: public BaseSemantics::InsnProcessor {
public:
    using D = DispatcherAarch32*;
    using Ops = BaseSemantics::RiscOperators*;
    using I = SgAsmAarch32Instruction*;
    using A = const SgAsmExpressionPtrList&;

    virtual void p(D, Ops, I, A) = 0;

    virtual void process(const BaseSemantics::DispatcherPtr &dispatcher_, SgAsmInstruction *insn_) ROSE_OVERRIDE {
        DispatcherAarch32Ptr dispatcher = DispatcherAarch32::promote(dispatcher_);
        BaseSemantics::RiscOperatorsPtr operators = dispatcher->operators();
        SgAsmAarch32Instruction *insn = isSgAsmAarch32Instruction(insn_);
        ASSERT_not_null(insn);
        ASSERT_require(insn == operators->currentInstruction());
        dispatcher->advanceInstructionPointer(insn);    // branch instructions will reassign
        SgAsmExpressionPtrList &operands = insn->get_operandList()->get_operands();
        for (size_t i = 0; i < operands.size(); ++i)
            dispatcher->preUpdate(operands[i]);
        p(dispatcher.get(), operators.get(), insn, operands);
        for (size_t i = 0; i < operands.size(); ++i)
            dispatcher->postUpdate(operands[i]);
    }

    void assert_args(I insn, A args, size_t nargs) {
        if (args.size() != nargs) {
            std::string mesg = "instruction must have " + StringUtility::plural(nargs, "arguments");
            throw BaseSemantics::Exception(mesg, insn);
        }
    }
};

} // namespace

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
DispatcherAarch32::initializeInsnDispatchTable() {}

void
DispatcherAarch32::initializeRegisterDescriptors() {
    if (regdict) {
        REG_PC = findRegister("pc", 32);
        REG_SP = findRegister("sp", 32);
        REG_LR = findRegister("lr", 32);
    }
}

void
DispatcherAarch32::initializeMemory() {
    if (BaseSemantics::StatePtr state = currentState()) {
        if (BaseSemantics::MemoryStatePtr memory = state->memoryState()) {
            switch (memory->get_byteOrder()) {
                case ByteOrder::ORDER_LSB:
                    break;
                case ByteOrder::ORDER_MSB:
                    mlog[WARN] <<"AArch32 memory state is using big-endian byte order\n";
                    break;
                case ByteOrder::ORDER_UNSPECIFIED:
                    memory->set_byteOrder(ByteOrder::ORDER_LSB);
                    break;
            }
        }
    }
}

RegisterDescriptor
DispatcherAarch32::instructionPointerRegister() const {
    return REG_PC;
}

RegisterDescriptor
DispatcherAarch32::stackPointerRegister() const {
    return REG_SP;
}

RegisterDescriptor
DispatcherAarch32::callReturnRegister() const {
    return REG_LR;
}

void
DispatcherAarch32::set_register_dictionary(const RegisterDictionary *regdict) {
    BaseSemantics::Dispatcher::set_register_dictionary(regdict);
    initializeRegisterDescriptors();
}

int
DispatcherAarch32::iproc_key(SgAsmInstruction *insn_) const {
    auto insn = isSgAsmAarch32Instruction(insn_);
    ASSERT_not_null(insn);
    return insn->get_kind();
}

} // namespace
} // namespace
} // namespace

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
BOOST_CLASS_EXPORT_IMPLEMENT(Rose::BinaryAnalysis::InstructionSemantics2::DispatcherAarch32);
#endif

#endif
