#include "rose.h"
using namespace std;
// Must use memory pool traversal here
// AstSimple traversal will skip types
class visitorTraversal:public ROSE_VisitTraversal
{ 
  public:
    virtual void visit (SgNode * n);
};

void
visitorTraversal::visit (SgNode * n)
{
  cout<<n->class_name()<<"-------------------"<<endl;
  cout<<SageInterface::generateUniqueName(n,false)<<endl;
}

int main ( int argc, char** argv )
{
  SgProject *project = frontend (argc, argv);

  // Dump unique names
  cout<<"----------- unique names-------------"<<endl;
  visitorTraversal exampleTraversal;
  exampleTraversal.traverseMemoryPool();

  return backend (project);
}

