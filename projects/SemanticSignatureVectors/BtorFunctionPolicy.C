#include "rose.h"
#include "x86InstructionSemantics.h"
#include "x86AssemblyToBtor.h"
#include "integerOps.h"
#include <cassert>
#include <cstdio>
#include <boost/static_assert.hpp>
#include "btorProblem.h"
#include "x86AssemblyToNetlist.h" // Mostly for BMCError and numBmcErrors

#include "BtorFunctionPolicy.h"
using namespace std;
using namespace IntegerOps;
using boost::array;

std::string btorTranslate(BtorFunctionPolicy& policy, const std::vector<SgAsmx86Instruction*>& instructions) {

  X86InstructionSemantics<BtorFunctionPolicy, BtorWordType> t(policy);
  assert (!instructions.empty());
  policy.writeIP(policy.number<32>(instructions.front()->get_address()));
  for (size_t i = 0; i < instructions.size(); ++i) {
    SgAsmx86Instruction* insn = isSgAsmx86Instruction(instructions[i]);
    ROSE_ASSERT (insn);
    try {
        t.processInstruction(insn);
    } catch (const X86InstructionSemantics<BtorFunctionPolicy, BtorWordType>::Exception &e) {
        fprintf(stderr, "%s: %s\n", e.mesg.c_str(), unparseInstructionWithAddress(e.insn).c_str());
    }
  }
  // Name everything so it will be unparsed
  for (size_t i = 0; i < 8; ++i) policy.problem.nameValue(policy.registerMap.gprs[i], std::string("new_e") + gprToString((X86GeneralPurposeRegister)i));
  // policy.problem.nameValue(policy.registerMap.ip, "new_eip");
  // policy.problem.nameValue(policy.registerMap.flags[x86_flag_cf], "new_cf");
  // policy.problem.nameValue(policy.registerMap.flags[x86_flag_pf], "new_pf");
  // policy.problem.nameValue(policy.registerMap.flags[x86_flag_af], "new_af");
  // policy.problem.nameValue(policy.registerMap.flags[x86_flag_zf], "new_zf");
  // policy.problem.nameValue(policy.registerMap.flags[x86_flag_sf], "new_sf");
  // policy.problem.nameValue(policy.registerMap.flags[x86_flag_df], "new_df");
  // policy.problem.nameValue(policy.registerMap.flags[x86_flag_of], "new_of");
  policy.problem.nameValue(policy.registerMap.memory, "new_memory");
  return policy.problem.unparse();
}

void BtorFunctionPolicy::hlt(){};

void BtorFunctionPolicy::makeRegMap(BTRegisterInfo& rm, const std::string& prefix) {
  for (size_t i = 0; i < 8; ++i) {
    rm.gprs[i] = problem.build_var(32, prefix + "e" + gprToString((X86GeneralPurposeRegister)i));
  }
  rm.ip = problem.build_var(32, prefix + "eip");
  writeFlag(x86_flag_cf, problem.build_var(1, "cf"));
  writeFlag(x86_flag_1, true_());
  writeFlag(x86_flag_pf, problem.build_var(1, "pf"));
  writeFlag(x86_flag_3, false_());
  writeFlag(x86_flag_af, problem.build_var(1, "af"));
  writeFlag(x86_flag_5, false_());
  writeFlag(x86_flag_zf, problem.build_var(1, "zf"));
  writeFlag(x86_flag_sf, problem.build_var(1, "sf"));
  writeFlag(x86_flag_tf, false_());
  writeFlag(x86_flag_if, true_());
  writeFlag(x86_flag_df, problem.build_var(1, "df"));
  writeFlag(x86_flag_of, problem.build_var(1, "of"));
  writeFlag(x86_flag_iopl0, false_());
  writeFlag(x86_flag_iopl1, false_());
  writeFlag(x86_flag_nt, false_());
  writeFlag(x86_flag_15, false_());
  rm.memory = problem.build_array(8, 32, prefix + "memory");
}

BtorFunctionPolicy::BtorFunctionPolicy(): problem() {
//  Comp stepCount = problem.build_var(32, "stepCount_saturating_at_" + boost::lexical_cast<std::string>(maxNumStepsToFindError + 1));
  makeRegMap(registerMap, "");
}

BtorWordType<32> BtorFunctionPolicy::readGPR(X86GeneralPurposeRegister r) {
  return registerMap.gprs[r];
}

void BtorFunctionPolicy::writeGPR(X86GeneralPurposeRegister r, const BtorWordType<32>& value) {
  registerMap.gprs[r] = value;
}

BtorWordType<16> BtorFunctionPolicy::readSegreg(X86SegmentRegister sr) {
  return problem.build_constant(16, 0x2B); // FIXME
}

void BtorFunctionPolicy::writeSegreg(X86SegmentRegister sr, BtorWordType<16> val) {
  // FIXME
}

BtorWordType<32> BtorFunctionPolicy::readIP() {
  return registerMap.ip;
}

void BtorFunctionPolicy::writeIP(const BtorWordType<32>& newIp) {
  registerMap.ip = newIp;
}

BtorWordType<1> BtorFunctionPolicy::readFlag(X86Flag f) {
  return registerMap.flags[f];
}

void BtorFunctionPolicy::writeFlag(X86Flag f, const BtorWordType<1>& value) {
  registerMap.flags[f] = value;
}

  Comp BtorFunctionPolicy::concat(const Comp& a, const Comp& b) {
    return problem.build_op_concat(b, a); // Our concat puts a on the LSB side of b, while BTOR does the opposite
  }

  Comp BtorFunctionPolicy::extractVar(const Comp& a, size_t from, size_t to) {
    assert (from < to);
    assert (from <= a.bitWidth());
    assert (to <= a.bitWidth());
    return problem.build_op_slice(a, to - 1, from);
  }

  Comp BtorFunctionPolicy::signExtendVar(const Comp& a, size_t to) {
    size_t from = a.bitWidth();
    if (from == to) return a;
    return concat(
             a,
             ite(extractVar(a, from - 1, from),
                 ones(to - from),
                 zero(to - from)));
  }

  Comp BtorFunctionPolicy::zeroExtendVar(const Comp& a, size_t to) {
    size_t from = a.bitWidth();
    if (from == to) return a;
    return concat(
             a,
             zero(to - from));
  }

  Comp BtorFunctionPolicy::addWithCarries(const Comp& a, const Comp& b, const Comp& carryIn, Comp& carries) { // Full case
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

  void BtorFunctionPolicy::interrupt(uint8_t num) {} // FIXME
  void BtorFunctionPolicy::sysenter() {} // FIXME
  BtorWordType<64> BtorFunctionPolicy::rdtsc() {return problem.build_var(64, "timestamp");}

  void BtorFunctionPolicy::startBlock(uint64_t addr) {
  }

  void BtorFunctionPolicy::finishBlock(uint64_t addr) {
  }

  void BtorFunctionPolicy::startInstruction(SgAsmx86Instruction* insn) {
    // fprintf(stderr, "startInstruction(0x%"PRIx64")\n", insn->get_address());
  }

  void BtorFunctionPolicy::finishInstruction(SgAsmx86Instruction* insn) {
    // fprintf(stderr, "finishInstruction(0x%"PRIx64")\n", insn->get_address());
  }

