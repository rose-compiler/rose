// A test case for SageInterface::replaceMacroCallsWithExpandedStrings()
// Liao, 3/31/2009
#include "rose.h" 
#include <iostream>
using namespace std;
class visitorTraversal:public AstSimpleProcessing
{
  public:
    virtual void visit (SgNode * n);
};
void
visitorTraversal::visit (SgNode * n)
{
  SgPragmaDeclaration * pragma = isSgPragmaDeclaration(n);
  if (pragma)
  {
    SageInterface::replaceMacroCallsWithExpandedStrings(pragma);
    cout<<"Modified pragma is:"<<pragma->unparseToString()<<endl;
  }
}
int
main (int argc, char *argv[])
{
  SgProject *project = frontend (argc, argv);
  visitorTraversal exampleTraversal;
  exampleTraversal.traverse(project, preorder);
  AstTests::runAllTests(project);
  return backend (project);
}


