#include "rose.h"

#include "sageInterfaceAsm.h"
#include "detectNopSequencesTraversal.h"

using namespace std;
using namespace SageInterface;

void CountTraversal::visit ( SgNode* n )
   {
     SgAsmInstruction* asmInstruction = isSgAsmInstruction(n);
     if (asmInstruction != NULL)
        {
       // Use the new interface support for this (this detects all multi-byte nop instructions). 
          if (SageInterface::isNOP(asmInstruction) == true)
             {
               if (previousInstructionWasNop == true)
                  {
                 // Increment the length of the identified NOP sequence
                    count++;
                  }
                 else
                  {
                    count = 1;
                 // Record the starting address of the NOP sequence
                    nopSequenceStart = asmInstruction;
                  }

               previousInstructionWasNop = true;
             }
            else
             {
               if (count > 0)
                   {
                  // Report the sequence when we have detected the end of the sequence.
                     SgAsmFunctionDeclaration* functionDeclaration = getAsmFunctionDeclaration(asmInstruction);
                     printf ("Reporting NOP sequence of length %3d at address %zu in function %s (reason for this being a function = %u = %s) \n",
                          count,nopSequenceStart->get_address(),functionDeclaration->get_name().c_str(),
                          functionDeclaration->get_reason(),functionDeclaration->get_functionReasonString().c_str());

                     nopSequences.push_back(pair<SgAsmInstruction*,int>(nopSequenceStart,count));

                     SgAsmBlock* block = isSgAsmBlock(nopSequenceStart->get_parent());
                     ROSE_ASSERT(block != NULL);
                     SgAsmStatementPtrList & l = block->get_statementList();

                  // Now iterate over the nop instructions in the sequence and report the lenght of each (can be multi-byte nop instructions).
                     SgAsmStatementPtrList::iterator i = find(l.begin(),l.end(),nopSequenceStart);
                     ROSE_ASSERT(i != l.end());
                     int counter = 0;
                     while ( (*i != asmInstruction) && (i != l.end()) )
                        {
                          printf ("--- NOP #%2d is length = %2d \n",counter++,(int)isSgAsmInstruction(*i)->get_raw_bytes().size());
                          i++;
                        }
                   }

               count = 0;
               previousInstructionWasNop = false;
             }
        }
   }
