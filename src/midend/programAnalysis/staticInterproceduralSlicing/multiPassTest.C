#include "rose.h"
#include <AstInterface.h>
#include <StmtInfoCollect.h>
#include <ReachingDefinition.h>
//#include <DefUseChain.h>
#include <DirectedGraph.h>
#include "DependenceGraph.h"
#include "SlicingInfo.h"
#include "CreateSlice.h"
#include "ControlFlowGraph.h"
#include "DominatorTree.h"
#include "CreateSliceSet.h"

#include <list>
#include <set>
#include <iostream>

#define DEBUG 1
using namespace DominatorTreesAndDominanceFrontiers;
using namespace std;

int main(int argc, char *argv[])
{
        SgProject *project = frontend(argc, argv);
        SlicingInfo si=SlicingInfo();
        si.traverse(project, preorder);

        SystemDependenceGraph * sdg=new SystemDependenceGraph();
        sdg->parseProject(project);

        CreateSliceSet sliceSet(sdg,si.getSlicingTargets());
        CreateSlice cs(sliceSet.computeSliceSet());

        cs.traverse(project);
        AstTests::runAllTests(project);

        delete(sdg);
        sdg=new SystemDependenceGraph();
        sdg->parseProject(project);
        
        project->unparse();     
}
