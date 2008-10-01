#ifndef ROSE_X86INSTRUCTIONSEMANTICS_H
#define ROSE_X86INSTRUCTIONSEMANTICS_H

#include "rose.h"
#include <cassert>
#include <cstdio>
#include <iostream>

static int numBytesInAsmType(SgAsmType* ty) {
  switch (ty->variantT()) {
    case V_SgAsmTypeByte: return 1;
    case V_SgAsmTypeWord: return 2;
    case V_SgAsmTypeDoubleWord: return 4;
    case V_SgAsmTypeQuadWord: return 8;
    default: {std::cerr << "Unhandled type " << ty->class_name() << " in numBytesInAsmType" << std::endl; abort();}
  }
}

template <size_t> struct NumberTag {};

static inline X86SegmentRegister getSegregFromMemoryReference(SgAsmMemoryReferenceExpression* mr) {
  X86SegmentRegister segreg = x86_segreg_none;
  SgAsmx86RegisterReferenceExpression* seg = isSgAsmx86RegisterReferenceExpression(mr->get_segment());
  if (seg) {
    ROSE_ASSERT (seg->get_register_class() == x86_regclass_segment);
    segreg = (X86SegmentRegister)(seg->get_register_number());
  } else {
    ROSE_ASSERT (!"Bad segment expr");
  }
  return segreg;
}

template <typename Policy>
struct X86InstructionSemantics {
#define Word(Len) typename Policy::template wordType<(Len)>::type
  Policy& policy;

  X86InstructionSemantics(Policy& policy): policy(policy) {}

  template <size_t Len>
  Word(Len) invertMaybe(const Word(Len)& w, bool inv) {
    if (inv) {
      return policy.invert(w);
    } else {
      return w;
    }
  }

  template <size_t Len>
  Word(Len) negate(const Word(Len)& w) {
    return policy.add(policy.invert(w), policy.template number<Len>(1));
  }

  template <size_t Len, size_t SCLen>
  Word(Len) rotateRight(Word(Len) w, Word(SCLen) sc) {
    return policy.rotateLeft(w, negate<SCLen>(sc));
  }

  template <size_t Len, size_t SCLen>
  Word(Len) shiftLeft(Word(Len) w, Word(SCLen) sc) {
    BOOST_STATIC_ASSERT ((Len & (Len - 1)) == 0); // Len is power of 2
    return policy.and_(policy.rotateLeft(w, sc),
                       policy.invert(policy.template generateMask<Len>(sc)));
  }

  template <size_t Len, size_t SCLen>
  Word(Len) shiftRight(Word(Len) w, Word(SCLen) sc) {
    BOOST_STATIC_ASSERT ((Len & (Len - 1)) == 0); // Len is power of 2
    return policy.and_(rotateRight<Len, SCLen>(w, sc),
                       policy.ite(
                         policy.equalToZero(sc),
                         policy.invert(policy.template number<Len>(0)),
                         policy.template generateMask<Len>(negate<SCLen>(sc))));
  }

  template <size_t Len, size_t SCLen>
  Word(Len) shiftRightArithmetic(Word(Len) w, Word(SCLen) sc) {
    BOOST_STATIC_ASSERT ((Len & (Len - 1)) == 0); // Len is power of 2
    return policy.ite(
             policy.equalToZero(sc),
             w,
             policy.or_(
               shiftRight<Len, SCLen>(w, sc),
               policy.ite(
                 policy.template extract<Len - 1, Len>(w),
                 policy.invert(policy.template generateMask<Len>(negate<SCLen>(sc))),
                 policy.template number<Len>(0))));
  }

  template <size_t Len>
  Word(1) greaterOrEqual(Word(Len) w, uint64_t n) {
    Word(Len) carries = policy.template number<Len>(0);
    policy.addWithCarries(w, policy.template number<Len>((~n) & ((1 << Len) - 1)), policy.true_(), carries);
    return policy.template extract<Len - 1, Len>(carries);
  }

  template <size_t Len> // In bits
  Word(Len) readMemory(X86SegmentRegister segreg, const Word(32)& addr) {
    return policy.template readMemory<Len>(segreg, addr);
  }

  Word(32) readEffectiveAddress(SgAsmExpression* expr) {
    assert (isSgAsmMemoryReferenceExpression(expr));
    return read<32>(isSgAsmMemoryReferenceExpression(expr)->get_address());
  }

  template <size_t Len>
  void writeMemory(X86SegmentRegister segreg, const Word(32)& addr, const Word(Len)& data) {
    policy.template writeMemory<Len>(segreg, addr, data);
  }

  Word(8) readRegHelper(const Word(32)& rawValue, X86PositionInRegister pos, NumberTag<8>) {
    switch (pos) {
      case x86_regpos_low_byte: return policy.template extract<0, 8>(rawValue);
      case x86_regpos_high_byte: return policy.template extract<8, 16>(rawValue);
      default: ROSE_ASSERT (!"Bad position in register");
    }
  }

  Word(16) readRegHelper(const Word(32)& rawValue, X86PositionInRegister pos, NumberTag<16>) {
    switch (pos) {
      case x86_regpos_word: return policy.template extract<0, 16>(rawValue);
      default: ROSE_ASSERT (!"Bad position in register");
    }
  }

  Word(32) readRegHelper(const Word(32)& rawValue, X86PositionInRegister pos, NumberTag<32>) {
    switch (pos) {
      case x86_regpos_dword: return rawValue;
      case x86_regpos_all: return rawValue;
      default: ROSE_ASSERT (!"Bad position in register");
    }
  }

  template <size_t Len>
  Word(Len) read(SgAsmExpression* e) {
    switch (e->variantT()) {
      case V_SgAsmx86RegisterReferenceExpression: {
        SgAsmx86RegisterReferenceExpression* rre = isSgAsmx86RegisterReferenceExpression(e);
        switch (rre->get_register_class()) {
          case x86_regclass_gpr: {
            X86GeneralPurposeRegister reg = (X86GeneralPurposeRegister)(rre->get_register_number());
            Word(32) rawValue = policy.readGPR(reg);
            return readRegHelper(rawValue, rre->get_position_in_register(), NumberTag<Len>());
          }
          default: fprintf(stderr, "Bad register class %u\n", rre->get_register_class()); abort();
        }
        break;
      }
      case V_SgAsmBinaryAdd: {
        return policy.add(read<Len>(isSgAsmBinaryAdd(e)->get_lhs()), read<Len>(isSgAsmBinaryAdd(e)->get_rhs()));
      }
      case V_SgAsmBinaryMultiply: {
        SgAsmByteValueExpression* rhs = isSgAsmByteValueExpression(isSgAsmBinaryMultiply(e)->get_rhs());
        ROSE_ASSERT (rhs);
        SgAsmExpression* lhs = isSgAsmBinaryMultiply(e)->get_lhs();
        return policy.template extract<0, Len>(policy.unsignedMultiply(read<Len>(lhs), read<8>(rhs)));
      }
      case V_SgAsmMemoryReferenceExpression: {
        return readMemory<Len>(getSegregFromMemoryReference(isSgAsmMemoryReferenceExpression(e)), readEffectiveAddress(e));
      }
      case V_SgAsmByteValueExpression: {
        uint64_t val = isSgAsmByteValueExpression(e)->get_value();
        ROSE_ASSERT ((val >> Len) == 0);
        return policy.template number<Len>(val);
      }
      case V_SgAsmWordValueExpression: {
        uint64_t val = isSgAsmWordValueExpression(e)->get_value();
        ROSE_ASSERT ((val >> Len) == 0);
        return policy.template number<Len>(val);
      }
      case V_SgAsmDoubleWordValueExpression: {
        uint64_t val = isSgAsmDoubleWordValueExpression(e)->get_value();
        ROSE_ASSERT ((val >> Len) == 0);
        return policy.template number<Len>(val);
      }
      case V_SgAsmQuadWordValueExpression: {
        uint64_t val = isSgAsmQuadWordValueExpression(e)->get_value();
        ROSE_ASSERT ((val >> Len) == 0);
        return policy.template number<Len>(val);
      }
      default: fprintf(stderr, "Bad variant %s in read\n", e->class_name().c_str()); abort();
    }
  }

  void write8(SgAsmExpression* e, const Word(8)& value) {
    switch (e->variantT()) {
      case V_SgAsmx86RegisterReferenceExpression: {
        SgAsmx86RegisterReferenceExpression* rre = isSgAsmx86RegisterReferenceExpression(e);
        switch (rre->get_register_class()) {
          case x86_regclass_gpr: {
            X86GeneralPurposeRegister reg = (X86GeneralPurposeRegister)(rre->get_register_number());
            Word(32) oldValue = policy.readGPR(reg);
            switch (rre->get_position_in_register()) {
              case x86_regpos_low_byte: {
                policy.writeGPR(reg, policy.concat(value, policy.template extract<8, 32>(oldValue)));
                break;
              }
              case x86_regpos_high_byte: {
                policy.writeGPR(reg, policy.concat(policy.template extract<0, 8>(oldValue), policy.concat(value, policy.template extract<16, 32>(oldValue))));
                break;
              }
              default: ROSE_ASSERT (!"Bad position in register");
            }
            break;
          }
          default: fprintf(stderr, "Bad register class %u\n", rre->get_register_class()); abort();
        }
        break;
      }
      case V_SgAsmMemoryReferenceExpression: {
        writeMemory<8>(getSegregFromMemoryReference(isSgAsmMemoryReferenceExpression(e)), readEffectiveAddress(e), value);
        break;
      }
      default: fprintf(stderr, "Bad variant %s in write8\n", e->class_name().c_str()); abort();
    }
  }

  void write16(SgAsmExpression* e, const Word(16)& value) {
    switch (e->variantT()) {
      case V_SgAsmx86RegisterReferenceExpression: {
        SgAsmx86RegisterReferenceExpression* rre = isSgAsmx86RegisterReferenceExpression(e);
        switch (rre->get_register_class()) {
          case x86_regclass_gpr: {
            X86GeneralPurposeRegister reg = (X86GeneralPurposeRegister)(rre->get_register_number());
            Word(32) oldValue = policy.readGPR(reg);
            switch (rre->get_position_in_register()) {
              case x86_regpos_word: {
                policy.writeGPR(reg, policy.concat(value, policy.template extract<16, 32>(oldValue)));
                break;
              }
              default: ROSE_ASSERT (!"Bad position in register");
            }
            break;
          }
          default: fprintf(stderr, "Bad register class %u\n", rre->get_register_class()); abort();
        }
        break;
      }
      case V_SgAsmMemoryReferenceExpression: {
        writeMemory<16>(getSegregFromMemoryReference(isSgAsmMemoryReferenceExpression(e)), readEffectiveAddress(e), value);
        break;
      }
      default: fprintf(stderr, "Bad variant %s in write16\n", e->class_name().c_str()); abort();
    }
  }

  void write32(SgAsmExpression* e, const Word(32)& value) {
    switch (e->variantT()) {
      case V_SgAsmx86RegisterReferenceExpression: {
        SgAsmx86RegisterReferenceExpression* rre = isSgAsmx86RegisterReferenceExpression(e);
        switch (rre->get_register_class()) {
          case x86_regclass_gpr: {
            X86GeneralPurposeRegister reg = (X86GeneralPurposeRegister)(rre->get_register_number());
            switch (rre->get_position_in_register()) {
              case x86_regpos_dword:
              case x86_regpos_all: {
                break;
              }
              default: ROSE_ASSERT (!"Bad position in register");
            }
            policy.writeGPR(reg, value);
            break;
          }
          default: fprintf(stderr, "Bad register class %u\n", rre->get_register_class()); abort();
        }
        break;
      }
      case V_SgAsmMemoryReferenceExpression: {
        writeMemory<32>(getSegregFromMemoryReference(isSgAsmMemoryReferenceExpression(e)), readEffectiveAddress(e), value);
        break;
      }
      default: fprintf(stderr, "Bad variant %s in write32\n", e->class_name().c_str()); abort();
    }
  }

  Word(1) parity(Word(8) w) {
    Word(1) p01 = policy.xor_(policy.template extract<0, 1>(w), policy.template extract<1, 2>(w));
    Word(1) p23 = policy.xor_(policy.template extract<2, 3>(w), policy.template extract<3, 4>(w));
    Word(1) p45 = policy.xor_(policy.template extract<4, 5>(w), policy.template extract<5, 6>(w));
    Word(1) p67 = policy.xor_(policy.template extract<6, 7>(w), policy.template extract<7, 8>(w));
    Word(1) p0123 = policy.xor_(p01, p23);
    Word(1) p4567 = policy.xor_(p45, p67);
    return policy.xor_(p0123, p4567);
  }

  template <size_t Len>
  void setFlagsForResult(const Word(Len)& result) {
    policy.writeFlag(x86_flag_pf, parity(policy.template extract<0, 8>(result)));
    policy.writeFlag(x86_flag_sf, policy.template extract<Len - 1, Len>(result));
    policy.writeFlag(x86_flag_zf, policy.equalToZero(result));
  }

  template <size_t Len>
  void setFlagsForResult(const Word(Len)& result, Word(1) cond) {
    policy.writeFlag(x86_flag_pf, policy.ite(cond, parity(policy.template extract<0, 8>(result)), policy.readFlag(x86_flag_pf)));
    policy.writeFlag(x86_flag_sf, policy.ite(cond, policy.template extract<Len - 1, Len>(result), policy.readFlag(x86_flag_sf)));
    policy.writeFlag(x86_flag_zf, policy.ite(cond, policy.equalToZero(result), policy.readFlag(x86_flag_zf)));
  }

  template <size_t Len>
  Word(Len) doAddOperation(const Word(Len)& a, const Word(Len)& b, bool invertCarries, Word(1) carryIn) { // Does add (subtract with two's complement input and invertCarries set), and sets correct flags; only does this if cond is true
    Word(Len) carries = policy.template number<Len>(0);
    Word(Len) result = policy.addWithCarries(a, b, invertMaybe<1>(carryIn, invertCarries), carries);
    setFlagsForResult<Len>(result);
    policy.writeFlag(x86_flag_af, invertMaybe<1>(policy.template extract<3, 4>(carries), invertCarries));
    policy.writeFlag(x86_flag_cf, invertMaybe<1>(policy.template extract<Len - 1, Len>(carries), invertCarries));
    policy.writeFlag(x86_flag_of, policy.xor_(policy.template extract<Len - 1, Len>(carries), policy.template extract<Len - 2, Len - 1>(carries)));
    return result;
  }

  template <size_t Len>
  Word(Len) doAddOperation(const Word(Len)& a, const Word(Len)& b, bool invertCarries, Word(1) carryIn, Word(1) cond) { // Does add (subtract with two's complement input and invertCarries set), and sets correct flags; only does this if cond is true
    Word(Len) carries = policy.template number<Len>(0);
    Word(Len) result = policy.addWithCarries(a, b, invertMaybe<1>(carryIn, invertCarries), carries);
    setFlagsForResult<Len>(result, cond);
    policy.writeFlag(x86_flag_af, policy.ite(cond, invertMaybe<1>(policy.template extract<3, 4>(carries), invertCarries), policy.readFlag(x86_flag_af)));
    policy.writeFlag(x86_flag_cf, policy.ite(cond, invertMaybe<1>(policy.template extract<Len - 1, Len>(carries), invertCarries), policy.readFlag(x86_flag_cf)));
    policy.writeFlag(x86_flag_of, policy.ite(cond, policy.xor_(policy.template extract<Len - 1, Len>(carries), policy.template extract<Len - 2, Len - 1>(carries)), policy.readFlag(x86_flag_of)));
    return result;
  }

  template <size_t Len>
  Word(Len) doIncOperation(const Word(Len)& a, bool dec, bool setCarry) { // Does inc (dec with dec set), and sets correct flags
    Word(Len) carries = policy.template number<Len>(0);
    Word(Len) result = policy.addWithCarries(a, policy.template number<Len>(dec ? -1 : 1), policy.false_(), carries);
    setFlagsForResult<Len>(result);
    policy.writeFlag(x86_flag_af, invertMaybe<1>(policy.template extract<3, 4>(carries), dec));
    policy.writeFlag(x86_flag_of, policy.xor_(policy.template extract<Len - 1, Len>(carries), policy.template extract<Len - 2, Len - 1>(carries)));
    if (setCarry) {
      policy.writeFlag(x86_flag_cf, policy.template extract<Len - 1, Len>(carries));
    }
    return result;
  }

  void translate(SgAsmx86Instruction* insn) {
    // fprintf(stderr, "%s\n", unparseInstructionWithAddress(insn).c_str());
    policy.writeIP(policy.template number<32>((unsigned int)(insn->get_address() + insn->get_raw_bytes().size())));
    X86InstructionKind kind = insn->get_kind();
    const SgAsmExpressionPtrList& operands = insn->get_operandList()->get_operands();
    switch (kind) {
      case x86_mov: {
        ROSE_ASSERT (operands.size() == 2);
        switch (numBytesInAsmType(operands[0]->get_type())) {
          case 1: write8(operands[0], read<8>(operands[1])); break;
          case 2: write16(operands[0], read<16>(operands[1])); break;
          case 4: write32(operands[0], read<32>(operands[1])); break;
          default: ROSE_ASSERT ("Bad size"); break;
        }
        break;
      }
      case x86_xchg: {
        ROSE_ASSERT (operands.size() == 2);
        switch (numBytesInAsmType(operands[0]->get_type())) {
          case 1: {Word(8) temp = read<8>(operands[1]); write8(operands[1], read<8>(operands[0])); write8(operands[0], temp); break;}
          case 2: {Word(16) temp = read<16>(operands[1]); write16(operands[1], read<16>(operands[0])); write16(operands[0], temp); break;}
          case 4: {Word(32) temp = read<32>(operands[1]); write32(operands[1], read<32>(operands[0])); write32(operands[0], temp); break;}
          default: ROSE_ASSERT ("Bad size"); break;
        }
        break;
      }
      case x86_movzx: {
        ROSE_ASSERT (operands.size() == 2);
        switch (numBytesInAsmType(operands[0]->get_type())) {
          case 2: write16(operands[0], policy.concat(read<8>(operands[1]), policy.template number<8>(0))); break;
          case 4: {
            switch (numBytesInAsmType(operands[1]->get_type())) {
              case 1: write32(operands[0], policy.concat(read<8>(operands[1]), policy.template number<24>(0))); break;
              case 2: write32(operands[0], policy.concat(read<16>(operands[1]), policy.template number<16>(0))); break;
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
            Word(8) op1 = read<8>(operands[1]);
            Word(16) result = policy.template signExtend<8, 16>(op1);
            write16(operands[0], result);
            break;
          }
          case 4: {
            switch (numBytesInAsmType(operands[1]->get_type())) {
              case 1: {
                Word(8) op1 = read<8>(operands[1]);
                Word(32) result = policy.template signExtend<8, 32>(op1);
                write32(operands[0], result);
                break;
              }
              case 2: {
                Word(16) op1 = read<16>(operands[1]);
                Word(32) result = policy.template signExtend<16, 32>(op1);
                write32(operands[0], result);
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
        policy.writeGPR(x86_gpr_ax, policy.concat(policy.template signExtend<8, 16>(policy.template extract<0, 8>(policy.readGPR(x86_gpr_ax))), policy.template extract<16, 32>(policy.readGPR(x86_gpr_ax))));
        break;
      }
      case x86_cwde: {
        ROSE_ASSERT (operands.size() == 0);
        policy.writeGPR(x86_gpr_ax, policy.template signExtend<16, 32>(policy.template extract<0, 16>(policy.readGPR(x86_gpr_ax))));
        break;
      }
      case x86_cwd: {
        ROSE_ASSERT (operands.size() == 0);
        policy.writeGPR(x86_gpr_dx, policy.concat(policy.template extract<16, 32>(policy.template signExtend<16, 32>(policy.template extract<0, 16>(policy.readGPR(x86_gpr_ax)))), policy.template extract<16, 32>(policy.readGPR(x86_gpr_dx))));
        break;
      }
      case x86_cdq: {
        ROSE_ASSERT (operands.size() == 0);
        policy.writeGPR(x86_gpr_dx, policy.template extract<32, 64>(policy.template signExtend<32, 64>(policy.readGPR(x86_gpr_ax))));
        break;
      }
      case x86_lea: {
        ROSE_ASSERT (operands.size() == 2);
        write32(operands[0], readEffectiveAddress(operands[1]));
        break;
      }
      case x86_and: {
        ROSE_ASSERT (operands.size() == 2);
        switch (numBytesInAsmType(operands[0]->get_type())) {
          case 1: {
            Word(8) result = policy.and_(read<8>(operands[0]), read<8>(operands[1]));
            setFlagsForResult<8>(result);
            write8(operands[0], result);
            break;
          }
          case 2: {
            Word(16) result = policy.and_(read<16>(operands[0]), read<16>(operands[1]));
            setFlagsForResult<16>(result);
            write16(operands[0], result);
            break;
          }
          case 4: {
            Word(32) result = policy.and_(read<32>(operands[0]), read<32>(operands[1]));
            setFlagsForResult<32>(result);
            write32(operands[0], result);
            break;
          }
          default: ROSE_ASSERT (!"Bad size"); break;
        }
        policy.writeFlag(x86_flag_of, policy.false_());
        policy.writeFlag(x86_flag_af, policy.false_()); // Undefined
        policy.writeFlag(x86_flag_cf, policy.false_());
        break;
      }
      case x86_or: {
        ROSE_ASSERT (operands.size() == 2);
        switch (numBytesInAsmType(operands[0]->get_type())) {
          case 1: {
            Word(8) result = policy.or_(read<8>(operands[0]), read<8>(operands[1]));
            setFlagsForResult<8>(result);
            write8(operands[0], result);
            break;
          }
          case 2: {
            Word(16) result = policy.or_(read<16>(operands[0]), read<16>(operands[1]));
            setFlagsForResult<16>(result);
            write16(operands[0], result);
            break;
          }
          case 4: {
            Word(32) result = policy.or_(read<32>(operands[0]), read<32>(operands[1]));
            setFlagsForResult<32>(result);
            write32(operands[0], result);
            break;
          }
          default: ROSE_ASSERT (!"Bad size"); break;
        }
        policy.writeFlag(x86_flag_of, policy.false_());
        policy.writeFlag(x86_flag_af, policy.false_()); // Undefined
        policy.writeFlag(x86_flag_cf, policy.false_());
        break;
      }
      case x86_test: {
        ROSE_ASSERT (operands.size() == 2);
        switch (numBytesInAsmType(operands[0]->get_type())) {
          case 1: {
            Word(8) result = policy.and_(read<8>(operands[0]), read<8>(operands[1]));
            setFlagsForResult<8>(result);
            break;
          }
          case 2: {
            Word(16) result = policy.and_(read<16>(operands[0]), read<16>(operands[1]));
            setFlagsForResult<16>(result);
            break;
          }
          case 4: {
            Word(32) result = policy.and_(read<32>(operands[0]), read<32>(operands[1]));
            setFlagsForResult<32>(result);
            break;
          }
          default: ROSE_ASSERT (!"Bad size"); break;
        }
        policy.writeFlag(x86_flag_of, policy.false_());
        policy.writeFlag(x86_flag_af, policy.false_()); // Undefined
        policy.writeFlag(x86_flag_cf, policy.false_());
        break;
      }
      case x86_xor: {
        ROSE_ASSERT (operands.size() == 2);
        switch (numBytesInAsmType(operands[0]->get_type())) {
          case 1: {
            Word(8) result = policy.xor_(read<8>(operands[0]), read<8>(operands[1]));
            setFlagsForResult<8>(result);
            write8(operands[0], result);
            break;
          }
          case 2: {
            Word(16) result = policy.xor_(read<16>(operands[0]), read<16>(operands[1]));
            setFlagsForResult<16>(result);
            write16(operands[0], result);
            break;
          }
          case 4: {
            Word(32) result = policy.xor_(read<32>(operands[0]), read<32>(operands[1]));
            setFlagsForResult<32>(result);
            write32(operands[0], result);
            break;
          }
          default: ROSE_ASSERT (!"Bad size"); break;
        }
        policy.writeFlag(x86_flag_of, policy.false_());
        policy.writeFlag(x86_flag_af, policy.false_()); // Undefined
        policy.writeFlag(x86_flag_cf, policy.false_());
        break;
      }
      case x86_not: {
        ROSE_ASSERT (operands.size() == 1);
        switch (numBytesInAsmType(operands[0]->get_type())) {
          case 1: {
            Word(8) result = policy.invert(read<8>(operands[0]));
            write8(operands[0], result);
            break;
          }
          case 2: {
            Word(16) result = policy.invert(read<16>(operands[0]));
            write16(operands[0], result);
            break;
          }
          case 4: {
            Word(32) result = policy.invert(read<32>(operands[0]));
            write32(operands[0], result);
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
            Word(8) result = doAddOperation<8>(read<8>(operands[0]), read<8>(operands[1]), false, policy.false_());
            write8(operands[0], result);
            break;
          }
          case 2: {
            Word(16) result = doAddOperation<16>(read<16>(operands[0]), read<16>(operands[1]), false, policy.false_());
            write16(operands[0], result);
            break;
          }
          case 4: {
            Word(32) result = doAddOperation<32>(read<32>(operands[0]), read<32>(operands[1]), false, policy.false_());
            write32(operands[0], result);
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
            Word(8) result = doAddOperation<8>(read<8>(operands[0]), read<8>(operands[1]), false, policy.readFlag(x86_flag_cf));
            write8(operands[0], result);
            break;
          }
          case 2: {
            Word(16) result = doAddOperation<16>(read<16>(operands[0]), read<16>(operands[1]), false, policy.readFlag(x86_flag_cf));
            write16(operands[0], result);
            break;
          }
          case 4: {
            Word(32) result = doAddOperation<32>(read<32>(operands[0]), read<32>(operands[1]), false, policy.readFlag(x86_flag_cf));
            write32(operands[0], result);
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
            Word(8) result = doAddOperation<8>(read<8>(operands[0]), policy.invert(read<8>(operands[1])), true, policy.false_());
            write8(operands[0], result);
            break;
          }
          case 2: {
            Word(16) result = doAddOperation<16>(read<16>(operands[0]), policy.invert(read<16>(operands[1])), true, policy.false_());
            write16(operands[0], result);
            break;
          }
          case 4: {
            Word(32) result = doAddOperation<32>(read<32>(operands[0]), policy.invert(read<32>(operands[1])), true, policy.false_());
            write32(operands[0], result);
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
            Word(8) result = doAddOperation<8>(read<8>(operands[0]), policy.invert(read<8>(operands[1])), true, policy.readFlag(x86_flag_cf));
            write8(operands[0], result);
            break;
          }
          case 2: {
            Word(16) result = doAddOperation<16>(read<16>(operands[0]), policy.invert(read<16>(operands[1])), true, policy.readFlag(x86_flag_cf));
            write16(operands[0], result);
            break;
          }
          case 4: {
            Word(32) result = doAddOperation<32>(read<32>(operands[0]), policy.invert(read<32>(operands[1])), true, policy.readFlag(x86_flag_cf));
            write32(operands[0], result);
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
            doAddOperation<8>(read<8>(operands[0]), policy.invert(read<8>(operands[1])), true, policy.false_());
            break;
          }
          case 2: {
            doAddOperation<16>(read<16>(operands[0]), policy.invert(read<16>(operands[1])), true, policy.false_());
            break;
          }
          case 4: {
            doAddOperation<32>(read<32>(operands[0]), policy.invert(read<32>(operands[1])), true, policy.false_());
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
            Word(8) result = negate<8>(read<8>(operands[0]));
            write8(operands[0], result);
            break;
          }
          case 2: {
            Word(16) result = negate<16>(read<16>(operands[0]));
            write16(operands[0], result);
            break;
          }
          case 4: {
            Word(32) result = negate<32>(read<32>(operands[0]));
            write32(operands[0], result);
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
            Word(8) result = doIncOperation<8>(read<8>(operands[0]), false, false);
            write8(operands[0], result);
            break;
          }
          case 2: {
            Word(16) result = doIncOperation<16>(read<16>(operands[0]), false, false);
            write16(operands[0], result);
            break;
          }
          case 4: {
            Word(32) result = doIncOperation<32>(read<32>(operands[0]), false, false);
            write32(operands[0], result);
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
            Word(8) result = doIncOperation<8>(read<8>(operands[0]), true, false);
            write8(operands[0], result);
            break;
          }
          case 2: {
            Word(16) result = doIncOperation<16>(read<16>(operands[0]), true, false);
            write16(operands[0], result);
            break;
          }
          case 4: {
            Word(32) result = doIncOperation<32>(read<32>(operands[0]), true, false);
            write32(operands[0], result);
            break;
          }
          default: ROSE_ASSERT (!"Bad size"); break;
        }
        break;
      }
      case x86_shl: {
        Word(5) shiftCount = policy.template extract<0, 5>(read<8>(operands[1]));
        Word(1) shiftCountZero = policy.equalToZero(shiftCount);
        policy.writeFlag(x86_flag_af, policy.ite(shiftCountZero, policy.readFlag(x86_flag_af), policy.false_())); // Undefined
        switch (numBytesInAsmType(operands[0]->get_type())) {
          case 1: {
            Word(8) op = read<8>(operands[0]);
            Word(8) output = shiftLeft<8, 5>(op, shiftCount);
            Word(1) newCf = policy.ite(shiftCountZero, policy.readFlag(x86_flag_cf), policy.template extract<7, 8>(shiftLeft<8, 5>(op, policy.add(shiftCount, policy.template number<5>(7)))));
            policy.writeFlag(x86_flag_cf, newCf);
            policy.writeFlag(x86_flag_of, policy.ite(shiftCountZero, policy.readFlag(x86_flag_of), policy.xor_(policy.template extract<7, 8>(output), newCf)));
            write8(operands[0], output);
            setFlagsForResult<8>(output, policy.invert(shiftCountZero));
            break;
          }
          case 2: {
            Word(16) op = read<16>(operands[0]);
            Word(16) output = shiftLeft<16, 5>(op, shiftCount);
            Word(1) newCf = policy.ite(shiftCountZero, policy.readFlag(x86_flag_cf), policy.template extract<15, 16>(shiftLeft<16, 5>(op, policy.add(shiftCount, policy.template number<5>(15)))));
            policy.writeFlag(x86_flag_cf, newCf);
            policy.writeFlag(x86_flag_of, policy.ite(shiftCountZero, policy.readFlag(x86_flag_of), policy.xor_(policy.template extract<15, 16>(output), newCf)));
            write16(operands[0], output);
            setFlagsForResult<16>(output, policy.invert(shiftCountZero));
            break;
          }
          case 4: {
            Word(32) op = read<32>(operands[0]);
            Word(32) output = shiftLeft<32, 5>(op, shiftCount);
            Word(1) newCf = policy.ite(shiftCountZero, policy.readFlag(x86_flag_cf), policy.template extract<31, 32>(shiftLeft<32, 5>(op, policy.add(shiftCount, policy.template number<5>(31)))));
            policy.writeFlag(x86_flag_cf, newCf);
            policy.writeFlag(x86_flag_of, policy.ite(shiftCountZero, policy.readFlag(x86_flag_of), policy.xor_(policy.template extract<31, 32>(output), newCf)));
            write32(operands[0], output);
            setFlagsForResult<32>(output, policy.invert(shiftCountZero));
            break;
          }
          default: ROSE_ASSERT (!"Bad size");
        }
        break;
      }
      case x86_shr: {
        Word(5) shiftCount = policy.template extract<0, 5>(read<8>(operands[1]));
        Word(1) shiftCountZero = policy.equalToZero(shiftCount);
        policy.writeFlag(x86_flag_af, policy.ite(shiftCountZero, policy.readFlag(x86_flag_af), policy.false_())); // Undefined
        switch (numBytesInAsmType(operands[0]->get_type())) {
          case 1: {
            Word(8) op = read<8>(operands[0]);
            Word(8) output = shiftRight<8, 5>(op, shiftCount);
            Word(1) newCf = policy.ite(shiftCountZero, policy.readFlag(x86_flag_cf), policy.template extract<0, 1>(shiftRight<8, 5>(op, policy.add(shiftCount, policy.template number<5>(7)))));
            policy.writeFlag(x86_flag_cf, newCf);
            policy.writeFlag(x86_flag_of, policy.ite(shiftCountZero, policy.readFlag(x86_flag_of), policy.template extract<7, 8>(op)));
            write8(operands[0], output);
            setFlagsForResult<8>(output, policy.invert(shiftCountZero));
            break;
          }
          case 2: {
            Word(16) op = read<16>(operands[0]);
            Word(16) output = shiftRight<16, 5>(op, shiftCount);
            Word(1) newCf = policy.ite(shiftCountZero, policy.readFlag(x86_flag_cf), policy.template extract<0, 1>(shiftRight<16, 5>(op, policy.add(shiftCount, policy.template number<5>(15)))));
            policy.writeFlag(x86_flag_cf, newCf);
            policy.writeFlag(x86_flag_of, policy.ite(shiftCountZero, policy.readFlag(x86_flag_of), policy.template extract<15, 16>(op)));
            write16(operands[0], output);
            setFlagsForResult<16>(output, policy.invert(shiftCountZero));
            break;
          }
          case 4: {
            Word(32) op = read<32>(operands[0]);
            Word(32) output = shiftRight<32, 5>(op, shiftCount);
            Word(1) newCf = policy.ite(shiftCountZero, policy.readFlag(x86_flag_cf), policy.template extract<0, 1>(shiftRight<32, 5>(op, policy.add(shiftCount, policy.template number<5>(31)))));
            policy.writeFlag(x86_flag_cf, newCf);
            policy.writeFlag(x86_flag_of, policy.ite(shiftCountZero, policy.readFlag(x86_flag_of), policy.template extract<31, 32>(op)));
            write32(operands[0], output);
            setFlagsForResult<32>(output, policy.invert(shiftCountZero));
            break;
          }
          default: ROSE_ASSERT (!"Bad size");
        }
        break;
      }
      case x86_sar: {
        Word(5) shiftCount = policy.template extract<0, 5>(read<8>(operands[1]));
        Word(1) shiftCountZero = policy.equalToZero(shiftCount);
        Word(1) shiftCountNotZero = policy.invert(shiftCountZero);
        policy.writeFlag(x86_flag_af, policy.ite(shiftCountZero, policy.readFlag(x86_flag_af), policy.false_())); // Undefined
        switch (numBytesInAsmType(operands[0]->get_type())) {
          case 1: {
            Word(8) op = read<8>(operands[0]);
            Word(8) output = shiftRightArithmetic<8, 5>(op, shiftCount);
            Word(1) newCf = policy.ite(shiftCountZero, policy.readFlag(x86_flag_cf), policy.template extract<0, 1>(shiftRight<8, 5>(op, policy.add(shiftCount, policy.template number<5>(7)))));
            policy.writeFlag(x86_flag_cf, newCf);
            policy.writeFlag(x86_flag_of, policy.ite(shiftCountZero, policy.readFlag(x86_flag_of), policy.false_())); // No change with sc = 0, clear when sc = 1, undefined otherwise
            write8(operands[0], output);
            setFlagsForResult<8>(output, shiftCountNotZero);
            break;
          }
          case 2: {
            Word(16) op = read<16>(operands[0]);
            Word(16) output = shiftRightArithmetic<16, 5>(op, shiftCount);
            Word(1) newCf = policy.ite(shiftCountZero, policy.readFlag(x86_flag_cf), policy.template extract<0, 1>(shiftRight<16, 5>(op, policy.add(shiftCount, policy.template number<5>(15)))));
            policy.writeFlag(x86_flag_cf, newCf);
            policy.writeFlag(x86_flag_of, policy.ite(shiftCountZero, policy.readFlag(x86_flag_of), policy.false_())); // No change with sc = 0, clear when sc = 1, undefined otherwise
            write16(operands[0], output);
            setFlagsForResult<16>(output, shiftCountNotZero);
            break;
          }
          case 4: {
            Word(32) op = read<32>(operands[0]);
            Word(32) output = shiftRightArithmetic<32, 5>(op, shiftCount);
            Word(1) newCf = policy.ite(shiftCountZero, policy.readFlag(x86_flag_cf), policy.template extract<0, 1>(shiftRight<32, 5>(op, policy.add(shiftCount, policy.template number<5>(31)))));
            policy.writeFlag(x86_flag_cf, newCf);
            policy.writeFlag(x86_flag_of, policy.ite(shiftCountZero, policy.readFlag(x86_flag_of), policy.false_())); // No change with sc = 0, clear when sc = 1, undefined otherwise
            write32(operands[0], output);
            setFlagsForResult<32>(output, shiftCountNotZero);
            break;
          }
          default: ROSE_ASSERT (!"Bad size");
        }
        break;
      }
      case x86_rol: {
        switch (numBytesInAsmType(operands[0]->get_type())) {
          case 1: {
            Word(8) op = read<8>(operands[0]);
            Word(5) shiftCount = policy.template extract<0, 5>(read<8>(operands[1]));
            Word(8) output = policy.rotateLeft(op, shiftCount);
            policy.writeFlag(x86_flag_cf, policy.ite(policy.equalToZero(shiftCount), policy.readFlag(x86_flag_cf), policy.template extract<0, 1>(output)));
            policy.writeFlag(x86_flag_of, policy.ite(policy.equalToZero(shiftCount), policy.readFlag(x86_flag_of), policy.xor_(policy.template extract<0, 1>(output), policy.template extract<7, 8>(output))));
            write8(operands[0], output);
            break;
          }
          case 2: {
            Word(16) op = read<16>(operands[0]);
            Word(5) shiftCount = policy.template extract<0, 5>(read<8>(operands[1]));
            Word(16) output = policy.rotateLeft(op, shiftCount);
            policy.writeFlag(x86_flag_cf, policy.ite(policy.equalToZero(shiftCount), policy.readFlag(x86_flag_cf), policy.template extract<0, 1>(output)));
            policy.writeFlag(x86_flag_of, policy.ite(policy.equalToZero(shiftCount), policy.readFlag(x86_flag_of), policy.xor_(policy.template extract<0, 1>(output), policy.template extract<15, 16>(output))));
            write16(operands[0], output);
            break;
          }
          case 4: {
            Word(32) op = read<32>(operands[0]);
            Word(5) shiftCount = policy.template extract<0, 5>(read<8>(operands[1]));
            Word(32) output = policy.rotateLeft(op, shiftCount);
            policy.writeFlag(x86_flag_cf, policy.ite(policy.equalToZero(shiftCount), policy.readFlag(x86_flag_cf), policy.template extract<0, 1>(output)));
            policy.writeFlag(x86_flag_of, policy.ite(policy.equalToZero(shiftCount), policy.readFlag(x86_flag_of), policy.xor_(policy.template extract<0, 1>(output), policy.template extract<31, 32>(output))));
            write32(operands[0], output);
            break;
          }
          default: ROSE_ASSERT (!"Bad size");
        }
        break;
      }
      case x86_ror: {
        switch (numBytesInAsmType(operands[0]->get_type())) {
          case 1: {
            Word(8) op = read<8>(operands[0]);
            Word(5) shiftCount = policy.template extract<0, 5>(read<8>(operands[1]));
            Word(8) output = rotateRight<8, 5>(op, shiftCount);
            policy.writeFlag(x86_flag_cf, policy.ite(policy.equalToZero(shiftCount), policy.readFlag(x86_flag_cf), policy.template extract<7, 8>(output)));
            policy.writeFlag(x86_flag_of, policy.ite(policy.equalToZero(shiftCount), policy.readFlag(x86_flag_of), policy.xor_(policy.template extract<6, 7>(output), policy.template extract<7, 8>(output))));
            write8(operands[0], output);
            break;
          }
          case 2: {
            Word(16) op = read<16>(operands[0]);
            Word(5) shiftCount = policy.template extract<0, 5>(read<8>(operands[1]));
            Word(16) output = rotateRight<16, 5>(op, shiftCount);
            policy.writeFlag(x86_flag_cf, policy.ite(policy.equalToZero(shiftCount), policy.readFlag(x86_flag_cf), policy.template extract<15, 16>(output)));
            policy.writeFlag(x86_flag_of, policy.ite(policy.equalToZero(shiftCount), policy.readFlag(x86_flag_of), policy.xor_(policy.template extract<14, 15>(output), policy.template extract<15, 16>(output))));
            write16(operands[0], output);
            break;
          }
          case 4: {
            Word(32) op = read<32>(operands[0]);
            Word(5) shiftCount = policy.template extract<0, 5>(read<8>(operands[1]));
            Word(32) output = rotateRight<32, 5>(op, shiftCount);
            policy.writeFlag(x86_flag_cf, policy.ite(policy.equalToZero(shiftCount), policy.readFlag(x86_flag_cf), policy.template extract<31, 32>(output)));
            policy.writeFlag(x86_flag_of, policy.ite(policy.equalToZero(shiftCount), policy.readFlag(x86_flag_of), policy.xor_(policy.template extract<30, 31>(output), policy.template extract<31, 32>(output))));
            write32(operands[0], output);
            break;
          }
          default: ROSE_ASSERT (!"Bad size");
        }
        break;
      }
      case x86_shld: {
        Word(5) shiftCount = policy.template extract<0, 5>(read<8>(operands[2]));
        switch (numBytesInAsmType(operands[0]->get_type())) {
          case 2: {
            Word(16) op1 = read<16>(operands[0]);
            Word(16) op2 = read<16>(operands[1]);
            Word(16) output1 = shiftLeft<16, 5>(op1, shiftCount);
            Word(16) output2 = policy.ite(policy.equalToZero(shiftCount),
                                          policy.template number<16>(0),
                                          shiftRight<16, 5>(op2, negate<5>(shiftCount)));
            Word(16) output = policy.or_(output1, output2);
            Word(1) newCf = policy.ite(policy.equalToZero(shiftCount),
                                       policy.readFlag(x86_flag_cf),
                                       policy.template extract<15, 16>(shiftLeft<16, 5>(op1, policy.add(shiftCount, policy.template number<5>(15)))));
            policy.writeFlag(x86_flag_cf, newCf);
            Word(1) newOf = policy.ite(policy.equalToZero(shiftCount),
                                       policy.readFlag(x86_flag_of), 
                                       policy.xor_(policy.template extract<15, 16>(output), newCf));
            policy.writeFlag(x86_flag_of, newOf);
            write16(operands[0], output);
            setFlagsForResult<16>(output);
            policy.writeFlag(x86_flag_af, policy.ite(policy.equalToZero(shiftCount),
                                                     policy.readFlag(x86_flag_af),
                                                     policy.false_())); // Undefined
            break;
          }
          case 4: {
            Word(32) op1 = read<32>(operands[0]);
            Word(32) op2 = read<32>(operands[1]);
            Word(5) shiftCount = policy.template extract<0, 5>(read<8>(operands[2]));
            Word(32) output1 = shiftLeft<32, 5>(op1, shiftCount);
            Word(32) output2 = policy.ite(policy.equalToZero(shiftCount),
                                          policy.template number<32>(0),
                                          shiftRight<32, 5>(op2, negate<5>(shiftCount)));
            Word(32) output = policy.or_(output1, output2);
            Word(1) newCf = policy.ite(policy.equalToZero(shiftCount),
                                       policy.readFlag(x86_flag_cf),
                                       policy.template extract<31, 32>(shiftLeft<32, 5>(op1, policy.add(shiftCount, policy.template number<5>(31)))));
            policy.writeFlag(x86_flag_cf, newCf);
            Word(1) newOf = policy.ite(policy.equalToZero(shiftCount),
                                       policy.readFlag(x86_flag_of), 
                                       policy.xor_(policy.template extract<31, 32>(output), newCf));
            policy.writeFlag(x86_flag_of, newOf);
            write32(operands[0], output);
            setFlagsForResult<32>(output);
            policy.writeFlag(x86_flag_af, policy.ite(policy.equalToZero(shiftCount),
                                                     policy.readFlag(x86_flag_af),
                                                     policy.false_())); // Undefined
            break;
          }
          default: ROSE_ASSERT (!"Bad size");
        }
        break;
      }
      case x86_shrd: { // FIXME check this
        Word(5) shiftCount = policy.template extract<0, 5>(read<8>(operands[2]));
        switch (numBytesInAsmType(operands[0]->get_type())) {
          case 2: {
            Word(16) op1 = read<16>(operands[0]);
            Word(16) op2 = read<16>(operands[1]);
            Word(16) output1 = shiftRight<16, 5>(op1, shiftCount);
            Word(16) output2 = policy.ite(policy.equalToZero(shiftCount),
                                          policy.template number<16>(0),
                                          shiftLeft<16, 5>(op2, negate<5>(shiftCount)));
            Word(16) output = policy.or_(output1, output2);
            Word(1) newCf = policy.ite(policy.equalToZero(shiftCount),
                                       policy.readFlag(x86_flag_cf),
                                       policy.template extract<0, 1>(shiftRight<16, 5>(op1, policy.add(shiftCount, policy.template number<5>(15)))));
            policy.writeFlag(x86_flag_cf, newCf);
            Word(1) newOf = policy.ite(policy.equalToZero(shiftCount),
                                       policy.readFlag(x86_flag_of), 
                                       policy.xor_(policy.template extract<15, 16>(output),
                                                   policy.template extract<15, 16>(op1)));
            policy.writeFlag(x86_flag_of, newOf);
            write16(operands[0], output);
            setFlagsForResult<16>(output);
            policy.writeFlag(x86_flag_af, policy.ite(policy.equalToZero(shiftCount),
                                                     policy.readFlag(x86_flag_af),
                                                     policy.false_())); // Undefined
            break;
          }
          case 4: {
            Word(32) op1 = read<32>(operands[0]);
            Word(32) op2 = read<32>(operands[1]);
            Word(32) output1 = shiftRight<32, 5>(op1, shiftCount);
            Word(32) output2 = policy.ite(policy.equalToZero(shiftCount),
                                          policy.template number<32>(0),
                                          shiftLeft<32, 5>(op2, negate<5>(shiftCount)));
            Word(32) output = policy.or_(output1, output2);
            Word(1) newCf = policy.ite(policy.equalToZero(shiftCount),
                                       policy.readFlag(x86_flag_cf),
                                       policy.template extract<0, 1>(shiftRight<32, 5>(op1, policy.add(shiftCount, policy.template number<5>(31)))));
            policy.writeFlag(x86_flag_cf, newCf);
            Word(1) newOf = policy.ite(policy.equalToZero(shiftCount),
                                       policy.readFlag(x86_flag_of), 
                                       policy.xor_(policy.template extract<31, 32>(output),
                                                   policy.template extract<31, 32>(op1)));
            policy.writeFlag(x86_flag_of, newOf);
            write32(operands[0], output);
            setFlagsForResult<32>(output);
            policy.writeFlag(x86_flag_af, policy.ite(policy.equalToZero(shiftCount),
                                                     policy.readFlag(x86_flag_af),
                                                     policy.false_())); // Undefined
            break;
          }
          default: ROSE_ASSERT (!"Bad size");
        }
        break;
      }
      case x86_bsf: {
        policy.writeFlag(x86_flag_of, policy.false_()); // Undefined
        policy.writeFlag(x86_flag_sf, policy.false_()); // Undefined
        policy.writeFlag(x86_flag_af, policy.false_()); // Undefined
        policy.writeFlag(x86_flag_pf, policy.false_()); // Undefined
        policy.writeFlag(x86_flag_cf, policy.false_()); // Undefined
        switch (numBytesInAsmType(operands[0]->get_type())) {
          case 2: {
            Word(16) op = read<16>(operands[1]);
            policy.writeFlag(x86_flag_zf, policy.equalToZero(op));
            Word(16) result = policy.ite(policy.readFlag(x86_flag_zf),
                                         read<16>(operands[0]),
                                         policy.leastSignificantSetBit(op));
            write16(operands[0], result);
            break;
          }
          case 4: {
            Word(32) op = read<32>(operands[1]);
              policy.writeFlag(x86_flag_zf, policy.equalToZero(op));
              Word(32) result = policy.ite(policy.readFlag(x86_flag_zf),
                                           read<32>(operands[0]),
                                           policy.leastSignificantSetBit(op));
            write32(operands[0], result);
            break;
          }
          default: ROSE_ASSERT (!"Bad size");
        }
        break;
      }
      case x86_bsr: {
        policy.writeFlag(x86_flag_of, policy.false_()); // Undefined
        policy.writeFlag(x86_flag_sf, policy.false_()); // Undefined
        policy.writeFlag(x86_flag_af, policy.false_()); // Undefined
        policy.writeFlag(x86_flag_pf, policy.false_()); // Undefined
        policy.writeFlag(x86_flag_cf, policy.false_()); // Undefined
        switch (numBytesInAsmType(operands[0]->get_type())) {
          case 2: {
            Word(16) op = read<16>(operands[1]);
            policy.writeFlag(x86_flag_zf, policy.equalToZero(op));
            Word(16) result = policy.ite(policy.readFlag(x86_flag_zf),
                                         read<16>(operands[0]),
                                         policy.mostSignificantSetBit(op));
            write16(operands[0], result);
            break;
          }
          case 4: {
            Word(32) op = read<32>(operands[1]);
            policy.writeFlag(x86_flag_zf, policy.equalToZero(op));
            Word(32) result = policy.ite(policy.readFlag(x86_flag_zf),
                                         read<32>(operands[0]),
                                         policy.mostSignificantSetBit(op));
            write32(operands[0], result);
            break;
          }
          default: ROSE_ASSERT (!"Bad size");
        }
        break;
      }
      case x86_imul: {
        switch (numBytesInAsmType(operands[0]->get_type())) {
          case 1: {
            Word(8) op0 = policy.template extract<0, 8>(policy.readGPR(x86_gpr_ax));
            Word(8) op1 = read<8>(operands[0]);
            Word(16) mulResult = policy.signedMultiply(op0, op1);
            policy.writeGPR(x86_gpr_ax, policy.concat(mulResult, policy.template extract<16, 32>(policy.readGPR(x86_gpr_ax))));
            Word(1) carry = policy.invert(policy.or_(policy.equalToZero(policy.invert(policy.template extract<7, 16>(mulResult))), policy.equalToZero(policy.template extract<7, 16>(mulResult))));
            policy.writeFlag(x86_flag_cf, carry);
            policy.writeFlag(x86_flag_of, carry);
            break;
          }
          case 2: {
            Word(16) op0 = operands.size() == 1 ? policy.template extract<0, 16>(policy.readGPR(x86_gpr_ax)) : read<16>(operands[operands.size() - 2]);
            Word(16) op1 = read<16>(operands[operands.size() - 1]);
            Word(32) mulResult = policy.signedMultiply(op0, op1);
            if (operands.size() == 1) {
              policy.writeGPR(x86_gpr_ax, policy.concat(policy.template extract<0, 16>(mulResult), policy.template extract<16, 32>(policy.readGPR(x86_gpr_ax))));
              policy.writeGPR(x86_gpr_dx, policy.concat(policy.template extract<16, 32>(mulResult), policy.template extract<16, 32>(policy.readGPR(x86_gpr_dx))));
            } else {
              write16(operands[0], policy.template extract<0, 16>(mulResult));
            }
            Word(1) carry = policy.invert(policy.or_(policy.equalToZero(policy.invert(policy.template extract<7, 32>(mulResult))), policy.equalToZero(policy.template extract<7, 32>(mulResult))));
            policy.writeFlag(x86_flag_cf, carry);
            policy.writeFlag(x86_flag_of, carry);
            break;
          }
          case 4: {
            Word(32) op0 = operands.size() == 1 ? policy.readGPR(x86_gpr_ax) : read<32>(operands[operands.size() - 2]);
            Word(32) op1 = read<32>(operands[operands.size() - 1]);
            Word(64) mulResult = policy.signedMultiply(op0, op1);
            if (operands.size() == 1) {
              policy.writeGPR(x86_gpr_ax, policy.template extract<0, 32>(mulResult));
              policy.writeGPR(x86_gpr_dx, policy.template extract<32, 64>(mulResult));
            } else {
              write32(operands[0], policy.template extract<0, 32>(mulResult));
            }
            Word(1) carry = policy.invert(policy.or_(policy.equalToZero(policy.invert(policy.template extract<7, 64>(mulResult))), policy.equalToZero(policy.template extract<7, 64>(mulResult))));
            policy.writeFlag(x86_flag_cf, carry);
            policy.writeFlag(x86_flag_of, carry);
            break;
          }
          default: ROSE_ASSERT (!"Bad size");
        }
        policy.writeFlag(x86_flag_sf, policy.false_()); // Undefined
        policy.writeFlag(x86_flag_zf, policy.false_()); // Undefined
        policy.writeFlag(x86_flag_af, policy.false_()); // Undefined
        policy.writeFlag(x86_flag_pf, policy.false_()); // Undefined
        break;
      }
      case x86_mul: {
        switch (numBytesInAsmType(operands[0]->get_type())) {
          case 1: {
            Word(8) op0 = policy.template extract<0, 8>(policy.readGPR(x86_gpr_ax));
            Word(8) op1 = read<8>(operands[0]);
            Word(16) mulResult = policy.unsignedMultiply(op0, op1);
            policy.writeGPR(x86_gpr_ax, policy.concat(mulResult, policy.template extract<16, 32>(policy.readGPR(x86_gpr_ax))));
            Word(1) carry = policy.invert(policy.equalToZero(policy.template extract<8, 16>(mulResult)));
            policy.writeFlag(x86_flag_cf, carry);
            policy.writeFlag(x86_flag_of, carry);
            break;
          }
          case 2: {
            Word(16) op0 = policy.template extract<0, 16>(policy.readGPR(x86_gpr_ax));
            Word(16) op1 = read<16>(operands[0]);
            Word(32) mulResult = policy.unsignedMultiply(op0, op1);
            policy.writeGPR(x86_gpr_ax, policy.concat(policy.template extract<0, 16>(mulResult), policy.template extract<16, 32>(policy.readGPR(x86_gpr_ax))));
            policy.writeGPR(x86_gpr_dx, policy.concat(policy.template extract<16, 32>(mulResult), policy.template extract<16, 32>(policy.readGPR(x86_gpr_dx))));
            Word(1) carry = policy.invert(policy.equalToZero(policy.template extract<16, 32>(mulResult)));
            policy.writeFlag(x86_flag_cf, carry);
            policy.writeFlag(x86_flag_of, carry);
            break;
          }
          case 4: {
            Word(32) op0 = policy.readGPR(x86_gpr_ax);
            Word(32) op1 = read<32>(operands[0]);
            Word(64) mulResult = policy.unsignedMultiply(op0, op1);
            policy.writeGPR(x86_gpr_ax, policy.template extract<0, 32>(mulResult));
            policy.writeGPR(x86_gpr_dx, policy.template extract<32, 64>(mulResult));
            Word(1) carry = policy.invert(policy.equalToZero(policy.template extract<32, 64>(mulResult)));
            policy.writeFlag(x86_flag_cf, carry);
            policy.writeFlag(x86_flag_of, carry);
            break;
          }
          default: ROSE_ASSERT (!"Bad size");
        }
        policy.writeFlag(x86_flag_sf, policy.false_()); // Undefined
        policy.writeFlag(x86_flag_zf, policy.false_()); // Undefined
        policy.writeFlag(x86_flag_af, policy.false_()); // Undefined
        policy.writeFlag(x86_flag_pf, policy.false_()); // Undefined
        break;
      }
      case x86_idiv: {
        switch (numBytesInAsmType(operands[0]->get_type())) {
          case 1: {
            Word(16) op0 = policy.template extract<0, 16>(policy.readGPR(x86_gpr_ax));
            Word(8) op1 = read<8>(operands[0]);
            // if op1 == 0, we should trap
            Word(16) divResult = policy.signedDivide(op0, op1);
            Word(8) modResult = policy.signedModulo(op0, op1);
            // if result overflows, we should trap
            policy.writeGPR(x86_gpr_ax, policy.concat(policy.concat(policy.template extract<0, 8>(divResult), modResult), policy.template extract<16, 32>(policy.readGPR(x86_gpr_ax))));
            break;
          }
          case 2: {
            Word(32) op0 = policy.concat(policy.template extract<0, 16>(policy.readGPR(x86_gpr_ax)), policy.template extract<0, 16>(policy.readGPR(x86_gpr_dx)));
            Word(16) op1 = read<16>(operands[0]);
            // if op1 == 0, we should trap
            Word(32) divResult = policy.signedDivide(op0, op1);
            Word(16) modResult = policy.signedModulo(op0, op1);
            // if result overflows, we should trap
            policy.writeGPR(x86_gpr_ax, policy.concat(policy.template extract<0, 16>(divResult), policy.template extract<16, 32>(policy.readGPR(x86_gpr_ax))));
            policy.writeGPR(x86_gpr_dx, policy.concat(modResult, policy.template extract<16, 32>(policy.readGPR(x86_gpr_dx))));
            break;
          }
          case 4: {
            Word(64) op0 = policy.concat(policy.readGPR(x86_gpr_ax), policy.readGPR(x86_gpr_dx));
            Word(32) op1 = read<32>(operands[0]);
            // if op1 == 0, we should trap
            Word(64) divResult = policy.signedDivide(op0, op1);
            Word(32) modResult = policy.signedModulo(op0, op1);
            // if result overflows, we should trap
            policy.writeGPR(x86_gpr_ax, policy.template extract<0, 32>(divResult));
            policy.writeGPR(x86_gpr_dx, modResult);
            break;
          }
          default: ROSE_ASSERT (!"Bad size");
        }
        policy.writeFlag(x86_flag_sf, policy.false_()); // Undefined
        policy.writeFlag(x86_flag_zf, policy.false_()); // Undefined
        policy.writeFlag(x86_flag_af, policy.false_()); // Undefined
        policy.writeFlag(x86_flag_pf, policy.false_()); // Undefined
        policy.writeFlag(x86_flag_cf, policy.false_()); // Undefined
        policy.writeFlag(x86_flag_of, policy.false_()); // Undefined
        break;
      }
      case x86_div: {
        switch (numBytesInAsmType(operands[0]->get_type())) {
          case 1: {
            Word(16) op0 = policy.template extract<0, 16>(policy.readGPR(x86_gpr_ax));
            Word(8) op1 = read<8>(operands[0]);
            // if op1 == 0, we should trap
            Word(16) divResult = policy.unsignedDivide(op0, op1);
            Word(8) modResult = policy.unsignedModulo(op0, op1);
            // if policy.template extract<8, 16> of divResult is non-zero (overflow), we should trap
            policy.writeGPR(x86_gpr_ax, policy.concat(policy.concat(policy.template extract<0, 8>(divResult), modResult), policy.template extract<16, 32>(policy.readGPR(x86_gpr_ax))));
            break;
          }
          case 2: {
            Word(32) op0 = policy.concat(policy.template extract<0, 16>(policy.readGPR(x86_gpr_ax)), policy.template extract<0, 16>(policy.readGPR(x86_gpr_dx)));
            Word(16) op1 = read<16>(operands[0]);
            // if op1 == 0, we should trap
            Word(32) divResult = policy.unsignedDivide(op0, op1);
            Word(16) modResult = policy.unsignedModulo(op0, op1);
            // if policy.template extract<16, 32> of divResult is non-zero (overflow), we should trap
            policy.writeGPR(x86_gpr_ax, policy.concat(policy.template extract<0, 16>(divResult), policy.template extract<16, 32>(policy.readGPR(x86_gpr_ax))));
            policy.writeGPR(x86_gpr_dx, policy.concat(modResult, policy.template extract<16, 32>(policy.readGPR(x86_gpr_dx))));
            break;
          }
          case 4: {
            Word(64) op0 = policy.concat(policy.readGPR(x86_gpr_ax), policy.readGPR(x86_gpr_dx));
            Word(32) op1 = read<32>(operands[0]);
            // if op1 == 0, we should trap
            Word(64) divResult = policy.unsignedDivide(op0, op1);
            Word(32) modResult = policy.unsignedModulo(op0, op1);
            // if policy.template extract<32, 64> of divResult is non-zero (overflow), we should trap
            policy.writeGPR(x86_gpr_ax, policy.template extract<0, 32>(divResult));
            policy.writeGPR(x86_gpr_dx, modResult);
            break;
          }
          default: ROSE_ASSERT (!"Bad size");
        }
        policy.writeFlag(x86_flag_sf, policy.false_()); // Undefined
        policy.writeFlag(x86_flag_zf, policy.false_()); // Undefined
        policy.writeFlag(x86_flag_af, policy.false_()); // Undefined
        policy.writeFlag(x86_flag_pf, policy.false_()); // Undefined
        policy.writeFlag(x86_flag_cf, policy.false_()); // Undefined
        policy.writeFlag(x86_flag_of, policy.false_()); // Undefined
        break;
      }

      case x86_aaa: {
        ROSE_ASSERT (operands.size() == 0);
        Word(1) incAh = policy.or_(policy.readFlag(x86_flag_af),
                                   greaterOrEqual<4>(policy.template extract<0, 4>(policy.readGPR(x86_gpr_ax)), 10));
        policy.writeGPR(x86_gpr_ax,
          policy.concat(
            policy.add(policy.ite(incAh, policy.template number<4>(6), policy.template number<4>(0)),
                       policy.template extract<0, 4>(policy.readGPR(x86_gpr_ax))),
            policy.concat(
              policy.template number<4>(0),
              policy.concat(
                policy.add(policy.ite(incAh, policy.template number<8>(1), policy.template number<8>(0)),
                           policy.template extract<8, 16>(policy.readGPR(x86_gpr_ax))),
                policy.template extract<16, 32>(policy.readGPR(x86_gpr_ax))))));
        policy.writeFlag(x86_flag_of, policy.false_()); // Undefined
        policy.writeFlag(x86_flag_sf, policy.false_()); // Undefined
        policy.writeFlag(x86_flag_zf, policy.false_()); // Undefined
        policy.writeFlag(x86_flag_pf, policy.false_()); // Undefined
        policy.writeFlag(x86_flag_af, incAh);
        policy.writeFlag(x86_flag_cf, incAh);
        break;
      }

      case x86_aas: {
        ROSE_ASSERT (operands.size() == 0);
        Word(1) decAh = policy.or_(policy.readFlag(x86_flag_af),
                                   greaterOrEqual<4>(policy.template extract<0, 4>(policy.readGPR(x86_gpr_ax)), 10));
        policy.writeGPR(x86_gpr_ax,
          policy.concat(
            policy.add(policy.ite(decAh, policy.template number<4>(-6), policy.template number<4>(0)),
                       policy.template extract<0, 4>(policy.readGPR(x86_gpr_ax))),
            policy.concat(
              policy.template number<4>(0),
              policy.concat(
                policy.add(policy.ite(decAh, policy.template number<8>(-1), policy.template number<8>(0)),
                           policy.template extract<8, 16>(policy.readGPR(x86_gpr_ax))),
                policy.template extract<16, 32>(policy.readGPR(x86_gpr_ax))))));
        policy.writeFlag(x86_flag_of, policy.false_()); // Undefined
        policy.writeFlag(x86_flag_sf, policy.false_()); // Undefined
        policy.writeFlag(x86_flag_zf, policy.false_()); // Undefined
        policy.writeFlag(x86_flag_pf, policy.false_()); // Undefined
        policy.writeFlag(x86_flag_af, decAh);
        policy.writeFlag(x86_flag_cf, decAh);
        break;
      }

      case x86_aam: {
        ROSE_ASSERT (operands.size() == 1);
        Word(8) al = policy.template extract<0, 8>(policy.readGPR(x86_gpr_ax));
        Word(8) divisor = read<8>(operands[0]);
        Word(8) newAh = policy.unsignedDivide(al, divisor);
        Word(8) newAl = policy.unsignedModulo(al, divisor);
        policy.writeGPR(x86_gpr_ax, policy.concat(newAl, policy.concat(newAh, policy.template extract<16, 32>(policy.readGPR(x86_gpr_ax)))));
        policy.writeFlag(x86_flag_of, policy.false_()); // Undefined
        policy.writeFlag(x86_flag_af, policy.false_()); // Undefined
        policy.writeFlag(x86_flag_cf, policy.false_()); // Undefined
        setFlagsForResult<8>(newAl);
        break;
      }

      case x86_aad: {
        ROSE_ASSERT (operands.size() == 1);
        Word(8) al = policy.template extract<0, 8>(policy.readGPR(x86_gpr_ax));
        Word(8) ah = policy.template extract<8, 16>(policy.readGPR(x86_gpr_ax));
        Word(8) divisor = read<8>(operands[0]);
        Word(8) newAl = policy.add(al, policy.template extract<0, 8>(policy.unsignedMultiply(ah, divisor)));
        policy.writeGPR(x86_gpr_ax, policy.concat(newAl, policy.concat(policy.template number<8>(0), policy.template extract<16, 32>(policy.readGPR(x86_gpr_ax)))));
        policy.writeFlag(x86_flag_of, policy.false_()); // Undefined
        policy.writeFlag(x86_flag_af, policy.false_()); // Undefined
        policy.writeFlag(x86_flag_cf, policy.false_()); // Undefined
        setFlagsForResult<8>(newAl);
        break;
      }

      case x86_bswap: {
        ROSE_ASSERT (operands.size() == 1);
        Word(32) oldVal = read<32>(operands[0]);
        Word(32) newVal = policy.concat(policy.template extract<24, 32>(oldVal), policy.concat(policy.template extract<16, 24>(oldVal), policy.concat(policy.template extract<8, 16>(oldVal), policy.template extract<0, 8>(oldVal))));
        write32(operands[0], newVal);
        break;
      }

      case x86_push: {
        ROSE_ASSERT (operands.size() == 1);
        ROSE_ASSERT (insn->get_addressSize() == x86_insnsize_32);
        ROSE_ASSERT (insn->get_operandSize() == x86_insnsize_32);
        Word(32) oldSp = policy.readGPR(x86_gpr_sp);
        Word(32) newSp = policy.add(oldSp, policy.template number<32>(-4));
        writeMemory<32>(x86_segreg_ss, newSp, read<32>(operands[0]));
        policy.writeGPR(x86_gpr_sp, newSp);
        break;
      }
      case x86_pop: {
        ROSE_ASSERT (operands.size() == 1);
        ROSE_ASSERT (insn->get_addressSize() == x86_insnsize_32);
        ROSE_ASSERT (insn->get_operandSize() == x86_insnsize_32);
        Word(32) oldSp = policy.readGPR(x86_gpr_sp);
        Word(32) newSp = policy.add(oldSp, policy.template number<32>(4));
        write32(operands[0], readMemory<32>(x86_segreg_ss, oldSp));
        policy.writeGPR(x86_gpr_sp, newSp);
        break;
      }
      case x86_leave: {
        ROSE_ASSERT (operands.size() == 0);
        policy.writeGPR(x86_gpr_sp, policy.readGPR(x86_gpr_bp));
        ROSE_ASSERT (insn->get_addressSize() == x86_insnsize_32);
        ROSE_ASSERT (insn->get_operandSize() == x86_insnsize_32);
        Word(32) oldSp = policy.readGPR(x86_gpr_sp);
        Word(32) newSp = policy.add(oldSp, policy.template number<32>(4));
        policy.writeGPR(x86_gpr_bp, readMemory<32>(x86_segreg_ss, oldSp));
        policy.writeGPR(x86_gpr_sp, newSp);
        break;
      }
      case x86_call: {
        ROSE_ASSERT (operands.size() == 1);
        ROSE_ASSERT (insn->get_addressSize() == x86_insnsize_32);
        ROSE_ASSERT (insn->get_operandSize() == x86_insnsize_32);
        Word(32) oldSp = policy.readGPR(x86_gpr_sp);
        Word(32) newSp = policy.add(oldSp, policy.template number<32>(-4));
        writeMemory<32>(x86_segreg_ss, newSp, policy.readIP());
        policy.writeIP(read<32>(operands[0]));
        policy.writeGPR(x86_gpr_sp, newSp);
        break;
      }
      case x86_ret: {
        ROSE_ASSERT (operands.size() <= 1);
        ROSE_ASSERT (insn->get_addressSize() == x86_insnsize_32);
        ROSE_ASSERT (insn->get_operandSize() == x86_insnsize_32);
        Word(32) extraBytes = (operands.size() == 1 ? read<32>(operands[0]) : policy.template number<32>(0));
        Word(32) oldSp = policy.readGPR(x86_gpr_sp);
        Word(32) newSp = policy.add(oldSp, policy.add(policy.template number<32>(4), extraBytes));
        policy.writeIP(readMemory<32>(x86_segreg_ss, oldSp));
        policy.writeGPR(x86_gpr_sp, newSp);
        break;
      }
      case x86_jmp: {
        ROSE_ASSERT (operands.size() == 1);
        policy.writeIP(read<32>(operands[0]));
        break;
      }
#define FLAGCOMBO_ne policy.invert(policy.readFlag(x86_flag_zf))
#define FLAGCOMBO_e policy.readFlag(x86_flag_zf)
#define FLAGCOMBO_no policy.invert(policy.readFlag(x86_flag_of))
#define FLAGCOMBO_o policy.readFlag(x86_flag_of)
#define FLAGCOMBO_ns policy.invert(policy.readFlag(x86_flag_sf))
#define FLAGCOMBO_s policy.readFlag(x86_flag_sf)
#define FLAGCOMBO_po policy.invert(policy.readFlag(x86_flag_pf))
#define FLAGCOMBO_pe policy.readFlag(x86_flag_pf)
#define FLAGCOMBO_ae policy.invert(policy.readFlag(x86_flag_cf))
#define FLAGCOMBO_b policy.readFlag(x86_flag_cf)
#define FLAGCOMBO_be policy.or_(FLAGCOMBO_b, FLAGCOMBO_e)
#define FLAGCOMBO_a policy.and_(FLAGCOMBO_ae, FLAGCOMBO_ne)
#define FLAGCOMBO_l policy.xor_(policy.readFlag(x86_flag_sf), policy.readFlag(x86_flag_of))
#define FLAGCOMBO_ge policy.invert(policy.xor_(policy.readFlag(x86_flag_sf), policy.readFlag(x86_flag_of)))
#define FLAGCOMBO_le policy.or_(FLAGCOMBO_e, FLAGCOMBO_l)
#define FLAGCOMBO_g policy.and_(FLAGCOMBO_ge, FLAGCOMBO_ne)
#define FLAGCOMBO_cxz policy.equalToZero(policy.template extract<0, 16>(policy.readGPR(x86_gpr_cx)))
#define FLAGCOMBO_ecxz policy.equalToZero(policy.readGPR(x86_gpr_cx))
#define JUMP(tag) case x86_j##tag: {ROSE_ASSERT(operands.size() == 1); policy.writeIP(policy.ite(FLAGCOMBO_##tag, read<32>(operands[0]), policy.readIP())); break;}
      JUMP(ne)
      JUMP(e)
      JUMP(no)
      JUMP(o)
      JUMP(po)
      JUMP(pe)
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
#define SET(tag) case x86_set##tag: {ROSE_ASSERT (operands.size() == 1); write8(operands[0], policy.concat(FLAGCOMBO_##tag, policy.template number<7>(0))); break;}
      SET(ne)
      SET(e)
      SET(no)
      SET(o)
      SET(po)
      SET(pe)
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
#define CMOV(tag) case x86_cmov##tag: { \
                    ROSE_ASSERT (operands.size() == 2); \
                    switch (numBytesInAsmType(operands[0]->get_type())) { \
                      case 2: write16(operands[0], policy.ite(FLAGCOMBO_##tag, read<16>(operands[1]), read<16>(operands[0]))); break; \
                      case 4: write32(operands[0], policy.ite(FLAGCOMBO_##tag, read<32>(operands[1]), read<32>(operands[0]))); break; \
                      default: ROSE_ASSERT ("Bad size"); break; \
                    } \
                    break; \
                  }
      CMOV(ne)
      CMOV(e)
      CMOV(no)
      CMOV(o)
      CMOV(po)
      CMOV(pe)
      CMOV(ns)
      CMOV(s)
      CMOV(ae)
      CMOV(b)
      CMOV(be)
      CMOV(a)
      CMOV(le)
      CMOV(g)
      CMOV(ge)
      CMOV(l)
#undef CMOV
#undef FLAGCOMBO_ne
#undef FLAGCOMBO_e
#undef FLAGCOMBO_ns
#undef FLAGCOMBO_s
#undef FLAGCOMBO_ae
#undef FLAGCOMBO_b
#undef FLAGCOMBO_be
#undef FLAGCOMBO_a
#undef FLAGCOMBO_l
#undef FLAGCOMBO_ge
#undef FLAGCOMBO_le
#undef FLAGCOMBO_g
#undef FLAGCOMBO_cxz
#undef FLAGCOMBO_ecxz
      case x86_cld: {
        ROSE_ASSERT (operands.size() == 0);
        policy.writeFlag(x86_flag_df, policy.false_());
        break;
      }
      case x86_std: {
        ROSE_ASSERT (operands.size() == 0);
        policy.writeFlag(x86_flag_df, policy.true_());
        break;
      }
      case x86_nop: break;
      case x86_repne_scasb: {
        ROSE_ASSERT (operands.size() == 0);
        ROSE_ASSERT (insn->get_addressSize() == x86_insnsize_32);
        Word(1) ecxNotZero = policy.invert(policy.equalToZero(policy.readGPR(x86_gpr_cx)));
        doAddOperation<8>(policy.template extract<0, 8>(policy.readGPR(x86_gpr_ax)), policy.invert(readMemory<8>(x86_segreg_es, policy.readGPR(x86_gpr_di))), true, policy.false_(), ecxNotZero);
        policy.writeGPR(x86_gpr_di, policy.add(policy.readGPR(x86_gpr_di), policy.ite(ecxNotZero, policy.ite(policy.readFlag(x86_flag_df), policy.template number<32>(-1), policy.template number<32>(1)), policy.template number<32>(0))));
        policy.writeIP(policy.ite(policy.and_(ecxNotZero, policy.invert(policy.readFlag(x86_flag_zf))), // If true, repeat this instruction, otherwise go to the next one
                                 policy.readIP(),
                                 policy.template number<32>((uint32_t)(insn->get_address()))));
        break;
      }
      case x86_repe_cmpsb: {
        ROSE_ASSERT (operands.size() == 0);
        ROSE_ASSERT (insn->get_addressSize() == x86_insnsize_32);
        Word(1) ecxNotZero = policy.invert(policy.equalToZero(policy.readGPR(x86_gpr_cx)));
        doAddOperation<8>(readMemory<8>(x86_segreg_ds, policy.readGPR(x86_gpr_si)), policy.invert(readMemory<8>(x86_segreg_es, policy.readGPR(x86_gpr_di))), true, policy.false_(), ecxNotZero);
        policy.writeGPR(x86_gpr_si, policy.add(policy.readGPR(x86_gpr_si), policy.ite(ecxNotZero, policy.ite(policy.readFlag(x86_flag_df), policy.template number<32>(-1), policy.template number<32>(1)), policy.template number<32>(0))));
        policy.writeGPR(x86_gpr_di, policy.add(policy.readGPR(x86_gpr_di), policy.ite(ecxNotZero, policy.ite(policy.readFlag(x86_flag_df), policy.template number<32>(-1), policy.template number<32>(1)), policy.template number<32>(0))));
        policy.readIP() = policy.ite(policy.and_(ecxNotZero, policy.readFlag(x86_flag_zf)), // If true, repeat this instruction, otherwise go to the next one
                                 policy.template number<32>((uint32_t)(insn->get_address())),
                                 policy.readIP());
        break;
      }
      case x86_rep_movsb: {
        ROSE_ASSERT (operands.size() == 0);
        ROSE_ASSERT (insn->get_addressSize() == x86_insnsize_32);
        Word(1) ecxNotZero = policy.invert(policy.equalToZero(policy.readGPR(x86_gpr_cx)));
        writeMemory<8>(x86_segreg_es, policy.readGPR(x86_gpr_di), readMemory<8>(insn->get_segmentOverride(), policy.readGPR(x86_gpr_si)));
        policy.writeGPR(x86_gpr_si, policy.add(policy.readGPR(x86_gpr_si), policy.ite(ecxNotZero, policy.ite(policy.readFlag(x86_flag_df), policy.template number<32>(-1), policy.template number<32>(1)), policy.template number<32>(0))));
        policy.writeGPR(x86_gpr_di, policy.add(policy.readGPR(x86_gpr_di), policy.ite(ecxNotZero, policy.ite(policy.readFlag(x86_flag_df), policy.template number<32>(-1), policy.template number<32>(1)), policy.template number<32>(0))));
        policy.readIP() = policy.ite(ecxNotZero, // If true, repeat this instruction, otherwise go to the next one
                                     policy.template number<32>((uint32_t)(insn->get_address())),
                                     policy.readIP());
        break;
      }
      case x86_rep_movsd: {
        ROSE_ASSERT (operands.size() == 0);
        ROSE_ASSERT (insn->get_addressSize() == x86_insnsize_32);
        Word(1) ecxNotZero = policy.invert(policy.equalToZero(policy.readGPR(x86_gpr_cx)));
        writeMemory<32>(x86_segreg_es, policy.readGPR(x86_gpr_di), readMemory<32>(insn->get_segmentOverride(), policy.readGPR(x86_gpr_si)));
        policy.writeGPR(x86_gpr_si, policy.add(policy.readGPR(x86_gpr_si), policy.ite(ecxNotZero, policy.ite(policy.readFlag(x86_flag_df), policy.template number<32>(-4), policy.template number<32>(4)), policy.template number<32>(0))));
        policy.writeGPR(x86_gpr_di, policy.add(policy.readGPR(x86_gpr_di), policy.ite(ecxNotZero, policy.ite(policy.readFlag(x86_flag_df), policy.template number<32>(-4), policy.template number<32>(4)), policy.template number<32>(0))));
        policy.readIP() = policy.ite(ecxNotZero, // If true, repeat this instruction, otherwise go to the next one
                                     policy.template number<32>((uint32_t)(insn->get_address())),
                                     policy.readIP());
        break;
      }
      case x86_rep_stosb: {
        ROSE_ASSERT (operands.size() == 0);
        ROSE_ASSERT (insn->get_addressSize() == x86_insnsize_32);
        Word(1) ecxNotZero = policy.invert(policy.equalToZero(policy.readGPR(x86_gpr_cx)));
        writeMemory<8>(x86_segreg_es, policy.readGPR(x86_gpr_di), policy.template extract<0, 8>(policy.readGPR(x86_gpr_ax)));
        policy.writeGPR(x86_gpr_di, policy.add(policy.readGPR(x86_gpr_di), policy.ite(ecxNotZero, policy.ite(policy.readFlag(x86_flag_df), policy.template number<32>(-1), policy.template number<32>(1)), policy.template number<32>(0))));
        policy.readIP() = policy.ite(ecxNotZero, // If true, repeat this instruction, otherwise go to the next one
                                     policy.template number<32>((uint32_t)(insn->get_address())),
                                     policy.readIP());
        break;
      }
      case x86_rep_stosd: {
        ROSE_ASSERT (operands.size() == 0);
        ROSE_ASSERT (insn->get_addressSize() == x86_insnsize_32);
        Word(1) ecxNotZero = policy.invert(policy.equalToZero(policy.readGPR(x86_gpr_cx)));
        writeMemory<32>(x86_segreg_es, policy.readGPR(x86_gpr_di), policy.readGPR(x86_gpr_ax));
        policy.writeGPR(x86_gpr_di, policy.add(policy.readGPR(x86_gpr_di), policy.ite(ecxNotZero, policy.ite(policy.readFlag(x86_flag_df), policy.template number<32>(-4), policy.template number<32>(4)), policy.template number<32>(0))));
        policy.readIP() = policy.ite(ecxNotZero, // If true, repeat this instruction, otherwise go to the next one
                                     policy.template number<32>((uint32_t)(insn->get_address())),
                                     policy.readIP());
        break;
      }
      case x86_lodsd: {
        policy.writeGPR(x86_gpr_ax, readMemory<32>(x86_segreg_ds, policy.readGPR(x86_gpr_si)));
        policy.writeGPR(x86_gpr_si, policy.add(policy.readGPR(x86_gpr_si), policy.ite(policy.readFlag(x86_flag_df), policy.template number<32>(-4), policy.template number<32>(4))));
        break;
      }
      case x86_lodsw: {
        policy.writeGPR(x86_gpr_ax, policy.concat(readMemory<16>(x86_segreg_ds, policy.readGPR(x86_gpr_si)), policy.template extract<16, 32>(policy.readGPR(x86_gpr_ax))));
        policy.writeGPR(x86_gpr_si, policy.add(policy.readGPR(x86_gpr_si), policy.ite(policy.readFlag(x86_flag_df), policy.template number<32>(-2), policy.template number<32>(2))));
        break;
      }
      case x86_lodsb: {
        policy.writeGPR(x86_gpr_ax, policy.concat(policy.template readMemory<8>(x86_segreg_ds, policy.readGPR(x86_gpr_si)), policy.template extract<8, 32>(policy.readGPR(x86_gpr_ax))));
        policy.writeGPR(x86_gpr_si, policy.add(policy.readGPR(x86_gpr_si), policy.ite(policy.readFlag(x86_flag_df), policy.template number<32>(-1), policy.template number<32>(1))));
        break;
      }
      case x86_hlt: {
        ROSE_ASSERT (operands.size() == 0);
        policy.hlt();
        break;
      }
      case x86_rdtsc: {
        ROSE_ASSERT (operands.size() == 0);
        Word(64) tsc = policy.rdtsc();
        policy.writeGPR(x86_gpr_ax, policy.template extract<0, 32>(tsc));
        policy.writeGPR(x86_gpr_dx, policy.template extract<32, 64>(tsc));
        break;
      }
      case x86_int: {
        ROSE_ASSERT (operands.size() == 1);
        SgAsmByteValueExpression* bv = isSgAsmByteValueExpression(operands[0]);
        ROSE_ASSERT (bv);
        policy.interrupt(bv->get_value());
        break;
      }
      default: fprintf(stderr, "Bad instruction %s\n", toString(kind).c_str()); abort();
    }
  }

  void processInstruction(SgAsmx86Instruction* insn) {
    ROSE_ASSERT (insn);
    policy.startInstruction(insn);
    translate(insn);
    policy.finishInstruction(insn);
  }

  void processBlock(const SgAsmStatementPtrList& stmts, size_t begin, size_t end) {
    if (begin == end) return;
    policy.startBlock(stmts[begin]->get_address());
    for (size_t i = begin; i < end; ++i) {
      processInstruction(isSgAsmx86Instruction(stmts[i]));
    }
    policy.finishBlock(stmts[begin]->get_address());
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

  void processBlock(SgAsmBlock* b) {
    const SgAsmStatementPtrList& stmts = b->get_statementList();
    if (stmts.empty()) return;
    if (!isSgAsmInstruction(stmts[0])) return; // A block containing functions or something
    size_t i = 0;
    while (i < stmts.size()) {
      size_t oldI = i;
      // Advance until either i points to a repeated string op or it is just after a hlt or int
      while (i < stmts.size() && !isRepeatedStringOp(stmts[i]) && (i == oldI || !isHltOrInt(stmts[i - 1]))) ++i;
      processBlock(stmts, oldI, i);
      if (i >= stmts.size()) break;
      if (isRepeatedStringOp(stmts[i])) {
        processBlock(stmts, i, i + 1);
        ++i;
      }
      ROSE_ASSERT (i != oldI);
    }
  }

};

#endif // ROSE_X86INSTRUCTIONSEMANTICS_H
