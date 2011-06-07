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
bool LiveVarsLattice::addVar(const varID& var)
{
	if(liveVars.find(var) == liveVars.end()) {
		liveVars.insert(var);
		return true;
	}
	return false;
}
bool LiveVarsLattice::remVar(const varID& var)
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
	oss << "[LiveVarsLattice: liveVars=[";
	for(set<varID>::iterator var=liveVars.begin(); var!=liveVars.end(); ) {
		oss << *var;
		var++;
		if(var!=liveVars.end())
			oss << ", ";
	}
	oss << "]]";
	return oss.str();
}

// ################################
// ##### LiveDeadVarsAnalysis #####
// ################################

LiveDeadVarsAnalysis::LiveDeadVarsAnalysis(SgProject *project, funcSideEffectUses* fseu): fseu(fseu), IntraBWDataflow()
{
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
	
	if(liveDeadAnalysisDebugLevel>=1) Dbg::dbg << indent << "liveLat="<<liveLat->str(indent + "    ")<<endl;
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
			if(liveDeadAnalysisDebugLevel>=1) Dbg::dbg << indent << "Live Expression"<<endl;
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
			// Conditionals (condE ? trueE : falseE)
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
				
				// If this function has no definition and the user provided a class to provide 
				// the variables that are used by such functions
				if(isSgFunctionCallExp(n.getNode())->getAssociatedFunctionDeclaration() && 
					isSgFunctionCallExp(n.getNode())->getAssociatedFunctionDeclaration()->get_definition()==NULL &&
					fseu) {
					set<varID> funcUsedVars = fseu->usedVarsInFunc(Function(isSgFunctionCallExp(n.getNode())->getAssociatedFunctionDeclaration()), n, state);
					usedVars.insert(funcUsedVars.begin(), funcUsedVars.end());
				}
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
			if(liveDeadAnalysisDebugLevel>=1) Dbg::dbg << indent << "Dead Expression"<<endl;
		}
		
		// Remove the expression itself since it has no uses above itself
		if(liveDeadAnalysisDebugLevel>=1) Dbg::dbg << indent << "   Removing "<<SgExpr2Var(isSgExpression(n.getNode()))<<endl;
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
		} else if(isSgIfStmt(n.getNode())) {
			//Dbg::dbg << "SgIfStmt"<<endl;
			ROSE_ASSERT(isSgExprStatement(isSgIfStmt(n.getNode())->get_conditional()));
			//Dbg::dbg << "    conditional stmt="<<Dbg::escape(isSgExprStatement(isSgIfStmt(n.getNode())->get_conditional())->unparseToString()) << " | " << isSgExprStatement(isSgIfStmt(n.getNode())->get_conditional())->class_name()<<endl;
			//Dbg::dbg << "    conditional expr="<<Dbg::escape(isSgExprStatement(isSgIfStmt(n.getNode())->get_conditional())->get_expression()->unparseToString()) << " | " << isSgExprStatement(isSgIfStmt(n.getNode())->get_conditional())->get_expression()->class_name()<<endl;
			//Dbg::dbg << "    conditional var="<<SgExpr2Var(isSgExprStatement(isSgIfStmt(n.getNode())->get_conditional())->get_expression())<<endl;
			usedVars.insert(SgExpr2Var(isSgExprStatement(isSgIfStmt(n.getNode())->get_conditional())->get_expression()));
		} else if(isSgForStatement(n.getNode())) {
			//Dbg::dbg << "test="<<Dbg::escape(isSgForStatement(n.getNode())->get_test()->unparseToString()) << " | " << isSgForStatement(n.getNode())->get_test()->class_name()<<endl;
			//Dbg::dbg << "increment="<<Dbg::escape(isSgForStatement(n.getNode())->get_increment()->unparseToString()) << " | " << isSgForStatement(n.getNode())->get_increment()->class_name()<<endl;
			
			ROSE_ASSERT(isSgExprStatement(isSgForStatement(n.getNode())->get_test()));
			usedVars.insert(SgExpr2Var(isSgExprStatement(isSgForStatement(n.getNode())->get_test())->get_expression()));
			
			usedVars.insert(SgExpr2Var(isSgForStatement(n.getNode())->get_increment()));
		} else if(isSgWhileStmt(n.getNode())) {
			ROSE_ASSERT(isSgExprStatement(isSgWhileStmt(n.getNode())->get_condition()));
			//Dbg::dbg << "condition="<<Dbg::escape(isSgWhileStmt(n.getNode())->get_condition()->unparseToString()) << " | " << isSgWhileStmt(n.getNode())->get_condition()->class_name()<<endl;
			usedVars.insert(SgExpr2Var(isSgExprStatement(isSgWhileStmt(n.getNode())->get_condition())->get_expression()));
		} else if(isSgDoWhileStmt(n.getNode())) {
			ROSE_ASSERT(isSgExprStatement(isSgDoWhileStmt(n.getNode())->get_condition()));
			//Dbg::dbg << "condition="<<Dbg::escape(isSgDoWhileStmt(n.getNode())->get_condition()->unparseToString()) << " | " << isSgDoWhileStmt(n.getNode())->get_condition()->class_name()<<endl;
			usedVars.insert(SgExpr2Var(isSgExprStatement(isSgDoWhileStmt(n.getNode())->get_condition())->get_expression()));
		}
	}
	
	// First process assignments, then uses since we may assign and use the same variable
	// and in the end we want to first remove it and then re-insert it.
	
	if(liveDeadAnalysisDebugLevel>=1) {
		Dbg::dbg << indent << "    usedVars=<";
		for(set<varID>::iterator var=usedVars.begin(); var!=usedVars.end(); var++)
			Dbg::dbg << var << ", ";
		Dbg::dbg << ">"<<endl;
		Dbg::dbg << indent << "    assignedVars=<";
		for(set<varID>::iterator var=assignedVars.begin(); var!=assignedVars.end(); var++)
			Dbg::dbg << var << ", ";
		Dbg::dbg << ">"<<endl;
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
	
	if(liveDeadAnalysisDebugLevel>=1) Dbg::dbg << indent << "    #usedVars="<<usedVars.size()<<" #assignedExprs="<<assignedExprs.size()<<endl;
	
	return modified;
}

// Initialize vars to hold all the variables and expressions that are live at DataflowNode n
void getAllLiveVarsAt(LiveDeadVarsAnalysis* ldva, const DataflowNode& n, const NodeState& state, set<varID>& vars, string indent)
{
	//Dbg::dbg << "getAllLiveVarsAt() n="<<Dbg::escape(n.getNode()->unparseToString()) << " | " << n.getNode()->class_name()<<" | "<<n.getIndex()<<endl;
	//Dbg::dbg << "    state.getLatticeAbove(ldva): #="<<state.getLatticeAbove(ldva).size()<<endl;
	//for(vector<Lattice*>::const_iterator lat=state.getLatticeAbove(ldva).begin(); lat!=state.getLatticeAbove(ldva).end(); lat++) {
	//	Dbg::dbg <<"        lat="<<*lat<<endl;
	//	Dbg::dbg <<"        lat="<<(*lat)->str("            ")<<endl;
	//}
	//Dbg::dbg << "    state.getLatticeBelow(ldva): #="<<state.getLatticeBelow(ldva).size()<<endl;
	//for(vector<Lattice*>::const_iterator lat=state.getLatticeBelow(ldva).begin(); lat!=state.getLatticeBelow(ldva).end(); lat++) {
	//	Dbg::dbg <<"        lat="<<*lat<<endl;
	//	Dbg::dbg <<"        lat="<<(*lat)->str("            ")<<endl;
	//}
	//Dbg::dbg << "    state = "<<state.str(ldva, "        ")<<endl;
	//Dbg::dbg.flush();
	
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

// Minimal constructor that initializes just the portions of the object required to make an 
// initial blank VarsExprsProductLattice
VarsExprsProductLattice::VarsExprsProductLattice(const DataflowNode& n, const NodeState& state) : n(n), state(state)
{ 
}                  	

class collectAllVarRefs: public AstSimpleProcessing {
	public:
	//set<SgVarRefExp*> refs;
	set<SgExpression*> refs;
	void visit(SgNode* n) {
		if(isSgExpression(n)) refs.insert(isSgExpression(n));
	}
};

// creates a new VarsExprsProductLattice
// perVarLattice - sample lattice that will be associated with every variable in scope at node n
//     it should be assumed that the object pointed to by perVarLattice will be either
//     used internally by this VarsExprsProductLatticeobject or deallocated
// constVarLattices - map of additional variables and their associated lattices, that will be 
//     incorporated into this VarsExprsProductLatticein addition to any other lattices for 
//     currently live variables (these correspond to various useful constant variables like zeroVar)
// allVarLattice - the lattice associated with allVar (the variable that represents all of memory)
//     if allVarLattice==NULL, no support is provided for allVar
// func - the current function
// n - the dataflow node that this lattice will be associated with
// state - the NodeState at this dataflow node
VarsExprsProductLattice::VarsExprsProductLattice
	                    (Lattice* perVarLattice, 
                        const map<varID, Lattice*>& constVarLattices, 
                        Lattice* allVarLattice,
                        LiveDeadVarsAnalysis* ldva, 
                        const DataflowNode& n, const NodeState& state) : 
                        	perVarLattice(perVarLattice), constVarLattices(constVarLattices), allVarLattice(allVarLattice), ldva(ldva), n(n), state(state)
{
	// If a LiveDeadVarsAnalysis was provided, create a lattice only for each live object
	if(ldva) { 
		// Initialize varLatticeIndex with instances of perVarLattice for each variable that is live at n
		varIDSet liveVars = getAllLiveVarsAt(ldva, n, state, "    ");
		int idx=0;
		for(varIDSet::iterator var=liveVars.begin(); var!=liveVars.end(); var++, idx++) {
			varLatticeIndex[*var] = idx;
			lattices.push_back(perVarLattice->copy());
		}
	} else {
		//Dbg::dbg << "n=<"<<Dbg::escape(n.getNode()->unparseToString()) << " | " << n.getNode()->class_name()<<" | "<<n.getIndex()<<">"<<endl;
		/*Dbg::dbg << "n->get_parent()=<"<<Dbg::escape(n.getNode()->get_parent()->unparseToString()) << " | " << n.getNode()->get_parent()->class_name()<<">"<<endl;*/
		
		// Get all the variables that were accessed in the function that contains the given DataflowNode
		set<SgInitializedName *> readVars, writeVars;
		SgNode* cur = n.getNode();
		while(cur && !isSgFunctionDefinition(cur)) { /*Dbg::dbg << "    cur=<"<<Dbg::escape(cur->unparseToString()) << " | " << cur->class_name()<<">"<<endl;*/ cur = cur->get_parent(); }
		/*SgFunctionDefinition *func;
		     if(isSgFunctionDefinition(n.getNode()))    func = isSgFunctionDefinition(n.getNode());
		else if(isSgFunctionParameterList(n.getNode())) func = isSgFunctionDefinition(isSgFunctionDeclaration(n.getNode()->get_parent())->get_definition());
		else                                            func = SageInterface::getEnclosingFunctionDefinition(n.getNode(), false);*/
		SgFunctionDefinition *func = isSgFunctionDefinition(cur);
		
		if(func) {
			//Dbg::dbg <<"    func name="<<func->get_mangled_name().getString()<<endl;
			//Rose_STL_Container<SgVarRefExp*> refs = NodeQuery::queryNodeList(((SgNode*)(func->get_body()), V_SgVarRefExp);
			collectAllVarRefs collect;
			collect.traverse(func, preorder);
			for(set<SgExpression*>::iterator ref=collect.refs.begin(); ref!=collect.refs.end(); ref++) {
				//Dbg::dbg << "        ref="<<Dbg::escape((*ref)->unparseToString()) << " | " << (*ref)->class_name()<<">"<<endl;
				varID var = SgExpr2Var(*ref);
				if(varLatticeIndex.find(var) == varLatticeIndex.end()) {
					varLatticeIndex[var] = lattices.size();
					lattices.push_back(perVarLattice->copy());
				}
			}
			/*SageInterface::collectReadWriteVariables(func->get_body(), readVars, writeVars);
	
			// Add these 
			int idx=0;
			for(set<SgInitializedName *>::iterator name=readVars.begin(); name!=readVars.end(); name++, idx++) {
				varID var(*name);
				Dbg::dbg << "    "<<var<<endl;
				if(varLatticeIndex.find(var) == varLatticeIndex.end()) {
					varLatticeIndex[var] = idx;
					lattices.push_back(perVarLattice->copy());
				}
			}
			for(set<SgInitializedName *>::iterator name=writeVars.begin(); name!=writeVars.end(); name++, idx++) {
				varID var(*name);
				Dbg::dbg << "    "<<var<<endl;
				if(varLatticeIndex.find(var) == varLatticeIndex.end()) {
					varLatticeIndex[var] = idx;
					lattices.push_back(perVarLattice->copy());
				}
			}*/
			//collectUseByAddressVariableRefs (const SgStatement *s, std::set< SgVarRefExp * > &varSetB)
		}
	}	
}

// Create a copy of that. It is assumed that the types of all the lattices in  VarsExprsProductLattice that are
// the same as in this.
VarsExprsProductLattice::VarsExprsProductLattice(const VarsExprsProductLattice& that) : n(that.n), state(that.state)
{
	//Dbg::dbg << "    VarsExprsProductLattice::VarsExprsProductLattice(const VarsExprsProductLattice& that)"<<endl;
	allVarLattice=NULL;
	perVarLattice = NULL;
	copy(&that);
}

VarsExprsProductLattice::~VarsExprsProductLattice()
{
	delete(allVarLattice);
	delete(perVarLattice);
	
	// The destructor for ProductLattice deallocates the Lattices in lattices
}

// Returns the Lattice mapped to the given variable of NULL if nothing is mapped to it
Lattice* VarsExprsProductLattice::getVarLattice(const varID& var)
{
	if(varLatticeIndex.find(var) != varLatticeIndex.end())
		return lattices[varLatticeIndex[var]];
	else
		return NULL;
}

// Returns the set of all variables mapped by this VarsExprsProductLattice
set<varID> VarsExprsProductLattice::getAllVars()
{
	set<varID> vars;
	for(map<varID, int>::iterator v=varLatticeIndex.begin(); v!=varLatticeIndex.end(); v++)
		vars.insert(v->first);
	return vars;
}

// Returns the index of var among the variables associated with func
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
	ROSE_ASSERT(that);
	copy(that);
}
// Overwrites the state of this Lattice with that of that Lattice
void VarsExprsProductLattice::copy(const VarsExprsProductLattice* that)
{
	ROSE_ASSERT(that);
	level = that->level;
	ldva = that->ldva;
	
	//if(&n != &that->n) {
	//	Dbg::dbg << "VarsExprsProductLattice::copy() this="<<this<<" that="<<that<<endl;
	//	Dbg::dbg << "    n="<<n.getNode()<<" that->n="<<that->n.getNode()<<endl;
	//	Dbg::dbg << "    this->n="<<Dbg::escape(n.getNode()->unparseToString()) << " | " << n.getNode()->class_name()<<" | "<<n.getIndex()<<endl;
	//	Dbg::dbg << "    that->n="<<Dbg::escape(that->n.getNode()->unparseToString()) << " | " << that->n.getNode()->class_name()<<" | "<<that->n.getIndex()<<endl;
	//}
	//ROSE_ASSERT(n.getNode() == that->n.getNode() && n.getIndex() == that->n.getIndex());// && (&state == &that->state));

	//Dbg::dbg << "VarsExprsProductLattice::copy() allVarLattice="<<allVarLattice<<" that->allVarLattice="<<that->allVarLattice<<" perVarLattice="<<perVarLattice<<" that->perVarLattice="<<perVarLattice<<endl;
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
	//Dbg::dbg << "        that="<<((VarsExprsProductLattice*)that)->str("        ")<<endl;
	
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
	
	// Copy all lattices in that->constVarLattices to This
	for(map<varID, Lattice*>::const_iterator var=that->constVarLattices.begin(); var!=that->constVarLattices.end(); var++) {
		if(constVarLattices[var->first]) constVarLattices[var->first]->copy(var->second);
		else                             constVarLattices.insert(make_pair(var->first, var->second->copy()));
	}
	
	// Remove all lattices in lattices/varLatticeIndex that don't appear in that.lattices/that.varLatticeIndex
	varsToDelete.clear();
	for(map<varID, int>::const_iterator varIdx=that->varLatticeIndex.begin(); varIdx!=that->varLatticeIndex.end(); varIdx++) {
		if(that->varLatticeIndex.find(varIdx->first) == that->varLatticeIndex.end())
			varsToDelete.insert(varIdx->first);
	}
	for(set<varID>::iterator var=varsToDelete.begin(); var!=varsToDelete.end(); var++) {
		delete constVarLattices[*var];
		constVarLattices.erase(*var);
	}
	
	//Dbg::dbg << "VarsExprsProductLattice::copy() lattices.size()="<<lattices.size()<<" varLatticeIndex.size()="<<varLatticeIndex.size()<<endl;
	//Dbg::dbg <<"     varLatticeIndex="<<endl;
	//for(map<varID, int>::iterator varIdx=varLatticeIndex.begin(); varIdx!=varLatticeIndex.end(); varIdx++)
	//	Dbg::dbg << "        "<<varIdx->first<<", "<<varIdx->second<<": "<<lattices[varIdx->second]<<endl;
	//Dbg::dbg <<"     that->varLatticeIndex    lattices.size()="<<that->lattices.size()<<" varLatticeIndex.size()="<<that->varLatticeIndex.size()<<endl;
	//for(map<varID, int>::const_iterator varIdx=that->varLatticeIndex.begin(); varIdx!=that->varLatticeIndex.end(); varIdx++) {
	//	Dbg::dbg << "        "<<varIdx->first<<", "<<varIdx->second<<" : "<<that->lattices[varIdx->second]<<endl;
	//}
		
	// Copy all lattices in that->lattices/that->varLatticeIndex to This, placing the lattices in 
	// This in the same order as they had in That
	vector<Lattice*> newLattices;
	newLattices.resize(that->lattices.size());
	for(map<varID, int>::const_iterator varIdx=that->varLatticeIndex.begin(); varIdx!=that->varLatticeIndex.end(); varIdx++) {
		if(varLatticeIndex.find(varIdx->first) != varLatticeIndex.end()) {
			ROSE_ASSERT(that->lattices[varIdx->second]);
			ROSE_ASSERT(lattices[varLatticeIndex[varIdx->first]]);
			lattices[varLatticeIndex[varIdx->first]]->copy(that->lattices[varIdx->second]);
			newLattices[varIdx->second] = lattices[varLatticeIndex[varIdx->first]];
		} else {
			newLattices[varIdx->second] = that->lattices[varIdx->second]->copy();
		}
	}
	varLatticeIndex = that->varLatticeIndex;
	lattices = newLattices;
	//Dbg::dbg << "VarsExprsProductLattice::copy() DONE"<<endl;
	//Dbg::dbg << "    varLatticeIndex="<<endl;
	//for(map<varID, int>::iterator varIdx=varLatticeIndex.begin(); varIdx!=varLatticeIndex.end(); varIdx++)
	//	Dbg::dbg << "        "<<varIdx->first<<", "<<varIdx->second<<endl;
	//Dbg::dbg << "    lattices (#="<<lattices.size()<<"="<<endl;
	//int i=0;
	//for(vector<Lattice*>::iterator lat=lattices.begin(); lat!=lattices.end(); lat++, i++)
	//	Dbg::dbg << "        "<<i<<": "<<*lat<<endl;
	//Dbg::dbg << "    "<<str("        ")<<endl;
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
//	Dbg::dbg << "remapVars("<<newFunc.get_name().getString()<<"()), func="<<func.get_name().getString<<endl;
	
	// The lattices associated with the variables at the top of new Func and the corresponding mapping 
	// of variables to indexes in newLattices. The original lattices and varLatticeIndex will be replaced
	// with these objects.
	vector<Lattice*> newLattices;
	map<varID, int> newVarLatticeIndex;
	
	// Fill newLattices with lattices associated with variables in the new function 
	DataflowNode funcCFGStart = cfgUtils::getFuncStartCFG(newFunc.get_definition());
	varIDSet newRefVars = getAllLiveVarsAt(ldva, funcCFGStart, *NodeState::getNodeState(funcCFGStart, 0), "    ");
	
	// Iterate through all the variables that are live at the top of newFunc and for each one 
	int idx=0;
	for(varIDSet::iterator it = newRefVars.begin(); it!=newRefVars.end(); it++, idx++)
	{
		varID newVar = *it;
//		printf("remapVars() newVar = %s\n", newVar.str().c_str());
		bool found = false;
		
		// If this variable is a copy of some variable currently in lattices, transfer this lattice over to newLattices
		for(map<varID, varID>::const_iterator itR = varNameMap.begin(); itR != varNameMap.end(); itR++)
		{
//			printf("remapVars() varNameMap[%s] = %s, newVar=%s, match=%d\n", itR->first.str().c_str(), itR->second.str().c_str(), newVar.str().c_str(), itR->second == newVar);
			// If this mapping corresponds to newVar
			if(itR->second == newVar) {
				// The original variable that got mapped to newVar
				varID oldVar = itR->first;
				
				Lattice* l = getVarLattice(oldVar);
				ROSE_ASSERT(l);
				newLattices.push_back(l);
				
				// Erase the mapping of oldVar in varLatticeIndex
				varLatticeIndex.erase(oldVar);
				
				found = true;
			}
		}
		
		// If this new variable is not a remapped old variable
		if(!found)
		{
			// Check if this new variable is in fact an old variable 
			Lattice* l = getVarLattice(newVar);
			
			/*Dbg::dbg << "VarsExprsProductLattice::remapVars() l = "<<l->str("") << endl;
			Dbg::dbg << "      getVarIndex(newFunc, newVar)=" << getVarIndex(newFunc, newVar) << endl;*/
			// If it is, add it at its new index
			if(l) {
				//newLattices[getVarIndex(newFunc, newVar)] = l;
				newLattices.push_back(l);
				// Erase the original mapping of newVar in varLatticeIndex
				varLatticeIndex.erase(newVar);
			// If not, add a fresh lattice for this variable
			} else
				newLattices.push_back(perVarLattice->copy());
		}
		
		// Record that newVar is at index idx
		newVarLatticeIndex[newVar] = idx;
	}
	
	// Deallocate the lattices of all the variables that do not exist in newFunc are are not 
	// remapped into its set of variables 
	for(map<varID, int>::iterator varIdx=varLatticeIndex.begin(); varIdx!=varLatticeIndex.end(); varIdx++) {
		ROSE_ASSERT(lattices[varIdx->second]);
		delete lattices[varIdx->second];
	}
	
	// Replace newVPL information with the remapped information
	lattices        = newLattices;
	varLatticeIndex = newVarLatticeIndex;
}

// Called by analyses to copy over from the that Lattice dataflow information into this Lattice.
// that contains data for a set of variables and incorporateVars must overwrite the state of just
// those variables, while leaving its state for other variables alone.
// We do not force child classes to define their own versions of this function since not all
//    Lattices have per-variable information.
void VarsExprsProductLattice::incorporateVars(Lattice* that_arg)
{
	initialize();
	
	VarsExprsProductLattice* that = dynamic_cast<VarsExprsProductLattice*>(that_arg); ROSE_ASSERT(that);
	// Both lattices need to be talking about variables in the same function
	ROSE_ASSERT((&n == &that->n) && (&state == &that->state));
	if(that->allVarLattice) { 
		ROSE_ASSERT(allVarLattice);
		this->allVarLattice->copy(that->allVarLattice);
	}
	
	// Iterate through all the lattices of constant variables, copying any lattices in That to This
	for(map<varID, Lattice*>::iterator var=that->constVarLattices.begin(); var!=that->constVarLattices.end(); var++) {
		if(constVarLattices.find(var->first) != constVarLattices.end()) {
			ROSE_ASSERT(constVarLattices[var->first]);
			constVarLattices[var->first]->copy(var->second);
		} else {
			ROSE_ASSERT(var->second);
			constVarLattices.insert(make_pair(var->first, var->second->copy()));
		}
	}
	
	// Iterate through all the variables mapped by this lattice, copying any lattices in That to This
	for(map<varID, int>::iterator var = that->varLatticeIndex.begin(); var != that->varLatticeIndex.end(); var++)
	{
		if(varLatticeIndex.find(var->first) != varLatticeIndex.end()) {
			ROSE_ASSERT(lattices[varLatticeIndex[var->first]]);
			lattices[varLatticeIndex[var->first]]->copy(that->lattices[var->second]);
		} else {
			varLatticeIndex[var->first] = lattices.size();
			ROSE_ASSERT(that->lattices[var->second]);
			lattices.push_back(that->lattices[var->second]->copy());
		}
	}
}

// Returns a Lattice that describes the information known within this lattice
// about the given expression. By default this could be the entire lattice or any portion of it.
// For example, a lattice that maintains lattices for different known variables and expression will 
// return a lattice for the given expression. Similarly, a lattice that keeps track of constraints
// on values of variables and expressions will return the portion of the lattice that relates to
// the given expression. 
// It it legal for this function to return NULL if no information is available.
// The function's caller is responsible for deallocating the returned object
Lattice* VarsExprsProductLattice::project(SgExpression* expr) 
{
	varID exprVar = SgExpr2Var(expr);
	VarsExprsProductLattice* exprState = blankVEPL(n, state);
	
	exprState->ldva = ldva;
	
	// Copy over all the default lattices
	if(perVarLattice) exprState->perVarLattice = perVarLattice->copy();
	else              exprState->perVarLattice = NULL;
	if(allVarLattice) exprState->allVarLattice = allVarLattice->copy();
	else              exprState->allVarLattice = NULL;
	
	for(map<varID, Lattice*>::iterator lat=constVarLattices.begin(); lat!=constVarLattices.end(); lat++) {
		ROSE_ASSERT(lat->second);
		exprState->constVarLattices.insert(make_pair(lat->first, lat->second->copy()));
	}
	
	// Copy over the lattice associated with exprVar
	if(varLatticeIndex.find(exprVar) != varLatticeIndex.end()) {
		exprState->varLatticeIndex[exprVar] = 0;
		ROSE_ASSERT(lattices[varLatticeIndex[exprVar]]);
		exprState->lattices.push_back(lattices[varLatticeIndex[exprVar]]->copy());
	}
	
	return exprState;
}

// The inverse of project(). The call is provided with an expression and a Lattice that describes
// the dataflow state that relates to expression. This Lattice must be of the same type as the lattice
// returned by project(). unProject() must incorporate this dataflow state into the overall state it holds.
// Call must make an internal copy of the passed-in lattice and the caller is responsible for deallocating it.
// Returns true if this causes this to change and false otherwise.
bool VarsExprsProductLattice::unProject(SgExpression* expr, Lattice* exprState_arg)
{ 
	varID exprVar = SgExpr2Var(expr);
	VarsExprsProductLattice* exprState = dynamic_cast<VarsExprsProductLattice*>(exprState_arg);
	ROSE_ASSERT(exprState);
	
	// Make sure that exprState has a mapping for exprVar
	ROSE_ASSERT(exprState->varLatticeIndex.find(exprVar) != exprState->varLatticeIndex.end());
	ROSE_ASSERT(exprState->lattices[exprState->varLatticeIndex[exprVar]]);
	
	// If This lattice has a mapping for exprVar, meet its Lattice in This with its lattice in exprState 
	if(varLatticeIndex.find(exprVar) != varLatticeIndex.end()) {
		ROSE_ASSERT(lattices[varLatticeIndex[exprVar]]);
		return lattices[varLatticeIndex[exprVar]]->meetUpdate(exprState->lattices[exprState->varLatticeIndex[exprVar]]);
	// Else, if This lattice has no mapping for exprVar, simply copy it from exprState to This
	} else {
		addVar(exprVar, exprState->lattices[exprState->varLatticeIndex[exprVar]]);
		return true;
	}
}

// Functions used to inform this lattice that a given variable is now in use (e.g. a variable has entered
//    scope or an expression is being analyzed) or is no longer in use (e.g. a variable has exited scope or
//    an expression or variable is dead).
// Returns true if this causes this Lattice to change and false otherwise.
bool VarsExprsProductLattice::addVar(const varID& var)
{
	if(varLatticeIndex.find(var) == varLatticeIndex.end()) {
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

// Sets the lattice of the given var to be lat. 
// If the variable is already mapped to some other Lattice, 
//   If *(the current lattice) == *lat, the mapping is not changed
//   If *(the current lattice) != *lat, the current lattice is deallocated and var is mapped to lat->copy()
// Returns true if this causes this Lattice to change and false otherwise.
bool VarsExprsProductLattice::addVar(const varID& var, Lattice* lat)
{
	if(varLatticeIndex.find(var) == varLatticeIndex.end())
	{
		varLatticeIndex.insert(make_pair(var, lattices.size()));
		lattices.push_back(lat);
		return true;
	} else {
		ROSE_ASSERT(lattices[varLatticeIndex[var]]);
		bool modified = (*(lattices[varLatticeIndex[var]]) != *lat);
		if(modified) {
			delete lattices[varLatticeIndex[var]];
			lattices[varLatticeIndex[var]] = lat->copy();
		}
		return modified;
	}
}

// The string that represents this object
// If indent!="", every line of this string must be prefixed by indent
// The last character of the returned string should not be '\n', even if it is a multi-line string.
string VarsExprsProductLattice::str(string indent)
{
	//printf("VarsExprsProductLattice::str() this->allVarLattice=%p\n", this->allVarLattice);
	
	ostringstream outs;
	//outs << "[VarsExprsProductLattice: n="<<n.getNode()<<" = <"<<Dbg::escape(n.getNode()->unparseToString())<<" | "<<n.getNode()->class_name()<<" | "<<n.getIndex()<<"> level="<<(getLevel()==uninitialized ? "uninitialized" : "initialized")<<endl;
	outs << "[VarsExprsProductLattice: n="<<n.getNode()<<" level="<<(getLevel()==uninitialized ? "uninitialized" : "initialized")<<endl;
	//varIDSet refVars;// = getVisibleVars(func);
	//for(varIDSet::iterator it = refVars.begin(); it!=refVars.end(); it++)
	for(map<varID, int>::iterator varIdx=varLatticeIndex.begin(); varIdx!=varLatticeIndex.end(); varIdx++)
	{
		outs  << indent;
		outs  << "    ";
		outs  << varIdx->first;
		outs  << ": "; // << lattices[varIdx->second] << " : ";
		ROSE_ASSERT(lattices[varIdx->second]);
		outs  << lattices[varIdx->second]->str("");
		outs  << endl;
	}
	
	if(allVarLattice)
		outs << indent << "allVarLattice: "<<endl<<allVarLattice->str(indent)<<endl;
	
	if(constVarLattices.size()>0)
	{
		outs << indent << "constVarLattices: "<<endl;fflush(stdout);
		for(map<varID, Lattice*>::const_iterator itC = constVarLattices.begin(); itC!=constVarLattices.end(); itC++)
		{
			outs << indent << "    " << itC->first.str() << ": " << itC->second->str("")<<endl;
		}
	}
	outs << indent << "]"<<endl;
	return outs.str();
}

/*****************************************
 ***** FiniteVarsExprsProductLattice *****
 *****************************************/

// Initial blank FiniteVarsExprsProductLattice
FiniteVarsExprsProductLattice::FiniteVarsExprsProductLattice(const DataflowNode& n, const NodeState& state) :
		VarsExprsProductLattice(n, state)
{}

// Retrns a blank instance of a VarsExprsProductLattice that only has the fields n and state set
VarsExprsProductLattice* FiniteVarsExprsProductLattice::blankVEPL(const DataflowNode& n, const NodeState& state)
{
	return new FiniteVarsExprsProductLattice(n, state);
}

// creates a new VarsExprsProductLattice
// perVarLattice - sample lattice that will be associated with every variable in scope at node n
//     it should be assumed that the object pointed to by perVarLattice will be either
//     used internally by this VarsExprsProductLattice object or deallocated
// constVarLattices - map of additional variables and their associated lattices, that will be 
//     incorporated into this VarsExprsProductLattice in addition to any other lattices for 
//     currently live variables (these correspond to various useful constant variables like zeroVar)
// allVarLattice - the lattice associated with allVar (the variable that represents all of memory)
//     if allVarLattice==NULL, no support is provided for allVar
// func - the current function
// n - the dataflow node that this lattice will be associated with
// state - the NodeState at this dataflow node
FiniteVarsExprsProductLattice::FiniteVarsExprsProductLattice(
                                      Lattice* perVarLattice, 
                                      const map<varID, Lattice*>& constVarLattices, 
                                      Lattice* allVarLattice,
                                      LiveDeadVarsAnalysis* ldva, 
                                      const DataflowNode& n, const NodeState& state) :
    VarsExprsProductLattice(perVarLattice, constVarLattices, allVarLattice, ldva, n, state), 
    FiniteProductLattice()
{
	//Dbg::dbg << "FiniteVarsExprsProductLattice n="<<n.getNode()<<" = <"<<Dbg::escape(n.getNode()->unparseToString())<<" | "<<n.getNode()->class_name()<<" | "<<n.getIndex()<<">"<<endl;
	verifyFinite();
}

FiniteVarsExprsProductLattice::FiniteVarsExprsProductLattice(const FiniteVarsExprsProductLattice& that) : 
	VarsExprsProductLattice(that), FiniteProductLattice()
{
	//Dbg::dbg << "FiniteVarsExprsProductLattice::copy n="<<n.getNode()<<" = <"<<Dbg::escape(n.getNode()->unparseToString())<<" | "<<n.getNode()->class_name()<<" | "<<n.getIndex()<<">"<<endl;
	verifyFinite();
}

// returns a copy of this lattice
Lattice* FiniteVarsExprsProductLattice::copy() const
{
	return new FiniteVarsExprsProductLattice(*this);
}

/*******************************************
 ***** InfiniteVarsExprsProductLattice *****
 *******************************************/

// Minimal constructor that initializes just the portions of the object required to make an 
// initial blank VarsExprsProductLattice
InfiniteVarsExprsProductLattice::InfiniteVarsExprsProductLattice(const DataflowNode& n, const NodeState& state) : 
		VarsExprsProductLattice(n, state)
{}

// Retrns a blank instance of a VarsExprsProductLattice that only has the fields n and state set
VarsExprsProductLattice* InfiniteVarsExprsProductLattice::blankVEPL(const DataflowNode& n, const NodeState& state)
{
	return new InfiniteVarsExprsProductLattice(n, state);
}

// creates a new VarsExprsProductLattice
// perVarLattice - sample lattice that will be associated with every variable in scope at node n
//     it should be assumed that the object pointed to by perVarLattice will be either
//     used internally by this VarsExprsProductLatticeobject or deallocated
// constVarLattices - map of additional variables and their associated lattices, that will be 
//     incorporated into this VarsExprsProductLatticein addition to any other lattices for 
//     currently live variables (these correspond to various useful constant variables like zeroVar)
// allVarLattice - the lattice associated with allVar (the variable that represents all of memory)
//     if allVarLattice==NULL, no support is provided for allVar
// func - the current function
// n - the dataflow node that this lattice will be associated with
// state - the NodeState at this dataflow node
InfiniteVarsExprsProductLattice::InfiniteVarsExprsProductLattice(Lattice* perVarLattice, 
                                        const map<varID, Lattice*>& constVarLattices, 
                                        Lattice* allVarLattice,
                                        LiveDeadVarsAnalysis* ldva, 
                                        const DataflowNode& n, const NodeState& state) :
    VarsExprsProductLattice(perVarLattice, constVarLattices, allVarLattice, ldva, n, state), 
    InfiniteProductLattice()
{
}

InfiniteVarsExprsProductLattice::InfiniteVarsExprsProductLattice(const FiniteVarsExprsProductLattice& that) : 
	VarsExprsProductLattice(that), InfiniteProductLattice()
{
}

// returns a copy of this lattice
Lattice* InfiniteVarsExprsProductLattice::copy() const
{
	return new InfiniteVarsExprsProductLattice(*this);
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
