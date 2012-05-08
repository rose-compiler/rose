#ifndef ROSE_JAVA_SUPPORT
#define ROSE_JAVA_SUPPORT

// Remove this !!
//extern SgGlobal *getGlobalScope();
extern SgGlobal *globalScope;
extern SgClassType *ObjectClassType;
extern SgClassDefinition *ObjectClassDefinition;

// This is used for both Fortran and Java support to point to the current SgSourceFile.
extern SgSourceFile *OpenFortranParser_globalFilePointer;

#include "jni_JavaSourceCodePosition.h"
#include "token.h"

// Control output from Fortran parser
#define DEBUG_JAVA_SUPPORT true
#define DEBUG_RULE_COMMENT_LEVEL 1
#define DEBUG_COMMENT_LEVEL 2

// Global stack of expressions, types and statements
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

    size_type size() { return std::list<SgNode*>::size(); }

    std::list<SgNode *>::iterator begin() {
        return std::list<SgNode *>::begin();
    }

    std::list<SgNode *>::iterator end() {
        return std::list<SgNode *>::end();
    }

    std::list<SgNode *>::reverse_iterator rbegin() {
        return std::list<SgNode *>::rbegin();
    }

    std::list<SgNode *>::reverse_iterator rend() {
        return std::list<SgNode *>::rend();
    }

    SgExpression *popExpression() {
        SgNode *n = pop();
        if (! isSgExpression(n)) {
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

// Global stack of scope elements.
class ScopeStack : private std::list<SgScopeStatement *> {
public:
    void push(SgScopeStatement *n) {
        if (SgProject::get_verbose() > 0) {
            std::cerr << "***Pushing node " << n -> class_name() << std::endl; 
            std::cerr.flush();
        }
        push_front(n);
    }

    SgScopeStatement *pop() {
        ROSE_ASSERT(size() > 0);
        SgScopeStatement *n = front();
        if (SgProject::get_verbose() > 0) {
            std::cerr << "***Popping node " << n -> class_name() << std::endl;
            std::cerr.flush();
        }
        pop_front();
        return n;
    }

    SgGlobal *popGlobal() {
        SgScopeStatement *n = pop();
        if (! isSgGlobal(n)) {
            std::cerr << "Invalid attempt to pop a node of type "
                     << n -> class_name()
                     << " as an SgGlobal"
                     << std::endl;
            ROSE_ASSERT(false);
        }
        return (SgGlobal *) n;
    }

    SgNamespaceDefinitionStatement *popNamespaceDefinitionStatement() {
        SgScopeStatement *n = pop();
        if (! isSgNamespaceDefinitionStatement(n)) {
            std::cerr << "Invalid attempt to pop a node of type "
                     << n -> class_name()
                     << " as an SgNamespaceDefinitionStatement"
                     << std::endl;
            ROSE_ASSERT(false);
        }
        return (SgNamespaceDefinitionStatement *) n;
    }

    SgClassDefinition *popClassDefinition() {
        SgScopeStatement *n = pop();
        if (! isSgClassDefinition(n)) {
            std::cerr << "Invalid attempt to pop a node of type "
                     << n -> class_name()
                     << " as an SgClassDefinition"
                     << std::endl;
            ROSE_ASSERT(false);
        }
        return (SgClassDefinition *) n;
    }

    SgBasicBlock *popBasicBlock() {
        SgScopeStatement *n = pop();
        if (! isSgBasicBlock(n)) {
            std::cerr << "Invalid attempt to pop a node of type "
                     << n -> class_name()
                     << " as an SgBasicBlock"
                     << std::endl;
            ROSE_ASSERT(false);
        }
        return (SgBasicBlock *) n;
    }

    SgIfStmt *popIfStmt() {
        SgScopeStatement *n = pop();
        if (! isSgIfStmt(n)) {
            std::cerr << "Invalid attempt to pop a node of type "
                     << n -> class_name()
                     << " as an SgIfStmt"
                     << std::endl;
            ROSE_ASSERT(false);
        }
        return (SgIfStmt *) n;
    }

    SgSwitchStatement *popSwitchStatement() {
        SgScopeStatement *n = pop();
        if (! isSgSwitchStatement(n)) {
            std::cerr << "Invalid attempt to pop a node of type "
                     << n -> class_name()
                     << " as an SgSwitchStatement"
                     << std::endl;
            ROSE_ASSERT(false);
        }
        return (SgSwitchStatement *) n;
    }

    SgCatchOptionStmt *popCatchOptionStmt() {
        SgScopeStatement *n = pop();
        if (! isSgCatchOptionStmt(n)) {
            std::cerr << "Invalid attempt to pop a node of type "
                     << n -> class_name()
                     << " as an SgCatchOptionStmt"
                     << std::endl;
            ROSE_ASSERT(false);
        }
        return (SgCatchOptionStmt *) n;
    }

    SgWhileStmt *popWhileStmt() {
        SgScopeStatement *n = pop();
        if (! isSgWhileStmt(n)) {
            std::cerr << "Invalid attempt to pop a node of type "
                     << n -> class_name()
                     << " as an SgWhileStmt"
                     << std::endl;
            ROSE_ASSERT(false);
        }
        return (SgWhileStmt *) n;
    }

    SgDoWhileStmt *popDoWhileStmt() {
        SgScopeStatement *n = pop();
        if (! isSgDoWhileStmt(n)) {
            std::cerr << "Invalid attempt to pop a node of type "
                     << n -> class_name()
                     << " as an SgDoWhileStmt"
                     << std::endl;
            ROSE_ASSERT(false);
        }
        return (SgDoWhileStmt *) n;
    }

    SgForStatement *popForStatement() {
        SgScopeStatement *n = pop();
        if (! isSgForStatement(n)) {
            std::cerr << "Invalid attempt to pop a node of type "
                     << n -> class_name()
                     << " as an SgForStatement"
                     << std::endl;
            ROSE_ASSERT(false);
        }
        return (SgForStatement *) n;
    }

    SgJavaForEachStatement *popJavaForEachStatement() {
        SgScopeStatement *n = pop();
        if (! isSgJavaForEachStatement(n)) {
            std::cerr << "Invalid attempt to pop a node of type "
                     << n -> class_name()
                     << " as an SgJavaForEachStatement"
                     << std::endl;
            ROSE_ASSERT(false);
        }
        return (SgJavaForEachStatement *) n;
    }

    SgJavaLabelStatement *popJavaLabelStatement() {
        SgScopeStatement *n = pop();
        if (! isSgJavaLabelStatement(n)) {
            std::cerr << "Invalid attempt to pop a node of type "
                     << n -> class_name()
                     << " as an SgJavaLabelStatement"
                     << std::endl;
            ROSE_ASSERT(false);
        }
        return (SgJavaLabelStatement *) n;
    }

    SgFunctionDefinition *popFunctionDefinition() {
        SgScopeStatement *n = pop();
        if (! isSgFunctionDefinition(n)) {
            std::cerr << "Invalid attempt to pop a node of type "
                     << n -> class_name()
                     << " as an SgFunctionDefinition"
                     << std::endl;
            ROSE_ASSERT(false);
        }
        return (SgFunctionDefinition *) n;
    }

    SgScopeStatement *top() { ROSE_ASSERT(size() > 0); return (SgScopeStatement *) front(); }

    bool empty() { return (size() == 0); }

    size_type size() { return std::list<SgScopeStatement *>::size(); }

    std::list<SgScopeStatement *>::iterator begin() {
        return std::list<SgScopeStatement *>::begin();
    }

    std::list<SgScopeStatement *>::iterator end() {
        return std::list<SgScopeStatement *>::end();
    }

    std::list<SgScopeStatement *>::reverse_iterator rbegin() {
        return std::list<SgScopeStatement *>::rbegin();
    }

    std::list<SgScopeStatement *>::reverse_iterator rend() {
        return std::list<SgScopeStatement *>::rend();
    }
};

// Global stack of AST components: Expressions, statements, types, etc...
extern ComponentStack astJavaComponentStack;

// Global stack of scopes
extern ScopeStack /* std::list<SgScopeStatement*> */ astJavaScopeStack;

// Global list of implicit classes
extern std::list<SgName> astJavaImplicitClassList;

std::list<SgStatement*> pop_from_stack_and_reverse(std::list<SgStatement*> &l, int nb_pop);

/* Create a token from a JavaToken jni object. Also converts JavaSourceCodeInformation to C */
Token_t *create_token(JNIEnv *env, jobject jToken);

void pushAndSetSourceCodePosition(JavaSourceCodePosition *pos, SgLocatedNode *sgnode);

// Duplicated setJavaSourcePosition signature.
// Not sure why but JNI wasn't happy if posInfo was assigned to a default value
void setJavaSourcePosition(SgLocatedNode *locatedNode);
void setJavaSourcePosition(SgLocatedNode *locatedNode, JavaSourceCodePosition *posInfo);

// DQ (8/16/2011): Added support using the jToken object.
void setJavaSourcePosition(SgLocatedNode *locatedNode, JNIEnv *env, jobject jToken);

// DQ (8/16/2011): Added support for marking nodes as compiler generated (implicit in Java).
void setJavaCompilerGenerated(SgLocatedNode *locatedNode);
void setJavaSourcePositionUnavailableInFrontend(SgLocatedNode *locatedNode);

//! This is how Java implicit classes are marked so that they can be avoided in output.
void setJavaFrontendSpecific(SgLocatedNode *locatedNode);


// *********************************************

std::string convertJavaStringToCxxString  (JNIEnv *env, const jstring &java_string);
int         convertJavaIntegerToCxxInteger(JNIEnv *env, const jint    &java_integer);
bool        convertJavaBooleanToCxxBoolean(JNIEnv *env, const jboolean &java_boolean);

// Specify the SgClassDefinition explicitly so that implicit classes are simpler to build.
// SgMemberFunctionDeclaration *buildSimpleMemberFunction(const SgName &name);
// SgMemberFunctionDeclaration *buildSimpleMemberFunction(const SgName &name, SgClassDefinition *classDefinition);

// DQ (3/25/2011): These will replace buildSimpleMemberFunction shortly.
SgMemberFunctionDeclaration *buildNonDefiningMemberFunction(const SgName &inputName, SgClassDefinition *classDefinition, int num_arguments);
SgMemberFunctionDeclaration *buildDefiningMemberFunction   (const SgName &inputName, SgClassDefinition *classDefinition, int num_arguments);

// Build a simple class in the current scope and set the scope to be the class definition.
void buildClass (const SgName &className, Token_t *token);
void buildImplicitClass (const SgName &className);
void buildClassSupport (const SgName &className, bool implicitClass, Token_t *token);

SgVariableDeclaration *buildSimpleVariableDeclaration(const SgName &name, SgType *type);

std::list<SgName> generateQualifierList (const SgName &classNameWithQualification);
SgName stripQualifiers (const SgName &classNameWithQualification);

// It might be that this function should take a "const SgName &" instead of a "std::string".
SgClassSymbol *lookupSymbolFromQualifiedName(std::string className);

SgClassType *lookupTypeFromQualifiedName(std::string className);

//! Support to get current class scope.
SgClassDefinition *getCurrentClassDefinition();

//! Strips off "#RAW" suffix from raw types (support for Java 1.5 and greater).
SgName processNameOfRawType(SgName name);

//! Support for identification of symbols using simple names in a given scope.
SgSymbol *lookupSimpleNameInClassScope(const SgName &name, SgClassDefinition *classDefinition);

//! Support for identification of symbols using simple names.
SgVariableSymbol *lookupVariableByName(const SgName &name);

//! Support for identification of symbols using qualified names (used by the import statement).
SgSymbol *lookupSymbolInParentScopesUsingQualifiedName(SgName qualifiedName, SgScopeStatement *currentScope);

//! charles4: Support for identification of symbols using qualified names (used by the import statement).
//SgSymbol *FindSymbolInParentScopesUsingQualifiedName(SgName qualifiedName, SgScopeStatement *currentScope);

//! charles4: 02/15/2012 
//SgSymbol *lookupSymbolInParentScopesUsingSimpleName(SgName name, SgScopeStatement *currentScope);

//! Refactored support to extraction of associated scope from symbol (where possible, i.e. SgClassSymbol, etc.).
SgScopeStatement *get_scope_from_symbol(SgSymbol *returnSymbol);

// ***********************************************************
//  Template Definitions (required to be in the header files)
// ***********************************************************

template< class T >
void unaryExpressionSupport() {
    SgExpression *operand = astJavaComponentStack.popExpression();

    // Build the assignment operator and push it onto the stack.
    // SgExpression *assignmentExpression = SageBuilder::buildUnaryExpression<T>(operand);
    SgExpression *resultExpression = SageBuilder::buildUnaryExpression<T>(operand);
    ROSE_ASSERT(resultExpression != NULL);
    astJavaComponentStack.push(resultExpression);
}

template< class T >
void binaryExpressionSupport() {
    SgExpression *rhs = astJavaComponentStack.popExpression();
    ROSE_ASSERT(rhs != NULL);

    SgExpression *lhs = astJavaComponentStack.popExpression();
    ROSE_ASSERT(lhs != NULL);

    // Build the assignment operator and push it onto the stack.
    // SgExpression *assignmentExpression = SageBuilder::buildBinaryExpression<T>(lhs, rhs);
    SgExpression *resultExpression = SageBuilder::buildBinaryExpression<T>(lhs, rhs);
    ROSE_ASSERT(resultExpression != NULL);

    astJavaComponentStack.push(resultExpression);
}

// endif for ROSE_JAVA_SUPPORT
#endif
