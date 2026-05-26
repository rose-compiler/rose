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

#include <SgAsmExpression.h>
#include <SgAsmIntegerValueExpression.h>
#include <SgAsmValueExpression.h>

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


// Auto-completed JVM instruction semantics skeleton for ROSE-style JVM semantics.
// Source for instruction descriptions: Java SE 26 JVM Specification, Chapter 6.5.
// Generated from the user's partial jvmSemantics.C and completed for the requested opcodes.
//
// The small JvmSemantics adapter functions referenced below intentionally isolate
// JVM-specific operations that depend on your surrounding framework: constant-pool
// access, object/array modeling, exception modeling, method return/branch effects,
// and category-1/category-2 stack manipulation.  Bind these helpers to the
// existing ROSE JVM semantics state where those facilities live.

namespace JvmSemantics {
    // Adapter declarations expected from the surrounding JVM semantics framework.
    using D = DispatcherJvm*;
    using Ops = RiscOperators*;
    using I = SgAsmJvmInstruction*;
    using A = const SgAsmExpressionPtrList&;

    uint8_t asU1(const SgAsmExpression* expr) {
        auto ivExpr = isSgAsmIntegerValueExpression(expr);
        ASSERT_not_null(ivExpr);
        return static_cast<uint8_t>(ivExpr->get_value());
    }

    SValue::Ptr
    arrayLength(Ops /*ops*/, SValue::Ptr array) {
        ASSERT_require(array->kind() == JvmValueKind::ArrayReference);
        ASSERT_require(array->hasArrayLength());

        // Get the array length from the array and convert it to Integer kind
        SValuePtr length = array->arrayLength();
        length->kind(JvmValueKind::Integer);
        return length;
    }

    SValue::Ptr
    arrayLoad(Ops /*ops*/, const std::string &/*kind*/, SValue::Ptr array, SValue::Ptr index) {
        // Validate harshly for now
        ASSERT_require(array->kind() == JvmValueKind::ArrayReference);
        ASSERT_require(index->kind() == JvmValueKind::Integer);

        // Ignore modeling a symbolic heap for now
        return {};
    }

    void
    arrayStore(Ops /*ops*/, const std::string &kind, SValue::Ptr array, SValue::Ptr index, SValue::Ptr value) {
        // Validate harshly for now
        ASSERT_require(array->kind() == JvmValueKind::ArrayReference);
        ASSERT_require(index->kind() == JvmValueKind::Integer);
        if (kind == "b") {
            ASSERT_require(value->kind() == JvmValueKind::Integer);
        }
        else {
            ASSERT_require2(false, "unknown kind not implemented for arrayStore()\n");
        }
        // Ignore modeling a symbolic heap for now
    }

    SValue::Ptr
    makeConstant(Ops ops, const std::string &kind, int64_t value, size_t nBits) {
        ASSERT_not_null(ops);
        JvmValueKind valueKind{JvmValueKind::Unknown};

        //TODO: convert to switch
        if (kind == "a") {
            valueKind = JvmValueKind::ArrayReference;
        }
        else if (kind == "o") {
            valueKind = JvmValueKind::ObjectReference;
        }
        else if (kind == "b") {
            valueKind = JvmValueKind::Integer;
        }
        else if (kind == "i") {
            valueKind = JvmValueKind::Integer;
        }
        else if (kind == "l") {
            valueKind = JvmValueKind::Long;
        }
        else if (kind == "f") {
            valueKind = JvmValueKind::Float;
        }
        else if (kind == "d") {
            valueKind = JvmValueKind::Double;
        }
        else {
            ASSERT_require2(false, "unimplemented JvmValueKind in makeConstant()\n");
        }

        // Create the SValue and set its type/kind
        auto sval = ops->number_(nBits, value);
        sval->kind(valueKind);

        return sval;
    }

} // namespace JvmSemantics

using JvmSemantics::asU1;
using JvmSemantics::arrayLength;
using JvmSemantics::arrayLoad;
using JvmSemantics::makeConstant;

// arraylength (190 (0xbe))
        // Description:
        //   Get the length of an array.
        // Run-time Exceptions:
        //   NullPointerException if arrayref is null.
struct IP_arraylength: P {
    void p(D /*d*/, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        SValue::Ptr arrayRef = ops->popOperand();
#if 0
// Verification
        JvmSemantics::throwIfNull(ops, "NullPointerException", arrayRef);
#endif
        ops->pushOperand(arrayLength(ops, arrayRef));
    }
};

// aload (25 (0x19))
        // Description:
        //   Load a reference onto the stack from a local variable #index.
        // Notes:
        //   The _<n> form uses an implicit local-variable index.
        //   aload cannot load returnAddress values; astore can store them.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_aload: P {
    void p(D /*d*/, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        uint8_t idx = asU1(args[0]);
        ops->pushOperand(ops->readLocal(idx));
    }
};

// aload_0 (42 (0x2a))
        // Description:
        //   Load a reference onto the stack from local variable 0.
        // Notes:
        //   The _<n> form uses an implicit local-variable index.
        //   aload cannot load returnAddress values; astore can store them.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_aload_0: P {
    void p(D /*d*/, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        ops->pushOperand(ops->readLocal(0));
    }
};

// aload_1 (43 (0x2b))
        // Description:
        //   Load a reference onto the stack from local variable 1.
        // Notes:
        //   The _<n> form uses an implicit local-variable index.
        //   aload cannot load returnAddress values; astore can store them.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_aload_1: P {
    void p(D /*d*/, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        ops->pushOperand(ops->readLocal(1));
    }
};

// aload_2 (44 (0x2c))
        // Description:
        //   Load a reference onto the stack from local variable 2.
        // Notes:
        //   The _<n> form uses an implicit local-variable index.
        //   aload cannot load returnAddress values; astore can store them.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_aload_2: P {
    void p(D /*d*/, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        ops->pushOperand(ops->readLocal(2));
    }
};

// aload_3 (45 (0x2d))
        // Description:
        //   Load a reference onto the stack from local variable 3.
        // Notes:
        //   The _<n> form uses an implicit local-variable index.
        //   aload cannot load returnAddress values; astore can store them.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_aload_3: P {
    void p(D /*d*/, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        ops->pushOperand(ops->readLocal(3));
    }
};

// anewarray (189 (0xbd))
        // Description:
        //   Create a new array of references of length count and component type identified by the class reference index.
        // Run-time Exceptions:
        //   NegativeArraySizeException if any requested dimension is negative.
struct IP_anewarray: P {
    void p(D /*d*/, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        uint8_t b1 = asU1(args[0]);
        uint8_t b2 = asU1(args[0]);
        // index into constant pool for array component type
        int64_t constantPoolIndex = (static_cast<uint16_t>(b1) << 8) | static_cast<uint16_t>(b2);
        (void) constantPoolIndex; // defeat clang warning

        // Pop the count and set it to have ArrayReference properties
        SValue::Ptr count = ops->popOperand();
        count->kind(JvmValueKind::ArrayReference);
        count->arrayLength(count);
        count->typeDescriptor("<unknown-type>"); // obtain from constant pool

        ops->pushOperand(count);
    }
};

// astore (58 (0x3a))
        // Description:
        //   Store a reference into a local variable #index
        // Notes:
        //   The _<n> form uses an implicit local-variable index.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_astore: P {
    void p(D /*d*/, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        uint8_t idx = asU1(args[0]);
        ops->writeLocal(idx, ops->popOperand());
    }
};

// astore_0 (75 (0x4b))
        // Description:
        //   Store a reference into local variable 0.
        // Notes:
        //   The _<n> form uses an implicit local-variable index.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_astore_0: P {
    void p(D /*d*/, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        ops->writeLocal(0, ops->popOperand());
    }
};

// astore_1 (76 (0x4c))
        // Description:
        //   Store a reference into local variable 1.
        // Notes:
        //   The _<n> form uses an implicit local-variable index.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_astore_1: P {
    void p(D /*d*/, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        ops->writeLocal(1, ops->popOperand());
    }
};

// astore_2 (77 (0x4d))
        // Description:
        //   Store a reference into local variable 2.
        // Notes:
        //   The _<n> form uses an implicit local-variable index.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_astore_2: P {
    void p(D /*d*/, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        ops->writeLocal(2, ops->popOperand());
    }
};

// astore_3 (78 (0x4e))
        // Description:
        //   Store a reference into local variable 3.
        // Notes:
        //   The _<n> form uses an implicit local-variable index.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_astore_3: P {
    void p(D /*d*/, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        ops->writeLocal(3, ops->popOperand());
    }
};

// baload (51 (0x33))
        // Description:
        //   Load a byte or Boolean value from an array
        // Run-time Exceptions:
        //   NullPointerException if arrayref is null.
        //   ArrayIndexOutOfBoundsException if index is outside the array bounds.
struct IP_baload: P {
    void p(D /*d*/, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        SValue::Ptr index = ops->popOperand();
        SValue::Ptr array = ops->popOperand();
#if 0
// Verification
        verify(...);
        JvmSemantics::throwIfNull(ops, "NullPointerException", array);
        JvmSemantics::throwIfArrayIndexOutOfBounds(ops, array, index);
#endif
        ops->pushOperand(JvmSemantics::arrayLoad(ops, "b", array, index));
    }
};

// bastore (84 (0x54))
        // Description:
        //   Store a byte or Boolean value into an array
        // Run-time Exceptions:
        //   NullPointerException if arrayref is null.
        //   ArrayIndexOutOfBoundsException if index is outside the array bounds.
struct IP_bastore: P {
    void p(D /*d*/, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        SValue::Ptr value = ops->popOperand();
        SValue::Ptr index = ops->popOperand();
        SValue::Ptr array = ops->popOperand();
#if 0
// Verification
        verify(...);
        JvmSemantics::throwIfNull(ops, "NullPointerException", array);
        JvmSemantics::throwIfArrayIndexOutOfBounds(ops, array, index);
#endif
        JvmSemantics::arrayStore(ops, "b", array, index, value);
    }
};

// bipush (16 (0x10))
        // Description:
        //   Push a byte onto the stack as an integer value.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_bipush: P {
    void p(D /*d*/, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        int64_t imm = asU1(args[0]);
        ops->pushOperand(makeConstant(ops, "b", imm, 32));
    }
};

// dup2 (92 (0x5c))
        // Description:
        //   Rearrange or discard operand stack values according to the JVM category-1/category-2 stack-form rules.
        // Notes:
        //   These instructions must not be used to split a category-2 value.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_dup2: P {
    void p(D /*d*/, Ops /*ops*/, I insn, A args) {
        assert_args(insn, args, 0);

//TODO:Implement
#if 0
        const SValuePtr &top = ops->peekOperand();
        if (top.kind() == JvmValueKind::Long || top.kind() == JvmValueKind::Double) {
            ops->pushOperand(top);
            return;
        }

        // ASSERT_require(operandStack_.size() >= 2);
        SValuePtr v1 = ops->popOperand();
        SValuePtr v2 = ops->popOperand();

        // ASSERT_require(isCategory1(value1.kind));
        ASSERT_require(v1.kind() != JvmValueKind::Long & v1.kind() != JvmValueKind::Double) {
        ASSERT_require(v2.kind() != JvmValueKind::Long & v2.kind() != JvmValueKind::Double) {

        ops->pushOperand(v2);
        ops->pushOperand(v1);
        ops->pushOperand(v2);
        ops->pushOperand(v1);
#endif
    }
};

// dconst_0 (14 (0x0e))
        // Description:
        //   Push the constant 0.0 (a double) onto the stack
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_dconst_0: P {
    void p(D /*d*/, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        ops->pushOperand(makeConstant(ops, "d", 0, 64));
    }
};

// dconst_1 (15 (0x0f))
        // Description:
        //   Push the constant 1.0 (a double) onto the stack
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_dconst_1: P {
    void p(D /*d*/, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        ops->pushOperand(makeConstant(ops, "d", 1, 64));
    }
};

// fconst_0 (11 (0x0b))
        // Description:
        //   Push 0.0f on the stack
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_fconst_0: P {
    void p(D /*d*/, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        ops->pushOperand(makeConstant(ops, "f", 0, 32));
    }
};

// fconst_1 (12 (0x0c))
        // Description:
        //   Push 1.0f on the stack
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_fconst_1: P {
    void p(D /*d*/, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        ops->pushOperand(makeConstant(ops, "f", 1, 32));
    }
};

// fconst_2 (13 (0x0d))
        // Description:
        //   Push 2.0f on the stack
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_fconst_2: P {
    void p(D /*d*/, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        ops->pushOperand(makeConstant(ops, "f", 2, 32));
    }
};

// fload (23 (0x17))
        // Description:
        //   Load a float value from a local variable #index.
        // Notes:
        //   The _<n> form uses an implicit local-variable index.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_fload: P {
    void p(D /*d*/, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        uint8_t idx = asU1(args[0]);
        ops->pushOperand(ops->readLocal(idx));
    }
};

// fload_0 (34 (0x22))
        // Description:
        //   Load a float value from local variable 0.
        // Notes:
        //   The _<n> form uses an implicit local-variable index.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_fload_0: P {
    void p(D /*d*/, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        ops->pushOperand(ops->readLocal(0));
    }
};

// fload_1 (35 (0x23))
        // Description:
        //   Load a float value from local variable 1.
        // Notes:
        //   The _<n> form uses an implicit local-variable index.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_fload_1: P {
    void p(D /*d*/, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        ops->pushOperand(ops->readLocal(1));
    }
};

// fload_2 (36 (0x24))
        // Description:
        //   Load a float value from local variable 2.
        // Notes:
        //   The _<n> form uses an implicit local-variable index.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_fload_2: P {
    void p(D /*d*/, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        ops->pushOperand(ops->readLocal(2));
    }
};

// fload_3 (37 (0x25))
        // Description:
        //   Load a float value from local variable 3.
        // Notes:
        //   The _<n> form uses an implicit local-variable index.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_fload_3: P {
    void p(D /*d*/, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        ops->pushOperand(ops->readLocal(3));
    }
};

// fstore (56 (0x38))
        // Description:
        //   Store a float value into a local variable #index
        // Notes:
        //   The _<n> form uses an implicit local-variable index.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_fstore: P {
    void p(D /*d*/, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        uint8_t idx = asU1(args[0]);
        ops->writeLocal(idx, ops->popOperand());
    }
};

// fstore_0 (67 (0x43))
        // Description:
        //   Store a float value into local variable 0
        // Notes:
        //   The _<n> form uses an implicit local-variable index.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_fstore_0: P {
    void p(D /*d*/, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        ops->writeLocal(0, ops->popOperand());
    }
};

// fstore_1 (68 (0x44))
        // Description:
        //   Store a float value into local variable 1
        // Notes:
        //   The _<n> form uses an implicit local-variable index.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_fstore_1: P {
    void p(D /*d*/, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        ops->writeLocal(1, ops->popOperand());
    }
};

// fstore_2 (69 (0x45))
        // Description:
        //   Store a float value into local variable 2
        // Notes:
        //   The _<n> form uses an implicit local-variable index.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_fstore_2: P {
    void p(D /*d*/, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        ops->writeLocal(2, ops->popOperand());
    }
};

// fstore_3 (70 (0x46))
        // Description:
        //   Store a float value into local variable 3
        // Notes:
        //   The _<n> form uses an implicit local-variable index.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_fstore_3: P {
    void p(D /*d*/, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        ops->writeLocal(3, ops->popOperand());
    }
};

// iadd (96 (0x60))
        // Description:
        //   Pop operands, apply the arithmetic/bitwise operation, and push the result.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_iadd: P {
    void p(D /*d*/, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        SValue::Ptr v2 = ops->popOperand();
        SValue::Ptr v1 = ops->popOperand();
        ops->pushOperand(ops->add(v1, v2));
    }
};

// iconst_0 (3 (0x03))
        // Description:
        //   Load the int value 0 onto the stack.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_iconst_0: P {
    void p(D /*d*/, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        ops->pushOperand(makeConstant(ops, "i", 0, 32));
    }
};

// iconst_1 (4 (0x04))
        // Description:
        //   Load the int value 1 onto the stack.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_iconst_1: P {
    void p(D /*d*/, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        ops->pushOperand(makeConstant(ops, "i", 1, 32));
    }
};

// iconst_2 (5 (0x05))
        // Description:
        //   Load the int value 2 onto the stack.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_iconst_2: P {
    void p(D /*d*/, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        ops->pushOperand(makeConstant(ops, "i", 2, 32));
    }
};

// iconst_3 (6 (0x06))
        // Description:
        //   Load the int value 3 onto the stack.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_iconst_3: P {
    void p(D /*d*/, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        ops->pushOperand(makeConstant(ops, "i", 3, 32));
    }
};

// iconst_4 (7 (0x07))
        // Description:
        //   Load the int value 4 onto the stack.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_iconst_4: P {
    void p(D /*d*/, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        ops->pushOperand(makeConstant(ops, "i", 4, 32));
    }
};

// iconst_5 (8 (0x08))
        // Description:
        //   Load the int value 5 onto the stack.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_iconst_5: P {
    void p(D /*d*/, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        ops->pushOperand(makeConstant(ops, "i", 5, 32));
    }
};

// iload (21 (0x15))
        // Description:
        //   Load an int value from a local variable #index
        // Notes:
        //   The _<n> form uses an implicit local-variable index.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_iload: P {
    void p(D /*d*/, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        uint8_t idx = asU1(args[0]);
        ops->pushOperand(ops->readLocal(idx));
    }
};

// iload_0 (26 (0x1a))
        // Description:
        //   Load an int value from a local variable #index
        // Notes:
        //   The _<n> form uses an implicit local-variable index.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_iload_0: P {
    void p(D /*d*/, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        ops->pushOperand(ops->readLocal(0));
    }
};

// iload_1 (27 (0x1b))
        // Description:
        //   Load a int value from a local variable and push it.
        // Notes:
        //   The _<n> form uses an implicit local-variable index.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_iload_1: P {
    void p(D /*d*/, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        ops->pushOperand(ops->readLocal(1));
    }
};

// iload_2 (28 (0x1c))
        // Description:
        //   Load a int value from a local variable and push it.
        // Notes:
        //   The _<n> form uses an implicit local-variable index.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_iload_2: P {
    void p(D /*d*/, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        ops->pushOperand(ops->readLocal(2));
    }
};

// iload_3 (29 (0x1d))
        // Description:
        //   Load a int value from a local variable and push it.
        // Notes:
        //   The _<n> form uses an implicit local-variable index.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_iload_3: P {
    void p(D /*d*/, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        ops->pushOperand(ops->readLocal(3));
    }
};

// istore (54 (0x36))
        // Description:
        //   Pop a int value and store it into a local variable.
        // Notes:
        //   The _<n> form uses an implicit local-variable index.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_istore: P {
    void p(D /*d*/, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        const uint8_t idx = asU1(args[0]);
        ops->writeLocal(idx, ops->popOperand());
    }
};

// istore_0 (59 (0x3b))
        // Description:
        //   Pop a int value and store it into a local variable.
        // Notes:
        //   The _<n> form uses an implicit local-variable index.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_istore_0: P {
    void p(D /*d*/, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        ops->writeLocal(0, ops->popOperand());
    }
};

// istore_1 (60 (0x3c))
        // Description:
        //   Pop a int value and store it into a local variable.
        // Notes:
        //   The _<n> form uses an implicit local-variable index.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_istore_1: P {
    void p(D /*d*/, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        ops->writeLocal(1, ops->popOperand());
    }
};

// istore_2 (61 (0x3d))
        // Description:
        //   Pop a int value and store it into a local variable.
        // Notes:
        //   The _<n> form uses an implicit local-variable index.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_istore_2: P {
    void p(D /*d*/, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        ops->writeLocal(2, ops->popOperand());
    }
};

// istore_3 (62 (0x3e))
        // Description:
        //   Pop a int value and store it into a local variable.
        // Notes:
        //   The _<n> form uses an implicit local-variable index.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_istore_3: P {
    void p(D /*d*/, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        ops->writeLocal(3, ops->popOperand());
    }
};

// lconst_0 (9 (0x09))
        // Description:
        //   Push 0L (the number zero with type long) onto the stack
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_lconst_0: P {
    void p(D /*d*/, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        ops->pushOperand(makeConstant(ops, "l", 0, 64));
    }
};

// lconst_1 (10 (0x0a))
        // Description:
        //   Push 1L (the number one with type long) onto the stack
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_lconst_1: P {
    void p(D /*d*/, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        ops->pushOperand(makeConstant(ops, "l", 1, 64));
    }
};

// lload (22 (0x16))
        // Description:
        //   Load a long value from a local variable and push it.
        // Notes:
        //   The _<n> form uses an implicit local-variable index.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_lload: P {
    void p(D /*d*/, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        const uint32_t index = asU1(args[0]);
        ops->pushOperand(ops->readLocal(index));
    }
};

// lload_0 (30 (0x1e))
        // Description:
        //   Load a long value from a local variable and push it.
        // Notes:
        //   The _<n> form uses an implicit local-variable index.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_lload_0: P {
    void p(D /*d*/, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        ops->pushOperand(ops->readLocal(0));
    }
};

// lload_1 (31 (0x1f))
        // Description:
        //   Load a long value from a local variable and push it.
        // Notes:
        //   The _<n> form uses an implicit local-variable index.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_lload_1: P {
    void p(D /*d*/, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        ops->pushOperand(ops->readLocal(1));
    }
};

// lload_2 (32 (0x20))
        // Description:
        //   Load a long value from a local variable and push it.
        // Notes:
        //   The _<n> form uses an implicit local-variable index.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_lload_2: P {
    void p(D /*d*/, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        ops->pushOperand(ops->readLocal(2));
    }
};

// lload_3 (33 (0x21))
        // Description:
        //   Load a long value from a local variable and push it.
        // Notes:
        //   The _<n> form uses an implicit local-variable index.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_lload_3: P {
    void p(D /*d*/, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        ops->pushOperand(ops->readLocal(3));
    }
};

// lstore (55 (0x37))
        // Description:
        //   Pop a long value and store it into a local variable.
        // Notes:
        //   The _<n> form uses an implicit local-variable index.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_lstore: P {
    void p(D /*d*/, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        const uint32_t index = asU1(args[0]);
        ops->writeLocal(index, ops->popOperand());
    }
};

// lstore_0 (63 (0x3f))
        // Description:
        //   Pop a long value and store it into a local variable.
        // Notes:
        //   The _<n> form uses an implicit local-variable index.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_lstore_0: P {
    void p(D /*d*/, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        ops->writeLocal(0, ops->popOperand());
    }
};

// lstore_1 (64 (0x40))
        // Description:
        //   Pop a long value and store it into a local variable.
        // Notes:
        //   The _<n> form uses an implicit local-variable index.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_lstore_1: P {
    void p(D /*d*/, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        ops->writeLocal(1, ops->popOperand());
    }
};

// lstore_2 (65 (0x41))
        // Description:
        //   Pop a long value and store it into a local variable.
        // Notes:
        //   The _<n> form uses an implicit local-variable index.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_lstore_2: P {
    void p(D /*d*/, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        ops->writeLocal(2, ops->popOperand());
    }
};

// lstore_3 (66 (0x42))
        // Description:
        //   Pop a long value and store it into a local variable.
        // Notes:
        //   The _<n> form uses an implicit local-variable index.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_lstore_3: P {
    void p(D /*d*/, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        ops->writeLocal(3, ops->popOperand());
    }
};

} // namespace


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      DispatcherJvm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
DispatcherJvm::initializeDispatchTable() {
    iprocSet(0xbd,  new Jvm::IP_anewarray);
    iprocSet(0xbe,  new Jvm::IP_arraylength);
    iprocSet(0x10,  new Jvm::IP_bipush);

    iprocSet(0x19,  new Jvm::IP_aload);
    iprocSet(0x2a,  new Jvm::IP_aload_0);
    iprocSet(0x2b,  new Jvm::IP_aload_1);
    iprocSet(0x2c,  new Jvm::IP_aload_2);
    iprocSet(0x2d,  new Jvm::IP_aload_3);

    iprocSet(0x3a,  new Jvm::IP_astore);
    iprocSet(0x4b,  new Jvm::IP_astore_0);
    iprocSet(0x4c,  new Jvm::IP_astore_1);
    iprocSet(0x4d,  new Jvm::IP_astore_2);
    iprocSet(0x4e,  new Jvm::IP_astore_3);

// Load a constant onto the operand stack.
    iprocSet(0x0e,  new Jvm::IP_dconst_0);
    iprocSet(0x0f,  new Jvm::IP_dconst_1);

    iprocSet(0x0b,  new Jvm::IP_fconst_0);
    iprocSet(0x0c,  new Jvm::IP_fconst_1);
    iprocSet(0x0d,  new Jvm::IP_fconst_2);

    iprocSet(0x17,  new Jvm::IP_fload);
    iprocSet(0x22,  new Jvm::IP_fload_0);
    iprocSet(0x23,  new Jvm::IP_fload_1);
    iprocSet(0x24,  new Jvm::IP_fload_2);
    iprocSet(0x25,  new Jvm::IP_fload_3);

    iprocSet(0x38,  new Jvm::IP_fstore);
    iprocSet(0x43,  new Jvm::IP_fstore_0);
    iprocSet(0x44,  new Jvm::IP_fstore_1);
    iprocSet(0x45,  new Jvm::IP_fstore_2);
    iprocSet(0x46,  new Jvm::IP_fstore_3);

    iprocSet(0x03,  new Jvm::IP_iconst_0);
    iprocSet(0x04,  new Jvm::IP_iconst_1);
    iprocSet(0x05,  new Jvm::IP_iconst_2);
    iprocSet(0x06,  new Jvm::IP_iconst_3);
    iprocSet(0x07,  new Jvm::IP_iconst_4);
    iprocSet(0x08,  new Jvm::IP_iconst_5);

    iprocSet(0x09,  new Jvm::IP_lconst_0);
    iprocSet(0x0a,  new Jvm::IP_lconst_1);

    iprocSet(0x15,  new Jvm::IP_iload);
    iprocSet(0x1a,  new Jvm::IP_iload_0);
    iprocSet(0x1b,  new Jvm::IP_iload_1);
    iprocSet(0x1c,  new Jvm::IP_iload_2);
    iprocSet(0x1d,  new Jvm::IP_iload_3);

    iprocSet(0x36,  new Jvm::IP_istore);
    iprocSet(0x3b,  new Jvm::IP_istore_0);
    iprocSet(0x3c,  new Jvm::IP_istore_1);
    iprocSet(0x3d,  new Jvm::IP_istore_2);
    iprocSet(0x3e,  new Jvm::IP_istore_3);

    iprocSet(0x60,  new Jvm::IP_iadd);

    iprocSet(0x16,  new Jvm::IP_lload);
    iprocSet(0x1e,  new Jvm::IP_lload_0);
    iprocSet(0x1f,  new Jvm::IP_lload_1);
    iprocSet(0x20,  new Jvm::IP_lload_2);
    iprocSet(0x21,  new Jvm::IP_lload_3);

    iprocSet(0x37,  new Jvm::IP_lstore);
    iprocSet(0x3f,  new Jvm::IP_lstore_0);
    iprocSet(0x40,  new Jvm::IP_lstore_1);
    iprocSet(0x41,  new Jvm::IP_lstore_2);
    iprocSet(0x42,  new Jvm::IP_lstore_3);

    iprocSet(0x5c,  new Jvm::IP_dup2);
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
