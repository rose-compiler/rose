#ifndef CODE_GENERATOR_VISITOR
#define CODE_GENERATOR_VISITOR

#include <stack>

#include <rosetollvm/LLVMVisitor.h>
#include <rosetollvm/LLVMAstAttributes.h>

class CodeGeneratorVisitor : public LLVMVisitor {
public:
    CodeGeneratorVisitor(Option &option_, Control &control_) : LLVMVisitor(option_, control_),
                                                               current_function_decls(NULL)
    {}

    void processRemainingFunctions();

protected:

    FunctionAstAttribute *current_function_decls;

    std::vector<LLVMAstAttributes *> revisitAttributes;

    /**
     * This stack keeps track of loop statements in order to be able to process break and continue statements.
     */
    std::stack<SgScopeStatement *> scopeStack;

    /**
     * This stack keeps track of the current switch statement, if any, is being processed.
     */
    std::stack<SgSwitchStatement *> switchStack;

    /**
     * If an initializer is encapsulated in a cast expression, return the real value.
     */
    SgValueExp *isValueInitializer(SgAssignInitializer *assign_init) {
        SgExpression *expression = assign_init -> get_operand();
        while (isSgCastExp(expression)) {
            expression = isSgCastExp(expression) -> get_operand();
        }
        return isSgValueExp(expression);
    }

    bool isUnsignedType(SgType *);
    bool isFloatType(SgType *);
    void preprocessGlobalInitializer(SgExpression *);
    std::string getGlobalReference(SgExpression *);

    void generateGlobals();
    void genGlobalAggregateInitialization(SgAggregateInitializer *);
    void genLocalAggregateInitialization(std::string var_name, SgAggregateInitializer *);
    void genBinaryCompareOperation(SgBinaryOp *, std::string);
    void genAddOrSubtractOperation(SgBinaryOp *, std::string);
    void genAddOrSubtractOperationAndAssign(SgBinaryOp *, std::string);
    void genBasicBinaryOperation(SgBinaryOp *, std::string);
    void genBasicBinaryOperationAndAssign(SgBinaryOp *, std::string);
    void genDivideBinaryOperation(SgBinaryOp *, std::string);
    void genDivideBinaryOperationAndAssign(SgBinaryOp *, std::string);

    void virtual preOrderVisit(SgNode *);
    void virtual postOrderVisit(SgNode *);
};

#endif
