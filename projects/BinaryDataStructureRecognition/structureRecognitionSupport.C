#include "rose.h"

#include "astStructRecognition.h"

// These function in this file are expected to become a part of the 
// Binary Analysis and Transformation interfaces in the nes ROSE API.

using namespace std;

// DQ (4/26/2010): These have been moved to the src/frontend/SageIII/sageInterface/sageBuilderAsm.h file
#if 0
void
addComment(SgAsmStatement* stmt, const string & input_string )
   {
  // This function allows for multiple strings to be added to a comment.

     ROSE_ASSERT(stmt != NULL);
     string final_string = stmt->get_comment();

     if (final_string.empty() == false)
          final_string += "\n   ";

     final_string = final_string + input_string;
     stmt->set_comment(final_string);
   }


SgAsmElfSection*
getSection ( SgProject* project, const string & name )
   {
     struct Visitor: public AstSimpleProcessing
        {
          SgAsmElfSection* readOnlyDataSection;
          string name;

          Visitor( const string & n) : readOnlyDataSection(NULL), name(n) {}

          virtual void visit(SgNode* astNode)
             {
               SgAsmElfSection* asmElfSection = isSgAsmElfSection(astNode);
               if (asmElfSection != NULL && asmElfSection->get_name()->get_string() == name)
                  {
                 // Note that it will be a problem if we traverse a binary with 
                 // DLL's and can have more then one ".rodata" section. So test.
                    ROSE_ASSERT(readOnlyDataSection == NULL);

                    readOnlyDataSection = asmElfSection;
                  }
             }
        };

      Visitor v(name);
      v.traverse(project, preorder);

      return v.readOnlyDataSection;
   }


//! Traverses AST backwards up the tree along the "parent" edges to the SgAsmFunctionDeclaration.
SgAsmFunctionDeclaration*
getAsmFunctionDeclaration ( SgAsmInstruction* asmInstruction )
   {
  // Every instruction should be in an associated function.
  // Note: this might also make a good test for the AST.

     ROSE_ASSERT(asmInstruction != NULL);

     SgNode* n = asmInstruction;
     ROSE_ASSERT(n != NULL);
     SgAsmFunctionDeclaration* asmFunctionDeclaration = isSgAsmFunctionDeclaration(n);
     while (asmFunctionDeclaration == NULL)
        {
          ROSE_ASSERT(n != NULL);
          n = n->get_parent();
          asmFunctionDeclaration = isSgAsmFunctionDeclaration(n);
        }

     ROSE_ASSERT(asmFunctionDeclaration != NULL);
     return asmFunctionDeclaration;
   }


std::string
generateUniqueName ( size_t value, std::map<size_t,int> & usedOffsets, size_t & counter )
   {
     if (usedOffsets.find(value) == usedOffsets.end())
        {
       // Add this entry so that it will not be reused.
          usedOffsets[value] = counter++;
        }

#if 1
  // This names the variables using integer suffixes
     return StringUtility::numberToString(usedOffsets[value]);
#else
  // This names the variables using offset as a suffixes
     return StringUtility::numberToString(value);
#endif
   }


//! Traverses AST backwards up the tree along the "parent" edges to the SgAsmFunctionDeclaration.
SgAsmInterpretation*
getAsmInterpretation ( SgAsmNode* asmNode )
   {
  // Every instruction should be in an associated function.
  // Note: this might also make a good test for the AST.

     ROSE_ASSERT(asmNode != NULL);

     SgNode* n = asmNode;
     ROSE_ASSERT(n != NULL);
     SgAsmInterpretation* asmInterpretation = isSgAsmInterpretation(n);
     while (asmInterpretation == NULL)
        {
          ROSE_ASSERT(n != NULL);
          n = n->get_parent();
          asmInterpretation = isSgAsmInterpretation(n);
        }

     ROSE_ASSERT(asmInterpretation != NULL);
     return asmInterpretation;
   }


//! Traverses AST backwards up the tree along the "parent" edges to the SgAsmFunctionDeclaration.
SgAsmBlock*
getAsmBlock ( SgAsmInstruction* asmInstruction )
   {
  // Every instruction should be in an associated function.
  // Note: this might also make a good test for the AST.

     ROSE_ASSERT(asmInstruction != NULL);

     SgNode* n = asmInstruction;
     ROSE_ASSERT(n != NULL);
     SgAsmBlock* asmBlock = isSgAsmBlock(n);
     while (asmBlock == NULL)
        {
          ROSE_ASSERT(n != NULL);
          n = n->get_parent();
          asmBlock = isSgAsmBlock(n);
        }

     ROSE_ASSERT(asmBlock != NULL);
     return asmBlock;
   }


// Simple way to get the section that that a pointer points into (if any).
SgAsmElfSection*
getSection ( SgProject* project, size_t ptr )
   {
     struct Visitor: public AstSimpleProcessing
        {
          SgAsmElfSection* targetSection;
          size_t ptr;

          Visitor( size_t p) : targetSection(NULL), ptr(p) {}

          virtual void visit(SgNode* astNode)
             {
               SgAsmElfSection* asmElfSection = isSgAsmElfSection(astNode);
               if (asmElfSection != NULL)
                  {
                 // Is there a better way to avoid searching over this section which contains all sections?
                    if (asmElfSection->get_name()->get_string() != "ELF Load (segment 2)")
                       {
                         size_t section_address_base = asmElfSection->get_mapped_preferred_rva();
                         size_t section_size         = asmElfSection->get_size();
                         if (ptr >= section_address_base && ptr < section_address_base + section_size)
                            {
                           // printf ("Pointer is to data in section = %s value = 0x%"PRIx64"\n",asmElfSection->get_name()->get_string().c_str(),ptr);
                              targetSection = asmElfSection;
                            }
                           else
                            {
                           // printf ("Not in section = %s \n",asmElfSection->get_name()->get_string().c_str());
                            }
                       }
                      else
                       {
                      // printf ("asmElfSection->get_name()->get_string() = %s is %s \n",asmElfSection->get_name()->get_string().c_str(),asmElfSection->class_name().c_str());
                       }
                  }
             }
        };

      Visitor v(ptr);
      v.traverse(project, preorder);

   // ROSE_ASSERT(v.targetSection != NULL);

      return v.targetSection;
   }



size_t
get_value( SgAsmValueExpression* asmValueExpression )
   {
  // This is a kind of functionality that should make it's way into the general analysis interface for binary analysis.

     ROSE_ASSERT(asmValueExpression != NULL);
     size_t value = 0;

  // printf ("Processing asmValueExpression = %p = %s \n",asmValueExpression,asmValueExpression->class_name().c_str());

  // Identify what type of value expression this is...
     switch (asmValueExpression->variantT())
        {
          case V_SgAsmByteValueExpression:
             {
               SgAsmByteValueExpression* asmByteValueExpression = isSgAsmByteValueExpression(asmValueExpression);
               value = asmByteValueExpression->get_value();
            // printf ("structure field assigned (Byte) value = %zu \n",value);
               break;
             }

          case V_SgAsmDoubleWordValueExpression:
             {
               SgAsmDoubleWordValueExpression* asmDoubleWordValueExpression = isSgAsmDoubleWordValueExpression(asmValueExpression);
               value = asmDoubleWordValueExpression->get_value();
            // printf ("structure field assigned (Double Word) value = %p \n",(void*)value);
               break;
             }

          default:
               printf ("Error: unsupported structure field assigned value type = %s \n",asmValueExpression->class_name().c_str());
               ROSE_ASSERT(false);
        }

     return value;
   }


string
get_valueString( SgAsmValueExpression* asmValueExpression )
   {
  // This is a kind of functionality that should make it's way into the general analysis interface for binary analysis.
     ROSE_ASSERT(asmValueExpression != NULL);

     return StringUtility::numberToString((void*) get_value(asmValueExpression));
   }


bool
isMovInstruction ( SgAsmInstruction* asmInstruction )
   {
     return isInstructionKind(asmInstruction,x86_mov);
   }

bool
isInstructionKind ( SgAsmInstruction* asmInstruction, X86InstructionKind instuctionKind )
   {
  // Lower level infrastructure for detecting different kinds of instructions.

     bool foundInstructionKind = false;

     SgAsmx86Instruction *x86instruction = isSgAsmx86Instruction(asmInstruction);
     ROSE_ASSERT(x86instruction != NULL);

  // Detect a "mov" instruction as in "mov edx, 0x8048868"
     if (x86instruction->get_kind() == instuctionKind)
        {
          foundInstructionKind = true;
        }

     return foundInstructionKind;
   }

bool
isAsmGeneralPurposeRegisterReferenceExpression( SgAsmRegisterReferenceExpression* asmRegisterReferenceExpression, X86RegisterClass registerClass, X86GeneralPurposeRegister generalPurposeRegister )
   {
  // Found the top level SgAsmMemoryReferenceExpression in a SgInstruction
     ROSE_ASSERT(asmRegisterReferenceExpression != NULL);

     bool foundAsmRegisterReferenceExpression = false;
     SgAsmx86RegisterReferenceExpression* asmx86RegisterReferenceExpression = isSgAsmx86RegisterReferenceExpression(asmRegisterReferenceExpression);
     if (asmx86RegisterReferenceExpression != NULL)
        {
       // Detect the "ds" segment register (might want to use a switch statement).
       // bool isRegisterClass = (asmx86RegisterReferenceExpression->get_register_class() == registerClass);
          bool isGeneralPurposeRegister = (asmx86RegisterReferenceExpression->get_register_class() == registerClass);
       // printf ("isRegisterClass = %s \n",isRegisterClass ? "true" : "false");

       // We need to fix the IR to have the "ds" register not require a magic number!
       // bool isSpecificSegment = (isRegisterClass == true && asmx86RegisterReferenceExpression->get_register_number() == segmentRegister);
          bool isSpecificRegister = (isGeneralPurposeRegister == true && asmx86RegisterReferenceExpression->get_register_number() == generalPurposeRegister);
       // printf ("isRegisterClass = %s \n",isRegisterClass ? "true" : "false");

          if (isSpecificRegister == true)
             {
               foundAsmRegisterReferenceExpression = true;
             }
        }

     return foundAsmRegisterReferenceExpression;
   }

bool
isAsmSegmentRegisterReferenceExpression( SgAsmRegisterReferenceExpression* asmRegisterReferenceExpression, X86RegisterClass registerClass,  X86SegmentRegister segmentRegister )
   {
  // Found the top level SgAsmMemoryReferenceExpression in a SgInstruction
     bool foundAsmRegisterReferenceExpression = false;

     ROSE_ASSERT(asmRegisterReferenceExpression != NULL);
  // SgAsmRegisterReferenceExpression* asmRegisterReferenceExpression = isSgAsmRegisterReferenceExpression(asmInstruction);

     SgAsmx86RegisterReferenceExpression* asmx86RegisterReferenceExpression = isSgAsmx86RegisterReferenceExpression(asmRegisterReferenceExpression);
     if (asmx86RegisterReferenceExpression != NULL)
        {
       // Detect the "ds" segment register (might want to use a switch statement).
          bool isRegisterClass = (asmx86RegisterReferenceExpression->get_register_class() == registerClass);
       // printf ("isRegisterClass = %s \n",isRegisterClass ? "true" : "false");

       // We need to fix the IR to have the "ds" register not require a magic number!
          bool isSpecificSegment = (isRegisterClass == true && asmx86RegisterReferenceExpression->get_register_number() == segmentRegister);
       // printf ("isRegisterClass = %s \n",isRegisterClass ? "true" : "false");

          if (isSpecificSegment == true)
             {
               foundAsmRegisterReferenceExpression = true;
             }
        }

     return foundAsmRegisterReferenceExpression;
   }

#if 0
bool
isAsmMemoryReferenceExpression( SgAsmInstruction* asmInstruction, X86RegisterClass registerClass, X86SegmentRegister segmentRegister )
   {
  // Found the top level SgAsmMemoryReferenceExpression in a SgInstruction
     SgAsmExpression* addressOrBinaryExpression = isSgAsmExpression(childAttributes[SgAsmMemoryReferenceExpression_address].node);
     SgAsmExpression* segmentExpression         = isSgAsmExpression(childAttributes[SgAsmMemoryReferenceExpression_segment].node);

     SgAsmx86RegisterReferenceExpression* asmx86RegisterReferenceExpression = isSgAsmx86RegisterReferenceExpression(segmentExpression);
     if (asmx86RegisterReferenceExpression != NULL)
        {
       // Detect the "ds" segment register (might want to use a switch statement).
          bool isSegment = (asmx86RegisterReferenceExpression->get_register_class() == x86_regclass_segment);
       // printf ("isSegment = %s \n",isSegment ? "true" : "false");

       // We need to fix the IR to have the "ds" register not require a magic number!
          bool isSpecificSegment = (isSegment == true && asmx86RegisterReferenceExpression->get_register_number() == x86_segreg_ds);
       // printf ("isDataSegment = %s \n",isDataSegment ? "true" : "false");

          if (isSpecificSegment == true)
             {
            // printf ("Found a asmx86RegisterReferenceExpression for ds \n");
            // Save the register reference and pass it up the AST.
               localResult.segmentRegisterReference = asmx86RegisterReferenceExpression;
             }
        }
   }
#endif

#endif


// DQ (4/26/2010): These have been moved to the src/frontend/SageIII/sageInterface/sageBuilderAsm.C file
#if 0
// Build interface for SgAsmInstructions

#if 0
// SgAsmx86Instruction* SageBuilderAsm::buildInstruction(
//      rose_addr_t address=0, string mnemonic="", X86InstructionKind kind=x86_unknown_instruction, 
//      X86InstructionSize baseSize=x86_insnsize_none, X86InstructionSize operandSize=x86_insnsize_none, 
//      X86InstructionSize addressSize=x86_insnsize_none);

SgAsmx86Instruction* SageBuilderAsm::buildx86Instruction(
   rose_addr_t address            = 0, 
   string mnemonic           = "", 
   X86InstructionKind kind        = x86_unknown_instruction, 
   X86InstructionSize baseSize    = x86_insnsize_none, 
   X86InstructionSize operandSize = x86_insnsize_none, 
   X86InstructionSize addressSize = x86_insnsize_none)
   {
     return new SgAsmx86Instruction(address,mnemonic,kind,baseSize,operandSize,addressSize);
   }
#endif

SgAsmx86Instruction* SageBuilderAsm::buildx86Instruction( X86InstructionKind kind )
   {
  // These are the default values used for the construction of new instructions.
     rose_addr_t address            = 0;
     string mnemonic           = "";
     X86InstructionSize baseSize    = x86_insnsize_none;
     X86InstructionSize operandSize = x86_insnsize_none; 
     X86InstructionSize addressSize = x86_insnsize_none;

     SgAsmx86Instruction* instruction = new SgAsmx86Instruction(address,mnemonic,kind,baseSize,operandSize,addressSize);

  // This should not have been set yet.
     ROSE_ASSERT(instruction->get_operandList() == NULL);

  // All instructions are required to have a valid SgAsmOperandList pointer.
     instruction->set_operandList(new SgAsmOperandList ());

  // Set the parent in the SgAsmOperandList
     instruction->get_operandList()->set_parent(instruction);

     return instruction;
   }

SgAsmx86Instruction* SageBuilderAsm::buildx86Instruction( X86InstructionKind kind, SgAsmExpression* operand )
   {
     SgAsmx86Instruction* instruction = buildx86Instruction(kind);
     appendOperand(instruction,operand);

     return instruction;
   }

SgAsmx86Instruction* SageBuilderAsm::buildx86Instruction( X86InstructionKind kind, SgAsmExpression* lhs, SgAsmExpression* rhs  )
   {
     SgAsmx86Instruction* instruction = buildx86Instruction(kind);
     appendOperand(instruction,lhs);
     appendOperand(instruction,rhs);

     return instruction;
   }

#if 0
SgAsmx86RegisterReferenceExpression* SageBuilderAsm::buildAsmx86RegisterReferenceExpression( X86RegisterClass register_class, int register_number, X86PositionInRegister position_in_register)
   {
     SgAsmx86RegisterReferenceExpression* exp = new SgAsmx86RegisterReferenceExpression(register_class,register_number,position_in_register);

  // Put tests for correctness of SgAsmx86RegisterReferenceExpression here.

     return exp;
   }
#endif

SgAsmx86RegisterReferenceExpression*
SageBuilderAsm::buildAsmx86RegisterReferenceExpression( X86RegisterClass register_class, SgAsmx86RegisterReferenceExpression::regnames8l_enum register_number, X86PositionInRegister position_in_register)
   {
     SgAsmx86RegisterReferenceExpression* exp = new SgAsmx86RegisterReferenceExpression(register_class,(int)register_number,position_in_register);
     return exp;
   }

SgAsmx86RegisterReferenceExpression*
SageBuilderAsm::buildAsmx86RegisterReferenceExpression( X86RegisterClass register_class, SgAsmx86RegisterReferenceExpression::regnames8h_enum register_number, X86PositionInRegister position_in_register)
   {
     SgAsmx86RegisterReferenceExpression* exp = new SgAsmx86RegisterReferenceExpression(register_class,(int)register_number,position_in_register);
     return exp;
   }

SgAsmx86RegisterReferenceExpression*
SageBuilderAsm::buildAsmx86RegisterReferenceExpression( X86RegisterClass register_class, SgAsmx86RegisterReferenceExpression::regnames16_enum register_number, X86PositionInRegister position_in_register)
   {
     SgAsmx86RegisterReferenceExpression* exp = new SgAsmx86RegisterReferenceExpression(register_class,(int)register_number,position_in_register);
     return exp;
   }

SgAsmx86RegisterReferenceExpression*
SageBuilderAsm::buildAsmx86RegisterReferenceExpression( X86RegisterClass register_class, SgAsmx86RegisterReferenceExpression::regnames32_enum register_number, X86PositionInRegister position_in_register)
   {
     SgAsmx86RegisterReferenceExpression* exp = new SgAsmx86RegisterReferenceExpression(register_class,(int)register_number,position_in_register);
     return exp;
   }

SgAsmx86RegisterReferenceExpression*
SageBuilderAsm::buildAsmx86RegisterReferenceExpression( X86RegisterClass register_class, SgAsmx86RegisterReferenceExpression::regnames64_enum register_number, X86PositionInRegister position_in_register)
   {
     SgAsmx86RegisterReferenceExpression* exp = new SgAsmx86RegisterReferenceExpression(register_class,(int)register_number,position_in_register);
     return exp;
   }

SgAsmx86RegisterReferenceExpression*
SageBuilderAsm::buildAsmx86RegisterReferenceExpression( X86RegisterClass register_class, SgAsmx86RegisterReferenceExpression::segregnames_enum register_number, X86PositionInRegister position_in_register)
   {
     SgAsmx86RegisterReferenceExpression* exp = new SgAsmx86RegisterReferenceExpression(register_class,(int)register_number,position_in_register);
     return exp;
   }

SgAsmDoubleWordValueExpression*
SageBuilderAsm::buildAsmDWordValue(uint32_t val)
   {
  // This build functions are just wrappers (for now) to the "make" functions of will there are only a few available.
     return SageBuilderAsm::makeDWordValue(val);
   }

SgAsmByteValueExpression*
SageBuilderAsm::buildAsmByteValue(uint8_t val)
   {
  // This build functions are just wrappers (for now) to the "make" functions of will there are only a few available.
     return SageBuilderAsm::makeByteValue(val);
   }

SgAsmWordValueExpression*
SageBuilderAsm::buildAsmWordValue(uint16_t val)
   {
  // This build functions are just wrappers (for now) to the "make" functions of will there are only a few available.
     return SageBuilderAsm::makeWordValue(val);
   }

SgAsmBinaryAdd*
SageBuilderAsm::buildAsmAddExpression(SgAsmExpression* lhs, SgAsmExpression* rhs)
   {
  // This build functions are just wrappers (for now) to the "make" functions of will there are only a few available.
     return SageBuilderAsm::makeAdd(lhs,rhs);
   }


SgAsmMemoryReferenceExpression*
SageBuilderAsm::buildAsmMemoryReferenceExpression(SgAsmExpression* addr, SgAsmExpression* segment, SgAsmType* t)
   {
     return SageBuilderAsm::makeMemoryReference(addr,segment,t);
   }


SgAsmTypeByte*
SageBuilderAsm::buildAsmTypeByte()
   {
     return new SgAsmTypeByte();
   }

SgAsmTypeWord*
SageBuilderAsm::buildAsmTypeWord()
   {
     return new SgAsmTypeWord();
   }

SgAsmTypeDoubleWord*
SageBuilderAsm::buildAsmTypeDoubleWord()
   {
     return new SgAsmTypeDoubleWord();
   }

SgAsmTypeQuadWord*
SageBuilderAsm::buildAsmTypeQuadWord()
   {
     return new SgAsmTypeQuadWord();
   }

SgAsmTypeSingleFloat*
SageBuilderAsm::buildAsmTypeSingleFloat()
   {
     return new SgAsmTypeSingleFloat();
   }

SgAsmTypeDoubleFloat*
SageBuilderAsm::buildAsmTypeDoubleFloat()
   {
     return new SgAsmTypeDoubleFloat();
   }

SgAsmType128bitFloat*
SageBuilderAsm::buildAsmType128bitFloat()
   {
     return new SgAsmType128bitFloat();
   }

SgAsmType80bitFloat*
SageBuilderAsm::buildAsmType80bitFloat()
   {
     return new SgAsmType80bitFloat();
   }

SgAsmTypeDoubleQuadWord*
SageBuilderAsm::buildAsmTypeDoubleQuadWord()
   {
     return new SgAsmTypeDoubleQuadWord();
   }

SgAsmTypeVector*
SageBuilderAsm::buildAsmTypeVector()
   {
     return new SgAsmTypeVector();
   }
#endif



















// DQ (4/26/2010): These have been moved to the src/frontend/SageIII/sageInterface/sageInterfaceAsm.C file
#if 0

// Definition of object equivalence for purposes of the AST matching using for instruction recognition.
bool equivalenceTest(SgNode* x, SgNode* y)
   {
  // This function tests deeper level of equivalence than just if the variant codes in ROSE match.
  // One example of why we need to do this is that all instructions use the same IR node but only
  // internally do they store a code that identifies the kind of instruction.  In the future
  // each instruction may have an overloaded operator==() function which specifies the default 
  // concept of equivalence.

     ROSE_ASSERT(x != NULL);
     ROSE_ASSERT(y != NULL);
     ROSE_ASSERT(x->variantT() == y->variantT());

     bool result = false;

     switch (x->variantT())
        {
          case V_SgAsmx86Instruction:
             {
            // Look at the instruction kind only.
               SgAsmx86Instruction* x_instruction = isSgAsmx86Instruction(x);
               SgAsmx86Instruction* y_instruction = isSgAsmx86Instruction(y);

               bool isSameKind = (x_instruction->get_kind() == y_instruction->get_kind());

               if (isSameKind == true)
                  {
                    result = true;
                  }

               break;
             }

          case V_SgAsmx86RegisterReferenceExpression:
             {
            // Look at the register kind and the register number only.
               SgAsmx86RegisterReferenceExpression* x_exp = isSgAsmx86RegisterReferenceExpression(x);
               SgAsmx86RegisterReferenceExpression* y_exp = isSgAsmx86RegisterReferenceExpression(y);

               bool isSameRegisterClass      = (x_exp->get_register_class() == y_exp->get_register_class());

            // Offset the register numbers in the template by one since they use the new form of enum not yet put into the disassembler.
               bool isSameRegister           = (x_exp->get_register_number() == y_exp->get_register_number());

            // bool isSamePositionInRegister = (x_exp->get_position_in_register() == y_exp->get_position_in_register());

               if (isSameRegisterClass == true && isSameRegister == true )
                  {
                    result = true;
                  }
#if 0
            // Debugging
               printf ("Comparing x_exp = %s to y_exp = %s (%s) \n",unparseX86Expression(x_exp,false).c_str(),unparseX86Expression(y_exp,false).c_str(),result ? "passed" : "failed");
               if (result == false)
                  {
                    printf ("   isSameRegisterClass = %s isSameRegister = %s isSamePositionInRegister = %s \n",isSameRegisterClass ? "true" : "false",isSameRegister ? "true" : "false",isSamePositionInRegister ? "true" : "false");
                  }
#endif
               break;
             }

          case V_SgAsmDoubleWordValueExpression:
             {
            // Look at the instruction kind only.
            // SgAsmDoubleWordValueExpression* x_value = isSgAsmDoubleWordValueExpression(x);
            // SgAsmDoubleWordValueExpression* y_value = isSgAsmDoubleWordValueExpression(y);

            // Treat all values as matching for now (later we will want a concept of regular expression matches on values).
               bool isSameKind = true;

               if (isSameKind == true)
                  {
                    result = true;
                  }

               break;
             }

          case V_SgAsmOperandList:
             {
            // We could compare the number of operands.
             }

          default:
             {
            // The default is to assume they are equivalent for initial testing.
               result = true;
             }
        }

     return result;
   }



vector<SgNode*>
flattenAST ( SgNode* node )
   {
  // This flattens the AST to a vector
     class FlatAST : public SgSimpleProcessing
        {
          public:
               vector<SgNode*> listOfNodes;
               void visit (SgNode* node)
                  {
                    listOfNodes.push_back(node);
                  }
        };

     FlatAST flatAST;

  // Collect the nodes in preorder so that we can test the first nodes quickly (from the head of the list).
     flatAST.traverse(node,preorder);

     return flatAST.listOfNodes;
   }

vector<SgNode*>
matchAST ( SgNode* node, vector<SgNode*> & listOfNodes, EquivalenceTestFunctionType equivalenceTest )
   {
  // This matches the AST against the target. Note that this matching makes some assumptions 
  // about how the ordering of nodes in the AST.  Some information is list when the AST is 
  // flattended.  This might be good enough for now and can likely be fixed by recording 
  // explicitly the leaf nodes.  In general this is not too much of a prblem because the
  // types of IR nodes make explicit requireemtns about the number of children that they have.
  // This assumption might make this simple matching more appropriate for the matching of
  // SgAsmInstruction subtrees and less appropriate for source code.

     class MatchAST : public SgSimpleProcessing
        {
          public:
               vector<SgNode*> listOfNodes;
               size_t index;
               vector<SgNode*> listOfMatchedNodes;
               SgNode* savedMatch;
               SgAsmInstruction* currentInstruction;

            // Function pointer
               EquivalenceTestFunctionType equivalenceTestFunction;

               MatchAST( vector<SgNode*> & l, EquivalenceTestFunctionType f ) : listOfNodes(l), index(0), savedMatch(NULL), equivalenceTestFunction(f) {}

               void visit (SgNode* node)
                  {
                    ROSE_ASSERT(index < listOfNodes.size());

                 // This is the first level of equivalence testing (quick but not deep enough)
#if 0
                    printf ("Testing match: listOfNodes[%zu] = %s node = %s \n",index,listOfNodes[index]->class_name().c_str(),node->class_name().c_str());
#endif
                    if (listOfNodes[index]->variantT() == node->variantT())
                       {
                         SgAsmInstruction* instruction = isSgAsmInstruction(node);
                         if (instruction != NULL)
                            {
                              currentInstruction = instruction;
#if 0
                              printf ("Found a possible matching IR position for instruction at address %p \n",(void*)currentInstruction->get_address());
#endif
                            }
#if 0
                         printf ("Found a possible matching IR position index = %zu node = %p = %s \n",index,node,node->class_name().c_str());
#endif
                      // Now we do a deeper and more custom test of equivalence (looking for matching data members, as required).
#if 0
                         printf ("Index = %zu of %zu: comparing target to currentInstruction = %s at address = %p \n",index,listOfNodes.size(),unparseInstruction(currentInstruction).c_str(),(void*)currentInstruction->get_address());
#endif
                         bool isAMatch = equivalenceTestFunction(listOfNodes[index],node);

                         if (isAMatch == true)
                            {
#if 0
                              printf ("Found a more exact match at index = %zu of %zu at node = %p = %s \n",index,listOfNodes.size(),node,node->class_name().c_str());
#endif
                              if (index == 0)
                                 {
                                   savedMatch = node;
                                 }

                              index++;

                           // Reset the index when we have reached the end of the match.
                              if (index == listOfNodes.size())
                                 {
                                   printf ("Saving an exact match with the target for instruction at address %p \n",(void*)currentInstruction->get_address());
                                   ROSE_ASSERT(savedMatch != NULL);
                                   listOfMatchedNodes.push_back(savedMatch);
                                   index = 0;
#if 0
                                   printf ("Exiting to debug exact match \n");
                                   ROSE_ASSERT(false);
#endif
                                 }
                            }
                           else
                            {
                           // Reset the index to restart the comparison.
                              index = 0;
                            }
                       }
                      else
                       {
                      // Reset the index to restart the comparison.
                         index = 0;
                       }
                  }
        };

     printf ("Input target listOfNodes.size() = %zu \n",listOfNodes.size());

     MatchAST matchAST(listOfNodes,equivalenceTest);

  // Collect the nodes in preorder so that we can test the first nodes quickly (from the head of the list).
     matchAST.traverse(node,preorder);

     return matchAST.listOfMatchedNodes;
   }

// General function to find matching target AST in larger AST.
// SgNode*
vector<SgNode*>
find ( SgNode* astNode, SgNode* target, EquivalenceTestFunctionType equivalenceTest )
   {
  // Preconditions
     ROSE_ASSERT(astNode != NULL);
     ROSE_ASSERT(target  != NULL);
     ROSE_ASSERT(equivalenceTest != NULL);

     vector<SgNode*> flattenedTargetList = flattenAST(target);

     printf ("flattenedTargetList.size() = %zu \n",flattenedTargetList.size());
     SgAsmInstruction* instruction = isSgAsmInstruction(flattenedTargetList[0]);
     if (instruction != NULL)
          printf ("   instruction = %s \n",unparseInstructionWithAddress(instruction).c_str());
     for (size_t i=0; i < flattenedTargetList.size(); i++)
        {
          ROSE_ASSERT(flattenedTargetList[i] != NULL);
          printf ("flattenedTargetList[%zu] = %s \n",i,flattenedTargetList[i]->class_name().c_str());
        }

     vector<SgNode*> matchList = matchAST (astNode,flattenedTargetList,equivalenceTest);

     printf ("Matching subtrees (matchList.size() = %zu) \n",matchList.size());
     for (size_t i=0; i < matchList.size(); i++)
        {
          ROSE_ASSERT(matchList[i] != NULL);
          SgAsmInstruction* instruction = isSgAsmInstruction(matchList[i]);
          ROSE_ASSERT(instruction != NULL);
          printf ("   instruction = %s \n",unparseInstructionWithAddress(instruction).c_str());
        }

     printf ("Leaving find() \n");
     return matchList;
   }
#endif


