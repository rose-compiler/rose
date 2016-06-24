// This example ROSE translator just does an analysis of the input binary.
// This is an example of a search on the binary for instructions that
// match a specific example pattern.

#include "rose.h"
#include "sageInterfaceAsm.h"

using namespace std;
using namespace SageInterface;

class CountTraversal : public SgSimpleProcessing
   {
     public:
          void visit ( SgNode* n );
   };

void CountTraversal::visit ( SgNode* n )
   {
     SgAsmInstruction* asmInstruction = isSgAsmInstruction(n);
     if (asmInstruction != NULL)
        {
        }
   }

int main( int argc, char * argv[] )
   {
  // Initialize and check compatibility. See rose::initialize
     ROSE_INITIALIZE;

  // Generate the ROSE AST.
     SgProject* project = frontend(argc,argv);

  // AST consistency tests (optional for users, but this enforces more of our tests)
     AstTests::runAllTests(project);

     printf ("Pattern search example not yet implemented! \n");
     ROSE_ASSERT(false);

     CountTraversal t;
     t.traverse(project,preorder);

  // regenerate the original executable.
     return backend(project);
   }
