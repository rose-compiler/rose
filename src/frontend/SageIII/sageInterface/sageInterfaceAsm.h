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

 //! Traverses AST backwards up the tree along the "parent" edges to the SgAsmFunction.
     SgAsmFunction* getAsmFunction ( SgAsmInstruction* asmInstruction );

 //! Traverses AST backwards up the tree along the "parent" edges to the SgAsmBlock.
     SgAsmBlock* getAsmBlock ( SgAsmInstruction* asmInstruction );

 //! Traverses AST backwards up the tree along the "parent" edges to the SgAsmInterpretation.
     SgAsmInterpretation* getAsmInterpretation ( SgAsmNode* asmNode );

     std::string generateUniqueName ( size_t value, std::map<size_t,int> & usedOffsets, size_t & counter );

     size_t get_value ( SgAsmValueExpression* asmValueExpression );
     std::string get_valueString( SgAsmValueExpression* asmValueExpression );

     bool isMovInstruction ( SgAsmInstruction* asmInstruction );

     bool isInstructionKind ( SgAsmInstruction* asmInstruction, Rose::BinaryAnalysis::X86InstructionKind instuctionKind );

     typedef bool (*EquivalenceTestFunctionType) (SgNode* x, SgNode* y);

  // Definition of object equivalence for purposes of the AST matching using for instruction recognition.
     bool equivalenceTest(SgNode* x, SgNode* y);

  // These functions are used internally within the find function.
     std::vector<SgNode*> flattenAST ( SgNode* node );
     std::vector<SgNode*> matchAST ( SgNode* node, std::vector<SgNode*> & listOfNodes, EquivalenceTestFunctionType equivalenceTest );

  // General function to find matching target AST in larger AST.
  // SgNode* find ( SgNode* astNode, SgNode* target, EquivalenceTestFunctionType equivalenceTest );
     std::vector<SgNode*> find ( SgNode* astNode, SgNode* target, EquivalenceTestFunctionType equivalenceTest );

  // DQ (4/28/2010): Added support for interface to detect NOP's in all their glory.
  //! Test an instruction for if it has no side-effect to the state (is so then it is a NOP). This is a more general test than if it is equivelent to the NOP memonic instruction.
  // bool isNOP ( const SgAsmInstruction* asmInstruction );
     bool isNOP ( SgAsmInstruction* asmInstruction );
  //! Test a sequence of instructions for it they (as a set) have no side-effects to the state (is so then it is a NOP sequence).
     bool isNOP ( const std::vector<SgAsmInstruction*> & asmInstructionList );
  //! find sequences of NOP instructions in a SgAsmBlock
     std::vector<std::vector<SgAsmInstruction*> > find_NOP_sequences ( SgAsmBlock* asmBlock );

  // DQ (5/1/2010): We need a whole set of interface functions similar to this.
 //! Support for insertion of instruction relative to a target instruction.
     void insertInstruction(SgAsmInstruction* targetInstruction, SgAsmInstruction* newInstruction, bool insertBefore);

 //! Insert a instruction before a target instruction
     void insertInstructionBefore(SgAsmInstruction* targetInstruction, SgAsmInstruction* newInstruction);

 //! Remove a instruction
     void removeInstruction(SgAsmStatement* instruction);

   }

// endif for ROSE_SAGE_INTERFACE_ASM
#endif
