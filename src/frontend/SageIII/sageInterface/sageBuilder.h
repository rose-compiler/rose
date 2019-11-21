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

#include "sageInterface.h"

#include "Diagnostics.h"

// forward declarations required for templated functions using those functions
namespace SageInterface {
  ROSE_DLL_API void setOneSourcePositionForTransformation (SgNode * root);
  ROSE_DLL_API void setSourcePosition(SgNode* node);
}

/** Functions that build an AST. */
namespace SageBuilder
{

// DQ (3/24/2016): Adding Robb's meageage mechanism (data member and function).
  extern Sawyer::Message::Facility mlog;
  void initDiagnostics();


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
ROSE_DLL_API void pushScopeStack (SgScopeStatement* stmt);

// DQ (3/20/2017): This function is not called (the function above is the more useful one that is used).
// ROSE_DLL_API void pushScopeStack (SgNode* node);

ROSE_DLL_API void popScopeStack();
ROSE_DLL_API SgScopeStatement* topScopeStack();
ROSE_DLL_API bool emptyScopeStack();
ROSE_DLL_API void clearScopeStack();

// DQ (3/11/2012): Added new function to the API for the internal scope stack.
//! Support to retrive the SgGlobal from the internal scope stack (error if not present in a non-empty list, return null for empty list).
SgScopeStatement* getGlobalScopeFromScopeStack();

// DQ (3/20/2017): This function is not used.
// bool isInScopeStack(SgScopeStatement * scope);

bool inSwitchScope();

// DQ (3/20/2017): This function is not used.
// TV: for debug purpose
// std::string stringFromScopeStack();


//@}

#if 0
// DQ (3/20/2017): These functions are not used (suggest using the API in the symbol table initialization).
//----------------------------------------------------------
//@{
/*! @name Case sensitivity interfaces
    \brief  Allows for setting case sensitivity for constructed scopes.

Nodes constructed for a Fortran file should be constructed with case insensitive.  All other should be case sensitive.  This interface supports multiple languages within the project.  The default is based on the type of file last parsed in the project.
*/

//! Set to insensitive case (Fortran files)
ROSE_DLL_API void setCaseInsensitive();
//! Set to sensitive case (all other languages)
ROSE_DLL_API void setCaseSensitive();
//! Set to that of an exsiting scope statement
ROSE_DLL_API void setCaseFromScope(SgScopeStatement* scope);

//@}
#endif


// *************************************************************************************************************
// DQ (5/1/2012): This is another possible interface: supporting how we set the source code position and mark is
// as either a transformation or as actual code to be assigned a source position as part of the AST construction.
// *************************************************************************************************************

enum SourcePositionClassification
   {
     e_sourcePositionError,                //! Error value for enum.
     e_sourcePositionDefault,              //! Default source position.
     e_sourcePositionTransformation,       //! Classify as a transformation.
     e_sourcePositionCompilerGenerated,    //! Classify as compiler generated code (e.g. template instantiation).
     e_sourcePositionNullPointers,         //! Set pointers to Sg_File_Info objects to NULL.
     e_sourcePositionFrontendConstruction, //! Specify as source position to be filled in as part of AST construction in the front-end.
     e_sourcePosition_last                 //! Last entry in enum.
   };

//! C++ SageBuilder namespace specific state for storage of the source code position state (used to control how the source code positon is defined for IR nodes built within the SageBuilder interface).
extern SourcePositionClassification SourcePositionClassificationMode;

//! Get the current source position classification (defines how IR nodes built by the SageBuilder interface will be classified).
ROSE_DLL_API SourcePositionClassification getSourcePositionClassificationMode();

//! display function for debugging
ROSE_DLL_API std::string display(SourcePositionClassification & scp);

//! Set the current source position classification (defines how IR nodes built by the SageBuilder interface will be classified).
ROSE_DLL_API void setSourcePositionClassificationMode(SourcePositionClassification X);

//! DQ (7/27/2012): changed semantics from removing the template arguments in names to adding the template arguments to names.
ROSE_DLL_API SgName appendTemplateArgumentsToName( const SgName & name, const SgTemplateArgumentPtrList & templateArgumentsList);

//! DQ (3/9/2018): Added to support debugging.
SgName unparseTemplateArgumentToString (SgTemplateArgument* templateArgument);

// *************************************************************************************************************



//--------------------------------------------------------------
//@{
/*! @name Builders for SgType
  \brief Builders for simple and complex SgType nodes, such as integer type, function type, array type, struct type, etc.

  \todo SgModifierType,SgNamedType(SgClassType,SgEnumType,SgTypedefType), SgQualifiedNameType, SgTemplateType,SgTypeComplex, SgTypeDefault,SgTypeEllipse,SgTypeGlobalVoid,SgTypeImaginary
*/

//! Built in simple types
ROSE_DLL_API SgTypeBool *  buildBoolType();
ROSE_DLL_API SgTypeNullptr* buildNullptrType();
ROSE_DLL_API SgTypeChar *  buildCharType();
ROSE_DLL_API SgTypeDouble* buildDoubleType();
ROSE_DLL_API SgTypeFloat*  buildFloatType();
ROSE_DLL_API SgTypeInt *   buildIntType();
ROSE_DLL_API SgTypeLong*    buildLongType();
ROSE_DLL_API SgTypeLongDouble* buildLongDoubleType();
ROSE_DLL_API SgTypeLongLong * buildLongLongType();
ROSE_DLL_API SgTypeShort*    buildShortType();
ROSE_DLL_API SgTypeFloat80*  buildFloat80Type();
ROSE_DLL_API SgTypeFloat128* buildFloat128Type();

//! DQ (8/21/2010): We want to move to the new buildStringType( SgExpression*,size_t) function over the older buildStringType() function.
ROSE_DLL_API SgTypeString* buildStringType();
// SgTypeString* buildStringType( SgExpression* stringLengthExpression, size_t stringLengthLiteral );
ROSE_DLL_API SgTypeString* buildStringType( SgExpression* stringLengthExpression );

ROSE_DLL_API SgTypeVoid * buildVoidType();
ROSE_DLL_API SgTypeWchar* buildWcharType();

// DQ (2/16/2018): Adding support for char16_t and char32_t (C99 and C++11 specific types).
ROSE_DLL_API SgTypeChar16* buildChar16Type();
ROSE_DLL_API SgTypeChar32* buildChar32Type();

ROSE_DLL_API SgTypeSignedChar*  buildSignedCharType();
ROSE_DLL_API SgTypeSignedInt*   buildSignedIntType();
ROSE_DLL_API SgTypeSignedLong*  buildSignedLongType();
ROSE_DLL_API SgTypeSignedLongLong* buildSignedLongLongType();
ROSE_DLL_API SgTypeSignedShort* buildSignedShortType();

#if 1
ROSE_DLL_API SgTypeSigned128bitInteger* buildSigned128bitIntegerType();
ROSE_DLL_API SgTypeUnsigned128bitInteger* buildUnsigned128bitIntegerType();
#endif

ROSE_DLL_API SgTypeUnsignedChar* buildUnsignedCharType();
ROSE_DLL_API SgTypeUnsignedInt* buildUnsignedIntType();
ROSE_DLL_API SgTypeUnsignedLong*    buildUnsignedLongType();
ROSE_DLL_API SgTypeUnsignedLongLong*    buildUnsignedLongLongType();
ROSE_DLL_API SgTypeUnsignedShort*    buildUnsignedShortType();
ROSE_DLL_API SgTypeUnknown * buildUnknownType();

//! Build a type based on Fortran's implicit typing rules.
//! Currently this interface does not take into account possible implicit
//! statements that change the rules.
ROSE_DLL_API SgType* buildFortranImplicitType(SgName name);

//! Build a pointer type
ROSE_DLL_API SgPointerType* buildPointerType(SgType *base_type = NULL);

//! Build a reference type
ROSE_DLL_API SgReferenceType* buildReferenceType(SgType *base_type = NULL);

//! Build a rvalue reference type
ROSE_DLL_API SgRvalueReferenceType* buildRvalueReferenceType(SgType *base_type);

//! Build a decltype reference type
ROSE_DLL_API SgDeclType* buildDeclType(SgExpression *base_expression, SgType* base_type);

//! Build a GNU typeof operator
ROSE_DLL_API SgTypeOfType* buildTypeOfType(SgExpression *base_expression, SgType* base_type);

// Liao, entirely phase out this function ! Build a modifier type with no modifiers set
//SgModifierType* buildModifierType(SgType *base_type = NULL);

// DQ (7/29/2010): Changed return type from SgType to SgModifierType for a number of the functions below.
//! Build a modifier type.
ROSE_DLL_API SgModifierType* buildModifierType(SgType* base_type = NULL);

//! Build a const type.
ROSE_DLL_API SgModifierType* buildConstType(SgType* base_type = NULL);

//! Build a volatile type.
ROSE_DLL_API SgModifierType* buildVolatileType(SgType* base_type = NULL);

//! Build a const volatile type.
ROSE_DLL_API SgModifierType* buildConstVolatileType(SgType* base_type = NULL);

//! Build a restrict type.
ROSE_DLL_API SgModifierType* buildRestrictType(SgType* base_type);

//! Build ArrayType
ROSE_DLL_API SgArrayType* buildArrayType(SgType* base_type=NULL, SgExpression* index=NULL);

// RASMUSSEN (1/25/2018)
//! Build an ArrayType based on dimension information.
//! Note, the index member variable will be set to a NullExpression.
//!
//! \param base_type The base type of the array.
//!        Note that if the base type is itself an array type, the shape of the array may be changed.
//! \param dim_info A list of expressions describing the shape of the array.
//!        The rank of the array is set from the length of this list.
ROSE_DLL_API SgArrayType* buildArrayType(SgType* base_type, SgExprListExp* dim_info);

// DQ (8/27/2010): Added Fortran specific support for types based on kind expressions.
//! Build a type based on the Fortran kind mechanism
ROSE_DLL_API SgModifierType* buildFortranKindType(SgType* base_type, SgExpression* kindExpression );

//! Build function type from return type and parameter type list
ROSE_DLL_API SgFunctionType* buildFunctionType(SgType* return_type, SgFunctionParameterTypeList * typeList=NULL);

//! Build function type from return type and parameter list
ROSE_DLL_API SgFunctionType* buildFunctionType(SgType* return_type, SgFunctionParameterList * argList=NULL);

//! DQ (1/16/2009): Added to support member function in C++ (for new interface)
ROSE_DLL_API SgMemberFunctionType* buildMemberFunctionType(SgType* return_type, SgFunctionParameterTypeList * typeList, SgScopeStatement *struct_name, unsigned int mfunc_specifier, unsigned int ref_qualifiers = 0);

// DQ (3/20/2017): This function is not used (so let's see if we can remove it).
//! DQ (12/2/2011): Added for symetry with other functios to generate SgFunctionType
// ROSE_DLL_API SgMemberFunctionType* buildMemberFunctionType(SgType* return_type, SgFunctionParameterList* argList = NULL, SgClassDefinition *struct_name = NULL, unsigned int mfunc_specifier = 0);

//! DQ (8/19/2012): Refactored some of the code supporting construction of the SgMemberFunctionType.
ROSE_DLL_API SgMemberFunctionType* buildMemberFunctionType(SgType* return_type, SgFunctionParameterTypeList* typeList, SgType *classType, unsigned int mfunc_specifier, unsigned int ref_qualifiers = 0);

// PP (07/14/2016):
//! Some support for building class template instantiation declarations.
//! Note, the template is not actually instantiated, but a `forward declaration'
//! node is created.
//!
//! \param template_decl the template class declaration
//!        (e.g., template <class T> struct matrix {};)
//! \param template_args the arguments of the template instantiation.
//!        (e.g., [SgTypeFloat]).
//!        WARNING: the objects in this list will be linked into the template declaration
//!                 and their parent pointer may change. Thus it is the caller's
//!                 responsibility to clone nodes if used elsewhere.
//!                 e.g., SomeClass<0> <- the expression representing 0 may be modified.
//! \result a class type for the instantiated template (e.g., matrix<float>)
ROSE_DLL_API
SgClassType*
buildClassTemplateType(SgTemplateClassDeclaration* template_decl, Rose_STL_Container<SgNode *>& template_args);



 //! Build an opaque type with a name, useful when a type's details are unknown during transformation, especially for a runtime library's internal type. Must provide scope here.
 /*! Some types are not known during translation but nevertheless are needed. For example, some
  * internal types from a runtime library.  To work around this problem: this function prepends a hidden typedef declaration into scope
  * 'typedef int OpaqueTypeName;'
  * The translation-generated code is expected to include the runtime library's headers to
  * have the real type declarations.
  */
ROSE_DLL_API SgType* buildOpaqueType(std::string const type_name, SgScopeStatement * scope);

// DQ (7/29/2010): Changed return type from SgType to SgModifierType for a number of the functions below.
//! Build a UPC strict type
ROSE_DLL_API SgModifierType* buildUpcStrictType(SgType *base_type = NULL);

//! Build a UPC relaxed type
ROSE_DLL_API SgModifierType* buildUpcRelaxedType(SgType *base_type = NULL);

//! Build a UPC shared type
ROSE_DLL_API SgModifierType* buildUpcSharedType(SgType *base_type = NULL, long layout = -1);
// SgModifierType* buildUpcSharedType(SgType *base_type = NULL);

//! Build a UPC shared[] type
ROSE_DLL_API SgModifierType* buildUpcBlockIndefiniteType(SgType *base_type = NULL);

//! Build a UPC shared[*] type
ROSE_DLL_API SgModifierType* buildUpcBlockStarType(SgType *base_type = NULL);

//! Build a UPC shared[n] type
ROSE_DLL_API SgModifierType* buildUpcBlockNumberType(SgType *base_type, long block_factor);

//! Build a complex type
ROSE_DLL_API SgTypeComplex* buildComplexType(SgType *base_type = NULL);

//! Build an imaginary type
ROSE_DLL_API SgTypeImaginary* buildImaginaryType(SgType *base_type = NULL);

//! Build a const/volatile type qualifier
ROSE_DLL_API SgConstVolatileModifier * buildConstVolatileModifier (SgConstVolatileModifier::cv_modifier_enum mtype=SgConstVolatileModifier::e_unknown);

//! Build a Matlab Matrix Type
ROSE_DLL_API SgTypeMatrix* buildMatrixType();

//! Build a tuple of types. Useful for a function returning multiple variables of different types
ROSE_DLL_API SgTypeTuple* buildTupleType(SgType *t1 = NULL, SgType *t2 = NULL, SgType *t3 = NULL, SgType *t4 = NULL, SgType *t5 = NULL, SgType *t6 = NULL, SgType *t7 = NULL, SgType *t8 = NULL, SgType *t9 = NULL, SgType *t10 = NULL);

//@}

//--------------------------------------------------------------
//@{
/*! @name Builders for expressions
  \brief handle side effects of parent pointers, Sg_File_Info, lvalue etc.

Expressions are usually built using bottomup approach, i.e. buiding operands first, then the expression operating on the operands. It is also possible to build expressions with NULL operands or empty values first, then set them afterwards.
  - Value string is not included in the argument list for simplicty. It can be set afterwards using set_valueString()
  - Expression builders are organized roughtly in the order of class hierarchy list of ROSE Web Reference
  - default values for arguments are provided to support top-down construction. Should use SageInterface::setOperand(),setLhsOperand(), setRhsOperand() etc to set operands and handle side effects.
  \todo SgAsmOp, SgAsteriskShapeExp,
  SgValueExp, SgEnumVal,
  SgThrowOp,
*/

// JJW (11/19/2008): _nfi versions of functions set file info objects to NULL (used in frontend)

ROSE_DLL_API SgVariantExpression * buildVariantExpression();

//! Build a null expression, set file info as the default one
ROSE_DLL_API SgNullExpression* buildNullExpression();
//! No file info version of buildNullExpression(). File info is to be set later on.
ROSE_DLL_API SgNullExpression* buildNullExpression_nfi();

//! Build a bool value expression, the name convention of SgBoolValExp is little different from others for some unknown reason
ROSE_DLL_API SgBoolValExp* buildBoolValExp(int value = 0);
ROSE_DLL_API SgBoolValExp* buildBoolValExp(bool value = 0);
ROSE_DLL_API SgBoolValExp* buildBoolValExp_nfi(int value);

ROSE_DLL_API SgCharVal* buildCharVal(char value = 0);
ROSE_DLL_API SgCharVal* buildCharVal_nfi(char value, const std::string& str);

//! DQ (7/31/2014): Adding support for C++11 nullptr const value expressions.
ROSE_DLL_API SgNullptrValExp* buildNullptrValExp();
ROSE_DLL_API SgNullptrValExp* buildNullptrValExp_nfi();

//! DQ (2/14/2019): Adding support for C++14 void value expressions.
ROSE_DLL_API SgVoidVal* buildVoidVal();
ROSE_DLL_API SgVoidVal* buildVoidVal_nfi();

ROSE_DLL_API SgWcharVal* buildWcharVal(wchar_t value = 0);
ROSE_DLL_API SgWcharVal* buildWcharVal_nfi(wchar_t value, const std::string& str);

// DQ (2/16/2018): Adding support for char16_t and char32_t (C99 and C++11 specific types).
ROSE_DLL_API SgChar16Val* buildChar16Val(unsigned short value = 0);
ROSE_DLL_API SgChar16Val* buildChar16Val_nfi(unsigned short value, const std::string& str);
ROSE_DLL_API SgChar32Val* buildChar32Val(unsigned int value = 0);
ROSE_DLL_API SgChar32Val* buildChar32Val_nfi(unsigned int value, const std::string& str);

// DQ (3/20/2017): This function has never existed (inputs must be SgValueExp pointers).
// ROSE_DLL_API SgComplexVal* buildComplexVal(long double real_value = 0.0, long double imaginary_value = 0.0 );
ROSE_DLL_API SgComplexVal* buildComplexVal(SgValueExp* real_value, SgValueExp* imaginary_value);
ROSE_DLL_API SgComplexVal* buildComplexVal_nfi(SgValueExp* real_value, SgValueExp* imaginary_value, const std::string& str);
ROSE_DLL_API SgComplexVal* buildImaginaryVal(long double imaginary_value);
ROSE_DLL_API SgComplexVal* buildImaginaryVal(SgValueExp* imaginary_value);
ROSE_DLL_API SgComplexVal* buildImaginaryVal_nfi(SgValueExp* imaginary_value, const std::string& str);

//! Build a double value expression
ROSE_DLL_API SgDoubleVal* buildDoubleVal(double value = 0.0);
ROSE_DLL_API SgDoubleVal* buildDoubleVal_nfi(double value, const std::string& str);

ROSE_DLL_API SgFloatVal* buildFloatVal(float value = 0.0);
ROSE_DLL_API SgFloatVal* buildFloatVal_nfi(float value, const std::string& str);

//! Build an integer value expression
ROSE_DLL_API SgIntVal* buildIntVal(int value = 0);
ROSE_DLL_API SgIntVal* buildIntValHex(int value = 0);
ROSE_DLL_API SgIntVal* buildIntVal_nfi(int value, const std::string& str);

//! Build a long integer value expression
ROSE_DLL_API SgLongIntVal* buildLongIntVal(long value = 0);
ROSE_DLL_API SgLongIntVal* buildLongIntValHex(long value = 0);
ROSE_DLL_API SgLongIntVal* buildLongIntVal_nfi(long value, const std::string& str);

//! Build a long long integer value expression
ROSE_DLL_API SgLongLongIntVal* buildLongLongIntVal(long long value = 0);
ROSE_DLL_API SgLongLongIntVal* buildLongLongIntValHex(long long value = 0);
ROSE_DLL_API SgLongLongIntVal* buildLongLongIntVal_nfi(long long value, const std::string& str);
// !Build enum val without file info: nfi
ROSE_DLL_API SgEnumVal* buildEnumVal_nfi(int value, SgEnumDeclaration* decl, SgName name);
// !Build enum val with transformation file info
ROSE_DLL_API SgEnumVal* buildEnumVal(int value, SgEnumDeclaration* decl, SgName name);
ROSE_DLL_API SgEnumVal* buildEnumVal(SgEnumFieldSymbol * sym);

ROSE_DLL_API SgLongDoubleVal* buildLongDoubleVal(long double value = 0.0);
ROSE_DLL_API SgLongDoubleVal* buildLongDoubleVal_nfi(long double value, const std::string& str);

ROSE_DLL_API SgFloat80Val* buildFloat80Val(long double value = 0.0);
ROSE_DLL_API SgFloat80Val* buildFloat80Val_nfi(long double value, const std::string& str);

ROSE_DLL_API SgFloat128Val* buildFloat128Val(long double value = 0.0);
ROSE_DLL_API SgFloat128Val* buildFloat128Val_nfi(long double value, const std::string& str);

ROSE_DLL_API SgShortVal* buildShortVal(short value = 0);
ROSE_DLL_API SgShortVal* buildShortValHex(short value = 0);
ROSE_DLL_API SgShortVal* buildShortVal_nfi(short value, const std::string& str);

ROSE_DLL_API SgStringVal* buildStringVal(std::string value="");
ROSE_DLL_API SgStringVal* buildStringVal_nfi(std::string value);

//! Build an unsigned char
ROSE_DLL_API SgUnsignedCharVal* buildUnsignedCharVal(unsigned char v = 0);
ROSE_DLL_API SgUnsignedCharVal* buildUnsignedCharValHex(unsigned char v = 0);
ROSE_DLL_API SgUnsignedCharVal* buildUnsignedCharVal_nfi(unsigned char v, const std::string& str);

//! Build an unsigned short integer
ROSE_DLL_API SgUnsignedShortVal* buildUnsignedShortVal(unsigned short v = 0);
ROSE_DLL_API SgUnsignedShortVal* buildUnsignedShortValHex(unsigned short v = 0);
ROSE_DLL_API SgUnsignedShortVal* buildUnsignedShortVal_nfi(unsigned short v, const std::string& str);

//! Build an unsigned integer
ROSE_DLL_API SgUnsignedIntVal* buildUnsignedIntVal(unsigned int v = 0);
ROSE_DLL_API SgUnsignedIntVal* buildUnsignedIntValHex(unsigned int v = 0);
ROSE_DLL_API SgUnsignedIntVal* buildUnsignedIntVal_nfi(unsigned int v, const std::string& str);

//! Build a unsigned long integer
ROSE_DLL_API SgUnsignedLongVal* buildUnsignedLongVal(unsigned long v = 0);
ROSE_DLL_API SgUnsignedLongVal* buildUnsignedLongValHex(unsigned long v = 0);
ROSE_DLL_API SgUnsignedLongVal* buildUnsignedLongVal_nfi(unsigned long v, const std::string& str);

//! Build an unsigned long long integer
ROSE_DLL_API SgUnsignedLongLongIntVal* buildUnsignedLongLongIntVal(unsigned long long v = 0);
ROSE_DLL_API SgUnsignedLongLongIntVal* buildUnsignedLongLongIntValHex(unsigned long long v = 0);
ROSE_DLL_API SgUnsignedLongLongIntVal* buildUnsignedLongLongIntVal_nfi(unsigned long long v, const std::string& str);

//! Build an template parameter value expression
ROSE_DLL_API SgTemplateParameterVal* buildTemplateParameterVal(int template_parameter_position = -1);
ROSE_DLL_API SgTemplateParameterVal* buildTemplateParameterVal_nfi(int template_parameter_position, const std::string& str);

//! Build a template type, used for template parameter and later argument
ROSE_DLL_API SgTemplateType* buildTemplateType(SgName name="");

//! Build a template parameter, passing enum kind and SgTemplateType
//! template_parameter_enum { parameter_undefined = 0, type_parameter = 1, nontype_parameter = 2,  template_parameter = 3}
ROSE_DLL_API SgTemplateParameter * buildTemplateParameter (SgTemplateParameter::template_parameter_enum parameterType, SgType*);

//! Build a declaration of a non-real class or class-member representing template parameters and their members
ROSE_DLL_API SgNonrealDecl * buildNonrealDecl(const SgName & name, SgDeclarationScope * scope, SgDeclarationScope * child_scope = NULL);

//! Build a reference to the non-real declaration of a member of a non-real class
ROSE_DLL_API SgNonrealRefExp * buildNonrealRefExp_nfi(SgNonrealSymbol * sym);

//! Build UPC THREADS (integer expression)
ROSE_DLL_API SgUpcThreads* buildUpcThreads();
ROSE_DLL_API SgUpcThreads* buildUpcThreads_nfi();

//! Build UPC  MYTHREAD (integer expression)
ROSE_DLL_API SgUpcMythread* buildUpcMythread();
ROSE_DLL_API SgUpcMythread* buildUpcMythread_nfi();

//! Build this pointer
ROSE_DLL_API SgThisExp* buildThisExp(SgSymbol* sym);
ROSE_DLL_API SgThisExp* buildThisExp_nfi(SgSymbol* sym);

//! Build super pointer
ROSE_DLL_API SgSuperExp* buildSuperExp(SgClassSymbol* sym);
ROSE_DLL_API SgSuperExp* buildSuperExp_nfi(SgClassSymbol* sym);

//! Build class pointer
ROSE_DLL_API SgClassExp* buildClassExp(SgClassSymbol* sym);
ROSE_DLL_API SgClassExp* buildClassExp_nfi(SgClassSymbol* sym);

//! Build lambda expression
ROSE_DLL_API SgLambdaRefExp* buildLambdaRefExp(SgType* return_type, SgFunctionParameterList* params, SgScopeStatement* scope);

#define BUILD_UNARY_PROTO(suffix) \
ROSE_DLL_API Sg##suffix * build##suffix(SgExpression* op =NULL); \
ROSE_DLL_API Sg##suffix * build##suffix##_nfi(SgExpression* op);

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

//Matlab transpose op
BUILD_UNARY_PROTO(MatrixTransposeOp)

//! Build a type casting expression
ROSE_DLL_API SgCastExp * buildCastExp(SgExpression *  operand_i = NULL,
                SgType * expression_type = NULL,
                SgCastExp::cast_type_enum cast_type = SgCastExp::e_C_style_cast);
ROSE_DLL_API SgCastExp * buildCastExp_nfi(SgExpression *  operand_i,
                SgType * expression_type,
                SgCastExp::cast_type_enum cast_type);

//! Build vararg op expression
ROSE_DLL_API SgVarArgOp * buildVarArgOp_nfi(SgExpression *  operand_i, SgType * expression_type);

//! Build -- expression, Sgop_mode is a value of either SgUnaryOp::prefix or SgUnaryOp::postfix
ROSE_DLL_API SgMinusOp *buildMinusOp(SgExpression* operand_i, SgUnaryOp::Sgop_mode  a_mode);
ROSE_DLL_API SgMinusOp *buildMinusOp_nfi(SgExpression* operand_i, SgUnaryOp::Sgop_mode  a_mode);
ROSE_DLL_API SgMinusMinusOp *buildMinusMinusOp(SgExpression* operand_i, SgUnaryOp::Sgop_mode  a_mode);
ROSE_DLL_API SgMinusMinusOp *buildMinusMinusOp_nfi(SgExpression* operand_i, SgUnaryOp::Sgop_mode  a_mode);

//! Build ++x or x++ , specify prefix or postfix using either SgUnaryOp::prefix or SgUnaryOp::postfix
ROSE_DLL_API SgPlusPlusOp* buildPlusPlusOp(SgExpression* operand_i, SgUnaryOp::Sgop_mode  a_mode);
ROSE_DLL_API SgPlusPlusOp* buildPlusPlusOp_nfi(SgExpression* operand_i, SgUnaryOp::Sgop_mode  a_mode);

//! Build a ThrowOp expression
ROSE_DLL_API SgThrowOp* buildThrowOp(SgExpression *, SgThrowOp::e_throw_kind);

ROSE_DLL_API SgNewExp * buildNewExp(SgType* type,
                       SgExprListExp* exprListExp,
                       SgConstructorInitializer* constInit,
                       SgExpression* expr,
                       short int val,
                       SgFunctionDeclaration* funcDecl);

ROSE_DLL_API SgDeleteExp* buildDeleteExp(SgExpression* variable,
                            short is_array,
                            short need_global_specifier,
                            SgFunctionDeclaration* deleteOperatorDeclaration);

//! DQ (1/25/2013): Added support for typeId operators.
ROSE_DLL_API SgTypeIdOp* buildTypeIdOp(SgExpression *operand_expr, SgType *operand_type);


#undef BUILD_UNARY_PROTO

/*! The instantiated functions' prototypes are not shown since they are expanded using macros.
 * Doxygen is not smart enough to handle macro expansion.
 */

#define BUILD_BINARY_PROTO(suffix) \
ROSE_DLL_API Sg##suffix * build##suffix(SgExpression* lhs =NULL, SgExpression* rhs =NULL); \
ROSE_DLL_API Sg##suffix * build##suffix##_nfi(SgExpression* lhs, SgExpression* rhs);

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
BUILD_BINARY_PROTO(ExponentiationAssignOp)
BUILD_BINARY_PROTO(GreaterOrEqualOp)
BUILD_BINARY_PROTO(GreaterThanOp)
BUILD_BINARY_PROTO(IntegerDivideOp)
BUILD_BINARY_PROTO(IntegerDivideAssignOp)
BUILD_BINARY_PROTO(IorAssignOp)
BUILD_BINARY_PROTO(IsOp)
BUILD_BINARY_PROTO(IsNotOp)

BUILD_BINARY_PROTO(LessOrEqualOp)
BUILD_BINARY_PROTO(LessThanOp)
BUILD_BINARY_PROTO(LshiftAssignOp)
BUILD_BINARY_PROTO(LshiftOp)

BUILD_BINARY_PROTO(MembershipOp)
BUILD_BINARY_PROTO(MinusAssignOp)
BUILD_BINARY_PROTO(ModAssignOp)
BUILD_BINARY_PROTO(ModOp)
BUILD_BINARY_PROTO(MultAssignOp)
BUILD_BINARY_PROTO(MultiplyOp)

BUILD_BINARY_PROTO(NonMembershipOp)
BUILD_BINARY_PROTO(NotEqualOp)
BUILD_BINARY_PROTO(OrOp)
BUILD_BINARY_PROTO(PlusAssignOp)
BUILD_BINARY_PROTO(PntrArrRefExp)
BUILD_BINARY_PROTO(RshiftAssignOp)
BUILD_BINARY_PROTO(JavaUnsignedRshiftAssignOp)

BUILD_BINARY_PROTO(RshiftOp)
BUILD_BINARY_PROTO(JavaUnsignedRshiftOp)
BUILD_BINARY_PROTO(ScopeOp)
BUILD_BINARY_PROTO(SubtractOp)
BUILD_BINARY_PROTO(XorAssignOp)

BUILD_BINARY_PROTO(VarArgCopyOp)
BUILD_BINARY_PROTO(VarArgStartOp)

BUILD_BINARY_PROTO(PowerOp);
BUILD_BINARY_PROTO(ElementwisePowerOp);
BUILD_BINARY_PROTO(ElementwiseMultiplyOp);
BUILD_BINARY_PROTO(ElementwiseDivideOp);
BUILD_BINARY_PROTO(LeftDivideOp);
BUILD_BINARY_PROTO(ElementwiseLeftDivideOp);
BUILD_BINARY_PROTO(ElementwiseAddOp);
BUILD_BINARY_PROTO(ElementwiseSubtractOp);

#undef BUILD_BINARY_PROTO

//! Build a conditional expression ?:
ROSE_DLL_API SgConditionalExp * buildConditionalExp(SgExpression* test =NULL, SgExpression* a =NULL, SgExpression* b =NULL);
SgConditionalExp * buildConditionalExp_nfi(SgExpression* test, SgExpression* a, SgExpression* b, SgType* t);

//! Build a SgExprListExp, used for function call parameter list etc.
ROSE_DLL_API SgExprListExp * buildExprListExp(SgExpression * expr1 = NULL, SgExpression* expr2 = NULL, SgExpression* expr3 = NULL, SgExpression* expr4 = NULL, SgExpression* expr5 = NULL, SgExpression* expr6 = NULL, SgExpression* expr7 = NULL, SgExpression* expr8 = NULL, SgExpression* expr9 = NULL, SgExpression* expr10 = NULL);
ROSE_DLL_API SgExprListExp * buildExprListExp(const std::vector<SgExpression*>& exprs);
SgExprListExp * buildExprListExp_nfi();
SgExprListExp * buildExprListExp_nfi(const std::vector<SgExpression*>& exprs);

//! Build a SgTupleExp
ROSE_DLL_API SgTupleExp * buildTupleExp(SgExpression * expr1 = NULL, SgExpression* expr2 = NULL, SgExpression* expr3 = NULL, SgExpression* expr4 = NULL, SgExpression* expr5 = NULL, SgExpression* expr6 = NULL, SgExpression* expr7 = NULL, SgExpression* expr8 = NULL, SgExpression* expr9 = NULL, SgExpression* expr10 = NULL);
ROSE_DLL_API SgTupleExp * buildTupleExp(const std::vector<SgExpression*>& exprs);
SgTupleExp * buildTupleExp_nfi();
SgTupleExp * buildTupleExp_nfi(const std::vector<SgExpression*>& exprs);

//! Build a SgListExp
ROSE_DLL_API SgListExp * buildListExp(SgExpression * expr1 = NULL, SgExpression* expr2 = NULL, SgExpression* expr3 = NULL, SgExpression* expr4 = NULL, SgExpression* expr5 = NULL, SgExpression* expr6 = NULL, SgExpression* expr7 = NULL, SgExpression* expr8 = NULL, SgExpression* expr9 = NULL, SgExpression* expr10 = NULL);
ROSE_DLL_API SgListExp * buildListExp(const std::vector<SgExpression*>& exprs);
SgListExp * buildListExp_nfi();
SgListExp * buildListExp_nfi(const std::vector<SgExpression*>& exprs);

ROSE_DLL_API SgComprehension * buildComprehension(SgExpression *target, SgExpression *iter, SgExprListExp *ifs);
SgComprehension * buildComprehension_nfi(SgExpression *target, SgExpression *iter, SgExprListExp *ifs);

ROSE_DLL_API SgListComprehension * buildListComprehension(SgExpression *elt, SgExprListExp *generators);
SgListComprehension * buildListComprehension_nfi(SgExpression *elt, SgExprListExp *generators);

ROSE_DLL_API SgSetComprehension * buildSetComprehension(SgExpression *elt, SgExprListExp *generators);
SgSetComprehension * buildSetComprehension_nfi(SgExpression *elt, SgExprListExp *generators);

ROSE_DLL_API SgDictionaryComprehension * buildDictionaryComprehension(SgKeyDatumPair *kd_pair, SgExprListExp *generators);
SgDictionaryComprehension * buildDictionaryComprehension_nfi(SgKeyDatumPair *kd_pair, SgExprListExp *generators);

//! Build SgVarRefExp based on a variable's Sage name. It will lookup symbol table internally starting from scope. A variable name is unique so type can be inferred (double check this).

/*!
It is possible to build a reference to a variable with known name before the variable is declaration, especially during bottomup construction of AST. In this case, SgTypeUnknown is used to indicate the variable reference needing postprocessing fix using fixVariableReferences() once the AST is complete and all variable declarations exist. But the side effect is some get_type() operation may not recognize the unknown type before the fix. So far, I extended SgPointerDerefExp::get_type() and SgPntrArrRefExp::get_type() for SgTypeUnknown. There may be others needing the same extension.
*/
ROSE_DLL_API SgVarRefExp * buildVarRefExp(const SgName& name, SgScopeStatement* scope=NULL);

//! Build SgVarRefExp based on a variable's name. It will lookup symbol table internally starting from scope. A variable is unique so type can be inferred.
ROSE_DLL_API SgVarRefExp * buildVarRefExp(const std::string& varName, SgScopeStatement* scope=NULL);

//! Build a variable reference using a C style char array
ROSE_DLL_API SgVarRefExp * buildVarRefExp(const char* varName, SgScopeStatement* scope=NULL);

//! Build a variable reference from an existing symbol
ROSE_DLL_API SgVarRefExp * buildVarRefExp(SgVariableSymbol* varSymbol);
ROSE_DLL_API SgVarRefExp * buildVarRefExp_nfi(SgVariableSymbol* varSymbol);

//! Build a variable reference from an existing variable declaration. The assumption is a SgVariableDeclartion only declares one variable in the ROSE AST.
ROSE_DLL_API SgVarRefExp * buildVarRefExp(SgVariableDeclaration* vardecl);

//!Build a variable reference from an initialized name
//!It first tries to grab the associated symbol, then call buildVarRefExp(const SgName& name, SgScopeStatement*) if symbol does not exist.
ROSE_DLL_API SgVarRefExp * buildVarRefExp(SgInitializedName* initname, SgScopeStatement* scope=NULL);

//!Build a variable reference expression at scope to an opaque variable which has unknown information except for its name.  Used when referring to an internal variable defined in some headers of runtime libraries.(The headers are not yet inserted into the file during translation). Similar to buildOpaqueType();
/*! It will declare a hidden int varName  at the specified scope to cheat the AST consistence tests.
 */
ROSE_DLL_API SgVarRefExp* buildOpaqueVarRefExp(const std::string& varName,SgScopeStatement* scope=NULL);

// DQ (9/4/2013): Added support for building compound literals (similar to a SgVarRefExp).
//! Build function for compound literals (uses a SgVariableSymbol and is similar to buildVarRefExp_nfi()).
SgCompoundLiteralExp* buildCompoundLiteralExp_nfi(SgVariableSymbol* varSymbol);
SgCompoundLiteralExp* buildCompoundLiteralExp(SgVariableSymbol* varSymbol);

//! Build a Fortran numeric label ref exp
ROSE_DLL_API SgLabelRefExp * buildLabelRefExp(SgLabelSymbol * s);

//! Build SgFunctionRefExp based on a C++ function's name and function type. It will lookup symbol table internally starting from scope. A hidden prototype will be created internally to introduce a new function symbol if the function symbol cannot be found.
ROSE_DLL_API SgFunctionRefExp * buildFunctionRefExp(const SgName& name, const SgType* func_type, SgScopeStatement* scope=NULL);

ROSE_DLL_API SgFunctionRefExp * buildFunctionRefExp(const char* name, const SgType* func_type, SgScopeStatement* scope=NULL);

//! Build SgFunctionRefExp based on a C function's name. It will lookup symbol table internally starting from scope and return the first matching function.
ROSE_DLL_API SgFunctionRefExp * buildFunctionRefExp(const SgName& name,SgScopeStatement* scope=NULL);

ROSE_DLL_API SgFunctionRefExp * buildFunctionRefExp(const char* name,SgScopeStatement* scope=NULL);

//! Build SgFunctionRefExp based on a function's declaration.
ROSE_DLL_API SgFunctionRefExp * buildFunctionRefExp(const SgFunctionDeclaration* func_decl);

//! Build SgFunctionRefExp based on a function's symbol.
ROSE_DLL_API SgFunctionRefExp * buildFunctionRefExp(SgFunctionSymbol* sym);

SgFunctionRefExp * buildFunctionRefExp_nfi(SgFunctionSymbol* sym);

//! DQ (12/15/2011): Adding template declaration support to the AST.
SgTemplateFunctionRefExp* buildTemplateFunctionRefExp_nfi(SgTemplateFunctionSymbol* sym);

//! DQ (12/29/2011): Adding template declaration support to the AST.
SgTemplateMemberFunctionRefExp* buildTemplateMemberFunctionRefExp_nfi(SgTemplateMemberFunctionSymbol* sym, bool virtual_call, bool need_qualifier);

SgMemberFunctionRefExp * buildMemberFunctionRefExp_nfi(SgMemberFunctionSymbol* sym, bool virtual_call, bool need_qualifier);
ROSE_DLL_API SgMemberFunctionRefExp * buildMemberFunctionRefExp(SgMemberFunctionSymbol* sym, bool virtual_call, bool need_qualifier);
SgClassNameRefExp * buildClassNameRefExp_nfi(SgClassSymbol* sym);
ROSE_DLL_API SgClassNameRefExp * buildClassNameRefExp(SgClassSymbol* sym);

//! Build a function call expression
ROSE_DLL_API SgFunctionCallExp* buildFunctionCallExp(SgFunctionSymbol* sym, SgExprListExp* parameters=NULL);
SgFunctionCallExp* buildFunctionCallExp_nfi(SgExpression* f, SgExprListExp* parameters=NULL);
ROSE_DLL_API SgFunctionCallExp* buildFunctionCallExp(SgExpression* f, SgExprListExp* parameters=NULL);

//! Build a function call expression,it will automatically search for function symbols internally to build a right function reference etc. It tolerates the lack of the function symbol to support generating calls to library functions whose headers have not yet been inserted.
ROSE_DLL_API SgFunctionCallExp*
buildFunctionCallExp(const SgName& name, SgType* return_type, SgExprListExp* parameters=NULL, SgScopeStatement* scope=NULL);

SgTypeTraitBuiltinOperator*
buildTypeTraitBuiltinOperator(SgName functionName, SgNodePtrList parameters);

//! Build a CUDA kernel call expression (kernel<<<config>>>(parameters))
SgCudaKernelCallExp * buildCudaKernelCallExp_nfi(
  SgExpression * kernel,
  SgExprListExp* parameters = NULL,
  SgCudaKernelExecConfig * config = NULL
);

//! Build a CUDA kernel execution configuration (<<<grid, blocks, shared, stream>>>)
SgCudaKernelExecConfig * buildCudaKernelExecConfig_nfi(
  SgExpression *grid = NULL,
  SgExpression *blocks = NULL,
  SgExpression *shared = NULL,
  SgExpression *stream = NULL
);

//! Build the rhs of a variable declaration which includes an assignment
ROSE_DLL_API SgAssignInitializer * buildAssignInitializer(SgExpression * operand_i = NULL, SgType * expression_type = NULL);
ROSE_DLL_API SgAssignInitializer * buildAssignInitializer_nfi(SgExpression * operand_i = NULL, SgType * expression_type = NULL);

//! Build an aggregate initializer
ROSE_DLL_API SgAggregateInitializer * buildAggregateInitializer(SgExprListExp * initializers = NULL, SgType * type = NULL);
ROSE_DLL_API SgAggregateInitializer * buildAggregateInitializer_nfi(SgExprListExp * initializers, SgType * type = NULL);

//! Build a compound initializer, for vector type initialization
ROSE_DLL_API SgCompoundInitializer * buildCompoundInitializer(SgExprListExp * initializers = NULL, SgType * type = NULL);
ROSE_DLL_API SgCompoundInitializer * buildCompoundInitializer_nfi(SgExprListExp * initializers, SgType * type = NULL);

// DQ (!/4/2009): Added support for building SgConstructorInitializer
ROSE_DLL_API SgConstructorInitializer * buildConstructorInitializer( SgMemberFunctionDeclaration *declaration,SgExprListExp *args,SgType *expression_type,bool need_name,bool need_qualifier,bool need_parenthesis_after_name,bool associated_class_unknown);
ROSE_DLL_API SgConstructorInitializer * buildConstructorInitializer_nfi( SgMemberFunctionDeclaration *declaration,SgExprListExp *args,SgType *expression_type,bool need_name,bool need_qualifier,bool need_parenthesis_after_name,bool associated_class_unknown);

//! Build an braced initializer
ROSE_DLL_API SgBracedInitializer * buildBracedInitializer(SgExprListExp * initializers = NULL, SgType * expression_type = NULL);
ROSE_DLL_API SgBracedInitializer * buildBracedInitializer_nfi(SgExprListExp * initializers = NULL, SgType * expression_type = NULL);

//! Build sizeof() expression with an expression parameter
ROSE_DLL_API SgSizeOfOp* buildSizeOfOp(SgExpression* exp = NULL);
ROSE_DLL_API SgSizeOfOp* buildSizeOfOp_nfi(SgExpression* exp);

//! Build sizeof() expression with a type parameter
ROSE_DLL_API SgSizeOfOp* buildSizeOfOp(SgType* type = NULL);
ROSE_DLL_API SgSizeOfOp* buildSizeOfOp_nfi(SgType* type);

//! Build __alignof__() expression with an expression parameter
ROSE_DLL_API SgAlignOfOp* buildAlignOfOp(SgExpression* exp = NULL);
ROSE_DLL_API SgAlignOfOp* buildAlignOfOp_nfi(SgExpression* exp);

//! Build __alignof__() expression with a type parameter
ROSE_DLL_API SgAlignOfOp* buildAlignOfOp(SgType* type = NULL);
ROSE_DLL_API SgAlignOfOp* buildAlignOfOp_nfi(SgType* type);

//! Build noecept operator expression with an expression parameter
ROSE_DLL_API SgNoexceptOp* buildNoexceptOp(SgExpression* exp = NULL);
ROSE_DLL_API SgNoexceptOp* buildNoexceptOp_nfi(SgExpression* exp);

// DQ (7/18/2011): Added support for SgJavaInstanceOfOp
//! This is part of Java specific operator support.
ROSE_DLL_API SgJavaInstanceOfOp* buildJavaInstanceOfOp(SgExpression* exp = NULL, SgType* type = NULL);

//! DQ (7/24/2014): Adding support for c11 generic operands.
ROSE_DLL_API SgTypeExpression *buildTypeExpression(SgType* type);

// DQ (8/11/2014): Added support for C++11 decltype used in new function return syntax.
ROSE_DLL_API SgFunctionParameterRefExp *buildFunctionParameterRefExp(int parameter_number, int parameter_level );
ROSE_DLL_API SgFunctionParameterRefExp *buildFunctionParameterRefExp_nfi(int parameter_number, int parameter_level );


//! DQ (9/3/2014): Adding support for C++11 Lambda expressions
ROSE_DLL_API SgLambdaExp* buildLambdaExp    (SgLambdaCaptureList* lambda_capture_list, SgClassDeclaration* lambda_closure_class, SgFunctionDeclaration* lambda_function);
ROSE_DLL_API SgLambdaExp* buildLambdaExp_nfi(SgLambdaCaptureList* lambda_capture_list, SgClassDeclaration* lambda_closure_class, SgFunctionDeclaration* lambda_function);

#if 0
ROSE_DLL_API SgLambdaCapture* buildLambdaCapture    (SgInitializedName* capture_variable, SgInitializedName* source_closure_variable, SgInitializedName* closure_variable);
ROSE_DLL_API SgLambdaCapture* buildLambdaCapture_nfi(SgInitializedName* capture_variable, SgInitializedName* source_closure_variable, SgInitializedName* closure_variable);
#else
ROSE_DLL_API SgLambdaCapture* buildLambdaCapture    (SgExpression* capture_variable, SgExpression* source_closure_variable, SgExpression* closure_variable);
ROSE_DLL_API SgLambdaCapture* buildLambdaCapture_nfi(SgExpression* capture_variable, SgExpression* source_closure_variable, SgExpression* closure_variable);
#endif

ROSE_DLL_API SgLambdaCaptureList* buildLambdaCaptureList    ();
ROSE_DLL_API SgLambdaCaptureList* buildLambdaCaptureList_nfi();

//@}

//@{
/*! @name Builders for Matlab nodes
 */
//! Build a Matlab range expression like start:end or start:stride:end
 ROSE_DLL_API SgRangeExp* buildRangeExp(SgExpression *start);

 //! Build a Matlab Matrix
 ROSE_DLL_API SgMatrixExp* buildMatrixExp(SgExprListExp *firstRow);

 //! Build a Matlab colon expression :
 ROSE_DLL_API SgMagicColonExp* buildMagicColonExp();

 //! Build a For-loop statement for matlab
 ROSE_DLL_API SgMatlabForStatement* buildMatlabForStatement(SgExpression* loop_index, SgExpression* loop_range, SgBasicBlock* loop_body);
//@}


//--------------------------------------------------------------
//@{
/*! @name Builders for support nodes
  \brief AST high level builders for SgSupport nodes

*/
//! Initialized names are tricky, their scope vary depending on context, so scope and symbol information are not needed until the initialized name is being actually used somewhere.

/*!e.g the scope of arguments of functions are different for defining and nondefining functions.
*/
ROSE_DLL_API SgInitializedName* buildInitializedName(const SgName & name, SgType* type, SgInitializer* init = NULL);
ROSE_DLL_API SgInitializedName* buildInitializedName(const std::string &name, SgType* type);
ROSE_DLL_API SgInitializedName* buildInitializedName(const char* name, SgType* type);
ROSE_DLL_API SgInitializedName* buildInitializedName_nfi(const SgName & name, SgType* type, SgInitializer* init);

//! Build SgFunctionParameterTypeList from SgFunctionParameterList
ROSE_DLL_API SgFunctionParameterTypeList *
buildFunctionParameterTypeList(SgFunctionParameterList * paralist);

//! Build SgFunctionParameterTypeList from an expression list, useful when building a function call
ROSE_DLL_API SgFunctionParameterTypeList *
buildFunctionParameterTypeList(SgExprListExp * expList);

//! Build an SgFunctionParameterTypeList from SgTypes. To build an
ROSE_DLL_API SgFunctionParameterTypeList *
buildFunctionParameterTypeList(SgType* type0 = NULL, SgType* type1 = NULL,
                               SgType* type2 = NULL, SgType* type3 = NULL,
                               SgType* type4 = NULL, SgType* type5 = NULL,
                               SgType* type6 = NULL, SgType* type7 = NULL);


//--------------------------------------------------------------
//@{
/*! @name Builders for statements
  \brief AST high level builders for SgStatement, explicit scope parameters are allowed for flexibility.
  Please use SageInterface::appendStatement(), prependStatement(), and insertStatement() to attach the newly built statements into an AST tree. Calling member functions like SgScopeStatement::prepend_statement() or using container functions such as pushback() is discouraged since they don't handle many side effects for symbol tables, source file information, scope and parent pointers etc.

*/

//! Build a variable declaration, handle symbol table transparently
ROSE_DLL_API SgVariableDeclaration*
buildVariableDeclaration(const SgName & name, SgType *type, SgInitializer *varInit=NULL, SgScopeStatement* scope=NULL);

ROSE_DLL_API SgVariableDeclaration*
buildVariableDeclaration(const std::string & name, SgType *type, SgInitializer *varInit=NULL, SgScopeStatement* scope=NULL);

ROSE_DLL_API SgVariableDeclaration*
buildVariableDeclaration(const char* name, SgType *type, SgInitializer *varInit=NULL, SgScopeStatement* scope=NULL);

// DQ (6/25/2019): Added support to force building the varialbe declaration from the variable use (e.g. within a class) 
// instead of from a declaration.  Variable declaration built from a use are reused when the declaration is seen (later 
// in processing of the class members).
// ROSE_DLL_API SgVariableDeclaration*
// buildVariableDeclaration_nfi(const SgName & name, SgType *type, SgInitializer *varInit, SgScopeStatement* scope);
ROSE_DLL_API SgVariableDeclaration*
buildVariableDeclaration_nfi(const SgName & name, SgType *type, SgInitializer *varInit, SgScopeStatement* scope, bool builtFromUseOnly = false);

//! Build variable definition
ROSE_DLL_API SgVariableDefinition*
buildVariableDefinition_nfi (SgVariableDeclaration* decl, SgInitializedName* init_name,  SgInitializer *init);


// DQ (8/31/2012): Note that this macro can't be used in header files since it can only be set
// after sage3.h has been read.  The reason is that this is a portability problem when "rose_config.h"
// appears in header files of applications using ROSE's header files.
// DQ (12/6/2011): Adding support for template declarations into the AST.
// SgTemplateDeclaration*
// SgVariableDeclaration* buildTemplateVariableDeclaration_nfi(const SgName & name, SgType *type, SgInitializer *varInit, SgScopeStatement* scope);
ROSE_DLL_API SgTemplateVariableDeclaration* buildTemplateVariableDeclaration_nfi(const SgName & name, SgType *type, SgInitializer *varInit, SgScopeStatement* scope);

//!Build a typedef declaration, such as: typedef int myint;  typedef struct A {..} s_A;
ROSE_DLL_API SgTypedefDeclaration*
buildTypedefDeclaration(const std::string& name, SgType* base_type, SgScopeStatement* scope = NULL, bool has_defining_base=false);

ROSE_DLL_API SgTypedefDeclaration*
buildTypedefDeclaration_nfi(const std::string& name, SgType* base_type, SgScopeStatement* scope = NULL, bool has_defining_base=false);

ROSE_DLL_API SgTemplateTypedefDeclaration*
buildTemplateTypedefDeclaration_nfi(const SgName & name, SgType* base_type, SgScopeStatement* scope = NULL, bool has_defining_base=false);

#if 1
// ROSE_DLL_API SgTemplateInstantiationTypedefDeclaration*
// buildTemplateInstantiationTypedefDeclaration_nfi(SgName name, SgType* base_type, SgScopeStatement* scope, bool has_defining_base, SgTemplateTypedefDeclaration* templateTypedefDeclaration, SgTemplateArgumentPtrList templateArgumentList);
// ROSE_DLL_API SgTemplateInstantiationTypedefDeclaration*
// buildTemplateInstantiationTypedefDeclaration_nfi(SgName name, SgType* base_type, SgScopeStatement* scope, bool has_defining_base, SgTemplateTypedefDeclaration* templateTypedefDeclaration);
// ROSE_DLL_API SgTemplateInstantiationTypedefDeclaration*
// buildTemplateInstantiationTypedefDeclaration_nfi();
ROSE_DLL_API SgTemplateInstantiationTypedefDeclaration*
buildTemplateInstantiationTypedefDeclaration_nfi(SgName & name, SgType* base_type, SgScopeStatement* scope, bool has_defining_base, SgTemplateTypedefDeclaration* templateTypedefDeclaration, SgTemplateArgumentPtrList & templateArgumentsList);
#endif

//! Build an empty SgFunctionParameterList, possibly with some initialized names filled in
ROSE_DLL_API SgFunctionParameterList * buildFunctionParameterList(SgInitializedName* in1 = NULL, SgInitializedName* in2 = NULL, SgInitializedName* in3 = NULL, SgInitializedName* in4 = NULL, SgInitializedName* in5 = NULL, SgInitializedName* in6 = NULL, SgInitializedName* in7 = NULL, SgInitializedName* in8 = NULL, SgInitializedName* in9 = NULL, SgInitializedName* in10 = NULL);
SgFunctionParameterList * buildFunctionParameterList_nfi();

//! Build an SgFunctionParameterList from SgFunctionParameterTypeList, like (int, float,...), used for parameter list of prototype functions when function type( including parameter type list) is known.
ROSE_DLL_API SgFunctionParameterList*
buildFunctionParameterList(SgFunctionParameterTypeList * paraTypeList);

ROSE_DLL_API SgFunctionParameterList*
buildFunctionParameterList_nfi(SgFunctionParameterTypeList * paraTypeList);

//! DQ (2/11/2012): Added support to set the template name in function template instantiations (member and non-member).
ROSE_DLL_API void setTemplateNameInTemplateInstantiations( SgFunctionDeclaration* func, const SgName & name );

// DQ (9/13/2012): Need to set the parents of SgTemplateArgument IR nodes now that they are passed in as part of the SageBuilder API.
ROSE_DLL_API void setTemplateArgumentParents( SgDeclarationStatement* decl );
ROSE_DLL_API void testTemplateArgumentParents( SgDeclarationStatement* decl );
ROSE_DLL_API SgTemplateArgumentPtrList* getTemplateArgumentList( SgDeclarationStatement* decl );

//! DQ (9/16/2012): Added function to support setting the template parameters and setting their parents (and for any relevant declaration).
ROSE_DLL_API void testTemplateParameterParents( SgDeclarationStatement* decl );
ROSE_DLL_API void setTemplateParameterParents( SgDeclarationStatement* decl );
ROSE_DLL_API SgTemplateParameterPtrList* getTemplateParameterList( SgDeclarationStatement* decl );

//! DQ (9/16/2012): Added function to support setting the template arguments and setting their parents (and for any relevant declaration).
ROSE_DLL_API void setTemplateArgumentsInDeclaration               ( SgDeclarationStatement* decl, SgTemplateArgumentPtrList* templateArgumentsList_input );
ROSE_DLL_API void setTemplateSpecializationArgumentsInDeclaration ( SgDeclarationStatement* decl, SgTemplateArgumentPtrList* templateSpecializationArgumentsList_input );
ROSE_DLL_API void setTemplateParametersInDeclaration              ( SgDeclarationStatement* decl, SgTemplateParameterPtrList* templateParametersList_input );

//! Build a prototype for a function, handle function type, symbol etc transparently
// DQ (7/26/2012): Changing the API to include template arguments so that we can generate names with and without template arguments (to support name mangiling).
ROSE_DLL_API SgFunctionDeclaration*
buildNondefiningFunctionDeclaration (const SgName & name, SgType* return_type, SgFunctionParameterList *parlist, SgScopeStatement* scope, SgExprListExp* decoratorList, bool buildTemplateInstantiation, SgTemplateArgumentPtrList* templateArgumentsList);

// DQ (8/28/2012): This preserves the original API with a simpler function (however for C++ at least, it is frequently not sufficent).
// We need to decide if the SageBuilder API should include these sorts of functions.
ROSE_DLL_API SgFunctionDeclaration* buildNondefiningFunctionDeclaration(const SgName& name, SgType* return_type, SgFunctionParameterList* parameter_list, SgScopeStatement* scope = NULL, SgExprListExp* decoratorList = NULL);

// DQ (8/11/2013): Even though template functions can't use partial specialization, they can be specialized,
// however the specialization does not define a template and instead defines a template instantiation, so we
// don't need the SgTemplateArgumentPtrList in this function.
// SgTemplateFunctionDeclaration* buildNondefiningTemplateFunctionDeclaration (const SgName & name, SgType* return_type, SgFunctionParameterList *parlist, SgScopeStatement* scope=NULL, SgExprListExp* decoratorList = NULL);
ROSE_DLL_API SgTemplateFunctionDeclaration*
buildNondefiningTemplateFunctionDeclaration (const SgName & name, SgType* return_type, SgFunctionParameterList *parlist, SgScopeStatement* scope=NULL, SgExprListExp* decoratorList = NULL, SgTemplateParameterPtrList* templateParameterList = NULL);

// DQ (8/11/2013): Note that access to the SgTemplateParameterPtrList should be handled through the first_nondefining_declaration (which is a required parameter).
// DQ (12/1/2011): Adding support for template declarations into the AST.
ROSE_DLL_API SgTemplateFunctionDeclaration*
buildDefiningTemplateFunctionDeclaration (const SgName & name, SgType* return_type, SgFunctionParameterList *parlist, SgScopeStatement* scope, SgExprListExp* decoratorList, SgTemplateFunctionDeclaration* first_nondefining_declaration);

//! Build a prototype for an existing function declaration (defining or nondefining is fine)
ROSE_DLL_API SgFunctionDeclaration *
buildNondefiningFunctionDeclaration (const SgFunctionDeclaration* funcdecl, SgScopeStatement* scope=NULL, SgExprListExp* decoratorList = NULL);

//! Build a prototype member function declaration
// SgMemberFunctionDeclaration * buildNondefiningMemberFunctionDeclaration (const SgName & name, SgType* return_type, SgFunctionParameterList *parlist, SgScopeStatement* scope=NULL, SgExprListExp* decoratorList = NULL);
// SgMemberFunctionDeclaration * buildNondefiningMemberFunctionDeclaration (const SgName & name, SgType* return_type, SgFunctionParameterList *parlist, SgScopeStatement* scope=NULL, SgExprListExp* decoratorList = NULL, unsigned int functionConstVolatileFlags = 0);
// SgMemberFunctionDeclaration* buildNondefiningMemberFunctionDeclaration (const SgName & name, SgType* return_type, SgFunctionParameterList *parlist, SgScopeStatement* scope=NULL, SgExprListExp* decoratorList = NULL, unsigned int functionConstVolatileFlags = 0, bool buildTemplateInstantiation = false);
ROSE_DLL_API SgMemberFunctionDeclaration*
buildNondefiningMemberFunctionDeclaration (const SgName & name, SgType* return_type, SgFunctionParameterList *parlist, SgScopeStatement* scope, SgExprListExp* decoratorList, unsigned int functionConstVolatileFlags, bool buildTemplateInstantiation, SgTemplateArgumentPtrList* templateArgumentsList);

// DQ (8/12/2013): This function needs to supporte SgTemplateParameterPtrList and SgTemplateArgumentPtrList parameters.
// SgTemplateMemberFunctionDeclaration* buildNondefiningTemplateMemberFunctionDeclaration (const SgName & name, SgType* return_type, SgFunctionParameterList *parlist, SgScopeStatement* scope = NULL, SgExprListExp* decoratorList = NULL, unsigned int functionConstVolatileFlags = 0);
ROSE_DLL_API SgTemplateMemberFunctionDeclaration*
buildNondefiningTemplateMemberFunctionDeclaration (const SgName & name, SgType* return_type, SgFunctionParameterList *parlist, SgScopeStatement* scope, SgExprListExp* decoratorList, unsigned int functionConstVolatileFlags, SgTemplateParameterPtrList* templateParameterList );

// DQ (12/1/2011): Adding support for template declarations in the AST.
ROSE_DLL_API SgTemplateMemberFunctionDeclaration*
buildDefiningTemplateMemberFunctionDeclaration (const SgName & name, SgType* return_type, SgFunctionParameterList *parlist, SgScopeStatement* scope, SgExprListExp* decoratorList, unsigned int functionConstVolatileFlags, SgTemplateMemberFunctionDeclaration* first_nondefing_declaration );

////! Build a prototype member function declaration
// SgMemberFunctionDeclaration* buildNondefiningMemberFunctionDeclaration (const SgName & name, SgMemberFunctionType* func_type, SgFunctionParameterList* paralist, SgScopeStatement* scope=NULL);

// DQ (8/11/2013): Note that the specification of the SgTemplateArgumentPtrList is somewhat redundant with the required parameter first_nondefinng_declaration (I think).
//! Build a defining ( non-prototype) member function declaration
// SgMemberFunctionDeclaration* buildDefiningMemberFunctionDeclaration (const SgName & name, SgType* return_type, SgFunctionParameterList *parlist, SgScopeStatement* scope, SgExprListExp* decoratorList, bool buildTemplateInstantiation, unsigned int functionConstVolatileFlags, SgMemberFunctionDeclaration* first_nondefinng_declaration);
ROSE_DLL_API SgMemberFunctionDeclaration*
buildDefiningMemberFunctionDeclaration (const SgName & name, SgType* return_type, SgFunctionParameterList *parlist, SgScopeStatement* scope, SgExprListExp* decoratorList, bool buildTemplateInstantiation, unsigned int functionConstVolatileFlags, SgMemberFunctionDeclaration* first_nondefinng_declaration, SgTemplateArgumentPtrList* templateArgumentsList);

#if 0
// DQ (3/20/2017): This function is not used (so let's see if we can remove it).
// DQ (8/29/2012): This is re-enabled because the backstroke project is using it (also added back the default parameters; which I don't think I like in the API).
// DQ (7/26/2012): I would like to remove this from the API (at least for now while debugging the newer API required for template argument handling).
//! Build a defining ( non-prototype) member function declaration from a SgMemberFunctionType
ROSE_DLL_API SgMemberFunctionDeclaration*
buildDefiningMemberFunctionDeclaration (const SgName & name, SgMemberFunctionType* func_type, SgScopeStatement* scope, SgExprListExp* decoratorList = NULL /* , unsigned int functionConstVolatileFlags = 0 */, SgMemberFunctionDeclaration* first_nondefinng_declaration = NULL);
#endif

#if 0
// DQ (3/20/2017): This function is not used (so let's see if we can remove it).
//! Build a prototype for an existing member function declaration (defining or nondefining is fine)
// SgMemberFunctionDeclaration*
ROSE_DLL_API SgMemberFunctionDeclaration*
buildNondefiningMemberFunctionDeclaration (const SgMemberFunctionDeclaration* funcdecl, SgScopeStatement* scope=NULL, SgExprListExp* decoratorList = NULL, unsigned int functionConstVolatileFlags = 0);
#endif

// DQ (8/28/2012): This preserves the original API with a simpler function (however for C++ at least, it is frequently not sufficent).
// We need to decide if the SageBuilder API should include these sorts of functions.
ROSE_DLL_API SgMemberFunctionDeclaration* buildNondefiningMemberFunctionDeclaration(const SgName& name, SgType* return_type, SgFunctionParameterList* parameter_list, SgScopeStatement* scope = NULL);

// DQ (8/28/2012): This preserves the original API with a simpler function (however for C++ at least, it is frequently not sufficent).
// We need to decide if the SageBuilder API should include these sorts of functions.
ROSE_DLL_API SgMemberFunctionDeclaration* buildDefiningMemberFunctionDeclaration(const SgName& name, SgType* return_type, SgFunctionParameterList* parameter_list, SgScopeStatement* scope = NULL);

// DQ (8/11/2013): Note that the specification of the SgTemplateArgumentPtrList is somewhat redundant with the required parameter first_nondefinng_declaration (I think).
//! Build a function declaration with a function body
// SgFunctionDeclaration* buildDefiningFunctionDeclaration (const SgName & name, SgType* return_type, SgFunctionParameterList * parlist, SgScopeStatement* scope, SgExprListExp* decoratorList, bool buildTemplateInstantiation, SgFunctionDeclaration* first_nondefinng_declaration);
ROSE_DLL_API SgFunctionDeclaration*
buildDefiningFunctionDeclaration (const SgName & name, SgType* return_type, SgFunctionParameterList * parlist, SgScopeStatement* scope, SgExprListExp* decoratorList, bool buildTemplateInstantiation = false, SgFunctionDeclaration* first_nondefinng_declaration = NULL, SgTemplateArgumentPtrList* templateArgumentsList = NULL);

// DQ (8/28/2012): This preserves the original API with a simpler function (however for C++ at least, it is frequently not sufficient).
// We need to decide if the SageBuilder API should include these sorts of functions.
ROSE_DLL_API SgFunctionDeclaration* buildDefiningFunctionDeclaration(const SgName& name, SgType* return_type, SgFunctionParameterList* parameter_list, SgScopeStatement* scope = NULL);

// DQ (8/28/2012): This preserves the original API with a simpler function (however for C++ at least, it is frequently not sufficient).
// We need to decide if the SageBuilder API should include these sorts of functions.
ROSE_DLL_API SgProcedureHeaderStatement* buildProcedureHeaderStatement(const SgName& name, SgType* return_type, SgFunctionParameterList* parameter_list, SgProcedureHeaderStatement::subprogram_kind_enum, SgScopeStatement* scope = NULL);

//! Build a Fortran subroutine or procedure
ROSE_DLL_API SgProcedureHeaderStatement*
buildProcedureHeaderStatement(const char* name, SgType* return_type, SgFunctionParameterList * parlist, SgProcedureHeaderStatement::subprogram_kind_enum, SgScopeStatement* scope, SgProcedureHeaderStatement* first_nondefining_declaration );

//! Build a regular function call statement
ROSE_DLL_API SgExprStatement*
buildFunctionCallStmt(const SgName& name, SgType* return_type, SgExprListExp* parameters=NULL, SgScopeStatement* scope=NULL);

//! Build a function call statement using function expression and argument list only, like (*funcPtr)(args);
ROSE_DLL_API SgExprStatement*
buildFunctionCallStmt(SgExpression* function, SgExprListExp* parameters=NULL);


//! Build a label statement, name is the label's name. Handling label symbol and scope internally.

//! Note that the scope of a label statement is special. It is SgFunctionDefinition,
//! not the closest scope statement such as SgBasicBlock.
ROSE_DLL_API SgLabelStatement * buildLabelStatement(const SgName& name, SgStatement * stmt = NULL, SgScopeStatement* scope=NULL);
SgLabelStatement * buildLabelStatement_nfi(const SgName& name, SgStatement * stmt, SgScopeStatement* scope);

//! Build a goto statement
ROSE_DLL_API SgGotoStatement * buildGotoStatement(SgLabelStatement *  label=NULL);
SgGotoStatement * buildGotoStatement_nfi(SgLabelStatement *  label);

//! Build a goto statement from a label symbol, supporting both C/C++ and Fortran cases
ROSE_DLL_API SgGotoStatement * buildGotoStatement(SgLabelSymbol*  symbol);

// DQ (11/22/2017): Added support for computed code goto as defined by GNU C/C++ extension.
//! Build a goto statement from a label expression, supporting only C/C++ and not Fortran cases
SgGotoStatement * buildGotoStatement_nfi(SgExpression*  expr);

//! Build a case option statement
ROSE_DLL_API SgCaseOptionStmt * buildCaseOptionStmt( SgExpression * key = NULL,SgStatement *body = NULL);
SgCaseOptionStmt * buildCaseOptionStmt_nfi( SgExpression * key,SgStatement *body);

//! Build a default option statement
ROSE_DLL_API SgDefaultOptionStmt * buildDefaultOptionStmt( SgStatement *body = NULL);
SgDefaultOptionStmt * buildDefaultOptionStmt_nfi( SgStatement *body);

//! Build a SgExprStatement, set File_Info automatically
ROSE_DLL_API SgExprStatement* buildExprStatement(SgExpression*  exp = NULL);
SgExprStatement* buildExprStatement_nfi(SgExpression*  exp);

// DQ (3/27/2015): Added support for SgStatementExpression.
//! Build a GNU statement expression
ROSE_DLL_API SgStatementExpression* buildStatementExpression(SgStatement* exp);
ROSE_DLL_API SgStatementExpression* buildStatementExpression_nfi(SgStatement* exp);

//! Build a switch statement
ROSE_DLL_API SgSwitchStatement* buildSwitchStatement(SgStatement *item_selector = NULL,SgStatement *body = NULL);
inline SgSwitchStatement* buildSwitchStatement(SgExpression *item_selector, SgStatement *body = NULL) {
  return buildSwitchStatement(buildExprStatement(item_selector), body);
}
ROSE_DLL_API SgSwitchStatement* buildSwitchStatement_nfi(SgStatement *item_selector,SgStatement *body);

//! Build if statement
ROSE_DLL_API SgIfStmt * buildIfStmt(SgStatement* conditional, SgStatement * true_body, SgStatement * false_body);
inline SgIfStmt * buildIfStmt(SgExpression* conditional, SgStatement * true_body, SgStatement * false_body) {
  return buildIfStmt(buildExprStatement(conditional), true_body, false_body);
}

ROSE_DLL_API SgIfStmt* buildIfStmt_nfi(SgStatement* conditional, SgStatement * true_body, SgStatement * false_body);

// Rasmussen (9/3/2018)
//! Build a Fortran do construct
ROSE_DLL_API SgFortranDo * buildFortranDo(SgExpression* initialization, SgExpression* bound, SgExpression* increment, SgBasicBlock* loop_body);

//! Build a for init statement
ROSE_DLL_API SgForInitStatement* buildForInitStatement();
ROSE_DLL_API SgForInitStatement* buildForInitStatement(const SgStatementPtrList & statements);
ROSE_DLL_API SgForInitStatement* buildForInitStatement_nfi(SgStatementPtrList & statements);

// DQ (10/12/2012): Added new function for a single statement.
ROSE_DLL_API SgForInitStatement* buildForInitStatement( SgStatement* statement );

//!Build a for statement, assume none of the arguments is NULL
ROSE_DLL_API SgForStatement * buildForStatement(SgStatement* initialize_stmt,  SgStatement * test, SgExpression * increment, SgStatement * loop_body, SgStatement * else_body = NULL);
ROSE_DLL_API SgForStatement * buildForStatement_nfi(SgStatement* initialize_stmt, SgStatement * test, SgExpression * increment, SgStatement * loop_body, SgStatement * else_body = NULL);
ROSE_DLL_API SgForStatement * buildForStatement_nfi(SgForInitStatement * init_stmt, SgStatement * test, SgExpression * increment, SgStatement * loop_body, SgStatement * else_body = NULL);
ROSE_DLL_API void buildForStatement_nfi(SgForStatement* result, SgForInitStatement * init_stmt, SgStatement * test, SgExpression * increment, SgStatement * loop_body, SgStatement * else_body = NULL);

// DQ (3/26/2018): Adding support for range based for statement.
// ROSE_DLL_API SgRangeBasedForStatement* buildRangeBasedForStatement_nfi(SgVariableDeclaration* initializer, SgExpression* range, SgStatement* body);
ROSE_DLL_API SgRangeBasedForStatement* buildRangeBasedForStatement_nfi(
     SgVariableDeclaration* initializer, SgVariableDeclaration* range, 
     SgVariableDeclaration* begin_declaration, SgVariableDeclaration* end_declaration, 
     SgExpression* not_equal_expression, SgExpression* increment_expression,
     SgStatement* body);

// EDG 4.8 handled the do-while statement differently (more similar to a block scope than before in EDG 4.7 (i.e. with an end-of-construct statement).
// So we need an builder function that can use the existing SgDoWhileStatement scope already on the stack.
ROSE_DLL_API void buildDoWhileStatement_nfi(SgDoWhileStmt* result, SgStatement * body, SgStatement * condition);

//! Build a UPC forall statement
ROSE_DLL_API SgUpcForAllStatement * buildUpcForAllStatement_nfi(SgStatement* initialize_stmt, SgStatement * test, SgExpression * increment, SgExpression* affinity, SgStatement * loop_body);
ROSE_DLL_API SgUpcForAllStatement * buildUpcForAllStatement_nfi(SgForInitStatement * init_stmt, SgStatement * test, SgExpression * increment, SgExpression* affinity, SgStatement * loop_body);

// DQ (3/3/2013): Added UPC specific build functions.
//! Build a UPC notify statement
ROSE_DLL_API SgUpcNotifyStatement* buildUpcNotifyStatement_nfi(SgExpression* exp);

//! Build a UPC wait statement
ROSE_DLL_API SgUpcWaitStatement* buildUpcWaitStatement_nfi(SgExpression* exp);

//! Build a UPC barrier statement
ROSE_DLL_API SgUpcBarrierStatement* buildUpcBarrierStatement_nfi(SgExpression* exp);

//! Build a UPC fence statement
ROSE_DLL_API SgUpcFenceStatement* buildUpcFenceStatement_nfi();


//! Build while statement
ROSE_DLL_API SgWhileStmt * buildWhileStmt(SgStatement *  condition, SgStatement *body, SgStatement *else_body = NULL);
inline SgWhileStmt * buildWhileStmt(SgExpression *  condition, SgStatement *body, SgStatement* else_body = NULL) {
  return buildWhileStmt(buildExprStatement(condition), body, else_body);
}
SgWhileStmt * buildWhileStmt_nfi(SgStatement *  condition, SgStatement *body, SgStatement *else_body = NULL);

//! Build a with statement
ROSE_DLL_API SgWithStatement* buildWithStatement(SgExpression* expr, SgStatement* body);
SgWithStatement* buildWithStatement_nfi(SgExpression* expr, SgStatement* body);

//! Build do-while statement
ROSE_DLL_API SgDoWhileStmt * buildDoWhileStmt(SgStatement *  body, SgStatement *condition);
inline SgDoWhileStmt * buildDoWhileStmt(SgStatement* body, SgExpression *  condition) {
  return buildDoWhileStmt(body, buildExprStatement(condition));
}
SgDoWhileStmt * buildDoWhileStmt_nfi(SgStatement *  body, SgStatement *condition);

//! Build pragma declaration, handle SgPragma and defining/nondefining pointers internally
ROSE_DLL_API SgPragmaDeclaration * buildPragmaDeclaration(const std::string & name, SgScopeStatement* scope=NULL);
SgPragmaDeclaration * buildPragmaDeclaration_nfi(const std::string & name, SgScopeStatement* scope);

//!Build SgPragma
ROSE_DLL_API SgPragma* buildPragma(const std::string & name);

//! Build an empty declaration (useful for adding precission to comments and CPP handling under token-based unparsing).
ROSE_DLL_API SgEmptyDeclaration* buildEmptyDeclaration();

//! Build a SgBasicBlock, setting file info internally
ROSE_DLL_API SgBasicBlock * buildBasicBlock(SgStatement * stmt1 = NULL, SgStatement* stmt2 = NULL, SgStatement* stmt3 = NULL, SgStatement* stmt4 = NULL, SgStatement* stmt5 = NULL, SgStatement* stmt6 = NULL, SgStatement* stmt7 = NULL, SgStatement* stmt8 = NULL, SgStatement* stmt9 = NULL, SgStatement* stmt10 = NULL);
ROSE_DLL_API SgBasicBlock * buildBasicBlock_nfi();
SgBasicBlock * buildBasicBlock_nfi(const std::vector<SgStatement*>&);

//! Build an assignment statement from lefthand operand and right hand operand
ROSE_DLL_API SgExprStatement*
buildAssignStatement(SgExpression* lhs,SgExpression* rhs);

// DQ (8/16/2011): Generated a new version of this function to define consistant semantics.
//! This version does not recursively reset the file info as a transformation.
ROSE_DLL_API SgExprStatement* buildAssignStatement_ast_translate(SgExpression* lhs,SgExpression* rhs);

//! Build a break statement
ROSE_DLL_API SgBreakStmt* buildBreakStmt();
SgBreakStmt* buildBreakStmt_nfi();

//! Build a continue statement
ROSE_DLL_API SgContinueStmt* buildContinueStmt();
SgContinueStmt* buildContinueStmt_nfi();

//! Build a pass statement
ROSE_DLL_API SgPassStatement* buildPassStatement();
SgPassStatement* buildPassStatement_nfi();

//! Build a Assert statement
ROSE_DLL_API SgAssertStmt* buildAssertStmt(SgExpression* test);
ROSE_DLL_API SgAssertStmt* buildAssertStmt(SgExpression *test, SgExpression *exceptionArgument);
SgAssertStmt* buildAssertStmt_nfi(SgExpression* test);

//! Build a yield statement
ROSE_DLL_API SgYieldExpression* buildYieldExpression(SgExpression* value);
SgYieldExpression* buildYieldExpression_nfi(SgExpression* value);

//! Build a key-datum pair
ROSE_DLL_API SgKeyDatumPair* buildKeyDatumPair    (SgExpression* key, SgExpression* datum);
SgKeyDatumPair* buildKeyDatumPair_nfi(SgExpression* key, SgExpression* datum);

//! Build a list of key-datum pairs
ROSE_DLL_API SgDictionaryExp* buildDictionaryExp    (std::vector<SgKeyDatumPair*> pairs);
SgDictionaryExp* buildDictionaryExp_nfi(std::vector<SgKeyDatumPair*> pairs);

//! Build an Actual Argument Expression
ROSE_DLL_API SgActualArgumentExpression* buildActualArgumentExpression(SgName arg_name, SgExpression* arg);
SgActualArgumentExpression* buildActualArgumentExpression_nfi(SgName arg_name, SgExpression* arg);

//! Build a delete statement
ROSE_DLL_API SgDeleteExp* buildDeleteExp(SgExpression *target, bool is_array = false, bool need_global_specifier = false, SgFunctionDeclaration *deleteOperatorDeclaration = NULL);
SgDeleteExp* buildDeleteExp_nfi(SgExpression *target, bool is_array = false, bool need_global_specifier = false, SgFunctionDeclaration *deleteOperatorDeclaration = NULL);

//! Build a class definition scope statement
// SgClassDefinition* buildClassDefinition(SgClassDeclaration *d = NULL);
ROSE_DLL_API SgClassDefinition* buildClassDefinition(SgClassDeclaration *d = NULL, bool buildTemplateInstantiation = false);

//! Build a class definition scope statement
// SgClassDefinition* buildClassDefinition_nfi(SgClassDeclaration *d = NULL);
SgClassDefinition* buildClassDefinition_nfi(SgClassDeclaration *d = NULL, bool buildTemplateInstantiation = false);

// DQ (11/19/2011): Added more template declaration support.
//! Build a template class definition statement
SgTemplateClassDefinition* buildTemplateClassDefinition(SgTemplateClassDeclaration *d = NULL );

//! Build a structure first nondefining declaration, without file info
// DQ (6/6/2012): Added support to get the template arguments into place before computing the type.
// SgClassDeclaration* buildNondefiningClassDeclaration_nfi(const SgName& name, SgClassDeclaration::class_types kind, SgScopeStatement* scope);
// SgClassDeclaration* buildNondefiningClassDeclaration_nfi(const SgName& name, SgClassDeclaration::class_types kind, SgScopeStatement* scope, bool buildTemplateInstantiation = false);
ROSE_DLL_API SgClassDeclaration* buildNondefiningClassDeclaration_nfi(const SgName& name, SgClassDeclaration::class_types kind, SgScopeStatement* scope, bool buildTemplateInstantiation, SgTemplateArgumentPtrList* templateArgumentsList);

// DQ (8/11/2013): We need to hand in both the SgTemplateParameterPtrList and the SgTemplateArgumentPtrList because class templates can be partially specialized.
// DQ (11/29/2011): Adding template declaration support to the AST.
// SgTemplateClassDeclaration* buildNondefiningTemplateClassDeclaration_nfi(const SgName& name, SgClassDeclaration::class_types kind, SgScopeStatement* scope);
ROSE_DLL_API SgTemplateClassDeclaration* buildNondefiningTemplateClassDeclaration_nfi(const SgName& name, SgClassDeclaration::class_types kind, SgScopeStatement* scope, SgTemplateParameterPtrList* templateParameterList, SgTemplateArgumentPtrList* templateSpecializationArgumentList);

//! DQ (11/7/2009): Added functions to build C++ class.
ROSE_DLL_API SgClassDeclaration* buildNondefiningClassDeclaration ( SgName name, SgScopeStatement* scope );
ROSE_DLL_API SgClassDeclaration* buildDefiningClassDeclaration    ( SgName name, SgScopeStatement* scope );

//! DQ (11/7/2009): Added function to build C++ class (builds both the non-defining and defining declarations; in that order).
ROSE_DLL_API SgClassDeclaration* buildClassDeclaration    ( SgName name, SgScopeStatement* scope );

//! Build an enum first nondefining declaration, without file info
ROSE_DLL_API SgEnumDeclaration* buildNondefiningEnumDeclaration_nfi(const SgName& name, SgScopeStatement* scope);

//! Build a structure, It is also a declaration statement in SAGE III
ROSE_DLL_API SgClassDeclaration * buildStructDeclaration(const SgName& name, SgScopeStatement* scope=NULL);
ROSE_DLL_API SgClassDeclaration * buildStructDeclaration(const std::string& name, SgScopeStatement* scope=NULL);
ROSE_DLL_API SgClassDeclaration * buildStructDeclaration(const char* name, SgScopeStatement* scope=NULL);

//! Build a StmtDeclarationStmt
ROSE_DLL_API SgStmtDeclarationStatement* buildStmtDeclarationStatement(SgStatement* stmt);
ROSE_DLL_API SgStmtDeclarationStatement* buildStmtDeclarationStatement_nfi(SgStatement* stmt);

//! tps (09/02/2009) : Added support for building namespaces
ROSE_DLL_API SgNamespaceDeclarationStatement *  buildNamespaceDeclaration(const SgName& name, SgScopeStatement* scope=NULL);
ROSE_DLL_API SgNamespaceDeclarationStatement *  buildNamespaceDeclaration_nfi(const SgName& name, bool unnamednamespace, SgScopeStatement* scope );
ROSE_DLL_API SgNamespaceDefinitionStatement * buildNamespaceDefinition(SgNamespaceDeclarationStatement* d=NULL);

//! driscoll6 (7/20/11) : Support n-ary operators for python
ROSE_DLL_API SgNaryComparisonOp* buildNaryComparisonOp(SgExpression* lhs);
ROSE_DLL_API SgNaryComparisonOp* buildNaryComparisonOp_nfi(SgExpression* lhs);
ROSE_DLL_API SgNaryBooleanOp* buildNaryBooleanOp(SgExpression* lhs);
ROSE_DLL_API SgNaryBooleanOp* buildNaryBooleanOp_nfi(SgExpression* lhs);

ROSE_DLL_API SgStringConversion* buildStringConversion(SgExpression* exp);
ROSE_DLL_API SgStringConversion* buildStringConversion_nfi(SgExpression* exp);

// DQ (6/6/2012): Addeding support to include template arguments in the generated type (template argument must be provided as early as possible).
// DQ (1/24/2009): Added this "_nfi" function but refactored buildStructDeclaration to also use it (this needs to be done uniformally).
// SgClassDeclaration * buildClassDeclaration_nfi(const SgName& name, SgClassDeclaration::class_types kind, SgScopeStatement* scope, SgClassDeclaration* nonDefiningDecl, bool buildTemplateInstantiation = false);
// SgClassDeclaration * buildClassDeclaration_nfi(const SgName& name, SgClassDeclaration::class_types kind, SgScopeStatement* scope, SgClassDeclaration* nonDefiningDecl, bool buildTemplateInstantiation);
ROSE_DLL_API SgClassDeclaration* buildClassDeclaration_nfi(const SgName& name, SgClassDeclaration::class_types kind, SgScopeStatement* scope, SgClassDeclaration* nonDefiningDecl, bool buildTemplateInstantiation, SgTemplateArgumentPtrList* templateArgumentsList);

// DQ (8/11/2013): I think that the specification of both SgTemplateParameterPtrList and SgTemplateArgumentPtrList is redundant with the nonDefiningDecl (which is a required parameter).
// DQ (11/19/2011): Added to support template class declaration using EDG 4.x support (to support the template declarations directly in the AST).
// SgTemplateClassDeclaration* buildTemplateClassDeclaration_nfi(const SgName& name, SgClassDeclaration::class_types kind, SgScopeStatement* scope, SgTemplateClassDeclaration* nonDefiningDecl );
ROSE_DLL_API SgTemplateClassDeclaration* buildTemplateClassDeclaration_nfi(const SgName& name, SgClassDeclaration::class_types kind, SgScopeStatement* scope, SgTemplateClassDeclaration* nonDefiningDecl,
                                                                           SgTemplateParameterPtrList* templateParameterList, SgTemplateArgumentPtrList* templateSpecializationArgumentList );

//! Build an SgDerivedTypeStatement Fortran derived type declaration with a
//! class declaration and definition (creating both the defining and nondefining declarations as required).
ROSE_DLL_API SgDerivedTypeStatement * buildDerivedTypeStatement (const SgName& name, SgScopeStatement* scope=NULL);

//! Build a Jovial table declaration statement
ROSE_DLL_API SgJovialTableStatement * buildJovialTableStatement (const SgName& name, SgScopeStatement* scope=NULL);

//! Build a Jovial table type with required class definition and defining and nondefining declarations.
 ROSE_DLL_API SgJovialTableType * buildJovialTableType (const SgName& name, SgType* base_type, SgExprListExp* dim_info, SgScopeStatement* scope=NULL);

//! Build a generic class declaration statement (SgClassDeclaration or subclass) with a
//! class declaration and definition (creating both the defining and nondefining declarations as required.
template <class DeclClass> ROSE_DLL_API
DeclClass * buildClassDeclarationStatement_nfi(const SgName& name, SgClassDeclaration::class_types kind,
                                               SgScopeStatement* scope=NULL, SgClassDeclaration* nonDefiningDecl=NULL);

//! Build an enum, It is also a declaration statement in SAGE III
ROSE_DLL_API SgEnumDeclaration * buildEnumDeclaration(const SgName& name, SgScopeStatement* scope=NULL);

//! Build an enum, It is also a declaration statement in SAGE III
ROSE_DLL_API SgEnumDeclaration * buildEnumDeclaration_nfi(const SgName& name, SgScopeStatement* scope=NULL);

//! Build a return statement
ROSE_DLL_API SgReturnStmt* buildReturnStmt(SgExpression* expression = NULL);
ROSE_DLL_API SgReturnStmt* buildReturnStmt_nfi(SgExpression* expression);

//! Build a NULL statement
ROSE_DLL_API SgNullStatement* buildNullStatement();
SgNullStatement* buildNullStatement_nfi();

//! Build Fortran attribute specification statement
ROSE_DLL_API SgAttributeSpecificationStatement * buildAttributeSpecificationStatement(SgAttributeSpecificationStatement::attribute_spec_enum kind);

//! Build Fortran include line
ROSE_DLL_API SgFortranIncludeLine* buildFortranIncludeLine(std::string filename);

//! Build a Fortran common block, possibly with a name
ROSE_DLL_API SgCommonBlockObject* buildCommonBlockObject(std::string name="", SgExprListExp* exp_list=NULL);

//! Build a Fortran Common statement
ROSE_DLL_API SgCommonBlock* buildCommonBlock(SgCommonBlockObject* first_block=NULL);

// driscoll6 (6/9/2011): Adding support for try stmts.
//! Build a catch statement.
ROSE_DLL_API SgCatchOptionStmt* buildCatchOptionStmt(SgVariableDeclaration* condition=NULL, SgStatement* body=NULL);

//! MH (6/10/2014): Added async support
ROSE_DLL_API SgAsyncStmt* buildAsyncStmt(SgBasicBlock *body);

//! MH (6/11/2014): Added finish support
ROSE_DLL_API SgFinishStmt* buildFinishStmt(SgBasicBlock *body);

//! MH (6/11/2014): Added at support
ROSE_DLL_API SgAtStmt* buildAtStmt(SgExpression *expression, SgBasicBlock *body);

// MH (11/12/2014): Added atomic support
ROSE_DLL_API SgAtomicStmt* buildAtomicStmt(SgBasicBlock *body);

ROSE_DLL_API SgWhenStmt* buildWhenStmt(SgExpression *expression, SgBasicBlock *body);

// MH (9/16/2014): Added at support
ROSE_DLL_API SgAtExp* buildAtExp(SgExpression *expression, SgBasicBlock *body);

// MH (11/7/2014): Added finish expression support
ROSE_DLL_API SgFinishExp* buildFinishExp(SgExpression *expression, SgBasicBlock *body);

ROSE_DLL_API SgHereExp* buildHereExpression();

ROSE_DLL_API SgDotDotExp* buildDotDotExp();


// driscoll6 (6/9/2011): Adding support for try stmts.
//! Build a try statement.
ROSE_DLL_API SgTryStmt* buildTryStmt(SgStatement* body,
                                     SgCatchOptionStmt* catch0=NULL,
                                     SgCatchOptionStmt* catch1=NULL,
                                     SgCatchOptionStmt* catch2=NULL,
                                     SgCatchOptionStmt* catch3=NULL,
                                     SgCatchOptionStmt* catch4=NULL);

// charles4 (9/16/2011): Adding support for try stmts.
//! Build a try statement.
//SgTryStmt* buildTryStmt(SgStatement *try_body, SgCatchStatementSeq *catches, SgStatement *finally_body = NULL);

// charles4 (9/16/2011): Adding support for try stmts.
//! Build a try statement.
ROSE_DLL_API SgTryStmt* buildTryStmt(SgBasicBlock *try_body, SgBasicBlock *finally_body = NULL);

// charles4 (9/16/2011): Adding support for Catch Blocks.
//! Build an initial sequence of Catch blocks containing 0 or 1 element.
ROSE_DLL_API SgCatchStatementSeq *buildCatchStatementSeq(SgCatchOptionStmt * = NULL);

// charles4 (8/25/2011): Adding support for Java Synchronized stmts.
//! Build a Java Synchronized statement.
ROSE_DLL_API SgJavaSynchronizedStatement *buildJavaSynchronizedStatement(SgExpression *, SgBasicBlock *);

// charles4 (8/25/2011): Adding support for Java Throw stmts.
//! Build a Java Throw statement.
ROSE_DLL_API SgJavaThrowStatement *buildJavaThrowStatement(SgThrowOp *);

// charles4 (8/25/2011): Adding support for Java Foreach stmts.
//! Build a Java Foreach statement.
// SgJavaForEachStatement *buildJavaForEachStatement(SgInitializedName * = NULL, SgExpression * = NULL, SgStatement * = NULL);
ROSE_DLL_API SgJavaForEachStatement *buildJavaForEachStatement(SgVariableDeclaration * = NULL, SgExpression * = NULL, SgStatement * = NULL);

// charles4 (8/25/2011): Adding support for Java Label stmts.
//! Build a Java Label statement.
ROSE_DLL_API SgJavaLabelStatement *buildJavaLabelStatement(const SgName &,  SgStatement * = NULL);

//! Build an exec statement
ROSE_DLL_API SgExecStatement* buildExecStatement(SgExpression* executable, SgExpression* globals = NULL, SgExpression* locals = NULL);
SgExecStatement* buildExecStatement_nfi(SgExpression* executable, SgExpression* globals = NULL, SgExpression* locals = NULL);

//! Build a python print statement
ROSE_DLL_API SgPythonPrintStmt* buildPythonPrintStmt(SgExpression* dest = NULL, SgExprListExp* values = NULL);
SgPythonPrintStmt* buildPythonPrintStmt_nfi(SgExpression* dest = NULL, SgExprListExp* values = NULL);

//! Build a python global statement
ROSE_DLL_API SgPythonGlobalStmt* buildPythonGlobalStmt(SgInitializedNamePtrList& names);
SgPythonGlobalStmt* buildPythonGlobalStmt_nfi(SgInitializedNamePtrList& names);

// DQ (4/30/2010): Added support for building asm statements.
//! Build a NULL statement
ROSE_DLL_API SgAsmStmt* buildAsmStatement(std::string s);
SgAsmStmt* buildAsmStatement_nfi(std::string s);

//! DQ (4/30/2010): Added support for building nop statement using asm statement
//! Building nop statement using asm statement
ROSE_DLL_API SgAsmStmt* buildMultibyteNopStatement( int n );

//! DQ (5/6/2013): Added build functions to support SgBaseClass construction.
ROSE_DLL_API SgBaseClass* buildBaseClass ( SgClassDeclaration* classDeclaration, SgClassDefinition* classDefinition, bool isVirtual, bool isDirect );

ROSE_DLL_API SgNonrealBaseClass* buildNonrealBaseClass ( SgNonrealDecl* classDeclaration, SgClassDefinition* classDefinition, bool isVirtual, bool isDirect );

// SgAccessModifier buildAccessModifier ( unsigned int access );

//! DQ (7/25/2014): Adding support for C11 static assertions.
ROSE_DLL_API SgStaticAssertionDeclaration* buildStaticAssertionDeclaration(SgExpression* condition, const SgName & string_literal);

//! DQ (8/17/2014): Adding support for Microsoft MSVC specific attributes.
ROSE_DLL_API SgMicrosoftAttributeDeclaration* buildMicrosoftAttributeDeclaration (const SgName & name);

//! Liao (9/18/2015): experimental support of building a statement from a string
ROSE_DLL_API SgStatement* buildStatementFromString(const std::string & stmt_str, SgScopeStatement* scope);

//@}

//--------------------------------------------------------------
//@{
/*! @name Builders for others
  \brief AST high level builders for others

*/
//! Build a SgFile node and attach it to SgProject
/*! The input file will be loaded if exists, or an empty one will be generated from scratch transparently. Output file name is used to specify the output file name of unparsing. The final SgFile will be inserted to project automatically. If not provided, a new SgProject will be generated internally. Using SgFile->get_project() to retrieve it in this case.
 */
ROSE_DLL_API SgFile* buildFile(const std::string& inputFileName,const std::string& outputFileName, SgProject* project=NULL);

//! Build a SgFile node and attach it to SgProject
/*! The file will be build with an empty global scope to support declarations being added.
 */
ROSE_DLL_API SgSourceFile* buildSourceFile(const std::string& outputFileName, SgProject* project=NULL);

//! Build a SgSourceFile node and attach it to SgProject
/*! The input file will be loaded if exists, or an empty one will be generated from scratch transparently. Output file name is used to specify the output file name of unparsing. The final SgFile will be inserted to project automatically. If not provided, a new SgProject will be generated internally. Using SgFile->get_project() to retrieve it in this case.
 */
ROSE_DLL_API SgSourceFile* buildSourceFile(const std::string& inputFileName, const std::string& outputFileName, SgProject* project);

// DQ (11/8/2019): Support function for the new file (to support changing the file names in the source position info objects of each AST subtree node.
//! Change the source file associated with the source position information in the AST.
// ROSE_DLL_API void fixupSourcePositionFileSpecification(SgNode* subtreeRoot, const std::string& newFileName);
ROSE_DLL_API void fixupSourcePositionFileSpecification(SgNode* subtreeRoot, const std::string& newFileName);

// DQ (11/10/2019): Support for sharing IR nodes when buildFile() is applied to an existing file.
//! Sharing IR nodes requires that the file id be added to the fileIDsToUnparse held in the Sg_File_Info object.
ROSE_DLL_API void fixupSharingSourcePosition(SgNode* subtreeRoot, int new_file_id);

//! Build and attach a comment, comment style is inferred from the language type of the target node if not provided. It is indeed a wrapper of SageInterface::attachComment().
ROSE_DLL_API PreprocessingInfo* buildComment(SgLocatedNode* target, const std::string & content,
               PreprocessingInfo::RelativePositionType position=PreprocessingInfo::before,
               PreprocessingInfo::DirectiveType dtype= PreprocessingInfo::CpreprocessorUnknownDeclaration);

//! Build and attach #define XX directives, pass "#define xxx xxx" as content.
ROSE_DLL_API PreprocessingInfo* buildCpreprocessorDefineDeclaration(SgLocatedNode* target,
                const std::string & content,
               PreprocessingInfo::RelativePositionType position=PreprocessingInfo::before);

#ifndef ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT
//! Build an abstract handle from a SgNode
ROSE_DLL_API AbstractHandle::abstract_handle * buildAbstractHandle(SgNode* n);
#endif

// 03/17/2014 PHL
// //! Build an equivalence statement from two expression operands
ROSE_DLL_API SgEquivalenceStatement*
buildEquivalenceStatement(SgExpression* lhs,SgExpression* rhs);


//! Fixup any AST moved from one file two another (references to symbols, types, etc.).
ROSE_DLL_API void fixupCopyOfAstFromSeparateFileInNewTargetAst(SgStatement *insertionPoint, bool insertionPointIsScope,
                                                               SgStatement *toInsert, SgStatement* original_before_copy);
ROSE_DLL_API void fixupCopyOfNodeFromSeparateFileInNewTargetAst(SgStatement* insertionPoint, bool insertionPointIsScope,
                                                                SgNode* node_copy, SgNode* node_original);
ROSE_DLL_API SgType* getTargetFileTypeSupport(SgType* snippet_type, SgScopeStatement* targetScope);
ROSE_DLL_API SgType* getTargetFileType(SgType* snippet_type, SgScopeStatement* targetScope);
ROSE_DLL_API SgSymbol* findAssociatedSymbolInTargetAST(SgDeclarationStatement* snippet_declaration, SgScopeStatement* targetScope);

//! Error checking the inserted snippet AST.
ROSE_DLL_API void errorCheckingTargetAST (SgNode* node_copy, SgNode* node_original, SgFile* targetFile, bool failOnWarning);

//-----------------------------------------------------------------------------
//#ifdef ROSE_BUILD_JAVA_LANGUAGE_SUPPORT
//-----------------------------------------------------------------------------
ROSE_DLL_API SgVarRefExp *buildJavaArrayLengthVarRefExp();
ROSE_DLL_API SgScopeStatement *buildScopeStatement(SgClassDefinition * = NULL);
ROSE_DLL_API SgJavaTypeExpression *buildJavaTypeExpression(SgType *);
ROSE_DLL_API SgJavaMarkerAnnotation *buildJavaMarkerAnnotation(SgType *);
ROSE_DLL_API SgJavaMemberValuePair *buildJavaMemberValuePair(const SgName &, SgExpression *);
ROSE_DLL_API SgJavaSingleMemberAnnotation *buildJavaSingleMemberAnnotation(SgType *, SgExpression *);
ROSE_DLL_API SgJavaNormalAnnotation *buildJavaNormalAnnotation(SgType *);
ROSE_DLL_API SgJavaNormalAnnotation *buildJavaNormalAnnotation(SgType *, std::list<SgJavaMemberValuePair *>&);
ROSE_DLL_API SgInitializedName *buildJavaFormalParameter(SgType *, const SgName &, bool is_var_args = false, bool is_final = false);

ROSE_DLL_API SgJavaPackageStatement *buildJavaPackageStatement(std::string);
ROSE_DLL_API SgJavaImportStatement *buildJavaImportStatement(std::string, bool);
ROSE_DLL_API SgClassDeclaration *buildJavaDefiningClassDeclaration(SgScopeStatement *, std::string, SgClassDeclaration::class_types kind = SgClassDeclaration::e_class);
ROSE_DLL_API SgSourceFile *buildJavaSourceFile(SgProject *, std::string, SgClassDefinition *, std::string);
ROSE_DLL_API SgArrayType *getUniqueJavaArrayType(SgType *, int);
ROSE_DLL_API SgJavaParameterizedType *getUniqueJavaParameterizedType(SgNamedType *, SgTemplateParameterPtrList *);
ROSE_DLL_API SgJavaQualifiedType *getUniqueJavaQualifiedType(SgClassDeclaration *, SgNamedType *, SgNamedType *);
ROSE_DLL_API SgJavaWildcardType *getUniqueJavaWildcardUnbound();
ROSE_DLL_API SgJavaWildcardType *getUniqueJavaWildcardExtends(SgType *);
ROSE_DLL_API SgJavaWildcardType *getUniqueJavaWildcardSuper(SgType *);

//@}


//----------------------------------------------------------
//@{
/*! @name Untyped IR Node Build Interfaces
    \brief  Build function for ROSE AST's in terms of Untyped IR nodes.

The ROSE Untyped IR nodes can be a starting place for defining the new language frontend, these IR nodes
address the interface from an external language parser and the construction of the ROSE Untyped AST.
Later iterations on the ROSE Untyped AST can be used to translate (or construct) a proper ROSE AST in
terms of non-untyped IR nodes.

 \todo define translation passes to construct non-untype IR nodes.
*/

/*! \brief build a concept of scope in the untyped AST.
*/
ROSE_DLL_API SgUntypedScope* buildUntypedScope();
ROSE_DLL_API SgUntypedScope* buildUntypedScope(SgUntypedDeclarationStatementList* declaration_list, SgUntypedStatementList* statement_list, SgUntypedFunctionDeclarationList* function_list);

ROSE_DLL_API SgUntypedGlobalScope*   buildUntypedGlobalScope  (SgUntypedDeclarationStatementList* declaration_list, SgUntypedStatementList* statement_list, SgUntypedFunctionDeclarationList* function_list);
ROSE_DLL_API SgUntypedFunctionScope* buildUntypedFunctionScope(SgUntypedDeclarationStatementList* declaration_list, SgUntypedStatementList* statement_list, SgUntypedFunctionDeclarationList* function_list);
ROSE_DLL_API SgUntypedModuleScope*   buildUntypedModuleScope  (SgUntypedDeclarationStatementList* declaration_list, SgUntypedStatementList* statement_list, SgUntypedFunctionDeclarationList* function_list);

ROSE_DLL_API SgUntypedBlockStatement* buildUntypedBlockStatement(std::string label_string, SgUntypedScope* scope=NULL);

ROSE_DLL_API SgUntypedFunctionDeclaration*      buildUntypedFunctionDeclaration(std::string name, SgUntypedInitializedNameList* parameters, SgUntypedType* type, SgUntypedFunctionScope* scope, SgUntypedNamedStatement* end_statement);
ROSE_DLL_API SgUntypedProgramHeaderDeclaration* buildUntypedProgramHeaderDeclaration(std::string name, SgUntypedInitializedNameList* parameters, SgUntypedType* type, SgUntypedFunctionScope* scope, SgUntypedNamedStatement* end_statement);
ROSE_DLL_API SgUntypedSubroutineDeclaration*    buildUntypedSubroutineDeclaration(std::string name, SgUntypedInitializedNameList* parameters, SgUntypedType* type, SgUntypedFunctionScope* scope, SgUntypedNamedStatement* end_statement);

ROSE_DLL_API SgUntypedFile* buildUntypedFile(SgUntypedGlobalScope* scope);


ROSE_DLL_API SgUntypedIfStatement* buildUntypedIfStatement(std::string label, SgUntypedExpression* conditional,
                                                           SgUntypedStatement* true_body, SgUntypedStatement* false_body);


//@}



 //----------------------build unary expressions----------------------
//!  Template function to build a unary expression of type T. Instantiated functions include:buildAddressOfOp(),buildBitComplementOp(),buildBitComplementOp(),buildMinusOp(),buildNotOp(),buildPointerDerefExp(),buildUnaryAddOp(),buildMinusMinusOp(),buildPlusPlusOp().  They are also used for the unary vararg operators (which are not technically unary operators).
/*! The instantiated functions' prototypes are not shown since they are expanded using macros.
 * Doxygen is not smart enough to handle macro expansion.
 */

template <class T>
  T* buildUnaryExpression(SgExpression* operand) {
  SgExpression* myoperand=operand;
  T* result = new T(myoperand, NULL);
  ROSE_ASSERT(result);
  if (myoperand!=NULL) {
    myoperand->set_parent(result);
    // set lvalue, it asserts operand!=NULL
    markLhsValues(result);
  }
  SageInterface::setOneSourcePositionForTransformation(result);
  return result;
 }

//!  Template function to build a unary expression of type T with no file info. Instantiated functions include:buildAddressOfOp(),buildBitComplementOp(),buildBitComplementOp(),buildMinusOp(),buildNotOp(),buildPointerDerefExp(),buildUnaryAddOp(),buildMinusMinusOp(),buildPlusPlusOp().  They are also used for the unary vararg operators (which are not technically unary operators).
/*! The instantiated functions' prototypes are not shown since they are expanded using macros.
 * Doxygen is not smart enough to handle macro expansion.
 */
template <class T>
T* buildUnaryExpression_nfi(SgExpression* operand) {
  SgExpression* myoperand = operand;
  T* result = new T(myoperand, NULL);
  ROSE_ASSERT(result);

  if (myoperand != NULL) {
    myoperand->set_parent(result);
    // set lvalue, it asserts operand!=NULL
    markLhsValues(result);
  }
  SageInterface::setSourcePosition(result);

  result->set_need_paren(false);
  return result;
 }

//---------------------binary expressions-----------------------

//! Template function to build a binary expression of type T, taking care of parent pointers, file info, lvalue, etc. Available instances include: buildAddOp(), buildAndAssignOp(), buildAndOp(), buildArrowExp(),buildArrowStarOp(), buildAssignOp(),buildBitAndOp(),buildBitOrOp(),buildBitXorOp(),buildCommaOpExp(), buildConcatenationOp(),buildDivAssignOp(),buildDivideOp(),buildDotExp(),buildEqualityOp(),buildExponentiationOp(),buildGreaterOrEqualOp(),buildGreaterThanOp(),buildIntegerDivideOp(),buildIorAssignOp(),buildLessOrEqualOp(),buildLessThanOp(),buildLshiftAssignOp(),buildLshiftOp(),buildMinusAssignOp(),buildModAssignOp(),buildModOp(),buildMultAssignOp(),buildMultiplyOp(),buildNotEqualOp(),buildOrOp(),buildPlusAssignOp(),buildPntrArrRefExp(),buildRshiftAssignOp(),buildRshiftOp(),buildScopeOp(),buildSubtractOp()buildXorAssignOp()
/*! The instantiated functions' prototypes are not shown since they are expanded using macros.
 * Doxygen is not smart enough to handle macro expansion.
 */
 template <class T>
   T* buildBinaryExpression(SgExpression* lhs, SgExpression* rhs) {
   SgExpression* mylhs, *myrhs;
   mylhs = lhs;
   myrhs = rhs;
   T* result = new T(mylhs,myrhs, NULL);
   ROSE_ASSERT(result);
   if (mylhs!=NULL) {
     mylhs->set_parent(result);
     // set lvalue
     markLhsValues(result);
   }
   if (myrhs!=NULL) myrhs->set_parent(result);
   SageInterface::setOneSourcePositionForTransformation(result);
   return result;
 }

//! Template function to build a binary expression of type T, taking care of parent pointers, but without file-info. Available instances include: buildAddOp(), buildAndAssignOp(), buildAndOp(), buildArrowExp(),buildArrowStarOp(), buildAssignOp(),buildBitAndOp(),buildBitOrOp(),buildBitXorOp(),buildCommaOpExp(), buildConcatenationOp(),buildDivAssignOp(),buildDivideOp(),buildDotExp(),buildEqualityOp(),buildExponentiationOp(),buildGreaterOrEqualOp(),buildGreaterThanOp(),buildIntegerDivideOp(),buildIorAssignOp(),buildLessOrEqualOp(),buildLessThanOp(),buildLshiftAssignOp(),buildLshiftOp(),buildMinusAssignOp(),buildModAssignOp(),buildModOp(),buildMultAssignOp(),buildMultiplyOp(),buildNotEqualOp(),buildOrOp(),buildPlusAssignOp(),buildPntrArrRefExp(),buildRshiftAssignOp(),buildRshiftOp(),buildScopeOp(),buildSubtractOp()buildXorAssignOp()
/*! The instantiated functions' prototypes are not shown since they are expanded using macros.
 * Doxygen is not smart enough to handle macro expansion.
 */
 template <class T>
   T* buildBinaryExpression_nfi(SgExpression* lhs, SgExpression* rhs) {
   SgExpression* mylhs, *myrhs;
   mylhs = lhs;
   myrhs = rhs;
   T* result = new T(mylhs,myrhs, NULL);
   ROSE_ASSERT(result);
   if (mylhs!=NULL)  {
    mylhs->set_parent(result);
    // set lvalue
    markLhsValues(result);
   }
   if (myrhs!=NULL) myrhs->set_parent(result);
   SageInterface::setSourcePosition(result);
   result->set_need_paren(false);

   return result;
 }

} // end of namespace

namespace Rose {
    namespace Frontend {
        namespace Java {

            extern ROSE_DLL_API SgClassDefinition *javaLangPackageDefinition;
            extern ROSE_DLL_API SgClassType *ObjectClassType;
            extern ROSE_DLL_API SgClassType *StringClassType;
            extern ROSE_DLL_API SgClassType *ClassClassType;
            extern ROSE_DLL_API SgVariableSymbol *lengthSymbol;

        }// ::Rose::frontend::java
    }// ::Rose::frontend
}// ::Rose
//-----------------------------------------------------------------------------
//#endif // ROSE_BUILD_JAVA_LANGUAGE_SUPPORT
//-----------------------------------------------------------------------------

#endif //ROSE_SAGE_BUILDER_INTERFACE
