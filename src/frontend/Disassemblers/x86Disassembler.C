#include <stdint.h>
#include <fcntl.h>
#include <vector>
#include "rose.h"
#include "integerOps.h"

using namespace std;
using namespace SageBuilderAsm;
using namespace IntegerOps;

// These are macros to make them look like constants while they are really
// function calls
#define BYTET (SgAsmTypeByte::createType())
#define WORDT (SgAsmTypeWord::createType())
#define DWORDT (SgAsmTypeDoubleWord::createType())
#define QWORDT (SgAsmTypeQuadWord::createType())
#define DQWORDT (SgAsmTypeDoubleQuadWord::createType())
#define FLOATT (SgAsmTypeSingleFloat::createType())
#define DOUBLET (SgAsmTypeDoubleFloat::createType())
#define LDOUBLET (SgAsmType80bitFloat::createType())
#define V8BYTET (SgAsmTypeVector::createType(8, BYTET))
#define V16BYTET (SgAsmTypeVector::createType(16, BYTET))
#define V4WORDT (SgAsmTypeVector::createType(4, WORDT))
#define V8WORDT (SgAsmTypeVector::createType(8, WORDT))
#define V2DWORDT (SgAsmTypeVector::createType(2, DWORDT))
#define V4DWORDT (SgAsmTypeVector::createType(4, DWORDT))
#define V2FLOATT (SgAsmTypeVector::createType(2, FLOATT))
#define V4FLOATT (SgAsmTypeVector::createType(4, FLOATT))
#define V2QWORDT (SgAsmTypeVector::createType(2, QWORDT))
#define V2DOUBLET (SgAsmTypeVector::createType(2, DOUBLET))


// Work left to do:
//   1) Fix existing FIXMEs and ASSERT's within the current implementation.
//   2) We need SgAsmTypes in the IR for more data types: Far Pointers, 
//      FPU environment (m14/28 byte, 14 bytes for 16 bit mode and 28 bytes for 32 bit mode),
//      FPU State (m94/108 byte see section about "fsave" page 3-372)
//      ..., also m16&16 (16bit pair of operands for bounds checking: "bound"),
//      also versions for 32bit and 64bit. (See section 3.1.1.2).
//   


// See section 9.11 in Volume 3A of Intel for details of how to store new microcode into the processor.


namespace X86Disassembler {

  enum RepeatPrefix {rpNone, rpRepne, rpRepe};
  enum MMPrefix {mmNone, mmF3, mm66, mmF2};

  enum RegisterMode {rmLegacyByte, rmRexByte, rmWord, rmDWord, rmQWord, rmSegment, rmST, rmMM, rmXMM, rmControl, rmDebug, rmReturnNull /* For mod r/m settings that should not create register exprs */};

  RegisterMode sizeToMode(X86InstructionSize s) {
    switch (s) {
      case x86_insnsize_16: return rmWord;
      case x86_insnsize_32: return rmDWord;
      case x86_insnsize_64: return rmQWord;
      default: ROSE_ASSERT (false);
    }
  }

  SgAsmx86RegisterReferenceExpression* makeRegister(uint8_t fullRegisterNumber, RegisterMode m, SgAsmType* registerType = NULL /* Only used for vector registers that can have more than one type */);

  SgAsmExpression* makeSegmentRegister(X86SegmentRegister so, bool insn64) {
    switch (so) {
      case x86_segreg_none: ROSE_ASSERT (!"makeSegmentRegister does not support x86_segreg_none");
      case x86_segreg_cs: return makeRegister(insn64 ? x86_segreg_ds : x86_segreg_cs, rmSegment);
      case x86_segreg_ds: return makeRegister(x86_segreg_ds, rmSegment);
      case x86_segreg_es: return makeRegister(insn64 ? x86_segreg_ds : x86_segreg_es, rmSegment);
      case x86_segreg_fs: return makeRegister(x86_segreg_fs, rmSegment);
      case x86_segreg_gs: return makeRegister(x86_segreg_gs, rmSegment);
      case x86_segreg_ss: return makeRegister(insn64 ? x86_segreg_ds : x86_segreg_ss, rmSegment);
      default: ROSE_ASSERT (!"Bad segment register in makeSegmentRegister");
    }
  }

  SgAsmx86Instruction* makeInstructionWithoutOperands(uint64_t address, const std::string& mnemonic, X86InstructionKind kind, X86InstructionSize baseSize, X86InstructionSize operandSize, X86InstructionSize addressSize, bool lockPrefix) {

 // DQ (8/23/2008): Moved comment support to SgAsmStatement since we can't yet support functions and I need to at least comment the AST as to were the functions are.
 // SgAsmx86Instruction* instruction = new SgAsmx86Instruction(address, mnemonic, "", kind, baseSize, operandSize, addressSize);
    SgAsmx86Instruction* instruction = new SgAsmx86Instruction(address, mnemonic, kind, baseSize, operandSize, addressSize);
    ROSE_ASSERT (instruction);
    instruction->set_lockPrefix(lockPrefix);
    return instruction;
  }

  SgAsmx86RegisterReferenceExpression* makeRegister(uint8_t fullRegisterNumber, RegisterMode m, SgAsmType* registerType /* Only used for vector registers that can have more than one type */) {
    if (m == rmReturnNull) return NULL;
    SgAsmx86RegisterReferenceExpression* ref = NULL;
    switch (m) {
      case rmLegacyByte: {
        ref = new SgAsmx86RegisterReferenceExpression(x86_regclass_gpr, fullRegisterNumber % 4);
        ref->set_position_in_register((fullRegisterNumber & 4) ? x86_regpos_high_byte : x86_regpos_low_byte);
        ref->set_type(BYTET);
        break;
      }
      case rmRexByte: {
        ref = new SgAsmx86RegisterReferenceExpression(x86_regclass_gpr, fullRegisterNumber);
        ref->set_position_in_register(x86_regpos_low_byte);
        ref->set_type(BYTET);
        break;
      }
      case rmWord: {
        ref = new SgAsmx86RegisterReferenceExpression(x86_regclass_gpr, fullRegisterNumber);
        ref->set_position_in_register(x86_regpos_word);
        ref->set_type(WORDT);
        break;
      }
      case rmDWord: {
        ref = new SgAsmx86RegisterReferenceExpression(x86_regclass_gpr, fullRegisterNumber);
        ref->set_position_in_register(x86_regpos_dword);
        ref->set_type(DWORDT);
        break;
      }
      case rmQWord: {
        ref = new SgAsmx86RegisterReferenceExpression(x86_regclass_gpr, fullRegisterNumber);
        ref->set_position_in_register(x86_regpos_qword);
        ref->set_type(QWORDT);
        break;
      }
      case rmSegment: {
        if (fullRegisterNumber >= 6) throw BadInstruction();
        ref = new SgAsmx86RegisterReferenceExpression(x86_regclass_segment, fullRegisterNumber);
        ref->set_position_in_register(x86_regpos_all);
        ref->set_type(WORDT);
        break;
      }
      case rmST: {
        ref = new SgAsmx86RegisterReferenceExpression(x86_regclass_st, fullRegisterNumber);
        ref->set_position_in_register(x86_regpos_all);
        ref->set_type(LDOUBLET);
        break;
      }
      case rmMM: {
        ref = new SgAsmx86RegisterReferenceExpression(x86_regclass_mm, fullRegisterNumber);
        ref->set_position_in_register(x86_regpos_all);
        ROSE_ASSERT (registerType);
        ref->set_type(registerType);
        break;
      }
      case rmXMM: {
        ref = new SgAsmx86RegisterReferenceExpression(x86_regclass_xmm, fullRegisterNumber);
        ref->set_position_in_register(x86_regpos_all);
        ROSE_ASSERT (registerType);
        ref->set_type(registerType);
        break;
      }
      case rmControl: {
        ref = new SgAsmx86RegisterReferenceExpression(x86_regclass_cr, fullRegisterNumber);
        ref->set_position_in_register(x86_regpos_all);
        ROSE_ASSERT (registerType);
        ref->set_type(registerType);
        break;
      }
      case rmDebug: {
        ref = new SgAsmx86RegisterReferenceExpression(x86_regclass_dr, fullRegisterNumber);
        ref->set_position_in_register(x86_regpos_all);
        ROSE_ASSERT (registerType);
        ref->set_type(registerType);
        break;
      }
      default: ROSE_ASSERT (false);
    }
    ROSE_ASSERT (ref);
    return ref;
  }

  struct SingleInstructionDisassembler {
    // Stuff that is not changed during the course of the instruction
    Parameters p;
    set<uint64_t>* knownSuccessorsReturn;

    // The instruction
    const uint8_t* const insn;
    const size_t insnSize;
    size_t positionInVector;

    // Temporary flags set by the instruction
    size_t positionInInstruction;
    X86SegmentRegister segOverride;
    X86BranchPrediction branchPrediction;
    bool branchPredictionEnabled;
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
    bool isUnconditionalJump; // True for jmp, farjmp, ret, retf, iret, and hlt; false for everything else

    SingleInstructionDisassembler(const Parameters& p, const uint8_t* const insn, const uint64_t insnSize, size_t positionInVector, set<uint64_t>* knownSuccessorsReturn):
      p(p),
      knownSuccessorsReturn(knownSuccessorsReturn),
      insn(insn),
      insnSize(insnSize),
      positionInVector(positionInVector),
      positionInInstruction(0),
      segOverride(x86_segreg_none),
      branchPrediction(x86_branch_prediction_none),
      branchPredictionEnabled(false),
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
      reg(NULL),
      isUnconditionalJump(false)
    {}

    X86InstructionSize effectiveAddressSize() const {
      if (addressSizeOverride) {
        switch (p.insnSize) {
          case x86_insnsize_16: return x86_insnsize_32;
          case x86_insnsize_32: return x86_insnsize_16;
          case x86_insnsize_64: return x86_insnsize_32;
          default: ROSE_ASSERT (false);
        }
      } else {
        return p.insnSize;
      }
    }

    X86InstructionSize effectiveOperandSize() const {
      if (operandSizeOverride) {
        switch (p.insnSize) {
          case x86_insnsize_16: return x86_insnsize_32;
          case x86_insnsize_32: return x86_insnsize_16;
          case x86_insnsize_64: return x86_insnsize_16;
          default: ROSE_ASSERT (false);
        }
      } else {
        X86InstructionSize s = p.insnSize;
        if (p.insnSize == x86_insnsize_64 && !rexW && !sizeMustBe64Bit) s = x86_insnsize_32;
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

    MMPrefix mmPrefix() const {
      switch (repeatPrefix) {
        case rpNone: {
          if (operandSizeOverride) {
            return mm66;
          } else {
            return mmNone;
          }
        }
        case rpRepne: {
          if (operandSizeOverride) {
            throw BadInstruction();
          } else {
            return mmF2;
          }
        }
        case rpRepe: {
          if (operandSizeOverride) {
            throw BadInstruction();
          } else {
            return mmF3;
          }
        }
        default: ROSE_ASSERT (false);
      }
    }

    SgAsmExpression* currentDataSegment() const {
      if (segOverride == x86_segreg_none) {
        return makeSegmentRegister(x86_segreg_ds, p.insnSize == x86_insnsize_64);
      } else {
        return makeSegmentRegister(segOverride, p.insnSize == x86_insnsize_64);
      }
    }

    void getByte(uint8_t& var) {
      if (positionInInstruction >= 15)
        throw BadInstruction();
      if (positionInVector + positionInInstruction >= insnSize)
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

    void getModRegRM(RegisterMode regMode, RegisterMode rmMode, SgAsmType* t, SgAsmType* tForReg = NULL) {

   // "RM" stands for Register or Memory and "Mod" is mode).
   // First parameter is the register kind for the reg field of the modregrm byte.
   // Second parameter is the register kind for the RM field when the mod refers to a register.

      if (!tForReg) {tForReg = t;}
      getByte(modregrmByte);
      modregrmByteSet = true;
      modeField = modregrmByte >> 6;
      regField = (modregrmByte & 070) >> 3;
      rmField = modregrmByte & 7;

      reg = makeModrmRegister(regMode, tForReg);
      modrm = makeModrmNormal(rmMode, t);
    }

    // If modrm is a memory reference, fill in its type; otherwise, make a
    // register with the appropriate mode and put it into modrm
    void fillInModRM(RegisterMode rmMode, SgAsmType* t) {
      if (modeField == 3) {
        modrm = makeRegister((rexB ? 8 : 0) + rmField, rmMode, t);
      } else {
        isSgAsmMemoryReferenceExpression(modrm)->set_type(t);
      }
    }

    SgAsmx86RegisterReferenceExpression* makeOperandRegisterByte(bool rexExtension, uint8_t registerNumber) {
      SgAsmx86RegisterReferenceExpression* ref = makeRegister((rexExtension ? 8 : 0) + registerNumber, (rexPresent ? rmRexByte : rmLegacyByte));
      return ref;
    }

    SgAsmx86RegisterReferenceExpression* makeOperandRegisterFull(bool rexExtension, uint8_t registerNumber) {
      SgAsmx86RegisterReferenceExpression* ref = makeRegister((rexExtension ? 8 : 0) + registerNumber, sizeToMode(p.insnSize));
      return ref;
    }

    SgAsmx86RegisterReferenceExpression* makeIP() {
      SgAsmx86RegisterReferenceExpression* r = new SgAsmx86RegisterReferenceExpression(x86_regclass_ip, 0);
      r->set_position_in_register(sizeToPos(p.insnSize));
      r->set_type(sizeToType(p.insnSize));
      return r;
    }

    SgAsmExpression* makeAddrSizeValue(int64_t val) {
      switch (effectiveAddressSize()) {
        case x86_insnsize_16: return makeWordValue((uint16_t)val);
        case x86_insnsize_32: return makeDWordValue((uint32_t)val);
        case x86_insnsize_64: return makeQWordValue((uint64_t)val);
        default: ROSE_ASSERT (false);
      }
    }

    SgAsmMemoryReferenceExpression* decodeModrmMemory() {
      ROSE_ASSERT (modregrmByteSet);
      SgAsmExpression* addressExpr = NULL;
      X86SegmentRegister defaultSeg = x86_segreg_ds;
      if (effectiveAddressSize() == x86_insnsize_16) {
        if (modeField == 0 && rmField == 6) { // Special case
          uint16_t offset;
          getWord(offset);
          addressExpr = makeWordValue(offset);
        } else {
          switch (rmField) {
            case 0: defaultSeg = x86_segreg_ds; addressExpr = makeAdd(makeRegister(3, rmWord), makeRegister(6, rmWord)); break;
            case 1: defaultSeg = x86_segreg_ds; addressExpr = makeAdd(makeRegister(3, rmWord), makeRegister(7, rmWord)); break;
            case 2: defaultSeg = x86_segreg_ss; addressExpr = makeAdd(makeRegister(5, rmWord), makeRegister(6, rmWord)); break;
            case 3: defaultSeg = x86_segreg_ss; addressExpr = makeAdd(makeRegister(5, rmWord), makeRegister(7, rmWord)); break;
            case 4: defaultSeg = x86_segreg_ds; addressExpr = makeRegister(6, rmWord); break;
            case 5: defaultSeg = x86_segreg_ds; addressExpr = makeRegister(7, rmWord); break;
            case 6: defaultSeg = x86_segreg_ss; addressExpr = makeRegister(5, rmWord); break;
            case 7: defaultSeg = x86_segreg_ds; addressExpr = makeRegister(3, rmWord); break;
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
          addressExpr = makeAddrSizeValue(signExtend<32, 64>((uint64_t)offset));
          if (p.insnSize == x86_insnsize_64) {
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
                case 0: {uint32_t offset; getDWord(offset); sibBase = makeAddrSizeValue(signExtend<32, 64>((uint64_t)offset)); break;}
                case 1: {sibBase = makeRegister((rexB ? 13 : 5), sizeToMode(p.insnSize)); defaultSeg = x86_segreg_ss; break;}
                case 2: {sibBase = makeRegister((rexB ? 13 : 5), sizeToMode(p.insnSize)); defaultSeg = x86_segreg_ss; break;}
                default: ROSE_ASSERT (false);
              }
            } else {
              sibBase = makeOperandRegisterFull(rexB, sibBaseField);
              if (sibBaseField == 4) defaultSeg = x86_segreg_ss;
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
            if (rmField == 5) defaultSeg = x86_segreg_ss;
          }
          switch (modeField) {
            case 0: break; // No offset
            case 1: {uint8_t offset; getByte(offset); addressExpr = makeAdd(addressExpr, makeAddrSizeValue(signExtend<8, 64>((uint64_t)offset))); break;}
            case 2: {uint32_t offset; getDWord(offset); addressExpr = makeAdd(addressExpr, makeAddrSizeValue(signExtend<32, 64>((uint64_t)offset))); break;}
            default: ROSE_ASSERT (false);
          }
        }
      }
      ROSE_ASSERT (addressExpr);
      X86SegmentRegister seg;
      if (segOverride != x86_segreg_none) {
        seg = segOverride;
      } else {
        seg = defaultSeg;
      }
      SgAsmMemoryReferenceExpression* mr = makeMemoryReference(addressExpr, makeSegmentRegister(seg, p.insnSize == x86_insnsize_64));
      return mr;
    }

    SgAsmx86RegisterReferenceExpression* makeRegisterEffective(uint8_t fullRegisterNumber) {
      return makeRegister(fullRegisterNumber, effectiveOperandMode());
    }

    SgAsmx86RegisterReferenceExpression* makeRegisterEffective(bool rexExtension, uint8_t registerNumber) {
      return makeRegister(registerNumber + (rexExtension ? 8 : 0), effectiveOperandMode());
    }

    SgAsmx86RegisterReferenceExpression* makeModrmRegister(RegisterMode m, SgAsmType* mrType = NULL /* Only used for vector registers */ ) {
      ROSE_ASSERT (modregrmByteSet);
      if (m == rmLegacyByte && rexPresent) m = rmRexByte;
      return makeRegister((rexR ? 8 : 0) + regField, m, mrType);
    }

    SgAsmExpression* makeModrmNormal(RegisterMode m, SgAsmType* mrType) {
      ROSE_ASSERT (modregrmByteSet);
      if (modeField == 3) { // Register
        if (m == rmLegacyByte && rexPresent) m = rmRexByte;
        return makeRegister((rexB ? 8 : 0) + rmField, m, mrType);
      } else {
        SgAsmMemoryReferenceExpression* modrm = decodeModrmMemory();
        modrm->set_type(mrType);
        return modrm;
      }
    }

    SgAsmExpression* getModrmForByte() {
      return makeModrmNormal(rmLegacyByte, BYTET);
    }

    SgAsmExpression* getModrmForXword() {
      return makeModrmNormal(effectiveOperandMode(), effectiveOperandType());
    }

    SgAsmExpression* getModrmForWord() {
      return makeModrmNormal(rmWord, WORDT);
    }

    SgAsmExpression* getModrmForFloat(SgAsmType* t) {
      return makeModrmNormal(rmST, t);
    }

    SgAsmx86Instruction* makeNullaryInstruction(const string& mnemonic, X86InstructionKind kind) {
      SgAsmx86Instruction* newInsn = makeInstructionWithoutOperands(p.ip, mnemonic, kind, p.insnSize, effectiveOperandSize(), effectiveAddressSize(), lock);
      newInsn->set_raw_bytes(SgUnsignedCharList(insn + positionInVector, insn + positionInVector + positionInInstruction));

      SgAsmOperandList* operands = new SgAsmOperandList();
      newInsn->set_operandList(operands);
      operands->set_parent(newInsn);
      newInsn->set_segmentOverride(segOverride);
      if (branchPredictionEnabled) newInsn->set_branchPrediction(branchPrediction);
      return newInsn;
    }
#define MAKE_INSN0(Tag, Mne) (makeNullaryInstruction(#Mne, x86_##Tag))

    SgAsmx86Instruction* makeUnaryInstruction(const string& mnemonic, X86InstructionKind kind, SgAsmExpression* op1) {
      SgAsmx86Instruction* newInsn = makeNullaryInstruction(mnemonic, kind);
      appendOperand(newInsn, op1);
      return newInsn;
    }
#define MAKE_INSN1(Tag, Mne, Op1) (makeUnaryInstruction(#Mne, x86_##Tag, (Op1)))

    SgAsmx86Instruction* makeBinaryInstruction(const string& mnemonic, X86InstructionKind kind, SgAsmExpression* op1, SgAsmExpression* op2) {
      SgAsmx86Instruction* newInsn = makeNullaryInstruction(mnemonic, kind);
      appendOperand(newInsn, op1);
      appendOperand(newInsn, op2);
      return newInsn;
    }
#define MAKE_INSN2(Tag, Mne, Op1, Op2) (makeBinaryInstruction(#Mne, x86_##Tag, (Op1), (Op2)))

    SgAsmx86Instruction* makeTernaryInstruction(const string& mnemonic, X86InstructionKind kind, SgAsmExpression* op1, SgAsmExpression* op2, SgAsmExpression* op3) {
      SgAsmx86Instruction* newInsn = makeNullaryInstruction(mnemonic, kind);
      appendOperand(newInsn, op1);
      appendOperand(newInsn, op2);
      appendOperand(newInsn, op3);
      return newInsn;
    }
#define MAKE_INSN3(Tag, Mne, Op1, Op2, Op3) (makeTernaryInstruction(#Mne, x86_##Tag, (Op1), (Op2), (Op3)))

    SgAsmx86Instruction* makeQuaternaryInstruction(const string& mnemonic, X86InstructionKind kind, SgAsmExpression* op1, SgAsmExpression* op2, SgAsmExpression* op3, SgAsmExpression* op4) {
      SgAsmx86Instruction* newInsn = makeNullaryInstruction(mnemonic, kind);
      appendOperand(newInsn, op1);
      appendOperand(newInsn, op2);
      appendOperand(newInsn, op3);
      appendOperand(newInsn, op4);
      return newInsn;
    }
#define MAKE_INSN4(Tag, Mne, Op1, Op2, Op3, Op4) (makeQuaternaryInstruction(#Mne, x86_##Tag, (Op1), (Op2), (Op3), (Op4)))

    SgAsmExpression* makeFullSizeValue(uint64_t val) {
      switch (effectiveOperandSize()) {
        case x86_insnsize_16: return makeWordValue(val);
        case x86_insnsize_32: return makeDWordValue(val);
        case x86_insnsize_64: return makeQWordValue(val);
        default: ROSE_ASSERT (false);
      }
    }

    SgAsmx86Instruction* decodeOpcode0F();

 // DQ (12/3/2008): Added initial support for SSSE3
    SgAsmx86Instruction* decodeOpcode0F38();

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

    bool longMode() const {return p.insnSize == x86_insnsize_64;}

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
      if (effectiveOperandSize() == x86_insnsize_16) {
        return getImmWord();
      } else {
        return getImmDWord();
      }
    }

    SgAsmExpression* getImmIzAsIv() {
      switch (effectiveOperandSize()) {
        case x86_insnsize_16: return getImmWord();
        case x86_insnsize_32: return getImmDWord();
        case x86_insnsize_64: {
          uint32_t val;
          getDWord(val);
          return makeQWordValue(signExtend<32, 64>((uint64_t)val));
        }
        default: ROSE_ASSERT (false);
      }
    }

    SgAsmExpression* getImmIv() {
      switch (effectiveOperandSize()) {
        case x86_insnsize_16: return getImmWord();
        case x86_insnsize_32: return getImmDWord();
        case x86_insnsize_64: return getImmQWord();
        default: ROSE_ASSERT (false);
      }
    }

    SgAsmExpression* getImmByteAsIv() {
      uint8_t val;
      getByte(val);
      switch (effectiveOperandSize()) {
        case x86_insnsize_16: return makeWordValue(signExtend<8, 16>((uint64_t)val));
        case x86_insnsize_32: return makeDWordValue(signExtend<8, 32>((uint64_t)val));
        case x86_insnsize_64: return makeQWordValue(signExtend<8, 64>((uint64_t)val));
        default: ROSE_ASSERT (false);
      }
    }

    SgAsmExpression* getImmJb() {
      uint8_t val;
      getByte(val);
      uint64_t target = p.ip + positionInInstruction + signExtend<8, 64>((uint64_t)val);
      if (knownSuccessorsReturn) {
        knownSuccessorsReturn->insert(target);
      }
      return makeQWordValue(target);
    }

    SgAsmExpression* getImmJz() {
      uint64_t val;
      if (effectiveOperandSize() == x86_insnsize_16) {
        uint16_t val2;
        getWord(val2);
        val = signExtend<16, 64>((uint64_t)val2);
      } else {
        uint32_t val2;
        getDWord(val2);
        val = signExtend<32, 64>((uint64_t)val2);
      }
      uint64_t target = p.ip + positionInInstruction + val;
      if (knownSuccessorsReturn) {
        knownSuccessorsReturn->insert(target);
      }
      return makeQWordValue(target);
    }

    SgAsmExpression* getImmForAddr() {
      switch (effectiveAddressSize()) {
        case x86_insnsize_16: return getImmWord();
        case x86_insnsize_32: return getImmDWord();
        case x86_insnsize_64: return getImmQWord();
        default: ROSE_ASSERT (false);
      }
    }

    void requireMemory() const {
      if (modeField == 3) throw BadInstruction();
    }

  };

  SgAsmx86Instruction* disassemble(const Parameters& p, const uint8_t* const insn, const uint64_t insnSize, size_t positionInVector, set<uint64_t>* knownSuccessorsReturn) {
    SingleInstructionDisassembler dis(p, insn, insnSize, positionInVector, knownSuccessorsReturn);
    return dis.disassemble();
  }

  SgAsmx86Instruction* SingleInstructionDisassembler::disassemble() {

 // Subsets of the x86 instruction set left to be implements (not implemented).
 //    1) SSE 4 (4a, 4b, 4.1) To generate binary that uses these instructions use a modern version of the Intel compiler.
 //    2) AVX (Intel instructions, these are new and not available in any software yet (Dec,2008))
 //    3) SSE 5 (AMD instructions, these instructions are not available in any software yet (Dec,2008))
 //    4) SSSE 3 (Supplemental SSE 3) To generate binary that uses these instructions use a modern version of the Intel compiler.

 // Note that prefix handling is supported by state that is set and used in the parsing of the instrcution op codes.
 // This state is fully contained in the SingleInstructionDisassembler class.

 // Note that this function is called recursively to handle REX and Legacy prefixs 
 // (see appendix B of volume 2B of Intel 64 and IA-32 Architecute Software Developers Manual).
 // Also, the order of the prefix is not inforced by this design. See section 2.1.1 of volume 2A.

    uint8_t opcode;
    getByte(opcode);
    SgAsmx86Instruction* insn = 0;
    switch (opcode) {
      case 0x00: {getModRegRM(rmLegacyByte, rmLegacyByte, BYTET); insn = MAKE_INSN2(add, add, modrm, reg); goto done;}
      case 0x01: {getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); insn = MAKE_INSN2(add, add, modrm, reg); goto done;}
      case 0x02: {getModRegRM(rmLegacyByte, rmLegacyByte, BYTET); insn = MAKE_INSN2(add, add, reg, modrm); goto done;}
      case 0x03: {getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); insn = MAKE_INSN2(add, add, reg, modrm); goto done;}

   // Immediate values are read as required and acts as a recursive decent parser.
   // Function names are taken from the manual (Intel x86 Instruction Set Reference: Appendix A: Opcode Map)
      case 0x04: {SgAsmExpression* imm = getImmByteAsIv(); insn = MAKE_INSN2(add, add, makeRegister(0, rmLegacyByte), imm); goto done;}
      case 0x05: {SgAsmExpression* imm = getImmIzAsIv(); insn = MAKE_INSN2(add, add, makeRegisterEffective(0), imm); goto done;}
      case 0x06: {not64(); insn = MAKE_INSN1(push, push, makeRegister(0, rmSegment)); goto done;}
      case 0x07: {not64(); insn = MAKE_INSN1(pop, pop, makeRegister(0, rmSegment)); goto done;}
      case 0x08: {getModRegRM(rmLegacyByte, rmLegacyByte, BYTET); insn = MAKE_INSN2(or, or, modrm, reg); goto done;}
      case 0x09: {getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); insn = MAKE_INSN2(or, or, modrm, reg); goto done;}
      case 0x0A: {getModRegRM(rmLegacyByte, rmLegacyByte, BYTET); insn = MAKE_INSN2(or, or, reg, modrm); goto done;}
      case 0x0B: {getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); insn = MAKE_INSN2(or, or, reg, modrm); goto done;}
      case 0x0C: {SgAsmExpression* imm = getImmByteAsIv(); insn = MAKE_INSN2(or, or, makeRegister(0, rmLegacyByte), imm); goto done;}
      case 0x0D: {SgAsmExpression* imm = getImmIzAsIv(); insn = MAKE_INSN2(or, or, makeRegisterEffective(0), imm); goto done;}
      case 0x0E: {not64(); insn = MAKE_INSN1(push, push, makeRegister(1, rmSegment)); goto done;}
      case 0x0F: {insn = decodeOpcode0F(); goto done;}
      case 0x10: {getModRegRM(rmLegacyByte, rmLegacyByte, BYTET); insn = MAKE_INSN2(adc, adc, modrm, reg); goto done;}
      case 0x11: {getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); insn = MAKE_INSN2(adc, adc, modrm, reg); goto done;}
      case 0x12: {getModRegRM(rmLegacyByte, rmLegacyByte, BYTET); insn = MAKE_INSN2(adc, adc, reg, modrm); goto done;}
      case 0x13: {getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); insn = MAKE_INSN2(adc, adc, reg, modrm); goto done;}
      case 0x14: {SgAsmExpression* imm = getImmByteAsIv(); insn = MAKE_INSN2(adc, adc, makeRegister(0, rmLegacyByte), imm); goto done;}
      case 0x15: {SgAsmExpression* imm = getImmIzAsIv(); insn = MAKE_INSN2(adc, adc, makeRegisterEffective(0), imm); goto done;}
      case 0x16: {not64(); insn = MAKE_INSN1(push, push, makeRegister(2, rmSegment)); goto done;}
      case 0x17: {not64(); insn = MAKE_INSN1(pop, pop, makeRegister(2, rmSegment)); goto done;}
      case 0x18: {getModRegRM(rmLegacyByte, rmLegacyByte, BYTET); insn = MAKE_INSN2(sbb, sbb, modrm, reg); goto done;}
      case 0x19: {getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); insn = MAKE_INSN2(sbb, sbb, modrm, reg); goto done;}
      case 0x1A: {getModRegRM(rmLegacyByte, rmLegacyByte, BYTET); insn = MAKE_INSN2(sbb, sbb, reg, modrm); goto done;}
      case 0x1B: {getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); insn = MAKE_INSN2(sbb, sbb, reg, modrm); goto done;}
      case 0x1C: {SgAsmExpression* imm = getImmByteAsIv(); insn = MAKE_INSN2(sbb, sbb, makeRegister(0, rmLegacyByte), imm); goto done;}
      case 0x1D: {SgAsmExpression* imm = getImmIzAsIv(); insn = MAKE_INSN2(sbb, sbb, makeRegisterEffective(0), imm); goto done;}
      case 0x1E: {not64(); insn = MAKE_INSN1(push, push, makeRegister(3, rmSegment)); goto done;}
      case 0x1F: {not64(); insn = MAKE_INSN1(pop, pop, makeRegister(3, rmSegment)); goto done;}
      case 0x20: {getModRegRM(rmLegacyByte, rmLegacyByte, BYTET); insn = MAKE_INSN2(and, and, modrm, reg); goto done;}
      case 0x21: {getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); insn = MAKE_INSN2(and, and, modrm, reg); goto done;}
      case 0x22: {getModRegRM(rmLegacyByte, rmLegacyByte, BYTET); insn = MAKE_INSN2(and, and, reg, modrm); goto done;}
      case 0x23: {getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); insn = MAKE_INSN2(and, and, reg, modrm); goto done;}
      case 0x24: {SgAsmExpression* imm = getImmByteAsIv(); insn = MAKE_INSN2(and, and, makeRegister(0, rmLegacyByte), imm); goto done;}
      case 0x25: {SgAsmExpression* imm = getImmIzAsIv(); insn = MAKE_INSN2(and, and, makeRegisterEffective(0), imm); goto done;}
      case 0x26: {segOverride = x86_segreg_es; insn = disassemble(); goto done;}
      case 0x27: {not64(); insn = MAKE_INSN0(daa, daa); goto done;}
      case 0x28: {getModRegRM(rmLegacyByte, rmLegacyByte, BYTET); insn = MAKE_INSN2(sub, sub, modrm, reg); goto done;}
      case 0x29: {getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); insn = MAKE_INSN2(sub, sub, modrm, reg); goto done;}
      case 0x2A: {getModRegRM(rmLegacyByte, rmLegacyByte, BYTET); insn = MAKE_INSN2(sub, sub, reg, modrm); goto done;}
      case 0x2B: {getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); insn = MAKE_INSN2(sub, sub, reg, modrm); goto done;}
      case 0x2C: {SgAsmExpression* imm = getImmByteAsIv(); insn = MAKE_INSN2(sub, sub, makeRegister(0, rmLegacyByte), imm); goto done;}
      case 0x2D: {SgAsmExpression* imm = getImmIzAsIv(); insn = MAKE_INSN2(sub, sub, makeRegisterEffective(0), imm); goto done;}

   // Example of recursive use of disassemble()
      case 0x2E: {segOverride = x86_segreg_cs; branchPrediction = x86_branch_prediction_not_taken; insn = disassemble(); goto done;}
      case 0x2F: {not64(); insn = MAKE_INSN0(das, das); goto done;}
      case 0x30: {getModRegRM(rmLegacyByte, rmLegacyByte, BYTET); insn = MAKE_INSN2(xor, xor, modrm, reg); goto done;}
      case 0x31: {getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); insn = MAKE_INSN2(xor, xor, modrm, reg); goto done;}
      case 0x32: {getModRegRM(rmLegacyByte, rmLegacyByte, BYTET); insn = MAKE_INSN2(xor, xor, reg, modrm); goto done;}
      case 0x33: {getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); insn = MAKE_INSN2(xor, xor, reg, modrm); goto done;}
      case 0x34: {SgAsmExpression* imm = getImmByteAsIv(); insn = MAKE_INSN2(xor, xor, makeRegister(0, rmLegacyByte), imm); goto done;}
      case 0x35: {SgAsmExpression* imm = getImmIzAsIv(); insn = MAKE_INSN2(xor, xor, makeRegisterEffective(0), imm); goto done;}
      case 0x36: {segOverride = x86_segreg_ss; insn = disassemble(); goto done;}
      case 0x37: {not64(); insn = MAKE_INSN0(aaa, aaa); goto done;}
      case 0x38: {getModRegRM(rmLegacyByte, rmLegacyByte, BYTET); insn = MAKE_INSN2(cmp, cmp, modrm, reg); goto done;}
      case 0x39: {getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); insn = MAKE_INSN2(cmp, cmp, modrm, reg); goto done;}
      case 0x3A: {getModRegRM(rmLegacyByte, rmLegacyByte, BYTET); insn = MAKE_INSN2(cmp, cmp, reg, modrm); goto done;}
      case 0x3B: {getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); insn = MAKE_INSN2(cmp, cmp, reg, modrm); goto done;}
      case 0x3C: {SgAsmExpression* imm = getImmByteAsIv(); insn = MAKE_INSN2(cmp, cmp, makeRegister(0, rmLegacyByte), imm); goto done;}
      case 0x3D: {SgAsmExpression* imm = getImmIzAsIv(); insn = MAKE_INSN2(cmp, cmp, makeRegisterEffective(0), imm); goto done;}
      case 0x3E: {segOverride = x86_segreg_ds; branchPrediction = x86_branch_prediction_taken; insn = disassemble(); goto done;}
      case 0x3F: {not64(); insn = MAKE_INSN0(aas, aas); goto done;}
      case 0x40: {if (longMode()) {setRex(0x40); insn = disassemble(); goto done;} else {insn = MAKE_INSN1(inc, inc, makeRegisterEffective(0)); goto done;}}
      case 0x41: {if (longMode()) {setRex(0x41); insn = disassemble(); goto done;} else {insn = MAKE_INSN1(inc, inc, makeRegisterEffective(1)); goto done;}}
      case 0x42: {if (longMode()) {setRex(0x42); insn = disassemble(); goto done;} else {insn = MAKE_INSN1(inc, inc, makeRegisterEffective(2)); goto done;}}
      case 0x43: {if (longMode()) {setRex(0x43); insn = disassemble(); goto done;} else {insn = MAKE_INSN1(inc, inc, makeRegisterEffective(3)); goto done;}}
      case 0x44: {if (longMode()) {setRex(0x44); insn = disassemble(); goto done;} else {insn = MAKE_INSN1(inc, inc, makeRegisterEffective(4)); goto done;}}
      case 0x45: {if (longMode()) {setRex(0x45); insn = disassemble(); goto done;} else {insn = MAKE_INSN1(inc, inc, makeRegisterEffective(5)); goto done;}}
      case 0x46: {if (longMode()) {setRex(0x46); insn = disassemble(); goto done;} else {insn = MAKE_INSN1(inc, inc, makeRegisterEffective(6)); goto done;}}
      case 0x47: {if (longMode()) {setRex(0x47); insn = disassemble(); goto done;} else {insn = MAKE_INSN1(inc, inc, makeRegisterEffective(7)); goto done;}}
      case 0x48: {if (longMode()) {setRex(0x48); insn = disassemble(); goto done;} else {insn = MAKE_INSN1(dec, dec, makeRegisterEffective(0)); goto done;}}
      case 0x49: {if (longMode()) {setRex(0x49); insn = disassemble(); goto done;} else {insn = MAKE_INSN1(dec, dec, makeRegisterEffective(1)); goto done;}}
      case 0x4A: {if (longMode()) {setRex(0x4A); insn = disassemble(); goto done;} else {insn = MAKE_INSN1(dec, dec, makeRegisterEffective(2)); goto done;}}
      case 0x4B: {if (longMode()) {setRex(0x4B); insn = disassemble(); goto done;} else {insn = MAKE_INSN1(dec, dec, makeRegisterEffective(3)); goto done;}}
      case 0x4C: {if (longMode()) {setRex(0x4C); insn = disassemble(); goto done;} else {insn = MAKE_INSN1(dec, dec, makeRegisterEffective(4)); goto done;}}
      case 0x4D: {if (longMode()) {setRex(0x4D); insn = disassemble(); goto done;} else {insn = MAKE_INSN1(dec, dec, makeRegisterEffective(5)); goto done;}}
      case 0x4E: {if (longMode()) {setRex(0x4E); insn = disassemble(); goto done;} else {insn = MAKE_INSN1(dec, dec, makeRegisterEffective(6)); goto done;}}
      case 0x4F: {if (longMode()) {setRex(0x4F); insn = disassemble(); goto done;} else {insn = MAKE_INSN1(dec, dec, makeRegisterEffective(7)); goto done;}}
      case 0x50: {sizeMustBe64Bit = true; insn = MAKE_INSN1(push, push, makeRegisterEffective(rexB, 0)); goto done;}
      case 0x51: {sizeMustBe64Bit = true; insn = MAKE_INSN1(push, push, makeRegisterEffective(rexB, 1)); goto done;}
      case 0x52: {sizeMustBe64Bit = true; insn = MAKE_INSN1(push, push, makeRegisterEffective(rexB, 2)); goto done;}
      case 0x53: {sizeMustBe64Bit = true; insn = MAKE_INSN1(push, push, makeRegisterEffective(rexB, 3)); goto done;}
      case 0x54: {sizeMustBe64Bit = true; insn = MAKE_INSN1(push, push, makeRegisterEffective(rexB, 4)); goto done;}
      case 0x55: {sizeMustBe64Bit = true; insn = MAKE_INSN1(push, push, makeRegisterEffective(rexB, 5)); goto done;}
      case 0x56: {sizeMustBe64Bit = true; insn = MAKE_INSN1(push, push, makeRegisterEffective(rexB, 6)); goto done;}
      case 0x57: {sizeMustBe64Bit = true; insn = MAKE_INSN1(push, push, makeRegisterEffective(rexB, 7)); goto done;}
      case 0x58: {sizeMustBe64Bit = true; insn = MAKE_INSN1(pop, pop, makeRegisterEffective(rexB, 0)); goto done;}
      case 0x59: {sizeMustBe64Bit = true; insn = MAKE_INSN1(pop, pop, makeRegisterEffective(rexB, 1)); goto done;}
      case 0x5A: {sizeMustBe64Bit = true; insn = MAKE_INSN1(pop, pop, makeRegisterEffective(rexB, 2)); goto done;}
      case 0x5B: {sizeMustBe64Bit = true; insn = MAKE_INSN1(pop, pop, makeRegisterEffective(rexB, 3)); goto done;}
      case 0x5C: {sizeMustBe64Bit = true; insn = MAKE_INSN1(pop, pop, makeRegisterEffective(rexB, 4)); goto done;}
      case 0x5D: {sizeMustBe64Bit = true; insn = MAKE_INSN1(pop, pop, makeRegisterEffective(rexB, 5)); goto done;}
      case 0x5E: {sizeMustBe64Bit = true; insn = MAKE_INSN1(pop, pop, makeRegisterEffective(rexB, 6)); goto done;}
      case 0x5F: {sizeMustBe64Bit = true; insn = MAKE_INSN1(pop, pop, makeRegisterEffective(rexB, 7)); goto done;}
      case 0x60: {not64(); if (effectiveOperandSize() == x86_insnsize_32) insn = MAKE_INSN0(pushad, pushad); else insn = MAKE_INSN0(pusha, pusha); goto done;}
      case 0x61: {not64(); if (effectiveOperandSize() == x86_insnsize_32) insn = MAKE_INSN0(popad, popad); else insn = MAKE_INSN0(popa, popa); goto done;}
      case 0x62: {not64(); getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); requireMemory(); insn = MAKE_INSN2(bound, bound, reg, modrm); goto done;}
      case 0x63: {if (longMode()) {getModRegRM(effectiveOperandMode(), rmDWord, DWORDT); insn = MAKE_INSN2(movsxd, movsxd, reg, modrm); goto done;} else {getModRegRM(rmWord, rmWord, WORDT); insn = MAKE_INSN2(arpl, arpl, modrm, reg); goto done;}}
      case 0x64: {segOverride = x86_segreg_fs; insn = disassemble(); goto done;}
      case 0x65: {segOverride = x86_segreg_gs; insn = disassemble(); goto done;}
      case 0x66: {operandSizeOverride = true; insn = disassemble(); goto done;}
      case 0x67: {addressSizeOverride = true; insn = disassemble(); goto done;}
      case 0x68: {sizeMustBe64Bit = true; SgAsmExpression* imm = getImmIzAsIv(); insn = MAKE_INSN1(push, push, imm); goto done;}
      case 0x69: {getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); SgAsmExpression* imm = getImmIzAsIv(); insn = MAKE_INSN3(imul, imul, reg, modrm, imm); goto done;}
      case 0x6A: {sizeMustBe64Bit = true; SgAsmExpression* imm = getImmByteAsIv(); insn = MAKE_INSN1(push, push, imm); goto done;}
      case 0x6B: {getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); SgAsmExpression* imm = getImmByteAsIv(); insn = MAKE_INSN3(imul, imul, reg, modrm, imm); goto done;}
      case 0x6C: {
        switch (repeatPrefix) {
          case rpNone: insn = MAKE_INSN0(insb, insb); goto done;
          case rpRepe: insn = MAKE_INSN0(rep_insb, rep_insb); goto done;
          default: throw BadInstruction();
        }
      }
      case 0x6D: {
        switch (effectiveOperandSize()) {
          case x86_insnsize_16: 
            switch (repeatPrefix) {
              case rpNone: insn = MAKE_INSN0(insw, insw); goto done;
              case rpRepe: insn = MAKE_INSN0(rep_insw, rep_insw); goto done;
              default: throw BadInstruction();
            }
          case x86_insnsize_32: 
          case x86_insnsize_64:
            switch (repeatPrefix) {
              case rpNone: insn = MAKE_INSN0(insd, insd); goto done;
              case rpRepe: insn = MAKE_INSN0(rep_insd, rep_insd); goto done;
              default: throw BadInstruction();
            }
          default: ROSE_ASSERT (false);
        }
      }
      case 0x6E: {
        switch (repeatPrefix) {
          case rpNone: insn = MAKE_INSN0(outsb, outsb); goto done;
          case rpRepe: insn = MAKE_INSN0(rep_outsb, rep_outsb); goto done;
          default: throw BadInstruction();
        }
      }
      case 0x6F: {
        switch (effectiveOperandSize()) {
          case x86_insnsize_16: 
            switch (repeatPrefix) {
              case rpNone: insn = MAKE_INSN0(outsw, outsw); goto done;
              case rpRepe: insn = MAKE_INSN0(rep_outsw, rep_outsw); goto done;
              default: throw BadInstruction();
            }
          case x86_insnsize_32: 
          case x86_insnsize_64:
            switch (repeatPrefix) {
              case rpNone: insn = MAKE_INSN0(outsd, outsd); goto done;
              case rpRepe: insn = MAKE_INSN0(rep_outsd, rep_outsd); goto done;
              default: throw BadInstruction();
            }
          default: ROSE_ASSERT (false);
        }
      }
      case 0x70: {SgAsmExpression* imm = getImmJb(); branchPredictionEnabled = true; insn = MAKE_INSN1(jo , jo , imm); goto done;}
      case 0x71: {SgAsmExpression* imm = getImmJb(); branchPredictionEnabled = true; insn = MAKE_INSN1(jno, jno, imm); goto done;}
      case 0x72: {SgAsmExpression* imm = getImmJb(); branchPredictionEnabled = true; insn = MAKE_INSN1(jb , jb , imm); goto done;}
      case 0x73: {SgAsmExpression* imm = getImmJb(); branchPredictionEnabled = true; insn = MAKE_INSN1(jae, jae, imm); goto done;}
      case 0x74: {SgAsmExpression* imm = getImmJb(); branchPredictionEnabled = true; insn = MAKE_INSN1(je , je , imm); goto done;}
      case 0x75: {SgAsmExpression* imm = getImmJb(); branchPredictionEnabled = true; insn = MAKE_INSN1(jne, jne, imm); goto done;}
      case 0x76: {SgAsmExpression* imm = getImmJb(); branchPredictionEnabled = true; insn = MAKE_INSN1(jbe, jbe, imm); goto done;}
      case 0x77: {SgAsmExpression* imm = getImmJb(); branchPredictionEnabled = true; insn = MAKE_INSN1(ja , ja , imm); goto done;}
      case 0x78: {SgAsmExpression* imm = getImmJb(); branchPredictionEnabled = true; insn = MAKE_INSN1(js , js , imm); goto done;}
      case 0x79: {SgAsmExpression* imm = getImmJb(); branchPredictionEnabled = true; insn = MAKE_INSN1(jns, jns, imm); goto done;}
      case 0x7A: {SgAsmExpression* imm = getImmJb(); branchPredictionEnabled = true; insn = MAKE_INSN1(jpe, jpe, imm); goto done;}
      case 0x7B: {SgAsmExpression* imm = getImmJb(); branchPredictionEnabled = true; insn = MAKE_INSN1(jpo, jpo, imm); goto done;}
      case 0x7C: {SgAsmExpression* imm = getImmJb(); branchPredictionEnabled = true; insn = MAKE_INSN1(jl , jl , imm); goto done;}
      case 0x7D: {SgAsmExpression* imm = getImmJb(); branchPredictionEnabled = true; insn = MAKE_INSN1(jge, jge, imm); goto done;}
      case 0x7E: {SgAsmExpression* imm = getImmJb(); branchPredictionEnabled = true; insn = MAKE_INSN1(jle, jle, imm); goto done;}
      case 0x7F: {SgAsmExpression* imm = getImmJb(); branchPredictionEnabled = true; insn = MAKE_INSN1(jg , jg , imm); goto done;}

   // The names for groups will make more sense relative to the AMD manual.
      case 0x80: {getModRegRM(rmReturnNull, rmLegacyByte, BYTET); SgAsmExpression* imm = getImmByte(); insn = decodeGroup1(imm); goto done;}

   // effectiveOperandMode() returns register mode for the effective operand size (16bit, 32, bit, 64bit)
   // effectiveOperandType() does the same thing but returne a SgAsmType.
      case 0x81: {getModRegRM(rmReturnNull, effectiveOperandMode(), effectiveOperandType()); SgAsmExpression* imm = getImmIzAsIv(); insn = decodeGroup1(imm); goto done;}
      case 0x82: {not64(); getModRegRM(rmReturnNull, rmLegacyByte, BYTET); SgAsmExpression* imm = getImmByte(); insn = decodeGroup1(imm); goto done;}
      case 0x83: {getModRegRM(rmReturnNull, effectiveOperandMode(), effectiveOperandType()); SgAsmExpression* imm = getImmByteAsIv(); insn = decodeGroup1(imm); goto done;}
      case 0x84: {getModRegRM(rmLegacyByte, rmLegacyByte, BYTET); insn = MAKE_INSN2(test, test, modrm, reg); goto done;}
      case 0x85: {getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); insn = MAKE_INSN2(test, test, modrm, reg); goto done;}
      case 0x86: {getModRegRM(rmLegacyByte, rmLegacyByte, BYTET); insn = MAKE_INSN2(xchg, xchg, modrm, reg); goto done;}
      case 0x87: {getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); insn = MAKE_INSN2(xchg, xchg, modrm, reg); goto done;}
      case 0x88: {getModRegRM(rmLegacyByte, rmLegacyByte, BYTET); insn = MAKE_INSN2(mov, mov, modrm, reg); goto done;}
      case 0x89: {getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); insn = MAKE_INSN2(mov, mov, modrm, reg); goto done;}
      case 0x8A: {getModRegRM(rmLegacyByte, rmLegacyByte, BYTET); insn = MAKE_INSN2(mov, mov, reg, modrm); goto done;}
      case 0x8B: {getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); insn = MAKE_INSN2(mov, mov, reg, modrm); goto done;}
      case 0x8C: {getModRegRM(rmSegment, effectiveOperandMode(), WORDT); insn = MAKE_INSN2(mov, mov, modrm, reg); goto done;}
      case 0x8D: {getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); requireMemory(); insn = MAKE_INSN2(lea, lea, reg, modrm); goto done;}
      case 0x8E: {getModRegRM(rmSegment, rmWord, WORDT); insn = MAKE_INSN2(mov, mov, reg, modrm); goto done;}
      case 0x8F: {getModRegRM(rmReturnNull, effectiveOperandMode(), effectiveOperandType()); insn = decodeGroup1a(); goto done;}
      case 0x90: {
        if (rexB) {
          insn = MAKE_INSN2(xchg, xchg, makeRegisterEffective(8), makeRegisterEffective(0));
          goto done;
        } else if (repeatPrefix == rpRepe) {
          insn = MAKE_INSN0(pause, pause); goto done;
        } else {
          insn = MAKE_INSN0(nop, nop); goto done;
        }
      }
      case 0x91: {insn = MAKE_INSN2(xchg, xchg, makeRegisterEffective(rexB, 1), makeRegisterEffective(0)); goto done;}
      case 0x92: {insn = MAKE_INSN2(xchg, xchg, makeRegisterEffective(rexB, 2), makeRegisterEffective(0)); goto done;}
      case 0x93: {insn = MAKE_INSN2(xchg, xchg, makeRegisterEffective(rexB, 3), makeRegisterEffective(0)); goto done;}
      case 0x94: {insn = MAKE_INSN2(xchg, xchg, makeRegisterEffective(rexB, 4), makeRegisterEffective(0)); goto done;}
      case 0x95: {insn = MAKE_INSN2(xchg, xchg, makeRegisterEffective(rexB, 5), makeRegisterEffective(0)); goto done;}
      case 0x96: {insn = MAKE_INSN2(xchg, xchg, makeRegisterEffective(rexB, 6), makeRegisterEffective(0)); goto done;}
      case 0x97: {insn = MAKE_INSN2(xchg, xchg, makeRegisterEffective(rexB, 7), makeRegisterEffective(0)); goto done;}
      case 0x98: {
        switch (effectiveOperandSize()) {
          case x86_insnsize_16: insn = MAKE_INSN0(cbw, cbw); goto done;
          case x86_insnsize_32: insn = MAKE_INSN0(cwde, cwde); goto done;
          case x86_insnsize_64: insn = MAKE_INSN0(cdqe, cdqe); goto done;
          default: ROSE_ASSERT (false);
        }
      }
      case 0x99: {
        switch (effectiveOperandSize()) {
          case x86_insnsize_16: insn = MAKE_INSN0(cwd, cwd); goto done;
          case x86_insnsize_32: insn = MAKE_INSN0(cdq, cdq); goto done;
          case x86_insnsize_64: insn = MAKE_INSN0(cqo, cqo); goto done;
          default: ROSE_ASSERT (false);
        }
      }
      case 0x9A: {not64(); SgAsmExpression* addr = getImmForAddr(); SgAsmExpression* seg = getImmWord(); insn = MAKE_INSN2(farcall, farCall, seg, addr); goto done;}
      case 0x9B: {insn = MAKE_INSN0(wait, wait); goto done;}
      case 0x9C: {
        sizeMustBe64Bit = true;
        switch (effectiveOperandSize()) {
          case x86_insnsize_16: insn = MAKE_INSN0(pushf, pushf); goto done;
          case x86_insnsize_32: insn = MAKE_INSN0(pushfd, pushfd); goto done;
          case x86_insnsize_64: insn = MAKE_INSN0(pushfq, pushfq); goto done;
          default: ROSE_ASSERT (false);
        }
      }
      case 0x9D: {
        sizeMustBe64Bit = true;
        switch (effectiveOperandSize()) {
          case x86_insnsize_16: insn = MAKE_INSN0(popf, popf); goto done;
          case x86_insnsize_32: insn = MAKE_INSN0(popfd, popfd); goto done;
          case x86_insnsize_64: insn = MAKE_INSN0(popfq, popfq); goto done;
          default: ROSE_ASSERT (false);
        }
      }
      case 0x9E: {insn = MAKE_INSN0(sahf, sahf); goto done;}
      case 0x9F: {insn = MAKE_INSN0(lahf, lahf); goto done;}
      case 0xA0: {SgAsmExpression* addr = getImmForAddr(); insn = MAKE_INSN2(mov, mov, makeRegister(0, rmLegacyByte), makeMemoryReference(addr, currentDataSegment(), BYTET)); goto done;}
      case 0xA1: {SgAsmExpression* addr = getImmForAddr(); insn = MAKE_INSN2(mov, mov, makeRegisterEffective(0), makeMemoryReference(addr, currentDataSegment(), effectiveOperandType())); goto done;}
      case 0xA2: {SgAsmExpression* addr = getImmForAddr(); insn = MAKE_INSN2(mov, mov, makeMemoryReference(addr, currentDataSegment(), BYTET), makeRegister(0, rmLegacyByte)); goto done;}
      case 0xA3: {SgAsmExpression* addr = getImmForAddr(); insn = MAKE_INSN2(mov, mov, makeMemoryReference(addr, currentDataSegment(), effectiveOperandType()), makeRegisterEffective(0)); goto done;}
      case 0xA4: {
        switch (repeatPrefix) {
          case rpNone: insn = MAKE_INSN0(movsb, movsb); goto done;
          case rpRepe: insn = MAKE_INSN0(rep_movsb, rep_movsb); goto done;
          default: throw BadInstruction();
        }
      }
      case 0xA5: {
        switch (effectiveOperandSize()) {
          case x86_insnsize_16: 
            switch (repeatPrefix) {
              case rpNone: insn = MAKE_INSN0(movsw, movsw); goto done;
              case rpRepe: insn = MAKE_INSN0(rep_movsw, rep_movsw); goto done;
              default: throw BadInstruction();
            }
          case x86_insnsize_32: 
            switch (repeatPrefix) {
              case rpNone: insn = MAKE_INSN0(movsd, movsd); goto done;
              case rpRepe: insn = MAKE_INSN0(rep_movsd, rep_movsd); goto done;
              default: throw BadInstruction();
            }
          case x86_insnsize_64:
            switch (repeatPrefix) {
              case rpNone: insn = MAKE_INSN0(movsq, movsq); goto done;
              case rpRepe: insn = MAKE_INSN0(rep_movsq, rep_movsq); goto done;
              default: throw BadInstruction();
            }
          default: ROSE_ASSERT (false);
        }
      }
      case 0xA6: {
        switch (repeatPrefix) {
          case rpNone: insn = MAKE_INSN0(cmpsb, cmpsb); goto done;
          case rpRepe: insn = MAKE_INSN0(repe_cmpsb, repe_cmpsb); goto done;
          case rpRepne: insn = MAKE_INSN0(repne_cmpsb, repne_cmpsb); goto done;
          default: throw BadInstruction();
        }
      }
      case 0xA7: {
        switch (effectiveOperandSize()) {
          case x86_insnsize_16: 
            switch (repeatPrefix) {
              case rpNone: insn = MAKE_INSN0(cmpsw, cmpsw); goto done;
              case rpRepe: insn = MAKE_INSN0(repe_cmpsw, repe_cmpsw); goto done;
              case rpRepne: insn = MAKE_INSN0(repne_cmpsw, repne_cmpsw); goto done;
              default: throw BadInstruction();
            }
          case x86_insnsize_32: 
            switch (repeatPrefix) {
              case rpNone: insn = MAKE_INSN0(cmpsd, cmpsd); goto done;
              case rpRepe: insn = MAKE_INSN0(repe_cmpsd, repe_cmpsd); goto done;
              case rpRepne: insn = MAKE_INSN0(repne_cmpsd, repne_cmpsd); goto done;
              default: throw BadInstruction();
            }
          case x86_insnsize_64:
            switch (repeatPrefix) {
              case rpNone: insn = MAKE_INSN0(cmpsq, cmpsq); goto done;
              case rpRepe: insn = MAKE_INSN0(repe_cmpsq, repe_cmpsq); goto done;
              case rpRepne: insn = MAKE_INSN0(repne_cmpsq, repne_cmpsq); goto done;
              default: throw BadInstruction();
            }
          default: ROSE_ASSERT (false);
        }
      }
      case 0xA8: {SgAsmExpression* imm = getImmByte(); insn = MAKE_INSN2(test, test, makeRegister(0, rmLegacyByte), imm); goto done;}
      case 0xA9: {SgAsmExpression* imm = getImmIzAsIv(); insn = MAKE_INSN2(test, test, makeRegisterEffective(0), imm); goto done;}
      case 0xAA: {
        switch (repeatPrefix) {
          case rpNone: insn = MAKE_INSN0(stosb, stosb); goto done;
          case rpRepe: insn = MAKE_INSN0(rep_stosb, rep_stosb); goto done;
          default: throw BadInstruction();
        }
      }
      case 0xAB: {
        switch (effectiveOperandSize()) {
          case x86_insnsize_16: 
            switch (repeatPrefix) {
              case rpNone: insn = MAKE_INSN0(stosw, stosw); goto done;
              case rpRepe: insn = MAKE_INSN0(rep_stosw, rep_stosw); goto done;
              default: throw BadInstruction();
            }
          case x86_insnsize_32: 
            switch (repeatPrefix) {
              case rpNone: insn = MAKE_INSN0(stosd, stosd); goto done;
              case rpRepe: insn = MAKE_INSN0(rep_stosd, rep_stosd); goto done;
              default: throw BadInstruction();
            }
          case x86_insnsize_64:
            switch (repeatPrefix) {
              case rpNone: insn = MAKE_INSN0(stosq, stosq); goto done;
              case rpRepe: insn = MAKE_INSN0(rep_stosq, rep_stosq); goto done;
              default: throw BadInstruction();
            }
          default: ROSE_ASSERT (false);
        }
      }
      case 0xAC: {
        switch (repeatPrefix) {
          case rpNone: insn = MAKE_INSN0(lodsb, lodsb); goto done;
          case rpRepe: insn = MAKE_INSN0(rep_lodsb, rep_lodsb); goto done;
          default: throw BadInstruction();
        }
      }
      case 0xAD: {
        switch (effectiveOperandSize()) {
          case x86_insnsize_16: 
            switch (repeatPrefix) {
              case rpNone: insn = MAKE_INSN0(lodsw, lodsw); goto done;
              case rpRepe: insn = MAKE_INSN0(rep_lodsw, rep_lodsw); goto done;
              default: throw BadInstruction();
            }
          case x86_insnsize_32: 
            switch (repeatPrefix) {
              case rpNone: insn = MAKE_INSN0(lodsd, lodsd); goto done;
              case rpRepe: insn = MAKE_INSN0(rep_lodsd, rep_lodsd); goto done;
              default: throw BadInstruction();
            }
          case x86_insnsize_64:
            switch (repeatPrefix) {
              case rpNone: insn = MAKE_INSN0(lodsq, lodsq); goto done;
              case rpRepe: insn = MAKE_INSN0(rep_lodsq, rep_lodsq); goto done;
              default: throw BadInstruction();
            }
          default: ROSE_ASSERT (false);
        }
      }
      case 0xAE: {
        switch (repeatPrefix) {
          case rpNone: insn = MAKE_INSN0(scasb, scasb); goto done;
          case rpRepe: insn = MAKE_INSN0(repe_scasb, repe_scasb); goto done;
          case rpRepne: insn = MAKE_INSN0(repne_scasb, repne_scasb); goto done;
          default: throw BadInstruction();
        }
      }
      case 0xAF: {
        switch (effectiveOperandSize()) {
          case x86_insnsize_16: 
            switch (repeatPrefix) {
              case rpNone: insn = MAKE_INSN0(scasw, scasw); goto done;
              case rpRepe: insn = MAKE_INSN0(repe_scasw, repe_scasw); goto done;
              case rpRepne: insn = MAKE_INSN0(repne_scasw, repne_scasw); goto done;
              default: throw BadInstruction();
            }
          case x86_insnsize_32: 
            switch (repeatPrefix) {
              case rpNone: insn = MAKE_INSN0(scasd, scasd); goto done;
              case rpRepe: insn = MAKE_INSN0(repe_scasd, repe_scasd); goto done;
              case rpRepne: insn = MAKE_INSN0(repne_scasd, repne_scasd); goto done;
              default: throw BadInstruction();
            }
          case x86_insnsize_64:
            switch (repeatPrefix) {
              case rpNone: insn = MAKE_INSN0(scasq, scasq); goto done;
              case rpRepe: insn = MAKE_INSN0(repe_scasq, repe_scasq); goto done;
              case rpRepne: insn = MAKE_INSN0(repne_scasq, repne_scasq); goto done;
              default: throw BadInstruction();
            }
          default: ROSE_ASSERT (false);
        }
      }
      case 0xB0: {SgAsmExpression* imm = getImmByte(); insn = MAKE_INSN2(mov, mov, makeOperandRegisterByte(rexB, 0), imm); goto done;}
      case 0xB1: {SgAsmExpression* imm = getImmByte(); insn = MAKE_INSN2(mov, mov, makeOperandRegisterByte(rexB, 1), imm); goto done;}
      case 0xB2: {SgAsmExpression* imm = getImmByte(); insn = MAKE_INSN2(mov, mov, makeOperandRegisterByte(rexB, 2), imm); goto done;}
      case 0xB3: {SgAsmExpression* imm = getImmByte(); insn = MAKE_INSN2(mov, mov, makeOperandRegisterByte(rexB, 3), imm); goto done;}
      case 0xB4: {SgAsmExpression* imm = getImmByte(); insn = MAKE_INSN2(mov, mov, makeOperandRegisterByte(rexB, 4), imm); goto done;}
      case 0xB5: {SgAsmExpression* imm = getImmByte(); insn = MAKE_INSN2(mov, mov, makeOperandRegisterByte(rexB, 5), imm); goto done;}
      case 0xB6: {SgAsmExpression* imm = getImmByte(); insn = MAKE_INSN2(mov, mov, makeOperandRegisterByte(rexB, 6), imm); goto done;}
      case 0xB7: {SgAsmExpression* imm = getImmByte(); insn = MAKE_INSN2(mov, mov, makeOperandRegisterByte(rexB, 7), imm); goto done;}
      case 0xB8: {SgAsmExpression* imm = getImmIv(); insn = MAKE_INSN2(mov, mov, makeRegisterEffective(rexB, 0), imm); goto done;}
      case 0xB9: {SgAsmExpression* imm = getImmIv(); insn = MAKE_INSN2(mov, mov, makeRegisterEffective(rexB, 1), imm); goto done;}
      case 0xBA: {SgAsmExpression* imm = getImmIv(); insn = MAKE_INSN2(mov, mov, makeRegisterEffective(rexB, 2), imm); goto done;}
      case 0xBB: {SgAsmExpression* imm = getImmIv(); insn = MAKE_INSN2(mov, mov, makeRegisterEffective(rexB, 3), imm); goto done;}
      case 0xBC: {SgAsmExpression* imm = getImmIv(); insn = MAKE_INSN2(mov, mov, makeRegisterEffective(rexB, 4), imm); goto done;}
      case 0xBD: {SgAsmExpression* imm = getImmIv(); insn = MAKE_INSN2(mov, mov, makeRegisterEffective(rexB, 5), imm); goto done;}
      case 0xBE: {SgAsmExpression* imm = getImmIv(); insn = MAKE_INSN2(mov, mov, makeRegisterEffective(rexB, 6), imm); goto done;}
      case 0xBF: {SgAsmExpression* imm = getImmIv(); insn = MAKE_INSN2(mov, mov, makeRegisterEffective(rexB, 7), imm); goto done;}
      case 0xC0: {getModRegRM(rmReturnNull, rmLegacyByte, BYTET); SgAsmExpression* imm = getImmByte(); insn = decodeGroup2(imm); goto done;}
      case 0xC1: {getModRegRM(rmReturnNull, effectiveOperandMode(), effectiveOperandType()); SgAsmExpression* imm = getImmByteAsIv(); insn = decodeGroup2(imm); goto done;}
      case 0xC2: {isUnconditionalJump = true; SgAsmExpression* imm = getImmWord(); insn = MAKE_INSN1(ret, ret, imm); goto done;}
      case 0xC3: {isUnconditionalJump = true; insn = MAKE_INSN0(ret, ret); goto done;}
      case 0xC4: {not64(); getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); requireMemory(); insn = MAKE_INSN2(les, les, reg, modrm); goto done;}
      case 0xC5: {not64(); getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); requireMemory(); insn = MAKE_INSN2(lds, lds, reg, modrm); goto done;}
      case 0xC6: {getModRegRM(rmReturnNull, rmLegacyByte, BYTET); SgAsmExpression* imm = getImmByte(); insn = decodeGroup11(imm); goto done;}
      case 0xC7: {getModRegRM(rmReturnNull, effectiveOperandMode(), effectiveOperandType()); SgAsmExpression* imm = getImmIzAsIv(); insn = decodeGroup11(imm); goto done;}
      case 0xC8: {SgAsmExpression* immw; {SgAsmExpression* imm = getImmWord(); immw = imm;} SgAsmExpression* imm = getImmByte(); insn = MAKE_INSN2(enter, enter, immw, imm); goto done;}
      case 0xC9: {insn = MAKE_INSN0(leave, leave); goto done;}
      case 0xCA: {isUnconditionalJump = true; SgAsmExpression* imm = getImmWord(); insn = MAKE_INSN1(retf, retf, imm); goto done;}
      case 0xCB: {isUnconditionalJump = true; insn = MAKE_INSN0(retf, retf); goto done;}
      case 0xCC: {insn = MAKE_INSN0(int3, int3); goto done;}
      case 0xCD: {SgAsmExpression* imm = getImmByte(); insn = MAKE_INSN1(int, int, imm); goto done;}
      case 0xCE: {not64(); insn = MAKE_INSN0(into, into); goto done;}
      case 0xCF: {isUnconditionalJump = true; insn = MAKE_INSN0(iret, iret); goto done;}
      case 0xD0: {getModRegRM(rmReturnNull, rmLegacyByte, BYTET); insn = decodeGroup2(makeByteValue(1)); goto done;}
      case 0xD1: {getModRegRM(rmReturnNull, effectiveOperandMode(), effectiveOperandType()); insn = decodeGroup2(makeByteValue(1)); goto done;}
      case 0xD2: {getModRegRM(rmReturnNull, rmLegacyByte, BYTET); insn = decodeGroup2(makeRegister(1, rmLegacyByte)); goto done;}
      case 0xD3: {getModRegRM(rmReturnNull, effectiveOperandMode(), effectiveOperandType()); insn = decodeGroup2(makeRegister(1, rmLegacyByte)); goto done;}
      case 0xD4: {not64(); SgAsmExpression* imm = getImmByte(); insn = MAKE_INSN1(aam, aam, imm); goto done;}
      case 0xD5: {not64(); SgAsmExpression* imm = getImmByte(); insn = MAKE_INSN1(aad, aad, imm); goto done;}
      case 0xD6: {not64(); insn = MAKE_INSN0(salc, salc); goto done;}
      case 0xD7: {insn = MAKE_INSN0(xlatb, xlatb); goto done;}
      case 0xD8: {insn = decodeX87InstructionD8(); goto done;}
      case 0xD9: {insn = decodeX87InstructionD9(); goto done;}
      case 0xDA: {insn = decodeX87InstructionDA(); goto done;}
      case 0xDB: {insn = decodeX87InstructionDB(); goto done;}
      case 0xDC: {insn = decodeX87InstructionDC(); goto done;}
      case 0xDD: {insn = decodeX87InstructionDD(); goto done;}
      case 0xDE: {insn = decodeX87InstructionDE(); goto done;}
      case 0xDF: {insn = decodeX87InstructionDF(); goto done;}
      case 0xE0: {SgAsmExpression* imm = getImmJb(); insn = MAKE_INSN1(loopnz, loopnz, imm); goto done;}
      case 0xE1: {SgAsmExpression* imm = getImmJb(); insn = MAKE_INSN1(loopz, loopz, imm); goto done;}
      case 0xE2: {SgAsmExpression* imm = getImmJb(); insn = MAKE_INSN1(loop, loop, imm); goto done;}
      case 0xE3: {
        SgAsmExpression* imm = getImmJb();
        branchPredictionEnabled = true;
        switch (effectiveOperandSize()) {
          case x86_insnsize_16: insn = MAKE_INSN1(jcxz, jcxz, imm); goto done;
          case x86_insnsize_32: insn = MAKE_INSN1(jecxz, jecxz, imm); goto done;
          case x86_insnsize_64: insn = MAKE_INSN1(jrcxz, jrcxz, imm); goto done;
          default: ROSE_ASSERT (false);
        }
      }
      case 0xE4: {SgAsmExpression* imm = getImmByte(); insn = MAKE_INSN2(in, in, makeRegister(0, rmLegacyByte), imm); goto done;}
      case 0xE5: {SgAsmExpression* imm = getImmByte(); insn = MAKE_INSN2(in, in, makeRegisterEffective(0), imm); goto done;}
      case 0xE6: {SgAsmExpression* imm = getImmByte(); insn = MAKE_INSN2(out, out, imm, makeRegister(0, rmLegacyByte)); goto done;}
      case 0xE7: {SgAsmExpression* imm = getImmByte(); insn = MAKE_INSN2(out, out, imm, makeRegisterEffective(0)); goto done;}
      case 0xE8: {SgAsmExpression* imm = getImmJz(); insn = MAKE_INSN1(call, call, imm); goto done;}
      case 0xE9: {SgAsmExpression* imm = getImmJz(); insn = MAKE_INSN1(jmp, jmp, imm); isUnconditionalJump = true; goto done;}
      case 0xEA: {not64(); SgAsmExpression* addr = getImmForAddr(); SgAsmExpression* seg = getImmWord(); insn = MAKE_INSN2(farjmp, farJmp, seg, addr); isUnconditionalJump = true; goto done;}
      case 0xEB: {SgAsmExpression* imm = getImmJb(); insn = MAKE_INSN1(jmp, jmp, imm); isUnconditionalJump = true; goto done;}
      case 0xEC: {insn = MAKE_INSN2(in, in, makeRegister(0, rmLegacyByte), makeRegister(2, rmWord)); goto done;}
      case 0xED: {insn = MAKE_INSN2(in, in, makeRegisterEffective(0), makeRegister(2, rmWord)); goto done;}
      case 0xEE: {insn = MAKE_INSN2(out, out, makeRegister(2, rmWord), makeRegister(0, rmLegacyByte)); goto done;}
      case 0xEF: {insn = MAKE_INSN2(out, out, makeRegister(2, rmWord), makeRegisterEffective(0)); goto done;}
      case 0xF0: {lock = true; insn = disassemble(); goto done;}
      case 0xF1: {insn = MAKE_INSN0(int1, int1); goto done;}
      case 0xF2: {repeatPrefix = rpRepne; insn = disassemble(); goto done;}
      case 0xF3: {repeatPrefix = rpRepe; insn = disassemble(); goto done;}
      case 0xF4: {insn = MAKE_INSN0(hlt, hlt); isUnconditionalJump = true; goto done;}
      case 0xF5: {insn = MAKE_INSN0(cmc, cmc); goto done;}
      case 0xF6: {getModRegRM(rmReturnNull, rmLegacyByte, BYTET); SgAsmExpression* immMaybe = NULL; if (regField <= 1) {SgAsmExpression* imm = getImmByteAsIv(); immMaybe = imm;}; insn = decodeGroup3(immMaybe); goto done;}
      case 0xF7: {getModRegRM(rmReturnNull, effectiveOperandMode(), effectiveOperandType()); SgAsmExpression* immMaybe = NULL; if (regField <= 1) {SgAsmExpression* imm = getImmIzAsIv(); immMaybe = imm;}; insn = decodeGroup3(immMaybe); goto done;}
      case 0xF8: {insn = MAKE_INSN0(clc, clc); goto done;}
      case 0xF9: {insn = MAKE_INSN0(stc, stc); goto done;}
      case 0xFA: {insn = MAKE_INSN0(cli, cli); goto done;}
      case 0xFB: {insn = MAKE_INSN0(sti, sti); goto done;}
      case 0xFC: {insn = MAKE_INSN0(cld, cld); goto done;}
      case 0xFD: {insn = MAKE_INSN0(std, std); goto done;}
      case 0xFE: {getModRegRM(rmReturnNull, rmLegacyByte, BYTET); insn = decodeGroup4(); goto done;}
      case 0xFF: {
        getModRegRM(rmReturnNull, rmReturnNull, NULL);
        if (regField >= 2 && regField <= 6) {sizeMustBe64Bit = true;}
        fillInModRM(effectiveOperandMode(), effectiveOperandType());
        insn = decodeGroup5(); goto done;
      }
      default: ROSE_ASSERT (!"Should not get here");
    }
done:
    ROSE_ASSERT (insn);
    if (insn && knownSuccessorsReturn /* && !isUnconditionalJump */ ) {
      knownSuccessorsReturn->insert(p.ip + insn->get_raw_bytes().size());
    }
    return insn;
  }

  SgAsmx86Instruction* SingleInstructionDisassembler::decodeX87InstructionD8() {
    getModRegRM(rmReturnNull, rmST, FLOATT);
    if (isSgAsmMemoryReferenceExpression(modrm)) {
      isSgAsmMemoryReferenceExpression(modrm)->set_type(FLOATT);
    }
    if (modregrmByte < 0xC0) { // Using memory
      switch (regField) {
        case 0: return MAKE_INSN1(fadd, fadd, modrm);
        case 1: return MAKE_INSN1(fmul, fmul, modrm);
        case 2: return MAKE_INSN1(fcom, fcom, modrm);
        case 3: return MAKE_INSN1(fcomp, fcomp, modrm);
        case 4: return MAKE_INSN1(fsub, fsub, modrm);
        case 5: return MAKE_INSN1(fsubr, fsubr, modrm);
        case 6: return MAKE_INSN1(fdiv, fdiv, modrm);
        case 7: return MAKE_INSN1(fdivr, fdivr, modrm);
        default: ROSE_ASSERT (false);
      }
    } else { // Two-operand register forms
      switch (regField) {
        case 0: return MAKE_INSN2(fadd, fadd,   makeRegister(0, rmST), modrm);
        case 1: return MAKE_INSN2(fmul, fmul,   makeRegister(0, rmST), modrm);
        case 2: return MAKE_INSN2(fcom, fcom,   makeRegister(0, rmST), modrm);
        case 3: return MAKE_INSN2(fcomp, fcomp, makeRegister(0, rmST), modrm);
        case 4: return MAKE_INSN2(fsub, fsub,   makeRegister(0, rmST), modrm);
        case 5: return MAKE_INSN2(fsubr, fsubr, makeRegister(0, rmST), modrm);
        case 6: return MAKE_INSN2(fdiv, fdiv,   makeRegister(0, rmST), modrm);
        case 7: return MAKE_INSN2(fdivr, fdivr, makeRegister(0, rmST), modrm);
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
        case 0: mr->set_type(FLOATT); return MAKE_INSN1(fld, fld, modrm);
        case 1: throw BadInstruction();
        case 2: mr->set_type(FLOATT); return MAKE_INSN1(fst, fst, modrm);
        case 3: mr->set_type(FLOATT); return MAKE_INSN1(fstp, fstp, modrm);
        case 4: mr->set_type(BYTET); return MAKE_INSN1(fldenv, fldenv, modrm);
        case 5: mr->set_type(WORDT); return MAKE_INSN1(fldcw, fldcw, modrm);
        case 6: mr->set_type(BYTET); return MAKE_INSN1(fnstenv, fnstenv, modrm);
        case 7: mr->set_type(WORDT); return MAKE_INSN1(fnstcw, fnstcw, modrm);
        default: ROSE_ASSERT (false);
      }
    } else if (regField == 0 || regField == 1) { // FLD and FXCH on registers
      modrm = makeModrmRegister(rmST);
      switch (regField) {
        case 0: return MAKE_INSN2(fld, fld, makeRegister(0, rmST), modrm);
        case 1: return MAKE_INSN2(fxch, fxch, makeRegister(0, rmST), modrm);
        default: ROSE_ASSERT (false);
      }
    } else {
      switch (modregrmByte) {
        case 0xD0: return MAKE_INSN0(fnop, fnop);
        case 0xE0: return MAKE_INSN0(fchs, fchs);
        case 0xE1: return MAKE_INSN0(fabs, fabs);
        case 0xE4: return MAKE_INSN0(ftst, ftst);
        case 0xE5: return MAKE_INSN0(fxam, fxam);
        case 0xE8: return MAKE_INSN0(fld1, fld1);
        case 0xE9: return MAKE_INSN0(fldl2t, fldl2t);
        case 0xEA: return MAKE_INSN0(fldl2e, fldl2e);
        case 0xEB: return MAKE_INSN0(fldpi, fldpi);
        case 0xEC: return MAKE_INSN0(fldlg2, fldlg2);
        case 0xED: return MAKE_INSN0(fldln2, fldln2);
        case 0xEE: return MAKE_INSN0(fldz, fldz);
        case 0xF0: return MAKE_INSN0(f2xm1  , f2xm1  );
        case 0xF1: return MAKE_INSN0(fyl2x  , fyl2x  );
        case 0xF2: return MAKE_INSN0(fptan  , fptan  );
        case 0xF3: return MAKE_INSN0(fpatan , fpatan );
        case 0xF4: return MAKE_INSN0(fxtract, fxtract);
        case 0xF5: return MAKE_INSN0(fprem1 , fprem1 );
        case 0xF6: return MAKE_INSN0(fdecstp, fdecstp);
        case 0xF7: return MAKE_INSN0(fincstp, fincstp);
        case 0xF8: return MAKE_INSN0(fprem  , fprem  );
        case 0xF9: return MAKE_INSN0(fyl2xp1, fyl2xp1);
        case 0xFA: return MAKE_INSN0(fsqrt  , fsqrt  );
        case 0xFB: return MAKE_INSN0(fsincos, fsincos);
        case 0xFC: return MAKE_INSN0(frndint, frndint);
        case 0xFD: return MAKE_INSN0(fscale , fscale );
        case 0xFE: return MAKE_INSN0(fsin   , fsin   );
        case 0xFF: return MAKE_INSN0(fcos   , fcos   );
        default: throw BadInstruction();
      }
    }
  }

  SgAsmx86Instruction* SingleInstructionDisassembler::decodeX87InstructionDA() {
    getModRegRM(rmReturnNull, rmReturnNull, DWORDT);
    if (modeField < 3) {
      switch (regField) {
        case 0: return MAKE_INSN1(fiadd, fiadd, modrm);
        case 1: return MAKE_INSN1(fimul, fimul, modrm);
        case 2: return MAKE_INSN1(ficom, ficom, modrm);
        case 3: return MAKE_INSN1(ficomp, ficomp, modrm);
        case 4: return MAKE_INSN1(fisub, fisub, modrm);
        case 5: return MAKE_INSN1(fisubr, fisubr, modrm);
        case 6: return MAKE_INSN1(fidiv, fidiv, modrm);
        case 7: return MAKE_INSN1(fidivr, fidivr, modrm);
        default: ROSE_ASSERT (false);
      }
    } else if (regField < 4) { // FCMOV{B,E,BE,U}
      modrm = makeModrmRegister(rmST);
      switch (regField) {
        case 0: return MAKE_INSN2(fcmovb, fcmovb, makeRegister(0, rmST), modrm);
        case 1: return MAKE_INSN2(fcmove, fcmove, makeRegister(0, rmST), modrm);
        case 2: return MAKE_INSN2(fcmovbe, fcmovbe, makeRegister(0, rmST), modrm);
        case 3: return MAKE_INSN2(fcmovu, fcmovu, makeRegister(0, rmST), modrm);
        default: ROSE_ASSERT (false);
      }
    } else {
      switch (modregrmByte) {
        case 0xE9: return MAKE_INSN0(fucompp, fucompp);
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
        mr->set_type(DWORDT);
      } else {
        mr->set_type(LDOUBLET);
      }
      switch (regField) {
        case 0: return MAKE_INSN1(fild, fild, modrm);
        case 1: return MAKE_INSN1(fisttp, fisttp, modrm);
        case 2: return MAKE_INSN1(fist, fist, modrm);
        case 3: return MAKE_INSN1(fistp, fistp, modrm);
        case 4: throw BadInstruction();
        case 5: return MAKE_INSN1(fld, fld, modrm);
        case 6: throw BadInstruction();
        case 7: return MAKE_INSN1(fstp, fstp, modrm);
        default: ROSE_ASSERT (false);
      }
    } else if (regField <= 3 || regField == 5 || regField == 6) { // FCMOV{NB,NE,NBE,NU}, FUCOMI, FCOMI
      modrm = makeModrmRegister(rmST);
      switch (regField) {
        case 0: return MAKE_INSN2(fcmovnb, fcmovnb, makeRegister(0, rmST), modrm);
        case 1: return MAKE_INSN2(fcmovne, fcmovne, makeRegister(0, rmST), modrm);
        case 2: return MAKE_INSN2(fcmovnbe, fcmovnbe, makeRegister(0, rmST), modrm);
        case 3: return MAKE_INSN2(fcmovnu, fcmovnu, makeRegister(0, rmST), modrm);
        case 5: return MAKE_INSN2(fucomi, fucomi, makeRegister(0, rmST), modrm);
        case 6: return MAKE_INSN2(fcomi, fcomi, makeRegister(0, rmST), modrm);
        default: ROSE_ASSERT (false);
      }
    } else {
      switch (modregrmByte) {
        case 0xE2: return MAKE_INSN0(fnclex, fnclex);
        case 0xE3: return MAKE_INSN0(fninit, fninit);
        default: throw BadInstruction();
      }
    }
  }

  SgAsmx86Instruction* SingleInstructionDisassembler::decodeX87InstructionDC() {
    getModRegRM(rmReturnNull, rmST, DOUBLET);
    if (modeField < 3) { // Using memory
      switch (regField & 7) {
        case 0: return MAKE_INSN1(fadd, fadd, modrm);
        case 1: return MAKE_INSN1(fmul, fmul, modrm);
        case 2: return MAKE_INSN1(fcom, fcom, modrm);
        case 3: return MAKE_INSN1(fcomp, fcomp, modrm);
        case 4: return MAKE_INSN1(fsub, fsub, modrm);
        case 5: return MAKE_INSN1(fsubr, fsubr, modrm);
        case 6: return MAKE_INSN1(fdiv, fdiv, modrm);
        case 7: return MAKE_INSN1(fdivr, fdivr, modrm);
        default: ROSE_ASSERT (false);
      }
    } else { // Two-operand register forms
      switch (regField & 7) {
        case 0: return MAKE_INSN2(fadd, fadd,   modrm, makeRegister(0, rmST));
        case 1: return MAKE_INSN2(fmul, fmul,   modrm, makeRegister(0, rmST));
        case 2: throw BadInstruction();
        case 3: throw BadInstruction();
        case 4: return MAKE_INSN2(fsub, fsub,   modrm, makeRegister(0, rmST));
        case 5: return MAKE_INSN2(fsubr, fsubr, modrm, makeRegister(0, rmST));
        case 6: return MAKE_INSN2(fdiv, fdiv,   modrm, makeRegister(0, rmST));
        case 7: return MAKE_INSN2(fdivr, fdivr, modrm, makeRegister(0, rmST));
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
        case 0: mr->set_type(DOUBLET); return MAKE_INSN1(fld, fld, modrm);
        case 1: mr->set_type(QWORDT); return MAKE_INSN1(fisttp, fisttp, modrm);
        case 2: mr->set_type(DOUBLET); return MAKE_INSN1(fst, fst, modrm);
        case 3: mr->set_type(DOUBLET); return MAKE_INSN1(fstp, fstp, modrm);
        case 4: mr->set_type(BYTET); return MAKE_INSN1(frstor, frstor, modrm);
        case 5: throw BadInstruction();
        case 6: mr->set_type(BYTET); return MAKE_INSN1(fnsave, fnsave, modrm);
        case 7: mr->set_type(WORDT); return MAKE_INSN1(fnstsw, fnstsw, modrm);
        default: ROSE_ASSERT (false);
      }
    } else { // Register forms
      switch (regField) {
        case 0: return MAKE_INSN1(ffree, ffree, modrm);
        case 1: throw BadInstruction();
        case 2: return MAKE_INSN1(fst, fst, modrm);
        case 3: return MAKE_INSN1(fstp, fstp, modrm);
        case 4: return MAKE_INSN2(fucom, fucom, modrm, makeRegister(0, rmST));
        case 5: return MAKE_INSN2(fucomp, fucomp, modrm, makeRegister(0, rmST));
        case 6: throw BadInstruction();
        case 7: throw BadInstruction();
        default: ROSE_ASSERT (false);
      }
    }
  }

  SgAsmx86Instruction* SingleInstructionDisassembler::decodeX87InstructionDE() {
    getModRegRM(rmReturnNull, rmST, WORDT);
    if (modeField < 3) { // Using memory
      switch (regField & 7) {
        case 0: return MAKE_INSN1(fiadd, fiadd, modrm);
        case 1: return MAKE_INSN1(fimul, fimul, modrm);
        case 2: return MAKE_INSN1(ficom, ficom, modrm);
        case 3: return MAKE_INSN1(ficomp, ficomp, modrm);
        case 4: return MAKE_INSN1(fisub, fisub, modrm);
        case 5: return MAKE_INSN1(fisubr, fisubr, modrm);
        case 6: return MAKE_INSN1(fidiv, fidiv, modrm);
        case 7: return MAKE_INSN1(fidivr, fidivr, modrm);
        default: ROSE_ASSERT (false);
      }
    } else {
      switch (regField & 7) {
        case 0: return MAKE_INSN2(faddp, faddp, modrm, makeRegister(0, rmST));
        case 1: return MAKE_INSN2(fmulp, fmulp, modrm, makeRegister(0, rmST));
        case 2: throw BadInstruction();
        case 3: {
          switch (modregrmByte) {
            case 0xD9: delete modrm; delete reg; return MAKE_INSN0(fcompp, fcompp);
            default: throw BadInstruction();
          }
        }
        case 4: return MAKE_INSN2(fsubrp, fsubrp, modrm, makeRegister(0, rmST));
        case 5: return MAKE_INSN2(fsubp, fsubp, modrm, makeRegister(0, rmST));
        case 6: return MAKE_INSN2(fdivrp, fdivrp, modrm, makeRegister(0, rmST));
        case 7: return MAKE_INSN2(fdivp, fdivp, modrm, makeRegister(0, rmST));
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
        case 0: mr->set_type(WORDT); return MAKE_INSN1(fild, fild, modrm);
        case 1: mr->set_type(WORDT); return MAKE_INSN1(fisttp, fisttp, modrm);
        case 2: mr->set_type(WORDT); return MAKE_INSN1(fist, fist, modrm);
        case 3: mr->set_type(WORDT); return MAKE_INSN1(fistp, fistp, modrm);
        case 4: mr->set_type(BYTET); return MAKE_INSN1(fbld, fbld, modrm);
        case 5: mr->set_type(QWORDT); return MAKE_INSN1(fild, fild, modrm);
        case 6: mr->set_type(BYTET); return MAKE_INSN1(fbstp, fbstp, modrm);
        case 7: mr->set_type(QWORDT); return MAKE_INSN1(fistp, fistp, modrm);
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
            return MAKE_INSN1(fnstsw, fnstsw, makeRegister(0, rmWord));
          } else {
            throw BadInstruction();
          }
        }
        case 5: return MAKE_INSN2(fucomip, fucomip, makeRegister(0, rmST), modrm);
        case 6: return MAKE_INSN2(fcomip, fcomip, makeRegister(0, rmST), modrm);
        case 7: throw BadInstruction();
        default: ROSE_ASSERT (false);
      }
    }
  }

  SgAsmx86Instruction* SingleInstructionDisassembler::decodeOpcode0F() {
    uint8_t opcode;
    getByte(opcode);
    switch (opcode) {
      case 0x00: {getModRegRM(rmReturnNull, rmWord, WORDT); return decodeGroup6();}
      case 0x01: return decodeGroup7();
      case 0x02: {getModRegRM(rmWord, rmWord, WORDT); return MAKE_INSN2(lar, lar, reg, modrm);}
      case 0x03: {getModRegRM(rmWord, rmWord, WORDT); return MAKE_INSN2(lsl, lsl, reg, modrm);}
      case 0x04: throw BadInstruction();
      case 0x05: return MAKE_INSN0(syscall, syscall);
      case 0x06: return MAKE_INSN0(clts, clts);
      case 0x07: return MAKE_INSN0(sysret, sysret);
      case 0x08: return MAKE_INSN0(invd, invd);
      case 0x09: return MAKE_INSN0(wbinvd, wbinvd);
      case 0x0A: throw BadInstruction();
      case 0x0B: return MAKE_INSN0(ud2, ud2);
      case 0x0C: throw BadInstruction();
      case 0x0D: return decodeGroupP();
      case 0x0E: return MAKE_INSN0(femms, femms);
      case 0x0F: { // 3DNow! (AMD Specific)
        getModRegRM(rmReturnNull, rmReturnNull, NULL);
        uint8_t thirdOpcodeByte;
        getByte(thirdOpcodeByte);
        if (thirdOpcodeByte < 0x80) { // Conversions
          switch (thirdOpcodeByte) {
            case 0x0C: {
              fillInModRM(rmMM, V4WORDT); 
              reg = makeModrmRegister(rmMM, V2FLOATT);
              return MAKE_INSN2(pi2fw, pi2fw, reg, modrm);
            }
            case 0x0D: {
              fillInModRM(rmMM, V2DWORDT); 
              reg = makeModrmRegister(rmMM, V2FLOATT);
              return MAKE_INSN2(pi2fd, pi2fd, reg, modrm);
            }
            case 0x1C: {
              fillInModRM(rmMM, V2FLOATT); 
              reg = makeModrmRegister(rmMM, V4WORDT);
              return MAKE_INSN2(pf2iw, pf2iw, reg, modrm);
            }
            case 0x1D: {
              fillInModRM(rmMM, V2FLOATT); 
              reg = makeModrmRegister(rmMM, V2DWORDT);
              return MAKE_INSN2(pf2id, pf2id, reg, modrm);
            }
            default: throw BadInstruction();
          }
        } else if (thirdOpcodeByte < 0xB7) { // Floating-point operations
          fillInModRM(rmMM, V2FLOATT);
          reg = makeModrmRegister(rmMM, V2FLOATT);
          switch (thirdOpcodeByte) {
            case 0x8A: return MAKE_INSN2(pfnacc, pfnacc, reg, modrm);
            case 0x8E: return MAKE_INSN2(pfpnacc, pfpnacc, reg, modrm);
            case 0x90: return MAKE_INSN2(pfcmpge, pfcmpge, reg, modrm);
            case 0x94: return MAKE_INSN2(pfmin, pfmin, reg, modrm);
            case 0x96: return MAKE_INSN2(pfrcp, pfrcp, reg, modrm);
            case 0x97: return MAKE_INSN2(pfrsqrt, pfrsqrt, reg, modrm);
            case 0x9A: return MAKE_INSN2(pfsub, pfsub, reg, modrm);
            case 0x9E: return MAKE_INSN2(pfadd, pfadd, reg, modrm);
            case 0xA0: return MAKE_INSN2(pfcmpgt, pfcmpgt, reg, modrm);
            case 0xA4: return MAKE_INSN2(pfmax, pfmax, reg, modrm);
            case 0xA6: return MAKE_INSN2(pfrcpit1, pfrcpit1, reg, modrm);
            case 0xA7: return MAKE_INSN2(pfrsqit1, pfrsqit1, reg, modrm);
            case 0xAA: return MAKE_INSN2(pfsubr, pfsubr, reg, modrm);
            case 0xAE: return MAKE_INSN2(pfacc, pfacc, reg, modrm);
            case 0xB0: return MAKE_INSN2(pfcmpeq, pfcmpeq, reg, modrm);
            case 0xB4: return MAKE_INSN2(pfmul, pfmul, reg, modrm);
            case 0xB6: return MAKE_INSN2(pfrcpit2, pfrcpit2, reg, modrm);
            default: throw BadInstruction();
          }
        } else { // Extra integer operations
          switch (thirdOpcodeByte) {
            case 0xB7: getModRegRM(rmMM, rmMM, V4WORDT); return MAKE_INSN2(pmulhrw, pmulhrw, reg, modrm);
            case 0xBB: getModRegRM(rmMM, rmMM, V2DWORDT); return MAKE_INSN2(pswapd, pswapd, reg, modrm);
            case 0xBF: getModRegRM(rmMM, rmMM, V8BYTET); return MAKE_INSN2(pavgusb, pavgusb, reg, modrm);
            default: throw BadInstruction();
          }
        }
      }
      case 0x10: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmXMM, rmXMM, V4FLOATT); return MAKE_INSN2(movups, movups, reg, modrm);
          case mmF3: getModRegRM(rmXMM, rmXMM, FLOATT); return MAKE_INSN2(movss, movss, reg, modrm);
          case mm66: getModRegRM(rmXMM, rmXMM, V2DOUBLET); return MAKE_INSN2(movupd, movupd, reg, modrm);
          case mmF2: getModRegRM(rmXMM, rmXMM, DOUBLET); return MAKE_INSN2(movsd_sse, movsd, reg, modrm);
        }
      }
      case 0x11: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmXMM, rmXMM, V4FLOATT); return MAKE_INSN2(movups, movups, modrm, reg);
          case mmF3: getModRegRM(rmXMM, rmXMM, FLOATT); return MAKE_INSN2(movss, movss, modrm, reg);
          case mm66: getModRegRM(rmXMM, rmXMM, V2DOUBLET); return MAKE_INSN2(movupd, movupd, modrm, reg);
          case mmF2: getModRegRM(rmXMM, rmXMM, DOUBLET); return MAKE_INSN2(movsd_sse, movsd, modrm, reg);
        }
      }
      case 0x12: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmXMM, rmXMM, V4FLOATT); if (modeField == 3) return MAKE_INSN2(movhlps, movhlps, reg, modrm); else return MAKE_INSN2(movlps, movlps, reg, modrm);
          case mmF3: getModRegRM(rmXMM, rmXMM, V4FLOATT); return MAKE_INSN2(movsldup, movsldup, reg, modrm);
          case mm66: getModRegRM(rmXMM, rmXMM, DOUBLET); requireMemory(); return MAKE_INSN2(movlpd, movlpd, reg, modrm);
          case mmF2: getModRegRM(rmXMM, rmXMM, DOUBLET); return MAKE_INSN2(movddup, movddup, reg, modrm);
        }
      }
      case 0x13: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmXMM, rmXMM, V4FLOATT); requireMemory(); return MAKE_INSN2(movlps, movlps, modrm, reg);
          case mmF3: throw BadInstruction();
          case mm66: getModRegRM(rmXMM, rmXMM, DOUBLET); requireMemory(); return MAKE_INSN2(movlpd, movlpd, modrm, reg);
          case mmF2: throw BadInstruction();
        }
      }
      case 0x14: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmXMM, rmXMM, V2FLOATT, V4FLOATT); return MAKE_INSN2(unpcklps, unpcklps, reg, modrm);
          case mmF3: throw BadInstruction();
          case mm66: getModRegRM(rmXMM, rmXMM, DOUBLET, V2DOUBLET); return MAKE_INSN2(unpcklpd, unpcklpd, reg, modrm);
          case mmF2: throw BadInstruction();
        }
      }
      case 0x15: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmXMM, rmXMM, V2FLOATT, V4FLOATT); return MAKE_INSN2(unpckhps, unpckhps, reg, modrm);
          case mmF3: throw BadInstruction();
          case mm66: getModRegRM(rmXMM, rmXMM, DOUBLET, V2DOUBLET); return MAKE_INSN2(unpckhpd, unpckhpd, reg, modrm);
          case mmF2: throw BadInstruction();
        }
      }
      case 0x16: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmXMM, rmXMM, V4FLOATT); if (modeField == 3) return MAKE_INSN2(movlhps, movlhps, reg, modrm); else return MAKE_INSN2(movhps, movhps, reg, modrm);
          case mmF3: getModRegRM(rmXMM, rmXMM, V4FLOATT); return MAKE_INSN2(movshdup, movshdup, reg, modrm);
          case mm66: getModRegRM(rmXMM, rmXMM, DOUBLET); requireMemory(); return MAKE_INSN2(movhpd, movhpd, reg, modrm);
          case mmF2: throw BadInstruction();
        }
      }
      case 0x17: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmXMM, rmXMM, V4FLOATT); requireMemory(); return MAKE_INSN2(movhps, movhps, modrm, reg);
          case mmF3: throw BadInstruction();
          case mm66: getModRegRM(rmXMM, rmXMM, DOUBLET); requireMemory(); return MAKE_INSN2(movhpd, movhpd, modrm, reg);
          case mmF2: throw BadInstruction();
        }
      }
      case 0x18: getModRegRM(rmLegacyByte, rmLegacyByte, BYTET); return decodeGroup16();
      case 0x19: getModRegRM(rmReturnNull, rmReturnNull, NULL); return MAKE_INSN0(nop, nop);
      case 0x1A: getModRegRM(rmReturnNull, rmReturnNull, NULL); return MAKE_INSN0(nop, nop);
      case 0x1B: getModRegRM(rmReturnNull, rmReturnNull, NULL); return MAKE_INSN0(nop, nop);
      case 0x1C: getModRegRM(rmReturnNull, rmReturnNull, NULL); return MAKE_INSN0(nop, nop);
      case 0x1D: getModRegRM(rmReturnNull, rmReturnNull, NULL); return MAKE_INSN0(nop, nop);
      case 0x1E: getModRegRM(rmReturnNull, rmReturnNull, NULL); return MAKE_INSN0(nop, nop);
      case 0x1F: getModRegRM(rmReturnNull, rmReturnNull, NULL); return MAKE_INSN0(nop, nop);

   // BUG: The mode and type fields should forced to the current processor number of bits 
   // instead of the size determied by the operand size flag. See documentation for move 
   // to control register ("lock mov cr0, *").  This may be an AMD specific issue, but the 
   // operand size issues is a bug everywhere.
      case 0x20: getModRegRM(rmControl, effectiveOperandMode(), effectiveOperandType()); if (modeField == 3) return MAKE_INSN2(mov, mov, modrm, reg); else throw BadInstruction();
      case 0x21: getModRegRM(rmDebug, effectiveOperandMode(), effectiveOperandType()); if (modeField == 3) return MAKE_INSN2(mov, mov, modrm, reg); else throw BadInstruction();
      case 0x22: getModRegRM(rmControl, effectiveOperandMode(), effectiveOperandType()); if (modeField == 3) return MAKE_INSN2(mov, mov, reg, modrm); else throw BadInstruction();
      case 0x23: getModRegRM(rmDebug, effectiveOperandMode(), effectiveOperandType()); if (modeField == 3) return MAKE_INSN2(mov, mov, reg, modrm); else throw BadInstruction();
      case 0x24: throw BadInstruction(); // Move from test register
      case 0x25: throw BadInstruction();
      case 0x26: throw BadInstruction(); // Move to test register
      case 0x27: throw BadInstruction();
      case 0x28: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmXMM, rmXMM, V4FLOATT); return MAKE_INSN2(movaps, movaps, reg, modrm);
          case mmF3: throw BadInstruction();
          case mm66: getModRegRM(rmXMM, rmXMM, V2DOUBLET); return MAKE_INSN2(movapd, movapd, reg, modrm);
          case mmF2: throw BadInstruction();
        }
      }
      case 0x29: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmXMM, rmXMM, V4FLOATT); return MAKE_INSN2(movaps, movaps, modrm, reg);
          case mmF3: throw BadInstruction();
          case mm66: getModRegRM(rmXMM, rmXMM, V2DOUBLET); return MAKE_INSN2(movapd, movapd, modrm, reg);
          case mmF2: throw BadInstruction();
        }
      }
      case 0x2A: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmXMM, rmMM, V2DWORDT, V4FLOATT); return MAKE_INSN2(cvtpi2ps, cvtpi2ps, reg, modrm);
          case mmF3: getModRegRM(rmXMM, effectiveOperandMode(), effectiveOperandType(), V4FLOATT); return MAKE_INSN2(cvtsi2ss, cvtsi2ss, reg, modrm);
          case mm66: getModRegRM(rmXMM, rmMM, V2DWORDT, V2DOUBLET); return MAKE_INSN2(cvtpi2pd, cvtpi2pd, reg, modrm);
          case mmF2: getModRegRM(rmXMM, effectiveOperandMode(), effectiveOperandType(), V2DOUBLET); return MAKE_INSN2(cvtsi2sd, cvtsi2sd, reg, modrm);
        }
      }
      case 0x2B: {
        requireMemory();
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmXMM, rmXMM, V4FLOATT); return MAKE_INSN2(movntps, movntps, modrm, reg);
          case mmF3: getModRegRM(rmXMM, rmXMM, FLOATT); return MAKE_INSN2(movntss, movntss, modrm, reg);
          case mm66: getModRegRM(rmXMM, rmXMM, V2DOUBLET); return MAKE_INSN2(movntpd, movntpd, modrm, reg);
          case mmF2: getModRegRM(rmXMM, rmXMM, DOUBLET); return MAKE_INSN2(movntsd, movntsd, modrm, reg);
        }
      }
      case 0x2C: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmMM, rmXMM, V4FLOATT, V2DWORDT); return MAKE_INSN2(cvttps2pi, cvttps2pi, reg, modrm);
          case mmF3: getModRegRM(effectiveOperandMode(), rmXMM, V4FLOATT, effectiveOperandType()); return MAKE_INSN2(cvttss2si, cvttss2si, reg, modrm);
          case mm66: getModRegRM(rmMM, rmXMM, V2DOUBLET, V2DWORDT); return MAKE_INSN2(cvttpd2pi, cvttpd2pi, reg, modrm);
          case mmF2: getModRegRM(effectiveOperandMode(), rmXMM, V2DOUBLET, effectiveOperandType()); return MAKE_INSN2(cvttsd2si, cvttsd2si, reg, modrm);
        }
      }
      case 0x2D: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmMM, rmXMM, V4FLOATT, V2DWORDT); return MAKE_INSN2(cvtps2pi, cvtps2pi, reg, modrm);
          case mmF3: getModRegRM(effectiveOperandMode(), rmXMM, V4FLOATT, effectiveOperandType()); return MAKE_INSN2(cvtss2si, cvtss2si, reg, modrm);
          case mm66: getModRegRM(rmMM, rmXMM, V2DOUBLET, V2DWORDT); return MAKE_INSN2(cvtpd2pi, cvtpd2pi, reg, modrm);
          case mmF2: getModRegRM(effectiveOperandMode(), rmXMM, V2DOUBLET, effectiveOperandType()); return MAKE_INSN2(cvtsd2si, cvtsd2si, reg, modrm);
        }
      }
      case 0x2E: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmXMM, rmXMM, FLOATT); return MAKE_INSN2(ucomiss, ucomiss, reg, modrm);
          case mmF3: throw BadInstruction();
          case mm66: getModRegRM(rmXMM, rmXMM, DOUBLET); return MAKE_INSN2(ucomisd, ucomisd, reg, modrm);
          case mmF2: throw BadInstruction();
        }
      }
      case 0x2F: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmXMM, rmXMM, FLOATT); return MAKE_INSN2(comiss, comiss, reg, modrm);
          case mmF3: throw BadInstruction();
          case mm66: getModRegRM(rmXMM, rmXMM, DOUBLET); return MAKE_INSN2(comisd, comisd, reg, modrm);
          case mmF2: throw BadInstruction();
        }
      }
      case 0x30: return MAKE_INSN0(wrmsr, wrmsr);
      case 0x31: return MAKE_INSN0(rdtsc, rdtsc);
      case 0x32: return MAKE_INSN0(rdmsr, rdmsr);
      case 0x33: return MAKE_INSN0(rdpmc, rdpmc);
      case 0x34: not64(); return MAKE_INSN0(sysenter, sysenter);
      case 0x35: not64(); return MAKE_INSN0(sysexit, sysexit);
      case 0x36: throw BadInstruction();
      case 0x37: return MAKE_INSN0(getsec, getsec);

   // DQ (12/3/2008): Adding instruction support for SSSE3.
   // case 0x38: ROSE_ASSERT (!"0F38");
      case 0x38: decodeOpcode0F38();

      case 0x39: throw BadInstruction();
      case 0x3A: ROSE_ASSERT (!"0F3A");
      case 0x3B: throw BadInstruction();
      case 0x3C: throw BadInstruction();
      case 0x3D: throw BadInstruction();
      case 0x3E: throw BadInstruction();
      case 0x3F: throw BadInstruction();
      case 0x40: getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); return MAKE_INSN2(cmovo, cmovo, reg, modrm);
      case 0x41: getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); return MAKE_INSN2(cmovno, cmovno, reg, modrm);
      case 0x42: getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); return MAKE_INSN2(cmovb, cmovb, reg, modrm);
      case 0x43: getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); return MAKE_INSN2(cmovae, cmovae, reg, modrm);
      case 0x44: getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); return MAKE_INSN2(cmove, cmove, reg, modrm);
      case 0x45: getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); return MAKE_INSN2(cmovne, cmovne, reg, modrm);
      case 0x46: getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); return MAKE_INSN2(cmovbe, cmovbe, reg, modrm);
      case 0x47: getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); return MAKE_INSN2(cmova, cmova, reg, modrm);
      case 0x48: getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); return MAKE_INSN2(cmovs, cmovs, reg, modrm);
      case 0x49: getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); return MAKE_INSN2(cmovns, cmovns, reg, modrm);
      case 0x4A: getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); return MAKE_INSN2(cmovpe, cmovpe, reg, modrm);
      case 0x4B: getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); return MAKE_INSN2(cmovpo, cmovpo, reg, modrm);
      case 0x4C: getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); return MAKE_INSN2(cmovl, cmovl, reg, modrm);
      case 0x4D: getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); return MAKE_INSN2(cmovge, cmovge, reg, modrm);
      case 0x4E: getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); return MAKE_INSN2(cmovle, cmovle, reg, modrm);
      case 0x4F: getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); return MAKE_INSN2(cmovg, cmovg, reg, modrm);
      case 0x50: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmDWord, rmXMM, V4FLOATT); if (modeField == 3) return MAKE_INSN2(movmskps, movmskps, reg, modrm); else throw BadInstruction();
          case mmF3: throw BadInstruction();
          case mm66: getModRegRM(rmDWord, rmXMM, V2DOUBLET); if (modeField == 3) return MAKE_INSN2(movmskpd, movmskpd, reg, modrm); else throw BadInstruction();
          case mmF2: throw BadInstruction();
        }
      }
      case 0x51: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmXMM, rmXMM, V4FLOATT); return MAKE_INSN2(sqrtps, sqrtps, reg, modrm);
          case mmF3: getModRegRM(rmXMM, rmXMM, FLOATT); return MAKE_INSN2(sqrtss, sqrtss, reg, modrm);
          case mm66: getModRegRM(rmXMM, rmXMM, V2DOUBLET); return MAKE_INSN2(sqrtpd, sqrtpd, reg, modrm);
          case mmF2: getModRegRM(rmXMM, rmXMM, DOUBLET); return MAKE_INSN2(sqrtsd, sqrtsd, reg, modrm);
        }
      }
      case 0x52: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmXMM, rmXMM, V4FLOATT); return MAKE_INSN2(rsqrtps, rsqrtps, reg, modrm);
          case mmF3: getModRegRM(rmXMM, rmXMM, FLOATT); return MAKE_INSN2(rsqrtss, rsqrtss, reg, modrm);
          case mm66: throw BadInstruction();
          case mmF2: throw BadInstruction();
        }
      }
      case 0x53: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmXMM, rmXMM, V4FLOATT); return MAKE_INSN2(rcpps, rcpps, reg, modrm);
          case mmF3: getModRegRM(rmXMM, rmXMM, FLOATT); return MAKE_INSN2(rcpss, rcpss, reg, modrm);
          case mm66: throw BadInstruction();
          case mmF2: throw BadInstruction();
        }
      }
      case 0x54: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmXMM, rmXMM, V4FLOATT); return MAKE_INSN2(andps, andps, reg, modrm);
          case mmF3: throw BadInstruction();
          case mm66: getModRegRM(rmXMM, rmXMM, V4FLOATT); return MAKE_INSN2(andpd, andpd, reg, modrm);
          case mmF2: throw BadInstruction();
        }
      }
      case 0x55: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmXMM, rmXMM, V4FLOATT); return MAKE_INSN2(andnps, andnps, reg, modrm);
          case mmF3: throw BadInstruction();
          case mm66: getModRegRM(rmXMM, rmXMM, V2DOUBLET); return MAKE_INSN2(andnpd, andnpd, reg, modrm);
          case mmF2: throw BadInstruction();
        }
      }
      case 0x56: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmXMM, rmXMM, V4FLOATT); return MAKE_INSN2(orps, orps, reg, modrm);
          case mmF3: throw BadInstruction();
          case mm66: getModRegRM(rmXMM, rmXMM, V2DOUBLET); return MAKE_INSN2(orpd, orpd, reg, modrm);
          case mmF2: throw BadInstruction();
        }
      }
      case 0x57: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmXMM, rmXMM, V4FLOATT); return MAKE_INSN2(xorps, xorps, reg, modrm);
          case mmF3: throw BadInstruction();
          case mm66: getModRegRM(rmXMM, rmXMM, V2DOUBLET); return MAKE_INSN2(xorpd, xorpd, reg, modrm);
          case mmF2: throw BadInstruction();
        }
      }
      case 0x58: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmXMM, rmXMM, V4FLOATT); return MAKE_INSN2(addps, addps, reg, modrm);
          case mmF3: getModRegRM(rmXMM, rmXMM, FLOATT); return MAKE_INSN2(addss, addss, reg, modrm);
          case mm66: getModRegRM(rmXMM, rmXMM, V2DOUBLET); return MAKE_INSN2(addpd, addpd, reg, modrm);
          case mmF2: getModRegRM(rmXMM, rmXMM, DOUBLET); return MAKE_INSN2(addsd, addsd, reg, modrm);
        }
      }
      case 0x59: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmXMM, rmXMM, V4FLOATT); return MAKE_INSN2(mulps, mulps, reg, modrm);
          case mmF3: getModRegRM(rmXMM, rmXMM, FLOATT); return MAKE_INSN2(mulss, mulss, reg, modrm);
          case mm66: getModRegRM(rmXMM, rmXMM, V2DOUBLET); return MAKE_INSN2(mulpd, mulpd, reg, modrm);
          case mmF2: getModRegRM(rmXMM, rmXMM, DOUBLET); return MAKE_INSN2(mulsd, mulsd, reg, modrm);
        }
      }
      case 0x5A: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmXMM, rmXMM, V4FLOATT, V2DOUBLET); return MAKE_INSN2(cvtps2pd, cvtps2pd, reg, modrm);
          case mmF3: getModRegRM(rmXMM, rmXMM, FLOATT, V2DOUBLET); return MAKE_INSN2(cvtss2sd, cvtss2sd, reg, modrm);
          case mm66: getModRegRM(rmXMM, rmXMM, V2DOUBLET, V4FLOATT); return MAKE_INSN2(cvtpd2ps, cvtpd2ps, reg, modrm);
          case mmF2: getModRegRM(rmXMM, rmXMM, DOUBLET, V4FLOATT); return MAKE_INSN2(cvtsd2ss, cvtsd2ss, reg, modrm);
        }
      }
      case 0x5B: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmXMM, rmXMM, V4DWORDT, V4FLOATT); return MAKE_INSN2(cvtdq2ps, cvtdq2ps, reg, modrm);
          case mmF3: getModRegRM(rmXMM, rmXMM, V4FLOATT, V4DWORDT); return MAKE_INSN2(cvttps2dq, cvttps2dq, reg, modrm);
          case mm66: getModRegRM(rmXMM, rmXMM, V4FLOATT, V4DWORDT); return MAKE_INSN2(cvtps2dq, cvtps2dq, reg, modrm);
          case mmF2: throw BadInstruction();
        }
      }
      case 0x5C: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmXMM, rmXMM, V4FLOATT); return MAKE_INSN2(subps, subps, reg, modrm);
          case mmF3: getModRegRM(rmXMM, rmXMM, FLOATT); return MAKE_INSN2(subss, subss, reg, modrm);
          case mm66: getModRegRM(rmXMM, rmXMM, V2DOUBLET); return MAKE_INSN2(subpd, subpd, reg, modrm);
          case mmF2: getModRegRM(rmXMM, rmXMM, DOUBLET); return MAKE_INSN2(subsd, subsd, reg, modrm);
        }
      }
      case 0x5D: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmXMM, rmXMM, V4FLOATT); return MAKE_INSN2(minps, minps, reg, modrm);
          case mmF3: getModRegRM(rmXMM, rmXMM, FLOATT); return MAKE_INSN2(minss, minss, reg, modrm);
          case mm66: getModRegRM(rmXMM, rmXMM, V2DOUBLET); return MAKE_INSN2(minpd, minpd, reg, modrm);
          case mmF2: getModRegRM(rmXMM, rmXMM, DOUBLET); return MAKE_INSN2(minsd, minsd, reg, modrm);
        }
      }
      case 0x5E: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmXMM, rmXMM, V4FLOATT); return MAKE_INSN2(divps, divps, reg, modrm);
          case mmF3: getModRegRM(rmXMM, rmXMM, FLOATT); return MAKE_INSN2(divss, divss, reg, modrm);
          case mm66: getModRegRM(rmXMM, rmXMM, V2DOUBLET); return MAKE_INSN2(divpd, divpd, reg, modrm);
          case mmF2: getModRegRM(rmXMM, rmXMM, DOUBLET); return MAKE_INSN2(divsd, divsd, reg, modrm);
        }
      }
      case 0x5F: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmXMM, rmXMM, V4FLOATT); return MAKE_INSN2(maxps, maxps, reg, modrm);
          case mmF3: getModRegRM(rmXMM, rmXMM, FLOATT); return MAKE_INSN2(maxss, maxss, reg, modrm);
          case mm66: getModRegRM(rmXMM, rmXMM, V2DOUBLET); return MAKE_INSN2(maxpd, maxpd, reg, modrm);
          case mmF2: getModRegRM(rmXMM, rmXMM, DOUBLET); return MAKE_INSN2(maxsd, maxsd, reg, modrm);
        }
      }
      case 0x60: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmMM, rmMM, V8BYTET, V4WORDT); return MAKE_INSN2(punpcklbw, punpcklbw, reg, modrm);
          case mmF3: throw BadInstruction();
          case mm66: getModRegRM(rmXMM, rmXMM, V16BYTET, V8WORDT); return MAKE_INSN2(punpcklbw, punpcklbw, reg, modrm);
          case mmF2: throw BadInstruction();
        }
      }
      case 0x61: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmMM, rmMM, V4WORDT, V2DWORDT); return MAKE_INSN2(punpcklwd, punpcklwd, reg, modrm);
          case mmF3: throw BadInstruction();
          case mm66: getModRegRM(rmXMM, rmXMM, V8WORDT, V4DWORDT); return MAKE_INSN2(punpcklwd, punpcklwd, reg, modrm);
          case mmF2: throw BadInstruction();
        }
      }
      case 0x62: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmMM, rmMM, V2DWORDT, QWORDT); return MAKE_INSN2(punpckldq, punpckldq, reg, modrm);
          case mmF3: throw BadInstruction();
          case mm66: getModRegRM(rmXMM, rmXMM, V4DWORDT, V2QWORDT); return MAKE_INSN2(punpckldq, punpckldq, reg, modrm);
          case mmF2: throw BadInstruction();
        }
      }
      case 0x63: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmMM, rmMM, V4WORDT, V8BYTET); return MAKE_INSN2(packsswb, packsswb, reg, modrm);
          case mmF3: throw BadInstruction();
          case mm66: getModRegRM(rmXMM, rmXMM, V8WORDT, V16BYTET); return MAKE_INSN2(packsswb, packsswb, reg, modrm);
          case mmF2: throw BadInstruction();
        }
      }
      case 0x64: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmMM, rmMM, V8BYTET); return MAKE_INSN2(pcmpgtb, pcmpgtb, reg, modrm);
          case mmF3: throw BadInstruction();
          case mm66: getModRegRM(rmXMM, rmXMM, V16BYTET); return MAKE_INSN2(pcmpgtb, pcmpgtb, reg, modrm);
          case mmF2: throw BadInstruction();
        }
      }
      case 0x65: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmMM, rmMM, V4WORDT); return MAKE_INSN2(pcmpgtw, pcmpgtw, reg, modrm);
          case mmF3: throw BadInstruction();
          case mm66: getModRegRM(rmXMM, rmXMM, V8WORDT); return MAKE_INSN2(pcmpgtw, pcmpgtw, reg, modrm);
          case mmF2: throw BadInstruction();
        }
      }
      case 0x66: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmMM, rmMM, V2DWORDT); return MAKE_INSN2(pcmpgtd, pcmpgtd, reg, modrm);
          case mmF3: throw BadInstruction();
          case mm66: getModRegRM(rmXMM, rmXMM, V4DWORDT); return MAKE_INSN2(pcmpgtd, pcmpgtd, reg, modrm);
          case mmF2: throw BadInstruction();
        }
      }
      case 0x67: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmMM, rmMM, V4WORDT, V8BYTET); return MAKE_INSN2(packuswb, packuswb, reg, modrm);
          case mmF3: throw BadInstruction();
          case mm66: getModRegRM(rmXMM, rmXMM, V8WORDT, V16BYTET); return MAKE_INSN2(packuswb, packuswb, reg, modrm);
          case mmF2: throw BadInstruction();
        }
      }
      case 0x68: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmMM, rmMM, V8BYTET, V4WORDT); return MAKE_INSN2(punpckhbw, punpckhbw, reg, modrm);
          case mmF3: throw BadInstruction();
          case mm66: getModRegRM(rmXMM, rmXMM, V16BYTET, V8WORDT); return MAKE_INSN2(punpckhbw, punpckhbw, reg, modrm);
          case mmF2: throw BadInstruction();
        }
      }
      case 0x69: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmMM, rmMM, V4WORDT, V2DWORDT); return MAKE_INSN2(punpckhwd, punpckhwd, reg, modrm);
          case mmF3: throw BadInstruction();
          case mm66: getModRegRM(rmXMM, rmXMM, V8WORDT, V4DWORDT); return MAKE_INSN2(punpckhwd, punpckhwd, reg, modrm);
          case mmF2: throw BadInstruction();
        }
      }
      case 0x6A: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmMM, rmMM, V2DWORDT, QWORDT); return MAKE_INSN2(punpckhdq, punpckhdq, reg, modrm);
          case mmF3: throw BadInstruction();
          case mm66: getModRegRM(rmXMM, rmXMM, V4DWORDT, V2QWORDT); return MAKE_INSN2(punpckhdq, punpckhdq, reg, modrm);
          case mmF2: throw BadInstruction();
        }
      }
      case 0x6B: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmMM, rmMM, V2DWORDT, V4WORDT); return MAKE_INSN2(packssdw, packssdw, reg, modrm);
          case mmF3: throw BadInstruction();
          case mm66: getModRegRM(rmXMM, rmXMM, V4DWORDT, V8WORDT); return MAKE_INSN2(packssdw, packssdw, reg, modrm);
          case mmF2: throw BadInstruction();
        }
      }
      case 0x6C: {
        switch (mmPrefix()) {
          case mmNone: throw BadInstruction();
          case mmF3: throw BadInstruction();
          case mm66: getModRegRM(rmXMM, rmXMM, V2QWORDT, DQWORDT); return MAKE_INSN2(punpcklqdq, punpcklqdq, reg, modrm);
          case mmF2: throw BadInstruction();
        }
      }
      case 0x6D: {
        switch (mmPrefix()) {
          case mmNone: throw BadInstruction();
          case mmF3: throw BadInstruction();
          case mm66: getModRegRM(rmXMM, rmXMM, V2QWORDT, DQWORDT); return MAKE_INSN2(punpckhqdq, punpckhqdq, reg, modrm);
          case mmF2: throw BadInstruction();
        }
      }
      case 0x6E: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmMM, effectiveOperandMode(), effectiveOperandType(), (effectiveOperandSize() == x86_insnsize_64 ? (SgAsmType*)QWORDT : V2DWORDT)); return MAKE_INSN2(movd, movd, reg, modrm);
          case mmF3: throw BadInstruction();
          case mm66: getModRegRM(rmXMM, effectiveOperandMode(), effectiveOperandType(), (effectiveOperandSize() == x86_insnsize_64 ? V2QWORDT : V4DWORDT)); return MAKE_INSN2(movd, movd, reg, modrm);
          case mmF2: throw BadInstruction();
        }
      }
      case 0x6F: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmMM, rmMM, QWORDT); return MAKE_INSN2(movq, movq, reg, modrm);
          case mmF3: getModRegRM(rmXMM, rmXMM, DQWORDT); return MAKE_INSN2(movdqu, movdqu, reg, modrm);
          case mm66: getModRegRM(rmXMM, rmXMM, DQWORDT); return MAKE_INSN2(movdqa, movdqa, reg, modrm);
          case mmF2: throw BadInstruction();
        }
      }
      case 0x70: {
        switch (mmPrefix()) {
          case mmNone: {getModRegRM(rmMM, rmMM, V4WORDT); SgAsmExpression* shufConstant = getImmByte(); return MAKE_INSN3(pshufw, pshufw, reg, modrm, shufConstant);}
          case mmF3: {getModRegRM(rmXMM, rmXMM, V8WORDT); SgAsmExpression* shufConstant = getImmByte(); return MAKE_INSN3(pshufhw, pshufhw, reg, modrm, shufConstant);}
          case mm66: {getModRegRM(rmXMM, rmXMM, V4DWORDT); SgAsmExpression* shufConstant = getImmByte(); return MAKE_INSN3(pshufd, pshufd, reg, modrm, shufConstant);}
          case mmF2: {getModRegRM(rmXMM, rmXMM, V8WORDT); SgAsmExpression* shufConstant = getImmByte(); return MAKE_INSN3(pshuflw, pshuflw, reg, modrm, shufConstant);}
        }
      }
      case 0x71: { // Group 12
        switch (mmPrefix()) {
          case mmNone: {
            getModRegRM(rmReturnNull, rmMM, V4WORDT);
            if (modeField != 3) throw BadInstruction();
            SgAsmExpression* shiftAmount = getImmByte();
            switch (regField) {
              case 0: throw BadInstruction();
              case 1: throw BadInstruction();
              case 2: return MAKE_INSN2(psrlw, psrlw, modrm, shiftAmount);
              case 3: throw BadInstruction();
              case 4: return MAKE_INSN2(psraw, psraw, modrm, shiftAmount);
              case 5: throw BadInstruction();
              case 6: return MAKE_INSN2(psllw, psllw, modrm, shiftAmount);
              case 7: throw BadInstruction();
              default: ROSE_ASSERT (false);
            }
          }
          case mmF3: throw BadInstruction();
          case mm66: {
            getModRegRM(rmReturnNull, rmXMM, V8WORDT);
            if (modeField != 3) throw BadInstruction();
            SgAsmExpression* shiftAmount = getImmByte();
            switch (regField) {
              case 0: throw BadInstruction();
              case 1: throw BadInstruction();
              case 2: return MAKE_INSN2(psrlw, psrlw, modrm, shiftAmount);
              case 3: throw BadInstruction();
              case 4: return MAKE_INSN2(psraw, psraw, modrm, shiftAmount);
              case 5: throw BadInstruction();
              case 6: return MAKE_INSN2(psllw, psllw, modrm, shiftAmount);
              case 7: throw BadInstruction();
              default: ROSE_ASSERT (false);
            }
          }
          case mmF2: throw BadInstruction();
        }
      }
      case 0x72: { // Group 13
        switch (mmPrefix()) {
          case mmNone: {
            getModRegRM(rmReturnNull, rmMM, V2DWORDT);
            if (modeField != 3) throw BadInstruction();
            SgAsmExpression* shiftAmount = getImmByte();
            switch (regField) {
              case 0: throw BadInstruction();
              case 1: throw BadInstruction();
              case 2: return MAKE_INSN2(psrld, psrld, modrm, shiftAmount);
              case 3: throw BadInstruction();
              case 4: return MAKE_INSN2(psrad, psrad, modrm, shiftAmount);
              case 5: throw BadInstruction();
              case 6: return MAKE_INSN2(pslld, pslld, modrm, shiftAmount);
              case 7: throw BadInstruction();
              default: ROSE_ASSERT (false);
            }
          }
          case mmF3: throw BadInstruction();
          case mm66: {
            getModRegRM(rmReturnNull, rmXMM, V4DWORDT);
            if (modeField != 3) throw BadInstruction();
            SgAsmExpression* shiftAmount = getImmByte();
            switch (regField) {
              case 0: throw BadInstruction();
              case 1: throw BadInstruction();
              case 2: return MAKE_INSN2(psrld, psrld, modrm, shiftAmount);
              case 3: throw BadInstruction();
              case 4: return MAKE_INSN2(psrad, psrad, modrm, shiftAmount);
              case 5: throw BadInstruction();
              case 6: return MAKE_INSN2(pslld, pslld, modrm, shiftAmount);
              case 7: throw BadInstruction();
              default: ROSE_ASSERT (false);
            }
          }
          case mmF2: throw BadInstruction();
        }
      }
      case 0x73: { // Group 14
        switch (mmPrefix()) {
          case mmNone: {
            getModRegRM(rmReturnNull, rmMM, QWORDT);
            if (modeField != 3) throw BadInstruction();
            SgAsmExpression* shiftAmount = getImmByte();
            switch (regField) {
              case 0: throw BadInstruction();
              case 1: throw BadInstruction();
              case 2: return MAKE_INSN2(psrlq, psrlq, modrm, shiftAmount);
              case 3: throw BadInstruction();
              case 4: return MAKE_INSN2(psraq, psraq, modrm, shiftAmount);
              case 5: throw BadInstruction();
              case 6: return MAKE_INSN2(psllq, psllq, modrm, shiftAmount);
              case 7: throw BadInstruction();
              default: ROSE_ASSERT (false);
            }
          }
          case mmF3: throw BadInstruction();
          case mm66: {
            getModRegRM(rmReturnNull, rmXMM, V2QWORDT);
            if (modeField != 3) throw BadInstruction();
            SgAsmExpression* shiftAmount = getImmByte();
            switch (regField) {
              case 0: throw BadInstruction();
              case 1: throw BadInstruction();
              case 2: return MAKE_INSN2(psrlq, psrlq, modrm, shiftAmount);
              case 3: isSgAsmx86RegisterReferenceExpression(modrm)->set_type(DQWORDT); return MAKE_INSN2(psrldq, psrldq, modrm, shiftAmount);
              case 4: throw BadInstruction();
              case 5: throw BadInstruction();
              case 6: return MAKE_INSN2(psllq, psllq, modrm, shiftAmount);
              case 7: isSgAsmx86RegisterReferenceExpression(modrm)->set_type(DQWORDT); return MAKE_INSN2(pslldq, pslldq, modrm, shiftAmount);
              default: ROSE_ASSERT (false);
            }
          }
          case mmF2: throw BadInstruction();
        }
      }
      case 0x74: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmMM, rmMM, V8BYTET); return MAKE_INSN2(pcmpeqb, pcmpeqb, reg, modrm);
          case mmF3: throw BadInstruction();
          case mm66: getModRegRM(rmXMM, rmXMM, V16BYTET); return MAKE_INSN2(pcmpeqb, pcmpeqb, reg, modrm);
          case mmF2: throw BadInstruction();
        }
      }
      case 0x75: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmMM, rmMM, V4WORDT); return MAKE_INSN2(pcmpeqw, pcmpeqw, reg, modrm);
          case mmF3: throw BadInstruction();
          case mm66: getModRegRM(rmXMM, rmXMM, V8WORDT); return MAKE_INSN2(pcmpeqw, pcmpeqw, reg, modrm);
          case mmF2: throw BadInstruction();
        }
      }
      case 0x76: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmMM, rmMM, V2DWORDT); return MAKE_INSN2(pcmpeqd, pcmpeqd, reg, modrm);
          case mmF3: throw BadInstruction();
          case mm66: getModRegRM(rmXMM, rmXMM, V4DWORDT); return MAKE_INSN2(pcmpeqd, pcmpeqd, reg, modrm);
          case mmF2: throw BadInstruction();
        }
      }
      case 0x77: {
        switch (mmPrefix()) {
          case mmNone: return MAKE_INSN0(emms, emms);
          case mmF3: throw BadInstruction();
          case mm66: throw BadInstruction();
          case mmF2: throw BadInstruction();
        }
      }
      case 0x78: {
        switch (mmPrefix()) {
          case mmNone: ROSE_ASSERT (!"vmread not supported");
          case mmF3: throw BadInstruction();
          case mm66: { // Group 17
            getModRegRM(rmReturnNull, rmXMM, DQWORDT);
            SgAsmExpression* imm1 = getImmByte();
            SgAsmExpression* imm2 = getImmByte();
            switch (regField) {
              case 0: 
                if (modeField == 3)
                  return MAKE_INSN3(extrq, extrq, modrm, imm1, imm2);
                else
                  throw BadInstruction();
              default: throw BadInstruction();
            }
          }
          case mmF2: {
            getModRegRM(rmXMM, rmXMM, QWORDT, DQWORDT);
            SgAsmExpression* imm1 = getImmByte();
            SgAsmExpression* imm2 = getImmByte();
            if (modeField == 3)
              return MAKE_INSN4(insertq, insertq, reg, modrm, imm1, imm2);
            else
              throw BadInstruction();
          }
        }
      }
      case 0x79: {
        switch (mmPrefix()) {
          case mmNone: ROSE_ASSERT (!"vmwrite not supported");
          case mmF3: throw BadInstruction();
          case mm66: getModRegRM(rmXMM, rmXMM, QWORDT, DQWORDT); if (modeField == 3) return MAKE_INSN2(extrq, extrq, reg, modrm); else throw BadInstruction();
          case mmF2: getModRegRM(rmXMM, rmXMM, DQWORDT); if (modeField == 3) return MAKE_INSN2(insertq, insertq, reg, modrm); else throw BadInstruction();
        }
      }
      case 0x7A: throw BadInstruction();
      case 0x7B: throw BadInstruction();
      case 0x7C: {
        switch (mmPrefix()) {
          case mmNone: throw BadInstruction();
          case mmF3: throw BadInstruction();
          case mm66: getModRegRM(rmXMM, rmXMM, V2DOUBLET); return MAKE_INSN2(haddpd, haddpd, reg, modrm);
          case mmF2: getModRegRM(rmXMM, rmXMM, V4FLOATT); return MAKE_INSN2(haddps, haddps, reg, modrm);
        }
      }
      case 0x7D: {
        switch (mmPrefix()) {
          case mmNone: throw BadInstruction();
          case mmF3: throw BadInstruction();
          case mm66: getModRegRM(rmXMM, rmXMM, V2DOUBLET); return MAKE_INSN2(hsubpd, hsubpd, reg, modrm);
          case mmF2: getModRegRM(rmXMM, rmXMM, V4FLOATT); return MAKE_INSN2(hsubps, hsubps, reg, modrm);
        }
      }
      case 0x7E: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmMM, effectiveOperandMode(), effectiveOperandType(), (effectiveOperandSize() == x86_insnsize_64 ? (SgAsmType*)QWORDT : V2DWORDT)); return MAKE_INSN2(movd, movd, modrm, reg);
          case mmF3: getModRegRM(rmXMM, rmXMM, V2QWORDT); return MAKE_INSN2(movq, movq, reg, modrm);
          case mm66: getModRegRM(rmXMM, effectiveOperandMode(), effectiveOperandType(), (effectiveOperandSize() == x86_insnsize_64 ? V2QWORDT : V4DWORDT)); return MAKE_INSN2(movd, movd, modrm, reg);
          case mmF2: throw BadInstruction();
        }
      }
      case 0x7F: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmMM, rmMM, QWORDT); return MAKE_INSN2(movq, movq, modrm, reg);
          case mmF3: getModRegRM(rmXMM, rmXMM, DQWORDT); return MAKE_INSN2(movdqu, movdqu, modrm, reg);
          case mm66: getModRegRM(rmXMM, rmXMM, DQWORDT); return MAKE_INSN2(movdqa, movdqa, modrm, reg);
          case mmF2: throw BadInstruction();
        }
      }
      case 0x80: {SgAsmExpression* imm = getImmJz(); branchPredictionEnabled = true; return MAKE_INSN1(jo , jo, imm);}
      case 0x81: {SgAsmExpression* imm = getImmJz(); branchPredictionEnabled = true; return MAKE_INSN1(jno, jno, imm);}
      case 0x82: {SgAsmExpression* imm = getImmJz(); branchPredictionEnabled = true; return MAKE_INSN1(jb,  jb, imm);}
      case 0x83: {SgAsmExpression* imm = getImmJz(); branchPredictionEnabled = true; return MAKE_INSN1(jae, jae, imm);}
      case 0x84: {SgAsmExpression* imm = getImmJz(); branchPredictionEnabled = true; return MAKE_INSN1(je,  je, imm);}
      case 0x85: {SgAsmExpression* imm = getImmJz(); branchPredictionEnabled = true; return MAKE_INSN1(jne, jne, imm);}
      case 0x86: {SgAsmExpression* imm = getImmJz(); branchPredictionEnabled = true; return MAKE_INSN1(jbe, jbe, imm);}
      case 0x87: {SgAsmExpression* imm = getImmJz(); branchPredictionEnabled = true; return MAKE_INSN1(ja,  ja, imm);}
      case 0x88: {SgAsmExpression* imm = getImmJz(); branchPredictionEnabled = true; return MAKE_INSN1(js , js, imm);}
      case 0x89: {SgAsmExpression* imm = getImmJz(); branchPredictionEnabled = true; return MAKE_INSN1(jns, jns, imm);}
      case 0x8A: {SgAsmExpression* imm = getImmJz(); branchPredictionEnabled = true; return MAKE_INSN1(jpe, jpe, imm);}
      case 0x8B: {SgAsmExpression* imm = getImmJz(); branchPredictionEnabled = true; return MAKE_INSN1(jpo, jpo, imm);}
      case 0x8C: {SgAsmExpression* imm = getImmJz(); branchPredictionEnabled = true; return MAKE_INSN1(jl,  jl, imm);}
      case 0x8D: {SgAsmExpression* imm = getImmJz(); branchPredictionEnabled = true; return MAKE_INSN1(jge, jge, imm);}
      case 0x8E: {SgAsmExpression* imm = getImmJz(); branchPredictionEnabled = true; return MAKE_INSN1(jle, jle, imm);}
      case 0x8F: {SgAsmExpression* imm = getImmJz(); branchPredictionEnabled = true; return MAKE_INSN1(jg,  jg, imm);}
      case 0x90: getModRegRM(rmLegacyByte, rmLegacyByte, BYTET); return MAKE_INSN1(seto,  seto, modrm);
      case 0x91: getModRegRM(rmLegacyByte, rmLegacyByte, BYTET); return MAKE_INSN1(setno, setno, modrm);
      case 0x92: getModRegRM(rmLegacyByte, rmLegacyByte, BYTET); return MAKE_INSN1(setb,  setb, modrm);
      case 0x93: getModRegRM(rmLegacyByte, rmLegacyByte, BYTET); return MAKE_INSN1(setae, setae, modrm);
      case 0x94: getModRegRM(rmLegacyByte, rmLegacyByte, BYTET); return MAKE_INSN1(sete,  sete, modrm);
      case 0x95: getModRegRM(rmLegacyByte, rmLegacyByte, BYTET); return MAKE_INSN1(setne, setne, modrm);
      case 0x96: getModRegRM(rmLegacyByte, rmLegacyByte, BYTET); return MAKE_INSN1(setbe, setbe, modrm);
      case 0x97: getModRegRM(rmLegacyByte, rmLegacyByte, BYTET); return MAKE_INSN1(seta,  seta, modrm);
      case 0x98: getModRegRM(rmLegacyByte, rmLegacyByte, BYTET); return MAKE_INSN1(sets,  sets, modrm);
      case 0x99: getModRegRM(rmLegacyByte, rmLegacyByte, BYTET); return MAKE_INSN1(setns, setns, modrm);
      case 0x9A: getModRegRM(rmLegacyByte, rmLegacyByte, BYTET); return MAKE_INSN1(setpe, setpe, modrm);
      case 0x9B: getModRegRM(rmLegacyByte, rmLegacyByte, BYTET); return MAKE_INSN1(setpo, setpo, modrm);
      case 0x9C: getModRegRM(rmLegacyByte, rmLegacyByte, BYTET); return MAKE_INSN1(setl,  setl, modrm);
      case 0x9D: getModRegRM(rmLegacyByte, rmLegacyByte, BYTET); return MAKE_INSN1(setge, setge, modrm);
      case 0x9E: getModRegRM(rmLegacyByte, rmLegacyByte, BYTET); return MAKE_INSN1(setle, setle, modrm);
      case 0x9F: getModRegRM(rmLegacyByte, rmLegacyByte, BYTET); return MAKE_INSN1(setg,  setg, modrm);
      case 0xA0: return MAKE_INSN1(push, push, makeRegister(4, rmSegment));
      case 0xA1: return MAKE_INSN1(pop, pop, makeRegister(4, rmSegment));
      case 0xA2: return MAKE_INSN0(cpuid, cpuid);
      case 0xA3: getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); return MAKE_INSN2(bt, bt, modrm, reg);
      case 0xA4: getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); return MAKE_INSN3(shld, shld, modrm, reg, getImmByte());
      case 0xA5: getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); return MAKE_INSN3(shld, shld, modrm, reg, makeRegister(1, rmLegacyByte));
      case 0xA6: throw BadInstruction();
      case 0xA7: throw BadInstruction();
      case 0xA8: return MAKE_INSN1(push, push, makeRegister(5, rmSegment));
      case 0xA9: return MAKE_INSN1(pop, pop, makeRegister(5, rmSegment));
      case 0xAA: return MAKE_INSN0(rsm, rsm);
      case 0xAB: getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); return MAKE_INSN2(btr, btr, modrm, reg);
      case 0xAC: getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); return MAKE_INSN3(shrd, shrd, modrm, reg, getImmByte());
      case 0xAD: getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); return MAKE_INSN3(shrd, shrd, modrm, reg, makeRegister(1, rmLegacyByte));
      case 0xAE: return decodeGroup15();
      case 0xAF: getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); return MAKE_INSN2(imul, imul, reg, modrm);
      case 0xB0: getModRegRM(rmLegacyByte, rmLegacyByte, BYTET); return MAKE_INSN2(cmpxchg, cmpxchg, modrm, reg);
      case 0xB1: getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); return MAKE_INSN2(cmpxchg, cmpxchg, modrm, reg);
      case 0xB2: getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); requireMemory(); return MAKE_INSN2(lss, lss, reg, modrm);
      case 0xB3: getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); return MAKE_INSN2(bts, bts, modrm, reg);
      case 0xB4: getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); requireMemory(); return MAKE_INSN2(lfs, lfs, reg, modrm);
      case 0xB5: getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); requireMemory(); return MAKE_INSN2(lgs, lgs, reg, modrm);
      case 0xB6: getModRegRM(effectiveOperandMode(), rmLegacyByte, BYTET); return MAKE_INSN2(movzx, movzx, reg, modrm);
      case 0xB7: getModRegRM(effectiveOperandMode(), rmWord, WORDT); return MAKE_INSN2(movzx, movzx, reg, modrm);
      case 0xB8: {
        getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType());

     // Here is an example of the existence of a prefix leading to two very different instructions.
        switch (mmPrefix()) {
          case mmNone: isUnconditionalJump = true; return MAKE_INSN1(jmpe, jmpe, modrm);
          case mmF3: return MAKE_INSN2(popcnt, popcnt, reg, modrm);
          default: throw BadInstruction();
        }
      }
      case 0xB9: throw BadInstruction();
      case 0xBA: getModRegRM(rmReturnNull, effectiveOperandMode(), effectiveOperandType()); return decodeGroup8(getImmByte());
      case 0xBB: getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); return MAKE_INSN2(btc, btc, modrm, reg);
      case 0xBC: getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); return MAKE_INSN2(bsf, bsf, reg, modrm);
      case 0xBD: getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); if (repeatPrefix == rpRepe) return MAKE_INSN2(lzcnt, lzcnt, reg, modrm); else return MAKE_INSN2(bsr, bsr, reg, modrm);
      case 0xBE: getModRegRM(effectiveOperandMode(), rmLegacyByte, BYTET); return MAKE_INSN2(movsx, movsx, reg, modrm);
      case 0xBF: getModRegRM(effectiveOperandMode(), rmWord, WORDT); return MAKE_INSN2(movsx, movsx, reg, modrm);
      case 0xC0: getModRegRM(rmLegacyByte, rmLegacyByte, BYTET); return MAKE_INSN2(xadd, xadd, modrm, reg);
      case 0xC1: getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); return MAKE_INSN2(xadd, xadd, modrm, reg);
      case 0xC2: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmXMM, rmXMM, V4FLOATT); return MAKE_INSN2(cmpps, cmpps, reg, modrm);
          case mmF3: getModRegRM(rmXMM, rmXMM, FLOATT); return MAKE_INSN2(cmpss, cmpss, reg, modrm);
          case mm66: getModRegRM(rmXMM, rmXMM, V2DOUBLET); return MAKE_INSN2(cmppd, cmppd, reg, modrm);
          case mmF2: getModRegRM(rmXMM, rmXMM, DOUBLET); return MAKE_INSN2(cmpsd, cmpsd, reg, modrm);
        }
      }
      case 0xC3: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType()); requireMemory(); return MAKE_INSN2(movnti, movnti, modrm, reg);
          case mmF3: throw BadInstruction();
          case mm66: throw BadInstruction();
          case mmF2: throw BadInstruction();
        }
      }
      case 0xC4: {
        switch (mmPrefix()) {
          case mmNone: {
            getModRegRM(rmMM, rmWord, WORDT, QWORDT);
            SgAsmExpression* imm = getImmByte();
            return MAKE_INSN3(pinsrw, pinsrw, reg, modrm, imm);
          }
          case mmF3: throw BadInstruction();
          case mm66: {
            getModRegRM(rmXMM, rmWord, WORDT, DQWORDT);
            SgAsmExpression* imm = getImmByte();
            return MAKE_INSN3(pinsrw, pinsrw, reg, modrm, imm);
          }
          case mmF2: throw BadInstruction();
        }
      }
      case 0xC5: {
        switch (mmPrefix()) {
          case mmNone: {
            getModRegRM(rmDWord, rmMM, V4WORDT, DWORDT);
            SgAsmExpression* imm = getImmByte();
            if (modeField == 3) return MAKE_INSN3(pextrw, pextrw, reg, modrm, imm); else throw BadInstruction();
          }
          case mmF3: throw BadInstruction();
          case mm66: {
            getModRegRM(rmDWord, rmXMM, V8WORDT, DWORDT);
            SgAsmExpression* imm = getImmByte();
            if (modeField == 3) return MAKE_INSN3(pextrw, pextrw, reg, modrm, imm); else throw BadInstruction();
          }
          case mmF2: throw BadInstruction();
        }
      }
      case 0xC6: {
        switch (mmPrefix()) {
          case mmNone: {
            getModRegRM(rmXMM, rmXMM, V4FLOATT);
            SgAsmExpression* shufConstant = getImmByte();
            return MAKE_INSN3(shufps, shufps, reg, modrm, shufConstant);
          }
          case mmF3: throw BadInstruction();
          case mm66: {
            getModRegRM(rmXMM, rmXMM, V2DOUBLET);
            SgAsmExpression* shufConstant = getImmByte();
            return MAKE_INSN3(shufpd, shufpd, reg, modrm, shufConstant);
          }
          case mmF2: throw BadInstruction();
        }
      }
      case 0xC7: { // Group 9
        getModRegRM(rmReturnNull, rmReturnNull, NULL);
        requireMemory();
        switch (regField) {
          case 1: {
            if (effectiveOperandSize() == x86_insnsize_64) {
              fillInModRM(rmReturnNull, DQWORDT);
              return MAKE_INSN1(cmpxchg16b, cmpxchg16b, modrm);
            } else {
              fillInModRM(rmReturnNull, QWORDT);
              return MAKE_INSN1(cmpxchg8b, cmpxchg8b, modrm);
            }
          }
          case 6: {
            fillInModRM(rmReturnNull, QWORDT);
            switch (mmPrefix()) {
              case mmNone: return MAKE_INSN1(vmptrld, vmptrld, modrm);
              case mmF3: return MAKE_INSN1(vmxon, vmxon, modrm);
              case mm66: return MAKE_INSN1(vmclear, vmclear, modrm);
              default: throw BadInstruction();
            }
          }
          case 7: {
            fillInModRM(rmReturnNull, QWORDT);
            return MAKE_INSN1(vmptrst, vmptrst, modrm);
          }
          default: throw BadInstruction();
        }
      }
      case 0xC8: return MAKE_INSN1(bswap, bswap, makeRegisterEffective(rexB, 0));
      case 0xC9: return MAKE_INSN1(bswap, bswap, makeRegisterEffective(rexB, 1));
      case 0xCA: return MAKE_INSN1(bswap, bswap, makeRegisterEffective(rexB, 2));
      case 0xCB: return MAKE_INSN1(bswap, bswap, makeRegisterEffective(rexB, 3));
      case 0xCC: return MAKE_INSN1(bswap, bswap, makeRegisterEffective(rexB, 4));
      case 0xCD: return MAKE_INSN1(bswap, bswap, makeRegisterEffective(rexB, 5));
      case 0xCE: return MAKE_INSN1(bswap, bswap, makeRegisterEffective(rexB, 6));
      case 0xCF: return MAKE_INSN1(bswap, bswap, makeRegisterEffective(rexB, 7));
      case 0xD0: {
        switch (mmPrefix()) {
          case mmNone: throw BadInstruction();
          case mmF3: throw BadInstruction();
          case mm66: getModRegRM(rmXMM, rmXMM, V2DOUBLET); return MAKE_INSN2(addsubpd, addsubpd, reg, modrm);
          case mmF2: getModRegRM(rmXMM, rmXMM, V4FLOATT); return MAKE_INSN2(addsubps, addsubps, reg, modrm);
        }
      }
      case 0xD1: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmMM, rmMM, V4WORDT); return MAKE_INSN2(psrlw, psrlw, reg, modrm);
          case mmF3: throw BadInstruction();
          case mm66: getModRegRM(rmXMM, rmXMM, V8WORDT); return MAKE_INSN2(psrlw, psrlw, reg, modrm);
          case mmF2: throw BadInstruction();
        }
      }
      case 0xD2: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmMM, rmMM, V2DWORDT); return MAKE_INSN2(psrld, psrld, reg, modrm);
          case mmF3: throw BadInstruction();
          case mm66: getModRegRM(rmXMM, rmXMM, V4DWORDT); return MAKE_INSN2(psrld, psrld, reg, modrm);
          case mmF2: throw BadInstruction();
        }
      }
      case 0xD3: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmMM, rmMM, QWORDT); return MAKE_INSN2(psrlq, psrlq, reg, modrm);
          case mmF3: throw BadInstruction();
          case mm66: getModRegRM(rmXMM, rmXMM, V2QWORDT); return MAKE_INSN2(psrlq, psrlq, reg, modrm);
          case mmF2: throw BadInstruction();
        }
      }
      case 0xD4: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmMM, rmMM, QWORDT); return MAKE_INSN2(paddq, paddq, reg, modrm);
          case mmF3: throw BadInstruction();
          case mm66: getModRegRM(rmXMM, rmXMM, V2QWORDT); return MAKE_INSN2(paddq, paddq, reg, modrm);
          case mmF2: throw BadInstruction();
        }
      }
      case 0xD5: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmMM, rmMM, V4WORDT); return MAKE_INSN2(pmullw, pmullw, reg, modrm);
          case mmF3: throw BadInstruction();
          case mm66: getModRegRM(rmXMM, rmXMM, V8WORDT); return MAKE_INSN2(pmullw, pmullw, reg, modrm);
          case mmF2: throw BadInstruction();
        }
      }
      case 0xD6: {
        switch (mmPrefix()) {
          case mmNone: throw BadInstruction();
          case mmF3: getModRegRM(rmMM, rmXMM, QWORDT, DQWORDT); if (modeField == 3) return MAKE_INSN2(movq2dq, movq2dq, reg, modrm); else throw BadInstruction();
          case mm66: getModRegRM(rmXMM, rmXMM, QWORDT); return MAKE_INSN2(movq, movq, modrm, reg);
          case mmF2: getModRegRM(rmXMM, rmMM, DQWORDT, QWORDT); if (modeField == 3) return MAKE_INSN2(movdq2q, movdq2q, reg, modrm); else throw BadInstruction();
        }
      }
      case 0xD7: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmDWord, rmMM, V8BYTET, DWORDT); if (modeField == 3) return MAKE_INSN2(pmovmskb, pmovmskb, reg, modrm); else throw BadInstruction();
          case mmF3: throw BadInstruction();
          case mm66: getModRegRM(rmDWord, rmXMM, V16BYTET, DWORDT); if (modeField == 3) return MAKE_INSN2(pmovmskb, pmovmskb, reg, modrm); else throw BadInstruction();
          case mmF2: throw BadInstruction();
        }
      }
      case 0xD8: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmMM, rmMM, V8BYTET); return MAKE_INSN2(psubusb, psubusb, reg, modrm);
          case mmF3: throw BadInstruction();
          case mm66: getModRegRM(rmXMM, rmXMM, V16BYTET); return MAKE_INSN2(psubusb, psubusb, reg, modrm);
          case mmF2: throw BadInstruction();
        }
      }
      case 0xD9: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmMM, rmMM, V4WORDT); return MAKE_INSN2(psubusw, psubusw, reg, modrm);
          case mmF3: throw BadInstruction();
          case mm66: getModRegRM(rmXMM, rmXMM, V8WORDT); return MAKE_INSN2(psubusw, psubusw, reg, modrm);
          case mmF2: throw BadInstruction();
        }
      }
      case 0xDA: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmMM, rmMM, V8BYTET); return MAKE_INSN2(pminub, pminub, reg, modrm);
          case mmF3: throw BadInstruction();
          case mm66: getModRegRM(rmXMM, rmXMM, V16BYTET); return MAKE_INSN2(pminub, pminub, reg, modrm);
          case mmF2: throw BadInstruction();
        }
      }
      case 0xDB: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmMM, rmMM, QWORDT); return MAKE_INSN2(pand, pand, reg, modrm);
          case mmF3: throw BadInstruction();
          case mm66: getModRegRM(rmXMM, rmXMM, DQWORDT); return MAKE_INSN2(pand, pand, reg, modrm);
          case mmF2: throw BadInstruction();
        }
      }
      case 0xDC: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmMM, rmMM, V8BYTET); return MAKE_INSN2(paddusb, paddusb, reg, modrm);
          case mmF3: throw BadInstruction();
          case mm66: getModRegRM(rmXMM, rmXMM, V16BYTET); return MAKE_INSN2(paddusb, paddusb, reg, modrm);
          case mmF2: throw BadInstruction();
        }
      }
      case 0xDD: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmMM, rmMM, V4WORDT); return MAKE_INSN2(paddusw, paddusw, reg, modrm);
          case mmF3: throw BadInstruction();
          case mm66: getModRegRM(rmXMM, rmXMM, V8WORDT); return MAKE_INSN2(paddusw, paddusw, reg, modrm);
          case mmF2: throw BadInstruction();
        }
      }
      case 0xDE: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmMM, rmMM, V8BYTET); return MAKE_INSN2(pmaxub, pmaxub, reg, modrm);
          case mmF3: throw BadInstruction();
          case mm66: getModRegRM(rmXMM, rmXMM, V16BYTET); return MAKE_INSN2(pmaxub, pmaxub, reg, modrm);
          case mmF2: throw BadInstruction();
        }
      }
      case 0xDF: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmMM, rmMM, QWORDT); return MAKE_INSN2(pandn, pandn, reg, modrm);
          case mmF3: throw BadInstruction();
          case mm66: getModRegRM(rmXMM, rmXMM, DQWORDT); return MAKE_INSN2(pandn, pandn, reg, modrm);
          case mmF2: throw BadInstruction();
        }
      }
      case 0xE0: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmMM, rmMM, V8BYTET); return MAKE_INSN2(pavgb, pavgb, reg, modrm);
          case mmF3: throw BadInstruction();
          case mm66: getModRegRM(rmXMM, rmXMM, V16BYTET); return MAKE_INSN2(pavgb, pavgb, reg, modrm);
          case mmF2: throw BadInstruction();
        }
      }
      case 0xE1: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmMM, rmMM, V4WORDT); return MAKE_INSN2(psraw, psraw, reg, modrm);
          case mmF3: throw BadInstruction();
          case mm66: getModRegRM(rmXMM, rmXMM, V8WORDT); return MAKE_INSN2(psraw, psraw, reg, modrm);
          case mmF2: throw BadInstruction();
        }
      }
      case 0xE2: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmMM, rmMM, V2DWORDT); return MAKE_INSN2(psrad, psrad, reg, modrm);
          case mmF3: throw BadInstruction();
          case mm66: getModRegRM(rmXMM, rmXMM, V4DWORDT); return MAKE_INSN2(psrad, psrad, reg, modrm);
          case mmF2: throw BadInstruction();
        }
      }
      case 0xE3: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmMM, rmMM, V4WORDT); return MAKE_INSN2(pavgw, pavgw, reg, modrm);
          case mmF3: throw BadInstruction();
          case mm66: getModRegRM(rmXMM, rmXMM, V8WORDT); return MAKE_INSN2(pavgw, pavgw, reg, modrm);
          case mmF2: throw BadInstruction();
        }
      }
      case 0xE4: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmMM, rmMM, V4WORDT); return MAKE_INSN2(pmulhuw, pmulhuw, reg, modrm);
          case mmF3: throw BadInstruction();
          case mm66: getModRegRM(rmXMM, rmXMM, V8WORDT); return MAKE_INSN2(pmulhuw, pmulhuw, reg, modrm);
          case mmF2: throw BadInstruction();
        }
      }
      case 0xE5: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmMM, rmMM, V4WORDT); return MAKE_INSN2(pmulhw, pmulhw, reg, modrm);
          case mmF3: throw BadInstruction();
          case mm66: getModRegRM(rmXMM, rmXMM, V8WORDT); return MAKE_INSN2(pmulhw, pmulhw, reg, modrm);
          case mmF2: throw BadInstruction();
        }
      }
      case 0xE6: {
        switch (mmPrefix()) {
          case mmNone: throw BadInstruction();
          case mmF3: getModRegRM(rmXMM, rmXMM, V2QWORDT, V2DOUBLET); return MAKE_INSN2(cvtdq2pd, cvtdq2pd, reg, modrm);
          case mm66: getModRegRM(rmXMM, rmXMM, V2DOUBLET, V2QWORDT); return MAKE_INSN2(cvttpd2dq, cvttpd2dq, reg, modrm);
          case mmF2: getModRegRM(rmXMM, rmXMM, V2DOUBLET, V2QWORDT); return MAKE_INSN2(cvtpd2dq, cvtpd2dq, reg, modrm);
        }
      }
      case 0xE7: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmMM, rmMM, QWORDT); requireMemory(); return MAKE_INSN2(movntq, movntq, modrm, reg);
          case mmF3: throw BadInstruction();
          case mm66: getModRegRM(rmXMM, rmXMM, DQWORDT); requireMemory(); return MAKE_INSN2(movntdq, movntdq, modrm, reg);
          case mmF2: throw BadInstruction();
        }
      }
      case 0xE8: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmMM, rmMM, V8BYTET); return MAKE_INSN2(psubsb, psubsb, reg, modrm);
          case mmF3: throw BadInstruction();
          case mm66: getModRegRM(rmXMM, rmXMM, V16BYTET); return MAKE_INSN2(psubsb, psubsb, reg, modrm);
          case mmF2: throw BadInstruction();
        }
      }
      case 0xE9: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmMM, rmMM, V4WORDT); return MAKE_INSN2(psubsw, psubsw, reg, modrm);
          case mmF3: throw BadInstruction();
          case mm66: getModRegRM(rmXMM, rmXMM, V8WORDT); return MAKE_INSN2(psubsw, psubsw, reg, modrm);
          case mmF2: throw BadInstruction();
        }
      }
      case 0xEA: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmMM, rmMM, V4WORDT); return MAKE_INSN2(pminsw, pminsw, reg, modrm);
          case mmF3: throw BadInstruction();
          case mm66: getModRegRM(rmXMM, rmXMM, V8WORDT); return MAKE_INSN2(pminsw, pminsw, reg, modrm);
          case mmF2: throw BadInstruction();
        }
      }
      case 0xEB: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmMM, rmMM, QWORDT); return MAKE_INSN2(por, por, reg, modrm);
          case mmF3: throw BadInstruction();
          case mm66: getModRegRM(rmXMM, rmXMM, DQWORDT); return MAKE_INSN2(por, por, reg, modrm);
          case mmF2: throw BadInstruction();
        }
      }
      case 0xEC: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmMM, rmMM, V8BYTET); return MAKE_INSN2(paddsb, paddsb, reg, modrm);
          case mmF3: throw BadInstruction();
          case mm66: getModRegRM(rmXMM, rmXMM, V16BYTET); return MAKE_INSN2(paddsb, paddsb, reg, modrm);
          case mmF2: throw BadInstruction();
        }
      }
      case 0xED: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmMM, rmMM, V4WORDT); return MAKE_INSN2(paddsw, paddsw, reg, modrm);
          case mmF3: throw BadInstruction();
          case mm66: getModRegRM(rmXMM, rmXMM, V8WORDT); return MAKE_INSN2(paddsw, paddsw, reg, modrm);
          case mmF2: throw BadInstruction();
        }
      }
      case 0xEE: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmMM, rmMM, V4WORDT); return MAKE_INSN2(pmaxsw, pmaxsw, reg, modrm);
          case mmF3: throw BadInstruction();
          case mm66: getModRegRM(rmXMM, rmXMM, V8WORDT); return MAKE_INSN2(pmaxsw, pmaxsw, reg, modrm);
          case mmF2: throw BadInstruction();
        }
      }
      case 0xEF: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmMM, rmMM, QWORDT); return MAKE_INSN2(pxor, pxor, reg, modrm);
          case mmF3: throw BadInstruction();
          case mm66: getModRegRM(rmXMM, rmXMM, DQWORDT); return MAKE_INSN2(pxor, pxor, reg, modrm);
          case mmF2: throw BadInstruction();
        }
      }
      case 0xF0: {
        switch (mmPrefix()) {
          case mmNone: throw BadInstruction();
          case mmF3: throw BadInstruction();
          case mm66: throw BadInstruction();
          case mmF2: getModRegRM(rmXMM, rmXMM, DQWORDT); requireMemory(); return MAKE_INSN2(lddqu, lddqu, reg, modrm);
        }
      }
      case 0xF1: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmMM, rmMM, V4WORDT); return MAKE_INSN2(psllw, psllw, reg, modrm);
          case mmF3: throw BadInstruction();
          case mm66: getModRegRM(rmXMM, rmXMM, V8WORDT); return MAKE_INSN2(psllw, psllw, reg, modrm);
          case mmF2: throw BadInstruction();
        }
      }
      case 0xF2: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmMM, rmMM, V2DWORDT); return MAKE_INSN2(pslld, pslld, reg, modrm);
          case mmF3: throw BadInstruction();
          case mm66: getModRegRM(rmXMM, rmXMM, V4DWORDT); return MAKE_INSN2(pslld, pslld, reg, modrm);
          case mmF2: throw BadInstruction();
        }
      }
      case 0xF3: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmMM, rmMM, QWORDT); return MAKE_INSN2(psllq, psllq, reg, modrm);
          case mmF3: throw BadInstruction();
          case mm66: getModRegRM(rmXMM, rmXMM, V2QWORDT); return MAKE_INSN2(psllq, psllq, reg, modrm);
          case mmF2: throw BadInstruction();
        }
      }
      case 0xF4: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmMM, rmMM, V2DWORDT, QWORDT); return MAKE_INSN2(pmuludq, pmuludq, reg, modrm);
          case mmF3: throw BadInstruction();
          case mm66: getModRegRM(rmXMM, rmXMM, V4DWORDT, V2QWORDT); return MAKE_INSN2(pmuludq, pmuludq, reg, modrm);
          case mmF2: throw BadInstruction();
        }
      }
      case 0xF5: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmMM, rmMM, V4WORDT, V2DWORDT); return MAKE_INSN2(pmaddwd, pmaddwd, reg, modrm);
          case mmF3: throw BadInstruction();
          case mm66: getModRegRM(rmXMM, rmXMM, V8WORDT, V4DWORDT); return MAKE_INSN2(pmaddwd, pmaddwd, reg, modrm);
          case mmF2: throw BadInstruction();
        }
      }
      case 0xF6: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmMM, rmMM, V8BYTET, V4WORDT); return MAKE_INSN2(psadbw, psadbw, reg, modrm);
          case mmF3: throw BadInstruction();
          case mm66: getModRegRM(rmXMM, rmXMM, V16BYTET, V8WORDT); return MAKE_INSN2(psadbw, psadbw, reg, modrm);
          case mmF2: throw BadInstruction();
        }
      }
      case 0xF7: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmMM, rmMM, QWORDT); if (modeField == 3) return MAKE_INSN2(movntq, movntq, reg, modrm); else throw BadInstruction();
          case mmF3: throw BadInstruction();
          case mm66: getModRegRM(rmXMM, rmXMM, DQWORDT); if (modeField == 3) return MAKE_INSN2(movntdq, movntdq, reg, modrm); else throw BadInstruction();
          case mmF2: throw BadInstruction();
        }
      }
      case 0xF8: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmMM, rmMM, V8BYTET); return MAKE_INSN2(psubb, psubb, reg, modrm);
          case mmF3: throw BadInstruction();
          case mm66: getModRegRM(rmXMM, rmXMM, V16BYTET); return MAKE_INSN2(psubb, psubb, reg, modrm);
          case mmF2: throw BadInstruction();
        }
      }
      case 0xF9: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmMM, rmMM, V4WORDT); return MAKE_INSN2(psubw, psubw, reg, modrm);
          case mmF3: throw BadInstruction();
          case mm66: getModRegRM(rmXMM, rmXMM, V8WORDT); return MAKE_INSN2(psubw, psubw, reg, modrm);
          case mmF2: throw BadInstruction();
        }
      }
      case 0xFA: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmMM, rmMM, V2DWORDT); return MAKE_INSN2(psubd, psubd, reg, modrm);
          case mmF3: throw BadInstruction();
          case mm66: getModRegRM(rmXMM, rmXMM, V4DWORDT); return MAKE_INSN2(psubd, psubd, reg, modrm);
          case mmF2: throw BadInstruction();
        }
      }
      case 0xFB: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmMM, rmMM, QWORDT); return MAKE_INSN2(psubq, psubq, reg, modrm);
          case mmF3: throw BadInstruction();
          case mm66: getModRegRM(rmXMM, rmXMM, V2QWORDT); return MAKE_INSN2(psubq, psubq, reg, modrm);
          case mmF2: throw BadInstruction();
        }
      }
      case 0xFC: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmMM, rmMM, V8BYTET); return MAKE_INSN2(paddb, paddb, reg, modrm);
          case mmF3: throw BadInstruction();
          case mm66: getModRegRM(rmXMM, rmXMM, V16BYTET); return MAKE_INSN2(paddb, paddb, reg, modrm);
          case mmF2: throw BadInstruction();
        }
      }
      case 0xFD: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmMM, rmMM, V4WORDT); return MAKE_INSN2(paddw, paddw, reg, modrm);
          case mmF3: throw BadInstruction();
          case mm66: getModRegRM(rmXMM, rmXMM, V8WORDT); return MAKE_INSN2(paddw, paddw, reg, modrm);
          case mmF2: throw BadInstruction();
        }
      }
      case 0xFE: {
        switch (mmPrefix()) {
          case mmNone: getModRegRM(rmMM, rmMM, V2DWORDT); return MAKE_INSN2(paddd, paddd, reg, modrm);
          case mmF3: throw BadInstruction();
          case mm66: getModRegRM(rmXMM, rmXMM, V4DWORDT); return MAKE_INSN2(paddd, paddd, reg, modrm);
          case mmF2: throw BadInstruction();
        }
      }
      case 0xFF: throw BadInstruction();
      default: ROSE_ASSERT (false);
    }
  }

// DQ (12/3/2008): Added initial support for SSSE3
SgAsmx86Instruction* SingleInstructionDisassembler::decodeOpcode0F38()
   {
 // Support for SSSE 3 (opcode 0F38)
    uint8_t opcode;

 // Get the third byte of the opcode (the first two were read by the caller (decodeOpcode0F())
    getByte(opcode);
    switch (opcode) {
      case 0x00: {
        switch (mmPrefix()) {
       // The pshufb name is used twice because one is for the generated enum name and other is for the instruction name.
       // Note that getModRegRM sets the states reg and modrm.
       // Also, standard prefixed used in the manual, "mm" refers to "mmx" registers and "xmm" refers to "sse" registers.
          case mmNone: getModRegRM(rmMM, rmMM, V2DWORDT); return MAKE_INSN2(pshufb, pshufb, reg, modrm);
          case mmF3: throw BadInstruction();
          case mm66: getModRegRM(rmXMM, rmXMM, V4DWORDT); return MAKE_INSN2(pshufb, pshufb, reg, modrm);
          case mmF2: throw BadInstruction();
        }
      }

      default: throw BadInstruction();
    }
  }


  SgAsmx86Instruction* SingleInstructionDisassembler::decodeGroup1(SgAsmExpression* imm) {
    switch (regField) {
      case 0: return MAKE_INSN2(add, add, modrm, imm);
      case 1: return MAKE_INSN2(or , or , modrm, imm);
      case 2: return MAKE_INSN2(adc, adc, modrm, imm);
      case 3: return MAKE_INSN2(sbb, sbb, modrm, imm);
      case 4: return MAKE_INSN2(and, and, modrm, imm);
      case 5: return MAKE_INSN2(sub, sub, modrm, imm);
      case 6: return MAKE_INSN2(xor, xor, modrm, imm);
      case 7: return MAKE_INSN2(cmp, cmp, modrm, imm);
      default: ROSE_ASSERT (false);
    }
  }

  SgAsmx86Instruction* SingleInstructionDisassembler::decodeGroup1a() {
    if (regField != 0) throw BadInstruction();
    return MAKE_INSN1(pop, pop, modrm);
  }

  SgAsmx86Instruction* SingleInstructionDisassembler::decodeGroup2(SgAsmExpression* count) {
    switch (regField) {
      case 0: return MAKE_INSN2(rol, rol, modrm, count);
      case 1: return MAKE_INSN2(ror, ror, modrm, count);
      case 2: return MAKE_INSN2(rcl, rcl, modrm, count);
      case 3: return MAKE_INSN2(rcr, rcr, modrm, count);
      case 4: return MAKE_INSN2(shl, shl, modrm, count);
      case 5: return MAKE_INSN2(shr, shr, modrm, count);
      case 6: return MAKE_INSN2(shl, shl, modrm, count);
      case 7: return MAKE_INSN2(sar, sar, modrm, count);
      default: ROSE_ASSERT (false);
    }
  }

  SgAsmx86Instruction* SingleInstructionDisassembler::decodeGroup3(SgAsmExpression* immMaybe) {
    switch (regField) {
      case 0:
      case 1: ROSE_ASSERT (immMaybe); return MAKE_INSN2(test, test, modrm, immMaybe);
      case 2: return MAKE_INSN1(not, not, modrm);
      case 3: return MAKE_INSN1(neg, neg, modrm);
      case 4: return MAKE_INSN1(mul, mul, modrm);
      case 5: return MAKE_INSN1(imul, imul, modrm);
      case 6: return MAKE_INSN1(div, div, modrm);
      case 7: return MAKE_INSN1(idiv, idiv, modrm);
      default: ROSE_ASSERT (false);
    }
  }

  SgAsmx86Instruction* SingleInstructionDisassembler::decodeGroup4() {
    switch (regField) {
      case 0: return MAKE_INSN1(inc, inc, modrm);
      case 1: return MAKE_INSN1(dec, dec, modrm);
      default: throw BadInstruction();
    }
  }

  SgAsmx86Instruction* SingleInstructionDisassembler::decodeGroup5() {
    switch (regField) {
      case 0: return MAKE_INSN1(inc, inc, modrm);
      case 1: return MAKE_INSN1(dec, dec, modrm);
      case 2: return MAKE_INSN1(call, call, modrm);
      case 3: return MAKE_INSN1(farcall, farCall, modrm);
      case 4: isUnconditionalJump = true; return MAKE_INSN1(jmp, jmp, modrm);
      case 5: isUnconditionalJump = true; return MAKE_INSN1(farjmp, farJmp, modrm);
      case 6: return MAKE_INSN1(push, push, modrm);
      case 7: throw BadInstruction();
      default: ROSE_ASSERT (false);
    }
  }

  SgAsmx86Instruction* SingleInstructionDisassembler::decodeGroup6() {
    switch (regField) {
      case 0: return MAKE_INSN1(sldt, sldt, modrm); // FIXME adjust register size
      case 1: return MAKE_INSN1(str, str, modrm); // FIXME adjust register size
      case 2: return MAKE_INSN1(lldt, lldt, modrm);
      case 3: return MAKE_INSN1(ltr, ltr, modrm);
      case 4: return MAKE_INSN1(verr, verr, modrm);
      case 5: return MAKE_INSN1(verw, verw, modrm);
      case 6: throw BadInstruction();
      case 7: throw BadInstruction();
      default: ROSE_ASSERT (false);
    }
  }

  SgAsmx86Instruction* SingleInstructionDisassembler::decodeGroup7() {
    getModRegRM(rmReturnNull, rmReturnNull, NULL);
    switch (regField) {
      case 0: {
        if (modeField == 3) {
          switch (rmField) {
            case 1: return MAKE_INSN0(vmcall, vmcall);
            case 2: return MAKE_INSN0(vmlaunch, vmlaunch);
            case 3: return MAKE_INSN0(vmresume, vmresume);
            case 4: return MAKE_INSN0(vmxoff, vmxoff);
            default: throw BadInstruction();
          }
        } else {
          fillInModRM(rmReturnNull, BYTET /* pseudo-descriptor */ );
          return MAKE_INSN1(sgdt, sgdt, modrm);
        }
      }
      case 1: {
        if (modeField == 3) {
          switch (rmField) {
            case 0: return MAKE_INSN0(monitor, monitor);
            case 1: return MAKE_INSN0(mwait, mwait);
            default: throw BadInstruction();
          }
        } else {
          fillInModRM(rmReturnNull, BYTET /* pseudo-descriptor */ );
          return MAKE_INSN1(sidt, sidt, modrm);
        }
      }
      case 2: {
        if (modeField ==3) {
          switch (rmField) {
            case 0: return MAKE_INSN0(xgetbv, xgetbv);
            case 1: return MAKE_INSN0(xsetbv, xsetbv);
            default: throw BadInstruction();
          }
        } else {
          fillInModRM(rmReturnNull, BYTET /* pseudo-descriptor */ );
          return MAKE_INSN1(lgdt, lgdt, modrm);
        }
      }
      case 3: {
        if (modeField == 3) {
          switch (rmField) {
            case 0: return MAKE_INSN0(vmrun, vmrun);
            case 1: return MAKE_INSN0(vmmcall, vmmcall);
            case 2: return MAKE_INSN0(vmload, vmload);
            case 3: return MAKE_INSN0(vmsave, vmsave);
            case 4: return MAKE_INSN0(stgi, stgi);
            case 5: return MAKE_INSN0(clgi, clgi);
            case 6: return MAKE_INSN0(skinit, skinit);
            case 7: return MAKE_INSN0(invlpga, invlpga);
            default: ROSE_ASSERT (false);
          }
        } else {
          fillInModRM(rmReturnNull, BYTET /* pseudo-descriptor */ );
          return MAKE_INSN1(lidt, lidt, modrm);
        }
      }
      case 4: {
        fillInModRM(effectiveOperandMode(), WORDT);
        return MAKE_INSN1(smsw, smsw, modrm);
      }
      case 5: throw BadInstruction();
      case 6: {
        fillInModRM(rmWord, WORDT);
        return MAKE_INSN1(lmsw, lmsw, modrm);
      }
      case 7: {
        if (modeField == 3) {
          switch (rmField) {
            case 0: return MAKE_INSN0(swapgs, swapgs);
            case 1: return MAKE_INSN0(rdtscp, rdtscp);
            default: throw BadInstruction();
          }
        } else {
          fillInModRM(rmReturnNull, BYTET);
          return MAKE_INSN1(invlpg, invlpg, modrm);
        }
      }
      default: ROSE_ASSERT (false);
    }
  }

  SgAsmx86Instruction* SingleInstructionDisassembler::decodeGroup8(SgAsmExpression* imm) {
    switch (regField) {
      case 0: throw BadInstruction();
      case 1: throw BadInstruction();
      case 2: throw BadInstruction();
      case 3: throw BadInstruction();
      case 4: return MAKE_INSN2(bt, bt, modrm, imm);
      case 5: return MAKE_INSN2(bts, bts, modrm, imm);
      case 6: return MAKE_INSN2(btr, btr, modrm, imm);
      case 7: return MAKE_INSN2(btc, btc, modrm, imm);
      default: ROSE_ASSERT (false);
    }
  }

  SgAsmx86Instruction* SingleInstructionDisassembler::decodeGroup11(SgAsmExpression* imm) {
    switch (regField) {
      case 0: return MAKE_INSN2(mov, mov, modrm, imm);
      default: throw BadInstruction();
    }
  }

  SgAsmx86Instruction* SingleInstructionDisassembler::decodeGroup15() {
    getModRegRM(rmReturnNull, rmReturnNull, NULL);
    switch (regField) {
      case 0: requireMemory(); fillInModRM(rmReturnNull, BYTET); return MAKE_INSN1(fxsave, fxsave, modrm);
      case 1: requireMemory(); fillInModRM(rmReturnNull, BYTET); return MAKE_INSN1(fxrstor, fxrstor, modrm);
      case 2: requireMemory(); fillInModRM(rmReturnNull, DWORDT); return MAKE_INSN1(ldmxcsr, ldmxcsr, modrm);
      case 3: requireMemory(); fillInModRM(rmReturnNull, DWORDT); return MAKE_INSN1(stmxcsr, stmxcsr, modrm);
      case 4: requireMemory(); fillInModRM(rmReturnNull, BYTET); return MAKE_INSN1(xsave, xsave, modrm);
      case 5: if (modeField == 3) return MAKE_INSN0(lfence, lfence); else return MAKE_INSN1(xrstor, xrstor, modrm);
      case 6: if (modeField == 3) return MAKE_INSN0(mfence, mfence); else throw BadInstruction();
      case 7: if (modeField == 3) return MAKE_INSN0(sfence, sfence); else return MAKE_INSN1(clflush, clflush, modrm);
      default: ROSE_ASSERT (false);
    }
  }

  SgAsmx86Instruction* SingleInstructionDisassembler::decodeGroup16() {
    requireMemory();
    switch (regField) {
      case 0: return MAKE_INSN1(prefetchnta, prefetchnta, modrm);
      case 1: return MAKE_INSN1(prefetcht0, prefetcht0, modrm);
      case 2: return MAKE_INSN1(prefetcht1, prefetcht1, modrm);
      case 3: return MAKE_INSN1(prefetcht2, prefetcht2, modrm);
      default: return MAKE_INSN1(prefetch, prefetch, modrm);
    }
  }

  SgAsmx86Instruction* SingleInstructionDisassembler::decodeGroupP() {
    getModRegRM(rmReturnNull, rmLegacyByte, BYTET);
    requireMemory();
    switch (regField) {
      case 0: return MAKE_INSN1(prefetch, prefetch, modrm);
      case 1: return MAKE_INSN1(prefetchw, prefetchw, modrm);
      case 3: return MAKE_INSN1(prefetchw, prefetchw, modrm);
      default: return MAKE_INSN1(prefetch, prefetch, modrm);
    }
  }

  static bool is_push_ebp(SgAsmStatement* insnx, bool reg64) {
    SgAsmx86Instruction* insn = isSgAsmx86Instruction(insnx);
    if (!insn) return false;
    if (insn->get_kind() != x86_push) return false;
    const SgAsmExpressionPtrList& operands = insn->get_operandList()->get_operands();
    if (operands.size() != 1) return false;
    SgAsmx86RegisterReferenceExpression* rr0 = isSgAsmx86RegisterReferenceExpression(operands[0]);
    if (!rr0) return false;
    if (rr0->get_register_class() != x86_regclass_gpr ||
        rr0->get_register_number() != x86_gpr_bp ||
        (rr0->get_position_in_register() != x86_regpos_all &&
         rr0->get_position_in_register() !=
           (reg64 ? x86_regpos_qword : x86_regpos_dword)))
      return false;
    return true;
  }

  static bool is_mov_ebp_esp(SgAsmStatement* insnx, bool reg64) {
    SgAsmx86Instruction* insn = isSgAsmx86Instruction(insnx);
    if (!insn) return false;
    if (insn->get_kind() != x86_mov) return false;
    const SgAsmExpressionPtrList& operands = insn->get_operandList()->get_operands();
    if (operands.size() != 2) return false;
    SgAsmx86RegisterReferenceExpression* rr0 = isSgAsmx86RegisterReferenceExpression(operands[0]);
    if (!rr0) return false;
    if (rr0->get_register_class() != x86_regclass_gpr ||
        rr0->get_register_number() != x86_gpr_bp ||
        (rr0->get_position_in_register() != x86_regpos_all &&
         rr0->get_position_in_register() !=
           (reg64 ? x86_regpos_qword : x86_regpos_dword)))
      return false;
    SgAsmx86RegisterReferenceExpression* rr1 = isSgAsmx86RegisterReferenceExpression(operands[1]);
    if (!rr1) return false;
    if (rr1->get_register_class() != x86_regclass_gpr ||
        rr1->get_register_number() != x86_gpr_sp ||
        (rr1->get_position_in_register() != x86_regpos_all &&
         rr1->get_position_in_register() !=
           (reg64 ? x86_regpos_qword : x86_regpos_dword)))
      return false;
    return true;
  }

  // Signatures are from http://events.ccc.de/congress/2005/fahrplan/attachments/631-rjohnsonDisassemblerInternalsII.ppt
  bool doesBBStartFunction(SgAsmBlock* bb, bool use64bit) {
    const SgAsmStatementPtrList& contents = bb->get_statementList();
    size_t i = 0;
    //    while (i < contents.size() && isSgAsmx86Instruction(contents[i]) && isSgAsmx86Instruction(contents[i])->get_kind() == x86_nop) ++i;
    //if (!use64bit && i < contents.size() && is_mov_edi_edi(contents[i])) ++i;
    for (unsigned int k=i;(k+1)< (contents.size() );++k) {
      if (isSgAsmx86Instruction(contents[k]) && isSgAsmx86Instruction(contents[k+1]))
	if ((is_push_ebp(contents[k], use64bit) && is_mov_ebp_esp(contents[k + 1], use64bit))) {
	  return true;
      }
    }
    
    return false;
  }

}
