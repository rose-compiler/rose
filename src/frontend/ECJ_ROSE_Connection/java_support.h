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

#include "jni_JavaSourceCodePosition.h"
#include "token.h"
#include "VisitorContext.h"

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

// Global stack of SgInitializedName IR nodes (used for processing function parameters)
extern std::list<SgInitializedName*> astJavaInitializedNameStack;

// Global stack of source code positions. The advantage of a stack is that we could 
// always reference the top of the stack, and monitor the depth of the stack, and make
// sure that we never deleted the last entry in the stack until the end of the program.
extern std::list<JavaSourceCodePosition*> astJavaSourceCodePositionStack;


SgGlobal* getGlobalScope();

// Function used by SgType::getCurrentScope()
bool emptyJavaStateStack();

// *********************************************
// Support for source code position???
// *********************************************
VisitorContext * getCurrentContext();
void pushContextStack(VisitorContext * ctx);
VisitorContext * popContextStack();

template <class VisitorContextType>
VisitorContextType * popContextStack_T() {
        VisitorContext * ctx_ = popContextStack();
        ROSE_ASSERT ((dynamic_cast<VisitorContextType*>(ctx_) != NULL) && "ERROR: Should have context on stack");
        VisitorContextType * ctx = (VisitorContextType*) ctx_;
        return ctx;
}

bool isStatementContext(VisitorContext * ctx);

std::list<SgStatement*> pop_from_stack_and_reverse(std::list<SgStatement*> &l, int nb_pop);

/* Create a token from a JavaToken jni object. Also converts JavaSourceCodeInformation to C */
Token_t * create_token(JNIEnv * env, jobject jToken);

void pushAndSetSourceCodePosition(JavaSourceCodePosition * pos, SgLocatedNode * sgnode);

// Duplicated setJavaSourcePosition signature.
// Not sure why but JNI wasn't happy if posInfo was assigned to a default value
void setJavaSourcePosition( SgLocatedNode* locatedNode);
void setJavaSourcePosition( SgLocatedNode* locatedNode, JavaSourceCodePosition * posInfo);

// DQ (8/16/2011): Added support using the jToken object.
void setJavaSourcePosition( SgLocatedNode* locatedNode, JNIEnv *env, jobject jToken);

// DQ (8/16/2011): Added support for marking nodes as compiler generated (implicit in Java).
void setJavaCompilerGenerated( SgLocatedNode* locatedNode );
void setJavaSourcePositionUnavailableInFrontend( SgLocatedNode* locatedNode );

//! This is how Java implicit classes are marked so that they can be avoided in output.
void setJavaFrontendSpecific( SgLocatedNode* locatedNode );


// *********************************************


// Debug function to inspect the stacks used to support AST translation.
void outputJavaState( const std::string label );

std::string convertJavaStringToCxxString  (JNIEnv *env, const jstring & java_string);
int         convertJavaIntegerToCxxInteger(JNIEnv *env, const jint    & java_integer);
bool        convertJavaBooleanToCxxBoolean(JNIEnv *env, const jboolean & java_boolean);

// Specify the SgClassDefinition explicitly so that implicit classes are simpler to build.
// SgMemberFunctionDeclaration* buildSimpleMemberFunction(const SgName & name);
// SgMemberFunctionDeclaration* buildSimpleMemberFunction(const SgName & name, SgClassDefinition* classDefinition);

// DQ (3/25/2011): These will replace buildSimpleMemberFunction shortly.
SgMemberFunctionDeclaration* buildNonDefiningMemberFunction(const SgName & inputName, SgClassDefinition* classDefinition);
SgMemberFunctionDeclaration* buildDefiningMemberFunction   (const SgName & inputName, SgClassDefinition* classDefinition);

// Build a simple class in the current scope and set the scope to be the class definition.
void buildClass (const SgName & className, Token_t* token);
void buildImplicitClass (const SgName & className);
void buildClassSupport (const SgName & className, bool implicitClass, Token_t* token);

SgVariableDeclaration* buildSimpleVariableDeclaration(const SgName & name);

std::list<SgName> generateQualifierList (const SgName & classNameWithQualification);
SgName stripQualifiers (const SgName & classNameWithQualification);

// It might be that this function should take a "const SgName &" instead of a "std::string".
SgClassSymbol* lookupSymbolFromQualifiedName(std::string className);

SgClassType* lookupTypeFromQualifiedName(std::string className);


//! Support function handles the complexity of handling append where the current scope is a SgIfStmt.
void appendStatement(SgStatement* statement);

//! Put the astJavaStatementStack into the current scope.
// void appendStatementStack();
void appendStatementStack(int numberOfStatements);

//! Support to get current class scope.
SgClassDefinition* getCurrentClassDefinition();





template< class T >
void
unaryExpressionSupport()
   {
     ROSE_ASSERT(astJavaExpressionStack.empty() == false);
     ROSE_ASSERT(astJavaExpressionStack.size() >= 1);

     SgExpression* operand = astJavaExpressionStack.front();
     ROSE_ASSERT(operand != NULL);
     astJavaExpressionStack.pop_front();

  // Build the assignment operator and push it onto the stack.
     SgExpression* assignmentExpression = SageBuilder::buildUnaryExpression<T>(operand);
     ROSE_ASSERT(assignmentExpression != NULL);
     astJavaExpressionStack.push_front(assignmentExpression);
     ROSE_ASSERT(astJavaExpressionStack.empty() == false);
   }

template< class T >
void
binaryExpressionSupport()
   {
     ROSE_ASSERT(astJavaExpressionStack.empty() == false);
     ROSE_ASSERT(astJavaExpressionStack.size() >= 2);

     SgExpression* rhs = astJavaExpressionStack.front();
     ROSE_ASSERT(rhs != NULL);
     astJavaExpressionStack.pop_front();

     SgExpression* lhs = astJavaExpressionStack.front();
     ROSE_ASSERT(lhs != NULL);
     astJavaExpressionStack.pop_front();

  // Build the assignment operator and push it onto the stack.
     SgExpression* assignmentExpression = SageBuilder::buildBinaryExpression<T>(lhs,rhs);
     ROSE_ASSERT(assignmentExpression != NULL);
     astJavaExpressionStack.push_front(assignmentExpression);
     ROSE_ASSERT(astJavaExpressionStack.empty() == false);
   }

template< class T >
void
binaryAssignmentStatementSupport()
   {
     ROSE_ASSERT(astJavaExpressionStack.empty() == false);
     ROSE_ASSERT(astJavaExpressionStack.size() >= 2);

     binaryExpressionSupport<T>();

     SgExpression* exp = astJavaExpressionStack.front();
     ROSE_ASSERT(exp != NULL);
     astJavaExpressionStack.pop_front();

     SgExprStatement* exprStatement = SageBuilder::buildExprStatement(exp);

     ROSE_ASSERT(exprStatement != NULL);
     ROSE_ASSERT(exp->get_parent() != NULL);

     astJavaStatementStack.push_front(exprStatement);
   }

// endif for ROSE_JAVA_SUPPORT
#endif
