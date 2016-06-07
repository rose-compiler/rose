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






/* You need to use myGraph type here because the conversion of StaticCFG::InterproceduralCFG or StaticCFG::CFG needs to be
in a boost form. The SgGraphTemplate.h file handles this conversion and myGraph is specific to that file */
typedef myGraph CFGforT;





/*
Your basic visitor traversal subclassed from SgGraphTraversal on the CFGforT template as defined
above 
*/
class visitorTraversal : public SgGraphTraversal<CFGforT>
   {
     public:
	  int paths;
       /* This is the function run by the algorithm on every path, VertexID is a type implemented in SgGraphTemplate.h */
	  void analyzePath(vector<VertexID>& pth);
   };

/* defining the analyzePath function. This simply counts paths as should be obvious. Again, VertexID is defined in SgGraphTemplate.h */
void visitorTraversal::analyzePath(vector<VertexID>& pth) {
        paths++;
}


int main(int argc, char *argv[]) {
  // Initialize and check compatibility. See rose::initialize
  ROSE_INITIALIZE;

  /* First you need to produce the project file*/
  SgProject* proj = frontend(argc,argv);
  ROSE_ASSERT (proj != NULL); 
 /* Getting the Function Declaration and Definition for producing the graph */
  SgFunctionDeclaration* mainDefDecl = SageInterface::findMain(proj);
  SgFunctionDefinition* mainDef = mainDefDecl->get_definition(); 
/* Instantiating the visitorTraversal */
   visitorTraversal* vis = new visitorTraversal();
/* This creates the StaticCFG::InterproceduralCFG object to be converted to a boost graph */
    StaticCFG::InterproceduralCFG cfg(mainDef);
    stringstream ss;
    SgIncidenceDirectedGraph* g = new SgIncidenceDirectedGraph();
    /* We got the necessary internal SgIncidenceDirectedGraph from the cfg */
    g = cfg.getGraph();
    myGraph* mg = new myGraph();
/* Converting the cfg to a boost graph */
    mg = instantiateGraph(g, cfg, mainDef);
/* Set internal variables */
    vis->paths = 0;
    /* invoking the traversal, the first argument is the graph, the second is true if you
    do not want bounds, false if you do, the third and fourth arguments are starting and stopping 
    vertices respectively, if you are not bounding simply insert 0. Finally the last argument is
    currently deprecated */
    vis->constructPathAnalyzer(mg, true, 0, 0, true);
    std::cout << "finished" << std::endl;
    std::cout << " paths: " << vis->paths << std::endl;
    delete vis;
}
