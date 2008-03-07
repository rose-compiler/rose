#include <stdint.h>
#include <vector>
#include <rose.h>

using namespace std;

#if 0

namespace X86Disassembler {

  enum Size {size16, size32, size64};
  enum SegmentOverride {soNone, soCS, soDS, soES, soFS, soGS, soSS};
  enum RepeatPrefix {rpNone, rpRepne, rpRepe};

  struct Parameters {
    uint64_t ip;
    Size insnSize;
  };

  struct SingleInstructionDisassembler {
    // Stuff that is not changed during the course of the instruction
    Parameters p;

    // The instruction
    const vector<uint8_t> insn;
    size_t positionInVector;

    // Temporary flags set by the instruction
    SegmentOverride segOverride;
    bool rexPresent, rexW, rexR, rexX, rexB;
    bool addressSizeOverride, operandSizeOverride;
    bool lock;
    RepeatPrefix repeatPrefix;

    SingleInstructionDisassembler(const Parameters& p, const vector<uint8_t>& insn, size_t positionInVector):
      p(p),
      insn(insn),
      positionInVector(positionInVector),
      segOverride(soNone),
      rexPresent(false),
      rexW(false),
      rexR(false),
      rexX(false),
      rexB(false),
      addressSizeOverride(false),
      operandSizeOverride(false),
      lock(false),
      repeatPrefix(rpNone)
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
        if (p.insnSize == size64 && !rexW) s = size32;
        return s;
        // This doesn't handle all of the special cases
      }
    }

  };

  struct BadInstruction {};

  void getByte(uint8_t& var) {
    if (positionInVector >= insn.size())
      throw BadInstruction();
    var = insn[++positionInVector];
    if (positionInVector >= 15)
      throw BadInstruction();
  }

#define GET_WORD(var) do {uint8_t high, low; GET_BYTE(low); GET_BYTE(high); var = (uint16_t(high) << 8) | uint16_t(low);} while (0)
#define GET_DWORD(var) do {uint16_t high, low; GET_WORD(low); GET_WORD(high); var = (uint32_t(high) << 16) | uint32_t(low);} while (0)
#define GET_QWORD(var) do {uint32_t high, low; GET_DWORD(low); GET_DWORD(high); var = (uint64_t(high) << 32) | uint64_t(low);} while (0)

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

  enum RegisterMode {rmLegacyByte, rmRexByte, rmWord, rmDWord, rmQWord, rmSegment, rmST, rmMM, rmXMM};

  RegisterMode sizeToMode(Size s) {
    switch (s) {
      case size16: return rmWord;
      case size32: return rmDWord;
      case size64: return rmQWord;
      default: ROSE_ASSERT (false);
    }
  }

  SgAsmRegisterReferenceExpression* makeRegister(uint8_t fullRegisterNumber, RegisterMode m) {
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

      default: ROSE_ASSERT (false);
    }
    return ref;
  }

  SgAsmRegisterReferenceExpression* makeOperandRegisterByte(const State& s, bool rexExtension, uint8_t registerNumber) {
    SgAsmRegisterReferenceExpression* ref = makeRegister((rexExtension ? 8 : 0) + registerNumber, (s.rexPresent ? rmRexByte : rmLegacyByte));
    return ref;
  }

  SgAsmRegisterReferenceExpression* makeOperandRegisterFull(const State& s, bool rexExtension, uint8_t registerNumber) {
    SgAsmRegisterReferenceExpression* ref = makeRegister((rexExtension ? 8 : 0) + registerNumber, sizeToMode(s.p.insnSize));
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

  SgAsmRegisterReferenceExpression* makeIP(const State& s) {
    SgAsmRegisterReferenceExpression* r = new SgAsmRegisterReferenceExpression();
    r->set_x86_register_code(SgAsmRegisterReferenceExpression::rIP);
    r->set_x86_position_in_register_code(sizeToPos(s.p.insnSize));
    r->set_type(sizeToType(s.p.insnSize));
    return r;
  }

  SgAsmExpression* decodeModrmMemory(const State& s, const vector<uint8_t>& insn, size_t& positionInVector, uint8_t modregrm) {
    uint8_t low3bits = modregrm & 7;
    uint8_t high2bits = modregrm >> 6;
    SgAsmExpression* addressExpr = NULL;
    if (s.effectiveAddressSize() == size16) {
      if ((modregrm & 0307) == 0006) { // Special case
        uint16_t offset;
        GET_WORD(offset);
        addressExpr = makeWordValue(offset);
      } else {
        switch (low3bits) {
          case 0: addressExpr = new SgAsmBinaryAdd(makeRegister(3, rmWord), makeRegister(6, rmWord)); break;
          case 1: addressExpr = new SgAsmBinaryAdd(makeRegister(3, rmWord), makeRegister(7, rmWord)); break;
          case 2: addressExpr = new SgAsmBinaryAdd(makeRegister(5, rmWord), makeRegister(6, rmWord)); break;
          case 3: addressExpr = new SgAsmBinaryAdd(makeRegister(5, rmWord), makeRegister(7, rmWord)); break;
          case 4: addressExpr = makeRegister(6, rmWord); break;
          case 5: addressExpr = makeRegister(7, rmWord); break;
          case 6: addressExpr = makeRegister(5, rmWord); break;
          case 7: addressExpr = makeRegister(3, rmWord); break;
          default: ROSE_ASSERT (false);
        }
        switch (high2bits) {
          case 0: break; // No offset
          case 1: {uint8_t offset; GET_BYTE(offset); addressExpr = new SgAsmBinaryAdd(addressExpr, makeByteValue(offset)); break;}
          case 2: {uint16_t offset; GET_WORD(offset); addressExpr = new SgAsmBinaryAdd(addressExpr, makeWordValue(offset)); break;}
          default: ROSE_ASSERT (false);
        }
      }
    } else { // 32 or 64 bits
      if ((modregrm & 0307) == 0006) { // Special case
        uint32_t offset;
        GET_DWORD(offset);
        addressExpr = makeDWordValue(offset);
        if (s.p.insnSize == size64) {
          addressExpr = new SgAsmBinaryAdd(makeIP(s), addressExpr);
        }
      } else {
        if (low3bits == 5) { // Need SIB
          uint8_t sib;
          GET_BYTE(sib);
          uint8_t sibScaleField = sib >> 6;
          uint8_t sibIndexField = (sib & 0070) >> 3;
          uint8_t sibBaseField = sib & 7;
          uint8_t actualScale = (1 << sibScaleField);
          SgAsmExpression* sibBase = NULL;
          if (sibBaseField == 5) {
            switch (high2bits) {
              case 0: {uint32_t offset; GET_DWORD(offset); sibBase = makeDWordValue(offset); break;}
              case 1: {uint8_t offset; GET_BYTE(offset); sibBase = new SgAsmBinaryAdd(makeRegister((s.rexB ? 13 : 5), sizeToMode(s.p.insnSize)), makeByteValue(offset)); break;}
              case 2: {uint32_t offset; GET_DWORD(offset); sibBase = new SgAsmBinaryAdd(makeRegister((s.rexB ? 13 : 5), sizeToMode(s.p.insnSize)), makeDWordValue(offset)); break;}
              default: ROSE_ASSERT (false);
            }
          } else {
            sibBase = makeOperandRegisterFull(s, s.rexB, sibBaseField);
          }
          if (sibIndexField == 4) {
            addressExpr = sibBase;
          } else if (actualScale == 1) {
            addressExpr = new SgAsmBinaryAdd(makeOperandRegisterFull(s, s.rexX, sibIndexField), sibBase);
          } else {
            addressExpr = new SgAsmBinaryAdd(new SgAsmBinaryMultiply(makeOperandRegisterFull(s, s.rexX, sibIndexField), makeByteValue(actualScale)), sibBase);
          }
        } else {
          addressExpr = makeOperandRegisterFull(s, s.rexB, low3bits);
        }
        switch (high2bits) {
          case 0: break; // No offset
          case 1: {uint8_t offset; GET_BYTE(offset); addressExpr = new SgAsmBinaryAdd(addressExpr, makeByteValue(offset)); break;}
          case 2: {uint32_t offset; GET_DWORD(offset); addressExpr = new SgAsmBinaryAdd(addressExpr, makeDWordValue(offset)); break;}
          default: ROSE_ASSERT (false);
        }
      }
    }
    ROSE_ASSERT (addressExpr);
    return new SgAsmMemoryReferenceExpression(addressExpr);
  }

  SgAsmMemoryReferenceExpression* makeMemoryReference(SgAsmExpression* addr, SgAsmType* t = NULL) {
    SgAsmMemoryReferenceExpression* r = new SgAsmMemoryReferenceExpression(addr);
    addr->set_parent(r);
    if (t) r->set_type(t);
    return r;
  }

  SgAsmRegisterReferenceExpression* makeRegisterEffective(const State& s, uint8_t fullRegisterNumber) {
    return makeRegister(fullRegisterNumber, sizeToMode(s.effectiveOperandSize()));
  }

  SgAsmRegisterReferenceExpression* makeRegisterEffective(const State& s, bool rexExtension, uint8_t registerNumber) {
    return makeRegister(registerNumber + (rexExtension ? 8 : 0), sizeToMode(s.effectiveOperandSize()));
  }

  void getModrmForByte(const State& s, const vector<uint8_t>& insn, size_t& positionInVector, uint8_t modregrm, SgAsmExpression*& modrm) {
    if ((modregrm >> 6) == 3) { // Register
      modrm = makeOperandRegisterByte(s, s.rexB, (modregrm & 7));
    } else {
      modrm = decodeModrmMemory(s, insn, positionInVector, modregrm);
      isSgAsmMemoryReferenceExpression(modrm)->set_type(SgAsmTypeByte::createType());
    }
  }

  void getModrmFullForByte(const State& s, const vector<uint8_t>& insn, size_t& positionInVector, uint8_t modregrm, SgAsmExpression*& modrm, SgAsmExpression*& reg) {
    reg = makeOperandRegisterByte(s, s.rexR, (modregrm & 0070) >> 3);
    getModrmForByte(s, insn, positionInVector, modregrm, modrm);
  }

  void getModrmOpcodeForByte(const State& s, const vector<uint8_t>& insn, size_t& positionInVector, uint8_t modregrm, SgAsmExpression*& modrm, uint8_t& reg) {
    reg = (modregrm & 0070) >> 3;
    getModrmForByte(s, insn, positionInVector, modregrm, modrm);
  }

  void getModrmForXword(const State& s, const vector<uint8_t>& insn, size_t& positionInVector, uint8_t modregrm, SgAsmExpression*& modrm) {
    if ((modregrm >> 6) == 3) { // Register
      modrm = makeRegisterEffective(s, s.rexB, (modregrm & 7));
    } else {
      modrm = decodeModrmMemory(s, insn, positionInVector, modregrm);
      isSgAsmMemoryReferenceExpression(modrm)->set_type(sizeToType(s.effectiveOperandSize()));
    }
  }

  void getModrmFullForXword(const State& s, const vector<uint8_t>& insn, size_t& positionInVector, uint8_t modregrm, SgAsmExpression*& modrm, SgAsmExpression*& reg) {
    reg = makeRegisterEffective(s, s.rexR, ((modregrm & 0070) >> 3));
    getModrmForXword(s, insn, positionInVector, modregrm, modrm);
  }

  void getModrmOpcodeForXword(const State& s, const vector<uint8_t>& insn, size_t& positionInVector, uint8_t modregrm, SgAsmExpression*& modrm, uint8_t& reg) {
    reg = (modregrm & 0070) >> 3;
    getModrmForXword(s, insn, positionInVector, modregrm, modrm);
  }

  void getModrmForWord(const State& s, const vector<uint8_t>& insn, size_t& positionInVector, uint8_t modregrm, SgAsmExpression*& modrm) {
    if ((modregrm >> 6) == 3) { // Register
      modrm = makeRegister((s.rexB ? 8 : 0) + (modregrm & 7), rmWord);
    } else {
      modrm = decodeModrmMemory(s, insn, positionInVector, modregrm);
      isSgAsmMemoryReferenceExpression(modrm)->set_type(SgAsmTypeWord::createType());
    }
  }

  void getModrmFullForWordSegreg(const State& s, const vector<uint8_t>& insn, size_t& positionInVector, uint8_t modregrm, SgAsmExpression*& modrm, SgAsmExpression*& reg) {
    reg = makeRegister(((modregrm & 0070) >> 3), rmSegment);
    getModrmForWord(s, insn, positionInVector, modregrm, modrm);
  }

  void getModrmForFloat(const State& s, const vector<uint8_t>& insn, size_t& positionInVector, uint8_t modregrm, SgAsmExpression*& modrm) {
    if ((modregrm >> 6) == 3) { // Register
      modrm = makeRegister((modregrm & 7), rmST);
    } else {
      modrm = decodeModrmMemory(s, insn, positionInVector, modregrm);
    }
  }

  void getModrmOpcodeForFloat(const State& s, const vector<uint8_t>& insn, size_t& positionInVector, uint8_t modregrm, SgAsmExpression*& modrm, uint8_t& reg) {
    reg = ((modregrm & 0070) >> 3);
    getModrmForFloat(s, insn, positionInVector, modregrm, modrm);
  }

  template <typename InsnType>
  InsnType* makeNullaryInstruction(const State& s, const string& mnemonic) {
    InsnType* insn = new InsnType(s.ip, mnemonic);
    return insn;
  }
#define MAKE_INSN0(Tag, Mne) (makeNullaryInstruction<SgAsmx86Instruction>(s, #Mne))

  template <typename InsnType>
  InsnType* makeUnaryInstruction(const State& s, const string& mnemonic, SgAsmExpression* op1) {
    InsnType* insn = new InsnType(s.ip, mnemonic);
    SgAsmOperandList* operands = new SgAsmOperandList();
    insn->set_operandList(operands);
    operands->set_parent(insn);
    operands->get_operands().push_back(op1);
    op1->set_parent(operands);
    return insn;
  }
#define MAKE_INSN1(Tag, Mne, Op1) (makeUnaryInstruction<SgAsmx86Instruction>(s, #Mne, (Op1)))

  template <typename InsnType>
  InsnType* makeBinaryInstruction(const State& s, const string& mnemonic, SgAsmExpression* op1, SgAsmExpression* op2) {
    InsnType* insn = new InsnType(s.ip, mnemonic);
    SgAsmOperandList* operands = new SgAsmOperandList();
    insn->set_operandList(operands);
    operands->set_parent(insn);
    operands->get_operands().push_back(op1);
    op1->set_parent(operands);
    operands->get_operands().push_back(op2);
    op2->set_parent(operands);
    return insn;
  }
#define MAKE_INSN2(Tag, Mne, Op1, Op2) (makeBinaryInstruction<SgAsmx86Instruction>(s, #Mne, (Op1), (Op2)))

  template <typename InsnType>
  InsnType* makeTernaryInstruction(const State& s, const string& mnemonic, SgAsmExpression* op1, SgAsmExpression* op2, SgAsmExpression* op3) {
    InsnType* insn = new InsnType(s.ip, mnemonic);
    SgAsmOperandList* operands = new SgAsmOperandList();
    insn->set_operandList(operands);
    operands->set_parent(insn);
    operands->get_operands().push_back(op1);
    op1->set_parent(operands);
    operands->get_operands().push_back(op2);
    op2->set_parent(operands);
    operands->get_operands().push_back(op3);
    op3->set_parent(operands);
    return insn;
  }
#define MAKE_INSN3(Tag, Mne, Op1, Op2, Op3) (makeTernaryInstruction<SgAsmx86##Tag>(s, #Mne, (Op1), (Op2), (Op3)))

  SgAsmExpression* makeFullSizeValue(const State& s, uint64_t val) {
    switch (s.effectiveOperandSize()) {
      case size16: return makeWordValue(val);
      case size32: return makeDWordValue(val);
      case size64: return makeQWordValue(val);
      default: ROSE_ASSERT (false);
    }
  }

  SgAsmx86Instruction* decodeOpcode0F(const State& s);
  SgAsmx86Instruction* decodeX87InstructionD8(const State& s, const vector<uint8_t>& insn, size_t& positionInVector);
  SgAsmx86Instruction* decodeX87InstructionD9(const State& s, const vector<uint8_t>& insn, size_t& positionInVector);
  SgAsmx86Instruction* decodeX87InstructionDA(const State& s, const vector<uint8_t>& insn, size_t& positionInVector);
  SgAsmx86Instruction* decodeX87InstructionDB(const State& s, const vector<uint8_t>& insn, size_t& positionInVector);
  SgAsmx86Instruction* decodeX87InstructionDC(const State& s, const vector<uint8_t>& insn, size_t& positionInVector);
  SgAsmx86Instruction* decodeX87InstructionDD(const State& s, const vector<uint8_t>& insn, size_t& positionInVector);
  SgAsmx86Instruction* decodeX87InstructionDE(const State& s, const vector<uint8_t>& insn, size_t& positionInVector);
  SgAsmx86Instruction* decodeX87InstructionDF(const State& s, const vector<uint8_t>& insn, size_t& positionInVector);
  SgAsmx86Instruction* decodeGroup1(const State& s, uint8_t reg, SgAsmExpression* modrm, SgAsmExpression* imm);
  SgAsmx86Instruction* decodeGroup1a(const State& s, uint8_t reg, SgAsmExpression* modrm);
  SgAsmx86Instruction* decodeGroup2(const State& s, uint8_t reg, SgAsmExpression* modrm, SgAsmExpression* count);
  SgAsmx86Instruction* decodeGroup3(const State& s, uint8_t reg, SgAsmExpression* modrm, SgAsmExpression* immMaybe);
  SgAsmx86Instruction* decodeGroup4(const State& s, uint8_t reg, SgAsmExpression* modrm);
  SgAsmx86Instruction* decodeGroup5(const State& s, uint8_t reg, SgAsmExpression* modrm);
  SgAsmx86Instruction* decodeGroup11(const State& s, uint8_t reg, SgAsmExpression* modrm, SgAsmExpression* imm);

  SgAsmx86Instruction* disassemble(State s /* Copied on purpose */, const vector<uint8_t>& insn /* Should be at least 15 bytes (max insn length) if possible */) {
    s.resetForNewInstruction();
    size_t positionInVector = 0;
#define REX(val) if (s.p.insnSize == size64) {s.rexPresent = true; s.rexW = (val & 8) != 0; s.rexR = (val & 4) != 0; s.rexX = (val & 2) != 0; s.rexB = (val & 1) != 0;}
#define GET_MODRM_FOR_BYTE uint8_t modregrm; SgAsmExpression* modrm = NULL; SgAsmExpression* reg = NULL; GET_BYTE(modregrm); getModrmFullForByte(s, insn, positionInVector, modregrm, modrm, reg); ROSE_ASSERT (reg); ROSE_ASSERT (modrm);
#define GET_MODRM_FOR_XWORD uint8_t modregrm; SgAsmExpression* modrm = NULL; SgAsmExpression* reg = NULL; GET_BYTE(modregrm); getModrmFullForXword(s, insn, positionInVector, modregrm, modrm, reg); ROSE_ASSERT (reg); ROSE_ASSERT (modrm);
#define GET_MODRM_FOR_WORD_SEGREG uint8_t modregrm; SgAsmExpression* modrm = NULL; SgAsmExpression* segreg = NULL; GET_BYTE(modregrm); getModrmFullForWordSegreg(s, insn, positionInVector, modregrm, modrm, segreg); ROSE_ASSERT (segreg); ROSE_ASSERT (modrm);
#define GET_MODRM_OPCODE_FOR_BYTE  uint8_t modregrm; SgAsmExpression* modrm = NULL; uint8_t reg = 0; GET_BYTE(modregrm); getModrmOpcodeForByte(s, insn, positionInVector, modregrm, modrm, reg); ROSE_ASSERT (modrm);
#define GET_MODRM_OPCODE_FOR_XWORD uint8_t modregrm; SgAsmExpression* modrm = NULL; uint8_t reg = 0; GET_BYTE(modregrm); getModrmOpcodeForXword(s, insn, positionInVector, modregrm, modrm, reg); ROSE_ASSERT (modrm);
#define GET_MODRM_OPCODE_FOR_FLOAT uint8_t modregrm; SgAsmExpression* modrm = NULL; uint8_t reg = 0; GET_BYTE(modregrm); getModrmOpcodeForFloat(s, insn, positionInVector, modregrm, modrm, reg); ROSE_ASSERT (modrm);
#define NOT_64 do {if (s.p.insnSize == size64) throw BadInstruction();} while (0)
#define GET_IMM_BYTE SgAsmExpression* imm = NULL; {uint8_t val; GET_BYTE(val); imm = makeByteValue(val); ROSE_ASSERT (imm);}
#define GET_IMM_WORD SgAsmExpression* imm = NULL; {uint16_t val; GET_WORD(val); imm = makeWordValue(val); ROSE_ASSERT (imm);}
#define GET_IMM_DWORD SgAsmExpression* imm = NULL; {uint32_t val; GET_DWORD(val); imm = makeDWordValue(val); ROSE_ASSERT (imm);}
#define GET_IMM_QWORD SgAsmExpression* imm = NULL; {uint64_t val; GET_QWORD(val); imm = makeQWordValue(val); ROSE_ASSERT (imm);}
#define GET_IMM_Iz SgAsmExpression* imm = NULL; if (s.effectiveOperandSize() == 16) {uint16_t val; GET_WORD(val); imm = makeWordValue(val); ROSE_ASSERT (imm);} else {uint32_t val; GET_DWORD(val); imm = makeDWordValue(val); ROSE_ASSERT (imm);}
#define GET_IMM_Iv SgAsmExpression* imm = NULL; if (s.effectiveOperandSize() == 16) {uint16_t val; GET_WORD(val); imm = makeWordValue(val); ROSE_ASSERT (imm);} else if (s.effectiveOperandSize() == 32) {uint32_t val; GET_DWORD(val); imm = makeDWordValue(val); ROSE_ASSERT (imm);} else {uint64_t val; GET_QWORD(val); imm = makeQWordValue(val); ROSE_ASSERT (imm);}
#define GET_IMM_Jb SgAsmExpression* imm = NULL; {uint8_t val; GET_BYTE(val); imm = makeFullSizeValue(s, s.ip + positionInVector + (int8_t)val); ROSE_ASSERT (imm);}
#define GET_IMM_Jz SgAsmExpression* imm = NULL; if (s.effectiveOperandSize() == 16) {uint16_t val; GET_WORD(val); imm = makeFullSizeValue(s, s.ip + positionInVector + (int16_t)val); ROSE_ASSERT (imm);} else {uint32_t val; GET_DWORD(val); imm = makeFullSizeValue(s, s.ip + positionInVector + (int32_t)val); ROSE_ASSERT (imm);}
#define GET_IMM_FOR_ADDR SgAsmExpression* imm = NULL; if (s.effectiveAddressSize() == 16) {uint16_t val; GET_WORD(val); imm = makeWordValue(val); ROSE_ASSERT (imm);} else if (s.effectiveAddressSize() == 32) {uint32_t val; GET_DWORD(val); imm = makeDWordValue(val); ROSE_ASSERT (imm);} else {uint64_t val; GET_QWORD(val); imm = makeQWordValue(val); ROSE_ASSERT (imm);}
#define REQUIRE_MEMORY(e) do {if (!isSgAsmMemoryReferenceExpression(e)) throw BadInstruction();} while (0)
#define GET_BRANCH_PREDICTION do {} while (0) // FIXME
    uint8_t opcode;
decodeOpcodeNormal:
    GET_BYTE(opcode);
    switch (opcode) {
      case 0x00: {GET_MODRM_FOR_BYTE; return MAKE_INSN2(Add, add, modrm, reg);}
      case 0x01: {GET_MODRM_FOR_XWORD; return MAKE_INSN2(Add, add, modrm, reg);}
      case 0x02: {GET_MODRM_FOR_BYTE; return MAKE_INSN2(Add, add, reg, modrm);}
      case 0x03: {GET_MODRM_FOR_XWORD; return MAKE_INSN2(Add, add, reg, modrm);}
      case 0x04: {GET_IMM_BYTE; return MAKE_INSN2(Add, add, makeRegister(0, rmLegacyByte), imm);}
      case 0x05: {GET_IMM_Iz; return MAKE_INSN2(Add, add, makeRegisterEffective(s, 0), imm);}
      case 0x06: {NOT_64; return MAKE_INSN1(Push, push, makeRegister(0, rmSegment));}
      case 0x07: {NOT_64; return MAKE_INSN1(Pop, pop, makeRegister(0, rmSegment));}
      case 0x08: {GET_MODRM_FOR_BYTE; return MAKE_INSN2(Or, or, modrm, reg);}
      case 0x09: {GET_MODRM_FOR_XWORD; return MAKE_INSN2(Or, or, modrm, reg);}
      case 0x0A: {GET_MODRM_FOR_BYTE; return MAKE_INSN2(Or, or, reg, modrm);}
      case 0x0B: {GET_MODRM_FOR_XWORD; return MAKE_INSN2(Or, or, reg, modrm);}
      case 0x0C: {GET_IMM_BYTE; return MAKE_INSN2(Or, or, makeRegister(0, rmLegacyByte), imm);}
      case 0x0D: {GET_IMM_Iz; return MAKE_INSN2(Or, or, makeRegisterEffective(s, 0), imm);}
      case 0x0E: {NOT_64; return MAKE_INSN1(Push, push, makeRegister(1, rmSegment));}
      case 0x0F: {return decodeOpcode0F(s);}
      case 0x10: {GET_MODRM_FOR_BYTE; return MAKE_INSN2(Adc, adc, modrm, reg);}
      case 0x11: {GET_MODRM_FOR_XWORD; return MAKE_INSN2(Adc, adc, modrm, reg);}
      case 0x12: {GET_MODRM_FOR_BYTE; return MAKE_INSN2(Adc, adc, reg, modrm);}
      case 0x13: {GET_MODRM_FOR_XWORD; return MAKE_INSN2(Adc, adc, reg, modrm);}
      case 0x14: {GET_IMM_BYTE; return MAKE_INSN2(Adc, adc, makeRegister(0, rmLegacyByte), imm);}
      case 0x15: {GET_IMM_Iz; return MAKE_INSN2(Adc, adc, makeRegisterEffective(s, 0), imm);}
      case 0x16: {NOT_64; return MAKE_INSN1(Push, push, makeRegister(2, rmSegment));}
      case 0x17: {NOT_64; return MAKE_INSN1(Pop, pop, makeRegister(2, rmSegment));}
      case 0x18: {GET_MODRM_FOR_BYTE; return MAKE_INSN2(Sbb, sbb, modrm, reg);}
      case 0x19: {GET_MODRM_FOR_XWORD; return MAKE_INSN2(Sbb, sbb, modrm, reg);}
      case 0x1A: {GET_MODRM_FOR_BYTE; return MAKE_INSN2(Sbb, sbb, reg, modrm);}
      case 0x1B: {GET_MODRM_FOR_XWORD; return MAKE_INSN2(Sbb, sbb, reg, modrm);}
      case 0x1C: {GET_IMM_BYTE; return MAKE_INSN2(Sbb, sbb, makeRegister(0, rmLegacyByte), imm);}
      case 0x1D: {GET_IMM_Iz; return MAKE_INSN2(Sbb, sbb, makeRegisterEffective(s, 0), imm);}
      case 0x1E: {NOT_64; return MAKE_INSN1(Push, push, makeRegister(3, rmSegment));}
      case 0x1F: {NOT_64; return MAKE_INSN1(Pop, pop, makeRegister(3, rmSegment));}
      case 0x20: {GET_MODRM_FOR_BYTE; return MAKE_INSN2(And, and, modrm, reg);}
      case 0x21: {GET_MODRM_FOR_XWORD; return MAKE_INSN2(And, and, modrm, reg);}
      case 0x22: {GET_MODRM_FOR_BYTE; return MAKE_INSN2(And, and, reg, modrm);}
      case 0x23: {GET_MODRM_FOR_XWORD; return MAKE_INSN2(And, and, reg, modrm);}
      case 0x24: {GET_IMM_BYTE; return MAKE_INSN2(And, and, makeRegister(0, rmLegacyByte), imm);}
      case 0x25: {GET_IMM_Iz; return MAKE_INSN2(And, and, makeRegisterEffective(s, 0), imm);}
      case 0x26: {s.segOverride = soES; goto decodeOpcodeNormal;}
      case 0x27: {NOT_64; return new SgAsmx86Daa();}
      case 0x28: {GET_MODRM_FOR_BYTE; return MAKE_INSN2(Sub, sub, modrm, reg);}
      case 0x29: {GET_MODRM_FOR_XWORD; return MAKE_INSN2(Sub, sub, modrm, reg);}
      case 0x2A: {GET_MODRM_FOR_BYTE; return MAKE_INSN2(Sub, sub, reg, modrm);}
      case 0x2B: {GET_MODRM_FOR_XWORD; return MAKE_INSN2(Sub, sub, reg, modrm);}
      case 0x2C: {GET_IMM_BYTE; return MAKE_INSN2(Sub, sub, makeRegister(0, rmLegacyByte), imm);}
      case 0x2D: {GET_IMM_Iz; return MAKE_INSN2(Sub, sub, makeRegisterEffective(s, 0), imm);}
      case 0x2E: {s.segOverride = soCS; goto decodeOpcodeNormal;}
      case 0x2F: {NOT_64; return new SgAsmx86Das();}
      case 0x30: {GET_MODRM_FOR_BYTE; return MAKE_INSN2(Xor, xor, modrm, reg);}
      case 0x31: {GET_MODRM_FOR_XWORD; return MAKE_INSN2(Xor, xor, modrm, reg);}
      case 0x32: {GET_MODRM_FOR_BYTE; return MAKE_INSN2(Xor, xor, reg, modrm);}
      case 0x33: {GET_MODRM_FOR_XWORD; return MAKE_INSN2(Xor, xor, reg, modrm);}
      case 0x34: {GET_IMM_BYTE; return MAKE_INSN2(Xor, xor, makeRegister(0, rmLegacyByte), imm);}
      case 0x35: {GET_IMM_Iz; return MAKE_INSN2(Xor, xor, makeRegisterEffective(s, 0), imm);}
      case 0x36: {s.segOverride = soSS; goto decodeOpcodeNormal;}
      case 0x37: {NOT_64; return new SgAsmx86Aaa();}
      case 0x38: {GET_MODRM_FOR_BYTE; return MAKE_INSN2(Cmp, cmp, modrm, reg);}
      case 0x39: {GET_MODRM_FOR_XWORD; return MAKE_INSN2(Cmp, cmp, modrm, reg);}
      case 0x3A: {GET_MODRM_FOR_BYTE; return MAKE_INSN2(Cmp, cmp, reg, modrm);}
      case 0x3B: {GET_MODRM_FOR_XWORD; return MAKE_INSN2(Cmp, cmp, reg, modrm);}
      case 0x3C: {GET_IMM_BYTE; return MAKE_INSN2(Cmp, cmp, makeRegister(0, rmLegacyByte), imm);}
      case 0x3D: {GET_IMM_Iz; return MAKE_INSN2(Cmp, cmp, makeRegisterEffective(s, 0), imm);}
      case 0x3E: {s.segOverride = soDS; goto decodeOpcodeNormal;}
      case 0x3F: {NOT_64; return new SgAsmx86Aas();}
      case 0x40: {REX(0x40) else return MAKE_INSN1(Inc, inc, makeRegisterEffective(s, 0));}
      case 0x41: {REX(0x41) else return MAKE_INSN1(Inc, inc, makeRegisterEffective(s, 1));}
      case 0x42: {REX(0x42) else return MAKE_INSN1(Inc, inc, makeRegisterEffective(s, 2));}
      case 0x43: {REX(0x43) else return MAKE_INSN1(Inc, inc, makeRegisterEffective(s, 3));}
      case 0x44: {REX(0x44) else return MAKE_INSN1(Inc, inc, makeRegisterEffective(s, 4));}
      case 0x45: {REX(0x45) else return MAKE_INSN1(Inc, inc, makeRegisterEffective(s, 5));}
      case 0x46: {REX(0x46) else return MAKE_INSN1(Inc, inc, makeRegisterEffective(s, 6));}
      case 0x47: {REX(0x47) else return MAKE_INSN1(Inc, inc, makeRegisterEffective(s, 7));}
      case 0x48: {REX(0x48) else return MAKE_INSN1(Dec, dec, makeRegisterEffective(s, 0));}
      case 0x49: {REX(0x49) else return MAKE_INSN1(Dec, dec, makeRegisterEffective(s, 1));}
      case 0x4A: {REX(0x4A) else return MAKE_INSN1(Dec, dec, makeRegisterEffective(s, 2));}
      case 0x4B: {REX(0x4B) else return MAKE_INSN1(Dec, dec, makeRegisterEffective(s, 3));}
      case 0x4C: {REX(0x4C) else return MAKE_INSN1(Dec, dec, makeRegisterEffective(s, 4));}
      case 0x4D: {REX(0x4D) else return MAKE_INSN1(Dec, dec, makeRegisterEffective(s, 5));}
      case 0x4E: {REX(0x4E) else return MAKE_INSN1(Dec, dec, makeRegisterEffective(s, 6));}
      case 0x4F: {REX(0x4F) else return MAKE_INSN1(Dec, dec, makeRegisterEffective(s, 7));}
      case 0x50: {return MAKE_INSN1(Push, push, makeRegisterEffective(s, s.rexB, 0));}
      case 0x51: {return MAKE_INSN1(Push, push, makeRegisterEffective(s, s.rexB, 1));}
      case 0x52: {return MAKE_INSN1(Push, push, makeRegisterEffective(s, s.rexB, 2));}
      case 0x53: {return MAKE_INSN1(Push, push, makeRegisterEffective(s, s.rexB, 3));}
      case 0x54: {return MAKE_INSN1(Push, push, makeRegisterEffective(s, s.rexB, 4));}
      case 0x55: {return MAKE_INSN1(Push, push, makeRegisterEffective(s, s.rexB, 5));}
      case 0x56: {return MAKE_INSN1(Push, push, makeRegisterEffective(s, s.rexB, 6));}
      case 0x57: {return MAKE_INSN1(Push, push, makeRegisterEffective(s, s.rexB, 7));}
      case 0x58: {return MAKE_INSN1(Pop, pop, makeRegisterEffective(s, s.rexB, 0));}
      case 0x59: {return MAKE_INSN1(Pop, pop, makeRegisterEffective(s, s.rexB, 1));}
      case 0x5A: {return MAKE_INSN1(Pop, pop, makeRegisterEffective(s, s.rexB, 2));}
      case 0x5B: {return MAKE_INSN1(Pop, pop, makeRegisterEffective(s, s.rexB, 3));}
      case 0x5C: {return MAKE_INSN1(Pop, pop, makeRegisterEffective(s, s.rexB, 4));}
      case 0x5D: {return MAKE_INSN1(Pop, pop, makeRegisterEffective(s, s.rexB, 5));}
      case 0x5E: {return MAKE_INSN1(Pop, pop, makeRegisterEffective(s, s.rexB, 6));}
      case 0x5F: {return MAKE_INSN1(Pop, pop, makeRegisterEffective(s, s.rexB, 7));}
      case 0x60: {NOT_64; return MAKE_INSN0(Pusha, pusha);}
      case 0x61: {NOT_64; return MAKE_INSN0(Popa, popa);}
      case 0x62: {NOT_64; GET_MODRM_FOR_XWORD; REQUIRE_MEMORY(modrm); return MAKE_INSN2(Bound, bound, reg, modrm);}
      case 0x63: ROSE_ASSERT (!"Not handled by ROSE"); // {GET_MODRM_FOR_XWORD; if (s.p.insnSize == size64) {return MAKE_INSN2(Movsxd, movsxd, reg, modrm);} else {return MAKE_INSN2(Arpl, arpl, modrm, reg);}}
      case 0x64: {s.segOverride = soFS; goto decodeOpcodeNormal;}
      case 0x65: {s.segOverride = soGS; goto decodeOpcodeNormal;}
      case 0x66: {s.operandSizeOverride = true; goto decodeOpcodeNormal;}
      case 0x67: {s.addressSizeOverride = true; goto decodeOpcodeNormal;}
      case 0x68: {GET_IMM_Iz; return MAKE_INSN1(Push, push, imm);}
      case 0x69: ROSE_ASSERT (!"Not handled by ROSE"); // {GET_MODRM_FOR_XWORD; GET_IMM_Iz; return MAKE_INSN3(Imul, imul, reg, modrm, imm);}
      case 0x6A: {GET_IMM_BYTE; return MAKE_INSN1(Push, push, imm);}
      case 0x6B: ROSE_ASSERT (!"Not handled by ROSE"); // {GET_MODRM_FOR_XWORD; GET_IMM_BYTE; return MAKE_INSN3(Imul, imul, reg, modrm, imm);}
      case 0x6C: {return MAKE_INSN0(Ins, ins);}
      case 0x6D: {return MAKE_INSN0(Ins, ins);}
      case 0x6E: {return MAKE_INSN0(Outs, outs);}
      case 0x6F: {return MAKE_INSN0(Outs, outs);}
      case 0x70: {GET_IMM_Jb; GET_BRANCH_PREDICTION; return MAKE_INSN1(Jo , jo , /* prediction, */ imm);}
      case 0x71: {GET_IMM_Jb; GET_BRANCH_PREDICTION; return MAKE_INSN1(Jno, jno, /* prediction, */ imm);}
      case 0x72: {GET_IMM_Jb; GET_BRANCH_PREDICTION; return MAKE_INSN1(Jb , jb , /* prediction, */ imm);}
      case 0x73: {GET_IMM_Jb; GET_BRANCH_PREDICTION; return MAKE_INSN1(Jae, jae, /* prediction, */ imm);}
      case 0x74: {GET_IMM_Jb; GET_BRANCH_PREDICTION; return MAKE_INSN1(Je , je , /* prediction, */ imm);}
      case 0x75: {GET_IMM_Jb; GET_BRANCH_PREDICTION; return MAKE_INSN1(Jne, jne, /* prediction, */ imm);}
      case 0x76: {GET_IMM_Jb; GET_BRANCH_PREDICTION; return MAKE_INSN1(Jbe, jbe, /* prediction, */ imm);}
      case 0x77: {GET_IMM_Jb; GET_BRANCH_PREDICTION; return MAKE_INSN1(Ja , ja , /* prediction, */ imm);}
      case 0x78: {GET_IMM_Jb; GET_BRANCH_PREDICTION; return MAKE_INSN1(Js , js , /* prediction, */ imm);}
      case 0x79: {GET_IMM_Jb; GET_BRANCH_PREDICTION; return MAKE_INSN1(Jns, jns, /* prediction, */ imm);}
      case 0x7A: {GET_IMM_Jb; GET_BRANCH_PREDICTION; return MAKE_INSN1(Jpe, jpe, /* prediction, */ imm);}
      case 0x7B: {GET_IMM_Jb; GET_BRANCH_PREDICTION; return MAKE_INSN1(Jpo, jpo, /* prediction, */ imm);}
      case 0x7C: {GET_IMM_Jb; GET_BRANCH_PREDICTION; return MAKE_INSN1(Jl , jl , /* prediction, */ imm);}
      case 0x7D: {GET_IMM_Jb; GET_BRANCH_PREDICTION; return MAKE_INSN1(Jge, jge, /* prediction, */ imm);}
      case 0x7E: {GET_IMM_Jb; GET_BRANCH_PREDICTION; return MAKE_INSN1(Jle, jle, /* prediction, */ imm);}
      case 0x7F: {GET_IMM_Jb; GET_BRANCH_PREDICTION; return MAKE_INSN1(Jg , jg , /* prediction, */ imm);}
      case 0x80: {GET_MODRM_OPCODE_FOR_BYTE; GET_IMM_BYTE; return decodeGroup1(s, reg, modrm, imm);}
      case 0x81: {GET_MODRM_OPCODE_FOR_XWORD; GET_IMM_Iz; return decodeGroup1(s, reg, modrm, imm);}
      case 0x82: {NOT_64; GET_MODRM_OPCODE_FOR_BYTE; GET_IMM_BYTE; return decodeGroup1(s, reg, modrm, imm);}
      case 0x83: {GET_MODRM_OPCODE_FOR_XWORD; GET_IMM_BYTE; return decodeGroup1(s, reg, modrm, imm);}
      case 0x84: {GET_MODRM_FOR_BYTE; return MAKE_INSN2(Test, test, modrm, reg);}
      case 0x85: {GET_MODRM_FOR_XWORD; return MAKE_INSN2(Test, test, modrm, reg);}
      case 0x86: {GET_MODRM_FOR_BYTE; return MAKE_INSN2(Xchg, xchg, modrm, reg);}
      case 0x87: {GET_MODRM_FOR_XWORD; return MAKE_INSN2(Xchg, xchg, modrm, reg);}
      case 0x88: {GET_MODRM_FOR_BYTE; return MAKE_INSN2(Mov, mov, modrm, reg);}
      case 0x89: {GET_MODRM_FOR_XWORD; return MAKE_INSN2(Mov, mov, modrm, reg);}
      case 0x8A: {GET_MODRM_FOR_BYTE; return MAKE_INSN2(Mov, mov, reg, modrm);}
      case 0x8B: {GET_MODRM_FOR_XWORD; return MAKE_INSN2(Mov, mov, reg, modrm);}
      case 0x8C: {GET_MODRM_FOR_WORD_SEGREG; return MAKE_INSN2(Mov, mov, modrm, segreg);}
      case 0x8D: {GET_MODRM_FOR_XWORD; REQUIRE_MEMORY(modrm); return MAKE_INSN2(Lea, lea, reg, modrm);}
      case 0x8E: {GET_MODRM_FOR_WORD_SEGREG; return MAKE_INSN2(Mov, mov, segreg, modrm);}
      case 0x8F: {GET_MODRM_OPCODE_FOR_XWORD; return decodeGroup1a(s, reg, modrm);}
      case 0x90: {if (s.rexB) return MAKE_INSN2(Xchg, xchg, makeRegisterEffective(s, 8), makeRegisterEffective(s, 0)); else return MAKE_INSN0(Nop, nop);}
      case 0x91: {return MAKE_INSN2(Xchg, xchg, makeRegisterEffective(s, s.rexB, 1), makeRegisterEffective(s, 0));}
      case 0x92: {return MAKE_INSN2(Xchg, xchg, makeRegisterEffective(s, s.rexB, 2), makeRegisterEffective(s, 0));}
      case 0x93: {return MAKE_INSN2(Xchg, xchg, makeRegisterEffective(s, s.rexB, 3), makeRegisterEffective(s, 0));}
      case 0x94: {return MAKE_INSN2(Xchg, xchg, makeRegisterEffective(s, s.rexB, 4), makeRegisterEffective(s, 0));}
      case 0x95: {return MAKE_INSN2(Xchg, xchg, makeRegisterEffective(s, s.rexB, 5), makeRegisterEffective(s, 0));}
      case 0x96: {return MAKE_INSN2(Xchg, xchg, makeRegisterEffective(s, s.rexB, 6), makeRegisterEffective(s, 0));}
      case 0x97: {return MAKE_INSN2(Xchg, xchg, makeRegisterEffective(s, s.rexB, 7), makeRegisterEffective(s, 0));}
      case 0x98: {return MAKE_INSN0(Cbw, cbw);}
      case 0x99: {return MAKE_INSN0(Cwd, cwd);}
      case 0x9A: ROSE_ASSERT (!"Not supported by ROSE"); // {NOT_64; GET_IMM_FOR_ADDR; GET_IMM_FOR_SEG; return new SgAsmx86FarCall(seg, addr);}
      case 0x9B: {return MAKE_INSN0(Wait, wait);}
      case 0x9C: {return MAKE_INSN0(Pushf, pushf);}
      case 0x9D: {return MAKE_INSN0(Popf, popf);}
      case 0x9E: {return MAKE_INSN0(Sahf, sahf);}
      case 0x9F: {return MAKE_INSN0(Lahf, lahf);}
      case 0xA0: {GET_IMM_BYTE; return MAKE_INSN2(Mov, mov, makeRegister(0, rmLegacyByte), makeMemoryReference(imm, SgAsmTypeByte::createType()));}
      case 0xA1: {GET_IMM_FOR_ADDR; return MAKE_INSN2(Mov, mov, makeRegisterEffective(s, 0), makeMemoryReference(imm, sizeToType(s.effectiveOperandSize())));}
      case 0xA2: {GET_IMM_BYTE; return MAKE_INSN2(Mov, mov, makeMemoryReference(imm, SgAsmTypeByte::createType()), makeRegister(0, rmLegacyByte));}
      case 0xA3: {GET_IMM_FOR_ADDR; return MAKE_INSN2(Mov, mov, makeMemoryReference(imm, sizeToType(s.effectiveOperandSize())), makeRegisterEffective(s, 0));}
      case 0xA4: {return MAKE_INSN0(Movs, movs);}
      case 0xA5: {return MAKE_INSN0(Movs, movs);}
      case 0xA6: {return MAKE_INSN0(Cmps, cmps);}
      case 0xA7: {return MAKE_INSN0(Cmps, cmps);}
      case 0xA8: {GET_IMM_BYTE; return MAKE_INSN2(Test, test, makeRegister(0, rmLegacyByte), imm);}
      case 0xA9: {GET_IMM_Iz; return MAKE_INSN2(Test, test, makeRegisterEffective(s, 0), imm);}
      case 0xAA: {return MAKE_INSN0(Stos, stos);}
      case 0xAB: {return MAKE_INSN0(Stos, stos);}
      case 0xAC: {return MAKE_INSN0(Lods, lods);}
      case 0xAD: {return MAKE_INSN0(Lods, lods);}
      case 0xAE: {return MAKE_INSN0(Scas, scas);}
      case 0xAF: {return MAKE_INSN0(Scas, scas);}
      case 0xB0: {GET_IMM_BYTE; return MAKE_INSN2(Mov, mov, makeRegister((s.rexB ? 8 : 0) + 0, s.rexPresent ? rmRexByte : rmLegacyByte), imm);}
      case 0xB1: {GET_IMM_BYTE; return MAKE_INSN2(Mov, mov, makeRegister((s.rexB ? 8 : 0) + 1, s.rexPresent ? rmRexByte : rmLegacyByte), imm);}
      case 0xB2: {GET_IMM_BYTE; return MAKE_INSN2(Mov, mov, makeRegister((s.rexB ? 8 : 0) + 2, s.rexPresent ? rmRexByte : rmLegacyByte), imm);}
      case 0xB3: {GET_IMM_BYTE; return MAKE_INSN2(Mov, mov, makeRegister((s.rexB ? 8 : 0) + 3, s.rexPresent ? rmRexByte : rmLegacyByte), imm);}
      case 0xB4: {GET_IMM_BYTE; return MAKE_INSN2(Mov, mov, makeRegister((s.rexB ? 8 : 0) + 4, s.rexPresent ? rmRexByte : rmLegacyByte), imm);}
      case 0xB5: {GET_IMM_BYTE; return MAKE_INSN2(Mov, mov, makeRegister((s.rexB ? 8 : 0) + 5, s.rexPresent ? rmRexByte : rmLegacyByte), imm);}
      case 0xB6: {GET_IMM_BYTE; return MAKE_INSN2(Mov, mov, makeRegister((s.rexB ? 8 : 0) + 6, s.rexPresent ? rmRexByte : rmLegacyByte), imm);}
      case 0xB7: {GET_IMM_BYTE; return MAKE_INSN2(Mov, mov, makeRegister((s.rexB ? 8 : 0) + 7, s.rexPresent ? rmRexByte : rmLegacyByte), imm);}
      case 0xB8: {GET_IMM_Iv; return MAKE_INSN2(Mov, mov, makeRegisterEffective(s, s.rexB, 0), imm);}
      case 0xB9: {GET_IMM_Iv; return MAKE_INSN2(Mov, mov, makeRegisterEffective(s, s.rexB, 1), imm);}
      case 0xBA: {GET_IMM_Iv; return MAKE_INSN2(Mov, mov, makeRegisterEffective(s, s.rexB, 2), imm);}
      case 0xBB: {GET_IMM_Iv; return MAKE_INSN2(Mov, mov, makeRegisterEffective(s, s.rexB, 3), imm);}
      case 0xBC: {GET_IMM_Iv; return MAKE_INSN2(Mov, mov, makeRegisterEffective(s, s.rexB, 4), imm);}
      case 0xBD: {GET_IMM_Iv; return MAKE_INSN2(Mov, mov, makeRegisterEffective(s, s.rexB, 5), imm);}
      case 0xBE: {GET_IMM_Iv; return MAKE_INSN2(Mov, mov, makeRegisterEffective(s, s.rexB, 6), imm);}
      case 0xBF: {GET_IMM_Iv; return MAKE_INSN2(Mov, mov, makeRegisterEffective(s, s.rexB, 7), imm);}
      case 0xC0: {GET_MODRM_OPCODE_FOR_BYTE; GET_IMM_BYTE; return decodeGroup2(s, reg, modrm, imm);}
      case 0xC1: {GET_MODRM_OPCODE_FOR_XWORD; GET_IMM_BYTE; return decodeGroup2(s, reg, modrm, imm);}
      case 0xC2: {GET_IMM_WORD; return MAKE_INSN1(Ret, ret, imm);}
      case 0xC3: {return MAKE_INSN0(Ret, ret);}
      case 0xC4: {NOT_64; GET_MODRM_FOR_XWORD; REQUIRE_MEMORY(modrm); return MAKE_INSN2(Les, les, reg, modrm);}
      case 0xC5: {NOT_64; GET_MODRM_FOR_XWORD; REQUIRE_MEMORY(modrm); return MAKE_INSN2(Lds, lds, reg, modrm);}
      case 0xC6: {GET_MODRM_OPCODE_FOR_BYTE; GET_IMM_BYTE; return decodeGroup11(s, reg, modrm, imm);}
      case 0xC7: {GET_MODRM_OPCODE_FOR_XWORD; GET_IMM_Iz; return decodeGroup11(s, reg, modrm, imm);}
      case 0xC8: {SgAsmExpression* immw; {GET_IMM_WORD; immw = imm;} GET_IMM_BYTE; return MAKE_INSN2(Enter, enter, immw, imm);}
      case 0xC9: {return MAKE_INSN0(Leave, leave);}
      case 0xCA: ROSE_ASSERT (!"Not supported by ROSE"); // {GET_IMM_WORD; return MAKE_INSN1(Retf, retf, imm);}
      case 0xCB: ROSE_ASSERT (!"Not supported by ROSE"); // {return MAKE_INSN0(Retf, retf);}
      case 0xCC: ROSE_ASSERT (!"Not supported by ROSE"); // {return MAKE_INSN0(Int3, int3);}
      case 0xCD: {GET_IMM_BYTE; return MAKE_INSN1(Int, int, imm);}
      case 0xCE: {NOT_64; return MAKE_INSN0(Into, into);}
      case 0xCF: ROSE_ASSERT (!"Not supported by ROSE"); // {return MAKE_INSN0(Iret, iret);}
      case 0xD0: {GET_MODRM_OPCODE_FOR_BYTE; return decodeGroup2(s, reg, modrm, makeByteValue(1));}
      case 0xD1: {GET_MODRM_OPCODE_FOR_XWORD; return decodeGroup2(s, reg, modrm, makeByteValue(1));}
      case 0xD2: {GET_MODRM_OPCODE_FOR_BYTE; return decodeGroup2(s, reg, modrm, makeRegister(2, rmLegacyByte));}
      case 0xD3: {GET_MODRM_OPCODE_FOR_XWORD; return decodeGroup2(s, reg, modrm, makeRegister(2, rmLegacyByte));}
      case 0xD4: {NOT_64; return MAKE_INSN0(Aam, aam);}
      case 0xD5: {NOT_64; return MAKE_INSN0(Aad, aad);}
      case 0xD6: ROSE_ASSERT (!"Not supported by ROSE"); // {NOT_64; return MAKE_INSN0(Salc, salc);}
      case 0xD7: {return MAKE_INSN0(Xlat, xlat);}
      case 0xD8: {return decodeX87InstructionD8(s, insn, positionInVector);}
      case 0xD9: {return decodeX87InstructionD9(s, insn, positionInVector);}
      case 0xDA: {return decodeX87InstructionDA(s, insn, positionInVector);}
      case 0xDB: {return decodeX87InstructionDB(s, insn, positionInVector);}
      case 0xDC: {return decodeX87InstructionDC(s, insn, positionInVector);}
      case 0xDD: {return decodeX87InstructionDD(s, insn, positionInVector);}
      case 0xDE: {return decodeX87InstructionDE(s, insn, positionInVector);}
      case 0xDF: {return decodeX87InstructionDF(s, insn, positionInVector);}
      case 0xE0: {GET_IMM_Jb; GET_BRANCH_PREDICTION; return MAKE_INSN1(Loopnz, loopnz, /* prediction, */ imm);}
      case 0xE1: {GET_IMM_Jb; GET_BRANCH_PREDICTION; return MAKE_INSN1(Loopz, loopz, /* prediction, */ imm);}
      case 0xE2: {GET_IMM_Jb; GET_BRANCH_PREDICTION; return MAKE_INSN1(Loop, loop, /* prediction, */ imm);}
      case 0xE3: {GET_IMM_Jb; GET_BRANCH_PREDICTION; return MAKE_INSN1(Jcxz, jcxz, /* prediction, */ imm);}
      case 0xE4: {GET_IMM_BYTE; return MAKE_INSN2(In, in, makeRegister(0, rmLegacyByte), imm);}
      case 0xE5: {GET_IMM_BYTE; return MAKE_INSN2(In, in, makeRegisterEffective(s, 0), imm);}
      case 0xE6: {GET_IMM_BYTE; return MAKE_INSN2(Out, out, imm, makeRegister(0, rmLegacyByte));}
      case 0xE7: {GET_IMM_BYTE; return MAKE_INSN2(Out, out, imm, makeRegisterEffective(s, 0));}
      case 0xE8: {GET_IMM_Jz; return MAKE_INSN1(Call, call, imm);}
      case 0xE9: {GET_IMM_Jz; return MAKE_INSN1(Jmp, jmp, imm);}
      case 0xEA: ROSE_ASSERT (!"Not supported by ROSE"); // {NOT_64; GET_IMM_FOR_ADDR; GET_IMM_FOR_SEG; return new SgAsmx86FarJmp(seg, addr);}
      case 0xEB: {GET_IMM_Jb; return MAKE_INSN1(Jmp, jmp, imm);}
      case 0xEC: {return MAKE_INSN2(In, in, makeRegister(0, rmLegacyByte), makeRegister(2, rmWord));}
      case 0xED: {return MAKE_INSN2(In, in, makeRegisterEffective(s, 0), makeRegister(2, rmWord));}
      case 0xEE: {return MAKE_INSN2(Out, out, makeRegister(2, rmWord), makeRegister(0, rmLegacyByte));}
      case 0xEF: {return MAKE_INSN2(Out, out, makeRegister(2, rmWord), makeRegisterEffective(s, 0));}
      case 0xF0: {s.lock = true; goto decodeOpcodeNormal;}
      case 0xF1: ROSE_ASSERT (!"Not supported by ROSE"); // {return MAKE_INSN0(Int1, int1);}
      case 0xF2: {s.repeatPrefix = rpRepne; goto decodeOpcodeNormal;}
      case 0xF3: {s.repeatPrefix = rpRepe; goto decodeOpcodeNormal;}
      case 0xF4: {return MAKE_INSN0(Hlt, hlt);}
      case 0xF5: {return MAKE_INSN0(Cmc, cmc);}
      case 0xF6: {GET_MODRM_OPCODE_FOR_BYTE; SgAsmExpression* immMaybe = NULL; if (reg < 1) {GET_IMM_BYTE; immMaybe = imm;}; return decodeGroup3(s, reg, modrm, immMaybe);}
      case 0xF7: {GET_MODRM_OPCODE_FOR_XWORD; SgAsmExpression* immMaybe = NULL; if (reg < 1) {GET_IMM_Iz; immMaybe = imm;}; return decodeGroup3(s, reg, modrm, immMaybe);}
      case 0xF8: {return MAKE_INSN0(Clc, clc);}
      case 0xF9: {return MAKE_INSN0(Stc, stc);}
      case 0xFA: {return MAKE_INSN0(Cli, cli);}
      case 0xFB: {return MAKE_INSN0(Sti, sti);}
      case 0xFC: {return MAKE_INSN0(Cld, cld);}
      case 0xFD: {return MAKE_INSN0(Std, std);}
      case 0xFE: {GET_MODRM_OPCODE_FOR_BYTE; return decodeGroup4(s, reg, modrm);}
      case 0xFF: {GET_MODRM_OPCODE_FOR_XWORD; return decodeGroup5(s, reg, modrm);}
      default: ROSE_ASSERT (!"Should not get here");
    }
  }

  SgAsmx86Instruction* decodeGroup1(const State& s, uint8_t reg, SgAsmExpression* modrm, SgAsmExpression* imm) {
    switch (reg & 7) {
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

  SgAsmx86Instruction* decodeGroup1a(const State& s, uint8_t reg, SgAsmExpression* modrm) {
    if ((reg & 7) != 0) throw BadInstruction();
    return MAKE_INSN1(Pop, pop, modrm);
  }

  SgAsmx86Instruction* decodeGroup2(const State& s, uint8_t reg, SgAsmExpression* modrm, SgAsmExpression* count) {
    switch (reg & 7) {
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

  SgAsmx86Instruction* decodeGroup3(const State& s, uint8_t reg, SgAsmExpression* modrm, SgAsmExpression* immMaybe) {
    switch (reg & 7) {
      case 0:
      case 1: ROSE_ASSERT (immMaybe); return MAKE_INSN2(Test, test, modrm, immMaybe);
      case 2: return MAKE_INSN1(Not, not, modrm);
      case 3: return MAKE_INSN1(Neg, neg, modrm);
      case 4: return MAKE_INSN1(Mul, mul, modrm);
      case 5: ROSE_ASSERT (!"Not supported by ROSE"); // return MAKE_INSN1(Imul, imul, modrm);
      case 6: return MAKE_INSN1(Div, div, modrm);
      case 7: ROSE_ASSERT (!"Not supported by ROSE"); // return MAKE_INSN1(Idiv, idiv, modrm);
      default: ROSE_ASSERT (false);
    }
  }

  SgAsmx86Instruction* decodeGroup4(const State& s, uint8_t reg, SgAsmExpression* modrm) {
    switch (reg & 7) {
      case 0: return MAKE_INSN1(Inc, inc, modrm);
      case 1: return MAKE_INSN1(Dec, dec, modrm);
      default: throw BadInstruction();
    }
  }

  SgAsmx86Instruction* decodeGroup5(const State& s, uint8_t reg, SgAsmExpression* modrm) {
    switch (reg & 7) {
      case 0: return MAKE_INSN1(Inc, inc, modrm);
      case 1: return MAKE_INSN1(Dec, dec, modrm);
      case 2: return MAKE_INSN1(Call, call, modrm);
      case 3: ROSE_ASSERT (!"Not supported by ROSE"); // return MAKE_INSN1(FarCall, farCall, modrm);
      case 4: return MAKE_INSN1(Jmp, jmp, modrm);
      case 5: ROSE_ASSERT (!"Not supported by ROSE"); // return MAKE_INSN1(FarJmp, farJmp, modrm);
      case 6: return MAKE_INSN1(Push, push, modrm);
      case 7: throw BadInstruction();
      default: ROSE_ASSERT (false);
    }
  }

  SgAsmx86Instruction* decodeGroup11(const State& s, uint8_t reg, SgAsmExpression* modrm, SgAsmExpression* imm) {
    switch (reg & 7) {
      case 0: return MAKE_INSN2(Mov, mov, modrm, imm);
      default: throw BadInstruction();
    }
  }

  SgAsmx86Instruction* decodeX87InstructionD8(const State& s, const vector<uint8_t>& insn, size_t& positionInVector) {
    GET_MODRM_OPCODE_FOR_FLOAT;
    if (isSgAsmMemoryReferenceExpression(modrm)) {
      isSgAsmMemoryReferenceExpression(modrm)->set_type(SgAsmTypeSingleFloat::createType());
    }
    if (modregrm < 0xC0) { // Using memory
      switch (reg & 7) {
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
      switch (reg & 7) {
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

  SgAsmx86Instruction* decodeX87InstructionD9(const State& s, const vector<uint8_t>& insn, size_t& positionInVector) {
    uint8_t modregrm;
    GET_BYTE(modregrm);
    SgAsmExpression* modrm = NULL;
    uint8_t reg = (modregrm >> 3) & 7;
    ROSE_ASSERT (modrm);
    if (modregrm < 0xC0) {
      modrm = decodeModrmMemory(s, insn, positionInVector, modregrm);
      ROSE_ASSERT (isSgAsmMemoryReferenceExpression(modrm));
      isSgAsmMemoryReferenceExpression(modrm)->set_type(SgAsmTypeSingleFloat::createType());
      switch (reg) {
        case 0: return MAKE_INSN1(Fld, fld, modrm);
        case 1: throw BadInstruction();
        case 2: return MAKE_INSN1(Fst, fst, modrm);
        case 3: return MAKE_INSN1(Fstp, fstp, modrm);
        case 4: return MAKE_INSN1(Fldenv, fldenv, modrm);
        case 5: return MAKE_INSN1(Fldcw, fldcw, modrm);
        case 6: return MAKE_INSN1(Fnstenv, fnstenv, modrm);
        case 7: return MAKE_INSN1(Fnstcw, fnstcw, modrm);
        default: ROSE_ASSERT (false);
      }
    } else if (modregrm < 0xD0) { // FLD and FXCH on registers
      getModrmForFloat(s, insn, positionInVector, modregrm, modrm);
      switch (reg) {
        case 0: return MAKE_INSN2(Fld, fld, makeRegister(0, rmST), modrm);
        case 1: return MAKE_INSN2(Fxch, fxch, makeRegister(0, rmST), modrm);
        default: ROSE_ASSERT (false);
      }
    } else {
      switch (modregrm) {
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

  SgAsmx86Instruction* decodeX87InstructionDA(const State& s, const vector<uint8_t>& insn, size_t& positionInVector) {
    uint8_t modregrm;
    GET_BYTE(modregrm);
    SgAsmExpression* modrm = NULL;
    uint8_t reg = (modregrm >> 3) & 7;
    ROSE_ASSERT (modrm);
    if (modregrm < 0xC0) {
      modrm = decodeModrmMemory(s, insn, positionInVector, modregrm);
      ROSE_ASSERT (isSgAsmMemoryReferenceExpression(modrm));
      isSgAsmMemoryReferenceExpression(modrm)->set_type(SgAsmTypeDoubleWord::createType());
      switch (reg) {
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
    } else if (modregrm < 0xE0) { // FCMOV{B,E,BE,U}
      getModrmForFloat(s, insn, positionInVector, modregrm, modrm);
      switch (reg) {
        case 0: return MAKE_INSN2(Fcmovb, fcmovb, makeRegister(0, rmST), modrm);
        case 1: return MAKE_INSN2(Fcmove, fcmove, makeRegister(0, rmST), modrm);
        case 2: return MAKE_INSN2(Fcmovbe, fcmovbe, makeRegister(0, rmST), modrm);
        case 3: return MAKE_INSN2(Fcmovu, fcmovu, makeRegister(0, rmST), modrm);
        default: ROSE_ASSERT (false);
      }
    } else {
      switch (modregrm) {
        case 0xE9: return MAKE_INSN0(Fucompp, fucompp);
        default: throw BadInstruction();
      }
    }
  }

  SgAsmx86Instruction* decodeX87InstructionDB(const State& s, const vector<uint8_t>& insn, size_t& positionInVector) {
    uint8_t modregrm;
    GET_BYTE(modregrm);
    SgAsmExpression* modrm = NULL;
    uint8_t reg = (modregrm >> 3) & 7;
    ROSE_ASSERT (modrm);
    if (modregrm < 0xC0) {
      modrm = decodeModrmMemory(s, insn, positionInVector, modregrm);
      ROSE_ASSERT (isSgAsmMemoryReferenceExpression(modrm));
      if (reg <= 3) {
        isSgAsmMemoryReferenceExpression(modrm)->set_type(SgAsmTypeDoubleWord::createType());
      } else {
        isSgAsmMemoryReferenceExpression(modrm)->set_type(SgAsmType80bitFloat::createType());
      }
      switch (reg) {
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
    } else if (reg <= 3 || reg == 5 || reg == 6) { // FCMOV{NB,NE,NBE,NU}, FUCOMI, FCOMI
      getModrmForFloat(s, insn, positionInVector, modregrm, modrm);
      switch (reg) {
        case 0: return MAKE_INSN2(Fcmovnb, fcmovnb, makeRegister(0, rmST), modrm);
        case 1: return MAKE_INSN2(Fcmovne, fcmovne, makeRegister(0, rmST), modrm);
        case 2: return MAKE_INSN2(Fcmovnbe, fcmovnbe, makeRegister(0, rmST), modrm);
        case 3: return MAKE_INSN2(Fcmovnu, fcmovnu, makeRegister(0, rmST), modrm);
        case 5: return MAKE_INSN2(Fucomi, fucomi, makeRegister(0, rmST), modrm);
        case 6: return MAKE_INSN2(Fcomi, fcomi, makeRegister(0, rmST), modrm);
        default: ROSE_ASSERT (false);
      }
    } else {
      switch (modregrm) {
        case 0xE2: return MAKE_INSN0(Fnclex, fnclex);
        case 0xE3: return MAKE_INSN0(Fninit, fninit);
        default: throw BadInstruction();
      }
    }
  }

  SgAsmx86Instruction* decodeX87InstructionDC(const State& s, const vector<uint8_t>& insn, size_t& positionInVector) {
    GET_MODRM_OPCODE_FOR_FLOAT;
    if (isSgAsmMemoryReferenceExpression(modrm)) {
      isSgAsmMemoryReferenceExpression(modrm)->set_type(SgAsmTypeDoubleFloat::createType());
    }
    if (modregrm < 0xC0) { // Using memory
      switch (reg & 7) {
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
      switch (reg & 7) {
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

  SgAsmx86Instruction* decodeX87InstructionDD(const State& s, const vector<uint8_t>& insn, size_t& positionInVector) {
    GET_MODRM_OPCODE_FOR_FLOAT;
    if (isSgAsmMemoryReferenceExpression(modrm)) {
      if (reg == 1) {
        isSgAsmMemoryReferenceExpression(modrm)->set_type(SgAsmTypeQuadWord::createType());
      } else if (reg <= 3) {
        isSgAsmMemoryReferenceExpression(modrm)->set_type(SgAsmTypeDoubleFloat::createType());
      } else if (reg <= 6) {
        isSgAsmMemoryReferenceExpression(modrm)->set_type(SgAsmTypeByte::createType()); // FIXME -- mem98/108env
      } else if (reg == 7) {
        isSgAsmMemoryReferenceExpression(modrm)->set_type(SgAsmTypeWord::createType());
      }
    }
    if (modregrm < 0xC0) { // Using memory
      switch (reg & 7) {
        case 0: return MAKE_INSN1(Fld, fld, modrm);
        case 1: return MAKE_INSN1(Fisttp, fisttp, modrm);
        case 2: return MAKE_INSN1(Fst, fst, modrm);
        case 3: return MAKE_INSN1(Fstp, fstp, modrm);
        case 4: return MAKE_INSN1(Frstor, frstor, modrm);
        case 5: throw BadInstruction();
        case 6: return MAKE_INSN1(Fnsave, fnsave, modrm);
        case 7: return MAKE_INSN1(Fnstsw, fnstsw, modrm);
        default: ROSE_ASSERT (false);
      }
    } else { // Two-operand register forms
      switch (reg & 7) {
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

  SgAsmx86Instruction* decodeX87InstructionDE(const State& s, const vector<uint8_t>& insn, size_t& positionInVector) {
    GET_MODRM_OPCODE_FOR_FLOAT;
    if (isSgAsmMemoryReferenceExpression(modrm)) {
      isSgAsmMemoryReferenceExpression(modrm)->set_type(SgAsmTypeWord::createType());
    }
    if (modregrm < 0xC0) { // Using memory
      switch (reg & 7) {
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
    } else { // Two-operand register forms
      switch (reg & 7) {
        case 0: return MAKE_INSN2(Faddp, faddp, modrm, makeRegister(0, rmST));
        case 1: return MAKE_INSN2(Fmulp, fmulp, modrm, makeRegister(0, rmST));
        case 2: throw BadInstruction();
        case 3: {
          switch (modregrm) {
            case 0xD9: return MAKE_INSN0(Fcompp, fcompp);
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

  SgAsmx86Instruction* decodeX87InstructionDF(const State& s, const vector<uint8_t>& insn, size_t& positionInVector) {
    GET_MODRM_OPCODE_FOR_FLOAT;
    if (isSgAsmMemoryReferenceExpression(modrm)) {
      if (reg <= 3) {
        isSgAsmMemoryReferenceExpression(modrm)->set_type(SgAsmTypeWord::createType());
      } else if (reg == 4 || reg == 6) {
        isSgAsmMemoryReferenceExpression(modrm)->set_type(SgAsmTypeByte::createType()); // FIXME -- mem80dec
      } else {
        isSgAsmMemoryReferenceExpression(modrm)->set_type(SgAsmTypeQuadWord::createType());
      }
    }
    if (modregrm < 0xC0) { // Using memory
      switch (reg & 7) {
        case 0: return MAKE_INSN1(Fild, fild, modrm);
        case 1: return MAKE_INSN1(Fisttp, fisttp, modrm);
        case 2: return MAKE_INSN1(Fist, fist, modrm);
        case 3: return MAKE_INSN1(Fistp, fistp, modrm);
        case 4: return MAKE_INSN1(Fbld, fbld, modrm);
        case 5: return MAKE_INSN1(Fild, fild, modrm);
        case 6: return MAKE_INSN1(Fbstp, fbstp, modrm);
        case 7: return MAKE_INSN1(Fistp, fistp, modrm);
        default: ROSE_ASSERT (false);
      }
    } else {
      switch (reg & 7) {
        case 0: throw BadInstruction();
        case 1: throw BadInstruction();
        case 2: throw BadInstruction();
        case 3: throw BadInstruction();
        case 4: {
          if (modregrm == 0xE0) {
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

  SgAsmx86Instruction* decodeOpcode0F(const State& s) {
    GET_BYTE(opcode);
    uint8_t low3bits = opcode & 7;
    uint8_t high5bits = opcode >> 3;
    switch (high5bits) {
      case (0x00 >> 3): {
        switch (low3bits) {
          case 0: {GET_MODRM_OPCODE_FOR_WORD; return decodeGroup6(s, reg, modrm);}
          case 1: return decodeGroup7(s);
          case 2: {GET_MODRM_FOR_WORD; return MAKE_INSN2(Lar, lar, reg, modrm);}
          case 3: {GET_MODRM_FOR_WORD; return MAKE_INSN2(Lsl, lsl, reg, modrm);}
          case 4: throw BadInstruction();
          case 5: return MAKE_INSN0(Syscall, syscall);
          case 6: return MAKE_INSN0(Clts, clts);
          case 7: return MAKE_INSN0(Sysret, sysret);
          default: ROSE_ASSERT (false);
        }
      }
      case (0x08 >> 3): {
        switch (low3bits) {
          case 0: return MAKE_INSN0(Invd, invd);
          case 1: return MAKE_INSN0(Wbinvd, wbinvd);
          case 2: throw BadInstruction();
          case 3: return MAKE_INSN0(Ud2, ud2);
          case 4: throw BadInstruction();
          case 5: return decodeGroupP(s);
          case 6: return MAKE_INSN0(Femms, femms);
          case 7: ROSE_ASSERT (!"3DNow! not supported 0F0F");
          default: ROSE_ASSERT (false);
        }
      }
      case (0x10 >> 3): ROSE_ASSERT (!"SIMD not supported 0F10");
      case (0x18 >> 3): {
        switch (low3bits) {
          case 0: {GET_MODRM_FOR_BYTE; return decodeGroup16(s, reg, modrm);}
          default: throw BadInstruction();
        }
      }
      case (0x20 >> 3): ROSE_ASSERT (!"C/D regs not supported 0F20");
      case (0x28 >> 3): ROSE_ASSERT (!"SIMD not supported 0F28");
      case (0x30 >> 3): {
        switch (low3bits) {
          case 0: return MAKE_INSN0(Wrmsr, wrmsr);
          case 1: return MAKE_INSN0(Rdtsc, rdtsc);
          case 2: return MAKE_INSN0(Rdmsr, rdmsr);
          case 3: return MAKE_INSN0(Rdpmc, rdpmc);
          case 4: NOT_64; return MAKE_INSN0(Sysenter, sysenter);
          case 5: NOT_64; return MAKE_INSN0(Sysexit, sysexit);
          case 6: throw BadInstruction();
          case 7: throw BadInstruction();
          default: ROSE_ASSERT (false);
        }
      }
      case (0x38 >> 3): throw BadInstruction();
      case (0x40 >> 3): {
        GET_MODRM_FOR_XWORD;
        switch (low3bits) {
          case 0: return MAKE_INSN2(Cmovo, cmovo, reg, modrm);
          case 1: return MAKE_INSN2(Cmovno, cmovno, reg, modrm);
          case 2: return MAKE_INSN2(Cmovb, cmovb, reg, modrm);
          case 3: return MAKE_INSN2(Cmovae, cmovae, reg, modrm);
          case 4: return MAKE_INSN2(Cmove, cmove, reg, modrm);
          case 5: return MAKE_INSN2(Cmovne, cmovne, reg, modrm);
          case 6: return MAKE_INSN2(Cmovbe, cmovbe, reg, modrm);
          case 7: return MAKE_INSN2(Cmova, cmova, reg, modrm);
          default: ROSE_ASSERT (false);
        }
      }
      case (0x48 >> 3): {
        GET_MODRM_FOR_XWORD;
        switch (low3bits) {
          case 0: return MAKE_INSN2(Cmovs, cmovs, reg, modrm);
          case 1: return MAKE_INSN2(Cmovns, cmovns, reg, modrm);
          case 2: return MAKE_INSN2(Cmovpe, cmovpe, reg, modrm);
          case 3: return MAKE_INSN2(Cmovpo, cmovpo, reg, modrm);
          case 4: return MAKE_INSN2(Cmovl, cmovl, reg, modrm);
          case 5: return MAKE_INSN2(Cmovge, cmovge, reg, modrm);
          case 6: return MAKE_INSN2(Cmovle, cmovle, reg, modrm);
          case 7: return MAKE_INSN2(Cmovg, cmovg, reg, modrm);
          default: ROSE_ASSERT (false);
        }
      }
      case (0x50 >> 3): ROSE_ASSERT (!"SIMD not supported 0F50");
      case (0x58 >> 3): ROSE_ASSERT (!"SIMD not supported 0F58");
      case (0x60 >> 3): ROSE_ASSERT (!"SIMD not supported 0F60");
      case (0x68 >> 3): ROSE_ASSERT (!"SIMD not supported 0F68");
      case (0x70 >> 3): ROSE_ASSERT (!"SIMD not supported 0F70");
      case (0x78 >> 3): ROSE_ASSERT (!"SIMD not supported 0F78");
      case (0x80 >> 3): {
        GET_IMM_Jz;
        switch (low3bits) {
          case 0: {GET_IMM_Jz; GET_BRANCH_PREDICTION; return MAKE_INSN1(Jo , jo, /* prediction, */ imm);}
          case 1: {GET_IMM_Jz; GET_BRANCH_PREDICTION; return MAKE_INSN1(Jno, jno, /* prediction, */ imm);}
          case 2: {GET_IMM_Jz; GET_BRANCH_PREDICTION; return MAKE_INSN1(Jb,  jb, /* prediction, */ imm);}
          case 3: {GET_IMM_Jz; GET_BRANCH_PREDICTION; return MAKE_INSN1(Jae, jae, /* prediction, */ imm);}
          case 4: {GET_IMM_Jz; GET_BRANCH_PREDICTION; return MAKE_INSN1(Je,  je, /* prediction, */ imm);}
          case 5: {GET_IMM_Jz; GET_BRANCH_PREDICTION; return MAKE_INSN1(Jne, jne, /* prediction, */ imm);}
          case 6: {GET_IMM_Jz; GET_BRANCH_PREDICTION; return MAKE_INSN1(Jbe, jbe, /* prediction, */ imm);}
          case 7: {GET_IMM_Jz; GET_BRANCH_PREDICTION; return MAKE_INSN1(Ja,  ja, /* prediction, */ imm);}
          default: ROSE_ASSERT (false);
        }
      }
      case (0x88 >> 3): {
        GET_MODRM_FOR_XWORD;
        switch (low3bits) {
          case 0: {GET_IMM_Jz; GET_BRANCH_PREDICTION; return MAKE_INSN1(Js , js, /* prediction, */ imm);}
          case 1: {GET_IMM_Jz; GET_BRANCH_PREDICTION; return MAKE_INSN1(Jns, jns, /* prediction, */ imm);}
          case 2: {GET_IMM_Jz; GET_BRANCH_PREDICTION; return MAKE_INSN1(Jpe, jpe, /* prediction, */ imm);}
          case 3: {GET_IMM_Jz; GET_BRANCH_PREDICTION; return MAKE_INSN1(Jpo, jpo, /* prediction, */ imm);}
          case 4: {GET_IMM_Jz; GET_BRANCH_PREDICTION; return MAKE_INSN1(Jl,  jl, /* prediction, */ imm);}
          case 5: {GET_IMM_Jz; GET_BRANCH_PREDICTION; return MAKE_INSN1(Jge, jge, /* prediction, */ imm);}
          case 6: {GET_IMM_Jz; GET_BRANCH_PREDICTION; return MAKE_INSN1(Jle, jle, /* prediction, */ imm);}
          case 7: {GET_IMM_Jz; GET_BRANCH_PREDICTION; return MAKE_INSN1(Jg,  jg, /* prediction, */ imm);}
          default: ROSE_ASSERT (false);
        }
      }
      case (0x90 >> 3): {
        GET_MODRM_FOR_BYTE;
        switch (low3bits) {
          case 0: return MAKE_INSN1(Seto,  seto, modrm);
          case 1: return MAKE_INSN1(Setno, setno, modrm);
          case 2: return MAKE_INSN1(Setb,  setb, modrm);
          case 3: return MAKE_INSN1(Setae, setae, modrm);
          case 4: return MAKE_INSN1(Sete,  sete, modrm);
          case 5: return MAKE_INSN1(Setne, setne, modrm);
          case 6: return MAKE_INSN1(Setbe, setbe, modrm);
          case 7: return MAKE_INSN1(Seta,  seta, modrm);
          default: ROSE_ASSERT (false);
        }
      }
      case (0x98 >> 3): {
        GET_MODRM_FOR_BYTE;
        switch (low3bits) {
          case 0: return MAKE_INSN1(Sets,  sets, modrm);
          case 1: return MAKE_INSN1(Setns, setns, modrm);
          case 2: return MAKE_INSN1(Setpe, setpe, modrm);
          case 3: return MAKE_INSN1(Setpo, setpo, modrm);
          case 4: return MAKE_INSN1(Setl,  setl, modrm);
          case 5: return MAKE_INSN1(Setge, setge, modrm);
          case 6: return MAKE_INSN1(Setle, setle, modrm);
          case 7: return MAKE_INSN1(Setg,  setg, modrm);
          default: ROSE_ASSERT (false);
        }
      }
      case (0xA0 >> 3): 
      case (0xA8 >> 3): 
      case (0xB0 >> 3): 
      case (0xB8 >> 3): 
      case (0xC0 >> 3): ROSE_ASSERT (!"SIMD not supported 0FC0");
      case (0xC8 >> 3): return MAKE_INSN1(Bswap, bswap, makeRegisterEffective(s, s.rexB, low3bits));
      case (0xD0 >> 3): ROSE_ASSERT (!"SIMD not supported 0FD0");
      case (0xD8 >> 3): ROSE_ASSERT (!"SIMD not supported 0FD8");
      case (0xE0 >> 3): ROSE_ASSERT (!"SIMD not supported 0FE0");
      case (0xE8 >> 3): ROSE_ASSERT (!"SIMD not supported 0FE8");
      case (0xF0 >> 3): ROSE_ASSERT (!"SIMD not supported 0FF0");
      case (0xF8 >> 3): ROSE_ASSERT (!"SIMD not supported 0FF8");
      default: ROSE_ASSERT (false);
    }
  }
}
#endif
