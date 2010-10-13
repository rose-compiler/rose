#include "rose.h"
#include <list>
#include <sstream>
#include <iostream>
#include <fstream>
#include <string.h>
using namespace std;

#include "common.h"
#include "cfgUtils.h"
#include "analysisCommon.h"
#include "nodeState.h"
#include "latticeFull.h"
#include "analysis.h"
#include "saveDotAnalysis.h"


int numFails=0;

// ====================================
// ===========   Dataflow   ===========
// ====================================

class saveAllDataflow : public IntraFWDataflow
{
	public:
	saveAllDataflow()
	{}
		
	// generates the initial lattice state for the given dataflow node, in the given function, with the given NodeState
	//vector<Lattice*> genInitState(const Function& func, const DataflowNode& n, const NodeState& state)
	void genInitState(const Function& func, const DataflowNode& n, const NodeState& state,
	                  vector<Lattice*>& initLattices, vector<NodeFact*>& initFacts)
	{
		//vector<Lattice*> initLattices;
		initLattices.push_back((Lattice*)(new BoolAndLattice()));
		initLattices.push_back((Lattice*)(new IntMaxLattice()));
		//return initLattices;
	}
	
	bool transfer(const Function& func, const DataflowNode& n, NodeState& state, const vector<Lattice*>& dfInfo)
	{
		printf("saveAllDataflow: node=<%s | %s>\n", n.getNode()->class_name().c_str(), n.getNode()->unparseToString().c_str());
		printf("   isSgAddOp(n.getNode()=%d\n", isSgAddOp(n.getNode()));
		bool modified = false;
		if(isSgAddOp(n.getNode()))
		{
			modified = dynamic_cast<BoolAndLattice*>(dfInfo.at(0))->set(true);
			modified = dynamic_cast<IntMaxLattice*>(dfInfo.at(1))->incr(1) || modified;
		}
		else
		{
			//modified = dynamic_cast<BoolAndLattice*>(dfInfo.at(0))->set(false);
			modified = dynamic_cast<IntMaxLattice*>(dfInfo.at(1))->maximum(0) || modified;
		}
		return modified;
	}
};

class checkAllDataflow : public UnstructuredPassIntraAnalysis
{
	public:
	saveAllDataflow* creator;
	
	checkAllDataflow(saveAllDataflow* creator)
	{
		this->creator = creator;
	}
	
	void visit(const Function& func, const DataflowNode& n, NodeState& state)
	{
		printf("checkAllDataflow: node=<%s | %s>\n", n.getNode()->class_name().c_str(), n.getNode()->unparseToString().c_str());
		/*stringWrapper* s0 = (stringWrapper*)state.getFact(creator, 0);
		stringWrapper* s1 = (stringWrapper*)state.getFact(creator, 1);
		printf("                     fact=<%p | %p>\n", s0, s1);
		printf("                     fact=<%s | %s>\n", s0->myStr.c_str(), s1->myStr.c_str());* /
		if(n.getNode()->class_name() != s0->myStr.c_str())
		{
			printf("ERROR in checkAllDataflow: Expected class name \"%s\" but the saved class name is \"%s\"\n", n.getNode()->class_name().c_str(), s0->myStr.c_str());
			numFails++;
		}
		
		if(n.getNode()->unparseToString() != s1->myStr.c_str())
		{
			printf("ERROR in checkAllDataflow: Expected class name \"%s\" but the saved class name is \"%s\"\n", n.getNode()->unparseToString().c_str(), s0->myStr.c_str());
			numFails++;
		}*/
		
		printf("                     lattice0 = %s\n", state.getLatticeBelow(creator, 0)->str().c_str());
		printf("                     lattice1 = %s\n", state.getLatticeBelow(creator, 1)->str().c_str());
	}
};


int main( int argc, char * argv[] ) 
{
	printf("========== S T A R T ==========\n");
	
	// Build the AST used by ROSE
	SgProject* project = frontend(argc,argv);
	
	cfgUtils::initCFGUtils(project);

	initAnalysis(project);
	
	analysisDebugLevel = 0;
	
	// Save the CFGs of all the functions into their own files
	//saveCFGsToDots();

	saveAllDataflow sad;
	UnstructuredPassInterAnalysis upia_sad(sad);
	upia_sad.runAnalysis();
	
	checkAllDataflow chadf(&sad);
	UnstructuredPassInterAnalysis upia_chadf(chadf);
	upia_chadf.runAnalysis();
	
	if(numFails==0)
		printf("PASS\n");
	else
		printf("FAIL!\n");
	
	printf("==========  E  N  D  ==========\n");
	
	// Unparse and compile the project (so this can be used for testing)
	return backend(project);
}
