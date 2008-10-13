#include "x86InstructionSemantics.h"
#include <cassert>
#include <cstdio>
#include <boost/static_assert.hpp>
#include "btorProblem.h"
#include "x86AssemblyToNetlist.h" // Mostly for BMCError and numBmcErrors

using namespace std;
using boost::array;

typedef BtorComputationPtr Comp;

struct BTRegisterInfo {
  Comp gprs[8];
  Comp ip;
  Comp flags[16];
  Comp errorFlag[numBmcErrors];
  Comp memory;
};

struct BtorTranslationPolicy {
  template <size_t Len>
  struct wordType {typedef Comp type;};

  BTRegisterInfo registerMap;
  BTRegisterInfo newRegisterMap;
  BTRegisterInfo origRegisterMap;
  Comp isValidIp;
  Comp notResetState;
  BtorProblem problem;

  void makeRegMap(BTRegisterInfo& rm, const string& prefix) {
    for (size_t i = 0; i < 8; ++i) {
      rm.gprs[i] = problem.build_var(32, prefix + "_gpr" + boost::lexical_cast<string>(i));
    }
    rm.ip = problem.build_var(32, prefix + "_ip");
    for (size_t i = 0; i < 16; ++i) {
      rm.flags[i] = problem.build_var(1, prefix + "_flag" + boost::lexical_cast<string>(i));
    }
    for (size_t i = 0; i < numBmcErrors; ++i) {
      rm.errorFlag[i] = false_();
    }
    rm.memory = problem.build_array(8, 32);
  }

  void addNext(Comp cur, Comp next) {
    problem.computations.push_back(problem.build_op_next(cur, next));
  }

  void addAnext(Comp cur, Comp next) {
    problem.computations.push_back(problem.build_op_anext(cur, next));
  }

  void addNexts() {
    for (size_t i = 0; i < 8; ++i) {
      addNext(origRegisterMap.gprs[i], newRegisterMap.gprs[i]);
    }
    addNext(origRegisterMap.ip, newRegisterMap.ip);
    for (size_t i = 0; i < 16; ++i) {
      addNext(origRegisterMap.flags[i], newRegisterMap.flags[i]);
    }
    addAnext(origRegisterMap.memory, newRegisterMap.memory);
  }

  void setInitialState(uint32_t entryPoint) {
    registerMap = origRegisterMap;
    writeGPR(x86_gpr_ax, zero(32));
    writeGPR(x86_gpr_cx, zero(32));
    writeGPR(x86_gpr_dx, zero(32));
    writeGPR(x86_gpr_bx, zero(32));
    writeGPR(x86_gpr_sp, number<32>(0xBFFF0000U));
    writeGPR(x86_gpr_bp, zero(32));
    writeGPR(x86_gpr_si, zero(32));
    writeGPR(x86_gpr_di, zero(32));
    writeIP(number<32>(entryPoint));
    writeBackReset();
  }

  BtorTranslationPolicy(uint32_t entryPoint): problem() {
    makeRegMap(origRegisterMap, "in");
    makeRegMap(newRegisterMap, "bogus");
    isValidIp = false_();
    notResetState = problem.build_var(1, "notFirstStep");
    addNext(notResetState, true_());
    setInitialState(entryPoint);
  }

  Comp readGPR(X86GeneralPurposeRegister r) {
    return registerMap.gprs[r];
  }

  void writeGPR(X86GeneralPurposeRegister r, const Comp& value) {
    registerMap.gprs[r] = value;
  }

  Comp readSegreg(X86SegmentRegister sr) {
    return problem.build_constant(16, 0x2B); // FIXME
  }

  void writeSegreg(X86SegmentRegister sr, Comp val) {
    // FIXME
  }

  Comp readIP() {
    return registerMap.ip;
  }

  void writeIP(const Comp& newIp) {
    registerMap.ip = newIp;
  }

  Comp readFlag(X86Flag f) {
    return registerMap.flags[f];
  }

  void writeFlag(X86Flag f, const Comp& value) {
    registerMap.flags[f] = value;
  }

  template <size_t Len>
  Comp number(uint64_t n) {
    return problem.build_constant(Len, (n & (SHL1<Len>::value - 1)));
  }

  Comp concat(const Comp& a, const Comp& b) {
    return problem.build_op_concat(a, b);
  }

  template <size_t From, size_t To>
  Comp extract(const Comp& a) {
    BOOST_STATIC_ASSERT(From < To);
    return problem.build_op_slice(a, From, To - 1);
  }

  Comp extractVar(const Comp& a, size_t from, size_t to) {
    assert (from < to);
    return problem.build_op_slice(a, from, to - 1);
  }

  Comp false_() {return zero(1);}
  Comp true_() {return ones(1);}

  Comp invert(const Comp& a) {
    return problem.build_op_not(a);
  }

  Comp and_(const Comp& a, const Comp& b) {
    return problem.build_op_and(a, b);
  }

  Comp or_(const Comp& a, const Comp& b) {
    return problem.build_op_or(a, b);
  }

  Comp xor_(const Comp& a, const Comp& b) {
    return problem.build_op_xor(a, b);
  }

  template <size_t From, size_t To>
  Comp signExtend(const Comp& a) {
    if (From == To) return a;
    return concat(
             a,
             ite(extract<From - 1, From>(a),
                 ones(To - From),
                 zero(To - From)));
  }

  Comp signExtendVar(const Comp& a, size_t to) {
    size_t from = a->type.bitWidth;
    if (from == to) return a;
    return concat(
             a,
             ite(extractVar(a, from - 1, from),
                 ones(to - from),
                 zero(to - from)));
  }

  Comp zeroExtendVar(const Comp& a, size_t to) {
    size_t from = a->type.bitWidth;
    if (from == to) return a;
    return concat(
             a,
             zero(to - from));
  }

  Comp ite(const Comp& sel, const Comp& ifTrue, const Comp& ifFalse) {
    return problem.build_op_cond(sel, ifTrue, ifFalse);
  }

  Comp equalToZero(const Comp& a) {
    return problem.build_op_not(problem.build_op_redor(a));
  }

  template <size_t Len>
  Comp generateMask(Comp w) {
    uint fullLen = 1ULL << (w->type.bitWidth);
    assert (Len <= fullLen);
    Comp w2 = ite(problem.build_op_ugt(w, problem.build_constant(w->type.bitWidth, Len - 1)), problem.build_constant(w->type.bitWidth, Len - 1), w);
    return extractVar(invert(problem.build_op_sll(ones(fullLen), w2)), 0, Len);
  }

  Comp zero(uint width) {
    assert (width != 0);
    return problem.build_op_zero(width);
  }

  Comp ones(uint width) {
    assert (width != 0);
    return problem.build_op_ones(width);
  }

  Comp add(const Comp& a, const Comp& b) { // Simple case
    return problem.build_op_add(a, b);
  }

  Comp addWithCarries(const Comp& a, const Comp& b, const Comp& carryIn, Comp& carries) { // Full case
    uint len = a->type.bitWidth + 1;
    Comp aFull = zeroExtendVar(a, len);
    Comp bFull = zeroExtendVar(b, len);
    Comp carryInFull = zeroExtendVar(carryIn, len);
    Comp sumFull = add(add(aFull, bFull), carryInFull);
    Comp sum = extractVar(sumFull, 0, a->type.bitWidth);
    Comp carriesFull = xor_(xor_(sumFull, aFull), bFull);
    carries = extractVar(carriesFull, 1, carriesFull->type.bitWidth);
    return sum;
  }

  static uint log2(uint a) {
    uint n = 1ULL;
    uint i = 0;
    while (n != 0 && n < a) {n <<= 1; ++i;}
    return i;
  }

  Comp rotateLeft(const Comp& a, const Comp& cnt) {
    return problem.build_op_ror(a, extractVar(cnt, 0, log2(a->type.bitWidth))); // Flipped because words are LSB first
  }

  // Expanding multiplies
  Comp signedMultiply(const Comp& a, const Comp& b) {
    uint len = a->type.bitWidth + b->type.bitWidth;
    Comp aFull = signExtendVar(a, len);
    Comp bFull = signExtendVar(b, len);
    return problem.build_op_mul(aFull, bFull);
  }

  Comp unsignedMultiply(const Comp& a, const Comp& b) {
    uint len = a->type.bitWidth + b->type.bitWidth;
    Comp aFull = zeroExtendVar(a, len);
    Comp bFull = zeroExtendVar(b, len);
    return problem.build_op_mul(aFull, bFull);
  }

  Comp signedDivide(const Comp& a, const Comp& b) {
    assert (a->type.bitWidth >= b->type.bitWidth);
    Comp bFull = signExtendVar(b, a->type.bitWidth);
    return problem.build_op_sdiv(a, bFull);
  }

  Comp signedModulo(const Comp& a, const Comp& b) {
    assert (a->type.bitWidth >= b->type.bitWidth);
    Comp bFull = signExtendVar(b, a->type.bitWidth);
    return extractVar(problem.build_op_smod(a, bFull), 0, b->type.bitWidth);
  }

  Comp unsignedDivide(const Comp& a, const Comp& b) {
    assert (a->type.bitWidth >= b->type.bitWidth);
    Comp bFull = zeroExtendVar(b, a->type.bitWidth);
    return problem.build_op_udiv(a, bFull);
  }

  Comp unsignedModulo(const Comp& a, const Comp& b) {
    assert (a->type.bitWidth >= b->type.bitWidth);
    Comp bFull = zeroExtendVar(b, a->type.bitWidth);
    return extractVar(problem.build_op_urem(a, bFull), 0, b->type.bitWidth);
  }

  Comp leastSignificantSetBit(const Comp& in, uint origWidth = 0, uint base = 0) {
    uint width = in->type.bitWidth;
    if (origWidth == 0) origWidth = width;
    if (width == 0) return problem.build_constant(origWidth, base);
    if (width == 1) return ite(in, problem.build_constant(origWidth, base), zero(origWidth));
    return ite(extractVar(in, 0, 1),
               problem.build_constant(origWidth, base),
               leastSignificantSetBit(extractVar(in, 1, width), origWidth, base + 1));
  }

  Comp mostSignificantSetBit(const Comp& in, uint origWidth = 0) {
    uint width = in->type.bitWidth;
    if (origWidth == 0) origWidth = width;
    if (width == 0) return zero(width);
    if (width == 1) return zero(width); // Return 0 for not found
    return ite(extractVar(in, width - 1, width),
               problem.build_constant(origWidth, width - 1),
               mostSignificantSetBit(extractVar(in, 0, width - 1), origWidth));
  }

  template <size_t Len> // In bits
  Comp readMemory(X86SegmentRegister segreg, const Comp& addr, Comp cond) {
    Comp result = zero(Len);
    for (size_t i = 0; i < Len / 8; ++i) {
      Comp thisByte = readMemoryByte(segreg, add(addr, number<32>(i)), cond);
      if (Len == 8) {
        result = thisByte;
      } else if (i == 0) {
        result = or_(result, concat(thisByte, zero(Len - 8)));
      } else if (i == Len / 8 - 1) {
        result = or_(result, concat(zero(Len - 8), thisByte));
      } else {
        result = or_(result, concat(concat(zero(i * 8), thisByte), zero(Len - ((i + 1) * 8))));
      }
    }
    return result;
  }

  Comp readMemoryByte(X86SegmentRegister segreg, const Comp& addr, Comp cond) {
    return problem.build_op_read(registerMap.memory, addr);
  }

  void writeMemoryByte(X86SegmentRegister segreg, const Comp& addr, const Comp& data, Comp cond) {
    registerMap.memory = problem.build_op_acond(cond, problem.build_op_write(registerMap.memory, addr, data), registerMap.memory);
  }

  template <size_t Len>
  void writeMemory(X86SegmentRegister segreg, const Comp& addr, const Comp& data, Comp cond) {
    BOOST_STATIC_ASSERT (Len % 8 == 0);
    for (size_t i = 0; i < Len / 8; ++i) {
      writeMemoryByte(segreg, (i == 0 ? addr : problem.build_op_add(addr, problem.build_constant(32, i))), problem.build_op_slice(data, i * 8, i * 8 + 7), cond);
    }
  }

  void hlt() {registerMap.errorFlag[bmc_error_program_failure] = true_();} // FIXME
  void interrupt(uint8_t num) {} // FIXME
  Comp rdtsc() {return problem.build_var(64, "timestamp");}

  void writeBackCond(Comp cond) {
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

  void writeBack(uint64_t addr) {
    Comp isThisIp = and_(problem.build_op_eq(origRegisterMap.ip, number<32>(addr)), notResetState);
    isValidIp = or_(isValidIp, isThisIp);
    writeBackCond(isThisIp);
  }

  void writeBackReset() {
    Comp cond = invert(notResetState);
    isValidIp = or_(isValidIp, cond);
    writeBackCond(cond);
  }

  void startBlock(uint64_t addr) {
    registerMap = origRegisterMap;
    fprintf(stderr, "Block 0x%08X\n", (unsigned int)addr);
  }

  void finishBlock(uint64_t addr) {
    writeBack(addr);
  }

  void startInstruction(SgAsmInstruction*) {}
  void finishInstruction(SgAsmInstruction*) {}

};

int main(int argc, char** argv) {
  SgProject* proj = frontend(argc, argv);
  FILE* f = fopen("foo.btor", "w");
  assert (f);
  vector<SgNode*> headers = NodeQuery::querySubTree(proj, V_SgAsmGenericHeader);
  ROSE_ASSERT (headers.size() == 1);
  SgAsmGenericHeader* header = isSgAsmGenericHeader(headers[0]);
  rose_addr_t entryPoint = header->get_entry_rva() + header->get_base_va();
  BtorTranslationPolicy policy((uint32_t)entryPoint);
  X86InstructionSemantics<BtorTranslationPolicy> t(policy);
  vector<SgNode*> blocks = NodeQuery::querySubTree(proj, V_SgAsmBlock);
  for (size_t i = 0; i < blocks.size(); ++i) {
    SgAsmBlock* b = isSgAsmBlock(blocks[i]);
    ROSE_ASSERT (b);
    t.processBlock(b);
  }
  // Add "bogus IP" error
  policy.newRegisterMap.errorFlag[bmc_error_bogus_ip] =
    policy.invert(policy.isValidIp);
  policy.addNexts();
  for (size_t i = 0; i < numBmcErrors; ++i) {
    policy.problem.computations.push_back(policy.problem.build_op_root(policy.newRegisterMap.errorFlag[i]));
  }
  string s = policy.problem.unparse();
  fprintf(f, "%s", s.c_str());
  fclose(f);
  return 0;
}
