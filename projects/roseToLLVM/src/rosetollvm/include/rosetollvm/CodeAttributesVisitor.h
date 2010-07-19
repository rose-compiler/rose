#ifndef CODE_ATTRIBUTES_VISITOR
#define CODE_ATTRIBUTES_VISITOR

#include <stack>
#include <iostream>

#include <rosetollvm/LLVMVisitor.h>

class CodeAttributesVisitor : public LLVMVisitor {
public:
    CodeAttributesVisitor(Option &option_, Control &control_) : LLVMVisitor(option_, control_),
                                                                current_function_attribute(NULL)
    {}

    void processRemainingComponents();

protected:

    FunctionAstAttribute *current_function_attribute;

    std::vector<LLVMAstAttributes *> revisitAttributes;

    std::vector<SgSourceFile *> sourceFiles;

    void processVariableDeclaration(SgInitializedName *);

    /**
     * This stack keeps track of the current switch statement, if any, is being processed.
     */
    std::stack<SgSwitchStatement *> switchStack;

    /**
     * This stack keeps track of loop statements in order to be able to process break and continue statements.
     */
    std::stack<SgScopeStatement *> scopeStack;

    bool isPrimitiveValue(SgExpression *n) {
        return isSgBoolValExp(n) ||
               isSgCharVal(n) ||
               isSgUnsignedCharVal(n) ||
               isSgShortVal(n) ||
               isSgUnsignedShortVal(n) ||
               isSgIntVal(n) ||
               isSgEnumVal(n) ||
               isSgUnsignedIntVal(n) ||
               isSgLongIntVal(n) ||
               isSgUnsignedLongVal(n) ||
               isSgLongLongIntVal(n) ||
               isSgUnsignedLongLongIntVal(n) ||
               isSgFloatVal(n) ||
               isSgDoubleVal(n) ||
               isSgLongDoubleVal(n);
    }

    SgFunctionDefinition *getFunctionDefinition(SgScopeStatement *scope) {
        SgFunctionDefinition *fd = isSgFunctionDefinition(scope);
        if (fd)
            return fd;
        if (isSgGlobal(scope))
            return NULL;
        return getFunctionDefinition(scope -> get_scope());
    }

    void checkVariableDeclaration(SgVarRefExp *);
    void checkFunctionDeclaration(SgFunctionRefExp *);
    void tagAggregate(SgAggregateInitializer *, SgType *, bool);

    SgNode *subtreeContainsNode(SgNode *, VariantT);
    bool hasChildren(SgNode *);

    void virtual preOrderVisit(SgNode *);

    void addBooleanExtensionAttributeIfNeeded(SgExpression *);
    void addIntegralConversionAttributeIfNeeded(SgBinaryOp *);
    void checkIntegralOperation(SgBinaryOp *);

    void virtual postOrderVisit(SgNode *);
};

#endif
