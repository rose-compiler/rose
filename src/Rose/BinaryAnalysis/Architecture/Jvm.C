#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Architecture/Jvm.h>

#include <Rose/BinaryAnalysis/Disassembler/Jvm.h>
#include <Rose/BinaryAnalysis/Unparser/Jvm.h>

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
