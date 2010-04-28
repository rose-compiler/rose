#ifndef ROSE_SAGE_INTERFACE_ASM
#define ROSE_SAGE_INTERFACE_ASM

// These functions in this file are expected to become a part of the 
// Binary Analysis and Transformation interfaces in the new ROSE API.

namespace SageInterface
   {
  // DQ (4/26/2010): Added this file of functions to support binary analysis work.

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

  // These functions are used internally within the find function.
     std::vector<SgNode*> flattenAST ( SgNode* node );
     std::vector<SgNode*> matchAST ( SgNode* node, std::vector<SgNode*> & listOfNodes, EquivalenceTestFunctionType equivalenceTest );

  // General function to find matching target AST in larger AST.
  // SgNode* find ( SgNode* astNode, SgNode* target, EquivalenceTestFunctionType equivalenceTest );
     std::vector<SgNode*> find ( SgNode* astNode, SgNode* target, EquivalenceTestFunctionType equivalenceTest );
   }

// endif for ROSE_SAGE_INTERFACE_ASM
#endif
