// This translator does a source-to-source transformation on 
// the input source code to introduce asm statements of NOP
// instructions at the top of each function.

#include "rose.h"

using namespace std;
using namespace SageInterface;
using namespace SageBuilder;

class NopTransform : public SgSimpleProcessing
   {
     public:
          NopTransform() { srand ( time(NULL) ); }
          void visit ( SgNode* n );
   };

void NopTransform::visit ( SgNode* n )
   {
     SgFunctionDefinition* functionDefinition = isSgFunctionDefinition(n);
     if (functionDefinition != NULL)
        {
       // Introduce NOP's into function definition
          SgBasicBlock* functionBody = functionDefinition->get_body();
          ROSE_ASSERT(functionBody != NULL);

       // Use a multi-byte NOP just for added fun.
       // SgAsmStmt* nopStatement = buildAsmStatement("nop");

       // Generate a single random multi-byte NOP (1-9 bytes long including no NOP, when n == 0)
          int n = rand() % 10;
          if (n > 0)
             {
               printf ("Introducing a multi-byte NOP instruction (n = %d) at the top of function %s \n",n,functionDefinition->get_declaration()->get_name().str());
               SgAsmStmt* nopStatement = buildMultibyteNopStatement(n);

            // Add to the front o the list of statements in the function body
               prependStatement(nopStatement,functionBody);
             }
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

     NopTransform t;
     t.traverse(project,preorder);

  // regenerate the original executable.
     return backend(project);
   }
