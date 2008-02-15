#ifndef ROSE_SAGE_BUILDER_INTERFACE
#define ROSE_SAGE_BUILDER_INTERFACE

#include "rose.h"
#include <string>

/*!
  \defgroup frontendSageHighLevelInterface High level AST builders
  \ingroup ROSE_FrontEndGroup
  \brief This namespace contains high level SAGEIII AST node and subtree builders

  Building AST trees using raw SgNode constructors is tedious and error-prone. It becomes 
  even more difficult with the presence of symbol tables. This namespace contains major
  AST node builders on top of the constructors to take care of symbol tables, various
  edges to scope,  parent and so on.

  \authors Chunhua Liao (last modified 2/1/2008)
  
*/
namespace SageBuilder{
// shorter and easier name

#if 0
//---------------------AST creation/building/construction-----------------
  SgExpression* buildConst(SgType* t, const string & val);
  SgExpression* buildConstInt(int val);

  // build sth in languages, but not in SAGE AST
  // how to specify position info?  then
  PreprocessingInfo* buildComment(style, std::string& content);

  SgClassDeclaration* buildClassDeclaration(...,class_type);
  // or
  SgClassDeclaration* buildStruct(...);
  SgClassDeclaration* buildClass(...);
  SgClassDeclaration* buildUnion(...);

  // build B from dependent A
 SgTemplateInstantiationMemberFunctionDecl*  buildForwardFunctionDeclaration
     (SgTemplateInstantiationMemberFunctionDecl * memberFunctionInstantiation);

  //previous attempt: src/midend/astUtil/astInterface
  //  AstNodePtr AstInterface::CreateConst( const string& val, const string& valtype) const
  //  AstNodePtr AstInterfaceBase::CreateConstInt( int val)
#endif

//----------------------------------------------------------
//@{
/*! @name Scope stack interfaces
    \brief  a global data structure to store current scope and parent scopes. 

  Manually passing scope parameters to builder functions is not necessary if the scope stack is kept up to date.  Please use the recommendeded operation functions for maintaining the stack scope. Don't use raw container access functions to ScopeStack.  e.g. avoid ScopeStack.push_back(), using pushScopeStack() instead

 \todo consider complex cases:   
	- how many scope stacks to keep? one. examine only one transparently
	- regular: push a child scope of current scope, add symbols etc.
	- irregular: push an arbitrary scope temporarily,  add some symbol, then pop
	- even push a chain of scopes
	- restore scopes
*/

// DQ (1/18/2008): Added declaration in source file with Liao.
/*! \brief intended to be a private member, don't access it directly. could be changed any time
*/
extern std::list<SgScopeStatement*> ScopeStack;

//! public interfaces of the scope stack, should be stable
void pushScopeStack (SgScopeStatement* stmt);
void popScopeStack();
SgScopeStatement* topScopeStack() ;
bool emptyScopeStack();
void clearScopeStack();
   
//@} 


//--------------------------------------------------------------
//@{
/*! @name Builders for SgType
  \brief Builders for simple and complex SgType nodes, such as integer type, function type, array type, etc Base type should use SgTypeXX::createType() instead.
*/

//! Built in simple types
SgTypeBool *  buildBoolType();
SgTypeChar *  buildCharType();
SgTypeDouble* buildDoubleType();
SgTypeFloat*  buildFloatType();
SgTypeInt *   buildIntType();
SgTypeLong*    buildLongType();
SgTypeLongDouble* buildLongDoubleType();
SgTypeLongLong * buildLongLongType();
SgTypeShort*    buildShortType();
SgTypeVoid * buildVoidType();
SgTypeWchar* buildWcharType();

SgTypeSignedChar*  buildSignedCharType();
SgTypeSignedInt*   buildSignedIntType();
SgTypeSignedLong*  buildSignedLongType();
SgTypeSignedShort* buildSignedShortType();

SgTypeUnsignedChar* buildUnsignedCharType();
SgTypeUnsignedInt* buildUnsignedIntType();
SgTypeUnsignedLong*    buildUnsignedLongType();
SgTypeUnsignedLongLong*    buildUnsignedLongLongType();
SgTypeUnsignedShort*    buildUnsignedShortType();

//! build a pointer type
SgPointerType* buildPointerType(SgType *base_type = NULL);

//! build a reference type
SgReferenceType* buildReferenceType(SgType *base_type = NULL);

//! build ArrayType
SgArrayType* buildArrayType(SgType* base_type=NULL, SgExpression* index=NULL);

//! build function type from return type and parameter type list
SgFunctionType * 
buildFunctionType(SgType* return_type, SgFunctionParameterTypeList * typeList=NULL);

//! build function type from return type and parameter list
SgFunctionType * 
buildFunctionType(SgType* return_type, SgFunctionParameterList * argList=NULL);

//@}

//--------------------------------------------------------------
//@{
/*! @name Builders for expressions
  \brief handle side effects of parent pointers, Sg_File_Info, lvalue etc.
*/
//! build an integer value expression
SgIntVal* buildIntVal(int value);

//! build a bool value expression
SgBoolValExp* buildBoolValExp(bool value);

//! build a double value expression
SgDoubleVal* buildDoubleVal(double v);

//! build a unsigned long integer
SgUnsignedLongVal* buildUnsignedLongVal(unsigned long v);

//! build a null expression
SgNullExpression* buildNullExpression();

//! build a type casting expression
SgCastExp * buildCastExp(SgExpression *  operand_i = NULL,
		SgType * expression_type = NULL,
		SgCastExp::cast_type_enum cast_type = SgCastExp::e_C_style_cast);

//!  template function to build a unary expression of type T
template <class T> T* buildUnaryExpression(SgExpression* operand);

//! template function to build a binary expression of type T, taking care of parent pointers, lvalue, etc.
template <class T> T* buildBinaryExpression(SgExpression* lhs, SgExpression* rhs);

SgAndOp* buildAndOp(SgExpression* lhs, SgExpression* rhs);

SgOrOp* buildOrOp(SgExpression* lhs, SgExpression* rhs);

//! build a conditional expression ?:
SgConditionalExp * buildConditionalExp(SgExpression* test, SgExpression* a, SgExpression* b);

SgNotOp* buildNotOp(SgExpression* operand);

SgBitAndOp* buildBitAndOp(SgExpression* lhs, SgExpression* rhs);

SgBitOrOp* buildBitOrOp(SgExpression* lhs, SgExpression* rhs);

//! build plus assignment expression: +=
SgPlusAssignOp * buildPlusAssignOp(SgExpression* lhs, SgExpression* rhs);

//! build an add expression +
SgAddOp * buildAddOp(SgExpression* lhs, SgExpression* rhs);

//SgAndAssignOp buildAndAssignOp(SgExpression* lhs, SgExpression* rhs);

//! minus operation expression
SgMinusOp* buildMinusOp(SgExpression* operand);

//! not operation expression
SgNotOp* buildNotOp(SgExpression* operand);

//! >= expression
SgGreaterThanOp * buildGreaterThanOp(SgExpression* lhs, SgExpression* rhs);

//! build <= expression
SgLessThanOp * buildLessThanOp(SgExpression* lhs, SgExpression* rhs);

//! build == expression
SgEqualityOp * buildEqualityOp(SgExpression* lhs, SgExpression* rhs);

//! SgPointerDerefExp
SgPointerDerefExp* buildPointerDerefExp(SgExpression* operand);

//!build pointer to array reference expression
SgPntrArrRefExp* buildPntrArrRefExp(SgExpression* lhs, SgExpression* rhs);

//! build &X, 
SgAddressOfOp* buildAddressOfOp (SgExpression* operand);

//! build a SgExpreListExp, used for function call parameter list etc.
SgExprListExp * buildExprListExp();


//! build SgVarRefExp based on a variable's Sage name. It will lookup symbol table internally starting from scope. A variable name is unqiue so type can be inferred (double check this).

/*! 
It is possible to build a reference to a variable with known name before the variable is declration, especially during bottomup construction of AST. In this case, SgTypeUnknown is used to indicate the variable reference needing postprocessing fix using fixVariableReferences() once the AST is complete and all variable declarations exist. But the side effect is some get_type() operation may not recognize the unknown type before the fix. So far, I extended SgPointerDerefExp::get_type() and SgPntrArrRefExp::get_type() for SgTypeUnknown. There may be others needing the same extension. 
*/
SgVarRefExp *
buildVarRefExp(const SgName& name, SgScopeStatement* scope=NULL);

//! build SgVarRefExp based on a variable's name. It will lookup symbol table internally starting from scope. A variable is unqiue so type can be inferred.
SgVarRefExp *
buildVarRefExp(const std::string& varName, SgScopeStatement* scope=NULL);

//! Build a variable reference using a C style char array
SgVarRefExp *
buildVarRefExp(const char* varName, SgScopeStatement* scope=NULL);

//! build a variable reference from an existing symbol
SgVarRefExp *
buildVarRefExp(SgVariableSymbol* varSymbol);
 
//!build a variable reference from an initialized name

//! It first tries to grab the associated symbol, then call buildVarRefExp(const SgName& name, SgScopeStatement*) if symbol does not exist.
SgVarRefExp *
buildVarRefExp(SgInitializedName* initname, SgScopeStatement* scope=NULL);

//! build SgFunctionRefExp based on a C++ function's name and function type. It will lookup symbol table internally starting from scope
SgFunctionRefExp *
buildFunctionRefExp(const SgName& name, const SgType* func_type, SgScopeStatement* scope=NULL);

// not provided right now
// build SgFunctionRefExp based on a C function's name. It will lookup symbol table internally starting from scope
//SgFunctionRefExp *
// buildFunctionRefExp(const SgName& name,SgScopeStatement* scope=NULL);

//! build a function call expression,it will automatically search for function symbols internally to build a right function reference etc.
SgFunctionCallExp* 
buildFunctionCallExp(const SgName& name, SgType* return_type, \
                SgExprListExp* parameters, SgScopeStatement* scope=NULL);

//! build the rhs of a variable declaration wiich includes an assignment
SgAssignInitializer * buildAssignInitializer(SgExpression * operand_i = NULL);
//@}

//--------------------------------------------------------------
//@{
/*! @name Builders for support nodes
  \brief AST high level builders for SgSupport nodes

*/
//! Initialized names are tricky, their scope vary depending on context, so hanlded by users

//!e.g the scope of arguments of functions are different for defining and nondefining functions
//! 
SgInitializedName* 
buildInitializedName(const SgName & name, SgType* type, SgScopeStatement* scope=NULL);

//! build SgFunctionParameterTypeList from SgFunctionParameterList
SgFunctionParameterTypeList * 
buildFunctionParameterTypeList(SgFunctionParameterList * paralist);

//! build SgFunctionParameterTypeList from an expression list, useful when building a function call
SgFunctionParameterTypeList *
buildFunctionParameterTypeList(SgExprListExp * expList);

//--------------------------------------------------------------
//@{
/*! @name Builders for statements
  \brief AST high level builders for SgStatement, explicit scope parameters are allowed for flexibility.

*/

//! build a variable declaration, handle symbol table transparently
SgVariableDeclaration* 
buildVariableDeclaration(const SgName & name, SgType *type, SgInitializer *varInit=NULL, SgScopeStatement* scope=NULL);

//! build an empty SgFunctionParameterList
SgFunctionParameterList * buildFunctionParameterList();

//! build an SgFunctionParameterList from SgFunctionParameterTypeList, like (int, float,...), used for parameter list of prototype functions when function type( including parameter type list) is known.
SgFunctionParameterList*
buildFunctionParameterList(SgFunctionParameterTypeList * paraTypeList);

//! build a prototype for a function, handle function type, symbol etc transparently
SgFunctionDeclaration *
buildNonDefiningFunctionDeclaration (const SgName & name, SgType* return_type, SgFunctionParameterList *parlist, SgScopeStatement* scope=NULL);

//! build a function declaration with function body
SgFunctionDeclaration *
buildDefiningFunctionDeclaration (const SgName & name, SgType* return_type, SgFunctionParameterList * parlist, SgScopeStatement* scope=NULL);

//! a template function for function declaration builders
template <class actualFunction>
SgFunctionDeclaration *
buildDefiningFunctionDeclaration_T (const SgName & name, SgType* return_type, SgFunctionParameterList * parlist, SgScopeStatement* scope=NULL);

//! build a function call statement
SgExprStatement*
buildFunctionCallStmt(const SgName& name, SgType* return_type, \
                SgExprListExp* parameters, SgScopeStatement* scope=NULL);

//! build a label statement, handling label symbol and scope internally.

//! Note that the scope of a label statement is special. It is SgFunctionDefinition,
//! not the closest scope statement such as SgBasicBlock. 
SgLabelStatement * buildLabelStatement(const SgName& name, SgStatement * stmt = NULL, SgScopeStatement* scope=NULL);

//! build a goto statement
SgGotoStatement * buildGotoStatement(SgLabelStatement *  label=NULL,SgScopeStatement* scope=NULL);

//! build a case option statement
SgCaseOptionStmt * buildCaseOptionStmt( SgExpression * key = NULL,SgBasicBlock *body = NULL, SgScopeStatement* scope=NULL);

//! build a default option statement
SgDefaultOptionStmt * buildDefaultOptionStmt( SgBasicBlock *body = NULL, SgScopeStatement* scope=NULL);

//! build a SgExprStatement, set File_Info automatically 
SgExprStatement*
buildExprStatement(SgExpression*  exp = NULL);

//! build a switch statement
SgSwitchStatement* buildSwitchStatement(SgStatement *item_selector = NULL,SgBasicBlock *body = NULL);
inline SgSwitchStatement* buildSwitchStatement(SgExpression *item_selector, SgBasicBlock *body = NULL) {
  return buildSwitchStatement(buildExprStatement(item_selector), body);
}

//! build if statement
SgIfStmt * buildIfStmt(SgStatement* conditional, SgBasicBlock * true_body, SgBasicBlock * false_body);
inline SgIfStmt * buildIfStmt(SgExpression* conditional, SgBasicBlock * true_body, SgBasicBlock * false_body) {
  return buildIfStmt(buildExprStatement(conditional), true_body, false_body);
}

//! build while statement
SgWhileStmt * buildWhileStmt(SgStatement *  condition, SgBasicBlock *body);
inline SgWhileStmt * buildWhileStmt(SgExpression *  condition, SgBasicBlock *body) {
  return buildWhileStmt(buildExprStatement(condition), body);
}

//! build pragma declaration, handle SgPragma and defining/nondefining pointers internally
SgPragmaDeclaration * buildPragmaDeclaration(const std::string & name, SgScopeStatement* scope=NULL);

//! build a SgBasicBlock, setting file info internally
SgBasicBlock * buildBasicBlock(SgStatement * stmt = NULL);

//! build an assignment statement from lefthand operand and right hand operand 
SgExprStatement* 
buildAssignStatement(SgExpression* lhs,SgExpression* rhs);

//! build a break statement
SgBreakStmt* buildBreakStmt();

//! build a continue statement
SgContinueStmt* buildContinueStmt();

//@}

} // end of namespace
#endif //ROSE_SAGE_BUILDER_INTERFACE
