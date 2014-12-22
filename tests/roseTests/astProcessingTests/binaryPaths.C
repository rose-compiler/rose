#include <iostream>
#include <fstream>
#include <rose.h>
//#include "interproceduralCFG.h"
#include <string>
#ifndef _MSC_VER
#include <err.h>
#endif
#include "graphProcessing.h"
#include "BinaryControlFlow.h"
#include "BinaryLoader.h"
/* Testing the graph traversal mechanism now implementing in AstProcessing.h (inside src/midend/astProcessing/)*/

using namespace std;
using namespace boost;
using namespace rose;


typedef boost::graph_traits<BinaryAnalysis::ControlFlow::Graph>::vertex_descriptor Vertex;   /**< Graph vertex type. */
typedef boost::graph_traits<BinaryAnalysis::ControlFlow::Graph>::edge_descriptor   Edge;     /**< Graph edge type. */




class visitorTraversal : public SgGraphTraversal<BinaryAnalysis::ControlFlow::Graph>
   {
     public:
         long int pths;
         long int tltnodes;
         std::map<Vertex, int> vV;

          virtual void analyzePath( vector<Vertex>& pth);
 
 
   };



void visitorTraversal::analyzePath(vector<Vertex>& pth) {
    //tltnodes += pth.size();
//    #pragma omp atomic
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
        std::ofstream mf;
        mf.open("analysis.dot");
        visitorTraversal* vis = new visitorTraversal;
        vis->tltnodes = 0;
        vis->pths = 0;
//	vis->firstPrepGraph(cfg);
        vis->constructPathAnalyzer(cfg, true, 0, 0, false);
        std::cout << "pths: " << vis->pths << std::endl;
        std::cout << "tltnodes: " << vis->tltnodes << std::endl;
}







