// Example translator to generate dot files of virtual, interprocedural control flow graphs
#include "rose.h"
#include "interproceduralCFG.h"
#include <string>
#include <err.h>
using namespace std;

int main(int argc, char *argv[]) 
{
  // Build the AST used by ROSE
  SgProject* proj = frontend(argc,argv);
  ROSE_ASSERT (proj != NULL); 

  SgFunctionDeclaration* mainDefDecl = SageInterface::findMain(proj);
  if (mainDefDecl == NULL) {
    warnx ("Could not find main(). Skipping Inteprocedural CFG test");
    return 0; 
  }

  SgFunctionDefinition* mainDef = mainDefDecl->get_definition();
  if (mainDef == NULL) {
    warnx ("Could not find main(). Skipping Inteprocedural CFG test");
    return 0; 
  }

  StaticCFG::InterproceduralCFG cfg(mainDef);
  cfg.buildFullCFG();

  return 0;
}
