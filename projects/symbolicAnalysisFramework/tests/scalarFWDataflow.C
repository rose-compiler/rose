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
#include "divAnalysis.h"
#include "sgnAnalysis.h"
#include "nodeConstAnalysis.h"
#include "affineInequality.h"
#include "scalarFWDataflow.h"
#include "saveDotAnalysis.h"

int numFails=0;

int main( int argc, char * argv[] ) 
{
	printf("========== S T A R T ==========\n");
	
	// Build the AST used by ROSE
	SgProject* project = frontend(argc,argv);
	
	//cfgUtils::initCFGUtils(project);

	initAnalysis(project);
	
	analysisDebugLevel = 0;
	
	SaveDotAnalysis sda;
	UnstructuredPassInterAnalysis upia_sda(sda);
	upia_sda.runAnalysis();
	
	/*analysisDebugLevel = 1;
	printf("***********************************************************\n");
	printf("*****************   Node Const Analysis   *****************\n");
	printf("***********************************************************\n");
	nodeConstAnalysis* nca = runNodeConstAnalysis();
	printNodeConstAnalysisStates(nca, "<");*/
	
	//analysisDebugLevel = 2;
	printf("*************************************************************\n");
	printf("*****************   Divisibility Analysis   *****************\n");
	printf("*************************************************************\n");
	DivAnalysis da;
	CallGraphBuilder cgb(project);
	cgb.buildCallGraph();
	//SgIncidenceDirectedGraph* graph = cgb.getGraph(); 
	//ContextInsensitiveInterProceduralDataflow ciipd_da(&da, graph);
	UnstructuredPassInterDataflow ciipd_da(&da);
	ciipd_da.runAnalysis();
	
	printDivAnalysisStates(&da, "[");
	
	analysisDebugLevel = 0;
	
	printf("*************************************************************\n");
	printf("*****************   Sign Analysis   *****************\n");
	printf("*************************************************************\n");
	SgnAnalysis sa;
	//ContextInsensitiveInterProceduralDataflow ciipd_sa(&sa, graph);
	UnstructuredPassInterDataflow ciipd_sa(&sa);
	ciipd_sa.runAnalysis();
	
	printSgnAnalysisStates(&sa, "|");
	
	analysisDebugLevel = 1;
	
	printf("*************************************************************\n");
	printf("******************   Affine Inequalities   ******************\n");
	printf("*************************************************************\n");
	//initAffineIneqs(project);
	affineInequalitiesPlacer aip;
	UnstructuredPassInterAnalysis upia_aip(aip);
	upia_aip.runAnalysis();
	
	/*printAffineInequalities pai(&aip);
	UnstructuredPassInterAnalysis upia_pai(pai);
	upia_pai.runAnalysis();*/
	printAffineInequalities(&aip, "(");
	
	printf("***************************************************************\n");
	printf("***************** Scalar FW Dataflow Analysis *****************\n");
	printf("***************************************************************\n");
	ScalarFWDataflow sfwd(&da, &sa, &aip);
	UnstructuredPassInterDataflow upid_sfwd(&sfwd);
	upid_sfwd.runAnalysis();
	
	vector<int> factNames;
	vector<int> latticeNames;
	latticeNames.push_back(0);
	printAnalysisStates pas(&sfwd, factNames, latticeNames, ":");
	UnstructuredPassInterAnalysis upia_pas(pas);
	upia_pas.runAnalysis();

	
	/*printf("*************************************************************\n");
	printf("*********** Checkpoint Range Verification Analysis **********\n");
	printf("*************************************************************\n");
	ChkptRangeVerifAnalysis crva(&cra);
	UnstructuredPassInterAnalysis upia_crva(crva);
	upia_crva.runAnalysis();*/

	if(numFails==0)
		printf("PASS\n");
	else
		printf("FAIL!\n");
	
	printf("==========  E  N  D  ==========\n");
	
	// Unparse and compile the project (so this can be used for testing)
	return backend(project);
}

/*******************************************************************************************
 *******************************************************************************************
 *******************************************************************************************/

map<varID, Lattice*> ScalarFWDataflow::constVars;

// generates the initial lattice state for the given dataflow node, in the given function, with the given NodeState
//vector<Lattice*> ScalarFWDataflow::genInitState(const Function& func, const DataflowNode& n, const NodeState& state)
void ScalarFWDataflow::genInitState(const Function& func, const DataflowNode& n, const NodeState& state,
                                  vector<Lattice*>& initLattices, vector<NodeFact*>& initFacts)
{
	//vector<Lattice*> initLattices;
/*printf("ScalarFWDataflow::genInitState() n=%p<%s | %s>\n", n.getNode(), n.getNode()->class_name().c_str(), n.getNode()->unparseToString().c_str());
printf("ScalarFWDataflow::genInitState() state=%p\n", &state);*/
	
	// create a constraint graph from the divisiblity and sign information at this CFG node
	FiniteVariablesProductLattice* divProdL = dynamic_cast<FiniteVariablesProductLattice*>(state.getLatticeBelow(divAnalysis, 0));
	FiniteVariablesProductLattice* sgnProdL = dynamic_cast<FiniteVariablesProductLattice*>(state.getLatticeBelow(sgnAnalysis, 0));
	initLattices.push_back(new ConstrGraph(func, divProdL, sgnProdL, false));
	//return initState;
}

// Returns a map of special constant variables (such as zeroVar) and the lattices that correspond to them
// These lattices are assumed to be constants: it is assumed that they are never modified and it is legal to 
//    maintain only one copy of each lattice may for the duration of the analysis.
map<varID, Lattice*>& ScalarFWDataflow::genConstVarLattices() const
{
	return constVars;
}
	
bool ScalarFWDataflow::transfer(const Function& func, const DataflowNode& n, NodeState& state, const vector<Lattice*>& dfInfo)
{
	printf("-----------------------------------\n");
	printf("ScalarFWDataflow::transfer() function %s() node=<%s | %s>\n", func.get_name().str(), n.getNode()->class_name().c_str(), n.getNode()->unparseToString().c_str());
	
	bool modified = false;
	ConstrGraph* cg = dynamic_cast<ConstrGraph*>(dfInfo.front());
	
	cg->beginTransaction();
	
	// upgrade cg to bottom if it is currently uninitialized
	cg->setToTop(true);
	
	printf("ScalarFWDataflow::transfer()   isSgExpression(n.getNode())=%p\n", isSgExpression(n.getNode()));
	if(isSgExpression(n.getNode()))
	{
		short op;
		varID i, j, k;
		bool negJ, negK;
		long c;
		if(cfgUtils::parseAssignment(isSgExpression(n.getNode()), op, i, j, negJ, k, negK, c))
		{
			printf("ScalarFWDataflow::transfer()   cfgUtils::parseAssignment(%p, %d, %s, %s, %d, %s, %d, %ld)\n", isSgExpression(n.getNode()), op, i.str().c_str(), j.str().c_str(), negJ, k.str().c_str(), negK, c);
			// i = j or i = c
			if(op == cfgUtils::none)
			{
				modified = cg->assign(i, j, 1, 1, c) || modified;
			}
			// i = (-)j+(-)k+c
			else if(op == cfgUtils::add)
			{
				printf("ScalarFWDataflow::transfer() op == cfgUtils::add\n");
				// i = j + c
				if(k == zeroVar && negJ==false)
				{
				printf("ScalarFWDataflow::transfer() k == zeroVar && negJ==false\n");
					modified = cg->assign(i, j, 1, 1, c) || modified;
				}
			}
			// i = (-)j*(-)k*c
			else if(op == cfgUtils::mult)
			{
				printf("ScalarFWDataflow::transfer() op == cfgUtils::mult\n");
				// i = j*b + 0
				if(k == oneVar && negJ==false)
				{
					printf("ScalarFWDataflow::transfer() k == zeroVar && negJ==false\n");
					modified = cg->assign(i, j, 1, c, 0) || modified;
				}
			}
			// i = (-)j/(-)k/c
			else if(op == cfgUtils::divide)
			{
				printf("ScalarFWDataflow::transfer() op == cfgUtils::divide\n");
				// i = j/c == i*c = j
				if(k == oneVar && negJ==false)
				{
					modified = cg->assign(i, j, c, 1, 0) || modified;
				}
			}
		}
	}
	
cout << "mid-Transfer Function:\n";
cout << cg->str("    ") << "\n";
	
	// incorporate this node's inequalities from conditionals
	incorporateConditionalsInfo(func, n, state, dfInfo);
	
cout << "mid2-Transfer Function:\n";
cout << cg->str("    ") << "\n";
	
	// incorporate this node's divisibility information
	incorporateDivInfo(func, n, state, dfInfo);

cout << "late-Transfer Function:\n";
cout << cg->str("    ") << "\n";
	
	cg->endTransaction();
	
	//cg->beginTransaction();
	removeConstrDivVars(func, n, state, dfInfo);
	cg->divVarsClosure();
	//cg->endTransaction();
	
	return modified;
}

// Incorporates the current node's inequality information from conditionals (ifs, fors, etc.) into the current node's 
// constraint graph.
// returns true if this causes the constraint graph to change and false otherwise
bool ScalarFWDataflow::incorporateConditionalsInfo(const Function& func, const DataflowNode& n, 
                                                   NodeState& state, const vector<Lattice*>& dfInfo)
{
	bool modified = false;
	ConstrGraph* cg = dynamic_cast<ConstrGraph*>(dfInfo.front());
	
	printf("incorporateConditionalsInfo()\n");
	affineInequalityFact* ineqFact = (affineInequalityFact*)state.getFact(affIneqPlacer, 0);
	if(ineqFact)
	{
		//cout << "Node <"<<n.getNode()->class_name()<<" | "<<n.getNode()->unparseToString()<<">\n";
		for(set<varAffineInequality>::iterator it = ineqFact->ineqs.begin(); it!=ineqFact->ineqs.end(); it++)
		{
			varAffineInequality varIneq = *it;
			//cout << varIneq.getIneq().str(varIneq.getX(), varIneq.getY(), "    ") << "\n";
			modified = cg->setVal(varIneq.getX(), varIneq.getY(), varIneq.getIneq().getA(), 
			                      varIneq.getIneq().getB(), varIneq.getIneq().getC()) || modified;
		}
	}
	return modified;
}

// incorporates the current node's divisibility information into the current node's constraint graph
// returns true if this causes the constraint graph to change and false otherwise
bool ScalarFWDataflow::incorporateDivInfo(const Function& func, const DataflowNode& n, NodeState& state, const vector<Lattice*>& dfInfo)
{
	bool modified = false;
	ConstrGraph* cg = dynamic_cast<ConstrGraph*>(dfInfo.front());
	
	printf("incorporateDivInfo()\n");
	FiniteVariablesProductLattice* prodL = dynamic_cast<FiniteVariablesProductLattice*>(state.getLatticeBelow(divAnalysis, 0));
	varIDSet visVars = prodL->getVisibleVars(func);
	for(varIDSet::iterator it = visVars.begin(); it!=visVars.end(); it++)
	{
		varID var = *it;
		// Re-add the connection between var and its divisibility variable. The divisibility variable
		// needs to be disconnected from the other variables so that we can re-compute its new relationships
		// solely based on its relationship with var. If we didn't do this, and var's relationship to its
		// divisibility variable changed from one CFG node to the next (i.e. from var=divvar to var = b*divvar),
		// we would not be able to capture this change.
		cg->addDivVar(var, true);
		
		/* // create the divisibility variable for the current variable 
		varID divVar = ConstrGraph::getDivScalar(var);*/
		
		/*DivLattice* varDivL = dynamic_cast<DivLattice*>(prodL->getVarLattice(func, var));
		
		// incorporate this variable's divisibility information (if any)
		if(varDivL->getLevel() == DivLattice::divKnown && !(varDivL->getDiv()==1 && varDivL->getRem()==0))
		{
			modified = cg->addDivVar(var, varDivL->getDiv(), varDivL->getRem()) || modified;
		}
		else if(varDivL->getLevel() != DivLattice::bottom)
		{
			modified = cg->addDivVar(var, 1, 0) || modified;
		}*/
	}
	
	return modified;
}

// For any variable for which we have divisibility info, remove its constraints to other variables (other than its
// divisibility variable)
bool ScalarFWDataflow::removeConstrDivVars(const Function& func, const DataflowNode& n, NodeState& state, const vector<Lattice*>& dfInfo)
{
	bool modified = false;
	ConstrGraph* cg = dynamic_cast<ConstrGraph*>(dfInfo.front());
	
	printf("removeConstrDivVars()\n");
	
	FiniteVariablesProductLattice* prodL = dynamic_cast<FiniteVariablesProductLattice*>(state.getLatticeBelow(divAnalysis, 0));
	varIDSet visVars = prodL->getVisibleVars(func);
	for(varIDSet::iterator it = visVars.begin(); it!=visVars.end(); it++)
	{
		varID var = *it;
		cg->disconnectDivOrigVar(var);
		/*DivLattice* varDivL = dynamic_cast<DivLattice*>(prodL->getVarLattice(func, var));
		
		// incorporate this variable's divisibility information (if any)
		if(varDivL->getLevel() == DivLattice::divKnown && !(varDivL->getDiv()==1 && varDivL->getRem()==0))
		{
			cg->disconnectDivOrigVar(var, varDivL->getDiv(), varDivL->getRem());
		}
		else if(varDivL->getLevel() != DivLattice::bottom)
		{
			cg->disconnectDivOrigVar(var, 1, 0);
		}*/
	}
	
	cout << cg->str("    ") << "\n";
	
	return modified;
}

