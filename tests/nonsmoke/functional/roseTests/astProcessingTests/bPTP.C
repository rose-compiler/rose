#include <iostream>
#include <fstream>
#include <rose.h>
//#include "interproceduralBinaryAnalysis::ControlFlow::Graph.h"
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

  typedef boost::graph_traits<BinaryAnalysis::ControlFlow::Graph>::vertex_iterator vertex_iterator;
    typedef boost::graph_traits<BinaryAnalysis::ControlFlow::Graph>::out_edge_iterator out_edge_iterator;
    typedef boost::graph_traits<BinaryAnalysis::ControlFlow::Graph>::in_edge_iterator in_edge_iterator;
    typedef boost::graph_traits<BinaryAnalysis::ControlFlow::Graph>::edge_iterator edge_iterator;





class visitorTraversal : public SgGraphTraversal<BinaryAnalysis::ControlFlow::Graph>
   {
     public:
         int pths;
         int tltnodes;
         std::map<Vertex, int> vV;
         std::set<std::vector<Vertex> > paths;
          virtual void analyzePath( vector<Vertex>& pth);
 
 
   };



void visitorTraversal::analyzePath(vector<Vertex>& pth) {
    //ROSE_ASSERT(paths.find(pth) == paths.end()); //{ 
    //    std::cout << "double path" << std::endl;
    //}
    //else {
        paths.insert(pth); 
        tltnodes += pth.size();
        pths++;
    //}
}


int main(int argc, char *argv[]) {
    
    /* Parse the binary file */
    SgProject *project = frontend(argc, argv);
    std::vector<SgAsmInterpretation*> interps = SageInterface::querySubTree<SgAsmInterpretation>(project);
    if (interps.empty()) {
        fprintf(stderr, "no binary interpretations found\n");
        exit(1);
    }
    std::vector<Vertex> nfurcations;
    /* Calculate plain old BinaryAnalysis::ControlFlow::Graph. */
        BinaryAnalysis::ControlFlow cfg_analyzer;
        BinaryAnalysis::ControlFlow::Graph* cfg = new BinaryAnalysis::ControlFlow::Graph;

        cfg_analyzer.build_block_cfg_from_ast(interps.back(), *cfg);
    
   vertex_iterator v1, vend1;
   for (tie(v1, vend1) = vertices(*cfg); v1 != vend1; ++v1)
    {
    int outedges = 0;
    out_edge_iterator i, j;
    for (boost::tie(i, j) = boost::out_edges(*v1, *cfg); i != j; ++i)
    {
        outedges++;
    }
    if (outedges > 1) {
        nfurcations.push_back(*v1);
    }
    }

        std::ofstream mf;
        mf.open("analysis.dot");
        visitorTraversal* vis = new visitorTraversal;
        vis->tltnodes = 0;
        vis->pths = 0;
	//vis->firstPrepGraph(cfg);
        for (unsigned int i = 0; i < nfurcations.size()-1; i++) {
            vis->tltnodes = 0;
            vis->pths = 0;
            vis->paths.clear();
            vis->constructPathAnalyzer(cfg,false, nfurcations[i], nfurcations[i+1]);
            //cout << "from: " << vis->vertintmap[nfurcations[i]] << " to: " << vis->vertintmap[nfurcations[i+1]] << " " << vis->pths << " paths" << std::endl; 
        }
        //vis->constructPathAnalyzer(cfg);
        //std::cout << "pths: " << vis->pths << std::endl;
        //std::cout << "tltnodes: " << vis->tltnodes << std::endl;
}







