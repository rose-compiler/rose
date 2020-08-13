#ifndef ROSE_JAVA_SUPPORT
#define ROSE_JAVA_SUPPORT

#include "ecj.h"

using namespace std;

extern SgProject *project;
extern SgGlobal *globalScope;
extern SgSourceFile *currentSourceFile;
extern SgClassDefinition *ObjectClassDefinition;

// Control output from Fortran parser
#define DEBUG_JAVA_SUPPORT true
#define DEBUG_RULE_COMMENT_LEVEL 1
#define DEBUG_COMMENT_LEVEL 2

// TODO: Remove this !!!
//extern string convertJavaPackageNameToCxxString(JNIEnv *env, const jstring &java_string);
//extern string convertJavaStringValToUtf8(JNIEnv *env, const jstring &java_string);
extern string javaStringToUtf8(const jstring &java_string);

//extern SgArrayType *getUniqueArrayType(SgType *, int);
//extern SgPointerType *getUniquePointerType(SgType *, int);
//extern SgJavaParameterizedType *getUniqueParameterizedType(SgNamedType *, SgTemplateParameterPtrList *);
//extern SgJavaWildcardType *getUniqueWildcardUnbound();
//extern SgJavaWildcardType *getUniqueWildcardExtends(SgType *);
//extern SgJavaWildcardType *getUniqueWildcardSuper(SgType *);
//extern SgJavaQualifiedType *getUniqueQualifiedType(SgClassDeclaration *, SgType *, SgType *);

string getExtensionNames(std::vector<SgNode *> &extension_list, SgClassDeclaration *class_declaration, bool has_super_class);

//bool isVisibleSimpleTypeName(SgNamedType *);
bool isConflictingType(string, SgClassType *);
bool isImportedType(SgClassType *);
bool isImportedTypeOnDemand(AstSgNodeListAttribute *, SgClassDefinition *, SgClassType *);

bool mustBeFullyQualified(SgClassType *class_type);
string markAndGetQualifiedTypeName(SgClassType *class_type);

bool hasConflicts(SgClassDeclaration *class_declaration);

string getPrimitiveTypeName(SgType *);
string getWildcardTypeName(SgJavaWildcardType *);
string getUnionTypeName(SgJavaUnionType *);
string getParameters(SgJavaParameterizedType *);
string getTypeName(SgClassType *class_type);
string getTypeName(SgType *);
string getUnqualifiedTypeName(SgType *);
string getFullyQualifiedTypeName(SgType *);

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
class ComponentStack : private list<SgNode *> {
public:
    void push(SgNode *n) {
        if (SgProject::get_verbose() > 0) {
            cerr << "***Pushing Component node " << n -> class_name() << endl; 
            cerr.flush();
        }
        push_front(n);
    }

    SgNode *pop() {
        ROSE_ASSERT(size() > 0);
        SgNode *n = front();
        if (SgProject::get_verbose() > 0) {
            cerr << "***Popping Component node ";
            cerr << n -> class_name();
            cerr << endl;
            cerr.flush();
        }
        pop_front();
        return n;
    }

    SgNode *top() { ROSE_ASSERT(size() > 0); return front(); }

    bool empty() { return (size() == 0); }

    size_type size() { return list<SgNode*>::size(); }

    list<SgNode *>::iterator begin() {
        return list<SgNode *>::begin();
    }

    list<SgNode *>::iterator end() {
        return list<SgNode *>::end();
    }

    list<SgNode *>::reverse_iterator rbegin() {
        return list<SgNode *>::rbegin();
    }

    list<SgNode *>::reverse_iterator rend() {
        return list<SgNode *>::rend();
    }

    SgExpression *popExpression() {
        SgNode *n = pop();
        if (! isSgExpression(n)) {
            ROSE_ASSERT(isSgClassDefinition(n) != NULL);
            cerr << "Invalid attempt to pop a Component node of type "
                     << (isSgClassDefinition(n) ? isSgClassDefinition(n) -> get_qualified_name().getString() : n -> class_name())
                     << " as an SgExpression"
                     << endl;
            ROSE_ASSERT(false);
        }
        return (SgExpression *) n;
    }

    SgStatement *popStatement() {
        SgNode *n = pop();
        if (isSgExpression(n)) {
            if (SgProject::get_verbose() > 0) {
                cerr << "***Turning node "
                          << n -> class_name()
                          << " into a SgExprStatement"
                          << endl;
            }
            // TODO: set source position for expr statement !!!?
            n = SageBuilder::buildExprStatement((SgExpression *) n);
        }
        else if (! isSgStatement(n)) {
            cerr << "Invalid attempt to pop a Component node of type "
                      << n -> class_name()
                      << " as an SgStatement"
                      << endl;
            ROSE_ASSERT(false);
        }
        return (SgStatement *) n;
    }

    SgType *popType() {
        SgNode *n = pop();
        if (! isSgType(n)) {
            cerr << "Invalid attempt to pop a Component node of type "
                     << n -> class_name()
                     << " as an SgType"
                     << endl;
            ROSE_ASSERT(false);
        }
        return (SgType *) n;
    }
};

//
// Global stack of scope elements.
//
class ScopeStack : private list<SgScopeStatement *> {
public:
    void push(SgScopeStatement *n) {
        if ((isSgLocatedNode(n) != NULL) && n != globalScope) {
           Sg_File_Info *file_info = isSgLocatedNode(n) -> get_startOfConstruct();
           if (file_info == NULL)
              cout << "Null file_info found while pushing scope node " << n -> class_name() << endl;
           else if (file_info -> get_filenameString().size() == 0)
              cout << "file_info with null string found while pushing scope node " << n -> class_name() << endl;
         //else 
         //cout << "file_info with file: " << file_info -> get_filenameString() << ", found while pushing scope node " << n -> class_name() << endl;
           cout.flush();
        }
        if (SgProject::get_verbose() > 0) {
            cerr << "***Pushing Stack node ";
            if (isSgClassDefinition(n))
                 cerr << isSgClassDefinition(n) -> get_qualified_name().getString();
            else if (isSgFunctionDefinition(n))
                 cerr << isSgFunctionDefinition(n) -> get_declaration() -> get_name().getString();
            else if (isSgFunctionDefinition(n))
                 cerr << isSgFunctionDefinition(n) -> get_declaration() -> get_name().getString();
            else cerr << n -> class_name();
            cerr << endl; 
            cerr.flush();
        }
        push_front(n);
    }

    SgScopeStatement *pop() {
        ROSE_ASSERT(size() > 0);
        SgScopeStatement *n = front();
        if (isSgLocatedNode(n) && n != ::globalScope) {
Sg_File_Info *file_info = isSgLocatedNode(n) -> get_startOfConstruct();
if (file_info == NULL)
cout << "Null file_info found while popping scope node " << n -> class_name() << endl;
else if (file_info -> get_filenameString().size() == 0)
cout << "file_info with null string found while popping scope node " << n -> class_name() << endl;
//else 
//cout << "file_info with file: " << file_info -> get_filenameString() << ", found while popping scope node " << n -> class_name() << endl;
cout.flush();
}
       if (SgProject::get_verbose() > 0) {
            cerr << "***Popping Stack node ";
            if (isSgClassDefinition(n) != NULL)
                 cerr << isSgClassDefinition(n) -> get_qualified_name().getString();
            else if (isSgFunctionDefinition(n) != NULL)
                 cerr << isSgFunctionDefinition(n) -> get_declaration() -> get_name().getString();
            else if (isSgFunctionDefinition(n) != NULL)
                 cerr << isSgFunctionDefinition(n) -> get_declaration() -> get_name().getString();
            else
                 cerr << n -> class_name();
            cerr << endl;
            cerr.flush();
        }
        pop_front();
        return n;
    }

    SgGlobal *popGlobal() {
        SgScopeStatement *n = pop();
        if (! isSgGlobal(n)) {
            cerr << "Invalid attempt to pop a Stack node of type "
                     << n -> class_name()
                     << " as an SgGlobal"
                     << endl;
            ROSE_ASSERT(false);
        }
        return (SgGlobal *) n;
    }

    SgNamespaceDefinitionStatement *popNamespaceDefinitionStatement() {
        SgScopeStatement *n = pop();
        if (! isSgNamespaceDefinitionStatement(n)) {
            cerr << "Invalid attempt to pop a Stack node of type "
                     << n -> class_name()
                     << " as an SgNamespaceDefinitionStatement"
                     << endl;
            ROSE_ASSERT(false);
        }
        return (SgNamespaceDefinitionStatement *) n;
    }

    SgClassDefinition *popPackage() {
        SgScopeStatement *n = pop();
        if (! isSgClassDefinition(n)) {
            cerr << "Invalid attempt to pop a Stack node of type "
                     << n -> class_name()
                     << " as an SgClassDefinition"
                     << endl;
            ROSE_ASSERT(false);
        }
        else if (! isSgJavaPackageDeclaration(isSgClassDefinition(n) -> get_declaration())) {
            cerr << "Invalid attempt to pop a Stack node of type "
                     << n -> class_name()
                     << " as an SgJavaPackageDeclaration"
                     << endl;
            ROSE_ASSERT(false);
        }
        return (SgClassDefinition *) n;
    }

    SgClassDefinition *popClassDefinition() {
        SgScopeStatement *n = pop();
        if (! isSgClassDefinition(n)) {
            cerr << "Invalid attempt to pop a Stack node of type "
                     << n -> class_name()
                     << " as an SgClassDefinition"
                     << endl;
            ROSE_ASSERT(false);
        }
        return (SgClassDefinition *) n;
    }

    SgBasicBlock *popBasicBlock() {
        SgScopeStatement *n = pop();
        if (! isSgBasicBlock(n)) {
            cerr << "Invalid attempt to pop a Stack node of type "
                     << n -> class_name()
                     << " as an SgBasicBlock"
                     << endl;
            ROSE_ASSERT(false);
        }
        return (SgBasicBlock *) n;
    }

    SgIfStmt *popIfStmt() {
        SgScopeStatement *n = pop();
        if (! isSgIfStmt(n)) {
            cerr << "Invalid attempt to pop a Stack node of type "
                     << n -> class_name()
                     << " as an SgIfStmt"
                     << endl;
            ROSE_ASSERT(false);
        }
        return (SgIfStmt *) n;
    }

    SgSwitchStatement *popSwitchStatement() {
        SgScopeStatement *n = pop();
        if (! isSgSwitchStatement(n)) {
            cerr << "Invalid attempt to pop a Stack node of type "
                     << n -> class_name()
                     << " as an SgSwitchStatement"
                     << endl;
            ROSE_ASSERT(false);
        }
        return (SgSwitchStatement *) n;
    }

    SgCatchOptionStmt *popCatchOptionStmt() {
        SgScopeStatement *n = pop();
        if (! isSgCatchOptionStmt(n)) {
            cerr << "Invalid attempt to pop a Stack node of type "
                     << n -> class_name()
                     << " as an SgCatchOptionStmt"
                     << endl;
            ROSE_ASSERT(false);
        }
        return (SgCatchOptionStmt *) n;
    }

    SgWhileStmt *popWhileStmt() {
        SgScopeStatement *n = pop();
        if (! isSgWhileStmt(n)) {
            cerr << "Invalid attempt to pop a Stack node of type "
                     << n -> class_name()
                     << " as an SgWhileStmt"
                     << endl;
            ROSE_ASSERT(false);
        }
        return (SgWhileStmt *) n;
    }

    SgDoWhileStmt *popDoWhileStmt() {
        SgScopeStatement *n = pop();
        if (! isSgDoWhileStmt(n)) {
            cerr << "Invalid attempt to pop a Stack node of type "
                     << n -> class_name()
                     << " as an SgDoWhileStmt"
                     << endl;
            ROSE_ASSERT(false);
        }
        return (SgDoWhileStmt *) n;
    }

    SgForStatement *popForStatement() {
        SgScopeStatement *n = pop();
        if (! isSgForStatement(n)) {
            cerr << "Invalid attempt to pop a Stack node of type "
                     << n -> class_name()
                     << " as an SgForStatement"
                     << endl;
            ROSE_ASSERT(false);
        }
        return (SgForStatement *) n;
    }

    SgJavaForEachStatement *popJavaForEachStatement() {
        SgScopeStatement *n = pop();
        if (! isSgJavaForEachStatement(n)) {
            cerr << "Invalid attempt to pop a Stack node of type "
                     << n -> class_name()
                     << " as an SgJavaForEachStatement"
                     << endl;
            ROSE_ASSERT(false);
        }
        return (SgJavaForEachStatement *) n;
    }

    SgJavaLabelStatement *popJavaLabelStatement() {
        SgScopeStatement *n = pop();
        if (! isSgJavaLabelStatement(n)) {
            cerr << "Invalid attempt to pop a Stack node of type "
                     << n -> class_name()
                     << " as an SgJavaLabelStatement"
                     << endl;
            ROSE_ASSERT(false);
        }
        return (SgJavaLabelStatement *) n;
    }

    SgFunctionDefinition *popFunctionDefinition() {
        SgScopeStatement *n = pop();
        if (! isSgFunctionDefinition(n)) {
            cerr << "Invalid attempt to pop a Stack node of type "
                     << n -> class_name()
                     << " as an SgFunctionDefinition"
                     << endl;
            ROSE_ASSERT(false);
        }
        return (SgFunctionDefinition *) n;
    }

    SgScopeStatement *top() { ROSE_ASSERT(size() > 0); return (SgScopeStatement *) front(); }

    bool empty() { return (size() == 0); }

    size_type size() { return list<SgScopeStatement *>::size(); }

    list<SgScopeStatement *>::iterator begin() {
        return list<SgScopeStatement *>::begin();
    }

    list<SgScopeStatement *>::iterator end() {
        return list<SgScopeStatement *>::end();
    }

    list<SgScopeStatement *>::reverse_iterator rbegin() {
        return list<SgScopeStatement *>::rbegin();
    }

    list<SgScopeStatement *>::reverse_iterator rend() {
        return list<SgScopeStatement *>::rend();
    }
};

// Global stack of AST components: Expressions, statements, types, etc...
extern ComponentStack astJavaComponentStack;

// Global stack of scopes
extern ScopeStack astJavaScopeStack;

class Token_t;
void setJavaSourcePosition(SgLocatedNode *locatedNode, Token_t *);
void setJavaSourcePosition(SgLocatedNode *locatedNode, JNIEnv *env, jobject jToken);
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
//SgMemberFunctionSymbol *findFunctionSymbolInClass(SgClassDefinition *classDefinition, const SgName &function_name, list<SgType *> &);

list<SgName> generateQualifierList (const SgName &classNameWithQualification);

bool isCompatibleTypes(SgType *source, SgType *target);

// TODO: Remove this !!!
// It might be that this function should take a "const SgName &" instead of a "string".
// SgClassSymbol *lookupSymbolFromQualifiedName(string className);

//
// This function is needed in order to bypass a serious bug in Rose.  See implementation for detail
//
SgClassSymbol *lookupClassSymbolInScope(SgScopeStatement *, const SgName &);

void lookupLocalTypeSymbols(list<SgClassSymbol *> &, SgName &type_name);

SgType *lookupTypeByName(SgName &packageName, SgName &typeName, int num_dimensions);

//! Support to get current class scope.
SgClassDefinition *getCurrentTypeDefinition();

// TODO: Remove this !!!
//SgClassSymbol *lookupParameterTypeByName(const SgName &name);

//! Support for identification of symbols using simple names in a given scope.
SgClassSymbol *lookupUniqueSimpleNameTypeInClass(const SgName &name, SgClassDefinition *classDefinition);
void lookupAllSimpleNameTypesInClass(list<SgClassSymbol *>&, const SgName &name, SgClassDefinition *classDefinition);
SgVariableSymbol *lookupSimpleNameVariableInClass(const SgName &name, SgClassDefinition *classDefinition);

//! Support for identification of variable symbols using simple names.
SgVariableSymbol *lookupVariableByName(const SgName &name);

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
