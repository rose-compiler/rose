/* SgAsmJvmInstruction member definitions */
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"

#define DEBUG_PRINT 0

using namespace Rose;                                   // temporary until this lives in "rose"
using namespace Rose::BinaryAnalysis;
using JvmOp = JvmInstructionKind;

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
      for (size_t n{3}; n < nOperands; n++) {
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

 // A branch instruction but branch target is not immediately available
    case JvmOp::invokevirtual:   // "Invoke instance method; dispatch based on class";
    case JvmOp::invokespecial:   // "Invoke instance method; direct invocation...";
    case JvmOp::invokestatic:    // "Invoke a class (static) method";
    case JvmOp::invokeinterface: // "Invoke interface method";
    case JvmOp::invokedynamic:   // "Invoke a dynamically-computed call site";
    case JvmOp::monitorenter: // "Enter monitor for object";
    case JvmOp::monitorexit:  // "Exit monitor for object";
      // Don't assume a CALL instruction returns to the fall-through address, but
      // by default, EngineJvm reverses this with partitioner->autoAddCallReturnEdges(true)
      return AddressSet{};

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
