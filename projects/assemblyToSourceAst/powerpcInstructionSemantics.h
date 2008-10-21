#ifndef ROSE_POWERPCINSTRUCTIONSEMANTICS_H
#define ROSE_POWERPCINSTRUCTIONSEMANTICS_H

#include "rose.h"
#include <stdint.h>
#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif
#include <inttypes.h>
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

static inline uint64_t shl1(unsigned int amount) { // 2**amount, safe for when amount >= 64
  if (amount >= 64) return 0;
  return 1ULL << amount;
}

template <uint64_t Amount>
struct SHL1 {
  static const uint64_t value = (Amount >= 64) ? 0 : (1ULL << Amount);
};

template <size_t> struct NumberTag {};

template <typename Policy>
struct PowerpcInstructionSemantics {
#define Word(Len) typename Policy::template wordType<(Len)>::type
  Policy& policy;

  PowerpcInstructionSemantics(Policy& policy): policy(policy) {}

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
  Word(Len) generateMaskReverse(Word(SCLen) sc) { // bit-reversal of policy.generateMask(sc)
    Word(1) allBitsSet = Len >= (SHL1<SCLen>::value) ? policy.false_() : greaterOrEqual<SCLen>(sc, Len);
    // sc2 is the number of off bits at the RHS of the number
    Word(SCLen) sc2 = policy.ite( // sc >= Len ? 0 : Len - sc
                        allBitsSet,
                        policy.template number<SCLen>(0),
                        policy.add( // Len - sc
                          policy.invert(sc),
                          policy.template number<SCLen>((Len + 1) % (SHL1<SCLen>::value))));
    Word(Len) mask = policy.ite(policy.equalToZero(sc), policy.template number<Len>(0), policy.invert(policy.template generateMask<Len>(sc2)));
    return mask;
  }
             

  template <size_t Len, size_t SCLen>
  Word(Len) rotateRight(Word(Len) w, Word(SCLen) sc) {
    // Because of RCL and RCR, this needs to work when Len is not a power of 2
    return policy.rotateLeft(w, policy.add(policy.invert(sc), policy.template number<SCLen>((1 + Len) % (SHL1<SCLen>::value))));
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
    Word(Len) result =
      policy.and_(rotateRight<Len, SCLen>(w, sc),
                  policy.invert(generateMaskReverse<Len, SCLen>(sc)));
    return result;
  }

  template <size_t Len, size_t SCLen>
  Word(Len) shiftRightArithmetic(Word(Len) w, Word(SCLen) sc) {
    BOOST_STATIC_ASSERT ((Len & (Len - 1)) == 0); // Len is power of 2
    return policy.or_(
             shiftRight<Len, SCLen>(w, sc),
             policy.ite(
               policy.template extract<Len - 1, Len>(w),
               generateMaskReverse<Len, SCLen>(sc),
               policy.template number<Len>(0)));
  }

  template <size_t Len>
  Word(1) greaterOrEqual(Word(Len) w, uint64_t n) {
    Word(Len) carries = policy.template number<Len>(0);
    policy.addWithCarries(w, policy.template number<Len>((~n) & ((SHL1<Len>::value) - 1)), policy.true_(), carries);
    return policy.template extract<Len - 1, Len>(carries);
  }

  template <size_t Len> // In bits
  Word(Len) readMemory(const Word(32)& addr, Word(1) cond) {
    return policy.template readMemory<Len>(addr, cond);
  }

  template <size_t Len>
  void writeMemory(const Word(32)& addr, const Word(Len)& data, Word(1) cond) {
    policy.template writeMemory<Len>(addr, data, cond);
  }

// DQ (10/20/2008): changed name of function from templated read version.
  Word(32) read32(SgAsmExpression* e) {
    switch (e->variantT()) {
      case V_SgAsmByteValueExpression: {
        uint64_t val = isSgAsmByteValueExpression(e)->get_value();
        return policy.template number<32>(val);
      }
      case V_SgAsmWordValueExpression: {
        uint64_t val = isSgAsmWordValueExpression(e)->get_value();
        return policy.template number<32>(val);
      }
      case V_SgAsmDoubleWordValueExpression: {
        uint64_t val = isSgAsmDoubleWordValueExpression(e)->get_value();
        return policy.template number<32>(val);
      }
      case V_SgAsmQuadWordValueExpression: {
        uint64_t val = isSgAsmQuadWordValueExpression(e)->get_value();
        return policy.template number<32>(val & 0xFFFFFFFFULL);
      }
      case V_SgAsmPowerpcRegisterReferenceExpression: {
        SgAsmPowerpcRegisterReferenceExpression* ref = isSgAsmPowerpcRegisterReferenceExpression(e);
        switch(ref->get_register_class())
           {
             case powerpc_regclass_gpr:
                {
                  Word(32) val = policy.readGPR(ref->get_register_number());
                  return val;
                }
              
             default:
                {
                  fprintf(stderr, "Bad register class %s\n", regclassToString(ref->get_register_class())); abort();
                }
           }
        
      }
      default: fprintf(stderr, "Bad variant %s in read\n", e->class_name().c_str()); abort();
    }
  }

  Word(32) readEffectiveAddress(SgAsmExpression* expr) {
    assert (isSgAsmMemoryReferenceExpression(expr));
    return read32(isSgAsmMemoryReferenceExpression(expr)->get_address());
  }

  void write8(SgAsmExpression* e, const Word(8)& value) {
    switch (e->variantT()) {
      case V_SgAsmMemoryReferenceExpression: {
        writeMemory<8>(readEffectiveAddress(e), value, policy.true_());
        break;
      }
      default: fprintf(stderr, "Bad variant %s in write8\n", e->class_name().c_str()); abort();
    }
  }

  void write16(SgAsmExpression* e, const Word(16)& value) {
    switch (e->variantT()) {
      case V_SgAsmMemoryReferenceExpression: {
        writeMemory<16>(readEffectiveAddress(e), value, policy.true_());
        break;
      }
      default: fprintf(stderr, "Bad variant %s in write16\n", e->class_name().c_str()); abort();
    }
  }

  void write32(SgAsmExpression* e, const Word(32)& value) {
    switch (e->variantT()) {
      case V_SgAsmMemoryReferenceExpression: {
        writeMemory<32>(readEffectiveAddress(e), value, policy.true_());
        break;
      }
      case V_SgAsmPowerpcRegisterReferenceExpression: {
        SgAsmPowerpcRegisterReferenceExpression* ref = isSgAsmPowerpcRegisterReferenceExpression(e);
        switch(ref->get_register_class())
           {
             case powerpc_regclass_gpr:
                {
                  policy.writeGPR(ref->get_register_number(),value);
                  break;
                }
              
             default:
                {
                  fprintf(stderr, "Bad register class %s\n", regclassToString(ref->get_register_class())); abort();
                }
           }
        
         break;
      }
      default: fprintf(stderr, "Bad variant %s in write32\n", e->class_name().c_str()); abort();
    }
  }

  void translate(SgAsmPowerpcInstruction* insn) {
    fprintf(stderr, "%s\n", unparseInstructionWithAddress(insn).c_str());
    policy.writeIP(policy.template number<32>((unsigned int)(insn->get_address() + 4)));
    PowerpcInstructionKind kind = insn->get_kind();
    const SgAsmExpressionPtrList& operands = insn->get_operandList()->get_operands();
    switch (kind) {

      case powerpc_or:
         {
           ROSE_ASSERT(operands.size() == 3);
           write32(operands[0], policy.or_(read32(operands[1]),read32(operands[2])));
           break;
         }
      case powerpc_rlwinm:
         {
           ROSE_ASSERT(operands.size() == 5);
           Word(32) RS = read32(operands[1]);
           Word(5) SH = policy.template extract<0, 5>(read32(operands[2]));

           SgAsmByteValueExpression* MB = isSgAsmByteValueExpression(operands[3]);
           ROSE_ASSERT(MB != NULL);
           int mb_value = MB->get_value();

           SgAsmByteValueExpression* ME = isSgAsmByteValueExpression(operands[4]);
           ROSE_ASSERT(ME != NULL);
           int me_value = ME->get_value();
           uint32_t mask = 0;
           if (mb_value <= me_value)
              {
             // PowerPC counts bits from the left.
                for(int i=mb_value; i <= me_value;  i++)
                     mask |= (1 << (31- i));
              }
             else
              {
                for(int i=mb_value; i <= 31;  i++)
                     mask |= (1 << (31- i));
                for(int i=0; i <= me_value; i++)
                     mask |= (1 << (31- i));
              }

           Word(32) rotatedReg = policy.rotateLeft(RS,SH);
           Word(32) bitMask = policy.template number<32>(mask);

           write32(operands[0],policy.and_(rotatedReg,bitMask));
           break;
         }

       default: fprintf(stderr, "Bad instruction %s\n", toString(kind).c_str()); //abort();
    }
  }

  void processInstruction(SgAsmPowerpcInstruction* insn) {
    ROSE_ASSERT (insn);
    policy.startInstruction(insn);
    translate(insn);
    policy.finishInstruction(insn);
  }

  void processBlock(const SgAsmStatementPtrList& stmts, size_t begin, size_t end) {
    if (begin == end) return;
    policy.startBlock(stmts[begin]->get_address());
    for (size_t i = begin; i < end; ++i) {
      processInstruction(isSgAsmPowerpcInstruction(stmts[i]));
    }
    policy.finishBlock(stmts[begin]->get_address());
  }

  void processBlock(SgAsmBlock* b) {
    const SgAsmStatementPtrList& stmts = b->get_statementList();
    if (stmts.empty()) return;
    if (!isSgAsmInstruction(stmts[0])) return; // A block containing functions or something
    processBlock(stmts, 0, stmts.size());
  }

};

#endif // ROSE_POWERPCINSTRUCTIONSEMANTICS_H
