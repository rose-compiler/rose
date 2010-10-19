#ifndef ROSE_SAGEBUILDERASM_H
#define ROSE_SAGEBUILDERASM_H

namespace SageBuilderAsm
   {
  // DQ (10/26/2009): I don't think this should be used (it is a copy of the constructor).
  // SgAsmx86Instruction* buildx86Instruction( rose_addr_t address = 0, std::string mnemonic = "", X86InstructionKind kind = x86_unknown_instruction, X86InstructionSize baseSize = x86_insnsize_none, X86InstructionSize operandSize = x86_insnsize_none, X86InstructionSize addressSize = x86_insnsize_none);

  // This is the simplest interface for the construction of an instruction.
     SgAsmx86Instruction* buildx86Instruction( X86InstructionKind kind );

  // Not clear which of these will be most useful (take a list of one or two operands).
     SgAsmx86Instruction* buildx86Instruction( X86InstructionKind kind, SgAsmExpression* operand );
     SgAsmx86Instruction* buildx86Instruction( X86InstructionKind kind, SgAsmExpression* lhs, SgAsmExpression* rhs );

  // Not clear which of these will be most useful (take an operand list).
     SgAsmx86Instruction* buildx86Instruction( X86InstructionKind kind, SgAsmOperandList* operands );

  // Build functions for SgAsmOperandList
     SgAsmOperandList* buildSgAsmOperandList();
     SgAsmOperandList* buildSgAsmOperandList( SgAsmExpression* operand );
     SgAsmOperandList* buildSgAsmOperandList( SgAsmExpression* lhs = NULL, SgAsmExpression* rhs = NULL );

template <class T> T* buildAsmUnaryExpression(SgAsmExpression* operand = NULL);

// SgAsmUnaryMinus * buildAsmUnaryMinus(SgAsmExpression* op = NULL);

#define BUILD_UNARY_PROTO(suffix) SgAsmUnary##suffix * buildAsmUnary##suffix(SgAsmExpression* op = NULL);

// There are more unary operators to add still
BUILD_UNARY_PROTO(Plus)

// BUILD_UNARY_PROTO(UnaryMinus)


template <class T> T* buildAsmBinaryExpression(SgAsmExpression* lhs = NULL, SgAsmExpression* rhs = NULL);

#define BUILD_BINARY_PROTO(suffix) SgAsmBinary##suffix * buildAsmBinary##suffix(SgAsmExpression* lhs = NULL, SgAsmExpression* rhs = NULL);

// There are more binary operators to add still
BUILD_BINARY_PROTO(Add)
BUILD_BINARY_PROTO(Divide)
BUILD_BINARY_PROTO(Mod)
BUILD_BINARY_PROTO(Multiply)
BUILD_BINARY_PROTO(Subtract)

  // These functions are copies of the "make" functions (wrappers for the "make" functions) to provide a consistant interface for the future API.
     SgAsmQuadWordValueExpression* buildAsmQWordValue(uint64_t val);
     SgAsmBinarySubtract* buildAsmSubtract(SgAsmExpression* lhs, SgAsmExpression* rhs);
     SgAsmBinaryAddPreupdate* buildAsmAddPreupdate(SgAsmExpression* lhs, SgAsmExpression* rhs);
     SgAsmBinarySubtractPreupdate* buildAsmSubtractPreupdate(SgAsmExpression* lhs, SgAsmExpression* rhs);
     SgAsmBinaryAddPostupdate* buildAsmAddPostupdate(SgAsmExpression* lhs, SgAsmExpression* rhs);
     SgAsmBinarySubtractPostupdate* buildAsmSubtractPostupdate(SgAsmExpression* lhs, SgAsmExpression* rhs);
     SgAsmBinaryMultiply* buildAsmMul(SgAsmExpression* lhs, SgAsmExpression* rhs);
     SgAsmBinaryLsl* buildAsmLsl(SgAsmExpression* lhs, SgAsmExpression* rhs);
     SgAsmBinaryLsr* buildAsmLsr(SgAsmExpression* lhs, SgAsmExpression* rhs);
     SgAsmBinaryAsr* buildAsmAsr(SgAsmExpression* lhs, SgAsmExpression* rhs);
     SgAsmBinaryRor* buildAsmRor(SgAsmExpression* lhs, SgAsmExpression* rhs);
     SgAsmUnaryRrx* buildAsmRrx(SgAsmExpression* lhs);
     SgAsmUnaryArmSpecialRegisterList* buildAsmArmSpecialRegisterList(SgAsmExpression* lhs);
     SgAsmExprListExp* buildAsmExprListExp();

  // Implemented versions of "make" functions wrapped as "build" functions.
     SgAsmDoubleWordValueExpression* buildAsmDWordValue(uint32_t val);
     SgAsmByteValueExpression* buildAsmByteValue(uint8_t val);
     SgAsmWordValueExpression* buildAsmWordValue(uint16_t val);

     SgAsmMemoryReferenceExpression* buildAsmMemoryReferenceExpression(SgAsmExpression* addr, SgAsmExpression* segment = NULL, SgAsmType* t = NULL);
     SgAsmBinaryAdd*                 buildAsmAddExpression(SgAsmExpression* lhs, SgAsmExpression* rhs);

     SgAsmTypeByte*           buildAsmTypeByte();
     SgAsmTypeWord*           buildAsmTypeWord();
     SgAsmTypeDoubleWord*     buildAsmTypeDoubleWord();
     SgAsmTypeQuadWord*       buildAsmTypeQuadWord();
     SgAsmTypeSingleFloat*    buildAsmTypeSingleFloat();
     SgAsmTypeDoubleFloat*    buildAsmTypeDoubleFloat();
     SgAsmType128bitFloat*    buildAsmType128bitFloat();
     SgAsmType80bitFloat*     buildAsmType80bitFloat();
     SgAsmTypeDoubleQuadWord* buildAsmTypeDoubleQuadWord();
     SgAsmTypeVector*         buildAsmTypeVector();

  // DQ (5/1/2010): Support for building multi-byte NOP instructions.
     SgAsmInstruction* buildMultibyteNopInstruction(int n);

#if 1
  // DQ (10/28/2009): These functions are not similar in name to the 
  // associated "build" functions in the sageBuilder.h.  They are however
  // needed elsewhere in ROSE (src/frontend/Disassemblers directory).

     SgAsmByteValueExpression* makeByteValue(uint8_t val);
     SgAsmWordValueExpression* makeWordValue(uint16_t val);
     SgAsmDoubleWordValueExpression* makeDWordValue(uint32_t val);
     SgAsmQuadWordValueExpression* makeQWordValue(uint64_t val);
     SgAsmMemoryReferenceExpression* makeMemoryReference(SgAsmExpression* addr, SgAsmExpression* segment = NULL, SgAsmType* t = NULL);
     SgAsmBinaryAdd* makeAdd(SgAsmExpression* lhs, SgAsmExpression* rhs);
     SgAsmBinarySubtract* makeSubtract(SgAsmExpression* lhs, SgAsmExpression* rhs);
     SgAsmBinaryAddPreupdate* makeAddPreupdate(SgAsmExpression* lhs, SgAsmExpression* rhs);
     SgAsmBinarySubtractPreupdate* makeSubtractPreupdate(SgAsmExpression* lhs, SgAsmExpression* rhs);
     SgAsmBinaryAddPostupdate* makeAddPostupdate(SgAsmExpression* lhs, SgAsmExpression* rhs);
     SgAsmBinarySubtractPostupdate* makeSubtractPostupdate(SgAsmExpression* lhs, SgAsmExpression* rhs);
     SgAsmBinaryMultiply* makeMul(SgAsmExpression* lhs, SgAsmExpression* rhs);
     SgAsmBinaryLsl* makeLsl(SgAsmExpression* lhs, SgAsmExpression* rhs);
     SgAsmBinaryLsr* makeLsr(SgAsmExpression* lhs, SgAsmExpression* rhs);
     SgAsmBinaryAsr* makeAsr(SgAsmExpression* lhs, SgAsmExpression* rhs);
     SgAsmBinaryRor* makeRor(SgAsmExpression* lhs, SgAsmExpression* rhs);
     SgAsmUnaryRrx* makeRrx(SgAsmExpression* lhs);
     SgAsmUnaryArmSpecialRegisterList* makeArmSpecialRegisterList(SgAsmExpression* lhs);
     SgAsmExprListExp* makeExprListExp();
#endif

    template <typename Insn> // For correct subtype to be returned
    inline Insn* appendOperand(Insn* insn, SgAsmExpression* op)
       {
         SgAsmOperandList* operands = insn->get_operandList();
         operands->get_operands().push_back(op);
         op->set_parent(operands);
         return insn; // For chaining this operation
       }

  // end of SageBuilderAsm namespace
   }
#endif // ROSE_SAGEBUILDERASM_H
