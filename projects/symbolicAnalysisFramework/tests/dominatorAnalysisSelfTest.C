#include "rose.h"
#include <list>
#include <sstream>
#include <iostream>
#include <fstream>
#include <string.h>
using namespace std;

#include "common.h"
#include "variables.h"
#include "cfgUtils.h"
#include "analysisCommon.h"
#include "functionState.h"
#include "latticeFull.h"
#include "analysis.h"
#include "dataflow.h"
#include "dominatorAnalysis.h"
#include "saveDotAnalysis.h"

int numFails=0;

/*class evaluateAnalysisStates : public UnstructuredPassIntraAnalysis
{
	public:
	LiveDeadVarsAnalysis* da;
	string indent;
	evaluateAnalysisStates(LiveDeadVarsAnalysis* da, string indent) : da(da), indent(indent)
	{}
	
	void visit(const Function& func, const DataflowNode& n, NodeState& state)
	{
		if(isSgFunctionCallExp(n.getNode()) && isSgFunctionCallExp(n.getNode())->getAssociatedFunctionSymbol()) {
			string funcName = isSgFunctionCallExp(n.getNode())->getAssociatedFunctionSymbol()->get_name().getString();
			if(funcName.find("testFunc")!=string::npos) {
				if(liveDeadAnalysisDebugLevel>=1) {
					set<varID> liveVars = getAllLiveVarsAt(da, n, state, "    ");
					cout << funcName<<"(): liveVars=";
					for(set<varID>::iterator v=liveVars.begin(); v!=liveVars.end(); v++)
						cout << *v <<", ";
					cout << "\n";
				}
				
				varID funcCallVar = SgExpr2Var(isSgFunctionCallExp(n.getNode()));
				if(funcName == "testFunc0") {
					set<varID>::iterator v = liveVars.begin();
					int i=0;
					if(funcCallVar!=*v) { cout << indent << "ERROR at "<<funcName<<"(): Live Var "<<i<<" should be \""<<funcCallVar<<"\"!\n"; numFails++; cout.flush(); return;  } v++; i++;
					if(v!=liveVars.end()) { numFails++;  cout << indent << "ERROR at "<<funcName<<"(): Too many live variables : #"<<i<<"!"; }
				} else if(funcName == "testFunc1") {
					set<varID>::iterator v = liveVars.begin();
					int i=0;
					if(funcCallVar!=*v) { cout << indent << "ERROR at "<<funcName<<"(): Live Var "<<i<<" should be \""<<funcCallVar<<"\"!\n"; numFails++; cout.flush(); return;  } v++; i++;
					if((*v).str()!="a") { cout << indent << "ERROR at "<<funcName<<"(): Live Var "<<i<<" should be \"a\"!\n"; numFails++; cout.flush(); return;  } v++; i++;
					if((*v).str()!="b") { cout << indent << "ERROR at "<<funcName<<"(): Live Var "<<i<<" should be \"b\"!\n"; numFails++; cout.flush(); return;  } v++; i++;
					if((*v).str()!="c") { cout << indent << "ERROR at "<<funcName<<"(): Live Var "<<i<<" should be \"c\"!\n"; numFails++; cout.flush(); return;  } v++; i++;
					if(v!=liveVars.end()) { numFails++;  cout << indent << "ERROR at "<<funcName<<"(): Too many live variables : #"<<i<<"!"; }
				} else if(funcName == "testFunc2") {
					set<varID>::iterator v = liveVars.begin();
					int i=0;
					if(funcCallVar!=*v) { cout << indent << "ERROR at "<<funcName<<"(): Live Var "<<i<<" should be \""<<funcCallVar<<"\"!\n"; numFails++; cout.flush(); return;  } v++; i++;
					if((*v).str()!="a") { cout << indent << "ERROR at "<<funcName<<"(): Live Var "<<i<<" should be \"a\"!\n"; numFails++; cout.flush(); return;  } v++; i++;
					if((*v).str()!="b") { cout << indent << "ERROR at "<<funcName<<"(): Live Var "<<i<<" should be \"b\"!\n"; numFails++; cout.flush(); return;  } v++; i++;
					if((*v).str()!="c") { cout << indent << "ERROR at "<<funcName<<"(): Live Var "<<i<<" should be \"c\"!\n"; numFails++; cout.flush(); return;  } v++; i++;
					if((*v).str()!="d") { cout << indent << "ERROR at "<<funcName<<"(): Live Var "<<i<<" should be \"d\"!\n"; numFails++; cout.flush(); return;  } v++; i++;
					if((*v).str()!="e") { cout << indent << "ERROR at "<<funcName<<"(): Live Var "<<i<<" should be \"e\"!\n"; numFails++; cout.flush(); return;  } v++; i++;
					if(v!=liveVars.end()) { numFails++;  cout << indent << "ERROR at "<<funcName<<"(): Too many live variables : #"<<i<<"!"; }
				} else if(funcName == "testFunc3") {
					set<varID>::iterator v = liveVars.begin();
					int i=0;
					if(funcCallVar!=*v) { cout << indent << "ERROR at "<<funcName<<"(): Live Var "<<i<<" should be \""<<funcCallVar<<"\"!\n"; numFails++; cout.flush(); return;  } v++; i++;
					if((*v).str()!="c") { cout << indent << "ERROR at "<<funcName<<"(): Live Var "<<i<<" should be \"c\"!\n"; numFails++; cout.flush(); return;  } v++; i++;
					if((*v).str()!="d") { cout << indent << "ERROR at "<<funcName<<"(): Live Var "<<i<<" should be \"d\"!\n"; numFails++; cout.flush(); return;  } v++; i++;
					if(v!=liveVars.end()) { numFails++;  cout << indent << "ERROR at "<<funcName<<"(): Too many live variables : #"<<i<<"!"; }
				} else if(funcName == "testFunc4") {
					set<varID>::iterator v = liveVars.begin();
					int i=0;
					if(funcCallVar!=*v) { cout << indent << "ERROR at "<<funcName<<"(): Live Var "<<i<<" should be \""<<funcCallVar<<"\"!\n"; numFails++; cout.flush(); return;  } v++; i++;
					if((*v).str()!="c") { cout << indent << "ERROR at "<<funcName<<"(): Live Var "<<i<<" should be \"c\"!\n"; numFails++; cout.flush(); return;  } v++; i++;
					if((*v).str()!="d") { cout << indent << "ERROR at "<<funcName<<"(): Live Var "<<i<<" should be \"d\"!\n"; numFails++; cout.flush(); return;  } v++; i++;
					if(v!=liveVars.end()) { numFails++;  cout << indent << "ERROR at "<<funcName<<"(): Too many live variables : #"<<i<<"!"; }
				} else if(funcName == "testFunc5") {
					set<varID>::iterator v = liveVars.begin();
					int i=0;
					if(funcCallVar!=*v) { cout << indent << "ERROR at "<<funcName<<"(): Live Var "<<i<<" should be \""<<funcCallVar<<"\"!\n"; numFails++; cout.flush(); return;  } v++; i++;
					if((*v).str()!="c") { cout << indent << "ERROR at "<<funcName<<"(): Live Var "<<i<<" should be \"c\"!\n"; numFails++; cout.flush(); return;  } v++; i++;
					if((*v).str()!="d") { cout << indent << "ERROR at "<<funcName<<"(): Live Var "<<i<<" should be \"d\"!\n"; numFails++; cout.flush(); return;  } v++; i++;
					if((*v).str()!="arr") { cout << indent << "ERROR at "<<funcName<<"(): Live Var "<<i<<" should be \"arr\"!\n"; numFails++; cout.flush(); return;  } v++; i++;
					if(v!=liveVars.end()) { numFails++;  cout << indent << "ERROR at "<<funcName<<"(): Too many live variables : #"<<i<<"!"; }
				} else if(funcName == "testFunc6") {
					set<varID>::iterator v = liveVars.begin();
					int i=0;
					if(funcCallVar!=*v) { cout << indent << "ERROR at "<<funcName<<"(): Live Var "<<i<<" should be \""<<funcCallVar<<"\"!\n"; numFails++; cout.flush(); return;  } v++; i++;
					if((*v).str()!="c") { cout << indent << "ERROR at "<<funcName<<"(): Live Var "<<i<<" should be \"c\"!\n"; numFails++; cout.flush(); return;  } v++; i++;
					if((*v).str()!="d") { cout << indent << "ERROR at "<<funcName<<"(): Live Var "<<i<<" should be \"d\"!\n"; numFails++; cout.flush(); return;  } v++; i++;
					if(v!=liveVars.end()) { numFails++;  cout << indent << "ERROR at "<<funcName<<"(): Too many live variables : #"<<i<<"!"; }
				} else if(funcName == "testFunc7") {
					set<varID>::iterator v = liveVars.begin();
					int i=0;
					if(funcCallVar!=*v) { cout << indent << "ERROR at "<<funcName<<"(): Live Var "<<i<<" should be \""<<funcCallVar<<"\"!\n"; numFails++; cout.flush(); return;  } v++; i++;
					if(v!=liveVars.end()) { numFails++;  cout << indent << "ERROR at "<<funcName<<"(): Too many live variables : #"<<i<<"!"; }
				}
			}
		}
	}
};*/

int main( int argc, char * argv[] ) 
{
	printf("========== S T A R T ==========\n");
	
	// Build the AST used by ROSE
	SgProject* project = frontend(argc,argv);
	
	initAnalysis(project);
	
	/*analysisDebugLevel = 0;
	
	SaveDotAnalysis sda;
	UnstructuredPassInterAnalysis upia_sda(sda);
	upia_sda.runAnalysis();*/
	
	liveDeadAnalysisDebugLevel = 0;
	analysisDebugLevel = 0;
	if(liveDeadAnalysisDebugLevel) {
		printf("*************************************************************\n");
		printf("*****************   Live/Dead Variable Analysis   *****************\n");
		printf("*************************************************************\n");
	}
	DominatorAnalysis da(project);
	//CallGraphBuilder cgb(project);
	//cgb.buildCallGraph();
	//SgIncidenceDirectedGraph* graph = cgb.getGraph(); 
	//ContextInsensitiveInterProceduralDataflow ciipd_da(&da, graph);
	UnstructuredPassInterDataflow ciipd_da(&da);
	ciipd_da.runAnalysis();
	
	printDominatorAnalysisStates(&da, "[");

	/*evaluateAnalysisStates eas(&da, "    ");
	UnstructuredPassInterAnalysis upia_eas(eas);
	upia_eas.runAnalysis();*/

	if(numFails==0)
		printf("PASS\n");
	else
		printf("FAIL!\n");
	
	printf("==========  E  N  D  ==========\n");
	
	// Unparse and compile the project (so this can be used for testing)
	return backend(project);
}
