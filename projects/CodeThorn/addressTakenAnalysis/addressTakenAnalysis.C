#include <iostream>
#include <fstream>
#include "rose.h"
#include "AstTerm.h"
#include "AstMatching.h"

using namespace CodeThorn;

int main(int argc, char* argv[])
{
  // Build the AST used by ROSE
  SgProject* sageProject = frontend(argc,argv);
  SgNode* root = sageProject;
  RoseAst ast(root);
  AstMatching m;
  return 0;
}
