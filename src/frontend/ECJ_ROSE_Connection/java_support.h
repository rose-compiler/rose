#ifndef ROSE_JAVA_SUPPORT
#define ROSE_JAVA_SUPPORT

extern SgGlobal *globalScope;
extern SgClassType *ObjectClassType;
extern SgClassType *StringClassType;
extern SgClassType *ClassClassType;
extern SgClassDefinition *ObjectClassDefinition;

extern int initializerCount;

// This is used for both Fortran and Java support to point to the current SgSourceFile.
extern SgSourceFile *OpenFortranParser_globalFilePointer;

#include "jni_JavaSourceCodePosition.h"
#include "token.h"

// Control output from Fortran parser
#define DEBUG_JAVA_SUPPORT true
#define DEBUG_RULE_COMMENT_LEVEL 1
#define DEBUG_COMMENT_LEVEL 2


extern SgArrayType *getUniqueArrayType(SgType *, int);
extern SgPointerType *getUniquePointerType(SgType *, int);
extern SgJavaParameterizedType *getUniqueParameterizedType(SgClassType *, SgTemplateParameterPtrList &);
extern SgJavaWildcardType *getUniqueWildcardUnbound();
extern SgJavaWildcardType *getUniqueWildcardExtends(SgType *);
extern SgJavaWildcardType *getUniqueWildcardSuper(SgType *);

using namespace std;

string getPrimitiveTypeName(SgType *);
string getWildcardTypeName(SgJavaWildcardType *);
string getArrayTypeName(SgPointerType *);
string getFullyQualifiedName(SgClassDefinition *);
string getFullyQualifiedTypeName(SgClassType *);
string getFullyQualifiedTypeName(SgJavaParameterizedType *);
string getTypeName(SgType *);

string normalize(string str);

//
// Attribute used to construct array types.
//
class AstArrayTypeAttribute : public AstAttribute {
public:
    SgArrayType *arrayType;

    AstArrayTypeAttribute(SgArrayType *array_type) : arrayType(array_type) {}

    SgArrayType *getArrayType() { return arrayType; }
};


//
// Attribute used to construct pointer types.  These are not valid Java types. However,
// they are required for multi-dimensional arrays in the Rose representation.
//
class AstPointerTypeAttribute : public AstAttribute {
public:
    SgPointerType *pointerType;

    AstPointerTypeAttribute(SgPointerType *pointer_type) : pointerType(pointer_type) {}

    SgPointerType *getPointerType() { return pointerType; }
};


//
// Attribute used to construct parameterized types.
//
class AstParameterizedTypeAttribute : public AstAttribute {
private:
    SgClassType *rawType;
    list<SgJavaParameterizedType *> parameterizedTypes;

public:
    AstParameterizedTypeAttribute(SgClassType *rawType_) : rawType(rawType_) {}

    SgJavaParameterizedType *findOrInsertParameterizedType(SgTemplateParameterPtrList &newArgs) {
        //
        // Keep track of parameterized types in a table so as not to duplicate them.
        //
        for (list<SgJavaParameterizedType *>::iterator type_it = parameterizedTypes.begin(); type_it != parameterizedTypes.end(); type_it++) {
            SgTemplateParameterList *type_arg_list = (*type_it) -> get_type_list();
            if (type_arg_list) {
                SgTemplateParameterPtrList args = type_arg_list -> get_args();
                if (args.size() == newArgs.size()) {
                    SgTemplateParameterPtrList::iterator arg_it = args.begin(),
                                                         newArg_it = newArgs.begin();
                    for (; arg_it != args.end(); arg_it++, newArg_it++) {
                        SgType *type1 = (*arg_it) -> get_type(),
                               *type2 = (*newArg_it) -> get_type();
                        if (type1 != type2) {
                            break;
                        }
                    }

                    if (arg_it == args.end()) { // Found a match!
                        return (*type_it);
                    }
                }
            }
        }

        //
        // This parameterized type does not yet exist. Create it, store it in the table and return it.
        //
        SgClassDeclaration *classDeclaration = isSgClassDeclaration(rawType -> getAssociatedDeclaration());
        ROSE_ASSERT(classDeclaration != NULL);
        SgTemplateParameterList *typeParameterList = new SgTemplateParameterList();
        typeParameterList -> set_args(newArgs);
        SgJavaParameterizedType *parameterizedType = new SgJavaParameterizedType(classDeclaration, rawType, typeParameterList);

        ROSE_ASSERT(parameterizedType != NULL);
        ROSE_ASSERT(parameterizedType -> get_raw_type() != NULL);
        ROSE_ASSERT(parameterizedType -> get_type_list() != NULL);

        parameterizedTypes.push_front(parameterizedType);

        return parameterizedType;
    }
};


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
            cerr << "***Popping Component node " << n -> class_name() << endl;
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
            cerr << "Invalid attempt to pop a Component node of type "
                     << n -> class_name()
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
        if (SgProject::get_verbose() > 0) {
            cerr << "***Pushing Stack node " << n -> class_name() << endl; 
            cerr.flush();
        }
        push_front(n);
    }

    SgScopeStatement *pop() {
        ROSE_ASSERT(size() > 0);
        SgScopeStatement *n = front();
        if (SgProject::get_verbose() > 0) {
            cerr << "***Popping Stack node " << n -> class_name() << endl;
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
        else if (! isSgClassDefinition(n) -> attributeExists("namespace")) {
            cerr << "Invalid attempt to pop a Stack node of type "
                     << n -> class_name()
                     << " without the attribute \"namespace\" as a package"
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

void setJavaSourcePosition(SgLocatedNode *locatedNode, JavaSourceCodePosition *posInfo);
void setJavaSourcePosition(SgLocatedNode *locatedNode, JNIEnv *env, jobject jToken);
void setJavaSourcePositionUnavailableInFrontend(SgLocatedNode *locatedNode);

// *********************************************

string convertJavaPackageNameToCxxString(JNIEnv *env, const jstring &java_string);
string convertJavaStringValToWString(JNIEnv *env, const jstring &java_string);
string convertJavaStringToCxxString(JNIEnv *env, const jstring &java_string);

SgMemberFunctionDeclaration *buildDefiningMemberFunction(const SgName &inputName, SgClassDefinition *classDefinition, int num_arguments, JNIEnv *env, jobject methodLoc, jobject argsLoc);
SgMemberFunctionDeclaration *lookupMemberFunctionDeclarationInClassScope(SgClassDefinition *classDefinition, const SgName &function_name, int num_arguments);
SgMemberFunctionDeclaration *lookupMemberFunctionDeclarationInClassScope(SgClassDefinition *classDefinition, const SgName &function_name, list<SgType *> &);
SgMemberFunctionDeclaration *findMemberFunctionDeclarationInClass(SgClassDefinition *classDefinition, const SgName &function_name, list<SgType *>& types);
SgMemberFunctionSymbol *findFunctionSymbolInClass(SgClassDefinition *classDefinition, const SgName &function_name, list<SgType *> &);

SgClassDeclaration *buildJavaClass (const SgName &className, SgScopeStatement *scope, JNIEnv *env, jobject jToken);

SgVariableDeclaration *buildSimpleVariableDeclaration(const SgName &name, SgType *type);

list<SgName> generateQualifierList (const SgName &classNameWithQualification);

bool isCompatibleTypes(SgType *source, SgType *target);

// It might be that this function should take a "const SgName &" instead of a "string".
SgClassSymbol *lookupSymbolFromQualifiedName(string className);

SgType *lookupTypeByName(SgName &packageName, SgName &typeName, int num_dimensions);

//! Support to get current class scope.
SgClassDefinition *getCurrentTypeDefinition();

//! Support to get current class scope.
SgFunctionDefinition *getCurrentMethodDefinition();

//! Support for identification of symbols using simple names in a given scope.
SgClassSymbol *lookupSimpleNameTypeInClass(const SgName &name, SgClassDefinition *classDefinition);
SgVariableSymbol *lookupSimpleNameVariableInClass(const SgName &name, SgClassDefinition *classDefinition);

//! Support for identification of variable symbols using simple names.
SgVariableSymbol *lookupVariableByName(const SgName &name);

//! Support for identification of label symbols using simple names.
SgJavaLabelSymbol *lookupLabelByName(const SgName &name);

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
