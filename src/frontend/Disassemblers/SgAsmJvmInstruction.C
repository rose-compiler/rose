/* SgAsmJvmInstruction member definitions */
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"

#define DEBUG_PRINT 0

using namespace Rose;                                   // temporary until this lives in "rose"
using namespace Rose::BinaryAnalysis;
using JvmOp = JvmInstructionKind;

// Make a (moderately) deep copy of this instruction.
// The parent is set to null so that it is detached from the ROSE AST.
SgAsmJvmInstruction*
SgAsmJvmInstruction::copy() const {
  auto insn = new SgAsmJvmInstruction(get_address(), get_mnemonic(), get_kind());

  insn->set_parent(nullptr); // SgNode member
  insn->set_comment(get_comment()); // SgAsmNode member
  insn->set_raw_bytes(get_raw_bytes()); // SgAsmInstruction member

  // Create new operand list
  auto operands = new SgAsmOperandList;
  insn->set_operandList(operands);
  operands->set_parent(insn);

  // Copy operands (warning they become aliased)
  for (auto expr: get_operandList()->get_operands()) {
    // At moment the operand isn't attached? Whine to inform of the need for exploration.
    ASSERT_require(expr->get_parent() == nullptr);
    operands->append_operand(expr);
  }

  return insn;
}

unsigned
SgAsmJvmInstruction::get_anyKind() const {
  return static_cast<unsigned>(p_kind);
}

bool
SgAsmJvmInstruction::terminatesBasicBlock() {
  switch (get_kind()) {
    /*153*/ case JvmOp::ifeq:
    /*154*/ case JvmOp::ifne:
    /*155*/ case JvmOp::iflt:
    /*156*/ case JvmOp::ifge:
    /*157*/ case JvmOp::ifgt:
    /*158*/ case JvmOp::ifle:
    /*159*/ case JvmOp::if_icmpeq:
    /*160*/ case JvmOp::if_icmpne:
    /*161*/ case JvmOp::if_icmplt:
    /*162*/ case JvmOp::if_icmpge:
    /*163*/ case JvmOp::if_icmpgt:
    /*164*/ case JvmOp::if_icmple:
    /*165*/ case JvmOp::if_acmpeq:
    /*166*/ case JvmOp::if_acmpne:
    /*167*/ case JvmOp::goto_:
    /*168*/ case JvmOp::jsr:
    /*169*/ case JvmOp::ret:
    /*170*/ case JvmOp::tableswitch:
    /*171*/ case JvmOp::lookupswitch:
    /*172*/ case JvmOp::ireturn:
    /*173*/ case JvmOp::lreturn:
    /*174*/ case JvmOp::freturn:
    /*175*/ case JvmOp::dreturn:
    /*176*/ case JvmOp::areturn:
    /*177*/ case JvmOp::return_:
#ifdef NOT_SURE
    /*178*/ case JvmOp::getstatic: return "Get static field from class";
    /*179*/ case JvmOp::putstatic: return "Set static field in class";
    /*180*/ case JvmOp::getfield:  return "Fetch field from object";
    /*181*/ case JvmOp::putfield:  return "Set field in object";
#endif
    /*182*/ case JvmOp::invokevirtual:
    /*183*/ case JvmOp::invokespecial:
    /*184*/ case JvmOp::invokestatic:
    /*185*/ case JvmOp::invokeinterface:
    /*186*/ case JvmOp::invokedynamic:
#ifdef NOT_SURE
    /*187*/ case JvmOp::new_:        return "Create new object";
    /*188*/ case JvmOp::newarray:    return "Create new array";
    /*189*/ case JvmOp::anewarray:   return "Create new array of reference";
    /*191*/ case JvmOp::athrow:      return "Throw exception or error";
    /*194*/ case JvmOp::monitorenter: return "Enter monitor for object";
    /*195*/ case JvmOp::monitorexit:  return "Exit monitor for object";
#endif
    /*198*/ case JvmOp::ifnull:
    /*199*/ case JvmOp::ifnonnull:
    /*200*/ case JvmOp::goto_w:
    /*201*/ case JvmOp::jsr_w:
#ifdef NOT_SURE
    /*202*/ case JvmOp::breakpoint: return "Breakpoint (reserved opcode)";
    /*254*/ case JvmOp::impdep1: return "Implementation dependent 1 (reserved opcode)";
    /*255*/ case JvmOp::impdep2: return "Implementation dependent 2 (reserved opcode)";
    /*666*/ case JvmOp::unknown: return "Unknown opcode";
#endif
      return true;
    default:
      return false;
  }
  ASSERT_not_reachable("invalid JVM instruction kind: " + std::to_string(static_cast<int>(get_kind())));

  return false;
}

bool
SgAsmJvmInstruction::isFunctionCallFast(const std::vector<SgAsmInstruction*> &insns, rose_addr_t *target, rose_addr_t *return_va)
{
  SgAsmJvmInstruction* last{nullptr};

  if (!insns.empty() && (last=isSgAsmJvmInstruction(insns.back()))) {
    // Quick method based only on the kind of instruction
    switch (last->get_kind()) {
      case JvmOp::invokevirtual:
      case JvmOp::invokespecial:
      case JvmOp::invokestatic:
      case JvmOp::invokeinterface:
      case JvmOp::invokedynamic:
//    case JvmOp::jsr: ???

        if (target) {
          last->branchTarget().assignTo(*target);
        }
        if (return_va) {
          *return_va = last->get_address() + last->get_size();
        }
        return true;

      default:
        return false;
    }
  }

  return false;
}

bool
SgAsmJvmInstruction::isFunctionCallSlow(const std::vector<SgAsmInstruction*> &insns, rose_addr_t *target, rose_addr_t *return_va)
{
  return isFunctionCallFast(insns, target, return_va);
}

bool
SgAsmJvmInstruction::isFunctionReturnFast(const std::vector<SgAsmInstruction*> &insns) {
  auto iJvm = isSgAsmJvmInstruction(insns.back());
  if (iJvm) {
    switch (iJvm->get_kind()) {
      case JvmOp::ret:
      case JvmOp::ireturn:
      case JvmOp::lreturn:
      case JvmOp::freturn:
      case JvmOp::dreturn:
      case JvmOp::areturn:
      case JvmOp::return_:
        return true;
      default:
        return false;
    }
  }
  return false;
}

bool
SgAsmJvmInstruction::isFunctionReturnSlow(const std::vector<SgAsmInstruction*> &insns) {
  return isFunctionReturnFast(insns);
}

bool
SgAsmJvmInstruction::isUnknown() const
{
  return JvmOp::unknown == get_kind();
}

static AddressSet
switchSuccessors(const SgAsmJvmInstruction* insn, bool &complete) {
  SgAsmIntegerValueExpression* ival{nullptr};
  AddressSet retval{};
  rose_addr_t va{insn->get_address()};
  auto kind{insn->get_kind()};

  complete = false;

  if ((kind == JvmOp::lookupswitch) && (1 < insn->nOperands())) {
    size_t nOperands{0};
    int32_t defOff{0}, nPairs{0};
    if ((ival = isSgAsmIntegerValueExpression(insn->operand(0)))) defOff = ival->get_signedValue();
    if ((ival = isSgAsmIntegerValueExpression(insn->operand(1)))) nPairs = ival->get_signedValue();

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
        }
        else return AddressSet{};
      }
    } else return AddressSet{};
  }
  else if ((kind == JvmOp::tableswitch) && (2 < insn->nOperands())) {
    size_t nOperands{0};
    int32_t defOff{0}, low{0}, high{0};
    if ((ival = isSgAsmIntegerValueExpression(insn->operand(0)))) defOff = ival->get_signedValue();
    if ((ival = isSgAsmIntegerValueExpression(insn->operand(1))))    low = ival->get_signedValue();
    if ((ival = isSgAsmIntegerValueExpression(insn->operand(2))))   high = ival->get_signedValue();

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
      if (kind == JvmOp::goto_) std::cout << "WARNING: GOTO!!!\n";
        std::cout << "... switchSuccessors (fall through): insert " << va + defOff << std::endl;
#endif
      for (int n{3}; n < nOperands; n++) {
        if ((ival = isSgAsmIntegerValueExpression(insn->operand(n)))) {
          retval.insert(va + ival->get_signedValue());
#if DEBUG_PRINT
          std::cout << "... switchSuccessors: insert " << va + ival->get_signedValue() << std::endl;
#endif
        }
        else return AddressSet{};
      }
    }
    else return AddressSet{};
  }
  else return AddressSet{};

  complete = true;
  return retval;
}

AddressSet
SgAsmJvmInstruction::getSuccessors(bool &complete) {
  auto kind{get_kind()};
  complete = false;

  switch (kind) {
    case JvmOp::tableswitch:  // "Access jump table by index and jump";
    case JvmOp::lookupswitch: // "Access jump table by key match and jump";
      return switchSuccessors(this, complete);

 // A branch instruction but branch target (an offset) is not available
    case JvmOp::invokevirtual:   // "Invoke instance method; dispatch based on class";
    case JvmOp::invokespecial:   // "Invoke instance method; direct invocation...";
    case JvmOp::invokestatic:    // "Invoke a class (static) method";
    case JvmOp::invokeinterface: // "Invoke interface method";
    case JvmOp::invokedynamic:   // "Invoke a dynamically-computed call site";
    case JvmOp::monitorenter: // "Enter monitor for object";
    case JvmOp::monitorexit:  // "Exit monitor for object";
      // Instruction falls through to the next instruction
      return AddressSet{get_address() + get_size()};

    case JvmOp::athrow: // "Throw exception or error";
      return AddressSet{};

    default:
      break;
  }

  if (auto target{branchTarget()}) {
    AddressSet retval{*target};
    // Add fall through target if not a branch always instruction
    if ((kind != JvmOp::goto_) && (kind != JvmOp::goto_w)) {
      retval.insert(get_address() + get_size());
    }
    complete = true;
    return retval;
  }

  return AddressSet{};
}

std::string
SgAsmJvmInstruction::description() const {
  switch (get_kind()) {
    /*  0*/ case JvmOp::nop: return "Do nothing";
    /*  1*/ case JvmOp::aconst_null: return "Push null";
    /*  2*/ case JvmOp::iconst_m1: return "Push int constant -1";
    /*  3*/ case JvmOp::iconst_0: return "Push int constant 0";
    /*  4*/ case JvmOp::iconst_1: return "Push int constant 1";
    /*  5*/ case JvmOp::iconst_2: return "Push int constant 2";
    /*  6*/ case JvmOp::iconst_3: return "Push int constant 3";
    /*  7*/ case JvmOp::iconst_4: return "Push int constant 4";
    /*  8*/ case JvmOp::iconst_5: return "Push int constant 5";
    /*  9*/ case JvmOp::lconst_0: return "Push long constant 0";
    /* 10*/ case JvmOp::lconst_1: return "Push long constant 1";
    /* 11*/ case JvmOp::fconst_0: return "Push float 0.0";
    /* 12*/ case JvmOp::fconst_1: return "Push float 1.0";
    /* 13*/ case JvmOp::fconst_2: return "Push float 2.0";
    /* 14*/ case JvmOp::dconst_0: return "Push double 0.0";
    /* 15*/ case JvmOp::dconst_1: return "Push double 1.0";
    /* 16*/ case JvmOp::bipush: return "Push byte";
    /* 17*/ case JvmOp::sipush: return "Push short";
    /* 18*/ case JvmOp::ldc:    return "Push item from run-time constant pool";
    /* 19*/ case JvmOp::ldc_w:  return "Push item from run-time constant pool (wide index)";
    /* 20*/ case JvmOp::ldc2_w: return "Push long or double from run-time constant pool (wide index)";
    /* 21*/ case JvmOp::iload: return "Load int from local variable";
    /* 22*/ case JvmOp::lload: return "Load long from local variable";
    /* 23*/ case JvmOp::fload: return "Load float from local variable";
    /* 24*/ case JvmOp::dload: return "Load double from local variable";
    /* 25*/ case JvmOp::aload: return "Load reference from local variable";
    /* 26*/ case JvmOp::iload_0: return "Load int from local variable 0";
    /* 27*/ case JvmOp::iload_1: return "Load int from local variable 1";
    /* 28*/ case JvmOp::iload_2: return "Load int from local variable 2";
    /* 29*/ case JvmOp::iload_3: return "Load int from local variable 3";
    /* 30*/ case JvmOp::lload_0: return "Load long from local variable 0";
    /* 31*/ case JvmOp::lload_1: return "Load long from local variable 1";
    /* 32*/ case JvmOp::lload_2: return "Load long from local variable 2";
    /* 33*/ case JvmOp::lload_3: return "Load long from local variable 3";
    /* 34*/ case JvmOp::fload_0: return "Load float from local variable 0";
    /* 35*/ case JvmOp::fload_1: return "Load float from local variable 1";
    /* 36*/ case JvmOp::fload_2: return "Load float from local variable 2";
    /* 37*/ case JvmOp::fload_3: return "Load float from local variable 3";
    /* 38*/ case JvmOp::dload_0: return "Load double from local variable 0";
    /* 39*/ case JvmOp::dload_1: return "Load double from local variable 1";
    /* 40*/ case JvmOp::dload_2: return "Load double from local variable 2";
    /* 41*/ case JvmOp::dload_3: return "Load double from local variable 3";
    /* 42*/ case JvmOp::aload_0: return "Load reference from local variable 0";
    /* 43*/ case JvmOp::aload_1: return "Load reference from local variable 1";
    /* 44*/ case JvmOp::aload_2: return "Load reference from local variable 2";
    /* 45*/ case JvmOp::aload_3: return "Load reference from local variable 3";
    /* 46*/ case JvmOp::iaload: return "Load int from array";
    /* 47*/ case JvmOp::laload: return "Load long from array";
    /* 48*/ case JvmOp::faload: return "Load float from array";
    /* 49*/ case JvmOp::daload: return "Load double from array";
    /* 50*/ case JvmOp::aaload: return "Load reference from array";
    /* 51*/ case JvmOp::baload: return "Load byte or boolean from array";
    /* 52*/ case JvmOp::caload: return "Load char from array";
    /* 53*/ case JvmOp::saload: return "Load short from array";
    /* 54*/ case JvmOp::istore: return "Store int into local variable";
    /* 55*/ case JvmOp::lstore: return "Store long into local variable";
    /* 56*/ case JvmOp::fstore: return "Store float into local variable";
    /* 57*/ case JvmOp::dstore: return "Store double into local variable";
    /* 58*/ case JvmOp::astore: return "Store reference into local variable";
    /* 59*/ case JvmOp::istore_0: return "Store int into local variable 0";
    /* 60*/ case JvmOp::istore_1: return "Store int into local variable 1";
    /* 61*/ case JvmOp::istore_2: return "Store int into local variable 2";
    /* 62*/ case JvmOp::istore_3: return "Store int into local variable 3";
    /* 63*/ case JvmOp::lstore_0: return "Store long into local variable 0";
    /* 64*/ case JvmOp::lstore_1: return "Store long into local variable 1";
    /* 65*/ case JvmOp::lstore_2: return "Store long into local variable 2";
    /* 66*/ case JvmOp::lstore_3: return "Store long into local variable 3";
    /* 67*/ case JvmOp::fstore_0: return "Store float into local variable 0";
    /* 68*/ case JvmOp::fstore_1: return "Store float into local variable 1";
    /* 69*/ case JvmOp::fstore_2: return "Store float into local variable 2";
    /* 70*/ case JvmOp::fstore_3: return "Store float into local variable 3";
    /* 71*/ case JvmOp::dstore_0: return "Store double into local variable 0";
    /* 72*/ case JvmOp::dstore_1: return "Store double into local variable 1";
    /* 73*/ case JvmOp::dstore_2: return "Store double into local variable 2";
    /* 74*/ case JvmOp::dstore_3: return "Store double into local variable 3";
    /* 75*/ case JvmOp::astore_0: return "Store reference into local variable 0";
    /* 76*/ case JvmOp::astore_1: return "Store reference into local variable 1";
    /* 77*/ case JvmOp::astore_2: return "Store reference into local variable 2";
    /* 78*/ case JvmOp::astore_3: return "Store reference into local variable 3";
    /* 79*/ case JvmOp::iastore: return "Store into int array";
    /* 80*/ case JvmOp::lastore: return "Store into long array";
    /* 81*/ case JvmOp::fastore: return "Store into float array";
    /* 82*/ case JvmOp::dastore: return "Store into double array";
    /* 83*/ case JvmOp::aastore: return "Store into reference array";
    /* 84*/ case JvmOp::bastore: return "Store into byte or boolean array";
    /* 85*/ case JvmOp::castore: return "Store into char array";
    /* 86*/ case JvmOp::sastore: return "Store into short array";
    /* 87*/ case JvmOp::pop:     return "Pop the top operand stack value";
    /* 88*/ case JvmOp::pop2:    return "Pop the top one or two operand stack values";
    /* 89*/ case JvmOp::dup:     return "Duplicate the top operand stack value";
    /* 90*/ case JvmOp::dup_x1:  return "Duplicate the top operand stack value and insert two values down";
    /* 91*/ case JvmOp::dup_x2:  return "Duplicate the top operand stack value and insert two or three values down";
    /* 92*/ case JvmOp::dup2:    return "Duplicate the top one or two operand stack values";
    /* 93*/ case JvmOp::dup2_x1: return "Duplicate the top one or two operand stack values and insert two or three values down";
    /* 94*/ case JvmOp::dup2_x2: return "Duplicate the top one or two operand stack values and insert two, three, or four values down";
    /* 95*/ case JvmOp::swap: return "Swap the top two operand stack values";
    /* 96*/ case JvmOp::iadd: return "Add int";
    /* 97*/ case JvmOp::ladd: return "Add long";
    /* 98*/ case JvmOp::fadd: return "Add float";
    /* 99*/ case JvmOp::dadd: return "Add double";
    /*100*/ case JvmOp::isub: return "Subtract int";
    /*101*/ case JvmOp::lsub: return "Subtract long";
    /*102*/ case JvmOp::fsub: return "Subtract float";
    /*103*/ case JvmOp::dsub: return "Subtract double";
    /*104*/ case JvmOp::imul: return "Multiply int";
    /*105*/ case JvmOp::lmul: return "Multiply long";
    /*106*/ case JvmOp::fmul: return "Multiply float";
    /*107*/ case JvmOp::dmul: return "Multiply double";
    /*108*/ case JvmOp::idiv: return "Divide int";
    /*109*/ case JvmOp::ldiv: return "Divide long";
    /*110*/ case JvmOp::fdiv: return "Divide float";
    /*111*/ case JvmOp::ddiv: return "Divide double";
    /*112*/ case JvmOp::irem: return "Remainder int";
    /*113*/ case JvmOp::lrem: return "Remainder long";
    /*114*/ case JvmOp::frem: return "Remainder float";
    /*115*/ case JvmOp::drem: return "Remainder double";
    /*116*/ case JvmOp::ineg: return "Negate int";
    /*117*/ case JvmOp::lneg: return "Negate long";
    /*118*/ case JvmOp::fneg: return "Negate float";
    /*119*/ case JvmOp::dneg: return "Negate double";
    /*120*/ case JvmOp::ishl: return "Shift left int";
    /*121*/ case JvmOp::lshl: return "Shift left long";
    /*122*/ case JvmOp::ishr: return "Shift right int";
    /*123*/ case JvmOp::lshr: return "Shift right long";
    /*124*/ case JvmOp::iushr: return "Logical shift right int";
    /*125*/ case JvmOp::lushr: return "Logical shift right long";
    /*126*/ case JvmOp::iand: return "Boolean AND int";
    /*127*/ case JvmOp::land: return "Boolean AND long";
    /*128*/ case JvmOp::ior:  return "Boolean OR int";
    /*129*/ case JvmOp::lor:  return "Boolean OR long";
    /*130*/ case JvmOp::ixor: return "Boolean XOR int";
    /*131*/ case JvmOp::lxor: return "Boolean XOR long";
    /*132*/ case JvmOp::iinc: return "Increment local variable";
    /*133*/ case JvmOp::i2l: return "Convert int to long";
    /*134*/ case JvmOp::i2f: return "Convert int to float";
    /*135*/ case JvmOp::i2d: return "Convert int to double";
    /*136*/ case JvmOp::l2i: return "Convert long to int";
    /*137*/ case JvmOp::l2f: return "Convert long to float";
    /*138*/ case JvmOp::l2d: return "Convert long to double";
    /*139*/ case JvmOp::f2i: return "Convert float to int";
    /*140*/ case JvmOp::f2l: return "Convert float to long";
    /*141*/ case JvmOp::f2d: return "Convert float to double";
    /*142*/ case JvmOp::d2i: return "Convert double to int";
    /*143*/ case JvmOp::d2l: return "Convert double to long";
    /*144*/ case JvmOp::d2f: return "Convert double to float";
    /*145*/ case JvmOp::i2b: return "Convert int to byte";
    /*146*/ case JvmOp::i2c: return "Convert int to char";
    /*147*/ case JvmOp::i2s: return "Convert int to short";
    /*148*/ case JvmOp::lcmp:  return "Compare long";
    /*149*/ case JvmOp::fcmpl: return "Compare float";
    /*150*/ case JvmOp::fcmpg: return "Compare float";
    /*151*/ case JvmOp::dcmpl: return "Compare double";
    /*152*/ case JvmOp::dcmpg: return "Compare double";
    /*153*/ case JvmOp::ifeq: return "Branch if int 'eq' comparison with zero succeeds";
    /*154*/ case JvmOp::ifne: return "Branch if int 'ne' comparison with zero succeeds";
    /*155*/ case JvmOp::iflt: return "Branch if int 'lt' comparison with zero succeeds";
    /*156*/ case JvmOp::ifge: return "Branch if int 'ge' comparison with zero succeeds";
    /*157*/ case JvmOp::ifgt: return "Branch if int 'gt' comparison with zero succeeds";
    /*158*/ case JvmOp::ifle: return "Branch if int 'le' comparison with zero succeeds";
    /*159*/ case JvmOp::if_icmpeq: return "Branch if int 'eq' comparison succeeds";
    /*160*/ case JvmOp::if_icmpne: return "Branch if int 'ne' comparison succeeds";
    /*161*/ case JvmOp::if_icmplt: return "Branch if int 'lt' comparison succeeds";
    /*162*/ case JvmOp::if_icmpge: return "Branch if int 'ge' comparison succeeds";
    /*163*/ case JvmOp::if_icmpgt: return "Branch if int 'gt' comparison succeeds";
    /*164*/ case JvmOp::if_icmple: return "Branch if int 'le' comparison succeeds";
    /*165*/ case JvmOp::if_acmpeq: return "Branch if reference 'eq' comparison succeeds";
    /*166*/ case JvmOp::if_acmpne: return "Branch if reference 'ne' comparison succeeds";
    /*167*/ case JvmOp::goto_: return "Branch always";
    /*168*/ case JvmOp::jsr:   return "Jump subroutine";
    /*169*/ case JvmOp::ret:   return "Return from subroutine";
    /*170*/ case JvmOp::tableswitch:  return "Access jump table by index and jump";
    /*171*/ case JvmOp::lookupswitch: return "Access jump table by key match and jump";
    /*172*/ case JvmOp::ireturn: return "Return int from method";
    /*173*/ case JvmOp::lreturn: return "Return long from method";
    /*174*/ case JvmOp::freturn: return "Return float from method";
    /*175*/ case JvmOp::dreturn: return "Return double from method";
    /*176*/ case JvmOp::areturn: return "Return reference from method";
    /*177*/ case JvmOp::return_: return "Return void from method";
    /*178*/ case JvmOp::getstatic: return "Get static field from class";
    /*179*/ case JvmOp::putstatic: return "Set static field in class";
    /*180*/ case JvmOp::getfield:  return "Fetch field from object";
    /*181*/ case JvmOp::putfield:  return "Set field in object";
    /*182*/ case JvmOp::invokevirtual:   return "Invoke instance method; dispatch based on class";
    /*183*/ case JvmOp::invokespecial:   return "Invoke instance method; direct invocation...";
    /*184*/ case JvmOp::invokestatic:    return "Invoke a class (static) method";
    /*185*/ case JvmOp::invokeinterface: return "Invoke interface method";
    /*186*/ case JvmOp::invokedynamic:   return "Invoke a dynamically-computed call site";
    /*187*/ case JvmOp::new_:        return "Create new object";
    /*188*/ case JvmOp::newarray:    return "Create new array";
    /*189*/ case JvmOp::anewarray:   return "Create new array of reference";
    /*190*/ case JvmOp::arraylength: return "Get length of array";
    /*191*/ case JvmOp::athrow:      return "Throw exception or error";
    /*192*/ case JvmOp::checkcast:   return "Check whether object is of given type";
    /*193*/ case JvmOp::instanceof:  return "Determine if object is of given type";
    /*194*/ case JvmOp::monitorenter: return "Enter monitor for object";
    /*195*/ case JvmOp::monitorexit:  return "Exit monitor for object";
    /*196*/ case JvmOp::wide: return "Extend local variable index by additional bytes";
    /*197*/ case JvmOp::multianewarray: return "Create new multidimensional array";
    /*198*/ case JvmOp::ifnull:    return "Branch if reference is null";
    /*199*/ case JvmOp::ifnonnull: return "Branch if reference not null";
    /*200*/ case JvmOp::goto_w: return "Branch always (wide index)";
    /*201*/ case JvmOp::jsr_w:  return "Jump subroutine (wide index)";

    /* 6.2 Reserved Opcodes (should not be encountered in valid class file) */
    /*202*/ case JvmOp::breakpoint: return "Breakpoint (reserved opcode)";
    /*254*/ case JvmOp::impdep1: return "Implementation dependent 1 (reserved opcode)";
    /*255*/ case JvmOp::impdep2: return "Implementation dependent 2 (reserved opcode)";

    /*666*/ case JvmOp::unknown: return "Unknown opcode";
  }
  ASSERT_not_reachable("invalid JVM instruction kind: " + std::to_string(static_cast<int>(get_kind())));
}

Sawyer::Optional<rose_addr_t>
SgAsmJvmInstruction::branchTarget() {
  switch (get_kind()) {
    case JvmOp::ifeq: // "Branch if int 'eq' comparison with zero succeeds"
    case JvmOp::ifne: // "Branch if int 'ne' comparison with zero succeeds";
    case JvmOp::iflt: // "Branch if int 'lt' comparison with zero succeeds";
    case JvmOp::ifge: // "Branch if int 'ge' comparison with zero succeeds";
    case JvmOp::ifgt: // "Branch if int 'gt' comparison with zero succeeds";
    case JvmOp::ifle: // "Branch if int 'le' comparison with zero succeeds";
    case JvmOp::if_icmpeq: // "Branch if int 'eq' comparison succeeds";
    case JvmOp::if_icmpne: // "Branch if int 'ne' comparison succeeds";
    case JvmOp::if_icmplt: // "Branch if int 'lt' comparison succeeds";
    case JvmOp::if_icmpge: // "Branch if int 'ge' comparison succeeds";
    case JvmOp::if_icmpgt: // "Branch if int 'gt' comparison succeeds";
    case JvmOp::if_icmple: // "Branch if int 'le' comparison succeeds";
    case JvmOp::if_acmpeq: // "Branch if reference 'eq' comparison succeeds";
    case JvmOp::if_acmpne: // "Branch if reference 'ne' comparison succeeds";
    case JvmOp::ifnull:    // "Branch if reference is null";
    case JvmOp::ifnonnull: // "Branch if reference not null";
    case JvmOp::goto_:     // "Branch always";
    case JvmOp::goto_w:    // "Branch always (wide index)";
    case JvmOp::jsr:       // "Jump subroutine";
    case JvmOp::jsr_w:     // "Jump subroutine (wide index)";
      break;

 // A branch instruction but branch target (an offset) is not available
    case JvmOp::invokevirtual:   // "Invoke instance method; dispatch based on class";
    case JvmOp::invokespecial:   // "Invoke instance method; direct invocation...";
    case JvmOp::invokestatic:    // "Invoke a class (static) method";
    case JvmOp::invokeinterface: // "Invoke interface method";
    case JvmOp::invokedynamic:   // "Invoke a dynamically-computed call site";
    case JvmOp::athrow:       // "Throw exception or error";
    case JvmOp::monitorenter: // "Enter monitor for object";
    case JvmOp::monitorexit:  // "Exit monitor for object";
      return Sawyer::Nothing();

 // A branch instruction but branch target(s) (offsets) need to be calculated
    case JvmOp::tableswitch:  // "Access jump table by index and jump";
    case JvmOp::lookupswitch: // "Access jump table by key match and jump";
      return Sawyer::Nothing();

 // Not a branching instruction
    default:
      return Sawyer::Nothing();
  }

  if (nOperands() == 1) {
    if (SgAsmIntegerValueExpression *ival = isSgAsmIntegerValueExpression(operand(0))) {
      return get_address() + ival->get_signedValue();
    }
  }
  return Sawyer::Nothing();
}

#endif
