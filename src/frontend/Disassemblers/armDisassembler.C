#include <stdint.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <fcntl.h>
#include <vector>
#include "rose.h"

using namespace std;
using namespace SageBuilderAsm;
using namespace Exec;

namespace ArmDisassembler {

  SgAsmArmInstruction* makeInstructionWithoutOperands(uint32_t address, const std::string& mnemonic, int condPos, ArmInstructionKind kind, ArmInstructionCondition cond, uint32_t insn) {
    SgAsmArmInstruction* instruction = new SgAsmArmInstruction(address, mnemonic, "", kind, cond, condPos);
    ROSE_ASSERT (instruction);
    SgAsmOperandList* operands = new SgAsmOperandList();
    instruction->set_operandList(operands);
    operands->set_parent(instruction);
    string bytes(4, '\0');
    for (int i = 0; i < 4; ++i) {
      bytes[i] = (insn >> (8 * i)) & 0xFF; // Force little-endian
    }
    instruction->set_raw_bytes(bytes);
    return instruction;
  }

  SgAsmArmRegisterReferenceExpression* makeRegister(uint8_t reg) {
    SgAsmArmRegisterReferenceExpression* r = new SgAsmArmRegisterReferenceExpression();
    r->set_arm_register_code((SgAsmArmRegisterReferenceExpression::arm_register_enum)(reg + 1));
    return r;
  }

  SgAsmArmRegisterReferenceExpression* makePsrFields(bool useSPSR, uint8_t fields) {
    SgAsmArmRegisterReferenceExpression* r = new SgAsmArmRegisterReferenceExpression();
    r->set_arm_register_code((SgAsmArmRegisterReferenceExpression::arm_register_enum)((useSPSR ? SgAsmArmRegisterReferenceExpression::spsr_fields : SgAsmArmRegisterReferenceExpression::cpsr_fields) + fields));
    return r;
  }

  SgAsmArmRegisterReferenceExpression* makePsr(bool useSPSR) {
    SgAsmArmRegisterReferenceExpression* r = new SgAsmArmRegisterReferenceExpression();
    r->set_arm_register_code((SgAsmArmRegisterReferenceExpression::arm_register_enum)(useSPSR ? SgAsmArmRegisterReferenceExpression::spsr : SgAsmArmRegisterReferenceExpression::cpsr));
    return r;
  }

  struct SingleInstructionDisassembler {
    const Parameters& p;
    uint32_t insn;
    std::set<uint64_t>* knownSuccessorsReturn;
    ArmInstructionCondition cond;

    SingleInstructionDisassembler(const Parameters& p, uint32_t insn, std::set<uint64_t>* knownSuccessorsReturn): p(p), insn(insn), knownSuccessorsReturn(knownSuccessorsReturn), cond(arm_cond_unknown) {}

    SgAsmExpression* makeRotatedImmediate() const {
      uint8_t rsField = (insn >> 8) & 15;
      uint8_t rotateCount = rsField * 2;
      uint32_t immRaw = insn & 0xFF;
      if (rotateCount == 0) {
        return makeDWordValue(immRaw);
      } else {
        return makeDWordValue((immRaw >> rotateCount) | (immRaw << (32 - rotateCount)));
      }
    }

    SgAsmExpression* makeShifterField() const { // Decode last 12 bits and bit 25 (I)
      bool i = (insn >> 25) & 1;
      uint8_t rsField = (insn >> 8) & 15;
      uint8_t rmField = insn & 15;
      uint8_t shiftCount = (insn >> 7) & 31;
      uint8_t shiftCountOr32 = shiftCount == 0 ? 32 : shiftCount;
      if (i) {
        return makeRotatedImmediate();
      } else if ((insn & 0xFF0) == 0) {
        return makeRegister(rmField);
      } else if ((insn & 0x070) == 0) {
        return makeLsl(makeRegister(rmField), makeByteValue(shiftCount));
      } else if ((insn & 0x0F0) == 0x010) {
        return makeLsl(makeRegister(rmField), makeRegister(rsField));
      } else if ((insn & 0x070) == 0x020) {
        return makeLsr(makeRegister(rmField), makeByteValue(shiftCountOr32));
      } else if ((insn & 0x0F0) == 0x030) {
        return makeLsr(makeRegister(rmField), makeRegister(rsField));
      } else if ((insn & 0x070) == 0x040) {
        return makeAsr(makeRegister(rmField), makeByteValue(shiftCountOr32));
      } else if ((insn & 0x0F0) == 0x050) {
        return makeAsr(makeRegister(rmField), makeRegister(rsField));
      } else if ((insn & 0xFF0) == 0x060) {
        return makeRrx(makeRegister(rmField));
      } else if ((insn & 0x070) == 0x060) {
        return makeRor(makeRegister(rmField), makeByteValue(shiftCount));
      } else if ((insn & 0x0F0) == 0x070) {
        return makeRor(makeRegister(rmField), makeRegister(rsField));
      } else throw BadInstruction();
    }

#define MAKE_INSN0(Mne, CondPos) (makeInstructionWithoutOperands(p.ip, #Mne, (CondPos), arm_##Mne, cond, insn))
#define MAKE_INSN1(Mne, CondPos, Op1) (appendOperand(MAKE_INSN0(Mne, CondPos), (Op1)))
#define MAKE_INSN2(Mne, CondPos, Op1, Op2) (appendOperand(MAKE_INSN1(Mne, CondPos, Op1), (Op2)))
#define MAKE_INSN3(Mne, CondPos, Op1, Op2, Op3) (appendOperand(MAKE_INSN2(Mne, CondPos, Op1, Op2), (Op3)))
#define MAKE_INSN4(Mne, CondPos, Op1, Op2, Op3, Op4) (appendOperand(MAKE_INSN3(Mne, CondPos, Op1, Op2, Op3), (Op4)))

    SgAsmArmInstruction* makeDataProcInstruction(uint8_t opcode, bool s, SgAsmExpression* rn, SgAsmExpression* rd, SgAsmExpression* rhsOperand) {
      switch ((s ? 16 : 0) | opcode) {
        case 0x00: return MAKE_INSN3(and, 3, rd, rn, rhsOperand);
        case 0x01: return MAKE_INSN3(eor, 3, rd, rn, rhsOperand);
        case 0x02: return MAKE_INSN3(sub, 3, rd, rn, rhsOperand);
        case 0x03: return MAKE_INSN3(rsb, 3, rd, rn, rhsOperand);
        case 0x04: return MAKE_INSN3(add, 3, rd, rn, rhsOperand);
        case 0x05: return MAKE_INSN3(adc, 3, rd, rn, rhsOperand);
        case 0x06: return MAKE_INSN3(sbc, 3, rd, rn, rhsOperand);
        case 0x07: return MAKE_INSN3(rsc, 3, rd, rn, rhsOperand);
        case 0x08: ROSE_ASSERT (!"Not a data processing insn");
        case 0x09: ROSE_ASSERT (!"Not a data processing insn");
        case 0x0A: ROSE_ASSERT (!"Not a data processing insn");
        case 0x0B: ROSE_ASSERT (!"Not a data processing insn");
        case 0x0C: return MAKE_INSN3(orr, 3, rd, rn, rhsOperand);
        case 0x0D: return MAKE_INSN2(mov, 3, rd, rhsOperand);
        case 0x0E: return MAKE_INSN3(bic, 3, rd, rn, rhsOperand);
        case 0x0F: return MAKE_INSN2(mvn, 3, rd, rhsOperand);
        case 0x10: return MAKE_INSN3(ands, 3, rd, rn, rhsOperand);
        case 0x11: return MAKE_INSN3(eors, 3, rd, rn, rhsOperand);
        case 0x12: return MAKE_INSN3(subs, 3, rd, rn, rhsOperand);
        case 0x13: return MAKE_INSN3(rsbs, 3, rd, rn, rhsOperand);
        case 0x14: return MAKE_INSN3(adds, 3, rd, rn, rhsOperand);
        case 0x15: return MAKE_INSN3(adcs, 3, rd, rn, rhsOperand);
        case 0x16: return MAKE_INSN3(sbcs, 3, rd, rn, rhsOperand);
        case 0x17: return MAKE_INSN3(rscs, 3, rd, rn, rhsOperand);
        case 0x18: return MAKE_INSN2(tst, 3, rn, rhsOperand);
        case 0x19: return MAKE_INSN2(teq, 3, rn, rhsOperand);
        case 0x1A: return MAKE_INSN2(cmp, 3, rn, rhsOperand);
        case 0x1B: return MAKE_INSN2(cmn, 3, rn, rhsOperand);
        case 0x1C: return MAKE_INSN3(orrs, 3, rd, rn, rhsOperand);
        case 0x1D: return MAKE_INSN2(movs, 3, rd, rhsOperand);
        case 0x1E: return MAKE_INSN3(bics, 3, rd, rn, rhsOperand);
        case 0x1F: return MAKE_INSN2(mvns, 3, rd, rhsOperand);
        default: ROSE_ASSERT (false);
      }
    }

    SgAsmDoubleWordValueExpression* makeSplit8bitOffset() const {
      int32_t val = ((insn >> 4) & 0xF0) | (insn & 0xF);
      val <<= 24;
      val >>= 24; // Arithmetic shift to copy highest bit of immediate
      return makeDWordValue((uint32_t)val);
    }

    SgAsmDoubleWordValueExpression* makeBranchTarget() const {
      int32_t val = insn & 0xFFFFFF;
      val <<= 8;
      val >>= 6; // Arithmetic shift to copy highest bit of immediate
      uint32_t targetAddr = p.ip + 8 + val;
      if (knownSuccessorsReturn) knownSuccessorsReturn->insert(targetAddr);
      return makeDWordValue(targetAddr);
    }

    SgAsmExpression* decodeMemoryAddress(SgAsmExpression* rn) const {
      bool bit25 = (insn >> 25) & 1;
      bool p = (insn >> 24) & 1;
      bool u = (insn >> 23) & 1;
      bool w = (insn >> 21) & 1;
      SgAsmExpression* offset = bit25 ? makeShifterField() : makeDWordValue(insn & 0xFFFU);
      switch ((p ? 4 : 0) | (u ? 2 : 0) | (w ? 1 : 0)) {
        case 0: return makeSubtractPostupdate(rn, offset);
        case 1: return makeSubtractPostupdate(rn, offset); // T suffix
        case 2: return makeAddPostupdate(rn, offset);
        case 3: return makeAddPostupdate(rn, offset); // T suffix
        case 4: return makeSubtract(rn, offset);
        case 5: return makeSubtractPreupdate(rn, offset);
        case 6: return makeAdd(rn, offset);
        case 7: return makeAddPreupdate(rn, offset);
        default: ROSE_ASSERT (false);
      }
    }

    SgAsmArmInstruction* decodeMediaInstruction() const {
      cerr << "ARM media instructions not supported: " << StringUtility::intToHex(insn) << endl;
      throw BadInstruction();
    }

    SgAsmArmInstruction* decodeMultiplyInstruction() const {
      SgAsmExpression* rn = makeRegister((insn >> 16) & 15);
      SgAsmExpression* rd = makeRegister((insn >> 12) & 15);
      SgAsmExpression* rs = makeRegister((insn >> 8) & 15);
      SgAsmExpression* rm = makeRegister(insn & 15);
      switch ((insn >> 20) & 15) {
        case 0x0: return MAKE_INSN3(mul, 3, rn, rm, rs);
        case 0x1: return MAKE_INSN3(muls, 3, rn, rm, rs);
        case 0x2: return MAKE_INSN4(mla, 3, rn, rm, rs, rd);
        case 0x3: return MAKE_INSN4(mlas, 3, rn, rm, rs, rd);
        case 0x4: return MAKE_INSN4(umaal, 3, rd, rn, rm, rs);
        case 0x5: throw BadInstruction();
        case 0x6: throw BadInstruction();
        case 0x7: throw BadInstruction();
        case 0x8: return MAKE_INSN4(umull, 5, rd, rn, rm, rs);
        case 0x9: return MAKE_INSN4(umulls, 5, rd, rn, rm, rs);
        case 0xA: return MAKE_INSN4(umlal, 5, rd, rn, rm, rs);
        case 0xB: return MAKE_INSN4(umlals, 5, rd, rn, rm, rs);
        case 0xC: return MAKE_INSN4(smull, 5, rd, rn, rm, rs);
        case 0xD: return MAKE_INSN4(smulls, 5, rd, rn, rm, rs);
        case 0xE: return MAKE_INSN4(smlal, 5, rd, rn, rm, rs);
        case 0xF: return MAKE_INSN4(smlals, 5, rd, rn, rm, rs);
        default: ROSE_ASSERT (false);
      }
    }

    SgAsmArmInstruction* decodeExtraLoadStores() const {
      bool bit5 = (insn >> 5) & 1;
      bool bit6 = (insn >> 6) & 1;
      bool bit20 = (insn >> 20) & 1;
      bool bit21 = (insn >> 21) & 1;
      bool bit22 = (insn >> 22) & 1;
      bool bit23 = (insn >> 23) & 1;
      bool bit24 = (insn >> 24) & 1;
      SgAsmExpression* rn = makeRegister((insn >> 16) & 15);
      SgAsmExpression* rd = makeRegister((insn >> 12) & 15);
      SgAsmExpression* offset = bit22 ? (SgAsmExpression*)makeSplit8bitOffset() : makeRegister(insn & 15);
      SgAsmExpression* addr = NULL;
      switch ((bit24 ? 4 : 0) | (bit23 ? 2 : 0) | (bit21 ? 1 : 0)) {
        case 0: addr = makeSubtractPostupdate(rn, offset); break;
        case 1: throw BadInstruction();
        case 2: addr = makeAddPostupdate(rn, offset); break;
        case 3: throw BadInstruction();
        case 4: addr = makeSubtract(rn, offset); break;
        case 5: addr = makeSubtractPreupdate(rn, offset); break;
        case 6: addr = makeAdd(rn, offset); break;
        case 7: addr = makeAddPreupdate(rn, offset); break;
        default: ROSE_ASSERT (false);
      }
      SgAsmExpression* memref = makeMemoryReference(addr);
      uint8_t lsh = (bit20 ? 4 : 0) | (bit6 ? 2 : 0) | (bit5 ? 1 : 0);
      switch (lsh) {
        case 0: ROSE_ASSERT (false); // Should have been handled in multiply code above
        case 1: return MAKE_INSN2(strh, 3, rd, memref);
        case 2: return MAKE_INSN2(ldrd, 3, rd, memref);
        case 3: return MAKE_INSN2(strd, 3, rd, memref);
        case 4: ROSE_ASSERT (false); // Should have been handled in multiply code above
        case 5: return MAKE_INSN2(ldruh, 3, rd, memref);
        case 6: return MAKE_INSN2(ldrsb, 3, rd, memref);
        case 7: return MAKE_INSN2(ldrsh, 3, rd, memref);
        default: ROSE_ASSERT (false);
      }
    }

    SgAsmArmInstruction* decodeMiscInstruction() const {
      bool bit7 = (insn >> 7) & 1;
      bool bit21 = (insn >> 21) & 1;
      bool bit22 = (insn >> 22) & 1;
      if (!bit7) {
        switch ((insn >> 4) & 7) {
          case 0: {
            if (bit21) {
              SgAsmArmRegisterReferenceExpression* rm = makeRegister(insn & 15);
              bool useSPSR = bit22;
              uint8_t mask = (insn >> 16) & 15;
              SgAsmArmRegisterReferenceExpression* psr = makePsrFields(useSPSR, mask);
              return MAKE_INSN2(msr, 3, psr, rm);
            } else {
              bool useSPSR = bit22;
              SgAsmArmRegisterReferenceExpression* rd = makeRegister((insn >> 12) & 15);
              SgAsmArmRegisterReferenceExpression* psr = makePsr(useSPSR);
              return MAKE_INSN2(mrs, 3, rd, psr);
            }
          }
          case 1: {
            if (bit22) {
              SgAsmArmRegisterReferenceExpression* rd = makeRegister((insn >> 12) & 15);
              SgAsmArmRegisterReferenceExpression* rm = makeRegister(insn & 15);
              return MAKE_INSN2(clz, 3, rd, rm);
            } else {
              return MAKE_INSN1(bx, 2, makeRegister(insn & 15));
            }
          }
          case 2: return MAKE_INSN1(bxj, 3, makeRegister(insn & 15));
          case 3: return MAKE_INSN1(blx, 3, makeRegister(insn & 15));
          case 4: throw BadInstruction();
          case 5: {
            SgAsmArmRegisterReferenceExpression* rd = makeRegister((insn >> 12) & 15);
            SgAsmArmRegisterReferenceExpression* rn = makeRegister((insn >> 16) & 15);
            SgAsmArmRegisterReferenceExpression* rm = makeRegister(insn & 15);
            uint8_t op = (insn >> 21) & 3;
            switch (op) {
              case 0: return MAKE_INSN3(qadd, 4, rd, rm, rn);
              case 1: return MAKE_INSN3(qsub, 4, rd, rm, rn);
              case 2: return MAKE_INSN3(qdadd, 5, rd, rm, rn);
              case 3: return MAKE_INSN3(qdsub, 5, rd, rm, rn);
              default: ROSE_ASSERT (false);
            }
          }
          case 6: throw BadInstruction();
          case 7: {
            uint16_t imm1 = (insn >> 8) & 0xFFF;
            uint16_t imm2 = insn & 0xF;
            uint16_t imm = (imm1 << 4) | imm2;
            return MAKE_INSN1(bkpt, 4, makeWordValue(imm));
          }
          default: ROSE_ASSERT (false);
        }
      } else { // bit 7 set -- signed mul
        SgAsmArmRegisterReferenceExpression* rd = makeRegister((insn >> 16) & 15);
        SgAsmArmRegisterReferenceExpression* rn = makeRegister((insn >> 12) & 15);
        SgAsmArmRegisterReferenceExpression* rs = makeRegister((insn >> 8) & 15);
        SgAsmArmRegisterReferenceExpression* rm = makeRegister(insn & 15);
        uint8_t op = (insn >> 21) & 3;
        bool y = (insn >> 6) & 1;
        bool x = (insn >> 5) & 1;
        switch ((op << 2) | (x ? 2 : 0) | (y ? 1 : 0)) {
          case 0x0: return MAKE_INSN4(smlabb, 6, rd, rm, rs, rn);
          case 0x1: return MAKE_INSN4(smlabt, 6, rd, rm, rs, rn);
          case 0x2: return MAKE_INSN4(smlatb, 6, rd, rm, rs, rn);
          case 0x3: return MAKE_INSN4(smlatt, 6, rd, rm, rs, rn);
          case 0x4: return MAKE_INSN4(smlawb, 6, rd, rm, rs, rn);
          case 0x5: return MAKE_INSN4(smlawt, 6, rd, rm, rs, rn);
          case 0x6: return MAKE_INSN4(smluwb, 6, rd, rm, rs, rn);
          case 0x7: return MAKE_INSN4(smluwt, 6, rd, rm, rs, rn);
          case 0x8: return MAKE_INSN4(smlalbb, 7, rn, rd, rm, rs);
          case 0x9: return MAKE_INSN4(smlalbt, 7, rn, rd, rm, rs);
          case 0xA: return MAKE_INSN4(smlaltb, 7, rn, rd, rm, rs);
          case 0xB: return MAKE_INSN4(smlaltt, 7, rn, rd, rm, rs);
          case 0xC: return MAKE_INSN3(smulbb, 6, rd, rm, rs);
          case 0xD: return MAKE_INSN3(smulbt, 6, rd, rm, rs);
          case 0xE: return MAKE_INSN3(smultb, 6, rd, rm, rs);
          case 0xF: return MAKE_INSN3(smultt, 6, rd, rm, rs);
          default: ROSE_ASSERT (false);
        }
      }
    }

    SgAsmArmInstruction* disassemble() {
      fprintf(stderr, "Disassembling insn 0x%08" PRIx32 " at addr 0x%08" PRIx32 "\n", insn, p.ip);
      uint8_t condField = (insn >> 28) & 0xF;
      bool bit4 = (insn >> 4) & 1;
      bool bit7 = (insn >> 7) & 1;
      bool bit9 = (insn >> 9) & 1;
      bool bit16 = (insn >> 16) & 1;
      bool bit20 = (insn >> 20) & 1;
      bool bit21 = (insn >> 21) & 1;
      bool bit22 = (insn >> 22) & 1;
      bool bit23 = (insn >> 23) & 1;
      bool bit24 = (insn >> 24) & 1;
      bool bit25 = (insn >> 25) & 1;
      bool bit4_and_bit7 = bit4 && bit7;
      if (knownSuccessorsReturn && (condField != 14 || (insn & 0x0F000000U) != 0x0A000000U)) {
        knownSuccessorsReturn->insert(p.ip + 4);
      }
      if (condField != 15 || !p.decodeUnconditionalInstructions) { // Normal instructions (or arm_cond_nv instructions if they are not treated specially)
        cond = (ArmInstructionCondition)(condField + 1);
        uint8_t dataProcOpcode = (insn >> 21) & 15;
        bool dpIsSpecial = (insn & 0x01900000) == 0x01000000;
        switch ((insn >> 26) & 3) {
          case 0: { // Data processing, misc, multiplies, undefined, move imm to status reg
            if ((insn & 0x0F0000F0U) == 0x00000090U) { // Multiplies
              return decodeMultiplyInstruction();
            } else if (bit4_and_bit7 && !bit25) {
              return decodeExtraLoadStores();
            } else if (dpIsSpecial && bit25) {
              if (bit21) {
                SgAsmExpression* imm = makeRotatedImmediate();
                bool useSPSR = bit22;
                uint8_t mask = (insn >> 16) & 15;
                SgAsmArmRegisterReferenceExpression* psr = makePsrFields(useSPSR, mask);
                return MAKE_INSN2(msr, 3, psr, imm);
              } else {
                throw BadInstruction();
              }
            } else if (dpIsSpecial && !bit25) {
              return decodeMiscInstruction();
            } else { // !dpIsSpecial && !bit4_and_bit7
              SgAsmExpression* rn = makeRegister((insn >> 16) & 15);
              SgAsmExpression* rd = makeRegister((insn >> 12) & 15);
              SgAsmExpression* shifterField = makeShifterField();
              return makeDataProcInstruction(dataProcOpcode, bit20, rn, rd, shifterField);
            }
          }
          case 1: { // Load-store, media, undefined
            if (!bit4 || !bit25) {
              SgAsmExpression* rn = makeRegister((insn >> 16) & 15);
              SgAsmExpression* memref = makeMemoryReference(decodeMemoryAddress(rn));
              SgAsmExpression* rd = makeRegister((insn >> 12) & 15);
              bool isLoad = bit20;
              bool isByte = bit22;
              bool isTranslated = !bit24 && bit21;
              switch ((isTranslated ? 4 : 0) | (isLoad ? 2 : 0) | (isByte ? 1 : 0)) {
                case 0: return MAKE_INSN2(str, 3, rd, memref);
                case 1: return MAKE_INSN2(strb, 3, rd, memref);
                case 2: return MAKE_INSN2(ldr, 3, rd, memref);
                case 3: return MAKE_INSN2(ldrb, 3, rd, memref);
                case 4: return MAKE_INSN2(strt, 3, rd, memref);
                case 5: return MAKE_INSN2(strbt, 3, rd, memref);
                case 6: return MAKE_INSN2(ldrt, 3, rd, memref);
                case 7: return MAKE_INSN2(ldrbt, 3, rd, memref);
                default: ROSE_ASSERT (false);
              }
            } else if ((insn & 0x0FF000F0U) == 0x07F000F0U) {
              return MAKE_INSN0(undefined, 9);
            } else {
              return decodeMediaInstruction();
            }
          }
          case 2: { // Load-store multiple, branches
            if (!bit25) {
              SgAsmExpression* rn = makeRegister((insn >> 16) & 15);
              SgAsmExprListExp* regs = makeExprListExp();
              for (int i = 0; i < 16; ++i) {
                if ((insn >> i) & 1) {
                  SgAsmArmRegisterReferenceExpression* reg = makeRegister(i);
                  regs->get_expressions().push_back(reg);
                  reg->set_parent(regs);
                }
              }
              SgAsmExpression* base = rn;
              if (bit21) { // w
                SgAsmExpression* offset = makeByteValue(regs->get_expressions().size() * 4);
                if (bit23) { // u
                  base = makeAddPostupdate(rn, offset);
                } else {
                  base = makeSubtractPostupdate(rn, offset);
                }
              }
              switch (((insn >> 21) & 62) | bit20) { // p, u, s, l
                case 0x0: return MAKE_INSN2(stmda, 3, rn, regs);
                case 0x1: return MAKE_INSN2(ldmda, 3, rn, regs);
                case 0x2: return MAKE_INSN2(stmda, 3, rn, makeArmSpecialRegisterList(regs));
                case 0x3: return MAKE_INSN2(ldmda, 3, rn, makeArmSpecialRegisterList(regs));
                case 0x4: return MAKE_INSN2(stmia, 3, rn, regs);
                case 0x5: return MAKE_INSN2(ldmia, 3, rn, regs);
                case 0x6: return MAKE_INSN2(stmia, 3, rn, makeArmSpecialRegisterList(regs));
                case 0x7: return MAKE_INSN2(ldmia, 3, rn, makeArmSpecialRegisterList(regs));
                case 0x8: return MAKE_INSN2(stmdb, 3, rn, regs);
                case 0x9: return MAKE_INSN2(ldmdb, 3, rn, regs);
                case 0xA: return MAKE_INSN2(stmdb, 3, rn, makeArmSpecialRegisterList(regs));
                case 0xB: return MAKE_INSN2(ldmdb, 3, rn, makeArmSpecialRegisterList(regs));
                case 0xC: return MAKE_INSN2(stmib, 3, rn, regs);
                case 0xD: return MAKE_INSN2(ldmib, 3, rn, regs);
                case 0xE: return MAKE_INSN2(stmib, 3, rn, makeArmSpecialRegisterList(regs));
                case 0xF: return MAKE_INSN2(ldmib, 3, rn, makeArmSpecialRegisterList(regs));
                default: ROSE_ASSERT (false);
              }
            } else {
              SgAsmExpression* target = makeBranchTarget();
              if ((insn >> 24) & 1) {
                return MAKE_INSN1(bl, 2, target);
              } else {
                return MAKE_INSN1(b, 1, target);
              }
            }
          }
          case 3: {
            if ((insn & 0x0F000000U) == 0x0F000000U) {
              return MAKE_INSN1(swi, 3, makeDWordValue(insn & 0x00FFFFFFU));
            } else {
              cerr << "Coprocessor not supported 0x" << StringUtility::intToHex(insn) << endl;
              throw BadInstruction();
            }
          }
          default: ROSE_ASSERT (!"Can't happen");
        }
      } else { // Unconditional instructions
        cond = arm_cond_al;
        uint16_t opcode1 = (insn >> 20) & 0xFF;
        uint16_t opcode2 = (insn >> 4) & 0xF;
        switch (opcode1) {
          case 0x10: {
            if (bit16) {
              return MAKE_INSN1(setend, 6, makeByteValue(bit9));
            } else {
              ROSE_ASSERT (!"CPS not supported");
            }
          }
          default: {
            cerr << "Cannot handle too many unconditional instructions: " << StringUtility::intToHex(insn) << endl;
            throw BadInstruction();
          }
        }
      }
      ROSE_ASSERT (!"Fell off end of disassemble");
    }

  };

  SgAsmArmInstruction* disassemble(const Parameters& p, const uint8_t* const insn, const uint64_t insnSize, size_t positionInVector, set<uint64_t>* knownSuccessorsReturn) {
    // This is the way it is to deal with overflows
    if (positionInVector >= insnSize || positionInVector + 4 > insnSize) {
      abort();
    }
    uint32_t c = insn[positionInVector + 3];
    c = (c << 8) | insn[positionInVector + 2];
    c = (c << 8) | insn[positionInVector + 1];
    c = (c << 8) | insn[positionInVector];
    SingleInstructionDisassembler sid(p, c, knownSuccessorsReturn);
    return sid.disassemble();
  }


  struct AsmFileWithData {
    ExecFile* ef;
    mutable size_t instructionsDisassembled;

    AsmFileWithData(ExecFile* ef): ef(ef), instructionsDisassembled(0) {}

    ExecSection* getSectionOfAddress(uint64_t addr) const {
      const vector<ExecSection*> possibleSections = ef->get_sections_by_rva(addr);
      if (possibleSections.empty()) {
        return NULL;
      } else if (possibleSections.size() != 1) {
        cerr << "Trying to disassemble code that is in multiple sections (addr = 0x" << hex << addr << ")" << endl;
        abort();
      }
      return possibleSections[0];
    }

    bool inCodeSegment(uint64_t addr) const {
      ExecSection* sectionOfThisPtr = getSectionOfAddress(addr);
      if (sectionOfThisPtr != NULL &&
          sectionOfThisPtr->is_mapped() &&
          sectionOfThisPtr->get_executable()) {
        return true;
      }
      return false;
    }

    size_t getFileOffsetOfAddress(uint64_t addr) const {
      ExecSection* section = getSectionOfAddress(addr);
      if (!section) abort();
      ROSE_ASSERT (section->is_mapped());
      return addr - section->get_mapped_rva() + section->get_offset();
    }

    SgAsmArmInstruction* disassembleOneAtAddress(uint64_t addr, Parameters params, set<uint64_t>& knownSuccessors) const {
      params.ip = addr;
      if (!inCodeSegment(addr)) {
        return 0;
      }
      size_t fileOffset = getFileOffsetOfAddress(addr);
      ROSE_ASSERT (fileOffset % 4 == 0);
      try {
        ROSE_ASSERT (fileOffset < ef->get_size());
        SgAsmArmInstruction* insn = disassemble(params, ef->content(), ef->get_size(), fileOffset, &knownSuccessors);
        ROSE_ASSERT (insn);
        return insn;
      } catch (BadInstruction) {
        knownSuccessors.clear();
        return 0;
      } catch (OverflowOfInstructionVector) {
        return 0;
      }
    }

    // Value field of basicBlockStarts is whether the block came from a
    // parameter, or just a constant jump target or fallthrough (when true, it
    // states that an indirect jump may be pointing there)

    void disassembleRecursively(uint64_t addr, Parameters params, map<uint64_t, SgAsmInstruction*>& insns, map<uint64_t, bool>& basicBlockStarts, set<uint64_t>& functionStarts) const {
      vector<uint64_t> worklist(1, addr);
      disassembleRecursively(worklist, params, insns, basicBlockStarts, functionStarts);
    }

    void disassembleRecursively(vector<uint64_t>& worklist, Parameters params, map<uint64_t, SgAsmInstruction*>& insns, map<uint64_t, bool>& basicBlockStarts, set<uint64_t>& functionStarts) const {
      while (!worklist.empty()) {
        uint64_t addr = worklist.back();
        worklist.pop_back();
        if (insns.find(addr) != insns.end()) continue;
        ++instructionsDisassembled;
        if (instructionsDisassembled % 10000 == 0) {
          cerr << instructionsDisassembled << " disassembling " << addr << " worklist size = " << worklist.size() << ", done = " << insns.size() << endl;
        }
        set<uint64_t> knownSuccessors;
        SgAsmArmInstruction* insn = disassembleOneAtAddress(addr, params, knownSuccessors);
        if (!insn) {cerr << "Bad instruction at 0x" << hex << addr << endl; continue;}
        insns.insert(make_pair(addr, insn));
        for (set<uint64_t>::const_iterator i = knownSuccessors.begin(); i != knownSuccessors.end(); ++i) {
          if (!inCodeSegment(*i)) { /* cerr << "Found succ outside code segment at 0x" << hex << *i << endl; */ continue;} // Assume no jumps to data segments
          if (knownSuccessors.size() != 1 || *i != addr + insn->get_raw_bytes().size()) {
            basicBlockStarts[*i] |= false; // Ensure it exists, but don't change its value if it was already true
            // basicBlockStarts[*i] = true; // Be more conservative
          }
          if (insns.find(*i) == insns.end()) {
            worklist.push_back(*i);
          }
        }
        // Scan for constant operands that are code pointers
        SgAsmOperandList* ol = insn->get_operandList();
        const vector<SgAsmExpression*>& operands = ol->get_operands();
        for (size_t i = 0; i < operands.size(); ++i) {
          uint64_t constant = 0;
          switch (operands[i]->variantT()) {
            case V_SgAsmWordValueExpression: constant = isSgAsmWordValueExpression(operands[i])->get_value(); break;
            case V_SgAsmDoubleWordValueExpression: constant = isSgAsmDoubleWordValueExpression(operands[i])->get_value(); break;
            case V_SgAsmQuadWordValueExpression: constant = isSgAsmQuadWordValueExpression(operands[i])->get_value(); break;
            default: continue; // Not an appropriately-sized constant
          }
          if (inCodeSegment(constant)) {
            basicBlockStarts[constant] = true;
            if (insns.find(constant) == insns.end()) {
              worklist.push_back(constant);
            }
          }
        }
      }
    }

  };

  void disassembleFile(SgAsmFile* f) {
    ExecFile* ef = Exec::parse(f->get_name().c_str());
    AsmFileWithData file(ef);
    // FIXME: Does not handle multi-architecture binaries
    InsSetArchitecture isa = ISA_UNSPECIFIED;
    const vector<ExecHeader*>& headers = ef->get_headers();
    ROSE_ASSERT (!headers.empty());
    for (size_t i = 0; i < headers.size(); ++i) {
      ExecHeader* hdr = headers[i];
      const Architecture& arch = hdr->get_target();
      InsSetArchitecture thisIsa = arch.get_isa();
      if (isa == ISA_UNSPECIFIED) {
        isa = thisIsa;
      }
      ROSE_ASSERT (isa == thisIsa);
    }
    ROSE_ASSERT (isa == ISA_ARM);
    Parameters p(0x0, true);
    map<uint64_t, SgAsmInstruction*> insns;
    map<uint64_t, bool> basicBlockStarts;
    set<uint64_t> functionStarts;
    uint64_t entryPoint = headers[0]->get_entry_rva();
    basicBlockStarts[entryPoint] = true;
    functionStarts.insert(entryPoint);
    file.disassembleRecursively(entryPoint, p, insns, basicBlockStarts, functionStarts);

    const vector<ExecSection*>& sections = ef->get_sections();
    size_t pointerSize = 4;
    for (size_t i = 0; i < sections.size(); ++i) {
      ExecSection* sect = sections[i];
      if (sect->is_mapped()) { // FIXME: Look for NOBITS sections
        // Scan for pointers to code
        uint64_t endOffset = sect->end_offset();
        ROSE_ASSERT (endOffset <= ef->get_size());
        for (uint64_t j = sect->get_offset();
             j + pointerSize <= endOffset;
             j += pointerSize) {
          uint64_t addr = 0;
          // FIXME: assumes file is little endian
          for (size_t k = pointerSize; k > 0; --k) {
            addr <<= 8;
            addr |= ef->content()[j + k - 1];
          }
          if (file.inCodeSegment(addr)) {
            basicBlockStarts[addr] = true;
            file.disassembleRecursively(addr, p, insns, basicBlockStarts, functionStarts);
          }
        }
      }
    }
    map<uint64_t, SgAsmBlock*> basicBlocks;
    for (map<uint64_t, bool>::const_iterator i = basicBlockStarts.begin(); i != basicBlockStarts.end(); ++i) {
      uint64_t addr = i->first;
      SgAsmBlock* b = new SgAsmBlock();
      b->set_address(addr);
      b->set_id(addr);
      b->set_externallyVisible(i->second);
      basicBlocks[addr] = b;
    }
    SgAsmBlock* blk = PutInstructionsIntoBasicBlocks::putInstructionsIntoBasicBlocks(basicBlocks, insns);
    f->set_global_block(blk);
    blk->set_parent(f);
    blk->set_externallyVisible(true);
  }

}
