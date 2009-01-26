#ifndef ROSE_DISASSEMBLERS_H
#define ROSE_DISASSEMBLERS_H

#include <stdint.h>
#include <vector>
#include <map>
#include <set>
#include "rose.h"

// DQ (8/21/2008): Removed references to old code from before the use of the new IR nodes.
// #include "ExecGeneric.h"

struct OverflowOfInstructionVector {};
struct BadInstruction {};

namespace X86Disassembler {

  struct Parameters {
    uint64_t ip;
    X86InstructionSize insnSize;
    Parameters(uint64_t ip = 0, X86InstructionSize insnSize = x86_insnsize_32): ip(ip), insnSize(insnSize) {}
  };

  SgAsmx86Instruction* disassemble(const Parameters& p, const uint8_t* const insn, const uint64_t insnSize, size_t positionInVector, std::set<uint64_t>* knownSuccessorsReturn = 0);
  void disassembleFile(SgAsmFile* f);
  bool doesBBStartFunction(SgAsmBlock* bb, bool use64bit);

  inline SgAsmType* sizeToType(X86InstructionSize s) {
    switch (s) {
      case x86_insnsize_none: return NULL;
      case x86_insnsize_16: return SgAsmTypeWord::createType();
      case x86_insnsize_32: return SgAsmTypeDoubleWord::createType();
      case x86_insnsize_64: return SgAsmTypeQuadWord::createType();
      default: abort();;
    }
  }

  inline X86PositionInRegister sizeToPos(X86InstructionSize s) {
    switch (s) {
      case x86_insnsize_none: return x86_regpos_all;
      case x86_insnsize_16: return x86_regpos_word;
      case x86_insnsize_32: return x86_regpos_dword;
      case x86_insnsize_64: return x86_regpos_qword;
      default: abort();;
    }
  }

}

namespace ArmDisassembler {

  struct Parameters {
    uint32_t ip;
    bool decodeUnconditionalInstructions;
    Parameters(uint32_t ip = 0, bool decodeUnconditionalInstructions = true): ip(ip), decodeUnconditionalInstructions(decodeUnconditionalInstructions) {}
  };

  SgAsmArmRegisterReferenceExpression* makeRegister(uint8_t reg);
  SgAsmArmInstruction* disassemble(const Parameters& p, const uint8_t* const insn, const uint64_t insnSize, size_t positionInVector, std::set<uint64_t>* knownSuccessorsReturn = 0);

// DQ (10/13/2008): Is this used?
// void disassembleFile(SgAsmFile* f);

}

// DQ (10/11/2008): Initial start at support for PowerPC (for BGL, Cray XT series, etc.)
namespace PowerpcDisassembler
   {
     struct Parameters
        {
          uint64_t ip;
          bool decodeUnconditionalInstructions;
          Parameters(uint64_t ip = 0): ip(ip) {}
        };

  // SgAsmPowerpcRegisterReferenceExpression* makeRegister(uint8_t reg);
     SgAsmPowerpcRegisterReferenceExpression* makeRegister(PowerpcRegisterClass reg_class, int reg_number, PowerpcConditionRegisterAccessGranularity reg_grainularity);

     SgAsmPowerpcInstruction* disassemble(const Parameters& p, const uint8_t* const insn, const uint64_t insnSize, size_t positionInVector, std::set<uint64_t>* knownSuccessorsReturn = 0);

  // This is not used for ARM, so I don't think we need it!
  // void disassembleFile(SgAsmFile* f);

  // DQ (10/14/2008): Provide a default implementation for now!
     bool doesBBStartFunction(SgAsmBlock* bb, bool use64bit);
   }

namespace DisassemblerCommon {
    /* Map of basic block starting addresses. The key is the RVA of the first instruction in the basic block; the value is the
     * set of all addresses of instructions known to branch to this basic block (i.e., set of all known callers). */
    typedef std::map<rose_addr_t, std::set<rose_addr_t> > BasicBlockStarts;

    /* Map of function starting addresses. The key is the RVA of the first instruction in the function; the value consists of
     * a bit flag indicating why we think this is the beginning of a function, and a name (if known) of the function. */
    struct FunctionStart {
        FunctionStart()
            : reason(SgAsmFunctionDeclaration::FUNC_NONE)
            {}
        FunctionStart(SgAsmFunctionDeclaration::FunctionReason reason, std::string name)
            : reason(reason), name(name)
            {}
        unsigned reason;                        /* SgAsmFunctionDeclaration::FunctionReason bit flags */
        std::string name;
    };
    typedef std::map<rose_addr_t, FunctionStart> FunctionStarts;

    struct AsmFileWithData {
        SgAsmInterpretation* interp;
        mutable size_t instructionsDisassembled;

        AsmFileWithData(SgAsmInterpretation* interp): interp(interp), instructionsDisassembled(0)
            {}
        SgAsmGenericSection* getSectionOfAddress(uint64_t addr) const;
        bool inCodeSegment(uint64_t addr) const;
        SgAsmInstruction* disassembleOneAtAddress(uint64_t addr, std::set<uint64_t>& knownSuccessors) const;

        void disassembleRecursively(uint64_t addr, std::map<uint64_t, SgAsmInstruction*>& insns,
                                    BasicBlockStarts&) const;
        void disassembleRecursively(std::vector<uint64_t>& worklist, std::map<uint64_t, SgAsmInstruction*>& insns,
                                    BasicBlockStarts&) const;
    };
    
    void detectFunctionStarts(SgAsmInterpretation *interp, std::map<uint64_t, SgAsmInstruction*> &insns,
                              BasicBlockStarts&, FunctionStarts&);
}

namespace Disassembler
   {
  // DQ (8/26/2008): Added initialization for default mode of disassembler
     extern bool aggressive_mode;
     extern bool heuristicFunctionDetection;

     void disassembleFile(SgAsmFile* f);
     void disassembleInterpretation(SgAsmInterpretation* interp);
   }

#endif // ROSE_DISASSEMBLERS_H
