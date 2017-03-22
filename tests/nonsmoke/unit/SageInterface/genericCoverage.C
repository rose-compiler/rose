#include <iostream>

#include "rose.h"
#include "sageGeneric.h"

static size_t count = 0;

struct AstExplorer
{
  void handle(SgNode&)
  {
    ++count;
  }

  void handle(SgInitializedName& n)
  {
    ++count;
    sg::dispatch(AstExplorer(), n.get_typeptr());
  }
};

struct VisitorTraversal : AstSimpleProcessing
{
  VisitorTraversal() {}

  virtual void visit(SgNode*);
};

void VisitorTraversal::visit(SgNode* n)
{
  sg::dispatch(AstExplorer(), n);
}


int main ( int argc, char* argv[] )
{
  SgProject* project = frontend(argc,argv);
  ROSE_ASSERT (project != NULL);

  // Build the traversal object
  VisitorTraversal exampleTraversal;

  exampleTraversal.traverse(project, preorder);
  std::cout << "#visited nodes = " << count << std::endl;

  return 0;
}
