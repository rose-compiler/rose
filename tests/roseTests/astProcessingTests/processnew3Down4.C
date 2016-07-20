#include <iostream>
#include <fstream>
#include <rose.h>
#include <string>
#include <err.h>
#include "graphProcessing.h"
#include "graphTemplate.h"


using namespace std;
using namespace boost;
using namespace rose;

using namespace Backstroke;

struct CFGNodeFilter2
{
	bool operator()(const VirtualCFG::CFGNode& cfgNode) const
	{
		return true;
	}
};



typedef CFG<CFGNodeFilter2> CFGforT;



class visitorTraversal : public SgGraphTraversal<CFGforT>
   {
     public: 
          int tltnodes;
          virtual void analyzePath(vector<CFGforT::Vertex>& pth);
          int pths;
   };


void visitorTraversal::analyzePath(vector<CFGforT::Vertex>& pth) {   
        tltnodes += pth.size();
        pths++;
        std::cout << "pth" << std::endl;
        for (int i = 0; i < pth.size(); i++) {
            std::cout << intvertmap[pth[i]] << ", ";
        }
        std::cout << "end" << std::endl;
}


int main(int argc, char *argv[]) {
 
    SgProject* proj = frontend(argc,argv);
    ROSE_ASSERT (proj != NULL); 
    SgFunctionDeclaration* mainDefDecl = SageInterface::findMain(proj);
    SgFunctionDefinition* mainDef = mainDefDecl->get_definition();
    CFGforT* cfggraph = new CFGforT;
    cfggraph->build(mainDef);
     visitorTraversal* vis = new visitorTraversal;
    string fileName= StringUtility::stripPathFromFileName(mainDef->get_file_info()->get_filenameString());
    cfggraph->toDot("dotcheck.dot");
    vis->pths = 0;
    vis->tltnodes = 0;
    vis->firstPrepGraph(cfggraph)
    vis->constructPathAnalyzer(cfggraph);
    cout << "path creation solution: " << vis->pths << std::endl;
    cout << "total nodes traversed (including duplications): " << vis->tltnodes << std::endl;
    return 0;
}
