#include <iostream>
#include <fstream>
#include <rose.h>
//#include "interproceduralCFG.h"
#include <string>
#include <err.h>

/* These are necessary for any binary Traversal */

#include "graphProcessing.h"
#include "BinaryControlFlow.h"
#include "BinaryLoader.h"
/* Testing the graph traversal mechanism now implementing in graphProcessing.h (inside src/midend/astProcessing/)*/

using namespace std;
using namespace boost;

/* These should just be copied verbatim */

typedef boost::graph_traits<BinaryAnalysis::ControlFlow::Graph>::vertex_descriptor Vertex;   /**< Graph vertex type. */
typedef boost::graph_traits<BinaryAnalysis::ControlFlow::Graph>::edge_descriptor   Edge;     /**< Graph edge type. */



/* We first make a visitorTraversal, subclassed from SgGraphTraversal templated on the BinaryAnalysis:ControlFlow::Graph
which is implemented as a boost graph */


class visitorTraversal : public SgGraphTraversal<BinaryAnalysis::ControlFlow::Graph>
   {
    
     public:
         long int pths;
         long int tltnodes;
          /* This needs to be in any visitorTraversal, it is the function that will be run on every path by the graph
          path analysis algorithm, notice the Vertex type is from the above typedefs */
          virtual void analyzePath( vector<Vertex>& pth);
 
 
   };


/* This is a very simple incarnation, it just counts paths */
void visitorTraversal::analyzePath(vector<Vertex>& pth) {
    pths++;
}


int main(int argc, char *argv[]) {
    // Initialize and check compatibility. See rose::initialize
    ROSE_INITIALIZE;

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
        /* Declaring the visitorTraversal */
        visitorTraversal* vis = new visitorTraversal;
        /* Setting internal variables*/
        vis->tltnodes = 0;
        vis->pths = 0;

        /* visitorTraversal has 5 arguments, the first is the ambient CFG, the second identifies whether or not
        you are bounding the graph, that is, whether you want all your paths to start at one specific node and end at
       another specific node, the fourth and fifth would be start and end if the graph were bounded. Since they aren't
       you can simply input 0, for the moment the final argument is deprecated, though it's purpose was to tell the program
        that your analysis function was thread safe, that is that openMP could run it without having a critical command. 
       Currently a critical is always used */

        vis->constructPathAnalyzer(cfg, true, 0, 0, false);
        std::cout << "pths: " << vis->pths << std::endl;
        std::cout << "tltnodes: " << vis->tltnodes << std::endl;
}







