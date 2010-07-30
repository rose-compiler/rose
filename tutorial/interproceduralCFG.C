// Example translator to generate dot files of virtual, interprocedural control flow graphs
#include "rose.h"
#include "interproceduralCFG.h"
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
  string dotFileName1=fileName+"."+ mainDef->get_declaration()->get_name() +".debug.dot";
  string dotFileName2=fileName+"."+ mainDef->get_declaration()->get_name() +".interesting.dot";

  StaticCFG::InterproceduralCFG cfg(mainDef);

  // Dump out the full CFG, including bookkeeping nodes
  cfg.buildFullCFG();
  cfg.cfgToDot(proc, dotFileName1);

  // Dump out only those nodes which are "interesting" for analyses
  cfg.buildFilteredCFG();
  cfg.cfgToDot(proc, dotFileName2);

  return 0;
}
