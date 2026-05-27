#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/InstructionSemantics/DispatcherJvm.h>

#include <Rose/As.h>
#include <Rose/Affirm.h>
#include <Rose/BinaryAnalysis/Architecture/Base.h>
#include <Rose/BinaryAnalysis/InstructionEnumsJvm.h>
#include <Rose/BinaryAnalysis/InstructionSemantics.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/Dispatcher.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/Exception.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/MemoryState.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/RiscOperators.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/State.h>
#include <Rose/StringUtility/Diagnostics.h>

#include <SgAsmExpression.h>
#include <SgAsmIntegerValueExpression.h>
#include <SgAsmJvmInstruction.h>
#include <SgAsmOperandList.h>
#include <SgAsmValueExpression.h>

#include <sageInterface.h>
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
    using Args = const SgAsmExpressionPtrList&;

    virtual void p(D, Ops, I, Args) = 0;

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

    void assert_args(I insn, Args args, size_t nargs) {
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

    namespace MyDomain {

     // class SValue: public InstructionSemantics::SymbolicSemantics::SValue {
        class SValue: public BaseSemantics::SValue {
            SgType* type_ = nullptr;
            uint64_t arrayLength_ = 0;  // Track array length

        public:
            using Ptr = Sawyer::SharedPointer<SValue>;

            SgType* get_type() const { return type_; }
            void set_type(SgType* t) { type_ = t; }

            static Ptr promote(const InstructionSemantics::BaseSemantics::SValue::Ptr &v) {
                Ptr retval = as<SValue>(v);
                ASSERT_not_null(retval);
                return retval;
            }

            uint64_t getArrayLength() const { return arrayLength_; }
            void setArrayLength(uint64_t len) { arrayLength_ = len; }
        };

        BaseSemantics::SValue::Ptr arrayLength(Ops ops, BaseSemantics::SValue::Ptr arrayref) {
            auto myArray = MyDomain::SValue::promote(arrayref);
            if (myArray) {
                return ops->number_(32, myArray->getArrayLength());
            }
            throw std::runtime_error("Array reference is not a MyDomain::SValue");
        }

    class State: public  Rose::BinaryAnalysis::InstructionSemantics::SymbolicSemantics::State {
    public:
        using Super = InstructionSemantics::SymbolicSemantics::State;
        using Ptr = boost::shared_ptr<State>;

    private:
        std::set<uint64_t> ownedMonitors_; // Track owned object addresses

    protected:
        // Real constructors
        State(const BaseSemantics::RegisterState::Ptr &registers,
              const BaseSemantics::MemoryState::Ptr &memory)
            : Super(registers, memory) {}

        State(const State &other)
            : Super(other), ownedMonitors_(other.ownedMonitors_) {}

    public:
        // Static allocating constructors
        static Ptr instance(const BaseSemantics::RegisterState::Ptr &registers,
                           const BaseSemantics::MemoryState::Ptr &memory) {
            return Ptr(new State(registers, memory));
        }

        static Ptr instance(const Ptr &other) {
            return Ptr(new State(*other));
        }

        // Virtual constructors
        virtual BaseSemantics::State::Ptr create(
            const BaseSemantics::RegisterState::Ptr &registers,
            const BaseSemantics::MemoryState::Ptr &memory) const override {
            return instance(registers, memory);
        }

        virtual BaseSemantics::State::Ptr clone() const override {
            return Ptr(new State(*this));
        }

        // Dynamic pointer cast
        static Ptr promote(const BaseSemantics::State::Ptr &x) {
            Ptr retval = Rose::as<State>(x);
            ASSERT_not_null(retval);
            return retval;
        }

        // Monitor ownership tracking
        bool ownsMonitor(const SValue::Ptr& objectref) {
            auto addr = objectref->toUnsigned();
            return addr && ownedMonitors_.find(*addr) != ownedMonitors_.end();
        }

        void acquireMonitor(const SValue::Ptr& objectref) {
            auto addr = objectref->toUnsigned();
            if (addr) ownedMonitors_.insert(*addr);
        }

        void releaseMonitor(const SValue::Ptr& objectref) {
            auto addr = objectref->toUnsigned();
            if (addr) ownedMonitors_.erase(*addr);
        }
    };

    }

    uint8_t asU1(const SgAsmExpression* expr) {
        auto ivExpr = isSgAsmIntegerValueExpression(expr);
        ASSERT_not_null(ivExpr);
        return static_cast<uint8_t>(ivExpr->get_value());
    }

    SValue::Ptr
    makeConstant(Ops ops, const std::string &kind, int64_t value, size_t nBits) {
        ASSERT_not_null(ops);
        JvmValueKind valueKind{JvmValueKind::Unknown};

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

    using Args = std::vector<SgAsmExpression*>;

    uint8_t  u1(SgAsmExpression*);
    uint16_t u2(Args);
    int8_t   s1(Args);
    int16_t  s2(Args);

    SValue::Ptr nullReference(Ops ops);
    SValue::Ptr arrayLoad(Ops ops, const char *kind, SValue::Ptr arrayref, SValue::Ptr index);
    void        arrayStore(Ops ops, const char *kind, SValue::Ptr arrayref, SValue::Ptr index, SValue::Ptr value);

    SValue::Ptr sub(Ops ops, SValue::Ptr a, SValue::Ptr b);
    SValue::Ptr mul(Ops ops, SValue::Ptr a, SValue::Ptr b);
    SValue::Ptr div(Ops ops, SValue::Ptr a, SValue::Ptr b);
    SValue::Ptr rem(Ops ops, SValue::Ptr a, SValue::Ptr b);
    SValue::Ptr neg(Ops ops, SValue::Ptr a);
    SValue::Ptr binary(Ops ops, const char *op, SValue::Ptr a, SValue::Ptr b);
    SValue::Ptr convert(Ops ops, const char *op, SValue::Ptr a);
    SValue::Ptr compare(Ops ops, const char *op, SValue::Ptr a, SValue::Ptr b);

    void methodReturn(Ops ops, I insn);
    void methodReturn(Ops ops, I insn, SValue::Ptr value);

    void stack_dup(Ops ops);
    void stack_dup_x1(Ops ops);
    void stack_dup_x2(Ops ops);
    void stack_dup2(Ops ops);
    void stack_dup2_x1(Ops ops);
    void stack_dup2_x2(Ops ops);
    void stack_pop(Ops ops);
    void stack_pop2(Ops ops);
    void stack_swap(Ops ops);
    void branch_goto(Ops ops, I insn, Args args);
    void branch_goto_w(Ops ops, I insn, Args args);
    void branch_if_acmpeq(Ops ops, I insn, Args args);
    void branch_if_acmpne(Ops ops, I insn, Args args);
    void branch_if_icmpeq(Ops ops, I insn, Args args);
    void branch_if_icmpne(Ops ops, I insn, Args args);
    void branch_if_icmplt(Ops ops, I insn, Args args);
    void branch_if_icmpge(Ops ops, I insn, Args args);
    void branch_if_icmpgt(Ops ops, I insn, Args args);
    void branch_if_icmple(Ops ops, I insn, Args args);
    void branch_ifeq(Ops ops, I insn, Args args);
    void branch_ifne(Ops ops, I insn, Args args);
    void branch_iflt(Ops ops, I insn, Args args);
    void branch_ifge(Ops ops, I insn, Args args);
    void branch_ifgt(Ops ops, I insn, Args args);
    void branch_ifle(Ops ops, I insn, Args args);
    void branch_ifnonnull(Ops ops, I insn, Args args);
    void branch_ifnull(Ops ops, I insn, Args args);
    void branch_jsr(Ops ops, I insn, Args args);
    void branch_jsr_w(Ops ops, I insn, Args args);
    void branch_lookupswitch(Ops ops, I insn, Args args);
    void branch_tableswitch(Ops ops, I insn, Args args);
    void execute_anewarray(Ops ops, I insn, Args args);
    void execute_arraylength(Ops ops, I insn, Args args);
    void execute_athrow(Ops ops, I insn, Args args);
    void execute_checkcast(Ops ops, I insn, Args args);
    void execute_getfield(Ops ops, I insn, Args args);
    void execute_getstatic(Ops ops, I insn, Args args);
    void execute_iconst_4(Ops ops, I insn, Args args);
    void execute_iconst_5(Ops ops, I insn, Args args);
    void execute_iinc(Ops ops, I insn, Args args);
    void execute_instanceof(Ops ops, I insn, Args args);
    void execute_invokedynamic(Ops ops, I insn, Args args);
    void execute_invokeinterface(Ops ops, I insn, Args args);
    void execute_invokespecial(Ops ops, I insn, Args args);
    void execute_invokestatic(Ops ops, I insn, Args args);
    void execute_invokevirtual(Ops ops, I insn, Args args);
    void execute_ldc(Ops ops, I insn, Args args);
    void execute_ldc_w(Ops ops, I insn, Args args);
    void execute_ldc2_w(Ops ops, I insn, Args args);
    void execute_monitorenter(Ops ops, I insn, Args args);
    void execute_monitorexit(Ops ops, I insn, Args args);
    void execute_multianewarray(Ops ops, I insn, Args args);
    void execute_new(Ops ops, I insn, Args args);
    void execute_newarray(Ops ops, I insn, Args args);
    void execute_putfield(Ops ops, I insn, Args args);
    void execute_putstatic(Ops ops, I insn, Args args);
    void execute_ret(Ops ops, I insn, Args args);
    void execute_wide(Ops ops, I insn, Args args);


    // Runtime-exception hooks. These are intended to be executable checks in the
    // concrete JVM semantics state, not documentation-only comments. A symbolic
    // implementation can fork states or emit path predicates; a concrete
    // implementation should raise/model the named Java exception.
    void throwIfNull(Ops ops, const char *exceptionName, SValue::Ptr ref);
    void throwIfArrayIndexOutOfBounds(Ops ops, SValue::Ptr arrayref, SValue::Ptr index);
    void throwIfArrayStoreNotCompatible(Ops ops, SValue::Ptr arrayref, SValue::Ptr value);
    void throwIfIntegerDivisorZero(Ops ops, SValue::Ptr divisor);
    void throwIfLongDivisorZero(Ops ops, SValue::Ptr divisor);
    void throwIfNegativeArraySize(Ops ops, SValue::Ptr count);
    void throwIfClassCastFails(Ops ops, I insn, Args args, SValue::Ptr objectref);
    void throwIfMonitorOwnershipInvalid(Ops ops, SValue::Ptr objectref);

    SValue::Ptr arrayLength(Ops ops, SValue::Ptr arrayref);
} // namespace JvmSemantics


// -----------------------------------------------------------------------------
// Default helper definitions.
//
// These definitions remove the unresolved adapter declarations from the generated
// file.  The simple stack/local/arithmetic cases are implemented directly using
// the ROSE-style `Ops` interface already used in the original file.  Operations
// that require JVM class-file state, constant-pool resolution, heap/array state,
// monitor ownership, or CFG successor construction call `jvmUnsupported` rather
// than silently doing the wrong thing.
// -----------------------------------------------------------------------------

using JvmSemantics::makeConstant;
using JvmSemantics::asU1;

namespace JvmSemantics {

    [[noreturn]] static void jvmUnsupported(const char *name) {
        throw std::logic_error(std::string("JVM semantic helper not bound to framework state: ") + name);
    }

    // I'd like to name these, for example, asU1(...), this follows Robb's convention
    uint8_t u1(SgAsmExpression *e) {
        ASSERT_not_null(e);

        uint8_t val = 0;
        if (SgAsmIntegerValueExpression* ival = isSgAsmIntegerValueExpression(e))
           {
             val = ival->get_absoluteValue();
           }
          else
           {
             printf ("Error: in u1(): input SgAsmExpression* expression is not a SgAsmIntegerValueExpression* \n");
             ROSE_ASSERT(false);
           }

        return val;
    }

    uint16_t u2(Args args) {
        ASSERT_forbid(args.empty());
        uint16_t val = 0;
        if (SgAsmIntegerValueExpression* ival = isSgAsmIntegerValueExpression(args[0]))
           {
             val = ival->get_absoluteValue();
           }
          else
           {
             printf ("Error: in u2(): input SgAsmExpression* expression is not a SgAsmIntegerValueExpression* \n");
             ROSE_ASSERT(false);
           }

        return val;
    }

    int8_t s1(Args args) {
        ASSERT_forbid(args.empty());
        int8_t val = 0;
        if (SgAsmIntegerValueExpression* ival = isSgAsmIntegerValueExpression(args[0]))
           {
             val = ival->get_absoluteValue();
           }
          else
           {
             printf ("Error: in s1(): input SgAsmExpression* expression is not a SgAsmIntegerValueExpression* \n");
             ROSE_ASSERT(false);
           }

        return val;
    }

    int16_t s2(Args args) {
        ASSERT_forbid(args.empty());
        int16_t val = 0;
        if (SgAsmIntegerValueExpression* ival = isSgAsmIntegerValueExpression(args[0]))
           {
             val = ival->get_absoluteValue();
           }
          else
           {
             printf ("Error: in s2(): input SgAsmExpression* expression is not a SgAsmIntegerValueExpression* \n");
             ROSE_ASSERT(false);
           }

        return val;
    }

    SValue::Ptr nullReference(Ops ops) {
        // The JVMS does not mandate a concrete representation for null.  This
        // default model uses a zero-valued reference-sized symbolic value.
        return ops->number_(64, 0);
    }

    SValue::Ptr arrayLoad(Ops ops, const char * /*kind*/, SValue::Ptr arrayref, SValue::Ptr index) {
     // 1. Calculate the address (base + index)
        SValue::Ptr addr = ops->add(arrayref, index);

     // 2. Define the size of the read (e.g., 32 bits) using an undefined value as a template
        SValue::Ptr dflt = ops->undefined_(32);

     // 3. Define the condition (usually constant true)
        SValue::Ptr cond = ops->boolean_(true);

     // 4. Call readMemory with all 4 required SValue::Ptr arguments
        return ops->readMemory(RegisterDescriptor(), addr, dflt, cond);
    }

    void arrayStore(Ops ops, const char * /*kind*/, SValue::Ptr arrayref, SValue::Ptr index, SValue::Ptr value) {
     // 1. Calculate the effective address (base + index)
        SValue::Ptr addr = ops->add(arrayref, index);

     // 2. Define the condition (usually constant true)
        SValue::Ptr cond = ops->boolean_(true);

     // 3. Perform the memory write using RiscOperators
        ops->writeMemory(RegisterDescriptor(), addr, value, cond);
    }

    SValue::Ptr sub(Ops ops, SValue::Ptr a, SValue::Ptr b) {
        return ops->subtract(a, b);
    }

    SValue::Ptr mul(Ops ops, SValue::Ptr a, SValue::Ptr b) {
        return ops->signedMultiply(a, b);
    }

    SValue::Ptr div(Ops ops, SValue::Ptr a, SValue::Ptr b) {
        return ops->signedDivide(a, b);
    }

    SValue::Ptr rem(Ops ops, SValue::Ptr a, SValue::Ptr b) {
        return ops->signedModulo(a, b);
    }

    SValue::Ptr neg(Ops ops, SValue::Ptr a) {
        return ops->negate(a);
    }

    SValue::Ptr binary(Ops ops, const char *op, SValue::Ptr a, SValue::Ptr b) {
        if (0 == strcmp(op, "and"))  return ops->and_(a, b);
        if (0 == strcmp(op, "or"))   return ops->or_(a, b);
        if (0 == strcmp(op, "xor"))  return ops->xor_(a, b);
        if (0 == strcmp(op, "shl"))  return ops->shiftLeft(a, b);
        if (0 == strcmp(op, "shr"))  return ops->shiftRight(a, b);
        if (0 == strcmp(op, "ushr")) return ops->shiftRight(a, b);
        jvmUnsupported(op);
    }

    SValue::Ptr convert(Ops ops, const char *op, SValue::Ptr a) {
        if (0 == strcmp(op, "i2b")) return ops->signExtend(ops->extract(a, 0, 8), 32);
        if (0 == strcmp(op, "i2c")) return ops->unsignedExtend(ops->extract(a, 0, 16), 32);
        if (0 == strcmp(op, "i2s")) return ops->signExtend(ops->extract(a, 0, 16), 32);
        if (0 == strcmp(op, "l2i")) return ops->extract(a, 0, 32);
        if (0 == strcmp(op, "i2l")) return ops->signExtend(a, 64);
        // Floating-point conversions require the JVM floating-point semantic domain.
        jvmUnsupported(op);
    }

    SValue::Ptr compare(Ops ops, const char *op, SValue::Ptr a, SValue::Ptr b) {
        // Default integer-valued compare result.  Full JVM fcmp/dcmp NaN handling
        // requires the floating-point semantic domain and must be supplied by the framework.

        if (0 == strcmp(op, "lcmp")) {
        // Use the 'is' prefix for comparison operators
            SValue::Ptr lt = ops->isSignedLessThan(a, b);
            SValue::Ptr gt = ops->isSignedLessThan(b, a);

            return ops->ite(lt, ops->number_(32, -1), ops->ite(gt, ops->number_(32, 1), ops->number_(32, 0)));
        }

        jvmUnsupported(op);
    }

    void methodReturn(Ops /*ops*/, I /*insn*/) { jvmUnsupported("methodReturn(void)"); }
    void methodReturn(Ops /*ops*/, I /*insn*/, SValue::Ptr /*value*/) { jvmUnsupported("methodReturn(value)"); }

    void stack_dup(Ops ops) {
        SValue::Ptr v1 = ops->popOperand();
        ops->pushOperand(v1); ops->pushOperand(v1);
    }
    void stack_dup_x1(Ops ops) {
        SValue::Ptr v1 = ops->popOperand(), v2 = ops->popOperand();
        ops->pushOperand(v1); ops->pushOperand(v2); ops->pushOperand(v1);
    }
    void stack_dup_x2(Ops ops) {
        SValue::Ptr v1 = ops->popOperand(), v2 = ops->popOperand(), v3 = ops->popOperand();
        ops->pushOperand(v1); ops->pushOperand(v3); ops->pushOperand(v2); ops->pushOperand(v1);
    }
    void stack_dup2(Ops ops) {
        SValue::Ptr v1 = ops->popOperand(), v2 = ops->popOperand();
        ops->pushOperand(v2); ops->pushOperand(v1); ops->pushOperand(v2); ops->pushOperand(v1);
    }
    void stack_dup2_x1(Ops ops) {
        SValue::Ptr v1 = ops->popOperand(), v2 = ops->popOperand(), v3 = ops->popOperand();
        ops->pushOperand(v2); ops->pushOperand(v1); ops->pushOperand(v3); ops->pushOperand(v2); ops->pushOperand(v1);
    }
    void stack_dup2_x2(Ops ops) {
        SValue::Ptr v1 = ops->popOperand(), v2 = ops->popOperand(), v3 = ops->popOperand(), v4 = ops->popOperand();
        ops->pushOperand(v2); ops->pushOperand(v1); ops->pushOperand(v4); ops->pushOperand(v3); ops->pushOperand(v2); ops->pushOperand(v1);
    }
    void stack_pop(Ops ops) { ops->popOperand(); }
    void stack_pop2(Ops ops) { ops->popOperand(); ops->popOperand(); }
    void stack_swap(Ops ops) {
        SValue::Ptr v1 = ops->popOperand(), v2 = ops->popOperand();
        ops->pushOperand(v1); ops->pushOperand(v2);
    }

    void branch_goto(Ops, I, Args) { jvmUnsupported("branch_goto"); }
    void branch_goto_w(Ops, I, Args) { jvmUnsupported("branch_goto_w"); }
    void branch_if_acmpeq(Ops, I, Args) { jvmUnsupported("branch_if_acmpeq"); }
    void branch_if_acmpne(Ops, I, Args) { jvmUnsupported("branch_if_acmpne"); }
    void branch_if_icmpeq(Ops, I, Args) { jvmUnsupported("branch_if_icmpeq"); }
    void branch_if_icmpne(Ops, I, Args) { jvmUnsupported("branch_if_icmpne"); }
    void branch_if_icmplt(Ops, I, Args) { jvmUnsupported("branch_if_icmplt"); }
    void branch_if_icmpge(Ops, I, Args) { jvmUnsupported("branch_if_icmpge"); }
    void branch_if_icmpgt(Ops, I, Args) { jvmUnsupported("branch_if_icmpgt"); }
    void branch_if_icmple(Ops, I, Args) { jvmUnsupported("branch_if_icmple"); }
    void branch_ifeq(Ops, I, Args) { jvmUnsupported("branch_ifeq"); }
    void branch_ifne(Ops, I, Args) { jvmUnsupported("branch_ifne"); }
    void branch_iflt(Ops, I, Args) { jvmUnsupported("branch_iflt"); }
    void branch_ifge(Ops, I, Args) { jvmUnsupported("branch_ifge"); }
    void branch_ifgt(Ops, I, Args) { jvmUnsupported("branch_ifgt"); }
    void branch_ifle(Ops, I, Args) { jvmUnsupported("branch_ifle"); }
    void branch_ifnonnull(Ops, I, Args) { jvmUnsupported("branch_ifnonnull"); }
    void branch_ifnull(Ops, I, Args) { jvmUnsupported("branch_ifnull"); }
    void branch_jsr(Ops, I, Args) { jvmUnsupported("branch_jsr"); }
    void branch_jsr_w(Ops, I, Args) { jvmUnsupported("branch_jsr_w"); }
    void branch_lookupswitch(Ops, I, Args) { jvmUnsupported("branch_lookupswitch"); }
    void branch_tableswitch(Ops, I, Args) { jvmUnsupported("branch_tableswitch"); }

    void execute_anewarray(Ops ops, I /*insn*/, Args args) {
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

    void execute_arraylength(Ops ops, I /*insn*/, Args args) {
        ASSERT_require(args.empty());
        SValue::Ptr arrayref = ops->popOperand();
        throwIfNull(ops, "NullPointerException", arrayref);
        ops->pushOperand(arrayLength(ops, arrayref));
    }

    void execute_athrow(Ops, I, Args) { jvmUnsupported("execute_athrow"); }
    void execute_checkcast(Ops, I, Args) { jvmUnsupported("execute_checkcast"); }
    void execute_getfield(Ops, I, Args) { jvmUnsupported("execute_getfield"); }
    void execute_getstatic(Ops, I, Args) { jvmUnsupported("execute_getstatic"); }
    void execute_iconst_4(Ops ops, I, Args args) { ASSERT_require(args.empty()); ops->pushOperand(ops->number_(32, 4)); }
    void execute_iconst_5(Ops ops, I, Args args) { ASSERT_require(args.empty()); ops->pushOperand(ops->number_(32, 5)); }
    void execute_iinc(Ops, I, Args) { jvmUnsupported("execute_iinc"); }
    void execute_instanceof(Ops, I, Args) { jvmUnsupported("execute_instanceof"); }
    void execute_invokedynamic(Ops, I, Args) { jvmUnsupported("execute_invokedynamic"); }
    void execute_invokeinterface(Ops, I, Args) { jvmUnsupported("execute_invokeinterface"); }
    void execute_invokespecial(Ops, I, Args) { jvmUnsupported("execute_invokespecial"); }
    void execute_invokestatic(Ops, I, Args) { jvmUnsupported("execute_invokestatic"); }
    void execute_invokevirtual(Ops, I, Args) { jvmUnsupported("execute_invokevirtual"); }
    void execute_ldc(Ops, I, Args) { jvmUnsupported("execute_ldc"); }
    void execute_ldc_w(Ops, I, Args) { jvmUnsupported("execute_ldc_w"); }
    void execute_ldc2_w(Ops, I, Args) { jvmUnsupported("execute_ldc2_w"); }
    void execute_monitorenter(Ops, I, Args) { jvmUnsupported("execute_monitorenter"); }
    void execute_monitorexit(Ops, I, Args) { jvmUnsupported("execute_monitorexit"); }
    void execute_multianewarray(Ops, I, Args) { jvmUnsupported("execute_multianewarray"); }
    void execute_new(Ops, I, Args) { jvmUnsupported("execute_new"); }
    void execute_newarray(Ops, I, Args) { jvmUnsupported("execute_newarray"); }
    void execute_putfield(Ops, I, Args) { jvmUnsupported("execute_putfield"); }
    void execute_putstatic(Ops, I, Args) { jvmUnsupported("execute_putstatic"); }
    void execute_ret(Ops, I, Args) { jvmUnsupported("execute_ret"); }
    void execute_wide(Ops, I, Args) { jvmUnsupported("execute_wide"); }

    void throwIfNull(Ops /*ops*/, const char *exceptionName, SValue::Ptr ref) {
        if (ref && ref->get_number() == 0)
            throw std::runtime_error(exceptionName);
    }

    void throwIfArrayIndexOutOfBounds(Ops ops, SValue::Ptr arrayref, SValue::Ptr index) {
     // 1. Calculate the actual address being accessed
        SValue::Ptr accessAddr = ops->add(arrayref, index);

     // 2. Retrieve the concrete address value (if available)
        auto va = accessAddr->toUnsigned();
        if (!va) return; // Cannot check bounds for purely symbolic addresses without a solver
#if 0
     // DQ (5/19/2026): Not clear how to implement this part (discuss with Craig and Robb).
     // 3. Get the intended region from the array reference
     // In PartitionerModel, SValues can carry region information
     // const AddressInterval referencedRegion = arrayref->region();
     // if (!referencedRegion) return;

     // Alternative to using Model Checking...
        namespace IntervalSemantics = Rose::BinaryAnalysis::InstructionSemantics::IntervalSemantics;

        if (IntervalSemantics::SValue::Ptr iv = IntervalSemantics::SValue::promote(abstractVal)) {
             const Intervals &possibleValues = iv->get_intervals();
          // Check if intervals are within your known array bounds
        }

     // 4. Check if the accessed address (and size, e.g., 1 byte) is within the region
        AddressInterval accessedRegion = AddressInterval::baseSize(*va, 1);
        if (!referencedRegion.contains(accessedRegion)) {
            throw std::runtime_error("ArrayIndexOutOfBoundsException");
        }
#endif
    }

    void throwIfArrayStoreNotCompatible(Ops /*ops*/, SValue::Ptr arrayref, SValue::Ptr value) {
        auto myValue = MyDomain::SValue::promote(value);
        auto myArray = MyDomain::SValue::promote(arrayref);

        if (myValue && myArray) {
            SgType* valType = myValue->get_type();
            if (valType && !SageInterface::isAssignable(valType)) {
                throw std::runtime_error("ArrayStoreException");
            }
        }
    }

    void throwIfIntegerDivisorZero(Ops /*ops*/, SValue::Ptr divisor) {
        if (divisor && divisor->get_number() == 0)
            throw std::runtime_error("ArithmeticException");
    }
    void throwIfLongDivisorZero(Ops ops, SValue::Ptr divisor) { throwIfIntegerDivisorZero(ops, divisor); }

    void throwIfNegativeArraySize(Ops /*ops*/, SValue::Ptr count) {
        if (count) {
            auto signedCount = count->toSigned();
            if (signedCount && *signedCount < 0) {
                throw std::runtime_error("NegativeArraySizeException");
            }
        }
    }

SgType* getTargetTypeFromInsn(I insn, Args /*args*/) {
    // Cast to JVM instruction
    auto jvmInsn = isSgAsmJvmInstruction(insn);
    if (!jvmInsn) return nullptr;

    // Only checkcast and instanceof have type targets
    switch (jvmInsn->get_kind()) {
        case JvmInstructionKind::checkcast:
        case JvmInstructionKind::instanceof: {
            // Get the first operand (constant pool index)
            if (auto expr = isSgAsmIntegerValueExpression(jvmInsn->get_operandList()->get_operands()[0])) {
                uint16_t classIndex = static_cast<uint16_t>(expr->get_value());
                (void) classIndex; // defeat clang warning

                // You need access to the constant pool here
                // This typically comes from the method or class context
                // SgAsmJvmConstantPool* pool = getConstantPoolFromContext();
                // std::string className = JvmClass::name(classIndex, pool);

                // Convert class name to SgType
                // return buildSgTypeFromClassName(className);
            }
            break;
        }
        default:
            return nullptr;
    }
    return nullptr;
}

bool isSubclassOf(SgClassType* derived, SgClassType* base);

bool isCastValid(SgType* objectType, SgType* targetType) {
    // 1. Null reference can always be cast to any reference type
    if (!objectType) return true;

    // 2. Exact type match is always valid
    if (objectType->isEquivalentType(targetType)) {
        return true;
    }

    // 3. For class types, check if objectType is a subclass of targetType
    if (auto objClass = isSgClassType(objectType)) {
        if (auto targetClass = isSgClassType(targetType)) {
            return isSubclassOf(objClass, targetClass);
        }
    }

    // 4. Handle array types (arrays can be cast to Object)
    if (isSgArrayType(objectType) && isSgClassType(targetType)) {
        // Arrays can be cast to Object or Cloneable
        // This is a simplified check - you may need more specific logic
        return true;
    }

    return false;
}

bool isSubclassOf(SgClassType* derived, SgClassType* base) {
    // Get class definitions
    SgClassDeclaration* derivedDecl = isSgClassDeclaration(derived->get_declaration());
    SgClassDeclaration* baseDecl = isSgClassDeclaration(base->get_declaration());

    if (!derivedDecl || !baseDecl) return false;

    SgClassDefinition* derivedDef = derivedDecl->get_definition();
    SgClassDefinition* baseDef = baseDecl->get_definition();

    if (!derivedDef || !baseDef) return false;

    // Check if they're the same class
    if (derivedDef == baseDef) return true;

    // Check base classes
    const SgBaseClassPtrList& bases = derivedDef->get_inheritances();
    for (SgBaseClass* baseClass : bases) {
        if (!baseClass->get_isDirectBaseClass()) continue;

        SgClassDeclaration* baseClassDecl = baseClass->get_base_class();
        if (!baseClassDecl) continue;

        SgClassDeclaration* baseClassDefDecl = isSgClassDeclaration(
            baseClassDecl->get_definingDeclaration());
        if (!baseClassDefDecl) continue;

        SgClassDefinition* baseClassDef = baseClassDefDecl->get_definition();
        if (!baseClassDef) continue;

        // Recursively check if this base class matches or is a subclass
        SgClassType* baseClassType = isSgClassType(baseClassDefDecl->get_type());
        if (baseClassType && isSubclassOf(baseClassType, base)) {
            return true;
        }
    }

    return false;
}

    void throwIfClassCastFails(Ops /*ops*/, I insn, Args args, SValue::Ptr objectref) {
     // if (objectref && objectref->classCastFails())
     //    throw std::runtime_error("ClassCastException");
     // 1. Promote to your custom domain's SValue to access type information
        auto myObject = MyDomain::SValue::promote(objectref);

        if (myObject) {
            // 2. Get the object's actual type (stored in your custom SValue)
            SgType* objectType = myObject->get_type();

            // 3. Get the target type from the instruction or args
            // This depends on how you're encoding the cast target in your JVM semantics
            SgType* targetType = getTargetTypeFromInsn(insn, args);

            // 4. Check if the cast is valid using ROSE's type system
            if (objectType && targetType && !isCastValid(objectType, targetType)) {
                throw std::runtime_error("ClassCastException");
            }
        }
    }

    void throwIfMonitorOwnershipInvalid(Ops ops, SValue::Ptr objectref) {
     // 1. Check if objectref is null (null reference never owns a monitor)
        if (!objectref) {
            throw std::runtime_error("IllegalMonitorStateException");
        }

     // 2. Get the current thread's monitor ownership state
        auto myState = MyDomain::State::promote(ops->currentState());

     // 3. Check if current thread owns this object's monitor
     // Promote objectref to MyDomain::SValue before passing to ownsMonitor
        auto myObjectRef = MyDomain::SValue::promote(objectref);
        if (!myState->ownsMonitor(myObjectRef)) {
            throw std::runtime_error("IllegalMonitorStateException");
        }
    }

    SValue::Ptr
    arrayLength(Ops /*ops*/, SValue::Ptr arrayRef) {
        ASSERT_require(arrayRef->kind() == JvmValueKind::ArrayReference);
        ASSERT_require(arrayRef->hasArrayLength());

        // Get the array length from the array and convert it to Integer kind
        SValuePtr length = arrayRef->arrayLength();
        length->kind(JvmValueKind::Integer);
        return length;
    }
}


// aaload (50 (0x32))
        // Description:
        //   Pop arrayref and index; load the reference component at index and push the value.
        // Run-time Exceptions:
        //   NullPointerException if arrayref is null.
        //   ArrayIndexOutOfBoundsException if index is outside the array bounds.
struct IP_aaload: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        SValue::Ptr index = ops->popOperand();
        SValue::Ptr arrayref = ops->popOperand();
        JvmSemantics::throwIfNull(ops, "NullPointerException", arrayref);

        JvmSemantics::throwIfArrayIndexOutOfBounds(ops, arrayref, index);

        ops->pushOperand(JvmSemantics::arrayLoad(ops, "a", arrayref, index));
    }
};


// aastore (83 (0x53))
        // Description:
        //   Pop value, index, and arrayref; store the reference value into the array component.
        // Run-time Exceptions:
        //   NullPointerException if arrayref is null.
        //   ArrayIndexOutOfBoundsException if index is outside the array bounds.
        //   ArrayStoreException if a non-null value is not assignment-compatible with the array component type.
struct IP_aastore: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        SValue::Ptr value = ops->popOperand();
        SValue::Ptr index = ops->popOperand();
        SValue::Ptr arrayref = ops->popOperand();
        JvmSemantics::throwIfNull(ops, "NullPointerException", arrayref);

        JvmSemantics::throwIfArrayIndexOutOfBounds(ops, arrayref, index);

        JvmSemantics::throwIfArrayStoreNotCompatible(ops, arrayref, value);

        JvmSemantics::arrayStore(ops, "a", arrayref, index, value);
    }
};

// aconst_null (1 (0x01))
        // Description:
        //   Push the null object reference.
        // Notes:
        //   The JVM does not prescribe a concrete bit pattern for null.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_aconst_null: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        ops->pushOperand(JvmSemantics::nullReference(ops));
    }
};


// aload (25 (0x19))
        // Description:
        //   Load a reference value from a local variable and push it.
        // Notes:
        //   The _<n> form uses an implicit local-variable index.
        //   aload cannot load returnAddress values; astore can store them.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_aload: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 1);
        const uint32_t index = asU1(args[0]);
        ops->pushOperand(ops->readLocal(index));
    }
};

// aload_0 (42 (0x2a))
        // Description:
        //   Load a reference value from a local variable and push it.
        // Notes:
        //   The _<n> form uses an implicit local-variable index.
        //   aload cannot load returnAddress values; astore can store them.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_aload_0: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        ops->pushOperand(ops->readLocal(0));
    }
};

// aload_1 (43 (0x2b))
        // Description:
        //   Load a reference value from a local variable and push it.
        // Notes:
        //   The _<n> form uses an implicit local-variable index.
        //   aload cannot load returnAddress values; astore can store them.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_aload_1: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        ops->pushOperand(ops->readLocal(1));
    }
};

// aload_2 (44 (0x2c))
        // Description:
        //   Load a reference value from a local variable and push it.
        // Notes:
        //   The _<n> form uses an implicit local-variable index.
        //   aload cannot load returnAddress values; astore can store them.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_aload_2: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        ops->pushOperand(ops->readLocal(2));
    }
};

// aload_3 (45 (0x2d))
        // Description:
        //   Load a reference value from a local variable and push it.
        // Notes:
        //   The _<n> form uses an implicit local-variable index.
        //   aload cannot load returnAddress values; astore can store them.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_aload_3: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        ops->pushOperand(ops->readLocal(3));
    }
};


// anewarray (189 (0xbd))
        // Description:
        //   Create a new array, using count values popped from the operand stack, and push the resulting array reference.
        // Run-time Exceptions:
        //   NegativeArraySizeException if any requested dimension is negative.
struct IP_anewarray: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 2);
        JvmSemantics::execute_anewarray(ops, insn, args);
    }
};

// areturn (176 (0xb0))
        // Description:
        //   Return from the current method; typed returns move the return value to the invoker frame, while return returns void.
        // Run-time Exceptions:
        //   IllegalMonitorStateException can be thrown for synchronized methods if structured locking ownership rules are violated.
struct IP_areturn: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        SValue::Ptr value = ops->popOperand();
        JvmSemantics::methodReturn(ops, insn, value);
    }
};

// arraylength (190 (0xbe))
        // Description:
        //   Pop arrayref and push its length as an int.
        // Run-time Exceptions:
        //   NullPointerException if arrayref is null.
struct IP_arraylength: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        SValue::Ptr arrayref = ops->popOperand();
        JvmSemantics::throwIfNull(ops, "NullPointerException", arrayref);
        ops->pushOperand(JvmSemantics::arrayLength(ops, arrayref));
    }
};

// astore (58 (0x3a))
        // Description:
        //   Pop a reference or returnAddress value and store it into a local variable.
        // Notes:
        //   The _<n> form uses an implicit local-variable index.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_astore: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 1);
        const uint32_t index = JvmSemantics::u1(args[0]);
        ops->writeLocal(index, ops->popOperand());
    }
};

// astore_0 (75 (0x4b))
        // Description:
        //   Pop a reference or returnAddress value and store it into a local variable.
        // Notes:
        //   The _<n> form uses an implicit local-variable index.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_astore_0: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        ops->writeLocal(0, ops->popOperand());
    }
};

// astore_1 (76 (0x4c))
        // Description:
        //   Pop a reference or returnAddress value and store it into a local variable.
        // Notes:
        //   The _<n> form uses an implicit local-variable index.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_astore_1: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        ops->writeLocal(1, ops->popOperand());
    }
};

// astore_2 (77 (0x4d))
        // Description:
        //   Pop a reference or returnAddress value and store it into a local variable.
        // Notes:
        //   The _<n> form uses an implicit local-variable index.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_astore_2: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        ops->writeLocal(2, ops->popOperand());
    }
};

// astore_3 (78 (0x4e))
        // Description:
        //   Pop a reference or returnAddress value and store it into a local variable.
        // Notes:
        //   The _<n> form uses an implicit local-variable index.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_astore_3: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        ops->writeLocal(3, ops->popOperand());
    }
};

// athrow (191 (0xbf))
        // Description:
        //   Throw the Throwable object reference on top of the operand stack.
        // Notes:
        //   The current stack is cleared and objectref remains as the sole operand while exception handling searches.
        // Run-time Exceptions:
        //   NullPointerException if objectref is null; otherwise the referenced Throwable is thrown.
struct IP_athrow: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        JvmSemantics::execute_athrow(ops, insn, args);
    }
};

// baload (51 (0x33))
        // Description:
        //   Pop arrayref and index; load the byte/boolean component at index and push the value.
        // Run-time Exceptions:
        //   NullPointerException if arrayref is null.
        //   ArrayIndexOutOfBoundsException if index is outside the array bounds.
struct IP_baload: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        SValue::Ptr index = ops->popOperand();
        SValue::Ptr arrayref = ops->popOperand();
        JvmSemantics::throwIfNull(ops, "NullPointerException", arrayref);

        JvmSemantics::throwIfArrayIndexOutOfBounds(ops, arrayref, index);

        ops->pushOperand(JvmSemantics::arrayLoad(ops, "b", arrayref, index));
    }
};

// bastore (84 (0x54))
        // Description:
        //   Pop value, index, and arrayref; store the byte/boolean value into the array component.
        // Run-time Exceptions:
        //   NullPointerException if arrayref is null.
        //   ArrayIndexOutOfBoundsException if index is outside the array bounds.
struct IP_bastore: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        SValue::Ptr value = ops->popOperand();
        SValue::Ptr index = ops->popOperand();
        SValue::Ptr arrayref = ops->popOperand();
        JvmSemantics::throwIfNull(ops, "NullPointerException", arrayref);

        JvmSemantics::throwIfArrayIndexOutOfBounds(ops, arrayref, index);

        JvmSemantics::arrayStore(ops, "b", arrayref, index, value);
    }
};

// bipush (16 (0x10))
        // Description:
        //   Sign-extend the immediate operand to int and push it.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_bipush: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 1);
        ops->pushOperand(makeConstant(ops, "b", asU1(args[0]), 32));
    }
};

// caload (52 (0x34))
        // Description:
        //   Pop arrayref and index; load the char component at index and push the value.
        // Run-time Exceptions:
        //   NullPointerException if arrayref is null.
        //   ArrayIndexOutOfBoundsException if index is outside the array bounds.
struct IP_caload: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        SValue::Ptr index = ops->popOperand();
        SValue::Ptr arrayref = ops->popOperand();
        JvmSemantics::throwIfNull(ops, "NullPointerException", arrayref);

        JvmSemantics::throwIfArrayIndexOutOfBounds(ops, arrayref, index);

        ops->pushOperand(JvmSemantics::arrayLoad(ops, "c", arrayref, index));
    }
};

// castore (85 (0x55))
        // Description:
        //   Pop value, index, and arrayref; store the char value into the array component.
        // Run-time Exceptions:
        //   NullPointerException if arrayref is null.
        //   ArrayIndexOutOfBoundsException if index is outside the array bounds.
struct IP_castore: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        SValue::Ptr value = ops->popOperand();
        SValue::Ptr index = ops->popOperand();
        SValue::Ptr arrayref = ops->popOperand();
        JvmSemantics::throwIfNull(ops, "NullPointerException", arrayref);

        JvmSemantics::throwIfArrayIndexOutOfBounds(ops, arrayref, index);

        JvmSemantics::arrayStore(ops, "c", arrayref, index, value);
    }
};

// checkcast (192 (0xc0))
        // Description:
        //   Check that objectref is null or assignment-compatible with the resolved type; push the unchanged reference if the check succeeds.
        // Notes:
        //   A null objectref always succeeds.
        // Run-time Exceptions:
        //   ClassCastException if objectref is non-null and not assignment-compatible with the resolved type.
struct IP_checkcast: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 2);
        JvmSemantics::execute_checkcast(ops, insn, args);
    }
};

// d2f (144 (0x90))
        // Description:
        //   Pop a numeric value, convert it to the target primitive type, and push the converted value.
        // Notes:
        //   Conversions follow JVM numeric conversion rules including rounding, NaN handling, and narrowing behavior.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_d2f: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        SValue::Ptr v = ops->popOperand();
        ops->pushOperand(JvmSemantics::convert(ops, "d2f", v));
    }
};

// d2i (142 (0x8e))
        // Description:
        //   Pop a numeric value, convert it to the target primitive type, and push the converted value.
        // Notes:
        //   Conversions follow JVM numeric conversion rules including rounding, NaN handling, and narrowing behavior.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_d2i: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        SValue::Ptr v = ops->popOperand();
        ops->pushOperand(JvmSemantics::convert(ops, "d2i", v));
    }
};

// d2l (143 (0x8f))
        // Description:
        //   Pop a numeric value, convert it to the target primitive type, and push the converted value.
        // Notes:
        //   Conversions follow JVM numeric conversion rules including rounding, NaN handling, and narrowing behavior.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_d2l: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        SValue::Ptr v = ops->popOperand();
        ops->pushOperand(JvmSemantics::convert(ops, "d2l", v));
    }
};

// dadd (99 (0x63))
        // Description:
        //   Pop operands, apply the arithmetic/bitwise operation, and push the result.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_dadd: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        SValue::Ptr v2 = ops->popOperand();
        SValue::Ptr v1 = ops->popOperand();
        ops->pushOperand(ops->add(v1, v2));
    }
};

// daload (49 (0x31))
        // Description:
        //   Pop arrayref and index; load the double component at index and push the value.
        // Run-time Exceptions:
        //   NullPointerException if arrayref is null.
        //   ArrayIndexOutOfBoundsException if index is outside the array bounds.
struct IP_daload: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        SValue::Ptr index = ops->popOperand();
        SValue::Ptr arrayref = ops->popOperand();
        JvmSemantics::throwIfNull(ops, "NullPointerException", arrayref);

        JvmSemantics::throwIfArrayIndexOutOfBounds(ops, arrayref, index);

        ops->pushOperand(JvmSemantics::arrayLoad(ops, "d", arrayref, index));
    }
};

// dastore (82 (0x52))
        // Description:
        //   Pop value, index, and arrayref; store the double value into the array component.
        // Run-time Exceptions:
        //   NullPointerException if arrayref is null.
        //   ArrayIndexOutOfBoundsException if index is outside the array bounds.
struct IP_dastore: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        SValue::Ptr value = ops->popOperand();
        SValue::Ptr index = ops->popOperand();
        SValue::Ptr arrayref = ops->popOperand();
        JvmSemantics::throwIfNull(ops, "NullPointerException", arrayref);

        JvmSemantics::throwIfArrayIndexOutOfBounds(ops, arrayref, index);

        JvmSemantics::arrayStore(ops, "d", arrayref, index, value);
    }
};

// dcmpl (151 (0x97))
        // Description:
        //   Compare two numeric values and push int -1, 0, or 1; cmpg/cmpl choose the NaN result direction.
        // Notes:
        //   For cmpg NaN yields 1; for cmpl NaN yields -1.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_dcmpl: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        SValue::Ptr v2 = ops->popOperand();
        SValue::Ptr v1 = ops->popOperand();
        ops->pushOperand(JvmSemantics::compare(ops, "dcmpl", v1, v2));
    }
};

// dcmpg (152 (0x98))
        // Description:
        //   Compare two numeric values and push int -1, 0, or 1; cmpg/cmpl choose the NaN result direction.
        // Notes:
        //   For cmpg NaN yields 1; for cmpl NaN yields -1.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_dcmpg: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        SValue::Ptr v2 = ops->popOperand();
        SValue::Ptr v1 = ops->popOperand();
        ops->pushOperand(JvmSemantics::compare(ops, "dcmpg", v1, v2));
    }
};

// dconst_0 (14 (0x0e))
        // Description:
        //   Push the named numeric constant.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_dconst_0: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        ops->pushOperand(makeConstant(ops, "d", 0, 64));
    }
};

// dconst_1 (15 (0x0f))
        // Description:
        //   Push the named numeric constant.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_dconst_1: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        ops->pushOperand(makeConstant(ops, "d", 1, 64));
    }
};

// ddiv (111 (0x6f))
        // Description:
        //   Pop operands, apply the arithmetic/bitwise operation, and push the result.
        // Notes:
        //   Floating-point division/remainder follows IEEE 754 behavior and does not throw on zero divisors.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_ddiv: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        SValue::Ptr v2 = ops->popOperand();
        SValue::Ptr v1 = ops->popOperand();
        ops->pushOperand(JvmSemantics::div(ops, v1, v2));
    }
};

// dload (24 (0x18))
        // Description:
        //   Load a double value from a local variable and push it.
        // Notes:
        //   The _<n> form uses an implicit local-variable index.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_dload: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 1);
        const uint32_t index = JvmSemantics::u1(args[0]);
        ops->pushOperand(ops->readLocal(index));
    }
};

// dload_0 (38 (0x26))
        // Description:
        //   Load a double value from a local variable and push it.
        // Notes:
        //   The _<n> form uses an implicit local-variable index.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_dload_0: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        ops->pushOperand(ops->readLocal(0));
    }
};

// dload_1 (39 (0x27))
        // Description:
        //   Load a double value from a local variable and push it.
        // Notes:
        //   The _<n> form uses an implicit local-variable index.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_dload_1: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        ops->pushOperand(ops->readLocal(1));
    }
};

// dload_2 (40 (0x28))
        // Description:
        //   Load a double value from a local variable and push it.
        // Notes:
        //   The _<n> form uses an implicit local-variable index.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_dload_2: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        ops->pushOperand(ops->readLocal(2));
    }
};

// dload_3 (41 (0x29))
        // Description:
        //   Load a double value from a local variable and push it.
        // Notes:
        //   The _<n> form uses an implicit local-variable index.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_dload_3: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        ops->pushOperand(ops->readLocal(3));
    }
};

// dmul (107 (0x6b))
        // Description:
        //   Pop operands, apply the arithmetic/bitwise operation, and push the result.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_dmul: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        SValue::Ptr v2 = ops->popOperand();
        SValue::Ptr v1 = ops->popOperand();
        ops->pushOperand(JvmSemantics::mul(ops, v1, v2));
    }
};

// dneg (119 (0x77))
        // Description:
        //   Pop a numeric value, negate it, and push the result.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_dneg: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        SValue::Ptr v = ops->popOperand();
        ops->pushOperand(JvmSemantics::neg(ops, v));
    }
};

// drem (115 (0x73))
        // Description:
        //   Pop operands, apply the arithmetic/bitwise operation, and push the result.
        // Notes:
        //   Floating-point division/remainder follows IEEE 754 behavior and does not throw on zero divisors.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_drem: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        SValue::Ptr v2 = ops->popOperand();
        SValue::Ptr v1 = ops->popOperand();
        ops->pushOperand(JvmSemantics::rem(ops, v1, v2));
    }
};

// dreturn (175 (0xaf))
        // Description:
        //   Return from the current method; typed returns move the return value to the invoker frame, while return returns void.
        // Run-time Exceptions:
        //   IllegalMonitorStateException can be thrown for synchronized methods if structured locking ownership rules are violated.
struct IP_dreturn: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        SValue::Ptr value = ops->popOperand();
        JvmSemantics::methodReturn(ops, insn, value);
    }
};

// dstore (57 (0x39))
        // Description:
        //   Pop a double value and store it into a local variable.
        // Notes:
        //   The _<n> form uses an implicit local-variable index.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_dstore: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 1);
        const uint32_t index = JvmSemantics::u1(args[0]);
        ops->writeLocal(index, ops->popOperand());
    }
};

// dstore_0 (71 (0x47))
        // Description:
        //   Pop a double value and store it into a local variable.
        // Notes:
        //   The _<n> form uses an implicit local-variable index.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_dstore_0: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        ops->writeLocal(0, ops->popOperand());
    }
};

// dstore_1 (72 (0x48))
        // Description:
        //   Pop a double value and store it into a local variable.
        // Notes:
        //   The _<n> form uses an implicit local-variable index.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_dstore_1: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        ops->writeLocal(1, ops->popOperand());
    }
};

// dstore_2 (73 (0x49))
        // Description:
        //   Pop a double value and store it into a local variable.
        // Notes:
        //   The _<n> form uses an implicit local-variable index.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_dstore_2: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        ops->writeLocal(2, ops->popOperand());
    }
};

// dstore_3 (74 (0x4a))
        // Description:
        //   Pop a double value and store it into a local variable.
        // Notes:
        //   The _<n> form uses an implicit local-variable index.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_dstore_3: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        ops->writeLocal(3, ops->popOperand());
    }
};

// dsub (103 (0x67))
        // Description:
        //   Pop operands, apply the arithmetic/bitwise operation, and push the result.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_dsub: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        SValue::Ptr v2 = ops->popOperand();
        SValue::Ptr v1 = ops->popOperand();
        ops->pushOperand(JvmSemantics::sub(ops, v1, v2));
    }
};

// dup (89 (0x59))
        // Description:
        //   Rearrange or discard operand stack values according to the JVM category-1/category-2 stack-form rules.
        // Notes:
        //   These instructions must not be used to split a category-2 value.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_dup: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        JvmSemantics::stack_dup(ops);
    }
};

// dup_x1 (90 (0x5a))
        // Description:
        //   Rearrange or discard operand stack values according to the JVM category-1/category-2 stack-form rules.
        // Notes:
        //   These instructions must not be used to split a category-2 value.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_dup_x1: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        JvmSemantics::stack_dup_x1(ops);
    }
};

// dup_x2 (91 (0x5b))
        // Description:
        //   Rearrange or discard operand stack values according to the JVM category-1/category-2 stack-form rules.
        // Notes:
        //   These instructions must not be used to split a category-2 value.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_dup_x2: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        JvmSemantics::stack_dup_x2(ops);
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
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        JvmSemantics::stack_dup2(ops);
    }
};

// dup2_x1 (93 (0x5d))
        // Description:
        //   Rearrange or discard operand stack values according to the JVM category-1/category-2 stack-form rules.
        // Notes:
        //   These instructions must not be used to split a category-2 value.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_dup2_x1: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        JvmSemantics::stack_dup2_x1(ops);
    }
};

// dup2_x2 (94 (0x5e))
        // Description:
        //   Rearrange or discard operand stack values according to the JVM category-1/category-2 stack-form rules.
        // Notes:
        //   These instructions must not be used to split a category-2 value.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_dup2_x2: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        JvmSemantics::stack_dup2_x2(ops);
    }
};

// f2d (141 (0x8d))
        // Description:
        //   Pop a numeric value, convert it to the target primitive type, and push the converted value.
        // Notes:
        //   Conversions follow JVM numeric conversion rules including rounding, NaN handling, and narrowing behavior.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_f2d: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        SValue::Ptr v = ops->popOperand();
        ops->pushOperand(JvmSemantics::convert(ops, "f2d", v));
    }
};

// f2i (139 (0x8b))
        // Description:
        //   Pop a numeric value, convert it to the target primitive type, and push the converted value.
        // Notes:
        //   Conversions follow JVM numeric conversion rules including rounding, NaN handling, and narrowing behavior.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_f2i: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        SValue::Ptr v = ops->popOperand();
        ops->pushOperand(JvmSemantics::convert(ops, "f2i", v));
    }
};

// f2l (140 (0x8c))
        // Description:
        //   Pop a numeric value, convert it to the target primitive type, and push the converted value.
        // Notes:
        //   Conversions follow JVM numeric conversion rules including rounding, NaN handling, and narrowing behavior.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_f2l: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        SValue::Ptr v = ops->popOperand();
        ops->pushOperand(JvmSemantics::convert(ops, "f2l", v));
    }
};

// fadd (98 (0x62))
        // Description:
        //   Pop operands, apply the arithmetic/bitwise operation, and push the result.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_fadd: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        SValue::Ptr v2 = ops->popOperand();
        SValue::Ptr v1 = ops->popOperand();
        ops->pushOperand(ops->add(v1, v2));
    }
};

// faload (48 (0x30))
        // Description:
        //   Pop arrayref and index; load the float component at index and push the value.
        // Run-time Exceptions:
        //   NullPointerException if arrayref is null.
        //   ArrayIndexOutOfBoundsException if index is outside the array bounds.
struct IP_faload: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        SValue::Ptr index = ops->popOperand();
        SValue::Ptr arrayref = ops->popOperand();
        JvmSemantics::throwIfNull(ops, "NullPointerException", arrayref);

        JvmSemantics::throwIfArrayIndexOutOfBounds(ops, arrayref, index);

        ops->pushOperand(JvmSemantics::arrayLoad(ops, "f", arrayref, index));
    }
};

// fastore (81 (0x51))
        // Description:
        //   Pop value, index, and arrayref; store the float value into the array component.
        // Run-time Exceptions:
        //   NullPointerException if arrayref is null.
        //   ArrayIndexOutOfBoundsException if index is outside the array bounds.
struct IP_fastore: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        SValue::Ptr value = ops->popOperand();
        SValue::Ptr index = ops->popOperand();
        SValue::Ptr arrayref = ops->popOperand();
        JvmSemantics::throwIfNull(ops, "NullPointerException", arrayref);

        JvmSemantics::throwIfArrayIndexOutOfBounds(ops, arrayref, index);

        JvmSemantics::arrayStore(ops, "f", arrayref, index, value);
    }
};

// fcmpl (149 (0x95))
        // Description:
        //   Compare two numeric values and push int -1, 0, or 1; cmpg/cmpl choose the NaN result direction.
        // Notes:
        //   For cmpg NaN yields 1; for cmpl NaN yields -1.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_fcmpl: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        SValue::Ptr v2 = ops->popOperand();
        SValue::Ptr v1 = ops->popOperand();
        ops->pushOperand(JvmSemantics::compare(ops, "fcmpl", v1, v2));
    }
};

// fcmpg (150 (0x96))
        // Description:
        //   Compare two numeric values and push int -1, 0, or 1; cmpg/cmpl choose the NaN result direction.
        // Notes:
        //   For cmpg NaN yields 1; for cmpl NaN yields -1.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_fcmpg: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        SValue::Ptr v2 = ops->popOperand();
        SValue::Ptr v1 = ops->popOperand();
        ops->pushOperand(JvmSemantics::compare(ops, "fcmpg", v1, v2));
    }
};

// fconst_0 (11 (0x0b))
        // Description:
        //   Push the named numeric constant.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_fconst_0: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        ops->pushOperand(makeConstant(ops, "f", 0, 32));
    }
};

// fconst_1 (12 (0x0c))
        // Description:
        //   Push the named numeric constant.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_fconst_1: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        ops->pushOperand(makeConstant(ops, "f", 1, 32));
    }
};

// fconst_2 (13 (0x0d))
        // Description:
        //   Push the named numeric constant.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_fconst_2: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        ops->pushOperand(makeConstant(ops, "f", 2, 32));
    }
};

// fdiv (110 (0x6e))
        // Description:
        //   Pop operands, apply the arithmetic/bitwise operation, and push the result.
        // Notes:
        //   Floating-point division/remainder follows IEEE 754 behavior and does not throw on zero divisors.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_fdiv: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        SValue::Ptr v2 = ops->popOperand();
        SValue::Ptr v1 = ops->popOperand();
        ops->pushOperand(JvmSemantics::div(ops, v1, v2));
    }
};

// fload (23 (0x17))
        // Description:
        //   Load a float value from a local variable and push it.
        // Notes:
        //   The _<n> form uses an implicit local-variable index.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_fload: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 1);
        const uint32_t index = JvmSemantics::u1(args[0]);
        ops->pushOperand(ops->readLocal(index));
    }
};

// fload_0 (34 (0x22))
        // Description:
        //   Load a float value from a local variable and push it.
        // Notes:
        //   The _<n> form uses an implicit local-variable index.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_fload_0: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        ops->pushOperand(ops->readLocal(0));
    }
};

// fload_1 (35 (0x23))
        // Description:
        //   Load a float value from a local variable and push it.
        // Notes:
        //   The _<n> form uses an implicit local-variable index.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_fload_1: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        ops->pushOperand(ops->readLocal(1));
    }
};

// fload_2 (36 (0x24))
        // Description:
        //   Load a float value from a local variable and push it.
        // Notes:
        //   The _<n> form uses an implicit local-variable index.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_fload_2: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        ops->pushOperand(ops->readLocal(2));
    }
};

// fload_3 (37 (0x25))
        // Description:
        //   Load a float value from a local variable and push it.
        // Notes:
        //   The _<n> form uses an implicit local-variable index.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_fload_3: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        ops->pushOperand(ops->readLocal(3));
    }
};

// fmul (106 (0x6a))
        // Description:
        //   Pop operands, apply the arithmetic/bitwise operation, and push the result.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_fmul: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        SValue::Ptr v2 = ops->popOperand();
        SValue::Ptr v1 = ops->popOperand();
        ops->pushOperand(JvmSemantics::mul(ops, v1, v2));
    }
};

// fneg (118 (0x76))
        // Description:
        //   Pop a numeric value, negate it, and push the result.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_fneg: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        SValue::Ptr v = ops->popOperand();
        ops->pushOperand(JvmSemantics::neg(ops, v));
    }
};

// frem (114 (0x72))
        // Description:
        //   Pop operands, apply the arithmetic/bitwise operation, and push the result.
        // Notes:
        //   Floating-point division/remainder follows IEEE 754 behavior and does not throw on zero divisors.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_frem: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        SValue::Ptr v2 = ops->popOperand();
        SValue::Ptr v1 = ops->popOperand();
        ops->pushOperand(JvmSemantics::rem(ops, v1, v2));
    }
};

// freturn (174 (0xae))
        // Description:
        //   Return from the current method; typed returns move the return value to the invoker frame, while return returns void.
        // Run-time Exceptions:
        //   IllegalMonitorStateException can be thrown for synchronized methods if structured locking ownership rules are violated.
struct IP_freturn: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        SValue::Ptr value = ops->popOperand();
        JvmSemantics::methodReturn(ops, insn, value);
    }
};

// fstore (56 (0x38))
        // Description:
        //   Pop a float value and store it into a local variable.
        // Notes:
        //   The _<n> form uses an implicit local-variable index.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_fstore: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 1);
        const uint32_t index = JvmSemantics::u1(args[0]);
        ops->writeLocal(index, ops->popOperand());
    }
};

// fstore_0 (67 (0x43))
        // Description:
        //   Pop a float value and store it into a local variable.
        // Notes:
        //   The _<n> form uses an implicit local-variable index.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_fstore_0: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        ops->writeLocal(0, ops->popOperand());
    }
};

// fstore_1 (68 (0x44))
        // Description:
        //   Pop a float value and store it into a local variable.
        // Notes:
        //   The _<n> form uses an implicit local-variable index.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_fstore_1: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        ops->writeLocal(1, ops->popOperand());
    }
};

// fstore_2 (69 (0x45))
        // Description:
        //   Pop a float value and store it into a local variable.
        // Notes:
        //   The _<n> form uses an implicit local-variable index.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_fstore_2: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        ops->writeLocal(2, ops->popOperand());
    }
};

// fstore_3 (70 (0x46))
        // Description:
        //   Pop a float value and store it into a local variable.
        // Notes:
        //   The _<n> form uses an implicit local-variable index.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_fstore_3: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        ops->writeLocal(3, ops->popOperand());
    }
};

// fsub (102 (0x66))
        // Description:
        //   Pop operands, apply the arithmetic/bitwise operation, and push the result.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_fsub: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        SValue::Ptr v2 = ops->popOperand();
        SValue::Ptr v1 = ops->popOperand();
        ops->pushOperand(JvmSemantics::sub(ops, v1, v2));
    }
};

// getfield (180 (0xb4))
        // Description:
        //   Resolve the referenced field and read or write the instance/static field value.
        // Run-time Exceptions:
        //   NullPointerException if objectref is null.
struct IP_getfield: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 2);
        JvmSemantics::execute_getfield(ops, insn, args);
    }
};

// getstatic (178 (0xb2))
        // Description:
        //   Resolve the referenced field and read or write the instance/static field value.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_getstatic: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 2);
        JvmSemantics::execute_getstatic(ops, insn, args);
    }
};

// goto (167 (0xa7))
        // Description:
        //   Branch unconditionally by the signed offset.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_goto: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 2);
        JvmSemantics::branch_goto(ops, insn, args);
    }
};

// goto_w (200 (0xc8))
        // Description:
        //   Branch unconditionally by the signed offset.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_goto_w: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 4);
        JvmSemantics::branch_goto_w(ops, insn, args);
    }
};

// i2b (145 (0x91))
        // Description:
        //   Pop a numeric value, convert it to the target primitive type, and push the converted value.
        // Notes:
        //   Conversions follow JVM numeric conversion rules including rounding, NaN handling, and narrowing behavior.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_i2b: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        SValue::Ptr v = ops->popOperand();
        ops->pushOperand(JvmSemantics::convert(ops, "i2b", v));
    }
};

// i2c (146 (0x92))
        // Description:
        //   Pop a numeric value, convert it to the target primitive type, and push the converted value.
        // Notes:
        //   Conversions follow JVM numeric conversion rules including rounding, NaN handling, and narrowing behavior.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_i2c: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        SValue::Ptr v = ops->popOperand();
        ops->pushOperand(JvmSemantics::convert(ops, "i2c", v));
    }
};

// i2d (135 (0x87))
        // Description:
        //   Pop a numeric value, convert it to the target primitive type, and push the converted value.
        // Notes:
        //   Conversions follow JVM numeric conversion rules including rounding, NaN handling, and narrowing behavior.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_i2d: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        SValue::Ptr v = ops->popOperand();
        ops->pushOperand(JvmSemantics::convert(ops, "i2d", v));
    }
};

// i2f (134 (0x86))
        // Description:
        //   Pop a numeric value, convert it to the target primitive type, and push the converted value.
        // Notes:
        //   Conversions follow JVM numeric conversion rules including rounding, NaN handling, and narrowing behavior.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_i2f: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        SValue::Ptr v = ops->popOperand();
        ops->pushOperand(JvmSemantics::convert(ops, "i2f", v));
    }
};

// i2l (133 (0x85))
        // Description:
        //   Pop a numeric value, convert it to the target primitive type, and push the converted value.
        // Notes:
        //   Conversions follow JVM numeric conversion rules including rounding, NaN handling, and narrowing behavior.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_i2l: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        SValue::Ptr v = ops->popOperand();
        ops->pushOperand(JvmSemantics::convert(ops, "i2l", v));
    }
};

// i2s (147 (0x93))
        // Description:
        //   Pop a numeric value, convert it to the target primitive type, and push the converted value.
        // Notes:
        //   Conversions follow JVM numeric conversion rules including rounding, NaN handling, and narrowing behavior.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_i2s: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        SValue::Ptr v = ops->popOperand();
        ops->pushOperand(JvmSemantics::convert(ops, "i2s", v));
    }
};

// iadd (96 (0x60))
        // Description:
        //   Pop operands, apply the arithmetic/bitwise operation, and push the result.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_iadd: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        SValue::Ptr v2 = ops->popOperand();
        SValue::Ptr v1 = ops->popOperand();
        ops->pushOperand(ops->add(v1, v2));
    }
};

// iaload (46 (0x2e))
        // Description:
        //   Pop arrayref and index; load the int component at index and push the value.
        // Run-time Exceptions:
        //   NullPointerException if arrayref is null.
        //   ArrayIndexOutOfBoundsException if index is outside the array bounds.
struct IP_iaload: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        SValue::Ptr index = ops->popOperand();
        SValue::Ptr arrayref = ops->popOperand();
        JvmSemantics::throwIfNull(ops, "NullPointerException", arrayref);

        JvmSemantics::throwIfArrayIndexOutOfBounds(ops, arrayref, index);

        ops->pushOperand(JvmSemantics::arrayLoad(ops, "i", arrayref, index));
    }
};

// iand (126 (0x7e))
        // Description:
        //   Pop operands, apply the arithmetic/bitwise operation, and push the result.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_iand: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        SValue::Ptr v2 = ops->popOperand();
        SValue::Ptr v1 = ops->popOperand();
        ops->pushOperand(JvmSemantics::binary(ops, "iand", v1, v2));
    }
};

// iastore (79 (0x4f))
        // Description:
        //   Pop value, index, and arrayref; store the int value into the array component.
        // Run-time Exceptions:
        //   NullPointerException if arrayref is null.
        //   ArrayIndexOutOfBoundsException if index is outside the array bounds.
struct IP_iastore: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        SValue::Ptr value = ops->popOperand();
        SValue::Ptr index = ops->popOperand();
        SValue::Ptr arrayref = ops->popOperand();
        JvmSemantics::throwIfNull(ops, "NullPointerException", arrayref);

        JvmSemantics::throwIfArrayIndexOutOfBounds(ops, arrayref, index);

        JvmSemantics::arrayStore(ops, "i", arrayref, index, value);
    }
};

// iconst_m1 (2 (0x02))
        // Description:
        //   Load the int value -1 onto the stack.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_iconst_m1: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        ASSERT_require2(false, "Can't make a negative integral constant");
        ops->pushOperand(makeConstant(ops, "i", -1, 32));
    }
};

// iconst_0 (3 (0x03))
        // Description:
        //   Load the int value 0 onto the stack.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_iconst_0: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
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
    void p(D /*d*/, Ops ops, I insn, Args args) {
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
    void p(D /*d*/, Ops ops, I insn, Args args) {
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
    void p(D /*d*/, Ops ops, I insn, Args args) {
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
    void p(D /*d*/, Ops ops, I insn, Args args) {
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
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        ops->pushOperand(makeConstant(ops, "i", 5, 32));
    }
};

// idiv (108 (0x6c))
        // Description:
        //   Pop operands, apply the arithmetic/bitwise operation, and push the result.
        // Run-time Exceptions:
        //   ArithmeticException if the divisor is zero.
struct IP_idiv: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        SValue::Ptr v2 = ops->popOperand();
        SValue::Ptr v1 = ops->popOperand();
        JvmSemantics::throwIfIntegerDivisorZero(ops, v2);
        ops->pushOperand(JvmSemantics::div(ops, v1, v2));
    }
};

// if_acmpeq (165 (0xa5))
        // Description:
        //   Pop comparison operand(s); branch to the signed offset if the condition is true, otherwise continue with the next instruction.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_if_acmpeq: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 2);
        JvmSemantics::branch_if_acmpeq(ops, insn, args);
    }
};

// if_acmpne (166 (0xa6))
        // Description:
        //   Pop comparison operand(s); branch to the signed offset if the condition is true, otherwise continue with the next instruction.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_if_acmpne: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 2);
        JvmSemantics::branch_if_acmpne(ops, insn, args);
    }
};

// if_icmpeq (159 (0x9f))
        // Description:
        //   Pop comparison operand(s); branch to the signed offset if the condition is true, otherwise continue with the next instruction.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_if_icmpeq: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 2);
        JvmSemantics::branch_if_icmpeq(ops, insn, args);
    }
};

// if_icmpne (160 (0xa0))
        // Description:
        //   Pop comparison operand(s); branch to the signed offset if the condition is true, otherwise continue with the next instruction.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_if_icmpne: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 2);
        JvmSemantics::branch_if_icmpne(ops, insn, args);
    }
};

// if_icmplt (161 (0xa1))
        // Description:
        //   Pop comparison operand(s); branch to the signed offset if the condition is true, otherwise continue with the next instruction.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_if_icmplt: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 2);
        JvmSemantics::branch_if_icmplt(ops, insn, args);
    }
};

// if_icmpge (162 (0xa2))
        // Description:
        //   Pop comparison operand(s); branch to the signed offset if the condition is true, otherwise continue with the next instruction.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_if_icmpge: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 2);
        JvmSemantics::branch_if_icmpge(ops, insn, args);
    }
};

// if_icmpgt (163 (0xa3))
        // Description:
        //   Pop comparison operand(s); branch to the signed offset if the condition is true, otherwise continue with the next instruction.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_if_icmpgt: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 2);
        JvmSemantics::branch_if_icmpgt(ops, insn, args);
    }
};

// if_icmple (164 (0xa4))
        // Description:
        //   Pop comparison operand(s); branch to the signed offset if the condition is true, otherwise continue with the next instruction.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_if_icmple: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 2);
        JvmSemantics::branch_if_icmple(ops, insn, args);
    }
};

// ifeq (153 (0x99))
        // Description:
        //   Pop comparison operand(s); branch to the signed offset if the condition is true, otherwise continue with the next instruction.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_ifeq: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 2);
        JvmSemantics::branch_ifeq(ops, insn, args);
    }
};

// ifne (154 (0x9a))
        // Description:
        //   Pop comparison operand(s); branch to the signed offset if the condition is true, otherwise continue with the next instruction.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_ifne: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 2);
        JvmSemantics::branch_ifne(ops, insn, args);
    }
};

// iflt (155 (0x9b))
        // Description:
        //   Pop comparison operand(s); branch to the signed offset if the condition is true, otherwise continue with the next instruction.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_iflt: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 2);
        JvmSemantics::branch_iflt(ops, insn, args);
    }
};

// ifge (156 (0x9c))
        // Description:
        //   Pop comparison operand(s); branch to the signed offset if the condition is true, otherwise continue with the next instruction.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_ifge: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 2);
        JvmSemantics::branch_ifge(ops, insn, args);
    }
};

// ifgt (157 (0x9d))
        // Description:
        //   Pop comparison operand(s); branch to the signed offset if the condition is true, otherwise continue with the next instruction.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_ifgt: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 2);
        JvmSemantics::branch_ifgt(ops, insn, args);
    }
};

// ifle (158 (0x9e))
        // Description:
        //   Pop comparison operand(s); branch to the signed offset if the condition is true, otherwise continue with the next instruction.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_ifle: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 2);
        JvmSemantics::branch_ifle(ops, insn, args);
    }
};

// ifnonnull (199 (0xc7))
        // Description:
        //   Pop comparison operand(s); branch to the signed offset if the condition is true, otherwise continue with the next instruction.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_ifnonnull: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 2);
        JvmSemantics::branch_ifnonnull(ops, insn, args);
    }
};

// ifnull (198 (0xc6))
        // Description:
        //   Pop comparison operand(s); branch to the signed offset if the condition is true, otherwise continue with the next instruction.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_ifnull: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 2);
        JvmSemantics::branch_ifnull(ops, insn, args);
    }
};

// iinc (132 (0x84))
        // Description:
        //   Increment the selected local int variable by the signed constant.
        // Notes:
        //   The wide prefix expands both the local-variable index and increment constant.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_iinc: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 2);
        JvmSemantics::execute_iinc(ops, insn, args);
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
    void p(D /*d*/, Ops ops, I insn, Args args) {
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
    void p(D /*d*/, Ops ops, I insn, Args args) {
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
    void p(D /*d*/, Ops ops, I insn, Args args) {
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
    void p(D /*d*/, Ops ops, I insn, Args args) {
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
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        ops->pushOperand(ops->readLocal(3));
    }
};


// imul (104 (0x68))
        // Description:
        //   Pop operands, apply the arithmetic/bitwise operation, and push the result.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_imul: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        SValue::Ptr v2 = ops->popOperand();
        SValue::Ptr v1 = ops->popOperand();
        ops->pushOperand(JvmSemantics::mul(ops, v1, v2));
    }
};

// ineg (116 (0x74))
        // Description:
        //   Pop a numeric value, negate it, and push the result.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_ineg: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        SValue::Ptr v = ops->popOperand();
        ops->pushOperand(JvmSemantics::neg(ops, v));
    }
};

// instanceof (193 (0xc1))
        // Description:
        //   Pop objectref and push int 1 if it is an instance of the resolved type, otherwise push int 0.
        // Notes:
        //   A null objectref results in 0.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_instanceof: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 2);
        JvmSemantics::execute_instanceof(ops, insn, args);
    }
};

// invokedynamic (186 (0xba))
        // Description:
        //   Resolve the call target, pop receiver/arguments as required, invoke the method, and push any return value.
        // Run-time Exceptions:
        //   NullPointerException if an instance invocation receiver is null.
        //   Errors from method resolution or class/interface initialization may be observed as specified by the JVM.
struct IP_invokedynamic: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 4);
        JvmSemantics::execute_invokedynamic(ops, insn, args);
    }
};

// invokeinterface (185 (0xb9))
        // Description:
        //   Resolve the call target, pop receiver/arguments as required, invoke the method, and push any return value.
        // Run-time Exceptions:
        //   NullPointerException if an instance invocation receiver is null.
        //   Errors from method resolution or class/interface initialization may be observed as specified by the JVM.
struct IP_invokeinterface: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 4);
        JvmSemantics::execute_invokeinterface(ops, insn, args);
    }
};

// invokespecial (183 (0xb7))
        // Description:
        //   Resolve the call target, pop receiver/arguments as required, invoke the method, and push any return value.
        // Run-time Exceptions:
        //   NullPointerException if an instance invocation receiver is null.
        //   Errors from method resolution or class/interface initialization may be observed as specified by the JVM.
struct IP_invokespecial: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 2);
        JvmSemantics::execute_invokespecial(ops, insn, args);
    }
};

// invokestatic (184 (0xb8))
        // Description:
        //   Resolve the call target, pop receiver/arguments as required, invoke the method, and push any return value.
        // Run-time Exceptions:
        //   NullPointerException if an instance invocation receiver is null.
        //   Errors from method resolution or class/interface initialization may be observed as specified by the JVM.
struct IP_invokestatic: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 2);
        JvmSemantics::execute_invokestatic(ops, insn, args);
    }
};

// invokevirtual (182 (0xb6))
        // Description:
        //   Resolve the call target, pop receiver/arguments as required, invoke the method, and push any return value.
        // Run-time Exceptions:
        //   NullPointerException if an instance invocation receiver is null.
        //   Errors from method resolution or class/interface initialization may be observed as specified by the JVM.
struct IP_invokevirtual: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 2);
        JvmSemantics::execute_invokevirtual(ops, insn, args);
    }
};

// ior (128 (0x80))
        // Description:
        //   Pop operands, apply the arithmetic/bitwise operation, and push the result.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_ior: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        SValue::Ptr v2 = ops->popOperand();
        SValue::Ptr v1 = ops->popOperand();
        ops->pushOperand(JvmSemantics::binary(ops, "ior", v1, v2));
    }
};

// irem (112 (0x70))
        // Description:
        //   Pop operands, apply the arithmetic/bitwise operation, and push the result.
        // Run-time Exceptions:
        //   ArithmeticException if the divisor is zero.
struct IP_irem: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        SValue::Ptr v2 = ops->popOperand();
        SValue::Ptr v1 = ops->popOperand();
        JvmSemantics::throwIfIntegerDivisorZero(ops, v2);
        ops->pushOperand(JvmSemantics::rem(ops, v1, v2));
    }
};

// ireturn (172 (0xac))
        // Description:
        //   Return from the current method; typed returns move the return value to the invoker frame, while return returns void.
        // Run-time Exceptions:
        //   IllegalMonitorStateException can be thrown for synchronized methods if structured locking ownership rules are violated.
struct IP_ireturn: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        SValue::Ptr value = ops->popOperand();
        JvmSemantics::methodReturn(ops, insn, value);
    }
};

// ishl (120 (0x78))
        // Description:
        //   Pop operands, apply the arithmetic/bitwise operation, and push the result.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_ishl: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        SValue::Ptr v2 = ops->popOperand();
        SValue::Ptr v1 = ops->popOperand();
        ops->pushOperand(JvmSemantics::binary(ops, "ishl", v1, v2));
    }
};

// ishr (122 (0x7a))
        // Description:
        //   Pop operands, apply the arithmetic/bitwise operation, and push the result.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_ishr: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        SValue::Ptr v2 = ops->popOperand();
        SValue::Ptr v1 = ops->popOperand();
        ops->pushOperand(JvmSemantics::binary(ops, "ishr", v1, v2));
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
    void p(D /*d*/, Ops ops, I insn, Args args) {
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
    void p(D /*d*/, Ops ops, I insn, Args args) {
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
    void p(D /*d*/, Ops ops, I insn, Args args) {
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
    void p(D /*d*/, Ops ops, I insn, Args args) {
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
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        ops->writeLocal(3, ops->popOperand());
    }
};

// isub (100 (0x64))
        // Description:
        //   Pop operands, apply the arithmetic/bitwise operation, and push the result.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_isub: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        SValue::Ptr v2 = ops->popOperand();
        SValue::Ptr v1 = ops->popOperand();
        ops->pushOperand(JvmSemantics::sub(ops, v1, v2));
    }
};

// iushr (124 (0x7c))
        // Description:
        //   Pop operands, apply the arithmetic/bitwise operation, and push the result.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_iushr: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        SValue::Ptr v2 = ops->popOperand();
        SValue::Ptr v1 = ops->popOperand();
        ops->pushOperand(JvmSemantics::binary(ops, "iushr", v1, v2));
    }
};

// ixor (130 (0x82))
        // Description:
        //   Pop operands, apply the arithmetic/bitwise operation, and push the result.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_ixor: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        SValue::Ptr v2 = ops->popOperand();
        SValue::Ptr v1 = ops->popOperand();
        ops->pushOperand(JvmSemantics::binary(ops, "ixor", v1, v2));
    }
};

// jsr (168 (0xa8))
        // Description:
        //   Push the returnAddress of the following instruction and branch to the subroutine offset.
        // Notes:
        //   jsr/jsr_w are obsolete and used with ret in pre-verification-era subroutines.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_jsr: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 2);
        JvmSemantics::branch_jsr(ops, insn, args);
    }
};

// jsr_w (201 (0xc9))
        // Description:
        //   Push the returnAddress of the following instruction and branch to the subroutine offset.
        // Notes:
        //   jsr/jsr_w are obsolete and used with ret in pre-verification-era subroutines.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_jsr_w: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 4);
        JvmSemantics::branch_jsr_w(ops, insn, args);
    }
};

// l2d (138 (0x8a))
        // Description:
        //   Pop a numeric value, convert it to the target primitive type, and push the converted value.
        // Notes:
        //   Conversions follow JVM numeric conversion rules including rounding, NaN handling, and narrowing behavior.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_l2d: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        SValue::Ptr v = ops->popOperand();
        ops->pushOperand(JvmSemantics::convert(ops, "l2d", v));
    }
};

// l2f (137 (0x89))
        // Description:
        //   Pop a numeric value, convert it to the target primitive type, and push the converted value.
        // Notes:
        //   Conversions follow JVM numeric conversion rules including rounding, NaN handling, and narrowing behavior.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_l2f: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        SValue::Ptr v = ops->popOperand();
        ops->pushOperand(JvmSemantics::convert(ops, "l2f", v));
    }
};

// l2i (136 (0x88))
        // Description:
        //   Pop a numeric value, convert it to the target primitive type, and push the converted value.
        // Notes:
        //   Conversions follow JVM numeric conversion rules including rounding, NaN handling, and narrowing behavior.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_l2i: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        SValue::Ptr v = ops->popOperand();
        ops->pushOperand(JvmSemantics::convert(ops, "l2i", v));
    }
};

// ladd (97 (0x61))
        // Description:
        //   Pop operands, apply the arithmetic/bitwise operation, and push the result.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_ladd: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        SValue::Ptr v2 = ops->popOperand();
        SValue::Ptr v1 = ops->popOperand();
        ops->pushOperand(ops->add(v1, v2));
    }
};

// laload (47 (0x2f))
        // Description:
        //   Pop arrayref and index; load the long component at index and push the value.
        // Run-time Exceptions:
        //   NullPointerException if arrayref is null.
        //   ArrayIndexOutOfBoundsException if index is outside the array bounds.
struct IP_laload: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        SValue::Ptr index = ops->popOperand();
        SValue::Ptr arrayref = ops->popOperand();
        JvmSemantics::throwIfNull(ops, "NullPointerException", arrayref);

        JvmSemantics::throwIfArrayIndexOutOfBounds(ops, arrayref, index);

        ops->pushOperand(JvmSemantics::arrayLoad(ops, "l", arrayref, index));
    }
};

// land (127 (0x7f))
        // Description:
        //   Pop operands, apply the arithmetic/bitwise operation, and push the result.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_land: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        SValue::Ptr v2 = ops->popOperand();
        SValue::Ptr v1 = ops->popOperand();
        ops->pushOperand(JvmSemantics::binary(ops, "land", v1, v2));
    }
};

// lastore (80 (0x50))
        // Description:
        //   Pop value, index, and arrayref; store the long value into the array component.
        // Run-time Exceptions:
        //   NullPointerException if arrayref is null.
        //   ArrayIndexOutOfBoundsException if index is outside the array bounds.
struct IP_lastore: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        SValue::Ptr value = ops->popOperand();
        SValue::Ptr index = ops->popOperand();
        SValue::Ptr arrayref = ops->popOperand();
        JvmSemantics::throwIfNull(ops, "NullPointerException", arrayref);

        JvmSemantics::throwIfArrayIndexOutOfBounds(ops, arrayref, index);

        JvmSemantics::arrayStore(ops, "l", arrayref, index, value);
    }
};

// lcmp (148 (0x94))
        // Description:
        //   Compare two numeric values and push int -1, 0, or 1; cmpg/cmpl choose the NaN result direction.
        // Notes:
        //   For cmpg NaN yields 1; for cmpl NaN yields -1.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_lcmp: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        SValue::Ptr v2 = ops->popOperand();
        SValue::Ptr v1 = ops->popOperand();
        ops->pushOperand(JvmSemantics::compare(ops, "lcmp", v1, v2));
    }
};

// lconst_0 (9 (0x09))
        // Description:
        //   Push the named numeric constant.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_lconst_0: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        ops->pushOperand(makeConstant(ops, "l", 0, 64));
    }
};

// lconst_1 (10 (0x0a))
        // Description:
        //   Push the named numeric constant.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_lconst_1: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        ops->pushOperand(makeConstant(ops, "l", 1, 64));
    }
};

// ldc (18 (0x12))
        // Description:
        //   Push a constant-pool constant or resolved symbolic reference value.
        // Notes:
        //   ldc2_w is used for long and double constants.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_ldc: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 1);
        JvmSemantics::execute_ldc(ops, insn, args);
    }
};

// ldc_w (19 (0x13))
        // Description:
        //   Push a constant-pool constant or resolved symbolic reference value.
        // Notes:
        //   ldc2_w is used for long and double constants.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_ldc_w: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 2);
        JvmSemantics::execute_ldc_w(ops, insn, args);
    }
};

// ldc2_w (20 (0x14))
        // Description:
        //   Push a constant-pool constant or resolved symbolic reference value.
        // Notes:
        //   ldc2_w is used for long and double constants.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_ldc2_w: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 2);
        JvmSemantics::execute_ldc2_w(ops, insn, args);
    }
};

// ldiv (109 (0x6d))
        // Description:
        //   Pop operands, apply the arithmetic/bitwise operation, and push the result.
        // Run-time Exceptions:
        //   ArithmeticException if the divisor is zero.
struct IP_ldiv: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        SValue::Ptr v2 = ops->popOperand();
        SValue::Ptr v1 = ops->popOperand();
        JvmSemantics::throwIfLongDivisorZero(ops, v2);
        ops->pushOperand(JvmSemantics::div(ops, v1, v2));
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
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 1);
        const uint32_t index = JvmSemantics::u1(args[0]);
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
    void p(D /*d*/, Ops ops, I insn, Args args) {
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
    void p(D /*d*/, Ops ops, I insn, Args args) {
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
    void p(D /*d*/, Ops ops, I insn, Args args) {
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
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        ops->pushOperand(ops->readLocal(3));
    }
};

// lmul (105 (0x69))
        // Description:
        //   Pop operands, apply the arithmetic/bitwise operation, and push the result.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_lmul: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        SValue::Ptr v2 = ops->popOperand();
        SValue::Ptr v1 = ops->popOperand();
        ops->pushOperand(JvmSemantics::mul(ops, v1, v2));
    }
};

// lneg (117 (0x75))
        // Description:
        //   Pop a numeric value, negate it, and push the result.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_lneg: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        SValue::Ptr v = ops->popOperand();
        ops->pushOperand(JvmSemantics::neg(ops, v));
    }
};

// lookupswitch (171 (0xab))
        // Description:
        //   Pop the int key and branch to the matching case offset or to the default offset.
        // Notes:
        //   Switch operands are padded so defaultbyte begins at a 4-byte boundary from the method start.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_lookupswitch: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        /* Variable-length instruction: decoded operands are supplied by the front end. */
        JvmSemantics::branch_lookupswitch(ops, insn, args);
    }
};

// lor (129 (0x81))
        // Description:
        //   Pop operands, apply the arithmetic/bitwise operation, and push the result.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_lor: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        SValue::Ptr v2 = ops->popOperand();
        SValue::Ptr v1 = ops->popOperand();
        ops->pushOperand(JvmSemantics::binary(ops, "lor", v1, v2));
    }
};

// lrem (113 (0x71))
        // Description:
        //   Pop operands, apply the arithmetic/bitwise operation, and push the result.
        // Run-time Exceptions:
        //   ArithmeticException if the divisor is zero.
struct IP_lrem: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        SValue::Ptr v2 = ops->popOperand();
        SValue::Ptr v1 = ops->popOperand();
        JvmSemantics::throwIfLongDivisorZero(ops, v2);
        ops->pushOperand(JvmSemantics::rem(ops, v1, v2));
    }
};

// lreturn (173 (0xad))
        // Description:
        //   Return from the current method; typed returns move the return value to the invoker frame, while return returns void.
        // Run-time Exceptions:
        //   IllegalMonitorStateException can be thrown for synchronized methods if structured locking ownership rules are violated.
struct IP_lreturn: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        SValue::Ptr value = ops->popOperand();
        JvmSemantics::methodReturn(ops, insn, value);
    }
};

// lshl (121 (0x79))
        // Description:
        //   Pop operands, apply the arithmetic/bitwise operation, and push the result.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_lshl: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        SValue::Ptr v2 = ops->popOperand();
        SValue::Ptr v1 = ops->popOperand();
        ops->pushOperand(JvmSemantics::binary(ops, "lshl", v1, v2));
    }
};

// lshr (123 (0x7b))
        // Description:
        //   Pop operands, apply the arithmetic/bitwise operation, and push the result.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_lshr: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        SValue::Ptr v2 = ops->popOperand();
        SValue::Ptr v1 = ops->popOperand();
        ops->pushOperand(JvmSemantics::binary(ops, "lshr", v1, v2));
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
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 1);
        const uint32_t index = JvmSemantics::u1(args[0]);
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
    void p(D /*d*/, Ops ops, I insn, Args args) {
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
    void p(D /*d*/, Ops ops, I insn, Args args) {
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
    void p(D /*d*/, Ops ops, I insn, Args args) {
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
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        ops->writeLocal(3, ops->popOperand());
    }
};

// lsub (101 (0x65))
        // Description:
        //   Pop operands, apply the arithmetic/bitwise operation, and push the result.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_lsub: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        SValue::Ptr v2 = ops->popOperand();
        SValue::Ptr v1 = ops->popOperand();
        ops->pushOperand(JvmSemantics::sub(ops, v1, v2));
    }
};

// lushr (125 (0x7d))
        // Description:
        //   Pop operands, apply the arithmetic/bitwise operation, and push the result.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_lushr: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        SValue::Ptr v2 = ops->popOperand();
        SValue::Ptr v1 = ops->popOperand();
        ops->pushOperand(JvmSemantics::binary(ops, "lushr", v1, v2));
    }
};

// lxor (131 (0x83))
        // Description:
        //   Pop operands, apply the arithmetic/bitwise operation, and push the result.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_lxor: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        SValue::Ptr v2 = ops->popOperand();
        SValue::Ptr v1 = ops->popOperand();
        ops->pushOperand(JvmSemantics::binary(ops, "lxor", v1, v2));
    }
};

// monitorenter (194 (0xc2))
        // Description:
        //   Enter or exit the monitor associated with objectref.
        // Run-time Exceptions:
        //   NullPointerException if objectref is null.
struct IP_monitorenter: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        JvmSemantics::execute_monitorenter(ops, insn, args);
    }
};

// monitorexit (195 (0xc3))
        // Description:
        //   Enter or exit the monitor associated with objectref.
        // Run-time Exceptions:
        //   NullPointerException if objectref is null.
        //   IllegalMonitorStateException if the current thread does not own the monitor.
struct IP_monitorexit: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        JvmSemantics::execute_monitorexit(ops, insn, args);
    }
};

// multianewarray (197 (0xc5))
        // Description:
        //   Create a new array, using count values popped from the operand stack, and push the resulting array reference.
        // Notes:
        //   Only the first dimensions operand counts are created; lower dimensions may remain unallocated.
        // Run-time Exceptions:
        //   NegativeArraySizeException if any requested dimension is negative.
struct IP_multianewarray: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 3);
        JvmSemantics::execute_multianewarray(ops, insn, args);
    }
};

// new (187 (0xbb))
        // Description:
        //   Resolve the class, allocate an uninitialized instance, and push its reference.
        // Run-time Exceptions:
        //   InstantiationError/IllegalAccessError/other resolution errors can occur as specified by class resolution.
struct IP_new: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 2);
        JvmSemantics::execute_new(ops, insn, args);
    }
};

// newarray (188 (0xbc))
        // Description:
        //   Create a new array, using count values popped from the operand stack, and push the resulting array reference.
        // Run-time Exceptions:
        //   NegativeArraySizeException if any requested dimension is negative.
struct IP_newarray: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 1);
        JvmSemantics::execute_newarray(ops, insn, args);
    }
};

// nop (0 (0x00))
        // Description:
        //   Do nothing.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_nop: P {
    void p(D /*d*/, Ops /*ops*/, I insn, Args args) {
        assert_args(insn, args, 0);
        /* no operation */
    }
};

// pop (87 (0x57))
        // Description:
        //   Rearrange or discard operand stack values according to the JVM category-1/category-2 stack-form rules.
        // Notes:
        //   These instructions must not be used to split a category-2 value.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_pop: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        JvmSemantics::stack_pop(ops);
    }
};

// pop2 (88 (0x58))
        // Description:
        //   Rearrange or discard operand stack values according to the JVM category-1/category-2 stack-form rules.
        // Notes:
        //   These instructions must not be used to split a category-2 value.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_pop2: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        JvmSemantics::stack_pop2(ops);
    }
};

// putfield (181 (0xb5))
        // Description:
        //   Resolve the referenced field and read or write the instance/static field value.
        // Run-time Exceptions:
        //   NullPointerException if objectref is null.
struct IP_putfield: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 2);
        JvmSemantics::execute_putfield(ops, insn, args);
    }
};

// putstatic (179 (0xb3))
        // Description:
        //   Resolve the referenced field and read or write the instance/static field value.
        // Run-time Exceptions:
        //   ExceptionInInitializerError or other class initialization errors may be observed while initializing the declaring class.
struct IP_putstatic: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 2);
        JvmSemantics::execute_putstatic(ops, insn, args);
    }
};

// ret (169 (0xa9))
        // Description:
        //   Continue execution at the returnAddress stored in the selected local variable.
        // Notes:
        //   Used with jsr/jsr_w subroutines; wide can extend the local-variable index.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_ret: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 1);
        JvmSemantics::execute_ret(ops, insn, args);
    }
};

// return (177 (0xb1))
        // Description:
        //   Return from the current method; typed returns move the return value to the invoker frame, while return returns void.
        // Run-time Exceptions:
        //   IllegalMonitorStateException can be thrown for synchronized methods if structured locking ownership rules are violated.
struct IP_return: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        JvmSemantics::methodReturn(ops, insn);
    }
};

// saload (53 (0x35))
        // Description:
        //   Pop arrayref and index; load the short component at index and push the value.
        // Run-time Exceptions:
        //   NullPointerException if arrayref is null.
        //   ArrayIndexOutOfBoundsException if index is outside the array bounds.
struct IP_saload: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        SValue::Ptr index = ops->popOperand();
        SValue::Ptr arrayref = ops->popOperand();
        JvmSemantics::throwIfNull(ops, "NullPointerException", arrayref);

        JvmSemantics::throwIfArrayIndexOutOfBounds(ops, arrayref, index);

        ops->pushOperand(JvmSemantics::arrayLoad(ops, "s", arrayref, index));
    }
};

// sastore (86 (0x56))
        // Description:
        //   Pop value, index, and arrayref; store the short value into the array component.
        // Run-time Exceptions:
        //   NullPointerException if arrayref is null.
        //   ArrayIndexOutOfBoundsException if index is outside the array bounds.
struct IP_sastore: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        SValue::Ptr value = ops->popOperand();
        SValue::Ptr index = ops->popOperand();
        SValue::Ptr arrayref = ops->popOperand();
        JvmSemantics::throwIfNull(ops, "NullPointerException", arrayref);

        JvmSemantics::throwIfArrayIndexOutOfBounds(ops, arrayref, index);

        JvmSemantics::arrayStore(ops, "s", arrayref, index, value);
    }
};

// sipush (17 (0x11))
        // Description:
        //   Sign-extend the immediate operand to int and push it.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_sipush: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 2);
        ops->pushOperand(ops->number_(32, JvmSemantics::s2(args)));
    }
};

// swap (95 (0x5f))
        // Description:
        //   Rearrange or discard operand stack values according to the JVM category-1/category-2 stack-form rules.
        // Notes:
        //   These instructions must not be used to split a category-2 value.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_swap: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        assert_args(insn, args, 0);
        JvmSemantics::stack_swap(ops);
    }
};

// tableswitch (170 (0xaa))
        // Description:
        //   Pop the int key and branch to the matching case offset or to the default offset.
        // Notes:
        //   Switch operands are padded so defaultbyte begins at a 4-byte boundary from the method start.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_tableswitch: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        /* Variable-length instruction: decoded operands are supplied by the front end. */
        JvmSemantics::branch_tableswitch(ops, insn, args);
    }
};

// wide (196 (0xc4))
        // Description:
        //   Modify a following local-variable instruction so it uses a 16-bit local-variable index; for iinc, also use a 16-bit signed constant.
        // Notes:
        //   wide is not a normal standalone execution step; it prefixes the following instruction.
        // Run-time Exceptions:
        //   None specified other than VirtualMachineError subclasses.
struct IP_wide: P {
    void p(D /*d*/, Ops ops, I insn, Args args) {
        /* Variable-length instruction: decoded operands are supplied by the front end. */
        JvmSemantics::execute_wide(ops, insn, args);
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

// Load a constant on to the operand stack.
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

    iprocSet(0x09,  new Jvm::IP_lconst_0);
    iprocSet(0x0a,  new Jvm::IP_lconst_1);

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
