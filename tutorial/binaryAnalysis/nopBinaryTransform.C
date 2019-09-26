// This tutorial example show how to introduce 
// transformations on the input binary exectutable.

#include "rose.h"

#include "sageInterfaceAsm.h"

#include "detectNopSequencesTraversal.h"

using namespace std;
using namespace Rose;
using namespace SageInterface;
using namespace SageBuilderAsm;


class NopReplacementTraversal : public SgSimpleProcessing
   {
     public:
       // Local Accumulator Attribute
          std::vector<std::pair<SgAsmInstruction*,int> > & nopSequences;
          std::vector<std::pair<SgAsmInstruction*,int> >::iterator listIterator;
          SgAsmStatementPtrList deleteList;

          NopReplacementTraversal(std::vector<std::pair<SgAsmInstruction*,int> > & X) : nopSequences(X)
             {
               listIterator = nopSequences.begin();
             }

          void visit ( SgNode* n );
   };


void NopReplacementTraversal::visit ( SgNode* n )
   {
     SgAsmInstruction* asmInstruction = isSgAsmInstruction(n);
     if (asmInstruction != NULL && asmInstruction == listIterator->first)
        {
       // This is the instruction in the AST that matches the start of one of the NOP sequences.
          SgAsmBlock* block = isSgAsmBlock(asmInstruction->get_parent());
          int numberOfOriginalNopInstructions = listIterator->second;

          printf ("numberOfOriginalNopInstructions = %d \n",numberOfOriginalNopInstructions);

       // SgAsmBlock* block = isSgAsmBlock(asmInstruction->get_parent());
          ROSE_ASSERT(block != NULL);
          SgAsmStatementPtrList & l = block->get_statementList();

       // Now iterate over the nop instructions in the sequence and report the lenght of each (can be multi-byte nop instructions).
          SgAsmStatementPtrList::iterator i = find(l.begin(),l.end(),asmInstruction);
          ROSE_ASSERT(i != l.end());
          int nop_sled_size = 0;
          for (int j = 0; j < numberOfOriginalNopInstructions; j++)
             {
               ROSE_ASSERT(i != l.end());
               printf ("--- NOP #%2d is length = %2d \n",j,(int)isSgAsmInstruction(*i)->get_raw_bytes().size());
               nop_sled_size += (int)isSgAsmInstruction(*i)->get_raw_bytes().size();
               i++;
             }

          printf ("nop_sled_size = %d \n",nop_sled_size);

       // From the size of the NOP sled, compute how many multi-byte NOPs we will want to use to cover the same space in the binary.
       // This code is specific to x86 supporting multi-byte NOPs in sized 1-9 bytes long.
          const int MAX_SIZE_MULTIBYTE_NOP = 9;
          int numberOfNopSizeN [MAX_SIZE_MULTIBYTE_NOP+1];

       // 0th element of numberOfNopSizeN is not used.
          numberOfNopSizeN[0] = 0;
          for (int i = MAX_SIZE_MULTIBYTE_NOP; i > 0; i--)
             {
            // int numberOfNopSize9 = nop_sled_size / 9;
               numberOfNopSizeN[i] = nop_sled_size / i;
               nop_sled_size -= numberOfNopSizeN[i] * i;
               if (numberOfNopSizeN[i] > 0)
                    printf ("numberOfNopSizeN[%d] = %d \n",i,numberOfNopSizeN[i]);
             }

       // Now rewrite the AST to use the number of multi-byte NOPS specified 
       // in the numberOfNopSizeN array for each size of multi-byte NOP.

          printf ("Rewrite the AST here! \n");

       // Ignore the 0th element of numberOfNopSizeN (since a 0 length NOP does not make sense).
          for (int i = 1; i <= MAX_SIZE_MULTIBYTE_NOP; i++)
             {
            // Build this many bytes of this size
               for (int j = 0; j < numberOfNopSizeN[i]; j++)
                  {
                 // We want to build a (binary AST node) SgAsmInstruction object instead of a (source code AST node) SgAsmStmt.
                 // SgAsmStmt* nopStatement = buildMultibyteNopStatement(i);
                 // SgAsmInstruction* multiByteNop = makeInstruction(x86_nop, "nop", modrm);
                    SgAsmInstruction* multiByteNopInstruction = buildX86MultibyteNopInstruction(i);

                 // Add to the front o the list of statements in the function body
                 // prependStatement(nopStatement,block);
                    insertInstructionBefore(/*target*/ asmInstruction,/*new instruction*/ multiByteNopInstruction);
                  }
             }

       // Now iterate over the nop instructions in the sequence and report the lenght of each (can be multi-byte nop instructions).
          i = find(l.begin(),l.end(),asmInstruction);
          ROSE_ASSERT(i != l.end());
          for (int j = 0; j < numberOfOriginalNopInstructions; j++)
             {
               ROSE_ASSERT(i != l.end());
            // printf ("Deleting original NOP instruction #%2d is length = %2d \n",j,(int)isSgAsmInstruction(*i)->get_raw_bytes().size());

            // Removing the original NOP instruction.
            // removeStatement(*i);
            // removeInstruction(*i);
               deleteList.push_back(*i);

               i++;
             }
#if 0
          for (SgAsmStatementPtrList::iterator k = deleteList.begin(); k != deleteList.end(); k++)
             {
               removeInstruction(*k);
             }
#endif
       // Increment the list iterator (for list of nop sequences)
          listIterator++;
        }
   }


int main( int argc, char * argv[] )
   {
  // Initialize and check compatibility. See Rose::initialize
     ROSE_INITIALIZE;

  // Generate the ROSE AST.
     SgProject* project = frontend(argc,argv);

  // AST consistency tests (optional for users, but this enforces more of our tests)
     AstTests::runAllTests(project);

  // Build the list of NOP sequences
     CountTraversal t1;
     t1.traverse(project,preorder);

  // Transform the AST for each entry in the list of NOP sequences
     NopReplacementTraversal t2(t1.nopSequences);
     t2.traverse(project,preorder);

  // Remove the instructions AFTER the AST traversal is finished.
     for (SgAsmStatementPtrList::iterator k = t2.deleteList.begin(); k != t2.deleteList.end(); k++)
        {
          removeInstruction(*k);
        }

  // regenerate the original executable.
     return backend(project);
   }
