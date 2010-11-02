#include "rankDepAnalysis.h"
#include "nodeState.h"

using namespace cfgUtils;

int MPIRankDepAnalysisDebugLevel=1;

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

/*printf("MPIRankNProcsDepLattice::meetUpdate\n");
cout << "this: " << str("") << "\n";
cout << "that: " << that->str("") << "\n";*/
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

/******************************
 ***** MPIRankDepAnalysis *****
 ******************************/

/*// Traverses the an AST subtree to determine whether any of the variables in the given ProductLattice
// depend on the process' rank or on the number of processes 
class MPIRankDepSubtreeTraversal : public AstSimpleProcessing
{
	const FiniteVariablesProductLattice& prodLat;
	const Function& func;
	MPIRankDepAnalysis* rankDepAnal;
	
	// true if some variable in the sub-tree depends on the rank, false otherwise
	bool subtreeRankDep;
	// true if some variable in the sub-tree depends on the number of processes, false otherwise
	bool subtreeNprocsDep;
	
	public:
	MPIRankDepSubtreeTraversal(FiniteVariablesProductLattice& prodLat, Function& func, 
	                           MPIRankDepAnalysis* rankDepAnal): prodLat(prodLat), func(func)
	{
		this->prodLat = prodLat;
		this->func = func;
		this->rankDepAnal = rankDepAnal;
		subtreeRankDep = false;
		subtreeNprocsDep = false;
	}
		
	virtual void visit(SgNode* n)
	{
		if(isSgExpression(n) && varID::isValidVarExp(isSgExpression(n)))
		{
			NodeState* state = NodeState::getNodeState(n, 0);
			FiniteVariablesProductLattice* prodLat = dynamic_cast<FiniteVariablesProductLattice*>(getLatticeBelow(rankDepAnal, 0));
			
			varID v(isSgExpression(n));
			MPIRankNProcsDepLattice* rnLat = dynamic_cast<MPIRankNProcsDepLattice*>(prodLat->getVarLattice(func, v));
			subtreeRankDep   = subtreeRankDep   || rnLat->getRankDep(true);
			subtreeNprocsDep = subtreeNprocsDep || rnLat->getNprocsDep(true);
		}
	}
	
	bool getRankDep()
	{ return subtreeRankDep; }
	
	bool getNprocsDep()
	{ return subtreeNprocsDep; }
}*/

// generates the initial lattice state for the given dataflow node, in the given function, with the given NodeState
//vector<Lattice*> MPIRankDepAnalysis::genInitState(const Function& func, const DataflowNode& n, const NodeState& state)
void MPIRankDepAnalysis::genInitState(const Function& func, const DataflowNode& n, const NodeState& state,
	                                   vector<Lattice*>& initLattices, vector<NodeFact*>& initFacts)
{
	//printf("MPIRankDepAnalysis::genInitState() n=%p<%s | %s>\n", n.getNode(), n.getNode()->class_name().c_str(), n.getNode()->unparseToString().c_str());
	//printf("MPIRankDepAnalysis::genInitState() state = %p\n", &state);
	
	//vector<Lattice*> initLattices;
	map<varID, Lattice*> constVars;
	initLattices.push_back(new FiniteVariablesProductLattice(true, false, new MPIRankNProcsDepLattice(), constVars, NULL, func, n, state));
	
	//return initLattices;
}

bool MPIRankDepAnalysis::transfer(const Function& func, const DataflowNode& n, NodeState& state, const vector<Lattice*>& dfInfo)
{
	//printf("MPIRankDepAnalysis::transfer\n");

	bool modified=false;
	
	FiniteVariablesProductLattice* prodLat = dynamic_cast<FiniteVariablesProductLattice*>(*(dfInfo.begin()));
		
	// make sure that all the non-constant Lattices are initialized
	const vector<Lattice*>& lattices = prodLat->getLattices();
	for(vector<Lattice*>::const_iterator it = lattices.begin(); it!=lattices.end(); it++)
	{
		(dynamic_cast<MPIRankNProcsDepLattice*>(*it))->initialize();
		//printf("MPIRankDepAnalysis::transfer initializing variable %s\n", (*it).get_name().str());
	}

	// Process any calls to MPI_Comm_rank and MPI_Comm_size to identify any new
	// dependencies on the process' rank or the number of processes
	if(isSgFunctionCallExp(n.getNode()))
	{
		//printf("    isSgFunctionCallExp\n");
		SgFunctionCallExp* fnCall = isSgFunctionCallExp(n.getNode());
		Function calledFunc(fnCall);
		
		if(calledFunc.get_name().getString() == "MPI_Comm_rank")
		{
			//printf("        MPI_Comm_rank\n");
			SgExpressionPtrList& args = fnCall->get_args()->get_expressions();
			// args[1]
			SgExpression* arg1 = *(++(args.begin()));
			//printf("arg1 = <%s | %s>\n", arg1->class_name().c_str(), arg1->unparseToString().c_str());
			
			// look at MPI_Comm_rank's second argument and record that it depends on the process' rank
			if(isSgAddressOfOp(arg1) && varID::isValidVarExp(isSgAddressOfOp(arg1)->get_operand()))
			{
				//varID rankVar((isSgVarRefExp(arg1))->get_symbol()->get_declaration());
				varID rankVar(isSgAddressOfOp(arg1)->get_operand());
				MPIRankNProcsDepLattice* rnLat = dynamic_cast<MPIRankNProcsDepLattice*>(prodLat->getVarLattice(func, rankVar));
				modified = rnLat->setRankDep(true) || modified;
			}
			else
			{ fprintf(stderr, "[rankDepAnalysis] MPIRankDepAnalysis::transfer() ERROR: second argument \"%s\" of function call \"%s\" is not a reference to a variable!\n", arg1->unparseToString().c_str(), n.getNode()->unparseToString().c_str()); exit(-1); }
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
				//varID rankVar((isSgVarRefExp(arg1))->get_symbol()->get_declaration());
				varID nprocsVar(isSgAddressOfOp(arg1)->get_operand());
				MPIRankNProcsDepLattice* rnLat = dynamic_cast<MPIRankNProcsDepLattice*>(prodLat->getVarLattice(func, nprocsVar));
				modified = rnLat->setNprocsDep(true);
			}
			else
			{ fprintf(stderr, "[rankDepAnalysis] MPIRankDepAnalysis::transfer() ERROR: second argument \"%s\" of function call \"%s\" is not a reference to a variable!\n", arg1->unparseToString().c_str(), n.getNode()->unparseToString().c_str()); exit(-1); }
		}
	}
	// Propagate dependence info through assignments
	else if(isSgAssignOp(n.getNode()) || isSgVariableDeclaration(n.getNode()))
	{
		//printf("    isSgAssignOp || isSgVariableDeclaration\n");
		varIDSet rhsVars;
		bool varAssign; // true if we're assigning to a regular variable, false otherwise
		varID* lhsVar;
		if(isSgVariableDeclaration(n.getNode()))
		{ 
			SgInitializer* init = isSgVariableDeclaration(n.getNode())->get_definition()->get_vardefn()->get_initializer();
			//printf("isSgVariableDeclaration %s = %p\n", isSgVariableDeclaration(n.getNode())->get_definition()->get_vardefn()->get_name().str(), init);
			if(init)
				rhsVars = getVarRefsInSubtree(init);
			varAssign = true;
			ROSE_ASSERT(isSgVariableDeclaration(n.getNode())->get_variables().size()==1);
			lhsVar = new varID(*(isSgVariableDeclaration(n.getNode())->get_variables().begin()));
		}
		else
		{
			SgAssignOp* assgn = isSgAssignOp(n.getNode());
		
			rhsVars = getVarRefsInSubtree(assgn->get_rhs_operand());
			varAssign = varID::isValidVarExp(assgn->get_lhs_operand());
			lhsVar = new varID(assgn->get_lhs_operand());
		}
		
		for(varIDSet::iterator it = rhsVars.begin(); it!=rhsVars.end(); it++)
		{ printf("        var %s\n", (*it).str().c_str()); }
		
		// true if some variable on the right-hand side of the assignment depends on the process' rank or 
		// the number of processes, false otherwise
		bool rhsRankDep = false;
		bool rhsNprocsDep = false;
		for(varIDSet::iterator it = rhsVars.begin(); it != rhsVars.end(); it++)
		{
			MPIRankNProcsDepLattice* rhsrnLat = dynamic_cast<MPIRankNProcsDepLattice*>(prodLat->getVarLattice(func, *it));
			rhsRankDep   = rhsRankDep   || rhsrnLat->getRankDep();
			rhsNprocsDep = rhsNprocsDep || rhsrnLat->getNprocsDep();
			if(rhsRankDep && rhsNprocsDep)
				break;
		}
	
		// if the rhs depends on process rank or the number of processes
		if(rhsRankDep || rhsNprocsDep)
		{
			// the left-hand side of the assignment must be a regular variable
			if(varAssign)
			{
				MPIRankNProcsDepLattice* lhsrnLat = dynamic_cast<MPIRankNProcsDepLattice*>(prodLat->getVarLattice(func, *lhsVar));
				if(MPIRankDepAnalysisDebugLevel>=1){
					printf("Assignment of variable %s: rhs rankDep=%d, nprocsDep=%d\n", lhsVar->str().c_str(), rhsRankDep, rhsNprocsDep);
				}
				
				modified = lhsrnLat->setRankDep(rhsRankDep)     || modified;
				modified = lhsrnLat->setNprocsDep(rhsNprocsDep) || modified;
			}
			else
			{ fprintf(stderr, "[rankDepAnalysis] MPIRankDepAnalysis::transfer() ERROR: left-hand side of assignment that depends on rank or nprocs \"%s\" is not a variable!\n", n.getNode()->unparseToString().c_str()); exit(-1); }
		}
		
		delete lhsVar;
	}
	
	return modified;
}

static MPIRankDepAnalysis* rankDepAnal=NULL;
static Analysis* rankDepAnalysis=NULL;
static ContextInsensitiveInterProceduralDataflow* rankDepAnal_inter=NULL;
void runMPIRankDepAnalysis()
{
	rankDepAnal = new MPIRankDepAnalysis();
	rankDepAnalysis = rankDepAnal;
	
	CallGraphBuilder cgb(getProject());
	cgb.buildCallGraph();
	rankDepAnal_inter = new ContextInsensitiveInterProceduralDataflow(rankDepAnal, cgb.getGraph());
	
	rankDepAnal_inter->runAnalysis();
	//printf("rankDepAnal=%p=%p=%p, rankDepAnal_inter=%p\n", rankDepAnal, (Analysis*)rankDepAnal, (IntraProceduralAnalysis*)rankDepAnal, rankDepAnal_inter);
	
	if(MPIRankDepAnalysisDebugLevel>0)
	{
		vector<int> factNames;
		vector<int> latticeNames;
		latticeNames.push_back(0);
		IntraProceduralAnalysis* rankDepAnal2 = (IntraProceduralAnalysis*)rankDepAnal;
		printAnalysisStates pas(rankDepAnal2, factNames, latticeNames, ":");
		UnstructuredPassInterAnalysis upia_pas(pas);
		upia_pas.runAnalysis();
	}
}

// Returns whether the given variable at the given DataflowNode depends on the process' rank
bool isMPIRankVarDep(const Function& func, const DataflowNode& n, varID var)
{
	if(!rankDepAnal)
		runMPIRankDepAnalysis();
	
	NodeState* state = NodeState::getNodeState(n, 0);
	FiniteVariablesProductLattice* prodLat = dynamic_cast<FiniteVariablesProductLattice*>(state->getLatticeBelow(rankDepAnalysis, 0));
	ROSE_ASSERT(prodLat);
	MPIRankNProcsDepLattice* rnLat = dynamic_cast<MPIRankNProcsDepLattice*>(prodLat->getVarLattice(func, var));
	if(!rnLat) return false;
	return rnLat->getRankDep();
}

// Returns whether the given variable at the given DataflowNode depends on the number of processes
bool isMPINprocsVarDep(const Function& func, const DataflowNode& n, varID var)
{
	if(!rankDepAnal)
		runMPIRankDepAnalysis();
	
	NodeState* state = NodeState::getNodeState(n, 0);
	FiniteVariablesProductLattice* prodLat = dynamic_cast<FiniteVariablesProductLattice*>(state->getLatticeBelow(rankDepAnalysis, 0));
	MPIRankNProcsDepLattice* rnLat = dynamic_cast<MPIRankNProcsDepLattice*>(prodLat->getVarLattice(func, var));
	return rnLat->getNprocsDep();
}

// Sets rankDep and nprocsDep to true if some variable in the expression depends on the process' rank or 
// the number of processes, respectively. False otherwise.
bool isMPIDep(const Function& func, const DataflowNode& n, bool& rankDep, bool& nprocsDep)
{
	if(!rankDepAnal)
		runMPIRankDepAnalysis();
	
	//cout << "n.getNode()="<<n.getNode()->unparseToString()<<"\n";
	varIDSet exprVars = getVarRefsInSubtree(n.getNode());

	/*for(varIDSet::iterator it = exprVars.begin(); it!=exprVars.end(); it++)
	{ printf("        var %s\n", (*it).str().c_str()); }*/
	
	NodeState* state = NodeState::getNodeState(n, 0);
	FiniteVariablesProductLattice* prodLat = dynamic_cast<FiniteVariablesProductLattice*>(state->getLatticeBelow(rankDepAnalysis, 0));
	
	// initialize both flags to false
	rankDep = false;
	nprocsDep = false;
	for(varIDSet::iterator it = exprVars.begin(); it != exprVars.end(); it++)
	{
		//cout << "prodLat->getVarLattice(func, "<<(*it).str()<<") = "<<prodLat->getVarLattice(func, *it)->str() << "\n";
		MPIRankNProcsDepLattice* rnLat = dynamic_cast<MPIRankNProcsDepLattice*>(prodLat->getVarLattice(func, *it));
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
