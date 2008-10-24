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

struct BTRegisterInfo {
  Comp gprs[8];
  Comp ip;
  Comp flags[16];
  Comp errorFlag[numBmcErrors];
  Comp memory;
};

template <typename Hooks>
struct BtorTranslationPolicy {
  template <size_t Len>
  struct wordType {typedef Comp type;};

  BTRegisterInfo registerMap;
  BTRegisterInfo newRegisterMap;
  BTRegisterInfo origRegisterMap;
  Comp isValidIp;
  std::vector<uint32_t> validIPs;
  Comp notResetState;
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

  BtorTranslationPolicy(Hooks& hooks): problem(), hooks(hooks) {
    makeRegMap(origRegisterMap, "");
    makeRegMapZero(newRegisterMap);
    isValidIp = false_();
    validIPs.clear();
    notResetState = problem.build_var(1, "notFirstStep");
    addNext(notResetState, true_());
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
    return problem.build_constant(Len, (n & (IntegerOps::GenMask<uint64_t, Len>::value)));
  }

  Comp concat(const Comp& a, const Comp& b) {
    return problem.build_op_concat(b, a); // Our concat puts a on the LSB side of b, while BTOR does the opposite
  }

  template <size_t From, size_t To>
  Comp extract(const Comp& a) {
    BOOST_STATIC_ASSERT(From < To);
    return extractVar(a, From, To);
  }

  Comp extractVar(const Comp& a, size_t from, size_t to) {
    assert (from < to);
    assert (from <= a.bitWidth());
    assert (to <= a.bitWidth());
    return problem.build_op_slice(a, to - 1, from);
  }

  Comp true_() {return ones(1);}
  Comp false_() {return zero(1);}
  Comp undefined_() {return problem.build_var(1);}

  Comp invert(const Comp& a) {
    return a.invert();
    // return problem.build_op_not(a);
  }

  Comp negate(const Comp& a) {
    return problem.build_op_neg(a);
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

  Comp ite(const Comp& sel, const Comp& ifTrue, const Comp& ifFalse) {
    return problem.build_op_cond(sel, ifTrue, ifFalse);
  }

  Comp equalToZero(const Comp& a) {
    return invert(problem.build_op_redor(a));
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
    assert (a.bitWidth() == b.bitWidth());
    assert (carryIn.bitWidth() == 1);
    uint len = a.bitWidth() + 1;
    Comp aFull = zeroExtendVar(a, len);
    Comp bFull = zeroExtendVar(b, len);
    Comp carryInFull = zeroExtendVar(carryIn, len);
    Comp sumFull = add(add(aFull, bFull), carryInFull);
    Comp sum = extractVar(sumFull, 0, a.bitWidth());
    Comp carriesFull = xor_(xor_(sumFull, aFull), bFull);
    carries = extractVar(carriesFull, 1, carriesFull.bitWidth());
    return sum;
  }

  Comp rotateLeft(const Comp& a, const Comp& cnt) {
    return problem.build_op_rol(a, extractVar(cnt, 0, log2(a.bitWidth())));
  }

  Comp rotateRight(const Comp& a, const Comp& cnt) {
    return problem.build_op_ror(a, extractVar(cnt, 0, log2(a.bitWidth())));
  }

  Comp shiftLeft(const Comp& a, const Comp& cnt) {
    return problem.build_op_sll(a, extractVar(cnt, 0, log2(a.bitWidth())));
  }

  Comp shiftRight(const Comp& a, const Comp& cnt) {
    return problem.build_op_srl(a, extractVar(cnt, 0, log2(a.bitWidth())));
  }

  Comp shiftRightArithmetic(const Comp& a, const Comp& cnt) {
    return problem.build_op_sra(a, extractVar(cnt, 0, log2(a.bitWidth())));
  }

  // Expanding multiplies
  Comp signedMultiply(const Comp& a, const Comp& b) {
    uint len = a.bitWidth() + b.bitWidth();
    Comp aFull = signExtendVar(a, len);
    Comp bFull = signExtendVar(b, len);
    return problem.build_op_mul(aFull, bFull);
  }

  Comp unsignedMultiply(const Comp& a, const Comp& b) {
    uint len = a.bitWidth() + b.bitWidth();
    Comp aFull = zeroExtendVar(a, len);
    Comp bFull = zeroExtendVar(b, len);
    return problem.build_op_mul(aFull, bFull);
  }

  Comp signedDivide(const Comp& a, const Comp& b) {
    assert (a.bitWidth() >= b.bitWidth());
    Comp bFull = signExtendVar(b, a.bitWidth());
    return problem.build_op_sdiv(a, bFull);
  }

  Comp signedModulo(const Comp& a, const Comp& b) {
    assert (a.bitWidth() >= b.bitWidth());
    Comp bFull = signExtendVar(b, a.bitWidth());
    return extractVar(problem.build_op_smod(a, bFull), 0, b.bitWidth());
  }

  Comp unsignedDivide(const Comp& a, const Comp& b) {
    assert (a.bitWidth() >= b.bitWidth());
    Comp bFull = zeroExtendVar(b, a.bitWidth());
    return problem.build_op_udiv(a, bFull);
  }

  Comp unsignedModulo(const Comp& a, const Comp& b) {
    assert (a.bitWidth() >= b.bitWidth());
    Comp bFull = zeroExtendVar(b, a.bitWidth());
    return extractVar(problem.build_op_urem(a, bFull), 0, b.bitWidth());
  }

  Comp leastSignificantSetBit(const Comp& in, uint origWidth = 0, uint base = 0) {
    uint width = in.bitWidth();
    if (origWidth == 0) origWidth = width;
    if (width == 0) return problem.build_constant(origWidth, base);
    if (width == 1) return ite(in, problem.build_constant(origWidth, base), zero(origWidth));
    return ite(extractVar(in, 0, 1),
               problem.build_constant(origWidth, base),
               leastSignificantSetBit(extractVar(in, 1, width), origWidth, base + 1));
  }

  Comp mostSignificantSetBit(const Comp& in, uint origWidth = 0) {
    uint width = in.bitWidth();
    if (origWidth == 0) origWidth = width;
    if (width == 0) return zero(origWidth);
    if (width == 1) return zero(origWidth); // Return 0 for not found
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
    assert (addr.bitWidth() == 32);
    assert (data.bitWidth() == 8);
    registerMap.memory = problem.build_op_acond(cond, problem.build_op_write(registerMap.memory, addr, data), registerMap.memory);
  }

  template <size_t Len>
  void writeMemory(X86SegmentRegister segreg, const Comp& addr, const Comp& data, Comp cond) {
    BOOST_STATIC_ASSERT (Len % 8 == 0);
    for (size_t i = 0; i < Len / 8; ++i) {
      writeMemoryByte(segreg, (i == 0 ? addr : problem.build_op_add(addr, problem.build_constant(32, i))), extractVar(data, i * 8, i * 8 + 8), cond);
    }
  }

  void hlt() {
    hooks.hlt(*this);
  }
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
    validIPs.push_back((uint32_t)addr);
    writeBackCond(isThisIp);
  }

  void writeBackReset() {
    Comp cond = invert(notResetState);
    isValidIp = or_(isValidIp, cond);
    writeBackCond(cond);
  }

  void startBlock(uint64_t addr) {
    registerMap = origRegisterMap;
    registerMap.ip = number<32>(0xDEADBEEF);
    fprintf(stderr, "Block 0x%08X\n", (unsigned int)addr);
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
std::string btorTranslate(TranslationPolicy& policy, SgProject* proj, FILE* outfile, bool initialConditionsAreUnknown) {
  std::vector<SgNode*> headers = NodeQuery::querySubTree(proj, V_SgAsmGenericHeader);
  ROSE_ASSERT (headers.size() == 1);
  SgAsmGenericHeader* header = isSgAsmGenericHeader(headers[0]);
  rose_addr_t entryPoint = header->get_entry_rva() + header->get_base_va();
  X86InstructionSemantics<TranslationPolicy> t(policy);
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
    if (i == bmc_error_bogus_ip) continue; // For testing
    policy.problem.computations.push_back(policy.problem.build_op_root(policy.newRegisterMap.errorFlag[i]));
  }
  policy.addNexts();
  return policy.problem.unparse();
}

#endif // X86ASSEMBLYTOBTOR_H
