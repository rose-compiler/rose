#ifndef ROSE_POWERPCINSTRUCTIONSEMANTICS_H
#define ROSE_POWERPCINSTRUCTIONSEMANTICS_H

#include "rose.h"
#include "semanticsModule.h"
#include <cassert>
#include <cstdio>
#include <iostream>

template <typename Policy>
struct PowerpcInstructionSemantics {
#define Word(Len) typename Policy::template wordType<(Len)>::type
  Policy& policy;

  PowerpcInstructionSemantics(Policy& policy): policy(policy) {}

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
