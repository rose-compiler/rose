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

  \authors Chunhua Liao (last modified 2/12/2008)
  
*/
namespace SageBuilder{
// shorter and easier name

#if 0
//---------------------AST creation/building/construction-----------------
  SgExpression* buildConst(SgType* t, const string & val);
  SgExpression* buildConstInt(int val);

  // build sth in languages, but not in SAGE AST
  // how to specify position info?  then

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

Scope stack is provided as an alternative to manually passing scope parameters to builder functions. It is not required to be used. Please use the recommendeded operation functions for maintaining the scope stack. Don't use raw container access functions to ScopeStack.  e.g. avoid ScopeStack.push_back(), using pushScopeStack() instead. 

 \todo consider complex cases:   
	- how many scope stacks to keep? one. examine only one transparently
	- regular: push a child scope of current scope, add symbols etc.
	- irregular: push an arbitrary scope temporarily,  add some symbol, then pop
	- even push a chain of scopes
	- restore scopes
*/

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
  \brief Builders for simple and complex SgType nodes, such as integer type, function type, array type, struct type, etc.

  \todo SgModifierType,SgNamedType(SgClassType,SgEnumType,SgTypedefType), SgQualifiedNameType, SgTemplateType,SgTypeComplex, SgTypeDefault,SgTypeEllipse,SgTypeGlobalVoid,SgTypeImaginary
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
SgTypeString* buildStringType();
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

//! Build a pointer type
SgPointerType* buildPointerType(SgType *base_type = NULL);

//! Build a reference type
SgReferenceType* buildReferenceType(SgType *base_type = NULL);

//! Build a const type.
SgType* buildConstType(SgType* base_type);

//! Build a volatile type.
SgType* buildVolatileType(SgType* base_type);

//! Build a restrict type.
SgType* buildRestrictType(SgType* base_type);

//! Build ArrayType
SgArrayType* buildArrayType(SgType* base_type=NULL, SgExpression* index=NULL);

//! Build function type from return type and parameter type list
SgFunctionType * 
buildFunctionType(SgType* return_type, SgFunctionParameterTypeList * typeList=NULL);

//! Build function type from return type and parameter list
SgFunctionType * 
buildFunctionType(SgType* return_type, SgFunctionParameterList * argList=NULL);

//@}

//--------------------------------------------------------------
//@{
/*! @name Builders for expressions
  \brief handle side effects of parent pointers, Sg_File_Info, lvalue etc.

Expressions are usually built using bottomup approach, i.e. buiding operands first, then the expression operating on the operands. It is also possible to build expressions with NULL operands or empty values first, then set them afterwards. 
  - Value string is not included in the argument list for simplicty. It can be set afterwards using set_valueString()
  - Expression builders are organized roughtly in the order of class hierarchy list of ROSE Web Reference
  - default values for arguments are provided to support top-down construction. Should use SageInterface::setOperand(),setLhsOperand(), setRhsOperand() etc to set operands and handle side effects.
  \todo SgActualArgumentExpression, SgAsmOp, SgAsteriskShapeExp, 
  SgValueExp, SgEnumVal,
  SgThrowOp,
*/
//! build a null expression
SgNullExpression* buildNullExpression();

//! build a bool value expression, the name convention of SgBoolValExp is little different from others for some unknown reason
SgBoolValExp* buildBoolValExp(int value = 0);
SgBoolValExp* buildBoolValExp(bool value = 0);

SgCharVal* buildCharVal(char value = 0);

SgComplexVal* buildComplexVal(long double real_value = 0.0, long double imaginary_value = 0.0 );

//! build a double value expression
SgDoubleVal* buildDoubleVal(double value = 0.0);

SgFloatVal* buildFloatVal(float value = 0.0);

//! build an integer value expression
SgIntVal* buildIntVal(int value = 0);

SgLongDoubleVal* buildLongDoubleVal(long double value = 0.0);

SgShortVal* buildShortVal(short value = 0);

SgStringVal* buildStringVal(std::string value="");

//! build a unsigned long integer
SgUnsignedLongVal* buildUnsignedLongVal(unsigned long v = 0);

//!  template function to build a unary expression of type T
template <class T> T* buildUnaryExpression(SgExpression* operand = NULL);

//! build &X, 
SgAddressOfOp* buildAddressOfOp (SgExpression* operand = NULL);

//! build ~X, 
SgBitComplementOp* buildBitComplementOp (SgExpression* operand = NULL);

//! build a type casting expression
SgCastExp * buildCastExp(SgExpression *  operand_i = NULL,
		SgType * expression_type = NULL,
		SgCastExp::cast_type_enum cast_type = SgCastExp::e_C_style_cast);
//! build -- expression, Sgop_mode is a value of either SgUnaryOp::prefix or SgUnaryOp::postfix
SgMinusMinusOp *buildMinusMinusOp(SgExpression* operand_i = NULL, SgUnaryOp::Sgop_mode  a_mode=SgUnaryOp::prefix);

//! build --x or x--. Remember to call SgUnaryOp::set_mode(SgUnaryOp::Sgop_mode  mode ) explicitly after calling this builder.
SgMinusMinusOp *buildMinusMinusOp(SgExpression* operand_i = NULL);

//! minus operation expression
SgMinusOp* buildMinusOp(SgExpression* operand = NULL);

//! not operation expression
SgNotOp* buildNotOp(SgExpression* operand = NULL);

//! build ++x or x++ , specify prefix or postfix using either SgUnaryOp::prefix or SgUnaryOp::postfix
SgPlusPlusOp* buildPlusPlusOp(SgExpression* operand_i = NULL, SgUnaryOp::Sgop_mode  a_mode = SgUnaryOp::prefix);

//! SgPointerDerefExp
SgPointerDerefExp* buildPointerDerefExp(SgExpression* operand_i =NULL);

SgUnaryAddOp* buildUnaryAddOp(SgExpression * operand_i = NULL);

//! template function to build a binary expression of type T, taking care of parent pointers, file info, lvalue, etc.
template <class T> T* buildBinaryExpression(SgExpression* lhs =NULL, SgExpression* rhs =NULL);

//! build an add expression +
SgAddOp * buildAddOp(SgExpression* lhs =NULL, SgExpression* rhs =NULL);

SgAndAssignOp* buildAndAssignOp(SgExpression* lhs =NULL, SgExpression* rhs =NULL);

SgAndOp* buildAndOp(SgExpression* lhs =NULL, SgExpression* rhs =NULL);

SgArrowExp* buildArrowExp(SgExpression* lhs =NULL, SgExpression* rhs =NULL);
SgArrowStarOp* buildArrowStarOp(SgExpression* lhs =NULL, SgExpression* rhs =NULL);
SgAssignOp* buildAssignOp(SgExpression* lhs =NULL, SgExpression* rhs =NULL);
SgBitAndOp* buildBitAndOp(SgExpression* lhs =NULL, SgExpression* rhs =NULL);
SgBitOrOp* buildBitOrOp(SgExpression* lhs =NULL, SgExpression* rhs =NULL);
SgBitXorOp* buildBitXorOp(SgExpression* lhs =NULL, SgExpression* rhs =NULL);
SgCommaOpExp* buildCommaOpExp(SgExpression* lhs =NULL, SgExpression* rhs =NULL);

SgConcatenationOp * buildConcatenationOp(SgExpression* lhs =NULL, SgExpression* rhs =NULL);
SgDivAssignOp * buildDivAssignOp(SgExpression* lhs =NULL, SgExpression* rhs =NULL);
SgDotExp * buildDotExp(SgExpression* lhs =NULL, SgExpression* rhs =NULL);
//! build == expression
SgEqualityOp * buildEqualityOp(SgExpression* lhs =NULL, SgExpression* rhs =NULL);
SgExponentiationOp * buildExponentiationOp(SgExpression* lhs =NULL, SgExpression* rhs =NULL);
SgGreaterOrEqualOp * buildGreaterOrEqualOp(SgExpression* lhs =NULL, SgExpression* rhs =NULL);
//! >= expression
SgGreaterThanOp* buildGreaterThanOp(SgExpression* lhs =NULL, SgExpression* rhs =NULL);
SgIntegerDivideOp* buildIntegerDivideOp(SgExpression* lhs =NULL, SgExpression* rhs =NULL);
SgIorAssignOp* buildIorAssignOp(SgExpression* lhs =NULL, SgExpression* rhs =NULL);
SgLessOrEqualOp* buildLessOrEqualOp(SgExpression* lhs =NULL, SgExpression* rhs =NULL);
//! build <= expression
SgLessThanOp* buildLessThanOp(SgExpression* lhs =NULL, SgExpression* rhs =NULL);
SgLshiftAssignOp * buildLshiftAssignOp(SgExpression* lhs =NULL, SgExpression* rhs =NULL);
SgLshiftOp* buildLshiftOp(SgExpression* lhs =NULL, SgExpression* rhs =NULL);
SgMinusAssignOp* buildMinusAssignOp(SgExpression* lhs =NULL, SgExpression* rhs =NULL);
SgModAssignOp* buildModAssignOp(SgExpression* lhs =NULL, SgExpression* rhs =NULL);
SgModOp* buildModOp(SgExpression* lhs =NULL, SgExpression* rhs =NULL);
SgMultAssignOp* buildMultAssignOp(SgExpression* lhs =NULL, SgExpression* rhs =NULL);
SgMultiplyOp* buildMultiplyOp(SgExpression* lhs =NULL, SgExpression* rhs =NULL);
SgNotEqualOp* buildNotEqualOp(SgExpression* lhs =NULL, SgExpression* rhs =NULL);
SgOrOp* buildOrOp(SgExpression* lhs, SgExpression* rhs =NULL);
//! build plus assignment expression: +=
SgPlusAssignOp* buildPlusAssignOp(SgExpression* lhs =NULL, SgExpression* rhs =NULL);
//!build pointer to array reference expression
SgPntrArrRefExp* buildPntrArrRefExp(SgExpression* lhs =NULL, SgExpression* rhs =NULL);
SgRshiftAssignOp* buildRshiftAssignOp(SgExpression* lhs =NULL, SgExpression* rhs =NULL);
SgRshiftOp* buildRshiftOp(SgExpression* lhs =NULL, SgExpression* rhs =NULL);
SgScopeOp* buildScopeOp(SgExpression* lhs =NULL, SgExpression* rhs =NULL);
SgSubtractOp* buildSubtractOp(SgExpression* lhs =NULL, SgExpression* rhs =NULL);
SgXorAssignOp * buildXorAssignOp(SgExpression* lhs =NULL, SgExpression* rhs =NULL);



//! build a conditional expression ?:
SgConditionalExp * buildConditionalExp(SgExpression* test =NULL, SgExpression* a =NULL, SgExpression* b =NULL);

//! build a SgExpreListExp, used for function call parameter list etc.
SgExprListExp * buildExprListExp();


//! build SgVarRefExp based on a variable's Sage name. It will lookup symbol table internally starting from scope. A variable name is unique so type can be inferred (double check this).

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

//! build SgFunctionRefExp based on a function's symbol.
SgFunctionRefExp *
buildFunctionRefExp(SgFunctionSymbol* sym);

//! build a function call expression
SgFunctionCallExp* 
buildFunctionCallExp(SgFunctionSymbol* sym, SgExprListExp* parameters);

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
//! Initialized names are tricky, their scope vary depending on context, so scope and symbol information are not needed until the initialized name is being actually used somewhere.

/*!e.g the scope of arguments of functions are different for defining and nondefining functions.
*/ 
SgInitializedName* 
buildInitializedName(const SgName & name, SgType* type);

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
  Please use SageInterface::appendStatement(), prependStatement(), and insertStatement() to attach the newly built statements into an AST tree. Calling member functions like SgScopeStatement::prepend_statement() or using container functions such as pushback() is discouraged since they don't handle many side effects for symbol tables, source file information, scope and parent pointers etc.

*/

//! build a variable declaration, handle symbol table transparently
SgVariableDeclaration* 
buildVariableDeclaration(const SgName & name, SgType *type, SgInitializer *varInit=NULL, SgScopeStatement* scope=NULL);

SgVariableDeclaration* 
buildVariableDeclaration(const std::string & name, SgType *type, SgInitializer *varInit=NULL, SgScopeStatement* scope=NULL);

SgVariableDeclaration* 
buildVariableDeclaration(const char* name, SgType *type, SgInitializer *varInit=NULL, SgScopeStatement* scope=NULL);

//! build an empty SgFunctionParameterList
SgFunctionParameterList * buildFunctionParameterList();

//! build an SgFunctionParameterList from SgFunctionParameterTypeList, like (int, float,...), used for parameter list of prototype functions when function type( including parameter type list) is known.
SgFunctionParameterList*
buildFunctionParameterList(SgFunctionParameterTypeList * paraTypeList);

//! a template function for function prototype declaration builders
template <class actualFunction>
actualFunction*
buildNondefiningFunctionDeclaration_T \
(const SgName & name, SgType* return_type, SgFunctionParameterList * paralist, SgScopeStatement* scope=NULL);

//! build a prototype for a function, handle function type, symbol etc transparently
SgFunctionDeclaration *
buildNondefiningFunctionDeclaration (const SgName & name, SgType* return_type, SgFunctionParameterList *parlist, SgScopeStatement* scope=NULL);

//! build a prototype member function declaration
SgMemberFunctionDeclaration *
buildNondefiningMemberFunctionDeclaration (const SgName & name, SgType* return_type, SgFunctionParameterList *parlist, SgScopeStatement* scope=NULL);

//! a template function for function declaration builders
template <class actualFunction>
SgFunctionDeclaration *
buildDefiningFunctionDeclaration_T (const SgName & name, SgType* return_type, SgFunctionParameterList * parlist, SgScopeStatement* scope=NULL);

//! build a function declaration with a function body
SgFunctionDeclaration *
buildDefiningFunctionDeclaration (const SgName & name, SgType* return_type, SgFunctionParameterList * parlist, SgScopeStatement* scope=NULL);

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

//! build a class definition scope statement
SgClassDefinition* buildClassDefinition(SgClassDeclaration *d = NULL);

//! build a structure, It is also a declaration statement in SAGE III
SgClassDeclaration * buildStructDeclaration(const SgName& name, SgScopeStatement* scope=NULL);
SgClassDeclaration * buildStructDeclaration(const std::string& name, SgScopeStatement* scope=NULL);
SgClassDeclaration * buildStructDeclaration(const char* name, SgScopeStatement* scope=NULL);

//@}

//--------------------------------------------------------------
//@{
/*! @name Builders for others
  \brief AST high level builders for others 

*/
//! Build a SgFile node and attach it to SgProject 
/*! The input file will be loaded if exists, or an empty one will be generated from scratch transparently. Output file name is used to specify the output file name of unparsing. The final SgFile will be inserted to project automatically. If not provided, a new SgProject will be generated internally. Using SgFile->get_project() to retrieve it in this case.
 */
SgFile* buildFile(const std::string& fileName,const std::string& outputFileName, SgProject* project=NULL);

//! Build and attach a comment, comment style is inferred from the language type of the target node if not provided. It is indeed a wrapper of SageInterface::attachComment().
PreprocessingInfo* buildComment(SgLocatedNode* target, const std::string & content,
               PreprocessingInfo::RelativePositionType position=PreprocessingInfo::before,
               PreprocessingInfo::DirectiveType dtype= PreprocessingInfo::CpreprocessorUnknownDeclaration);

//@}

} // end of namespace
#endif //ROSE_SAGE_BUILDER_INTERFACE
