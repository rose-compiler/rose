#include "contProcMatchAnalysis.h"
#include <sys/time.h>

static int profileLevel=1;
int MPIAnalysisDebugLevel = 0;

#define startProfileFunc(funcName)     \
	struct timeval startTime, endTime;  \
	if(profileLevel>=1)                 \
	{                                   \
		cout << funcName << "() <<< \n"; \
		gettimeofday(&startTime, NULL);  \
	}

#define endProfileFunc(funcName)                                                                                                                         \
	if(profileLevel>=1)                                                                                                                                   \
	{                                                                                                                                                     \
		gettimeofday(&endTime, NULL);                                                                                                                      \
		cout << funcName << "() >>> time="<<((double)((endTime.tv_sec*1000000+endTime.tv_usec)-(startTime.tv_sec*1000000+startTime.tv_usec)))/1000000.0<<"\n"; \
	}

/****************************************************************************************
 *************                  pCFG_contProcMatchAnalysis                  *************
 ****************************************************************************************/

varID pCFG_contProcMatchAnalysis::rankVar("rankVar");
varID pCFG_contProcMatchAnalysis::nprocsVar("nprocsVar");
	
contRangeProcSet pCFG_contProcMatchAnalysis::rankSet(false);

// Generates the initial lattice state for the given dataflow node, in the given function, with the given NodeState
void pCFG_contProcMatchAnalysis::genInitState(const Function& func, const pCFGNode& n, const NodeState& state,
	                                           vector<Lattice*>& initLattices, vector<NodeFact*>& initFacts)
{
	//vector<Lattice*> initLattices;
/*printf("pCFG_contProcMatchAnalysis::genInitState() n=%p<%s | %s>\n", n.getNode(), n.getNode()->class_name().c_str(), n.getNode()->unparseToString().c_str());
printf("pCFG_contProcMatchAnalysis::genInitState() state=%p\n", &state);*/
	
	// Create the divisibility and sign lattice maps that map each process set to its respective divisibility
	// and sign product lattice
	int curPSet=0;
	
	map<pair<string, void*>, FiniteVariablesProductLattice*> divL;
	map<pair<string, void*>, FiniteVariablesProductLattice*> sgnL;
	for(vector<DataflowNode>::const_iterator it=n.getPSetDFNodes().begin(); it!=n.getPSetDFNodes().end(); it++, curPSet++)
	{
		NodeState* state = NodeState::getNodeState(*it);
		pair<string, void*> annotation(getVarAnn(curPSet), (void*)1);
		
		divL[annotation] = dynamic_cast<FiniteVariablesProductLattice*>(state->getLatticeBelow(divAnalysis, 0));
		sgnL[annotation] = dynamic_cast<FiniteVariablesProductLattice*>(state->getLatticeBelow(sgnAnalysis, 0));
	}
	// Create a constraint graph from the divisiblity and sign information at all the CFG nodes that make up this pCFG node
	ConstrGraph* cg = new ConstrGraph(func, divL, sgnL, false);
	
	// Create a copy of each function-visible variable (except zeroVar) for each process set in n
	set<pair<string, void*> > noCopyAnnots;
	set<varID> noCopyVars;
	noCopyVars.insert(zeroVar);
	curPSet=0;
	for(vector<DataflowNode>::const_iterator it=n.getPSetDFNodes().begin(); it!=n.getPSetDFNodes().end(); it++, curPSet++)
	{
		cg->copyAnnotVars("", (void*)1, getVarAnn(curPSet), (void*)1,
		                  noCopyAnnots, noCopyVars);
	}
	
	// For each process set create a version of the key variables and add them to the constraint graph
	varID nprocsVarAllPSets, zeroVarAllPSets;
	annotateCommonVars(/*n, */zeroVar, zeroVarAllPSets, nprocsVar, nprocsVarAllPSets);
	//cg->addScalar(/*nprocsVarAllPSets*/nprocsVarPSet);
	cg->replaceVar(zeroVar, zeroVarAllPSets);
	//cg->addScalar(zeroVarAllPSets);
	
	curPSet=0;
	for(vector<DataflowNode>::const_iterator it=n.getPSetDFNodes().begin(); it!=n.getPSetDFNodes().end(); it++, curPSet++)
	{
		contRangeProcSet rankSetPSet = rankSet; rankSetPSet.addAnnotation(getVarAnn(curPSet), (void*)1);
		varID rankVarPSet = rankVar; rankVarPSet.addAnnotation(getVarAnn(curPSet), (void*)1);
		varID nprocsVarPSet = nprocsVar; nprocsVarPSet.addAnnotation(getVarAnn(curPSet), (void*)1);
		cg->addScalar(/*nprocsVarAllPSets*/nprocsVarPSet);
		
		cg->addScalar(rankSetPSet.getLB());
		cg->addScalar(rankSetPSet.getUB());
		cg->addScalar(rankVarPSet);
		
		// Add the default process set invariants: [0<= lb <= ub < nprocsVar] and [lb <= rankVar <= ub]
		assertProcSetInvariants(cg, rankSetPSet, zeroVarAllPSets, rankVarPSet, nprocsVarPSet);
	}
	cg->transitiveClosure();
	cg->setToUninitialized();
	
	initLattices.push_back(cg);
	
	//contRangeProcSet* nodeRankSet = new contRangeProcSet(rankSet);
	//initFacts.push_back(nodeRankSet);
}

// Copies the dataflow information from the srcPSet to the tgtPSet and updates the copy with the
// partitionCond (presumably using initPSetDFfromPartCond). Adds the new info directly to lattices and facts.
// It is assumed that pCFGNode n contains both srcPSet and tgtPSet.
// If omitRankSet==true, does not copy the constraints on srcPSet's bounds variables but
// instead just adds them with no non-trivial constraints.
void pCFG_contProcMatchAnalysis::copyPSetState(const Function& func, const pCFGNode& n, 
                                               int srcPSet, int tgtPSet, NodeState& state,
                                               vector<Lattice*>& lattices, vector<NodeFact*>& facts, 
                                               ConstrGraph* partitionCond, bool omitRankSet)
{
	ConstrGraph* cg = dynamic_cast<ConstrGraph*>(*(lattices.begin()));

if(MPIAnalysisDebugLevel>0)
{	
	cout << "copyPSetState(srcPSet="<<srcPSet<<", tgtPSet="<<tgtPSet<<")\n";
	cout << "cg="<<cg->str("    ")<<"\n";
	cout << "partitionCond="<<partitionCond->str("    ")<<"\n";
}	
	//cg->beginTransaction();
	
	// Add the divisibility and sign lattices to the constraint graph under the annotation that belongs to tgtPSet
	cg->addDivL(dynamic_cast<FiniteVariablesProductLattice*>(state.getLatticeBelow(divAnalysis, 0)), getVarAnn(tgtPSet), (void*)1);
	cg->addSgnL(dynamic_cast<FiniteVariablesProductLattice*>(state.getLatticeBelow(sgnAnalysis, 0)), getVarAnn(tgtPSet), (void*)1);

	// For each process set create a version of the key variables and add them to the constraint graph
	varID nprocsVarAllPSets, zeroVarAllPSets;
	annotateCommonVars(/*n, */zeroVar, zeroVarAllPSets, nprocsVar, nprocsVarAllPSets);
	
	// Copy the state of variables from srcPSet and associate them with tgtPSet
	// We leave the variables from other process sets and variables common to all process sets alone
	set<pair<string, void*> > noCopyAnnots;
	pair<string, void*> commonAnnotation("pCFG_common", (void*)1);
	noCopyAnnots.insert(commonAnnotation);
	set<varID> noCopyVars;
	if(omitRankSet)
	{
		// Do not copy srcPSet's bounds variables
		contRangeProcSet rankSetPSet = rankSet; // The rankSet of the process set pSet
		rankSetPSet.setConstrAnnot(cg, getVarAnn(srcPSet), (void*)1);
		noCopyVars.insert(rankSetPSet.getLB());
		noCopyVars.insert(rankSetPSet.getUB());
	}
	
	// <<<<<<<<<<<<<<<<<<<<<<<<<
	// Perform the copy srcPSet->tgtPSet copy
	cg->copyAnnotVars(getVarAnn(srcPSet), (void*)1, getVarAnn(tgtPSet), (void*)1, noCopyAnnots, noCopyVars);
	// >>>>>>>>>>>>>>>>>>>>>>>>>
	if(MPIAnalysisDebugLevel>0)
	{
		cout << "After copy cg="<<cg->str("    ")<<"\n";
		cout << "After copy partitionCond="<<partitionCond->str("    ")<<"\n";
	}
	
	if(omitRankSet)
	{
		varID nprocsVarPSet = nprocsVar; nprocsVarPSet.addAnnotation(getVarAnn(tgtPSet), (void*)1);
		
		// Set up the bounds of tgtPSet as the default bounds for rank sets
		resetPSet(tgtPSet, cg, /*nprocsVarAllPSets*/nprocsVarPSet, zeroVarAllPSets);
	}
	
	// Update the newly-copied dataflow state with partitionCond
	//initDFfromPartCond(func, n, tgtPSet, state, lattices, facts, partitionCond);
	
	// Update the state of process set tgtPSet in cg from the partition condition
	initPSetDFfromPartCond_ex(func, n, tgtPSet, lattices, facts, partitionCond);
	
	// Annotate all the un-annotated variables in partitionCond to mark them
	// as belonging to process set tgtPSet
	//partitionCond->addVarAnnot("", (void*)1, getVarAnn(tgtPSet), (void*)1);
	// Add the information in partitionCond to cg
	//cg->andUpd(partitionCond);
	
	//cg->endTransaction();
	cg->transitiveClosure();
}

// Removes all known bounds on pSet's process set in dfInfo and replaces them with the default
// constraints on process set bounds.
void pCFG_contProcMatchAnalysis::resetPSet(int pSet, vector<Lattice*>& lattices)
{
	ConstrGraph* cg = dynamic_cast<ConstrGraph*>(*(lattices.begin()));
	
	varID nprocsVarAllPSets, zeroVarAllPSets;
	annotateCommonVars(/*n, */zeroVar, zeroVarAllPSets, nprocsVar, nprocsVarAllPSets);
	varID nprocsVarPSet = nprocsVar; nprocsVarPSet.addAnnotation(getVarAnn(pSet), (void*)1);
	
	resetPSet(pSet, cg, /*nprocsVarAllPSets*/nprocsVarPSet, zeroVarAllPSets);
}

// Helper method for resetPSet that makes it easier to call it from inside pCFG_contProcMatchAnalysis.
void pCFG_contProcMatchAnalysis::resetPSet(int pSet, ConstrGraph* cg, 
                                           const varID& /*nprocsVarAllPSets*/nprocsVarPSet, 
                                           const varID& zeroVarAllPSets)
{
	if(MPIAnalysisDebugLevel>0)
		cout << "--resetPSet ("<<pSet<<")--\n";
	
	// Since we did not create bounds variables for tgtPSet, create them now and
	// set up the default invariants
	contRangeProcSet rankSetPSet = rankSet; // The rankSet of the process set pSet
	rankSetPSet.setConstrAnnot(cg, getVarAnn(pSet), (void*)1);
	
	if(MPIAnalysisDebugLevel>0)
	{
		cout << "    Removing "<<rankSetPSet.getLB().str()<<"\n";
		cout << "    Removing "<<rankSetPSet.getUB().str()<<"\n";
	}
	cg->eraseVarConstr(rankSetPSet.getLB());
	cg->eraseVarConstr(rankSetPSet.getUB());
	
	// adds rankSetPSet.lb, rankSetPSet.ub and 
	// asserts rankSetPSet.lb <= rankSetPSet.ub
	rankSetPSet.refreshInvariants();
	
	if(MPIAnalysisDebugLevel>0)
		cout << "    Refreshed "<<rankSetPSet.str()<<"\n";
	
	varID rankVarPSet = rankVar; rankVarPSet.addAnnotation(getVarAnn(pSet), (void*)1);
	cg->eraseVarConstr(rankVarPSet);
	cg->eraseVarConstr(/*nprocsVarAllPSets*/nprocsVarPSet);
	
	// Add the default process set invariants: [0<= lb <= ub < nprocsVar] and [lb <= rankVar <= ub]
	assertProcSetInvariants(cg, rankSetPSet, zeroVarAllPSets, rankVarPSet, /*nprocsVarAllPSets*/nprocsVarPSet);
}

// Resets the state of rankVar inside the given constraint graph and re-establishes its basic 
// constraints: 0<=rankVar<nprocsVar
/*void pCFG_contProcMatchAnalysis::resetRank(const pCFGNode& n, ConstrGraph* cg)
{
	// Annotate zeroVar, nprocsVar and rankVar appropriately to represent 
	// their respective sets within the pCFG node
	voidID zeroVarAllPSets, nprocsVarAllPSets;
	varID rankVarPSet = rankVar; rankVarPSet.addAnnotation(getVarAnn(curPSet), (void*)1);
	annotateCommonVars(n, zeroVar, zeroVarAllPSets, nprocsVar, nprocsVarAllPSets);
	
	cg->eraseVarConstr(rankVarPSet);
	cg->addScalar(rankVarPSet);
	// 0<=rankVar<nprocsVar
	cg->assertCond(zeroVarAllPSets, rankVarPSet,   1, 1, 0);
	cg->assertCond(rankVarPSet, nprocsVarAllPSets, 1, 1, -1);
	cg->assertCond(zeroVarAllPSets, nprocsVarAllPSets, 1, 1, -1);	
}*/

// Annotate the given zeroVar and nprocsVar with the annotations that identify them as being available
// to all process sets within the given pCFG node. Specifically, these variables are annotated with
// all the annotations of all the process sets, plus another annotation that identifies them as common.
void pCFG_contProcMatchAnalysis::annotateCommonVars(/*pCFGNode n, */const varID& zeroVar, varID& zeroVarAllPSets, 
                                                    const varID& nprocsVar, varID& nprocsVarAllPSets)
{
	// For each process set create a version of the key variables and add them to the constraint graph
	nprocsVarAllPSets = nprocsVar;
	zeroVarAllPSets = zeroVar;
	/*
	// Generate the nprocsVar and zeroVar with annotations for all process sets since 
	// they belong to all process sets
	int curPSet=0;
	for(vector<DataflowNode>::const_iterator it=n.getPSetDFNodes().begin(); it!=n.getPSetDFNodes().end(); it++, curPSet++)
	{
		nprocsVarAllPSets.addAnnotation(getVarAnn(curPSet), (void*)1);
		zeroVarAllPSets.addAnnotation(getVarAnn(curPSet), (void*)1);
	}*/
	// Also give nprocsVarAllPSets and zeroVarAllPSets and special annotation that identifies them as common
	// (this is in contrast to the situation where there is only one set and nprocsVarAllPSets and 
	//  zeroVarAllPSets just happen to have the same annotations the set's private variables)
	nprocsVarAllPSets.addAnnotation("pCFG_common", (void*)1);
	zeroVarAllPSets.addAnnotation("pCFG_common", (void*)1);
}

// Returns the name of the variable annotation to be used when talking about process set pSet
string pCFG_contProcMatchAnalysis::getVarAnn(int pSet)
{
	ostringstream outs;
	outs << "pCFG_"<<pSet;
	return outs.str();
}

// Asserts within this constraint graph the standard invariants on process sets:\
// [0<= lb <= ub < nprocsVar] and [lb <= rankVar <= ub]
// Returns true if this causes the constraint graph to change, false otherwise
bool pCFG_contProcMatchAnalysis::assertProcSetInvariants(
                             ConstrGraph* cg, const contRangeProcSet& rankSetPSet,
                             const varID& zeroVarAllPSets, const varID& rankVarPSet,
                             const varID& nprocsVarPSet)
{
	bool modified = false;
	
	//cout << "assertProcSetInvariants cg="<<cg<<"\n";
	// 0<= lb <= ub < nprocsVar
	/*cout << "                        "<<zeroVarAllPSets.str()<<" <= "<<rankSetPSet.getLB().str()<<"\n";
	cout << "                        "<<rankSetPSet.getLB().str()<<" <= "<<rankSetPSet.getUB().str()<<"\n";
	cout << "                        "<<rankSetPSet.getUB().str()<<" <= "<<nprocsVarPSet.str()<<"\n";*/
	modified = cg->assertCond(zeroVarAllPSets,     rankSetPSet.getLB(), 1, 1, 0) || modified;
	modified = cg->assertCond(rankSetPSet.getLB(), rankSetPSet.getUB(), 1, 1, 0) || modified;
	modified = cg->assertCond(rankSetPSet.getUB(), /*nprocsVarAllPSets*/nprocsVarPSet,   1, 1, -1) || modified;
	
	// lb <= rankVar <= ub
	/*cout << "                        "<<rankSetPSet.getLB().str()<<" <= "<<rankVarPSet.str()<<"\n";
	cout << "                        "<<rankSetPSet.str()<<" <= "<<rankSetPSet.getUB().str()<<"\n";*/
	modified = cg->assertCond(rankSetPSet.getLB(), rankVarPSet,         1, 1, 0) || modified;
	modified = cg->assertCond(rankVarPSet,         rankSetPSet.getUB(), 1, 1, 0) || modified;
	
	return modified;
}

// Update varIneq's variables with the annotations that connect them to pSet
void pCFG_contProcMatchAnalysis::connectVAItoPSet(int pSet, varAffineInequality& varIneq)
{
	// Annotations that identify this process set or the variables common to all process sets
	set<string> tgtAnnotNames;
	tgtAnnotNames.insert(getVarAnn(pSet));
	tgtAnnotNames.insert("pCFG_common");
	
	if(varIneq.getX()==zeroVar   || varIneq.getY()==zeroVar ||
	   varIneq.getX()==nprocsVar || varIneq.getY()==nprocsVar)
	{
		varID zeroVarAllPSets, nprocsVarAllPSets;
		annotateCommonVars(/*n, */zeroVar, zeroVarAllPSets, nprocsVar, nprocsVarAllPSets);
		varID nprocsVarPSet = nprocsVar; nprocsVarPSet.addAnnotation(getVarAnn(pSet), (void*)1);
		
		if(varIneq.getX()==zeroVar)        varIneq.setX(zeroVarAllPSets);
		else if(varIneq.getX()==nprocsVar) varIneq.setX(/*nprocsVarAllPSets*/nprocsVarPSet);
		// If varIneq.x is neither zeroVar, not nprocsVar and does not have our target annotation, add it
		else if(!varIneq.getX().hasAnyAnnotation(tgtAnnotNames))
		{
			varID x = varIneq.getX(); x.addAnnotation(getVarAnn(pSet), (void*)1);
			varIneq.setX(x);
		}
		
		if(varIneq.getY()==zeroVar)        varIneq.setY(zeroVarAllPSets);
		else if(varIneq.getY()==nprocsVar) varIneq.setY(/*nprocsVarAllPSets*/nprocsVarPSet);
		// If varIneq.y is neither zeroVar, not nprocsVar and does not have our target annotation, add it
		else if(!varIneq.getY().hasAnyAnnotation(tgtAnnotNames))
		{
			varID y = varIneq.getY(); y.addAnnotation(getVarAnn(pSet), (void*)1);
			varIneq.setY(y);
		}
	}
	else
	{
		// If varIneq.x does not have our target annotation, add it
		if(!varIneq.getX().hasAnyAnnotation(tgtAnnotNames))
		{
			varID x = varIneq.getX(); x.addAnnotation(getVarAnn(pSet), (void*)1);
			varIneq.setX(x);
		}
		
		// If varIneq.y does not have our target annotation, add it
		if(!varIneq.getY().hasAnyAnnotation(tgtAnnotNames))
		{
			varID y = varIneq.getY(); y.addAnnotation(getVarAnn(pSet), (void*)1);
			varIneq.setY(y);
		}
	}
}

// The transfer function that is applied to every node
// n - the pCFG node that is being processed
// pSet - the process set that is currently transitioning
// func - the function that is currently being analyzed
// state - the NodeState object that describes the state of the node, as established by earlier 
//         analysis passes
// dfInfo - the Lattices that this transfer function operates on. The function takes these lattices
//          as input and overwrites them with the result of the transfer.
// deadPSet - may be set by the call to true to indicate that the dataflow state at this node constains an 
//            inconsistency that makes this an impossible execution state
// splitPSet - set by the call to indicate if the current process set must be broken up into two process sets,
//             each of which takes a different path as a result of a conditional or a loop.
// splitPSetNodes - if splitPNode, splitNodes is filled with the nodes along which each new process set must advance
// splitPNode - set by the call to indicate if the current process set must take two different paths as a result
//             of a conditional or a loop, causing the analysis to advance along two different pCFGNodes
// splitConditions - if splitPNode==true or splitPSet==true, splitConditions is filled with the information 
//             about the state along both sides of the split of pCFGNode or pSet split
// blockPSet - set to true by the call if progress along the given dataflow node needs to be blocked 
//             until the next send-receive matching point. If all process sets become blocked, we 
//             perform send-receive matching.
// Returns true if any of the input lattices changed as a result of the transfer function and
//    false otherwise.
bool pCFG_contProcMatchAnalysis::transfer(const pCFGNode& n, int pSet, const Function& func, 
                                          NodeState& state, const vector<Lattice*>&  dfInfo, 
                                          bool& deadPSet, bool& splitPSet, vector<DataflowNode>& splitPSetNodes,
                                          bool& splitPNode, vector<ConstrGraph*>& splitConditions, bool& blockPSet)
{
	startProfileFunc("transfer");
	
	/*printf("    -----------------------------------\n");
	printf("    pCFG_contProcMatchAnalysis::transfer() function %s() node=<%s | %s | %d>\n", func.get_name().str(), n.getNode()->class_name().c_str(), n.getNode()->unparseToString().c_str(), n.getIndex());
	*/
	const DataflowNode& dfNode = n.getCurNode(pSet);
	bool modified = false;
	ConstrGraph* cg = dynamic_cast<ConstrGraph*>(*(dfInfo.begin()));
	
	if(MPIAnalysisDebugLevel>0)
		cout << "transfer() Initial cg = "<<cg->str()<<"\n";
	
	//contRangeProcSet* nodeRankSet = dynamic_cast<contRangeProcSet*>(state.getFact((Analysis*)this, 0));
	// If the constraint graph is already bottom, tell the calling analysis that we're at an impossible state
	if(cg->isBottom())
	{
		deadPSet = true;
		endProfileFunc("transfer");
		return modified;
	}
	
	// The rank variable of the given process set and the versions zeroVar and nprocsVar annotated
	// to indicate that they are used in all process sets
	varID rankVarPSet = rankVar; rankVarPSet.addAnnotation(getVarAnn(pSet), (void*)1);
	varID zeroVarAllPSets, nprocsVarAllPSets;
	annotateCommonVars(/*n, */zeroVar, zeroVarAllPSets, nprocsVar, nprocsVarAllPSets);
	//mesgBuf* sentMesgs = dynamic_cast<mesgBuf*>(*(dfInfo.begin()+1));
	varID nprocsVarPSet = nprocsVar; nprocsVarPSet.addAnnotation(getVarAnn(pSet), (void*)1);
	
	//cout << "cg->scalars = ";
	//for(varIDSet::iterator it=cg->getScalars().begin(); it!=cg->getScalars().end(); it++)
	//{ cout << (*it).str() << ", "; }
	//cout << "\n";
	
	cg->beginTransaction();
	
	// Upgrade ial to bottom if it is currently uninitialized
	//printf("before: cg=%s\n", cg->str("").c_str());
	cg->initialize();
	//printf("after: cg->mayTrue=%d cg=%s\n", cg->mayTrue(), cg->str().c_str());
	
	// If this is an if statement
	if((isSgIfStmt(dfNode.getNode()) && dfNode.getIndex()==1) ||
	   (isSgForStatement(dfNode.getNode()) && dfNode.getIndex()==2))
	{
if(MPIAnalysisDebugLevel>0)
	cout << "transfer SgIfStmt\n";
		// Look at the condition of this if statement, which should be this DataflowNode's only predecessor
		vector<DataflowEdge> edges = dfNode.inEdges();
		ROSE_ASSERT(edges.size()==1);
		/*for(vector<DataflowEdge>::iterator ei = edges.begin(); ei!=edges.end(); ei++)
		{
			DataflowNode pred = (*ei).source();
			cout << "Predecessor: "<<pred.getNode()->unparseToString()<<"\n";
		}*/
		
		// if this if statement depends on the process rank
		if(isMPIRankDep(func, (*(edges.begin())).source()))
		{
			// This analysis splits over if statements, with one sub-analysis following each branch
			splitPSet=true;
			
			vector<DataflowEdge> edges = dfNode.outEdges();
			for(vector<DataflowEdge>::iterator ei = edges.begin(); ei!=edges.end(); ei++)
				splitPSetNodes.push_back((*ei).target());
			fillEdgeSplits(func, n, pSet, dfNode, state, cg, splitConditions, rankVarPSet, nprocsVarPSet);
			
			ROSE_ASSERT(splitPSetNodes.size() == splitConditions.size());
		}
		else
		{
			// This analysis splits over if statements, with the analysis following both resulting pCFGNodes
			splitPNode=true;
			fillEdgeSplits(func, n, pSet, dfNode, state, cg, splitConditions, rankVarPSet, nprocsVarPSet);
		}
	}
	else if(cg->mayTrue())
	{
if(MPIAnalysisDebugLevel>0)
	cout << "transfer mayTrue\n";
		// Regular Nodes
		if(cfgUtils::isAssignment(dfNode.getNode()))
		{
			short op;
			varID i, j, k;
			bool negJ, negK;
			long c;
			
			if(cfgUtils::parseAssignment(dfNode.getNode(), op, i, j, negJ, k, negK, c))
			{
				//cout << "i("<<i.str()<<") == zeroVar("<<zeroVar.str()<<" = "<<(i == zeroVar)<<"\n";
				if(i == zeroVar) i.addAnnotation("pCFG_common", (void*)1);
				else             i.addAnnotation(getVarAnn(pSet), (void*)1);
				if(j == zeroVar) j.addAnnotation("pCFG_common", (void*)1);
				else             j.addAnnotation(getVarAnn(pSet), (void*)1);
				//k.addAnnotation(getVarAnn(pSet), (void*)1);
				if(MPIAnalysisDebugLevel>0)
					printf("    pCFG_contProcMatchAnalysis::transfer()   cfgUtils::parseAssignment(%p, %d, %s, %s, %d, %s, %d, %d)\n", isSgExpression(dfNode.getNode()), op, i.str().c_str(), j.str().c_str(), negJ, k.str().c_str(), negK, c);
				// i = j or i = c
				if(op == cfgUtils::none)
				{
					modified = cg->assign(i, j, 1, 1, c) || modified;
				}
				// i = (-)j+(-)k+c
				else if(op == cfgUtils::add)
				{
					if(MPIAnalysisDebugLevel>0)
						printf("    pCFG_contProcMatchAnalysis::transfer() op == cfgUtils::add\n");
					// i = j + c
					if(k == zeroVar && negJ==false)
					{
						if(MPIAnalysisDebugLevel>0)
							printf("    pCFG_contProcMatchAnalysis::transfer() k == zeroVar && negJ==false\n");
						modified = cg->assign(i, j, 1, 1, c) || modified;
					}
				}
				// i = (-)j*(-)k*c
				else if(op == cfgUtils::mult)
				{
					if(MPIAnalysisDebugLevel>0)
						printf("    pCFG_contProcMatchAnalysis::transfer() op == cfgUtils::mult\n");
					// i = j*b + 0
					if(k == oneVar && negJ==false)
					{
						if(MPIAnalysisDebugLevel>0)
							printf("    pCFG_contProcMatchAnalysis::transfer() k == zeroVar && negJ==false\n");
						modified = cg->assign(i, j, 1, c, 0) || modified;
					}
				}
				// i = (-)j/(-)k/c
				else if(op == cfgUtils::divide)
				{
					if(MPIAnalysisDebugLevel>0)
						printf("    pCFG_contProcMatchAnalysis::transfer() op == cfgUtils::divide\n");
					// i = j/c == i*c = j
					if(k == oneVar && negJ==false)
					{
						modified = cg->assign(i, j, c, 1, 0) || modified;
					}
				}
			}
		}
		else if(isSgFunctionCallExp(dfNode.getNode()) && dfNode.getIndex()==2)
		{
if(MPIAnalysisDebugLevel>0)
	cout << "transfer SgFunctionCallExp\n";
			SgFunctionCallExp* fnCall = isSgFunctionCallExp(dfNode.getNode());
			Function calledFunc(fnCall);
			
			//cout << "calledFunc.get_name().getString() = "<<calledFunc.get_name().getString()<<"\n";
			if(calledFunc.get_name().getString() == "MPI_Init")
			{
				/*cg->addScalar(nprocsVar);
				// 0<=rankVar<nprocsVar
				resetRank(cg, n);*/
				
				contRangeProcSet rankSetPSet = rankSet; // The rankSet of the process set pSet
				// rankSet_{pSet} = [0, nprocsVar-1]
				rankSetPSet.setConstrAnnot(cg, getVarAnn(pSet), (void*)1);
				rankSetPSet.assignLB(zeroVarAllPSets);
				rankSetPSet.assignUB(/*nprocsVarAllPSets*/nprocsVarPSet, 1, 1, -1);
				rankSetPSet.refreshInvariants();
				
				if(MPIAnalysisDebugLevel>0)
					cout << "cg = "<<cg->str()<<"\n";
			}
			else if(calledFunc.get_name().getString() == "MPI_Comm_rank")
			{
				//printf("        MPI_Comm_rank\n");
				SgExpressionPtrList& args = fnCall->get_args()->get_expressions();
				// args[1]
				SgExpression* arg1 = *(++(args.begin()));
				//printf("arg1 = <%s | %s>\n", arg1->class_name().c_str(), arg1->unparseToString().c_str(), varID::isValidVarExp(isSgAddressOfOp(arg1)->get_operand()));
				
				// Look at MPI_Comm_rank's second argument and add the constraint
				// that it is now equal to the global rank variable
				if(isSgAddressOfOp(arg1) && varID::isValidVarExp(isSgAddressOfOp(arg1)->get_operand()))
				{
					varID rankArgVar(isSgAddressOfOp(arg1)->get_operand()); rankArgVar.addAnnotation(getVarAnn(pSet), (void*)1);
					cg->addScalar(rankArgVar);
					modified = cg->assign(rankArgVar, rankVarPSet, 1, 1, 0) ||  modified;
				}
				else
				{ cerr << "[commTopoAnal] pCFG_contProcMatchAnalysis::transfer() ERROR: second argument \""<<arg1->unparseToString()<<"\" of function call \""<<dfNode.str()<<"\" is not a reference to a variable!\n"; exit(-1); }
			}
			else if(calledFunc.get_name().getString() == "MPI_Comm_size")
			{
				//printf("        MPI_Comm_rank\n");
				SgExpressionPtrList& args = fnCall->get_args()->get_expressions();
				// args[1]
				SgExpression* arg1 = *(++(args.begin()));
				
				// look at MPI_Comm_size's second argument and record that it depends on the number of processes
				if(isSgAddressOfOp(arg1) && varID::isValidVarExp(isSgAddressOfOp(arg1)->get_operand()))
				{
					varID nprocsArgVar(isSgAddressOfOp(arg1)->get_operand()); nprocsArgVar.addAnnotation(getVarAnn(pSet), (void*)1);
					cg->addScalar(nprocsArgVar);
					modified = cg->assign(nprocsArgVar, /*nprocsVarAllPSets*/nprocsVarPSet, 1, 1, 0) ||  modified;
				}
				else
				{ cerr << "[commTopoAnal] pCFG_contProcMatchAnalysis::transfer() ERROR: second argument \""<<arg1->unparseToString()<<"\"  of function call \""<<dfNode.str()<<"\" is not a reference to a variable!\n"; exit(-1); }
			}
			else if(calledFunc.get_name().getString() == "MPI_Send")
			{
				blockPSet = true;
				modified = false;
			}
			else if(calledFunc.get_name().getString() == "MPI_Recv")
			{
				blockPSet = true;
				modified = false;
			}
		}
	}
	if(MPIAnalysisDebugLevel>0)
		printf("~~~\n");
	
	/*cout << "mid-Transfer Function:\n";
	cout << cg->str("    ") << "\n";*/
	
	// If this is not the meet node of an if statement, incorporate information from
	//    conditionals. The reason for this is that if one branch of an if statement is missing,
	//    its condition gets placed on the meet node of the if statement, and if we didn't do this
	//    this condition get picked up by dataflow that is coming down the other branch of the 
	//    if statement.
	if(MPIAnalysisDebugLevel>0)
	{
		cout << "isIfMeetNode(dfNode)="<<isIfMeetNode(dfNode)<<"\n";
		cout << "isSgIfStmt(dfNode.getNode())="<<isSgIfStmt(dfNode.getNode())<<" dfNode.getIndex()="<<dfNode.getIndex()<<"\n";
	}
	//if(!isIfMeetNode(dfNode))
	if(!isSgIfStmt(dfNode.getNode()) || dfNode.getIndex()!=2)
		// incorporate this node's inequalities from conditionals
		incorporateConditionalsInfo(n, pSet, func, dfNode, state, dfInfo);
	
	/*cout << "mid2-Transfer Function:\n";
	cout << cg->str("    ") << "\n";*/
	
	// incorporate this node's divisibility information
	//incorporateDivInfo(func, n, state, dfInfo);

	/*cout << "late-Transfer Function:\n";
	cout << cg->str("    ") << "\n";*/
	cg->endTransaction();
	
	//cg->beginTransaction();
	/*removeConstrDivVars(func, n, state, dfInfo);
	cg->divVarsClosure();*/
	//cg->endTransaction();

	if(MPIAnalysisDebugLevel>0)
	{
		cout << "end-Transfer Function:\n";
		cout << cg->str("    ") << "\n";
	}
	/*cout << "cg->scalars = ";
	for(varIDSet::iterator it=cg->getScalars().begin(); it!=cg->getScalars().end(); it++)
	{ cout << (*it).str() << ", "; }
	cout << "\n";*/
	
	// If this transfer function causes the constraint graph to become bottom,
	// tell the calling analysis that we're at an impossible state
	if(cg->isBottom())
		deadPSet = true;
	
	endProfileFunc("transfer");

	return modified;
}

// Iterates over the outgoing edges of a given node and fills splitConditions with the actual split conditions
// of these edges. It is specifically designed to look for how each edge condition relates to tgtVar
// and adds those conditions to splitConditions as well
void pCFG_contProcMatchAnalysis::fillEdgeSplits(const Function& func, const pCFGNode& n, int splitPSet, 
	                                             const DataflowNode& dfNode, NodeState& state, ConstrGraph* cg, 
	                                             vector<ConstrGraph*>& splitConditions, const varID& tgtVar, const varID& nprocsVarPSet)
{
	// Iterate over both the descendants, adding their logical conditions to splitConditions
	vector<DataflowEdge> edges = dfNode.outEdges();
	if(MPIAnalysisDebugLevel>0)
		printf("pCFG_contProcMatchAnalysis::fillEdgeSplits(): n.outEdges().size()=%d\n", dfNode.outEdges().size());
	int i=0;
	for(vector<DataflowEdge>::iterator ei = edges.begin(); ei!=edges.end(); ei++, i++)
	{
		if(MPIAnalysisDebugLevel>0)
			cout << "Descendant "<<i<<": "<<(*ei).target().str()<<"\n";
		const set<varAffineInequality>& ineqs = getAffineIneq((*ei).target());
		
		/*FiniteVariablesProductLattice* divProdL = dynamic_cast<FiniteVariablesProductLattice*>(state.getLatticeBelow(divAnalysis, 0));
		FiniteVariablesProductLattice* sgnProdL = dynamic_cast<FiniteVariablesProductLattice*>(state.getLatticeBelow(sgnAnalysis, 0));
		ConstrGraph* edgeCG = new ConstrGraph(func, divProdL, sgnProdL, false);*/
		ConstrGraph* edgeCG = new ConstrGraph(func, NULL, NULL, false);

		//printf("ineqs.size()=%d\n", ineqs.size());
		// Iterate through the current descendant's affine inequalities
		for(set<varAffineInequality>::const_iterator it=ineqs.begin(); it!=ineqs.end(); it++)
		{
			varAffineInequality ineq = *it;
			ROSE_ASSERT(ineq.getX()!=nprocsVar && ineq.getY()!=nprocsVar);
			// If either variable in the inequality is zeroVar or nprocsVar, set up their
			//    annotations appropriately since these are shared variables. The other variables
			//    will be handled by initDFfromPartCond() when we know which process set 
			//    this partition condition will belong to.
			// !!! Actually, it shouldn't be possible for either variable to be
			// !!! nprocsVar since nprocsVar is not a real application variable.
			if(ineq.getX()==zeroVar   || ineq.getY()==zeroVar ||
			   ineq.getX()==nprocsVar || ineq.getY()==nprocsVar)
			{
				varID zeroVarAllPSets, nprocsVarAllPSets;
				annotateCommonVars(/*n, */zeroVar, zeroVarAllPSets, nprocsVar, nprocsVarAllPSets);
				
				if(ineq.getX()==zeroVar)        ineq.setX(zeroVarAllPSets);
				else if(ineq.getX()==nprocsVar) ineq.setX(/*nprocsVarAllPSets*/nprocsVarPSet);
					
				if(ineq.getY()==zeroVar)        ineq.setY(zeroVarAllPSets);
				else if(ineq.getY()==nprocsVar) ineq.setY(/*nprocsVarAllPSets*/nprocsVarPSet);
			}
			
			if(MPIAnalysisDebugLevel>0)
				cout << "   ineq="<<ineq.str()<<", tgtVar="<<tgtVar.str()<<"\n";
			// Assert the current inequality in this edge's constraint graph
			edgeCG->assertCond(ineq);
			
			// --------------------------------------------------------------------------------
			// Check to see if either variable is equal to nprocsVar. If so, we need to
			// propagate these constrains to the nprocsVars of other process sets since
			// all those individual variables correspond to a single value across all processes
			int a, b, c;
			varID xPSet(ineq.getX()); xPSet.addAnnotation(getVarAnn(splitPSet), (void*)1);
			if(MPIAnalysisDebugLevel>0)
				cout << "    xPSet="<<xPSet.str()<<" nprocsVarPSet="<<nprocsVarPSet.str()<<"\n";
			if(cg->isEqVars(xPSet, nprocsVarPSet, a, b, c) && a==1 && b==1)
			{
				varAffineInequality ineqNprocsVar(ineq);
				int curPSet=0;
				for(vector<DataflowNode>::const_iterator it=n.getPSetDFNodes().begin(); it!=n.getPSetDFNodes().end(); it++, curPSet++)
				{
					// x = nprocsVarPSet && x<=y+c => nprocsVarCurPSet<=y+c
					varID nprocsVarCurPSet = nprocsVar; nprocsVarCurPSet.addAnnotation(getVarAnn(curPSet), (void*)1);
					ineqNprocsVar.setX(nprocsVarCurPSet);
					if(MPIAnalysisDebugLevel>0)
						cout << "   +ineqNprocsVar="<<ineqNprocsVar.str()<<"\n";
					// Assert the derived inequality about nprocsVar in this edge's constraint graph
					edgeCG->assertCond(ineqNprocsVar);
				}
			}
			
			varID yPSet(ineq.getY()); yPSet.addAnnotation(getVarAnn(splitPSet), (void*)1);
			if(MPIAnalysisDebugLevel>0)
				cout << "    yPSet="<<xPSet.str()<<" nprocsVarPSet="<<nprocsVarPSet.str()<<"\n";
			if(cg->isEqVars(yPSet, nprocsVarPSet, a, b, c) && a==1 && b==1)
			{
				varAffineInequality ineqNprocsVar(ineq);
				int curPSet=0;
				for(vector<DataflowNode>::const_iterator it=n.getPSetDFNodes().begin(); it!=n.getPSetDFNodes().end(); it++, curPSet++)
				{
					// y = nprocsVarPSet && x<=y+c => x<=nprocsVarCurPSet+c
					varID nprocsVarCurPSet = nprocsVar; nprocsVarCurPSet.addAnnotation(getVarAnn(curPSet), (void*)1);
					ineqNprocsVar.setY(nprocsVarCurPSet);
					if(MPIAnalysisDebugLevel>0)
						cout << "   +ineqNprocsVar="<<ineqNprocsVar.str()<<"\n";
					// Assert the derived inequality about nprocsVar in this edge's constraint graph
					edgeCG->assertCond(ineqNprocsVar);
				}
			}
			
			
			
			//const varAffineInequality& ineq = *it;
			//varAffineInequality* rankIneq;
			if(MPIAnalysisDebugLevel>0)
				cout << "edgeCG="<<edgeCG->str()<<"\n";
			
			// If possible, connect the current inequality to an inequality about tgtVar
			// and assert that new inequality
			connectIneqToVar(cg, edgeCG, ineq, tgtVar, (*ei).target());
		}

		//cout << "(*ei).target() = "<<(*ei).target().getNode()->unparseToString()<<"\n";
		//cout << "edgeCG = "<<edgeCG->str("")<<"\n";
		//splitConditions.push_back((printable*)rankIneq);
		splitConditions.push_back((ConstrGraph*)edgeCG);
	}
}

// If possible according to the inequalities in cg, connect the given inequality to an 
// inequality about tgtVar and assert that new inequality in newCG.
// Returns true if this causes cg to change, false otherwise.
bool pCFG_contProcMatchAnalysis::connectIneqToVar(ConstrGraph* cg, ConstrGraph* newCG, const varAffineInequality& ineq, 
                                                  const varID& tgtVar, DataflowNode n)
{
	int xa, xb, xc, ya, yb, yc;
	
	if(MPIAnalysisDebugLevel>0)
		cout << "        cg->isEqVars("<<ineq.getX().str()<<", "<<tgtVar.str()<<", xa, xb, xc)="<<cg->isEqVars(ineq.getX(), tgtVar, xa, xb, xc)<<" && !cg->isEqVars("<<ineq.getY().str()<<", "<<tgtVar.str()<<", ya, yb, yc)="<<cg->isEqVars(ineq.getY(), tgtVar, ya, yb, yc)<<"\n";
	
	// x*a <= y*b + c && x*xa = tgtVar*xb + xc and y not equal to tgtVar
	if(cg->isEqVars(ineq.getX(), tgtVar, xa, xb, xc) && !cg->isEqVars(ineq.getY(), tgtVar, ya, yb, yc))
	{
		// x*a <= y*b + c && x*xa = tgtVar*xb + xc
		// x*xa*a <= y*xa*b + c*xa && x*xa*a = tgtVar*xb*a + xc*a
		// tgtVar*xb*a + xc*a <= y*xa*b + c*xa
		// tgtVar*xb*a <= y*xa*b + c*xa - xc*a
		if(xb*ineq.getA() != 1 || xa*ineq.getB()!=1)
		{
			cout << "ERROR: rank expression "<<n.getNode()->unparseToString()<< " too complex!\n";
			ROSE_ASSERT(0);
		}
		
		// tgtVar <= y + c*xa - xc*a
		/*rankIneq = new varAffineInequality(tgtVar, ineq.getY(), 
		                                   1, 1, ineq.getC()*xa - xc*ineq.getA(), false, false);*/
		return newCG->assertCond(tgtVar, ineq.getY(), 1, 1, ineq.getC()*xa - xc*ineq.getA());
	}
	// x*a <= y*b + c && y*ya = tgtVar*yb + yc and x not equal to tgtVar
	else if(!cg->isEqVars(ineq.getX(), tgtVar, xa, xb, xc) && cg->isEqVars(ineq.getY(), tgtVar, ya, yb, yc))
	{
		// x*a <= y*b + c && y*ya = tgtVar*yb + yc
		// x*ya*a <= y*ya*b + c*ya && y*ya*b = tgtVar*yb*b + yc*b
		// x*ya*a <= tgtVar*yb*b + yc*b + c*ya
		if(ya*ineq.getA() != 1 || yb*ineq.getB()!=1)
		{
			cout << "ERROR: rank expression "<<n.getNode()->unparseToString()<< " too complex!\n";
			ROSE_ASSERT(0);
		}
		
		// x <= tgtVar + yc*b + c*ya
		/*rankIneq = new varAffineInequality(ineq.getX(), tgtVar, 
		                                   1, 1, yc*ineq.getB() - ineq.getC()*ya, false, false);*/
		return newCG->assertCond(ineq.getX(), tgtVar, 1, 1, yc*ineq.getB() + ineq.getC()*ya);
	}
	return false;
}

// Called when a partition is created to allow a specific analysis to initialize
// its dataflow information from the partition condition
/*void pCFG_contProcMatchAnalysis::initDFfromPartCond(const Function& func, const pCFGNode& n, int pSet, NodeState& state, 
	                                                 const vector<Lattice*>& dfInfo, const vector<NodeFact*>& facts,
	                                                 ConstrGraph* partitionCond)
{
	ConstrGraph* cg = dynamic_cast<ConstrGraph*>(dfInfo.front());
	//contRangeProcSet* nodeRankSet = dynamic_cast<contRangeProcSet*>(facts.front());
	
	ROSE_ASSERT(partitionCond);

	cg->beginTransaction();
	{
		printf("======================================\n");
		printf("Initializing partition's dataflow info\n");
		printf("         CG = %p: %s\n", cg, cg->str("").c_str());

		// Annotate all the un-annotated variables in partitionCond to mark them
		// as belonging to process set pSet
		partitionCond->addVarAnnot("", (void*)1, getVarAnn(pSet), (void*)1);
		
		printf("      && partitionCond: %p %s\n", partitionCond, partitionCond->str("").c_str());
		
		// -----------------------------------------------------------------------------------------------
		// Update this partition's constraint graph with the new constraints from partitionCond
		// We first compute the conjunction of this constraint graph with the partitionCond and then
		// widen the constraint graph with the new conjuncted constraint graph
		//cg->widenUpdateLimitToThat(partitionCond);
		//cg->meetUpdateLimitToThat(partitionCond);
		ConstrGraph* conjCG = new ConstrGraph(cg);
		conjCG->andUpd(partitionCond);
		
		printf("      conjCG = %s\n", cg->str("").c_str());
		
		cg->widenUpdate(conjCG);
		delete(conjCG);
		
		printf("      => %s\n", cg->str("").c_str());
		
		printf("      Updated rankSet:\n", cg->str("").c_str());
		printf("      ~~~~~~\n");

		inferBoundConstraints(func, n, pSet, cg);
		
		//printf("      ~~~~~> %s\n", cg->str("", false).c_str());
		printf("      => %s\n", cg->str("").c_str());
	}
	
	cg->endTransaction();
}*/

// Called when a process set or pCFGNode is partitioned to allow the specific analysis to update the
// dataflow state for that process set with the set's specific condition.
// Returns true if this causes the dataflow state to change and false otherwise
bool pCFG_contProcMatchAnalysis::initPSetDFfromPartCond(
	                            const Function& func, const pCFGNode& n, int pSet, 
	                            const vector<Lattice*>& dfInfo, const vector<NodeFact*>& facts,
	                            ConstrGraph* partitionCond)
{
	bool modified = false;
	ConstrGraph* cg = dynamic_cast<ConstrGraph*>(dfInfo.front());
//	cg->beginTransaction();
	
	modified = initPSetDFfromPartCond_ex(func, n, pSet, dfInfo, facts, partitionCond) || modified;
	
	//cg->endTransaction();
	cg->transitiveClosure();
	
	return modified;
}

// Version of initPSetDFfromPartCond that doesn't perform a transitive closure at the end
bool pCFG_contProcMatchAnalysis::initPSetDFfromPartCond_ex(
	                            const Function& func, const pCFGNode& n, int pSet, 
	                            const vector<Lattice*>& dfInfo, const vector<NodeFact*>& facts,
	                            ConstrGraph* partitionCond)
{
	bool modified = false;
	ConstrGraph* cg = dynamic_cast<ConstrGraph*>(dfInfo.front());
	
	if(MPIAnalysisDebugLevel>0)
	{
		printf("======================================\n");
		printf("Initializing dataflow info of process set %d\n", pSet);
		printf("         CG = %p: %s\n", cg, cg->str("").c_str());
	}
	
	// Annotate all the un-annotated variables in partitionCond to mark them
	// as belonging to process set pSet
	modified = partitionCond->addVarAnnot("", (void*)1, getVarAnn(pSet), (void*)1) || modified;
	
	if(MPIAnalysisDebugLevel>0)
		printf("      && partitionCond: %p %s\n", partitionCond, partitionCond->str("").c_str());
	
	// Update the constraint graph with the partition condition
	modified = cg->andUpd(partitionCond) || modified;
	
	cg->transitiveClosure();
	
	if(MPIAnalysisDebugLevel>0)
	{
		printf("      => %s\n", cg->str("").c_str());
		printf("      Updated rankSet:\n", cg->str("").c_str());
		printf("      ~~~~~~\n");
	}
	
	// Infer constraints on the process set's bounds from the constraints on its rankVars
	modified = inferBoundConstraints(func, n, pSet, cg) || modified;
	
	// Transfer constraints from other process set's nprocsVar to this process set's nprocsVar
	modified = forceNProcsUnity(func, n, pSet, cg) || modified;
	
	if(MPIAnalysisDebugLevel>0)
		printf("      => %s\n", cg->str("").c_str());
	
	return modified;
}

// Merge the dataflow information of two process sets. The space of process set IDs will be 
// compressed to remove the holes left by the removal.
// pSetMigrations (initially assumed empty) is set to indicate which process sets have moved
//    to new ids, with the key representing the process set's original id and the value entry
//    representing the new id.
// It is assumed that pSetsToMerge is a sorted list.
void pCFG_contProcMatchAnalysis::mergePCFGStates(
                     const list<int>& pSetsToMerge, const pCFGNode& n, const Function& func, 
                     const vector<Lattice*>& dfInfo, NodeState& state, map<int, int>& pSetMigrations)
{
	startProfileFunc("mergePCFGStates");
	
	ROSE_ASSERT(pSetsToMerge.size()>=2);
	ConstrGraph* cg = dynamic_cast<ConstrGraph*>(dfInfo.front());
	
	if(MPIAnalysisDebugLevel>0)
		cout << "mergePCFGStates() initial cg="<<cg->str()<<"\n";
	
	list<int>::const_iterator it=pSetsToMerge.begin();
	string rootPSetAnnot = getVarAnn(*it);
	it++;
	// Merge all the process sets other than the first with the first
	for(; it!=pSetsToMerge.end(); it++)
	{
		string childPSetAnnot = getVarAnn(*it);
		set<pair<string, void*> > noCopyAnnots;
		set<varID> noCopyVars;
		if(MPIAnalysisDebugLevel>0)
			cout << "mergePCFGStates() Merging annotations of "<<childPSetAnnot<<" into "<<rootPSetAnnot<<"\n";
		cg->mergeAnnotVars(rootPSetAnnot, (void*)1, childPSetAnnot, (void*)1,
	                      noCopyAnnots, noCopyVars);
	}
	if(MPIAnalysisDebugLevel>0)
		cout << "mergePCFGStates() post-merge cg="<<cg->str()<<"\n";
	
	// Compress any holes that result from the merging
	it=pSetsToMerge.begin();
	it++;
	int freePSet=*it;
	int usedPSet=freePSet;
	while(freePSet<(n.getPSetDFNodes().size() - (pSetsToMerge.size()-1)))
	{	
		if(MPIAnalysisDebugLevel>0)
			cout << "mergePCFGStates() A freePSet="<<freePSet<<", usedPSet="<<usedPSet<<"\n";
		
		// Find the next process set id that is still going to be used
		int nextHole;
		do
		{
			usedPSet++;
			it++;
			if(it==pSetsToMerge.end()) nextHole = n.getPSetDFNodes().size();
			else                       nextHole = *it;
		} while(usedPSet==nextHole);
		
		if(MPIAnalysisDebugLevel>0)
			cout << "mergePCFGStates() B freePSet="<<freePSet<<", usedPSet="<<usedPSet<<"\n";
		
		// usedPSet now refers to the next process set id that will be used
		
		// Move the next few used process set ids to the free spots left by merged process sets
		for(; usedPSet<nextHole; usedPSet++, freePSet++)
		{
			string usedPSetAnnot = getVarAnn(usedPSet);
			string freePSetAnnot = getVarAnn(freePSet);
			if(MPIAnalysisDebugLevel>0)
			{
				cout << "mergePCFGStates() Migrating pSet "<<usedPSet<<" to "<<freePSet<<"\n";
				cout << "mergePCFGStates() Replacing annotation "<<usedPSetAnnot<<" with "<<freePSetAnnot<<"\n";
			}
			cg->replaceVarAnnot(usedPSetAnnot, (void*)1, freePSetAnnot, (void*)1);
			
			pSetMigrations[usedPSet] = freePSet;
		}
		
		if(MPIAnalysisDebugLevel>0)
			cout << "mergePCFGStates() C freePSet="<<freePSet<<", usedPSet="<<usedPSet<<"\n";
	}
	if(MPIAnalysisDebugLevel>0)
		cout << "mergePCFGStates() post-replace cg="<<cg->str()<<"\n";
	
	endProfileFunc("mergePCFGStates");
}

// Infers the best possible constraints on the upper and lower bounds of the process set from 
// the constraints on rankVar
// Returns true if this causes the dataflow state to change and false otherwise
bool pCFG_contProcMatchAnalysis::inferBoundConstraints(const Function& func, const pCFGNode& n, int pSet, ConstrGraph* cg)
{
	bool modified = false;
	
	// Update rankSet from rankVar's current constraints. 
	// This is important for partitionConds from splits that were caused by IfStmts that depend 
	// on process rank and not important for splits that happened due to send-receive matching.
	
	// We only want to infer information from variables that belong to pSet or that are common
	set<string> tgtAnnotNames;
	tgtAnnotNames.insert(getVarAnn(pSet));
	tgtAnnotNames.insert("pCFG_common");
	
	varID rankVarPSet = rankVar; rankVarPSet.addAnnotation(getVarAnn(pSet), (void*)1);
	contRangeProcSet rankSetPSet = rankSet; // The rankSet of the process set pSet
	rankSetPSet.setConstrAnnot(cg, getVarAnn(pSet), (void*)1);
	
	// ====
	// rankVar <= var + c && !(var*a' <= rankVar*b' + c')
	// The additions that we'll be making to cg. Each entry <var, c> corresponds ub*1 = var*1 + c.
	map<varID, int> ubUpdates;
	{
		// Iterate through all constraints rankVar*a <= var*b + c
		for(ConstrGraph::leIterator leIt=cg->leBegin(rankVarPSet); leIt!=cg->leEnd(); leIt++)
		{
			varAffineInequality ineq = *leIt;
			const varID& var=ineq.getY();
			// Version of var with no annotations to get the right result from isMPIRankVarDep()
			varID varNoAnnot(var); varNoAnnot.remAllAnnotations(); 
			//cout << "rankVar <= var = "<<var.str()<<" (mpidep)="<<isMPIRankVarDep(func, n.getCurNode(pSet), var)<<" ineq = "<<ineq.str()<<"\n";
			
			// Skip the rankSet bounds since we're currently trying to reset them and they're not real variables anyhow
			if(ineq.getY()==rankSetPSet.getUB() || var==rankSetPSet.getLB()) continue;
//cout << "var = "<<var.str()<<"\n";
			// Skip any rankVar-dependent variables
			if(isMPIRankVarDep(func, n.getCurNode(pSet), varNoAnnot)) continue;
			// Skip variables that are not related to this process set
			//if(!var.hasAnyAnnotation(tgtAnnotNames)) continue;
			
			// Variables varUB s.t. varUB*a = rankSet.ub*b + d (a==1, b==1)
			map<varID, affineInequality> ubEqVars = cg->getEqVars(rankSetPSet.getUB());
			for(map<varID, affineInequality>::iterator it=ubEqVars.begin(); it!=ubEqVars.end(); it++)
			{
				//cout << "          ineq="<<ineq.str()<<" ub="<<it->first.str()<<" : "<<it->second.str()<<"\n";
				//cout << "              it->second.getA()=="<<it->second.getA()<<" && it->second.getB()="<<it->second.getB()<<" cg->lteVars("<<
				//        var.str()<<", "<<it->first.str()<<", 1, 1, "<<(it->second.getC() - ineq.getC())<<")="<<cg->lteVars(var, it->first, 1, 1, it->second.getC() - ineq.getC())<<"\n";
				
				// if ub=q+d && var+c<=q+d == var<=q+d-c then ub = var+c
				if(it->second.getA()==1 && it->second.getB()==1 &&
				   cg->lteVars(var, it->first, 1, 1, it->second.getC() - ineq.getC()))
				{
					if(MPIAnalysisDebugLevel>0)
						cout << ":::Asserting "<<rankSetPSet.getUB().str()<<" = "<<var.str()<<" + "<<ineq.getC()<<"\n";
					// Insert the new assignment into ubUpades, ensuring that all the variables in ubUpdates
					// are equal to each other and are the smallest thing that we can assign ub to
					addToBoundUpdatesMap(cg, ubUpdates, var, ineq.getC(), true);
				}
			}
		}
		modified = (ubUpdates.size()>0) || modified;
	}
			
	if(MPIAnalysisDebugLevel>0) printf("      ~~~ = ~~~\n");
	// ====
	// var <= rankVar + c && !(rankVar*a' <= var*b' + c')
	// The additions that we'll be making to cg. Each entry <var, c> corresponds lb*1 = var*1 + c
	map<varID, int> lbUpdates;
	{
		//cout << "LB: rankVarPSet="<<rankVarPSet.str()<<"\n";
		
		// Iterate through all constraints var*a <= rankVar*b + c
		for(ConstrGraph::geIterator geIt=cg->geBegin(rankVarPSet); geIt!=cg->geEnd(); geIt++)
		{
			varAffineInequality ineq = *geIt;
			const varID& var=ineq.getX();
			// Version of var with no annotations to get the right result from isMPIRankVarDep()
			varID varNoAnnot(var); varNoAnnot.remAllAnnotations(); 
			if(MPIAnalysisDebugLevel>0)
				cout << "var = "<<var.str()<<" <= rankVar + ineq = "<<ineq.str()<<"\n";
			
			// Skip the rankSet bounds since we're currently trying to reset them and they're not real variables anyhow
			if(var==rankSetPSet.getUB() || var==rankSetPSet.getLB()) continue;
			// Skip any rankVar-dependent variables
			if(isMPIRankVarDep(func, n.getCurNode(pSet), varNoAnnot)) continue;
			// Skip variables that are not related to this process set
			//if(!var.hasAnyAnnotation(tgtAnnotNames)) continue;
			
			// Variables varLB s.t. varLB*a = rankSet.lb*b + d (a==1, b==1)
			map<varID, affineInequality> lbEqVars = cg->getEqVars(rankSetPSet.getLB());
			for(map<varID, affineInequality>::iterator it=lbEqVars.begin(); it!=lbEqVars.end(); it++)
			{
				//cout << "          ineq="<<ineq.str()<<" lb="<<it->first.str()<<" : "<<it->second.str()<<"\n";
				//cout << "              it->second.getA()=="<<it->second.getA()<<" && it->second.getB()="<<it->second.getB()<<" cg->lteVars("<<var.str()<<", "<<it->first.str()<<", 1, 1, "<<(0-it->second.getC() - ineq.getC())<<")="<<cg->lteVars(var, it->first, 1, 1, 0-it->second.getC() - ineq.getC())<<"\n";
				// if lb=q+d && lb=q+d<=var-c<=rankVar == q<=var-c-d then lb = var-c
				if(it->second.getA()==1 && it->second.getB()==1 &&
				   cg->lteVars(var, it->first, 1, 1, 0 - it->second.getC() - ineq.getC()))
				{
					if(MPIAnalysisDebugLevel>0)
						cout << ":::Asserting "<<rankSetPSet.getLB().str()<<" = "<<var.str()<<" + "<<(0-ineq.getC())<<"\n";
					// Insert the new assignment into ubUpades, ensuring that all the variables in ubUpdates
					// are equal to each other and are the smallest thing that we can assign ub to
					addToBoundUpdatesMap(cg, lbUpdates, var, 0-ineq.getC(), false);
				}
			}
		}
		modified = (lbUpdates.size()>0) || modified;
		
	}
	
	// Perform the updates to the upper bound
	performBoundUpdates(cg, ubUpdates, rankSetPSet.getUB());
	
	// Perform the updates to the lower bound
	performBoundUpdates(cg, lbUpdates, rankSetPSet.getLB());

	// lb <= ub
	modified = cg->assertCond(rankSetPSet.getLB(), rankSetPSet.getUB(), 1, 1, 0) || modified;
	
	return modified;
}

// Transfer constraints from other process set's nprocsVar to this process set's nprocsVar
// Returns true if this causes the dataflow state to change and false otherwise
bool pCFG_contProcMatchAnalysis::forceNProcsUnity(const Function& func, const pCFGNode& n, int pSet, ConstrGraph* cg)
{
	bool modified = false;
	varID nprocsVarPSet = nprocsVar; nprocsVarPSet.addAnnotation(getVarAnn(pSet), (void*)1);
	
	// Look at the constraints on the nprocsVars of other sets. For any constraint
	// nprocsVarOtherSet <= var, intersect this constraint with the nprocsVarPSet <= var constraint (if any)
	int otherPSet=0;
	for(vector<DataflowNode>::const_iterator it=n.getPSetDFNodes().begin(); it!=n.getPSetDFNodes().end(); it++, otherPSet++)
	{
		if(otherPSet != pSet)
		{
			varID nprocsVarOtherSet = nprocsVar; nprocsVarOtherSet.addAnnotation(getVarAnn(otherPSet), (void*)1);
			
			// Iterate through all constraints of the form nprocsVarOtherSet <= var
			for(ConstrGraph::leIterator leIt=cg->leBegin(nprocsVarOtherSet); leIt!=cg->leEnd(); leIt++)
			{
				varAffineInequality ineq = *leIt;
				ROSE_ASSERT(ineq.getX() == nprocsVarOtherSet);
				
				if(ineq.getY() != nprocsVarPSet)
				{
					// Modify the current inequality to refer to nprocsVarPSet instead of nprocsVarOtherSet
					ineq.setX(nprocsVarPSet);
									
					if(MPIAnalysisDebugLevel>0)
						cout << "||| Transferring "<<ineq.str()<<" from pSet "<<otherPSet<<" to pSet "<<pSet<<"\n";
					
					// Update the constraint graph with the new info about nprocsVarPSet
					modified = cg->assertCond(ineq) || modified;
				}
			}
		}
	}
	
	return modified;
}


// Inserts the given var + c value into the updates map. This map contains expressions var + c to which we'll
// assign the process set upper or lower bound (upper if ub=true, lower if ub=false).
void pCFG_contProcMatchAnalysis::addToBoundUpdatesMap(ConstrGraph* cg, map<varID, int>& updates, const varID& var, int c, bool ub)
{
	// Identify the relationships between var+c and the expressions in updates
	bool allEq = true;   // true if var+c is equal to the expressions in updates, false otherwise
	bool allIneq = true; // true if var+c is tighter than the expressions in updates (var <= others if ub=true, 
	                     // var >= others if ub=false), false otherwise
	for(map<varID, int>::iterator itUpd=updates.begin(); itUpd!=updates.end(); itUpd++)
	{
		if(allEq && !cg->eqVars(var, itUpd->first, 1, 1, itUpd->second - c))
			allEq = false;
		if(allIneq &&
			((ub && !cg->lteVars(var, itUpd->first, 1, 1, itUpd->second - c)) ||
			 (!ub && !cg->lteVars(itUpd->first, var, 1, 1, c - itUpd->second))))
			allIneq = false;
	}
	// If the current variable is equal to all the other variables that we'll be assigning ub to, add it to the list
	if(allEq) 
	{
		if(MPIAnalysisDebugLevel>0)
			cout << "    Equal to others. Accept.\n";
		updates[var] = c;
	}
	// If not, but this variable is <= all the other variables, then it is a strictly tighter fit than these variables
	// and will be chosen instead of those variables
	else if(allIneq)
	{
		if(MPIAnalysisDebugLevel>0)
			cout << "    Tighter than others. Replace.\n";
		updates.clear();
		updates[var] = c;
	}
	else
	{
		if(MPIAnalysisDebugLevel>0)
			cout << "    Not Equal to or Tighter than others. Reject.\n";
	}
}

// Perform the given updates to the given node set bound variable. The updates are expressed
// as a list of <x, c> pairs (a map) and the updates to cg will be:
//    - erase boundVar's constraints in cg
//    - for each <x, c> in updates, add the boundVar = x + c constraint to cg
void pCFG_contProcMatchAnalysis::performBoundUpdates(ConstrGraph* cg, map<varID, int>& updates, const varID& boundVar)
{
	// If we're updating from an if statement that depends on the process rank,
	// the number of updates must be >0. However, we don't know when this is the case
	// by simply looking at the checkpoint, so we can't check for this.
	//ROSE_ASSERT(updates.size()>0);
	if(updates.size()>0)
	{
		map<varID, int>::iterator itUpd=updates.begin();
		cg->assign(boundVar, itUpd->first, 1, 1, itUpd->second);
		if(MPIAnalysisDebugLevel>0)
			cout << ";;;Assigning "<<boundVar.str()<<" = "<<itUpd->first.str()<<" + "<< itUpd->second<<"\n";
		itUpd++;
		for(; itUpd!=updates.end(); itUpd++)
		{
			cg->assertEq(boundVar, itUpd->first, 1, 1, itUpd->second);
			if(MPIAnalysisDebugLevel>0)
				cout << ";;;Asserting "<<boundVar.str()<<" = "<<itUpd->first.str()<<" + "<< itUpd->second<<"\n";
		}
	}
}

/*
// Inserts the given <varA, varB, ineq> tuple into the given map
void pCFG_contProcMatchAnalysis::addVarVarIneqMap(map<varID, map<varID, affineInequality> >& vvi, 
	                                         const varID& varA, const varID& varB, const affineInequality& ineq)
{
	//cout << "addVarVarIneqMap("<<varA.str()<<", "<<varB.str()<<", "<<ineq.str()<<"\n";
	map<varID, map<varID, affineInequality> >::iterator itA = vvi.find(varA);
	if(itA != vvi.end())
	{
		//cout << "    "<<varA.str()<<" found\n";
		map<varID, affineInequality>::iterator itB = itA->second.find(varB);
		if(itB != itA->second.end())
		{
			//cout << "    "<<varB.str()<<" found\n";
			itB->second = ineq;
		}
		else
		{
			//cout << "    "<<varB.str()<<" not found\n";
			itA->second[varB] = ineq;
		}
	}
	else
	{
		//cout << "    "<<varA.str()<<" found\n";
		map<varID, affineInequality> newMap;
		newMap[varB] = ineq;
		vvi[varA] = newMap;
	}
}*/

// Incorporates the current node's inequality information from conditionals (ifs, fors, etc.) into the current node's 
// constraint graph.
// returns true if this causes the constraint graph to change and false otherwise
bool pCFG_contProcMatchAnalysis::incorporateConditionalsInfo(const pCFGNode& n, int pSet, const Function& func, const DataflowNode& dfNode, 
                                                             NodeState& state, const vector<Lattice*>& dfInfo)
{
	bool modified = false;
	ConstrGraph* cg = dynamic_cast<ConstrGraph*>(dfInfo.front());
	
	const set<varAffineInequality>& ineqs = getAffineIneq(dfNode);
	if(MPIAnalysisDebugLevel>0)
		printf("    incorporateConditionalsInfo(), ineqs.size()=%d\n", ineqs.size());
	//cout << "Node <"<<n.getNode()->class_name()<<" | "<<dfNode.getNode()->unparseToString()<<">\n";
	for(set<varAffineInequality>::const_iterator it = ineqs.begin(); it!=ineqs.end(); it++)
	{
		varAffineInequality varIneq = *it;
		
		// Update varIneq's variables with the annotations that connect them to pSet
		connectVAItoPSet(pSet, varIneq);
		
		//cout << "        it="<<(*it).str()<<"\n";
		if(MPIAnalysisDebugLevel>0)
			cout << "incorporateConditionalsInfo() Asserting "<<varIneq.getIneq().str(varIneq.getX(), varIneq.getY(), "    ") << "\n";
		modified = cg->assertCond(varIneq.getX(), varIneq.getY(), varIneq.getIneq().getA(), 
		                          varIneq.getIneq().getB(), varIneq.getIneq().getC()) || modified;
		
		// If this inequality says something about nprocsVar, assert this nprocsVar inequality in vs
		modified = connectIneqToVar(cg, cg, varIneq, nprocsVar, dfNode) || modified;
	}
	return modified;
}

// incorporates the current node's divisibility information into the current node's constraint graph
// returns true if this causes the constraint graph to change and false otherwise
bool pCFG_contProcMatchAnalysis::incorporateDivInfo(const Function& func, const DataflowNode& n, NodeState& state, const vector<Lattice*>& dfInfo)
{
	bool modified = false;
	ConstrGraph* cg = dynamic_cast<ConstrGraph*>(dfInfo.front());
	
	printf("    incorporateDivInfo(%s)\n", func.get_name().str());
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
		if(varDivL->getLevel() == DivLattice::divKnown/* && !(varDivL->getDiv()==1 && varDivL->getRem()==0)* /)
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
bool pCFG_contProcMatchAnalysis::removeConstrDivVars(const Function& func, const DataflowNode& n, NodeState& state, const vector<Lattice*>& dfInfo)
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
		if(varDivL->getLevel() == DivLattice::divKnown/* && !(varDivL->getDiv()==1 && varDivL->getRem()==0)* /)
		{
			cg->disconnectDivOrigVar(var, varDivL->getDiv(), varDivL->getRem());
		}
		else if(varDivL->getLevel() != DivLattice::bottom)
		{
			cg->disconnectDivOrigVar(var, 1, 0);
		}*/
	}
	
	if(MPIAnalysisDebugLevel>0)
		cout << cg->str("    ") << "\n";
	
	return modified;
}

// Creates a new instance of the derived object that is a copy of the original instance.
// This instance will be used to instantiate a new partition of the analysis.
/*IntraPartitionDataflow* pCFG_contProcMatchAnalysis::copy()
{
	return new pCFG_contProcMatchAnalysis(*this);
}*/

// Performs send-receive matching on a fully-blocked analysis partition. 
// If some process sets need to be split, returns the set of checkpoints that corresponds to this pCFG node's descendants.
// Otherwise, returns an empty set.
// Even if no split is required, matchSendsRecvs may modify activePSets and blockedPSets to release some process sets
//    that participated in a successful send-receive matching.
/*set<pCFG_Checkpoint*> pCFG_contProcMatchAnalysis::matchSendsRecvs(
                                                     const pCFGNode& n, NodeState* state, 
                                                     set<int>& activePSets, set<int>& blockedPSets, set<int>& releasedPSets,
                                                     const Function& func, NodeState* fState)*/
void pCFG_contProcMatchAnalysis::matchSendsRecvs(
	                     const pCFGNode& n, const vector<Lattice*>& dfInfo, NodeState* state, 
	                     // Set by analysis to identify the process set that was split
	                     int& splitPSet,
	                     vector<ConstrGraph*>& splitConditions, 
	                     vector<DataflowNode>& splitPSetNodes,
	                     // for each split process set, true if its active and false if it is blocked
	                     vector<bool>&         splitPSetActive,
	                     // for each process set that was not split, true if becomes active as a result of the match,
	                     // false if its status doesn't change
	                     vector<bool>&         pSetActive,
	                     const Function& func, NodeState* fState)
{
	startProfileFunc("matchSendsRecvs");
	//ROSE_ASSERT(activePSets.size()==0);
	//ROSE_ASSERT(blockedPSets.size()==n.getPSetDFNodes().size());
	
	/*map<IntraPartitionDataflow*, OneDmesgExpr> sends;
	map<IntraPartitionDataflow*, OneDmesgExpr> recvs;*/
	map<int, pair<OneDmesgExpr, DataflowNode> > sends;
	map<int, pair<OneDmesgExpr, DataflowNode> > recvs;
	
	DataflowNode funcCFGEnd = cfgUtils::getFuncEndCFG(func.get_definition());

	ConstrGraph* cg = dynamic_cast<ConstrGraph*>(state->getLatticeAbove((pCFG_FWDataflow*)this, 0));
	// We operate on a copy of cg since we don't want to modify the original with
	// the temporaty process sets that we create
	cg = new ConstrGraph(cg);
	
	if(MPIAnalysisDebugLevel>0)
	{
		cout << "============================\n";
		cout << "matchSendsRecvs() n="<<n.str()<<"\n";
		cout << "matchSendsRecvs() cg="<<cg->str()<<"\n";
	}

	// -------------------------------------------------------------------------------------------------
	// Look over all the partitions in this split set to see whether all the active dataflow nodes 
	// in the partition are blocked on a send or receive and record their respective message expressions
	// in sends and recvs.
	int curPSet=0;
	for(vector<DataflowNode>::const_iterator it=n.getPSetDFNodes().begin(); it!=n.getPSetDFNodes().end(); it++, curPSet++)
	{
		const DataflowNode&  dfNode = *it;
		
		if(MPIAnalysisDebugLevel>0)
			cout << "Process set "<<curPSet << " : ";
		
		// Records whether all the DataflowNodes on which the current partition is blocked are all sends, all receives 
		// or some mixture, in which case we have a conflict
		bool allSends=false, allRecvs=false;
		
		if(MPIAnalysisDebugLevel>0)
			cout << "    matchSendsRecvs() n = "<<dfNode.str()<<"\n";
		ROSE_ASSERT(!cg->isBottom());
		
		// Skip over process sets that are blocked on the end of the function
		if(dfNode == funcCFGEnd)
		{
			if(MPIAnalysisDebugLevel>0) cout << "blocked on function end\n"; 
			continue;
		}
		
		SgFunctionCallExp* fnCall = isSgFunctionCallExp(dfNode.getNode());
		ROSE_ASSERT(fnCall);
		Function calledFunc(fnCall);
		
		if(calledFunc.get_name().getString() == "MPI_Send" || calledFunc.get_name().getString() == "MPI_Recv")
		{
			SgExpressionPtrList& args = fnCall->get_args()->get_expressions();
			SgExpression* procArg = *(++(++(++args.begin())));
			if(calledFunc.get_name().getString() == "MPI_Send")
			{
				OneDmesgExpr mesgExpr(procArg, cg, curPSet);
				mesgExpr.setConstr(cg);
				pair<OneDmesgExpr, DataflowNode> md(mesgExpr, dfNode);
				sends.insert(make_pair(curPSet, md));
				if(MPIAnalysisDebugLevel>0)
					cout << "sending "<<mesgExpr.str()<<"\n";
			}
			else if(calledFunc.get_name().getString() == "MPI_Recv")
			{
				OneDmesgExpr mesgExpr(procArg, cg, curPSet);
				mesgExpr.setConstr(cg);
				pair<OneDmesgExpr, DataflowNode> md(mesgExpr, dfNode);
				recvs.insert(make_pair(curPSet, md));
				if(MPIAnalysisDebugLevel>0)
					cout << "receiving "<<mesgExpr.str()<<"\n";
			}
		}
		else
		{
			if(MPIAnalysisDebugLevel>0)
				cout << "    calledFunc.get_name().getString() = "<<calledFunc.get_name().getString()<<"\n";
			ROSE_ASSERT(calledFunc.get_name().getString() == "MPI_Send" ||
			            calledFunc.get_name().getString() == "MPI_Recv");
		}
		
		/*
		// Look at the dataflow state at the end of the function to check for any previously sent messages
		{
			DataflowNode funcCFGEnd = cfgUtils::getFuncEndCFG(s->func.get_definition());
			NodeState* ns = NodeState::getNodeState(funcCFGEnd);
			ConstrGraph* cg = dynamic_cast<ConstrGraph*>(ns->getLatticeAbove(analysis, 0));
			
		}*/
	}
	
	// -------------------------------------------------------------------------------
	// If we're not yet finished with the current function because some dataflow nodes 
	// are blocked on a send or receive
	if(sends.size()>0 || recvs.size()>0)
	{
		// If we only have sends we have a potential deadlock situation
		if(recvs.size()==0)
		{
			if(MPIAnalysisDebugLevel>0)
				cout << "ERROR: No receives during send-receive matching of partition. Possible deadlock!\n";
			//ROSE_ASSERT(recvs.size()>0);
			endProfileFunc("matchSendsRecvs");
			return;
		}
		// If we only have receives, we have a definite deadlock
		else if(sends.size()==0)
		{
			if(MPIAnalysisDebugLevel>0)
				cout << "ERROR: No sends during send-receive matching of partition. Deadlock!\n";
			//ROSE_ASSERT(sends.size()>0);
			endProfileFunc("matchSendsRecvs");
			return;
		}
		
		if(MPIAnalysisDebugLevel>0)
			cout << "sends.size()="<<sends.size()<<"  recvs.size()="<<recvs.size()<<"\n";
	
		// ---------------------------------------------------------
		// Now match all send expressions to all receive expressions
		
		// Iterate over all pairs of sender and receiver partitions to identify any possible
		// matches. Whenever a given sender/receiver pair successfully matches, the corresponding
		// matches are removed from their respective message expressions and will not be used for
		// subsequent matches.
		for(map<int, pair<OneDmesgExpr, DataflowNode> >::iterator sIt=sends.begin(); sIt!=sends.end(); sIt++)
		{
			int sender = sIt->first;
			contRangeProcSet sRank = rankSet;
			sRank.setConstr(cg);
			sRank.addAnnotation(getVarAnn(sender), (void*)1);
			const OneDmesgExpr& sMesg = sIt->second.first;
			const DataflowNode& senderDF = sIt->second.second;
			if(MPIAnalysisDebugLevel>0)
			{
				cout << "Sender set "<<sender<<": \n";
				cout << "    sMesg="<<sMesg.str()<<"\n";
				cout << "    sRank="<<sRank.str()<<"\n";
			}
			
			for(map<int, pair<OneDmesgExpr, DataflowNode> >::iterator rIt=recvs.begin(); rIt!=recvs.end(); )
			{
				int receiver = rIt->first;
				contRangeProcSet rRank = rankSet; 
				rRank.setConstr(cg);
				rRank.addAnnotation(getVarAnn(receiver), (void*)1);
				
				const OneDmesgExpr& rMesg = rIt->second.first;
				const DataflowNode& receiverDF = rIt->second.second;
				if(MPIAnalysisDebugLevel>0)
				{
					cout << "Receiver set "<<receiver<<": \n";
					cout << "    rMesg="<<rMesg.str()<<"\n";
					cout << "    rRank="<<rRank.str()<<"\n";
					/*cout << "sRank.getConstr()="<<sRank.getConstr()<<"\n";
					cout << "sMesg.getConstr()="<<sMesg.getConstr()<<"\n";
					cout << "rRank.getConstr()="<<rRank.getConstr()<<"\n";
					cout << "rMesg.getConstr()="<<rMesg.getConstr()<<"\n";*/
				}
				
				ROSE_ASSERT(sender != receiver);
				
				// The sets of successful/unsuccessful senders and receivers
				contRangeProcSet *senders=NULL,   *nonSenders=NULL;
				contRangeProcSet *receivers=NULL, *nonReceivers=NULL;
				
				// -------------------------------------------------------------
				// Compute the set of processes that successfully sent a message
				senders = dynamic_cast<contRangeProcSet*>(&(sMesg.getIdentityDomain(rMesg, sMesg, sRank)));
				if(MPIAnalysisDebugLevel>0)
					cout << "    senders="<<senders->str()<<"\n";
				ROSE_ASSERT(senders->validSet());
				if(!senders->emptySet())
				{
					// Processes that did not send a message
					nonSenders = dynamic_cast<contRangeProcSet*>(&(sRank.rem(*senders)));
					if(MPIAnalysisDebugLevel>0)
						cout << "    nonSenders="<<nonSenders->str()<<"\n";
					ROSE_ASSERT(nonSenders->validSet());
					
					// Processes that successfully received the message
					receivers = dynamic_cast<contRangeProcSet*>(&(sMesg.getImage(*senders)));
					if(MPIAnalysisDebugLevel>0)
						cout << "    receivers="<<receivers->str()<<"\n";
					ROSE_ASSERT(receivers->validSet());
		
					// Processes that did not receive the message
					nonReceivers = dynamic_cast<contRangeProcSet*>(&(rRank.rem(*receivers)));
					if(MPIAnalysisDebugLevel>0)
						cout << "    nonReceivers="<<nonReceivers->str()<<"\n";
					ROSE_ASSERT(nonReceivers->validSet());
				}
				else
				{
					senders      = new contRangeProcSet(); senders->emptify();
					nonSenders   = new contRangeProcSet(); nonSenders->emptify();
					receivers    = new contRangeProcSet(); receivers->emptify();
					nonReceivers = new contRangeProcSet(); nonReceivers->emptify();
				}
				
				// Only bother if some senders got matched to some receivers
				if(!senders->emptySet())
				{
					ROSE_ASSERT(!receivers->emptySet());
					
					if(MPIAnalysisDebugLevel>0)
						cout << "nonSenders->emptySet()="<<nonSenders->emptySet()<<" nonReceivers->emptySet()="<<nonReceivers->emptySet()<<"\n";
					
					// Process sets that will not be used to define the process sets that result from the split
					set<contRangeProcSet> otherProcs;
					otherProcs.insert(sMesg.mRange);
					otherProcs.insert(rMesg.mRange);
					
					// Perform appropriate set splits (if senders/receivers and nonSenders/nonReceivers are both !=emptySet)
					// Members of senders/receivers get released from the join and deleted from sends/recvs
					// Members of nonSenders/nonReceivers stay in the join and will see be candidates for subsequent matches.
					
					// If all senders in sRank were matched but not all receivers in rRank were, 
					// split the set of receivers, move on to the next set of senders
					if(nonSenders->emptySet() && !nonReceivers->emptySet())
					{
						/*set<pCFG_Checkpoint*> partitionChkpts = splitProcs(n, cg, receiver, receiverDF, activePSets, blockedPSets, releasedPSets,
                                                                     *receivers, *nonReceivers, *senders, *nonSenders,
                                                                     func, fState);*/
                  otherProcs.insert(*senders);
						otherProcs.insert(*nonSenders);
						
						splitProcs(n, cg, receiverDF, receiver, 
						           splitConditions, splitPSetNodes, splitPSetActive, 
						           *receivers, *nonReceivers, otherProcs);
						
						// Identify the set of receiver processes as the set that was split
						splitPSet = receiver;
                  // Release the senders set and leave the other process sets alone
                  int curPSet=0;
                  if(MPIAnalysisDebugLevel>0)
                  	cout << "matchSendsRecvs: Adding to pSetActive, sender="<<sender<<", pSetActive.size()="<<pSetActive.size()<<" n.getPSetDFNodes().size()="<<n.getPSetDFNodes().size()<<"\n";
						for(vector<DataflowNode>::const_iterator it=n.getPSetDFNodes().begin(); 
						    it!=n.getPSetDFNodes().end(); it++, curPSet++)
						{
							if(curPSet == sender)  pSetActive.push_back(true);
							else                   pSetActive.push_back(false);
						}
						if(MPIAnalysisDebugLevel>0)
							cout << "matchSendsRecvs: After pSetActive.size()="<<pSetActive.size()<<"\n";
						// !!! Currently do not support making more than one match per call to matchSendsRecvs()
						//return partitionChkpts;
						delete cg;
						endProfileFunc("matchSendsRecvs");
						return;
					}
					// If all receivers were matched but not all senders in sRank, 
					// split the set of senders, move on to the next set of receivers in rRank
					else if(nonReceivers->emptySet() && !nonSenders->emptySet())
					{
						/*set<pCFG_Checkpoint*> partitionChkpts = splitProcs(n, cg, sender, senderDF, activePSets, blockedPSets, releasedPSets, 
                                                                     *senders, *nonSenders, *receivers, *nonReceivers, 
                                                                     func, fState);*/
                  otherProcs.insert(*receivers);
						otherProcs.insert(nonReceivers);
						
                  splitProcs(n, cg, senderDF, sender, 
                             splitConditions, splitPSetNodes, splitPSetActive, 
                             *senders, *nonSenders, otherProcs);
						
						// Identify the set of sender processes as the set that was split
						splitPSet = sender;
						// Release the receivers set and leave the other process sets alone
                  int curPSet=0;
						for(vector<DataflowNode>::const_iterator it=n.getPSetDFNodes().begin(); 
						    it!=n.getPSetDFNodes().end(); it++, curPSet++)
						{
							if(curPSet == receiver) pSetActive.push_back(true);
							else                    pSetActive.push_back(false);
						}
						
						// !!! Currently do not support making more than one match per call to matchSendsRecvs()
						//return partitionChkpts;
						delete cg;
						endProfileFunc("matchSendsRecvs");
						return;
					}
					// If a subset of senders in sRank was matched to a subset of receivers in rRank, 
					// split both sets into all four combinations
					else if(!nonSenders->emptySet() && !nonReceivers->emptySet())
					{
						if(MPIAnalysisDebugLevel>0) cout << "[commTopoAnal] pCFG_contProcMatchAnalysis::matchSendsRecvs() ERROR: currently do not support splitting both the set of senders and receivers!\n";
						ROSE_ASSERT(0);
					}
					// If all senders in sRank were matched to all receivers in rRank, move on to the next set of senders and receivers
					else
					{
						// Release both the sender and the receiver process sets
						//releasePSet(sender, activePSets, blockedPSets, releasedPSets);
						//releasePSet(receiver, activePSets, blockedPSets, releasedPSets);
						
						// !!! Currently do not support making more than one match per call to matchSendsRecvs()
						/*set<pCFG_Checkpoint*> noChkpts;
						return noChkpts;*/
						ROSE_ASSERT(0);
						delete cg;
						endProfileFunc("matchSendsRecvs");
						return;
					}
				}
				if(MPIAnalysisDebugLevel>0)
					cout << "Done with Receiver "<<receiver<<"\n"; fflush(stdout);
			}
			if(MPIAnalysisDebugLevel>0)
				cout << "Done with Sender "<<sender<<"\n"; fflush(stdout);
		} // for(sIt=sends.begin(); sIt!=sends.end(); sIt++)
	} // if(sends.size()>0 || recvs.size()>0)
	
	if(MPIAnalysisDebugLevel>0)
		printf("matchSendsRecvs made no matches\n");
	/*set<pCFG_Checkpoint*> noChkpts;
	return noChkpts;*/
	
	delete cg;
	
	endProfileFunc("matchSendsRecvs");
}

// Release the given process set by moving it from from blockedPSets to activePSets and releasedPSets
void pCFG_contProcMatchAnalysis::releasePSet(int pSet, set<int>& activePSets, set<int>& blockedPSets, set<int>& releasedPSets)
{
	pCFG_FWDataflow::movePSet(pSet, activePSets, blockedPSets);
	releasedPSets.insert(pSet);
}

// Split the given partition into released processes and blocked processes, using the given process sets. All process
//    sets are assumed to use the cg constraint graph, which contains information about all of them.
//    otherProcs1 and otherProcs2 are the remaining two sets of processes (if released/blocked are senders, then these are
//    the receivers and vice versa).
// Returns the set of checkpoints for partitions that result from this split
/*set<pCFG_Checkpoint*> void pCFG_contProcMatchAnalysis::splitProcs(
                              const pCFGNode& n, ConstrGraph* cg, int splitPSet, const DataflowNode& dfNode,
                              set<int>& activePSets, set<int>& blockedPSets, set<int>& releasedPSets, 
                              int splitPSet,
                              vector<ConstrGraph*> splitConditions, 
                              vector<DataflowNode> splitPSetNodes,
                              const contRangeProcSet& releasedProcs, const contRangeProcSet& blockedProcs, 
                              const contRangeProcSet& otherProcs1, const contRangeProcSet& otherProcs2,
                              const Function& func, NodeState* fState)*/
void pCFG_contProcMatchAnalysis::splitProcs(
                              const pCFGNode& n, ConstrGraph* cg, const DataflowNode& dfNode, int splitPSet, 
                              vector<ConstrGraph*>& splitConditions, vector<DataflowNode>& splitPSetNodes,
                              // for each split process set, true if its active and false if it is blocked
	                     		vector<bool>&         splitPSetActive,
                              const contRangeProcSet& releasedProcs, const contRangeProcSet& blockedProcs,
                              const set<contRangeProcSet>& otherProcs)
{
	if(MPIAnalysisDebugLevel>0)
		cout << "=== splitProcs ===\n";
	
   // Remove the splitPSet's process set bounds, as well as the the rankVar and 
   //    nprocsVar variables from the constraint graph because we're about 
   //    to split this process set into the set of released and blocked processes
   //    and we don't want to drag the constraints on the original process 
   //    set's rank bounds into the new split process sets.
   varID zeroVarAllPSets, nprocsVarAllPSets;
	annotateCommonVars(/*n, */zeroVar, zeroVarAllPSets, nprocsVar, nprocsVarAllPSets);
   contRangeProcSet splitRankSet = rankSet;
   splitRankSet.setConstrAnnot(cg, getVarAnn(splitPSet), (void*)1);
   splitRankSet.cgDisconnect();
   varID rankVarSplitPSet = rankVar;     rankVarSplitPSet.addAnnotation(getVarAnn(splitPSet), (void*)1);
	varID nprocsVarSplitPSet = nprocsVar; nprocsVarSplitPSet.addAnnotation(getVarAnn(splitPSet), (void*)1);
   cg->removeScalar(rankVarSplitPSet);
   //cg->removeScalar(nprocsVarSplitPSet);
	
	if(MPIAnalysisDebugLevel>0)
	{
		cout << "releasedProcs="<<releasedProcs.str()<<"\n";
		cout << "blockedProcs="<<blockedProcs.str()<<"\n";
		/*cout << "otherProcs1="<<otherProcs1.str()<<"\n";
		cout << "otherProcs2="<<otherProcs2.str()<<"\n";*/
	}
	
	// Create the partition condition for both the released and blocked process sets
	if(MPIAnalysisDebugLevel>0)
		cout << "Released: \n";
	ConstrGraph* relCG_proj = createPSetCondition(cg, splitPSet, releasedProcs, blockedProcs, otherProcs);
	if(MPIAnalysisDebugLevel>0)
		cout << "Blocked: \n";
	ConstrGraph* blockCG_proj = createPSetCondition(cg, splitPSet, blockedProcs, releasedProcs, otherProcs);
	
	// Both process sets will start at the same DataflowNodes
	splitPSetNodes.push_back(dfNode);
	splitPSetNodes.push_back(dfNode);
	
	// Record the partition conditions the blocked and released process sets
	splitConditions.push_back(relCG_proj);
	splitConditions.push_back(blockCG_proj);
	
	// The released process set becomes active, the blocked one will be blocked
	splitPSetActive.push_back(true);
	splitPSetActive.push_back(false);
}

// Create the partition condition constraint graph for process splitPSet's rank bounds being turned 
//    into then tgtProcs set and return it.
// otherProcs and otherProcsSet contain the other relevant process sets whose bounds need to be removed 
//    from the partition condition's constraint graph.
ConstrGraph* pCFG_contProcMatchAnalysis::createPSetCondition(
                                 ConstrGraph* cg, 
                                 int splitPSet, const contRangeProcSet& tgtProcs, 
                                 const contRangeProcSet& otherProcs, const set<contRangeProcSet>& otherProcsSet)
{
	varID zeroVarAllPSets, nprocsVarAllPSets;
	annotateCommonVars(/*n, */zeroVar, zeroVarAllPSets, nprocsVar, nprocsVarAllPSets);
	
	// Get a version of cg that only contains the constraints that relate to the releasedProcs set
	varIDSet rankSetVars;
	rankSetVars.insert(tgtProcs.getLB());
	rankSetVars.insert(tgtProcs.getUB());
	ConstrGraph* projCG = cg->getProjection(rankSetVars);
	
	projCG->beginTransaction();
	
	// Replace the bounds variables of the tgtProcs set with the standard rankSet.
	// This way, when we use projCG as a partition condition, we'll be making a statement
	// about rankSet rather than some random set.
	projCG->replaceVar(tgtProcs.getLB(), rankSet.getLB());
	projCG->replaceVar(tgtProcs.getUB(), rankSet.getUB());
	
	// Clean out all references to the original split set by removing its annotations
	// from its variables. initPSetDFfromPartCond() will later add these appropriately
	// when we pick the process set ID that this partition will go to.
	projCG->removeVarAnnot(getVarAnn(splitPSet), (void*)1);

	// Add the default process set invariants: [0<= lb <= ub < nprocsVar] and [lb <= rankVar <= ub]
	// We use the original, un-annotated versions of rankSet, rankVar and nprocsVar
	//    because this partition has not yet been assigned to a specific process set id
	projCG->addScalar(rankVar);
	projCG->addScalar(nprocsVar);
	assertProcSetInvariants(projCG, rankSet, zeroVarAllPSets, rankVar, nprocsVar);
	
	otherProcs.cgDisconnect(projCG);
	for(set<contRangeProcSet>::iterator it=otherProcsSet.begin(); it!=otherProcsSet.end(); it++)
	{	
		if(!(*it).emptySet() && (*it).validSet())
			(*it).cgDisconnect(projCG);
	}
	projCG->endTransaction();
	
	if(MPIAnalysisDebugLevel>0)
		cout << "projCG = "<<projCG->str()<<"\n";
	return projCG;
}

// Replaces the upper and lower bounds of goodSet with those of rank set so that some partition can
//     assume the identity of the ranks in goodSet.
// Removes the upper and lower bounds of the badSets from the given constraint graph.
void pCFG_contProcMatchAnalysis::cleanProcSets(ConstrGraph* cg, 
	                             const contRangeProcSet& rankSetPSet, const contRangeProcSet& goodSet,
                                const contRangeProcSet& badSet1, const contRangeProcSet& badSet2, 
                                const contRangeProcSet& badSet3)
{
	//if(MPIAnalysisDebugLevel>0)
	//{
	//	cout << "cleanProcSets rankSet="<<rankSet.str()<<"\n";
	//	cout << "cleanProcSets goodSet="<<goodSet.str()<<"\n";
	//	cout << "cleanProcSets badSet1="<<badSet1.str()<<"\n";
	//	cout << "cleanProcSets badSet2="<<badSet2.str()<<"\n";
	//	cout << "cleanProcSets badSet3="<<badSet3.str()<<"\n";
	//}
	
//cout << "cleanProcSets Before cg="<<cg->str()<<"\n";
	// Assign the rankSet to be the same as the goodSet
	cg->replaceVar(goodSet.getLB(), rankSetPSet.getLB());
	cg->replaceVar(goodSet.getUB(), rankSetPSet.getUB());
	
//cout << "cleanProcSets rankVar = "<<pCFG_contProcMatchAnalysis::rankVar.str()<<"\n";
	// Reset rankVar to be strictly inside the bounds of the rankSet: rankSet.lb <= rankVar <= rankSet.ub
	cg->eraseVarConstr(pCFG_contProcMatchAnalysis::rankVar);
	cg->addScalar(pCFG_contProcMatchAnalysis::rankVar);
	cg->assertCond(rankSetPSet.getLB(), pCFG_contProcMatchAnalysis::rankVar, 1, 1, 0);
	cg->assertCond(pCFG_contProcMatchAnalysis::rankVar, rankSetPSet.getUB(), 1, 1, 0);
	
	if(!badSet1.emptySet() && badSet1.validSet())
	{
		cg->removeScalar(badSet1.getLB());
		cg->removeScalar(badSet1.getUB());
	}
	if(!badSet2.emptySet() && badSet2.validSet())
	{
		cg->removeScalar(badSet2.getLB());
		cg->removeScalar(badSet2.getUB());
	}
	if(!badSet3.emptySet() && badSet3.validSet())
	{
		cg->removeScalar(badSet3.getLB());
		cg->removeScalar(badSet3.getUB());
	}
//cout << "cleanProcSets After cg="<<cg->str()<<"\n";

	cg->transitiveClosure();
//cout << "cleanProcSets After TC cg="<<cg<<"  "<<cg->str()<<"\n";
}
