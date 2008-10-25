#ifndef X86ASSEMBLYTOBTOR_H
#define X86ASSEMBLYTOBTOR_H

#include "x86InstructionSemantics.h"
#include "integerOps.h"
#include <cassert>
#include <cstdio>
#include <boost/static_assert.hpp>
#include "btorProblem.h"
#include "x86AssemblyToNetlist.h" // Mostly for BMCError and numBmcErrors

typedef BtorComputationPtr Comp;

template <size_t Len> struct BtorWordType: public Comp {
  BtorWordType(const Comp& c): Comp(c) {
    assert (c.bitWidth() == Len);
  }
};

struct BTRegisterInfo {
  Comp gprs[8];
  Comp ip;
  Comp flags[16];
  Comp errorFlag[numBmcErrors];
  Comp memory;
};

template <typename Hooks>
struct BtorTranslationPolicy {
  BTRegisterInfo registerMap;
  BTRegisterInfo newRegisterMap;
  BTRegisterInfo origRegisterMap;
  Comp isValidIp;
  std::vector<uint32_t> validIPs;
  Comp resetState;
  Comp errorsEnabled;
  BtorProblem problem;
  Hooks& hooks;

  void makeRegMap(BTRegisterInfo& rm, const std::string& prefix) {
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

  void makeRegMapZero(BTRegisterInfo& rm) {
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

  void setInitialState(uint32_t entryPoint, bool initialConditionsAreUnknown) {
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
        initialEip = ite(problem.build_var(1), number<32>(validIPs[i]), initialEip);
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

  BtorTranslationPolicy(Hooks& hooks, uint32_t minNumStepsToFindError, uint32_t maxNumStepsToFindError): problem(), hooks(hooks) {
    assert (minNumStepsToFindError >= 1); // Can't find an error on the first step
    assert (maxNumStepsToFindError < 0xFFFFFFFFU); // Prevent overflows
    assert (minNumStepsToFindError <= maxNumStepsToFindError);
    makeRegMap(origRegisterMap, "");
    makeRegMapZero(newRegisterMap);
    isValidIp = false_();
    validIPs.clear();
    Comp stepCount = problem.build_var(32, "stepCount_saturating_at_" + boost::lexical_cast<std::string>(maxNumStepsToFindError + 1));
    addNext(stepCount, ite(problem.build_op_eq(stepCount, number<32>(maxNumStepsToFindError + 1)), number<32>(maxNumStepsToFindError + 1), problem.build_op_inc(stepCount)));
    resetState = problem.build_op_eq(stepCount, zero(32));
    errorsEnabled = problem.build_op_and(problem.build_op_ugte(stepCount, number<32>(minNumStepsToFindError)), problem.build_op_ulte(stepCount, number<32>(maxNumStepsToFindError)));
  }

  BtorWordType<32> readGPR(X86GeneralPurposeRegister r) {
    return registerMap.gprs[r];
  }

  void writeGPR(X86GeneralPurposeRegister r, const BtorWordType<32>& value) {
    registerMap.gprs[r] = value;
  }

  BtorWordType<16> readSegreg(X86SegmentRegister sr) {
    return problem.build_constant(16, 0x2B); // FIXME
  }

  void writeSegreg(X86SegmentRegister sr, BtorWordType<16> val) {
    // FIXME
  }

  BtorWordType<32> readIP() {
    return registerMap.ip;
  }

  void writeIP(const BtorWordType<32>& newIp) {
    registerMap.ip = newIp;
  }

  BtorWordType<1> readFlag(X86Flag f) {
    return registerMap.flags[f];
  }

  void writeFlag(X86Flag f, const BtorWordType<1>& value) {
    registerMap.flags[f] = value;
  }

  template <size_t Len>
  BtorWordType<Len> number(uint64_t n) {
    return problem.build_constant(Len, (n & (IntegerOps::GenMask<uint64_t, Len>::value)));
  }

  Comp concat(const Comp& a, const Comp& b) {
    return problem.build_op_concat(b, a); // Our concat puts a on the LSB side of b, while BTOR does the opposite
  }

  template <size_t Len1, size_t Len2>
  BtorWordType<Len1 + Len2> concat(const BtorWordType<Len1>& a, const BtorWordType<Len2>& b) {
    return problem.build_op_concat(b, a); // Our concat puts a on the LSB side of b, while BTOR does the opposite
  }

  template <size_t From, size_t To>
  BtorWordType<To - From> extract(const Comp& a) {
    BOOST_STATIC_ASSERT(From < To);
    return extractVar(a, From, To);
  }

  Comp extractVar(const Comp& a, size_t from, size_t to) {
    assert (from < to);
    assert (from <= a.bitWidth());
    assert (to <= a.bitWidth());
    return problem.build_op_slice(a, to - 1, from);
  }

  BtorWordType<1> true_() {return ones(1);}
  BtorWordType<1> false_() {return zero(1);}
  BtorWordType<1> undefined_() {return problem.build_var(1);}

  template <size_t Len>
  BtorWordType<Len> invert(const BtorWordType<Len>& a) {
    return a.invert();
    // return problem.build_op_not(a);
  }

  Comp invert(const Comp& a) {
    return a.invert();
    // return problem.build_op_not(a);
  }

  template <size_t Len>
  BtorWordType<Len> negate(const BtorWordType<Len>& a) {
    return problem.build_op_neg(a);
  }

  template <size_t Len>
  BtorWordType<Len> and_(const BtorWordType<Len>& a, const BtorWordType<Len>& b) {
    return problem.build_op_and(a, b);
  }

  template <size_t Len>
  BtorWordType<Len> or_(const BtorWordType<Len>& a, const BtorWordType<Len>& b) {
    return problem.build_op_or(a, b);
  }

  template <size_t Len>
  BtorWordType<Len> xor_(const BtorWordType<Len>& a, const BtorWordType<Len>& b) {
    return problem.build_op_xor(a, b);
  }

  template <size_t From, size_t To>
  BtorWordType<To> signExtend(const BtorWordType<From>& a) {
    if (From == To) return a;
    return concat(
             a,
             ite(extract<From - 1, From>(a),
                 ones<To - From>(),
                 zero<To - From>()));
  }

  Comp signExtendVar(const Comp& a, size_t to) {
    size_t from = a.bitWidth();
    if (from == to) return a;
    return concat(
             a,
             ite(extractVar(a, from - 1, from),
                 ones(to - from),
                 zero(to - from)));
  }

  Comp zeroExtendVar(const Comp& a, size_t to) {
    size_t from = a.bitWidth();
    if (from == to) return a;
    return concat(
             a,
             zero(to - from));
  }

  template <size_t Len>
  BtorWordType<Len> ite(const BtorWordType<1>& sel, const BtorWordType<Len>& ifTrue, const BtorWordType<Len>& ifFalse) {
    return problem.build_op_cond(sel, ifTrue, ifFalse);
  }

  Comp ite(const Comp& sel, const Comp& ifTrue, const Comp& ifFalse) {
    return problem.build_op_cond(sel, ifTrue, ifFalse);
  }

  template <size_t Len>
  BtorWordType<1> equalToZero(const BtorWordType<Len>& a) {
    return invert(problem.build_op_redor(a));
  }

  template <size_t Len>
  BtorWordType<Len> zero() {
    assert (Len != 0);
    return problem.build_op_zero(Len);
  }

  Comp zero(uint width) {
    assert (width != 0);
    return problem.build_op_zero(width);
  }

  template <size_t Len>
  BtorWordType<Len> ones() {
    assert (Len != 0);
    return problem.build_op_ones(Len);
  }

  Comp ones(uint width) {
    assert (width != 0);
    return problem.build_op_ones(width);
  }

  template <size_t Len>
  BtorWordType<Len> add(const BtorWordType<Len>& a, const BtorWordType<Len>& b) { // Simple case
    return problem.build_op_add(a, b);
  }

  template <size_t Len>
  BtorWordType<Len> addWithCarries(const BtorWordType<Len>& a, const BtorWordType<Len>& b, const BtorWordType<1>& carryIn, BtorWordType<Len>& carries) { // Full case
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

  template <size_t Len, size_t SCLen>
  BtorWordType<Len> rotateLeft(const BtorWordType<Len>& a, const BtorWordType<SCLen>& cnt) {
    return problem.build_op_rol(a, extractVar(cnt, 0, log2(a.bitWidth())));
  }

  template <size_t Len, size_t SCLen>
  BtorWordType<Len> rotateRight(const BtorWordType<Len>& a, const BtorWordType<SCLen>& cnt) {
    return problem.build_op_ror(a, extractVar(cnt, 0, log2(a.bitWidth())));
  }

  template <size_t Len, size_t SCLen>
  BtorWordType<Len> shiftLeft(const BtorWordType<Len>& a, const BtorWordType<SCLen>& cnt) {
    return problem.build_op_sll(a, extractVar(cnt, 0, log2(a.bitWidth())));
  }

  template <size_t Len, size_t SCLen>
  BtorWordType<Len> shiftRight(const BtorWordType<Len>& a, const BtorWordType<SCLen>& cnt) {
    return problem.build_op_srl(a, extractVar(cnt, 0, log2(a.bitWidth())));
  }

  template <size_t Len, size_t SCLen>
  BtorWordType<Len> shiftRightArithmetic(const BtorWordType<Len>& a, const BtorWordType<SCLen>& cnt) {
    return problem.build_op_sra(a, extractVar(cnt, 0, log2(a.bitWidth())));
  }

  // Expanding multiplies
  template <size_t Len1, size_t Len2>
  BtorWordType<Len1 + Len2> signedMultiply(const BtorWordType<Len1>& a, const BtorWordType<Len2>& b) {
    uint len = a.bitWidth() + b.bitWidth();
    Comp aFull = signExtendVar(a, len);
    Comp bFull = signExtendVar(b, len);
    return problem.build_op_mul(aFull, bFull);
  }

  template <size_t Len1, size_t Len2>
  BtorWordType<Len1 + Len2> unsignedMultiply(const BtorWordType<Len1>& a, const BtorWordType<Len2>& b) {
    uint len = a.bitWidth() + b.bitWidth();
    Comp aFull = zeroExtendVar(a, len);
    Comp bFull = zeroExtendVar(b, len);
    return problem.build_op_mul(aFull, bFull);
  }

  template <size_t Len1, size_t Len2>
  BtorWordType<Len1> signedDivide(const BtorWordType<Len1>& a, const BtorWordType<Len2>& b) {
    assert (a.bitWidth() >= b.bitWidth());
    Comp bFull = signExtendVar(b, a.bitWidth());
    return problem.build_op_sdiv(a, bFull);
  }

  template <size_t Len1, size_t Len2>
  BtorWordType<Len2> signedModulo(const BtorWordType<Len1>& a, const BtorWordType<Len2>& b) {
    assert (a.bitWidth() >= b.bitWidth());
    Comp bFull = signExtendVar(b, a.bitWidth());
    return extractVar(problem.build_op_smod(a, bFull), 0, b.bitWidth());
  }

  template <size_t Len1, size_t Len2>
  BtorWordType<Len1> unsignedDivide(const BtorWordType<Len1>& a, const BtorWordType<Len2>& b) {
    assert (a.bitWidth() >= b.bitWidth());
    Comp bFull = zeroExtendVar(b, a.bitWidth());
    return problem.build_op_udiv(a, bFull);
  }

  template <size_t Len1, size_t Len2>
  BtorWordType<Len2> unsignedModulo(const BtorWordType<Len1>& a, const BtorWordType<Len2>& b) {
    assert (a.bitWidth() >= b.bitWidth());
    Comp bFull = zeroExtendVar(b, a.bitWidth());
    return extractVar(problem.build_op_urem(a, bFull), 0, b.bitWidth());
  }

  template <size_t Len>
  BtorWordType<Len> leastSignificantSetBit(const BtorWordType<Len>& in) {
    Comp result = number<Len>(0); // Not found
    for (size_t i = Len; i > 0; --i) { // Scan from MSB to LSB as later checks override earlier ones
      result = ite(extractVar(in, i - 1, i), number<Len>(i - 1), result);
    }
    return result;
  }

  template <size_t Len>
  BtorWordType<Len> mostSignificantSetBit(const BtorWordType<Len>& in) {
    Comp result = number<Len>(0); // Not found
    for (size_t i = 0; i < Len; ++i) { // Scan from LSB to MSB as later checks override earlier ones
      result = ite(extractVar(in, i, i + 1), number<Len>(i), result);
    }
    return result;
  }

  template <size_t Len> // In bits
  BtorWordType<Len> readMemory(X86SegmentRegister segreg, const BtorWordType<32>& addr, BtorWordType<1> cond) {
    BtorWordType<Len> result = zero(Len);
    for (size_t i = 0; i < Len / 8; ++i) {
      Comp thisByte = readMemoryByte(segreg, add(addr, number<32>(i)), cond);
      if (Len == 8) {
        result = thisByte;
      } else if (i == 0) {
        result = problem.build_op_or(result, concat(thisByte, zero(Len - 8)));
      } else if (i == Len / 8 - 1) {
        result = problem.build_op_or(result, concat(zero(Len - 8), thisByte));
      } else {
        result = problem.build_op_or(result, concat(concat(zero(i * 8), thisByte), zero(Len - ((i + 1) * 8))));
      }
    }
    return result;
  }

  Comp readMemoryByte(X86SegmentRegister segreg, const BtorWordType<32>& addr, BtorWordType<1> cond) {
    return problem.build_op_read(registerMap.memory, addr);
  }

  void writeMemoryByte(X86SegmentRegister segreg, const BtorWordType<32>& addr, const Comp& data, BtorWordType<1> cond) {
    assert (addr.bitWidth() == 32);
    assert (data.bitWidth() == 8);
    registerMap.memory = problem.build_op_acond(cond, problem.build_op_write(registerMap.memory, addr, data), registerMap.memory);
  }

  template <size_t Len>
  void writeMemory(X86SegmentRegister segreg, const BtorWordType<32>& addr, const BtorWordType<Len>& data, BtorWordType<1> cond) {
    BOOST_STATIC_ASSERT (Len % 8 == 0);
    for (size_t i = 0; i < Len / 8; ++i) {
      writeMemoryByte(segreg, (i == 0 ? (Comp)addr : problem.build_op_add(addr, problem.build_constant(32, i))), extractVar(data, i * 8, i * 8 + 8), cond);
    }
  }

  BtorWordType<32> filterIndirectJumpTarget(const BtorWordType<32>& addr) {
    return addr;
  }

  BtorWordType<32> filterCallTarget(const BtorWordType<32>& addr) {
    return addr;
  }

  BtorWordType<32> filterReturnTarget(const BtorWordType<32>& addr) {
    return addr;
  }

  void hlt() {
    hooks.hlt(*this);
  }
  void interrupt(uint8_t num) {} // FIXME
  BtorWordType<64> rdtsc() {return problem.build_var(64, "timestamp");}

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
    Comp isThisIp = problem.build_op_and(problem.build_op_eq(origRegisterMap.ip, number<32>(addr)), invert(resetState));
    isValidIp = problem.build_op_or(isValidIp, isThisIp);
    validIPs.push_back((uint32_t)addr);
    writeBackCond(isThisIp);
  }

  void writeBackReset() {
    Comp cond = resetState;
    isValidIp = problem.build_op_or(isValidIp, cond);
    writeBackCond(cond);
  }

  void startBlock(uint64_t addr) {
    registerMap = origRegisterMap;
    registerMap.ip = number<32>(0xDEADBEEF);
    // fprintf(stderr, "Block 0x%08X\n", (unsigned int)addr);
    hooks.startBlock(*this, addr);
  }

  void finishBlock(uint64_t addr) {
    writeBack(addr);
    hooks.finishBlock(*this, addr);
  }

  void startInstruction(SgAsmx86Instruction* insn) {
    hooks.startInstruction(*this, insn);
  }
  void finishInstruction(SgAsmx86Instruction* insn) {
    hooks.finishInstruction(*this, insn);
  }

};

template <typename TranslationPolicy>
std::string btorTranslate(TranslationPolicy& policy, SgProject* proj, FILE* outfile, bool initialConditionsAreUnknown, bool bogusIpIsError) {
  std::vector<SgNode*> headers = NodeQuery::querySubTree(proj, V_SgAsmGenericHeader);
  ROSE_ASSERT (headers.size() == 1);
  SgAsmGenericHeader* header = isSgAsmGenericHeader(headers[0]);
  rose_addr_t entryPoint = header->get_entry_rva() + header->get_base_va();
  X86InstructionSemantics<TranslationPolicy, BtorWordType> t(policy);
  std::vector<SgNode*> blocks = NodeQuery::querySubTree(proj, V_SgAsmBlock);
  for (size_t i = 0; i < blocks.size(); ++i) {
    SgAsmBlock* b = isSgAsmBlock(blocks[i]);
    ROSE_ASSERT (b);
    if (b->get_statementList().empty()) continue;
    t.processBlock(b);
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

#endif // X86ASSEMBLYTOBTOR_H
