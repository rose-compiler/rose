#include <iostream>
#include <fstream>
#include <rose.h>
//#include "interproceduralCFG.h"
#include <string>
#include <err.h>
#include "graphProcessing.h"
#include "BinaryControlFlow.h"

/* Testing the graph traversal mechanism now implementing in AstProcessing.h (inside src/midend/astProcessing/)*/

typedef bool SynthesizedAttribute;


typedef boost::graph_traits<BinaryAnalysis::ControlFlow::Graph>::vertex_descriptor Vertex;   /**< Graph vertex type. */
typedef boost::graph_traits<BinaryAnalysis::ControlFlow::Graph>::edge_descriptor   Edge;     /**< Graph edge type. */

using namespace std;
using namespace boost;

class InheritedAttribute
   {
     public:
         int val;                
	InheritedAttribute (int Z) : val(Z) {};
        InheritedAttribute () {};
	~InheritedAttribute () {};
   };


class visitorTraversal : public SgGraphTraversal<InheritedAttribute, SynthesizedAttribute, BinaryAnalysis::ControlFlow::Graph>
   {
     public:
         int pths;
         int tltnodes;
         std::map<Vertex, int> vV;
         virtual InheritedAttribute evaluateInheritedAttribute(Vertex &n, vector<InheritedAttribute> &inheritedAttributeSet);
          virtual void pathAnalyze( vector<int>& pth, bool loop,  set<vector<int> >& incloops);
          virtual void quickPathAnalyze(vector<int>& pth);
 
   };


struct GraphvizVertexWriter {
    const BinaryAnalysis::ControlFlow::Graph &cfg;
    GraphvizVertexWriter(BinaryAnalysis::ControlFlow::Graph &cfg): cfg(cfg) {}
    void operator()(std::ostream &output, const BinaryAnalysis::ControlFlow::Vertex &v) {
        SgAsmBlock *block = get(boost::vertex_name, cfg, v);
        output <<"[ label=\"" <<StringUtility::addrToString(block->get_address()) <<"\" ]";
    }
};


void visitorTraversal::quickPathAnalyze(vector<int>& pth) {
    pths++;
}


void visitorTraversal::pathAnalyze( vector<int>& pth, bool loop,  set<vector<int> >& incloops) {
    tltnodes += pth.size();
    if (loop == false) {
        pths++;
    }
    std::cout << std::endl;        
}


InheritedAttribute visitorTraversal::evaluateInheritedAttribute(Vertex  &child, vector<InheritedAttribute> &inheritedAttributesSet) {
    vV[child] = 0;
    InheritedAttribute inh = InheritedAttribute(0);
    return inh;
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
    //if (algorithm=="A") {
        BinaryAnalysis::ControlFlow cfg_analyzer;
        BinaryAnalysis::ControlFlow::Graph* cfg = new BinaryAnalysis::ControlFlow::Graph;

        *cfg = cfg_analyzer.build_graph(interps.back());
        std::ofstream mf;
        mf.open("analysis.dot");
        boost::write_graphviz(mf, *cfg, GraphvizVertexWriter(*cfg));
        visitorTraversal* vis = new visitorTraversal;
        vis->tltnodes = 0;
        vis->pths = 0;
        InheritedAttribute inh = InheritedAttribute(0);
        InheritedAttribute nullInh = InheritedAttribute(0);
        Vertex cfgb;
        Vertex cfgc;

        InheritedAttribute inhcountP = vis->traverse(cfgb, cfg, inh, nullInh, cfgc, false, true);
        std::cout << "pths: " << vis->pths << std::endl;
        std::cout << "tltnodes: " << vis->tltnodes << std::endl;
}







