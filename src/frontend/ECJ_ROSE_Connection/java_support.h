#ifndef ROSE_JAVA_SUPPORT
#define ROSE_JAVA_SUPPORT

// This is used for both Fortran and Java support to point to the current SgSourceFile.
extern SgSourceFile* OpenFortranParser_globalFilePointer;

#include "jni_JavaSourceCodePosition.h"
#include "token.h"
#include "VisitorContext.h"

// Control output from Fortran parser
#define DEBUG_JAVA_SUPPORT true
#define DEBUG_RULE_COMMENT_LEVEL 1
#define DEBUG_COMMENT_LEVEL 2

// Global stack of scopes
extern std::list<SgScopeStatement*> astJavaScopeStack;

// Global stack of expressions and statements
class ComponentStack : private std::list<SgNode *> {
public:
    void push(SgNode *n) {
        if (SgProject::get_verbose() > 0) {
            std::cerr << "***Pushing node " << n -> class_name() << std::endl; 
            std::cerr.flush();
        }
        push_front(n);
    }
    SgNode *pop() {
        ROSE_ASSERT(size() > 0);
        SgNode *n = front();
        if (SgProject::get_verbose() > 0) {
            std::cerr << "***Popping node " << n -> class_name() << std::endl;
            std::cerr.flush();
        }
        pop_front();
        return n;
    }
    SgNode *top() { ROSE_ASSERT(size() > 0); return front(); }
    bool empty() { return (size() == 0); }

    SgExpression *popExpression() {
        SgNode *n = pop();
        if (! isSgExpression(n)) {
            //
            // TODO: REMOVE ME WHEN QualifiedNameReference is supported
            //
            if (isSgTypeDefault(n)) { // charles4 11/10/2011 TODO: Remove this when QualifiedNameReference is supported
            std::cerr << "*** No support yet for a QualifiedNameReference dereferencing a field in an expression"
                      << std::endl;
            std::cerr.flush();
            } else
            //
            // TODO: END REMOVE ME WHEN QualifiedNameReference is supported...
            //

            std::cerr << "Invalid attempt to pop a node of type "
                     << n -> class_name()
                     << " as an SgExpression"
                     << std::endl;
            ROSE_ASSERT(false);
        }
        return (SgExpression *) n;
    }
    SgStatement *popStatement() {
        SgNode *n = pop();
        if (isSgExpression(n)) {
            if (SgProject::get_verbose() > 0) {
                std::cerr << "***Turning node "
                          << n -> class_name()
                          << " into a SgExprStatement"
                          << std::endl;
            }
            // TODO: set source position for expr statement !!!?
            n = SageBuilder::buildExprStatement((SgExpression *) n);
        }
        else if (! isSgStatement(n)) {
            std::cerr << "Invalid attemtp to pop a node of type "
                      << n -> class_name()
                      << " as an SgStatement"
                      << std::endl;
            ROSE_ASSERT(false);
        }
        return (SgStatement *) n;
    }
    SgType *popType() {
        SgNode *n = pop();
        if (! isSgType(n)) {
            std::cerr << "Invalid attempt to pop a node of type "
                     << n -> class_name()
                     << " as an SgType"
                     << std::endl;
            ROSE_ASSERT(false);
        }
        return (SgType *) n;
    }
};

extern ComponentStack astJavaComponentStack;

// Remove this!
// Global stack of expressions 
//extern std::list<SgExpression*> astJavaExpressionStack;

// Global stack of types
extern std::list<SgType*> astJavaTypeStack;

// Remove this!
// Global stack of statements
//extern std::list<SgStatement*> astJavaStatementStack;

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


// Remove this!
//! Support function handles the complexity of handling append where the current scope is a SgIfStmt.
//void appendStatement(SgStatement* statement);

// Remove this!
//! Put the astJavaStatementStack into the current scope.
// void appendStatementStack();
//void appendStatementStack(int numberOfStatements);

//! Support to get current class scope.
SgClassDefinition* getCurrentClassDefinition();

//! Strips off "#RAW" suffix from raw types (support for Java 1.5 and greater).
SgName processNameOfRawType(SgName name);

//! Support for identification of symbols using qualified names (used by the import statement).
SgSymbol* lookupSymbolInParentScopesUsingQualifiedName( SgName qualifiedName, SgScopeStatement* currentScope);

//! Refactored support to extraction of associated scope from symbol (where possible, i.e. SgClassSymbol, etc.).
SgScopeStatement* get_scope_from_symbol( SgSymbol* returnSymbol );

//! Parse the parameterized type names into parts.
std::list<SgName> generateGenericTypeNameList (const SgName & parameterizedTypeName);




// ***********************************************************
//  Template Definitions (required to be in the header files)
// ***********************************************************

template< class T >
void
unaryExpressionSupport()
   {
// Remove this!
//     ROSE_ASSERT(astJavaExpressionStack.empty() == false);
//     ROSE_ASSERT(astJavaExpressionStack.size() >= 1);
//
//     SgExpression* operand = astJavaExpressionStack.front();
     SgExpression* operand = astJavaComponentStack.popExpression();
// Remove this!
//     ROSE_ASSERT(operand != NULL);
//     astJavaExpressionStack.pop_front();

  // Build the assignment operator and push it onto the stack.
  // SgExpression* assignmentExpression = SageBuilder::buildUnaryExpression<T>(operand);
     SgExpression* resultExpression = SageBuilder::buildUnaryExpression<T>(operand);
     ROSE_ASSERT(resultExpression != NULL);
// Remove this!
//     astJavaExpressionStack.push_front(assignmentExpression);
//     ROSE_ASSERT(astJavaExpressionStack.empty() == false);
     astJavaComponentStack.push(resultExpression);
   }

template< class T >
void
binaryExpressionSupport()
   {
// Remove this!
//     ROSE_ASSERT(astJavaExpressionStack.empty() == false);
//     ROSE_ASSERT(astJavaExpressionStack.size() >= 2);
//
//     SgExpression* rhs = astJavaExpressionStack.front();
     SgExpression* rhs = astJavaComponentStack.popExpression();
     ROSE_ASSERT(rhs != NULL);
// Remove this!
//     astJavaExpressionStack.pop_front();

     SgExpression* lhs = astJavaComponentStack.popExpression();
     ROSE_ASSERT(lhs != NULL);
// Remove this!
//     astJavaExpressionStack.pop_front();

  // Build the assignment operator and push it onto the stack.
  // SgExpression* assignmentExpression = SageBuilder::buildBinaryExpression<T>(lhs,rhs);
     SgExpression* resultExpression = SageBuilder::buildBinaryExpression<T>(lhs,rhs);
     ROSE_ASSERT(resultExpression != NULL);
// Remove this!
//     astJavaExpressionStack.push_front(assignmentExpression);
     astJavaComponentStack.push(resultExpression);
// Remove this!
//     ROSE_ASSERT(astJavaExpressionStack.empty() == false);
   }

// charles4 10/14/2011: Remove this
//template< class T >
//void
//binaryAssignmentStatementSupport()
//   {
// Remove this!
//     ROSE_ASSERT(astJavaExpressionStack.empty() == false);
//     ROSE_ASSERT(astJavaExpressionStack.size() >= 2);
//
//     binaryExpressionSupport<T>();
//
// Remove this!
//     SgExpression* exp = astJavaExpressionStack.front();
//     ROSE_ASSERT(exp != NULL);
//     astJavaExpressionStack.pop_front();
//     SgExpression* exp = astJavaComponentStack.popExpression();
//
//     SgExprStatement* exprStatement = SageBuilder::buildExprStatement(exp);
//
//     ROSE_ASSERT(exprStatement != NULL);
//     ROSE_ASSERT(exp->get_parent() != NULL);
//
// Remove this!
//     astJavaStatementStack.push_front(exprStatement);
//     astJavaComponentStack.push(exprStatement);
//   }

// endif for ROSE_JAVA_SUPPORT
#endif
