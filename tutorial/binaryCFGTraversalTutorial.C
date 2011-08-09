#include <iostream>
#include <fstream>
#include <rose.h>
//#include "interproceduralCFG.h"
#include <string>
#include <err.h>
#include "graphProcessing.h"
#include "BinaryControlFlow.h"

/*This tutorial explains the interface to graphProcessing that allows for binary CFG traversal*/

//This must be defined, though currently it is not used. It is not certain that this will not be of some use later
typedef bool SynthesizedAttribute;

//These must be defined relative to the BinaryAnalysis CFG
typedef boost::graph_traits<BinaryAnalysis::ControlFlow::Graph>::vertex_descriptor Vertex;   /**< Graph vertex type. */
typedef boost::graph_traits<BinaryAnalysis::ControlFlow::Graph>::edge_descriptor   Edge;     /**< Graph edge type. */


//this just makes things a bit less wordy
using namespace std;
using namespace boost;

//Very simple example of an InheritedAttribute. This is required though it will only be utilized when calculating
//InheritedAttributes via the evaluateInheritedAttribute function, as seen later on in the code
class InheritedAttribute
   {
     public:
         int val;                
	InheritedAttribute (int Z) : val(Z) {};
        InheritedAttribute () {};
	~InheritedAttribute () {};
   };


//Must be in the code, this is your interface to the Graph Traversal methods. The template parameters here are
//necessary for binary Traversal, the first is the InheritedAttribute Type defined above, the SynthesizedAttribute Type defined above as well
//And finally the CFG to be used. This CFG must conform to the boost graph model in the boost graph library
class visitorTraversal : public SgGraphTraversal<InheritedAttribute, SynthesizedAttribute, BinaryAnalysis::ControlFlow::Graph>
   {
     public:
         int pths;
         int tltnodes;
         std::map<Vertex, int> vV;

//these virtual functions are NECESSARY, they are in the SgGraphTraversal class, but do not have values in
//that situation so that the user can utilize these functions to analyze specific paths (or the InheritedAttribute nodal analysis)
         virtual InheritedAttribute evaluateInheritedAttribute(Vertex &n, vector<InheritedAttribute> &inheritedAttributeSet);
          virtual void pathAnalyze( vector<int>& pth, bool loop,  set<vector<int> >& incloops);
          virtual void quickPathAnalyze(vector<int>& pth);
 
   };

//not necessary in general, but used in a binary CFG example elsewhere
struct GraphvizVertexWriter {
    typedef boost::graph_traits<BinaryAnalysis::ControlFlow::Graph>::vertex_descriptor Vertex;
    const BinaryAnalysis::ControlFlow::Graph &cfg;
    GraphvizVertexWriter(BinaryAnalysis::ControlFlow::Graph &cfg): cfg(cfg) {}
    void operator()(std::ostream &output, const Vertex &v) {
        SgAsmBlock *block = get(boost::vertex_name, cfg, v);
        output <<"[ label=\"" <<StringUtility::addrToString(block->get_address()) <<"\" ]";
    }
};

//This function is currently not in use, but should be implemented if subpath analysis is implemented later
void visitorTraversal::quickPathAnalyze(vector<int>& pth) {
    pths++;
}

//This function will be run on
void visitorTraversal::pathAnalyze( vector<int>& pth, bool loop,  set<vector<int> >& incloops) {
    tltnodes += pth.size();
    if (loop == false) {
        pths++;
    }
    std::cout << std::endl;        
}

//The basic InheritedAttribute evaluation method
//InheritedAttributes are explained further in the documentation, but to quickly summarize
//The InheritedAttribute is determined via the InheritedAttributes of its "parents" (nodes connected via in-edges) that are not in loops.
//One must use this method with regards to loops because otherwise you have the pathological case of a node that is it's own ancestor
//Consider

// A -->  B
//   <--

//B is an out node of A and A is an outnode of B, so by the definition of InheritedAttribute these are interconnected, thus making a definition
//of parent node quite difficult.
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

//Basic starting point for getting the necessary graph to be evaluated
        BinaryAnalysis::ControlFlow cfg_analyzer;
        BinaryAnalysis::ControlFlow::Graph* cfg = new BinaryAnalysis::ControlFlow::Graph;
//builds the graph
        *cfg = cfg_analyzer.build_cfg_from_ast(interps.back());
//not necessary, just making a dot file for later analysis, graph traversal is not dependent on this
        std::ofstream mf;
        mf.open("analysis.dot");
        boost::write_graphviz(mf, *cfg, GraphvizVertexWriter(*cfg));

//defining a new instance of the visitorTraversal child class of SgGraphTraversal
        visitorTraversal* vis = new visitorTraversal;
//setting two variables used in path analysis
        vis->tltnodes = 0;
        vis->pths = 0;
//setting up base InheritedAttributes
//the first is for the startnode
        InheritedAttribute inh = InheritedAttribute(0);
//this is for the endnode (this isn't clear in binary CFG analysis without further clarification as there are many sinks and sources
        InheritedAttribute nullInh = InheritedAttribute(0);
//Two dummy vertices, in source traversals they would be the start and end nodes, as described above there are multiple sinks and
//sources in a binary CFG, thus making the idea moot
        Vertex cfgb;
        Vertex cfgc;
//The InheritedAttribute returned here is that of the endnode described above
        InheritedAttribute inhcountP = vis->traverse(cfgb, cfg, inh, nullInh, cfgc, false, true);
//returning information described in the path analysis function
        std::cout << "pths: " << vis->pths << std::endl;
        std::cout << "tltnodes: " << vis->tltnodes << std::endl;
}







