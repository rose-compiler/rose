#include "rose.h"
#include "x86InstructionSemantics.h"
#include "x86AssemblyToBtor.h"
#include "integerOps.h"
#include <cassert>
#include <cstdio>
#include <boost/static_assert.hpp>
#include "btorProblem.h"
#include "x86AssemblyToNetlist.h" // Mostly for BMCError and numBmcErrors

using namespace std;
using namespace IntegerOps;
using boost::array;

std::string btorTranslate(BtorTranslationPolicy& policy, SgProject* proj, FILE* outfile, bool initialConditionsAreUnknown, bool bogusIpIsError) {
  std::vector<SgNode*> headers = NodeQuery::querySubTree(proj, V_SgAsmGenericHeader);
  ROSE_ASSERT (headers.size() == 1);
  SgAsmGenericHeader* header = isSgAsmGenericHeader(headers[0]);
  rose_addr_t entryPoint = header->get_entry_rva() + header->get_base_va();
  X86InstructionSemantics<BtorTranslationPolicy, BtorWordType> t(policy);
  std::vector<SgNode*> instructions = NodeQuery::querySubTree(proj, V_SgAsmx86Instruction);
  for (size_t i = 0; i < instructions.size(); ++i) {
    SgAsmx86Instruction* insn = isSgAsmx86Instruction(instructions[i]);
    ROSE_ASSERT (insn);
    try {
        t.processInstruction(insn);
    } catch (const X86InstructionSemantics<BtorTranslationPolicy, BtorWordType>::Exception &e) {
        fprintf(stderr, "%s: %s\n", e.mesg.c_str(), unparseInstructionWithAddress(e.insn).c_str());
    }
  }
  policy.setInitialState(entryPoint, initialConditionsAreUnknown);
  // Add "bogus IP" error
  policy.newRegisterMap.errorFlag[bmc_error_bogus_ip] =
    policy.invert(policy.isValidIp);
  for (size_t i = 0; i < numBmcErrors; ++i) {
    if (i == bmc_error_bogus_ip && !bogusIpIsError) continue; // For testing
    policy.problem.computations.push_back(policy.problem.build_op_root(policy.problem.build_op_and(policy.errorsEnabled, policy.newRegisterMap.errorFlag[i])));
  }
  policy.addNexts();
  return policy.problem.unparse();
}

void BtorTranslationPolicy::makeRegMap(BTRegisterInfo& rm, const std::string& prefix) {
  for (size_t i = 0; i < 8; ++i) {
    rm.gprs[i] = problem.build_var(32, prefix + "e" + gprToString((X86GeneralPurposeRegister)i));
  }
  rm.ip = problem.build_var(32, prefix + "eip");
  for (size_t i = 0; i < 16; ++i) {
    rm.flags[i] = problem.build_var(1, prefix + flagToString((X86Flag)i));
  }
  for (size_t i = 0; i < numBmcErrors; ++i) {
    rm.errorFlag[i] = false_();
  }
  rm.memory = problem.build_array(8, 32, prefix + "memory");
}

void BtorTranslationPolicy::makeRegMapZero(BTRegisterInfo& rm) {
  for (size_t i = 0; i < 8; ++i) {
    rm.gprs[i] = number<32>(0xBEEF0000 + i);
  }
  rm.ip = number<32>(0xBEEF1111);
  for (size_t i = 0; i < 16; ++i) {
    rm.flags[i] = problem.build_op_zero(1);
  }
  for (size_t i = 0; i < numBmcErrors; ++i) {
    rm.errorFlag[i] = false_();
  }
  rm.memory = problem.build_array(8, 32, "initial_memory");
}

void BtorTranslationPolicy::addNext(Comp cur, Comp next) {
  problem.computations.push_back(problem.build_op_next(cur, next));
}

void BtorTranslationPolicy::addAnext(Comp cur, Comp next) {
  problem.computations.push_back(problem.build_op_anext(cur, next));
}

void BtorTranslationPolicy::addNexts() {
  for (size_t i = 0; i < 8; ++i) {
    addNext(origRegisterMap.gprs[i], newRegisterMap.gprs[i]);
  }
  addNext(origRegisterMap.ip, newRegisterMap.ip);
  for (size_t i = 0; i < 16; ++i) {
    addNext(origRegisterMap.flags[i], newRegisterMap.flags[i]);
  }
  addAnext(origRegisterMap.memory, newRegisterMap.memory);
}

void BtorTranslationPolicy::setInitialState(uint32_t entryPoint, bool initialConditionsAreUnknown) {
  registerMap = origRegisterMap;
  if (initialConditionsAreUnknown) {
    writeGPR(x86_gpr_ax, problem.build_var(32, "initial_eax"));
    writeGPR(x86_gpr_cx, problem.build_var(32, "initial_ecx"));
    writeGPR(x86_gpr_dx, problem.build_var(32, "initial_edx"));
    writeGPR(x86_gpr_bx, problem.build_var(32, "initial_ebx"));
    writeGPR(x86_gpr_sp, problem.build_var(32, "initial_esp"));
    writeGPR(x86_gpr_bp, problem.build_var(32, "initial_ebp"));
    writeGPR(x86_gpr_si, problem.build_var(32, "initial_esi"));
    writeGPR(x86_gpr_di, problem.build_var(32, "initial_edi"));
    assert (!validIPs.empty());
    Comp initialEip = number<32>(entryPoint);
    for (size_t i = 0; i < validIPs.size(); ++i) {
      initialEip = problem.build_op_cond(problem.build_var(1), number<32>(validIPs[i]), initialEip);
    }
    writeIP(initialEip);
    writeFlag(x86_flag_cf, problem.build_var(1, "initial_cf"));
    writeFlag(x86_flag_1, true_());
    writeFlag(x86_flag_pf, problem.build_var(1, "initial_pf"));
    writeFlag(x86_flag_3, false_());
    writeFlag(x86_flag_af, problem.build_var(1, "initial_af"));
    writeFlag(x86_flag_5, false_());
    writeFlag(x86_flag_zf, problem.build_var(1, "initial_zf"));
    writeFlag(x86_flag_sf, problem.build_var(1, "initial_sf"));
    writeFlag(x86_flag_tf, false_());
    writeFlag(x86_flag_if, true_());
    writeFlag(x86_flag_df, problem.build_var(1, "initial_df"));
    writeFlag(x86_flag_of, problem.build_var(1, "initial_of"));
    writeFlag(x86_flag_iopl0, false_());
    writeFlag(x86_flag_iopl1, false_());
    writeFlag(x86_flag_nt, false_());
    writeFlag(x86_flag_15, false_());
  } else {
    writeGPR(x86_gpr_ax, zero(32));
    writeGPR(x86_gpr_cx, zero(32));
    writeGPR(x86_gpr_dx, zero(32));
    writeGPR(x86_gpr_bx, zero(32));
    writeGPR(x86_gpr_sp, number<32>(0xBFFF0000U));
    writeGPR(x86_gpr_bp, zero(32));
    writeGPR(x86_gpr_si, zero(32));
    writeGPR(x86_gpr_di, zero(32));
    writeIP(number<32>(entryPoint));
    writeFlag(x86_flag_cf, false_());
    writeFlag(x86_flag_1, true_());
    writeFlag(x86_flag_pf, false_());
    writeFlag(x86_flag_3, false_());
    writeFlag(x86_flag_af, false_());
    writeFlag(x86_flag_5, false_());
    writeFlag(x86_flag_zf, false_());
    writeFlag(x86_flag_sf, false_());
    writeFlag(x86_flag_tf, false_());
    writeFlag(x86_flag_if, true_());
    writeFlag(x86_flag_df, false_());
    writeFlag(x86_flag_of, false_());
    writeFlag(x86_flag_iopl0, false_());
    writeFlag(x86_flag_iopl1, false_());
    writeFlag(x86_flag_nt, false_());
    writeFlag(x86_flag_15, false_());
  }
  writeBackReset();
}

BtorTranslationPolicy::BtorTranslationPolicy(BtorTranslationHooks* hooks, uint32_t minNumStepsToFindError, uint32_t maxNumStepsToFindError, SgProject* proj): problem(), hooks(hooks) {
  assert (minNumStepsToFindError >= 1); // Can't find an error on the first step
  assert (maxNumStepsToFindError < 0xFFFFFFFFU); // Prevent overflows
  assert (minNumStepsToFindError <= maxNumStepsToFindError || maxNumStepsToFindError == 0);
  makeRegMap(origRegisterMap, "");
  makeRegMapZero(newRegisterMap);
  isValidIp = false_();
  validIPs.clear();
  Comp stepCount = problem.build_var(32, "stepCount_saturating_at_" + boost::lexical_cast<std::string>(maxNumStepsToFindError + 1));
  addNext(stepCount, ite(problem.build_op_eq(stepCount, number<32>(maxNumStepsToFindError + 1)), number<32>(maxNumStepsToFindError + 1), problem.build_op_inc(stepCount)));
  resetState = problem.build_op_eq(stepCount, zero(32));
  errorsEnabled =
    problem.build_op_and(
      problem.build_op_ugte(stepCount, number<32>(minNumStepsToFindError)),
      (maxNumStepsToFindError == 0 ?
       true_() :
       problem.build_op_ulte(stepCount, number<32>(maxNumStepsToFindError))));
  {
    vector<SgNode*> functions = NodeQuery::querySubTree(proj, V_SgAsmFunctionDeclaration);
    for (size_t i = 0; i < functions.size(); ++i) {
      functionStarts.push_back(isSgAsmFunctionDeclaration(functions[i])->get_address());
      // fprintf(stderr, "functionStarts 0x%"PRIx64"\n", isSgAsmFunctionDeclaration(functions[i])->get_address());
    }
  }
  {
    vector<SgNode*> blocks = NodeQuery::querySubTree(proj, V_SgAsmBlock);
    for (size_t i = 0; i < blocks.size(); ++i) {
      SgAsmBlock* b = isSgAsmBlock(blocks[i]);
      if (!b->get_statementList().empty() && isSgAsmx86Instruction(b->get_statementList().front())) {
        blockStarts.push_back(b->get_address());
        // fprintf(stderr, "blockStarts 0x%"PRIx64"\n", b->get_address());
      }
    }
  }
  {
    vector<SgNode*> calls = NodeQuery::querySubTree(proj, V_SgAsmx86Instruction);
    for (size_t i = 0; i < calls.size(); ++i) {
      SgAsmx86Instruction* b = isSgAsmx86Instruction(calls[i]);
      if (b->get_kind() != x86_call) continue;
      returnPoints.push_back(b->get_address() + b->get_raw_bytes().size());
      // fprintf(stderr, "returnPoints 0x%"PRIx64"\n", b->get_address() + b->get_raw_bytes().size());
    }
  }
  {
    vector<SgNode*> instructions = NodeQuery::querySubTree(proj, V_SgAsmx86Instruction);
    for (size_t i = 0; i < instructions.size(); ++i) {
      SgAsmx86Instruction* b = isSgAsmx86Instruction(instructions[i]);
      validIPs.push_back(b->get_address());
    }
  }
}

BtorWordType<32> BtorTranslationPolicy::readGPR(X86GeneralPurposeRegister r) {
  return registerMap.gprs[r];
}

void BtorTranslationPolicy::writeGPR(X86GeneralPurposeRegister r, const BtorWordType<32>& value) {
  registerMap.gprs[r] = value;
}

BtorWordType<16> BtorTranslationPolicy::readSegreg(X86SegmentRegister sr) {
  return problem.build_constant(16, 0x2B); // FIXME
}

void BtorTranslationPolicy::writeSegreg(X86SegmentRegister sr, BtorWordType<16> val) {
  // FIXME
}

BtorWordType<32> BtorTranslationPolicy::readIP() {
  return registerMap.ip;
}

void BtorTranslationPolicy::writeIP(const BtorWordType<32>& newIp) {
  registerMap.ip = newIp;
}

BtorWordType<1> BtorTranslationPolicy::readFlag(X86Flag f) {
  return registerMap.flags[f];
}

void BtorTranslationPolicy::writeFlag(X86Flag f, const BtorWordType<1>& value) {
  registerMap.flags[f] = value;
}

  Comp BtorTranslationPolicy::concat(const Comp& a, const Comp& b) {
    return problem.build_op_concat(b, a); // Our concat puts a on the LSB side of b, while BTOR does the opposite
  }

  Comp BtorTranslationPolicy::extractVar(const Comp& a, size_t from, size_t to) {
    assert (from < to);
    assert (from <= a.bitWidth());
    assert (to <= a.bitWidth());
    return problem.build_op_slice(a, to - 1, from);
  }

  Comp BtorTranslationPolicy::signExtendVar(const Comp& a, size_t to) {
    size_t from = a.bitWidth();
    if (from == to) return a;
    return concat(
             a,
             ite(extractVar(a, from - 1, from),
                 ones(to - from),
                 zero(to - from)));
  }

  Comp BtorTranslationPolicy::zeroExtendVar(const Comp& a, size_t to) {
    size_t from = a.bitWidth();
    if (from == to) return a;
    return concat(
             a,
             zero(to - from));
  }

  Comp BtorTranslationPolicy::addWithCarries(const Comp& a, const Comp& b, const Comp& carryIn, Comp& carries) { // Full case
    assert (a.bitWidth() == b.bitWidth());
    assert (carryIn.bitWidth() == 1);
    uint len = a.bitWidth() + 1;
    Comp aFull = zeroExtendVar(a, len);
    Comp bFull = zeroExtendVar(b, len);
    Comp carryInFull = zeroExtendVar(carryIn, len);
    Comp sumFull = problem.build_op_add(problem.build_op_add(aFull, bFull), carryInFull);
    Comp sum = extractVar(sumFull, 0, a.bitWidth());
    Comp carriesFull = problem.build_op_xor(problem.build_op_xor(sumFull, aFull), bFull);
    carries = extractVar(carriesFull, 1, carriesFull.bitWidth());
    return sum;
  }

  BtorWordType<32> BtorTranslationPolicy::filterIndirectJumpTarget(const BtorWordType<32>& addr) {
    return blockStarts.empty() ? addr : limitToElements(addr, blockStarts);
  }

  BtorWordType<32> BtorTranslationPolicy::filterCallTarget(const BtorWordType<32>& addr) {
    return functionStarts.empty() ? addr : limitToElements(addr, functionStarts);
  }

  BtorWordType<32> BtorTranslationPolicy::filterReturnTarget(const BtorWordType<32>& addr) {
    // returnPoints seems not to work for some reason
    // return addr; // return returnPoints.empty() ? addr : limitToElements(addr, returnPoints);
    return returnPoints.empty() ? addr : limitToElements(addr, blockStarts);
  }

  BtorWordType<32> BtorTranslationPolicy::limitToElements(const BtorWordType<32>& elt, const vector<uint32_t>& ls) {
    // This is nasty, but seems to be needed
    ROSE_ASSERT (!ls.empty());
    Comp withinList = false_();
    for (size_t i = 0; i < ls.size(); ++i) {
      withinList = problem.build_op_or(withinList, problem.build_op_eq(elt, number<32>(ls[i])));
    }
    return problem.build_op_cond(withinList, elt, number<32>(ls[0]));
  }

  void BtorTranslationPolicy::hlt() {
    hooks->hlt(this);
  }
  void BtorTranslationPolicy::interrupt(uint8_t num) {} // FIXME
  void BtorTranslationPolicy::sysenter() {} // FIXME
  BtorWordType<64> BtorTranslationPolicy::rdtsc() {return problem.build_var(64, "timestamp");}

  void BtorTranslationPolicy::writeBackCond(Comp cond) {
    for (size_t i = 0; i < 8; ++i) {
      newRegisterMap.gprs[i] = ite(cond, registerMap.gprs[i], newRegisterMap.gprs[i]);
    }
    newRegisterMap.ip = ite(cond, registerMap.ip, newRegisterMap.ip);
    for (size_t i = 0; i < 16; ++i) {
      newRegisterMap.flags[i] = ite(cond, registerMap.flags[i], newRegisterMap.flags[i]);
    }
    for (size_t i = 0; i < numBmcErrors; ++i) {
      if (i == bmc_error_bogus_ip) continue; // Handled separately
      newRegisterMap.errorFlag[i] = ite(cond, registerMap.errorFlag[i], newRegisterMap.errorFlag[i]);
    }
    newRegisterMap.memory = problem.build_op_acond(cond, registerMap.memory, newRegisterMap.memory);
  }

  void BtorTranslationPolicy::writeBack(uint64_t addr) {
    Comp isThisIp = problem.build_op_and(problem.build_op_eq(origRegisterMap.ip, number<32>(addr)), invert(resetState));
    isValidIp = problem.build_op_or(isValidIp, isThisIp);
    writeBackCond(isThisIp);
  }

  void BtorTranslationPolicy::writeBackReset() {
    Comp cond = resetState;
    isValidIp = problem.build_op_or(isValidIp, cond);
    writeBackCond(cond);
  }

  void BtorTranslationPolicy::startBlock(uint64_t addr) {
  }

  void BtorTranslationPolicy::finishBlock(uint64_t addr) {
  }

  void BtorTranslationPolicy::startInstruction(SgAsmx86Instruction* insn) {
    // fprintf(stderr, "startInstruction(0x%"PRIx64")\n", insn->get_address());
    registerMap = origRegisterMap;
    hooks->startInstruction(this, insn); // Allow hook to modify initial conditions
    registerMap.ip = number<32>(0xDEADBEEF); // For debugging
  }

  void BtorTranslationPolicy::finishInstruction(SgAsmx86Instruction* insn) {
    // fprintf(stderr, "finishInstruction(0x%"PRIx64")\n", insn->get_address());
    hooks->finishInstruction(this, insn); // Allow hook to modify result
    writeBack(insn->get_address());
  }

