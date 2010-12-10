#ifndef ROSE_SAGE_BUILDER_INTERFACE
#define ROSE_SAGE_BUILDER_INTERFACE


#include <string>

/*!
  \defgroup frontendSageHighLevelInterface High level AST builders
  \ingroup ROSE_FrontEndGroup
  \brief This namespace contains high level SAGE III AST node and subtree builders

  Building AST trees using raw SgNode constructors is tedious and error-prone. It becomes 
  even more difficult with the presence of symbol tables. This namespace contains major
  AST node builders on top of the constructors to take care of symbol tables, various
  edges to scope,  parent and so on.

  \authors Chunhua Liao (last modified 2/12/2008)
  
*/
namespace SageBuilder{

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

// DQ (11/30/2010): Added support for building Fortran case insensitive symbol table handling.
//! Support for construction of case sensitive/insensitive symbol table handling in scopes.
extern bool symbol_table_case_insensitive_semantics;

//! Public interfaces of the scope stack, should be stable
void pushScopeStack (SgScopeStatement* stmt);
void pushScopeStack (SgNode* node);
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

// DQ (8/21/2010): We want to move to the new buildStringType( SgExpression*,size_t) function over the older buildStringType() function.
SgTypeString* buildStringType();
// SgTypeString* buildStringType( SgExpression* stringLengthExpression, size_t stringLengthLiteral );
SgTypeString* buildStringType( SgExpression* stringLengthExpression );

SgTypeVoid * buildVoidType();
SgTypeWchar* buildWcharType();

SgTypeSignedChar*  buildSignedCharType();
SgTypeSignedInt*   buildSignedIntType();
SgTypeSignedLong*  buildSignedLongType();
SgTypeSignedLongLong* buildSignedLongLongType();
SgTypeSignedShort* buildSignedShortType();

SgTypeUnsignedChar* buildUnsignedCharType();
SgTypeUnsignedInt* buildUnsignedIntType();
SgTypeUnsignedLong*    buildUnsignedLongType();
SgTypeUnsignedLongLong*    buildUnsignedLongLongType();
SgTypeUnsignedShort*    buildUnsignedShortType();
SgTypeUnknown * buildUnknownType();

//! Build a pointer type
SgPointerType* buildPointerType(SgType *base_type = NULL);

//! Build a reference type
SgReferenceType* buildReferenceType(SgType *base_type = NULL);

// Liao, entirely phase out this function ! Build a modifier type with no modifiers set
//SgModifierType* buildModifierType(SgType *base_type = NULL);

// DQ (7/29/2010): Changed return type from SgType to SgModifierType for a number of the functions below.
//! Build a const type.
SgModifierType* buildConstType(SgType* base_type = NULL);

//! Build a volatile type.
SgModifierType* buildVolatileType(SgType* base_type = NULL);

//! Build a restrict type.
SgModifierType* buildRestrictType(SgType* base_type);

//! Build ArrayType
SgArrayType* buildArrayType(SgType* base_type=NULL, SgExpression* index=NULL);

// DQ (8/27/2010): Added Fortran specific support for types based on kind expressions.
//! Build a type based on the Fortran kind mechanism
SgModifierType* buildFortranKindType(SgType* base_type, SgExpression* kindExpression );

//! Build function type from return type and parameter type list
SgFunctionType * 
buildFunctionType(SgType* return_type, SgFunctionParameterTypeList * typeList=NULL);

//! Build function type from return type and parameter list
SgFunctionType * 
buildFunctionType(SgType* return_type, SgFunctionParameterList * argList=NULL);

// DQ (1/16/2009): Added to support member function in C++ (for new interface)
SgMemberFunctionType * 
buildMemberFunctionType(SgType* return_type, SgFunctionParameterTypeList * typeList, SgClassDefinition *struct_name, unsigned int mfunc_specifier);

 //! Build an opaque type with a name, useful when a type's details are unknown during transformation, especially for a runtime library's internal type. Must provide scope here. 
 /*! Some types are not known during translation but nevertheless are needed. For example, some 
  * internal types from a runtime library.  To work around this problem: this function prepends a hidden typedef declaration into scope
  * 'typedef int OpaqueTypeName;' 
  * The translation-generated code is expected to include the runtime library's headers to 
  * have the real type declarations. 
  */
SgType* buildOpaqueType(std::string const type_name, SgScopeStatement * scope);

// DQ (7/29/2010): Changed return type from SgType to SgModifierType for a number of the functions below.
//! Build a UPC strict type
SgModifierType* buildUpcStrictType(SgType *base_type = NULL);

//! Build a UPC relaxed type
SgModifierType* buildUpcRelaxedType(SgType *base_type = NULL);

//! Build a UPC shared type
// SgModifierType* buildUpcSharedType(SgType *base_type = NULL);
SgModifierType* buildUpcSharedType(SgType *base_type = NULL, long layout = -1);

//! Build a UPC shared[] type
SgModifierType* buildUpcBlockIndefiniteType(SgType *base_type = NULL);

//! Build a UPC shared[*] type
SgModifierType* buildUpcBlockStarType(SgType *base_type = NULL);

//! Build a UPC shared[n] type
SgModifierType* buildUpcBlockNumberType(SgType *base_type, long block_factor);

//! Build a complex type
SgTypeComplex* buildComplexType(SgType *base_type = NULL);

//! Build an imaginary type
SgTypeImaginary* buildImaginaryType(SgType *base_type = NULL);

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

// JJW (11/19/2008): _nfi versions of functions set file info objects to NULL (used in frontend)

//! Build a null expression, set file info as the default one
SgNullExpression* buildNullExpression();
//! No file info version of buildNullExpression(). File info is to be set later on. 
SgNullExpression* buildNullExpression_nfi();

//! Build a bool value expression, the name convention of SgBoolValExp is little different from others for some unknown reason
SgBoolValExp* buildBoolValExp(int value = 0);
SgBoolValExp* buildBoolValExp(bool value = 0);
SgBoolValExp* buildBoolValExp_nfi(int value);

SgCharVal* buildCharVal(char value = 0);
SgCharVal* buildCharVal_nfi(char value, const std::string& str);

SgWcharVal* buildWcharVal(wchar_t value = 0);
SgWcharVal* buildWcharVal_nfi(wchar_t value, const std::string& str);

SgComplexVal* buildComplexVal(long double real_value = 0.0, long double imaginary_value = 0.0 );
SgComplexVal* buildComplexVal(SgValueExp* real_value, SgValueExp* imaginary_value);
SgComplexVal* buildComplexVal_nfi(SgValueExp* real_value, SgValueExp* imaginary_value, const std::string& str);
SgComplexVal* buildImaginaryVal(long double imaginary_value);
SgComplexVal* buildImaginaryVal(SgValueExp* imaginary_value);
SgComplexVal* buildImaginaryVal_nfi(SgValueExp* imaginary_value, const std::string& str);

//! Build a double value expression
SgDoubleVal* buildDoubleVal(double value = 0.0);
SgDoubleVal* buildDoubleVal_nfi(double value, const std::string& str);

SgFloatVal* buildFloatVal(float value = 0.0);
SgFloatVal* buildFloatVal_nfi(float value, const std::string& str);

//! Build an integer value expression
SgIntVal* buildIntVal(int value = 0);
SgIntVal* buildIntValHex(int value = 0);
SgIntVal* buildIntVal_nfi(int value, const std::string& str);

//! Build a long integer value expression
SgLongIntVal* buildLongIntVal(long value = 0);
SgLongIntVal* buildLongIntValHex(long value = 0);
SgLongIntVal* buildLongIntVal_nfi(long value, const std::string& str);

//! Build a long long integer value expression
SgLongLongIntVal* buildLongLongIntVal(long long value = 0);
SgLongLongIntVal* buildLongLongIntValHex(long long value = 0);
SgLongLongIntVal* buildLongLongIntVal_nfi(long long value, const std::string& str);

SgEnumVal* buildEnumVal_nfi(int value, SgEnumDeclaration* decl, SgName name);

SgLongDoubleVal* buildLongDoubleVal(long double value = 0.0);
SgLongDoubleVal* buildLongDoubleVal_nfi(long double value, const std::string& str);

SgShortVal* buildShortVal(short value = 0);
SgShortVal* buildShortValHex(short value = 0);
SgShortVal* buildShortVal_nfi(short value, const std::string& str);

SgStringVal* buildStringVal(std::string value="");
SgStringVal* buildStringVal_nfi(std::string value);

//! Build an unsigned char
SgUnsignedCharVal* buildUnsignedCharVal(unsigned char v = 0);
SgUnsignedCharVal* buildUnsignedCharValHex(unsigned char v = 0);
SgUnsignedCharVal* buildUnsignedCharVal_nfi(unsigned char v, const std::string& str);

//! Build an unsigned short integer
SgUnsignedShortVal* buildUnsignedShortVal(unsigned short v = 0);
SgUnsignedShortVal* buildUnsignedShortValHex(unsigned short v = 0);
SgUnsignedShortVal* buildUnsignedShortVal_nfi(unsigned short v, const std::string& str);

//! Build an unsigned integer
SgUnsignedIntVal* buildUnsignedIntVal(unsigned int v = 0);
SgUnsignedIntVal* buildUnsignedIntValHex(unsigned int v = 0);
SgUnsignedIntVal* buildUnsignedIntVal_nfi(unsigned int v, const std::string& str);

//! Build a unsigned long integer
SgUnsignedLongVal* buildUnsignedLongVal(unsigned long v = 0);
SgUnsignedLongVal* buildUnsignedLongValHex(unsigned long v = 0);
SgUnsignedLongVal* buildUnsignedLongVal_nfi(unsigned long v, const std::string& str);

//! Build an unsigned long long integer
SgUnsignedLongLongIntVal* buildUnsignedLongLongIntVal(unsigned long long v = 0);
SgUnsignedLongLongIntVal* buildUnsignedLongLongIntValHex(unsigned long long v = 0);
SgUnsignedLongLongIntVal* buildUnsignedLongLongIntVal_nfi(unsigned long long v, const std::string& str);

//! Build UPC THREADS (integer expression)
SgUpcThreads* buildUpcThreads();
SgUpcThreads* buildUpcThreads_nfi();

//! Build UPC  MYTHREAD (integer expression)
SgUpcMythread* buildUpcMythread();
SgUpcMythread* buildUpcMythread_nfi();

//! Build this pointer
SgThisExp* buildThisExp(SgClassSymbol* sym);
SgThisExp* buildThisExp_nfi(SgClassSymbol* sym);

//!  Template function to build a unary expression of type T. Instantiated functions include:buildAddressOfOp(),buildBitComplementOp(),buildBitComplementOp(),buildMinusOp(),buildNotOp(),buildPointerDerefExp(),buildUnaryAddOp(),buildMinusMinusOp(),buildPlusPlusOp().  They are also used for the unary vararg operators (which are not technically unary operators).
/*! The instantiated functions' prototypes are not shown since they are expanded using macros.
 * Doxygen is not smart enough to handle macro expansion. 
 */
template <class T> T* buildUnaryExpression(SgExpression* operand = NULL);
template <class T> T* buildUnaryExpression_nfi(SgExpression* operand);
//!  Template function to build a unary expression of type T with advanced information specified such as parenthesis and file info. Instantiated functions include:buildAddressOfOp(),buildBitComplementOp(),buildBitComplementOp(),buildMinusOp(),buildNotOp(),buildPointerDerefExp(),buildUnaryAddOp(),buildMinusMinusOp(),buildPlusPlusOp(). 
/*! The instantiated functions' prototypes are not shown since they are expanded using macros.
 * Doxygen is not smart enough to handle macro expansion. 
 */

#define BUILD_UNARY_PROTO(suffix) \
Sg##suffix * build##suffix(SgExpression* op =NULL); \
Sg##suffix * build##suffix##_nfi(SgExpression* op);

BUILD_UNARY_PROTO(AddressOfOp)
BUILD_UNARY_PROTO(BitComplementOp)
BUILD_UNARY_PROTO(MinusOp)
BUILD_UNARY_PROTO(NotOp)
BUILD_UNARY_PROTO(PointerDerefExp)
BUILD_UNARY_PROTO(UnaryAddOp)
BUILD_UNARY_PROTO(MinusMinusOp)
BUILD_UNARY_PROTO(PlusPlusOp)
BUILD_UNARY_PROTO(RealPartOp)
BUILD_UNARY_PROTO(ImagPartOp)
BUILD_UNARY_PROTO(ConjugateOp)
BUILD_UNARY_PROTO(VarArgStartOneOperandOp)
BUILD_UNARY_PROTO(VarArgEndOp)

//! Build a type casting expression
SgCastExp * buildCastExp(SgExpression *  operand_i = NULL,
		SgType * expression_type = NULL,
		SgCastExp::cast_type_enum cast_type = SgCastExp::e_C_style_cast);
SgCastExp * buildCastExp_nfi(SgExpression *  operand_i,
		SgType * expression_type,
		SgCastExp::cast_type_enum cast_type);

//! Build vararg op expression
SgVarArgOp * buildVarArgOp_nfi(SgExpression *  operand_i, SgType * expression_type);

//! Build -- expression, Sgop_mode is a value of either SgUnaryOp::prefix or SgUnaryOp::postfix
SgMinusMinusOp *buildMinusMinusOp(SgExpression* operand_i, SgUnaryOp::Sgop_mode  a_mode);
SgMinusMinusOp *buildMinusMinusOp_nfi(SgExpression* operand_i, SgUnaryOp::Sgop_mode  a_mode);

//! Build ++x or x++ , specify prefix or postfix using either SgUnaryOp::prefix or SgUnaryOp::postfix
SgPlusPlusOp* buildPlusPlusOp(SgExpression* operand_i, SgUnaryOp::Sgop_mode  a_mode);
SgPlusPlusOp* buildPlusPlusOp_nfi(SgExpression* operand_i, SgUnaryOp::Sgop_mode  a_mode);

SgNewExp * buildNewExp(SgType* type, 
                       SgExprListExp* exprListExp, 
		       SgConstructorInitializer* constInit, 
		       SgExpression* expr, 
		       short int val, 
		       SgFunctionDeclaration* funcDecl);
 

#undef BUILD_UNARY_PROTO

//! Template function to build a binary expression of type T, taking care of parent pointers, file info, lvalue, etc. Available instances include: buildAddOp(), buildAndAssignOp(), buildAndOp(), buildArrowExp(),buildArrowStarOp(), buildAssignOp(),buildBitAndOp(),buildBitOrOp(),buildBitXorOp(),buildCommaOpExp(), buildConcatenationOp(),buildDivAssignOp(),buildDivideOp(),buildDotExp(),buildEqualityOp(),buildExponentiationOp(),buildGreaterOrEqualOp(),buildGreaterThanOp(),buildIntegerDivideOp(),buildIorAssignOp(),buildLessOrEqualOp(),buildLessThanOp(),buildLshiftAssignOp(),buildLshiftOp(),buildMinusAssignOp(),buildModAssignOp(),buildModOp(),buildMultAssignOp(),buildMultiplyOp(),buildNotEqualOp(),buildOrOp(),buildPlusAssignOp(),buildPntrArrRefExp(),buildRshiftAssignOp(),buildRshiftOp(),buildScopeOp(),buildSubtractOp()buildXorAssignOp()
/*! The instantiated functions' prototypes are not shown since they are expanded using macros.
 * Doxygen is not smart enough to handle macro expansion. 
 */
template <class T> T* buildBinaryExpression(SgExpression* lhs =NULL, SgExpression* rhs =NULL);
template <class T> T* buildBinaryExpression_nfi(SgExpression* lhs, SgExpression* rhs);
//! Template function to build a binary expression of type T,with extra information for parenthesis and file info,  Instantiated functions include: buildAddOp(), buildAndAssignOp(), buildAndOp(), buildArrowExp(),buildArrowStarOp(), buildAssignOp(),buildBitAndOp(),buildBitOrOp(),buildBitXorOp(),buildCommaOpExp(), buildConcatenationOp(),buildDivAssignOp(), buildDivideOp(),buildDotExp(),buildEqualityOp(),buildExponentiationOp(),buildGreaterOrEqualOp(),buildGreaterThanOp(),buildIntegerDivideOp(),buildIorAssignOp(),buildLessOrEqualOp(),buildLessThanOp(),buildLshiftAssignOp(),buildLshiftOp(),buildMinusAssignOp(),buildModAssignOp(),buildModOp(),buildMultAssignOp(),buildMultiplyOp(),buildNotEqualOp(),buildOrOp(),buildPlusAssignOp(),buildPntrArrRefExp(),buildRshiftAssignOp(),buildRshiftOp(),buildScopeOp(),buildSubtractOp()buildXorAssignOp()
/*! The instantiated functions' prototypes are not shown since they are expanded using macros.
 * Doxygen is not smart enough to handle macro expansion. 
 */

#define BUILD_BINARY_PROTO(suffix) \
Sg##suffix * build##suffix(SgExpression* lhs =NULL, SgExpression* rhs =NULL); \
Sg##suffix * build##suffix##_nfi(SgExpression* lhs, SgExpression* rhs);

BUILD_BINARY_PROTO(AddOp)
BUILD_BINARY_PROTO(AndAssignOp)
BUILD_BINARY_PROTO(AndOp)
BUILD_BINARY_PROTO(ArrowExp)
BUILD_BINARY_PROTO(ArrowStarOp)
BUILD_BINARY_PROTO(AssignOp)
BUILD_BINARY_PROTO(BitAndOp)
BUILD_BINARY_PROTO(BitOrOp)
BUILD_BINARY_PROTO(BitXorOp)

BUILD_BINARY_PROTO(CommaOpExp)
BUILD_BINARY_PROTO(ConcatenationOp)
BUILD_BINARY_PROTO(DivAssignOp)
BUILD_BINARY_PROTO(DivideOp)
BUILD_BINARY_PROTO(DotExp)
BUILD_BINARY_PROTO(DotStarOp)
BUILD_BINARY_PROTO(EqualityOp)

BUILD_BINARY_PROTO(ExponentiationOp)
BUILD_BINARY_PROTO(GreaterOrEqualOp)
BUILD_BINARY_PROTO(GreaterThanOp)
BUILD_BINARY_PROTO(IntegerDivideOp)
BUILD_BINARY_PROTO(IorAssignOp)

BUILD_BINARY_PROTO(LessOrEqualOp)
BUILD_BINARY_PROTO(LessThanOp)
BUILD_BINARY_PROTO(LshiftAssignOp)
BUILD_BINARY_PROTO(LshiftOp)

BUILD_BINARY_PROTO(MinusAssignOp)
BUILD_BINARY_PROTO(ModAssignOp)
BUILD_BINARY_PROTO(ModOp)
BUILD_BINARY_PROTO(MultAssignOp)
BUILD_BINARY_PROTO(MultiplyOp)

BUILD_BINARY_PROTO(NotEqualOp)
BUILD_BINARY_PROTO(OrOp)
BUILD_BINARY_PROTO(PlusAssignOp)
BUILD_BINARY_PROTO(PntrArrRefExp)
BUILD_BINARY_PROTO(RshiftAssignOp)

BUILD_BINARY_PROTO(RshiftOp)
BUILD_BINARY_PROTO(ScopeOp)
BUILD_BINARY_PROTO(SubtractOp)
BUILD_BINARY_PROTO(XorAssignOp)

BUILD_BINARY_PROTO(VarArgCopyOp)
BUILD_BINARY_PROTO(VarArgStartOp)

#undef BUILD_BINARY_PROTO

//! Build a conditional expression ?:
SgConditionalExp * buildConditionalExp(SgExpression* test =NULL, SgExpression* a =NULL, SgExpression* b =NULL);
SgConditionalExp * buildConditionalExp_nfi(SgExpression* test, SgExpression* a, SgExpression* b, SgType* t);

//! Build a SgExprListExp, used for function call parameter list etc.
SgExprListExp * buildExprListExp(SgExpression * expr1 = NULL, SgExpression* expr2 = NULL, SgExpression* expr3 = NULL, SgExpression* expr4 = NULL, SgExpression* expr5 = NULL, SgExpression* expr6 = NULL, SgExpression* expr7 = NULL, SgExpression* expr8 = NULL, SgExpression* expr9 = NULL, SgExpression* expr10 = NULL);
SgExprListExp * buildExprListExp(const std::vector<SgExpression*>& exprs);
SgExprListExp * buildExprListExp_nfi();
SgExprListExp * buildExprListExp_nfi(const std::vector<SgExpression*>& exprs);


//! Build SgVarRefExp based on a variable's Sage name. It will lookup symbol table internally starting from scope. A variable name is unique so type can be inferred (double check this).

/*! 
It is possible to build a reference to a variable with known name before the variable is declaration, especially during bottomup construction of AST. In this case, SgTypeUnknown is used to indicate the variable reference needing postprocessing fix using fixVariableReferences() once the AST is complete and all variable declarations exist. But the side effect is some get_type() operation may not recognize the unknown type before the fix. So far, I extended SgPointerDerefExp::get_type() and SgPntrArrRefExp::get_type() for SgTypeUnknown. There may be others needing the same extension. 
*/
SgVarRefExp * buildVarRefExp(const SgName& name, SgScopeStatement* scope=NULL);

//! Build SgVarRefExp based on a variable's name. It will lookup symbol table internally starting from scope. A variable is unique so type can be inferred.
SgVarRefExp * buildVarRefExp(const std::string& varName, SgScopeStatement* scope=NULL);

//! Build a variable reference using a C style char array
SgVarRefExp * buildVarRefExp(const char* varName, SgScopeStatement* scope=NULL);

//! Build a variable reference from an existing symbol
SgVarRefExp * buildVarRefExp(SgVariableSymbol* varSymbol);
SgVarRefExp * buildVarRefExp_nfi(SgVariableSymbol* varSymbol);

//! Build a variable reference from an existing variable declaration. The assumption is a SgVariableDeclartion only declares one variable in the ROSE AST.
SgVarRefExp * buildVarRefExp(SgVariableDeclaration* vardecl);
 
//!build a variable reference from an initialized name
//! It first tries to grab the associated symbol, then call buildVarRefExp(const SgName& name, SgScopeStatement*) if symbol does not exist.
SgVarRefExp * buildVarRefExp(SgInitializedName* initname, SgScopeStatement* scope=NULL);

//!Build a variable reference expression at scope to an opaque variable which has unknown information except for its name.  Used when referring to an internal variable defined in some headers of runtime libraries.(The headers are not yet inserted into the file during translation). Similar to buildOpaqueType(); 
/*! It will declare a hidden int varName  at the specified scope to cheat the AST consistence tests.
 */
SgVarRefExp* buildOpaqueVarRefExp(const std::string& varName,SgScopeStatement* scope=NULL);

//! Build SgFunctionRefExp based on a C++ function's name and function type. It will lookup symbol table internally starting from scope. A hidden prototype will be created internally to introduce a new function symbol if the function symbol cannot be found. 
SgFunctionRefExp * buildFunctionRefExp(const SgName& name, const SgType* func_type, SgScopeStatement* scope=NULL);

SgFunctionRefExp * buildFunctionRefExp(const char* name, const SgType* func_type, SgScopeStatement* scope=NULL);

//! Build SgFunctionRefExp based on a C function's name. It will lookup symbol table internally starting from scope and return the first matching function.
SgFunctionRefExp * buildFunctionRefExp(const SgName& name,SgScopeStatement* scope=NULL);

SgFunctionRefExp * buildFunctionRefExp(const char* name,SgScopeStatement* scope=NULL);

//! Build SgFunctionRefExp based on a function's declaration.
SgFunctionRefExp * buildFunctionRefExp(const SgFunctionDeclaration* func_decl);

//! Build SgFunctionRefExp based on a function's symbol.
SgFunctionRefExp * buildFunctionRefExp(SgFunctionSymbol* sym);
SgFunctionRefExp * buildFunctionRefExp_nfi(SgFunctionSymbol* sym);
SgMemberFunctionRefExp * buildMemberFunctionRefExp_nfi(SgMemberFunctionSymbol* sym, bool virtual_call, bool need_qualifier);
SgMemberFunctionRefExp * buildMemberFunctionRefExp(SgMemberFunctionSymbol* sym, bool virtual_call, bool need_qualifier);
SgClassNameRefExp * buildClassNameRefExp_nfi(SgClassSymbol* sym);
SgClassNameRefExp * buildClassNameRefExp(SgClassSymbol* sym);

//! Build a function call expression
SgFunctionCallExp* buildFunctionCallExp(SgFunctionSymbol* sym, SgExprListExp* parameters=NULL);
SgFunctionCallExp* buildFunctionCallExp_nfi(SgExpression* f, SgExprListExp* parameters=NULL);
SgFunctionCallExp* buildFunctionCallExp(SgExpression* f, SgExprListExp* parameters=NULL);

//! Build a function call expression,it will automatically search for function symbols internally to build a right function reference etc. It tolerates the lack of the function symbol to support generating calls to library functions whose headers have not yet been inserted.
SgFunctionCallExp* 
buildFunctionCallExp(const SgName& name, SgType* return_type, \
                SgExprListExp* parameters=NULL, SgScopeStatement* scope=NULL);

//! Build the rhs of a variable declaration which includes an assignment
SgAssignInitializer * buildAssignInitializer(SgExpression * operand_i = NULL, SgType * expression_type = NULL);
SgAssignInitializer * buildAssignInitializer_nfi(SgExpression * operand_i = NULL, SgType * expression_type = NULL);

//! Build an aggregate initializer
SgAggregateInitializer * buildAggregateInitializer(SgExprListExp * initializers = NULL, SgType * type = NULL);
SgAggregateInitializer * buildAggregateInitializer_nfi(SgExprListExp * initializers, SgType * type = NULL);

// DQ (!/4/2009): Added support for building SgConstructorInitializer
SgConstructorInitializer * buildConstructorInitializer( SgMemberFunctionDeclaration *declaration,SgExprListExp *args,SgType *expression_type,bool need_name,bool need_qualifier,bool need_parenthesis_after_name,bool associated_class_unknown);
SgConstructorInitializer * buildConstructorInitializer_nfi( SgMemberFunctionDeclaration *declaration,SgExprListExp *args,SgType *expression_type,bool need_name,bool need_qualifier,bool need_parenthesis_after_name,bool associated_class_unknown);

//! Build sizeof() expression with an expression parameter
SgSizeOfOp* buildSizeOfOp(SgExpression* exp= NULL);
SgSizeOfOp* buildSizeOfOp_nfi(SgExpression* exp);

//! Build sizeof() expression with a type parameter
SgSizeOfOp* buildSizeOfOp(SgType* type = NULL);
SgSizeOfOp* buildSizeOfOp_nfi(SgType* type);



//@}

//--------------------------------------------------------------
//@{
/*! @name Builders for support nodes
  \brief AST high level builders for SgSupport nodes

*/
//! Initialized names are tricky, their scope vary depending on context, so scope and symbol information are not needed until the initialized name is being actually used somewhere.

/*!e.g the scope of arguments of functions are different for defining and nondefining functions.
*/ 
SgInitializedName* buildInitializedName(const SgName & name, SgType* type, SgInitializer* init = NULL);
SgInitializedName* buildInitializedName(const std::string &name, SgType* type);
SgInitializedName* buildInitializedName(const char* name, SgType* type);
SgInitializedName* buildInitializedName_nfi(const SgName & name, SgType* type, SgInitializer* init);

//! Build SgFunctionParameterTypeList from SgFunctionParameterList
SgFunctionParameterTypeList * 
buildFunctionParameterTypeList(SgFunctionParameterList * paralist);

//! Build SgFunctionParameterTypeList from an expression list, useful when building a function call
SgFunctionParameterTypeList *
buildFunctionParameterTypeList(SgExprListExp * expList);

//! Build an empty SgFunctionParameterTypeList 
SgFunctionParameterTypeList *
buildFunctionParameterTypeList();


//--------------------------------------------------------------
//@{
/*! @name Builders for statements
  \brief AST high level builders for SgStatement, explicit scope parameters are allowed for flexibility. 
  Please use SageInterface::appendStatement(), prependStatement(), and insertStatement() to attach the newly built statements into an AST tree. Calling member functions like SgScopeStatement::prepend_statement() or using container functions such as pushback() is discouraged since they don't handle many side effects for symbol tables, source file information, scope and parent pointers etc.

*/

//! Build a variable declaration, handle symbol table transparently
SgVariableDeclaration* 
buildVariableDeclaration(const SgName & name, SgType *type, SgInitializer *varInit=NULL, SgScopeStatement* scope=NULL);

SgVariableDeclaration* 
buildVariableDeclaration(const std::string & name, SgType *type, SgInitializer *varInit=NULL, SgScopeStatement* scope=NULL);

SgVariableDeclaration* 
buildVariableDeclaration(const char* name, SgType *type, SgInitializer *varInit=NULL, SgScopeStatement* scope=NULL);

SgVariableDeclaration* 
buildVariableDeclaration_nfi(const SgName & name, SgType *type, SgInitializer *varInit, SgScopeStatement* scope);

//!Build a typedef declaration, such as: typedef int myint; 
SgTypedefDeclaration* 
buildTypedefDeclaration(const std::string& name, SgType* base_type, SgScopeStatement* scope = NULL);

SgTypedefDeclaration* 
buildTypedefDeclaration_nfi(const std::string& name, SgType* base_type, SgScopeStatement* scope = NULL);

//! Build an empty SgFunctionParameterList, possibly with some initialized names filled in
SgFunctionParameterList * buildFunctionParameterList(SgInitializedName* in1 = NULL, SgInitializedName* in2 = NULL, SgInitializedName* in3 = NULL, SgInitializedName* in4 = NULL, SgInitializedName* in5 = NULL, SgInitializedName* in6 = NULL, SgInitializedName* in7 = NULL, SgInitializedName* in8 = NULL, SgInitializedName* in9 = NULL, SgInitializedName* in10 = NULL);
SgFunctionParameterList * buildFunctionParameterList_nfi();

//! Build an SgFunctionParameterList from SgFunctionParameterTypeList, like (int, float,...), used for parameter list of prototype functions when function type( including parameter type list) is known.
SgFunctionParameterList*
buildFunctionParameterList(SgFunctionParameterTypeList * paraTypeList);

SgFunctionParameterList*
buildFunctionParameterList_nfi(SgFunctionParameterTypeList * paraTypeList);

// DQ (1/21/2009): This is a support function called by the buildNondefiningFunctionDeclaration() and 
// buildNondefiningMemberFunctionDeclaration() functions.  Since we constructe the function type in 
// the support function and require either a SgFunctionType or SgMemberFunctionType, we need to to pass in 
// a flag to specify which function type to build (bool isMemberFunction).
//! A template function for function prototype declaration builders
template <class actualFunction>
actualFunction*
buildNondefiningFunctionDeclaration_T (const SgName & name, SgType* return_type, SgFunctionParameterList * paralist, bool isMemberFunction, SgScopeStatement* scope=NULL);

//! Build a prototype for a function, handle function type, symbol etc transparently
SgFunctionDeclaration *
buildNondefiningFunctionDeclaration (const SgName & name, SgType* return_type, SgFunctionParameterList *parlist, SgScopeStatement* scope=NULL);

//! Build a prototype for an existing function declaration (defining or nondefining is fine) 
SgFunctionDeclaration *
buildNondefiningFunctionDeclaration (const SgFunctionDeclaration* funcdecl, SgScopeStatement* scope=NULL);

//! Build a prototype member function declaration
SgMemberFunctionDeclaration *
buildNondefiningMemberFunctionDeclaration (const SgName & name, SgType* return_type, SgFunctionParameterList *parlist, SgScopeStatement* scope=NULL);

//! Build a defining ( non-prototype) member function declaration
SgMemberFunctionDeclaration *
buildDefiningMemberFunctionDeclaration (const SgName & name, SgType* return_type, SgFunctionParameterList *parlist, SgScopeStatement* scope=NULL);

//! Build a prototype for an existing member function declaration (defining or nondefining is fine) 
SgMemberFunctionDeclaration *
buildNondefiningMemberFunctionDeclaration (const SgMemberFunctionDeclaration* funcdecl, SgScopeStatement* scope=NULL);

//! A template function for function declaration builders
template <class actualFunction>
actualFunction *
buildDefiningFunctionDeclaration_T (const SgName & name, SgType* return_type, SgFunctionParameterList * parlist, SgScopeStatement* scope=NULL);

//! Build a function declaration with a function body
SgFunctionDeclaration *
buildDefiningFunctionDeclaration (const SgName & name, SgType* return_type, SgFunctionParameterList * parlist, SgScopeStatement* scope=NULL);

SgFunctionDeclaration *
buildDefiningFunctionDeclaration (const std::string & name, SgType* return_type, SgFunctionParameterList * parlist, SgScopeStatement* scope=NULL);

SgFunctionDeclaration *
buildDefiningFunctionDeclaration (const char* name, SgType* return_type, SgFunctionParameterList * parlist, SgScopeStatement* scope=NULL);

//! Build a Fortran subroutine or procedure
SgProcedureHeaderStatement* 
buildProcedureHeaderStatement(const char* name, SgType* return_type, SgFunctionParameterList * parlist, SgProcedureHeaderStatement::subprogram_kind_enum, SgScopeStatement* scope=NULL);

//! Build a regular function call statement
SgExprStatement*
buildFunctionCallStmt(const SgName& name, SgType* return_type, SgExprListExp* parameters=NULL, SgScopeStatement* scope=NULL);

//! Build a function call statement using function expression and argument list only, like (*funcPtr)(args);
SgExprStatement*
buildFunctionCallStmt(SgExpression* function, SgExprListExp* parameters=NULL);


//! Build a label statement, handling label symbol and scope internally.

//! Note that the scope of a label statement is special. It is SgFunctionDefinition,
//! not the closest scope statement such as SgBasicBlock. 
SgLabelStatement * buildLabelStatement(const SgName& name, SgStatement * stmt = NULL, SgScopeStatement* scope=NULL);
SgLabelStatement * buildLabelStatement_nfi(const SgName& name, SgStatement * stmt, SgScopeStatement* scope);

//! Build a goto statement
SgGotoStatement * buildGotoStatement(SgLabelStatement *  label=NULL);
SgGotoStatement * buildGotoStatement_nfi(SgLabelStatement *  label);

//! Build a case option statement
SgCaseOptionStmt * buildCaseOptionStmt( SgExpression * key = NULL,SgStatement *body = NULL);
SgCaseOptionStmt * buildCaseOptionStmt_nfi( SgExpression * key,SgStatement *body);

//! Build a default option statement
SgDefaultOptionStmt * buildDefaultOptionStmt( SgStatement *body = NULL);
SgDefaultOptionStmt * buildDefaultOptionStmt_nfi( SgStatement *body);

//! Build a SgExprStatement, set File_Info automatically 
SgExprStatement* buildExprStatement(SgExpression*  exp = NULL);
SgExprStatement* buildExprStatement_nfi(SgExpression*  exp);

//! Build a switch statement
SgSwitchStatement* buildSwitchStatement(SgStatement *item_selector = NULL,SgStatement *body = NULL);
inline SgSwitchStatement* buildSwitchStatement(SgExpression *item_selector, SgStatement *body = NULL) {
  return buildSwitchStatement(buildExprStatement(item_selector), body);
}
SgSwitchStatement* buildSwitchStatement_nfi(SgStatement *item_selector,SgStatement *body);

//! Build if statement
SgIfStmt * buildIfStmt(SgStatement* conditional, SgStatement * true_body, SgStatement * false_body);
inline SgIfStmt * buildIfStmt(SgExpression* conditional, SgStatement * true_body, SgStatement * false_body) {
  return buildIfStmt(buildExprStatement(conditional), true_body, false_body);
}
SgIfStmt * buildIfStmt_nfi(SgStatement* conditional, SgStatement * true_body, SgStatement * false_body);

//!Build a for statement, assume none of the arguments is NULL
SgForStatement * buildForStatement(SgStatement* initialize_stmt,  SgStatement * test, SgExpression * increment, SgStatement * loop_body);
SgForStatement * buildForStatement_nfi(SgStatement* initialize_stmt, SgStatement * test, SgExpression * increment, SgStatement * loop_body);

//! Build a UPC forall statement
SgUpcForAllStatement * buildUpcForAllStatement_nfi(SgStatement* initialize_stmt, SgStatement * test, SgExpression * increment, SgExpression* affinity, SgStatement * loop_body);

//! Build while statement
SgWhileStmt * buildWhileStmt(SgStatement *  condition, SgStatement *body);
inline SgWhileStmt * buildWhileStmt(SgExpression *  condition, SgStatement *body) {
  return buildWhileStmt(buildExprStatement(condition), body);
}
SgWhileStmt * buildWhileStmt_nfi(SgStatement *  condition, SgStatement *body);

//! Build do-while statement
SgDoWhileStmt * buildDoWhileStmt(SgStatement *  body, SgStatement *condition);
inline SgDoWhileStmt * buildDoWhileStmt(SgStatement* body, SgExpression *  condition) {
  return buildDoWhileStmt(body, buildExprStatement(condition));
}
SgDoWhileStmt * buildDoWhileStmt_nfi(SgStatement *  body, SgStatement *condition);

//! Build pragma declaration, handle SgPragma and defining/nondefining pointers internally
SgPragmaDeclaration * buildPragmaDeclaration(const std::string & name, SgScopeStatement* scope=NULL);
SgPragmaDeclaration * buildPragmaDeclaration_nfi(const std::string & name, SgScopeStatement* scope);

//!Build SgPragma
SgPragma* buildPragma(const std::string & name);

//! Build a SgBasicBlock, setting file info internally
SgBasicBlock * buildBasicBlock(SgStatement * stmt1 = NULL, SgStatement* stmt2 = NULL, SgStatement* stmt3 = NULL, SgStatement* stmt4 = NULL, SgStatement* stmt5 = NULL, SgStatement* stmt6 = NULL, SgStatement* stmt7 = NULL, SgStatement* stmt8 = NULL, SgStatement* stmt9 = NULL, SgStatement* stmt10 = NULL);
SgBasicBlock * buildBasicBlock_nfi();
SgBasicBlock * buildBasicBlock_nfi(const std::vector<SgStatement*>&);

//! Build an assignment statement from lefthand operand and right hand operand 
SgExprStatement* 
buildAssignStatement(SgExpression* lhs,SgExpression* rhs);

//! Build a break statement
SgBreakStmt* buildBreakStmt();
SgBreakStmt* buildBreakStmt_nfi();

//! Build a continue statement
SgContinueStmt* buildContinueStmt();
SgContinueStmt* buildContinueStmt_nfi();

//! Build a class definition scope statement
SgClassDefinition* buildClassDefinition(SgClassDeclaration *d = NULL);

//! Build a class definition scope statement
SgClassDefinition* buildClassDefinition_nfi(SgClassDeclaration *d = NULL);

//! Build a structure first nondefining declaration, without file info
SgClassDeclaration* buildNondefiningClassDeclaration_nfi(const SgName& name, SgClassDeclaration::class_types kind, SgScopeStatement* scope);

// DQ (11/7/2009): Added functions to build C++ class.
SgClassDeclaration* buildNondefiningClassDeclaration ( SgName name, SgScopeStatement* scope );
SgClassDeclaration* buildDefiningClassDeclaration    ( SgName name, SgScopeStatement* scope );

// DQ (11/7/2009): Added function to build C++ class (builts both the definig and non-defining declarations).
SgClassDeclaration* buildClassDeclaration    ( SgName name, SgScopeStatement* scope );

//! Build an enum first nondefining declaration, without file info
SgEnumDeclaration* buildNondefiningEnumDeclaration_nfi(const SgName& name, SgScopeStatement* scope);

//! Build a structure, It is also a declaration statement in SAGE III
SgClassDeclaration * buildStructDeclaration(const SgName& name, SgScopeStatement* scope=NULL);
SgClassDeclaration * buildStructDeclaration(const std::string& name, SgScopeStatement* scope=NULL);
SgClassDeclaration * buildStructDeclaration(const char* name, SgScopeStatement* scope=NULL);

// tps (09/02/2009) : Added support for building namespaces
SgNamespaceDeclarationStatement *  buildNamespaceDeclaration(const SgName& name, SgScopeStatement* scope=NULL);
SgNamespaceDeclarationStatement *  buildNamespaceDeclaration_nfi(const SgName& name, bool unnamednamespace, SgScopeStatement* scope );
SgNamespaceDefinitionStatement * buildNamespaceDefinition(SgNamespaceDeclarationStatement* d=NULL);


// DQ (1/24/2009): Added this "_nfi" function but refactored buildStructDeclaration to also use it (thsi needs to be done uniformally).
SgClassDeclaration * buildClassDeclaration_nfi(const SgName& name, SgClassDeclaration::class_types kind, SgScopeStatement* scope, SgClassDeclaration* nonDefiningDecl);

//! Build an enum, It is also a declaration statement in SAGE III
SgEnumDeclaration * buildEnumDeclaration(const SgName& name, SgScopeStatement* scope=NULL);

//! Build an enum, It is also a declaration statement in SAGE III
SgEnumDeclaration * buildEnumDeclaration_nfi(const SgName& name, SgScopeStatement* scope=NULL);

//! Build a return statement
SgReturnStmt* buildReturnStmt(SgExpression* expression = NULL);
SgReturnStmt* buildReturnStmt_nfi(SgExpression* expression);

//! Build a NULL statement
SgNullStatement* buildNullStatement();
SgNullStatement* buildNullStatement_nfi();

//! Build Fortran attribute specification statement
SgAttributeSpecificationStatement * buildAttributeSpecificationStatement(SgAttributeSpecificationStatement::attribute_spec_enum kind);

//! Build Fortran include line
SgFortranIncludeLine* buildFortranIncludeLine(std::string filename);

//! Build a Fortran common block, possibly with a name
SgCommonBlockObject* buildCommonBlockObject(std::string name="", SgExprListExp* exp_list=NULL);

//! Build a Fortran Common statement
SgCommonBlock* buildCommonBlock(SgCommonBlockObject* first_block=NULL);

// DQ (4/30/2010): Added support for building asm statements.
//! Build a NULL statement
SgAsmStmt* buildAsmStatement(std::string s);
SgAsmStmt* buildAsmStatement_nfi(std::string s);

// DQ (4/30/2010): Added support for building nop statement using asm statement
// ! Building nop statement using asm statement
SgAsmStmt* buildMultibyteNopStatement( int n );


//@}

//--------------------------------------------------------------
//@{
/*! @name Builders for others
  \brief AST high level builders for others 

*/
//! Build a SgFile node and attach it to SgProject 
/*! The input file will be loaded if exists, or an empty one will be generated from scratch transparently. Output file name is used to specify the output file name of unparsing. The final SgFile will be inserted to project automatically. If not provided, a new SgProject will be generated internally. Using SgFile->get_project() to retrieve it in this case.
 */
SgFile* buildFile(const std::string& inputFileName,const std::string& outputFileName, SgProject* project=NULL);

//! Build and attach a comment, comment style is inferred from the language type of the target node if not provided. It is indeed a wrapper of SageInterface::attachComment().
PreprocessingInfo* buildComment(SgLocatedNode* target, const std::string & content,
               PreprocessingInfo::RelativePositionType position=PreprocessingInfo::before,
               PreprocessingInfo::DirectiveType dtype= PreprocessingInfo::CpreprocessorUnknownDeclaration);

//! Build and attach #define XX directives, pass "#define xxx xxx" as content.
PreprocessingInfo* buildCpreprocessorDefineDeclaration(SgLocatedNode* target, 
                const std::string & content,
               PreprocessingInfo::RelativePositionType position=PreprocessingInfo::before);

//! Build an abstract handle from a SgNode
AbstractHandle::abstract_handle * buildAbstractHandle(SgNode* n);
//@}

} // end of namespace
#endif //ROSE_SAGE_BUILDER_INTERFACE
