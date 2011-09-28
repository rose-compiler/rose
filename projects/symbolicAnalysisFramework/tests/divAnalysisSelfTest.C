#include "rose.h"
#include <list>
#include <sstream>
#include <iostream>
#include <fstream>
#include <string.h>
#include <map>
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
#include "divAnalysis.h"

int numFails = 0, numPass = 0;

class evaluateAnalysisStates : public UnstructuredPassIntraAnalysis
{
public:
  DivAnalysis* div;
  string indent;
  map<string, map<SgName, DivLattice> > expectations;
  int total_expectations;
  
  evaluateAnalysisStates(DivAnalysis* div_, string indent_) : div(div_), indent(indent_), total_expectations(0)
  {
    expectations["testFunc1"]["a"] = DivLattice(1);
    expectations["testFunc1"]["b"] = DivLattice(4);
    expectations["testFunc1"]["c"] = DivLattice(5);
    expectations["testFunc1"]["d"] = DivLattice(4, 3);
    expectations["testFunc1"]["e"] = DivLattice(20, 15);
    expectations["testFunc1"]["f"] = DivLattice(20, 16);
    expectations["testFunc1"]["g"] = DivLattice(20, 0);


    for (map<string, map<SgName, DivLattice> >::iterator i = expectations.begin(); i != expectations.end(); ++i)
      total_expectations += i->second.size();

    cout << "Total expectations: " << total_expectations << endl;
  }
	
  void visit(const Function& func, const DataflowNode& n, NodeState& state)
  {
    SgFunctionCallExp *fnCall = isSgFunctionCallExp(n.getNode());
    if (!fnCall) return;
    if (!fnCall->getAssociatedFunctionSymbol()) return;

    string funcName = fnCall->getAssociatedFunctionSymbol()->get_name().getString();
    if(funcName.find("testFunc") == string::npos) return;

    FiniteVarsExprsProductLattice *lat = dynamic_cast<FiniteVarsExprsProductLattice *>(state.getLatticeAbove(div)[0]);
    cout << indent << "Lattice before call to " << funcName << ": " << lat->str() << endl;
              
    set<varID> allVars = lat->getAllVars();
    for (set<varID>::iterator i = allVars.begin(); i != allVars.end(); ++i) {
      string name = i->str();
      cout << "Variable " << name << " ";

      if (expectations[funcName].find(name) == expectations[funcName].end()) {
        cout << "unspecified" << endl;
        continue;
      }

      Lattice *got = lat->getVarLattice(*i);
      ROSE_ASSERT(got);
      if (expectations[funcName][name] != got) {
        cout << "mismatched: " << got->str() << " was not the expected " << expectations[funcName][name].str();
        numFails++;
      }
      else {
        cout << "matched";
        numPass++;
      }
      cout << endl;
    }
  }
};

int main( int argc, char * argv[] ) 
{
	printf("========== S T A R T ==========\n");
	
	// Build the AST used by ROSE
	SgProject* project = frontend(argc,argv);
	
	initAnalysis(project);
	Dbg::init("Divisibility Analysis Test", ".", "index.html");	
	
	/*analysisDebugLevel = 0;
	
	SaveDotAnalysis sda;
	UnstructuredPassInterAnalysis upia_sda(sda);
	upia_sda.runAnalysis();*/
	
	liveDeadAnalysisDebugLevel = 0;
	analysisDebugLevel = 0;
	if(liveDeadAnalysisDebugLevel) {
		printf("*************************************************************\n");
		printf("*****************   Divisibility Analysis   *****************\n");
		printf("*************************************************************\n");
	}
	LiveDeadVarsAnalysis ldva(project);
	UnstructuredPassInterDataflow ciipd_ldva(&ldva);
	ciipd_ldva.runAnalysis();
	
        CallGraphBuilder cgb(project);
        cgb.buildCallGraph();
        SgIncidenceDirectedGraph* graph = cgb.getGraph(); 

	analysisDebugLevel = 1;
        DivAnalysis divA(&ldva);
        ContextInsensitiveInterProceduralDataflow divInter(&divA, graph);
        divInter.runAnalysis();

	evaluateAnalysisStates eas(&divA, "    ");
	UnstructuredPassInterAnalysis upia_eas(eas);
	upia_eas.runAnalysis();

	if(numFails == 0 && numPass == eas.total_expectations)
          printf("PASS: %d / %d\n", numPass, eas.total_expectations);
	else
          printf("FAIL!: %d / %d\n", numPass, eas.total_expectations);
	
	printf("==========  E  N  D  ==========\n");
	
	// Unparse and compile the project (so this can be used for testing)
	return /*backend(project) +*/ (eas.total_expectations != numPass) || (numFails != 0);
}
