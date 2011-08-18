#include <iostream>
#include <fstream>
//#include <rose.h>
#include <string>
#include <err.h>
#include <SgGraphTemplate.h>
#include <graphProcessing.h>
#include <staticCFG.h>


using namespace std;
using namespace boost;






//For StaticCFG this is necessary, it allows the traversal to accept a graph that it can run on
typedef myGraph CFGforT;





//This is similar to the visitorTraversals in 
class visitorTraversal : public SgGraphTraversal<CFGforT>
   {
     public:
          //these are just some variables we'll use in the traversal
          //total number of nodes traversed
          int tltnodes;
          //the number of paths
	  int paths;
          //a user defined function that is activated inside the algorithm to give the user
          //the result they're looking for
	  void analyzePath(vector<VertexID>& pth);
   };

//The path analysis function, VertexID is used because that's
//what the template in SgGraphTemplate uses
void visitorTraversal::analyzePath(vector<VertexID>& pth) {
        tltnodes += pth.size();
        paths++;

}

//Quick point:
// any "new" declarations in this should always be used in this traversal
// without them you can get very strange errors, like a SegFault at the
// return statement
int main(int argc, char *argv[]) {
  
//necessary preamble
  SgProject* proj = frontend(argc,argv);
  ROSE_ASSERT (proj != NULL); 

  SgFunctionDeclaration* mainDefDecl = SageInterface::findMain(proj);

  SgFunctionDefinition* mainDef = mainDefDecl->get_definition();
   //creating the traversal 
   visitorTraversal* vis = new visitorTraversal;
    StaticCFG::CFG cfg(mainDef);
//end of preamble
//This is necessary tof the cfgToDot function, not necessary but gives an output to analyze the graph visually
    stringstream ss;
    string fileName= StringUtility::stripPathFromFileName(mainDef->get_file_info()->get_filenameString());
    string dotFileName1=fileName+"."+ mainDef->get_declaration()->get_name() +".dot";
    //the cfgToDot Function to produce the dot file
    cfgToDot(mainDef,dotFileName1); 
    //Here's the graph we want
    SgIncidenceDirectedGraph* g = new SgIncidenceDirectedGraph();
    g = cfg.getGraph();
    //declaration of the algorithm readable CFG
    CFGforT* constcfg = new CFGforT;
    //constructs the CFG
    constcfg = instantiateGraph(g, cfg);
    //initializing visitor variables
    vis->tltnodes = 0;
    vis->paths = 0;
    //The function to construct 
    vis->constructPathAnalyzer(constcfg);
    //test complete
    std::cout << "finished" << std::endl;
    //display visitor results
    std::cout << "tltnodes: " << vis->tltnodes << " paths: " << vis->paths << std::endl;
    
    return 1;
}
