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
#include "divAnalysis.h"
// GB : 2011-03-05 (Removing Sign Lattice Dependence) #include "sgnAnalysis.h"
#include "nodeConstAnalysis.h"
#include "affineInequality.h"
#include "ConstrGraphAnalysis.h"
#include "saveDotAnalysis.h"
#include "ConstrGraph.h"

/*******************************************************************************************
 *******************************************************************************************
 *******************************************************************************************/

map<varID, Lattice*> ConstrGraphAnalysis::constVars;

// Generates the initial lattice state for the given dataflow node, in the given function, with the given NodeState
//vector<Lattice*> ConstrGraphAnalysis::genInitState(const Function& func, const DataflowNode& n, const NodeState& state)
void ConstrGraphAnalysis::genInitState(const Function& func, const DataflowNode& n, const NodeState& state,
                                       vector<Lattice*>& initLattices, vector<NodeFact*>& initFacts)
{
	//vector<Lattice*> initLattices;
//if(isSgIntVal(n.getNode())) {
//	printf("ConstrGraphAnalysis::genInitState() n=%p<%s | %s>\n", n.getNode(), n.getNode()->class_name().c_str(), n.getNode()->unparseToString().c_str());
/*	printf("ConstrGraphAnalysis::genInitState() state=%p\n", &state);
}*/
	
	// Create a constraint graph from the divisiblity and sign information at this CFG node
	FiniteVarsExprsProductLattice* divProdL = dynamic_cast<FiniteVarsExprsProductLattice*>(state.getLatticeBelow(divAnalysis, 0));
/*if(isSgIntVal(n.getNode()))
	cout << "    divProdL="<<divProdL->str("        ")<<"\n";*/
	//FiniteVarsExprsProductLattice* sgnProdL = dynamic_cast<FiniteVarsExprsProductLattice*>(state.getLatticeBelow(sgnAnalysis, 0));
	ConstrGraph* cg = new ConstrGraph(func, n, state, ldva, divProdL, /* GB : 2011-03-05 (Removing Sign Lattice Dependence) sgnProdL, */false, "");
	initLattices.push_back(cg);
//if(isSgIntVal(n.getNode())) {
//	cout << "cg="<<cg<<" cg->divLattices=\n";
//	cout << "    "<<cg->DivLattices2Str("    ")<<"\n";
//}
	// Create a product lattice that will maintain for each variable a ConstraintGraph that stores the constraints
	// represented by the variable
	// ??? map<varID, Lattice*> emptyM;
	// ??? varIDSet scalars, arrays;
	// ??? InfiniteVarsExprsProductLattice* l = 
	// ??? 	new InfiniteVarsExprsProductLattice(true, false, true, 
	// ??? 	                                    (Lattice*)new ConstrGraph(func, n, state, ldva, divProdL, sgnProdL, true, ""), 
	// ??? 	                                    emptyM, (Lattice*)NULL, ldva, n, state);
	// ??? //printf("DivAnalysis::genInitState, returning %p\n", l);
	// ??? initLattices.push_back(l);
	
	//return initState;
}

// Returns a map of special constant variables (such as zeroVar) and the lattices that correspond to them
// These lattices are assumed to be constants: it is assumed that they are never modified and it is legal to 
//    maintain only one copy of each lattice may for the duration of the analysis.
/*map<varID, Lattice*>& ConstrGraphAnalysis::genConstVarLattices() const
{
	return constVars;
}*/

bool ConstrGraphAnalysis::transfer(const Function& func, const DataflowNode& n, NodeState& state, const vector<Lattice*>& dfInfo)
{
	string indent="            ";
	printf("%s-----------------------------------\n", indent.c_str());
	printf("%sConstrGraphAnalysis::transfer() function %s() node=<%s | %s>\n", indent.c_str(), func.get_name().str(), n.getNode()->class_name().c_str(), n.getNode()->unparseToString().c_str());
	
	bool modified = false;
	ConstrGraph* cg = dynamic_cast<ConstrGraph*>(dfInfo[0]);
	set<varID> liveVars = getAllLiveVarsAt(ldva, n, state, "    ");
	/* ??? InfiniteVarsExprsProductLattice* prodLat = dynamic_cast<InfiniteVarsExprsProductLattice*>(dfInfo[1]);
	
	// Make sure that all the lattices are initialized
	const vector<Lattice*>& lattices = prodLat->getLattices();
	for(vector<Lattice*>::const_iterator it = lattices.begin(); it!=lattices.end(); it++) {
		(dynamic_cast<ConstrGraph*>(*it))->initialize("");	
	}*/
	
	cg->beginTransaction();
	
	// Upgrade cg to bottom if it is currently uninitialized
	cg->initialize(indent+"    ");
	
	cout << indent << "cg->divLattices=\n";
	cout << indent << "    "<<cg->DivLattices2Str(indent+"    ")<<"\n";
	
	// Plain assignment: lhs = rhs
	if(isSgAssignOp(n.getNode())) {
		varID res = SgExpr2Var(isSgExpression(n.getNode()));
		varID lhs = SgExpr2Var(isSgAssignOp(n.getNode())->get_lhs_operand());
		varID rhs = SgExpr2Var(isSgAssignOp(n.getNode())->get_rhs_operand());
		cout << indent << "res="<<res.str()<<" lhs="<<lhs.str()<<" rhs="<<rhs.str()<<"\n";
		bool resLive = (liveVars.find(res) != liveVars.end());
		bool lhsLive = (liveVars.find(lhs) != liveVars.end());
		
		// ??? ConstrGraph* resLat = dynamic_cast<ConstrGraph*>(prodLat->getVarLattice(res));
		// ??? ConstrGraph* lhsLat = dynamic_cast<ConstrGraph*>(prodLat->getVarLattice(lhs));
		// ??? ConstrGraph* rhsLat = dynamic_cast<ConstrGraph*>(prodLat->getVarLattice(rhs));
		// ??? 
		// ??? if(resLat) cout << "resLat=\n    "<<resLat->str(indent+"    ")<<"\n";
		// ??? if(lhsLat) cout << "lhsLat=\n    "<<lhsLat->str(indent+"    ")<<"\n";
		// ??? if(rhsLat) cout << "rhsLat=\n    "<<rhsLat->str(indent+"    ")<<"\n";
		
		// Copy the lattice of the right-hand-side to both the left-hand-side variable and to the assignment expression itself
		// ??? if(resLat) {
		if(resLive) {
			// Add the constraint res=rhs
			modified = cg->assign(res, rhs, 1, 1, 0, indent+"    ") || modified;
			// Copy the constraints represetented by rhs to res
			//??? modified = resLat->copyFromReplace(*rhsLat, res, rhs, indent+"    ") || modified;
		}
		// If the left-hand-side contains a live expression or variable
		// ??? if(lhsLat) {
		if(lhsLive) {
			// Add the constraint lhs=rhs
			modified = cg->assign(lhs, rhs, 1, 1, 0) || modified;
			// Copy the constraints represetented by rhs to lhs
			//??? modified = resLat->copyFromReplace(*asgnLat, lhs, rhs, indent+"    ") || modified;
		}
	// Initializer for a variable
	} else if(isSgAssignInitializer(n.getNode())) {
		varID res = SgExpr2Var(isSgAssignInitializer(n.getNode()));
		varID asgn = SgExpr2Var(isSgAssignInitializer(n.getNode())->get_operand());

		// ??? ConstrGraph* asgnLat = dynamic_cast<ConstrGraph*>(prodLat->getVarLattice(asgn));
		// ??? ConstrGraph* resLat = dynamic_cast<ConstrGraph*>(prodLat->getVarLattice(res));
		//if(asgnLat) cout << "asgnLat=    "<<asgnLat->str(indent+"    ")<<"\n";
		//if(resLat) cout << "resLat=    "<<resLat->str(indent+"    ")<<"\n";

		// If the result expression is live
		// ??? if(resLat) {
		if(liveVars.find(res) != liveVars.end()) {
			// Add the constraint res=asgn
			modified = cg->assign(res, asgn, 1, 1, 0) || modified;
			// Copy the constraints represetented by asgn to lhs and record that res = asgn
			//??? modified = resLat->copyFromReplace(*asgnLat, res, asgn, indent+"    ") || modified;
		}
	// Variable Declaration
	} else if(isSgInitializedName(n.getNode())) {
		SgInitializedName* initName = isSgInitializedName(n.getNode());
		varID var(initName);
		// ??? ConstrGraph* varLat = dynamic_cast<ConstrGraph*>(prodLat->getVarLattice(var));
		
		//cout << "DivAnalysis::transfer() isSgInitializedName var="<<var.str()<<" varLat="<<varLat<<"\n";
		
		// If this is a scalar that we care about, initialize it to Bottom
		// ??? if(varLat)
		if(liveVars.find(var) != liveVars.end())
		{
			//if(divAnalysisDebugLevel>=1) cout << "Variable declaration: "<<var.str()<<", get_initializer()="<<initName->get_initializer()<<"\n";
			// If there is NO initializer
			if(initName->get_initializer()==NULL) {
				// Initialize the constraints on var's value and constraints represented by var to Bottom
				modified = cg->assignBot(var);
				//??? modified = varLat->setToBottom() || modified;
			// If there is an initializer
			} else {
				varID init = SgExpr2Var(initName->get_initializer());
				// ??? ConstrGraph* initLat = dynamic_cast<ConstrGraph*>(prodLat->getVarLattice(init));
				//if(divAnalysisDebugLevel>=1) cout << "    init="<<init.str()<<" initLat="<<initLat<<"\n";
				// ??? if(initLat) {
				if(liveVars.find(init) != liveVars.end()) {
					// Add the constraint var=init
					modified = cg->assign(var, init, 1, 1, 0) || modified;
					// Copy the constraints represented by init to var and record that var = init
					//??? modified = varLat->copyFromReplace(*initLat, var, init, indent+"    ") || modified;
				}
			}
		}
	// Integral Numeric Constants
	} else if(isSgLongLongIntVal(n.getNode())         || isSgLongIntVal(n.getNode()) || 
	          isSgIntVal(n.getNode())                 || isSgShortVal(n.getNode()) ||
	          isSgUnsignedLongLongIntVal(n.getNode()) || isSgUnsignedLongVal(n.getNode()) || 
	          isSgUnsignedIntVal(n.getNode())         || isSgUnsignedShortVal(n.getNode())) {

		varID res = SgExpr2Var(isSgExpression(n.getNode()));
		//??? ConstrGraph* resLat = dynamic_cast<ConstrGraph*>(prodLat->getVarLattice(res));
		
		// If the result expression is live
		// ??? if(resLat) {
		if(liveVars.find(res) != liveVars.end()) {
			// Add the constraint var=constant
			     if(isSgLongLongIntVal(n.getNode()))         modified = cg->assign(res, zeroVar, 1, 1, isSgLongLongIntVal(n.getNode())->get_value())         || modified;
			else if(isSgLongIntVal(n.getNode()))             modified = cg->assign(res, zeroVar, 1, 1, isSgLongIntVal(n.getNode())->get_value())             || modified;
			else if(isSgIntVal(n.getNode()))                 modified = cg->assign(res, zeroVar, 1, 1, isSgIntVal(n.getNode())->get_value())                 || modified;
			else if(isSgShortVal(n.getNode()))               modified = cg->assign(res, zeroVar, 1, 1, isSgShortVal(n.getNode())->get_value())               || modified;
			else if(isSgUnsignedLongLongIntVal(n.getNode())) modified = cg->assign(res, zeroVar, 1, 1, isSgUnsignedLongLongIntVal(n.getNode())->get_value()) || modified;
			else if(isSgUnsignedLongVal(n.getNode()))        modified = cg->assign(res, zeroVar, 1, 1, isSgUnsignedLongVal(n.getNode())->get_value())        || modified;
			else if(isSgUnsignedIntVal(n.getNode()))         modified = cg->assign(res, zeroVar, 1, 1, isSgUnsignedIntVal(n.getNode())->get_value())         || modified;
			else if(isSgUnsignedShortVal(n.getNode()))       modified = cg->assign(res, zeroVar, 1, 1, isSgUnsignedShortVal(n.getNode())->get_value())       || modified;
				
			// Update resLat to represent the logical expression "val != 0"
			// ??? ConstrGraph* resLatCopy = dynamic_cast<ConstrGraph*>(resLat->copy()); // Copy of the old value of resLat
			// ??? resLat->setToBottom();
			// ???      if(isSgLongLongIntVal(n.getNode()))         resLat->assign(res, zeroVar, 1, 1, isSgLongLongIntVal(n.getNode())->get_value());
			// ??? else if(isSgLongIntVal(n.getNode()))             resLat->assign(res, zeroVar, 1, 1, isSgLongIntVal(n.getNode())->get_value());
			// ??? else if(isSgIntVal(n.getNode()))                 resLat->assign(res, zeroVar, 1, 1, isSgIntVal(n.getNode())->get_value());
			// ??? else if(isSgShortVal(n.getNode()))               resLat->assign(res, zeroVar, 1, 1, isSgShortVal(n.getNode())->get_value());
			// ??? else if(isSgUnsignedLongLongIntVal(n.getNode())) resLat->assign(res, zeroVar, 1, 1, isSgUnsignedLongLongIntVal(n.getNode())->get_value());
			// ??? else if(isSgUnsignedLongVal(n.getNode()))        resLat->assign(res, zeroVar, 1, 1, isSgUnsignedLongVal(n.getNode())->get_value());
			// ??? else if(isSgUnsignedIntVal(n.getNode()))         resLat->assign(res, zeroVar, 1, 1, isSgUnsignedIntVal(n.getNode())->get_value());
			// ??? else if(isSgUnsignedShortVal(n.getNode()))       resLat->assign(res, zeroVar, 1, 1, isSgUnsignedShortVal(n.getNode())->get_value());
			// ??? resLat->negate();
			// ??? 
			// ??? modified = (*resLat == resLatCopy) || modified;
			// ??? delete resLatCopy;
		}
	// Boolean Numeric Constant
	} else if(isSgBoolValExp(n.getNode())) {
		varID res = SgExpr2Var(isSgExpression(n.getNode()));
		// ??? ConstrGraph* resLat = dynamic_cast<ConstrGraph*>(prodLat->getVarLattice(res));
		
		// If the result expression is live
		// ??? if(resLat) {
		if(liveVars.find(res) != liveVars.end()) {
			// Add the constraint res=Top, since res is not representable as a numeric integer
			modified = cg->assignTop(res) || modified;
			
			// ??? // Set resLat to be the constraint represented by this boolean
			// ??? // Boolean = TRUE
			// ??? if(isSgBoolValExp(n.getNode())->get_value())
			// ??? 	// True if equivalent to no constraints at all. True or CG = True / Bottom or CG = Bottom. True AND CG = CG / Bottom AND CG = CG
			// ??? 	modified = resLat->setToBottom() || modified;
			// ??? // Boolean = FALSE
			// ??? else
			// ??? 	// False if equivalent to impossible constraints. False or CG = CG / Top or CG = CG. False AND CG = False / Top AND CG = Top
			// ??? 	modified = resLat->setToTop() || modified;
		}
	// Non-integral Constants
	} else if(isSgValueExp(n.getNode())) {
		varID res = SgExpr2Var(isSgExpression(n.getNode()));
		// ??? ConstrGraph* resLat = dynamic_cast<ConstrGraph*>(prodLat->getVarLattice(res));
		// ??? 
		// ??? // If the result expression is live
		// ??? if(resLat) {
		// ??? 	// Add the constraint res=Top, since res is not representable as a numeric integer
		// ??? 	modified = cg->assignTop(res) || modified;
		// ??? }
	// Arithmetic Operations
	} else if(isSgPlusAssignOp(n.getNode())  || isSgAddOp(n.getNode())      ||
	          isSgMinusAssignOp(n.getNode()) || isSgSubtractOp(n.getNode()) ||
	          isSgModAssignOp(n.getNode())   || isSgModOp(n.getNode())      ||
	          isSgMultAssignOp(n.getNode())  || isSgMultiplyOp(n.getNode()) ||
	          isSgDivAssignOp(n.getNode())   || isSgDivideOp(n.getNode())   ||
	          isSgMinusMinusOp(n.getNode())  || isSgMinusOp(n.getNode())    ||
	          isSgPlusPlusOp(n.getNode())    || isSgUnaryAddOp(n.getNode())
	          // Mod and exponentiation
	          ) {
		varID lhs, arg1, arg2;
		varID res = SgExpr2Var(isSgExpression(n.getNode()));
		// ConstrGraph *resLat=NULL, *lhsLat=NULL, *arg1Lat=NULL, *arg2Lat=NULL;
		bool resLive = (liveVars.find(res) != liveVars.end());
		bool lhsLive=false, arg1Live=false, arg2Live=false;
		
		// Set up the information on the arguments and target of the arithmetic operation
		if(isSgBinaryOp(n.getNode())) {
			if(isSgPlusAssignOp(n.getNode()) || isSgMinusAssignOp(n.getNode()) ||
			   isSgModAssignOp(n.getNode())  || isSgMultAssignOp(n.getNode())  ||
			   isSgDivAssignOp(n.getNode())) {
				lhs = SgExpr2Var(isSgBinaryOp(n.getNode())->get_lhs_operand());
				arg1 = lhs;
				arg2 = SgExpr2Var(isSgBinaryOp(n.getNode())->get_rhs_operand());
				lhsLive = (liveVars.find(lhs) != liveVars.end());
				//cout << "lhs="<<lhs.str()<<" arg1="<<arg1.str()<<" arg2="<<arg2.str()<<"\n";
			} else {
				arg1 = SgExpr2Var(isSgBinaryOp(n.getNode())->get_lhs_operand());
				arg2 = SgExpr2Var(isSgBinaryOp(n.getNode())->get_rhs_operand());
			}
			// ??? arg1Lat = dynamic_cast<ConstrGraph*>(prodLat->getVarLattice(arg1));
			// ??? arg2Lat = dynamic_cast<ConstrGraph*>(prodLat->getVarLattice(arg2));
			arg1Live = (liveVars.find(arg1) != liveVars.end());;
			arg2Live = (liveVars.find(arg2) != liveVars.end());
		} else if(isSgUnaryOp(n.getNode())) {
			ROSE_ASSERT(isSgPlusPlusOp(n.getNode()) || isSgMinusMinusOp(n.getNode()));
			lhs = SgExpr2Var(isSgUnaryOp(n.getNode())->get_operand());
			lhsLive = (liveVars.find(lhs) != liveVars.end());
			
			arg1 = SgExpr2Var(isSgUnaryOp(n.getNode())->get_operand());
			arg1Live = (liveVars.find(arg1) != liveVars.end());
			arg2 = oneVar;
			arg2Live = true;
			// ??? arg1Lat = dynamic_cast<ConstrGraph*>(prodLat->getVarLattice(arg1));
			// Unary Update
			/*if(isSgMinusMinusOp(n.getNode()) || isSgPlusPlusOp(n.getNode())) {
				arg2Lat = new ConstrGraph(1);
			}*/
			//cout << "res="<<res.str()<<" arg1="<<arg1.str()<<" arg1Lat="<<arg1Lat<<", arg2Lat="<<arg2Lat<<"\n";
		}
		// ??? resLat = dynamic_cast<ConstrGraph*>(prodLat->getVarLattice(res));
		cout << indent << "res="<<res<<" resLive="<<resLive<<" lhs="<<lhs<<" lhsLive="<<lhsLive<<" arg1="<<arg1<<" arg1Live="<<arg1Live<<" arg2="<<arg2<<" arg2Live="<<arg2Live<<"\n";
		
		// If the result expression is dead but the left-hand-side of the expression is live,
		// update the left-hand-side with the result
		// ??? if(resLat==NULL && 
		// ??? 	(isSgPlusAssignOp(n.getNode()) || isSgMinusAssignOp(n.getNode()) ||
		// ??? 	 isSgMultAssignOp(n.getNode()) || isSgDivAssignOp(n.getNode()) ||
		// ??? 	 isSgModAssignOp(n.getNode())) &&
		// ??? 	prodLat->getVarLattice(lhs)!=NULL)
		// ??? { lhsLat = dynamic_cast<ConstrGraph*>(prodLat->getVarLattice(lhs)); }
		
		//cout << "transfer C, resLat="<<resLat<<"\n";
		// If the result or left-hand-side expression as well as the arguments are live
		// ??? if((resLat ||lhsLat) && arg1Lat && arg2Lat) {
		if((resLive || lhsLive) && arg1Live && arg2Live) {
			// ADDITION / SUBTRACTION / MULTIPLICATION / DIVISION / MODULUS
			if(isSgPlusAssignOp(n.getNode())  || isSgAddOp(n.getNode()) ||
		      isSgMinusAssignOp(n.getNode()) || isSgSubtractOp(n.getNode()) ||
		      isSgMinusMinusOp(n.getNode())  || isSgPlusPlusOp(n.getNode()) ||
		      isSgMultAssignOp(n.getNode())  || isSgMultiplyOp(n.getNode()) ||
		      isSgDivAssignOp(n.getNode())   || isSgDivideOp(n.getNode()) ||
		      isSgModAssignOp(n.getNode())   || isSgModOp(n.getNode())) {
		      		      
		      // If either arg1 or arg2 is a constant, these variables hold the relationship between it and zeroVar
		      int a_arg1, b_arg1, c_arg1, a_arg2, b_arg2, c_arg2;
		      bool resToTop = false; // True if the expression arg1+arg2 turns out to be too complex to be represented
		      varID var; // The variable that res will be related to after the addition, or zeroVar if res will be a constant
		      int b_res=1, c_res=0; // The value of b and c used in the assignment res = var*b + c
		      
		      // ADDITION / SUBTRACTION
		      if(isSgPlusAssignOp(n.getNode())  || isSgAddOp(n.getNode()) ||
		      	isSgMinusAssignOp(n.getNode()) || isSgSubtractOp(n.getNode()) ||
		      	isSgMinusMinusOp(n.getNode())  || isSgPlusPlusOp(n.getNode())) {
		      		cout << indent << "ADDITION / SUBTRACTION\n";
			      // ??? if(arg1Lat->isEqVars(arg1, zeroVar, a_arg1, b_arg1, c_arg1)) {
			      if(cg->isEqVars(arg1, zeroVar, a_arg1, b_arg1, c_arg1)) {
			      	// If both arg1 and arg2 are constants
			      	// ??? if(arg2Lat->isEqVars(arg2, zeroVar, a_arg2, b_arg2, c_arg2)) {
			      	if(arg2==oneVar) {
			      		// arg1*a_arg1 = 0 + c_arg1 => arg1 = c_arg1/a_arg1
			      		// arg2 = 1
			      		// arg1 + arg2 = c_arg1/a_arg1 + 1
			      		c_res = c_arg1/a_arg1 + 1;
			      		var = zeroVar;
			      		goto DONE_NO_TOP;
			      	} else if(cg->isEqVars(arg2, zeroVar, a_arg2, b_arg2, c_arg2)) {
			      		// arg1*a_arg1 = 0 + c_arg1 => arg1 = c_arg1/a_arg1
			      		// arg2*a_arg2 = 0 + c_arg2 => arg2 = c_arg2/a_arg2
			      		// arg1 + arg2 = c_arg1/a_arg1 + c_arg2/a_arg2
			      		c_res = c_arg1/a_arg1 + c_arg2/a_arg2;
			      		var = zeroVar;
			      		goto DONE_NO_TOP;
			      	// If arg1 is a constant but arg2 is not
			      	} else {
			      		// arg1*a_arg1 = 0 + c_arg1 => arg1 = c_arg1/a_arg1
			      		// arg1 + arg2 = c_arg1/a_arg1 + arg2
			      		c_res = c_arg1/a_arg1;
			      		var = arg2;
			      		goto DONE_NO_TOP;
			      	}
			      // If arg1 is not constant but arg2==1
			      } else if(arg2==oneVar) {
			      	ROSE_ASSERT(isSgPlusPlusOp(n.getNode()) || isSgMinusMinusOp(n.getNode()));
			      	// If this is an increment operation
			      	if(isSgPlusPlusOp(n.getNode())) {
				      	// res = arg1 + 1
				      	c_res = 1;
				      	var = arg1;
				      	goto DONE_NO_TOP;
				      // If this is an decrement operation
				      } else if(isSgMinusMinusOp(n.getNode())) {
				      	// res = arg1 - 1
				      	c_res = -1;
				      	var = arg1;
				      	goto DONE_NO_TOP;
				      }
			      // If arg1 is not constant but arg2 is constant
			      // ??? } else if(arg2Lat->isEqVars(arg2, zeroVar, a_arg2, b_arg2, c_arg2)) {
			      } else if(cg->isEqVars(arg2, zeroVar, a_arg2, b_arg2, c_arg2)) {
			      	// arg2*a_arg2 = 0 + c_arg2 => arg2 = c_arg2/a_arg2
			      	// arg1 + arg2 = arg1 + c_arg2/a_arg2
			      	c_res = c_arg2/a_arg2;
			      	var = arg1;
			      	goto DONE_NO_TOP;
			      // Else, if neither input variable is a constant
			      } else {
			      	// There is no x*a<=y*b+c constraint that describes res's relationship to arg1 or arg2
			   	}
			   // MULTIPLICATION
		   	} else if(isSgMultAssignOp(n.getNode())  || isSgMultiplyOp(n.getNode())) {
			   	// ??? if(arg1Lat->isEqVars(arg1, zeroVar, a_arg1, b_arg1, c_arg1)) {
			   	if(cg->isEqVars(arg1, zeroVar, a_arg1, b_arg1, c_arg1)) {
			      	// If both arg1 and arg2 are constants
			      	// ??? if(arg2Lat->isEqVars(arg2, zeroVar, a_arg2, b_arg2, c_arg2)) {
			      	if(cg->isEqVars(arg2, zeroVar, a_arg2, b_arg2, c_arg2)) {
			      		// arg1*a_arg1 = 0 + c_arg1 => arg1 = c_arg1/a_arg1
			      		// arg2*a_arg2 = 0 + c_arg2 => arg2 = c_arg2/a_arg2
			      		// arg1 * arg2 = c_arg1/a_arg1 * c_arg2/a_arg2
			      		if(((a_arg1*a_arg2) % (c_arg1*c_arg2) == 0)) {
				      		c_res = (c_arg1*c_arg2)/(a_arg1*a_arg2);
				      		var = zeroVar;
				      		goto DONE_NO_TOP;
				      	}
			      	// If arg1 is a constant but arg2 is not
			      	} else {
			      		// arg1*a_arg1 = 0 + c_arg1 => arg1 = c_arg1/a_arg1
			      		// arg1 * arg2 = c_arg1/a_arg1 * arg2
			      		if((a_arg1 % c_arg1) == 0) {
				      		b_res = c_arg1/a_arg1;
				      		var = arg2;
				      		goto DONE_NO_TOP;
				      	}
			      	}
			      // If arg1 is not constant but arg2 is constant
			      // ??? } else if(arg2Lat->isEqVars(arg2, zeroVar, a_arg2, b_arg2, c_arg2)) {
			      } else if(cg->isEqVars(arg2, zeroVar, a_arg2, b_arg2, c_arg2)) {
			      	// arg2*a_arg2 = 0 + c_arg2 => arg2 = c_arg2/a_arg2
			      	// arg1 + arg2 = arg1 * c_arg2/a_arg2
			      	if((a_arg2 % c_arg2) == 0) {
				      	b_res = c_arg2/a_arg2;
				      	var = arg1;
				      	goto DONE_NO_TOP;
				      }
			      // Else, if neither input variable is a constant
			      } else {
			      	// There is no x*a<=y*b+c constraint that describes res's relationship to arg1 or arg2
			   	}
				// DIVISION
			   } else if(isSgDivAssignOp(n.getNode())   || isSgDivideOp(n.getNode())) {
					// ??? if(arg1Lat->isEqVars(arg1, zeroVar, a_arg1, b_arg1, c_arg1)) {
					if(cg->isEqVars(arg1, zeroVar, a_arg1, b_arg1, c_arg1)) {
			      	// If both arg1 and arg2 are constants
			      	// ??? if(arg2Lat->isEqVars(arg2, zeroVar, a_arg2, b_arg2, c_arg2)) {
			      	if(cg->isEqVars(arg2, zeroVar, a_arg2, b_arg2, c_arg2)) {
			      		// arg1*a_arg1 = 0 + c_arg1 => arg1 = c_arg1/a_arg1
			      		// arg2*a_arg2 = 0 + c_arg2 => arg2 = c_arg2/a_arg2
			      		// arg1 / arg2 = c_arg1/a_arg1 / c_arg2/a_arg2 
			      		if((a_arg1*c_arg2 % (c_arg1*a_arg2)) == 0) {
				      		c_res = (c_arg1*a_arg2)/(a_arg1*c_arg2);
				      		var = zeroVar;
				      		goto DONE_NO_TOP;
				      	}
			      	// If arg1 is a constant but arg2 is not
			      	} else {
			      		// arg1*a_arg1 = 0 + c_arg1 => arg1 = c_arg1/a_arg1
			      		// arg1 / arg2 = c_arg1/a_arg1 / arg2
			      		// There is no x*a<=y*b+c constraint that describes res's relationship to arg1 or arg2
			      	}
			      // If arg1 is not constant but arg2 is constant
			      // ??? } else if(arg2Lat->isEqVars(arg2, zeroVar, a_arg2, b_arg2, c_arg2)) {
			      } else if(cg->isEqVars(arg2, zeroVar, a_arg2, b_arg2, c_arg2)) {
			      	// arg2*a_arg2 = 0 + c_arg2 => arg2 = c_arg2/a_arg2
			      	// arg1 / arg2 = arg1 / c_arg2/a_arg2 = arg1 * a_arg2/c_arg2
			      	if((a_arg2 % c_arg2) == 0) {
				      	b_res = a_arg2/c_arg2;
				      	var = arg1;
				      	goto DONE_NO_TOP;
				      }
			      // Else, if neither input variable is a constant
			      } else {
			      	// There is no x*a<=y*b+c constraint that describes res's relationship to arg1 or arg2
			   	}
			   // MODULUS
				} else if(isSgModAssignOp(n.getNode()) || isSgModOp(n.getNode())) {
					// ??? if(arg1Lat->isEqVars(arg1, zeroVar, a_arg1, b_arg1, c_arg1)) {
					if(cg->isEqVars(arg1, zeroVar, a_arg1, b_arg1, c_arg1)) {
			      	// If both arg1 and arg2 are constants
			      	// ??? if(arg2Lat->isEqVars(arg2, zeroVar, a_arg2, b_arg2, c_arg2)) {
			      	if(cg->isEqVars(arg2, zeroVar, a_arg2, b_arg2, c_arg2)) {
			      		// arg1*a_arg1 = 0 + c_arg1 => arg1 = c_arg1/a_arg1
			      		// arg2*a_arg2 = 0 + c_arg2 => arg2 = c_arg2/a_arg2
			      		// arg1 % arg2 = c_arg1/a_arg1 % c_arg2/a_arg2
			      		if(((a_arg1 % c_arg1) == 0) && ((a_arg2 % c_arg2) == 0)) {
				      		c_res = (c_arg1/a_arg1) % (c_arg2/a_arg2);
				      		var = zeroVar;
				      		goto DONE_NO_TOP;
				      	}
			      	// If arg1 is a constant but arg2 is not
			      	} else {
			      		// arg1*a_arg1 = 0 + c_arg1 => arg1 = c_arg1/a_arg1
			      		// arg1 % arg2 = c_arg1/a_arg1 % arg2
			      		// There is no x*a<=y*b+c constraint that describes res's relationship to arg1 or arg2
			      	}
			      // If arg1 is not constant but arg2 is constant
			      // ??? } else if(arg2Lat->isEqVars(arg2, zeroVar, a_arg2, b_arg2, c_arg2)) {
			      } else if(cg->isEqVars(arg2, zeroVar, a_arg2, b_arg2, c_arg2)) {
			      	// arg2*a_arg2 = 0 + c_arg2 => arg2 = c_arg2/a_arg2
			      	// arg1 % arg2 = arg1 % c_arg2/a_arg2 :
			      	//    res = arg1 % c_arg2/a_arg2 <= var * b + c
			      	// There is no clean x*a<=y*b+c constraint that describes res's relationship to arg1 or arg2
			      	// THIS MAY BE POSSIBLE BY USING DIVISIBILITY VARIABLES BUT SINCE WE'RE ALREADY INCORPORATING THE RESULTS OF DivAnalysis, THIS IS PROBABLY NOT NEEDED.
			      // Else, if neither input variable is a constant
			      } else {
			      	// There is no x*a<=y*b+c constraint that describes res's relationship to arg1 or arg2
			   	}
				}
				
				// If we couldn't find a way to represent the results of the arithmetic operation statically,
				// Represent it as top
				cout << indent <<" Assigning "<<res<<"(resLive="<<resLive<<") and/or "<<lhs<<"(lhsLive="<<lhsLive<<") to Top\n";
	      	if(resLive) modified = cg->assignTop(res, indent+"    ") || modified;
	      	if(lhsLive) modified = cg->assignTop(lhs, indent+"    ") || modified;
	      	resToTop = true;
				
				// The code above skips here if it was able to represent the results of the arithmetic operation
				DONE_NO_TOP:
				
		   	// Update cg to include the assignment res=arg1+arg2.
		   	// Update resLat to represent the logical expression "arg1+arg1 != 0".
				// If the expression is too complex to be represented.
				if(resToTop)
					// ??? modified = resLat->setToTop() || modified;
					;
					// No need to negate top since neg(Top) = Top
				// If the expression can be represented.
				else {
					cout << indent <<" Assigning "<<res<<"(resLive="<<resLive<<") and/or "<<lhs<<"(lhsLive="<<lhsLive<<") to = "<<var<<"*"<<b_res<<" + "<<c_res<<"\n";
					if(resLive) modified = cg->assign(res, var, 1, b_res, c_res, indent+"    ") || modified;
					if(lhsLive) modified = cg->assign(lhs, var, 1, b_res, c_res, indent+"    ") || modified;
					
					// Create the copy of resLat if we need to check if resLat changes
					/*ConstrGraph* resLatCopy=NULL;
		      	if(!modified) resLatCopy = dynamic_cast<ConstrGraph*>(resLat->copy());
					resLat->setToBottom();*/
					//??? resLat->assign(res, var, 1, b_res, c_res);
					//??? modified = resLat->copyFromReplace(*resLat, res, var, indent+"    ") || modified;
					//??? modified = resLat->negate(indent+"    ") || modified;
					/*if(resLatCopy) {
						modified = (*resLat == resLatCopy) || modified;
						delete resLatCopy;
					}*/
				}
			// Negation
			} else if(isSgMinusOp(n.getNode())) {
				// res = -arg1, which is too complex too be represented
		      modified = cg->assignTop(res) || modified;
		      //??? modified = resLat->setToTop() || modified;
			// PLUS SIGN
			} else if(isSgUnaryAddOp(n.getNode())) {
				modified = cg->assign(res, arg1, 1, 1, 0) || modified;
				//??? modified = resLat->copyFromReplace(*asgnLat, res, arg1, indent+"    ") || modified;
			}
			
			// If there is a left-hand side, copy the final lattice to the lhs variable
			/* ??? if(isSgPlusAssignOp(n.getNode()) || isSgMinusAssignOp(n.getNode()) ||
				isSgMultAssignOp(n.getNode()) || isSgDivAssignOp(n.getNode()) ||
				isSgModAssignOp(n.getNode())) {
			if(lhsLive) {
				// If we didn't use the lhs lattice as resLat, copy resLat into lhsLat
				//cout << "prodLat->getVarLattice("<<res.str()<<")="<<prodLat->getVarLattice(res)<<"\n";
				if(prodLat->getVarLattice(res)!=NULL) {
					//ConstrGraph* lhsLat = dynamic_cast<ConstrGraph*>(prodLat->getVarLattice(lhs));
					//cout << "prodLat->getVarLattice("<<lhs.str()<<")="<<lhsLat<<"\n";
					//if(lhsLat){ // If the left-hand-side contains an identifiable, live variable
					
						cg->assign(lhs, res, 1, 1, 0);
						
						//??? modified = lhsLat->copyFromReplace(*resLat, lhs, res, indent+"    ") || modified;
					}
				}
			}*/
		}
		
		// Deallocate newly-created objects
		/*if(isSgMinusMinusOp(n.getNode()) || isSgPlusPlusOp(n.getNode()))
			delete arg2Lat;*/
	// !!!NEED CONDITIONALS!!!
	//} else if(isSgLessThanOp(n.getNode()) ||
	} else
		modified = false;
	
	cout << indent << "Current Operation Transferred:\n";
	cout << indent << cg->str(indent+"    ") << "\n";
	
	/*// incorporate this node's inequalities from conditionals
	incorporateConditionalsInfo(func, n, state, dfInfo);
	
cout << indent << "mid2-Transfer Function:\n";
cout << indent << cg->str(indent+"    ") << "\n";*/
	
	// Incorporate this node's divisibility information
	/*incorporateDivInfo(func, n, state, dfInfo, indent+"    ");

	cout << indent << "Divisibility Information Incorporated:\n";
	cout << indent << cg->str(indent+"    ") << "\n";*/
	
	cg->endTransaction();
	
	cout << indent << "Divisibility + Closure:\n";
	cout << indent << cg->str(indent+"    ") << "\n";
	
	//cg->beginTransaction();
	
	//NEED TO PERFORM CLOSURE HERE BECAUSE WE HAVEN'T YET CONNECTED DIVISIBILITY VARIABLES TO ANYTHING ELSE.
	
	/*removeConstrDivVars(func, n, state, dfInfo, indent+"    ");
	cg->divVarsClosure(indent+"    ");*/
	//cg->endTransaction();
	
	return modified;
}

/* // Incorporates the current node's inequality information from conditionals (ifs, fors, etc.) into the current node's 
// constraint graph.
// returns true if this causes the constraint graph to change and false otherwise
bool ConstrGraphAnalysis::incorporateConditionalsInfo(const Function& func, const DataflowNode& n, 
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
}*/

// Incorporates the current node's divisibility information into the current node's constraint graph
// returns true if this causes the constraint graph to change and false otherwise
/*bool ConstrGraphAnalysis::incorporateDivInfo(const Function& func, const DataflowNode& n, NodeState& state, const vector<Lattice*>& dfInfo, string indent)
{
	bool modified = false;
	ConstrGraph* cg = dynamic_cast<ConstrGraph*>(dfInfo.front());
	
	//cout << indent << "incorporateDivInfo()\n";
	varIDSet liveVars = getAllLiveVarsAt(ldva, n, state, "");
	for(varIDSet::iterator it = liveVars.begin(); it!=liveVars.end(); it++)
	{
		//cout << indent << "    "<<(*it)<<"\n";
		varID var = *it;
		// Re-add the connection between var and its divisibility variable. The divisibility variable
		// needs to be disconnected from the other variables so that we can re-compute its new relationships
		// solely based on its relationship with var. If we didn't do this, and var's relationship to its
		// divisibility variable changed from one CFG node to the next (i.e. from var=divvar to var = b*divvar),
		// we would not be able to capture this change.
		cg->addDivVar(var, true, indent+"    ");
		
		/* // create the divisibility variable for the current variable 
		varID divVar = ConstrGraph::getDivScalar(var);* /
		
		/*DivLattice* varDivL = dynamic_cast<DivLattice*>(prodL->getVarLattice(func, var));
		
		// incorporate this variable's divisibility information (if any)
		if(varDivL->getLevel() == DivLattice::divKnown && !(varDivL->getDiv()==1 && varDivL->getRem()==0))
		{
			modified = cg->addDivVar(var, varDivL->getDiv(), varDivL->getRem()) || modified;
		}
		else if(varDivL->getLevel() != DivLattice::bottom)
		{
			modified = cg->addDivVar(var, 1, 0) || modified;
		}* /
	}
	
	return modified;
}*/

// For any variable for which we have divisibility info, remove its constraints to other variables (other than its
// divisibility variable)
bool ConstrGraphAnalysis::removeConstrDivVars(const Function& func, const DataflowNode& n, NodeState& state, const vector<Lattice*>& dfInfo, string indent)
{
	bool modified = false;
	ConstrGraph* cg = dynamic_cast<ConstrGraph*>(dfInfo.front());
	
	cout << indent << "removeConstrDivVars()\n";
	cout << indent << "     Initial cg="<<cg->str(indent+"        ")<<"\n";
	
	varIDSet liveVars = getAllLiveVarsAt(ldva, n, state, "");
	for(varIDSet::iterator it = liveVars.begin(); it!=liveVars.end(); it++)
	{
		varID var = *it;
		//cg->disconnectDivOrigVar(var, indent+"     ");
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
	
	cout << indent << "    Final cg="<<cg->str(indent+"    ") << "\n";
	
	return modified;
}

// Prints the Lattices set by the given ConstrGraphAnalysis
void printConstrGraphAnalysisStates(ConstrGraphAnalysis* cga, string indent)
{
	vector<int> factNames;
	vector<int> latticeNames;
	latticeNames.push_back(0);
	latticeNames.push_back(1);
	printAnalysisStates pas(cga, factNames, latticeNames, printAnalysisStates::below, indent);
	UnstructuredPassInterAnalysis upia_pas(pas);
	upia_pas.runAnalysis();
}
