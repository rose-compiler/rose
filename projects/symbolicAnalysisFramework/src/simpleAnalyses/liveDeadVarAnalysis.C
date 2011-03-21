#include "liveDeadVarAnalysis.h"

int liveDeadAnalysisDebugLevel=0;

// ###########################
// ##### LiveVarsLattice #####
// ###########################

LiveVarsLattice::LiveVarsLattice() {}
	
LiveVarsLattice::LiveVarsLattice(const varID& var)
{
	liveVars.insert(var);
}

LiveVarsLattice::LiveVarsLattice(const set<varID>& liveVars) : liveVars(liveVars)
{ }
	
// Initializes this Lattice to its default state, if it is not already initialized
void LiveVarsLattice::initialize()
{}

// Returns a copy of this lattice
Lattice* LiveVarsLattice::copy() const
{ return new LiveVarsLattice(); }

// Overwrites the state of this Lattice with that of that Lattice
void LiveVarsLattice::copy(Lattice* that)
{
	liveVars = dynamic_cast<LiveVarsLattice*>(that)->liveVars;
}

// Called by analyses to create a copy of this lattice. However, if this lattice maintains any 
//    information on a per-variable basis, these per-variable mappings must be converted from 
//    the current set of variables to another set. This may be needed during function calls, 
//    when dataflow information from the caller/callee needs to be transferred to the callee/calleer.
// We do not force child classes to define their own versions of this function since not all
//    Lattices have per-variable information.
// varNameMap - maps all variable names that have changed, in each mapping pair, pair->first is the 
//              old variable and pair->second is the new variable
// func - the function that the copy Lattice will now be associated with
void LiveVarsLattice::remapVars(const map<varID, varID>& varNameMap, const Function& newFunc)
{
	// Iterate over all the remapped variables
	for(map<varID, varID>::const_iterator var=varNameMap.begin(); var!=varNameMap.end(); var++) {
		// If the current remapped variable is live, replace its old name with its new one
		if(liveVars.find(var->first) != liveVars.end()) {
			liveVars.erase(var->first);
			liveVars.insert(var->second);
		}
	}
}

// Called by analyses to copy over from the that Lattice dataflow information into this Lattice.
// that contains data for a set of variables and incorporateVars must overwrite the state of just
// those variables, while leaving its state for other variables alone.
// We do not force child classes to define their own versions of this function since not all
//    Lattices have per-variable information.
void LiveVarsLattice::incorporateVars(Lattice* that_arg)
{
	LiveVarsLattice* that = dynamic_cast<LiveVarsLattice*>(that_arg);
	for(set<varID>::iterator var=that->liveVars.begin(); var!=that->liveVars.end(); var++)
		liveVars.insert(*var);
}

// Returns a Lattice that describes the information known within this lattice
// about the given expression. By default this could be the entire lattice or any portion of it.
// For example, a lattice that maintains lattices for different known variables and expression will 
// return a lattice for the given expression. Similarly, a lattice that keeps track of constraints
// on values of variables and expressions will return the portion of the lattice that relates to
// the given expression. 
// It it legal for this function to return NULL if no information is available.
// The function's caller is responsible for deallocating the returned object
Lattice* LiveVarsLattice::project(SgExpression* expr) { 
	varID var = SgExpr2Var(expr);
	if(liveVars.find(var) != liveVars.end())
		return new LiveVarsLattice(var);
	else
		return new LiveVarsLattice();
}

// The inverse of project(). The call is provided with an expression and a Lattice that describes
// the dataflow state that relates to expression. This Lattice must be of the same type as the lattice
// returned by project(). unProject() must incorporate this dataflow state into the overall state it holds.
// Call must make an internal copy of the passed-in lattice and the caller is responsible for deallocating it.
// Returns true if this causes this to change and false otherwise.
bool LiveVarsLattice::unProject(SgExpression* expr, Lattice* exprState) { 
	LiveVarsLattice* that = dynamic_cast<LiveVarsLattice*>(exprState);
	varID var = SgExpr2Var(expr);
	bool modified = false;
	if(that->liveVars.find(var) != that->liveVars.end()) {
		modified = modified || (liveVars.find(var) == liveVars.end());
		liveVars.insert(var);
	}
	return modified;
}

// computes the meet of this and that and saves the result in this
// returns true if this causes this to change and false otherwise
bool LiveVarsLattice::meetUpdate(Lattice* that_arg)
{
	bool modified = false;
	LiveVarsLattice* that = dynamic_cast<LiveVarsLattice*>(that_arg);
	
	// Add all variables from that to this
	for(set<varID>::iterator var=that->liveVars.begin(); var!=that->liveVars.end(); var++) {
		// If this lattice doesn't yet record *var as being live
		if(liveVars.find(*var) == liveVars.end()) {
			modified = true;
			liveVars.insert(*var);
		}
	}
	
	return modified;	
}

bool LiveVarsLattice::operator==(Lattice* that_arg)
{
	LiveVarsLattice* that = dynamic_cast<LiveVarsLattice*>(that_arg);
	return liveVars == that->liveVars;
}

// Functions used to inform this lattice that a given variable is now in use (e.g. a variable has entered 
//    scope or an expression is being analyzed) or is no longer in use (e.g. a variable has exited scope or
//    an expression or variable is dead).
// It is assumed that a newly-added variable has not been added before and that a variable that is being
//    removed was previously added
// Returns true if this causes the lattice to change and false otherwise.
bool LiveVarsLattice::addVar(varID var)
{
	if(liveVars.find(var) == liveVars.end()) {
		liveVars.insert(var);
		return true;
	}
	return false;
}
bool LiveVarsLattice::remVar(varID var)
{
	if(liveVars.find(var) != liveVars.end()) {
		liveVars.erase(var);
		return true;
	}
	return false;
}

// Returns true if the given variable is recorded as live and false otherwise
bool LiveVarsLattice::isLiveVar(varID var)
{
	return liveVars.find(var) != liveVars.end();
}

// The string that represents this object
// If indent!="", every line of this string must be prefixed by indent
// The last character of the returned string should not be '\n', even if it is a multi-line string.
string LiveVarsLattice::str(string indent)
{
	ostringstream oss;
	oss << "<LiveVarsLattice: liveVars=[";
	for(set<varID>::iterator var=liveVars.begin(); var!=liveVars.end(); ) {
		oss << *var;
		var++;
		if(var!=liveVars.end())
			oss << ", ";
	}
	oss << "]>";
	return oss.str();
}

// ################################
// ##### LiveDeadVarsAnalysis #####
// ################################

LiveDeadVarsAnalysis::LiveDeadVarsAnalysis(SgProject *project): IntraBWDataflow()
{
	// Create unique annotations on each expression to make it possible to assign each expression a unique variable name
	SageInterface::annotateExpressionsWithUniqueNames(project);
}

// Generates the initial lattice state for the given dataflow node, in the given function, with the given NodeState
void LiveDeadVarsAnalysis::genInitState(const Function& func, const DataflowNode& n, const NodeState& state,
                  vector<Lattice*>& initLattices, vector<NodeFact*>& initFacts)
{
	initLattices.push_back(new LiveVarsLattice());	
}

bool LiveDeadVarsAnalysis::transfer(const Function& func, const DataflowNode& n, NodeState& state, const vector<Lattice*>& dfInfo) 
{ 
	string indent="    ";
	bool modified = false;
	
	// At every variable reference, add the variable to liveVars since it is used.
	// At every variable assignment, remove the variable from liveVars since its original value just got killed.
	// At every reference to an expression, 
	// 	- Add the expression's arguments to liveVars since the expression uses the arguments 
	// 	- Remove the expression itself from liveVars because expressions that are arguments of other expressions
	// 	     appear earlier in the CFG, meaning that this expression is the definition of the expression and its
	// 	     parent expression is its use.
	
	// !!! NOTE: WE NEED TO PERFORM AN ADDITIONAL ANALYSIS TO DETERMINE IF THE SUB-EXPRESSIONS THAT 
	// !!!       ARE USED IN n.getNode() ARE JUST COMPUTING A REFERENCE TO A MEMORY LOCATION AND 
	// !!!       SHOULD DECLARE THAT MEMORY LOCATION LIVE. WE DON'T NEED TO DO THE SAME FOR THE THE 
	// !!!       SUB-EXPRESSIONS THAT DESCRIBE MEMORY THAT IS DEFINED SINCE WE'RE OK WITH MISTAKING 
	// !!!       SOME DEAD EXPRESSIONS AND VARIABLES FOR BEING LIVE.
	
	LiveVarsLattice* liveLat = dynamic_cast<LiveVarsLattice*>(*(dfInfo.begin()));
	
	if(liveDeadAnalysisDebugLevel>=1) cout << indent << "liveLat="<<liveLat->str(indent + "    ")<<"\n";
	// Make sure that all the lattice is initialized
	liveLat->initialize();
	
	// Expressions that are assigned by the current operation
	set<SgExpression*> assignedExprs;
	// Variables that are assigned by the current operation
	set<varID> assignedVars;
	
	// Variables that are used/read by the current operation
	set<varID> usedVars;
	
	if(isSgExpression(n.getNode())) {
		if(liveLat->isLiveVar(SgExpr2Var(isSgExpression(n.getNode())))) {
			if(liveDeadAnalysisDebugLevel>=1) cout << indent << "Live Expression\n";
			// Plain assignment: lhs = rhs
			if(isSgAssignOp(n.getNode())) {
				assignedExprs.insert(isSgAssignOp(n.getNode())->get_lhs_operand());
				
				// If the lhs of the assignment is a complex expression (i.e. it refers to a variable that may be live) OR
				// if is a known expression that is known to may-be-live
				// THIS CODE ONLY APPLIES TO RHSs THAT ARE SIDE-EFFECT-FREE AND WE DON'T HAVE AN ANALYSIS FOR THAT YET
				/*if(!isVarExpr(isSgAssignOp(n.getNode())->get_lhs_operand()) || 
				   (isVarExpr(isSgAssignOp(n.getNode())->get_lhs_operand()) && 
				    liveLat->isLiveVar(SgExpr2Var(isSgAssignOp(n.getNode())->get_lhs_operand()))))
				{ */
				usedVars.insert(SgExpr2Var(isSgAssignOp(n.getNode())->get_rhs_operand()));
			// Initializer for a variable
			} else if(isSgAssignInitializer(n.getNode())) {
				usedVars.insert(SgExpr2Var(isSgAssignInitializer(n.getNode())->get_operand()));
			// Initializer for a function arguments
			} else if(isSgConstructorInitializer(n.getNode())) {
				SgExprListExp* exprList = isSgConstructorInitializer(n.getNode())->get_args();
				for(SgExpressionPtrList::iterator expr=exprList->get_expressions().begin();
				    expr!=exprList->get_expressions().end(); expr++)
					usedVars.insert(SgExpr2Var(*expr));
			// Initializer that captures internal stucture of structs or arrays ("int x[2] = {1,2};", it is the "1,2")
			} else if(isSgAggregateInitializer(n.getNode())) {
				SgExprListExp* exprList = isSgAggregateInitializer(n.getNode())->get_initializers();
				for(SgExpressionPtrList::iterator expr=exprList->get_expressions().begin();
				    expr!=exprList->get_expressions().end(); expr++)
					usedVars.insert(SgExpr2Var(*expr));
			// Designated Initializer 
			} else if(isSgDesignatedInitializer(n.getNode())) {
				SgExprListExp* exprList = isSgDesignatedInitializer(n.getNode())->get_designatorList();
				for(SgExpressionPtrList::iterator expr=exprList->get_expressions().begin();
				    expr!=exprList->get_expressions().end(); expr++)
					usedVars.insert(SgExpr2Var(*expr));
			// Constant expressions
			} else if(isSgValueExp(n.getNode())) {
			// Binary Operations
			} else if(isSgBinaryOp(n.getNode())) {
				// Self-update expressions, where the lhs is assigned
				if(isSgPlusAssignOp(n.getNode()) || isSgMinusAssignOp(n.getNode()) ||
				   isSgModAssignOp(n.getNode())  || isSgMultAssignOp(n.getNode())  ||
				   isSgDivAssignOp(n.getNode())) {
				   assignedExprs.insert(isSgBinaryOp(n.getNode())->get_lhs_operand());
				}
				// Both the lhs and rhs are used
			   usedVars.insert(SgExpr2Var(isSgBinaryOp(n.getNode())->get_lhs_operand()));
		   	usedVars.insert(SgExpr2Var(isSgBinaryOp(n.getNode())->get_rhs_operand()));
			// Unary Operations
			} else if(isSgUnaryOp(n.getNode())) {
				// If this is an auto-update operation
				if(isSgMinusMinusOp(n.getNode()) || isSgPlusPlusOp(n.getNode())) {
					// The argument is defined
					assignedExprs.insert(isSgUnaryOp(n.getNode())->get_operand());
					// The argument is used
					usedVars.insert(SgExpr2Var(isSgUnaryOp(n.getNode())->get_operand()));
				} else {
					// The argument is used
					usedVars.insert(SgExpr2Var(isSgUnaryOp(n.getNode())->get_operand()));
				}
			// Conditionals
			} else if(isSgConditionalExp(n.getNode())) {
				// The arguments are used
				usedVars.insert(SgExpr2Var(isSgConditionalExp(n.getNode())->get_conditional_exp()));
				usedVars.insert(SgExpr2Var(isSgConditionalExp(n.getNode())->get_true_exp()));
				usedVars.insert(SgExpr2Var(isSgConditionalExp(n.getNode())->get_false_exp()));
			// Delete
			} else if(isSgDeleteExp(n.getNode())) {
				// Delete expressions return nothing
				// The arguments are used
				usedVars.insert(SgExpr2Var(isSgDeleteExp(n.getNode())->get_variable()));
			// New
			} else if(isSgNewExp(n.getNode())) {
				// The placement arguments are used
				SgExprListExp* exprList = isSgNewExp(n.getNode())->get_placement_args();
				for(SgExpressionPtrList::iterator expr=exprList->get_expressions().begin();
				    expr!=exprList->get_expressions().end(); expr++)
					usedVars.insert(SgExpr2Var(*expr));
				
				// The placement arguments are used
				exprList = isSgNewExp(n.getNode())->get_constructor_args()->get_args();
				for(SgExpressionPtrList::iterator expr=exprList->get_expressions().begin();
				    expr!=exprList->get_expressions().end(); expr++)
					usedVars.insert(SgExpr2Var(*expr));
				
				// The built-in arguments are used (DON'T KNOW WHAT THESE ARE!)
				usedVars.insert(isSgNewExp(n.getNode())->get_builtin_args());
			// Function Calls
			} else if(isSgFunctionCallExp(n.getNode())) {
				// !!! CURRENTLY WE HAVE NO NOTION OF VARIABLES THAT IDENTIFY FUNCTIONS, SO THIS CASE IS EXCLUDED FOR NOW
				/*// The expression that identifies the called function is used
				usedVars.insert(isSgFunctionCallExp(n.getNode())->get_function());*/
				
				// The function call's arguments are used
				SgExprListExp* exprList = isSgFunctionCallExp(n.getNode())->get_args();
				for(SgExpressionPtrList::iterator expr=exprList->get_expressions().begin();
				    expr!=exprList->get_expressions().end(); expr++)
					usedVars.insert(SgExpr2Var(*expr));
			// Function Reference
			// !!! CURRENTLY WE HAVE NO NOTION OF VARIABLES THAT IDENTIFY FUNCTIONS, SO THIS CASE IS EXCLUDED FOR NOW
			/*} else if(isSgFunctionRefExp(n.getNode())) {*/
			/*} else if(isSgMemberFunctionRefExp(n.getNode())) {*/
			// Sizeof
			} else if(isSgSizeOfOp(n.getNode())) {
				// The arguments are used
				usedVars.insert(SgExpr2Var(isSgSizeOfOp(n.getNode())->get_operand_expr()));
			// !!! DON'T KNOW HOW TO HANDLE THESE
			/*} else if(isSgStatementExpression(n.getNode())) {(*/
			// This
			} else if(isSgThisExp(n.getNode())) {
			// Typeid
			// !!! DON'T KNOW WHAT TO DO HERE SINCE THE RETURN VALUE IS A TYPE AND THE ARGUMENT'S VALUE IS NOT USED
			/*} else if(isSgTypeIdOp(n.getNode())) {*/
			// Var Args
			// !!! DON'T HANDLE THESE RIGHT NOW. WILL HAVE TO IN THE FUTURE
			/*	SgVarArgOp 
					SgExpression * 	get_operand_expr () const 
				SgVarArgCopyOp
					SgExpression * 	get_lhs_operand () const
					SgExpression * 	get_rhs_operand () const  
				SgVarArgEndOp 
					SgExpression * 	get_operand_expr 
				SgVarArgStartOneOperandOp 
					SgExpression * 	get_operand_expr () const 
				SgVarArgStartOp 
					SgExpression * 	get_lhs_operand () const
					SgExpression * 	get_rhs_operand () const */
			// !!! WHAT IS THIS?
			/*	SgVariantExpression*/
			// Variable Reference (we know this expression is live)
			} else if(isSgVarRefExp(n.getNode())) {
				usedVars.insert(SgExpr2Var(isSgVarRefExp(n.getNode())));
			}
		} else {
			if(liveDeadAnalysisDebugLevel>=1) cout << indent << "Dead Expression\n";
		}
		
		// Remove the expression itself since it has no uses above itself
		if(liveDeadAnalysisDebugLevel>=1) cout << indent << "   Removing "<<SgExpr2Var(isSgExpression(n.getNode()))<<"\n";
		modified = liveLat->remVar(SgExpr2Var(isSgExpression(n.getNode()))) || modified;
	// Variable Declaration
	} else if(isSgInitializedName(n.getNode())) {
		SgInitializedName* initName = isSgInitializedName(n.getNode());
		varID var(initName);
		assignedVars.insert(var);
		// If this is the instance of SgInitializedName that occurrs immediately after the declaration's initializer AND
		// this declaration has an initializer, add it as a use
		if(n.getIndex()==1 && isSgInitializedName(n.getNode())->get_initializer())
			usedVars.insert(SgExpr2Var(isSgInitializedName(n.getNode())->get_initializer()));
	} else if(isSgStatement(n.getNode())) {
		if(isSgReturnStmt(n.getNode())) {
			usedVars.insert(SgExpr2Var(isSgReturnStmt(n.getNode())->get_expression()));
		} else if(isSgExprStatement(n.getNode())) {
			usedVars.insert(SgExpr2Var(isSgExprStatement(n.getNode())->get_expression()));
		} else if(isSgCaseOptionStmt(n.getNode())) {
			usedVars.insert(SgExpr2Var(isSgCaseOptionStmt(n.getNode())->get_key()));
			usedVars.insert(SgExpr2Var(isSgCaseOptionStmt(n.getNode())->get_key_range_end()));
		}
	}
	
	// First process assignments, then uses since we may assign and use the same variable
	// and in the end we want to first remove it and then re-insert it.
	
	if(liveDeadAnalysisDebugLevel>=1) {
		cout << indent << "    usedVars=<";
		for(set<varID>::iterator var=usedVars.begin(); var!=usedVars.end(); var++)
			cout << var << ", ";
		cout << ">\n";
		cout << indent << "    assignedVars=<";
		for(set<varID>::iterator var=assignedVars.begin(); var!=assignedVars.end(); var++)
			cout << var << ", ";
		cout << ">\n";
	}
	
	// Record for each assigned expression:
	//    If the expression corresponds to a variable, record that the variable is dead.
	//    Otherwise, record that the expression that computes the assigned memory location is live
	for(set<SgExpression*>::iterator asgn=assignedExprs.begin(); asgn!=assignedExprs.end(); asgn++) {
		// If the lhs is a variable reference, remove it from live variables unless we also use this variable
		if(isVarExpr(*asgn))
		{ if(usedVars.find(SgExpr2Var(*asgn)) != usedVars.end()) modified = liveLat->remVar(SgExpr2Var(*asgn)) || modified; }
		else
			modified = liveLat->addVar(SgExpr2Var(*asgn)) || modified;
	}
	for(set<varID>::iterator asgn=assignedVars.begin(); asgn!=assignedVars.end(); asgn++) {
		// Remove this variable from live variables unless we also use this variable
		if(usedVars.find(*asgn) == usedVars.end())
			modified = liveLat->remVar(*asgn) || modified;
	}
	
	// Record that the used variables are live
	for(set<varID>::iterator var=usedVars.begin(); var!=usedVars.end(); var++)
		modified = liveLat->addVar(*var) || modified;
	
	if(liveDeadAnalysisDebugLevel>=1) cout << indent << "    #usedVars="<<usedVars.size()<<" #assignedExprs="<<assignedExprs.size()<<"\n";
	
	return modified;
}

// Initialize vars to hold all the variables and expressions that are live at DataflowNode n
void getAllLiveVarsAt(LiveDeadVarsAnalysis* ldva, const DataflowNode& n, const NodeState& state, set<varID>& vars, string indent)
{
	LiveVarsLattice* liveLAbove = dynamic_cast<LiveVarsLattice*>(*(state.getLatticeAbove(ldva).begin()));
	LiveVarsLattice* liveLBelow = dynamic_cast<LiveVarsLattice*>(*(state.getLatticeBelow(ldva).begin()));

	// The set of live vars AT this node is the union of vars that are live above it and below it
	for(set<varID>::iterator var=liveLAbove->liveVars.begin(); var!=liveLAbove->liveVars.end(); var++)
		vars.insert(*var);
	for(set<varID>::iterator var=liveLBelow->liveVars.begin(); var!=liveLBelow->liveVars.end(); var++)
		vars.insert(*var);
}

// Returns the set of variables and expressions that are live at DataflowNode n
set<varID> getAllLiveVarsAt(LiveDeadVarsAnalysis* ldva, const DataflowNode& n, const NodeState& state, string indent)
{
	set<varID> vars;
	getAllLiveVarsAt(ldva, n, state, vars, indent);
	return vars;
}

// ###################################
// ##### VarsExprsProductLattice #####
// ###################################

VarsExprsProductLattice::VarsExprsProductLattice(const VarsExprsProductLattice& that) : n(that.n), state(that.state)
{
	allVarLattice = NULL;
	perVarLattice = NULL;
	copy(&that);
}

Lattice* VarsExprsProductLattice::getVarLattice(const varID& var)
{
	if(varLatticeIndex.find(var) != varLatticeIndex.end())
		return lattices[varLatticeIndex[var]];
	else
		return NULL;
}

// returns the index of var among the variables associated with func
// or -1 otherwise
int VarsExprsProductLattice::getVarIndex(const varID& var)
{
	if(varLatticeIndex.find(var) != varLatticeIndex.end()) return varLatticeIndex[var];
	
	return -1;
}

// Overwrites the state of this Lattice with that of that Lattice
void VarsExprsProductLattice::copy(Lattice* that_arg)
{
	VarsExprsProductLattice* that = dynamic_cast<VarsExprsProductLattice*>(that_arg);
	copy(that);
}
// Overwrites the state of this Lattice with that of that Lattice
void VarsExprsProductLattice::copy(const VarsExprsProductLattice* that)
{
	//if(&n != &that->n) {
		cout << "VarsExprsProductLattice::copy() this="<<this<<" that="<<that<<"\n";
		cout << "    n="<<n.getNode()<<" that->n="<<that->n.getNode()<<"\n";
		cout << "    this->n="<<n.getNode()->unparseToString() << " | " << n.getNode()->class_name()<<" | "<<n.getIndex()<<"\n";
		cout << "    that->n="<<that->n.getNode()->unparseToString() << " | " << that->n.getNode()->class_name()<<" | "<<that->n.getIndex()<<"\n";
	//}
	//ROSE_ASSERT(n.getNode() == that->n.getNode() && n.getIndex() == that->n.getIndex());// && (&state == &that->state));

	//cout << "VarsExprsProductLattice::copy() allVarLattice="<<allVarLattice<<" that->allVarLattice="<<that->allVarLattice<<" perVarLattice="<<perVarLattice<<" that->perVarLattice="<<perVarLattice<<"\n";
	if(that->allVarLattice) {
		if(allVarLattice) allVarLattice->copy(that->allVarLattice);
		else              allVarLattice = that->allVarLattice->copy();
	} else {
		if(allVarLattice) {
			delete allVarLattice;
			allVarLattice = NULL;
		}
	}
	
	if(that->perVarLattice) {
		if(perVarLattice) perVarLattice->copy(that->perVarLattice);
		else              perVarLattice = that->perVarLattice->copy();
	} else {
		if(perVarLattice) {
			delete perVarLattice;
			perVarLattice = NULL;
		}
	}
	
	// Remove all lattices in constVarLattices that don't appear in that->constVarLattices
	set<varID> varsToDelete;
	for(map<varID, Lattice*>::iterator var=constVarLattices.begin(); var!=constVarLattices.end(); var++) {
		if(that->constVarLattices.find(var->first) == that->constVarLattices.end())
			varsToDelete.insert(var->first);
	}
	for(set<varID>::iterator var=varsToDelete.begin(); var!=varsToDelete.end(); var++) {
		delete constVarLattices[*var];
		constVarLattices.erase(*var);
	}
	
	// Copy all lattices in that->constVarLattices to this
	for(map<varID, Lattice*>::const_iterator var=that->constVarLattices.begin(); var!=that->constVarLattices.end(); var++) {
		if(constVarLattices[var->first]) constVarLattices[var->first]->copy(var->second);
		else                             constVarLattices[var->first] = var->second->copy();
	}
	
	varLatticeIndex = that->varLatticeIndex;
	ldva = that->ldva;

	ProductLattice::copy((Lattice*)that);
}

// Called by analyses to create a copy of this lattice. However, if this lattice maintains any 
//    information on a per-variable basis, these per-variable mappings must be converted from 
//    the current set of variables to another set. This may be needed during function calls, 
//    when dataflow information from the caller/callee needs to be transferred to the callee/calleer.
// varNameMap - maps all variable names that have changed, in each mapping pair, pair->first is the 
//              old variable and pair->second is the new variable
// func - the function that the copy Lattice will now be associated with
void VarsExprsProductLattice::remapVars(const map<varID, varID>& varNameMap, const Function& newFunc)
{
//	printf("remapVars(%s()), func=%s\n", newFunc.get_name().str(), func.get_name().str());
	
	vector<Lattice*> newLattices;
	
	// Fill newLattices with lattices associated with variables in the new function 
	varIDSet newRefVars;// = getVisibleVars(newFunc);
	for(varIDSet::iterator it = newRefVars.begin(); it!=newRefVars.end(); it++)
	{
		varID newVar = *it;
//		printf("remapVars() newVar = %s\n", newVar.str().c_str());
		bool found = false;
		
		// If this variable is a copy of some variable currently in lattices
		for(map<varID, varID>::const_iterator itR = varNameMap.begin(); itR != varNameMap.end(); itR++)
		{
//			printf("remapVars() varNameMap[%s] = %s, newVar=%s, match=%d\n", itR->first.str().c_str(), itR->second.str().c_str(), newVar.str().c_str(), itR->second == newVar);
			// if this mapping corresponds to newVar
			if(itR->second == newVar)
			{
				// the original variable that got mapped to newVar
				varID oldVar = itR->first;
				
				Lattice* l = getVarLattice(oldVar);
				ROSE_ASSERT(l);
				newLattices.push_back(l->copy());
				
				found = true;
			}
		}
		
		// If this new variable is not a remapped old variable
		if(!found)
		{
			// Check if this new variable is in fact an old variable 
			Lattice* l = getVarLattice(newVar);
			
			/*cout << "VarsExprsProductLattice::remapVars() l = "<<l->str("") << "\n";
			cout << "      getVarIndex(newFunc, newVar)=" << getVarIndex(newFunc, newVar) << "\n";*/
			// If it does, add it at its new index
			if(l)
				//newLattices[getVarIndex(newFunc, newVar)] = l;
				newLattices.push_back(l);
			// If not, add a fresh lattice for this variable
			else
				newLattices.push_back(/*perVarLattice->copy()*/NULL);
		}
	}
	
	// Replace newVPL information with the remapped information
	lattices.clear();
//	!!! What about old Lattices in lattices ???
	lattices = newLattices;
}

// Called by analyses to copy over from the that Lattice dataflow information into this Lattice.
// that contains data for a set of variables and incorporateVars must overwrite the state of just
// those variables, while leaving its state for other variables alone.
// We do not force child classes to define their own versions of this function since not all
//    Lattices have per-variable information.
void VarsExprsProductLattice::incorporateVars(Lattice* that_arg)
{
	initialize();
	
	VarsExprsProductLattice* that = dynamic_cast<VarsExprsProductLattice*>(that_arg);
	// both lattices need to be talking about variables in the same function
	ROSE_ASSERT((&n == &that->n) && (&state == &that->state));
	if(that->allVarLattice) this->allVarLattice->copy(that->allVarLattice);
	
	// Iterate through all the lattices of constant variables, copying any lattices in That to This
	for(map<varID, Lattice*>::iterator var=that->constVarLattices.begin(); var!=that->constVarLattices.end(); var++) {
		if(constVarLattices.find(var->first) != constVarLattices.end())
			constVarLattices[var->first]->copy(var->second);
		else
			constVarLattices[var->first] = var->second->copy();
	}
	
	// Iterate through all the variables mapped by this lattice, copying any lattices in That to This
	for(map<varID, int>::iterator var = that->varLatticeIndex.begin(); var != that->varLatticeIndex.end(); var++)
	{
		if(varLatticeIndex.find(var->first) != varLatticeIndex.end())
			lattices[varLatticeIndex[var->first]]->copy(that->lattices[var->second]);
		else {
			varLatticeIndex[var->first] = lattices.size();
			lattices.push_back(that->lattices[var->second]->copy());
		}
	}
}

// Functions used to inform this lattice that a given variable is now in use (e.g. a variable has entered
//    scope or an expression is being analyzed) or is no longer in use (e.g. a variable has exited scope or
//    an expression or variable is dead).
// Returns true if this causes this Lattice to change and false otherwise.
bool VarsExprsProductLattice::addVar(const varID& var)
{
	if(varLatticeIndex.find(var) == varLatticeIndex.end())
	{
		varLatticeIndex.insert(make_pair(var, lattices.size()));
		lattices.push_back(perVarLattice->copy());
		return true;
	} else
		return false;
}
bool VarsExprsProductLattice::remVar(const varID& var)
{
	map<varID, int>::iterator it;
	if((it = varLatticeIndex.find(var)) == varLatticeIndex.end())
		return false;
	else {
		delete lattices[it->second];
		lattices[it->second]=NULL;
		// !!! NOTE: THIS INTRODUCES A MINOR DATA LEAK SINCE THE LATTICES VECTOR MAY END UP WITH A LOT OF 
		// !!!       EMPTY REGIONS. WE MAY NEED TO COME UP WITH A SCHEME TO COMPRESS IT.
		varLatticeIndex.erase(var);
		return true;
	}
}


// The string that represents this object
// If indent!="", every line of this string must be prefixed by indent
// The last character of the returned string should not be '\n', even if it is a multi-line string.
string VarsExprsProductLattice::str(string indent)
{
	//printf("VarsExprsProductLattice::str() this->allVarLattice=%p\n", this->allVarLattice);
	
	ostringstream outs;
	//outs << "[VarsExprsProductLattice: n="<<n.getNode()<<" = <"<<n.getNode()->unparseToString()<<" | "<<n.getNode()->class_name()<<" | "<<n.getIndex()<<"> level="<<(getLevel()==uninitialized ? "uninitialized" : "initialized")<<"\n";
	outs << "[VarsExprsProductLattice: n="<<n.getNode()<<" level="<<(getLevel()==uninitialized ? "uninitialized" : "initialized")<<"\n";
	varIDSet refVars;// = getVisibleVars(func);
	for(varIDSet::iterator it = refVars.begin(); it!=refVars.end(); it++)
	{
		int varIndex = varLatticeIndex.find(*it)->second;
		outs  << indent;
		outs   << "    ";
		outs  << (*it).str();
		outs  << ": ";
		outs  << lattices[varIndex]->str("");
		outs  << "\n";
	}
	
	if(allVarLattice)
		outs << indent << "allVarLattice: \n"<<allVarLattice->str(indent)<<"\n";
	
	if(constVarLattices.size()>0)
	{
		outs << indent << "constVarLattices: \n";fflush(stdout);
		for(map<varID, Lattice*>::const_iterator itC = constVarLattices.begin(); itC!=constVarLattices.end(); itC++)
		{
			outs << indent << "    " << itC->first.str() << ": " << itC->second->str("")<<"\n";fflush(stdout);
		}
	}
	outs << indent << "]\n";fflush(stdout);
	return outs.str();
}

// prints the Lattices set by the given LiveDeadVarsAnalysis 
void printLiveDeadVarsAnalysisStates(LiveDeadVarsAnalysis* ldva, string indent)
{
	vector<int> factNames;
	vector<int> latticeNames;
	latticeNames.push_back(0);
	printAnalysisStates pas(ldva, factNames, latticeNames, printAnalysisStates::above, indent);
	UnstructuredPassInterAnalysis upia_pas(pas);
	upia_pas.runAnalysis();
}
