#include "rose.h"
#include "x86AssemblyToC.h"
#include <cassert>
#include <cstdio>
#include "satProblem.h"

using namespace std;
using boost::array;

static int numBytesInAsmType(SgAsmType* ty) {
  switch (ty->variantT()) {
    case V_SgAsmTypeByte: return 1;
    case V_SgAsmTypeWord: return 2;
    case V_SgAsmTypeDoubleWord: return 4;
    case V_SgAsmTypeQuadWord: return 8;
    default: {cerr << "Unhandled type " << ty->class_name() << " in numBytesInAsmType" << endl; abort();}
  }
}

template <size_t Len>
struct ExtractReadHelper {
  static LitList(Len) go(const LitList(32)& fullDWord, X86PositionInRegister pos);
};

template <> struct ExtractReadHelper<8> {
  static LitList(8) go(const LitList(32)& fullDWord, X86PositionInRegister pos) {
    switch (pos) {
      case x86_regpos_low_byte: return extract<0, 8>(fullDWord);
      case x86_regpos_high_byte: return extract<8, 16>(fullDWord);
      default: ROSE_ASSERT (false);
    }
  }
};

template <> struct ExtractReadHelper<16> {
  static LitList(16) go(const LitList(32)& fullDWord, X86PositionInRegister pos) {
    switch (pos) {
      case x86_regpos_word: return extract<0, 16>(fullDWord);
      default: ROSE_ASSERT (false);
    }
  }
};

template <> struct ExtractReadHelper<32> {
  static LitList(32) go(const LitList(32)& fullDWord, X86PositionInRegister pos) {
    switch (pos) {
      case x86_regpos_dword: return fullDWord;
      default: ROSE_ASSERT (false);
    }
  }
};

template <size_t Len, size_t Offset, typename NLTranslator>
struct WriteMemoryHelper {
  static void go(const LitList(32)& addr, const LitList(Len)& data, NLTranslator& trans) {
    trans.writeMemoryByte((Offset == 0 ? addr : trans.problem.adder(addr, number<32>(Offset / 8))), extract<Offset, Offset + 8>(data));
    WriteMemoryHelper<Len, Offset + 8, NLTranslator>::go(addr, data, trans);
  }
};

template <size_t Len, typename NLTranslator>
struct WriteMemoryHelper<Len, Len, NLTranslator> {
  static void go(const LitList(32)& addr, const LitList(Len)& data, NLTranslator& trans) {}
};

struct NetlistTranslator {
  typedef pair<X86RegisterClass, int> FullReg;
  typedef map<FullReg, LitList(32)> RegMap;
  typedef map<X86Flag, Lit> FlagMap;
  typedef vector<pair<LitList(32), LitList(8) > > MemoryWriteList;
  typedef vector<pair<LitList(32), LitList(8) > > MemoryReadList;
  RegMap registerMap;
  RegMap newRegisterMap;
  FlagMap flagMap;
  FlagMap newFlagMap;
  MemoryWriteList memoryWrites;
  MemoryReadList initialMemoryReads;
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

  void makeRegMaps(RegMap& rm, FlagMap& fm) {
    for (int r = 0; r < 8; ++r) {
      rm.insert(make_pair(gpr((X86GeneralPurposeRegister)r), problem.newVars<32>()));
    }
    rm.insert(make_pair(ip(), problem.newVars<32>()));
    fm.insert(make_pair(x86flag_cf, problem.newVar()));
    fm.insert(make_pair(x86flag_pf, problem.newVar()));
    fm.insert(make_pair(x86flag_af, problem.newVar()));
    fm.insert(make_pair(x86flag_zf, problem.newVar()));
    fm.insert(make_pair(x86flag_sf, problem.newVar()));
    fm.insert(make_pair(x86flag_df, problem.newVar()));
    fm.insert(make_pair(x86flag_of, problem.newVar()));
  }

  NetlistTranslator(FILE* outfile): problem(outfile) {
    makeRegMaps(newRegisterMap, newFlagMap);
  }

  template <size_t Len> // In bits
  LitList(Len) readMemory(const LitList(32)& addr) {
    LitList(Len) result;
    for (size_t i = 0; i < Len / 8; ++i) {
      LitList(8) thisByte = readMemoryByte(problem.adder(addr, number<32>(i)));
      for (size_t j = 0; j < 8; ++j) {
        result[i * 8 + j] = thisByte[j];
      }
    }
    return result;
  }

  LitList(32) readEffectiveAddress(SgAsmExpression* expr) {
    assert (isSgAsmMemoryReferenceExpression(expr));
    return read<32>(isSgAsmMemoryReferenceExpression(expr)->get_address());
  }

  LitList(8) readMemoryByte(const LitList(32)& addr) {
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
      Lit thisEquality = problem.equal(addr, initialMemoryReads[i].first);
      for (size_t j = 0; j < 8; ++j) {
        problem.condEquivalence(thisEquality, initialMemoryReads[i].second[j], result[j]);
      }
    }
    for (size_t i = 0; i < memoryWrites.size(); ++i) {
      result = problem.ite(problem.equal(addr, memoryWrites[i].first), memoryWrites[i].second, result);
    }
    initialMemoryReads.push_back(make_pair(addr, result)); // If address doesn't alias any previous reads or writes
    return result;
  }

  void writeMemoryByte(const LitList(32)& addr, const LitList(8)& data) {
    memoryWrites.push_back(make_pair(addr, data));
  }

  template <size_t Len>
  void writeMemory(const LitList(32)& addr, const LitList(Len)& data) {
    WriteMemoryHelper<Len, 0, NetlistTranslator>::go(addr, data, *this);
  }

  template <size_t Len>
  LitList(Len) read(SgAsmExpression* e) {
    switch (e->variantT()) {
      case V_SgAsmx86RegisterReferenceExpression: {
        SgAsmx86RegisterReferenceExpression* rre = isSgAsmx86RegisterReferenceExpression(e);
        switch (rre->get_register_class()) {
          case x86_regclass_gpr: {
            X86GeneralPurposeRegister reg = (X86GeneralPurposeRegister)(rre->get_register_number());
            return extractRead<Len>(vars(gpr(reg)), rre->get_position_in_register());
          }
          default: fprintf(stderr, "Bad register class %u\n", rre->get_register_class()); abort();
        }
        break;
      }
      case V_SgAsmBinaryAdd: {
        return problem.adder(read<Len>(isSgAsmBinaryAdd(e)->get_lhs()), read<Len>(isSgAsmBinaryAdd(e)->get_rhs()));
      }
      case V_SgAsmBinaryMultiply: {
        SgAsmByteValueExpression* rhs = isSgAsmByteValueExpression(isSgAsmBinaryMultiply(e)->get_rhs());
        ROSE_ASSERT (rhs);
        SgAsmExpression* lhs = isSgAsmBinaryMultiply(e)->get_lhs();
        switch (rhs->get_value()) {
          case 1: return read<Len>(lhs);
          case 2: return concat(number<1>(0), extract<0, Len - 1>(read<Len>(lhs)));
          case 4: return concat(number<2>(0), extract<0, Len - 2>(read<Len>(lhs)));
          case 8: return concat(number<3>(0), extract<0, Len - 3>(read<Len>(lhs)));
          default: ROSE_ASSERT (!"Bad scaling factor");
        }
      }
      case V_SgAsmMemoryReferenceExpression: {
        return readMemory<Len>(readEffectiveAddress(e));
      }
      case V_SgAsmByteValueExpression: {
        uint64_t val = isSgAsmByteValueExpression(e)->get_value();
        ROSE_ASSERT ((val >> Len) == 0);
        return number<Len>((unsigned int)(val));
      }
      case V_SgAsmWordValueExpression: {
        uint64_t val = isSgAsmWordValueExpression(e)->get_value();
        ROSE_ASSERT ((val >> Len) == 0);
        return number<Len>((unsigned int)(val));
      }
      case V_SgAsmDoubleWordValueExpression: {
        uint64_t val = isSgAsmDoubleWordValueExpression(e)->get_value();
        ROSE_ASSERT ((val >> Len) == 0);
        return number<Len>((unsigned int)(val));
      }
      case V_SgAsmQuadWordValueExpression: {
        uint64_t val = isSgAsmQuadWordValueExpression(e)->get_value();
        ROSE_ASSERT ((val >> Len) == 0);
        return number<Len>((unsigned int)(val));
      }
      default: fprintf(stderr, "Bad variant %s in read\n", e->class_name().c_str()); abort();
    }
  }

  template <size_t Len>
  void write(SgAsmExpression* e, const LitList(Len)& value) {
    switch (e->variantT()) {
      case V_SgAsmx86RegisterReferenceExpression: {
        SgAsmx86RegisterReferenceExpression* rre = isSgAsmx86RegisterReferenceExpression(e);
        switch (rre->get_register_class()) {
          case x86_regclass_gpr: {
            X86GeneralPurposeRegister reg = (X86GeneralPurposeRegister)(rre->get_register_number());
            LitList(32) newValue = blendInWrittenValue<Len>(vars(gpr(reg)), rre->get_position_in_register(), value);
            vars(gpr(reg)) = newValue;
            break;
          }
          default: fprintf(stderr, "Bad register class %u\n", rre->get_register_class()); abort();
        }
        break;
      }
      case V_SgAsmMemoryReferenceExpression: {
        writeMemory<Len>(readEffectiveAddress(e), value);
        break;
      }
      default: fprintf(stderr, "Bad variant %s in write\n", e->class_name().c_str()); abort();
    }
  }

  template <size_t Len>
  LitList(Len) extractRead(const LitList(32)& fullDWord, X86PositionInRegister pos) const {
    return ExtractReadHelper<Len>::go(fullDWord, pos);
  }

  template <size_t Len>
  LitList(32) blendInWrittenValue(const LitList(32)& oldDWord, X86PositionInRegister pos, const LitList(Len)& newValue) const {
    switch (pos) {
      case x86_regpos_all: // The copy is for static type safety
      case x86_regpos_dword: {
        ROSE_ASSERT (Len == 32);
        LitList(32) newVal = oldDWord;
        for (size_t i = 0; i < 32; ++i) newVal[i] = newValue[i];
        return newVal;
      }

      case x86_regpos_word: {
        ROSE_ASSERT (Len == 16);
        LitList(32) newVal = oldDWord;
        for (size_t i = 0; i < 16; ++i) newVal[i] = newValue[i];
        return newVal;
      }

      case x86_regpos_low_byte: {
        ROSE_ASSERT (Len == 8);
        LitList(32) newVal = oldDWord;
        for (size_t i = 0; i < 8; ++i) newVal[i] = newValue[i];
        return newVal;
      }

      case x86_regpos_high_byte: {
        ROSE_ASSERT (Len == 8);
        LitList(32) newVal = oldDWord;
        for (size_t i = 0; i < 8; ++i) newVal[i + 8] = newValue[i];
        return newVal;
      }

      default: ROSE_ASSERT (!"Bad position in register");
    }
  }

  template <size_t Len>
  void setFlagsForResult(const LitList(Len)& result, Lit cond = TRUE) {
    flag(x86flag_pf) = problem.mux(cond, problem.xorAcross(extract<0, 8>(result)), flag(x86flag_pf));
    flag(x86flag_sf) = problem.mux(cond, result[Len - 1], flag(x86flag_sf));
    flag(x86flag_zf) = problem.mux(cond, problem.norAcross(result), flag(x86flag_zf));
  }

  template <size_t Len>
  LitList(Len) doAddOperation(const LitList(Len)& a, const LitList(Len)& b, bool invertCarries, Lit carryIn, Lit cond) { // Does add (subtract with two's complement input and invertCarries set), and sets correct flags; only does this if cond is true
    LitList(Len) carries;
    LitList(Len) result = problem.adder(a, b, invertMaybe(carryIn, invertCarries), &carries);
    setFlagsForResult(result, cond);
    flag(x86flag_af) = problem.mux(cond, invertMaybe(carries[3], invertCarries), flag(x86flag_af));
    flag(x86flag_cf) = problem.mux(cond, invertMaybe(carries[Len - 1], invertCarries), flag(x86flag_cf));
    flag(x86flag_of) = problem.mux(cond, problem.xorGate(carries[Len - 1], carries[Len - 2]), flag(x86flag_of));
    return result;
  }

  template <size_t Len>
  LitList(Len) doIncOperation(const LitList(Len)& a, bool dec, bool setCarry) { // Does inc (dec with dec set), and sets correct flags
    LitList(Len) carries;
    LitList(Len) result = problem.adder(a, number<Len>(dec ? -1 : 1), FALSE, &carries);
    setFlagsForResult(result);
    flag(x86flag_af) = invertMaybe(carries[3], dec);
    flag(x86flag_of) = problem.xorGate(carries[Len - 1], carries[Len - 2]);
    if (setCarry) flag(x86flag_cf) = carries[Len - 1];
    return result;
  }

  void translate(SgAsmx86Instruction* insn) {
    vars(ip()) = number<32>((unsigned int)(insn->get_address() + insn->get_raw_bytes().size()));
    X86InstructionKind kind = insn->get_kind();
    const SgAsmExpressionPtrList& operands = insn->get_operandList()->get_operands();
    switch (kind) {
      case x86_mov: {
        ROSE_ASSERT (operands.size() == 2);
        switch (numBytesInAsmType(operands[0]->get_type())) {
          case 1: write(operands[0], read<8>(operands[1])); break;
          case 2: write(operands[0], read<16>(operands[1])); break;
          case 4: write(operands[0], read<32>(operands[1])); break;
          default: ROSE_ASSERT ("Bad size"); break;
        }
        break;
      }
      case x86_xchg: {
        ROSE_ASSERT (operands.size() == 2);
        switch (numBytesInAsmType(operands[0]->get_type())) {
          case 1: {LitList(8) temp = read<8>(operands[1]); write(operands[1], read<8>(operands[0])); write(operands[0], temp); break;}
          case 2: {LitList(16) temp = read<16>(operands[1]); write(operands[1], read<16>(operands[0])); write(operands[0], temp); break;}
          case 4: {LitList(32) temp = read<32>(operands[1]); write(operands[1], read<32>(operands[0])); write(operands[0], temp); break;}
          default: ROSE_ASSERT ("Bad size"); break;
        }
        break;
      }
      case x86_movzx: {
        ROSE_ASSERT (operands.size() == 2);
        switch (numBytesInAsmType(operands[0]->get_type())) {
          case 2: write(operands[0], concat(read<8>(operands[1]), number<8>(0))); break;
          case 4: {
            switch (numBytesInAsmType(operands[1]->get_type())) {
              case 1: write(operands[0], concat(read<8>(operands[1]), number<24>(0))); break;
              case 2: write(operands[0], concat(read<16>(operands[1]), number<16>(0))); break;
              default: ROSE_ASSERT ("Bad size");
            }
            break;
          }
          default: ROSE_ASSERT ("Bad size"); break;
        }
        break;
      }
      case x86_movsx: {
        ROSE_ASSERT (operands.size() == 2);
        switch (numBytesInAsmType(operands[0]->get_type())) {
          case 2: {
            LitList(8) op1 = read<8>(operands[1]);
            LitList(16) result;
            for (size_t i = 0; i < 16; ++i) result[i] = (i < 8 ? op1[i] : op1[7]);
            write(operands[0], result);
            break;
          }
          case 4: {
            switch (numBytesInAsmType(operands[1]->get_type())) {
              case 1: {
                LitList(8) op1 = read<8>(operands[1]);
                LitList(32) result;
                for (size_t i = 0; i < 32; ++i) result[i] = (i < 8 ? op1[i] : op1[7]);
                write(operands[0], result);
                break;
              }
              case 2: {
                LitList(16) op1 = read<16>(operands[1]);
                LitList(32) result;
                for (size_t i = 0; i < 32; ++i) result[i] = (i < 16 ? op1[i] : op1[15]);
                write(operands[0], result);
                break;
              }
              default: ROSE_ASSERT ("Bad size");
            }
            break;
          }
          default: ROSE_ASSERT ("Bad size"); break;
        }
        break;
      }
      case x86_cbw: {
        ROSE_ASSERT (operands.size() == 0);
        LitList(32) result = vars(gpr(x86_gpr_ax));
        Lit signbit = result[7];
        for (size_t i = 8; i < 15; ++i) result[i] = signbit;
        vars(gpr(x86_gpr_ax)) = result;
        break;
      }
      case x86_cwde: {
        ROSE_ASSERT (operands.size() == 0);
        LitList(32) result = vars(gpr(x86_gpr_ax));
        Lit signbit = result[15];
        for (size_t i = 16; i < 32; ++i) result[i] = signbit;
        vars(gpr(x86_gpr_ax)) = result;
        break;
      }
      case x86_cwd: {
        ROSE_ASSERT (operands.size() == 0);
        LitList(32) result = vars(gpr(x86_gpr_dx));
        Lit signbit = vars(gpr(x86_gpr_ax))[15];
        for (size_t i = 0; i < 15; ++i) result[i] = signbit;
        vars(gpr(x86_gpr_dx)) = result;
        break;
      }
      case x86_cdq: {
        ROSE_ASSERT (operands.size() == 0);
        LitList(32) result;
        Lit signbit = vars(gpr(x86_gpr_ax))[31];
        for (size_t i = 0; i < 32; ++i) result[i] = signbit;
        vars(gpr(x86_gpr_dx)) = result;
        break;
      }
      case x86_lea: {
        ROSE_ASSERT (operands.size() == 2);
        write(operands[0], readEffectiveAddress(operands[1]));
        break;
      }
      case x86_and: {
        ROSE_ASSERT (operands.size() == 2);
        switch (numBytesInAsmType(operands[0]->get_type())) {
          case 1: {
            LitList(8) result = problem.andWords(read<8>(operands[0]), read<8>(operands[1]));
            setFlagsForResult(result);
            write(operands[0], result);
            break;
          }
          case 2: {
            LitList(16) result = problem.andWords(read<16>(operands[0]), read<16>(operands[1]));
            setFlagsForResult(result);
            write(operands[0], result);
            break;
          }
          case 4: {
            LitList(32) result = problem.andWords(read<32>(operands[0]), read<32>(operands[1]));
            setFlagsForResult(result);
            write(operands[0], result);
            break;
          }
          default: ROSE_ASSERT (!"Bad size"); break;
        }
        flag(x86flag_of) = FALSE;
        flag(x86flag_af) = FALSE; // Undefined
        flag(x86flag_cf) = FALSE;
        break;
      }
      case x86_or: {
        ROSE_ASSERT (operands.size() == 2);
        switch (numBytesInAsmType(operands[0]->get_type())) {
          case 1: {
            LitList(8) result = problem.orWords(read<8>(operands[0]), read<8>(operands[1]));
            setFlagsForResult(result);
            write(operands[0], result);
            break;
          }
          case 2: {
            LitList(16) result = problem.orWords(read<16>(operands[0]), read<16>(operands[1]));
            setFlagsForResult(result);
            write(operands[0], result);
            break;
          }
          case 4: {
            LitList(32) result = problem.orWords(read<32>(operands[0]), read<32>(operands[1]));
            setFlagsForResult(result);
            write(operands[0], result);
            break;
          }
          default: ROSE_ASSERT (!"Bad size"); break;
        }
        flag(x86flag_of) = FALSE;
        flag(x86flag_af) = FALSE; // Undefined
        flag(x86flag_cf) = FALSE;
        break;
      }
      case x86_test: {
        ROSE_ASSERT (operands.size() == 2);
        switch (numBytesInAsmType(operands[0]->get_type())) {
          case 1: {
            LitList(8) result = problem.andWords(read<8>(operands[0]), read<8>(operands[1]));
            setFlagsForResult(result);
            break;
          }
          case 2: {
            LitList(16) result = problem.andWords(read<16>(operands[0]), read<16>(operands[1]));
            setFlagsForResult(result);
            break;
          }
          case 4: {
            LitList(32) result = problem.andWords(read<32>(operands[0]), read<32>(operands[1]));
            setFlagsForResult(result);
            break;
          }
          default: ROSE_ASSERT (!"Bad size"); break;
        }
        flag(x86flag_of) = FALSE;
        flag(x86flag_af) = FALSE; // Undefined
        flag(x86flag_cf) = FALSE;
        break;
      }
      case x86_xor: {
        ROSE_ASSERT (operands.size() == 2);
        switch (numBytesInAsmType(operands[0]->get_type())) {
          case 1: {
            LitList(8) result = problem.xorWords(read<8>(operands[0]), read<8>(operands[1]));
            setFlagsForResult(result);
            write(operands[0], result);
            break;
          }
          case 2: {
            LitList(16) result = problem.xorWords(read<16>(operands[0]), read<16>(operands[1]));
            setFlagsForResult(result);
            write(operands[0], result);
            break;
          }
          case 4: {
            LitList(32) result = problem.xorWords(read<32>(operands[0]), read<32>(operands[1]));
            setFlagsForResult(result);
            write(operands[0], result);
            break;
          }
          default: ROSE_ASSERT (!"Bad size"); break;
        }
        flag(x86flag_of) = FALSE;
        flag(x86flag_af) = FALSE; // Undefined
        flag(x86flag_cf) = FALSE;
        break;
      }
      case x86_not: {
        ROSE_ASSERT (operands.size() == 1);
        switch (numBytesInAsmType(operands[0]->get_type())) {
          case 1: {
            LitList(8) result = invertWord(read<8>(operands[0]));
            write(operands[0], result);
            break;
          }
          case 2: {
            LitList(16) result = invertWord(read<16>(operands[0]));
            write(operands[0], result);
            break;
          }
          case 4: {
            LitList(32) result = invertWord(read<32>(operands[0]));
            write(operands[0], result);
            break;
          }
          default: ROSE_ASSERT (!"Bad size"); break;
        }
        break;
      }
      case x86_add: {
        ROSE_ASSERT (operands.size() == 2);
        switch (numBytesInAsmType(operands[0]->get_type())) {
          case 1: {
            LitList(8) result = doAddOperation(read<8>(operands[0]), read<8>(operands[1]), false, FALSE, TRUE);
            write(operands[0], result);
            break;
          }
          case 2: {
            LitList(16) result = doAddOperation(read<16>(operands[0]), read<16>(operands[1]), false, FALSE, TRUE);
            write(operands[0], result);
            break;
          }
          case 4: {
            LitList(32) result = doAddOperation(read<32>(operands[0]), read<32>(operands[1]), false, FALSE, TRUE);
            write(operands[0], result);
            break;
          }
          default: ROSE_ASSERT (!"Bad size"); break;
        }
        break;
      }
      case x86_adc: {
        ROSE_ASSERT (operands.size() == 2);
        switch (numBytesInAsmType(operands[0]->get_type())) {
          case 1: {
            LitList(8) result = doAddOperation(read<8>(operands[0]), read<8>(operands[1]), false, flag(x86flag_cf), TRUE);
            write(operands[0], result);
            break;
          }
          case 2: {
            LitList(16) result = doAddOperation(read<16>(operands[0]), read<16>(operands[1]), false, flag(x86flag_cf), TRUE);
            write(operands[0], result);
            break;
          }
          case 4: {
            LitList(32) result = doAddOperation(read<32>(operands[0]), read<32>(operands[1]), false, flag(x86flag_cf), TRUE);
            write(operands[0], result);
            break;
          }
          default: ROSE_ASSERT (!"Bad size"); break;
        }
        break;
      }
      case x86_sub: {
        ROSE_ASSERT (operands.size() == 2);
        switch (numBytesInAsmType(operands[0]->get_type())) {
          case 1: {
            LitList(8) result = doAddOperation(read<8>(operands[0]), invertWord(read<8>(operands[1])), true, FALSE, TRUE);
            write(operands[0], result);
            break;
          }
          case 2: {
            LitList(16) result = doAddOperation(read<16>(operands[0]), invertWord(read<16>(operands[1])), true, FALSE, TRUE);
            write(operands[0], result);
            break;
          }
          case 4: {
            LitList(32) result = doAddOperation(read<32>(operands[0]), invertWord(read<32>(operands[1])), true, FALSE, TRUE);
            write(operands[0], result);
            break;
          }
          default: ROSE_ASSERT (!"Bad size"); break;
        }
        break;
      }
      case x86_sbb: {
        ROSE_ASSERT (operands.size() == 2);
        switch (numBytesInAsmType(operands[0]->get_type())) {
          case 1: {
            LitList(8) result = doAddOperation(read<8>(operands[0]), invertWord(read<8>(operands[1])), true, flag(x86flag_cf), TRUE);
            write(operands[0], result);
            break;
          }
          case 2: {
            LitList(16) result = doAddOperation(read<16>(operands[0]), invertWord(read<16>(operands[1])), true, flag(x86flag_cf), TRUE);
            write(operands[0], result);
            break;
          }
          case 4: {
            LitList(32) result = doAddOperation(read<32>(operands[0]), invertWord(read<32>(operands[1])), true, flag(x86flag_cf), TRUE);
            write(operands[0], result);
            break;
          }
          default: ROSE_ASSERT (!"Bad size"); break;
        }
        break;
      }
      case x86_cmp: {
        ROSE_ASSERT (operands.size() == 2);
        switch (numBytesInAsmType(operands[0]->get_type())) {
          case 1: {
            doAddOperation(read<8>(operands[0]), invertWord(read<8>(operands[1])), true, FALSE, TRUE);
            break;
          }
          case 2: {
            doAddOperation(read<16>(operands[0]), invertWord(read<16>(operands[1])), true, FALSE, TRUE);
            break;
          }
          case 4: {
            doAddOperation(read<32>(operands[0]), invertWord(read<32>(operands[1])), true, FALSE, TRUE);
            break;
          }
          default: ROSE_ASSERT (!"Bad size"); break;
        }
        break;
      }
      case x86_neg: {
        ROSE_ASSERT (operands.size() == 1);
        switch (numBytesInAsmType(operands[0]->get_type())) {
          case 1: {
            LitList(8) result = doIncOperation(invertWord(read<8>(operands[0])), false, true);
            write(operands[0], result);
            break;
          }
          case 2: {
            LitList(16) result = doIncOperation(invertWord(read<16>(operands[0])), false, true);
            write(operands[0], result);
            break;
          }
          case 4: {
            LitList(32) result = doIncOperation(invertWord(read<32>(operands[0])), false, true);
            write(operands[0], result);
            break;
          }
          default: ROSE_ASSERT (!"Bad size"); break;
        }
        break;
      }
      case x86_inc: {
        ROSE_ASSERT (operands.size() == 1);
        switch (numBytesInAsmType(operands[0]->get_type())) {
          case 1: {
            LitList(8) result = doIncOperation(read<8>(operands[0]), false, false);
            write(operands[0], result);
            break;
          }
          case 2: {
            LitList(16) result = doIncOperation(read<16>(operands[0]), false, false);
            write(operands[0], result);
            break;
          }
          case 4: {
            LitList(32) result = doIncOperation(read<32>(operands[0]), false, false);
            write(operands[0], result);
            break;
          }
          default: ROSE_ASSERT (!"Bad size"); break;
        }
        break;
      }
      case x86_dec: {
        ROSE_ASSERT (operands.size() == 1);
        switch (numBytesInAsmType(operands[0]->get_type())) {
          case 1: {
            LitList(8) result = doIncOperation(read<8>(operands[0]), true, false);
            write(operands[0], result);
            break;
          }
          case 2: {
            LitList(16) result = doIncOperation(read<16>(operands[0]), true, false);
            write(operands[0], result);
            break;
          }
          case 4: {
            LitList(32) result = doIncOperation(read<32>(operands[0]), true, false);
            write(operands[0], result);
            break;
          }
          default: ROSE_ASSERT (!"Bad size"); break;
        }
        break;
      }
      case x86_shl: {
        switch (numBytesInAsmType(operands[0]->get_type())) {
          case 1: {
            LitList(8) op = read<8>(operands[0]);
            LitList(5) shiftCount = extract<0, 5>(read<8>(operands[1]));
            LitList(9) opWithCf = concat(op, single(flag(x86flag_cf)));
            LitList(9) outputWithCf = problem.rightShifter(opWithCf, shiftCount); // Because op is LSB first
            flag(x86flag_cf) = outputWithCf[8];
            flag(x86flag_of) = problem.xorGate(outputWithCf[7], outputWithCf[8]);
            write(operands[0], extract<0, 8>(outputWithCf));
            setFlagsForResult(extract<0, 8>(outputWithCf));
            flag(x86flag_af) = FALSE; // Undefined
            break;
          }
          case 2: {
            LitList(16) op = read<16>(operands[0]);
            LitList(5) shiftCount = extract<0, 5>(read<8>(operands[1]));
            LitList(17) opWithCf = concat(op, single(flag(x86flag_cf)));
            LitList(17) outputWithCf = problem.rightShifter(opWithCf, shiftCount); // Because op is LSB first
            flag(x86flag_cf) = outputWithCf[16];
            flag(x86flag_of) = problem.xorGate(outputWithCf[15], outputWithCf[16]);
            write(operands[0], extract<0, 16>(outputWithCf));
            setFlagsForResult(extract<0, 16>(outputWithCf));
            flag(x86flag_af) = FALSE; // Undefined
            break;
          }
          case 4: {
            LitList(32) op = read<32>(operands[0]);
            LitList(5) shiftCount = extract<0, 5>(read<8>(operands[1]));
            LitList(33) opWithCf = concat(op, single(flag(x86flag_cf)));
            LitList(33) outputWithCf = problem.rightShifter(opWithCf, shiftCount); // Because op is LSB first
            flag(x86flag_cf) = outputWithCf[32];
            flag(x86flag_of) = problem.xorGate(outputWithCf[31], outputWithCf[32]);
            write(operands[0], extract<0, 32>(outputWithCf));
            setFlagsForResult(extract<0, 32>(outputWithCf));
            flag(x86flag_af) = FALSE; // Undefined
            break;
          }
          default: ROSE_ASSERT (!"Bad size");
        }
        break;
      }
      case x86_shr: {
        switch (numBytesInAsmType(operands[0]->get_type())) {
          case 1: {
            LitList(8) op = read<8>(operands[0]);
            LitList(5) shiftCount = extract<0, 5>(read<8>(operands[1]));
            LitList(9) opWithCf = concat(op, single(flag(x86flag_cf)));
            LitList(9) outputWithCf = problem.leftShifter(opWithCf, shiftCount); // Because op is LSB first
            flag(x86flag_cf) = outputWithCf[8];
            flag(x86flag_of) = problem.xorGate(outputWithCf[7], outputWithCf[8]);
            write(operands[0], extract<0, 8>(outputWithCf));
            setFlagsForResult(extract<0, 8>(outputWithCf));
            flag(x86flag_af) = FALSE; // Undefined
            break;
          }
          case 2: {
            LitList(16) op = read<16>(operands[0]);
            LitList(5) shiftCount = extract<0, 5>(read<8>(operands[1]));
            LitList(17) opWithCf = concat(op, single(flag(x86flag_cf)));
            LitList(17) outputWithCf = problem.leftShifter(opWithCf, shiftCount); // Because op is LSB first
            flag(x86flag_cf) = outputWithCf[16];
            flag(x86flag_of) = problem.xorGate(outputWithCf[15], outputWithCf[16]);
            write(operands[0], extract<0, 16>(outputWithCf));
            setFlagsForResult(extract<0, 16>(outputWithCf));
            flag(x86flag_af) = FALSE; // Undefined
            break;
          }
          case 4: {
            LitList(32) op = read<32>(operands[0]);
            LitList(5) shiftCount = extract<0, 5>(read<8>(operands[1]));
            LitList(33) opWithCf = concat(op, single(flag(x86flag_cf)));
            LitList(33) outputWithCf = problem.leftShifter(opWithCf, shiftCount); // Because op is LSB first
            flag(x86flag_cf) = outputWithCf[32];
            flag(x86flag_of) = problem.xorGate(outputWithCf[31], outputWithCf[32]);
            write(operands[0], extract<0, 32>(outputWithCf));
            setFlagsForResult(extract<0, 32>(outputWithCf));
            flag(x86flag_af) = FALSE; // Undefined
            break;
          }
          default: ROSE_ASSERT (!"Bad size");
        }
        break;
      }
      case x86_sar: {
        switch (numBytesInAsmType(operands[0]->get_type())) {
          case 1: {
            LitList(8) op = read<8>(operands[0]);
            LitList(5) shiftCount = extract<0, 5>(read<8>(operands[1]));
            LitList(9) opWithCf = concat(op, single(flag(x86flag_cf)));
            LitList(9) outputWithCf = problem.arithmeticLeftShifter(opWithCf, shiftCount); // Because op is LSB first
            flag(x86flag_cf) = outputWithCf[8];
            flag(x86flag_of) = problem.xorGate(outputWithCf[7], outputWithCf[8]);
            write(operands[0], extract<0, 8>(outputWithCf));
            setFlagsForResult(extract<0, 8>(outputWithCf));
            flag(x86flag_af) = FALSE; // Undefined
            break;
          }
          case 2: {
            LitList(16) op = read<16>(operands[0]);
            LitList(5) shiftCount = extract<0, 5>(read<8>(operands[1]));
            LitList(17) opWithCf = concat(op, single(flag(x86flag_cf)));
            LitList(17) outputWithCf = problem.arithmeticLeftShifter(opWithCf, shiftCount); // Because op is LSB first
            flag(x86flag_cf) = outputWithCf[16];
            flag(x86flag_of) = problem.xorGate(outputWithCf[15], outputWithCf[16]);
            write(operands[0], extract<0, 16>(outputWithCf));
            setFlagsForResult(extract<0, 16>(outputWithCf));
            flag(x86flag_af) = FALSE; // Undefined
            break;
          }
          case 4: {
            LitList(32) op = read<32>(operands[0]);
            LitList(5) shiftCount = extract<0, 5>(read<8>(operands[1]));
            LitList(33) opWithCf = concat(op, single(flag(x86flag_cf)));
            LitList(33) outputWithCf = problem.arithmeticLeftShifter(opWithCf, shiftCount); // Because op is LSB first
            flag(x86flag_cf) = outputWithCf[32];
            flag(x86flag_of) = problem.xorGate(outputWithCf[31], outputWithCf[32]);
            write(operands[0], extract<0, 32>(outputWithCf));
            setFlagsForResult(extract<0, 32>(outputWithCf));
            flag(x86flag_af) = FALSE; // Undefined
            break;
          }
          default: ROSE_ASSERT (!"Bad size");
        }
        break;
      }
      case x86_rol: {
        switch (numBytesInAsmType(operands[0]->get_type())) {
          case 1: {
            LitList(8) op = read<8>(operands[0]);
            LitList(5) shiftCount = extract<0, 5>(read<8>(operands[1]));
            LitList(8) output = problem.rightRotater(op, shiftCount); // Because op is LSB first
            flag(x86flag_cf) = problem.mux(problem.norAcross(shiftCount), flag(x86flag_cf), output[0]);
            flag(x86flag_of) = problem.mux(problem.norAcross(shiftCount), flag(x86flag_of), problem.xorGate(output[0], output[7]));
            write(operands[0], output);
            break;
          }
          case 2: {
            LitList(16) op = read<16>(operands[0]);
            LitList(5) shiftCount = extract<0, 5>(read<8>(operands[1]));
            LitList(16) output = problem.rightRotater(op, shiftCount); // Because op is LSB first
            flag(x86flag_cf) = problem.mux(problem.norAcross(shiftCount), flag(x86flag_cf), output[0]);
            flag(x86flag_of) = problem.mux(problem.norAcross(shiftCount), flag(x86flag_of), problem.xorGate(output[0], output[15]));
            write(operands[0], output);
            break;
          }
          case 4: {
            LitList(32) op = read<32>(operands[0]);
            LitList(5) shiftCount = extract<0, 5>(read<8>(operands[1]));
            LitList(32) output = problem.rightRotater(op, shiftCount); // Because op is LSB first
            flag(x86flag_cf) = problem.mux(problem.norAcross(shiftCount), flag(x86flag_cf), output[0]);
            flag(x86flag_of) = problem.mux(problem.norAcross(shiftCount), flag(x86flag_of), problem.xorGate(output[0], output[31]));
            write(operands[0], output);
            break;
          }
          default: ROSE_ASSERT (!"Bad size");
        }
        break;
      }
      case x86_ror: {
        switch (numBytesInAsmType(operands[0]->get_type())) {
          case 1: {
            LitList(8) op = read<8>(operands[0]);
            LitList(5) shiftCount = extract<0, 5>(read<8>(operands[1]));
            LitList(8) output = problem.leftRotater(op, shiftCount); // Because op is LSB first
            flag(x86flag_cf) = problem.mux(problem.norAcross(shiftCount), flag(x86flag_cf), output[7]);
            flag(x86flag_of) = problem.mux(problem.norAcross(shiftCount), flag(x86flag_of), problem.xorGate(output[6], output[7]));
            write(operands[0], output);
            break;
          }
          case 2: {
            LitList(16) op = read<16>(operands[0]);
            LitList(5) shiftCount = extract<0, 5>(read<8>(operands[1]));
            LitList(16) output = problem.leftRotater(op, shiftCount); // Because op is LSB first
            flag(x86flag_cf) = problem.mux(problem.norAcross(shiftCount), flag(x86flag_cf), output[15]);
            flag(x86flag_of) = problem.mux(problem.norAcross(shiftCount), flag(x86flag_of), problem.xorGate(output[14], output[15]));
            write(operands[0], output);
            break;
          }
          case 4: {
            LitList(32) op = read<32>(operands[0]);
            LitList(5) shiftCount = extract<0, 5>(read<8>(operands[1]));
            LitList(32) output = problem.leftRotater(op, shiftCount); // Because op is LSB first
            flag(x86flag_cf) = problem.mux(problem.norAcross(shiftCount), flag(x86flag_cf), output[31]);
            flag(x86flag_of) = problem.mux(problem.norAcross(shiftCount), flag(x86flag_of), problem.xorGate(output[30], output[31]));
            write(operands[0], output);
            break;
          }
          default: ROSE_ASSERT (!"Bad size");
        }
        break;
      }
      case x86_shld: { // FIXME check this
        switch (numBytesInAsmType(operands[0]->get_type())) {
          case 2: {
            LitList(32) op = concat(read<16>(operands[1]), read<16>(operands[0]));
            LitList(5) shiftCount = extract<0, 5>(read<8>(operands[2]));
            LitList(33) opWithCf = concat(op, single(flag(x86flag_cf)));
            LitList(33) outputWithCf = problem.rightShifter(opWithCf, shiftCount); // Because op is LSB first
            flag(x86flag_cf) = outputWithCf[32];
            flag(x86flag_of) = problem.xorGate(outputWithCf[31], outputWithCf[32]);
            write(operands[0], extract<16, 32>(outputWithCf));
            setFlagsForResult(extract<16, 32>(outputWithCf));
            flag(x86flag_af) = FALSE; // Undefined
            break;
          }
          case 4: {
            LitList(64) op = concat(read<32>(operands[1]), read<32>(operands[0]));
            LitList(5) shiftCount = extract<0, 5>(read<8>(operands[2]));
            LitList(65) opWithCf = concat(op, single(flag(x86flag_cf)));
            LitList(65) outputWithCf = problem.rightShifter(opWithCf, shiftCount); // Because op is LSB first
            flag(x86flag_cf) = outputWithCf[64];
            flag(x86flag_of) = problem.xorGate(outputWithCf[63], outputWithCf[64]);
            write(operands[0], extract<32, 64>(outputWithCf));
            setFlagsForResult(extract<32, 64>(outputWithCf));
            flag(x86flag_af) = FALSE; // Undefined
            break;
          }
          default: ROSE_ASSERT (!"Bad size");
        }
        break;
      }
      case x86_shrd: { // FIXME check this
        switch (numBytesInAsmType(operands[0]->get_type())) {
          case 2: {
            LitList(32) op = concat(read<16>(operands[0]), read<16>(operands[1]));
            LitList(5) shiftCount = extract<0, 5>(read<8>(operands[2]));
            LitList(33) opWithCf = concat(op, single(flag(x86flag_cf)));
            LitList(33) outputWithCf = problem.leftShifter(opWithCf, shiftCount); // Because op is LSB first
            flag(x86flag_cf) = outputWithCf[16];
            flag(x86flag_of) = problem.xorGate(outputWithCf[15], outputWithCf[16]);
            write(operands[0], extract<0, 16>(outputWithCf));
            setFlagsForResult(extract<0, 16>(outputWithCf));
            flag(x86flag_af) = FALSE; // Undefined
            break;
          }
          case 4: {
            LitList(64) op = concat(read<32>(operands[0]), read<32>(operands[1]));
            LitList(5) shiftCount = extract<0, 5>(read<8>(operands[2]));
            LitList(65) opWithCf = concat(op, single(flag(x86flag_cf)));
            LitList(65) outputWithCf = problem.leftShifter(opWithCf, shiftCount); // Because op is LSB first
            flag(x86flag_cf) = outputWithCf[32];
            flag(x86flag_of) = problem.xorGate(outputWithCf[31], outputWithCf[32]);
            write(operands[0], extract<0, 32>(outputWithCf));
            setFlagsForResult(extract<0, 32>(outputWithCf));
            flag(x86flag_af) = FALSE; // Undefined
            break;
          }
          default: ROSE_ASSERT (!"Bad size");
        }
        break;
      }
      case x86_bsf: {
        flag(x86flag_of) = FALSE; // Undefined
        flag(x86flag_sf) = FALSE; // Undefined
        flag(x86flag_af) = FALSE; // Undefined
        flag(x86flag_pf) = FALSE; // Undefined
        flag(x86flag_cf) = FALSE; // Undefined
        switch (numBytesInAsmType(operands[0]->get_type())) {
          case 2: {
            LitList(16) op = read<16>(operands[1]);
            flag(x86flag_zf) = problem.norAcross(op);
            LitList(16) result = problem.ite(flag(x86flag_zf),
                                             read<16>(operands[0]),
                                             problem.leastSignificantSetBit(op));
            write(operands[0], result);
            break;
          }
          case 4: {
            LitList(32) op = read<32>(operands[1]);
              flag(x86flag_zf) = problem.norAcross(op);
              LitList(32) result = problem.ite(flag(x86flag_zf),
                                             read<32>(operands[0]),
                                             problem.leastSignificantSetBit(op));
            write(operands[0], result);
            break;
          }
          default: ROSE_ASSERT (!"Bad size");
        }
        break;
      }
      case x86_bsr: {
        flag(x86flag_of) = FALSE; // Undefined
        flag(x86flag_sf) = FALSE; // Undefined
        flag(x86flag_af) = FALSE; // Undefined
        flag(x86flag_pf) = FALSE; // Undefined
        flag(x86flag_cf) = FALSE; // Undefined
        switch (numBytesInAsmType(operands[0]->get_type())) {
          case 2: {
            LitList(16) op = read<16>(operands[1]);
            flag(x86flag_zf) = problem.norAcross(op);
            LitList(16) result = problem.ite(flag(x86flag_zf),
                                             read<16>(operands[0]),
                                             problem.xorWords(number<16>(15),
                                                              problem.leastSignificantSetBit(problem.bitReverse(op))));
            write(operands[0], result);
            break;
          }
          case 4: {
            LitList(32) op = read<32>(operands[1]);
            flag(x86flag_zf) = problem.norAcross(op);
            LitList(32) result = problem.ite(flag(x86flag_zf),
                                             read<32>(operands[0]),
                                             problem.xorWords(number<32>(31),
                                                              problem.leastSignificantSetBit(problem.bitReverse(op))));
            write(operands[0], result);
            break;
          }
          default: ROSE_ASSERT (!"Bad size");
        }
        break;
      }
      case x86_imul: {
        switch (numBytesInAsmType(operands[0]->get_type())) {
          case 1: {
            LitList(8) op0 = extract<0, 8>(vars(gpr(x86_gpr_ax)));
            LitList(8) op1 = read<8>(operands[0]);
            LitList(16) mulResult = problem.signedMultiplier(op0, op1);
            vars(gpr(x86_gpr_ax)) = concat(mulResult, extract<16, 32>(vars(gpr(x86_gpr_ax))));
            Lit carry = problem.norGate(problem.andAcross(extract<7, 16>(mulResult)), problem.norAcross(extract<7, 16>(mulResult)));
            flag(x86flag_cf) = flag(x86flag_of) = carry;
            break;
          }
          case 2: {
            LitList(16) op0 = operands.size() == 1 ? extract<0, 16>(vars(gpr(x86_gpr_ax))) : read<16>(operands[operands.size() - 2]);
            LitList(16) op1 = read<16>(operands[operands.size() - 1]);
            LitList(32) mulResult = problem.signedMultiplier(op0, op1);
            if (operands.size() == 1) {
              vars(gpr(x86_gpr_ax)) = concat(extract<0, 16>(mulResult), extract<16, 32>(vars(gpr(x86_gpr_ax))));
              vars(gpr(x86_gpr_dx)) = concat(extract<16, 32>(mulResult), extract<16, 32>(vars(gpr(x86_gpr_dx))));
            } else {
              write(operands[0], extract<0, 16>(mulResult));
            }
            Lit carry = problem.norGate(problem.andAcross(extract<7, 32>(mulResult)), problem.norAcross(extract<7, 32>(mulResult)));
            flag(x86flag_cf) = flag(x86flag_of) = carry;
            break;
          }
          case 4: {
            LitList(32) op0 = operands.size() == 1 ? vars(gpr(x86_gpr_ax)) : read<32>(operands[operands.size() - 2]);
            LitList(32) op1 = read<32>(operands[operands.size() - 1]);
            LitList(64) mulResult = problem.signedMultiplier(op0, op1);
            if (operands.size() == 1) {
              vars(gpr(x86_gpr_ax)) = extract<0, 32>(mulResult);
              vars(gpr(x86_gpr_dx)) = extract<32, 64>(mulResult);
            } else {
              write(operands[0], extract<0, 32>(mulResult));
            }
            Lit carry = problem.norGate(problem.andAcross(extract<7, 64>(mulResult)), problem.norAcross(extract<7, 64>(mulResult)));
            flag(x86flag_cf) = flag(x86flag_of) = carry;
            break;
          }
          default: ROSE_ASSERT (!"Bad size");
        }
        flag(x86flag_sf) = FALSE; // Undefined
        flag(x86flag_zf) = FALSE; // Undefined
        flag(x86flag_af) = FALSE; // Undefined
        flag(x86flag_pf) = FALSE; // Undefined
        break;
      }
      case x86_mul: {
        switch (numBytesInAsmType(operands[0]->get_type())) {
          case 1: {
            LitList(8) op0 = extract<0, 8>(vars(gpr(x86_gpr_ax)));
            LitList(8) op1 = read<8>(operands[0]);
            LitList(16) mulResult = problem.unsignedMultiplier(op0, op1);
            vars(gpr(x86_gpr_ax)) = concat(mulResult, extract<16, 32>(vars(gpr(x86_gpr_ax))));
            Lit carry = problem.orAcross(extract<8, 16>(mulResult));
            flag(x86flag_cf) = flag(x86flag_of) = carry;
            break;
          }
          case 2: {
            LitList(16) op0 = extract<0, 16>(vars(gpr(x86_gpr_ax)));
            LitList(16) op1 = read<16>(operands[0]);
            LitList(32) mulResult = problem.unsignedMultiplier(op0, op1);
            vars(gpr(x86_gpr_ax)) = concat(extract<0, 16>(mulResult), extract<16, 32>(vars(gpr(x86_gpr_ax))));
            vars(gpr(x86_gpr_dx)) = concat(extract<16, 32>(mulResult), extract<16, 32>(vars(gpr(x86_gpr_dx))));
            Lit carry = problem.orAcross(extract<16, 32>(mulResult));
            flag(x86flag_cf) = flag(x86flag_of) = carry;
            break;
          }
          case 4: {
            LitList(32) op0 = vars(gpr(x86_gpr_ax));
            LitList(32) op1 = read<32>(operands[0]);
            LitList(64) mulResult = problem.unsignedMultiplier(op0, op1);
            vars(gpr(x86_gpr_ax)) = extract<0, 32>(mulResult);
            vars(gpr(x86_gpr_dx)) = extract<32, 64>(mulResult);
            Lit carry = problem.orAcross(extract<32, 64>(mulResult));
            flag(x86flag_cf) = flag(x86flag_of) = carry;
            break;
          }
          default: ROSE_ASSERT (!"Bad size");
        }
        flag(x86flag_sf) = FALSE; // Undefined
        flag(x86flag_zf) = FALSE; // Undefined
        flag(x86flag_af) = FALSE; // Undefined
        flag(x86flag_pf) = FALSE; // Undefined
        break;
      }
      case x86_idiv: {
        switch (numBytesInAsmType(operands[0]->get_type())) {
          case 1: {
            LitList(16) op0 = extract<0, 16>(vars(gpr(x86_gpr_ax)));
            LitList(8) op1 = read<8>(operands[0]);
            // if op1 == 0, we should trap
            LitList(24) divResult = problem.signedDivider(op0, op1); // 16 bits quotient, 8 remainder
            // if result overflows, we should trap
            vars(gpr(x86_gpr_ax)) = concat(concat(extract<0, 8>(divResult), extract<16, 24>(divResult)), extract<16, 32>(vars(gpr(x86_gpr_ax))));
            break;
          }
          case 2: {
            LitList(32) op0 = concat(extract<0, 16>(vars(gpr(x86_gpr_ax))), extract<0, 16>(vars(gpr(x86_gpr_dx))));
            LitList(16) op1 = read<16>(operands[0]);
            // if op1 == 0, we should trap
            LitList(48) divResult = problem.signedDivider(op0, op1); // 32 bits quotient, 16 remainder
            // if result overflows, we should trap
            vars(gpr(x86_gpr_ax)) = concat(extract<0, 16>(divResult), extract<16, 32>(vars(gpr(x86_gpr_ax))));
            vars(gpr(x86_gpr_dx)) = concat(extract<32, 48>(divResult), extract<16, 32>(vars(gpr(x86_gpr_dx))));
            break;
          }
          case 4: {
            LitList(64) op0 = concat(vars(gpr(x86_gpr_ax)), vars(gpr(x86_gpr_dx)));
            LitList(32) op1 = read<32>(operands[0]);
            // if op1 == 0, we should trap
            LitList(96) divResult = problem.signedDivider(op0, op1); // 64 bits quotient, 32 remainder
            // if result overflows, we should trap
            vars(gpr(x86_gpr_ax)) = extract<0, 32>(divResult);
            vars(gpr(x86_gpr_dx)) = extract<64, 96>(divResult);
            break;
          }
          default: ROSE_ASSERT (!"Bad size");
        }
        flag(x86flag_sf) = FALSE; // Undefined
        flag(x86flag_zf) = FALSE; // Undefined
        flag(x86flag_af) = FALSE; // Undefined
        flag(x86flag_pf) = FALSE; // Undefined
        flag(x86flag_cf) = FALSE; // Undefined
        flag(x86flag_of) = FALSE; // Undefined
        break;
      }
      case x86_div: {
        switch (numBytesInAsmType(operands[0]->get_type())) {
          case 1: {
            LitList(16) op0 = extract<0, 16>(vars(gpr(x86_gpr_ax)));
            LitList(8) op1 = read<8>(operands[0]);
            // if op1 == 0, we should trap
            LitList(24) divResult = problem.unsignedDivider(op0, op1); // 16 bits quotient, 8 remainder
            // if extract<8, 16> of divResult is non-zero (overflow), we should trap
            vars(gpr(x86_gpr_ax)) = concat(concat(extract<0, 8>(divResult), extract<16, 24>(divResult)), extract<16, 32>(vars(gpr(x86_gpr_ax))));
            break;
          }
          case 2: {
            LitList(32) op0 = concat(extract<0, 16>(vars(gpr(x86_gpr_ax))), extract<0, 16>(vars(gpr(x86_gpr_dx))));
            LitList(16) op1 = read<16>(operands[0]);
            // if op1 == 0, we should trap
            LitList(48) divResult = problem.unsignedDivider(op0, op1); // 32 bits quotient, 16 remainder
            // if extract<16, 32> of divResult is non-zero (overflow), we should trap
            vars(gpr(x86_gpr_ax)) = concat(extract<0, 16>(divResult), extract<16, 32>(vars(gpr(x86_gpr_ax))));
            vars(gpr(x86_gpr_dx)) = concat(extract<32, 48>(divResult), extract<16, 32>(vars(gpr(x86_gpr_dx))));
            break;
          }
          case 4: {
            LitList(64) op0 = concat(vars(gpr(x86_gpr_ax)), vars(gpr(x86_gpr_dx)));
            LitList(32) op1 = read<32>(operands[0]);
            // if op1 == 0, we should trap
            LitList(96) divResult = problem.unsignedDivider(op0, op1); // 64 bits quotient, 32 remainder
            // if extract<32, 64> of divResult is non-zero (overflow), we should trap
            vars(gpr(x86_gpr_ax)) = extract<0, 32>(divResult);
            vars(gpr(x86_gpr_dx)) = extract<64, 96>(divResult);
            break;
          }
          default: ROSE_ASSERT (!"Bad size");
        }
        flag(x86flag_sf) = FALSE; // Undefined
        flag(x86flag_zf) = FALSE; // Undefined
        flag(x86flag_af) = FALSE; // Undefined
        flag(x86flag_pf) = FALSE; // Undefined
        flag(x86flag_cf) = FALSE; // Undefined
        flag(x86flag_of) = FALSE; // Undefined
        break;
      }
      case x86_push: {
        ROSE_ASSERT (operands.size() == 1);
        ROSE_ASSERT (insn->get_addressSize() == x86_insnsize_32);
        ROSE_ASSERT (insn->get_operandSize() == x86_insnsize_32);
        LitList(32) oldSp = vars(gpr(x86_gpr_sp));
        LitList(32) newSp = problem.adder(oldSp, number<32>(-4));
        writeMemory<32>(newSp, read<32>(operands[0]));
        vars(gpr(x86_gpr_sp)) = newSp;
        break;
      }
      case x86_pop: {
        ROSE_ASSERT (operands.size() == 1);
        ROSE_ASSERT (insn->get_addressSize() == x86_insnsize_32);
        ROSE_ASSERT (insn->get_operandSize() == x86_insnsize_32);
        LitList(32) oldSp = vars(gpr(x86_gpr_sp));
        LitList(32) newSp = problem.adder(oldSp, number<32>(4));
        write<32>(operands[0], readMemory<32>(oldSp));
        vars(gpr(x86_gpr_sp)) = newSp;
        break;
      }
      case x86_call: {
        ROSE_ASSERT (operands.size() == 1);
        ROSE_ASSERT (insn->get_addressSize() == x86_insnsize_32);
        ROSE_ASSERT (insn->get_operandSize() == x86_insnsize_32);
        LitList(32) oldSp = vars(gpr(x86_gpr_sp));
        LitList(32) newSp = problem.adder(oldSp, number<32>(-4));
        writeMemory<32>(newSp, vars(ip()));
        vars(ip()) = read<32>(operands[0]);
        vars(gpr(x86_gpr_sp)) = newSp;
        break;
      }
      case x86_ret: {
        ROSE_ASSERT (operands.size() == 0);
        ROSE_ASSERT (insn->get_addressSize() == x86_insnsize_32);
        ROSE_ASSERT (insn->get_operandSize() == x86_insnsize_32);
        LitList(32) oldSp = vars(gpr(x86_gpr_sp));
        LitList(32) newSp = problem.adder(oldSp, number<32>(4));
        vars(ip()) = readMemory<32>(oldSp);
        vars(gpr(x86_gpr_sp)) = newSp;
        break;
      }
      case x86_jmp: {
        ROSE_ASSERT (operands.size() == 1);
        vars(ip()) = read<32>(operands[0]);
        break;
      }
#define FLAGCOMBO_ne invert(flag(x86flag_zf))
#define FLAGCOMBO_e flag(x86flag_zf)
#define FLAGCOMBO_ns invert(flag(x86flag_sf))
#define FLAGCOMBO_s flag(x86flag_sf)
#define FLAGCOMBO_ae invert(flag(x86flag_cf))
#define FLAGCOMBO_b flag(x86flag_cf)
#define FLAGCOMBO_be problem.orGate(FLAGCOMBO_b, FLAGCOMBO_e)
#define FLAGCOMBO_a problem.andGate(FLAGCOMBO_ae, FLAGCOMBO_ne)
#define FLAGCOMBO_l problem.xorGate(flag(x86flag_sf), flag(x86flag_of))
#define FLAGCOMBO_ge invert(problem.xorGate(flag(x86flag_sf), flag(x86flag_of)))
#define FLAGCOMBO_le problem.orGate(FLAGCOMBO_e, FLAGCOMBO_l)
#define FLAGCOMBO_g problem.andGate(FLAGCOMBO_ge, FLAGCOMBO_ne)
#define FLAGCOMBO_cxz problem.norAcross(extract<0, 16>(vars(gpr(x86_gpr_cx))))
#define FLAGCOMBO_ecxz problem.norAcross(vars(gpr(x86_gpr_cx)))
#define JUMP(tag) case x86_j##tag: {ROSE_ASSERT(operands.size() == 1); vars(ip()) = problem.ite(FLAGCOMBO_##tag, read<32>(operands[0]), vars(ip())); break;}
      JUMP(ne)
      JUMP(e)
      JUMP(ns)
      JUMP(s)
      JUMP(ae)
      JUMP(b)
      JUMP(be)
      JUMP(a)
      JUMP(le)
      JUMP(g)
      JUMP(ge)
      JUMP(l)
      JUMP(cxz)
      JUMP(ecxz)
#undef JUMP
#define SET(tag) case x86_set##tag: {ROSE_ASSERT (operands.size() == 1); write<8>(operands[0], concat(single(FLAGCOMBO_##tag), number<7>(0))); break;}
      SET(ne)
      SET(e)
      SET(ns)
      SET(s)
      SET(ae)
      SET(b)
      SET(be)
      SET(a)
      SET(le)
      SET(g)
      SET(ge)
      SET(l)
#undef SET
      case x86_cld: {
        ROSE_ASSERT (operands.size() == 0);
        flag(x86flag_df) = FALSE;
        break;
      }
      case x86_std: {
        ROSE_ASSERT (operands.size() == 0);
        flag(x86flag_df) = TRUE;
        break;
      }
      case x86_nop: break;
      case x86_repne_scasb: {
        ROSE_ASSERT (operands.size() == 0);
        ROSE_ASSERT (insn->get_addressSize() == x86_insnsize_32);
        Lit ecxNotZero = problem.orAcross(vars(gpr(x86_gpr_cx)));
        doAddOperation(extract<0, 8>(vars(gpr(x86_gpr_ax))), invertWord(readMemoryByte(vars(gpr(x86_gpr_di)))), true, FALSE, ecxNotZero);
        vars(ip()) = problem.ite(problem.andGate(ecxNotZero, invert(flag(x86flag_zf))), // If true, repeat this instruction, otherwise go to the next one
                                 vars(ip()),
                                 number<32>((uint32_t)(insn->get_address())));
        break;
      }
      case x86_repe_cmpsb: {
        ROSE_ASSERT (operands.size() == 0);
        ROSE_ASSERT (insn->get_addressSize() == x86_insnsize_32);
        Lit ecxNotZero = problem.orAcross(vars(gpr(x86_gpr_cx)));
        doAddOperation(readMemoryByte(vars(gpr(x86_gpr_si))), invertWord(readMemoryByte(vars(gpr(x86_gpr_di)))), true, FALSE, ecxNotZero);
        vars(ip()) = problem.ite(problem.andGate(ecxNotZero, flag(x86flag_zf)), // If true, repeat this instruction, otherwise go to the next one
                                 number<32>((uint32_t)(insn->get_address())),
                                 vars(ip()));
        break;
      }
      case x86_lodsd: {
        vars(gpr(x86_gpr_ax)) = readMemory<32>(vars(gpr(x86_gpr_si)));
        vars(gpr(x86_gpr_si)) = problem.adder(vars(gpr(x86_gpr_si)), problem.ite(flag(x86flag_df), number<32>(-4), number<32>(4)));
        break;
      }
      case x86_lodsw: {
        vars(gpr(x86_gpr_ax)) = concat(readMemory<16>(vars(gpr(x86_gpr_si))), extract<16, 32>(vars(gpr(x86_gpr_ax))));
        vars(gpr(x86_gpr_si)) = problem.adder(vars(gpr(x86_gpr_si)), problem.ite(flag(x86flag_df), number<32>(-2), number<32>(2)));
        break;
      }
      case x86_lodsb: {
        vars(gpr(x86_gpr_ax)) = concat(readMemory<8>(vars(gpr(x86_gpr_si))), extract<8, 32>(vars(gpr(x86_gpr_ax))));
        vars(gpr(x86_gpr_si)) = problem.adder(vars(gpr(x86_gpr_si)), problem.ite(flag(x86flag_df), number<32>(-1), number<32>(1)));
        break;
      }
      case x86_hlt: break; // FIXME
      case x86_int: break; // FIXME
      default: fprintf(stderr, "Bad instruction %s\n", toString(kind).c_str()); abort();
    }
  }

  void writeBack(Lit isThisIp) {
    fprintf(stderr, "Have %d variables and %zu clauses so far\n", problem.numVariables, problem.numClauses);
    for (RegMap::const_iterator i = registerMap.begin(); i != registerMap.end(); ++i) {
      for (size_t j = 0; j < 32; ++j) {
        problem.condEquivalence(isThisIp, (i->second)[j], newRegisterMap[i->first][j]);
      }
    }
    for (FlagMap::const_iterator i = flagMap.begin(); i != flagMap.end(); ++i) {
      problem.condEquivalence(isThisIp, i->second, newFlagMap[i->first]);
    }
    fprintf(stderr, "Have %d variables and %zu clauses so far\n", problem.numVariables, problem.numClauses);
  }

  void processBlock(const SgAsmStatementPtrList& stmts, size_t begin, size_t end, const LitList(32)& origIp) {
    if (begin == end) return;
    fprintf(stderr, "Block 0x%08X contains %zu instruction(s)\n", (unsigned int)(stmts[begin]->get_address()), end - begin);
    for (size_t i = begin; i < end; ++i) {
      SgAsmx86Instruction* insn = isSgAsmx86Instruction(stmts[i]);
      ROSE_ASSERT (insn);
      translate(insn);
    }
    writeBack(problem.equal(origIp, number<32>(stmts[begin]->get_address())));
  }

  static bool isRepeatedStringOp(SgAsmStatement* s) {
    SgAsmx86Instruction* insn = isSgAsmx86Instruction(s);
    if (!insn) return false;
    switch (insn->get_kind()) {
      case x86_repe_cmpsb: return true;
      case x86_repe_cmpsd: return true;
      case x86_repe_cmpsq: return true;
      case x86_repe_cmpsw: return true;
      case x86_repe_scasb: return true;
      case x86_repe_scasd: return true;
      case x86_repe_scasq: return true;
      case x86_repe_scasw: return true;
      case x86_rep_insb: return true;
      case x86_rep_insd: return true;
      case x86_rep_insw: return true;
      case x86_rep_lodsb: return true;
      case x86_rep_lodsd: return true;
      case x86_rep_lodsq: return true;
      case x86_rep_lodsw: return true;
      case x86_rep_movsb: return true;
      case x86_rep_movsd: return true;
      case x86_rep_movsq: return true;
      case x86_rep_movsw: return true;
      case x86_repne_cmpsb: return true;
      case x86_repne_cmpsd: return true;
      case x86_repne_cmpsq: return true;
      case x86_repne_cmpsw: return true;
      case x86_repne_scasb: return true;
      case x86_repne_scasd: return true;
      case x86_repne_scasq: return true;
      case x86_repne_scasw: return true;
      case x86_rep_outsb: return true;
      case x86_rep_outsd: return true;
      case x86_rep_outsw: return true;
      case x86_rep_stosb: return true;
      case x86_rep_stosd: return true;
      case x86_rep_stosq: return true;
      case x86_rep_stosw: return true;
      default: return false;
    }
  }

  static bool isHltOrInt(SgAsmStatement* s) {
    SgAsmx86Instruction* insn = isSgAsmx86Instruction(s);
    if (!insn) return false;
    switch (insn->get_kind()) {
      case x86_hlt: return true;
      case x86_int: return true;
      default: return false;
    }
  }

  void processBlock(SgAsmBlock* b, const LitList(32)& origIp) {
    const SgAsmStatementPtrList& stmts = b->get_statementList();
    if (stmts.empty()) return;
    if (!isSgAsmInstruction(stmts[0])) return; // A block containing functions or something
    size_t i = 0;
    while (i < stmts.size()) {
      size_t oldI = i;
      // Advance until either i points to a repeated string op or it is just after a hlt or int
      while (i < stmts.size() && !isRepeatedStringOp(stmts[i]) && (i == oldI || !isHltOrInt(stmts[i - 1]))) ++i;
      processBlock(stmts, oldI, i, origIp);
      if (i >= stmts.size()) break;
      if (isRepeatedStringOp(stmts[i])) {
        processBlock(stmts, i, i + 1, origIp);
        ++i;
      }
      ROSE_ASSERT (i != oldI);
    }
  }

};

int main(int argc, char** argv) {
  SgProject* proj = frontend(argc, argv);
  FILE* f = fopen("foo.dimacs", "w");
  assert (f);
  NetlistTranslator t(f);
  NetlistTranslator::RegMap origRegisterMap;
  NetlistTranslator::FlagMap origFlagMap;
  t.makeRegMaps(origRegisterMap, origFlagMap);
  vector<SgNode*> blocks = NodeQuery::querySubTree(proj, V_SgAsmBlock);
  for (size_t i = 0; i < blocks.size(); ++i) {
    SgAsmBlock* b = isSgAsmBlock(blocks[i]);
    ROSE_ASSERT (b);
    t.registerMap = origRegisterMap;
    t.flagMap = origFlagMap;
    t.memoryWrites.clear();
    t.initialMemoryReads.clear();
    t.processBlock(b, origRegisterMap[NetlistTranslator::ip()]);
  }
  t.problem.toDimacs();
  fclose(f);
  return 0;
}
