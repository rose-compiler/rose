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

    void processDimensionExpressions();
    void processRemainingFunctions();

    void setupAdHocVisitor(LLVMAstAttributes *attributes) {
        setAttributes(attributes);
    }
      
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
     * \brief Series of consecutive pragmas preceding the current node.
     *
     * Each SgPragma has an SgPragmaDeclaration as its parent.  If there
     * is a non-pragma sibling node immediately following the
     * SgPragmaDeclaration, this vector is cleared at the end of the
     * preVisit for that sibling.  If not, this vector is cleared at
     * the beginning of the postVisit for the SgPragmaDeclaration's
     * parent.  Thus, if pragmas should be recorded for a particular
     * node type, check precedingPragmas in that node type's preVisit.
     */
    std::vector<SgPragma *> precedingPragmas;

    std::string getGlobalReference(SgExpression *);

    void generateGlobals();
    void genGlobalExpressionInitialization(SgInitializedName *, std::string , SgExpression *);
    void genGlobalAggregateInitialization(SgInitializedName *, SgAggregateInitializer *);
    void genBinaryCompareOperation(SgBinaryOp *, std::string, std::string const &);
    void genZeroCompareOperation(SgExpression *, std::string const &);
    void genAddOrSubtractOperation(SgBinaryOp *, std::string, std::string const &);
    void genAddOrSubtractOperationAndAssign(SgBinaryOp *, std::string, std::string const &);
    void genBasicBinaryOperation(SgBinaryOp *, std::string, std::string const &, bool type_from_lhs = false);
    void genBasicBinaryOperationAndAssign(SgBinaryOp *, std::string, std::string const &, bool op_signedness = false);
    void genDivideBinaryOperation(SgBinaryOp *, std::string, std::string const &);
    void genDivideBinaryOperationAndAssign(SgBinaryOp *, std::string, std::string const &);

    SgFunctionType *getFunctionType(SgType *);

    /**
     * TODO: Obsolete !
    void generateArraySizeCode(const std::string &, SgType *, SgArrayType *);
    */

    virtual bool preVisitEnter(SgNode *);
    virtual void preVisit(SgNode *);
    virtual void preVisitExit(SgNode *);

    virtual bool postVisitEnter(SgNode *);
    virtual void postVisit(SgNode *);
    virtual void postVisitExit(SgNode *);
};

#endif
