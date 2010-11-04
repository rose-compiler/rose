/*
 * Generic path-sensitive analysis that uses the PartitionedAnalysis framework to create a 
 * separate intra-procedural analysis for every if statement in the function
 */

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
#include "intArithLogical.h"
#include "saveDotAnalysis.h"
#include "partitionedAnalysis.h"
#include "pathSensitive.h"

int numFails=0;

int main( int argc, char * argv[] ) 
{
	printf("========== S T A R T ==========\n");
	
	// Build the AST used by ROSE
	SgProject* project = frontend(argc,argv);
	
	cfgUtils::initCFGUtils(project);

	initAnalysis(project);
	
	analysisDebugLevel = 0;
	
	/*SaveDotAnalysis sda;
	UnstructuredPassInterAnalysis upia_sda(sda);
	upia_sda.runAnalysis();
	
	CallGraphBuilder cgb(project);
	cgb.buildCallGraph();
	SgIncidenceDirectedGraph* graph = cgb.getGraph(); 
	*/
	analysisDebugLevel = 0;
	
	runIfMeetDetector(false);
	
	analysisDebugLevel = 0;
	printf("*************************************************************\n");
	printf("*****************   Divisibility Analysis   *****************\n");
	printf("*************************************************************\n");
	DivAnalysis da;
	//ContextInsensitiveInterProceduralDataflow ciipd_da(&da, graph);
	UnstructuredPassInterDataflow ciipd_da(&da);
	ciipd_da.runAnalysis();
	
	//printDivAnalysisStates(&da, "[");
	
	analysisDebugLevel = 0;
	
	printf("*************************************************************\n");
	printf("*****************   Sign Analysis   *****************\n");
	printf("*************************************************************\n");
	SgnAnalysis sa;
	//ContextInsensitiveInterProceduralDataflow ciipd_sa(&sa, graph);
	UnstructuredPassInterDataflow ciipd_sa(&sa);
	ciipd_sa.runAnalysis();
	
	//printSgnAnalysisStates(&sa, "|");
	
	analysisDebugLevel = 0;
	
	printf("*************************************************************\n");
	printf("******************   Affine Inequalities   ******************\n");
	printf("*************************************************************\n");
	runAffineIneqPlacer(true);
	
	/*analysisDebugLevel = 1;
	
	printf("*************************************************************\n");
	printf("**********   Integer-Arithmetic-Logical Expressions *********\n");
	printf("*************************************************************\n");
	runIntArithLogicalPlacer(true);*/
	
	analysisDebugLevel = 1;
	
	printf("***************************************************************\n");
	printf("***************** Scalar FW Dataflow Analysis *****************\n");
	printf("***************************************************************\n");
	scalarFWDataflowPartitionedAnalysis sfwdpa(&da, &sa);
	UnstructuredPassInterAnalysis upia_sfwdpa(sfwdpa);
	upia_sfwdpa.runAnalysis();
	
	printf("*********************************************************\n");
	printf("***************** Final Analysis States *****************\n");
	printf("*********************************************************\n");
	
	vector<int> factNames;
	vector<int> latticeNames;
	latticeNames.push_back(0);
	printf("Master Analysis = %p\n", sfwdpa.getMasterDFAnalysis());
	printAnalysisStates pas(sfwdpa.getMasterDFAnalysis(), factNames, latticeNames, ":");
	UnstructuredPassInterAnalysis upia_pas(pas);
	upia_pas.runAnalysis();

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

/******************
 * IfMeetLat *
 ******************/
IfMeetLat::IfMeetLat(DataflowNode parentNode): ifLeft(parentNode), ifRight(parentNode), parentNode(parentNode)
{
	initialized = false;
	isIfMeet = false;
}

IfMeetLat::IfMeetLat(const IfMeetLat &that): ifLeft(that.parentNode), ifRight(that.parentNode), parentNode(that.parentNode)
{ 
	this->initialized = that.initialized;
	this->ifHist      = that.ifHist;
	this->isIfMeet    = that.isIfMeet;
}

// initializes this Lattice to its default state
void IfMeetLat::initialize()
{
	initialized = true;
	ifHist.clear();
	isIfMeet = false;
	ifLeft   = parentNode;
	ifRight  = parentNode;
}

// returns a copy of this lattice
Lattice* IfMeetLat::copy() const
{ return new IfMeetLat(*this); }

// overwrites the state of this Lattice with that of that Lattice
void IfMeetLat::copy(Lattice* that)
{
	initialized = dynamic_cast<IfMeetLat*>(that)->initialized;
	ifHist      = dynamic_cast<IfMeetLat*>(that)->ifHist;
	isIfMeet    = dynamic_cast<IfMeetLat*>(that)->isIfMeet;
	ifLeft      = dynamic_cast<IfMeetLat*>(that)->ifLeft;
	ifRight     = dynamic_cast<IfMeetLat*>(that)->ifRight;
	parentNode  = dynamic_cast<IfMeetLat*>(that)->parentNode;
}

// computes the meet of this and that and saves the result in this
// returns true if this causes this to change and false otherwise
bool IfMeetLat::meetUpdate(Lattice* that)
{
	if(!initialized)
	{
		copy(that);
		isIfMeet = false;
		initialized=true;
		//printf("meetUpdate Case 1, result: %s\n", str("").c_str());
		return true;
	}
	else if(!dynamic_cast<IfMeetLat*>(that)->initialized)
	{
		initialized=true;
		//printf("meetUpdate Case 2, result: %s\n", str("").c_str());
		return false;
	}
	else 
	{
		initialized=true;
		//printf("ifHist.size()=%d (that)->ifHist.size()=%d\n", ifHist.size(), dynamic_cast<IfMeetLat*>(that)->ifHist.size());
		//ROSE_ASSERT(ifHist.size() == dynamic_cast<IfMeetLat*>(that)->ifHist.size());

		// Only bother once we have information from both incoming branches 
		if(ifHist.size() > 0 && dynamic_cast<IfMeetLat*>(that)->ifHist.size()>0)
		{
			//printf("meetUpdate Case 3, this: %s\n", str("").c_str());
			//printf("                   that: %s\n", dynamic_cast<IfMeetLat*>(that)->str("").c_str());
			
			// If the two lattices came from different sides of an if statement
			if(ifHist[ifHist.size()-1] != dynamic_cast<IfMeetLat*>(that)->ifHist[ifHist.size()-1])
			{
				// Record that this is a meet node for an if statement
				isIfMeet = true;
				// erase the marker for the just-finished if statement from this lattice
				ifHist.pop_back();
				//printf("meetUpdate Case 3a\n");
				return true;
			}
			//printf("meetUpdate Case 3b\n");
			return false;
		}
		// Else, if we have new information with a shorter ifHist list, cut the ifHist
		// list of this lattice down to match it since this information is fresher
		else if(dynamic_cast<IfMeetLat*>(that)->ifHist.size() < ifHist.size())
		{
			while(dynamic_cast<IfMeetLat*>(that)->ifHist.size() < ifHist.size())
				ifHist.erase(ifHist.end());
			//printf("after length cut ifHist.size()=%d (that)->ifHist.size()=%d\n", ifHist.size(), dynamic_cast<IfMeetLat*>(that)->ifHist.size());
		}	
		else
		{
			//printf("meetUpdate Case 4, result: %s\n", str("").c_str());
			return false;
		}
	}
}

bool IfMeetLat::operator==(Lattice* that)
{
	return initialized == dynamic_cast<IfMeetLat*>(that)->initialized && 
	       (ifHist == dynamic_cast<IfMeetLat*>(that)->ifHist) &&
	       (isIfMeet == dynamic_cast<IfMeetLat*>(that)->isIfMeet);
}

// The string that represents this object
// If indent!="", every line of this string must be prefixed by indent
// The last character of the returned string should not be '\n', even if it is a multi-line string.
string IfMeetLat::str(string indent)
{
	ostringstream ss;

	ss << indent << "IfMeetLat[initialized="<<(initialized?"true":"false")<<", isIfMeet="<<(isIfMeet?"true":"false")<<"\n";
	ss << indent << "          ifHist = <";
	for(vector<bool>::iterator it = ifHist.begin(); it!=ifHist.end(); )
	{
		ss << (*it?"true":"false");
		it++;
		if(it!=ifHist.end()) ss << ", ";
	}
	ss << ">]";
	return ss.str();
}

bool IfMeetLat::getIsIfMeet()
{
	return isIfMeet;
}

/******************
 * IfMeetDetector *
 ******************/

// generates the initial lattice state for the given dataflow node, in the given function, with the given NodeState
//vector<Lattice*> IfMeetDetector::genInitState(const Function& func, const DataflowNode& n, const NodeState& state)
void IfMeetDetector::genInitState(const Function& func, const DataflowNode& n, const NodeState& state,
                                  vector<Lattice*>& initLattices, vector<NodeFact*>& initFacts)
{
//		printf("IfMeetDetector::genInitState() A\n");
	//vector<Lattice*> initLattices;
//		printf("IfMeetDetector::genInitState() B\n");
	IfMeetLat* ifml = new IfMeetLat(n);
//		printf("IfMeetDetector::genInitState() C\n");
	initLattices.push_back(ifml);
//		printf("IfMeetDetector::genInitState() D\n");
	
	//return initLattices;
}
		
// the transfer function that is applied to every node
// n - the dataflow node that is being processed
// state - the NodeState object that describes the state of the node, as established by earlier 
//         analysis passes
// dfInfo - the Lattices that this transfer function operates on. The function takes these lattices
//          as input and overwrites them with the result of the transfer.
// Returns true if any of the input lattices changed as a result of the transfer function and
//    false otherwise.
bool IfMeetDetector::transfer(const Function& func, const DataflowNode& n, NodeState& state, const vector<Lattice*>& dfInfo)
{
	/*printf("-----------------------------------\n");
	printf("IfMeetDetector::transfer() function %s() node=<%s | %s>\n", func.get_name().str(), n.getNode()->class_name().c_str(), n.getNode()->unparseToString().c_str());*/
	
	//bool modified = false;
	IfMeetLat* ifml = dynamic_cast<IfMeetLat*>(dfInfo.front());
	
	// if this node is a descendant of an if statement
	if(n == ifml->ifLeft)
	{
		ifml->ifHist.push_back(0);
		// change ifLeft so that we won't push 0 again in case this transfer function ever gets called again
		ifml->ifLeft = ifml->parentNode;
	}
	if(n == ifml->ifRight)
	{
		ifml->ifHist.push_back(1);
		// change ifRight so that we won't push 1 again in case this transfer function ever gets called again
		ifml->ifRight = ifml->parentNode;
	}
		
	if(isSgIfStmt(n.getNode()) && n.getIndex()==1)
	{
		// iterate over both the descendants
		vector<DataflowEdge> edges = n.outEdges();
		for(vector<DataflowEdge>::iterator ei = edges.begin(); ei!=edges.end(); ei++)
		{
			// Set ifLeft and ifRight to tell each descendant that they're on the left/right branch of this if statement
			if((*ei).condition() == eckTrue)
			{
				ifml->ifLeft = ((*ei).target());
			}
			else if((*ei).condition() == eckFalse)
			{
				ifml->ifRight = ((*ei).target());
			}
		}
	}
	
	//printf("ifml->ifHist.size()=%d\n", ifml->ifHist.size());
	ifml->initialized = true;
	return true;
}

// prints the Lattices set by the given DivAnalysis 
void printIfMeetDetectorStates(IfMeetDetector* ifmd, string indent)
{
	vector<int> factNames;
	vector<int> latticeNames;
	latticeNames.push_back(0);
	printAnalysisStates pas(ifmd, factNames, latticeNames, indent);
	UnstructuredPassInterAnalysis upia_pas(pas);
	upia_pas.runAnalysis();
}

static IfMeetDetector* ifmd=NULL;
// Runs the IfMeetDetector analysis pass
void runIfMeetDetector(bool printStates)
{
	if(ifmd==NULL)
	{
		ifmd = new IfMeetDetector();	
		UnstructuredPassInterDataflow upid_ifmd(ifmd);
		upid_ifmd.runAnalysis();
		
		if(printStates)
			printIfMeetDetectorStates(ifmd, ":");
	}
}

// returns true if the given dataflow node is a meet point for an if statement and false otherwise
bool isIfMeetNode(const DataflowNode& n)
{
	NodeState* state = NodeState::getNodeState(n, 0);
	Lattice* ifmdLat = state->getLatticeBelow(ifmd, 0);
	return dynamic_cast<IfMeetLat*>(ifmdLat)->getIsIfMeet();
}

/******************************************************************************************
 *************                      scalarFWDataflowPart                      *************
 ******************************************************************************************/

map<varID, Lattice*> scalarFWDataflowPart::constVars;

// generates the initial lattice state for the given dataflow node, in the given function, with the given NodeState
//vector<Lattice*> scalarFWDataflowPart::genInitState(const Function& func, const DataflowNode& n, const NodeState& state)
void scalarFWDataflowPart::genInitState(const Function& func, const DataflowNode& n, const NodeState& state,
                                  vector<Lattice*>& initLattices, vector<NodeFact*>& initFacts)
{
	//vector<Lattice*> initLattices;
/*printf("scalarFWDataflowPart::genInitState() n=%p<%s | %s>\n", n.getNode(), n.getNode()->class_name().c_str(), n.getNode()->unparseToString().c_str());
printf("scalarFWDataflowPart::genInitState() state=%p\n", &state);*/
	
	// create a constraint graph from the divisiblity and sign information at this CFG node
	FiniteVariablesProductLattice* divProdL = dynamic_cast<FiniteVariablesProductLattice*>(state.getLatticeBelow(divAnalysis, 0));
	FiniteVariablesProductLattice* sgnProdL = dynamic_cast<FiniteVariablesProductLattice*>(state.getLatticeBelow(sgnAnalysis, 0));
	initLattices.push_back(new ConstrGraph(func, divProdL, sgnProdL, false));
	//initLattices.push_back(new IntArithLogical());
	//return initLattices;
}

// Returns a map of special constant variables (such as zeroVar) and the lattices that correspond to them
// These lattices are assumed to be constants: it is assumed that they are never modified and it is legal to 
//    maintain only one copy of each lattice may for the duration of the analysis.
map<varID, Lattice*>& scalarFWDataflowPart::genConstVarLattices() const
{
	return constVars;
}

// the transfer function that is applied to every node
// n - the dataflow node that is being processed
// state - the NodeState object that describes the state of the node, as established by earlier 
//         analysis passes
// dfInfo - the Lattices that this transfer function operates on. The function takes these lattices
//          as input and overwrites them with the result of the transfer.
// splitAnalysis - set by callee to 
//    - noSplit if the analysis does not want a split
//    - splitNew if the analysis wants to perform a split and place the newly-generated partitions into
//      a fresh split set that is nested inside this partition's current split
//    - splitParent if the analysis wants to perform a split and place the newly-generated partitions 
//      into this partition's current split (i.e. on the same split level as the current partition)
// splitConditions - if splitAnalysis==splitNew or ==splitParent, the analysis sets this vector to the conditions for all the 
//                   descendant CFG nodes in the split
// joinNode - set to true if progress along the given dataflow node needs to be blocked until the next join point.
//            If all paths of dataflow progress are blocked in this analysis, this is the same as the analysis 
//            requesting to be joined.
// Returns true if any of the input lattices changed as a result of the transfer function and
//    false otherwise.
bool scalarFWDataflowPart::transfer(const Function& func, const DataflowNode& n, NodeState& state, 
                                    const vector<Lattice*>& dfInfo, IntraPartitionFWDataflow::splitType& splitAnalysis, 
                                    vector</*LogicalCond**/printable*>& splitConditions, bool& joinNode)
{
	printf("    -----------------------------------\n");
	printf("    scalarFWDataflowPart::transfer() function %s() node=<%s | %s>\n", func.get_name().str(), n.getNode()->class_name().c_str(), n.getNode()->unparseToString().c_str());
	printf("    isIfMeetNode(n)=%d\n", isIfMeetNode(n));
	
	bool modified = false;
	ConstrGraph* cg = dynamic_cast<ConstrGraph*>(dfInfo.front());
	
	cg->beginTransaction();
	
	// Upgrade ial to bottom if it is currently uninitialized
	//printf("before: cg=%s\n", cg->str("").c_str());
	cg->initialize();
	printf("after: cg->mayTrue=%d cg=%s\n", cg->mayTrue(), cg->str().c_str());
	
	// Update ial with this partition's condition
	//ial->andUpd(*getIntArithLogical())
	
	// if the constraints are already impossible, don't bother adding any more
	//printf("    scalarFWDataflowPart::transfer()   isSgExpression(n.getNode())=%p\n", isSgExpression(n.getNode()));
	if(isSgIfStmt(n.getNode()) && n.getIndex()==1)
	{
		FiniteVariablesProductLattice* divProdL = dynamic_cast<FiniteVariablesProductLattice*>(state.getLatticeBelow(divAnalysis, 0));
		FiniteVariablesProductLattice* sgnProdL = dynamic_cast<FiniteVariablesProductLattice*>(state.getLatticeBelow(sgnAnalysis, 0));
		
		// This analysis splits over if statements, with one sub-analysis following each branch
		splitAnalysis=IntraPartitionFWDataflow::splitNew;
		// Iterate over both the descendants, adding their logical conditions to splitConditions
		vector<DataflowEdge> edges = n.outEdges();
		int i;
		for(vector<DataflowEdge>::iterator ei = edges.begin(); ei!=edges.end(); ei++, i++)
		{
			const set<varAffineInequality>& ineqs = getAffineIneq((*ei).target());
			ConstrGraph* expr = new ConstrGraph(ineqs, func, divProdL, sgnProdL);

			//cout << "expr = "<<expr->str("")<<"\n";
			splitConditions.push_back((/*LogicalCond**/printable*)expr);
		}
	}
	// if this is a meet node for an if statement, join the split created at the if statement
	else if(isIfMeetNode(n))
	{
		joinNode = true;
		return false;
	}
	else if(cg->mayTrue())
	{
		// Regular Nodes
		if(cfgUtils::isAssignment(n.getNode()))
		{
			short op;
			varID i, j, k;
			bool negJ, negK;
			long c;
			
			if(cfgUtils::parseAssignment(n.getNode(), op, i, j, negJ, k, negK, c))
			{
				printf("    scalarFWDataflowPart::transfer()   cfgUtils::parseAssignment(%p, %d, %s, %s, %d, %s, %d, %ld)\n", isSgExpression(n.getNode()), op, i.str().c_str(), j.str().c_str(), negJ, k.str().c_str(), negK, c);
				// i = j or i = c
				if(op == cfgUtils::none)
				{
					modified = cg->assign(i, j, 1, 1, c) || modified;
				}
				// i = (-)j+(-)k+c
				else if(op == cfgUtils::add)
				{
					printf("    scalarFWDataflowPart::transfer() op == cfgUtils::add\n");
					// i = j + c
					if(k == zeroVar && negJ==false)
					{
						printf("    scalarFWDataflowPart::transfer() k == zeroVar && negJ==false\n");
						modified = cg->assign(i, j, 1, 1, c) || modified;
					}
				}
				// i = (-)j*(-)k*c
				else if(op == cfgUtils::mult)
				{
					printf("    scalarFWDataflowPart::transfer() op == cfgUtils::mult\n");
					// i = j*b + 0
					if(k == oneVar && negJ==false)
					{
						printf("    scalarFWDataflowPart::transfer() k == zeroVar && negJ==false\n");
						modified = cg->assign(i, j, 1, c, 0) || modified;
					}
				}
				// i = (-)j/(-)k/c
				else if(op == cfgUtils::divide)
				{
					printf("    scalarFWDataflowPart::transfer() op == cfgUtils::divide\n");
					// i = j/c == i*c = j
					if(k == oneVar && negJ==false)
					{
						modified = cg->assign(i, j, c, 1, 0) || modified;
					}
				}
			}
		}
		
			
		/*cout << "mid-Transfer Function:\n";
		cout << cg->str("    ") << "\n";*/
		
		// incorporate this node's inequalities from conditionals
		incorporateConditionalsInfo(func, n, state, dfInfo);
		
	/*cout << "mid2-Transfer Function:\n";
	cout << cg->str("    ") << "\n";*/
		
		// incorporate this node's divisibility information
		incorporateDivInfo(func, n, state, dfInfo);
	
	/*cout << "late-Transfer Function:\n";
	cout << cg->str("    ") << "\n";*/
		cg->endTransaction();
		
		//cg->beginTransaction();
		removeConstrDivVars(func, n, state, dfInfo);
		cg->divVarsClosure();
		//cg->endTransaction();
	}

	cout << "end-Transfer Function:\n";
	cout << cg->str("    ") << "\n";
	
/*
	bool modified = false;
	IntArithLogical* ial= dynamic_cast<IntArithLogical*>(dfInfo.front());

	// Upgrade ial to bottom if it is currently uninitialized
	//printf("before: ial=%s\n", ial->str().c_str());
	ial->initialize();
	//printf("after: ial=%s\n", ial->str().c_str());
	
	// Update ial with this partition's condition
	//ial->andUpd(*getIntArithLogical())
	
	printf("    scalarFWDataflowPart::transfer()   isSgExpression(n.getNode())=%p\n", isSgExpression(n.getNode()));
	
	if(isSgIfStmt(n.getNode()) && n.getIndex()==1)
	{
		// This analysis splits over if statements, with one sub-analysis following each branch
		splitAnalysis=IntraPartitionFWDataflow::splitNew;
		// Iterate over both the descendants, adding their logical conditions to splitConditions
		vector<DataflowEdge> edges = n.outEdges();
		int i;
		for(vector<DataflowEdge>::iterator ei = edges.begin(); ei!=edges.end(); ei++, i++)
		{
			IntArithLogical* expr = new IntArithLogical(getIntArithLogical((*ei).target()));

			//cout << "expr = "<<expr->str("")<<"\n";
			splitConditions.push_back((LogicalCond* printable*)expr);
		}
	}
	// if this is a meet node for an if statement, join the split created at the if statement
	else if(isIfMeetNode(n))
	{
		joinNode = true;
		return false;
	}
	else
	{
		// Regular Nodes
		if(isSgExpression(n.getNode()))
		{
			short op;
			varID i, j, k;
			bool negJ, negK;
			long c;
			
			if(cfgUtils::parseAssignment(isSgExpression(n.getNode()), op, i, j, negJ, k, negK, c))
			{
				printf("    scalarFWDataflowPart::transfer()   cfgUtils::parseAssignment(%p, %d, %s, %s, %d, %s, %d, %d)\n", isSgExpression(n.getNode()), op, i.str().c_str(), j.str().c_str(), negJ, k.str().c_str(), negK, c);
				// i = j or i = c
				if(op == cfgUtils::none)
				{
					modified = ial->assign(1, i, 1, j, c) || modified;
				}
				// i = (-)j+(-)k+c
				else if(op == cfgUtils::add)
				{
					printf("    scalarFWDataflowPart::transfer() op == cfgUtils::add\n");
					// i = j + c
					if(k == zeroVar && negJ==false)
					{
						printf("    scalarFWDataflowPart::transfer() k == zeroVar && negJ==false\n");
						modified = ial->assign(1, i, 1, j, c) || modified;
					}
				}
				// i = (-)j*(-)k*c
				else if(op == cfgUtils::mult)
				{
					printf("    scalarFWDataflowPart::transfer() op == cfgUtils::mult\n");
					// i = j*b + 0
					if(k == oneVar && negJ==false)
					{
						printf("    scalarFWDataflowPart::transfer() k == zeroVar && negJ==false\n");
						modified = ial->assign(1, i, c, j, 0) || modified;
					}
				}
				// i = (-)j/(-)k/c
				else if(op == cfgUtils::divide)
				{
					printf("    scalarFWDataflowPart::transfer() op == cfgUtils::divide\n");
					// i = j/c == i*c = j
					if(k == oneVar && negJ==false)
					{
						modified = ial->assign(c, i, 1, j, 0) || modified;
					}
				}
			}
		}
		
	cout << "mid-Transfer Function:\n";
	cout << ial->str("    ") << "\n";
	}*/
	
	return modified;
}

// Called when a partition is created to allow a specific analysis to initialize
// its dataflow information from the partition condition
void scalarFWDataflowPart::initDFfromPartCond(const Function& func, const DataflowNode& n, NodeState& state, 
	                                           const vector<Lattice*>& dfInfo, const vector<NodeFact*>& facts,
	                                           /*LogicalCond*/printable* partitionCond)
{
	ConstrGraph* cg = dynamic_cast<ConstrGraph*>(dfInfo.front());
	if(partitionCond)
	{
		ConstrGraph* partCG = dynamic_cast<ConstrGraph*>(partitionCond);
		printf("Initializing partition's dataflow info\n");
		printf("         CFG: %s\n", cg->str("").c_str());
		printf("      && partitionCond: %s\n", partitionCond->str("").c_str());
		cg->andUpd(partCG);
		printf("      => %s\n", cg->str("").c_str());
	}
	
	/*IntArithLogical* ial = dynamic_cast<IntArithLogical*>(dfInfo.front());
	if(partitionCond)
	{
		printf("Initializing partition's dataflow info\n");
		printf("         %s\n", ial->str("").c_str());
		printf("      && %s\n", partitionCond->str("").c_str());
		ial->andUpd(*partitionCond);
		printf("      => %s\n", ial->str("").c_str());
		//printf("      ial->mayTrue()=%d\n", ial->mayTrue());
		if(!ial->mayTrue())
			ial->setToFalse();
	}*/
}


// Incorporates the current node's inequality information from conditionals (ifs, fors, etc.) into the current node's 
// constraint graph.
// returns true if this causes the constraint graph to change and false otherwise
bool scalarFWDataflowPart::incorporateConditionalsInfo(const Function& func, const DataflowNode& n, 
                                                   NodeState& state, const vector<Lattice*>& dfInfo)
{
	bool modified = false;
	ConstrGraph* cg = dynamic_cast<ConstrGraph*>(dfInfo.front());
	
	printf("    incorporateConditionalsInfo()\n");
	const set<varAffineInequality>& ineqs = getAffineIneq(n);
	cout << "Node <"<<n.getNode()->class_name()<<" | "<<n.getNode()->unparseToString()<<">\n";
	printf("       ineqs.size()=%lu\n", (unsigned long) (ineqs.size()));
	for(set<varAffineInequality>::const_iterator it = ineqs.begin(); it!=ineqs.end(); it++)
	{
		varAffineInequality varIneq = *it;
		//cout << varIneq.getIneq().str(varIneq.getX(), varIneq.getY(), "    ") << "\n";
		modified = cg->setVal(varIneq.getX(), varIneq.getY(), varIneq.getIneq().getA(), 
		                      varIneq.getIneq().getB(), varIneq.getIneq().getC()) || modified;
	}
	return modified;
}

// incorporates the current node's divisibility information into the current node's constraint graph
// returns true if this causes the constraint graph to change and false otherwise
bool scalarFWDataflowPart::incorporateDivInfo(const Function& func, const DataflowNode& n, NodeState& state, const vector<Lattice*>& dfInfo)
{
	bool modified = false;
	ConstrGraph* cg = dynamic_cast<ConstrGraph*>(dfInfo.front());
	
	printf("    incorporateDivInfo()\n");
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
bool scalarFWDataflowPart::removeConstrDivVars(const Function& func, const DataflowNode& n, NodeState& state, const vector<Lattice*>& dfInfo)
{
	bool modified = false;
	ConstrGraph* cg = dynamic_cast<ConstrGraph*>(dfInfo.front());
	
	printf("    removeConstrDivVars()\n");
	
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

// Creates a new instance of the derived object that is a copy of the original instance.
// This instance will be used to instantiate a new partition of the analysis.
IntraPartitionDataflow* scalarFWDataflowPart::copy()
{
	return new scalarFWDataflowPart(*this);
}
