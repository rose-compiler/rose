#ifndef CODE_ATTRIBUTES_VISITOR
#define CODE_ATTRIBUTES_VISITOR

#include <stack>
#include <iostream>
#include <rosetollvm/LLVMVisitor.h>

class CodeAttributesVisitor : public LLVMVisitor {
public:
    CodeAttributesVisitor(Option &option_, Control &control_) : LLVMVisitor(option_, control_)
    {}

    void processRemainingComponents();

    void setupAdHocVisitor(LLVMAstAttributes *attributes) {
        setAttributes(attributes);
    }
      
protected:

    /**
     * \brief The stack of functions being translated.
     *
     * Nested function definitions are not permitted in C99 and earlier,
     * but nested function prototypes are permitted in C99 (and C89
     * according to gcc -std=c89 -pedantic).  Thus, the size of the
     * stack can reach at most 2: the current function definition and
     * some function prototype within it.
     */
    std::stack<FunctionAstAttribute *> current_function_attribute;

    std::vector<LLVMAstAttributes *> revisitAttributes;

    std::vector<SgSourceFile *> sourceFiles;

    void oldprocessVariableDeclaration(SgInitializedName *);
    void preprocessVariableDeclaration(SgInitializedName *);
    void postprocessVariableDeclaration(SgInitializedName *);

    /**
     * This stack keeps track of the current switch statement, if any, is being processed.
     */
    std::stack<SgSwitchStatement *> switchStack;

    /**
     * This stack keeps track of loop statements in order to be able to process break and continue statements.
     */
    std::stack<SgScopeStatement *> scopeStack;

    SgFunctionDefinition *getFunctionDefinition(SgScopeStatement *scope) {
        SgFunctionDefinition *fd = isSgFunctionDefinition(scope);
        if (fd)
            return fd;
        if (isSgGlobal(scope))
            return NULL;
        return getFunctionDefinition(scope -> get_scope());
    }

    long long computeCaseValue(SgExpression *);

    void checkVariableDeclaration(SgVarRefExp *);
    void checkFunctionDeclaration(SgFunctionRefExp *);
    void tagAggregate(SgAggregateInitializer *, SgType *, bool);

    SgNode *subtreeContainsNode(SgNode *, VariantT);
    bool hasChildren(SgNode *);

    virtual bool preVisitEnter(SgNode *);
    virtual void preVisit(SgNode *);
    virtual void preVisitExit(SgNode *);

    void addBooleanCast(SgExpression *);
    void promoteExpression(SgExpression *n, SgType *target_type);
    void demoteExpression(SgExpression *n, SgType *target_type);
    void addBooleanExtensionAttributeIfNeeded(SgExpression *, SgType *target_type = NULL);
    void addConversionAttributeIfNeeded(SgBinaryOp *);
    void checkIntegralOperation(SgBinaryOp *);

    virtual bool postVisitEnter(SgNode *);
    virtual void postVisit(SgNode *);
    virtual void postVisitExit(SgNode *);

};

#endif
