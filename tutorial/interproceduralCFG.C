// Example translator to generate dot files of virtual, interprocedural control flow graphs
#include "rose.h"
#include <string>
using namespace std;

int main(int argc, char *argv[]) 
{
  // Build the AST used by ROSE
  SgProject* proj = frontend(argc,argv);
  ROSE_ASSERT (proj != NULL); 

  SgFunctionDeclaration* mainDef = SageInterface::findMain(proj);
  ROSE_ASSERT (mainDef != NULL); 

  SgFunctionDefinition* main = mainDef->get_definition();
  ROSE_ASSERT (main != NULL); 

  string fileName= StringUtility::stripPathFromFileName(main->get_file_info()->get_filenameString());
  string dotFileName = fileName + ".IP.dot";

  // Dump out the interprocedural CFG, including bookkeeping nodes
  VirtualCFG::interproceduralCfgToDotForDebugging(main, dotFileName);

  return 0;
}

