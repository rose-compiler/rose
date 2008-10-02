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

struct NetlistTranslationPolicy {
  template <size_t Len>
  struct wordType {typedef LitList(Len) type;};

  typedef pair<X86RegisterClass, int> FullReg;
  typedef map<FullReg, LitList(32)> RegMap;
  typedef map<X86Flag, Lit> FlagMap;
  typedef vector<MemoryAccess> MemoryAccessList;
  RegMap registerMap;
  RegMap newRegisterMap;
  RegMap origRegisterMap;
  FlagMap flagMap;
  FlagMap newFlagMap;
  FlagMap origFlagMap;
  size_t memoryWriteCountBase;
  size_t memoryReadCountBase;
  MemoryAccessList memoryWrites;
  MemoryAccessList initialMemoryReads;
  SatProblem problem;

  static FullReg gpr(X86GeneralPurposeRegister reg) {
    return make_pair(x86_regclass_gpr, reg);
  }

  static FullReg ip() {
    return make_pair(x86_regclass_ip, 0);
  }

  const LitList(32)& vars(FullReg r) const {
    RegMap::const_iterator i = registerMap.find(r);
    ROSE_ASSERT (i != registerMap.end());
    return i->second;
  }

  LitList(32)& vars(FullReg r) {
    RegMap::iterator i = registerMap.find(r);
    ROSE_ASSERT (i != registerMap.end());
    return i->second;
  }

  Lit flag(X86Flag f) const {
    FlagMap::const_iterator i = flagMap.find(f);
    ROSE_ASSERT (i != flagMap.end());
    return i->second;
  }

  Lit& flag(X86Flag f) {
    FlagMap::iterator i = flagMap.find(f);
    ROSE_ASSERT (i != flagMap.end());
    return i->second;
  }

  void makeRegMaps(RegMap& rm, FlagMap& fm, const std::string& prefix) {
    for (int r = 0; r < 8; ++r) {
      LitList(32) thisReg = problem.newVars<32>();
      problem.addInterface(prefix + "_gpr" + boost::lexical_cast<std::string>(r), toVector(thisReg));
      rm.insert(make_pair(gpr((X86GeneralPurposeRegister)r), thisReg));
    }
    LitList(32) ipReg = problem.newVars<32>();
    problem.addInterface(prefix + "_ip", toVector(ipReg));
    rm.insert(make_pair(ip(), ipReg));
    LitList(7) flagsReg = problem.newVars<7>();
    problem.addInterface(prefix + "_flags", toVector(flagsReg));
    fm.insert(make_pair(x86_flag_cf, flagsReg[0]));
    fm.insert(make_pair(x86_flag_pf, flagsReg[1]));
    fm.insert(make_pair(x86_flag_af, flagsReg[2]));
    fm.insert(make_pair(x86_flag_zf, flagsReg[3]));
    fm.insert(make_pair(x86_flag_sf, flagsReg[4]));
    fm.insert(make_pair(x86_flag_df, flagsReg[5]));
    fm.insert(make_pair(x86_flag_of, flagsReg[6]));
  }

  NetlistTranslationPolicy(): memoryWriteCountBase(0), memoryReadCountBase(0), problem() {
    makeRegMaps(newRegisterMap, newFlagMap, "out");
  }

  LitList(32) readGPR(X86GeneralPurposeRegister r) {
    return vars(gpr(r));
  }

  void writeGPR(X86GeneralPurposeRegister r, const LitList(32)& value) {
    vars(gpr(r)) = value;
  }

  LitList(32) readIP() {
    return vars(ip());
  }

  void writeIP(const LitList(32)& newIp) {
    vars(ip()) = newIp;
  }

  LitList(1) readFlag(X86Flag f) {
    return single(flag(f));
  }

  void writeFlag(X86Flag f, const LitList(1)& value) {
    flag(f) = value[0];
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
  }

  void writeMemoryByte(X86SegmentRegister segreg, const LitList(32)& addr, const LitList(8)& data, Lit cond) {
    memoryWrites.push_back(MemoryAccess(addr, data, cond));
  }

  template <size_t Len>
  void writeMemory(X86SegmentRegister segreg, const LitList(32)& addr, const LitList(Len)& data, LitList(1) cond) {
    WriteMemoryHelper<Len, 0, NetlistTranslationPolicy>::go(segreg, addr, data, cond[0], *this);
  }

  void hlt() {} // FIXME
  void interrupt(uint8_t num) {} // FIXME
  LitList(64) rdtsc() {return number<64>(0);} // FIXME

  void writeBack(Lit isThisIp) {
    fprintf(stderr, "Have %zu variables and %zu clauses so far\n", problem.numVariables, problem.clauses.size());
    for (RegMap::const_iterator i = registerMap.begin(); i != registerMap.end(); ++i) {
      for (size_t j = 0; j < 32; ++j) {
        // fprintf(stderr, "Adding register output equivalence %d -> %d == %d\n", isThisIp, (i->second)[j], newRegisterMap[i->first][j]);
        problem.condEquivalence(isThisIp, (i->second)[j], newRegisterMap[i->first][j]);
      }
    }
    for (FlagMap::const_iterator i = flagMap.begin(); i != flagMap.end(); ++i) {
      problem.condEquivalence(isThisIp, i->second, newFlagMap[i->first]);
    }
    for (MemoryAccessList::const_iterator i = memoryWrites.begin(); i != memoryWrites.end(); ++i) {
      problem.addInterface("memoryWrite_" + boost::lexical_cast<std::string>(memoryWriteCountBase), toVector(concat(single(problem.andGate(i->cond, isThisIp)), concat(i->addr, i->data))));
      ++memoryWriteCountBase;
    }
    fprintf(stderr, "Have %zu variables and %zu clauses so far\n", problem.numVariables, problem.clauses.size());
  }

  void startBlock(uint64_t addr) {
    registerMap = origRegisterMap;
    flagMap = origFlagMap;
    memoryWrites.clear();
    initialMemoryReads.clear();
    fprintf(stderr, "Block 0x%08X\n", (unsigned int)addr);
  }

  void finishBlock(uint64_t addr) {
    writeBack(problem.equal(origRegisterMap[ip()], ::number<32>(addr)));
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
  policy.makeRegMaps(policy.origRegisterMap, policy.origFlagMap, "in");
  vector<SgNode*> blocks = NodeQuery::querySubTree(proj, V_SgAsmBlock);
  for (size_t i = 0; i < blocks.size(); ++i) {
    SgAsmBlock* b = isSgAsmBlock(blocks[i]);
    ROSE_ASSERT (b);
    t.processBlock(b);
  }
  policy.problem.unparse(f);
  fclose(f);
  return 0;
}
