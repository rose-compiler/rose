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
// =========== Unstructured ===========
// ====================================
class stringWrapper : public NodeFact
{
	public:
	string myStr;
	stringWrapper(string myStr)
	{
		this->myStr = myStr;
	}
	
	NodeFact* copy() const
	{ return new stringWrapper(myStr); }
	
	// The string that represents this object
	// If indent!="", every line of this string must be prefixed by indent
	// The last character of the returned string should not be '\n', even if it is a multi-line string.
	string str(string indent="")
	{
		return indent + myStr;
	}
};

class saveAllUnstructured : public UnstructuredPassIntraAnalysis
{
	public:
	saveAllUnstructured()
	{
	}
	
	void visit(const Function& func, const DataflowNode& n, NodeState& state)
	{
		//printf("saveAllUnstructured: node=<%s | %s>\n", n.getNode()->class_name().c_str(), n.getNode()->unparseToString().c_str());
		stringWrapper* s0 = new stringWrapper(n.getNode()->class_name());
		stringWrapper* s1 = new stringWrapper(n.getNode()->unparseToString());
		//printf("                      fact=<%p | %p>\n", s0, s1);
		/*NodeFact* n0 = new NodeFact(s0);
		NodeFact* n1 = new NodeFact(s1);*/
		//printf("                      nodefact=<%p | %p>\n", n0, n1);
		state.addFact(this, 0, s0);
		state.addFact(this, 1, s1);
	}
};

class checkAllUnstructured : public UnstructuredPassIntraAnalysis
{
	public:
	saveAllUnstructured* creator;
		
	checkAllUnstructured(saveAllUnstructured* creator)
	{
		this->creator = creator;
	}
	
	void visit(const Function& func, const DataflowNode& n, NodeState& state)
	{
		stringWrapper* s0 = (stringWrapper*)state.getFact(creator, 0);
		stringWrapper* s1 = (stringWrapper*)state.getFact(creator, 1);
		//printf("checkAllUnstructured: node=<%s | %s>\n", n.getNode()->class_name().c_str(), n.getNode()->unparseToString().c_str());
		//printf("                     fact=<%p | %p>\n", s0, s1);
		//printf("                     fact=<%s | %s>\n", s0->myStr.c_str(), s1->myStr.c_str());
		
		if(n.getNode()->class_name() != s0->myStr)
		{
			printf("ERROR in checkAllUnstructured: Expected class name \"%s\" but the saved class name is \"%s\"\n", n.getNode()->class_name().c_str(), s0->myStr.c_str());
			numFails++;
		}
		
		if(n.getNode()->unparseToString() != s1->myStr)
		{
			printf("ERROR in checkAllUnstructured: Expected class name \"%s\" but the saved class name is \"%s\"\n", n.getNode()->unparseToString().c_str(), s0->myStr.c_str());
			numFails++;
		}
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

	saveAllUnstructured saau;
	UnstructuredPassInterAnalysis upia_saau(saau);
	upia_saau.runAnalysis();
	
	checkAllUnstructured chau(&saau);
	UnstructuredPassInterAnalysis upia_shau(chau);
	upia_shau.runAnalysis();
	
	if(numFails==0)
		printf("PASS\n");
	else
		printf("FAIL!\n");
	
	printf("==========  E  N  D  ==========\n");
	
	// Unparse and compile the project (so this can be used for testing)
	return backend(project);
}
