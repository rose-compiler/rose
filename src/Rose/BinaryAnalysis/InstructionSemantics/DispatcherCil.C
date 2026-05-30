#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/InstructionSemantics/DispatcherCil.h>

#include <Rose/Affirm.h>
#include <Rose/As.h>
#include <Rose/BinaryAnalysis/Architecture/Base.h>
#include <Rose/BinaryAnalysis/InstructionEnumsCil.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/Dispatcher.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/Exception.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/MemoryState.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/RiscOperators.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/State.h>
#include <Rose/StringUtility/Diagnostics.h>

#include <SgAsmCilInstruction.h>
#include <SgAsmFloatType.h>
#include <SgAsmExpression.h>
#include <SgAsmIntegerValueExpression.h>
#include <SgAsmOperandList.h>
#include <SgAsmValueExpression.h>

#include <Cxx_GrammarDowncast.h>

#include <algorithm>
#include <climits>
#include <cstdint>
#include <string>

using namespace Rose::BinaryAnalysis::InstructionSemantics::BaseSemantics;

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Functors that handle individual CIL instruction kinds
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Cil {

// This implementation intentionally mirrors DispatcherJvm.C: one small processor adapter performs all common dispatcher,
// operator, instruction, operand-list, PC-advance, and checking work, then delegates to instruction-specific semantics.
class P: public BaseSemantics::InsnProcessor {
public:
    using D = DispatcherCil*;
    using Ops = RiscOperators*;
    using I = SgAsmCilInstruction*;
    using Args = const SgAsmExpressionPtrList&;

    virtual void p(D, Ops, I, Args) = 0;

    virtual void process(const Dispatcher::Ptr &dispatcher_, SgAsmInstruction *insn_) override {
        DispatcherCil::Ptr dispatcher = DispatcherCil::promote(dispatcher_);
        RiscOperators::Ptr operators = dispatcher->operators();
        SgAsmCilInstruction *insn = isSgAsmCilInstruction(insn_);
        ASSERT_not_null(insn);
        ASSERT_require(insn == operators->currentInstruction());

        dispatcher->advanceInstructionPointer(insn);
        SgAsmExpressionPtrList &operands = insn->get_operandList()->get_operands();

        operators->comment("executing CIL instruction core");
        p(dispatcher.get(), operators.get(), insn, operands);
    }

    void assert_args(I insn, Args args, size_t nargs) {
        if (args.size() != nargs) {
            std::string mesg = "CIL instruction must have " + StringUtility::plural(nargs, "operand");
            throw BaseSemantics::Exception(mesg, insn);
        }
    }
};

namespace CilSemantics {
    using D = DispatcherCil*;
    using Ops = RiscOperators*;
    using I = SgAsmCilInstruction*;
    using Args = const SgAsmExpressionPtrList&;

    uint64_t asUnsigned(const SgAsmExpression *expr, uint64_t dflt = 0) {
        if (const SgAsmIntegerValueExpression *ival = isSgAsmIntegerValueExpression(expr))
            return ival->get_value();
        return dflt;
    }

    uint32_t asIndex(const SgAsmExpression *expr) {
        return static_cast<uint32_t>(asUnsigned(expr));
    }

    void doNothing(Ops ops) {
        ASSERT_not_null(ops);
    }

    void pushUnknown(Ops ops, size_t nbits) {
        ASSERT_not_null(ops);
        ops->pushOperand(ops->undefined_(nbits));
    }

    void pushAddress(Ops ops) {
        pushUnknown(ops, 32);                         // TODO: use architecture pointer width when CIL exposes it.
    }

    void pushConstant(Ops ops, Args args, size_t nbits) {
        ASSERT_not_null(ops);
        ASSERT_require(!args.empty());
        ops->pushOperand(ops->number_(nbits, asUnsigned(args[0])));
    }

    void discard(Ops ops, size_t nvalues) {
        ASSERT_not_null(ops);
        while (nvalues-- > 0)
            ops->popOperand();
    }

    void dup(Ops ops) {
        ASSERT_not_null(ops);
        SValue::Ptr v = ops->popOperand();
        ops->pushOperand(v);
        ops->pushOperand(v);
    }

    void unaryResult(Ops ops, size_t nbits = 32) {
        ASSERT_not_null(ops);
        SValue::Ptr v = ops->popOperand();
        ops->pushOperand(ops->undefined_(std::max(nbits, v ? v->nBits() : nbits)));
    }

    void binaryResult(Ops ops, size_t nbits = 32) {
        ASSERT_not_null(ops);
        SValue::Ptr rhs = ops->popOperand();
        SValue::Ptr lhs = ops->popOperand();
        const size_t lhsBits = lhs ? lhs->nBits() : nbits;
        const size_t rhsBits = rhs ? rhs->nBits() : nbits;
        ops->pushOperand(ops->undefined_(std::max(nbits, std::max(lhsBits, rhsBits))));
    }

    void compareResult(Ops ops) {
        ASSERT_not_null(ops);
        discard(ops, 2);
        ops->pushOperand(ops->undefined_(32));
    }

    void convertResult(Ops ops, size_t nbits = 32) {
        ASSERT_not_null(ops);
        discard(ops, 1);
        ops->pushOperand(ops->undefined_(nbits));
    }

    void loadResult(Ops ops, size_t nbits) {
        ASSERT_not_null(ops);
        discard(ops, 1);
        ops->pushOperand(ops->undefined_(nbits));
    }

    void storeEffect(Ops ops) {
        ASSERT_not_null(ops);
        discard(ops, 2);
    }

    void callEffect(Ops ops) {
        ASSERT_not_null(ops);
        // TODO: resolve the metadata signature, pop actual arguments, and push the return value when non-void.
        ops->pushOperand(ops->undefined_(32));
    }

    void branchEffect(D /*dispatcher*/, Ops ops, I /*insn*/, Args /*args*/) {
        ASSERT_not_null(ops);
        // TODO: decode the inline CIL target and update the instruction pointer when the branch is taken.
    }

    void returnEffect(Ops /*ops*/) {
        // TODO: model transfer of the optional method return value to the caller.
    }

    void throwEffect(Ops ops) {
        ASSERT_not_null(ops);
        discard(ops, 1);
    }

    void copyBlock(Ops ops) {
        ASSERT_not_null(ops);
        discard(ops, 3);                              // destination, source, size
    }

    void initBlock(Ops ops) {
        ASSERT_not_null(ops);
        discard(ops, 3);                              // address, value, size
    }

    void prefixEffect(Ops ops) {
        ASSERT_not_null(ops);
        // TODO: carry prefix state into interpretation of the following instruction.
    }
}

// nop (0 (0x00))
        // Description:
        //   No operation; may be used to fill space when opcodes are patched.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_nop: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::doNothing(ops);
    }
};

// break (1 (0x01))
        // Description:
        //   Signals a breakpoint to an attached debugger.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_break: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::doNothing(ops);
    }
};

// ldarg_0 (2 (0x02))
        // Description:
        //   Load argument 0 onto the evaluation stack.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_ldarg_0: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        ops->pushOperand(ops->readLocal(0));
    }
};

// ldarg_1 (3 (0x03))
        // Description:
        //   Load argument 1 onto the evaluation stack.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_ldarg_1: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        ops->pushOperand(ops->readLocal(1));
    }
};

// ldarg_2 (4 (0x04))
        // Description:
        //   Load argument 2 onto the evaluation stack.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_ldarg_2: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        ops->pushOperand(ops->readLocal(2));
    }
};

// ldarg_3 (5 (0x05))
        // Description:
        //   Load argument 3 onto the evaluation stack.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_ldarg_3: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        ops->pushOperand(ops->readLocal(3));
    }
};

// ldloc_0 (6 (0x06))
        // Description:
        //   Load local variable 0 onto the evaluation stack.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_ldloc_0: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        ops->pushOperand(ops->readLocal(0));
    }
};

// ldloc_1 (7 (0x07))
        // Description:
        //   Load local variable 1 onto the evaluation stack.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_ldloc_1: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        ops->pushOperand(ops->readLocal(1));
    }
};

// ldloc_2 (8 (0x08))
        // Description:
        //   Load local variable 2 onto the evaluation stack.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_ldloc_2: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        ops->pushOperand(ops->readLocal(2));
    }
};

// ldloc_3 (9 (0x09))
        // Description:
        //   Load local variable 3 onto the evaluation stack.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_ldloc_3: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        ops->pushOperand(ops->readLocal(3));
    }
};

// stloc_0 (10 (0x0a))
        // Description:
        //   Store the top evaluation-stack value into local variable 0.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_stloc_0: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        ops->writeLocal(0, ops->popOperand());
    }
};

// stloc_1 (11 (0x0b))
        // Description:
        //   Store the top evaluation-stack value into local variable 1.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_stloc_1: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        ops->writeLocal(1, ops->popOperand());
    }
};

// stloc_2 (12 (0x0c))
        // Description:
        //   Store the top evaluation-stack value into local variable 2.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_stloc_2: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        ops->writeLocal(2, ops->popOperand());
    }
};

// stloc_3 (13 (0x0d))
        // Description:
        //   Store the top evaluation-stack value into local variable 3.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_stloc_3: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        ops->writeLocal(3, ops->popOperand());
    }
};

// ldarg_s (14 (0x0e))
        // Description:
        //   Load argument s onto the evaluation stack.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_ldarg_s: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 1);
        ops->pushOperand(ops->readLocal(CilSemantics::asIndex(args[0])));
    }
};

// ldarga_s (15 (0x0f))
        // Description:
        //   Load the address of an argument, selected by an inline index operand, onto the evaluation stack.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_ldarga_s: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 1);
        CilSemantics::pushAddress(ops);
    }
};

// starg_s (16 (0x10))
        // Description:
        //   Store the top evaluation-stack value into an argument selected by an inline index operand.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_starg_s: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 1);
        CilSemantics::discard(ops, 1);
    }
};

// ldloc_s (17 (0x11))
        // Description:
        //   Load local variable s onto the evaluation stack.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_ldloc_s: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 1);
        ops->pushOperand(ops->readLocal(CilSemantics::asIndex(args[0])));
    }
};

// ldloca_s (18 (0x12))
        // Description:
        //   Load the address of a local variable, selected by an inline index operand, onto the evaluation stack.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_ldloca_s: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 1);
        CilSemantics::pushAddress(ops);
    }
};

// stloc_s (19 (0x13))
        // Description:
        //   Store the top evaluation-stack value into local variable s.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_stloc_s: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 1);
        ops->writeLocal(CilSemantics::asIndex(args[0]), ops->popOperand());
    }
};

// ldnull (20 (0x14))
        // Description:
        //   Push a null object reference onto the evaluation stack.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_ldnull: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        ops->pushOperand(ops->number_(32, 0));
    }
};

// ldc_i4_m1 (21 (0x15))
        // Description:
        //   Push a numeric constant onto the evaluation stack.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_ldc_i4_m1: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        ops->pushOperand(ops->number_(32, uint64_t(-1)));
    }
};

// ldc_i4_0 (22 (0x16))
        // Description:
        //   Push a numeric constant onto the evaluation stack.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_ldc_i4_0: P {
    void p(D dispatcher, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        ops->pushOperand(dispatcher->makeConstant("i", 0, 32));
    }
};

// ldc_i4_1 (23 (0x17))
        // Description:
        //   Push a numeric constant onto the evaluation stack.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_ldc_i4_1: P {
    void p(D dispatcher, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        ops->pushOperand(dispatcher->makeConstant("i", 1, 32));
    }
};

// ldc_i4_2 (24 (0x18))
        // Description:
        //   Push a numeric constant onto the evaluation stack.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_ldc_i4_2: P {
    void p(D dispatcher, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        ops->pushOperand(dispatcher->makeConstant("i", 2, 32));
    }
};

// ldc_i4_3 (25 (0x19))
        // Description:
        //   Push a numeric constant onto the evaluation stack.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_ldc_i4_3: P {
    void p(D dispatcher, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        ops->pushOperand(dispatcher->makeConstant("i", 3, 32));
    }
};

// ldc_i4_4 (26 (0x1a))
        // Description:
        //   Push a numeric constant onto the evaluation stack.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_ldc_i4_4: P {
    void p(D dispatcher, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        ops->pushOperand(dispatcher->makeConstant("i", 4, 32));
    }
};

// ldc_i4_5 (27 (0x1b))
        // Description:
        //   Push a numeric constant onto the evaluation stack.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_ldc_i4_5: P {
    void p(D dispatcher, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        ops->pushOperand(dispatcher->makeConstant("i", 5, 32));
    }
};

// ldc_i4_6 (28 (0x1c))
        // Description:
        //   Push a numeric constant onto the evaluation stack.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_ldc_i4_6: P {
    void p(D dispatcher, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        ops->pushOperand(dispatcher->makeConstant("i", 6, 32));
    }
};

// ldc_i4_7 (29 (0x1d))
        // Description:
        //   Push a numeric constant onto the evaluation stack.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_ldc_i4_7: P {
    void p(D dispatcher, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        ops->pushOperand(dispatcher->makeConstant("i", 7, 32));
    }
};

// ldc_i4_8 (30 (0x1e))
        // Description:
        //   Push a numeric constant onto the evaluation stack.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_ldc_i4_8: P {
    void p(D dispatcher, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        ops->pushOperand(dispatcher->makeConstant("i", 8, 32));
    }
};

// ldc_i4_s (31 (0x1f))
        // Description:
        //   Push a numeric constant onto the evaluation stack.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_ldc_i4_s: P {
    void p(D dispatcher, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 1);
        //signed byte
        auto imm = dispatcher->asS1(args[0]);
        ops->pushOperand(dispatcher->makeConstant("i", imm, 32));
    }
};

// ldc_i4 (32 (0x20))
        // Description:
        //   Push a numeric constant onto the evaluation stack.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_ldc_i4: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 1);
        CilSemantics::pushConstant(ops, args, 32);
    }
};

// ldc_i8 (33 (0x21))
        // Description:
        //   Push a numeric constant onto the evaluation stack.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_ldc_i8: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 1);
        CilSemantics::pushConstant(ops, args, 64);
    }
};

// ldc_r4 (34 (0x22))
        // Description:
        //   Push a numeric constant onto the evaluation stack.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_ldc_r4: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 1);
        CilSemantics::pushUnknown(ops, 32);
    }
};

// ldc_r8 (35 (0x23))
        // Description:
        //   Push a numeric constant onto the evaluation stack.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_ldc_r8: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 1);
        CilSemantics::pushUnknown(ops, 64);
    }
};

// dup (37 (0x25))
        // Description:
        //   Duplicate the value currently on top of the evaluation stack.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_dup: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::dup(ops);
    }
};

// pop (38 (0x26))
        // Description:
        //   Remove the value currently on top of the evaluation stack.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_pop: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::discard(ops, 1);
    }
};

// jmp (39 (0x27))
        // Description:
        //   Unconditionally transfer control to the inline target for jmp.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_jmp: P {
    void p(D dispatcher, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 1);
        CilSemantics::branchEffect(dispatcher, ops, insn, args);
    }
};

// call (40 (0x28))
        // Description:
        //   Call, create, or otherwise transfer to the metadata target specified by call.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_call: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 1);
        CilSemantics::callEffect(ops);
    }
};

// calli (41 (0x29))
        // Description:
        //   Call, create, or otherwise transfer to the metadata target specified by calli.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_calli: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 1);
        CilSemantics::callEffect(ops);
    }
};

// ret (42 (0x2a))
        // Description:
        //   Return from the current method.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_ret: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::returnEffect(ops);
    }
};

// br_s (43 (0x2b))
        // Description:
        //   Unconditionally transfer control to the inline target for br.s.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_br_s: P {
    void p(D dispatcher, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 1);
        CilSemantics::branchEffect(dispatcher, ops, insn, args);
    }
};

// brfalse_s (44 (0x2c))
        // Description:
        //   Conditionally transfer control to the inline target for brfalse.s.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_brfalse_s: P {
    void p(D dispatcher, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 1);
        CilSemantics::branchEffect(dispatcher, ops, insn, args);
    }
};

// brtrue_s (45 (0x2d))
        // Description:
        //   Conditionally transfer control to the inline target for brtrue.s.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_brtrue_s: P {
    void p(D dispatcher, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 1);
        CilSemantics::branchEffect(dispatcher, ops, insn, args);
    }
};

// beq_s (46 (0x2e))
        // Description:
        //   Conditionally transfer control to the inline target for beq.s.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_beq_s: P {
    void p(D dispatcher, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 1);
        CilSemantics::branchEffect(dispatcher, ops, insn, args);
    }
};

// bge_s (47 (0x2f))
        // Description:
        //   Conditionally transfer control to the inline target for bge.s.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_bge_s: P {
    void p(D dispatcher, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 1);
        CilSemantics::branchEffect(dispatcher, ops, insn, args);
    }
};

// bgt_s (48 (0x30))
        // Description:
        //   Conditionally transfer control to the inline target for bgt.s.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_bgt_s: P {
    void p(D dispatcher, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 1);
        CilSemantics::branchEffect(dispatcher, ops, insn, args);
    }
};

// ble_s (49 (0x31))
        // Description:
        //   Conditionally transfer control to the inline target for ble.s.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_ble_s: P {
    void p(D dispatcher, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 1);
        CilSemantics::branchEffect(dispatcher, ops, insn, args);
    }
};

// blt_s (50 (0x32))
        // Description:
        //   Conditionally transfer control to the inline target for blt.s.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_blt_s: P {
    void p(D dispatcher, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 1);
        CilSemantics::branchEffect(dispatcher, ops, insn, args);
    }
};

// bne_un_s (51 (0x33))
        // Description:
        //   Conditionally transfer control to the inline target for bne.un.s.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_bne_un_s: P {
    void p(D dispatcher, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 1);
        CilSemantics::branchEffect(dispatcher, ops, insn, args);
    }
};

// bge_un_s (52 (0x34))
        // Description:
        //   Conditionally transfer control to the inline target for bge.un.s.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_bge_un_s: P {
    void p(D dispatcher, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 1);
        CilSemantics::branchEffect(dispatcher, ops, insn, args);
    }
};

// bgt_un_s (53 (0x35))
        // Description:
        //   Conditionally transfer control to the inline target for bgt.un.s.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_bgt_un_s: P {
    void p(D dispatcher, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 1);
        CilSemantics::branchEffect(dispatcher, ops, insn, args);
    }
};

// ble_un_s (54 (0x36))
        // Description:
        //   Conditionally transfer control to the inline target for ble.un.s.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_ble_un_s: P {
    void p(D dispatcher, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 1);
        CilSemantics::branchEffect(dispatcher, ops, insn, args);
    }
};

// blt_un_s (55 (0x37))
        // Description:
        //   Conditionally transfer control to the inline target for blt.un.s.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_blt_un_s: P {
    void p(D dispatcher, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 1);
        CilSemantics::branchEffect(dispatcher, ops, insn, args);
    }
};

// br (56 (0x38))
        // Description:
        //   Unconditionally transfer control to the inline target for br.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_br: P {
    void p(D dispatcher, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 1);
        CilSemantics::branchEffect(dispatcher, ops, insn, args);
    }
};

// brfalse (57 (0x39))
        // Description:
        //   Conditionally transfer control to the inline target for brfalse.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_brfalse: P {
    void p(D dispatcher, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 1);
        CilSemantics::branchEffect(dispatcher, ops, insn, args);
    }
};

// brtrue (58 (0x3a))
        // Description:
        //   Conditionally transfer control to the inline target for brtrue.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_brtrue: P {
    void p(D dispatcher, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 1);
        CilSemantics::branchEffect(dispatcher, ops, insn, args);
    }
};

// beq (59 (0x3b))
        // Description:
        //   Conditionally transfer control to the inline target for beq.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_beq: P {
    void p(D dispatcher, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 1);
        CilSemantics::branchEffect(dispatcher, ops, insn, args);
    }
};

// bge (60 (0x3c))
        // Description:
        //   Conditionally transfer control to the inline target for bge.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_bge: P {
    void p(D dispatcher, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 1);
        CilSemantics::branchEffect(dispatcher, ops, insn, args);
    }
};

// bgt (61 (0x3d))
        // Description:
        //   Conditionally transfer control to the inline target for bgt.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_bgt: P {
    void p(D dispatcher, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 1);
        CilSemantics::branchEffect(dispatcher, ops, insn, args);
    }
};

// ble (62 (0x3e))
        // Description:
        //   Conditionally transfer control to the inline target for ble.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_ble: P {
    void p(D dispatcher, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 1);
        CilSemantics::branchEffect(dispatcher, ops, insn, args);
    }
};

// blt (63 (0x3f))
        // Description:
        //   Conditionally transfer control to the inline target for blt.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_blt: P {
    void p(D dispatcher, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 1);
        CilSemantics::branchEffect(dispatcher, ops, insn, args);
    }
};

// bne_un (64 (0x40))
        // Description:
        //   Conditionally transfer control to the inline target for bne.un.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_bne_un: P {
    void p(D dispatcher, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 1);
        CilSemantics::branchEffect(dispatcher, ops, insn, args);
    }
};

// bge_un (65 (0x41))
        // Description:
        //   Conditionally transfer control to the inline target for bge.un.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_bge_un: P {
    void p(D dispatcher, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 1);
        CilSemantics::branchEffect(dispatcher, ops, insn, args);
    }
};

// bgt_un (66 (0x42))
        // Description:
        //   Conditionally transfer control to the inline target for bgt.un.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_bgt_un: P {
    void p(D dispatcher, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 1);
        CilSemantics::branchEffect(dispatcher, ops, insn, args);
    }
};

// ble_un (67 (0x43))
        // Description:
        //   Conditionally transfer control to the inline target for ble.un.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_ble_un: P {
    void p(D dispatcher, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 1);
        CilSemantics::branchEffect(dispatcher, ops, insn, args);
    }
};

// blt_un (68 (0x44))
        // Description:
        //   Conditionally transfer control to the inline target for blt.un.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_blt_un: P {
    void p(D dispatcher, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 1);
        CilSemantics::branchEffect(dispatcher, ops, insn, args);
    }
};

// switch (69 (0x45))
        // Description:
        //   Transfer control to one of the inline switch targets selected by an int32 stack value.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_switch: P {
    void p(D dispatcher, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 1);
        CilSemantics::branchEffect(dispatcher, ops, insn, args);
    }
};

// ldind_i1 (70 (0x46))
        // Description:
        //   Load a value indirectly from the address on top of the evaluation stack using ldind.i1.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_ldind_i1: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::loadResult(ops, 32);
    }
};

// ldind_u1 (71 (0x47))
        // Description:
        //   Load a value indirectly from the address on top of the evaluation stack using ldind.u1.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_ldind_u1: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::loadResult(ops, 32);
    }
};

// ldind_i2 (72 (0x48))
        // Description:
        //   Load a value indirectly from the address on top of the evaluation stack using ldind.i2.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_ldind_i2: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::loadResult(ops, 32);
    }
};

// ldind_u2 (73 (0x49))
        // Description:
        //   Load a value indirectly from the address on top of the evaluation stack using ldind.u2.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_ldind_u2: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::loadResult(ops, 32);
    }
};

// ldind_i4 (74 (0x4a))
        // Description:
        //   Load a value indirectly from the address on top of the evaluation stack using ldind.i4.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_ldind_i4: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::loadResult(ops, 32);
    }
};

// ldind_u4 (75 (0x4b))
        // Description:
        //   Load a value indirectly from the address on top of the evaluation stack using ldind.u4.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_ldind_u4: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::loadResult(ops, 32);
    }
};

// ldind_i8 (76 (0x4c))
        // Description:
        //   Load a value indirectly from the address on top of the evaluation stack using ldind.i8.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_ldind_i8: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::loadResult(ops, 64);
    }
};

// ldind_i (77 (0x4d))
        // Description:
        //   Load a value indirectly from the address on top of the evaluation stack using ldind.i.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_ldind_i: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::loadResult(ops, 32);
    }
};

// ldind_r4 (78 (0x4e))
        // Description:
        //   Load a value indirectly from the address on top of the evaluation stack using ldind.r4.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_ldind_r4: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::loadResult(ops, 32);
    }
};

// ldind_r8 (79 (0x4f))
        // Description:
        //   Load a value indirectly from the address on top of the evaluation stack using ldind.r8.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_ldind_r8: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::loadResult(ops, 64);
    }
};

// ldind_ref (80 (0x50))
        // Description:
        //   Load a value indirectly from the address on top of the evaluation stack using ldind.ref.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_ldind_ref: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::loadResult(ops, 32);
    }
};

// stind_ref (81 (0x51))
        // Description:
        //   Store a value indirectly through the address on the evaluation stack using stind.ref.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_stind_ref: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::storeEffect(ops);
    }
};

// stind_i1 (82 (0x52))
        // Description:
        //   Store a value indirectly through the address on the evaluation stack using stind.i1.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_stind_i1: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::storeEffect(ops);
    }
};

// stind_i2 (83 (0x53))
        // Description:
        //   Store a value indirectly through the address on the evaluation stack using stind.i2.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_stind_i2: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::storeEffect(ops);
    }
};

// stind_i4 (84 (0x54))
        // Description:
        //   Store a value indirectly through the address on the evaluation stack using stind.i4.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_stind_i4: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::storeEffect(ops);
    }
};

// stind_i8 (85 (0x55))
        // Description:
        //   Store a value indirectly through the address on the evaluation stack using stind.i8.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_stind_i8: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::storeEffect(ops);
    }
};

// stind_r4 (86 (0x56))
        // Description:
        //   Store a value indirectly through the address on the evaluation stack using stind.r4.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_stind_r4: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::storeEffect(ops);
    }
};

// stind_r8 (87 (0x57))
        // Description:
        //   Store a value indirectly through the address on the evaluation stack using stind.r8.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_stind_r8: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::storeEffect(ops);
    }
};

// add (88 (0x58))
        // Description:
        //   Pop two values, compute add, and push the result onto the evaluation stack.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_add: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::binaryResult(ops);
    }
};

// sub (89 (0x59))
        // Description:
        //   Pop two values, compute sub, and push the result onto the evaluation stack.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_sub: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::binaryResult(ops);
    }
};

// mul (90 (0x5a))
        // Description:
        //   Pop two values, compute mul, and push the result onto the evaluation stack.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_mul: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::binaryResult(ops);
    }
};

// div (91 (0x5b))
        // Description:
        //   Pop two values, compute div, and push the result onto the evaluation stack.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_div: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::binaryResult(ops);
    }
};

// div_un (92 (0x5c))
        // Description:
        //   Pop two values, compute div.un, and push the result onto the evaluation stack.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_div_un: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::binaryResult(ops);
    }
};

// rem (93 (0x5d))
        // Description:
        //   Pop two values, compute rem, and push the result onto the evaluation stack.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_rem: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::binaryResult(ops);
    }
};

// rem_un (94 (0x5e))
        // Description:
        //   Pop two values, compute rem.un, and push the result onto the evaluation stack.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_rem_un: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::binaryResult(ops);
    }
};

// and (95 (0x5f))
        // Description:
        //   Pop two values, compute and, and push the result onto the evaluation stack.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_and: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::binaryResult(ops);
    }
};

// or (96 (0x60))
        // Description:
        //   Pop two values, compute or, and push the result onto the evaluation stack.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_or: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::binaryResult(ops);
    }
};

// xor (97 (0x61))
        // Description:
        //   Pop two values, compute xor, and push the result onto the evaluation stack.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_xor: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::binaryResult(ops);
    }
};

// shl (98 (0x62))
        // Description:
        //   Pop two values, compute shl, and push the result onto the evaluation stack.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_shl: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::binaryResult(ops);
    }
};

// shr (99 (0x63))
        // Description:
        //   Pop two values, compute shr, and push the result onto the evaluation stack.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_shr: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::binaryResult(ops);
    }
};

// shr_un (100 (0x64))
        // Description:
        //   Pop two values, compute shr.un, and push the result onto the evaluation stack.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_shr_un: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::binaryResult(ops);
    }
};

// neg (101 (0x65))
        // Description:
        //   Execute the CIL instruction neg.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_neg: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::unaryResult(ops);
    }
};

// not (102 (0x66))
        // Description:
        //   Execute the CIL instruction not.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_not: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::unaryResult(ops);
    }
};

// conv_i1 (103 (0x67))
        // Description:
        //   Convert the top evaluation-stack value according to conv.i1.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_conv_i1: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::convertResult(ops);
    }
};

// conv_i2 (104 (0x68))
        // Description:
        //   Convert the top evaluation-stack value according to conv.i2.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_conv_i2: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::convertResult(ops);
    }
};

// conv_i4 (105 (0x69))
        // Description:
        //   Convert the top evaluation-stack value according to conv.i4.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_conv_i4: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::convertResult(ops);
    }
};

// conv_i8 (106 (0x6a))
        // Description:
        //   Convert the top evaluation-stack value according to conv.i8.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_conv_i8: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::convertResult(ops);
    }
};

// conv_r4 (107 (0x6b))
        // Description:
        //   Convert the top evaluation-stack value according to conv.r4.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_conv_r4: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::convertResult(ops);
    }
};

// conv_r8 (108 (0x6c))
        // Description:
        //   Convert the top evaluation-stack value according to conv.r8.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_conv_r8: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::convertResult(ops);
    }
};

// conv_u4 (109 (0x6d))
        // Description:
        //   Convert the top evaluation-stack value according to conv.u4.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_conv_u4: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::convertResult(ops);
    }
};

// conv_u8 (110 (0x6e))
        // Description:
        //   Convert the top evaluation-stack value according to conv.u8.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_conv_u8: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::convertResult(ops);
    }
};

// callvirt (111 (0x6f))
        // Description:
        //   Call, create, or otherwise transfer to the metadata target specified by callvirt.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_callvirt: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 1);
        CilSemantics::callEffect(ops);
    }
};

// cpobj (112 (0x70))
        // Description:
        //   Execute the CIL instruction cpobj.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_cpobj: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 1);
        CilSemantics::storeEffect(ops);
    }
};

// ldobj (113 (0x71))
        // Description:
        //   Load a value indirectly from the address on top of the evaluation stack using ldobj.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_ldobj: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 1);
        CilSemantics::loadResult(ops, 32);
    }
};

// ldstr (114 (0x72))
        // Description:
        //   Push a CIL value modeled here as an unknown 32-bit value for ldstr.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_ldstr: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 1);
        CilSemantics::pushUnknown(ops, 32);
    }
};

// newobj (115 (0x73))
        // Description:
        //   Call, create, or otherwise transfer to the metadata target specified by newobj.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_newobj: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 1);
        CilSemantics::callEffect(ops);
    }
};

// castclass (116 (0x74))
        // Description:
        //   Execute the CIL instruction castclass.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_castclass: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 1);
        CilSemantics::unaryResult(ops);
    }
};

// isinst (117 (0x75))
        // Description:
        //   Execute the CIL instruction isinst.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_isinst: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 1);
        CilSemantics::unaryResult(ops);
    }
};

// conv_r_un (118 (0x76))
        // Description:
        //   Convert the top evaluation-stack value according to conv.r.un.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_conv_r_un: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::convertResult(ops);
    }
};

// unbox (121 (0x79))
        // Description:
        //   Execute the CIL instruction unbox.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_unbox: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 1);
        CilSemantics::unaryResult(ops);
    }
};

// throw (122 (0x7a))
        // Description:
        //   Throw or rethrow an exception using throw.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_throw: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::throwEffect(ops);
    }
};

// ldfld (123 (0x7b))
        // Description:
        //   Load a field value or field address identified by the inline metadata token using ldfld.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_ldfld: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 1);
        CilSemantics::loadResult(ops, 32);
    }
};

// ldflda (124 (0x7c))
        // Description:
        //   Load a field value or field address identified by the inline metadata token using ldflda.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_ldflda: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 1);
        CilSemantics::pushAddress(ops);
    }
};

// stfld (125 (0x7d))
        // Description:
        //   Store the top value into the field identified by the inline metadata token using stfld.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_stfld: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 1);
        CilSemantics::storeEffect(ops);
    }
};

// ldsfld (126 (0x7e))
        // Description:
        //   Push a CIL value modeled here as an unknown 32-bit value for ldsfld.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_ldsfld: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 1);
        CilSemantics::pushUnknown(ops, 32);
    }
};

// ldsflda (127 (0x7f))
        // Description:
        //   Push a managed/native address modeled here as an unknown pointer-sized value for ldsflda.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_ldsflda: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 1);
        CilSemantics::pushAddress(ops);
    }
};

// stsfld (128 (0x80))
        // Description:
        //   Execute the CIL instruction stsfld.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_stsfld: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 1);
        CilSemantics::doNothing(ops);
    }
};

// stobj (129 (0x81))
        // Description:
        //   Store a value indirectly through the address on the evaluation stack using stobj.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_stobj: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 1);
        CilSemantics::storeEffect(ops);
    }
};

// conv_ovf_i1_un (130 (0x82))
        // Description:
        //   Convert the top evaluation-stack value according to conv.ovf.i1.un.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_conv_ovf_i1_un: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::convertResult(ops);
    }
};

// conv_ovf_i2_un (131 (0x83))
        // Description:
        //   Convert the top evaluation-stack value according to conv.ovf.i2.un.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_conv_ovf_i2_un: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::convertResult(ops);
    }
};

// conv_ovf_i4_un (132 (0x84))
        // Description:
        //   Convert the top evaluation-stack value according to conv.ovf.i4.un.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_conv_ovf_i4_un: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::convertResult(ops);
    }
};

// conv_ovf_i8_un (133 (0x85))
        // Description:
        //   Convert the top evaluation-stack value according to conv.ovf.i8.un.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_conv_ovf_i8_un: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::convertResult(ops);
    }
};

// conv_ovf_u1_un (134 (0x86))
        // Description:
        //   Convert the top evaluation-stack value according to conv.ovf.u1.un.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_conv_ovf_u1_un: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::convertResult(ops);
    }
};

// conv_ovf_u2_un (135 (0x87))
        // Description:
        //   Convert the top evaluation-stack value according to conv.ovf.u2.un.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_conv_ovf_u2_un: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::convertResult(ops);
    }
};

// conv_ovf_u4_un (136 (0x88))
        // Description:
        //   Convert the top evaluation-stack value according to conv.ovf.u4.un.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_conv_ovf_u4_un: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::convertResult(ops);
    }
};

// conv_ovf_u8_un (137 (0x89))
        // Description:
        //   Convert the top evaluation-stack value according to conv.ovf.u8.un.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_conv_ovf_u8_un: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::convertResult(ops);
    }
};

// conv_ovf_i_un (138 (0x8a))
        // Description:
        //   Convert the top evaluation-stack value according to conv.ovf.i.un.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_conv_ovf_i_un: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::convertResult(ops);
    }
};

// conv_ovf_u_un (139 (0x8b))
        // Description:
        //   Convert the top evaluation-stack value according to conv.ovf.u.un.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_conv_ovf_u_un: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::convertResult(ops);
    }
};

// box (140 (0x8c))
        // Description:
        //   Execute the CIL instruction box.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_box: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 1);
        CilSemantics::unaryResult(ops);
    }
};

// newarr (141 (0x8d))
        // Description:
        //   Execute the CIL instruction newarr.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_newarr: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 1);
        CilSemantics::unaryResult(ops);
    }
};

// ldlen (142 (0x8e))
        // Description:
        //   Execute the CIL instruction ldlen.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_ldlen: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::unaryResult(ops);
    }
};

// ldelema (143 (0x8f))
        // Description:
        //   Pop two values, compute ldelema, and push the result onto the evaluation stack.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_ldelema: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::binaryResult(ops);
    }
};

// ldelem_i1 (144 (0x90))
        // Description:
        //   Load an array element selected by array reference and index using ldelem.i1.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_ldelem_i1: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::loadResult(ops, 32);
    }
};

// ldelem_u1 (145 (0x91))
        // Description:
        //   Load an array element selected by array reference and index using ldelem.u1.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_ldelem_u1: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::loadResult(ops, 32);
    }
};

// ldelem_i2 (146 (0x92))
        // Description:
        //   Load an array element selected by array reference and index using ldelem.i2.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_ldelem_i2: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::loadResult(ops, 32);
    }
};

// ldelem_u2 (147 (0x93))
        // Description:
        //   Load an array element selected by array reference and index using ldelem.u2.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_ldelem_u2: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::loadResult(ops, 32);
    }
};

// ldelem_i4 (148 (0x94))
        // Description:
        //   Load an array element selected by array reference and index using ldelem.i4.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_ldelem_i4: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::loadResult(ops, 32);
    }
};

// ldelem_u4 (149 (0x95))
        // Description:
        //   Load an array element selected by array reference and index using ldelem.u4.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_ldelem_u4: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::loadResult(ops, 32);
    }
};

// ldelem_i8 (150 (0x96))
        // Description:
        //   Load an array element selected by array reference and index using ldelem.i8.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_ldelem_i8: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::loadResult(ops, 64);
    }
};

// ldelem_i (151 (0x97))
        // Description:
        //   Load an array element selected by array reference and index using ldelem.i.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_ldelem_i: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::loadResult(ops, 32);
    }
};

// ldelem_r4 (152 (0x98))
        // Description:
        //   Load an array element selected by array reference and index using ldelem.r4.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_ldelem_r4: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::loadResult(ops, 32);
    }
};

// ldelem_r8 (153 (0x99))
        // Description:
        //   Load an array element selected by array reference and index using ldelem.r8.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_ldelem_r8: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::loadResult(ops, 64);
    }
};

// ldelem_ref (154 (0x9a))
        // Description:
        //   Load an array element selected by array reference and index using ldelem.ref.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_ldelem_ref: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::loadResult(ops, 32);
    }
};

// stelem_i (155 (0x9b))
        // Description:
        //   Store an array element selected by array reference and index using stelem.i.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_stelem_i: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::storeEffect(ops);
    }
};

// stelem_i1 (156 (0x9c))
        // Description:
        //   Store an array element selected by array reference and index using stelem.i1.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_stelem_i1: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::storeEffect(ops);
    }
};

// stelem_i2 (157 (0x9d))
        // Description:
        //   Store an array element selected by array reference and index using stelem.i2.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_stelem_i2: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::storeEffect(ops);
    }
};

// stelem_i4 (158 (0x9e))
        // Description:
        //   Store an array element selected by array reference and index using stelem.i4.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_stelem_i4: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::storeEffect(ops);
    }
};

// stelem_i8 (159 (0x9f))
        // Description:
        //   Store an array element selected by array reference and index using stelem.i8.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_stelem_i8: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::storeEffect(ops);
    }
};

// stelem_r4 (160 (0xa0))
        // Description:
        //   Store an array element selected by array reference and index using stelem.r4.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_stelem_r4: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::storeEffect(ops);
    }
};

// stelem_r8 (161 (0xa1))
        // Description:
        //   Store an array element selected by array reference and index using stelem.r8.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_stelem_r8: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::storeEffect(ops);
    }
};

// stelem_ref (162 (0xa2))
        // Description:
        //   Store an array element selected by array reference and index using stelem.ref.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_stelem_ref: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::storeEffect(ops);
    }
};

// ldelem (163 (0xa3))
        // Description:
        //   Load an array element selected by array reference and index using ldelem.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_ldelem: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::loadResult(ops, 32);
    }
};

// stelem (164 (0xa4))
        // Description:
        //   Store an array element selected by array reference and index using stelem.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_stelem: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::storeEffect(ops);
    }
};

// unbox_any (165 (0xa5))
        // Description:
        //   Execute the CIL instruction unbox.any.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_unbox_any: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 1);
        CilSemantics::unaryResult(ops);
    }
};

// conv_ovf_i1 (179 (0xb3))
        // Description:
        //   Convert the top evaluation-stack value according to conv.ovf.i1.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_conv_ovf_i1: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::convertResult(ops);
    }
};

// conv_ovf_u1 (180 (0xb4))
        // Description:
        //   Convert the top evaluation-stack value according to conv.ovf.u1.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_conv_ovf_u1: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::convertResult(ops);
    }
};

// conv_ovf_i2 (181 (0xb5))
        // Description:
        //   Convert the top evaluation-stack value according to conv.ovf.i2.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_conv_ovf_i2: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::convertResult(ops);
    }
};

// conv_ovf_u2 (182 (0xb6))
        // Description:
        //   Convert the top evaluation-stack value according to conv.ovf.u2.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_conv_ovf_u2: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::convertResult(ops);
    }
};

// conv_ovf_i4 (183 (0xb7))
        // Description:
        //   Convert the top evaluation-stack value according to conv.ovf.i4.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_conv_ovf_i4: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::convertResult(ops);
    }
};

// conv_ovf_u4 (184 (0xb8))
        // Description:
        //   Convert the top evaluation-stack value according to conv.ovf.u4.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_conv_ovf_u4: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::convertResult(ops);
    }
};

// conv_ovf_i8 (185 (0xb9))
        // Description:
        //   Convert the top evaluation-stack value according to conv.ovf.i8.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_conv_ovf_i8: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::convertResult(ops);
    }
};

// conv_ovf_u8 (186 (0xba))
        // Description:
        //   Convert the top evaluation-stack value according to conv.ovf.u8.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_conv_ovf_u8: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::convertResult(ops);
    }
};

// refanyval (194 (0xc2))
        // Description:
        //   Execute the CIL instruction refanyval.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_refanyval: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 1);
        CilSemantics::unaryResult(ops);
    }
};

// ckfinite (195 (0xc3))
        // Description:
        //   Execute the CIL instruction ckfinite.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_ckfinite: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::unaryResult(ops);
    }
};

// mkrefany (198 (0xc6))
        // Description:
        //   Execute the CIL instruction mkrefany.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_mkrefany: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 1);
        CilSemantics::unaryResult(ops);
    }
};

// ldtoken (208 (0xd0))
        // Description:
        //   Push a CIL value modeled here as an unknown 32-bit value for ldtoken.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_ldtoken: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 1);
        CilSemantics::pushUnknown(ops, 32);
    }
};

// conv_u2 (209 (0xd1))
        // Description:
        //   Convert the top evaluation-stack value according to conv.u2.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_conv_u2: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::convertResult(ops);
    }
};

// conv_u1 (210 (0xd2))
        // Description:
        //   Convert the top evaluation-stack value according to conv.u1.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_conv_u1: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::convertResult(ops);
    }
};

// conv_i (211 (0xd3))
        // Description:
        //   Convert the top evaluation-stack value according to conv.i.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_conv_i: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::convertResult(ops);
    }
};

// conv_ovf_i (212 (0xd4))
        // Description:
        //   Convert the top evaluation-stack value according to conv.ovf.i.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_conv_ovf_i: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::convertResult(ops);
    }
};

// conv_ovf_u (213 (0xd5))
        // Description:
        //   Convert the top evaluation-stack value according to conv.ovf.u.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_conv_ovf_u: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::convertResult(ops);
    }
};

// add_ovf (214 (0xd6))
        // Description:
        //   Pop two values, compute add.ovf, and push the result onto the evaluation stack.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_add_ovf: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::binaryResult(ops);
    }
};

// add_ovf_un (215 (0xd7))
        // Description:
        //   Pop two values, compute add.ovf.un, and push the result onto the evaluation stack.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_add_ovf_un: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::binaryResult(ops);
    }
};

// mul_ovf (216 (0xd8))
        // Description:
        //   Pop two values, compute mul.ovf, and push the result onto the evaluation stack.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_mul_ovf: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::binaryResult(ops);
    }
};

// mul_ovf_un (217 (0xd9))
        // Description:
        //   Pop two values, compute mul.ovf.un, and push the result onto the evaluation stack.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_mul_ovf_un: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::binaryResult(ops);
    }
};

// sub_ovf (218 (0xda))
        // Description:
        //   Pop two values, compute sub.ovf, and push the result onto the evaluation stack.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_sub_ovf: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::binaryResult(ops);
    }
};

// sub_ovf_un (219 (0xdb))
        // Description:
        //   Pop two values, compute sub.ovf.un, and push the result onto the evaluation stack.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_sub_ovf_un: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::binaryResult(ops);
    }
};

// endfinally (220 (0xdc))
        // Description:
        //   Execute the CIL instruction endfinally.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_endfinally: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::doNothing(ops);
    }
};

// leave (221 (0xdd))
        // Description:
        //   Unconditionally transfer control to the inline target for leave.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_leave: P {
    void p(D dispatcher, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 1);
        CilSemantics::branchEffect(dispatcher, ops, insn, args);
    }
};

// leave_s (222 (0xde))
        // Description:
        //   Unconditionally transfer control to the inline target for leave.s.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_leave_s: P {
    void p(D dispatcher, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 1);
        CilSemantics::branchEffect(dispatcher, ops, insn, args);
    }
};

// stind_i (223 (0xdf))
        // Description:
        //   Store a value indirectly through the address on the evaluation stack using stind.i.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_stind_i: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::storeEffect(ops);
    }
};

// conv_u (224 (0xe0))
        // Description:
        //   Convert the top evaluation-stack value according to conv.u.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_conv_u: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::convertResult(ops);
    }
};

// arglist (65024 (0xfe00))
        // Description:
        //   Push a CIL value modeled here as an unknown 32-bit value for arglist.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_arglist: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::pushUnknown(ops, 32);
    }
};

// ceq (65025 (0xfe01))
        // Description:
        //   Pop two values, compare them using ceq, and push an int32 boolean result.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_ceq: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::compareResult(ops);
    }
};

// cgt (65026 (0xfe02))
        // Description:
        //   Pop two values, compare them using cgt, and push an int32 boolean result.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_cgt: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::compareResult(ops);
    }
};

// cgt_un (65027 (0xfe03))
        // Description:
        //   Pop two values, compare them using cgt.un, and push an int32 boolean result.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_cgt_un: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::compareResult(ops);
    }
};

// clt (65028 (0xfe04))
        // Description:
        //   Pop two values, compare them using clt, and push an int32 boolean result.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_clt: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::compareResult(ops);
    }
};

// clt_un (65029 (0xfe05))
        // Description:
        //   Pop two values, compare them using clt.un, and push an int32 boolean result.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_clt_un: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::compareResult(ops);
    }
};

// ldftn (65030 (0xfe06))
        // Description:
        //   Push a CIL value modeled here as an unknown 32-bit value for ldftn.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_ldftn: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 1);
        CilSemantics::pushUnknown(ops, 32);
    }
};

// ldvirtftn (65031 (0xfe07))
        // Description:
        //   Execute the CIL instruction ldvirtftn.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_ldvirtftn: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 1);
        CilSemantics::unaryResult(ops);
    }
};

// ldarg (65033 (0xfe09))
        // Description:
        //   Load an argument, selected by an inline index operand, onto the evaluation stack.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_ldarg: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 1);
        ops->pushOperand(ops->readLocal(CilSemantics::asIndex(args[0])));
    }
};

// ldarga (65034 (0xfe0a))
        // Description:
        //   Load the address of an argument, selected by an inline index operand, onto the evaluation stack.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_ldarga: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 1);
        CilSemantics::pushAddress(ops);
    }
};

// starg (65035 (0xfe0b))
        // Description:
        //   Store the top evaluation-stack value into an argument selected by an inline index operand.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_starg: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 1);
        CilSemantics::discard(ops, 1);
    }
};

// ldloc (65036 (0xfe0c))
        // Description:
        //   Load a local variable, selected by an inline index operand, onto the evaluation stack.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_ldloc: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 1);
        ops->pushOperand(ops->readLocal(CilSemantics::asIndex(args[0])));
    }
};

// ldloca (65037 (0xfe0d))
        // Description:
        //   Load the address of a local variable, selected by an inline index operand, onto the evaluation stack.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_ldloca: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 1);
        CilSemantics::pushAddress(ops);
    }
};

// stloc (65038 (0xfe0e))
        // Description:
        //   Store the top evaluation-stack value into a local variable selected by an inline index operand.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_stloc: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 1);
        ops->writeLocal(CilSemantics::asIndex(args[0]), ops->popOperand());
    }
};

// localloc (65039 (0xfe0f))
        // Description:
        //   Execute the CIL instruction localloc.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_localloc: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::unaryResult(ops);
    }
};

// endfilter (65041 (0xfe11))
        // Description:
        //   Execute the CIL instruction endfilter.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_endfilter: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::doNothing(ops);
    }
};

// unaligned (65042 (0xfe12))
        // Description:
        //   CIL prefix instruction that modifies interpretation of the following instruction: unaligned.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_unaligned: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 1);
        CilSemantics::prefixEffect(ops);
    }
};

// volatile (65043 (0xfe13))
        // Description:
        //   CIL prefix instruction that modifies interpretation of the following instruction: volatile.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_volatile: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::prefixEffect(ops);
    }
};

// tail (65044 (0xfe14))
        // Description:
        //   CIL prefix instruction that modifies interpretation of the following instruction: tail.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_tail: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::prefixEffect(ops);
    }
};

// initobj (65045 (0xfe15))
        // Description:
        //   Execute the CIL instruction initobj.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_initobj: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 1);
        CilSemantics::doNothing(ops);
    }
};

// constrained (65046 (0xfe16))
        // Description:
        //   CIL prefix instruction that modifies interpretation of the following instruction: constrained.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_constrained: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 1);
        CilSemantics::prefixEffect(ops);
    }
};

// cpblk (65047 (0xfe17))
        // Description:
        //   Copy a block of memory.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_cpblk: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::copyBlock(ops);
    }
};

// initblk (65048 (0xfe18))
        // Description:
        //   Initialize a block of memory.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_initblk: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::initBlock(ops);
    }
};

// rethrow (65050 (0xfe1a))
        // Description:
        //   Throw or rethrow an exception using rethrow.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_rethrow: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::throwEffect(ops);
    }
};

// sizeof (65052 (0xfe1c))
        // Description:
        //   Push a CIL value modeled here as an unknown 32-bit value for sizeof.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_sizeof: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 1);
        CilSemantics::pushUnknown(ops, 32);
    }
};

// refanytype (65053 (0xfe1d))
        // Description:
        //   Execute the CIL instruction refanytype.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_refanytype: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::unaryResult(ops);
    }
};

// readonly (65054 (0xfe1e))
        // Description:
        //   CIL prefix instruction that modifies interpretation of the following instruction: readonly.
        // Notes:
        //   Generated in the same per-instruction-processor style as DispatcherJvm.C. Some effects are conservative until metadata/type-aware CIL helpers are connected.
        // Run-time Exceptions:
        //   CIL/CLI exceptions are not modeled precisely by this initial dispatcher skeleton.
struct IP_readonly: P {
    void p(D /*dispatcher*/, Ops ops, I insn, Args args) override {
        assert_args(insn, args, 0);
        CilSemantics::prefixEffect(ops);
    }
};

} // namespace Cil

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      DispatcherCil
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
DispatcherCil::initializeDispatchTable() {
    iprocSet(0x00, new Cil::IP_nop);
    iprocSet(0x01, new Cil::IP_break);
    iprocSet(0x02, new Cil::IP_ldarg_0);
    iprocSet(0x03, new Cil::IP_ldarg_1);
    iprocSet(0x04, new Cil::IP_ldarg_2);
    iprocSet(0x05, new Cil::IP_ldarg_3);
    iprocSet(0x06, new Cil::IP_ldloc_0);
    iprocSet(0x07, new Cil::IP_ldloc_1);
    iprocSet(0x08, new Cil::IP_ldloc_2);
    iprocSet(0x09, new Cil::IP_ldloc_3);
    iprocSet(0x0a, new Cil::IP_stloc_0);
    iprocSet(0x0b, new Cil::IP_stloc_1);
    iprocSet(0x0c, new Cil::IP_stloc_2);
    iprocSet(0x0d, new Cil::IP_stloc_3);
    iprocSet(0x0e, new Cil::IP_ldarg_s);
    iprocSet(0x0f, new Cil::IP_ldarga_s);
    iprocSet(0x10, new Cil::IP_starg_s);
    iprocSet(0x11, new Cil::IP_ldloc_s);
    iprocSet(0x12, new Cil::IP_ldloca_s);
    iprocSet(0x13, new Cil::IP_stloc_s);
    iprocSet(0x14, new Cil::IP_ldnull);
    iprocSet(0x15, new Cil::IP_ldc_i4_m1);
    iprocSet(0x16, new Cil::IP_ldc_i4_0);
    iprocSet(0x17, new Cil::IP_ldc_i4_1);
    iprocSet(0x18, new Cil::IP_ldc_i4_2);
    iprocSet(0x19, new Cil::IP_ldc_i4_3);
    iprocSet(0x1a, new Cil::IP_ldc_i4_4);
    iprocSet(0x1b, new Cil::IP_ldc_i4_5);
    iprocSet(0x1c, new Cil::IP_ldc_i4_6);
    iprocSet(0x1d, new Cil::IP_ldc_i4_7);
    iprocSet(0x1e, new Cil::IP_ldc_i4_8);
    iprocSet(0x1f, new Cil::IP_ldc_i4_s);
    iprocSet(0x20, new Cil::IP_ldc_i4);
    iprocSet(0x21, new Cil::IP_ldc_i8);
    iprocSet(0x22, new Cil::IP_ldc_r4);
    iprocSet(0x23, new Cil::IP_ldc_r8);
    iprocSet(0x25, new Cil::IP_dup);
    iprocSet(0x26, new Cil::IP_pop);
    iprocSet(0x27, new Cil::IP_jmp);
    iprocSet(0x28, new Cil::IP_call);
    iprocSet(0x29, new Cil::IP_calli);
    iprocSet(0x2a, new Cil::IP_ret);
    iprocSet(0x2b, new Cil::IP_br_s);
    iprocSet(0x2c, new Cil::IP_brfalse_s);
    iprocSet(0x2d, new Cil::IP_brtrue_s);
    iprocSet(0x2e, new Cil::IP_beq_s);
    iprocSet(0x2f, new Cil::IP_bge_s);
    iprocSet(0x30, new Cil::IP_bgt_s);
    iprocSet(0x31, new Cil::IP_ble_s);
    iprocSet(0x32, new Cil::IP_blt_s);
    iprocSet(0x33, new Cil::IP_bne_un_s);
    iprocSet(0x34, new Cil::IP_bge_un_s);
    iprocSet(0x35, new Cil::IP_bgt_un_s);
    iprocSet(0x36, new Cil::IP_ble_un_s);
    iprocSet(0x37, new Cil::IP_blt_un_s);
    iprocSet(0x38, new Cil::IP_br);
    iprocSet(0x39, new Cil::IP_brfalse);
    iprocSet(0x3a, new Cil::IP_brtrue);
    iprocSet(0x3b, new Cil::IP_beq);
    iprocSet(0x3c, new Cil::IP_bge);
    iprocSet(0x3d, new Cil::IP_bgt);
    iprocSet(0x3e, new Cil::IP_ble);
    iprocSet(0x3f, new Cil::IP_blt);
    iprocSet(0x40, new Cil::IP_bne_un);
    iprocSet(0x41, new Cil::IP_bge_un);
    iprocSet(0x42, new Cil::IP_bgt_un);
    iprocSet(0x43, new Cil::IP_ble_un);
    iprocSet(0x44, new Cil::IP_blt_un);
    iprocSet(0x45, new Cil::IP_switch);
    iprocSet(0x46, new Cil::IP_ldind_i1);
    iprocSet(0x47, new Cil::IP_ldind_u1);
    iprocSet(0x48, new Cil::IP_ldind_i2);
    iprocSet(0x49, new Cil::IP_ldind_u2);
    iprocSet(0x4a, new Cil::IP_ldind_i4);
    iprocSet(0x4b, new Cil::IP_ldind_u4);
    iprocSet(0x4c, new Cil::IP_ldind_i8);
    iprocSet(0x4d, new Cil::IP_ldind_i);
    iprocSet(0x4e, new Cil::IP_ldind_r4);
    iprocSet(0x4f, new Cil::IP_ldind_r8);
    iprocSet(0x50, new Cil::IP_ldind_ref);
    iprocSet(0x51, new Cil::IP_stind_ref);
    iprocSet(0x52, new Cil::IP_stind_i1);
    iprocSet(0x53, new Cil::IP_stind_i2);
    iprocSet(0x54, new Cil::IP_stind_i4);
    iprocSet(0x55, new Cil::IP_stind_i8);
    iprocSet(0x56, new Cil::IP_stind_r4);
    iprocSet(0x57, new Cil::IP_stind_r8);
    iprocSet(0x58, new Cil::IP_add);
    iprocSet(0x59, new Cil::IP_sub);
    iprocSet(0x5a, new Cil::IP_mul);
    iprocSet(0x5b, new Cil::IP_div);
    iprocSet(0x5c, new Cil::IP_div_un);
    iprocSet(0x5d, new Cil::IP_rem);
    iprocSet(0x5e, new Cil::IP_rem_un);
    iprocSet(0x5f, new Cil::IP_and);
    iprocSet(0x60, new Cil::IP_or);
    iprocSet(0x61, new Cil::IP_xor);
    iprocSet(0x62, new Cil::IP_shl);
    iprocSet(0x63, new Cil::IP_shr);
    iprocSet(0x64, new Cil::IP_shr_un);
    iprocSet(0x65, new Cil::IP_neg);
    iprocSet(0x66, new Cil::IP_not);
    iprocSet(0x67, new Cil::IP_conv_i1);
    iprocSet(0x68, new Cil::IP_conv_i2);
    iprocSet(0x69, new Cil::IP_conv_i4);
    iprocSet(0x6a, new Cil::IP_conv_i8);
    iprocSet(0x6b, new Cil::IP_conv_r4);
    iprocSet(0x6c, new Cil::IP_conv_r8);
    iprocSet(0x6d, new Cil::IP_conv_u4);
    iprocSet(0x6e, new Cil::IP_conv_u8);
    iprocSet(0x6f, new Cil::IP_callvirt);
    iprocSet(0x70, new Cil::IP_cpobj);
    iprocSet(0x71, new Cil::IP_ldobj);
    iprocSet(0x72, new Cil::IP_ldstr);
    iprocSet(0x73, new Cil::IP_newobj);
    iprocSet(0x74, new Cil::IP_castclass);
    iprocSet(0x75, new Cil::IP_isinst);
    iprocSet(0x76, new Cil::IP_conv_r_un);
    iprocSet(0x79, new Cil::IP_unbox);
    iprocSet(0x7a, new Cil::IP_throw);
    iprocSet(0x7b, new Cil::IP_ldfld);
    iprocSet(0x7c, new Cil::IP_ldflda);
    iprocSet(0x7d, new Cil::IP_stfld);
    iprocSet(0x7e, new Cil::IP_ldsfld);
    iprocSet(0x7f, new Cil::IP_ldsflda);
    iprocSet(0x80, new Cil::IP_stsfld);
    iprocSet(0x81, new Cil::IP_stobj);
    iprocSet(0x82, new Cil::IP_conv_ovf_i1_un);
    iprocSet(0x83, new Cil::IP_conv_ovf_i2_un);
    iprocSet(0x84, new Cil::IP_conv_ovf_i4_un);
    iprocSet(0x85, new Cil::IP_conv_ovf_i8_un);
    iprocSet(0x86, new Cil::IP_conv_ovf_u1_un);
    iprocSet(0x87, new Cil::IP_conv_ovf_u2_un);
    iprocSet(0x88, new Cil::IP_conv_ovf_u4_un);
    iprocSet(0x89, new Cil::IP_conv_ovf_u8_un);
    iprocSet(0x8a, new Cil::IP_conv_ovf_i_un);
    iprocSet(0x8b, new Cil::IP_conv_ovf_u_un);
    iprocSet(0x8c, new Cil::IP_box);
    iprocSet(0x8d, new Cil::IP_newarr);
    iprocSet(0x8e, new Cil::IP_ldlen);
    iprocSet(0x8f, new Cil::IP_ldelema);
    iprocSet(0x90, new Cil::IP_ldelem_i1);
    iprocSet(0x91, new Cil::IP_ldelem_u1);
    iprocSet(0x92, new Cil::IP_ldelem_i2);
    iprocSet(0x93, new Cil::IP_ldelem_u2);
    iprocSet(0x94, new Cil::IP_ldelem_i4);
    iprocSet(0x95, new Cil::IP_ldelem_u4);
    iprocSet(0x96, new Cil::IP_ldelem_i8);
    iprocSet(0x97, new Cil::IP_ldelem_i);
    iprocSet(0x98, new Cil::IP_ldelem_r4);
    iprocSet(0x99, new Cil::IP_ldelem_r8);
    iprocSet(0x9a, new Cil::IP_ldelem_ref);
    iprocSet(0x9b, new Cil::IP_stelem_i);
    iprocSet(0x9c, new Cil::IP_stelem_i1);
    iprocSet(0x9d, new Cil::IP_stelem_i2);
    iprocSet(0x9e, new Cil::IP_stelem_i4);
    iprocSet(0x9f, new Cil::IP_stelem_i8);
    iprocSet(0xa0, new Cil::IP_stelem_r4);
    iprocSet(0xa1, new Cil::IP_stelem_r8);
    iprocSet(0xa2, new Cil::IP_stelem_ref);
    iprocSet(0xa3, new Cil::IP_ldelem);
    iprocSet(0xa4, new Cil::IP_stelem);
    iprocSet(0xa5, new Cil::IP_unbox_any);
    iprocSet(0xb3, new Cil::IP_conv_ovf_i1);
    iprocSet(0xb4, new Cil::IP_conv_ovf_u1);
    iprocSet(0xb5, new Cil::IP_conv_ovf_i2);
    iprocSet(0xb6, new Cil::IP_conv_ovf_u2);
    iprocSet(0xb7, new Cil::IP_conv_ovf_i4);
    iprocSet(0xb8, new Cil::IP_conv_ovf_u4);
    iprocSet(0xb9, new Cil::IP_conv_ovf_i8);
    iprocSet(0xba, new Cil::IP_conv_ovf_u8);
    iprocSet(0xc2, new Cil::IP_refanyval);
    iprocSet(0xc3, new Cil::IP_ckfinite);
    iprocSet(0xc6, new Cil::IP_mkrefany);
    iprocSet(0xd0, new Cil::IP_ldtoken);
    iprocSet(0xd1, new Cil::IP_conv_u2);
    iprocSet(0xd2, new Cil::IP_conv_u1);
    iprocSet(0xd3, new Cil::IP_conv_i);
    iprocSet(0xd4, new Cil::IP_conv_ovf_i);
    iprocSet(0xd5, new Cil::IP_conv_ovf_u);
    iprocSet(0xd6, new Cil::IP_add_ovf);
    iprocSet(0xd7, new Cil::IP_add_ovf_un);
    iprocSet(0xd8, new Cil::IP_mul_ovf);
    iprocSet(0xd9, new Cil::IP_mul_ovf_un);
    iprocSet(0xda, new Cil::IP_sub_ovf);
    iprocSet(0xdb, new Cil::IP_sub_ovf_un);
    iprocSet(0xdc, new Cil::IP_endfinally);
    iprocSet(0xdd, new Cil::IP_leave);
    iprocSet(0xde, new Cil::IP_leave_s);
    iprocSet(0xdf, new Cil::IP_stind_i);
    iprocSet(0xe0, new Cil::IP_conv_u);
    iprocSet(0xfe00, new Cil::IP_arglist);
    iprocSet(0xfe01, new Cil::IP_ceq);
    iprocSet(0xfe02, new Cil::IP_cgt);
    iprocSet(0xfe03, new Cil::IP_cgt_un);
    iprocSet(0xfe04, new Cil::IP_clt);
    iprocSet(0xfe05, new Cil::IP_clt_un);
    iprocSet(0xfe06, new Cil::IP_ldftn);
    iprocSet(0xfe07, new Cil::IP_ldvirtftn);
    iprocSet(0xfe09, new Cil::IP_ldarg);
    iprocSet(0xfe0a, new Cil::IP_ldarga);
    iprocSet(0xfe0b, new Cil::IP_starg);
    iprocSet(0xfe0c, new Cil::IP_ldloc);
    iprocSet(0xfe0d, new Cil::IP_ldloca);
    iprocSet(0xfe0e, new Cil::IP_stloc);
    iprocSet(0xfe0f, new Cil::IP_localloc);
    iprocSet(0xfe11, new Cil::IP_endfilter);
    iprocSet(0xfe12, new Cil::IP_unaligned);
    iprocSet(0xfe13, new Cil::IP_volatile);
    iprocSet(0xfe14, new Cil::IP_tail);
    iprocSet(0xfe15, new Cil::IP_initobj);
    iprocSet(0xfe16, new Cil::IP_constrained);
    iprocSet(0xfe17, new Cil::IP_cpblk);
    iprocSet(0xfe18, new Cil::IP_initblk);
    iprocSet(0xfe1a, new Cil::IP_rethrow);
    iprocSet(0xfe1c, new Cil::IP_sizeof);
    iprocSet(0xfe1d, new Cil::IP_refanytype);
    iprocSet(0xfe1e, new Cil::IP_readonly);
}

DispatcherCil::DispatcherCil() {}

DispatcherCil::~DispatcherCil() {}

DispatcherCil::DispatcherCil(const Architecture::Base::ConstPtr &arch)
    : BaseSemantics::Dispatcher(arch) {}

DispatcherCil::DispatcherCil(const Architecture::Base::ConstPtr &arch, const BaseSemantics::RiscOperators::Ptr &ops)
    : Super(arch, ops),
      REG_PC(arch->registerDictionary()->findOrThrow("pc")) {
    initializeDispatchTable();
    initializeMemoryState();
    initializeState(ops->currentState());
}

DispatcherCil::Ptr
DispatcherCil::instance(const Architecture::Base::ConstPtr &arch) {
    return Ptr(new DispatcherCil(arch));
}

DispatcherCil::Ptr
DispatcherCil::instance(const Architecture::Base::ConstPtr &arch, const BaseSemantics::RiscOperators::Ptr &ops) {
    return Ptr(new DispatcherCil(arch, ops));
}

BaseSemantics::Dispatcher::Ptr
DispatcherCil::create(const BaseSemantics::RiscOperators::Ptr &ops) const {
    return instance(architecture(), ops);
}

DispatcherCil::Ptr
DispatcherCil::promote(const BaseSemantics::Dispatcher::Ptr &d) {
    Ptr retval = as<DispatcherCil>(d);
    ASSERT_not_null(retval);
    return retval;
}

int
DispatcherCil::iprocKey(SgAsmInstruction *insn_) const {
    SgAsmCilInstruction *insn = isSgAsmCilInstruction(insn_);
    ASSERT_not_null(insn);
    return static_cast<int>(insn->get_kind());
}

InsnProcessor*
DispatcherCil::iprocLookup(SgAsmInstruction* insn) {
    int key = (int) insn->get_rawBytes()[0];
    ASSERT_require(key >= 0);
    return iprocGet(key);
}

void
DispatcherCil::initializeMemoryState() {
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

RegisterDescriptor
DispatcherCil::instructionPointerRegister() const {
    return REG_PC;
}

RegisterDescriptor
DispatcherCil::stackPointerRegister() const {
    return RegisterDescriptor();
}


SValuePtr
DispatcherCil::condition(CilInstructionKind, RiscOperators *ops) {
    ASSERT_not_null(ops);
    return ops->undefined_(1);
}

BaseSemantics::SValuePtr
DispatcherCil::read(SgAsmExpression *e, size_t value_nbits, size_t addr_nbits/*=0*/) {
    ASSERT_not_null(e);
    return Dispatcher::read(e, value_nbits, addr_nbits);
}

void
DispatcherCil::updateFpsrExcInan(const SValuePtr&, SgAsmType*, const SValuePtr&, SgAsmType*) {}

void
DispatcherCil::updateFpsrExcIde(const SValuePtr&, SgAsmType*, const SValuePtr&, SgAsmType*) {}

void
DispatcherCil::updateFpsrExcOvfl(const SValuePtr&, SgAsmType*, SgAsmType*, SgAsmType*) {}

void
DispatcherCil::updateFpsrExcUnfl(const SValuePtr&, SgAsmType*, SgAsmType*, SgAsmType*) {}

void
DispatcherCil::updateFpsrExcInex() {}

void
DispatcherCil::accumulateFpExceptions() {}

void
DispatcherCil::adjustFpConditionCodes(const SValuePtr&, SgAsmFloatType*) {}

} // namespace InstructionSemantics
} // namespace BinaryAnalysis
} // namespace Rose

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
BOOST_CLASS_EXPORT_IMPLEMENT(Rose::BinaryAnalysis::InstructionSemantics::DispatcherCil);
#endif

#endif
