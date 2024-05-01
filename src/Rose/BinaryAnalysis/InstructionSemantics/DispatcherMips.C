#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/InstructionSemantics/DispatcherMips.h>

#include <Rose/Affirm.h>
#include <Rose/BinaryAnalysis/Architecture/Base.h>
#include <Rose/BinaryAnalysis/InstructionEnumsMips.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/Dispatcher.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/Exception.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/MemoryState.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/RiscOperators.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/State.h>
#include <Rose/StringUtility/Diagnostics.h>

#include <SgAsmMipsInstruction.h>
#include <SgAsmOperandList.h>

#include <Cxx_GrammarDowncast.h>

using namespace Rose::BinaryAnalysis::InstructionSemantics::BaseSemantics;
using namespace Sawyer::Message::Common;

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Functors that handle individual MIPS instruction kinds
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Mips {

// An intermediate class that reduces the amount of typing in all that follows.  Its `process` method does some up-front checking,
// dynamic casting, and pointer dereferencing and then calls the `p` method that does the real work.
class P: public BaseSemantics::InsnProcessor {
public:
    using D = DispatcherMips*;
    using Ops = RiscOperators*;
    using I = SgAsmMipsInstruction*;
    using A = const SgAsmExpressionPtrList&;

    virtual void p(D, Ops, I, A) = 0;

    // Perform the semantics for the specified instruction.
    virtual void process(const Dispatcher::Ptr &dispatcher_, SgAsmInstruction *insn_) override {
        DispatcherMipsPtr dispatcher = DispatcherMips::promote(dispatcher_);
        RiscOperators::Ptr operators = dispatcher->operators();
        SgAsmMipsInstruction *insn = isSgAsmMipsInstruction(insn_);
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

// Add immediate word
struct IP_addi: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        const size_t nBits = d->architecture()->bitsPerWord();

        // Compute sum
        SValue::Ptr rs = d->read(args[1], nBits);       // 2-args version does unsigned extend or truncate
        SValue::Ptr imm = ops->signExtend(d->read(args[2]), nBits);
        SValue::Ptr sum = ops->add(rs, imm);

        // Calculate overflow
        SValue::Ptr wideRs = ops->signExtend(d->read(args[1]), nBits + 1);
        SValue::Ptr wideImm = ops->signExtend(d->read(args[2]), nBits + 1);
        SValue::Ptr wideSum = ops->add(wideRs, wideImm);
        SValue::Ptr wideSum32 = ops->extract(wideSum, 32, 33); // bit #32
        SValue::Ptr wideSum31 = ops->extract(wideSum, 31, 32); // bit #31
        SValue::Ptr overflow = ops->isNotEqual(wideSum31, wideSum32);
        SValue::Ptr oldRt = d->read(args[0], nBits);
        SValue::Ptr result = ops->ite(overflow, oldRt, sum);

        // Craig: always do side effects after everything else.
        // Side effects
        d->write(args[0], result);
        ops->raiseInterrupt(mips_signal_exception, mips_integer_overflow, overflow);
    }
};

// Add immediate unsigned word
struct IP_addiu: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        const size_t nBits = d->architecture()->bitsPerWord();
        SValue::Ptr rs = d->read(args[1]);
        SValue::Ptr imm = ops->signExtend(d->read(args[2]), nBits);
        SValue::Ptr result = ops->add(rs, imm);
        d->write(args[0], result);
    }
};

// Add unsigned word
struct IP_addu: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        SValue::Ptr rs = d->read(args[1]);
        SValue::Ptr rt = d->read(args[2]);
        SValue::Ptr result = ops->add(rs, rt);
#if 0 // [Robb Matzke 2024-05-01] don't do this
        //                            side-effect-1     side-effect-2     (GCC)
        //                            side-effect-2     side-effect-1     (clang)
        SValue::Ptr result = ops->add(d->read(args[1]), d->read(args[2]));
#endif
        d->write(args[0], result);
    }
};

// Breakpoint
struct IP_break: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        ops->raiseInterrupt(mips_signal_exception, mips_breakpoint, ops->boolean_(true));
    }
};

// Count leading ones in word
struct IP_clo: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValue::Ptr rs = d->read(args[1]);
        SValue::Ptr result = ops->countLeadingOnes(rs);
        d->write(args[0], result);
    }
};

// Divide word
struct IP_div: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        SValue::Ptr rs = d->read(args[0]);
        SValue::Ptr rt = d->read(args[1]);
        SValue::Ptr quotient = ops->signedDivide(rs, rt);
        SValue::Ptr remainder = ops->signedModulo(rs, rt);
        ops->writeRegister(d->REG_LO, quotient);
        ops->writeRegister(d->REG_HI, remainder);
    }
};

// Load byte
struct IP_lb: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        size_t nBits = d->architecture()->bitsPerWord();
        SValue::Ptr result = ops->signExtend(d->read(args[1]), nBits);
        d->write(args[0], result);
    }
};

} // namespace

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      DispatcherMips
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
DispatcherMips::initializeDispatchTable() {
    iprocSet(mips_addi,  new Mips::IP_addi);
    iprocSet(mips_addiu, new Mips::IP_addiu);
    iprocSet(mips_addu,  new Mips::IP_addu);
    iprocSet(mips_break, new Mips::IP_break);
    iprocSet(mips_clo,   new Mips::IP_clo);
    iprocSet(mips_div,   new Mips::IP_div);
    iprocSet(mips_lb,    new Mips::IP_lb);
}

DispatcherMips::~DispatcherMips() {}

DispatcherMips::DispatcherMips(const Architecture::Base::ConstPtr &arch)
    : Super(arch) {}

DispatcherMips::DispatcherMips(const Architecture::Base::ConstPtr &arch, const RiscOperators::Ptr &ops)
    : Super(arch, ops),
      REG_R0(arch->registerDictionary()->findOrThrow("r0")),
      REG_R1(arch->registerDictionary()->findOrThrow("r1")),
      REG_HI(arch->registerDictionary()->findOrThrow("hi")),
      REG_LO(arch->registerDictionary()->findOrThrow("lo")),
      REG_PC(arch->registerDictionary()->findOrThrow("pc")),
      REG_SP(arch->registerDictionary()->findOrThrow("sp")),
      REG_FP(arch->registerDictionary()->findOrThrow("fp")),
      REG_RA(arch->registerDictionary()->findOrThrow("ra")) {
    initializeDispatchTable();
    initializeMemoryState();
}

DispatcherMips::Ptr
DispatcherMips::instance(const Architecture::Base::ConstPtr &arch) {
    return Ptr(new DispatcherMips(arch));
}

DispatcherMips::Ptr
DispatcherMips::instance(const Architecture::Base::ConstPtr &arch, const RiscOperators::Ptr &ops) {
    return Ptr(new DispatcherMips(arch, ops));
}

BaseSemantics::Dispatcher::Ptr
DispatcherMips::create(const BaseSemantics::RiscOperators::Ptr &ops) const {
    return instance(architecture(), ops);
}

DispatcherMips::Ptr
DispatcherMips::promote(const BaseSemantics::Dispatcher::Ptr &d) {
    Ptr retval = boost::dynamic_pointer_cast<DispatcherMips>(d);
    ASSERT_not_null(retval);
    return retval;
}

int
DispatcherMips::iprocKey(SgAsmInstruction *insn_) const {
    SgAsmMipsInstruction *insn = isSgAsmMipsInstruction(insn_);
    ASSERT_not_null(insn);
    return insn->get_kind();
}

void
DispatcherMips::initializeMemoryState() {
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
BOOST_CLASS_EXPORT_IMPLEMENT(Rose::BinaryAnalysis::InstructionSemantics::DispatcherMips);
#endif

#endif
