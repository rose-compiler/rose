//Test some type-related SageInterface functions
// by traversing memory pools
//
//Liao, 3/3/2009
#include "rose.h"
#include <iostream>
using namespace std;

class RoseVisitor : public ROSE_VisitTraversal
{
  public:
    void visit ( SgNode* node);
};


void RoseVisitor::visit ( SgNode* node)
{
  if (isSgType(node))
  {
    if (SageInterface::isCopyConstructible(isSgType(node)))
      cout<<"found a copy constructible type:\t"<<node->unparseToString()<<endl;
    else
      cout<<"      found a non-copy constructible type:\t"<<node->unparseToString()<<endl;

    if (SageInterface::isAssignable(isSgType(node)))
      cout<<"found an assignable type:\t"<<node->unparseToString()<<endl;
    else
      cout<<"      found a non-assignable type:\t"<<node->unparseToString()<<endl;
  }

}

int
main ( int argc, char* argv[] )
{
  SgProject* project = frontend(argc,argv);
  ROSE_ASSERT (project != NULL);

  // ROSE visit traversal
  RoseVisitor visitor;
  visitor.traverseMemoryPool();

  return backend(project);
}

