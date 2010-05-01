// This tutorial example show how to introduce 
// transformations on the input binary exectutable.

#include "rose.h"

#include "sageInterfaceAsm.h"

#include "detectNopSequencesTraversal.h"

using namespace std;
using namespace SageInterface;


class NopReplacementTraversal : public SgSimpleProcessing
   {
     public:
       // Local Accumulator Attribute
          std::vector<std::pair<SgAsmInstruction*,int> > & nopSequences;
          std::vector<std::pair<SgAsmInstruction*,int> >::iterator listIterator;

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

          int numberOfNopSizeN [10];
          for (int i = 9; i > 0; i--)
             {
            // int numberOfNopSize9 = nop_sled_size / 9;
               numberOfNopSizeN[i] = nop_sled_size / i;
               nop_sled_size -= numberOfNopSizeN[i] * i;
               printf ("numberOfNopSizeN[%d] = %d \n",i,numberOfNopSizeN[i]);
             }

       // Now rewrite the AST to use the number of multi-byte NOPS specified 
       // in the numberOfNopSizeN array for each size of multi-byte NOP.

          printf ("Rewrite the AST here! \n");

       // Increment the list iterator (for list of nop sequences)
          listIterator++;
        }
   }


int main( int argc, char * argv[] )
   {
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

  // regenerate the original executable.
     return backend(project);
   }
