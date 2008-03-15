#include <stdint.h>
#include <fcntl.h>
#include <vector>
#include "rose.h"
#include "x86Disassembler.h"

using namespace std;

namespace X86Disassembler {

  enum SegmentOverride {soNone, soCS, soDS, soES, soFS, soGS, soSS};
  enum RepeatPrefix {rpNone, rpRepne, rpRepe};
  enum BranchPrediction {bpNone, bpTaken, bpNotTaken};

  SgAsmType* sizeToType(Size s) {
    switch (s) {
      case size16: return SgAsmTypeWord::createType();
      case size32: return SgAsmTypeDoubleWord::createType();
      case size64: return SgAsmTypeQuadWord::createType();
      default: ROSE_ASSERT (false);
    }
  }

  SgAsmRegisterReferenceExpression::x86_position_in_register_enum sizeToPos(Size s) {
    switch (s) {
      case size16: return SgAsmRegisterReferenceExpression::word;
      case size32: return SgAsmRegisterReferenceExpression::dword;
      case size64: return SgAsmRegisterReferenceExpression::qword;
      default: ROSE_ASSERT (false);
    }
  }

  enum RegisterMode {rmLegacyByte, rmRexByte, rmWord, rmDWord, rmQWord, rmSegment, rmST, rmMM, rmXMM, rmReturnNull /* For mod r/m settings that should not create register exprs */};

  RegisterMode sizeToMode(Size s) {
    switch (s) {
      case size16: return rmWord;
      case size32: return rmDWord;
      case size64: return rmQWord;
      default: ROSE_ASSERT (false);
    }
  }

  SgAsmx86Instruction* makeInstructionWithoutOperands(uint64_t address, const std::string& mnemonic) {
    SgAsmx86Instruction* instruction = NULL;
#include "instruction_x86.inc"
    ROSE_ASSERT (instruction);
    return instruction;
  }

  SgAsmRegisterReferenceExpression* makeRegister(uint8_t fullRegisterNumber, RegisterMode m) {
    if (m == rmReturnNull) return NULL;
    SgAsmRegisterReferenceExpression* ref = new SgAsmRegisterReferenceExpression();
    ROSE_ASSERT (ref);
    switch (m) {
      case rmLegacyByte: {
        ref->set_x86_position_in_register_code((fullRegisterNumber & 1) ? SgAsmRegisterReferenceExpression::high_byte : SgAsmRegisterReferenceExpression::low_byte);
        ref->set_type(SgAsmTypeByte::createType());
        break;
      }
      case rmRexByte: {
        ref->set_x86_position_in_register_code(SgAsmRegisterReferenceExpression::low_byte);
        ref->set_type(SgAsmTypeByte::createType());
        break;
      }
      case rmWord: {
        ref->set_x86_position_in_register_code(SgAsmRegisterReferenceExpression::word);
        ref->set_type(SgAsmTypeWord::createType());
        break;
      }
      case rmDWord: {
        ref->set_x86_position_in_register_code(SgAsmRegisterReferenceExpression::dword);
        ref->set_type(SgAsmTypeDoubleWord::createType());
        break;
      }
      case rmQWord: {
        ref->set_x86_position_in_register_code(SgAsmRegisterReferenceExpression::qword);
        ref->set_type(SgAsmTypeQuadWord::createType());
        break;
      }
      case rmSegment: {
        ref->set_x86_position_in_register_code(SgAsmRegisterReferenceExpression::all);
        ref->set_type(SgAsmTypeWord::createType());
        break;
      }
      case rmST: {
        ref->set_x86_position_in_register_code(SgAsmRegisterReferenceExpression::all);
        ref->set_type(SgAsmType80bitFloat::createType());
        break;
      }
      case rmMM: {
        ref->set_x86_position_in_register_code(SgAsmRegisterReferenceExpression::all);
        ref->set_type(SgAsmTypeVector::createType());
        break;
      }
      case rmXMM: {
        ref->set_x86_position_in_register_code(SgAsmRegisterReferenceExpression::all);
        ref->set_type(SgAsmTypeVector::createType());
        break;
      }
      default: ROSE_ASSERT (false);
    }
    if (m == rmLegacyByte) fullRegisterNumber >>= 1;
    switch (m) {
      case rmLegacyByte:
      case rmRexByte:
      case rmWord:
      case rmDWord:
      case rmQWord: {
        switch (fullRegisterNumber) {
          case  0: ref->set_x86_register_code(SgAsmRegisterReferenceExpression::rAX); break;
          case  1: ref->set_x86_register_code(SgAsmRegisterReferenceExpression::rCX); break;
          case  2: ref->set_x86_register_code(SgAsmRegisterReferenceExpression::rDX); break;
          case  3: ref->set_x86_register_code(SgAsmRegisterReferenceExpression::rBX); break;
          case  4: ref->set_x86_register_code(SgAsmRegisterReferenceExpression::rSP); break;
          case  5: ref->set_x86_register_code(SgAsmRegisterReferenceExpression::rBP); break;
          case  6: ref->set_x86_register_code(SgAsmRegisterReferenceExpression::rSI); break;
          case  7: ref->set_x86_register_code(SgAsmRegisterReferenceExpression::rDI); break;
          case  8: ref->set_x86_register_code(SgAsmRegisterReferenceExpression::r8); break;
          case  9: ref->set_x86_register_code(SgAsmRegisterReferenceExpression::r9); break;
          case 10: ref->set_x86_register_code(SgAsmRegisterReferenceExpression::r10); break;
          case 11: ref->set_x86_register_code(SgAsmRegisterReferenceExpression::r11); break;
          case 12: ref->set_x86_register_code(SgAsmRegisterReferenceExpression::r12); break;
          case 13: ref->set_x86_register_code(SgAsmRegisterReferenceExpression::r13); break;
          case 14: ref->set_x86_register_code(SgAsmRegisterReferenceExpression::r14); break;
          case 15: ref->set_x86_register_code(SgAsmRegisterReferenceExpression::r15); break;
          default: ROSE_ASSERT (false);
        }
        break;
      }

      case rmSegment: {
        switch (fullRegisterNumber & 7) {
          case 0: ref->set_x86_register_code(SgAsmRegisterReferenceExpression::ES); break;
          case 1: ref->set_x86_register_code(SgAsmRegisterReferenceExpression::CS); break;
          case 2: ref->set_x86_register_code(SgAsmRegisterReferenceExpression::SS); break;
          case 3: ref->set_x86_register_code(SgAsmRegisterReferenceExpression::DS); break;
          case 4: ref->set_x86_register_code(SgAsmRegisterReferenceExpression::FS); break;
          case 5: ref->set_x86_register_code(SgAsmRegisterReferenceExpression::GS); break;
          default: throw BadInstruction();
        }
        break;
      }

      case rmST: {
        ref->set_x86_register_code((SgAsmRegisterReferenceExpression::x86_register_enum)(SgAsmRegisterReferenceExpression::STnumbered + (fullRegisterNumber & 7)));
        break;
      }

      case rmMM: {
        ref->set_x86_register_code((SgAsmRegisterReferenceExpression::x86_register_enum)(SgAsmRegisterReferenceExpression::MM + (fullRegisterNumber & 7)));
        break;
      }

      case rmXMM: {
        ref->set_x86_register_code((SgAsmRegisterReferenceExpression::x86_register_enum)(SgAsmRegisterReferenceExpression::XMM + fullRegisterNumber));
        break;
      }

      default: ROSE_ASSERT (false);
    }
    return ref;
  }

  SgAsmByteValueExpression* makeByteValue(uint8_t val) {
    SgAsmByteValueExpression* v = new SgAsmByteValueExpression(val);
    ROSE_ASSERT (v);
    return v;
  }

  SgAsmWordValueExpression* makeWordValue(uint16_t val) {
    SgAsmWordValueExpression* v = new SgAsmWordValueExpression(val);
    ROSE_ASSERT (v);
    return v;
  }

  SgAsmDoubleWordValueExpression* makeDWordValue(uint32_t val) {
    SgAsmDoubleWordValueExpression* v = new SgAsmDoubleWordValueExpression(val);
    ROSE_ASSERT (v);
    return v;
  }

  SgAsmQuadWordValueExpression* makeQWordValue(uint64_t val) {
    SgAsmQuadWordValueExpression* v = new SgAsmQuadWordValueExpression(val);
    ROSE_ASSERT (v);
    return v;
  }

  SgAsmMemoryReferenceExpression* makeMemoryReference(SgAsmExpression* addr, SgAsmType* t = NULL) {
    SgAsmMemoryReferenceExpression* r = new SgAsmMemoryReferenceExpression(addr);
    addr->set_parent(r);
    if (t) r->set_type(t);
    return r;
  }

  SgAsmBinaryAdd* makeAdd(SgAsmExpression* lhs, SgAsmExpression* rhs) {
    SgAsmBinaryAdd* a = new SgAsmBinaryAdd(lhs, rhs);
    lhs->set_parent(a);
    rhs->set_parent(a);
    return a;
  }

  SgAsmBinaryMultiply* makeMul(SgAsmExpression* lhs, SgAsmExpression* rhs) {
    SgAsmBinaryMultiply* a = new SgAsmBinaryMultiply(lhs, rhs);
    lhs->set_parent(a);
    rhs->set_parent(a);
    return a;
  }

  struct SingleInstructionDisassembler {
    // Stuff that is not changed during the course of the instruction
    Parameters p;
    vector<uint64_t>* knownSuccessorsReturn;

    // The instruction
    const vector<uint8_t> insn;
    size_t positionInVector;

    // Temporary flags set by the instruction
    size_t positionInInstruction;
    SegmentOverride segOverride;
    BranchPrediction branchPrediction;
    bool rexPresent, rexW, rexR, rexX, rexB;
    bool sizeMustBe64Bit;
    bool addressSizeOverride, operandSizeOverride;
    bool lock;
    RepeatPrefix repeatPrefix;
    bool modregrmByteSet;
    uint8_t modregrmByte;
    uint8_t modeField, regField, rmField;
    SgAsmExpression* modrm;
    SgAsmExpression* reg;

    SingleInstructionDisassembler(const Parameters& p, const vector<uint8_t>& insn, size_t positionInVector, vector<uint64_t>* knownSuccessorsReturn):
      p(p),
      knownSuccessorsReturn(knownSuccessorsReturn),
      insn(insn),
      positionInVector(positionInVector),
      positionInInstruction(0),
      segOverride(soNone),
      branchPrediction(bpNone),
      rexPresent(false),
      rexW(false),
      rexR(false),
      rexX(false),
      rexB(false),
      sizeMustBe64Bit(false),
      addressSizeOverride(false),
      operandSizeOverride(false),
      lock(false),
      repeatPrefix(rpNone),
      modregrmByteSet(false),
      modregrmByte(0),
      modeField(0),
      regField(0),
      rmField(0),
      modrm(NULL),
      reg(NULL)
    {}

    Size effectiveAddressSize() const {
      if (addressSizeOverride) {
        switch (p.insnSize) {
          case size16: return size32;
          case size32: return size16;
          case size64: return size32;
          default: ROSE_ASSERT (false);
        }
      } else {
        return p.insnSize;
      }
    }

    Size effectiveOperandSize() const {
      if (operandSizeOverride) {
        switch (p.insnSize) {
          case size16: return size32;
          case size32: return size16;
          case size64: return size16;
          default: ROSE_ASSERT (false);
        }
      } else {
        Size s = p.insnSize;
        if (p.insnSize == size64 && !rexW && !sizeMustBe64Bit) s = size32;
        return s;
        // This doesn't handle all of the special cases
      }
    }

    SgAsmType* effectiveOperandType() const {
      return sizeToType(effectiveOperandSize());
    }

    RegisterMode effectiveOperandMode() const {
      return sizeToMode(effectiveOperandSize());
    }

    void getByte(uint8_t& var) {
      if (positionInInstruction >= 15)
        throw BadInstruction();
      if (positionInVector + positionInInstruction >= insn.size())
        throw OverflowOfInstructionVector();
      var = insn[positionInVector + positionInInstruction++];
    }

    void getWord(uint16_t& var) {
      uint8_t high, low;
      getByte(low);
      getByte(high);
      var = (uint16_t(high) << 8) | uint16_t(low);
    }

    void getDWord(uint32_t& var) {
      uint16_t high, low;
      getWord(low);
      getWord(high);
      var = (uint32_t(high) << 16) | uint32_t(low);
    }

    void getQWord(uint64_t& var) {
      uint32_t high, low;
      getDWord(low);
      getDWord(high);
      var = (uint64_t(high) << 32) | uint64_t(low);
    }

    void getModRegRM(RegisterMode regMode, RegisterMode rmMode, SgAsmType* t) {
      getByte(modregrmByte);
      modregrmByteSet = true;
      modeField = modregrmByte >> 6;
      regField = (modregrmByte & 070) >> 3;
      rmField = modregrmByte & 7;
      reg = makeModrmRegister(regMode);
      modrm = makeModrmNormal(rmMode, t);
    }

    // If modrm is a memory reference, fill in its type; otherwise, make a
    // register with the appropriate mode and put it into modrm
    void fillInModRM(RegisterMode rmMode, SgAsmType* t) {
      if (modeField == 3) {
        modrm = makeRegister((rexB ? 8 : 0) + rmField, rmMode);
      } else {
        isSgAsmMemoryReferenceExpression(modrm)->set_type(t);
      }
    }

    SgAsmRegisterReferenceExpression* makeOperandRegisterByte(bool rexExtension, uint8_t registerNumber) {
      SgAsmRegisterReferenceExpression* ref = makeRegister((rexExtension ? 8 : 0) + registerNumber, (rexPresent ? rmRexByte : rmLegacyByte));
      return ref;
    }

    SgAsmRegisterReferenceExpression* makeOperandRegisterFull(bool rexExtension, uint8_t registerNumber) {
      SgAsmRegisterReferenceExpression* ref = makeRegister((rexExtension ? 8 : 0) + registerNumber, sizeToMode(p.insnSize));
      return ref;
    }

    SgAsmRegisterReferenceExpression* makeIP() {
      SgAsmRegisterReferenceExpression* r = new SgAsmRegisterReferenceExpression();
      r->set_x86_register_code(SgAsmRegisterReferenceExpression::rIP);
      r->set_x86_position_in_register_code(sizeToPos(p.insnSize));
      r->set_type(sizeToType(p.insnSize));
      return r;
    }

    SgAsmExpression* makeAddrSizeValue(int64_t val) {
      switch (effectiveAddressSize()) {
        case size16: return makeWordValue((uint16_t)val);
        case size32: return makeDWordValue((uint32_t)val);
        case size64: return makeQWordValue((uint64_t)val);
        default: ROSE_ASSERT (false);
      }
    }

    SgAsmMemoryReferenceExpression* decodeModrmMemory() {
      ROSE_ASSERT (modregrmByteSet);
      SgAsmExpression* addressExpr = NULL;
      if (effectiveAddressSize() == size16) {
        if (modeField == 0 && rmField == 6) { // Special case
          uint16_t offset;
          getWord(offset);
          addressExpr = makeWordValue(offset);
        } else {
          switch (rmField) {
            case 0: addressExpr = makeAdd(makeRegister(3, rmWord), makeRegister(6, rmWord)); break;
            case 1: addressExpr = makeAdd(makeRegister(3, rmWord), makeRegister(7, rmWord)); break;
            case 2: addressExpr = makeAdd(makeRegister(5, rmWord), makeRegister(6, rmWord)); break;
            case 3: addressExpr = makeAdd(makeRegister(5, rmWord), makeRegister(7, rmWord)); break;
            case 4: addressExpr = makeRegister(6, rmWord); break;
            case 5: addressExpr = makeRegister(7, rmWord); break;
            case 6: addressExpr = makeRegister(5, rmWord); break;
            case 7: addressExpr = makeRegister(3, rmWord); break;
            default: ROSE_ASSERT (false);
          }
          switch (modeField) {
            case 0: break; // No offset
            case 1: {uint8_t offset; getByte(offset); addressExpr = makeAdd(addressExpr, makeWordValue((int16_t)(int8_t)offset)); break;}
            case 2: {uint16_t offset; getWord(offset); addressExpr = makeAdd(addressExpr, makeWordValue(offset)); break;}
            default: ROSE_ASSERT (false);
          }
        }
      } else { // 32 or 64 bits
        if (modeField == 0 && rmField == 5) { // Special case
          uint32_t offset;
          getDWord(offset);
          addressExpr = makeAddrSizeValue((int64_t)(int32_t)offset);
          if (p.insnSize == size64) {
            addressExpr = makeAdd(makeIP(), addressExpr);
          }
        } else {
          if (rmField == 4) { // Need SIB
            uint8_t sib;
            getByte(sib);
            uint8_t sibScaleField = sib >> 6;
            uint8_t sibIndexField = (sib & 0070) >> 3;
            uint8_t sibBaseField = sib & 7;
            uint8_t actualScale = (1 << sibScaleField);
            SgAsmExpression* sibBase = NULL;
            if (sibBaseField == 5) {
              switch (modeField) {
                case 0: {uint32_t offset; getDWord(offset); sibBase = makeAddrSizeValue((int64_t)(int32_t)offset); break;}
                case 1: {uint8_t offset; getByte(offset); sibBase = makeAdd(makeRegister((rexB ? 13 : 5), sizeToMode(p.insnSize)), makeAddrSizeValue((int64_t)(int8_t)offset)); break;}
                case 2: {uint32_t offset; getDWord(offset); sibBase = makeAdd(makeRegister((rexB ? 13 : 5), sizeToMode(p.insnSize)), makeAddrSizeValue((int64_t)(int32_t)offset)); break;}
                default: ROSE_ASSERT (false);
              }
            } else {
              sibBase = makeOperandRegisterFull(rexB, sibBaseField);
            }
            if (sibIndexField == 4) {
              addressExpr = sibBase;
            } else if (actualScale == 1) {
              addressExpr = makeAdd(sibBase, makeOperandRegisterFull(rexX, sibIndexField));
            } else {
              addressExpr = makeAdd(sibBase, makeMul(makeOperandRegisterFull(rexX, sibIndexField), makeByteValue(actualScale)));
            }
          } else {
            addressExpr = makeOperandRegisterFull(rexB, rmField);
          }
          switch (modeField) {
            case 0: break; // No offset
            case 1: {uint8_t offset; getByte(offset); addressExpr = makeAdd(addressExpr, makeAddrSizeValue((int64_t)(int8_t)offset)); break;}
            case 2: {uint32_t offset; getDWord(offset); addressExpr = makeAdd(addressExpr, makeAddrSizeValue((int64_t)(int32_t)offset)); break;}
            default: ROSE_ASSERT (false);
          }
        }
      }
      ROSE_ASSERT (addressExpr);
      SgAsmMemoryReferenceExpression* mr = new SgAsmMemoryReferenceExpression(addressExpr);
      addressExpr->set_parent(mr);
      return mr;
    }

    SgAsmRegisterReferenceExpression* makeRegisterEffective(uint8_t fullRegisterNumber) {
      return makeRegister(fullRegisterNumber, effectiveOperandMode());
    }

    SgAsmRegisterReferenceExpression* makeRegisterEffective(bool rexExtension, uint8_t registerNumber) {
      return makeRegister(registerNumber + (rexExtension ? 8 : 0), effectiveOperandMode());
    }

    SgAsmRegisterReferenceExpression* makeModrmRegister(RegisterMode m) {
      ROSE_ASSERT (modregrmByteSet);
      if (m == rmLegacyByte && rexPresent) m = rmRexByte;
      return makeRegister((rexR ? 8 : 0) + regField, m);
    }

    SgAsmExpression* makeModrmNormal(RegisterMode m, SgAsmType* mrType) {
      ROSE_ASSERT (modregrmByteSet);
      if (modeField == 3) { // Register
        if (m == rmLegacyByte && rexPresent) m = rmRexByte;
        return makeRegister((rexB ? 8 : 0) + rmField, m);
      } else {
        SgAsmMemoryReferenceExpression* modrm = decodeModrmMemory();
        modrm->set_type(mrType);
        return modrm;
      }
    }

    SgAsmExpression* getModrmForByte() {
      return makeModrmNormal(rmLegacyByte, SgAsmTypeByte::createType());
    }

    SgAsmExpression* getModrmForXword() {
      return makeModrmNormal(effectiveOperandMode(), effectiveOperandType());
    }

    SgAsmExpression* getModrmForWord() {
      return makeModrmNormal(rmWord, SgAsmTypeWord::createType());
    }

    SgAsmExpression* getModrmForFloat(SgAsmType* t) {
      return makeModrmNormal(rmST, t);
    }

    SgAsmx86Instruction* makeNullaryInstruction(const string& mnemonic) {
      SgAsmx86Instruction* newInsn = makeInstructionWithoutOperands(p.ip, mnemonic);
      newInsn->set_raw_bytes(string(insn.begin() + positionInVector, insn.begin() + positionInVector + positionInInstruction));
      SgAsmOperandList* operands = new SgAsmOperandList();
      newInsn->set_operandList(operands);
      operands->set_parent(newInsn);
      return newInsn;
    }
#define MAKE_INSN0(Tag, Mne) (makeNullaryInstruction(#Mne))

    SgAsmx86Instruction* makeUnaryInstruction(const string& mnemonic, SgAsmExpression* op1) {
      SgAsmx86Instruction* newInsn = makeInstructionWithoutOperands(p.ip, mnemonic);
      newInsn->set_raw_bytes(string(insn.begin() + positionInVector, insn.begin() + positionInVector + positionInInstruction));
      SgAsmOperandList* operands = new SgAsmOperandList();
      newInsn->set_operandList(operands);
      operands->set_parent(newInsn);
      operands->get_operands().push_back(op1);
      op1->set_parent(operands);
      return newInsn;
    }
#define MAKE_INSN1(Tag, Mne, Op1) (makeUnaryInstruction(#Mne, (Op1)))

    SgAsmx86Instruction* makeBinaryInstruction(const string& mnemonic, SgAsmExpression* op1, SgAsmExpression* op2) {
      SgAsmx86Instruction* newInsn = makeInstructionWithoutOperands(p.ip, mnemonic);
      newInsn->set_raw_bytes(string(insn.begin() + positionInVector, insn.begin() + positionInVector + positionInInstruction));
      SgAsmOperandList* operands = new SgAsmOperandList();
      newInsn->set_operandList(operands);
      operands->set_parent(newInsn);
      operands->get_operands().push_back(op1);
      op1->set_parent(operands);
      operands->get_operands().push_back(op2);
      op2->set_parent(operands);
      return newInsn;
    }
#define MAKE_INSN2(Tag, Mne, Op1, Op2) (makeBinaryInstruction(#Mne, (Op1), (Op2)))

    SgAsmx86Instruction* makeTernaryInstruction(const string& mnemonic, SgAsmExpression* op1, SgAsmExpression* op2, SgAsmExpression* op3) {
      SgAsmx86Instruction* newInsn = makeInstructionWithoutOperands(p.ip, mnemonic);
      newInsn->set_raw_bytes(string(insn.begin() + positionInVector, insn.begin() + positionInVector + positionInInstruction));
      SgAsmOperandList* operands = new SgAsmOperandList();
      newInsn->set_operandList(operands);
      operands->set_parent(newInsn);
      operands->get_operands().push_back(op1);
      op1->set_parent(operands);
      operands->get_operands().push_back(op2);
      op2->set_parent(operands);
      operands->get_operands().push_back(op3);
      op3->set_parent(operands);
      return newInsn;
    }
#define MAKE_INSN3(Tag, Mne, Op1, Op2, Op3) (makeTernaryInstruction(#Mne, (Op1), (Op2), (Op3)))

    SgAsmExpression* makeFullSizeValue(uint64_t val) {
      switch (effectiveOperandSize()) {
        case size16: return makeWordValue(val);
        case size32: return makeDWordValue(val);
        case size64: return makeQWordValue(val);
        default: ROSE_ASSERT (false);
      }
    }

    SgAsmx86Instruction* decodeOpcode0F();
    SgAsmx86Instruction* decodeX87InstructionD8();
    SgAsmx86Instruction* decodeX87InstructionD9();
    SgAsmx86Instruction* decodeX87InstructionDA();
    SgAsmx86Instruction* decodeX87InstructionDB();
    SgAsmx86Instruction* decodeX87InstructionDC();
    SgAsmx86Instruction* decodeX87InstructionDD();
    SgAsmx86Instruction* decodeX87InstructionDE();
    SgAsmx86Instruction* decodeX87InstructionDF();
    SgAsmx86Instruction* decodeGroup1(SgAsmExpression* imm);
    SgAsmx86Instruction* decodeGroup1a();
    SgAsmx86Instruction* decodeGroup2(SgAsmExpression* count);
    SgAsmx86Instruction* decodeGroup3(SgAsmExpression* immMaybe);
    SgAsmx86Instruction* decodeGroup4();
    SgAsmx86Instruction* decodeGroup5();
    SgAsmx86Instruction* decodeGroup6();
    SgAsmx86Instruction* decodeGroup7();
    SgAsmx86Instruction* decodeGroup8(SgAsmExpression* imm);
    SgAsmx86Instruction* decodeGroup11(SgAsmExpression* imm);
    SgAsmx86Instruction* decodeGroup15();
    SgAsmx86Instruction* decodeGroup16();
    SgAsmx86Instruction* decodeGroupP();

    SgAsmx86Instruction* disassemble();

    bool longMode() const {return p.insnSize == size64;}

    void setRex(uint8_t opcode) {
      rexPresent = true;
      rexW = (opcode & 8) != 0;
      rexR = (opcode & 4) != 0;
      rexX = (opcode & 2) != 0;
      rexB = (opcode & 1) != 0;
    }

    void not64() const {
      if (longMode()) throw BadInstruction();
    }

    SgAsmExpression* getImmByte() {
      uint8_t val;
      getByte(val);
      return makeByteValue(val);
    }

    SgAsmExpression* getImmWord() {
      uint16_t val;
      getWord(val);
      return makeWordValue(val);
    }

    SgAsmExpression* getImmDWord() {
      uint32_t val;
      getDWord(val);
      return makeDWordValue(val);
    }

    SgAsmExpression* getImmQWord() {
      uint64_t val;
      getQWord(val);
      return makeQWordValue(val);
    }

    SgAsmExpression* getImmIz() {
      if (effectiveOperandSize() == size16) {
        return getImmWord();
      } else {
        return getImmDWord();
      }
    }

    SgAsmExpression* getImmIzAsIv() {
      switch (effectiveOperandSize()) {
        case size16: return getImmWord();
        case size32: return getImmDWord();
        case size64: {
          uint32_t val;
          getDWord(val);
          return makeQWordValue((int64_t)(int32_t)val);
        }
        default: ROSE_ASSERT (false);
      }
    }

    SgAsmExpression* getImmIv() {
      switch (effectiveOperandSize()) {
        case size16: return getImmWord();
        case size32: return getImmDWord();
        case size64: return getImmQWord();
        default: ROSE_ASSERT (false);
      }
    }

    SgAsmExpression* getImmByteAsIv() {
      uint8_t val;
      getByte(val);
      switch (effectiveOperandSize()) {
        case size16: return makeWordValue((int16_t)(int8_t)val);
        case size32: return makeDWordValue((int32_t)(int8_t)val);
        case size64: return makeQWordValue((int64_t)(int8_t)val);
        default: ROSE_ASSERT (false);
      }
    }

    SgAsmExpression* getImmJb() {
      int8_t val;
      getByte((uint8_t&)val);
      if (knownSuccessorsReturn) {
        knownSuccessorsReturn->push_back(p.ip + positionInInstruction + (int64_t)(int8_t)val);
      }
      return makeFullSizeValue(p.ip + positionInInstruction + (int64_t)(int8_t)val);
    }

    SgAsmExpression* getImmJz() {
      int64_t val;
      if (effectiveOperandSize() == size16) {
        uint16_t val2;
        getWord(val2);
        val = (int64_t)(int16_t)val2;
      } else {
        uint32_t val2;
        getDWord(val2);
        val = (int64_t)(int32_t)val2;
      }
      if (knownSuccessorsReturn) {
        knownSuccessorsReturn->push_back(p.ip + positionInInstruction + val);
      }
      return makeFullSizeValue(p.ip + positionInInstruction + val);
    }

    SgAsmExpression* getImmForAddr() {
      switch (effectiveAddressSize()) {
        case size16: return getImmWord();
        case size32: return getImmDWord();
        case size64: return getImmQWord();
        default: ROSE_ASSERT (false);
      }
    }

    void requireMemory() const {
      if (modeField == 3) throw BadInstruction();
    }

  };

  SgAsmx86Instruction* disassemble(const Parameters& p, const vector<uint8_t>& insn, size_t positionInVector, vector<uint64_t>* knownSuccessorsReturn) {
    SingleInstructionDisassembler dis(p, insn, positionInVector, knownSuccessorsReturn);
    return dis.disassemble();
  }

  SgAsmx86Instruction* SingleInstructionDisassembler::disassemble() {
    uint8_t opcode;
    getByte(opcode);
    SgAsmx86Instruction* insn = 0;
    switch (opcode) {
      case 0x00: {getModRegRM(rmLegacyByte, rmLegacyByte, SgAsmTypeByte::createType()); insn = MAKE_INSN2(Add, add, modrm, reg); goto done;}
      case 0x01: {getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); insn = MAKE_INSN2(Add, add, modrm, reg); goto done;}
      case 0x02: {getModRegRM(rmLegacyByte, rmLegacyByte, SgAsmTypeByte::createType()); insn = MAKE_INSN2(Add, add, reg, modrm); goto done;}
      case 0x03: {getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); insn = MAKE_INSN2(Add, add, reg, modrm); goto done;}
      case 0x04: {SgAsmExpression* imm = getImmByteAsIv(); insn = MAKE_INSN2(Add, add, makeRegister(0, rmLegacyByte), imm); goto done;}
      case 0x05: {SgAsmExpression* imm = getImmIzAsIv(); insn = MAKE_INSN2(Add, add, makeRegisterEffective(0), imm); goto done;}
      case 0x06: {not64(); insn = MAKE_INSN1(Push, push, makeRegister(0, rmSegment)); goto done;}
      case 0x07: {not64(); insn = MAKE_INSN1(Pop, pop, makeRegister(0, rmSegment)); goto done;}
      case 0x08: {getModRegRM(rmLegacyByte, rmLegacyByte, SgAsmTypeByte::createType()); insn = MAKE_INSN2(Or, or, modrm, reg); goto done;}
      case 0x09: {getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); insn = MAKE_INSN2(Or, or, modrm, reg); goto done;}
      case 0x0A: {getModRegRM(rmLegacyByte, rmLegacyByte, SgAsmTypeByte::createType()); insn = MAKE_INSN2(Or, or, reg, modrm); goto done;}
      case 0x0B: {getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); insn = MAKE_INSN2(Or, or, reg, modrm); goto done;}
      case 0x0C: {SgAsmExpression* imm = getImmByteAsIv(); insn = MAKE_INSN2(Or, or, makeRegister(0, rmLegacyByte), imm); goto done;}
      case 0x0D: {SgAsmExpression* imm = getImmIzAsIv(); insn = MAKE_INSN2(Or, or, makeRegisterEffective(0), imm); goto done;}
      case 0x0E: {not64(); insn = MAKE_INSN1(Push, push, makeRegister(1, rmSegment)); goto done;}
      case 0x0F: {insn = decodeOpcode0F(); goto done;}
      case 0x10: {getModRegRM(rmLegacyByte, rmLegacyByte, SgAsmTypeByte::createType()); insn = MAKE_INSN2(Adc, adc, modrm, reg); goto done;}
      case 0x11: {getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); insn = MAKE_INSN2(Adc, adc, modrm, reg); goto done;}
      case 0x12: {getModRegRM(rmLegacyByte, rmLegacyByte, SgAsmTypeByte::createType()); insn = MAKE_INSN2(Adc, adc, reg, modrm); goto done;}
      case 0x13: {getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); insn = MAKE_INSN2(Adc, adc, reg, modrm); goto done;}
      case 0x14: {SgAsmExpression* imm = getImmByteAsIv(); insn = MAKE_INSN2(Adc, adc, makeRegister(0, rmLegacyByte), imm); goto done;}
      case 0x15: {SgAsmExpression* imm = getImmIzAsIv(); insn = MAKE_INSN2(Adc, adc, makeRegisterEffective(0), imm); goto done;}
      case 0x16: {not64(); insn = MAKE_INSN1(Push, push, makeRegister(2, rmSegment)); goto done;}
      case 0x17: {not64(); insn = MAKE_INSN1(Pop, pop, makeRegister(2, rmSegment)); goto done;}
      case 0x18: {getModRegRM(rmLegacyByte, rmLegacyByte, SgAsmTypeByte::createType()); insn = MAKE_INSN2(Sbb, sbb, modrm, reg); goto done;}
      case 0x19: {getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); insn = MAKE_INSN2(Sbb, sbb, modrm, reg); goto done;}
      case 0x1A: {getModRegRM(rmLegacyByte, rmLegacyByte, SgAsmTypeByte::createType()); insn = MAKE_INSN2(Sbb, sbb, reg, modrm); goto done;}
      case 0x1B: {getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); insn = MAKE_INSN2(Sbb, sbb, reg, modrm); goto done;}
      case 0x1C: {SgAsmExpression* imm = getImmByteAsIv(); insn = MAKE_INSN2(Sbb, sbb, makeRegister(0, rmLegacyByte), imm); goto done;}
      case 0x1D: {SgAsmExpression* imm = getImmIzAsIv(); insn = MAKE_INSN2(Sbb, sbb, makeRegisterEffective(0), imm); goto done;}
      case 0x1E: {not64(); insn = MAKE_INSN1(Push, push, makeRegister(3, rmSegment)); goto done;}
      case 0x1F: {not64(); insn = MAKE_INSN1(Pop, pop, makeRegister(3, rmSegment)); goto done;}
      case 0x20: {getModRegRM(rmLegacyByte, rmLegacyByte, SgAsmTypeByte::createType()); insn = MAKE_INSN2(And, and, modrm, reg); goto done;}
      case 0x21: {getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); insn = MAKE_INSN2(And, and, modrm, reg); goto done;}
      case 0x22: {getModRegRM(rmLegacyByte, rmLegacyByte, SgAsmTypeByte::createType()); insn = MAKE_INSN2(And, and, reg, modrm); goto done;}
      case 0x23: {getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); insn = MAKE_INSN2(And, and, reg, modrm); goto done;}
      case 0x24: {SgAsmExpression* imm = getImmByteAsIv(); insn = MAKE_INSN2(And, and, makeRegister(0, rmLegacyByte), imm); goto done;}
      case 0x25: {SgAsmExpression* imm = getImmIzAsIv(); insn = MAKE_INSN2(And, and, makeRegisterEffective(0), imm); goto done;}
      case 0x26: {segOverride = soES; insn = disassemble(); goto done;}
      case 0x27: {not64(); insn = MAKE_INSN0(Daa, daa); goto done;}
      case 0x28: {getModRegRM(rmLegacyByte, rmLegacyByte, SgAsmTypeByte::createType()); insn = MAKE_INSN2(Sub, sub, modrm, reg); goto done;}
      case 0x29: {getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); insn = MAKE_INSN2(Sub, sub, modrm, reg); goto done;}
      case 0x2A: {getModRegRM(rmLegacyByte, rmLegacyByte, SgAsmTypeByte::createType()); insn = MAKE_INSN2(Sub, sub, reg, modrm); goto done;}
      case 0x2B: {getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); insn = MAKE_INSN2(Sub, sub, reg, modrm); goto done;}
      case 0x2C: {SgAsmExpression* imm = getImmByteAsIv(); insn = MAKE_INSN2(Sub, sub, makeRegister(0, rmLegacyByte), imm); goto done;}
      case 0x2D: {SgAsmExpression* imm = getImmIzAsIv(); insn = MAKE_INSN2(Sub, sub, makeRegisterEffective(0), imm); goto done;}
      case 0x2E: {segOverride = soCS; branchPrediction = bpNotTaken; insn = disassemble(); goto done;}
      case 0x2F: {not64(); insn = MAKE_INSN0(Das, das); goto done;}
      case 0x30: {getModRegRM(rmLegacyByte, rmLegacyByte, SgAsmTypeByte::createType()); insn = MAKE_INSN2(Xor, xor, modrm, reg); goto done;}
      case 0x31: {getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); insn = MAKE_INSN2(Xor, xor, modrm, reg); goto done;}
      case 0x32: {getModRegRM(rmLegacyByte, rmLegacyByte, SgAsmTypeByte::createType()); insn = MAKE_INSN2(Xor, xor, reg, modrm); goto done;}
      case 0x33: {getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); insn = MAKE_INSN2(Xor, xor, reg, modrm); goto done;}
      case 0x34: {SgAsmExpression* imm = getImmByteAsIv(); insn = MAKE_INSN2(Xor, xor, makeRegister(0, rmLegacyByte), imm); goto done;}
      case 0x35: {SgAsmExpression* imm = getImmIzAsIv(); insn = MAKE_INSN2(Xor, xor, makeRegisterEffective(0), imm); goto done;}
      case 0x36: {segOverride = soSS; insn = disassemble(); goto done;}
      case 0x37: {not64(); insn = MAKE_INSN0(Aaa, aaa); goto done;}
      case 0x38: {getModRegRM(rmLegacyByte, rmLegacyByte, SgAsmTypeByte::createType()); insn = MAKE_INSN2(Cmp, cmp, modrm, reg); goto done;}
      case 0x39: {getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); insn = MAKE_INSN2(Cmp, cmp, modrm, reg); goto done;}
      case 0x3A: {getModRegRM(rmLegacyByte, rmLegacyByte, SgAsmTypeByte::createType()); insn = MAKE_INSN2(Cmp, cmp, reg, modrm); goto done;}
      case 0x3B: {getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); insn = MAKE_INSN2(Cmp, cmp, reg, modrm); goto done;}
      case 0x3C: {SgAsmExpression* imm = getImmByteAsIv(); insn = MAKE_INSN2(Cmp, cmp, makeRegister(0, rmLegacyByte), imm); goto done;}
      case 0x3D: {SgAsmExpression* imm = getImmIzAsIv(); insn = MAKE_INSN2(Cmp, cmp, makeRegisterEffective(0), imm); goto done;}
      case 0x3E: {segOverride = soDS; branchPrediction = bpTaken; insn = disassemble(); goto done;}
      case 0x3F: {not64(); insn = MAKE_INSN0(Aas, aas); goto done;}
      case 0x40: {if (longMode()) {setRex(0x40); insn = disassemble(); goto done;} else {insn = MAKE_INSN1(Inc, inc, makeRegisterEffective(0)); goto done;}}
      case 0x41: {if (longMode()) {setRex(0x41); insn = disassemble(); goto done;} else {insn = MAKE_INSN1(Inc, inc, makeRegisterEffective(1)); goto done;}}
      case 0x42: {if (longMode()) {setRex(0x42); insn = disassemble(); goto done;} else {insn = MAKE_INSN1(Inc, inc, makeRegisterEffective(2)); goto done;}}
      case 0x43: {if (longMode()) {setRex(0x43); insn = disassemble(); goto done;} else {insn = MAKE_INSN1(Inc, inc, makeRegisterEffective(3)); goto done;}}
      case 0x44: {if (longMode()) {setRex(0x44); insn = disassemble(); goto done;} else {insn = MAKE_INSN1(Inc, inc, makeRegisterEffective(4)); goto done;}}
      case 0x45: {if (longMode()) {setRex(0x45); insn = disassemble(); goto done;} else {insn = MAKE_INSN1(Inc, inc, makeRegisterEffective(5)); goto done;}}
      case 0x46: {if (longMode()) {setRex(0x46); insn = disassemble(); goto done;} else {insn = MAKE_INSN1(Inc, inc, makeRegisterEffective(6)); goto done;}}
      case 0x47: {if (longMode()) {setRex(0x47); insn = disassemble(); goto done;} else {insn = MAKE_INSN1(Inc, inc, makeRegisterEffective(7)); goto done;}}
      case 0x48: {if (longMode()) {setRex(0x48); insn = disassemble(); goto done;} else {insn = MAKE_INSN1(Dec, dec, makeRegisterEffective(0)); goto done;}}
      case 0x49: {if (longMode()) {setRex(0x49); insn = disassemble(); goto done;} else {insn = MAKE_INSN1(Dec, dec, makeRegisterEffective(1)); goto done;}}
      case 0x4A: {if (longMode()) {setRex(0x4A); insn = disassemble(); goto done;} else {insn = MAKE_INSN1(Dec, dec, makeRegisterEffective(2)); goto done;}}
      case 0x4B: {if (longMode()) {setRex(0x4B); insn = disassemble(); goto done;} else {insn = MAKE_INSN1(Dec, dec, makeRegisterEffective(3)); goto done;}}
      case 0x4C: {if (longMode()) {setRex(0x4C); insn = disassemble(); goto done;} else {insn = MAKE_INSN1(Dec, dec, makeRegisterEffective(4)); goto done;}}
      case 0x4D: {if (longMode()) {setRex(0x4D); insn = disassemble(); goto done;} else {insn = MAKE_INSN1(Dec, dec, makeRegisterEffective(5)); goto done;}}
      case 0x4E: {if (longMode()) {setRex(0x4E); insn = disassemble(); goto done;} else {insn = MAKE_INSN1(Dec, dec, makeRegisterEffective(6)); goto done;}}
      case 0x4F: {if (longMode()) {setRex(0x4F); insn = disassemble(); goto done;} else {insn = MAKE_INSN1(Dec, dec, makeRegisterEffective(7)); goto done;}}
      case 0x50: {sizeMustBe64Bit = true; insn = MAKE_INSN1(Push, push, makeRegisterEffective(rexB, 0)); goto done;}
      case 0x51: {sizeMustBe64Bit = true; insn = MAKE_INSN1(Push, push, makeRegisterEffective(rexB, 1)); goto done;}
      case 0x52: {sizeMustBe64Bit = true; insn = MAKE_INSN1(Push, push, makeRegisterEffective(rexB, 2)); goto done;}
      case 0x53: {sizeMustBe64Bit = true; insn = MAKE_INSN1(Push, push, makeRegisterEffective(rexB, 3)); goto done;}
      case 0x54: {sizeMustBe64Bit = true; insn = MAKE_INSN1(Push, push, makeRegisterEffective(rexB, 4)); goto done;}
      case 0x55: {sizeMustBe64Bit = true; insn = MAKE_INSN1(Push, push, makeRegisterEffective(rexB, 5)); goto done;}
      case 0x56: {sizeMustBe64Bit = true; insn = MAKE_INSN1(Push, push, makeRegisterEffective(rexB, 6)); goto done;}
      case 0x57: {sizeMustBe64Bit = true; insn = MAKE_INSN1(Push, push, makeRegisterEffective(rexB, 7)); goto done;}
      case 0x58: {sizeMustBe64Bit = true; insn = MAKE_INSN1(Pop, pop, makeRegisterEffective(rexB, 0)); goto done;}
      case 0x59: {sizeMustBe64Bit = true; insn = MAKE_INSN1(Pop, pop, makeRegisterEffective(rexB, 1)); goto done;}
      case 0x5A: {sizeMustBe64Bit = true; insn = MAKE_INSN1(Pop, pop, makeRegisterEffective(rexB, 2)); goto done;}
      case 0x5B: {sizeMustBe64Bit = true; insn = MAKE_INSN1(Pop, pop, makeRegisterEffective(rexB, 3)); goto done;}
      case 0x5C: {sizeMustBe64Bit = true; insn = MAKE_INSN1(Pop, pop, makeRegisterEffective(rexB, 4)); goto done;}
      case 0x5D: {sizeMustBe64Bit = true; insn = MAKE_INSN1(Pop, pop, makeRegisterEffective(rexB, 5)); goto done;}
      case 0x5E: {sizeMustBe64Bit = true; insn = MAKE_INSN1(Pop, pop, makeRegisterEffective(rexB, 6)); goto done;}
      case 0x5F: {sizeMustBe64Bit = true; insn = MAKE_INSN1(Pop, pop, makeRegisterEffective(rexB, 7)); goto done;}
      case 0x60: {not64(); insn = MAKE_INSN0(Pusha, pusha); goto done;}
      case 0x61: {not64(); insn = MAKE_INSN0(Popa, popa); goto done;}
      case 0x62: {not64(); getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); requireMemory(); insn = MAKE_INSN2(Bound, bound, reg, modrm); goto done;}
      case 0x63: {if (longMode()) {getModRegRM(effectiveOperandMode(), rmDWord, SgAsmTypeDoubleWord::createType()); insn = MAKE_INSN2(Movsxd, movsxd, reg, modrm); goto done;} else {getModRegRM(rmWord, rmWord, SgAsmTypeWord::createType()); insn = MAKE_INSN2(Arpl, arpl, modrm, reg); goto done;}}
      case 0x64: {segOverride = soFS; insn = disassemble(); goto done;}
      case 0x65: {segOverride = soGS; insn = disassemble(); goto done;}
      case 0x66: {operandSizeOverride = true; insn = disassemble(); goto done;}
      case 0x67: {addressSizeOverride = true; insn = disassemble(); goto done;}
      case 0x68: {sizeMustBe64Bit = true; SgAsmExpression* imm = getImmIzAsIv(); insn = MAKE_INSN1(Push, push, imm); goto done;}
      case 0x69: {getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); SgAsmExpression* imm = getImmIzAsIv(); insn = MAKE_INSN3(Imul, imul, reg, modrm, imm); goto done;}
      case 0x6A: {sizeMustBe64Bit = true; SgAsmExpression* imm = getImmByteAsIv(); insn = MAKE_INSN1(Push, push, imm); goto done;}
      case 0x6B: {getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); SgAsmExpression* imm = getImmByteAsIv(); insn = MAKE_INSN3(Imul, imul, reg, modrm, imm); goto done;}
      case 0x6C: {insn = MAKE_INSN0(Ins, ins); goto done;}
      case 0x6D: {insn = MAKE_INSN0(Ins, ins); goto done;}
      case 0x6E: {insn = MAKE_INSN0(Outs, outs); goto done;}
      case 0x6F: {insn = MAKE_INSN0(Outs, outs); goto done;}
      case 0x70: {SgAsmExpression* imm = getImmJb(); insn = MAKE_INSN1(Jo , jo , /* prediction, */ imm); goto done;}
      case 0x71: {SgAsmExpression* imm = getImmJb(); insn = MAKE_INSN1(Jno, jno, /* prediction, */ imm); goto done;}
      case 0x72: {SgAsmExpression* imm = getImmJb(); insn = MAKE_INSN1(Jb , jb , /* prediction, */ imm); goto done;}
      case 0x73: {SgAsmExpression* imm = getImmJb(); insn = MAKE_INSN1(Jae, jae, /* prediction, */ imm); goto done;}
      case 0x74: {SgAsmExpression* imm = getImmJb(); insn = MAKE_INSN1(Je , je , /* prediction, */ imm); goto done;}
      case 0x75: {SgAsmExpression* imm = getImmJb(); insn = MAKE_INSN1(Jne, jne, /* prediction, */ imm); goto done;}
      case 0x76: {SgAsmExpression* imm = getImmJb(); insn = MAKE_INSN1(Jbe, jbe, /* prediction, */ imm); goto done;}
      case 0x77: {SgAsmExpression* imm = getImmJb(); insn = MAKE_INSN1(Ja , ja , /* prediction, */ imm); goto done;}
      case 0x78: {SgAsmExpression* imm = getImmJb(); insn = MAKE_INSN1(Js , js , /* prediction, */ imm); goto done;}
      case 0x79: {SgAsmExpression* imm = getImmJb(); insn = MAKE_INSN1(Jns, jns, /* prediction, */ imm); goto done;}
      case 0x7A: {SgAsmExpression* imm = getImmJb(); insn = MAKE_INSN1(Jpe, jpe, /* prediction, */ imm); goto done;}
      case 0x7B: {SgAsmExpression* imm = getImmJb(); insn = MAKE_INSN1(Jpo, jpo, /* prediction, */ imm); goto done;}
      case 0x7C: {SgAsmExpression* imm = getImmJb(); insn = MAKE_INSN1(Jl , jl , /* prediction, */ imm); goto done;}
      case 0x7D: {SgAsmExpression* imm = getImmJb(); insn = MAKE_INSN1(Jge, jge, /* prediction, */ imm); goto done;}
      case 0x7E: {SgAsmExpression* imm = getImmJb(); insn = MAKE_INSN1(Jle, jle, /* prediction, */ imm); goto done;}
      case 0x7F: {SgAsmExpression* imm = getImmJb(); insn = MAKE_INSN1(Jg , jg , /* prediction, */ imm); goto done;}
      case 0x80: {getModRegRM(rmReturnNull, rmLegacyByte, SgAsmTypeByte::createType()); SgAsmExpression* imm = getImmByte(); insn = decodeGroup1(imm); goto done;}
      case 0x81: {getModRegRM(rmReturnNull, effectiveOperandMode(), effectiveOperandType()); SgAsmExpression* imm = getImmIzAsIv(); insn = decodeGroup1(imm); goto done;}
      case 0x82: {not64(); getModRegRM(rmReturnNull, rmLegacyByte, SgAsmTypeByte::createType()); SgAsmExpression* imm = getImmByte(); insn = decodeGroup1(imm); goto done;}
      case 0x83: {getModRegRM(rmReturnNull, effectiveOperandMode(), effectiveOperandType()); SgAsmExpression* imm = getImmByteAsIv(); insn = decodeGroup1(imm); goto done;}
      case 0x84: {getModRegRM(rmLegacyByte, rmLegacyByte, SgAsmTypeByte::createType()); insn = MAKE_INSN2(Test, test, modrm, reg); goto done;}
      case 0x85: {getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); insn = MAKE_INSN2(Test, test, modrm, reg); goto done;}
      case 0x86: {getModRegRM(rmLegacyByte, rmLegacyByte, SgAsmTypeByte::createType()); insn = MAKE_INSN2(Xchg, xchg, modrm, reg); goto done;}
      case 0x87: {getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); insn = MAKE_INSN2(Xchg, xchg, modrm, reg); goto done;}
      case 0x88: {getModRegRM(rmLegacyByte, rmLegacyByte, SgAsmTypeByte::createType()); insn = MAKE_INSN2(Mov, mov, modrm, reg); goto done;}
      case 0x89: {getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); insn = MAKE_INSN2(Mov, mov, modrm, reg); goto done;}
      case 0x8A: {getModRegRM(rmLegacyByte, rmLegacyByte, SgAsmTypeByte::createType()); insn = MAKE_INSN2(Mov, mov, reg, modrm); goto done;}
      case 0x8B: {getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); insn = MAKE_INSN2(Mov, mov, reg, modrm); goto done;}
      case 0x8C: {getModRegRM(rmSegment, effectiveOperandMode(), SgAsmTypeWord::createType()); insn = MAKE_INSN2(Mov, mov, modrm, reg); goto done;}
      case 0x8D: {getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); requireMemory(); insn = MAKE_INSN2(Lea, lea, reg, modrm); goto done;}
      case 0x8E: {getModRegRM(rmSegment, rmWord, SgAsmTypeWord::createType()); insn = MAKE_INSN2(Mov, mov, reg, modrm); goto done;}
      case 0x8F: {getModRegRM(rmReturnNull, effectiveOperandMode(), effectiveOperandType()); insn = decodeGroup1a(); goto done;}
      case 0x90: {if (rexB) {insn = MAKE_INSN2(Xchg, xchg, makeRegisterEffective(8), makeRegisterEffective(0)); goto done;} else {insn = MAKE_INSN0(Nop, nop); goto done;}}
      case 0x91: {insn = MAKE_INSN2(Xchg, xchg, makeRegisterEffective(rexB, 1), makeRegisterEffective(0)); goto done;}
      case 0x92: {insn = MAKE_INSN2(Xchg, xchg, makeRegisterEffective(rexB, 2), makeRegisterEffective(0)); goto done;}
      case 0x93: {insn = MAKE_INSN2(Xchg, xchg, makeRegisterEffective(rexB, 3), makeRegisterEffective(0)); goto done;}
      case 0x94: {insn = MAKE_INSN2(Xchg, xchg, makeRegisterEffective(rexB, 4), makeRegisterEffective(0)); goto done;}
      case 0x95: {insn = MAKE_INSN2(Xchg, xchg, makeRegisterEffective(rexB, 5), makeRegisterEffective(0)); goto done;}
      case 0x96: {insn = MAKE_INSN2(Xchg, xchg, makeRegisterEffective(rexB, 6), makeRegisterEffective(0)); goto done;}
      case 0x97: {insn = MAKE_INSN2(Xchg, xchg, makeRegisterEffective(rexB, 7), makeRegisterEffective(0)); goto done;}
      case 0x98: {insn = MAKE_INSN0(Cbw, cbw); goto done;}
      case 0x99: {insn = MAKE_INSN0(Cwd, cwd); goto done;}
      case 0x9A: {not64(); SgAsmExpression* addr = getImmForAddr(); SgAsmExpression* seg = getImmWord(); insn = MAKE_INSN2(FarCall, farCall, seg, addr); goto done;}
      case 0x9B: {insn = MAKE_INSN0(Wait, wait); goto done;}
      case 0x9C: {sizeMustBe64Bit = true; insn = MAKE_INSN0(Pushf, pushf); goto done;}
      case 0x9D: {sizeMustBe64Bit = true; insn = MAKE_INSN0(Popf, popf); goto done;}
      case 0x9E: {insn = MAKE_INSN0(Sahf, sahf); goto done;}
      case 0x9F: {insn = MAKE_INSN0(Lahf, lahf); goto done;}
      case 0xA0: {SgAsmExpression* addr = getImmForAddr(); insn = MAKE_INSN2(Mov, mov, makeRegister(0, rmLegacyByte), makeMemoryReference(addr, SgAsmTypeByte::createType())); goto done;}
      case 0xA1: {SgAsmExpression* addr = getImmForAddr(); insn = MAKE_INSN2(Mov, mov, makeRegisterEffective(0), makeMemoryReference(addr, effectiveOperandType())); goto done;}
      case 0xA2: {SgAsmExpression* addr = getImmForAddr(); insn = MAKE_INSN2(Mov, mov, makeMemoryReference(addr, SgAsmTypeByte::createType()), makeRegister(0, rmLegacyByte)); goto done;}
      case 0xA3: {SgAsmExpression* addr = getImmForAddr(); insn = MAKE_INSN2(Mov, mov, makeMemoryReference(addr, effectiveOperandType()), makeRegisterEffective(0)); goto done;}
      case 0xA4: {insn = MAKE_INSN0(Movs, movs); goto done;}
      case 0xA5: {insn = MAKE_INSN0(Movs, movs); goto done;}
      case 0xA6: {insn = MAKE_INSN0(Cmps, cmps); goto done;}
      case 0xA7: {insn = MAKE_INSN0(Cmps, cmps); goto done;}
      case 0xA8: {SgAsmExpression* imm = getImmByte(); insn = MAKE_INSN2(Test, test, makeRegister(0, rmLegacyByte), imm); goto done;}
      case 0xA9: {SgAsmExpression* imm = getImmIzAsIv(); insn = MAKE_INSN2(Test, test, makeRegisterEffective(0), imm); goto done;}
      case 0xAA: {insn = MAKE_INSN0(Stos, stos); goto done;}
      case 0xAB: {insn = MAKE_INSN0(Stos, stos); goto done;}
      case 0xAC: {insn = MAKE_INSN0(Lods, lods); goto done;}
      case 0xAD: {insn = MAKE_INSN0(Lods, lods); goto done;}
      case 0xAE: {insn = MAKE_INSN0(Scas, scas); goto done;}
      case 0xAF: {insn = MAKE_INSN0(Scas, scas); goto done;}
      case 0xB0: {SgAsmExpression* imm = getImmByte(); insn = MAKE_INSN2(Mov, mov, makeOperandRegisterByte(rexB, 0), imm); goto done;}
      case 0xB1: {SgAsmExpression* imm = getImmByte(); insn = MAKE_INSN2(Mov, mov, makeOperandRegisterByte(rexB, 1), imm); goto done;}
      case 0xB2: {SgAsmExpression* imm = getImmByte(); insn = MAKE_INSN2(Mov, mov, makeOperandRegisterByte(rexB, 2), imm); goto done;}
      case 0xB3: {SgAsmExpression* imm = getImmByte(); insn = MAKE_INSN2(Mov, mov, makeOperandRegisterByte(rexB, 3), imm); goto done;}
      case 0xB4: {SgAsmExpression* imm = getImmByte(); insn = MAKE_INSN2(Mov, mov, makeOperandRegisterByte(rexB, 4), imm); goto done;}
      case 0xB5: {SgAsmExpression* imm = getImmByte(); insn = MAKE_INSN2(Mov, mov, makeOperandRegisterByte(rexB, 5), imm); goto done;}
      case 0xB6: {SgAsmExpression* imm = getImmByte(); insn = MAKE_INSN2(Mov, mov, makeOperandRegisterByte(rexB, 6), imm); goto done;}
      case 0xB7: {SgAsmExpression* imm = getImmByte(); insn = MAKE_INSN2(Mov, mov, makeOperandRegisterByte(rexB, 7), imm); goto done;}
      case 0xB8: {SgAsmExpression* imm = getImmIv(); insn = MAKE_INSN2(Mov, mov, makeRegisterEffective(rexB, 0), imm); goto done;}
      case 0xB9: {SgAsmExpression* imm = getImmIv(); insn = MAKE_INSN2(Mov, mov, makeRegisterEffective(rexB, 1), imm); goto done;}
      case 0xBA: {SgAsmExpression* imm = getImmIv(); insn = MAKE_INSN2(Mov, mov, makeRegisterEffective(rexB, 2), imm); goto done;}
      case 0xBB: {SgAsmExpression* imm = getImmIv(); insn = MAKE_INSN2(Mov, mov, makeRegisterEffective(rexB, 3), imm); goto done;}
      case 0xBC: {SgAsmExpression* imm = getImmIv(); insn = MAKE_INSN2(Mov, mov, makeRegisterEffective(rexB, 4), imm); goto done;}
      case 0xBD: {SgAsmExpression* imm = getImmIv(); insn = MAKE_INSN2(Mov, mov, makeRegisterEffective(rexB, 5), imm); goto done;}
      case 0xBE: {SgAsmExpression* imm = getImmIv(); insn = MAKE_INSN2(Mov, mov, makeRegisterEffective(rexB, 6), imm); goto done;}
      case 0xBF: {SgAsmExpression* imm = getImmIv(); insn = MAKE_INSN2(Mov, mov, makeRegisterEffective(rexB, 7), imm); goto done;}
      case 0xC0: {getModRegRM(rmReturnNull, rmLegacyByte, SgAsmTypeByte::createType()); SgAsmExpression* imm = getImmByte(); insn = decodeGroup2(imm); goto done;}
      case 0xC1: {getModRegRM(rmReturnNull, effectiveOperandMode(), effectiveOperandType()); SgAsmExpression* imm = getImmByteAsIv(); insn = decodeGroup2(imm); goto done;}
      case 0xC2: {SgAsmExpression* imm = getImmWord(); insn = MAKE_INSN1(Ret, ret, imm); goto done;}
      case 0xC3: {insn = MAKE_INSN0(Ret, ret); goto done;}
      case 0xC4: {not64(); getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); requireMemory(); insn = MAKE_INSN2(Les, les, reg, modrm); goto done;}
      case 0xC5: {not64(); getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); requireMemory(); insn = MAKE_INSN2(Lds, lds, reg, modrm); goto done;}
      case 0xC6: {getModRegRM(rmReturnNull, rmLegacyByte, SgAsmTypeByte::createType()); SgAsmExpression* imm = getImmByte(); insn = decodeGroup11(imm); goto done;}
      case 0xC7: {getModRegRM(rmReturnNull, effectiveOperandMode(), effectiveOperandType()); SgAsmExpression* imm = getImmIzAsIv(); insn = decodeGroup11(imm); goto done;}
      case 0xC8: {SgAsmExpression* immw; {SgAsmExpression* imm = getImmWord(); immw = imm;} SgAsmExpression* imm = getImmByte(); insn = MAKE_INSN2(Enter, enter, immw, imm); goto done;}
      case 0xC9: {insn = MAKE_INSN0(Leave, leave); goto done;}
      case 0xCA: {SgAsmExpression* imm = getImmWord(); insn = MAKE_INSN1(Retf, retf, imm); goto done;}
      case 0xCB: {insn = MAKE_INSN0(Retf, retf); goto done;}
      case 0xCC: {insn = MAKE_INSN0(Int3, int3); goto done;}
      case 0xCD: {SgAsmExpression* imm = getImmByte(); insn = MAKE_INSN1(Int, int, imm); goto done;}
      case 0xCE: {not64(); insn = MAKE_INSN0(Into, into); goto done;}
      case 0xCF: {insn = MAKE_INSN0(Iret, iret); goto done;}
      case 0xD0: {getModRegRM(rmReturnNull, rmLegacyByte, SgAsmTypeByte::createType()); insn = decodeGroup2(makeByteValue(1)); goto done;}
      case 0xD1: {getModRegRM(rmReturnNull, effectiveOperandMode(), effectiveOperandType()); insn = decodeGroup2(makeByteValue(1)); goto done;}
      case 0xD2: {getModRegRM(rmReturnNull, rmLegacyByte, SgAsmTypeByte::createType()); insn = decodeGroup2(makeRegister(2, rmLegacyByte)); goto done;}
      case 0xD3: {getModRegRM(rmReturnNull, effectiveOperandMode(), effectiveOperandType()); insn = decodeGroup2(makeRegister(2, rmLegacyByte)); goto done;}
      case 0xD4: {not64(); insn = MAKE_INSN0(Aam, aam); goto done;}
      case 0xD5: {not64(); insn = MAKE_INSN0(Aad, aad); goto done;}
      case 0xD6: {not64(); insn = MAKE_INSN0(Salc, salc); goto done;}
      case 0xD7: {insn = MAKE_INSN0(Xlat, xlat); goto done;}
      case 0xD8: {insn = decodeX87InstructionD8(); goto done;}
      case 0xD9: {insn = decodeX87InstructionD9(); goto done;}
      case 0xDA: {insn = decodeX87InstructionDA(); goto done;}
      case 0xDB: {insn = decodeX87InstructionDB(); goto done;}
      case 0xDC: {insn = decodeX87InstructionDC(); goto done;}
      case 0xDD: {insn = decodeX87InstructionDD(); goto done;}
      case 0xDE: {insn = decodeX87InstructionDE(); goto done;}
      case 0xDF: {insn = decodeX87InstructionDF(); goto done;}
      case 0xE0: {SgAsmExpression* imm = getImmJb(); insn = MAKE_INSN1(Loopnz, loopnz, /* prediction, */ imm); goto done;}
      case 0xE1: {SgAsmExpression* imm = getImmJb(); insn = MAKE_INSN1(Loopz, loopz, /* prediction, */ imm); goto done;}
      case 0xE2: {SgAsmExpression* imm = getImmJb(); insn = MAKE_INSN1(Loop, loop, /* prediction, */ imm); goto done;}
      case 0xE3: {SgAsmExpression* imm = getImmJb(); insn = MAKE_INSN1(Jcxz, jcxz, /* prediction, */ imm); goto done;}
      case 0xE4: {SgAsmExpression* imm = getImmByte(); insn = MAKE_INSN2(In, in, makeRegister(0, rmLegacyByte), imm); goto done;}
      case 0xE5: {SgAsmExpression* imm = getImmByte(); insn = MAKE_INSN2(In, in, makeRegisterEffective(0), imm); goto done;}
      case 0xE6: {SgAsmExpression* imm = getImmByte(); insn = MAKE_INSN2(Out, out, imm, makeRegister(0, rmLegacyByte)); goto done;}
      case 0xE7: {SgAsmExpression* imm = getImmByte(); insn = MAKE_INSN2(Out, out, imm, makeRegisterEffective(0)); goto done;}
      case 0xE8: {SgAsmExpression* imm = getImmJz(); insn = MAKE_INSN1(Call, call, imm); goto done;}
      case 0xE9: {SgAsmExpression* imm = getImmJz(); insn = MAKE_INSN1(Jmp, jmp, imm); goto done;}
      case 0xEA: {not64(); SgAsmExpression* addr = getImmForAddr(); SgAsmExpression* seg = getImmWord(); insn = MAKE_INSN2(FarJmp, farJmp, seg, addr); goto done;}
      case 0xEB: {SgAsmExpression* imm = getImmJb(); insn = MAKE_INSN1(Jmp, jmp, imm); goto done;}
      case 0xEC: {insn = MAKE_INSN2(In, in, makeRegister(0, rmLegacyByte), makeRegister(2, rmWord)); goto done;}
      case 0xED: {insn = MAKE_INSN2(In, in, makeRegisterEffective(0), makeRegister(2, rmWord)); goto done;}
      case 0xEE: {insn = MAKE_INSN2(Out, out, makeRegister(2, rmWord), makeRegister(0, rmLegacyByte)); goto done;}
      case 0xEF: {insn = MAKE_INSN2(Out, out, makeRegister(2, rmWord), makeRegisterEffective(0)); goto done;}
      case 0xF0: {lock = true; insn = disassemble(); goto done;}
      case 0xF1: {insn = MAKE_INSN0(Int1, int1); goto done;}
      case 0xF2: {repeatPrefix = rpRepne; insn = disassemble(); goto done;}
      case 0xF3: {repeatPrefix = rpRepe; insn = disassemble(); goto done;}
      case 0xF4: {insn = MAKE_INSN0(Hlt, hlt); goto done;}
      case 0xF5: {insn = MAKE_INSN0(Cmc, cmc); goto done;}
      case 0xF6: {getModRegRM(rmReturnNull, rmLegacyByte, SgAsmTypeByte::createType()); SgAsmExpression* immMaybe = NULL; if (regField <= 1) {SgAsmExpression* imm = getImmByteAsIv(); immMaybe = imm;}; insn = decodeGroup3(immMaybe); goto done;}
      case 0xF7: {getModRegRM(rmReturnNull, effectiveOperandMode(), effectiveOperandType()); SgAsmExpression* immMaybe = NULL; if (regField <= 1) {SgAsmExpression* imm = getImmIzAsIv(); immMaybe = imm;}; insn = decodeGroup3(immMaybe); goto done;}
      case 0xF8: {insn = MAKE_INSN0(Clc, clc); goto done;}
      case 0xF9: {insn = MAKE_INSN0(Stc, stc); goto done;}
      case 0xFA: {insn = MAKE_INSN0(Cli, cli); goto done;}
      case 0xFB: {insn = MAKE_INSN0(Sti, sti); goto done;}
      case 0xFC: {insn = MAKE_INSN0(Cld, cld); goto done;}
      case 0xFD: {insn = MAKE_INSN0(Std, std); goto done;}
      case 0xFE: {getModRegRM(rmReturnNull, rmLegacyByte, SgAsmTypeByte::createType()); insn = decodeGroup4(); goto done;}
      case 0xFF: {
        getModRegRM(rmReturnNull, rmReturnNull, NULL);
        if (regField >= 2 && regField <= 6) {sizeMustBe64Bit = true;}
        fillInModRM(effectiveOperandMode(), effectiveOperandType());
        insn = decodeGroup5(); goto done;
      }
      default: ROSE_ASSERT (!"Should not get here");
    }
done:
    if (insn && knownSuccessorsReturn) {
      knownSuccessorsReturn->push_back(p.ip + insn->get_raw_bytes().size());
    }
    return insn;
  }

  SgAsmx86Instruction* SingleInstructionDisassembler::decodeX87InstructionD8() {
    getModRegRM(rmReturnNull, rmST, SgAsmTypeSingleFloat::createType());
    if (isSgAsmMemoryReferenceExpression(modrm)) {
      isSgAsmMemoryReferenceExpression(modrm)->set_type(SgAsmTypeSingleFloat::createType());
    }
    if (modregrmByte < 0xC0) { // Using memory
      switch (regField) {
        case 0: return MAKE_INSN1(Fadd, fadd, modrm);
        case 1: return MAKE_INSN1(Fmul, fmul, modrm);
        case 2: return MAKE_INSN1(Fcom, fcom, modrm);
        case 3: return MAKE_INSN1(Fcomp, fcomp, modrm);
        case 4: return MAKE_INSN1(Fsub, fsub, modrm);
        case 5: return MAKE_INSN1(Fsubr, fsubr, modrm);
        case 6: return MAKE_INSN1(Fdiv, fdiv, modrm);
        case 7: return MAKE_INSN1(Fdivr, fdivr, modrm);
        default: ROSE_ASSERT (false);
      }
    } else { // Two-operand register forms
      switch (regField) {
        case 0: return MAKE_INSN2(Fadd, fadd,   makeRegister(0, rmST), modrm);
        case 1: return MAKE_INSN2(Fmul, fmul,   makeRegister(0, rmST), modrm);
        case 2: return MAKE_INSN2(Fcom, fcom,   makeRegister(0, rmST), modrm);
        case 3: return MAKE_INSN2(Fcomp, fcomp, makeRegister(0, rmST), modrm);
        case 4: return MAKE_INSN2(Fsub, fsub,   makeRegister(0, rmST), modrm);
        case 5: return MAKE_INSN2(Fsubr, fsubr, makeRegister(0, rmST), modrm);
        case 6: return MAKE_INSN2(Fdiv, fdiv,   makeRegister(0, rmST), modrm);
        case 7: return MAKE_INSN2(Fdivr, fdivr, makeRegister(0, rmST), modrm);
        default: ROSE_ASSERT (false);
      }
    }
  }

  SgAsmx86Instruction* SingleInstructionDisassembler::decodeX87InstructionD9() {
    getModRegRM(rmReturnNull, rmReturnNull, NULL);
    if (modeField < 3) {
      SgAsmMemoryReferenceExpression* mr = isSgAsmMemoryReferenceExpression(modrm);
      ROSE_ASSERT (mr);
      switch (regField) {
        case 0: mr->set_type(SgAsmTypeSingleFloat::createType()); return MAKE_INSN1(Fld, fld, modrm);
        case 1: throw BadInstruction();
        case 2: mr->set_type(SgAsmTypeSingleFloat::createType()); return MAKE_INSN1(Fst, fst, modrm);
        case 3: mr->set_type(SgAsmTypeSingleFloat::createType()); return MAKE_INSN1(Fstp, fstp, modrm);
        case 4: mr->set_type(SgAsmTypeByte::createType()); return MAKE_INSN1(Fldenv, fldenv, modrm);
        case 5: mr->set_type(SgAsmTypeWord::createType()); return MAKE_INSN1(Fldcw, fldcw, modrm);
        case 6: mr->set_type(SgAsmTypeByte::createType()); return MAKE_INSN1(Fnstenv, fnstenv, modrm);
        case 7: mr->set_type(SgAsmTypeWord::createType()); return MAKE_INSN1(Fnstcw, fnstcw, modrm);
        default: ROSE_ASSERT (false);
      }
    } else if (regField == 0 || regField == 1) { // FLD and FXCH on registers
      modrm = makeModrmRegister(rmST);
      switch (regField) {
        case 0: return MAKE_INSN2(Fld, fld, makeRegister(0, rmST), modrm);
        case 1: return MAKE_INSN2(Fxch, fxch, makeRegister(0, rmST), modrm);
        default: ROSE_ASSERT (false);
      }
    } else {
      switch (modregrmByte) {
        case 0xD0: return MAKE_INSN0(Fnop, fnop);
        case 0xE0: return MAKE_INSN0(Fchs, fchs);
        case 0xE1: return MAKE_INSN0(Fabs, fabs);
        case 0xE4: return MAKE_INSN0(Ftst, ftst);
        case 0xE5: return MAKE_INSN0(Fxam, fxam);
        case 0xE8: return MAKE_INSN0(Fld1, fld1);
        case 0xE9: return MAKE_INSN0(Fldl2t, fldl2t);
        case 0xEA: return MAKE_INSN0(Fldl2e, fldl2e);
        case 0xEB: return MAKE_INSN0(Fldpi, fldpi);
        case 0xEC: return MAKE_INSN0(Fldlg2, fldlg2);
        case 0xED: return MAKE_INSN0(Fldln2, fldln2);
        case 0xEE: return MAKE_INSN0(Fldz, fldz);
        case 0xF0: return MAKE_INSN0(F2xm1  , f2xm1  );
        case 0xF1: return MAKE_INSN0(Fyl2x  , fyl2x  );
        case 0xF2: return MAKE_INSN0(Fptan  , fptan  );
        case 0xF3: return MAKE_INSN0(Fpatan , fpatan );
        case 0xF4: return MAKE_INSN0(Fxtract, fxtract);
        case 0xF5: return MAKE_INSN0(Fprem1 , fprem1 );
        case 0xF6: return MAKE_INSN0(Fdecstp, fdecstp);
        case 0xF7: return MAKE_INSN0(Fincstp, fincstp);
        case 0xF8: return MAKE_INSN0(Fprem  , fprem  );
        case 0xF9: return MAKE_INSN0(Fyl2xp1, fyl2xp1);
        case 0xFA: return MAKE_INSN0(Fsqrt  , fsqrt  );
        case 0xFB: return MAKE_INSN0(Fsincos, fsincos);
        case 0xFC: return MAKE_INSN0(Frndint, frndint);
        case 0xFD: return MAKE_INSN0(Fscale , fscale );
        case 0xFE: return MAKE_INSN0(Fsin   , fsin   );
        case 0xFF: return MAKE_INSN0(Fcos   , fcos   );
        default: throw BadInstruction();
      }
    }
  }

  SgAsmx86Instruction* SingleInstructionDisassembler::decodeX87InstructionDA() {
    getModRegRM(rmReturnNull, rmReturnNull, SgAsmTypeDoubleWord::createType());
    if (modeField < 3) {
      switch (regField) {
        case 0: return MAKE_INSN1(Fiadd, fiadd, modrm);
        case 1: return MAKE_INSN1(Fimul, fimul, modrm);
        case 2: return MAKE_INSN1(Ficom, ficom, modrm);
        case 3: return MAKE_INSN1(Ficomp, ficomp, modrm);
        case 4: return MAKE_INSN1(Fisub, fisub, modrm);
        case 5: return MAKE_INSN1(Fisubr, fisubr, modrm);
        case 6: return MAKE_INSN1(Fidiv, fidiv, modrm);
        case 7: return MAKE_INSN1(Fidivr, fidivr, modrm);
        default: ROSE_ASSERT (false);
      }
    } else if (regField < 4) { // FCMOV{B,E,BE,U}
      modrm = makeModrmRegister(rmST);
      switch (regField) {
        case 0: return MAKE_INSN2(Fcmovb, fcmovb, makeRegister(0, rmST), modrm);
        case 1: return MAKE_INSN2(Fcmove, fcmove, makeRegister(0, rmST), modrm);
        case 2: return MAKE_INSN2(Fcmovbe, fcmovbe, makeRegister(0, rmST), modrm);
        case 3: return MAKE_INSN2(Fcmovu, fcmovu, makeRegister(0, rmST), modrm);
        default: ROSE_ASSERT (false);
      }
    } else {
      switch (modregrmByte) {
        case 0xE9: return MAKE_INSN0(Fucompp, fucompp);
        default: throw BadInstruction();
      }
    }
  }

  SgAsmx86Instruction* SingleInstructionDisassembler::decodeX87InstructionDB() {
    getModRegRM(rmReturnNull, rmReturnNull, NULL);
    if (modeField < 3) {
      SgAsmMemoryReferenceExpression* mr = isSgAsmMemoryReferenceExpression(modrm);
      ROSE_ASSERT (mr);
      if (regField <= 3) {
        mr->set_type(SgAsmTypeDoubleWord::createType());
      } else {
        mr->set_type(SgAsmType80bitFloat::createType());
      }
      switch (regField) {
        case 0: return MAKE_INSN1(Fild, fild, modrm);
        case 1: return MAKE_INSN1(Fisttp, fisttp, modrm);
        case 2: return MAKE_INSN1(Fist, fist, modrm);
        case 3: return MAKE_INSN1(Fistp, fistp, modrm);
        case 4: throw BadInstruction();
        case 5: return MAKE_INSN1(Fld, fld, modrm);
        case 6: throw BadInstruction();
        case 7: return MAKE_INSN1(Fstp, fstp, modrm);
        default: ROSE_ASSERT (false);
      }
    } else if (regField <= 3 || regField == 5 || regField == 6) { // FCMOV{NB,NE,NBE,NU}, FUCOMI, FCOMI
      modrm = makeModrmRegister(rmST);
      switch (regField) {
        case 0: return MAKE_INSN2(Fcmovnb, fcmovnb, makeRegister(0, rmST), modrm);
        case 1: return MAKE_INSN2(Fcmovne, fcmovne, makeRegister(0, rmST), modrm);
        case 2: return MAKE_INSN2(Fcmovnbe, fcmovnbe, makeRegister(0, rmST), modrm);
        case 3: return MAKE_INSN2(Fcmovnu, fcmovnu, makeRegister(0, rmST), modrm);
        case 5: return MAKE_INSN2(Fucomi, fucomi, makeRegister(0, rmST), modrm);
        case 6: return MAKE_INSN2(Fcomi, fcomi, makeRegister(0, rmST), modrm);
        default: ROSE_ASSERT (false);
      }
    } else {
      switch (modregrmByte) {
        case 0xE2: return MAKE_INSN0(Fnclex, fnclex);
        case 0xE3: return MAKE_INSN0(Fninit, fninit);
        default: throw BadInstruction();
      }
    }
  }

  SgAsmx86Instruction* SingleInstructionDisassembler::decodeX87InstructionDC() {
    getModRegRM(rmReturnNull, rmST, SgAsmTypeDoubleFloat::createType());
    if (modeField < 3) { // Using memory
      switch (regField & 7) {
        case 0: return MAKE_INSN1(Fadd, fadd, modrm);
        case 1: return MAKE_INSN1(Fmul, fmul, modrm);
        case 2: return MAKE_INSN1(Fcom, fcom, modrm);
        case 3: return MAKE_INSN1(Fcomp, fcomp, modrm);
        case 4: return MAKE_INSN1(Fsub, fsub, modrm);
        case 5: return MAKE_INSN1(Fsubr, fsubr, modrm);
        case 6: return MAKE_INSN1(Fdiv, fdiv, modrm);
        case 7: return MAKE_INSN1(Fdivr, fdivr, modrm);
        default: ROSE_ASSERT (false);
      }
    } else { // Two-operand register forms
      switch (regField & 7) {
        case 0: return MAKE_INSN2(Fadd, fadd,   modrm, makeRegister(0, rmST));
        case 1: return MAKE_INSN2(Fmul, fmul,   modrm, makeRegister(0, rmST));
        case 2: throw BadInstruction();
        case 3: throw BadInstruction();
        case 4: return MAKE_INSN2(Fsub, fsub,   modrm, makeRegister(0, rmST));
        case 5: return MAKE_INSN2(Fsubr, fsubr, modrm, makeRegister(0, rmST));
        case 6: return MAKE_INSN2(Fdiv, fdiv,   modrm, makeRegister(0, rmST));
        case 7: return MAKE_INSN2(Fdivr, fdivr, modrm, makeRegister(0, rmST));
        default: ROSE_ASSERT (false);
      }
    }
  }

  SgAsmx86Instruction* SingleInstructionDisassembler::decodeX87InstructionDD() {
    getModRegRM(rmReturnNull, rmST, NULL);
    if (modeField < 3) { // Using memory
      SgAsmMemoryReferenceExpression* mr = isSgAsmMemoryReferenceExpression(modrm);
      ROSE_ASSERT (mr);
      switch (regField) {
        case 0: mr->set_type(SgAsmTypeDoubleFloat::createType()); return MAKE_INSN1(Fld, fld, modrm);
        case 1: mr->set_type(SgAsmTypeQuadWord::createType()); return MAKE_INSN1(Fisttp, fisttp, modrm);
        case 2: mr->set_type(SgAsmTypeDoubleFloat::createType()); return MAKE_INSN1(Fst, fst, modrm);
        case 3: mr->set_type(SgAsmTypeDoubleFloat::createType()); return MAKE_INSN1(Fstp, fstp, modrm);
        case 4: mr->set_type(SgAsmTypeByte::createType()); return MAKE_INSN1(Frstor, frstor, modrm);
        case 5: throw BadInstruction();
        case 6: mr->set_type(SgAsmTypeByte::createType()); return MAKE_INSN1(Fnsave, fnsave, modrm);
        case 7: mr->set_type(SgAsmTypeWord::createType()); return MAKE_INSN1(Fnstsw, fnstsw, modrm);
        default: ROSE_ASSERT (false);
      }
    } else { // Register forms
      switch (regField) {
        case 0: return MAKE_INSN1(Ffree, ffree, modrm);
        case 1: throw BadInstruction();
        case 2: return MAKE_INSN1(Fst, fst, modrm);
        case 3: return MAKE_INSN1(Fstp, fstp, modrm);
        case 4: return MAKE_INSN2(Fucom, fucom, modrm, makeRegister(0, rmST));
        case 5: return MAKE_INSN2(Fucomp, fucomp, modrm, makeRegister(0, rmST));
        case 6: throw BadInstruction();
        case 7: throw BadInstruction();
        default: ROSE_ASSERT (false);
      }
    }
  }

  SgAsmx86Instruction* SingleInstructionDisassembler::decodeX87InstructionDE() {
    getModRegRM(rmReturnNull, rmST, SgAsmTypeWord::createType());
    if (modeField < 3) { // Using memory
      switch (regField & 7) {
        case 0: return MAKE_INSN1(Fiadd, fiadd, modrm);
        case 1: return MAKE_INSN1(Fimul, fimul, modrm);
        case 2: return MAKE_INSN1(Ficom, ficom, modrm);
        case 3: return MAKE_INSN1(Ficomp, ficomp, modrm);
        case 4: return MAKE_INSN1(Fisub, fisub, modrm);
        case 5: return MAKE_INSN1(Fisubr, fisubr, modrm);
        case 6: return MAKE_INSN1(Fidiv, fidiv, modrm);
        case 7: return MAKE_INSN1(Fidivr, fidivr, modrm);
        default: ROSE_ASSERT (false);
      }
    } else {
      switch (regField & 7) {
        case 0: return MAKE_INSN2(Faddp, faddp, modrm, makeRegister(0, rmST));
        case 1: return MAKE_INSN2(Fmulp, fmulp, modrm, makeRegister(0, rmST));
        case 2: throw BadInstruction();
        case 3: {
          switch (modregrmByte) {
            case 0xD9: delete modrm; delete reg; return MAKE_INSN0(Fcompp, fcompp);
            default: throw BadInstruction();
          }
        }
        case 4: return MAKE_INSN2(Fsubrp, fsubrp, modrm, makeRegister(0, rmST));
        case 5: return MAKE_INSN2(Fsubp, fsubp, modrm, makeRegister(0, rmST));
        case 6: return MAKE_INSN2(Fdivrp, fdivrp, modrm, makeRegister(0, rmST));
        case 7: return MAKE_INSN2(Fdivp, fdivp, modrm, makeRegister(0, rmST));
        default: ROSE_ASSERT (false);
      }
    }
  }

  SgAsmx86Instruction* SingleInstructionDisassembler::decodeX87InstructionDF() {
    getModRegRM(rmReturnNull, rmReturnNull, NULL);
    if (modeField < 3) { // Using memory
      SgAsmMemoryReferenceExpression* mr = isSgAsmMemoryReferenceExpression(modrm);
      ROSE_ASSERT (mr);
      switch (regField) {
        case 0: mr->set_type(SgAsmTypeWord::createType()); return MAKE_INSN1(Fild, fild, modrm);
        case 1: mr->set_type(SgAsmTypeWord::createType()); return MAKE_INSN1(Fisttp, fisttp, modrm);
        case 2: mr->set_type(SgAsmTypeWord::createType()); return MAKE_INSN1(Fist, fist, modrm);
        case 3: mr->set_type(SgAsmTypeWord::createType()); return MAKE_INSN1(Fistp, fistp, modrm);
        case 4: mr->set_type(SgAsmTypeByte::createType()); return MAKE_INSN1(Fbld, fbld, modrm);
        case 5: mr->set_type(SgAsmTypeQuadWord::createType()); return MAKE_INSN1(Fild, fild, modrm);
        case 6: mr->set_type(SgAsmTypeByte::createType()); return MAKE_INSN1(Fbstp, fbstp, modrm);
        case 7: mr->set_type(SgAsmTypeQuadWord::createType()); return MAKE_INSN1(Fistp, fistp, modrm);
        default: ROSE_ASSERT (false);
      }
    } else {
      modrm = makeModrmRegister(rmST);
      switch (regField) {
        case 0: throw BadInstruction();
        case 1: throw BadInstruction();
        case 2: throw BadInstruction();
        case 3: throw BadInstruction();
        case 4: {
          if (modregrmByte == 0xE0) {
            return MAKE_INSN1(Fnstsw, fnstsw, makeRegister(0, rmWord));
          } else {
            throw BadInstruction();
          }
        }
        case 5: return MAKE_INSN2(Fucomip, fucomip, makeRegister(0, rmST), modrm);
        case 6: return MAKE_INSN2(Fcomip, fcomip, makeRegister(0, rmST), modrm);
        case 7: throw BadInstruction();
        default: ROSE_ASSERT (false);
      }
    }
  }

  SgAsmx86Instruction* SingleInstructionDisassembler::decodeOpcode0F() {
    uint8_t opcode;
    getByte(opcode);
    switch (opcode) {
      case 0x00: {getModRegRM(rmReturnNull, rmWord, SgAsmTypeWord::createType()); return decodeGroup6();}
      case 0x01: return decodeGroup7();
      case 0x02: {getModRegRM(rmWord, rmWord, SgAsmTypeWord::createType()); return MAKE_INSN2(Lar, lar, reg, modrm);}
      case 0x03: {getModRegRM(rmWord, rmWord, SgAsmTypeWord::createType()); return MAKE_INSN2(Lsl, lsl, reg, modrm);}
      case 0x04: throw BadInstruction();
      case 0x05: return MAKE_INSN0(Syscall, syscall);
      case 0x06: return MAKE_INSN0(Clts, clts);
      case 0x07: return MAKE_INSN0(Sysret, sysret);
      case 0x08: return MAKE_INSN0(Invd, invd);
      case 0x09: return MAKE_INSN0(Wbinvd, wbinvd);
      case 0x0A: throw BadInstruction();
      case 0x0B: return MAKE_INSN0(Ud2, ud2);
      case 0x0C: throw BadInstruction();
      case 0x0D: return decodeGroupP();
      case 0x0E: return MAKE_INSN0(Femms, femms);
      case 0x0F: ROSE_ASSERT (!"3DNow! not supported 0F0F");
      case 0x10: ROSE_ASSERT (!"SIMD not supported 0F10");
      case 0x11: ROSE_ASSERT (!"SIMD not supported 0F10");
      case 0x12: ROSE_ASSERT (!"SIMD not supported 0F10");
      case 0x13: ROSE_ASSERT (!"SIMD not supported 0F10");
      case 0x14: ROSE_ASSERT (!"SIMD not supported 0F10");
      case 0x15: ROSE_ASSERT (!"SIMD not supported 0F10");
      case 0x16: ROSE_ASSERT (!"SIMD not supported 0F10");
      case 0x17: ROSE_ASSERT (!"SIMD not supported 0F10");
      case 0x18: getModRegRM(rmLegacyByte, rmLegacyByte, SgAsmTypeByte::createType()); return decodeGroup16();
      case 0x19: throw BadInstruction();
      case 0x1A: throw BadInstruction();
      case 0x1B: throw BadInstruction();
      case 0x1C: throw BadInstruction();
      case 0x1D: throw BadInstruction();
      case 0x1E: throw BadInstruction();
      case 0x1F: throw BadInstruction();
      case 0x20: ROSE_ASSERT (!"C/D regs not supported 0F20");
      case 0x21: ROSE_ASSERT (!"C/D regs not supported 0F21");
      case 0x22: ROSE_ASSERT (!"C/D regs not supported 0F22");
      case 0x23: ROSE_ASSERT (!"C/D regs not supported 0F23");
      case 0x24: throw BadInstruction();
      case 0x25: throw BadInstruction();
      case 0x26: throw BadInstruction();
      case 0x27: throw BadInstruction();
      case 0x28: ROSE_ASSERT (!"SIMD not supported 0F28");
      case 0x29: ROSE_ASSERT (!"SIMD not supported 0F28");
      case 0x2A: ROSE_ASSERT (!"SIMD not supported 0F28");
      case 0x2B: ROSE_ASSERT (!"SIMD not supported 0F28");
      case 0x2C: ROSE_ASSERT (!"SIMD not supported 0F28");
      case 0x2D: ROSE_ASSERT (!"SIMD not supported 0F28");
      case 0x2E: ROSE_ASSERT (!"SIMD not supported 0F28");
      case 0x2F: ROSE_ASSERT (!"SIMD not supported 0F28");
      case 0x30: return MAKE_INSN0(Wrmsr, wrmsr);
      case 0x31: return MAKE_INSN0(Rdtsc, rdtsc);
      case 0x32: return MAKE_INSN0(Rdmsr, rdmsr);
      case 0x33: return MAKE_INSN0(Rdpmc, rdpmc);
      case 0x34: not64(); return MAKE_INSN0(Sysenter, sysenter);
      case 0x35: not64(); return MAKE_INSN0(Sysexit, sysexit);
      case 0x36: throw BadInstruction();
      case 0x37: throw BadInstruction();
      case 0x38: throw BadInstruction();
      case 0x39: throw BadInstruction();
      case 0x3A: throw BadInstruction();
      case 0x3B: throw BadInstruction();
      case 0x3C: throw BadInstruction();
      case 0x3D: throw BadInstruction();
      case 0x3E: throw BadInstruction();
      case 0x3F: throw BadInstruction();
      case 0x40: getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); return MAKE_INSN2(Cmovo, cmovo, reg, modrm);
      case 0x41: getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); return MAKE_INSN2(Cmovno, cmovno, reg, modrm);
      case 0x42: getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); return MAKE_INSN2(Cmovb, cmovb, reg, modrm);
      case 0x43: getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); return MAKE_INSN2(Cmovae, cmovae, reg, modrm);
      case 0x44: getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); return MAKE_INSN2(Cmove, cmove, reg, modrm);
      case 0x45: getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); return MAKE_INSN2(Cmovne, cmovne, reg, modrm);
      case 0x46: getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); return MAKE_INSN2(Cmovbe, cmovbe, reg, modrm);
      case 0x47: getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); return MAKE_INSN2(Cmova, cmova, reg, modrm);
      case 0x48: getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); return MAKE_INSN2(Cmovs, cmovs, reg, modrm);
      case 0x49: getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); return MAKE_INSN2(Cmovns, cmovns, reg, modrm);
      case 0x4A: getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); return MAKE_INSN2(Cmovpe, cmovpe, reg, modrm);
      case 0x4B: getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); return MAKE_INSN2(Cmovpo, cmovpo, reg, modrm);
      case 0x4C: getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); return MAKE_INSN2(Cmovl, cmovl, reg, modrm);
      case 0x4D: getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); return MAKE_INSN2(Cmovge, cmovge, reg, modrm);
      case 0x4E: getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); return MAKE_INSN2(Cmovle, cmovle, reg, modrm);
      case 0x4F: getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); return MAKE_INSN2(Cmovg, cmovg, reg, modrm);
      case 0x50: ROSE_ASSERT (!"SIMD not supported 0F50");
      case 0x51: ROSE_ASSERT (!"SIMD not supported 0F51");
      case 0x52: ROSE_ASSERT (!"SIMD not supported 0F52");
      case 0x53: ROSE_ASSERT (!"SIMD not supported 0F53");
      case 0x54: ROSE_ASSERT (!"SIMD not supported 0F54");
      case 0x55: ROSE_ASSERT (!"SIMD not supported 0F55");
      case 0x56: ROSE_ASSERT (!"SIMD not supported 0F56");
      case 0x57: ROSE_ASSERT (!"SIMD not supported 0F57");
      case 0x58: ROSE_ASSERT (!"SIMD not supported 0F58");
      case 0x59: ROSE_ASSERT (!"SIMD not supported 0F59");
      case 0x5A: ROSE_ASSERT (!"SIMD not supported 0F5A");
      case 0x5B: ROSE_ASSERT (!"SIMD not supported 0F5B");
      case 0x5C: ROSE_ASSERT (!"SIMD not supported 0F5C");
      case 0x5D: ROSE_ASSERT (!"SIMD not supported 0F5D");
      case 0x5E: ROSE_ASSERT (!"SIMD not supported 0F5E");
      case 0x5F: ROSE_ASSERT (!"SIMD not supported 0F5F");
      case 0x60: ROSE_ASSERT (!"SIMD not supported 0F60");
      case 0x61: ROSE_ASSERT (!"SIMD not supported 0F61");
      case 0x62: ROSE_ASSERT (!"SIMD not supported 0F62");
      case 0x63: ROSE_ASSERT (!"SIMD not supported 0F63");
      case 0x64: ROSE_ASSERT (!"SIMD not supported 0F64");
      case 0x65: ROSE_ASSERT (!"SIMD not supported 0F65");
      case 0x66: ROSE_ASSERT (!"SIMD not supported 0F66");
      case 0x67: ROSE_ASSERT (!"SIMD not supported 0F67");
      case 0x68: ROSE_ASSERT (!"SIMD not supported 0F68");
      case 0x69: ROSE_ASSERT (!"SIMD not supported 0F69");
      case 0x6A: ROSE_ASSERT (!"SIMD not supported 0F6A");
      case 0x6B: ROSE_ASSERT (!"SIMD not supported 0F6B");
      case 0x6C: ROSE_ASSERT (!"SIMD not supported 0F6C");
      case 0x6D: ROSE_ASSERT (!"SIMD not supported 0F6D");
      case 0x6E: ROSE_ASSERT (!"SIMD not supported 0F6E");
      case 0x6F: ROSE_ASSERT (!"SIMD not supported 0F6F");
      case 0x70: ROSE_ASSERT (!"SIMD not supported 0F70");
      case 0x71: ROSE_ASSERT (!"SIMD not supported 0F71");
      case 0x72: ROSE_ASSERT (!"SIMD not supported 0F72");
      case 0x73: ROSE_ASSERT (!"SIMD not supported 0F73");
      case 0x74: ROSE_ASSERT (!"SIMD not supported 0F74");
      case 0x75: ROSE_ASSERT (!"SIMD not supported 0F75");
      case 0x76: ROSE_ASSERT (!"SIMD not supported 0F76");
      case 0x77: ROSE_ASSERT (!"SIMD not supported 0F77");
      case 0x78: ROSE_ASSERT (!"SIMD not supported 0F78");
      case 0x79: ROSE_ASSERT (!"SIMD not supported 0F79");
      case 0x7A: ROSE_ASSERT (!"SIMD not supported 0F7A");
      case 0x7B: ROSE_ASSERT (!"SIMD not supported 0F7B");
      case 0x7C: ROSE_ASSERT (!"SIMD not supported 0F7C");
      case 0x7D: ROSE_ASSERT (!"SIMD not supported 0F7D");
      case 0x7E: ROSE_ASSERT (!"SIMD not supported 0F7E");
      case 0x7F: ROSE_ASSERT (!"SIMD not supported 0F7F");
      case 0x80: {SgAsmExpression* imm = getImmJz(); return MAKE_INSN1(Jo , jo, /* prediction, */ imm);}
      case 0x81: {SgAsmExpression* imm = getImmJz(); return MAKE_INSN1(Jno, jno, /* prediction, */ imm);}
      case 0x82: {SgAsmExpression* imm = getImmJz(); return MAKE_INSN1(Jb,  jb, /* prediction, */ imm);}
      case 0x83: {SgAsmExpression* imm = getImmJz(); return MAKE_INSN1(Jae, jae, /* prediction, */ imm);}
      case 0x84: {SgAsmExpression* imm = getImmJz(); return MAKE_INSN1(Je,  je, /* prediction, */ imm);}
      case 0x85: {SgAsmExpression* imm = getImmJz(); return MAKE_INSN1(Jne, jne, /* prediction, */ imm);}
      case 0x86: {SgAsmExpression* imm = getImmJz(); return MAKE_INSN1(Jbe, jbe, /* prediction, */ imm);}
      case 0x87: {SgAsmExpression* imm = getImmJz(); return MAKE_INSN1(Ja,  ja, /* prediction, */ imm);}
      case 0x88: {SgAsmExpression* imm = getImmJz(); return MAKE_INSN1(Js , js, /* prediction, */ imm);}
      case 0x89: {SgAsmExpression* imm = getImmJz(); return MAKE_INSN1(Jns, jns, /* prediction, */ imm);}
      case 0x8A: {SgAsmExpression* imm = getImmJz(); return MAKE_INSN1(Jpe, jpe, /* prediction, */ imm);}
      case 0x8B: {SgAsmExpression* imm = getImmJz(); return MAKE_INSN1(Jpo, jpo, /* prediction, */ imm);}
      case 0x8C: {SgAsmExpression* imm = getImmJz(); return MAKE_INSN1(Jl,  jl, /* prediction, */ imm);}
      case 0x8D: {SgAsmExpression* imm = getImmJz(); return MAKE_INSN1(Jge, jge, /* prediction, */ imm);}
      case 0x8E: {SgAsmExpression* imm = getImmJz(); return MAKE_INSN1(Jle, jle, /* prediction, */ imm);}
      case 0x8F: {SgAsmExpression* imm = getImmJz(); return MAKE_INSN1(Jg,  jg, /* prediction, */ imm);}
      case 0x90: getModRegRM(rmLegacyByte, rmLegacyByte, SgAsmTypeByte::createType()); return MAKE_INSN1(Seto,  seto, modrm);
      case 0x91: getModRegRM(rmLegacyByte, rmLegacyByte, SgAsmTypeByte::createType()); return MAKE_INSN1(Setno, setno, modrm);
      case 0x92: getModRegRM(rmLegacyByte, rmLegacyByte, SgAsmTypeByte::createType()); return MAKE_INSN1(Setb,  setb, modrm);
      case 0x93: getModRegRM(rmLegacyByte, rmLegacyByte, SgAsmTypeByte::createType()); return MAKE_INSN1(Setae, setae, modrm);
      case 0x94: getModRegRM(rmLegacyByte, rmLegacyByte, SgAsmTypeByte::createType()); return MAKE_INSN1(Sete,  sete, modrm);
      case 0x95: getModRegRM(rmLegacyByte, rmLegacyByte, SgAsmTypeByte::createType()); return MAKE_INSN1(Setne, setne, modrm);
      case 0x96: getModRegRM(rmLegacyByte, rmLegacyByte, SgAsmTypeByte::createType()); return MAKE_INSN1(Setbe, setbe, modrm);
      case 0x97: getModRegRM(rmLegacyByte, rmLegacyByte, SgAsmTypeByte::createType()); return MAKE_INSN1(Seta,  seta, modrm);
      case 0x98: getModRegRM(rmLegacyByte, rmLegacyByte, SgAsmTypeByte::createType()); return MAKE_INSN1(Sets,  sets, modrm);
      case 0x99: getModRegRM(rmLegacyByte, rmLegacyByte, SgAsmTypeByte::createType()); return MAKE_INSN1(Setns, setns, modrm);
      case 0x9A: getModRegRM(rmLegacyByte, rmLegacyByte, SgAsmTypeByte::createType()); return MAKE_INSN1(Setpe, setpe, modrm);
      case 0x9B: getModRegRM(rmLegacyByte, rmLegacyByte, SgAsmTypeByte::createType()); return MAKE_INSN1(Setpo, setpo, modrm);
      case 0x9C: getModRegRM(rmLegacyByte, rmLegacyByte, SgAsmTypeByte::createType()); return MAKE_INSN1(Setl,  setl, modrm);
      case 0x9D: getModRegRM(rmLegacyByte, rmLegacyByte, SgAsmTypeByte::createType()); return MAKE_INSN1(Setge, setge, modrm);
      case 0x9E: getModRegRM(rmLegacyByte, rmLegacyByte, SgAsmTypeByte::createType()); return MAKE_INSN1(Setle, setle, modrm);
      case 0x9F: getModRegRM(rmLegacyByte, rmLegacyByte, SgAsmTypeByte::createType()); return MAKE_INSN1(Setg,  setg, modrm);
      case 0xA0: return MAKE_INSN1(Push, push, makeRegister(4, rmSegment));
      case 0xA1: return MAKE_INSN1(Pop, pop, makeRegister(4, rmSegment));
      case 0xA2: return MAKE_INSN0(Cpuid, cpuid);
      case 0xA3: getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); return MAKE_INSN2(Bt, bt, modrm, reg);
      case 0xA4: getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); return MAKE_INSN3(Shld, shld, modrm, reg, getImmByte());
      case 0xA5: getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); return MAKE_INSN3(Shld, shld, modrm, reg, makeRegister(2, rmLegacyByte));
      case 0xA6: throw BadInstruction();
      case 0xA7: throw BadInstruction();
      case 0xA8: return MAKE_INSN1(Push, push, makeRegister(5, rmSegment));
      case 0xA9: return MAKE_INSN1(Pop, pop, makeRegister(5, rmSegment));
      case 0xAA: return MAKE_INSN0(Rsm, rsm);
      case 0xAB: getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); return MAKE_INSN2(Btr, btr, modrm, reg);
      case 0xAC: getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); return MAKE_INSN3(Shrd, shrd, modrm, reg, getImmByte());
      case 0xAD: getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); return MAKE_INSN3(Shrd, shrd, modrm, reg, makeRegister(2, rmLegacyByte));
      case 0xAE: return decodeGroup15();
      case 0xAF: getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); return MAKE_INSN2(Imul, imul, reg, modrm);
      case 0xB0: getModRegRM(rmLegacyByte, rmLegacyByte, SgAsmTypeByte::createType()); return MAKE_INSN2(Cmpxchg, cmpxchg, modrm, reg);
      case 0xB1: getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); return MAKE_INSN2(Cmpxchg, cmpxchg, modrm, reg);
      case 0xB2: getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); requireMemory(); return MAKE_INSN2(Lss, lss, reg, modrm);
      case 0xB3: getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); return MAKE_INSN2(Bts, bts, modrm, reg);
      case 0xB4: getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); requireMemory(); return MAKE_INSN2(Lfs, lfs, reg, modrm);
      case 0xB5: getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); requireMemory(); return MAKE_INSN2(Lgs, lgs, reg, modrm);
      case 0xB6: getModRegRM(effectiveOperandMode(), rmLegacyByte, SgAsmTypeByte::createType()); return MAKE_INSN2(Movzx, movzx, reg, modrm);
      case 0xB7: getModRegRM(effectiveOperandMode(), rmWord, SgAsmTypeWord::createType()); return MAKE_INSN2(Movzx, movzx, reg, modrm);
      case 0xB8: if (repeatPrefix == rpRepe) {getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); return MAKE_INSN2(Popcnt, popcnt, reg, modrm);} else throw BadInstruction();
      case 0xB9: throw BadInstruction();
      case 0xBA: getModRegRM(rmReturnNull, effectiveOperandMode(), effectiveOperandType()); return decodeGroup8(getImmByte());
      case 0xBB: getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); return MAKE_INSN2(Btc, btc, modrm, reg);
      case 0xBC: getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); return MAKE_INSN2(Bsf, bsf, reg, modrm);
      case 0xBD: getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); if (repeatPrefix == rpRepe) return MAKE_INSN2(Lzcnt, lzcnt, reg, modrm); else return MAKE_INSN2(Bsr, bsr, reg, modrm);
      case 0xBE: getModRegRM(effectiveOperandMode(), rmLegacyByte, SgAsmTypeByte::createType()); return MAKE_INSN2(Movsx, movsx, reg, modrm);
      case 0xBF: getModRegRM(effectiveOperandMode(), rmWord, SgAsmTypeWord::createType()); return MAKE_INSN2(Movsx, movsx, reg, modrm);
      case 0xC0: getModRegRM(rmLegacyByte, rmLegacyByte, SgAsmTypeByte::createType()); return MAKE_INSN2(Xadd, xadd, modrm, reg);
      case 0xC1: getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); return MAKE_INSN2(Xadd, xadd, modrm, reg);
      case 0xC2: ROSE_ASSERT (!"SIMD not supported 0FC2");
      case 0xC3: ROSE_ASSERT (!"SIMD not supported 0FC3");
      case 0xC4: ROSE_ASSERT (!"SIMD not supported 0FC4");
      case 0xC5: ROSE_ASSERT (!"SIMD not supported 0FC5");
      case 0xC6: ROSE_ASSERT (!"SIMD not supported 0FC6");
      case 0xC7: ROSE_ASSERT (!"SIMD not supported 0FC7");
      case 0xC8: return MAKE_INSN1(Bswap, bswap, makeRegisterEffective(rexB, 0));
      case 0xC9: return MAKE_INSN1(Bswap, bswap, makeRegisterEffective(rexB, 1));
      case 0xCA: return MAKE_INSN1(Bswap, bswap, makeRegisterEffective(rexB, 2));
      case 0xCB: return MAKE_INSN1(Bswap, bswap, makeRegisterEffective(rexB, 3));
      case 0xCC: return MAKE_INSN1(Bswap, bswap, makeRegisterEffective(rexB, 4));
      case 0xCD: return MAKE_INSN1(Bswap, bswap, makeRegisterEffective(rexB, 5));
      case 0xCE: return MAKE_INSN1(Bswap, bswap, makeRegisterEffective(rexB, 6));
      case 0xCF: return MAKE_INSN1(Bswap, bswap, makeRegisterEffective(rexB, 7));
      case 0xD0: ROSE_ASSERT (!"SIMD not supported 0FD0");
      case 0xD1: ROSE_ASSERT (!"SIMD not supported 0FD1");
      case 0xD2: ROSE_ASSERT (!"SIMD not supported 0FD2");
      case 0xD3: ROSE_ASSERT (!"SIMD not supported 0FD3");
      case 0xD4: ROSE_ASSERT (!"SIMD not supported 0FD4");
      case 0xD5: ROSE_ASSERT (!"SIMD not supported 0FD5");
      case 0xD6: ROSE_ASSERT (!"SIMD not supported 0FD6");
      case 0xD7: ROSE_ASSERT (!"SIMD not supported 0FD7");
      case 0xD8: ROSE_ASSERT (!"SIMD not supported 0FD8");
      case 0xD9: ROSE_ASSERT (!"SIMD not supported 0FD9");
      case 0xDA: ROSE_ASSERT (!"SIMD not supported 0FDA");
      case 0xDB: ROSE_ASSERT (!"SIMD not supported 0FDB");
      case 0xDC: ROSE_ASSERT (!"SIMD not supported 0FDC");
      case 0xDD: ROSE_ASSERT (!"SIMD not supported 0FDD");
      case 0xDE: ROSE_ASSERT (!"SIMD not supported 0FDE");
      case 0xDF: ROSE_ASSERT (!"SIMD not supported 0FDF");
      case 0xE0: ROSE_ASSERT (!"SIMD not supported 0FE0");
      case 0xE1: ROSE_ASSERT (!"SIMD not supported 0FE1");
      case 0xE2: ROSE_ASSERT (!"SIMD not supported 0FE2");
      case 0xE3: ROSE_ASSERT (!"SIMD not supported 0FE3");
      case 0xE4: ROSE_ASSERT (!"SIMD not supported 0FE4");
      case 0xE5: ROSE_ASSERT (!"SIMD not supported 0FE5");
      case 0xE6: ROSE_ASSERT (!"SIMD not supported 0FE6");
      case 0xE7: ROSE_ASSERT (!"SIMD not supported 0FE7");
      case 0xE8: ROSE_ASSERT (!"SIMD not supported 0FE8");
      case 0xE9: ROSE_ASSERT (!"SIMD not supported 0FE9");
      case 0xEA: ROSE_ASSERT (!"SIMD not supported 0FEA");
      case 0xEB: ROSE_ASSERT (!"SIMD not supported 0FEB");
      case 0xEC: ROSE_ASSERT (!"SIMD not supported 0FEC");
      case 0xED: ROSE_ASSERT (!"SIMD not supported 0FED");
      case 0xEE: ROSE_ASSERT (!"SIMD not supported 0FEE");
      case 0xEF: ROSE_ASSERT (!"SIMD not supported 0FEF");
      case 0xF0: ROSE_ASSERT (!"SIMD not supported 0FF0");
      case 0xF1: ROSE_ASSERT (!"SIMD not supported 0FF1");
      case 0xF2: ROSE_ASSERT (!"SIMD not supported 0FF2");
      case 0xF3: ROSE_ASSERT (!"SIMD not supported 0FF3");
      case 0xF4: ROSE_ASSERT (!"SIMD not supported 0FF4");
      case 0xF5: ROSE_ASSERT (!"SIMD not supported 0FF5");
      case 0xF6: ROSE_ASSERT (!"SIMD not supported 0FF6");
      case 0xF7: ROSE_ASSERT (!"SIMD not supported 0FF7");
      case 0xF8: ROSE_ASSERT (!"SIMD not supported 0FF8");
      case 0xF9: ROSE_ASSERT (!"SIMD not supported 0FF9");
      case 0xFA: ROSE_ASSERT (!"SIMD not supported 0FFA");
      case 0xFB: ROSE_ASSERT (!"SIMD not supported 0FFB");
      case 0xFC: ROSE_ASSERT (!"SIMD not supported 0FFC");
      case 0xFD: ROSE_ASSERT (!"SIMD not supported 0FFD");
      case 0xFE: ROSE_ASSERT (!"SIMD not supported 0FFE");
      case 0xFF: ROSE_ASSERT (!"SIMD not supported 0FFF");
      default: ROSE_ASSERT (false);
    }
  }

  SgAsmx86Instruction* SingleInstructionDisassembler::decodeGroup1(SgAsmExpression* imm) {
    switch (regField) {
      case 0: return MAKE_INSN2(Add, add, modrm, imm);
      case 1: return MAKE_INSN2(Or , or , modrm, imm);
      case 2: return MAKE_INSN2(Adc, adc, modrm, imm);
      case 3: return MAKE_INSN2(Sbb, sbb, modrm, imm);
      case 4: return MAKE_INSN2(And, and, modrm, imm);
      case 5: return MAKE_INSN2(Sub, sub, modrm, imm);
      case 6: return MAKE_INSN2(Xor, xor, modrm, imm);
      case 7: return MAKE_INSN2(Cmp, cmp, modrm, imm);
      default: ROSE_ASSERT (false);
    }
  }

  SgAsmx86Instruction* SingleInstructionDisassembler::decodeGroup1a() {
    if (regField != 0) throw BadInstruction();
    return MAKE_INSN1(Pop, pop, modrm);
  }

  SgAsmx86Instruction* SingleInstructionDisassembler::decodeGroup2(SgAsmExpression* count) {
    switch (regField) {
      case 0: return MAKE_INSN2(Rol, rol, modrm, count);
      case 1: return MAKE_INSN2(Ror, ror, modrm, count);
      case 2: return MAKE_INSN2(Rcl, rcl, modrm, count);
      case 3: return MAKE_INSN2(Rcr, rcr, modrm, count);
      case 4: return MAKE_INSN2(Shl, shl, modrm, count);
      case 5: return MAKE_INSN2(Shr, shr, modrm, count);
      case 6: return MAKE_INSN2(Shl, shl, modrm, count);
      case 7: return MAKE_INSN2(Sar, sar, modrm, count);
      default: ROSE_ASSERT (false);
    }
  }

  SgAsmx86Instruction* SingleInstructionDisassembler::decodeGroup3(SgAsmExpression* immMaybe) {
    switch (regField) {
      case 0:
      case 1: ROSE_ASSERT (immMaybe); return MAKE_INSN2(Test, test, modrm, immMaybe);
      case 2: return MAKE_INSN1(Not, not, modrm);
      case 3: return MAKE_INSN1(Neg, neg, modrm);
      case 4: return MAKE_INSN1(Mul, mul, modrm);
      case 5: return MAKE_INSN1(Imul, imul, modrm);
      case 6: return MAKE_INSN1(Div, div, modrm);
      case 7: return MAKE_INSN1(Idiv, idiv, modrm);
      default: ROSE_ASSERT (false);
    }
  }

  SgAsmx86Instruction* SingleInstructionDisassembler::decodeGroup4() {
    switch (regField) {
      case 0: return MAKE_INSN1(Inc, inc, modrm);
      case 1: return MAKE_INSN1(Dec, dec, modrm);
      default: throw BadInstruction();
    }
  }

  SgAsmx86Instruction* SingleInstructionDisassembler::decodeGroup5() {
    switch (regField) {
      case 0: return MAKE_INSN1(Inc, inc, modrm);
      case 1: return MAKE_INSN1(Dec, dec, modrm);
      case 2: return MAKE_INSN1(Call, call, modrm);
      case 3: return MAKE_INSN1(FarCall, farCall, modrm);
      case 4: return MAKE_INSN1(Jmp, jmp, modrm);
      case 5: return MAKE_INSN1(FarJmp, farJmp, modrm);
      case 6: return MAKE_INSN1(Push, push, modrm);
      case 7: throw BadInstruction();
      default: ROSE_ASSERT (false);
    }
  }

  SgAsmx86Instruction* SingleInstructionDisassembler::decodeGroup6() {
    switch (regField) {
      case 0: return MAKE_INSN1(Sldt, sldt, modrm); // FIXME adjust register size
      case 1: return MAKE_INSN1(Str, str, modrm); // FIXME adjust register size
      case 2: return MAKE_INSN1(Lldt, lldt, modrm);
      case 3: return MAKE_INSN1(Ltr, ltr, modrm);
      case 4: return MAKE_INSN1(Verr, verr, modrm);
      case 5: return MAKE_INSN1(Verw, verw, modrm);
      case 6: throw new BadInstruction();
      case 7: throw new BadInstruction();
      default: ROSE_ASSERT (false);
    }
  }

  SgAsmx86Instruction* SingleInstructionDisassembler::decodeGroup7() {
    ROSE_ASSERT (!"Group 7 not supported");
  }

  SgAsmx86Instruction* SingleInstructionDisassembler::decodeGroup8(SgAsmExpression* imm) {
    switch (regField) {
      case 0: throw BadInstruction();
      case 1: throw BadInstruction();
      case 2: throw BadInstruction();
      case 3: throw BadInstruction();
      case 4: return MAKE_INSN2(Bt, bt, modrm, imm);
      case 5: return MAKE_INSN2(Bts, bts, modrm, imm);
      case 6: return MAKE_INSN2(Btr, btr, modrm, imm);
      case 7: return MAKE_INSN2(Btc, btc, modrm, imm);
      default: ROSE_ASSERT (false);
    }
  }

  SgAsmx86Instruction* SingleInstructionDisassembler::decodeGroup11(SgAsmExpression* imm) {
    switch (regField) {
      case 0: return MAKE_INSN2(Mov, mov, modrm, imm);
      default: throw BadInstruction();
    }
  }

  SgAsmx86Instruction* SingleInstructionDisassembler::decodeGroup15() {
    ROSE_ASSERT (!"Group 15 not supported");
  }

  SgAsmx86Instruction* SingleInstructionDisassembler::decodeGroup16() {
    ROSE_ASSERT (!"Group 16 not supported");
  }

  SgAsmx86Instruction* SingleInstructionDisassembler::decodeGroupP() {
    ROSE_ASSERT (!"Group P not supported");
  }

  static void addInstruction(SgAsmBlock* b, SgAsmInstruction* insn) {
    b->get_statementList().push_back(insn);
    insn->set_parent(b);
  }

  struct AsmFileWithData {
    SgAsmFile* f;
    vector<uint8_t> data;

    AsmFileWithData(SgAsmFile* f): f(f) {
      int fd = open(f->get_name().c_str(), O_RDONLY);
      if (fd == -1) {
        perror(("open of " + f->get_name()).c_str());
        abort();
      }
      uint8_t buffer[1048576];
      while (true) {
        int status = read(fd, buffer, 1048576);
        if (status == -1) {
          perror("read");
          abort();
        }
        if (status == 0) break; // EOF
        data.insert(data.end(), buffer, buffer + status);
      }
      close(fd);
    }

    SgAsmSectionHeader* getSectionOfAddress(uint64_t addr) {
      ROSE_ASSERT (f->get_sectionHeaderList());
      const vector<SgAsmSectionHeader*>& sections = f->get_sectionHeaderList()->get_section_headers();
      for (size_t i = 0; i < sections.size(); ++i) {
        if (sections[i] &&
            addr >= sections[i]->get_starting_memory_address() &&
            addr < sections[i]->get_starting_memory_address() + sections[i]->get_size() &&
            (sections[i]->get_flags() & 2) != 0 /* ALLOC */) {
          return sections[i];
        }
      }
      if (addr != 0) {
        cerr << "Address " << addr << " not found in file" << endl;
      }
      return 0;
    }

    size_t getFileOffsetOfAddress(uint64_t addr) {
      SgAsmSectionHeader* section = getSectionOfAddress(addr);
      if (!section) abort();
      return addr - section->get_starting_memory_address() + section->get_starting_file_offset();
    }

    SgAsmInstruction* disassembleOneAtAddress(uint64_t addr, Parameters params, vector<uint64_t>& knownSuccessors) {
      params.ip = addr;
      SgAsmSectionHeader* section = getSectionOfAddress(addr);
      if (!section) return 0;
      size_t fileOffset = addr - section->get_starting_memory_address() + section->get_starting_file_offset();
      try {
        SgAsmInstruction* insn = disassemble(params, data, fileOffset, &knownSuccessors);
        return insn;
      } catch (BadInstruction) {
        return 0;
      } catch (OverflowOfInstructionVector) {
        return 0;
      }
    }

    void disassembleRecursively(uint64_t addr, Parameters params, map<uint64_t, SgAsmInstruction*>& insns, set<uint64_t>& basicBlockStarts) {
      if (insns.find(addr) != insns.end()) return;
      vector<uint64_t> knownSuccessors;
      SgAsmInstruction* insn = disassembleOneAtAddress(addr, params, knownSuccessors);
      if (!insn) return;
      insns.insert(make_pair(addr, insn));
      for (size_t i = 0; i < knownSuccessors.size(); ++i) {
        if (knownSuccessors.size() != 1 || knownSuccessors[i] != addr + insn->get_raw_bytes().size()) {
          basicBlockStarts.insert(knownSuccessors[i]);
        }
        disassembleRecursively(knownSuccessors[i], params, insns, basicBlockStarts);
      }
    }

  };

  void disassembleFile(SgAsmFile* f) {
    AsmFileWithData file(f);
    ROSE_ASSERT (f->get_machine_architecture() == SgAsmFile::e_machine_architecture_Intel_80386 || f->get_machine_architecture() == SgAsmFile::e_machine_architecture_AMD_x86_64_architecture);
    Parameters p(0x0, size32);
    if (f->get_machine_architecture() == SgAsmFile::e_machine_architecture_AMD_x86_64_architecture) {
      p.insnSize = size64;
    }
    map<uint64_t, SgAsmInstruction*> insns;
    set<uint64_t> basicBlockStarts;
    basicBlockStarts.insert(f->get_associated_entry_point());
    file.disassembleRecursively(f->get_associated_entry_point(), p, insns, basicBlockStarts);
    map<uint64_t, SgAsmBlock*> basicBlocks;
    for (set<uint64_t>::const_iterator i = basicBlockStarts.begin(); i != basicBlockStarts.end(); ++i) {
      uint64_t addr = *i;
      SgAsmBlock* b = new SgAsmBlock();
      b->set_address(addr);
      b->set_id(addr);
      basicBlocks[addr] = b;
    }
    SgAsmBlock* blk = ObjdumpToRoseBinaryAst::putInstructionsIntoBasicBlocks(basicBlocks, insns);
    f->set_global_block(blk);
    blk->set_parent(f);
  }

}
