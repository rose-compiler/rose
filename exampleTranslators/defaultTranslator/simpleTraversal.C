#include "rose.h"

using namespace std;

class visitorTraversal : public AstSimpleProcessing
{
  protected:
    void virtual visit (SgNode* n)
    {
      cout<<n->class_name() <<"@"<<n<<endl;
    }
};

int
main ( int argc, char* argv[] )
{
  // Initialize and check compatibility. See Rose::initialize
  ROSE_INITIALIZE;

  SgProject* project = frontend(argc,argv);
  visitorTraversal exampleTraversal;

  // Call the traversal starting at the project node of the AST
  exampleTraversal.traverseInputFiles(project,preorder);

  return 0;
}
