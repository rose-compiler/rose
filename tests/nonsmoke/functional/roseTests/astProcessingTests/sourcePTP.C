#include <iostream>
#include <fstream>
//#include <rose.h>
#include <string>
#ifndef _MSC_VER
#include <err.h>
#endif
#include "SgGraphTemplate.h"
#include "graphProcessing.h"
#include "staticCFG.h"
/* Testing the graph traversal mechanism now implementing in AstProcessing.h (inside src/midend/astProcessing/)*/


using namespace std;
using namespace boost;
using namespace Rose;







typedef myGraph CFGforT;





//SgIncidenceDirectedGraph* g makeSmallGraph(SgGraphNode* begin, SgIncidenceDirectedGraph* g)


std::vector<SgNode*> fors;

class visitorTraversal : public SgGraphTraversal<CFGforT>
   {
     public:
          int tltnodes;
	  int paths;
          //myGraph* orig;
    //      std::vector<std::vector<VertexID> > pathstore;
	  void analyzePath(vector<VertexID>& pth);
   };

CFGforT* orig;



void visitorTraversal::analyzePath(vector<VertexID>& pathR) {
      //  ROSE_ASSERT(find(pathstore.begin(), pathstore.end(), pth) == pathstore.end());
      //  pathstore.push_back(pth);
        tltnodes += pathR.size();
        paths++;
       std::vector<SgGraphNode*> path;
       int pR = (int) pathR.size();
       for (int j = 0; j < pR; j++) {
           //SgGraphNode* R = (*orig)[pathR[j]].sg;
          // if (isSgForStatement(R->get_SgNode())) {
          //     if (find(fors.begin(), fors.end(), R->get_SgNode()) == fors.end()) {
          //         fors.push_back(R->get_SgNode());
          //     }
          // }
        }
        
        std::cout << "paths: " << paths << std::endl;

}


int main(int argc, char *argv[]) {
  

  SgProject* proj = frontend(argc,argv);
  ROSE_ASSERT (proj != NULL); 

  SgFunctionDeclaration* mainDefDecl = SageInterface::findMain(proj);

 SgFunctionDefinition* mainDef = mainDefDecl->get_definition(); 
 //  visitorTraversal* vis = new visitorTraversal();
    StaticCFG::CFG cfg(mainDef);
  Rose_STL_Container<SgNode*> functionDeclarationList = NodeQuery::querySubTree(proj,V_SgForStatement);
  std::vector<SgNode*> forsA;
 // int counter = 0;
  for (Rose_STL_Container<SgNode*>::iterator i = functionDeclarationList.begin(); i != functionDeclarationList.end(); i++) {

          SgForStatement* fni = isSgForStatement(*i);
          ROSE_ASSERT(fni != NULL);
          forsA.push_back(*i);
  }
  visitorTraversal* visif = new visitorTraversal();
  SgIncidenceDirectedGraph* g = cfg.getGraph();
  myGraph* mg = new myGraph();
  //std::set<SgNode*> completedIfs;
  mg = instantiateGraph(g, cfg);
  orig = mg;
  //vis->orig = mg;
  for (unsigned int i=0; i < forsA.size(); i++) {
     if (find(fors.begin(), fors.end(), forsA[i]) == fors.end()) {
     // if (completedIfs.find(ifs[i]) == completedIfs.end()) {
      std::cout << i << std::endl;
      fors.push_back(forsA[i]);
      //visitorTraversal* visif = new visitorTraversal();
      //SgIncidenceDirectedGraph* g = cfg.getGraph();
      //myGraph* mg = new myGraph();
      //mg = instantiateGraph(g, cfg);
      visif->tltnodes = 0;
      visif->paths = 0;
      visif->constructPathAnalyzer(mg, false,VSlink[cfg.cfgForBeginning(fors[i])],VSlink[cfg.cfgForEnd(fors[i])]);
     // visif->constructPathAnalyzer(mg, VSlink[cfg.cfgForEnd(fors[i-1])], VSlink[cfg.cfgForBeginning(fors[i])]);
      std::cout << "between: " << i << " and " << i+1 << " there are " << visif->paths << " paths." << std::endl;
     // completedIfs.insert(ifs[i]);
     // }
     }
  }
  
}
/*


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
*/

