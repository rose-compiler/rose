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
#include "liveDeadVarAnalysis.h"
#include "saveDotAnalysis.h"

int numFails=0;

class evaluateAnalysisStates : public UnstructuredPassIntraAnalysis
{
	public:
	LiveDeadVarsAnalysis* ldva;
	string indent;
	evaluateAnalysisStates(LiveDeadVarsAnalysis* ldva, string indent) : ldva(ldva), indent(indent)
	{}
	
	void visit(const Function& func, const DataflowNode& n, NodeState& state)
	{
		if(isSgFunctionCallExp(n.getNode()) && isSgFunctionCallExp(n.getNode())->getAssociatedFunctionSymbol()) {
			string funcName = isSgFunctionCallExp(n.getNode())->getAssociatedFunctionSymbol()->get_name().getString();
			if(funcName.find("testFunc")!=string::npos) {
				set<varID> liveVars = getAllLiveVarsAt(ldva, n, state, "    ");
				if(liveDeadAnalysisDebugLevel>=1) {
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
};

int main( int argc, char * argv[] ) 
{
	printf("========== S T A R T ==========\n");
	
	// Build the AST used by ROSE
	SgProject* project = frontend(argc,argv);
	
	initAnalysis(project);
	Dbg::init("Communication Topology Analysis Test", ".", "index.html");	
	
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
	LiveDeadVarsAnalysis ldva(project);
	//CallGraphBuilder cgb(project);
	//cgb.buildCallGraph();
	//SgIncidenceDirectedGraph* graph = cgb.getGraph(); 
	//ContextInsensitiveInterProceduralDataflow ciipd_da(&ldva, graph);
	UnstructuredPassInterDataflow ciipd_ldva(&ldva);
	ciipd_ldva.runAnalysis();
	
	// printLiveDeadVarsAnalysisStates(&ldva, "[");

	evaluateAnalysisStates eas(&ldva, "    ");
	UnstructuredPassInterAnalysis upia_eas(eas);
	upia_eas.runAnalysis();

	if(numFails==0)
		printf("PASS\n");
	else
		printf("FAIL!\n");
	
	printf("==========  E  N  D  ==========\n");
	
	// Unparse and compile the project (so this can be used for testing)
	return backend(project);
}
