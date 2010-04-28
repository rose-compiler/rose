// Example ROSE Translator used for testing ROSE infrastructure
#include "rose.h"
#include "sageInterfaceAsm.h"

using namespace std;
using namespace SageInterface;

class CountTraversal : public SgSimpleProcessing
   {
     public:
       // Local Accumulator Attribute
          int count;
          int maxCount;
          bool previousInstructionWasNop;
          SgAsmInstruction* nopSequenceStart;
          std::vector<pair<SgAsmInstruction*,int> > nopSequences;

          CountTraversal() : count(0), maxCount(0), previousInstructionWasNop(false) {}
          void visit ( SgNode* n );
   };

void CountTraversal::visit ( SgNode* n )
   {
     SgAsmInstruction* asmInstruction = isSgAsmInstruction(n);
     if (asmInstruction != NULL)
        {
          if (isInstructionKind(asmInstruction,x86_nop) == true)
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
            // Save the count if it was larger than the max count.
            // if (count > maxCount)
               if (count > 0)
                   {
                     maxCount = count;
                     SgAsmFunctionDeclaration* functionDeclaration = getAsmFunctionDeclaration(asmInstruction);
                     printf ("Reporting NOP sequence of length %3d at address %p in function %s (reason for this being a function = %u = %s) \n",
                          count,nopSequenceStart->get_address(),functionDeclaration->get_name().c_str(),
                          functionDeclaration->get_reason(),functionDeclaration->get_functionReasonString().c_str());
                     nopSequences.push_back(pair<SgAsmInstruction*,int>(nopSequenceStart,count));
                   }

               count = 0;
               previousInstructionWasNop = false;
             }
        }
   }

int main( int argc, char * argv[] )
   {
  // Generate the ROSE AST.
     SgProject* project = frontend(argc,argv);

  // AST consistency tests (optional for users, but this enforces more of our tests)
     AstTests::runAllTests(project);

     CountTraversal t;
     t.traverse(project,preorder);

  // regenerate the original executable.
     return backend(project);
   }
