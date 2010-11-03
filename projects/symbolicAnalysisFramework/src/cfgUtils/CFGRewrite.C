#include "rose.h"
#include "patternRewrite.h"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <functional>
#include <utility>
#include <stdint.h>

#include "rwAccessLabeler.h"
#include "CFGRewrite.h"

using namespace std;
using namespace rwAccessLabeler;

/*** PULLED FROM replaceExpressionWithStatement.C ***/
			void FixSgTree(SgNode*);
/*** PULLED FROM replaceExpressionWithStatement.C ***/

namespace VirtualCFG {

#define SgDefaultFile Sg_File_Info::generateDefaultFileInfoForTransformationNode()
	
void initCFGRewrite(SgProject* project)
{
	// add the annotations that identify each part of the AST as used for read or write accesses
	addRWAnnotations(project);
}

// returns true if the given SgNode is a value expression or a computational expression with no side-effects
bool isNonMutatingOperator(SgNode* n) {
  ROSE_ASSERT (n);
  switch (n->variantT()) {
    case V_SgClassNameRefExp:
    case V_SgConditionalExp:
    case V_SgExprListExp:
    case V_SgFunctionRefExp:
    case V_SgMemberFunctionRefExp:
    case V_SgNullExpression:
    case V_SgRefExp:
    case V_SgSizeOfOp:
    case V_SgThisExp:
    case V_SgTypeIdOp:
    case V_SgVarRefExp:
    case V_SgAddOp:
    case V_SgAndOp:
    case V_SgArrowExp:
    case V_SgArrowStarOp:
    case V_SgBitAndOp:
    case V_SgBitOrOp:
    case V_SgBitXorOp:
    case V_SgCommaOpExp:
    case V_SgDivideOp:
    case V_SgDotExp:
    case V_SgDotStarOp:
    case V_SgEqualityOp:
    case V_SgGreaterOrEqualOp:
    case V_SgGreaterThanOp:
    case V_SgIntegerDivideOp:
    case V_SgLessOrEqualOp:
    case V_SgLessThanOp:
    case V_SgLshiftOp:
    case V_SgModOp:
    case V_SgMultiplyOp:
    case V_SgNotEqualOp:
    case V_SgOrOp:
    case V_SgPntrArrRefExp:
    case V_SgRshiftOp:
    case V_SgScopeOp:
    case V_SgSubtractOp:
    case V_SgAddressOfOp:
    case V_SgBitComplementOp:
    case V_SgMinusOp:
    case V_SgNotOp:
    case V_SgPointerDerefExp:
    case V_SgUnaryAddOp:
      return true;

    default: return isSgValueExp(n);
  }
}

class sideEffectsDetector : public AstBottomUpProcessing<bool>
{
	public:
	bool evaluateSynthesizedAttribute(SgNode* n, SynthesizedAttributesList children )
	{
		// if n has side-effects
		if(!isNonMutatingOperator(n)) return true;
			
		for(SynthesizedAttributesList::iterator it = children.begin();
		    it != children.end(); it++)
		   // if the current child has side-effects
			if((*it)) return true;
		
		// this sub-tree has no side-effects
		return false;
	}
	
	// by default a given blank sub-tree has no side-effects
	bool defaultSynthesizedAttribute() { return false; }
};

// returns true if the given SgNode's sub-tree has no side-effects
bool isNonMutatingSubTree(SgNode* n) {
	sideEffectsDetector d;
	return !d.traverse(n);
}

// if the given SgNode is an SgStatement, returns that SgStatement. Otherwise, if it is an
//    gExpression or SgInitializedName, wraps it in an SgStatement and returns that.
static SgStatement* convertToStatement(SgNode* n) {
	if (isSgStatement(n)) {
		return isSgStatement(n);
	} else if (isSgExpression(n)) {
		SgExprStatement* s = new SgExprStatement(SgDefaultFile, isSgExpression(n));
		n->set_parent(s);
		return s;
	} else if (isSgInitializedName(n)) {
		SgVariableDeclaration* vardecl = new SgVariableDeclaration(SgDefaultFile);
		vardecl->get_variables().push_back(isSgInitializedName(n));
		n->set_parent(vardecl);
		return vardecl;
	} else {
		ROSE_ASSERT (!"convertToStatement: Can't handle this case");
	}
}

// Replace the expression from with the expression to in the SgNode parent, which
//    must be from's parent. Function checks to ensure that it is used properly.
// Note that if parent is an SgInitializedName, to must be SgInitializer in order 
//    for the replacement to work properly.
void replaceExpressionChecked(SgNode* parent, SgExpression* from, SgExpression* to) {
	ROSE_ASSERT (from);
	ROSE_ASSERT (to);
	ROSE_ASSERT (parent);
	ROSE_ASSERT (parent != from);
	ROSE_ASSERT (parent != to);
	vector<SgNode*> children = parent->get_traversalSuccessorContainer();
	bool foundFrom = false;
	// verify that from is indeed one of parent's children
	for (size_t i = 0; i < children.size(); ++i) {
		if (children[i] == from) {foundFrom = true; break;}
	}
	// if this is not the case, yell
	if (!foundFrom) {
		cerr << "foundFrom failed: looking for " << from->unparseToString() << " in " << parent->unparseToString() << endl;
		ROSE_ASSERT (foundFrom);
	}
	
	if (isSgExprStatement(parent)) {
		// replace parent's expression (used to be from) with to
		ROSE_ASSERT (isSgExprStatement(parent)->get_expression() == from);
		isSgExprStatement(parent)->set_expression(to);
		to->set_parent(parent);
	} else if (isSgReturnStmt(parent)) {
		// replace parent's expression (used to be from) with to
		ROSE_ASSERT (isSgReturnStmt(parent)->get_expression() == from);
		isSgReturnStmt(parent)->set_expression(to);
		to->set_parent(parent);
	} else if (isSgExpression(parent)) {
		// replace parent's from sub-expression with to
		isSgExpression(parent)->replace_expression(from, to);
		to->set_parent(parent);
		// verify that to is now a sub-expression of parent
		vector<SgNode*> children = parent->get_traversalSuccessorContainer();
		for (size_t i = 0; i < children.size(); ++i) {
			if (children[i] == to) return;
		}
		ROSE_ASSERT (!"replace_expression didn't work");
	} else if (isSgInitializedName(parent)) {
		// replace parent's initializer with the to initializer
		ROSE_ASSERT (isSgInitializer(to));
		isSgInitializedName(parent)->set_initializer(isSgInitializer(to));
		to->set_parent(parent);
	} else if (isSgForStatement(parent)) {
		// ensure that we're only trying to replace the for statement's increment expression as 
		// the for statement's other components are statements, not expressions
		ROSE_ASSERT (isSgForStatement(parent)->get_increment() == from);
		isSgForStatement(parent)->set_increment(to);
		to->set_parent(parent);
	} else {
		cerr << "replaceExpressionChecked: parent is <" << parent->class_name() << " | " << parent->unparseToString() << ">, from is <" << from->class_name() << "> \n";
		ROSE_ASSERT (!"Unhandled case");
	}
}

// replace the from statement with the to statement in the parent SgNode
// Note: Only parent = SgBasicBlock
void replaceStatement(SgNode* parent, SgStatement* from, SgStatement* to) {
	switch (parent->variantT()) {
		case V_SgBasicBlock: {
			SgStatementPtrList& children = isSgBasicBlock(parent)->get_statements();
			SgStatementPtrList::iterator i = find(children.begin(), children.end(), from);
			ROSE_ASSERT (i != children.end());
			*i = to;
			to->set_parent(parent);
			break;
		}
		default: ROSE_ASSERT (!"Unhandled case");
	}
}

Sg_File_Info* getFileInfo(SgNode* n) {
  if (isSgLocatedNode(n)) {
    return isSgLocatedNode(n)->get_file_info();
  } else if (isSgInitializedName(n)) {
    return isSgInitializedName(n)->get_file_info();
  } else return 0;
}

string getFileInfoString(SgNode* n) {
  Sg_File_Info* fi = getFileInfo(n);
  if (!fi) return "";
  ostringstream o;
  o << fi->get_line() << ":" << fi->get_col();
  return o.str();
}

string getFileInfoString(CFGNode n) {
  return getFileInfoString(n.getNode());
}

// Returns the source of n's only in-edge. Yells if n has multiple in-edges.
CFGNode theInEdge(CFGNode n) {
  vector<CFGEdge> e = n.inEdges();
  ROSE_ASSERT (e.size() == 1);
  return e.front().source();
}

// Returns the target of n's only out-edge. Yells if n has multiple out-edges.
CFGNode theOutEdge(CFGNode n) {
  vector<CFGEdge> e = n.outEdges();
  ROSE_ASSERT (e.size() == 1);
  return e.front().target();
}

// FIXME -- these functions need to split subexpressions, since the expression
// evaluation order in the CFG and the one used by the compiler may be
// different

// replace expr with (newNode, expr)
// returns the resulting SgExpression
SgExpression* insertBeforeExpression(SgExpression* expr, SgExpression* newNode) {
	// Do the real insertion of a comma operator
	SgNode* oldParent = expr->get_parent();
	SgCommaOpExp* co = new SgCommaOpExp(SgDefaultFile, newNode, expr);
	// copy the access type from the original expression to the new comma expression
	cloneAccessType(co, expr);
	// wrap this comma expression in parentheses
	co->set_need_paren(true);
	replaceExpressionChecked(oldParent, expr, co);
	expr->set_parent(co);
	newNode->set_parent(co);
	
	return co;
}

// replaces expr with (expr, newNode)
// returns the resulting SgExpression
SgExpression* insertAfterExpression(SgExpression* expr, SgExpression* newNode) {
	// Do the real insertion of a comma operator
	SgNode* oldParent = expr->get_parent();
	SgCommaOpExp* co = new SgCommaOpExp(SgDefaultFile, expr, newNode);
	// copy the access type from the new expression to the comma expression
	cloneAccessType(co, expr);
	// wrap this comma expression in parentheses
	co->set_need_paren(true);
	replaceExpressionChecked(oldParent, expr, co);
	expr->set_parent(co);
	newNode->set_parent(co);
	
	return co;
}


// replace stmt; with {newNode; stmt;}
void replaceStatementByBlockBefore(SgStatement* stmt, SgStatement* newNode) {
	// Insert a block to wrap the two statements
	SgNode* parent = stmt->get_parent();
	SgBasicBlock* b = new SgBasicBlock(SgDefaultFile);
	b->get_statements().push_back(newNode);
	b->get_statements().push_back(stmt);
	newNode->set_parent(b);
	stmt->set_parent(b);
	replaceStatement(parent, stmt, b);
}

/*** PULLED FROM replaceExpressionWithStatement.C ***/
			
			class AndOpGenerator: public SageInterface::StatementGenerator
			   {
			     SgAndOp* op;
			
			     public:
			          AndOpGenerator(SgAndOp* op): op(op) {}
			
			          virtual SgStatement* generate(SgExpression* lhs)
			             {
			            // SgStatement* tree = 
			               SgIfStmt* tree =
			                    new SgIfStmt(SgDefaultFile, new SgExprStatement(SgDefaultFile,op->get_lhs_operand()),
			                         new SgBasicBlock(SgDefaultFile,new SgExprStatement(SgDefaultFile,new SgAssignOp(SgDefaultFile,lhs,op->get_rhs_operand()))),
			                         new SgBasicBlock(SgDefaultFile,new SgExprStatement(SgDefaultFile,new SgAssignOp(SgDefaultFile,lhs,new SgBoolValExp(SgDefaultFile, false)))));
			#if 1
			               printf ("Building IR node #1: tree = %p \n",tree);
			               printf ("Building IR node #4: new SgBasicBlock = %p \n",tree->get_true_body());
			               printf ("Building IR node #5: new SgBasicBlock = %p \n",tree->get_false_body());
			#endif
			               FixSgTree(tree);
			               return tree;
			             }
			   };
			
			class OrOpGenerator: public SageInterface::StatementGenerator
			   {
			     SgOrOp* op;
			
			     public:
			          OrOpGenerator(SgOrOp* op): op(op) {}
			
			          virtual SgStatement* generate(SgExpression* lhs)
			             {
			            // SgStatement* tree =
			               SgIfStmt* tree =
			                    new SgIfStmt(SgDefaultFile, new SgExprStatement(SgDefaultFile, op->get_lhs_operand()),
			                         new SgBasicBlock(SgDefaultFile,new SgExprStatement(SgDefaultFile,new SgAssignOp(SgDefaultFile,lhs,new SgBoolValExp(SgDefaultFile, true)))),
			                         new SgBasicBlock(SgDefaultFile,new SgExprStatement(SgDefaultFile,new SgAssignOp(SgDefaultFile,lhs,op->get_rhs_operand()))));
			#if 1
			               printf ("Building IR node #2: tree = %p \n",tree);
			               printf ("Building IR node #6: new SgBasicBlock = %p \n",tree->get_true_body());
			               printf ("Building IR node #7: new SgBasicBlock = %p \n",tree->get_false_body());
			#endif
			               FixSgTree(tree);
			               return tree;
			             }
			   };
			
			class ConditionalExpGenerator: public SageInterface::StatementGenerator
			   {
			     SgConditionalExp* op;
			
			     public:
			          ConditionalExpGenerator(SgConditionalExp* op): op(op) {}
			
			          virtual SgStatement* generate(SgExpression* lhs)
			             {
			            // SgStatement* tree =
			               SgIfStmt* tree = 
			                    new SgIfStmt(SgDefaultFile,new SgExprStatement(SgDefaultFile,op->get_conditional_exp()),
			                         new SgBasicBlock(SgDefaultFile,new SgExprStatement(SgDefaultFile,new SgAssignOp(SgDefaultFile,lhs,op->get_true_exp()))),
			                         new SgBasicBlock(SgDefaultFile,new SgExprStatement(SgDefaultFile,new SgAssignOp(SgDefaultFile,lhs,op->get_false_exp()))));
			#if 1
			               printf ("Building IR node #3: tree = %p \n",tree);
			               printf ("Building IR node #8: new SgBasicBlock = %p \n",tree->get_true_body());
			               printf ("Building IR node #9: new SgBasicBlock = %p \n",tree->get_false_body());
			#endif
			               ROSE_ASSERT(tree->get_parent() == NULL);
			               FixSgTree(tree);
			               ROSE_ASSERT(tree->get_parent() == NULL);
			               return tree;
			             }
			   };
/*** PULLED FROM replaceExpressionWithStatement.C ***/

//! Get a reference to the last element of an STL container
template <class Cont>
inline typename Cont::value_type&
lastElementOfContainer(Cont& c) {
  // Get reference to last element of an STL container;
  assert (!c.empty());
  typename Cont::iterator last = c.end();
  --last;
  return *last;
}

// creates a new variable declaration for a temporary variable
// newName: the name of the temporary variable. If newName=="", a random name is generated.
// varType: the type of the temporary variable, if byReference==true, the new variable's type is a reference type of varType
// 
// sets varname to the new variable's SgName
// sets initName to the new variable's SgInitializedName 
// sets newType to the new variable's type
// sets newType to the new variable's declaration
void createTmpVarInit(SgType* varType, string newName, bool byReference, 
                      SgName& varName, SgInitializedName *& initName, SgType*& newType, SgVariableDeclaration*& varDecl)
{
	// Create the new temporary variable's name and type
	// cout << "parent is a " << parent->sage_class_name() << endl;
	// cout << "parent is " << parent->unparseToString() << endl;
	// cout << "stmt is " << stmt->unparseToString() << endl;
	varName = "rose_temp__";
	if (newName == "") {
		varName << ++SageInterface::gensym_counter;
	} else {
		varName = newName;
	}
	
	if(byReference && !isSgReferenceType(varType))
		newType = new SgReferenceType(varType);
	else
		newType = varType;
	
	// Create the temporary variable's declaration
	varDecl = new SgVariableDeclaration(SgDefaultFile, varName, newType, 0);
	initName = lastElementOfContainer(varDecl->get_variables());
}

// creates and returns a statement contains a call to the given function with no arguments
SgStatement* createFuncCallStmt(SgFunctionDeclaration* funcDecl)
{
	SgExprListExp* argList = new SgExprListExp(SgDefaultFile);
	
	// create the function call expression
	SgFunctionCallExp* funcCall;
	{
		SgFunctionSymbol* funcSymb = new SgFunctionSymbol(funcDecl);
		SgFunctionRefExp* funcRefExp = new SgFunctionRefExp(SgDefaultFile, funcSymb);
		funcCall = new SgFunctionCallExp(SgDefaultFile, funcRefExp, argList);
		funcRefExp->set_parent(funcCall);
		argList->set_parent(funcCall);
	}
	
	// create the statement;
	{
		SgExprStatement* newStmt = new SgExprStatement(SgDefaultFile, funcCall);
		funcCall->set_parent(newStmt);
		return newStmt;
	}
}

// given a SgInitializedName, returns a SgVariableSymbol for the variable declared in the SgInitializedName
SgVariableSymbol* varSymFromInitName(SgInitializedName* initName)
{
	SgVariableSymbol* sym = new SgVariableSymbol(initName);
	ROSE_ASSERT(sym);
	
	return sym;
}

// given a SgInitializedName, returns a SgVarRefExp to the variable declared in the SgInitializedName
SgVarRefExp* varRefFromInitName(SgInitializedName* initName)
{
	SgVariableSymbol* sym = new SgVariableSymbol(initName);
	ROSE_ASSERT(sym);
	SgVarRefExp *refExp = new SgVarRefExp(SgDefaultFile, sym);
	ROSE_ASSERT(refExp);
	sym->set_parent(refExp);
	
	return refExp;
}

/*** MODIFIED FROM replaceExpressionWithStatement.C ***/
// Add a new temporary variable to contain the value of from
// Change reference to from to use this new variable
// if byReference==true, the new variable carries the reference to from, rather than its value 
//    (useful for expressions that identify an address to be modified, like the "a" in a=???)
// Assumptions: from is not within the test of a loop or if
//              not currently traversing from or the statement it is in
SgAssignInitializer* splitExpression_GB(SgExpression* from, SgVarRefExp* &replacement, string newName, bool byReference) {
	if (!SageInterface::isCopyConstructible(from->get_type())) {
		std::cerr << "Type " << from->get_type()->unparseToString() << " of expression " << from->unparseToString() << " is not copy constructible" << std::endl;
		ROSE_ASSERT (false);
	}
	assert (SageInterface::isCopyConstructible(from->get_type())); // How do we report errors?
	SgStatement* stmt = getStatementOfExpression(from);
	assert (stmt);
	
	// if the from's surrounding statement is the test condition of a for loop, let the for loop be 
	// the surrounding statement, since we'll be inserting the new variable declaration before the for loop
	if(isSgForStatement(stmt->get_parent()) && isSgForStatement(stmt->get_parent())->get_test()==stmt)
		stmt = isSgForStatement(stmt->get_parent());
	
	SgScopeStatement* parent = isSgScopeStatement(stmt->get_parent());
	// cout << "parent is a " << (parent ? parent->sage_class_name() : "NULL") << endl;
	if (!parent && isSgForInitStatement(stmt->get_parent()))
		parent = isSgScopeStatement(stmt->get_parent()->get_parent()->get_parent());
	assert (parent);
	
	// transform the from expression into a statement
	SgNode* fromparent = from->get_parent();
	list<SgExpression*> ancestors;
	for (SgExpression* anc = isSgExpression(fromparent); anc != 0;
	     anc = isSgExpression(anc->get_parent())) {
		if (isSgAndOp(anc) || isSgOrOp(anc) || isSgConditionalExp(anc))
			ancestors.push_front(anc); // Closest last
	}
	// cout << "This expression to split has " << ancestors.size() << " ancestor(s)" << endl;
	for (list<SgExpression*>::iterator ai = ancestors.begin(); ai != ancestors.end(); ++ai)
	{
		SageInterface::StatementGenerator* gen;
		switch ((*ai)->variantT()) {
			case V_SgAndOp: 
				gen = new AndOpGenerator(isSgAndOp(*ai)); break;
			case V_SgOrOp:
				gen = new OrOpGenerator(isSgOrOp(*ai)); break;
			case V_SgConditionalExp:
				gen = new ConditionalExpGenerator(isSgConditionalExp(*ai)); break;
			default: assert (!"Should not happen");
		}
		SageInterface::replaceExpressionWithStatement(*ai, gen);
		delete gen;
	}
	if (ancestors.size() != 0) {
		return SageInterface::splitExpression(from); 
		// Need to recompute everything if there were ancestors
	}
	
	// Create the new temporary variable and insert it before
	SgName varName;
	SgInitializedName* initName;
	SgType* varType;
	SgVariableDeclaration* varDecl;
	createTmpVarInit(from->get_type(), newName, byReference, varName, initName, varType, varDecl);

	// replace the original expression with a reference to the temporary variable
	replacement = varRefFromInitName(initName);
	// copy from's access type into its corresponding temporary variable reference
	cloneAccessType(replacement, from);
	replaceExpressionWithExpression(from, replacement);
	// std::cout << "Unparsed 3: " << fromparent->sage_class_name() << " --- " << fromparent->unparseToString() << endl;
	// cout << "From is a " << from->sage_class_name() << endl;
	// std::cout << " --- " << isSgExpressionRoot(fromparent)->get_operand_i()->unparseToString() << " --- " << isSgExpressionRoot(fromparent)->get_statement()->unparseToString() << std::endl;
	
	// Create the temporary variable's initialization
	SgAssignInitializer* ai = new SgAssignInitializer(SgDefaultFile, from, varType);
	from->set_parent(ai);
	initName->set_initializer(ai);
	ai->set_parent(initName);
	//  }
	initName->set_parent(varDecl);
//printf("calling myStatementInsert(%s)\n", stmt->unparseToString().c_str());
/*printf("                get_parent()=<%s | %s>\n", stmt->get_parent()->unparseToString().c_str(), stmt->get_parent()->class_name().c_str());
if(isSgForStatement(stmt->get_parent())){
	SgStatementPtrList& siblings_ptr = isSgScopeStatement(stmt->get_parent())->getStatementList();
	for(SgStatementPtrList::iterator it=siblings_ptr.begin(); it!=siblings_ptr.end(); it++)
	{
		printf("                sibling=<%s | %s>\n", (*it)->unparseToString().c_str(), (*it)->class_name().c_str());
	}
}*/
	myStatementInsert(stmt, varDecl, true);
	// vardecl->set_parent(stmt->get_parent());
	// FixSgTree(vardecl);
	// FixSgTree(parent);
	parent->insert_symbol(varName, (SgSymbol*)varSymFromInitName(initName));
	return ai;
}
/*** MODIFIED FROM replaceExpressionWithStatement.C ***/

/*** MODIFIED FROM replaceExpressionWithStatement.C ***/
// Replace the expression "from" with another expression "to", wherever it
// appears in the AST.  The expression "from" is not deleted, and may be
// reused elsewhere in the AST.
void replaceExpressionWithExpression_GB(SgExpression* from, SgExpression* to, SgNode* fromParent=NULL) {
	if(fromParent==NULL)
	fromParent = from->get_parent();
	
	// DQ (11/7/2006): I think i have fixed this up correctly.
	// SgExpressionRoot* toRoot = isSgExpressionRoot(to);
	SgExpression* toRoot = isSgExpression(to);
	
	to->set_parent(fromParent);
	// The parent of an expression should only be a statement if the expression
	// is a SgExpressionRoot
	if (isSgExprStatement(fromParent)) {
		ROSE_ASSERT (toRoot);
		isSgExprStatement(fromParent)->set_expression(toRoot);
	} else if (isSgReturnStmt(fromParent)) {
		ROSE_ASSERT (toRoot);
		isSgReturnStmt(fromParent)->set_expression(to);
	} else if (isSgDoWhileStmt(fromParent)) {
		ROSE_ASSERT (!"FIXME -- this case is present for when the test of a do-while statement is changed to an expression rather than a statement");
	} else if (isSgForStatement(fromParent)) {
		ROSE_ASSERT (isSgForStatement(fromParent)->get_increment()
		             == from); // If not, fromParent should be an SgExpressionRoot
		ROSE_ASSERT (toRoot);
		isSgForStatement(fromParent)->set_increment(toRoot);
	} else if (isSgExpression(fromParent)) {
		// std::cout << "Unparsed: " << fromParent->sage_class_name() << " --- " << from->unparseToString() << std::endl;
		// std::cout << "Unparsed 2: " << varref->sage_class_name() << " --- " << varref->unparseToString() << std::endl;
		int worked = isSgExpression(fromParent)->replace_expression(from, to);
		ROSE_ASSERT (worked);
	} else {
		ROSE_ASSERT (!"Parent of expression is an unhandled case");
	}
}
/*** MODIFIED FROM replaceExpressionWithStatement.C ***/

// replaces the given SgExpression with a SgAssignOp (lhsVar = orig)
// returns the SgAssignOp
SgAssignOp * replaceExprWithAssignOp(SgExpression* orig, SgVarRefExp* lhsVar)
{
	SgNode* origParent = orig->get_parent();
	SgAssignOp *asgn = new SgAssignOp(SgDefaultFile, lhsVar, orig, orig->get_type());
	// copy from's access type into its corresponding temporary variable reference
	cloneAccessType(lhsVar, orig);
	replaceExpressionWithExpression_GB(orig, asgn, origParent);
	return asgn;
}

// Creates a declaration of a new temporary variable of type varType and inserts it before anchor
// if before=true, the temporary variable in inserted before anchor and otherwise, after anchor
// sets initName to the SgInitializedName of the declaration
void insertVarDecl(SgStatement *anchor, SgType *varType, bool before, SgInitializedName*& initName)
{
	SgName varName;
	SgType* newType;
	SgVariableDeclaration* varDecl;
	createTmpVarInit(varType, "", false, varName, initName, newType, varDecl);
	
	myStatementInsert(anchor, varDecl, before);
}

// inserts the given expression before or after a given SgVariableDeclaration that appears inside a SgForStatement
void insertAroundForInit(SgVariableDeclaration* n, SgExpression *newNode, bool before)
{
	SgForInitStatement* init = isSgForInitStatement(n->get_parent());
	ROSE_ASSERT(init);
	SgStatementPtrList initStmts = init->get_init_stmt();
	SgForStatement* parentFor = isSgForStatement(init->get_parent());
	ROSE_ASSERT(parentFor);
	
	// create a new loop initializer that only contains the declarations that follow n
	SgForInitStatement* newInit = new SgForInitStatement(SgDefaultFile);
	
	/*SgStatementPtrList::iterator it2 = initStmts.begin();
		it2++;it2++;
	n = isSgVariableDeclaration(*it2);*/
	
	SgStatementPtrList::iterator it = initStmts.begin();
	for(; it!=initStmts.end() && (*it)!=n; it++) { }
	// if we want to insert after n, do not leave it in the for loop's initializers list
	if(!before)
		if(it!=initStmts.end()) it++;
		
	for(; it!=initStmts.end(); it++)
	{
		newInit->append_init_stmt(*it);
		(*it)->set_parent(newInit);
	}
	parentFor->set_for_init_stmt(newInit);
	newInit->set_parent(parentFor);

	// place n and the declarations that came before it before the for loop
	it = initStmts.begin();
	for(; it!=initStmts.end() && (*it)!=n; it++)
		myStatementInsert(parentFor, *it, true, true);
	// if we want to insert after n, put it before the for loop
	if(!before)
		myStatementInsert(parentFor, *it, true, true);
	
	// insert the new expression before the for loop
	myStatementInsert(parentFor, convertToStatement(newNode), true, true);
}


/************************
 *** cfgRWTransaction ***
 ************************/

string cfgRWTransaction::insertRequest::str()
{
	std::stringstream c;
	
	if(type==insBef) c << "ins_Bef ";
	else if(type==insAft) c << "ins_Aft ";
	c << "<" << getTgtNode()->unparseToString() << " | " << getTgtNode()->class_name() << ">";
	c << " --> " << newNode->unparseToString();
	string out;
	c >> out;
	return out;
}

string cfgRWTransaction::transformRequest::str()
{
	std::stringstream c;
	
	c << "callback ";
	c << "<" << target->unparseToString() << " | " << target->class_name() << ">";
	string out;
	c >> out;
	return out;
}

cfgRWTransaction::cfgRWTransaction() {}
	
void cfgRWTransaction::beginTransaction()
{
	requests.clear();
}
	
void cfgRWTransaction::insertBefore(DataflowNode n, SgExpression* newNode)
{
	//printf("insertBefore(%s)\n", newNode->unparseToString().c_str());
	requests.push_back(new insertRequest(insBef, n.getNode(), newNode));
}

void cfgRWTransaction::insertBefore(SgNode* n, SgExpression* newNode)
{
	insertRequest* req = new insertRequest(insBef, n, newNode);
	requests.push_back(req);
	//printf("insertBefore(%s) req=0x%x  requests.size()=%d\n", newNode->unparseToString().c_str(), req, requests.size());
}

void cfgRWTransaction::insertAfter(DataflowNode n, SgExpression* newNode)
{
	//printf("insertAfter(%s)\n", newNode->unparseToString().c_str());
	requests.push_back(new insertRequest(insAft, n.getNode(), newNode));
}

void cfgRWTransaction::insertAfter(SgNode* n, SgExpression* newNode)
{
	insertRequest* req = new insertRequest(insAft, n, newNode);
	//printf("insertAfter(%s)\n", newNode->unparseToString().c_str());
	requests.push_back(req);
	
	//printf("insertAfter(%s) req=0x%x  requests.size()=%d\n", newNode->unparseToString().c_str(), req, requests.size());
}

void cfgRWTransaction::transform(CFGTransform callbackFunc, SgNode* target, SgNode* newNode, void* data=NULL)
{
	//printf("transform(%s)\n", newNode->unparseToString().c_str());
	requests.push_back(new transformRequest(callbackFunc, target, newNode, data));
}

// insert an SgNode along the given CFGEdge
void cfgRWTransaction::insertAlong(DataflowEdge e, SgExpression* newNode) {
	//cerr << "insertAlong: " << e.source().toString() << " -> " << e.target().toString() << " at " << getFileInfoString(e.source()) << ", adding " << newNode->unparseToString() << endl;
	if (e.target().inEdges().size() == 1) {
		insertBefore(e.target(), newNode);
	} else if (e.source().outEdges().size() == 1) {
		insertAfter(e.source(), newNode);
	} else ROSE_ASSERT (!"Cannot handle source and target with multiple edges");
}

void cfgRWTransaction::commitTransaction()
{
	//printf("cfgRWTransaction::commitTransaction() requests.size()=%d\n", requests.size());
	while(requests.size()>0)
	{
		modRequest* req = requests.front();
		
		if(req->getType() == insBef)
		{
			//printf("   next request 0x%x: %d, %s\n", req, req->getType(), req->str().c_str());
			do_insertBefore(((insertRequest*)req)->getTgtNode(), ((insertRequest*)req)->newNode);
		}
		else if(req->getType() == insAft)
		{
			//printf("   next request 0x%x: %d, %s\n", req, req->getType(), req->str().c_str());
			do_insertAfter(((insertRequest*)req)->getTgtNode(), ((insertRequest*)req)->newNode);
		}
		else if(req->getType() == callback)
		{
			//printf("   next request 0x%x: %d, %s\n", req, req->getType(), req->str().c_str());
			(*((transformRequest*)req)->callbackFunc)(((transformRequest*)req)->target, 
			                                          ((transformRequest*)req)->newNode,
			                                          ((transformRequest*)req)->data);
		}
		else 
		{
			printf("req->getType()=%d=%s\n", req->getType(), req->str().c_str());
			ROSE_ASSERT(!"impossible case");
		}
		
		requests.pop_front();
		delete req;
	}
}

void cfgRWTransaction::do_insertBefore(DataflowNode n, SgExpression* newNode)
{
	do_insertBefore(n.getNode(), newNode);
}

void cfgRWTransaction::do_insertBefore(SgNode* n, SgExpression* newNode)
{
	ROSE_ASSERT(isSgExpression(n) || isSgInitializedName(n) || isSgStatement(n));
	printf("cfgRWTransaction::do_insertBefore(%p, <%s | %s>)\n", n, n->unparseToString().c_str(), n->class_name().c_str());
	printf("                                 (%p, <%s | %s>)\n", newNode, newNode->unparseToString().c_str(), newNode->class_name().c_str());

	SgNode* parent = n->get_parent();
	if(isSgExpression(n))
	{
		if (isSgAssignInitializer(n)) {
			ROSE_ASSERT(isSgInitializedName(parent));
			ROSE_ASSERT(isSgVariableDeclaration(parent->get_parent()));
			ROSE_ASSERT(isSgVariableDeclaration(parent->get_parent())->get_variables().size()==1);
			printf("insertBefore Node: (asgnInit) <%s | %s>\n", n->unparseToString().c_str(), n->class_name().c_str());
			
			// insert after SgVariableDeclaration			
			myStatementInsert(isSgVariableDeclaration(parent->get_parent()), convertToStatement(newNode), true, true);
		}
		else
		{
			//SgStatement* parentStmt=getStatementOfExpression(n);
			//printf("insertBefore Node: (default) <%s | %s>\n", n->unparseToString().c_str(), n->class_name().c_str());
			
			// insert the new SgNode immediately before this SgExpression
			
			{
				// use the comma operator
				insertBeforeExpression(isSgExpression(n), isSgExpression(newNode));
			}
/*			// else, if its a SgStatement, insert it immediately before n's surrounding statement
			else if(isSgStatement(newNode))
			{
				// split off all the children expressions that have side-effects
				vector<SgNode*> children = n->get_traversalSuccessorContainer();
				for(vector<SgNode*>::iterator it = children.begin(); it!=children.end(); it++) {
					printf("Node <%s|%s>, isNonMutatingSubTree=%d\n", (*it)->class_name().c_str(), (*it)->unparseToString().c_str(),isNonMutatingSubTree(*it));
					if(!isNonMutatingSubTree(*it))
					{
						SgVarRefExp* replacement;
						SgAssignInitializer* init = splitExpression_GB(isSgExpression(*it), replacement, "", true);
					}
				}
				
				myStatementInsert(parentStmt, isSgStatement(newNode), true, true);
			}*/
		}
	}
	else if (isSgInitializedName(n)) {
		ROSE_ASSERT(isSgVariableDeclaration(parent));
		ROSE_ASSERT(isSgVariableDeclaration(parent)->get_variables().size()==1);
		
		// insert after SgVariableDeclaration
		/*printf("insertBefore Node: (initName) <%s | %s>\n", n->unparseToString().c_str(), n->class_name().c_str());
		printf("                      parent: <%s | %s>\n", n->get_parent()->unparseToString().c_str(), n->get_parent()->class_name().c_str());
		printf("                 initializer: 0x%x\n", isSgInitializedName(n)->get_initializer());*/
		
		/*if(isSgInitializedName(n)->get_initializer())
		{
			printf("                 initializer: <%s | %s>\n", isSgInitializedName(n)->get_initializer()->unparseToString().c_str(), isSgInitializedName(n)->get_initializer()->class_name().c_str());
			do_insertBefore(isSgInitializedName(n)->get_initializer(), newNode);
		}
		else*/
			do_insertBefore(isSgInitializedName(n)->get_parent(), newNode);
		
		/*** THIS DOESN'T WORK RIGHT NOW BECAUSE EACH isSgInitializedName THAT HAS A isSgAssignInitializer
		 *** ACTUALLY GETS TWO DataflowNodes *** /
		myStatementInsert(isSgVariableDeclaration(parent), convertToStatement(newNode), true, true);*/
	}
	else if(isSgReturnStmt(n))
	{
		// split out the target expression using the comma operator
		SgVarRefExp* replacement;
		//SgAssignInitializer* init = splitExpression_GB(isSgReturnStmt(n)->get_expression(), replacement, "", getAccessType(n)!=readAccess);
		insertBeforeExpression(replacement, newNode);
	}
	else if(isSgStatement(n))
	{
		// if this is the initialization expression inside a for loop
		if(isSgVariableDeclaration(n) &&
		   n->get_parent()&& isSgForInitStatement(n->get_parent()))
		{
			insertAroundForInit(isSgVariableDeclaration(n), newNode, true);
		}
		// if this is a function definition
		else if(isSgFunctionDefinition(n))
		{
			printf("inserting before isSgFunctionDefinition\n");
			// The problem with SgFunctionDefinitions is that each function's CFG starts and ends with its SgFunctionDefinition
			// as such, if the application wants to place something before a SgFunctionDefinition, it just means
			// that they want to place something immediately before the end of the function.
			SgBasicBlock* funcBody = isSgFunctionDefinition(n)->get_body();
			funcBody->insertStatementInScope(convertToStatement(newNode), false);
		}
		else
		{
			printf("do_insertBefore calling myStatementInsert\n");
			myStatementInsert(isSgStatement(n), convertToStatement(newNode), true, true);
		}
	}
	else
		ROSE_ASSERT(!"Error: unhandled insert-before case!");
}

void cfgRWTransaction::do_insertAfter(DataflowNode n, SgExpression* newNode)
{
	do_insertAfter(n.getNode(), newNode);
}

void cfgRWTransaction::do_insertAfter(SgNode* n, SgExpression* newNode)
{
	//printf("cfgRWTransaction::do_insertAfter(<%s | %s>, <%s | %s>)\n", n->unparseToString().c_str(), n->class_name().c_str(), newNode->unparseToString().c_str(), newNode->class_name().c_str());
	ROSE_ASSERT(isSgExpression(n) || isSgInitializedName(n) || isSgStatement(n));
	
	
	SgNode* parent = n->get_parent();
	SgNode* grandparent = parent->get_parent();
	if(isSgExpression(n))
	{
		if (isSgAssignInitializer(n)) {
			ROSE_ASSERT(isSgInitializedName(parent));
			ROSE_ASSERT(isSgVariableDeclaration(parent->get_parent()));
			ROSE_ASSERT(isSgVariableDeclaration(parent->get_parent())->get_variables().size()==1);
			
			// insert after SgVariableDeclaration			
			myStatementInsert(isSgVariableDeclaration(parent->get_parent()), convertToStatement(newNode), false, true);
		}
		// else, if this is a regular expression
		else 
		{			
			// if this is the increment expression inside a for loop
			if(isSgForStatement(parent) && isSgForStatement(parent)->get_increment() == n)
			{
				//printf("insertAfter Node: (for-incr) <%s | %s>\n", n->unparseToString().c_str(), n->class_name().c_str());
				insertAfterExpression(isSgExpression(n), newNode);
			}
			// if this is the only expression inside a SgExprStatement
			else if(isSgExprStatement(parent))
			{
				/*printf("parent = <%s | %s>\n", parent->unparseToString().c_str(), parent->class_name().c_str());
				if(isSgForStatement(grandparent))
					printf("n=0x%x  get_increment()=0x%x  get_test()=0x%x\n", n, isSgForStatement(grandparent)->get_increment(), isSgForStatement(grandparent)->get_test());*/

				// if this is the test expression inside a for loop
				if(isSgForStatement(grandparent) && isSgForStatement(grandparent)->get_test() == parent)
				{
					//printf("insertAfter Node: (for-test) <%s | %s>\n", n->unparseToString().c_str(), n->class_name().c_str());
					SgInitializedName* initName;
					// declare a new temporary variable 
					insertVarDecl(isSgForStatement(grandparent), isSgExpression(n)->get_type(), true, initName);
					// replace n with (temp = n)
					SgAssignOp *asgn = replaceExprWithAssignOp(isSgExpression(n), varRefFromInitName(initName));
					// insert newNode immediately after (temp = n)
					SgExpression* newExpr = insertAfterExpression(asgn, newNode);
					
					// insert a reference to the temporary variable after the assignment and the newNode
					insertAfterExpression(newExpr, varRefFromInitName(initName));
				}
				else
				{
					//printf("insertAfter Node: (exprStmt) <%s | %s>\n", n->unparseToString().c_str(), n->class_name().c_str());

					//ROSE_ASSERT(isSgExprStatement(parent)->get_the_expr() == n);
					//insertAfterExpression(isSgExpression(n), newNode);
					myStatementInsert(isSgExprStatement(parent), new SgExprStatement(SgDefaultFile, newNode), false, true);
				}
			}
			// if this expression is the right-hand side of a comma expression
			else if(isSgCommaOpExp(parent) && isSgCommaOpExp(parent)->get_rhs_operand()==isSgExpression(n))
			{
				printf("insertAfter Node: (comma_rhs) <%s | %s>\n", n->unparseToString().c_str(), n->class_name().c_str()); fflush(stdout);
				
				// get the root of the nest of SgCommaOpExp where n appears as the last expression
				SgNode *grandparent = parent->get_parent();
				while(isSgCommaOpExp(grandparent) && isSgCommaOpExp(grandparent)->get_rhs_operand()==isSgExpression(parent))
				{
					parent = parent->get_parent();
					grandparent = parent->get_parent();
				}

				// insert after the SgCommaOpExp root
				do_insertAfter(parent, newNode);

				/*printf("                     grandparent = <%s | %s>\n", grandparent->unparseToString().c_str(), grandparent->class_name().c_str());
				// if the parent SgCommaOpExp is alone inside an SgExprStatement that is free
				//   (e.g. not inside a SgForStatement)
				if(isSgExprStatement(grandparent) && !isSgForStatement(grandparent->get_parent()))
				{
					myStatementInsert(isSgExprStatement(grandparent), new SgExprStatement(SgDefaultFile, newNode), false, true);
				}
				else
				{
					// split out the parent expression using the comma operator
					SgVarRefExp* replacement;
					SgAssignInitializer* init = splitExpression_GB(isSgCommaOpExp(parent), replacement, "", true);
					insertBeforeExpression(replacement, newNode);
				}*/
			}
			else
			{
				//SgStatement* parentStmt=getStatementOfExpression(n);
				//printf("insertAfter Node: (other) <%s | %s>\n", n->unparseToString().c_str(), n->class_name().c_str());
				
				// split out the target expression using the comma operator
				SgVarRefExp* replacement;
				//SgAssignInitializer* init = splitExpression_GB(isSgExpression(n), replacement, "", getAccessType(n)!=readAccess);
				insertBeforeExpression(replacement, newNode);
			}
		}
	}
	else if (isSgInitializedName(n)) {
		ROSE_ASSERT(isSgVariableDeclaration(parent));
		ROSE_ASSERT(isSgVariableDeclaration(parent)->get_variables().size()==1);
		
		// insert after SgVariableDeclaration
		/*** THIS DOESN'T WORK RIGHT NOW BECAUSE EACH isSgInitializedName THAT HAS A isSgAssignInitializer
		 *** ACTUALLY GETS TWO DataflowNodes *** /
		ROSE_ASSERT(false);*/
		myStatementInsert(isSgVariableDeclaration(parent), convertToStatement(newNode), false, true);
	}
	else if(isSgStatement(n))
	{
		/*printf("                        parent = <%s | %s>\n", n->get_parent()->unparseToString().c_str(), n->get_parent()->class_name().c_str());
		printf("                   grandparent = <%s | %s>\n", n->get_parent()->get_parent()->unparseToString().c_str(), n->get_parent()->get_parent()->class_name().c_str());*/
		
		// if this is the initialization expression inside a for loop
		if(isSgVariableDeclaration(n) &&
		   n->get_parent()&& isSgForInitStatement(n->get_parent()))
		{
			insertAroundForInit(isSgVariableDeclaration(n), newNode, false);
		}
		// if this is a function definition
		else if(isSgFunctionDefinition(n))
		{
			printf("inserting before isSgFunctionDefinition\n");
			// The problem with SgFunctionDefinitions is that each function's CFG starts and ends with its SgFunctionDefinition
			// as such, if the application wants to place something after a SgFunctionDefinition, it just means
			// that they want to place something immediately after the start of the function.
			SgBasicBlock* funcBody = isSgFunctionDefinition(n)->get_body();
			funcBody->insertStatementInScope(convertToStatement(newNode), true);
		}
		else
			myStatementInsert(isSgStatement(n), convertToStatement(newNode), false, true);
	}
	else
		ROSE_ASSERT(!"Error: unhandled insert-before case!");
}


// replace stmt; with {stmt; newNode;}
void replaceStatementByBlockAfter(SgStatement* stmt, SgStatement* newNode) {
	// Insert a block to wrap the two statements
	SgNode* parent = stmt->get_parent();
	SgBasicBlock* b = new SgBasicBlock(SgDefaultFile);
	b->get_statements().push_back(stmt);
	b->get_statements().push_back(newNode);
	newNode->set_parent(b);
	stmt->set_parent(b);
	replaceStatement(parent, stmt, b);
}




/*void insertAfter(CFGNode n, SgNode* newNode) {
	top:
	cerr << "insertAfter: " << n.toString() << " at " << getFileInfoString(n) << ", adding " << newNode->unparseToString() << endl;
	n = walkForwardToStatementBoundary(n);
	if (isSgBasicBlock(n.getNode())) { // Does not invalidate CFG node n
		SgStatement* s = convertToStatement(newNode);
		SgBasicBlock* bb = isSgBasicBlock(n.getNode());
		bb->append_statement(s);
	} else if (n.getIndex() == n.getNode()->cfgIndexForEnd()) {
		if (isSgForInitStatement(n.getNode())) {
			isSgForInitStatement(n.getNode())->append_init_stmt(convertToStatement(newNode));
		} else if (isSgStatement(n.getNode())) {
			SgStatement* s = convertToStatement(newNode);
			myStatementInsert(isSgStatement(n.getNode()), s, false, true);
		} else if (isSgExpression(n.getNode())) {
			ROSE_ASSERT (isSgExpression(newNode));
			SgNode* oldParent = n.getNode()->get_parent();
			if (isSgExpression(oldParent) && isSgExpression(oldParent->get_parent())) {
				CFGNode newN = theOutEdge(n);
				insertBefore(newN, newNode);
				return;
			} else ROSE_ASSERT (!"Bad case for inserting after expression");
		} else if (isSgInitializedName(n.getNode())) {
			SgInitializedName* in = isSgInitializedName(n.getNode());
			ROSE_ASSERT (!isSgForInitStatement(in->get_parent()));
			ROSE_ASSERT (isSgInitializedName(newNode));
			SgInitializedName* newVar = isSgInitializedName(newNode);
			SgVariableDeclaration* decl = isSgVariableDeclaration(in->get_declaration());
			ROSE_ASSERT (decl); // FIXME -- should handle parameter lists
			SgInitializedNamePtrList& vars = decl->get_variables();
			SgInitializedNamePtrList::iterator i = find(vars.begin(), vars.end(), in);
			ROSE_ASSERT (i != vars.end());
			++i;
			vars.insert(i, newVar);
			newVar->set_parent(decl);
		} else ROSE_ASSERT (!"Bad n.getNode() case");
	} else {
		cerr << "insertAfter: bad position " << n.toString() << " to insert " << newNode->unparseToString() << endl;
		ROSE_ASSERT (false);
	}
}*/

#if 0 // Liao 10/28/2010, not used
static void appendToBasicBlock(SgNode *target, SgNode *newNode, void* data)
{
	ROSE_ASSERT(target && newNode);
	ROSE_ASSERT(isSgBasicBlock(target));
	ROSE_ASSERT(isSgStatement(newNode));
	
	isSgBasicBlock(target)->append_statement(isSgStatement(newNode));
}
#endif 
/*************************************************************
 *** CALL-BACK FUNCTIONS FOR cfgRWTransaction::transform() ***
 *************************************************************/

// places newNode as thh first statement in the given SgScopeStatement 
void prependToScopeStmt(SgNode *target, SgNode *newNode, void* data)
{
	ROSE_ASSERT(target && newNode);
	ROSE_ASSERT(isSgScopeStatement (target));
	
	isSgScopeStatement(target)->insertStatementInScope(convertToStatement(newNode), true);
}

// places newNode as thh last statement in the given SgScopeStatement 
void appendToScopeStmt(SgNode *target, SgNode *newNode, void* data)
{
	ROSE_ASSERT(target && newNode);
	ROSE_ASSERT(isSgScopeStatement (target));
	
	isSgScopeStatement(target)->insertStatementInScope(convertToStatement(newNode), false);
}

}
