// Example ROSE Translator: used within ROSE/tutorial

#include "rose.h"
#include <GraphUpdate.h>
#include "CFGImpl.h"
#include "GraphDotOutput.h"
#include "controlFlowGraph.h"
#include "CommandOptions.h"

using namespace std;

// Use the ControlFlowGraph is defined in both PRE
// and the DominatorTreesAndDominanceFrontiers namespaces.
// We want to use the one in the PRE namespace.
using namespace PRE;

class visitorTraversal : public AstSimpleProcessing
   {
     public:
          virtual void visit(SgNode* n);
   };

void visitorTraversal::visit(SgNode* n)
   {
     SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(n);
     if (functionDeclaration != NULL)
        {
          SgFunctionDefinition* functionDefinition = functionDeclaration->get_definition();
          if (functionDefinition != NULL)
             {
               SgBasicBlock* functionBody = functionDefinition->get_body();
               ROSE_ASSERT(functionBody != NULL);

               ControlFlowGraph controlflow;

            // The CFG can only be called on a function definition (at present)
               makeCfg(functionDefinition,controlflow);
               string fileName = functionDeclaration->get_name().str();
               fileName += ".dot";
               ofstream dotfile(fileName.c_str());
               printCfgAsDot(dotfile, controlflow);
             }
        }
   }

int main( int argc, char * argv[] )
   {
  // Initialize and check compatibility. See rose::initialize
     ROSE_INITIALIZE;

  // Build the AST used by ROSE
     SgProject* project = frontend(argc,argv);

     CmdOptions::GetInstance()->SetOptions(argc,argv);

  // Build the traversal object
     visitorTraversal exampleTraversal;

  // Call the traversal starting at the project node of the AST
     exampleTraversal.traverseInputFiles(project,preorder);

     return 0;
   }

