#include <iostream>
#include <fstream>
#include <rose.h>
#include <string>
#include <err.h>
#include "graphProcessing.h"
#include "BinaryControlFlow.h"


using namespace std;
// you need boost to put together the graph
using namespace boost;
using namespace rose;

//renaming to Vertex and Edge to save space
typedef boost::graph_traits<BinaryAnalysis::ControlFlow::Graph>::vertex_descriptor Vertex;   /**< Graph vertex type. */
typedef boost::graph_traits<BinaryAnalysis::ControlFlow::Graph>::edge_descriptor   Edge;     /**< Graph edge type. */



//visitor class, analyze path runs on the paths as they are completed, it is declared in the graphProcessing.h as a virtual function
//that will be implemented by the user
class visitorTraversal : public SgGraphTraversal<BinaryAnalysis::ControlFlow::Graph>
   {
     public:
         int pths;
         int tltnodes;

          virtual void analyzePath( vector<Vertex>& pth);
 
 
   };


//The path analysis function, relatively self explanatory
void visitorTraversal::analyzePath(vector<Vertex>& pth) {
    tltnodes += pth.size();
    pths++;
}


int main(int argc, char *argv[]) {
    
    /* Parse the binary file */
    SgProject *project = frontend(argc, argv);
    std::vector<SgAsmInterpretation*> interps = SageInterface::querySubTree<SgAsmInterpretation>(project);
    if (interps.empty()) {
        fprintf(stderr, "no binary interpretations found\n");
        exit(1);
    }

    /* Calculate plain old CFG. */
        BinaryAnalysis::ControlFlow cfg_analyzer;
        BinaryAnalysis::ControlFlow::Graph* cfg = new BinaryAnalysis::ControlFlow::Graph;

        cfg_analyzer.build_block_cfg_from_ast(interps.back(), *cfg);
//instantiating the graph visitor
        visitorTraversal* vis = new visitorTraversal;
//setting object variables
        vis->tltnodes = 0;
        vis->pths = 0;
//the algorithm
        vis->constructPathAnalyzer(cfg);
//printing object variables
        std::cout << "pths: " << vis->pths << std::endl;
        std::cout << "tltnodes: " << vis->tltnodes << std::endl;
        return 0;
}







