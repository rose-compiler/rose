#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Architecture/Jvm.h>

#include <Rose/BinaryAnalysis/Disassembler/Jvm.h>
#include <Rose/BinaryAnalysis/Unparser/Jvm.h>

#include <SgAsmExecutableFileFormat.h>
#include <SgAsmGenericHeader.h>
#include <SgAsmIntegerValueExpression.h>
#include <SgAsmJvmInstruction.h>
#include <Cxx_GrammarDowncast.h>

// FIXME[Robb Matzke 2023-12-04]: copied from old code; use Sawyer::Message instead.
#define DEBUG_PRINT 0

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

Jvm::Jvm()
    : Base("jvm", 1, ByteOrder::ORDER_LSB) {}

Jvm::~Jvm() {}

Jvm::Ptr
Jvm::instance() {
    return Ptr(new Jvm);
}

RegisterDictionary::Ptr
Jvm::registerDictionary() const {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);
    if (!registerDictionary_.isCached())
        registerDictionary_ = RegisterDictionary::instance(name());
    return registerDictionary_.get();
}

bool
Jvm::matchesHeader(SgAsmGenericHeader *header) const {
    ASSERT_not_null(header);
    const SgAsmExecutableFileFormat::InsSetArchitecture isa = header->get_isa();
    return (isa & SgAsmExecutableFileFormat::ISA_FAMILY_MASK) == SgAsmExecutableFileFormat::ISA_JVM;
}

Sawyer::Container::Interval<size_t>
Jvm::bytesPerInstruction() const {
    return Sawyer::Container::Interval<size_t>::whole();
}

Alignment
Jvm::instructionAlignment() const {
    return Alignment(1, bitsPerWord());
}

std::string
Jvm::instructionDescription(const SgAsmInstruction *insn_) const {
    auto insn = isSgAsmJvmInstruction(insn_);
    ASSERT_not_null(insn);
    switch (insn->get_kind()) {
        case JvmInstructionKind::nop: return "Do nothing";
        case JvmInstructionKind::aconst_null: return "Push null";
        case JvmInstructionKind::iconst_m1: return "Push int constant -1";
        case JvmInstructionKind::iconst_0: return "Push int constant 0";
        case JvmInstructionKind::iconst_1: return "Push int constant 1";
        case JvmInstructionKind::iconst_2: return "Push int constant 2";
        case JvmInstructionKind::iconst_3: return "Push int constant 3";
        case JvmInstructionKind::iconst_4: return "Push int constant 4";
        case JvmInstructionKind::iconst_5: return "Push int constant 5";
        case JvmInstructionKind::lconst_0: return "Push long constant 0";
        case JvmInstructionKind::lconst_1: return "Push long constant 1";
        case JvmInstructionKind::fconst_0: return "Push float 0.0";
        case JvmInstructionKind::fconst_1: return "Push float 1.0";
        case JvmInstructionKind::fconst_2: return "Push float 2.0";
        case JvmInstructionKind::dconst_0: return "Push double 0.0";
        case JvmInstructionKind::dconst_1: return "Push double 1.0";
        case JvmInstructionKind::bipush: return "Push byte";
        case JvmInstructionKind::sipush: return "Push short";
        case JvmInstructionKind::ldc:    return "Push item from run-time constant pool";
        case JvmInstructionKind::ldc_w:  return "Push item from run-time constant pool (wide index)";
        case JvmInstructionKind::ldc2_w: return "Push long or double from run-time constant pool (wide index)";
        case JvmInstructionKind::iload: return "Load int from local variable";
        case JvmInstructionKind::lload: return "Load long from local variable";
        case JvmInstructionKind::fload: return "Load float from local variable";
        case JvmInstructionKind::dload: return "Load double from local variable";
        case JvmInstructionKind::aload: return "Load reference from local variable";
        case JvmInstructionKind::iload_0: return "Load int from local variable 0";
        case JvmInstructionKind::iload_1: return "Load int from local variable 1";
        case JvmInstructionKind::iload_2: return "Load int from local variable 2";
        case JvmInstructionKind::iload_3: return "Load int from local variable 3";
        case JvmInstructionKind::lload_0: return "Load long from local variable 0";
        case JvmInstructionKind::lload_1: return "Load long from local variable 1";
        case JvmInstructionKind::lload_2: return "Load long from local variable 2";
        case JvmInstructionKind::lload_3: return "Load long from local variable 3";
        case JvmInstructionKind::fload_0: return "Load float from local variable 0";
        case JvmInstructionKind::fload_1: return "Load float from local variable 1";
        case JvmInstructionKind::fload_2: return "Load float from local variable 2";
        case JvmInstructionKind::fload_3: return "Load float from local variable 3";
        case JvmInstructionKind::dload_0: return "Load double from local variable 0";
        case JvmInstructionKind::dload_1: return "Load double from local variable 1";
        case JvmInstructionKind::dload_2: return "Load double from local variable 2";
        case JvmInstructionKind::dload_3: return "Load double from local variable 3";
        case JvmInstructionKind::aload_0: return "Load reference from local variable 0";
        case JvmInstructionKind::aload_1: return "Load reference from local variable 1";
        case JvmInstructionKind::aload_2: return "Load reference from local variable 2";
        case JvmInstructionKind::aload_3: return "Load reference from local variable 3";
        case JvmInstructionKind::iaload: return "Load int from array";
        case JvmInstructionKind::laload: return "Load long from array";
        case JvmInstructionKind::faload: return "Load float from array";
        case JvmInstructionKind::daload: return "Load double from array";
        case JvmInstructionKind::aaload: return "Load reference from array";
        case JvmInstructionKind::baload: return "Load byte or boolean from array";
        case JvmInstructionKind::caload: return "Load char from array";
        case JvmInstructionKind::saload: return "Load short from array";
        case JvmInstructionKind::istore: return "Store int into local variable";
        case JvmInstructionKind::lstore: return "Store long into local variable";
        case JvmInstructionKind::fstore: return "Store float into local variable";
        case JvmInstructionKind::dstore: return "Store double into local variable";
        case JvmInstructionKind::astore: return "Store reference into local variable";
        case JvmInstructionKind::istore_0: return "Store int into local variable 0";
        case JvmInstructionKind::istore_1: return "Store int into local variable 1";
        case JvmInstructionKind::istore_2: return "Store int into local variable 2";
        case JvmInstructionKind::istore_3: return "Store int into local variable 3";
        case JvmInstructionKind::lstore_0: return "Store long into local variable 0";
        case JvmInstructionKind::lstore_1: return "Store long into local variable 1";
        case JvmInstructionKind::lstore_2: return "Store long into local variable 2";
        case JvmInstructionKind::lstore_3: return "Store long into local variable 3";
        case JvmInstructionKind::fstore_0: return "Store float into local variable 0";
        case JvmInstructionKind::fstore_1: return "Store float into local variable 1";
        case JvmInstructionKind::fstore_2: return "Store float into local variable 2";
        case JvmInstructionKind::fstore_3: return "Store float into local variable 3";
        case JvmInstructionKind::dstore_0: return "Store double into local variable 0";
        case JvmInstructionKind::dstore_1: return "Store double into local variable 1";
        case JvmInstructionKind::dstore_2: return "Store double into local variable 2";
        case JvmInstructionKind::dstore_3: return "Store double into local variable 3";
        case JvmInstructionKind::astore_0: return "Store reference into local variable 0";
        case JvmInstructionKind::astore_1: return "Store reference into local variable 1";
        case JvmInstructionKind::astore_2: return "Store reference into local variable 2";
        case JvmInstructionKind::astore_3: return "Store reference into local variable 3";
        case JvmInstructionKind::iastore: return "Store into int array";
        case JvmInstructionKind::lastore: return "Store into long array";
        case JvmInstructionKind::fastore: return "Store into float array";
        case JvmInstructionKind::dastore: return "Store into double array";
        case JvmInstructionKind::aastore: return "Store into reference array";
        case JvmInstructionKind::bastore: return "Store into byte or boolean array";
        case JvmInstructionKind::castore: return "Store into char array";
        case JvmInstructionKind::sastore: return "Store into short array";
        case JvmInstructionKind::pop:     return "Pop the top operand stack value";
        case JvmInstructionKind::pop2:    return "Pop the top one or two operand stack values";
        case JvmInstructionKind::dup:     return "Duplicate the top operand stack value";
        case JvmInstructionKind::dup_x1:  return "Duplicate the top operand stack value and insert two values down";
        case JvmInstructionKind::dup_x2:  return "Duplicate the top operand stack value and insert two or three values down";
        case JvmInstructionKind::dup2:    return "Duplicate the top one or two operand stack values";
        case JvmInstructionKind::dup2_x1: return "Duplicate the top one or two operand stack values and insert two or three values down";
        case JvmInstructionKind::dup2_x2: return "Duplicate the top one or two operand stack values and insert two, three, or four values down";
        case JvmInstructionKind::swap: return "Swap the top two operand stack values";
        case JvmInstructionKind::iadd: return "Add int";
        case JvmInstructionKind::ladd: return "Add long";
        case JvmInstructionKind::fadd: return "Add float";
        case JvmInstructionKind::dadd: return "Add double";
        case JvmInstructionKind::isub: return "Subtract int";
        case JvmInstructionKind::lsub: return "Subtract long";
        case JvmInstructionKind::fsub: return "Subtract float";
        case JvmInstructionKind::dsub: return "Subtract double";
        case JvmInstructionKind::imul: return "Multiply int";
        case JvmInstructionKind::lmul: return "Multiply long";
        case JvmInstructionKind::fmul: return "Multiply float";
        case JvmInstructionKind::dmul: return "Multiply double";
        case JvmInstructionKind::idiv: return "Divide int";
        case JvmInstructionKind::ldiv: return "Divide long";
        case JvmInstructionKind::fdiv: return "Divide float";
        case JvmInstructionKind::ddiv: return "Divide double";
        case JvmInstructionKind::irem: return "Remainder int";
        case JvmInstructionKind::lrem: return "Remainder long";
        case JvmInstructionKind::frem: return "Remainder float";
        case JvmInstructionKind::drem: return "Remainder double";
        case JvmInstructionKind::ineg: return "Negate int";
        case JvmInstructionKind::lneg: return "Negate long";
        case JvmInstructionKind::fneg: return "Negate float";
        case JvmInstructionKind::dneg: return "Negate double";
        case JvmInstructionKind::ishl: return "Shift left int";
        case JvmInstructionKind::lshl: return "Shift left long";
        case JvmInstructionKind::ishr: return "Shift right int";
        case JvmInstructionKind::lshr: return "Shift right long";
        case JvmInstructionKind::iushr: return "Logical shift right int";
        case JvmInstructionKind::lushr: return "Logical shift right long";
        case JvmInstructionKind::iand: return "Boolean AND int";
        case JvmInstructionKind::land: return "Boolean AND long";
        case JvmInstructionKind::ior:  return "Boolean OR int";
        case JvmInstructionKind::lor:  return "Boolean OR long";
        case JvmInstructionKind::ixor: return "Boolean XOR int";
        case JvmInstructionKind::lxor: return "Boolean XOR long";
        case JvmInstructionKind::iinc: return "Increment local variable";
        case JvmInstructionKind::i2l: return "Convert int to long";
        case JvmInstructionKind::i2f: return "Convert int to float";
        case JvmInstructionKind::i2d: return "Convert int to double";
        case JvmInstructionKind::l2i: return "Convert long to int";
        case JvmInstructionKind::l2f: return "Convert long to float";
        case JvmInstructionKind::l2d: return "Convert long to double";
        case JvmInstructionKind::f2i: return "Convert float to int";
        case JvmInstructionKind::f2l: return "Convert float to long";
        case JvmInstructionKind::f2d: return "Convert float to double";
        case JvmInstructionKind::d2i: return "Convert double to int";
        case JvmInstructionKind::d2l: return "Convert double to long";
        case JvmInstructionKind::d2f: return "Convert double to float";
        case JvmInstructionKind::i2b: return "Convert int to byte";
        case JvmInstructionKind::i2c: return "Convert int to char";
        case JvmInstructionKind::i2s: return "Convert int to short";
        case JvmInstructionKind::lcmp:  return "Compare long";
        case JvmInstructionKind::fcmpl: return "Compare float";
        case JvmInstructionKind::fcmpg: return "Compare float";
        case JvmInstructionKind::dcmpl: return "Compare double";
        case JvmInstructionKind::dcmpg: return "Compare double";
        case JvmInstructionKind::ifeq: return "Branch if int 'eq' comparison with zero succeeds";
        case JvmInstructionKind::ifne: return "Branch if int 'ne' comparison with zero succeeds";
        case JvmInstructionKind::iflt: return "Branch if int 'lt' comparison with zero succeeds";
        case JvmInstructionKind::ifge: return "Branch if int 'ge' comparison with zero succeeds";
        case JvmInstructionKind::ifgt: return "Branch if int 'gt' comparison with zero succeeds";
        case JvmInstructionKind::ifle: return "Branch if int 'le' comparison with zero succeeds";
        case JvmInstructionKind::if_icmpeq: return "Branch if int 'eq' comparison succeeds";
        case JvmInstructionKind::if_icmpne: return "Branch if int 'ne' comparison succeeds";
        case JvmInstructionKind::if_icmplt: return "Branch if int 'lt' comparison succeeds";
        case JvmInstructionKind::if_icmpge: return "Branch if int 'ge' comparison succeeds";
        case JvmInstructionKind::if_icmpgt: return "Branch if int 'gt' comparison succeeds";
        case JvmInstructionKind::if_icmple: return "Branch if int 'le' comparison succeeds";
        case JvmInstructionKind::if_acmpeq: return "Branch if reference 'eq' comparison succeeds";
        case JvmInstructionKind::if_acmpne: return "Branch if reference 'ne' comparison succeeds";
        case JvmInstructionKind::goto_: return "Branch always";
        case JvmInstructionKind::jsr:   return "Jump subroutine";
        case JvmInstructionKind::ret:   return "Return from subroutine";
        case JvmInstructionKind::tableswitch:  return "Access jump table by index and jump";
        case JvmInstructionKind::lookupswitch: return "Access jump table by key match and jump";
        case JvmInstructionKind::ireturn: return "Return int from method";
        case JvmInstructionKind::lreturn: return "Return long from method";
        case JvmInstructionKind::freturn: return "Return float from method";
        case JvmInstructionKind::dreturn: return "Return double from method";
        case JvmInstructionKind::areturn: return "Return reference from method";
        case JvmInstructionKind::return_: return "Return void from method";
        case JvmInstructionKind::getstatic: return "Get static field from class";
        case JvmInstructionKind::putstatic: return "Set static field in class";
        case JvmInstructionKind::getfield:  return "Fetch field from object";
        case JvmInstructionKind::putfield:  return "Set field in object";
        case JvmInstructionKind::invokevirtual:   return "Invoke instance method; dispatch based on class";
        case JvmInstructionKind::invokespecial:   return "Invoke instance method; direct invocation...";
        case JvmInstructionKind::invokestatic:    return "Invoke a class (static) method";
        case JvmInstructionKind::invokeinterface: return "Invoke interface method";
        case JvmInstructionKind::invokedynamic:   return "Invoke a dynamically-computed call site";
        case JvmInstructionKind::new_:        return "Create new object";
        case JvmInstructionKind::newarray:    return "Create new array";
        case JvmInstructionKind::anewarray:   return "Create new array of reference";
        case JvmInstructionKind::arraylength: return "Get length of array";
        case JvmInstructionKind::athrow:      return "Throw exception or error";
        case JvmInstructionKind::checkcast:   return "Check whether object is of given type";
        case JvmInstructionKind::instanceof:  return "Determine if object is of given type";
        case JvmInstructionKind::monitorenter: return "Enter monitor for object";
        case JvmInstructionKind::monitorexit:  return "Exit monitor for object";
        case JvmInstructionKind::wide: return "Extend local variable index by additional bytes";
        case JvmInstructionKind::multianewarray: return "Create new multidimensional array";
        case JvmInstructionKind::ifnull:    return "Branch if reference is null";
        case JvmInstructionKind::ifnonnull: return "Branch if reference not null";
        case JvmInstructionKind::goto_w: return "Branch always (wide index)";
        case JvmInstructionKind::jsr_w:  return "Jump subroutine (wide index)";

            /* 6.2 Reserved Opcodes (should not be encountered in valid class file) */
        case JvmInstructionKind::breakpoint: return "Breakpoint (reserved opcode)";
        case JvmInstructionKind::impdep1: return "Implementation dependent 1 (reserved opcode)";
        case JvmInstructionKind::impdep2: return "Implementation dependent 2 (reserved opcode)";

        case JvmInstructionKind::unknown: return "Unknown opcode";
    }
    ASSERT_not_reachable("invalid JVM instruction kind: " + std::to_string(static_cast<int>(insn->get_kind())));
}

bool
Jvm::isUnknown(const SgAsmInstruction *insn_) const {
    auto insn = isSgAsmJvmInstruction(insn_);
    ASSERT_not_null(insn);
    return JvmInstructionKind::unknown == insn->get_kind();
}

bool
Jvm::terminatesBasicBlock(SgAsmInstruction *insn_) const {
    auto insn = isSgAsmJvmInstruction(insn_);
    ASSERT_not_null(insn);
    switch (insn->get_kind()) {
        case JvmInstructionKind::ifeq:
        case JvmInstructionKind::ifne:
        case JvmInstructionKind::iflt:
        case JvmInstructionKind::ifge:
        case JvmInstructionKind::ifgt:
        case JvmInstructionKind::ifle:
        case JvmInstructionKind::if_icmpeq:
        case JvmInstructionKind::if_icmpne:
        case JvmInstructionKind::if_icmplt:
        case JvmInstructionKind::if_icmpge:
        case JvmInstructionKind::if_icmpgt:
        case JvmInstructionKind::if_icmple:
        case JvmInstructionKind::if_acmpeq:
        case JvmInstructionKind::if_acmpne:
        case JvmInstructionKind::goto_:
        case JvmInstructionKind::jsr:
        case JvmInstructionKind::ret:
        case JvmInstructionKind::tableswitch:
        case JvmInstructionKind::lookupswitch:
        case JvmInstructionKind::ireturn:
        case JvmInstructionKind::lreturn:
        case JvmInstructionKind::freturn:
        case JvmInstructionKind::dreturn:
        case JvmInstructionKind::areturn:
        case JvmInstructionKind::return_:
#if 0 // NOT_SURE
        case JvmInstructionKind::getstatic: return "Get static field from class";
        case JvmInstructionKind::putstatic: return "Set static field in class";
        case JvmInstructionKind::getfield:  return "Fetch field from object";
        case JvmInstructionKind::putfield:  return "Set field in object";
#endif
        case JvmInstructionKind::invokevirtual:
        case JvmInstructionKind::invokespecial:
        case JvmInstructionKind::invokestatic:
        case JvmInstructionKind::invokeinterface:
        case JvmInstructionKind::invokedynamic:
#if 0 // NOT_SURE
        case JvmInstructionKind::new_:        return "Create new object";
        case JvmInstructionKind::newarray:    return "Create new array";
        case JvmInstructionKind::anewarray:   return "Create new array of reference";
        case JvmInstructionKind::athrow:      return "Throw exception or error";
        case JvmInstructionKind::monitorenter: return "Enter monitor for object";
        case JvmInstructionKind::monitorexit:  return "Exit monitor for object";
#endif
        case JvmInstructionKind::ifnull:
        case JvmInstructionKind::ifnonnull:
        case JvmInstructionKind::goto_w:
        case JvmInstructionKind::jsr_w:
#if 0 // NOT_SURE
        case JvmInstructionKind::breakpoint: return "Breakpoint (reserved opcode)";
        case JvmInstructionKind::impdep1: return "Implementation dependent 1 (reserved opcode)";
        case JvmInstructionKind::impdep2: return "Implementation dependent 2 (reserved opcode)";
        case JvmInstructionKind::unknown: return "Unknown opcode";
#endif
            return true;
        default:
            return false;
    }
}

bool
Jvm::isFunctionCallFast(const std::vector<SgAsmInstruction*> &insns, rose_addr_t *target, rose_addr_t *return_va) const {
    if (insns.empty())
        return false;

    auto last = isSgAsmJvmInstruction(insns.back());
    ASSERT_not_null(last);

    // Quick method based only on the kind of instruction
    switch (last->get_kind()) {
        case JvmInstructionKind::invokevirtual:
        case JvmInstructionKind::invokespecial:
        case JvmInstructionKind::invokestatic:
        case JvmInstructionKind::invokeinterface:
        case JvmInstructionKind::invokedynamic:
            //    case JvmInstructionKind::jsr: ???

            if (target) {
                branchTarget(last).assignTo(*target);
            }
            if (return_va) {
                *return_va = last->get_address() + last->get_size();
            }
            return true;

        default:
            return false;
    }
}

bool
Jvm::isFunctionReturnFast(const std::vector<SgAsmInstruction*> &insns) const {
    auto last = isSgAsmJvmInstruction(insns.back());
    ASSERT_not_null(last);

    switch (last->get_kind()) {
        case JvmInstructionKind::ret:
        case JvmInstructionKind::ireturn:
        case JvmInstructionKind::lreturn:
        case JvmInstructionKind::freturn:
        case JvmInstructionKind::dreturn:
        case JvmInstructionKind::areturn:
        case JvmInstructionKind::return_:
            return true;
        default:
            return false;
    }
}

Sawyer::Optional<rose_addr_t>
Jvm::branchTarget(SgAsmInstruction *insn_) const {
    auto insn = isSgAsmJvmInstruction(insn_);
    ASSERT_not_null(insn);

    switch (insn->get_kind()) {
        case JvmInstructionKind::ifeq: // "Branch if int 'eq' comparison with zero succeeds"
        case JvmInstructionKind::ifne: // "Branch if int 'ne' comparison with zero succeeds";
        case JvmInstructionKind::iflt: // "Branch if int 'lt' comparison with zero succeeds";
        case JvmInstructionKind::ifge: // "Branch if int 'ge' comparison with zero succeeds";
        case JvmInstructionKind::ifgt: // "Branch if int 'gt' comparison with zero succeeds";
        case JvmInstructionKind::ifle: // "Branch if int 'le' comparison with zero succeeds";
        case JvmInstructionKind::if_icmpeq: // "Branch if int 'eq' comparison succeeds";
        case JvmInstructionKind::if_icmpne: // "Branch if int 'ne' comparison succeeds";
        case JvmInstructionKind::if_icmplt: // "Branch if int 'lt' comparison succeeds";
        case JvmInstructionKind::if_icmpge: // "Branch if int 'ge' comparison succeeds";
        case JvmInstructionKind::if_icmpgt: // "Branch if int 'gt' comparison succeeds";
        case JvmInstructionKind::if_icmple: // "Branch if int 'le' comparison succeeds";
        case JvmInstructionKind::if_acmpeq: // "Branch if reference 'eq' comparison succeeds";
        case JvmInstructionKind::if_acmpne: // "Branch if reference 'ne' comparison succeeds";
        case JvmInstructionKind::ifnull:    // "Branch if reference is null";
        case JvmInstructionKind::ifnonnull: // "Branch if reference not null";
        case JvmInstructionKind::goto_:     // "Branch always";
        case JvmInstructionKind::goto_w:    // "Branch always (wide index)";
        case JvmInstructionKind::jsr:       // "Jump subroutine";
        case JvmInstructionKind::jsr_w:     // "Jump subroutine (wide index)";
            break;

            // A branch instruction but branch target (an offset) is not available
        case JvmInstructionKind::invokevirtual:   // "Invoke instance method; dispatch based on class";
        case JvmInstructionKind::invokespecial:   // "Invoke instance method; direct invocation...";
        case JvmInstructionKind::invokestatic:    // "Invoke a class (static) method";
        case JvmInstructionKind::invokeinterface: // "Invoke interface method";
        case JvmInstructionKind::invokedynamic:   // "Invoke a dynamically-computed call site";
        case JvmInstructionKind::athrow:       // "Throw exception or error";
        case JvmInstructionKind::monitorenter: // "Enter monitor for object";
        case JvmInstructionKind::monitorexit:  // "Exit monitor for object";
            return Sawyer::Nothing();

            // A branch instruction but branch target(s) (offsets) need to be calculated
        case JvmInstructionKind::tableswitch:  // "Access jump table by index and jump";
        case JvmInstructionKind::lookupswitch: // "Access jump table by key match and jump";
            return Sawyer::Nothing();

            // Not a branching instruction
        default:
            return Sawyer::Nothing();
    }

    if (insn->nOperands() == 1) {
        if (SgAsmIntegerValueExpression *ival = isSgAsmIntegerValueExpression(insn->operand(0))) {
            return insn->get_address() + ival->get_signedValue();
        }
    }
    return Sawyer::Nothing();
}

static AddressSet
switchSuccessors(const SgAsmJvmInstruction* insn, bool &complete) {
    SgAsmIntegerValueExpression* ival{nullptr};
    AddressSet retval{};
    rose_addr_t va{insn->get_address()};
    auto kind{insn->get_kind()};

    complete = false;

    if ((kind == JvmInstructionKind::lookupswitch) && (1 < insn->nOperands())) {
        size_t nOperands{0};
        int32_t defOff{0}, nPairs{0};
        if ((ival = isSgAsmIntegerValueExpression(insn->operand(0))))
            defOff = ival->get_signedValue();
        if ((ival = isSgAsmIntegerValueExpression(insn->operand(1))))
            nPairs = ival->get_signedValue();

        nOperands = 2 + 2*nPairs;
        if (nOperands == insn->nOperands()) {
            retval.insert(va + defOff);
#if DEBUG_PRINT
            std::cout << "... switchSuccessors: insert " << va + defOff << std::endl;
#endif
            for (size_t n{3}; n < nOperands; n+=2) {
                if ((ival = isSgAsmIntegerValueExpression(insn->operand(n)))) {
                    retval.insert(va + ival->get_signedValue());
#if DEBUG_PRINT
                    std::cout << "... switchSuccessors: insert " << va + ival->get_signedValue() << std::endl;
#endif
                } else {
                    return AddressSet{};
                }
            }
        } else {
            return AddressSet{};
        }
    } else if ((kind == JvmInstructionKind::tableswitch) && (2 < insn->nOperands())) {
        size_t nOperands{0};
        int32_t defOff{0}, low{0}, high{0};
        if ((ival = isSgAsmIntegerValueExpression(insn->operand(0))))
            defOff = ival->get_signedValue();
        if ((ival = isSgAsmIntegerValueExpression(insn->operand(1))))
            low = ival->get_signedValue();
        if ((ival = isSgAsmIntegerValueExpression(insn->operand(2))))
            high = ival->get_signedValue();

#if DEBUG_PRINT
        std::cout << "... switchSuccessors: " << insn->nOperands()
                  << ": " << defOff
                  << ": " << low
                  << ": " << high
                  << ": va: " << va
                  << ": fall_through: " << va + insn->get_size()
                  << std::endl;
#endif

        nOperands = 3 + high - low + 1;
        if (nOperands == insn->nOperands()) {
            retval.insert(va + defOff);
#if DEBUG_PRINT
            if (kind == JvmInstructionKind::goto_)
                std::cout << "WARNING: GOTO!!!\n";
            std::cout << "... switchSuccessors (fall through): insert " << va + defOff << std::endl;
#endif
            for (size_t n{3}; n < nOperands; n++) {
                if ((ival = isSgAsmIntegerValueExpression(insn->operand(n)))) {
                    retval.insert(va + ival->get_signedValue());
#if DEBUG_PRINT
                    std::cout << "... switchSuccessors: insert " << va + ival->get_signedValue() << std::endl;
#endif
                } else {
                    return AddressSet{};
                }
            }
        } else {
            return AddressSet{};
        }
    } else {
        return AddressSet{};
    }

    complete = true;
    return retval;
}

AddressSet
Jvm::getSuccessors(SgAsmInstruction *insn_, bool &complete) const {
    auto insn = isSgAsmJvmInstruction(insn_);
    ASSERT_not_null(insn);

    auto kind{insn->get_kind()};
    complete = false;

    switch (kind) {
        case JvmInstructionKind::tableswitch:  // "Access jump table by index and jump";
        case JvmInstructionKind::lookupswitch: // "Access jump table by key match and jump";
            return switchSuccessors(insn, complete);

            // A branch instruction but branch target is not immediately available
        case JvmInstructionKind::invokevirtual:   // "Invoke instance method; dispatch based on class";
        case JvmInstructionKind::invokespecial:   // "Invoke instance method; direct invocation...";
        case JvmInstructionKind::invokestatic:    // "Invoke a class (static) method";
        case JvmInstructionKind::invokeinterface: // "Invoke interface method";
        case JvmInstructionKind::invokedynamic:   // "Invoke a dynamically-computed call site";
        case JvmInstructionKind::monitorenter: // "Enter monitor for object";
        case JvmInstructionKind::monitorexit:  // "Exit monitor for object";
            // Don't assume a CALL instruction returns to the fall-through address, but
            // by default, EngineJvm reverses this with partitioner->autoAddCallReturnEdges(true)
            return AddressSet{};

        case JvmInstructionKind::athrow: // "Throw exception or error";
            return AddressSet{};

        default:
            break;
    }

    if (auto target{branchTarget(insn)}) {
        AddressSet retval{*target};
        // Add fall through target if not a branch always instruction
        if ((kind != JvmInstructionKind::goto_) && (kind != JvmInstructionKind::goto_w)) {
            retval.insert(insn->get_address() + insn->get_size());
        }
        complete = true;
        return retval;
    }

    return AddressSet{};
}

Disassembler::Base::Ptr
Jvm::newInstructionDecoder() const {
    return Disassembler::Jvm::instance(shared_from_this());
}

Unparser::Base::Ptr
Jvm::newUnparser() const {
    return Unparser::Jvm::instance(shared_from_this());
}

} // namespace
} // namespace
} // namespace

#endif
