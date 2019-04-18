#include "sage3basic.h"

#include "sageInterfaceAsm.h"
#include "AsmUnparser_compat.h"

// These function in this file are expected to become a part of the 
// Binary Analysis and Transformation interfaces in the nes ROSE API.

using namespace std;
using namespace Rose;
using namespace Rose::BinaryAnalysis;

// DQ (4/26/2010): Added functions to support ASM specific details in the ROSE interface.
void
SageInterface::addComment(SgAsmStatement* stmt, const string & input_string )
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
SageInterface::getSection ( SgProject* project, const string & name )
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


//! Traverses AST backwards up the tree along the "parent" edges to the SgAsmFunction.
SgAsmFunction*
SageInterface::getAsmFunction ( SgAsmInstruction* asmInstruction )
   {
  // Every instruction should be in an associated function.
  // Note: this might also make a good test for the AST.

     ROSE_ASSERT(asmInstruction != NULL);

     SgNode* n = asmInstruction;
     ROSE_ASSERT(n != NULL);
     SgAsmFunction* asmFunctionDeclaration = isSgAsmFunction(n);
     while (asmFunctionDeclaration == NULL)
        {
          ROSE_ASSERT(n != NULL);
          n = n->get_parent();
          asmFunctionDeclaration = isSgAsmFunction(n);
        }

     ROSE_ASSERT(asmFunctionDeclaration != NULL);
     return asmFunctionDeclaration;
   }


std::string
SageInterface::generateUniqueName ( size_t value, std::map<size_t,int> & usedOffsets, size_t & counter )
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


//! Traverses AST backwards up the tree along the "parent" edges to the SgAsmInterpretation.
SgAsmInterpretation*
SageInterface::getAsmInterpretation ( SgAsmNode* asmNode )
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


//! Traverses AST backwards up the tree along the "parent" edges to the SgAsmBlock.
SgAsmBlock*
SageInterface::getAsmBlock ( SgAsmInstruction* asmInstruction )
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
SageInterface::getSection ( SgProject* project, size_t ptr )
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
SageInterface::get_value( SgAsmValueExpression* asmValueExpression )
   {
  // This is a kind of functionality that should make it's way into the general analysis interface for binary analysis.

     ROSE_ASSERT(asmValueExpression != NULL);
     size_t value = 0;

  // printf ("Processing asmValueExpression = %p = %s \n",asmValueExpression,asmValueExpression->class_name().c_str());

  // Identify what type of value expression this is...
     switch (asmValueExpression->variantT())
        {
          case V_SgAsmIntegerValueExpression:
             {
               SgAsmIntegerValueExpression* asmIntegerValueExpression = isSgAsmIntegerValueExpression(asmValueExpression);
               value = asmIntegerValueExpression->get_value();
            // printf ("structure field assigned (Byte) value = %" PRIuPTR " \n",value);
               break;
             }

          default:
               printf ("Error: unsupported structure field assigned value type = %s \n",asmValueExpression->class_name().c_str());
               ROSE_ASSERT(false);
        }

     return value;
   }


string
SageInterface::get_valueString( SgAsmValueExpression* asmValueExpression )
   {
  // This is a kind of functionality that should make it's way into the general analysis interface for binary analysis.
     ROSE_ASSERT(asmValueExpression != NULL);

     return StringUtility::numberToString((void*) get_value(asmValueExpression));
   }


bool
SageInterface::isMovInstruction ( SgAsmInstruction* asmInstruction )
   {
     return isInstructionKind(asmInstruction, x86_mov);
   }

bool
SageInterface::isInstructionKind ( SgAsmInstruction* asmInstruction, X86InstructionKind instuctionKind )
   {
  // Lower level infrastructure for detecting different kinds of instructions.

     bool foundInstructionKind = false;

     SgAsmX86Instruction *x86instruction = isSgAsmX86Instruction(asmInstruction);
     ROSE_ASSERT(x86instruction != NULL);

  // Detect a "mov" instruction as in "mov edx, 0x8048868"
     if (x86instruction->get_kind() == instuctionKind)
        {
          foundInstructionKind = true;
        }

     return foundInstructionKind;
   }

// Definition of object equivalence for purposes of the AST matching using for instruction recognition.
bool
SageInterface::equivalenceTest(SgNode* x, SgNode* y)
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
          case V_SgAsmX86Instruction:
             {
            // Look at the instruction kind only.
               SgAsmX86Instruction* x_instruction = isSgAsmX86Instruction(x);
               SgAsmX86Instruction* y_instruction = isSgAsmX86Instruction(y);

               bool isSameKind = (x_instruction->get_kind() == y_instruction->get_kind());

               if (isSameKind == true)
                  {
                    result = true;
                  }

               break;
             }

          case V_SgAsmRegisterReferenceExpression:
          case V_SgAsmDirectRegisterExpression:
             {
               SgAsmRegisterReferenceExpression* x_exp = isSgAsmRegisterReferenceExpression(x);
               SgAsmRegisterReferenceExpression* y_exp = isSgAsmRegisterReferenceExpression(y);
               result = x_exp->get_descriptor() == y_exp->get_descriptor();
               break;
             }

          case V_SgAsmIndirectRegisterExpression: {
              SgAsmIndirectRegisterExpression* x_exp = isSgAsmIndirectRegisterExpression(x);
              SgAsmIndirectRegisterExpression* y_exp = isSgAsmIndirectRegisterExpression(y);
              result = (x_exp->get_offset() == y_exp->get_offset() &&
                        x_exp->get_stride() == y_exp->get_stride() &&
                        x_exp->get_index() == y_exp->get_index() &&
                        x_exp->get_modulus() == y_exp->get_modulus());
              break;
          }

          case V_SgAsmIntegerValueExpression:
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
SageInterface::flattenAST ( SgNode* node )
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
SageInterface::matchAST ( SgNode* node, vector<SgNode*> & listOfNodes, EquivalenceTestFunctionType equivalenceTest )
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
                    printf ("Testing match: listOfNodes[%" PRIuPTR "] = %s node = %s \n",index,listOfNodes[index]->class_name().c_str(),node->class_name().c_str());
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
                         printf ("Found a possible matching IR position index = %" PRIuPTR " node = %p = %s \n",index,node,node->class_name().c_str());
#endif
                      // Now we do a deeper and more custom test of equivalence (looking for matching data members, as required).
#if 0
                         printf ("Index = %" PRIuPTR " of %" PRIuPTR ": comparing target to currentInstruction = %s at address = %p \n",index,listOfNodes.size(),unparseInstruction(currentInstruction).c_str(),(void*)currentInstruction->get_address());
#endif
                         bool isAMatch = equivalenceTestFunction(listOfNodes[index],node);

                         if (isAMatch == true)
                            {
#if 0
                              printf ("Found a more exact match at index = %" PRIuPTR " of %" PRIuPTR " at node = %p = %s \n",index,listOfNodes.size(),node,node->class_name().c_str());
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

     printf ("Input target listOfNodes.size() = %" PRIuPTR " \n",listOfNodes.size());

     MatchAST matchAST(listOfNodes,equivalenceTest);

  // Collect the nodes in preorder so that we can test the first nodes quickly (from the head of the list).
     matchAST.traverse(node,preorder);

     return matchAST.listOfMatchedNodes;
   }

// General function to find matching target AST in larger AST.
// SgNode*
vector<SgNode*>
SageInterface::find ( SgNode* astNode, SgNode* target, EquivalenceTestFunctionType equivalenceTest )
   {
  // Preconditions
     ROSE_ASSERT(astNode != NULL);
     ROSE_ASSERT(target  != NULL);
     ROSE_ASSERT(equivalenceTest != NULL);

     vector<SgNode*> flattenedTargetList = flattenAST(target);

     printf ("flattenedTargetList.size() = %" PRIuPTR " \n",flattenedTargetList.size());
     SgAsmInstruction* instruction = isSgAsmInstruction(flattenedTargetList[0]);
     if (instruction != NULL)
         printf ("   instruction = %s\n", instruction->toString().c_str());
     for (size_t i=0; i < flattenedTargetList.size(); i++)
        {
          ROSE_ASSERT(flattenedTargetList[i] != NULL);
          printf ("flattenedTargetList[%" PRIuPTR "] = %s \n",i,flattenedTargetList[i]->class_name().c_str());
        }

     vector<SgNode*> matchList = matchAST (astNode,flattenedTargetList,equivalenceTest);

     printf ("Matching subtrees (matchList.size() = %" PRIuPTR ") \n",matchList.size());
     for (size_t i=0; i < matchList.size(); i++)
        {
          ROSE_ASSERT(matchList[i] != NULL);
          SgAsmInstruction* instruction = isSgAsmInstruction(matchList[i]);
          ROSE_ASSERT(instruction != NULL);
          printf ("   instruction = %s\n", instruction->toString().c_str());
        }

     printf ("Leaving find() \n");
     return matchList;
   }


// DQ (4/28/2010): Added support for interface to detect NOP's.  This function is for a single instruction.
// Not only detects x86 NOP instructions, but any instruction whose only effect is to advance the instruction
// pointer to the fall-through address.  Works for any architecture. See SgAsmX86Instruction::hasEffect() for details.
bool
SageInterface::isNOP ( SgAsmInstruction* asmInstruction )
   {
       return !asmInstruction->hasEffect();
   }

// DQ (4/28/2010): Added support for interface to detect NOP's.  This function is for a list of instructions.
// Not only detects x86 NOP instructions, but any sequence of instructions whose only effect is to advance the instruction
// pointer to the fall-through address.  Works for any architecture. See SgAsmX86Instruction::hasEffect() for details.
bool
SageInterface::isNOP ( const std::vector<SgAsmInstruction*> & asmInstructionList )
   {
       if (asmInstructionList.empty())
           return true;
       return !asmInstructionList.front()->hasEffect(asmInstructionList);
   }

// DQ (4/28/2010): Added support for interface to detect NOP's.  This function detects NOP sequences in a SgAsmBlock.
// Not only detects x86 NOP instructions, but any subsequence of instructions whose only effect is to advance the instruction
// pointer to the fall-through address.  Works for any architecture. See SgAsmX86Instruction::findNoopSubsequences()
// for details.
std::vector<std::vector<SgAsmInstruction*> >
SageInterface::find_NOP_sequences (SgAsmBlock* asmBlock)
   {
       std::vector<std::vector<SgAsmInstruction*> > retval;
       std::vector<SgAsmInstruction*> insns = SageInterface::querySubTree<SgAsmInstruction>(asmBlock, V_SgAsmInstruction);
       if (insns.empty()) return retval;

       /* Find the subsequences (index,size pairs) */
       typedef std::vector<std::pair<size_t, size_t> > Subsequences;
       Subsequences sequences = insns.front()->findNoopSubsequences(insns);

       /* Build the return value */
       retval.reserve(sequences.size());
       for (Subsequences::const_iterator si=sequences.begin(); si!=sequences.end(); ++si) {
           retval.resize(retval.size()+1);
           std::vector<SgAsmInstruction*> &back = retval.back();
           for (size_t i=(*si).first; i<(*si).second; ++i)
               back.push_back(insns[i]);
       }
       
       return retval;
   }

void
SageInterface::insertInstruction(SgAsmInstruction* targetInstruction, SgAsmInstruction* newInstruction, bool insertBefore)
   {
     ROSE_ASSERT(targetInstruction && newInstruction);
     ROSE_ASSERT(targetInstruction != newInstruction); // should not share statement nodes!
     SgNode* parent = targetInstruction->get_parent();
     if (parent == NULL)
        {
          cerr << "Empty parent pointer for target instruction. May be caused by the wrong order of target and new instructions in insertInstruction(targetStmt, newStmt)" << endl;
          ROSE_ASSERT(parent);
        }

  // Set the parent
     newInstruction->set_parent(parent);

     SgAsmBlock* block = isSgAsmBlock(parent);
     ROSE_ASSERT(block != NULL);

     SgAsmStatementPtrList & l = block->get_statementList();
     SgAsmStatementPtrList::iterator i = find(l.begin(),l.end(),targetInstruction);
     ROSE_ASSERT(i != l.end());

     if (insertBefore == true)
        {
          l.insert(i,newInstruction);
        }
       else
        {
          l.insert(i,newInstruction);
        }
  }


 //! Insert a instruction before a target instruction
void
SageInterface::insertInstructionBefore(SgAsmInstruction* targetInstruction, SgAsmInstruction* newInstruction)
   {
     printf ("Function not implemented: SageInterface::insertInstructionBefore(SgAsmInstruction* targetInstruction, SgAsmInstruction* newInstruction) \n");
  // ROSE_ASSERT(false);

     insertInstruction (targetInstruction,newInstruction,true);
   }

 //! Remove a instruction
void
SageInterface::removeInstruction(SgAsmStatement* instruction)
   {
     printf ("Function not implemented: SageInterface::removeInstruction(SgAsmInstruction* instuction) \n");
  // ROSE_ASSERT(false);

     SgNode* parent = instruction->get_parent();
     ROSE_ASSERT(parent != NULL);

     SgAsmBlock* block = isSgAsmBlock(parent);
     ROSE_ASSERT(block != NULL);

     SgAsmStatementPtrList & l = block->get_statementList();
     SgAsmStatementPtrList::iterator i = find(l.begin(),l.end(),instruction);
     ROSE_ASSERT(i != l.end());

     l.erase(i);
   }

