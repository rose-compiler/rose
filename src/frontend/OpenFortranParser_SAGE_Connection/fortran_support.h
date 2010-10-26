
#ifndef ROSE_FORTRAN_SUPPORT
#define ROSE_FORTRAN_SUPPORT

extern SgSourceFile* OpenFortranParser_globalFilePointer;


#ifdef __cplusplus
extern "C" {
#endif

#include "ActionEnums.h"
#include "token.h"
#include "FortranParserAction.h"

#ifdef __cplusplus
} /* End extern C. */
#endif

// Include the module specific header file here.
#include "FortranModuleInfo.h"

// Control output from Fortran parser
#define DEBUG_FORTRAN_SUPPORT true
#define DEBUG_RULE_COMMENT_LEVEL 1
#define DEBUG_COMMENT_LEVEL 2

// DQ (2/18/2008): I think this might be fixed now!
// DQ (12/16/2007): This is associated with a bug that cause lables for the computed goto to be processed twice with R313
#define OFP_LABEL_BUG false

// We make heavy use of ROSE IR nodes and support for build the AST from actions 
// that will be specified by the LANL Fortran 2003 parser.  This is header file 
// for ROSE.
// #include "rose.h"

// This header file contains the Fortran parser actions (functions) that will be 
// called by the parser to build the AST.
// #include "IFortranParserAction.h"

// define ROSE_IMPLICIT_PROGRAM_HEADER_NAME "rose_implicit_program_header"
// define ROSE_IMPLICIT_FORTRAN_PROGRAM_NAME "rose_fortran_main"

// Global stack of scopes
extern std::list<SgScopeStatement*> astScopeStack;

// Global stack of expressions 
extern std::list<SgExpression*> astExpressionStack;

// Simplifying type for the setSourcePosition() functions
// typedef std::vector<SgToken*> TokenListType;
typedef std::vector<Token_t*> TokenListType;

// Global stack of IR nodes
// extern std::vector<SgNode*> astNodeStack;
extern std::list<SgNode*> astNodeStack;

// Global token stack (used to hold state between the <fortran_rule>__begin() and the <fortran_rule>()
extern TokenListType globalTokenList;

// Intend stack used to holding intend specifiers
extern std::list<int> astIntentSpecStack;

// Attribute spec for holding attributes
extern std::list<int> astAttributeSpecStack;

// Global stack of lists of SgInitializedName containers nodes
// extern std::list<SgInitializedNamePtrListPtr> astInitializedNameListStack;
// typedef std::string AstNameType;
// typedef struct { std::string name; Token_t* token } AstNameType;
typedef Token_t AstNameType;
typedef std::list< AstNameType* > AstNameListType;
extern AstNameListType astNameStack;

// typedef std::list< AstNameListType* > AstNameListStackType;
// extern AstNameListStackType astNameListStack;

// Global stack of SgDeclarationStatement IR nodes
// extern std::vector<SgDeclarationStatement*> astDeclarationStatementStack;
extern std::list<SgDeclarationStatement*> astDeclarationStatementStack;

// Global stack of SgType IR nodes
extern std::list<SgType*> astTypeStack;

// DQ (12/8/2007): Global stack of SgType IR nodes used to hold the base type seperately from the
// the constructed types build from the base type.  This is designed to handle the case of 
// "integer i(5),j" and "character*100 k,l" (see test2007_148.f)
extern std::list<SgType*> astBaseTypeStack;

// DQ (4/4/2008): I think that type initialization shows that we can't have a separate astInitializerStack
// See comment in fortran_support.C file.  For now use a macro to test this idea.
// Global stack of expressions used for initialization of variables in declarations.
// extern std::list<SgExpression*> astInitializerStack;

// DQ (11/30/2007): Function attributes are held as tokens and not not defined as integer value codes (like other attributes)
extern AstNameListType astFunctionAttributeStack;

// Global stack for type kind expressions (should generally only be depth == 1)
extern std::list<SgExpression*> astTypeKindStack;

// Global stack for type parameters (should generally only be depth == 1)
extern std::list<SgExpression*> astTypeParameterStack;

// Global stack for label symbols
extern std::list<SgLabelSymbol*> astLabelSymbolStack;

// Global stack for SgIfStmt objects (allows scopes pushed onto stack to be clean off back to the initial SgIfStmt)
extern std::list<SgIfStmt*> astIfStatementStack;

// DQ (11/30/2007): Actual arguments have associated names which have to be recorded on to a separate stack.
// test2007_162.h demonstrates this problems (and test2007_184.f)
extern AstNameListType astActualArgumentNameStack;

// DQ (10/1/2008): To simplify the handling of interfaces and the many functions 
// and function prototypes of function not defined in the interface we need attach 
// declarations and names to the SgInterfaceStatement as they are seen.  Since this 
// is nt always just the last statement, it is easier to support this using a stack.
extern std::list<SgInterfaceStatement*> astInterfaceStack;

// DQ (2/18/2008): This is the support for the Frotran include stack.
// This is specific to the Fortran include mechanism, not the CPP include 
// mechanism. Though at some point a unified approach might be required.
extern std::vector<std::string> astIncludeStack;



// ***********************************************
//              Function Prototypes
// ***********************************************

std::string getCurrentFilename();

void setSourcePosition  ( SgLocatedNode* locatedNode );
void setSourcePositionCompilerGenerated( SgLocatedNode* locatedNode );

void setSourcePosition  ( SgInitializedName* initializedName );
void setSourcePosition  ( SgLocatedNode* locatedNode, const TokenListType & tokenList );
void setSourcePosition  ( SgInitializedName* initializedName, const TokenListType & tokenList );

void setSourcePosition  ( SgLocatedNode* locatedNode, Token_t* token );
void setSourcePosition  ( SgInitializedName* initializedName, Token_t* token );

// DQ (10/6/2008): the need for this is eliminated by moving some nodes that have source 
// position from SgSupport to teh new SgLocatedNodeSupport subtree of SgLocatedNode.
// void setSourcePosition  ( SgRenamePair* namePair, Token_t* token );
// void setSourcePosition  ( SgSupport* namePair, Token_t* token );

void setOperatorSourcePosition  ( SgExpression* expr, Token_t* token );

void resetSourcePosition( SgLocatedNode* locatedNode, const TokenListType & tokenList );
void resetSourcePosition( SgLocatedNode* targetLocatedNode, const SgLocatedNode* sourceLocatedNode );

// DQ (10/10/2010): This function is added to support resetting the end 
// of the blocks (required to get comments woven into the AST properly).
void resetEndingSourcePosition( SgLocatedNode* targetLocatedNode, Token_t* token );
void resetEndingSourcePosition( SgLocatedNode* targetLocatedNode, SgStatement* sourceStatement );
void resetEndingSourcePosition( SgLocatedNode* targetLocatedNode, int newLineNumber );

// Function to create SgType IR nodes from an Open Fortran Parser type code.
SgType* createType(int typeCode);
SgType* generateImplicitType( std::string name );
SgType* generateIntrinsicFunctionReturnType( std::string s );
bool isIntrinsicFunctionReturningNonmatchingType( std::string s);

SgScopeStatement* getTopOfScopeStack();

// SgVariableDeclaration* buildVariableDeclaration (Token_t * label, bool buildingImplicitVariable = false);
   SgVariableDeclaration* buildVariableDeclaration (Token_t * label, bool buildingImplicitVariable );

// bool matchAgainstImplicitFunctionList( std::string s );
bool matchAgainstIntrinsicFunctionList( std::string s );

SgExpression* createUnaryOperator  ( SgExpression* exp, std::string name, bool is_user_defined_operator );
SgExpression* createBinaryOperator ( SgExpression* lhs, SgExpression* rhs, std::string name, bool is_user_defined_operator );

void build_implicit_program_statement_if_required();

void outputStateSupport( const std::string & s, int fieldWidth );

void outputState( const std::string label );

SgScopeStatement* getTopOfScopeStack();

SgType* getTopOfTypeStack();

AstNameType* getTopOfNameStack();

SgExpression* getTopOfExpressionStack();

SgFunctionDefinition* getFunctionDefinitionFromScopeStack();

void setStatementNumericLabel    (SgStatement* stmt, Token_t* label);
void setStatementElseNumericLabel(SgStatement* stmt, Token_t* label);
void setStatementEndNumericLabel (SgStatement* stmt, Token_t* label);
void setStatementStringLabel     (SgStatement* stmt, Token_t* label);
void setStatementNumericLabelUsingStack(SgStatement* statement);
SgExpression* buildLabelRefExp(SgExpression* expression);

// SgLabelSymbol* buildNumericLabelSymbol(SgStatement* stmt, Token_t* label);
SgLabelSymbol* buildNumericLabelSymbol(Token_t* label);
SgLabelSymbol* buildNumericLabelSymbolAndAssociateWithStatement(SgStatement* stmt, Token_t* label);

void trace_back_through_parent_scopes_searching_for_module (const SgName & moduleName, SgScopeStatement* currentScope, SgClassSymbol* & moduleSymbol );

SgVariableSymbol* trace_back_through_parent_scopes_lookup_variable_symbol(const SgName & variableName, SgScopeStatement* currentScope );

// DQ (4/30/2008): Modified to handle derived types
// void trace_back_through_parent_scopes_lookup_variable_symbol_but_do_not_build_variable(const SgName & variableName, SgScopeStatement* currentScope, SgVariableSymbol* & variableSymbol, SgFunctionSymbol* & functionSymbol );
void trace_back_through_parent_scopes_lookup_variable_symbol_but_do_not_build_variable(const SgName & variableName, SgScopeStatement* currentScope, SgVariableSymbol* & variableSymbol, SgFunctionSymbol* & functionSymbol, SgClassSymbol* & classSymbol);

SgClassSymbol* trace_back_through_parent_scopes_lookup_derived_type_symbol(const SgName & derivedTypeName, SgScopeStatement* currentScope );

SgFunctionSymbol* trace_back_through_parent_scopes_lookup_function_symbol(const SgName & functionName, SgScopeStatement* currentScope );

SgDerivedTypeStatement* buildDerivedTypeStatementAndDefinition (std::string name, SgScopeStatement* scope);
SgModuleStatement* buildModuleStatementAndDefinition (std::string name, SgScopeStatement* scope);

void initialize_global_scope_if_required();

// void build_implicit_program_statement_if_required();

// bool matchAgainstImplicitFunctionList( std::string s );

SgFunctionType* generateImplicitFunctionType( std::string functionName);

extern void buildAttributeSpecificationStatement ( SgAttributeSpecificationStatement::attribute_spec_enum kind, Token_t *label, Token_t *sourcePositionToken );

// DQ (8/28/2010): This is now generalized to support any SgDeclarationStatement (fix for test2010_34.f90; required for type declarations)
// void setDeclarationAttributeSpec ( SgVariableDeclaration* variableDeclaration, int astAttributeSpec );
void setDeclarationAttributeSpec ( SgDeclarationStatement* variableDeclaration, int astAttributeSpec );

SgArrayType* convertTypeOnStackToArrayType ( int count );

void processBindingAttribute( SgDeclarationStatement* declaration);

void processBindingAttributeSupport( std::string & bind_language, std::string & binding_label);

void processFunctionPrefix( SgFunctionDeclaration* functionDeclaration );

void generateFunctionCall( Token_t * nameToken );

SgFunctionRefExp* generateFunctionRefExp( Token_t* nameToken );

void convertVariableSymbolToFunctionCallExp( SgVariableSymbol* variableSymbol, Token_t* nameToken);
void convertExpressionOnStackToFunctionCallExp();

bool matchingName ( std::string x, std::string y );

void buildProcedureSupport(SgProcedureHeaderStatement* procedureDeclaration, bool hasDummyArgList);

// Both R834 and R838 require the same code (factored here)
void markDoLoopAsUsingEndDo();

SgExpression* buildSubscriptExpression ( bool hasLowerBound, bool hasUpperBound, bool hasStride, bool isAmbiguous );

bool isImplicitNoneScope();

bool isPubliclyAccessible( SgSymbol* symbol );

// DQ (11/12/2008): Used to convert module names to lower case.
std::string convertToLowerCase( const std::string & inputString );

// DQ (1/27/2009): Refactored code so that I can better support test2009_13.f
// (multiple levels of include files).
void cleanupTypeStackAfterDeclaration();

// DQ (1/27/2009): Refactored code so that I can better support test2009_13.f
// (multiple levels of include files).
void buildVariableDeclarationAndCleanupTypeStack( Token_t * label );

// DQ (1/28/2009): Added fix to avoid output of SgFortranInclude IR nodes when we switch
// file streams to read a ROSE generated *.rmod file.
bool isARoseModuleFile( std::string filename );

void generateAssignmentStatement( Token_t* label, bool isPointerAssignment );

void convertBaseTypeOnStackToPointer();

SgVariableSymbol* add_external_team_decl( std::string );

//! Fixes up function symbols to be in the correct scope when the function call appears before the function declaration.
void fixupModuleScope( SgClassDefinition* moduleScope );

//! Build the module symbols required to support intrinsic modules.
SgClassSymbol* buildIntrinsicModule ( const std::string & name );

// endif for ROSE_FORTRAN_SUPPORT
#endif


