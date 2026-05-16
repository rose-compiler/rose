#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/InstructionSemantics/DispatcherJvm.h>

#include <Rose/Affirm.h>
#include <Rose/BinaryAnalysis/Architecture/Base.h>
#include <Rose/BinaryAnalysis/InstructionEnumsJvm.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/Dispatcher.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/Exception.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/MemoryState.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/RiscOperators.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/State.h>
#include <Rose/StringUtility/Diagnostics.h>

#include <SgAsmJvmInstruction.h>
#include <SgAsmOperandList.h>

#include <Cxx_GrammarDowncast.h>

using namespace Rose::BinaryAnalysis::InstructionSemantics::BaseSemantics;
using namespace Sawyer::Message::Common;

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Functors that handle individual JVM instruction kinds
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Jvm {

// An intermediate class that reduces the amount of typing in all that follows.  Its `process` method does some up-front checking,
// dynamic casting, and pointer dereferencing and then calls the `p` method that does the real work.
class P: public BaseSemantics::InsnProcessor {
public:
    using D = DispatcherJvm*;
    using Ops = RiscOperators*;
    using I = SgAsmJvmInstruction*;
    using A = const SgAsmExpressionPtrList&;

    virtual void p(D, Ops, I, A) = 0;

    // Perform the semantics for the specified instruction.
    virtual void process(const Dispatcher::Ptr &dispatcher_, SgAsmInstruction *insn_) override {
        DispatcherJvmPtr dispatcher = DispatcherJvm::promote(dispatcher_);
        RiscOperators::Ptr operators = dispatcher->operators();
        SgAsmJvmInstruction* insn = isSgAsmJvmInstruction(insn_);
        ASSERT_not_null(insn);
        ASSERT_require(insn == operators->currentInstruction());

        // Update the program counter/instruction pointer (PC) to point to the fall-through instruction before we process the
        // instruction, so that the semantics for individual instructions (like branches) can override this choice by assigning a
        // new value to PC.  However, we must be careful of PC-relative addressing and remember that we already incremented the PC.
        dispatcher->advanceInstructionPointer(insn);
        SgAsmExpressionPtrList &operands = insn->get_operandList()->get_operands();

        operators->comment("executing instruction core");
        p(dispatcher.get(), operators.get(), insn, operands);
    }

    void assert_args(I insn, A args, size_t nargs) {
        if (args.size()!=nargs) {
            std::string mesg = "instruction must have " + StringUtility::plural(nargs, "arguments");
            throw BaseSemantics::Exception(mesg, insn);
        }
    }
};

// Push int constant 3 (0x06)
struct IP_iconst_3: P {
    void p(D /*d*/, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        SValue::Ptr value = ops->number_(32, 3);
        ops->pushOperand(value);
    }
};

// Push int constant 5 (0x08)
struct IP_iconst_5: P {
    void p(D /*d*/, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        SValue::Ptr value = ops->number_(32, 5);
        ops->pushOperand(value);
    }
};

// Load int from local variable (0x15)
struct IP_load: P {
    void p(D /*d*/, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);

        SgAsmExpression* value = args[0];
        (void) value; // value needs to be converted to an integral index

        //TODO: get index value from the expression (don't know how to do this, punt for now)
        uint8_t index{0};

        // Read local variable 0 and push result
        ops->pushOperand(ops->readLocal(index));
    }
};

// Load int from local variable 0 (0x1a)
struct IP_load_0: P {
    void p(D /*d*/, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);

        // Read local variable 0 and push result
        ops->pushOperand(ops->readLocal(0));
    }
};

// Add int (0x60)
struct IP_iadd: P {
    void p(D /*d*/, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);

        // Pop operands
        SValue::Ptr value1 = ops->popOperand();
        SValue::Ptr value2 = ops->popOperand();

        // Compute sum and push result
        ops->pushOperand(ops->add(value1, value2));
    }
};

// Store int into local variable 0 (0x3b)
struct IP_store_0: P {
    void p(D /*d*/, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);

        // Pop value from operand stack, then write it to local variable 0
        SValue::Ptr value = ops->popOperand();
        ops->writeLocal(0, value);
    }
};

} // namespace

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      DispatcherJvm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
DispatcherJvm::initializeDispatchTable() {
    iprocSet(0x06,  new Jvm::IP_iconst_3);
    iprocSet(0x08,  new Jvm::IP_iconst_5);
    iprocSet(0x15,  new Jvm::IP_load);
    iprocSet(0x1a,  new Jvm::IP_load_0);
    iprocSet(0x3b,  new Jvm::IP_store_0);
    iprocSet(0x60,  new Jvm::IP_iadd);
}

DispatcherJvm::~DispatcherJvm() {}

DispatcherJvm::DispatcherJvm(const Architecture::Base::ConstPtr &arch)
    : Super(arch) {}

DispatcherJvm::DispatcherJvm(const Architecture::Base::ConstPtr &arch, const RiscOperators::Ptr &ops)
    : Super(arch, ops),
      REG_PC(arch->registerDictionary()->findOrThrow("pc")),
      REG_SP(arch->registerDictionary()->findOrThrow("sp")),
      REG_FP(arch->registerDictionary()->findOrThrow("fp")) {
    initializeDispatchTable();
    initializeMemoryState();
}

DispatcherJvm::Ptr
DispatcherJvm::instance(const Architecture::Base::ConstPtr &arch) {
    return Ptr(new DispatcherJvm(arch));
}

DispatcherJvm::Ptr
DispatcherJvm::instance(const Architecture::BaseConstPtr &arch,
                        const BaseSemantics::RiscOperatorsPtr &ops) {
    return Ptr(new DispatcherJvm(arch, ops));
}

BaseSemantics::Dispatcher::Ptr
DispatcherJvm::create(const BaseSemantics::RiscOperators::Ptr &ops) const {
    return instance(architecture(), ops);
}

DispatcherJvm::Ptr
DispatcherJvm::promote(const BaseSemantics::Dispatcher::Ptr &d) {
    Ptr retval = boost::dynamic_pointer_cast<DispatcherJvm>(d);
    ASSERT_not_null(retval);
    return retval;
}

int
DispatcherJvm::iprocKey(SgAsmInstruction *insn_) const {
    SgAsmJvmInstruction *insn = isSgAsmJvmInstruction(insn_);
    ASSERT_not_null(insn);
    return static_cast<int>(insn->get_kind());
}

void
DispatcherJvm::initializeMemoryState() {
    if (State::Ptr state = currentState()) {
        if (MemoryState::Ptr memory = state->memoryState()) {
            switch (memory->get_byteOrder()) {
                case ByteOrder::BE:
                case ByteOrder::EL:
                    break;
                case ByteOrder::ORDER_UNSPECIFIED:
                    memory->set_byteOrder(architecture()->byteOrder());
                    break;
            }
        }
    }
}

} // namespace
} // namespace
} // namespace

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
BOOST_CLASS_EXPORT_IMPLEMENT(Rose::BinaryAnalysis::InstructionSemantics::DispatcherJvm);
#endif

#endif
