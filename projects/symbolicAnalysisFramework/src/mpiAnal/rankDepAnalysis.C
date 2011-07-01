#include "rankDepAnalysis.h"
#include "nodeState.h"
#include "liveDeadVarAnalysis.h"

using namespace cfgUtils;

int MPIRankDepAnalysisDebugLevel=0;

/***********************************
 ***** MPIRankNProcsDepLattice *****
 ***********************************/

// returns a copy of this lattice
Lattice* MPIRankNProcsDepLattice::copy() const
{
	return new MPIRankNProcsDepLattice(*this);
}

// overwrites the state of this Lattice with that of that Lattice
void MPIRankNProcsDepLattice::copy(Lattice* that_arg)
{
	MPIRankNProcsDepLattice* that = dynamic_cast<MPIRankNProcsDepLattice*>(that_arg);
	
	this->initialized = that->initialized;
	this->rankDep     = that->rankDep;
	this->nprocsDep   = that->nprocsDep;
}

/*
// overwrites the state of this Lattice with that of that Lattice
// returns true if this causes this lattice to change and false otherwise
bool MPIRankNProcsDepLattice::copyMod(Lattice* that_arg)
{
	MPIRankNProcsDepLattice* that = dynamic_cast<MPIRankNProcsDepLattice*>(that_arg);
	bool modified = (this->sgnState != that->sgnState) || (this->level != that->level);
	
	this->sgnState = that->sgnState;
	this->level = that->level;
	
	return modified;
}*/


// computes the meet of this and that and saves the result in this
// returns true if this causes this to change and false otherwise
bool MPIRankNProcsDepLattice::meetUpdate(Lattice* that_arg)
{
	MPIRankNProcsDepLattice* that = dynamic_cast<MPIRankNProcsDepLattice*>(that_arg);

/*Dbg::dbg << "MPIRankNProcsDepLattice::meetUpdateate"<<endl;
Dbg::dbg << "this: " << str("") << endl;
Dbg::dbg << "that: " << that->str("") << endl;*/
	bool oldInitialized = initialized;
	bool oldRankDep     = rankDep;
	bool oldNprocsDep   = nprocsDep;
	
	// if this object is uninitialized, just copy the state of that
	if(!initialized)
	{
		if(that->initialized)
			copy(that);
	}
	// else, if that is uninitialized, leave this alone
	else if(!that->initialized)
	{ }
	// if both are initialized, perform the meet
	else
	{
		rankDep   = rankDep   || that->rankDep;
		nprocsDep = nprocsDep || that->nprocsDep;
	}
	return oldInitialized!=initialized || oldRankDep!=rankDep || oldNprocsDep!=nprocsDep;
}

bool MPIRankNProcsDepLattice::operator==(Lattice* that_arg)
{
	MPIRankNProcsDepLattice* that = dynamic_cast<MPIRankNProcsDepLattice*>(that_arg);
	
	return (initialized == that->initialized) &&
	       (rankDep     == that->rankDep) &&
	       (nprocsDep   == that->nprocsDep);
}

// returns the current state of this object
bool MPIRankNProcsDepLattice::getRankDep() const
{ return rankDep; }

bool MPIRankNProcsDepLattice::getNprocsDep() const
{ return nprocsDep; }

// set the current state of this object, returning true if it causes 
// the object to change and false otherwise
bool MPIRankNProcsDepLattice::setRankDep(bool rankDep)
{
	bool modified = this->rankDep != rankDep;
	this->rankDep = rankDep;
	return modified;
}

bool MPIRankNProcsDepLattice::setNprocsDep(bool nprocsDep)
{
	bool modified = this->nprocsDep != nprocsDep;
	this->nprocsDep = nprocsDep;
	return modified;
}
	
// Sets the state of this lattice to bottom (false, false)
// returns true if this causes the lattice's state to change, false otherwise
bool MPIRankNProcsDepLattice::setToBottom()
{
	bool modified = initialized!=true && rankDep!=false && nprocsDep!=false;
	initialized = true;
	rankDep     = false;
	nprocsDep   = false;
	return modified;
}

// Sets the state of this lattice to bottom (true, true)
// returns true if this causes the lattice's state to change, false otherwise
bool MPIRankNProcsDepLattice::setToTop()
{
	bool modified = initialized!=true && rankDep!=true && nprocsDep!=true;
	initialized = true;
	rankDep     = true;
	nprocsDep   = true;
	return modified;
}

string MPIRankNProcsDepLattice::str(string indent)
{
	ostringstream outs;
	if(initialized)
		outs << indent << "[MPIRankNProcsDepLattice: rankDep="<<(rankDep?"true":"false")<<", nprocsDep="<<(nprocsDep?"true":"false")<<"]";
	else
		outs << indent << "[MPIRankNProcsDepLattice: uninitialized]";
	return outs.str();
}

// generates the initial lattice state for the given dataflow node, in the given function, with the given NodeState
//vector<Lattice*> MPIRankDepAnalysis::genInitState(const Function& func, const DataflowNode& n, const NodeState& state)
void MPIRankDepAnalysis::genInitState(const Function& func, const DataflowNode& n, const NodeState& state,
	                                   vector<Lattice*>& initLattices, vector<NodeFact*>& initFacts)
{
	//printf("MPIRankDepAnalysis::genInitState() n=%p[%s | %s]\n", n.getNode(), n.getNode()->class_name().c_str(), n.getNode()->unparseToString().c_str());
	//printf("MPIRankDepAnalysis::genInitState() state = %p\n", &state);
	
	//vector<Lattice*> initLattices;
	map<varID, Lattice*> constVars;
	
	FiniteVarsExprsProductLattice* prodLat = new FiniteVarsExprsProductLattice(new MPIRankNProcsDepLattice(), constVars, NULL, NULL, n, state);
	//Dbg::dbg << "prodLat = "<<prodLat->str("    ")<<endl;
	initLattices.push_back(prodLat);
	
	//return initLattices;
}

bool MPIRankDepAnalysis::transfer(const Function& func, const DataflowNode& n, NodeState& state, const vector<Lattice*>& dfInfo)
{
	//Dbg::dbg << "MPIRankDepAnalysis::transfer"<<endl;
	bool modified=false;
	
	FiniteVarsExprsProductLattice* prodLat = dynamic_cast<FiniteVarsExprsProductLattice*>(*(dfInfo.begin()));
	
	// make sure that all the non-constant Lattices are initialized
	for(vector<Lattice*>::const_iterator it = prodLat->getLattices().begin(); it!=prodLat->getLattices().end(); it++)
		(dynamic_cast<MPIRankNProcsDepLattice*>(*it))->initialize();
	
	SgNode *sgn = n.getNode();

	// Process any calls to MPI_Comm_rank and MPI_Comm_size to identify any new
	// dependencies on the process' rank or the number of processes
	if(isSgFunctionCallExp(sgn))
	{
		//Dbg::dbg << "    isSgFunctionCallExp"<<endl;
		SgFunctionCallExp* fnCall = isSgFunctionCallExp(sgn);
		Function calledFunc(fnCall);
		
		if(calledFunc.get_name().getString() == "MPI_Comm_rank")
		{
			//Dbg::dbg << "        MPI_Comm_rank"<<endl;
			SgExpressionPtrList& args = fnCall->get_args()->get_expressions();
			// args[1]
			SgExpression* arg1 = *(++(args.begin()));
			//printf("arg1 = [%s | %s]]\n", arg1->class_name().c_str(), arg1->unparseToString().c_str());
			
			// Look at MPI_Comm_rank's second argument and record that it depends on the process' rank
			if(isSgAddressOfOp(arg1) && varID::isValidVarExp(isSgAddressOfOp(arg1)->get_operand()))
			{
				//varID rankVar((isSgVarRefExp(arg1))->get_symbol()->get_declaration());
				varID rankVar = SgExpr2Var(isSgAddressOfOp(arg1)->get_operand());
				MPIRankNProcsDepLattice* rnLat = dynamic_cast<MPIRankNProcsDepLattice*>(prodLat->getVarLattice(rankVar));
				Dbg::dbg << "rankVar="<<rankVar<<" rnLat="<<rnLat<<endl;
				Dbg::dbg << "prodLat="<<prodLat->str("    ")<<endl;
				modified = rnLat->setRankDep(true) || modified;
				Dbg::dbg << "prodLat="<<prodLat->str("    ")<<endl;
			}
			else
			{ fprintf(stderr, "[rankDepAnalysis] MPIRankDepAnalysis::transfer() ERROR: second argument \"%s\" of function call \"%s\" is not a reference to a variable!\n", arg1->unparseToString().c_str(), sgn->unparseToString().c_str()); exit(-1); }
		}
		else if(calledFunc.get_name().getString() == "MPI_Comm_size")
		{
			//Dbg::dbg << "        MPI_Comm_rank"<<endl;
			SgExpressionPtrList& args = fnCall->get_args()->get_expressions();
			// args[1]
			SgExpression* arg1 = *(++(args.begin()));
			
			// look at MPI_Comm_size's second argument and record that it depends on the number of processes
			if(isSgAddressOfOp(arg1) && varID::isValidVarExp(isSgAddressOfOp(arg1)->get_operand()))
			{
				//varID rankVar((isSgVarRefExp(arg1))->get_symbol()->get_declaration());
				varID nprocsVar= SgExpr2Var(isSgAddressOfOp(arg1)->get_operand());
				MPIRankNProcsDepLattice* rnLat = dynamic_cast<MPIRankNProcsDepLattice*>(prodLat->getVarLattice(nprocsVar));
				modified = rnLat->setNprocsDep(true);
			}
			else
			{ fprintf(stderr, "[rankDepAnalysis] MPIRankDepAnalysis::transfer() ERROR: second argument \"%s\" of function call \"%s\" is not a reference to a variable!\n", arg1->unparseToString().c_str(), sgn->unparseToString().c_str()); exit(-1); }
		}
	// Binary operations: lhs=rhs, lhs+=rhs, lhs+rhs, ...
	} else if(isSgBinaryOp(sgn)) {
		// Memory objects denoted by the expression’s left- and right-hand   
		// sides as well as the SgAssignOp itself
		varID lhs = SgExpr2Var(isSgBinaryOp(sgn)->get_lhs_operand());
		varID rhs = SgExpr2Var(isSgBinaryOp(sgn)->get_rhs_operand());
		varID res = SgExpr2Var(isSgBinaryOp(sgn));

		// The lattices associated the three memory objects
		MPIRankNProcsDepLattice* resLat = 
		        dynamic_cast<MPIRankNProcsDepLattice*>(prodLat->getVarLattice(res));
		MPIRankNProcsDepLattice* lhsLat = 
		        dynamic_cast<MPIRankNProcsDepLattice*>(prodLat->getVarLattice(lhs));
		MPIRankNProcsDepLattice* rhsLat = 
		        dynamic_cast<MPIRankNProcsDepLattice*>(prodLat->getVarLattice(rhs));
		ROSE_ASSERT(rhsLat);
		
		// Assignment: lhs = rhs, lhs+=rhs, lhs*=rhs,  lhs/=rhs, ...
		//    dependence flows from rhs to lhs and res
		if(isSgAssignOp(sgn))
		{
			// If the lhs and/or the SgAssignOp are live, copy lattice from the rhs
			if(lhsLat){ lhsLat->copy(rhsLat); modified = true; }
			if(resLat){ resLat->copy(rhsLat); modified = true; }
		}
		else if(isSgCompoundAssignOp(sgn))
		{
		  if(lhsLat) {
		    lhsLat->meetUpdate(rhsLat);
		    modified = true;
		  }
		  if(resLat) {
		    // 'lhs' must be alive here, and thus provide a lattice value, beacuse 'res' depends on it
		    ROSE_ASSERT(lhsLat);
		    resLat->copy(lhsLat);
		    modified = true;
		  }
		}
		// Non-assignments that do not refer to variables: lhs+rhs
		// dependence flows from lhs and rhs to res
		else {
			if(resLat) {
				resLat->copy(rhsLat); 
				resLat->meetUpdate(lhsLat); 
				modified = true;
			}
		}
		// NOTE: we need to deal with expressions such as a.b, a->b and a[i] specially since they refer to memory locations,
		//       especially sub-expressions (e.g. a.b for a.b.c.d or a[i] for a[i][j][k]) but we don't yet have good abstraction for this.
	// Unary operations
	} else if(isSgUnaryOp(sgn)) {
		if(!isSgAddressOfOp(sgn)) {
			// Memory objects denoted by the expression’s oprand as well as the expression itself
			varID op = SgExpr2Var(isSgUnaryOp(sgn)->get_operand());
			varID res = SgExpr2Var(isSgUnaryOp(sgn));
	
			// The lattices associated the three memory objects
			MPIRankNProcsDepLattice* opLat = 
			        dynamic_cast<MPIRankNProcsDepLattice*>(prodLat->getVarLattice(op));
			MPIRankNProcsDepLattice* resLat = 
			        dynamic_cast<MPIRankNProcsDepLattice*>(prodLat->getVarLattice(res));
			
			ROSE_ASSERT(opLat);
			
			// Copy lattice from the operand
			resLat->copy(opLat);
			modified = true;
		}
	// Conditional Operators: (x? y: z)
	} else if(isSgConditionalExp(sgn)) {
		// Memory objects denoted by the expression’s condition, true and false sub-expressions
		varID condE  = SgExpr2Var(isSgConditionalExp(sgn)->get_conditional_exp());
		varID trueE  = SgExpr2Var(isSgConditionalExp(sgn)->get_true_exp());
		varID falseE = SgExpr2Var(isSgConditionalExp(sgn)->get_false_exp());
		varID res    = SgExpr2Var(isSgConditionalExp(sgn));

		// The lattices associated the three memory objects
		MPIRankNProcsDepLattice* resLat = 
		        dynamic_cast<MPIRankNProcsDepLattice*>(prodLat->getVarLattice(res));
		MPIRankNProcsDepLattice* condLat = 
		        dynamic_cast<MPIRankNProcsDepLattice*>(prodLat->getVarLattice(condE));
		MPIRankNProcsDepLattice* trueLat = 
		        dynamic_cast<MPIRankNProcsDepLattice*>(prodLat->getVarLattice(trueE));
		MPIRankNProcsDepLattice* falseLat = 
		        dynamic_cast<MPIRankNProcsDepLattice*>(prodLat->getVarLattice(falseE));
		ROSE_ASSERT(condLat); ROSE_ASSERT(trueLat); ROSE_ASSERT(falseLat);
		
		// Dependence flows from the sub-expressions of the SgConditionalExp to res
		if(resLat) {
			resLat->copy(condLat); 
			resLat->meetUpdate(trueLat); 
			resLat->meetUpdate(falseLat); 
			modified = true;
		}
	// Variable Declaration
	} else if(isSgInitializedName(sgn)) {
		varID var(isSgInitializedName(sgn));
		MPIRankNProcsDepLattice* varLat = dynamic_cast<MPIRankNProcsDepLattice*>(prodLat->getVarLattice(var));
		
		// If this variable is live
		if(varLat) {
			// If there was no initializer, initialize its lattice to Bottom
			if(isSgInitializedName(sgn)->get_initializer()==NULL)
				modified = varLat->setToBottom() || modified;
			// Otherwise, copy the lattice of the initializer to the variable
			else {
				varID init = SgExpr2Var(isSgInitializedName(sgn)->get_initializer());
				MPIRankNProcsDepLattice* initLat = dynamic_cast<MPIRankNProcsDepLattice*>(prodLat->getVarLattice(init));
				ROSE_ASSERT(initLat);
				varLat->copy(initLat);
				modified = true;
			}
		}
	// Initializer for a variable
	} else if(isSgAssignInitializer(sgn)) {
		// Memory objects of the initialized variable and the 
		// initialization expression
		varID res = SgExpr2Var(isSgAssignInitializer(sgn));
		varID asgn = SgExpr2Var(isSgAssignInitializer(sgn)->get_operand());
		
		// The lattices associated both memory objects
		MPIRankNProcsDepLattice* resLat = dynamic_cast<MPIRankNProcsDepLattice*>(prodLat->getVarLattice(res));
		MPIRankNProcsDepLattice* asgnLat = dynamic_cast<MPIRankNProcsDepLattice*>(prodLat->getVarLattice(asgn));
		ROSE_ASSERT(resLat);
		
		// Copy lattice from the assignment
		resLat->copy(asgnLat);
		modified = true;
	}
	
	return modified;
}

static MPIRankDepAnalysis* rankDepAnal=NULL;
static Analysis* rankDepAnalysis=NULL;
static ContextInsensitiveInterProceduralDataflow* rankDepAnal_inter=NULL;
MPIRankDepAnalysis* runMPIRankDepAnalysis(SgIncidenceDirectedGraph* graph, string indent)
{
	rankDepAnal = new MPIRankDepAnalysis();
	rankDepAnalysis = rankDepAnal;
	rankDepAnal_inter = new ContextInsensitiveInterProceduralDataflow(rankDepAnal, graph);
	rankDepAnal_inter->runAnalysis();
	//printf("rankDepAnal=%p=%p=%p, rankDepAnal_inter=%p\n", rankDepAnal, (Analysis*)rankDepAnal, (IntraProceduralAnalysis*)rankDepAnal, rankDepAnal_inter);
	
	if(MPIRankDepAnalysisDebugLevel>0)
		printMPIRankDepAnalysisStates(rankDepAnal, "");
	
	return rankDepAnal;
}

// Prints the Lattices set by the given MPIRankDepAnalysis 
void printMPIRankDepAnalysisStates(string indent) {
	return printMPIRankDepAnalysisStates(rankDepAnal, indent);
}
void printMPIRankDepAnalysisStates(MPIRankDepAnalysis* rankDepAnal, string indent)
{
	vector<int> factNames;
	vector<int> latticeNames;
	latticeNames.push_back(0);
	Dbg::enterFunc("States of MPIRankDepAnalysis");
	printAnalysisStates pas(rankDepAnal, factNames, latticeNames, printAnalysisStates::above, indent);
	UnstructuredPassInterAnalysis upia_pas(pas);
	upia_pas.runAnalysis();
	Dbg::exitFunc("States of MPIRankDepAnalysis");
}


// Returns whether the given variable at the given DataflowNode depends on the process' rank
bool isMPIRankVarDep(const Function& func, const DataflowNode& n, varID var)
{
	ROSE_ASSERT(rankDepAnal);
	
	NodeState* state = NodeState::getNodeState(n, 0);
	FiniteVarsExprsProductLattice* prodLat = dynamic_cast<FiniteVarsExprsProductLattice*>(state->getLatticeBelow(rankDepAnalysis, 0));
	ROSE_ASSERT(prodLat);
	MPIRankNProcsDepLattice* rnLat = dynamic_cast<MPIRankNProcsDepLattice*>(prodLat->getVarLattice(var));
	//Dbg::dbg << "isMPIRankVarDep() n.getNode()="<<Dbg::escape(n.getNode()->unparseToString())<<" var="<<var<<" rnLat="<<rnLat<<endl;
	//cout << "isMPIRankVarDep() n.getNode()="<<Dbg::escape(n.getNode()->unparseToString())<<" var="<<var<<" rnLat="<<rnLat<<endl;
	if(!rnLat) return false;
	Dbg::dbg << "        isMPIRankVarDep() var="<<var<<" rnLat="<<rnLat->str()<<endl;
	//cout << "isMPIRankVarDep() rnLat="<<rnLat->str()<<endl;
	return rnLat->getRankDep();
}

// Returns whether the given variable at the given DataflowNode depends on the number of processes
bool isMPINprocsVarDep(const Function& func, const DataflowNode& n, varID var)
{
	ROSE_ASSERT(rankDepAnal);
	
	NodeState* state = NodeState::getNodeState(n, 0);
	FiniteVarsExprsProductLattice* prodLat = dynamic_cast<FiniteVarsExprsProductLattice*>(state->getLatticeBelow(rankDepAnalysis, 0));
	MPIRankNProcsDepLattice* rnLat = dynamic_cast<MPIRankNProcsDepLattice*>(prodLat->getVarLattice(var));
	return rnLat->getNprocsDep();
}

// Sets rankDep and nprocsDep to true if some variable in the expression depends on the process' rank or 
// the number of processes, respectively. False otherwise.
bool isMPIDep(const Function& func, const DataflowNode& n, bool& rankDep, bool& nprocsDep)
{
	ROSE_ASSERT(rankDepAnal);
	
	//Dbg::dbg << "n.getNode()="<<n.getNode()->unparseToString()<<endl;
	varIDSet exprVars = getVarRefsInSubtree(n.getNode());

	/*for(varIDSet::iterator it = exprVars.begin(); it!=exprVars.end(); it++)
	{ printf("        var %s\n", (*it).str().c_str()); }*/
	
	NodeState* state = NodeState::getNodeState(n, 0);
	FiniteVarsExprsProductLattice* prodLat = dynamic_cast<FiniteVarsExprsProductLattice*>(state->getLatticeBelow(rankDepAnalysis, 0));
	
	// initialize both flags to false
	rankDep = false;
	nprocsDep = false;
	for(varIDSet::iterator it = exprVars.begin(); it != exprVars.end(); it++)
	{
		if(prodLat->getVarLattice(*it)) Dbg::dbg << "prodLat-&lt;getVarLattice(func, "<<(*it)<<") = "<<prodLat->getVarLattice(*it)->str() << endl;
		else                            Dbg::dbg << "prodLat-&lt;getVarLattice(func, "<<(*it)<<") = "<<prodLat->getVarLattice(*it) << endl;
		MPIRankNProcsDepLattice* rnLat = dynamic_cast<MPIRankNProcsDepLattice*>(prodLat->getVarLattice(*it));
		rankDep   = rankDep   || rnLat->getRankDep();
		nprocsDep = nprocsDep || rnLat->getNprocsDep();
		if(rankDep && nprocsDep)
			break;
	}
	
	return rankDep || nprocsDep;
}

// Returns whether some variable at the given DataflowNode depends on the process' rank
bool isMPIRankDep(const Function& func, const DataflowNode& n)
{
	bool rankDep, nprocsDep;
	isMPIDep(func, n, rankDep, nprocsDep);
	return rankDep;
}

// Returns whether some variable at the given DataflowNode depends on the number of processes
bool isMPINprocsDep(const Function& func, const DataflowNode& n)
{
	bool rankDep, nprocsDep;
	isMPIDep(func, n, rankDep, nprocsDep);
	return nprocsDep;
}
