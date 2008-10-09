#include "x86InstructionSemantics.h"
#include <cassert>
#include <cstdio>
#include "satProblem.h"

using namespace std;
using boost::array;

struct MemoryAccess {
  LitList(32) addr;
  LitList(8) data;
  Lit cond;
  MemoryAccess(LitList(32) addr, LitList(8) data, Lit cond): addr(addr), data(data), cond(cond) {}
};

template <size_t Len, size_t Offset, typename NLTranslator>
struct WriteMemoryHelper {
  static void go(X86SegmentRegister segreg, const LitList(32)& addr, const LitList(Len)& data, Lit cond, NLTranslator& trans) {
    trans.writeMemoryByte(segreg, (Offset == 0 ? addr : trans.problem.adder(addr, number<32>(Offset / 8))), extract<Offset, Offset + 8>(data), cond);
    WriteMemoryHelper<Len, Offset + 8, NLTranslator>::go(segreg, addr, data, cond, trans);
  }
};

template <size_t Len, typename NLTranslator>
struct WriteMemoryHelper<Len, Len, NLTranslator> {
  static void go(X86SegmentRegister segreg, const LitList(32)& addr, const LitList(Len)& data, Lit cond, NLTranslator& trans) {}
};

struct RegisterInfo {
  LitList(32) gprs[8];
  LitList(32) ip;
  Lit flags[16];
  Lit errorFlag;
};

struct NetlistTranslationPolicy {
  template <size_t Len>
  struct wordType {typedef LitList(Len) type;};

  RegisterInfo registerMap;
  RegisterInfo newRegisterMap;
  RegisterInfo origRegisterMap;
  vector<vector<MemoryAccess> > memoryWrites; // Outer vector is sequence of writes, inner vector is the set of mutually exclusive writes at that position
  vector<vector<MemoryAccess> > memoryReads; // Outer vector is sequence of reads, inner vector is the set of mutually exclusive reads at that position
  Lit isThisIp;
  size_t currentMemoryIndex;
  SatProblem problem;

  void makeRegMaps(RegisterInfo& rm, const std::string& prefix) {
    for (int r = 0; r < 8; ++r) {
      LitList(32) thisReg = problem.newVars<32>();
      problem.addInterface(prefix + "_gpr" + boost::lexical_cast<std::string>(r), toVector(thisReg));
      rm.gprs[r] = thisReg;
    }
    LitList(32) ipReg = problem.newVars<32>();
    problem.addInterface(prefix + "_ip", toVector(ipReg));
    rm.ip = ipReg;
    LitList(7) flagsReg = problem.newVars<7>();
    problem.addInterface(prefix + "_flags", toVector(flagsReg));
    rm.flags[x86_flag_cf] = flagsReg[0];
    rm.flags[x86_flag_pf] = flagsReg[1];
    rm.flags[x86_flag_af] = flagsReg[2];
    rm.flags[x86_flag_zf] = flagsReg[3];
    rm.flags[x86_flag_sf] = flagsReg[4];
    rm.flags[x86_flag_df] = flagsReg[5];
    rm.flags[x86_flag_of] = flagsReg[6];
    rm.flags[x86_flag_1] = TRUE;
    rm.flags[x86_flag_3] = FALSE;
    rm.flags[x86_flag_5] = FALSE;
    rm.flags[x86_flag_tf] = FALSE;
    rm.flags[x86_flag_if] = TRUE;
    rm.flags[x86_flag_iopl0] = FALSE;
    rm.flags[x86_flag_iopl1] = FALSE;
    rm.flags[x86_flag_nt] = FALSE;
    rm.flags[x86_flag_15] = FALSE;
    rm.errorFlag = problem.newVar();
    problem.addInterface(prefix + "_error", toVector(rm.errorFlag));
  }

  NetlistTranslationPolicy(): isThisIp(FALSE), currentMemoryIndex(0), problem() {
    makeRegMaps(newRegisterMap, "out");
  }

  LitList(32) readGPR(X86GeneralPurposeRegister r) {
    return registerMap.gprs[r];
  }

  void writeGPR(X86GeneralPurposeRegister r, const LitList(32)& value) {
    registerMap.gprs[r] = value;
  }

  LitList(16) readSegreg(X86SegmentRegister sr) {
    return ::number<16>(0x2B); // FIXME
  }

  void writeSegreg(X86SegmentRegister sr, LitList(16) val) {
    // FIXME
  }

  LitList(32) readIP() {
    return registerMap.ip;
  }

  void writeIP(const LitList(32)& newIp) {
    registerMap.ip = newIp;
  }

  LitList(1) readFlag(X86Flag f) {
    return single(registerMap.flags[f]);
  }

  void writeFlag(X86Flag f, const LitList(1)& value) {
    registerMap.flags[f] = value[0];
  }

  template <size_t Len>
  LitList(Len) number(uint64_t n) {
    return ::number<Len>(n);
  }

  template <size_t Len1, size_t Len2>
  LitList(Len1 + Len2) concat(const LitList(Len1)& a, const LitList(Len2)& b) {
    return ::concat(a, b);
  }

  template <size_t From, size_t To, size_t Len>
  LitList(To - From) extract(const LitList(Len)& a) {
    return ::extract<From, To>(a);
  }

  LitList(1) false_() {return single(FALSE);}
  LitList(1) true_() {return single(TRUE);}

  template <size_t Len>
  LitList(Len) invert(const LitList(Len)& a) {
    return ::invertWord(a);
  }

  template <size_t Len>
  LitList(Len) and_(const LitList(Len)& a, const LitList(Len)& b) {
    return problem.andWords(a, b);
  }

  template <size_t Len>
  LitList(Len) or_(const LitList(Len)& a, const LitList(Len)& b) {
    return problem.orWords(a, b);
  }

  template <size_t Len>
  LitList(Len) xor_(const LitList(Len)& a, const LitList(Len)& b) {
    return problem.xorWords(a, b);
  }

  template <size_t From, size_t To>
  LitList(To) signExtend(const LitList(From)& a) {
    LitList(To) result;
    for (size_t i = 0; i < To; ++i) {
      result[i] = (i >= From) ? a[From - 1] : a[i];
    }
    return result;
  }

  template <size_t Len>
  LitList(Len) ite(const LitList(1)& sel, const LitList(Len)& ifTrue, const LitList(Len)& ifFalse) {
    return problem.ite(sel[0], ifTrue, ifFalse);
  }

  template <size_t Len>
  LitList(1) equalToZero(const LitList(Len)& a) {
    return single(problem.norAcross(a));
  }

  template <size_t Len, size_t SCLen>
  LitList(Len) generateMask(LitList(SCLen) w) {
    LitList(Len) result;
    assert (Len != 0);
    result[0] = problem.equal(w, ::number<SCLen>(1));
    for (size_t i = 1; i < Len; ++i) {
      result[i] = problem.orGate(result[i - 1], problem.equal(w, ::number<SCLen>(i + 1)));
    }
    return result;
  }

  template <size_t Len>
  LitList(Len) add(const LitList(Len)& a, const LitList(Len)& b) { // Simple case
    return ::extract<0, Len>(problem.adder(a, b));
  }

  template <size_t Len>
  LitList(Len) addWithCarries(const LitList(Len)& a, const LitList(Len)& b, const LitList(1)& carryIn, LitList(Len)& carries) { // Full case
    return ::extract<0, Len>(problem.adder(a, b, carryIn[0], &carries));
  }

  template <size_t Len, size_t SALen>
  LitList(Len) rotateLeft(const LitList(Len)& a, const LitList(SALen)& cnt) {
    return problem.rightRotater(a, cnt); // Flipped because words are LSB first
  }

  template <size_t Len1, size_t Len2>
  LitList(Len1 + Len2) signedMultiply(const LitList(Len1)& a, const LitList(Len2)& b) {
    return problem.signedMultiplier(a, b);
  }

  template <size_t Len1, size_t Len2>
  LitList(Len1 + Len2) unsignedMultiply(const LitList(Len1)& a, const LitList(Len2)& b) {
    return problem.unsignedMultiplier(a, b);
  }

  template <size_t Len1, size_t Len2>
  LitList(Len1) signedDivide(const LitList(Len1)& a, const LitList(Len2)& b) {
    return extract<0, Len1>(problem.signedDivider(a, b));
  }

  template <size_t Len1, size_t Len2>
  LitList(Len2) signedModulo(const LitList(Len1)& a, const LitList(Len2)& b) {
    return extract<Len1, Len1 + Len2>(problem.signedDivider(a, b));
  }

  template <size_t Len1, size_t Len2>
  LitList(Len1) unsignedDivide(const LitList(Len1)& a, const LitList(Len2)& b) {
    return extract<0, Len1>(problem.unsignedDivider(a, b));
  }

  template <size_t Len1, size_t Len2>
  LitList(Len2) unsignedModulo(const LitList(Len1)& a, const LitList(Len2)& b) {
    return extract<Len1, Len1 + Len2>(problem.unsignedDivider(a, b));
  }

  template <size_t Len>
  LitList(Len) leastSignificantSetBit(const LitList(Len)& in) {
    return problem.leastSignificantSetBit(in);
  }

  template <size_t Len>
  LitList(Len) mostSignificantSetBit(const LitList(Len)& in) {
    return problem.mostSignificantSetBit(in);
  }

  template <size_t Len> // In bits
  LitList(Len) readMemory(X86SegmentRegister segreg, const LitList(32)& addr, LitList(1) cond) {
    LitList(Len) result;
    for (size_t i = 0; i < Len / 8; ++i) {
      LitList(8) thisByte = readMemoryByte(segreg, problem.adder(addr, number<32>(i)), cond[0]);
      for (size_t j = 0; j < 8; ++j) {
        result[i * 8 + j] = thisByte[j];
      }
    }
    return result;
  }

  LitList(8) readMemoryByte(X86SegmentRegister segreg, const LitList(32)& addr, Lit cond) {
#if 0
    // The priority order for reads goes from bottom to top.  First, we check
    // for any writes that have the same address, and use the most recent of
    // those that match.  Next, we look at those previous reads that did not
    // match a write (i.e., use the values from the beginning of the basic
    // block), using any with the same address, or an arbitrary value if there
    // are no matches.  The point of the memory read list is that we want two
    // memory reads with the same address to return the same result, even if we
    // don't have a definition of what that value actually is.
    LitList(8) result = problem.newVars<8>();
    for (size_t i = 0; i < initialMemoryReads.size(); ++i) {
      Lit thisEquality = problem.andGate(initialMemoryReads[i].cond, problem.equal(addr, initialMemoryReads[i].addr));
      for (size_t j = 0; j < 8; ++j) {
        problem.condEquivalence(thisEquality, initialMemoryReads[i].data[j], result[j]);
      }
    }
    for (size_t i = 0; i < memoryWrites.size(); ++i) {
      result = problem.ite(problem.andGate(memoryWrites[i].cond, problem.equal(addr, memoryWrites[i].addr)), memoryWrites[i].data, result);
    }
    initialMemoryReads.push_back(MemoryAccess(addr, result, cond)); // If address doesn't alias any previous reads or writes
    problem.addInterface("memoryRead_" + boost::lexical_cast<std::string>(memoryReadCountBase), toVector(concat(addr, result)));
    ++memoryReadCountBase;
    return result;
#endif
    // fprintf(stderr, "read(memory%zu, addr, cond)\n", currentMemoryIndex);
    LitList(8) data = problem.newVars<8>();
    MemoryAccess ma(addr, data, problem.andGate(cond, isThisIp));
    if (memoryReads.size() <= currentMemoryIndex) {
      memoryReads.resize(currentMemoryIndex + 1);
    }
    memoryReads[currentMemoryIndex].push_back(ma);
    ++currentMemoryIndex;
    return data;
  }

  void writeMemoryByte(X86SegmentRegister segreg, const LitList(32)& addr, const LitList(8)& data, Lit cond) {
    // fprintf(stderr, "memory%zu = write(memory%zu, addr, data, cond)\n", memoryIndexCounter, currentMemoryIndex);
    MemoryAccess ma(addr, data, problem.andGate(cond, isThisIp));
    if (memoryWrites.size() <= currentMemoryIndex) {
      memoryWrites.resize(currentMemoryIndex + 1);
    }
    memoryWrites[currentMemoryIndex].push_back(ma);
    ++currentMemoryIndex;
  }

  template <size_t Len>
  void writeMemory(X86SegmentRegister segreg, const LitList(32)& addr, const LitList(Len)& data, LitList(1) cond) {
    WriteMemoryHelper<Len, 0, NetlistTranslationPolicy>::go(segreg, addr, data, cond[0], *this);
  }

  void hlt() {registerMap.errorFlag = TRUE;} // FIXME
  void interrupt(uint8_t num) {} // FIXME
  LitList(64) rdtsc() {return problem.newVars<64>();}

  void writeBack() {
    fprintf(stderr, "Have %zu variables and %zu clauses so far\n", problem.numVariables, problem.clauses.size());
    for (size_t i = 0; i < 8; ++i) {
      problem.condEquivalenceWords(isThisIp, registerMap.gprs[i], newRegisterMap.gprs[i]);
    }
    problem.condEquivalenceWords(isThisIp, registerMap.ip, newRegisterMap.ip);
    for (size_t i = 0; i < 16; ++i) {
      problem.condEquivalence(isThisIp, registerMap.flags[i], newRegisterMap.flags[i]);
    }
    problem.condEquivalence(isThisIp, registerMap.errorFlag, newRegisterMap.errorFlag);
    fprintf(stderr, "Have %zu variables and %zu clauses so far\n", problem.numVariables, problem.clauses.size());
  }

  void startBlock(uint64_t addr) {
    isThisIp = problem.equal(origRegisterMap.ip, ::number<32>(addr));
    registerMap = origRegisterMap;
    currentMemoryIndex = 0;
    fprintf(stderr, "Block 0x%08X, isThisIp = %d\n", (unsigned int)addr, isThisIp);
  }

  void finishBlock(uint64_t addr) {
    writeBack();
    isThisIp = FALSE;
  }

  void startInstruction(SgAsmInstruction*) {}
  void finishInstruction(SgAsmInstruction*) {}

};

int main(int argc, char** argv) {
  SgProject* proj = frontend(argc, argv);
  FILE* f = fopen("foo.dimacs", "w");
  assert (f);
  NetlistTranslationPolicy policy;
  X86InstructionSemantics<NetlistTranslationPolicy> t(policy);
  policy.makeRegMaps(policy.origRegisterMap, "in");
  vector<SgNode*> blocks = NodeQuery::querySubTree(proj, V_SgAsmBlock);
  for (size_t i = 0; i < blocks.size(); ++i) {
    SgAsmBlock* b = isSgAsmBlock(blocks[i]);
    ROSE_ASSERT (b);
    t.processBlock(b);
  }
  size_t memoryUseCount = policy.memoryWrites.size();
  if (memoryUseCount < policy.memoryReads.size()) {
    memoryUseCount = policy.memoryReads.size();
  }
  policy.memoryReads.resize(memoryUseCount);
  policy.memoryWrites.resize(memoryUseCount);
  vector<MemoryAccess> totalWrites;
  size_t actualMemoryWriteCount = 0, actualMemoryReadCount = 0;
  for (size_t i = 0; i < memoryUseCount; ++i) {
    LitList(32) totalAddress = policy.problem.newVars<32>();
    LitList(8) totalData = policy.problem.newVars<8>();
    Lit totalCond = FALSE;
    for (size_t j = 0; j < policy.memoryWrites[i].size(); ++j) {
      Lit thisCond = policy.memoryWrites[i][j].cond;
      policy.problem.condEquivalenceWords(thisCond, policy.memoryWrites[i][j].addr, totalAddress);
      policy.problem.condEquivalenceWords(thisCond, policy.memoryWrites[i][j].data, totalData);
      totalCond = policy.problem.orGate(totalCond, thisCond);
    }
    totalWrites.push_back(MemoryAccess(totalAddress, totalData, totalCond));
    if (totalCond == FALSE) continue; // We need to keep cond=0 writes in totalWrites because indices in there must match up with those in memoryReads
    policy.problem.addInterface("memoryWrite_" + boost::lexical_cast<string>(actualMemoryWriteCount++), toVector(concat(single(totalCond), concat(totalAddress, totalData))));
  }
  for (size_t i = 0; i < memoryUseCount; ++i) {
    LitList(32) totalAddress = policy.problem.newVars<32>();
    LitList(8) totalData = policy.problem.newVars<8>();
    Lit totalCond = FALSE;
    for (size_t j = 0; j < policy.memoryReads[i].size(); ++j) {
      Lit thisCond = policy.memoryReads[i][j].cond;
      Lit newTotalCond = policy.problem.orGate(totalCond, thisCond);
      // fprintf(stderr, "%d | %d -> %d\n", totalCond, thisCond, newTotalCond);
      policy.problem.condEquivalenceWords(thisCond, policy.memoryReads[i][j].addr, totalAddress);
      policy.problem.condEquivalenceWords(thisCond, policy.memoryReads[i][j].data, totalData);
      totalCond = newTotalCond;
    }
    // Try to match with earlier writes
    // Note that the loop is from earlier to later so that later writes will be
    // preferred
    if (totalCond == FALSE) continue;
    LitList(8) matchedData = policy.problem.newVars<8>();
    LitList(8) origMatchedData = matchedData;
    for (size_t j = 0; j < i; ++j) {
      Lit match = policy.problem.andGate(policy.problem.andGate(totalCond, totalWrites[j].cond), policy.problem.equal(totalAddress, totalWrites[j].addr));
      matchedData = policy.problem.ite(match, totalWrites[j].data, matchedData);
    }
    for (size_t j = 0; j < 8; ++j) {
      policy.problem.identify(matchedData[j], totalData[j]);
    }
    policy.problem.addInterface("memoryRead_" + boost::lexical_cast<string>(actualMemoryReadCount++), toVector(concat(single(totalCond), concat(totalAddress, origMatchedData)))); // Use the original value as this interface should only cover writes before the current BB (i.e., all reads in the interface are from the initial memory at the start of the BB)
  }
  policy.problem.unparse(f);
  fclose(f);
  return 0;
}
