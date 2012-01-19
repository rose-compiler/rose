#include <iostream>
#include <fstream>
//#include <rose.h>
#include <string>
#include <err.h>
#include "SgGraphTemplate.h"
#include "graphProcessing.h"

#include "staticCFG.h"
#include "interproceduralCFG.h"
/* Testing the graph traversal mechanism now implementing in AstProcessing.h (inside src/midend/astProcessing/)*/
#include <sys/time.h>
#include <sys/resource.h>
using namespace std;
using namespace boost;







typedef myGraph CFGforT;






class visitorTraversal : public SgGraphTraversal<CFGforT>
   {
     public:
	  int paths;
	  void analyzePath(vector<VertexID>& pth);
   };


void visitorTraversal::analyzePath(vector<VertexID>& pth) {
        #pragma omp atomic
        paths++;

}


int main(int argc, char *argv[]) {
  
  SgProject* proj = frontend(argc,argv);
  ROSE_ASSERT (proj != NULL); 

  SgFunctionDeclaration* mainDefDecl = SageInterface::findMain(proj);

  SgFunctionDefinition* mainDef = mainDefDecl->get_definition(); 
   visitorTraversal* vis = new visitorTraversal();
    StaticCFG::InterproceduralCFG cfg(mainDef);
    stringstream ss;
    SgIncidenceDirectedGraph* g = new SgIncidenceDirectedGraph();
    g = cfg.getGraph();
    myGraph* mg = new myGraph();
    mg = instantiateGraph(g, cfg, mainDef);
    vis->tltnodes = 0;
    vis->paths = 0;
    //vis->firstPrepGraph(constcfg);
    vis->constructPathAnalyzer(mg, true, 0, 0, true);
    std::cout << "took: " << timeDifference(t2, t1) << std::endl;
    //cfg.clearNodesAndEdges();
    std::cout << "finished" << std::endl;
    std::cout << " paths: " << vis->paths << std::endl;
    delete vis;
}
