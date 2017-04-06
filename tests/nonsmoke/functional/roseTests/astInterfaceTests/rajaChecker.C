// An analyzer for RAJA code, trying to report a set of properties
//
//
// Liao, 4/6/2017
#include "rose.h"
#include <iostream>
using namespace std;
using namespace SageInterface;

class RoseVisitor : public ROSE_VisitTraversal
{
  public:
    void visit ( SgNode* node);
};

void RoseVisitor::visit ( SgNode* node)
{

}

int
main ( int argc, char* argv[])
{
  SgProject* project = frontend(argc,argv);
  ROSE_ASSERT (project != NULL);

  // ROSE memory traversal to catch all sorts of nodes, not just those on visible AST
  RoseVisitor visitor;
  visitor.traverseMemoryPool();

  return backend(project);
}

