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
  // std::cout << "#visited nodes = " << count << std::endl;

  // OK, if program does not terminate before
  SgFilePtrList fl = project->get_files();
  SgFile*       firstfile = fl[0];
  ROSE_ASSERT(firstfile!=NULL);

  string        filename = rose::StringUtility::stripPathFromFileName (firstfile->getFileName());
  string        ofilename = filename+".output";
  ofstream      ofile(ofilename.c_str());

  ofile << "ok." << std::endl;
  return 0;
}
