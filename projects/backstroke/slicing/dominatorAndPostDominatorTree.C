#include <rose.h>

using namespace std;
using namespace boost;

void buildDominitorTree(const StaticCFG::CFG& cfg)
{
	//set<
}

int main(int argc, char *argv[])
{
  // Build the AST used by ROSE
  SgProject* sageProject = frontend(argc,argv);

  // Process all function definition bodies for static control flow graph generation
  Rose_STL_Container<SgNode*> functions = NodeQuery::querySubTree(sageProject, V_SgFunctionDefinition);
  for (Rose_STL_Container<SgNode*>::const_iterator i = functions.begin(); i != functions.end(); ++i)
  {
    SgFunctionDefinition* proc = isSgFunctionDefinition(*i);
    ROSE_ASSERT (proc != NULL);
    string fileName= StringUtility::stripPathFromFileName(proc->get_file_info()->get_filenameString());
    string dotFileName1=fileName+"."+ proc->get_declaration()->get_name() +".debug.dot";
    string dotFileName2=fileName+"."+ proc->get_declaration()->get_name() +".interesting.dot";

    StaticCFG::CFG cfg(proc);

    // Dump out the full CFG, including bookkeeping nodes
    cfg.buildFullCFG();
    cfg.cfgToDot(proc, dotFileName1);

    // Dump out only those nodes which are "interesting" for analyses
    cfg.buildFilteredCFG();
    cfg.cfgToDot(proc, dotFileName2);
  }

  return 0;
}

