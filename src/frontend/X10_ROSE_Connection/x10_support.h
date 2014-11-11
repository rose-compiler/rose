#ifndef ROSE_X10_SUPPORT
#define ROSE_X10_SUPPORT

#include "jserver.h"

extern SgProject *project;
extern SgGlobal *globalScope;
extern SgSourceFile *currentSourceFile;
extern SgClassDefinition *ObjectClassDefinition;
extern jstring currentFilePath;
extern std::string currentTypeName;

// Control output from Fortran parser
#define DEBUG_X10_SUPPORT true
#define DEBUG_RULE_COMMENT_LEVEL 1
#define DEBUG_COMMENT_LEVEL 2

extern std::string javaStringToUtf8(const jstring &java_string);

std::string convertX10PackageNameToCxxString(JNIEnv *env, const jstring &java_string);
extern SgPointerType *getUniquePointerType(SgType *, int);

std::string getExtensionNames(std::vector<SgNode *> &extension_list, SgClassDeclaration *class_declaration, bool has_super_class);

bool isConflictingType(std::string, SgClassType *);
bool isImportedType(SgClassType *);
bool isImportedTypeOnDemand(AstSgNodeListAttribute *, SgClassDefinition *, SgClassType *);

bool mustBeFullyQualified(SgClassType *class_type);
std::string markAndGetQualifiedTypeName(SgClassType *class_type);

bool hasConflicts(SgClassDeclaration *class_declaration);

void replaceString (std::string&, const std::string&, const std::string&);

std::string getPrimitiveTypeName(SgType *);
std::string getWildcardTypeName(SgJavaWildcardType *);
std::string getUnionTypeName(SgJavaUnionType *);
std::string getParameters(SgJavaParameterizedType *);
std::string getTypeName(SgClassType *class_type);
std::string getTypeName(SgType *);
std::string getUnqualifiedTypeName(SgType *);
std::string getFullyQualifiedTypeName(SgType *);

//
// This class is kept here as documentation. IT is declared in:  ./src/midend/astProcessing/AstAttributeMechanism.h
//
// class AstSgNodeListAttribute : public AstAttribute {
//     std::vector<SgNode *> nodeList;
//
//     public:
//         std::vector<SgNode *> &getNodeList();
//         void addNode(SgNode *);
//         SgNode *getNode(int);
//         void setNode(SgNode *, int);
//         int size();
//
//         AstSgNodeListAttribute();
//         AstSgNodeListAttribute(std::vector<SgNode *> &);
// };


//
// Global stack of expressions, types and statements
//
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
            std::cerr << "Invalid attempt to pop a node of type "
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

    SgClassDefinition *popPackage() {
        SgScopeStatement *n = pop();
        if (! isSgClassDefinition(n)) {
            std::cerr << "Invalid attempt to pop a node of type "
                     << n -> class_name()
                     << " as an SgClassDefinition"
                     << std::endl;
            ROSE_ASSERT(false);
        }
        else if (! isSgJavaPackageDeclaration(isSgClassDefinition(n) -> get_declaration())) {
            std::cerr << "Invalid attempt to pop a node of type "
                     << n -> class_name()
                     << " as an SgJavaPackageDeclaration"
                     << std::endl;
            ROSE_ASSERT(false);
        }
        return (SgClassDefinition *) n;
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
extern ComponentStack astX10ComponentStack;

// Global stack of scopes
extern ScopeStack astX10ScopeStack;

class X10_Token_t;
void setX10SourcePosition(SgLocatedNode *locatedNode, X10_Token_t *);
void setX10SourcePosition(SgLocatedNode *locatedNode, JNIEnv *env, jobject jToken);
//void setJavaSourcePositionUnavailableInFrontend(SgLocatedNode *locatedNode);

// *********************************************

//SgClassDeclaration *buildDefiningClassDeclaration(SgClassDeclaration::class_types kind, SgName, SgScopeStatement *);
SgJavaPackageDeclaration *buildPackageDeclaration(SgScopeStatement *, const SgName &, JNIEnv *, jobject);
SgClassDefinition *findOrInsertPackage(SgScopeStatement *, const SgName &, JNIEnv *env, jobject loc);
SgClassDefinition *findOrInsertPackage(SgName &, JNIEnv *env, jobject loc);
SgJavaPackageDeclaration *findPackageDeclaration(SgName &);

SgMemberFunctionDeclaration *buildDefiningMemberFunction(const SgName &inputName, SgClassDefinition *classDefinition, int num_arguments, JNIEnv *env, jobject methodLoc, jobject argsLoc);
// TODO: Remove this !!!
//SgMemberFunctionDeclaration *lookupMemberFunctionDeclarationInClassScope(SgClassDefinition *classDefinition, const SgName &function_name, int num_arguments);
//SgMemberFunctionDeclaration *lookupMemberFunctionDeclarationInClassScope(SgClassDefinition *classDefinition, const SgName &function_name, list<SgType *> &);
//SgMemberFunctionDeclaration *findMemberFunctionDeclarationInClass(SgClassDefinition *classDefinition, const SgName &function_name, list<SgType *>& types);
#if 1
SgMemberFunctionSymbol *findFunctionSymbolInClass(SgClassDefinition *classDefinition, const SgName &function_name, std::list<SgType *> &, JNIEnv *env);
#else
SgMemberFunctionSymbol *findFunctionSymbolInClass(SgClassDefinition *classDefinition, const SgName &function_name, std::list<SgType *> &, JNIEnv *env, jobject x10Visitor);
#endif

std::list<SgName> generateQualifierList (const SgName &classNameWithQualification);

bool isCompatibleTypes(SgType *source, SgType *target);

// TODO: Remove this !!!
// It might be that this function should take a "const SgName &" instead of a "string".
// SgClassSymbol *lookupSymbolFromQualifiedName(string className);

//
// This function is needed in order to bypass a serious bug in Rose.  See implementation for detail
//
SgClassSymbol *lookupClassSymbolInScope(SgScopeStatement *, const SgName &);

void lookupLocalTypeSymbols(std::list<SgClassSymbol *> &, SgName &type_name);

SgType *lookupTypeByName(SgName &packageName, SgName &typeName, int num_dimensions);

//! Support to get current class scope.
SgClassDefinition *getCurrentTypeDefinition();

// TODO: Remove this !!!
//SgClassSymbol *lookupParameterTypeByName(const SgName &name);

//! Support for identification of symbols using simple names in a given scope.
SgClassSymbol *lookupUniqueSimpleNameTypeInClass(const SgName &name, SgClassDefinition *classDefinition);
void lookupAllSimpleNameTypesInClass(std::list<SgClassSymbol *>&, const SgName &name, SgClassDefinition *classDefinition);
SgVariableSymbol *lookupSimpleNameVariableInClass(const SgName &name, SgClassDefinition *classDefinition);

//! Support for identification of variable symbols using simple names.
SgVariableSymbol *lookupVariableByName(JNIEnv *env, const SgName &name);

//! Support for identification of label symbols using simple names.
SgJavaLabelSymbol *lookupLabelByName(const SgName &name);

// TODO: Remove this !!!
//! Refactored support to extraction of associated scope from symbol (where possible, i.e. SgClassSymbol, etc.).
//SgScopeStatement *get_scope_from_symbol(SgSymbol *returnSymbol);

// ***********************************************************
//  Template Definitions (required to be in the header files)
// ***********************************************************

template< class T >
void unaryExpressionSupport() {
    SgExpression *operand = astX10ComponentStack.popExpression();

    // Build the assignment operator and push it onto the stack.
    // SgExpression *assignmentExpression = SageBuilder::buildUnaryExpression<T>(operand);
    SgExpression *resultExpression = SageBuilder::buildUnaryExpression<T>(operand);
    ROSE_ASSERT(resultExpression != NULL);
    astX10ComponentStack.push(resultExpression);
}

template< class T >
void binaryExpressionSupport() {
    SgExpression *rhs = astX10ComponentStack.popExpression();
    ROSE_ASSERT(rhs != NULL);

    SgExpression *lhs = astX10ComponentStack.popExpression();
    ROSE_ASSERT(lhs != NULL);

    // Build the assignment operator and push it onto the stack.
    // SgExpression *assignmentExpression = SageBuilder::buildBinaryExpression<T>(lhs, rhs);
    SgExpression *resultExpression = SageBuilder::buildBinaryExpression<T>(lhs, rhs);
    ROSE_ASSERT(resultExpression != NULL);

    astX10ComponentStack.push(resultExpression);
}

#include <map>
extern std::map <std::string, ScopeStack> scopeMap;
extern std::map <std::string, ComponentStack> componentMap;

// endif for ROSE_X10_SUPPORT
#endif
