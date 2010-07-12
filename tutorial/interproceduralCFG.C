// Example translator to generate dot files of virtual, interprocedural control flow graphs
#include "rose.h"
#include <string>
using namespace std;

int main(int argc, char *argv[]) 
{
  // Build the AST used by ROSE
  SgProject* proj = frontend(argc,argv);
  ROSE_ASSERT (proj != NULL); 

  SgFunctionDeclaration* mainDefDecl = SageInterface::findMain(proj);
  ROSE_ASSERT (mainDefDecl != NULL); 

  SgFunctionDefinition* mainDef = mainDefDecl->get_definition();
  ROSE_ASSERT (mainDef != NULL); 

  string fileName= StringUtility::stripPathFromFileName(mainDef->get_file_info()->get_filenameString());
  string dotFileName = fileName + ".IP.dot";

  // Dump out the interprocedural CFG, including bookkeeping nodes
  VirtualCFG::interproceduralCfgToDotForDebugging(mainDef, dotFileName);

  return 0;
}

