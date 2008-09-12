#include "rose.h"
#include "x86AssemblyToC.h"
#include <boost/array.hpp>
#include <cassert>
#include <cstdio>

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

typedef int Var;
#define VarList(N) boost::array<Var, (N)> // Stored LSB first, counted in bits
// Len and similar template parameters are always in bits, not bytes
const Var TRUE = 0x80000000, FALSE = 0;
inline bool isConstantBool(int var) {return (var & 0x7fffffff) == 0;}
inline int invert(int var) {if (isConstantBool(var)) return var ^ 0x80000000; else return -var;}
template <size_t Len>
inline VarList(Len) invertWord(const VarList(Len)& a) {
  VarList(Len) result;
  for (size_t i = 0; i < Len; ++i) result[i] = invert(a[i]);
  return result;
}
inline Var invertMaybe(Var a, bool inv) {
  return inv ? invert(a) : a;
}

template <size_t From, size_t To, size_t InputSize>
VarList(To - From) extract(const VarList(InputSize)& in) {
  VarList(To - From) result;
  for (size_t i = 0; i < To - From; ++i) {
    result[i] = in[i + From];
  }
  return result;
}

template <size_t Len1, size_t Len2>
VarList(Len1 + Len2) concat(const VarList(Len1)& a, const VarList(Len2)& b) {
  VarList(Len1 + Len2) result;
  for (size_t i = 0; i < Len1; ++i) result[i] = a[i];
  for (size_t i = 0; i < Len2; ++i) result[i + Len1] = b[i];
  return result;
}

template <size_t Len>
struct ExtractReadHelper {
  static VarList(Len) go(const VarList(32)& fullDWord, X86PositionInRegister pos);
};

template <> struct ExtractReadHelper<8> {
  static VarList(8) go(const VarList(32)& fullDWord, X86PositionInRegister pos) {
    switch (pos) {
      case x86_regpos_low_byte: return extract<0, 8>(fullDWord);
      case x86_regpos_high_byte: return extract<8, 16>(fullDWord);
      default: ROSE_ASSERT (false);
    }
  }
};

template <> struct ExtractReadHelper<16> {
  static VarList(16) go(const VarList(32)& fullDWord, X86PositionInRegister pos) {
    switch (pos) {
      case x86_regpos_word: return extract<0, 16>(fullDWord);
      default: ROSE_ASSERT (false);
    }
  }
};

template <> struct ExtractReadHelper<32> {
  static VarList(32) go(const VarList(32)& fullDWord, X86PositionInRegister pos) {
    switch (pos) {
      case x86_regpos_dword: return fullDWord;
      default: ROSE_ASSERT (false);
    }
  }
};

template <size_t Len, size_t Offset, typename NLTranslator>
struct WriteMemoryHelper {
  static void go(const VarList(32)& addr, const VarList(Len)& data, NLTranslator& trans) {
    trans.writeMemoryByte((Offset == 0 ? addr : trans.adder(addr, trans.template number<32>(Offset / 8))), extract<Offset, Offset + 8>(data));
    WriteMemoryHelper<Len, Offset + 8, NLTranslator>::go(addr, data, trans);
  }
};

template <size_t Len, typename NLTranslator>
struct WriteMemoryHelper<Len, Len, NLTranslator> {
  static void go(const VarList(32)& addr, const VarList(Len)& data, NLTranslator& trans) {}
};

struct NetlistTranslator {
  typedef pair<X86RegisterClass, int> FullReg;
  typedef map<FullReg, VarList(32)> RegMap;
  typedef map<X86Flag, Var> FlagMap;
  typedef vector<pair<VarList(32), VarList(8) > > MemoryWriteList;
  RegMap registerMap;
  FlagMap flagMap;
  MemoryWriteList memoryWrites;
  int numVariables;

  int newVar() {return ++numVariables;}

  template <size_t Count>
  VarList(Count) newVars() {VarList(Count) vl; for (size_t i = 0; i < Count; ++i) vl[i] = newVar(); return vl;}

  static FullReg gpr(X86GeneralPurposeRegister reg) {
    return make_pair(x86_regclass_gpr, reg);
  }

  static FullReg ip() {
    return make_pair(x86_regclass_ip, 0);
  }

  const VarList(32)& vars(FullReg r) const {
    RegMap::const_iterator i = registerMap.find(r);
    ROSE_ASSERT (i != registerMap.end());
    return i->second;
  }

  VarList(32)& vars(FullReg r) {
    RegMap::iterator i = registerMap.find(r);
    ROSE_ASSERT (i != registerMap.end());
    return i->second;
  }

  Var flag(X86Flag f) const {
    FlagMap::const_iterator i = flagMap.find(f);
    ROSE_ASSERT (i != flagMap.end());
    return i->second;
  }

  Var& flag(X86Flag f) {
    FlagMap::iterator i = flagMap.find(f);
    ROSE_ASSERT (i != flagMap.end());
    return i->second;
  }

  template <size_t NumBits>
  VarList(NumBits) number(unsigned int n) {
    VarList(NumBits) result;
    for (size_t i = 0; i < NumBits; ++i) {
      result[i] = (n % 2 == 1) ? TRUE : FALSE;
      n >>= 1;
    }
    return result;
  }

  NetlistTranslator(): numVariables(0) {
    for (int r = 0; r < 8; ++r) {
      registerMap.insert(make_pair(gpr((X86GeneralPurposeRegister)r), newVars<32>()));
    }
    registerMap.insert(make_pair(ip(), newVars<32>()));
    flagMap.insert(make_pair(x86flag_cf, newVar()));
    flagMap.insert(make_pair(x86flag_pf, newVar()));
    flagMap.insert(make_pair(x86flag_af, newVar()));
    flagMap.insert(make_pair(x86flag_zf, newVar()));
    flagMap.insert(make_pair(x86flag_sf, newVar()));
    flagMap.insert(make_pair(x86flag_df, newVar()));
    flagMap.insert(make_pair(x86flag_of, newVar()));
  }

  template <size_t NBits>
  VarList(NBits) adder(const VarList(NBits)& a, const VarList(NBits)& b, Var carryIn = FALSE, VarList(NBits)* carriesOutOpt = NULL) {
    if (carriesOutOpt != NULL) {
      *carriesOutOpt = newVars<NBits>(); // FIXME
    }
    return newVars<NBits>(); // FIXME
  }

  template <size_t Len1, size_t Len2>
  VarList(Len1 + Len2) signedMultiplier(const VarList(Len1)& a, const VarList(Len2)& b) {
    return newVars<Len1 + Len2>(); // FIXME
  }

  template <size_t Len1, size_t Len2>
  VarList(Len1 + Len2) unsignedMultiplier(const VarList(Len1)& a, const VarList(Len2)& b) {
    return newVars<Len1 + Len2>(); // FIXME
  }

  template <size_t Len1, size_t Len2>
  VarList(Len1 + Len2) unsignedDivider(const VarList(Len1)& dividend, const VarList(Len2)& divisor) {
    // Returns Len1 bits quotient, Len2 bits remainder concatenated
    return newVars<Len1 + Len2>(); // FIXME
  }

  Var andGate(Var a, Var b) {
    return newVar(); // FIXME
  }

  Var nandGate(Var a, Var b) {
    return newVar(); // FIXME
  }

  Var orGate(Var a, Var b) {
    return newVar(); // FIXME
  }

  Var norGate(Var a, Var b) {
    return newVar(); // FIXME
  }

  Var xorGate(Var a, Var b) {
    return newVar(); // FIXME
  }

  template <size_t Len>
  VarList(Len) andWords(const VarList(Len)& a, const VarList(Len)& b) {
    return newVars<Len>(); // FIXME
  }

  template <size_t Len>
  VarList(Len) orWords(const VarList(Len)& a, const VarList(Len)& b) {
    return newVars<Len>(); // FIXME
  }

  template <size_t Len>
  VarList(Len) xorWords(const VarList(Len)& a, const VarList(Len)& b) {
    return newVars<Len>(); // FIXME
  }

  template <size_t Len>
  Var andAcross(const VarList(Len)& a) {
    return newVar(); // FIXME
  }

  template <size_t Len>
  Var orAcross(const VarList(Len)& a) {
    return newVar(); // FIXME
  }

  template <size_t Len>
  Var xorAcross(const VarList(Len)& a) {
    return newVar(); // FIXME
  }

  template <size_t Len>
  Var norAcross(const VarList(Len)& a) {
    return invert(orAcross(a));
  }

  template <size_t NBits>
  Var equal(const VarList(NBits)& a, const VarList(NBits)& b) {
    return norAcross(xorWords(a, b));
  }

  template <size_t NBits>
  VarList(NBits) ite(Var cond, const VarList(NBits)& ifTrue, const VarList(NBits)& ifFalse) {
    return newVars<NBits>(); // FIXME
  }

  template <size_t Len> // In bits
  VarList(Len) readMemory(const VarList(32)& addr) {
    VarList(Len) result;
    for (size_t i = 0; i < Len / 8; ++i) {
      VarList(8) thisByte = readMemoryByte(adder(addr, number<32>(i)));
      for (size_t j = 0; j < 8; ++j) {
        result[i * 8 + j] = thisByte[j];
      }
    }
    return result;
  }

  VarList(32) readEffectiveAddress(SgAsmExpression* expr) {
    assert (isSgAsmMemoryReferenceExpression(expr));
    return read<32>(isSgAsmMemoryReferenceExpression(expr)->get_address());
  }

  VarList(8) readMemoryByte(const VarList(32)& addr) {
    VarList(8) result = newVars<8>();
    for (size_t i = 0; i < memoryWrites.size(); ++i) {
      result = ite(equal(addr, memoryWrites[i].first), memoryWrites[i].second, result);
    }
    return result;
  }

  void writeMemoryByte(const VarList(32)& addr, const VarList(8)& data) {
    memoryWrites.push_back(make_pair(addr, data));
  }

  template <size_t Len>
  void writeMemory(const VarList(32)& addr, const VarList(Len)& data) {
    WriteMemoryHelper<Len, 0, NetlistTranslator>::go(addr, data, *this);
  }

  template <size_t Len>
  VarList(Len) read(SgAsmExpression* e) {
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
        return adder(read<Len>(isSgAsmBinaryAdd(e)->get_lhs()), read<Len>(isSgAsmBinaryAdd(e)->get_rhs()));
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
  void write(SgAsmExpression* e, const VarList(Len)& value) {
    switch (e->variantT()) {
      case V_SgAsmx86RegisterReferenceExpression: {
        SgAsmx86RegisterReferenceExpression* rre = isSgAsmx86RegisterReferenceExpression(e);
        switch (rre->get_register_class()) {
          case x86_regclass_gpr: {
            X86GeneralPurposeRegister reg = (X86GeneralPurposeRegister)(rre->get_register_number());
            VarList(32) newValue = blendInWrittenValue<Len>(vars(gpr(reg)), rre->get_position_in_register(), value);
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
  VarList(Len) extractRead(const VarList(32)& fullDWord, X86PositionInRegister pos) const {
    return ExtractReadHelper<Len>::go(fullDWord, pos);
  }

  template <size_t Len>
  VarList(32) blendInWrittenValue(const VarList(32)& oldDWord, X86PositionInRegister pos, const VarList(Len)& newValue) const {
    switch (pos) {
      case x86_regpos_all: // The copy is for static type safety
      case x86_regpos_dword: {
        ROSE_ASSERT (Len == 32);
        VarList(32) newVal = oldDWord;
        for (size_t i = 0; i < 32; ++i) newVal[i] = newValue[i];
        return newVal;
      }

      case x86_regpos_word: {
        ROSE_ASSERT (Len == 16);
        VarList(32) newVal = oldDWord;
        for (size_t i = 0; i < 16; ++i) newVal[i] = newValue[i];
        return newVal;
      }

      case x86_regpos_low_byte: {
        ROSE_ASSERT (Len == 8);
        VarList(32) newVal = oldDWord;
        for (size_t i = 0; i < 8; ++i) newVal[i] = newValue[i];
        return newVal;
      }

      case x86_regpos_high_byte: {
        ROSE_ASSERT (Len == 8);
        VarList(32) newVal = oldDWord;
        for (size_t i = 0; i < 8; ++i) newVal[i + 8] = newValue[i];
        return newVal;
      }

      default: ROSE_ASSERT (!"Bad position in register");
    }
  }

  template <size_t Len>
  void setFlagsForResult(const VarList(Len)& result) {
    flag(x86flag_pf) = xorAcross(extract<0, 8>(result));
    flag(x86flag_sf) = result[Len - 1];
    flag(x86flag_zf) = norAcross(result);
  }

  template <size_t Len>
  VarList(Len) doAddOperation(const VarList(Len)& a, const VarList(Len)& b, bool invertCarries, Var carryIn = FALSE) { // Does add (subtract with two's complement input and invertCarries set), and sets correct flags
    VarList(Len) carries;
    VarList(Len) result = adder(a, b, invertMaybe(carryIn, invertCarries), &carries);
    setFlagsForResult(result);
    flag(x86flag_af) = invertMaybe(carries[3], invertCarries);
    flag(x86flag_cf) = invertMaybe(carries[Len - 1], invertCarries);
    flag(x86flag_of) = xorGate(carries[Len - 1], carries[Len - 2]);
    return result;
  }

  template <size_t Len>
  VarList(Len) doIncOperation(const VarList(Len)& a, bool dec) { // Does inc (dec with dec set), and sets correct flags
    VarList(Len) carries;
    VarList(Len) result = adder(a, number<Len>(dec ? -1 : 1), FALSE, &carries);
    setFlagsForResult(result);
    flag(x86flag_af) = invertMaybe(carries[3], dec);
    flag(x86flag_of) = xorGate(carries[Len - 1], carries[Len - 2]);
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
            VarList(8) op1 = read<8>(operands[1]);
            VarList(16) result;
            for (size_t i = 0; i < 16; ++i) result[i] = (i < 8 ? op1[i] : op1[7]);
            write(operands[0], result);
            break;
          }
          case 4: {
            switch (numBytesInAsmType(operands[1]->get_type())) {
              case 1: {
                VarList(8) op1 = read<8>(operands[1]);
                VarList(32) result;
                for (size_t i = 0; i < 32; ++i) result[i] = (i < 8 ? op1[i] : op1[7]);
                write(operands[0], result);
                break;
              }
              case 2: {
                VarList(16) op1 = read<16>(operands[1]);
                VarList(32) result;
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
        VarList(32) result = vars(gpr(x86_gpr_ax));
        Var signbit = result[7];
        for (size_t i = 8; i < 15; ++i) result[i] = signbit;
        vars(gpr(x86_gpr_ax)) = result;
        break;
      }
      case x86_cwde: {
        ROSE_ASSERT (operands.size() == 0);
        VarList(32) result = vars(gpr(x86_gpr_ax));
        Var signbit = result[15];
        for (size_t i = 16; i < 32; ++i) result[i] = signbit;
        vars(gpr(x86_gpr_ax)) = result;
        break;
      }
      case x86_cwd: {
        ROSE_ASSERT (operands.size() == 0);
        VarList(32) result = vars(gpr(x86_gpr_dx));
        Var signbit = vars(gpr(x86_gpr_ax))[15];
        for (size_t i = 0; i < 15; ++i) result[i] = signbit;
        vars(gpr(x86_gpr_dx)) = result;
        break;
      }
      case x86_cdq: {
        ROSE_ASSERT (operands.size() == 0);
        VarList(32) result;
        Var signbit = vars(gpr(x86_gpr_ax))[31];
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
            VarList(8) result = andWords(read<8>(operands[0]), read<8>(operands[1]));
            setFlagsForResult(result);
            write(operands[0], result);
            break;
          }
          case 2: {
            VarList(16) result = andWords(read<16>(operands[0]), read<16>(operands[1]));
            setFlagsForResult(result);
            write(operands[0], result);
            break;
          }
          case 4: {
            VarList(32) result = andWords(read<32>(operands[0]), read<32>(operands[1]));
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
            VarList(8) result = orWords(read<8>(operands[0]), read<8>(operands[1]));
            setFlagsForResult(result);
            write(operands[0], result);
            break;
          }
          case 2: {
            VarList(16) result = orWords(read<16>(operands[0]), read<16>(operands[1]));
            setFlagsForResult(result);
            write(operands[0], result);
            break;
          }
          case 4: {
            VarList(32) result = orWords(read<32>(operands[0]), read<32>(operands[1]));
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
            VarList(8) result = andWords(read<8>(operands[0]), read<8>(operands[1]));
            setFlagsForResult(result);
            break;
          }
          case 2: {
            VarList(16) result = andWords(read<16>(operands[0]), read<16>(operands[1]));
            setFlagsForResult(result);
            break;
          }
          case 4: {
            VarList(32) result = andWords(read<32>(operands[0]), read<32>(operands[1]));
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
            VarList(8) result = xorWords(read<8>(operands[0]), read<8>(operands[1]));
            setFlagsForResult(result);
            write(operands[0], result);
            break;
          }
          case 2: {
            VarList(16) result = xorWords(read<16>(operands[0]), read<16>(operands[1]));
            setFlagsForResult(result);
            write(operands[0], result);
            break;
          }
          case 4: {
            VarList(32) result = xorWords(read<32>(operands[0]), read<32>(operands[1]));
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
      case x86_add: {
        ROSE_ASSERT (operands.size() == 2);
        switch (numBytesInAsmType(operands[0]->get_type())) {
          case 1: {
            VarList(8) result = doAddOperation(read<8>(operands[0]), read<8>(operands[1]), false);
            write(operands[0], result);
            break;
          }
          case 2: {
            VarList(16) result = doAddOperation(read<16>(operands[0]), read<16>(operands[1]), false);
            write(operands[0], result);
            break;
          }
          case 4: {
            VarList(32) result = doAddOperation(read<32>(operands[0]), read<32>(operands[1]), false);
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
            VarList(8) result = doAddOperation(read<8>(operands[0]), read<8>(operands[1]), false, flag(x86flag_cf));
            write(operands[0], result);
            break;
          }
          case 2: {
            VarList(16) result = doAddOperation(read<16>(operands[0]), read<16>(operands[1]), false, flag(x86flag_cf));
            write(operands[0], result);
            break;
          }
          case 4: {
            VarList(32) result = doAddOperation(read<32>(operands[0]), read<32>(operands[1]), false, flag(x86flag_cf));
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
            VarList(8) result = doAddOperation(read<8>(operands[0]), invertWord(read<8>(operands[1])), true);
            write(operands[0], result);
            break;
          }
          case 2: {
            VarList(16) result = doAddOperation(read<16>(operands[0]), invertWord(read<16>(operands[1])), true);
            write(operands[0], result);
            break;
          }
          case 4: {
            VarList(32) result = doAddOperation(read<32>(operands[0]), invertWord(read<32>(operands[1])), true);
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
            VarList(8) result = doAddOperation(read<8>(operands[0]), invertWord(read<8>(operands[1])), true, flag(x86flag_cf));
            write(operands[0], result);
            break;
          }
          case 2: {
            VarList(16) result = doAddOperation(read<16>(operands[0]), invertWord(read<16>(operands[1])), true, flag(x86flag_cf));
            write(operands[0], result);
            break;
          }
          case 4: {
            VarList(32) result = doAddOperation(read<32>(operands[0]), invertWord(read<32>(operands[1])), true, flag(x86flag_cf));
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
            VarList(8) result = doAddOperation(read<8>(operands[0]), invertWord(read<8>(operands[1])), true);
            break;
          }
          case 2: {
            VarList(16) result = doAddOperation(read<16>(operands[0]), invertWord(read<16>(operands[1])), true);
            break;
          }
          case 4: {
            VarList(32) result = doAddOperation(read<32>(operands[0]), invertWord(read<32>(operands[1])), true);
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
            VarList(8) result = doIncOperation(read<8>(operands[0]), false);
            write(operands[0], result);
            break;
          }
          case 2: {
            VarList(16) result = doIncOperation(read<16>(operands[0]), false);
            write(operands[0], result);
            break;
          }
          case 4: {
            VarList(32) result = doIncOperation(read<32>(operands[0]), false);
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
            VarList(8) result = doIncOperation(read<8>(operands[0]), true);
            write(operands[0], result);
            break;
          }
          case 2: {
            VarList(16) result = doIncOperation(read<16>(operands[0]), true);
            write(operands[0], result);
            break;
          }
          case 4: {
            VarList(32) result = doIncOperation(read<32>(operands[0]), true);
            write(operands[0], result);
            break;
          }
          default: ROSE_ASSERT (!"Bad size"); break;
        }
        break;
      }
      case x86_imul: {
        switch (numBytesInAsmType(operands[0]->get_type())) {
          case 1: {
            VarList(8) op0 = extract<0, 8>(vars(gpr(x86_gpr_ax)));
            VarList(8) op1 = read<8>(operands[0]);
            VarList(16) mulResult = signedMultiplier(op0, op1);
            vars(gpr(x86_gpr_ax)) = concat(mulResult, extract<16, 32>(vars(gpr(x86_gpr_ax))));
            Var carry = norGate(andAcross(extract<7, 16>(mulResult)), norAcross(extract<7, 16>(mulResult)));
            flag(x86flag_cf) = flag(x86flag_of) = carry;
            break;
          }
          case 2: {
            VarList(16) op0 = operands.size() == 1 ? extract<0, 16>(vars(gpr(x86_gpr_ax))) : read<16>(operands[operands.size() - 2]);
            VarList(16) op1 = read<16>(operands[operands.size() - 1]);
            VarList(32) mulResult = signedMultiplier(op0, op1);
            if (operands.size() == 1) {
              vars(gpr(x86_gpr_ax)) = concat(extract<0, 16>(mulResult), extract<16, 32>(vars(gpr(x86_gpr_ax))));
              vars(gpr(x86_gpr_dx)) = concat(extract<16, 32>(mulResult), extract<16, 32>(vars(gpr(x86_gpr_dx))));
            } else {
              write(operands[0], extract<0, 16>(mulResult));
            }
            Var carry = norGate(andAcross(extract<7, 32>(mulResult)), norAcross(extract<7, 32>(mulResult)));
            flag(x86flag_cf) = flag(x86flag_of) = carry;
            break;
          }
          case 4: {
            VarList(32) op0 = operands.size() == 1 ? vars(gpr(x86_gpr_ax)) : read<32>(operands[operands.size() - 2]);
            VarList(32) op1 = read<32>(operands[operands.size() - 1]);
            VarList(64) mulResult = signedMultiplier(op0, op1);
            if (operands.size() == 1) {
              vars(gpr(x86_gpr_ax)) = extract<0, 32>(mulResult);
              vars(gpr(x86_gpr_dx)) = extract<32, 64>(mulResult);
            } else {
              write(operands[0], extract<0, 32>(mulResult));
            }
            Var carry = norGate(andAcross(extract<7, 64>(mulResult)), norAcross(extract<7, 64>(mulResult)));
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
            VarList(8) op0 = extract<0, 8>(vars(gpr(x86_gpr_ax)));
            VarList(8) op1 = read<8>(operands[0]);
            VarList(16) mulResult = unsignedMultiplier(op0, op1);
            vars(gpr(x86_gpr_ax)) = concat(mulResult, extract<16, 32>(vars(gpr(x86_gpr_ax))));
            Var carry = orAcross(extract<8, 16>(mulResult));
            flag(x86flag_cf) = flag(x86flag_of) = carry;
            break;
          }
          case 2: {
            VarList(16) op0 = extract<0, 16>(vars(gpr(x86_gpr_ax)));
            VarList(16) op1 = read<16>(operands[0]);
            VarList(32) mulResult = unsignedMultiplier(op0, op1);
            vars(gpr(x86_gpr_ax)) = concat(extract<0, 16>(mulResult), extract<16, 32>(vars(gpr(x86_gpr_ax))));
            vars(gpr(x86_gpr_dx)) = concat(extract<16, 32>(mulResult), extract<16, 32>(vars(gpr(x86_gpr_dx))));
            Var carry = orAcross(extract<16, 32>(mulResult));
            flag(x86flag_cf) = flag(x86flag_of) = carry;
            break;
          }
          case 4: {
            VarList(32) op0 = vars(gpr(x86_gpr_ax));
            VarList(32) op1 = read<32>(operands[0]);
            VarList(64) mulResult = unsignedMultiplier(op0, op1);
            vars(gpr(x86_gpr_ax)) = extract<0, 32>(mulResult);
            vars(gpr(x86_gpr_dx)) = extract<32, 64>(mulResult);
            Var carry = orAcross(extract<32, 64>(mulResult));
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
      case x86_div: {
        switch (numBytesInAsmType(operands[0]->get_type())) {
          case 1: {
            VarList(16) op0 = extract<0, 16>(vars(gpr(x86_gpr_ax)));
            VarList(8) op1 = read<8>(operands[0]);
            // if op1 == 0, we should trap
            VarList(24) divResult = unsignedDivider(op0, op1); // 16 bits quotient, 8 remainder
            // if extract<8, 16> of divResult is non-zero (overflow), we should trap
            vars(gpr(x86_gpr_ax)) = concat(concat(extract<0, 8>(divResult), extract<16, 24>(divResult)), extract<16, 32>(vars(gpr(x86_gpr_ax))));
            break;
          }
          case 2: {
            VarList(32) op0 = concat(extract<0, 16>(vars(gpr(x86_gpr_ax))), extract<0, 16>(vars(gpr(x86_gpr_dx))));
            VarList(16) op1 = read<16>(operands[0]);
            // if op1 == 0, we should trap
            VarList(48) divResult = unsignedDivider(op0, op1); // 32 bits quotient, 16 remainder
            // if extract<16, 32> of divResult is non-zero (overflow), we should trap
            vars(gpr(x86_gpr_ax)) = concat(extract<0, 16>(divResult), extract<16, 32>(vars(gpr(x86_gpr_ax))));
            vars(gpr(x86_gpr_dx)) = concat(extract<32, 48>(divResult), extract<16, 32>(vars(gpr(x86_gpr_dx))));
            break;
          }
          case 4: {
            VarList(64) op0 = concat(vars(gpr(x86_gpr_ax)), vars(gpr(x86_gpr_dx)));
            VarList(32) op1 = read<32>(operands[0]);
            // if op1 == 0, we should trap
            VarList(96) divResult = unsignedDivider(op0, op1); // 64 bits quotient, 32 remainder
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
        VarList(32) oldSp = vars(gpr(x86_gpr_sp));
        VarList(32) newSp = adder(oldSp, number<32>(-4));
        writeMemory<32>(newSp, read<32>(operands[0]));
        vars(gpr(x86_gpr_sp)) = newSp;
        break;
      }
      case x86_pop: {
        ROSE_ASSERT (operands.size() == 1);
        ROSE_ASSERT (insn->get_addressSize() == x86_insnsize_32);
        ROSE_ASSERT (insn->get_operandSize() == x86_insnsize_32);
        VarList(32) oldSp = vars(gpr(x86_gpr_sp));
        VarList(32) newSp = adder(oldSp, number<32>(4));
        write<32>(operands[0], readMemory<32>(oldSp));
        vars(gpr(x86_gpr_sp)) = newSp;
        break;
      }
      case x86_call: {
        ROSE_ASSERT (operands.size() == 1);
        ROSE_ASSERT (insn->get_addressSize() == x86_insnsize_32);
        ROSE_ASSERT (insn->get_operandSize() == x86_insnsize_32);
        VarList(32) oldSp = vars(gpr(x86_gpr_sp));
        VarList(32) newSp = adder(oldSp, number<32>(-4));
        writeMemory<32>(newSp, vars(ip()));
        vars(ip()) = read<32>(operands[0]);
        vars(gpr(x86_gpr_sp)) = newSp;
        break;
      }
      case x86_ret: {
        ROSE_ASSERT (operands.size() == 0);
        ROSE_ASSERT (insn->get_addressSize() == x86_insnsize_32);
        ROSE_ASSERT (insn->get_operandSize() == x86_insnsize_32);
        VarList(32) oldSp = vars(gpr(x86_gpr_sp));
        VarList(32) newSp = adder(oldSp, number<32>(4));
        vars(ip()) = readMemory<32>(oldSp);
        vars(gpr(x86_gpr_sp)) = newSp;
        break;
      }
      case x86_jmp: {
        ROSE_ASSERT (operands.size() == 1);
        vars(ip()) = read<32>(operands[0]);
        break;
      }
      case x86_jne: {
        ROSE_ASSERT (operands.size() == 1);
        vars(ip()) = ite(flag(x86flag_zf), vars(ip()), read<32>(operands[0]));
        break;
      }
      case x86_je: {
        ROSE_ASSERT (operands.size() == 1);
        vars(ip()) = ite(flag(x86flag_zf), read<32>(operands[0]), vars(ip()));
        break;
      }
      case x86_jns: {
        ROSE_ASSERT (operands.size() == 1);
        vars(ip()) = ite(flag(x86flag_sf), vars(ip()), read<32>(operands[0]));
        break;
      }
      case x86_js: {
        ROSE_ASSERT (operands.size() == 1);
        vars(ip()) = ite(flag(x86flag_sf), read<32>(operands[0]), vars(ip()));
        break;
      }
      case x86_jae: {
        ROSE_ASSERT (operands.size() == 1);
        vars(ip()) = ite(flag(x86flag_cf), vars(ip()), read<32>(operands[0]));
        break;
      }
      case x86_jb: {
        ROSE_ASSERT (operands.size() == 1);
        vars(ip()) = ite(flag(x86flag_cf), read<32>(operands[0]), vars(ip()));
        break;
      }
      case x86_jbe: {
        ROSE_ASSERT (operands.size() == 1);
        vars(ip()) = ite(orGate(flag(x86flag_zf), flag(x86flag_cf)), read<32>(operands[0]), vars(ip()));
        break;
      }
      case x86_ja: {
        ROSE_ASSERT (operands.size() == 1);
        vars(ip()) = ite(orGate(flag(x86flag_zf), flag(x86flag_cf)), vars(ip()), read<32>(operands[0]));
        break;
      }
      case x86_jle: {
        ROSE_ASSERT (operands.size() == 1);
        vars(ip()) = ite(orGate(flag(x86flag_zf), xorGate(flag(x86flag_sf), flag(x86flag_of))), read<32>(operands[0]), vars(ip()));
        break;
      }
      case x86_jg: {
        ROSE_ASSERT (operands.size() == 1);
        vars(ip()) = ite(orGate(flag(x86flag_zf), xorGate(flag(x86flag_sf), flag(x86flag_of))), vars(ip()), read<32>(operands[0]));
        break;
      }
      case x86_jl: {
        ROSE_ASSERT (operands.size() == 1);
        vars(ip()) = ite(xorGate(flag(x86flag_sf), flag(x86flag_of)), read<32>(operands[0]), vars(ip()));
        break;
      }
      case x86_jge: {
        ROSE_ASSERT (operands.size() == 1);
        vars(ip()) = ite(xorGate(flag(x86flag_sf), flag(x86flag_of)), vars(ip()), read<32>(operands[0]));
        break;
      }
      case x86_nop: break;
      case x86_hlt: break; // FIXME
      default: fprintf(stderr, "Bad instruction %s\n", toString(kind).c_str()); abort();
    }
  }

  RegMap writeBack() const { // FIXME
    return registerMap;
  }

  static void processBlock(SgAsmBlock* b) {
    const SgAsmStatementPtrList& stmts = b->get_statementList();
    if (stmts.empty()) return;
    if (!isSgAsmInstruction(stmts[0])) return; // A block containing functions or something
    fprintf(stderr, "Block 0x%08X contains %zu instruction(s)\n", (unsigned int)(b->get_address()), stmts.size());
    NetlistTranslator t;
    for (size_t i = 0; i < stmts.size(); ++i) {
      SgAsmx86Instruction* insn = isSgAsmx86Instruction(stmts[i]);
      ROSE_ASSERT (insn);
      t.translate(insn);
    }
    t.writeBack();
  }

};

int main(int argc, char** argv) {
  SgProject* proj = frontend(argc, argv);
  fprintf(stdout, "(");
  vector<SgNode*> blocks = NodeQuery::querySubTree(proj, V_SgAsmBlock);
  for (size_t i = 0; i < blocks.size(); ++i) {
    SgAsmBlock* b = isSgAsmBlock(blocks[i]);
    ROSE_ASSERT (b);
    NetlistTranslator::processBlock(b);
  }
  fprintf(stdout, ")");
  return 0;
}
