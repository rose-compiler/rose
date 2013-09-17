
// #undef __STDC_FORMAT_MACROS
// #define __STDC_FORMAT_MACROS
// #include <inttypes.h>

// These functions in this file are expected to become a part of the 
// Binary Analysis and Transformation interfaces in the new ROSE API.

// DQ (4/26/2010): These have been moved to the src/frontend/SageIII/sageInterface/sageBuilderAsm.h file
#if 0
void addComment(SgAsmStatement* stmt, const std::string & input_string );

SgAsmElfSection* getSection ( SgProject* project, const std::string & name );

// Simple way to get the section that that a pointer points into (if any).
SgAsmElfSection* getSection ( SgProject* project, size_t ptr );

//! Traverses AST backwards up the tree along the "parent" edges to the SgAsmFunctionDeclaration.
SgAsmFunctionDeclaration* getAsmFunctionDeclaration ( SgAsmInstruction* asmInstruction );

//! Traverses AST backwards up the tree along the "parent" edges to the SgAsmBlock.
SgAsmBlock* getAsmBlock ( SgAsmInstruction* asmInstruction );

//! Traverses AST backwards up the tree along the "parent" edges to the SgAsmInterpretation.
SgAsmInterpretation* getAsmInterpretation ( SgAsmNode* asmNode );

std::string generateUniqueName ( size_t value, std::map<size_t,int> & usedOffsets, size_t & counter );

size_t get_value ( SgAsmValueExpression* asmValueExpression );
std::string get_valueString( SgAsmValueExpression* asmValueExpression );

bool isMovInstruction ( SgAsmInstruction* asmInstruction );

bool isInstructionKind ( SgAsmInstruction* asmInstruction, X86InstructionKind instuctionKind );

bool isAsmGeneralPurposeRegisterReferenceExpression ( SgAsmRegisterReferenceExpression* asmRegisterReferenceExpression, X86RegisterClass registerClass, X86GeneralPurposeRegister generalPurposeRegister );
bool isAsmSegmentRegisterReferenceExpression ( SgAsmRegisterReferenceExpression* asmRegisterReferenceExpression, X86RegisterClass registerClass, X86SegmentRegister segmentRegister );


// bool isAsmRegisterReferenceExpression ( SgAsmRegisterReferenceExpression* asmRegisterReferenceExpression, X86RegisterClass registerClass, X86GeneralPurposeRegister generalPurposeRegister, X86SegmentRegister segmentRegister );

typedef bool (*EquivalenceTestFunctionType) (SgNode* x, SgNode* y);

// Definition of object equivalence for purposes of the AST matching using for instruction recognition.
bool equivalenceTest(SgNode* x, SgNode* y);

// General function to find matching target AST in larger AST.
// SgNode* find ( SgNode* astNode, SgNode* target, EquivalenceTestFunctionType equivalenceTest );
std::vector<SgNode*>
find ( SgNode* astNode, SgNode* target, EquivalenceTestFunctionType equivalenceTest );
#endif


// DQ (4/26/2010): These have been moved to the src/frontend/SageIII/sageInterface/sageBuilderAsm.h file
#if 0
// Build interface for SgAsmInstructions

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

  // New functions not in the list of "make" functions
  // SgAsmx86RegisterReferenceExpression* buildAsmx86RegisterReferenceExpression( X86RegisterClass register_class = x86_regclass_unknown, int register_number = 0, X86PositionInRegister position_in_register = x86_regpos_unknown);
  // SgAsmx86RegisterReferenceExpression* buildAsmx86RegisterReferenceExpression( X86RegisterClass register_class, int register_number, X86PositionInRegister position_in_register = x86_regpos_unknown);
     SgAsmx86RegisterReferenceExpression* buildAsmx86RegisterReferenceExpression( X86RegisterClass register_class, SgAsmx86RegisterReferenceExpression::regnames8l_enum register_number, X86PositionInRegister position_in_register = x86_regpos_low_byte);
     SgAsmx86RegisterReferenceExpression* buildAsmx86RegisterReferenceExpression( X86RegisterClass register_class, SgAsmx86RegisterReferenceExpression::regnames8h_enum register_number, X86PositionInRegister position_in_register = x86_regpos_high_byte);
     SgAsmx86RegisterReferenceExpression* buildAsmx86RegisterReferenceExpression( X86RegisterClass register_class, SgAsmx86RegisterReferenceExpression::regnames16_enum register_number, X86PositionInRegister position_in_register = x86_regpos_word);
     SgAsmx86RegisterReferenceExpression* buildAsmx86RegisterReferenceExpression( X86RegisterClass register_class, SgAsmx86RegisterReferenceExpression::regnames32_enum register_number, X86PositionInRegister position_in_register = x86_regpos_dword);
     SgAsmx86RegisterReferenceExpression* buildAsmx86RegisterReferenceExpression( X86RegisterClass register_class, SgAsmx86RegisterReferenceExpression::regnames64_enum register_number, X86PositionInRegister position_in_register = x86_regpos_qword);

     SgAsmx86RegisterReferenceExpression* buildAsmx86RegisterReferenceExpression( X86RegisterClass register_class, SgAsmx86RegisterReferenceExpression::segregnames_enum register_number, X86PositionInRegister position_in_register = x86_regpos_all);

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

#if 0
  // Use the version that is already defined in the "sageBuilderAsm.h".

     template <typename Insn> // For correct subtype to be returned
     inline Insn* appendOperand(Insn* insn, SgAsmExpression* op)
        {
          SgAsmOperandList* operands = insn->get_operandList();
          ROSE_ASSERT(operands != NULL);
          operands->get_operands().push_back(op);
          op->set_parent(operands);
          return insn; // For chaining this operation
        }
#endif
   }
#endif
