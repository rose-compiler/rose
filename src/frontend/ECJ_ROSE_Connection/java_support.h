#ifndef ROSE_JAVA_SUPPORT
#define ROSE_JAVA_SUPPORT

// This is used for both Fortran and Java support to point to the current SgSourceFile.
extern SgSourceFile* OpenFortranParser_globalFilePointer;


#ifdef __cplusplus
extern "C" {
#endif

#if 0
// Later I would like to provide some similar support for source position inforamtion.
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

// Simplifying type for the setSourcePosition() functions
// typedef std::vector<Token_t*> TokenListType;

// Global stack of IR nodes
extern std::list<SgNode*> astJavaNodeStack;

// Attribute spec for holding attributes
// extern std::list<int> astAttributeSpecStack;






// Function used by SgType::getCurrentScope()
bool emptyJavaStateStack();


std::string convertJavaStringToCxxString(JNIEnv *env, const jstring & java_string);

SgMemberFunctionDeclaration* buildSimpleMemberFunction(const SgName & name);

// Build a simple class in the current scope and set the scope to be the class definition.
void buildClass (const SgName & className);

SgVariableDeclaration* buildSimpleVariableDeclaration(const SgName & name);


// endif for ROSE_JAVA_SUPPORT
#endif
