#include <iostream>
#include <fstream>
//#include <rose.h>
#include <string>
#include <err.h>
#include <SgGraphTemplate.h>
#include <graphProcessing.h>
#include <staticCFG.h>
/* Testing the graph traversal mechanism now implementing in AstProcessing.h (inside src/midend/astProcessing/)*/


using namespace std;
using namespace boost;







typedef myGraph CFGforT;






class visitorTraversal : public SgGraphTraversal<CFGforT>
   {
     public:
          int tltnodes;
	  int paths;
    //      std::vector<std::vector<VertexID> > pathstore;
	  void analyzePath(vector<VertexID>& pth);
   };


void visitorTraversal::analyzePath(vector<VertexID>& pth) {
      //  ROSE_ASSERT(find(pathstore.begin(), pathstore.end(), pth) == pathstore.end());
      //  pathstore.push_back(pth);
        tltnodes += pth.size();
        paths++;
        std::cout << "paths: " << paths << std::endl;

}


int main(int argc, char *argv[]) {
  

  SgProject* proj = frontend(argc,argv);
  ROSE_ASSERT (proj != NULL); 

  SgFunctionDeclaration* mainDefDecl = SageInterface::findMain(proj);

  SgFunctionDefinition* mainDef = mainDefDecl->get_definition(); 
   visitorTraversal* vis = new visitorTraversal();
    StaticCFG::CFG cfg(mainDef);
   //cfg.buildFullCFG();
    stringstream ss;
    string fileName= StringUtility::stripPathFromFileName(mainDef->get_file_info()->get_filenameString());
    string dotFileName1=fileName+"."+ mainDef->get_declaration()->get_name() +".dot";

    cfgToDot(mainDef,dotFileName1); 
    //cfg->buildFullCFG();
    SgIncidenceDirectedGraph* g = new SgIncidenceDirectedGraph();
    g = cfg.getGraph();
    myGraph* mg = new myGraph();
    mg = instantiateGraph(g, cfg);
    vis->tltnodes = 0;
    vis->paths = 0;
    //vis->firstPrepGraph(constcfg);
    vis->constructPathAnalyzer(mg);
    //cfg.clearNodesAndEdges();
    std::cout << "finished" << std::endl;
    std::cout << "tltnodes: " << vis->tltnodes << " paths: " << vis->paths << std::endl;
    delete vis;
}
