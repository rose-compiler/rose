#include "rose.h"
#include <virtualCFG.h>
#include <cfgToDot.h>
#include <list>
#include "filteredCFG.h"
#include "DominatorTree.h"
#include "DominanceFrontier.h"
#include "DependenceGraph.h"
#include <string>

using namespace std;
using namespace VirtualCFG;
using namespace DominatorTreesAndDominanceFrontiers;

int main(int argc, char **argv)
{

    SgProject *project = frontend(argc, argv);
                string outputFileName=(*(*project->get_fileList()).begin())->get_sourceFileNameWithoutPath ();

    list < SgNode * >fd = NodeQuery::querySubTree(project, V_SgFunctionDefinition);
    // in the test example there is only one function, therefore we don't need 
    // to iterate over all functions
    // fd functionDefinition
    for (list < SgNode * >::iterator i = fd.begin(); i != fd.end(); i++)
    {
        SgFunctionDefinition *fDef = isSgFunctionDefinition(*i);

                                SliceCFGNode cfg=SliceCFGNode(fDef->cfgForBeginning());

                                string filename;
                                
                                filename =outputFileName+"."+(fDef->get_declaration()->get_name().getString())+".DF.dot";
                                
                                SliceDominatorTree dt(cfg,PRE_DOMINATOR);
                                SliceDominanceFrontier df(dt);
                                df.printFrontiers();
    }
    return 0;
}
