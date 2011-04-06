#ifndef ROSE_JAVA_SUPPORT
#define ROSE_JAVA_SUPPORT

// This is used for both Fortran and Java support to point to the current SgSourceFile.
extern SgSourceFile* OpenFortranParser_globalFilePointer;


#ifdef __cplusplus
extern "C" {
#endif

#if 0
// DQ (1/14/2011): Later I would like to provide some similar support for source position inforamtion.
#include "ActionEnums.h"
#include "token.h"
#include "JavaParserAction.h"
#endif

#ifdef __cplusplus
} /* End extern C. */
#endif


// Control output from Fortran parser
#define DEBUG_JAVA_SUPPORT true
#define DEBUG_RULE_COMMENT_LEVEL 1
#define DEBUG_COMMENT_LEVEL 2

// Global stack of scopes
extern std::list<SgScopeStatement*> astJavaScopeStack;

// Global stack of expressions 
extern std::list<SgExpression*> astJavaExpressionStack;

// Global stack of types
extern std::list<SgType*> astJavaTypeStack;

// Global stack of statements
extern std::list<SgStatement*> astJavaStatementStack;

// Simplifying type for the setSourcePosition() functions
// typedef std::vector<Token_t*> TokenListType;

// Global stack of IR nodes
extern std::list<SgNode*> astJavaNodeStack;

// Attribute spec for holding attributes
// extern std::list<int> astAttributeSpecStack;

// Global list of implicit classes
extern std::list<SgName> astJavaImplicitClassList;




SgGlobal* getGlobalScope();


// Function used by SgType::getCurrentScope()
bool emptyJavaStateStack();

void setJavaSourcePosition  ( SgLocatedNode* locatedNode );

void outputJavaState( const std::string label );

std::string convertJavaStringToCxxString  (JNIEnv *env, const jstring & java_string);
int         convertJavaIntegerToCxxInteger(JNIEnv *env, const jint    & java_integer);

// Specify the SgClassDefinition explicitly so that implicit classes are simpler to build.
// SgMemberFunctionDeclaration* buildSimpleMemberFunction(const SgName & name);
// SgMemberFunctionDeclaration* buildSimpleMemberFunction(const SgName & name, SgClassDefinition* classDefinition);

// DQ (3/25/2011): These will replace buildSimpleMemberFunction shortly.
SgMemberFunctionDeclaration* buildNonDefiningMemberFunction(const SgName & inputName, SgClassDefinition* classDefinition);
SgMemberFunctionDeclaration* buildDefiningMemberFunction   (const SgName & inputName, SgClassDefinition* classDefinition);

// Build a simple class in the current scope and set the scope to be the class definition.
void buildClass (const SgName & className);
void buildImplicitClass (const SgName & className);
void buildClassSupport (const SgName & className, bool implicitClass);

SgVariableDeclaration* buildSimpleVariableDeclaration(const SgName & name);

std::list<SgName> generateQualifierList (const SgName & classNameWithQualification);
SgName stripQualifiers (const SgName & classNameWithQualification);

// It might be that this function should take a "const SgName &" instead of a "std::string".
SgClassSymbol* lookupSymbolFromQualifiedName(std::string className);


// endif for ROSE_JAVA_SUPPORT
#endif
