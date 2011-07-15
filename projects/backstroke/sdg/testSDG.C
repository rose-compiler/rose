#include "newCFG.h"
#include "newCDG.h"
#include "newDDG.h"

int main(int argc, char *argv[])
{
    // Build the AST used by ROSE
    SgProject* project = frontend(argc,argv);
    SgSourceFile* sourceFile = isSgSourceFile((*project)[0]);

    // Process all function definition bodies for static control flow graph generation
    Rose_STL_Container<SgNode*> functions = NodeQuery::querySubTree(project, V_SgFunctionDefinition);
    for (Rose_STL_Container<SgNode*>::const_iterator i = functions.begin(); i != functions.end(); ++i)
    {
        SgFunctionDefinition* proc = isSgFunctionDefinition(*i);
        ROSE_ASSERT (proc != NULL);

        if (!proc->get_file_info()->isSameFile(sourceFile))
            continue;

        Backstroke::ControlFlowGraph cfg(proc, Backstroke::InterestingCFGNodeFilter());
        cfg.toDot("CFG.dot");
        
        Backstroke::ControlDependenceGraph cdg(cfg);
        cdg.toDot("CDG.dot");
        
        Backstroke::DataDependenceGraph ddg(cfg);
        ddg.toDot("DDG.dot");
        
        break;
    }
}
