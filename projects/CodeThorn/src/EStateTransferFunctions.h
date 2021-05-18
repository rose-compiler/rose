#ifndef ESTATE_TRANSFER_FUNCTIONS
#define ESTATE_TRANSFER_FUNCTIONS

#include "EState.h"
#include "DFTransferFunctions.h"
#include "CTIOLabeler.h"

namespace CodeThorn {
  class CTAnalysis;
  class ExprAnalyzer;
  
  class EStateTransferFunctions : public DFTransferFunctions {
  public:
    EStateTransferFunctions();
    static void initDiagnostics();
    // must be set
    void setAnalyzer(CodeThorn::CTAnalysis* analyzer);
    CTAnalysis* getAnalyzer();
    // obtained from analyzer
    ExprAnalyzer* getExprAnalyzer();
    // obtained from analyzer
    CTIOLabeler* getLabeler() override;
    // obtained from analyzer
    VariableIdMappingExtended* getVariableIdMapping() override;
    
    
    EState createEState(Label label, CallString cs, PState pstate, ConstraintSet cset);
    EState createEState(Label label, CallString cs, PState pstate, ConstraintSet cset, InputOutput io);
    static Sawyer::Message::Facility logger;
    bool getOptionOutputWarnings();
    
    std::list<EState> transferFunctionCallLocalEdge(Edge edge, const EState* estate);
    std::list<EState> transferFunctionCall(Edge edge, const EState* estate);
    std::list<EState> transferFunctionCallExternal(Edge edge, const EState* estate);
    std::list<EState> transferFunctionCallReturn(Edge edge, const EState* estate);
    std::list<EState> transferFunctionEntry(Edge edge, const EState* estate);
    std::list<EState> transferFunctionExit(Edge edge, const EState* estate);
    std::list<EState> transferReturnStmt(Edge edge, const EState* estate);

    std::list<EState> transferCaseOptionStmt(SgCaseOptionStmt* stmt,Edge edge, const EState* estate);
    std::list<EState> transferDefaultOptionStmt(SgDefaultOptionStmt* stmt,Edge edge, const EState* estate);
    std::list<EState> transferVariableDeclaration(SgVariableDeclaration* decl,Edge edge, const EState* estate);

    std::list<EState> transferExprStmt(SgNode* nextNodeToAnalyze1, Edge edge, const EState* estate);
    std::list<EState> transferIdentity(Edge edge, const EState* estate);
    std::list<EState> transferAssignOp(SgAssignOp* assignOp, Edge edge, const EState* estate);
    std::list<EState> transferIncDecOp(SgNode* nextNodeToAnalyze2, Edge edge, const EState* estate);
    std::list<EState> transferAsmStmt(Edge edge, const EState* estate);
    std::list<EState> transferGnuExtensionStmtExpr(SgNode* nextNodeToAnalyze1, Edge edge, const EState* estate);

    // special case, called from transferFunctionCall
    std::list<EState> transferForkFunction(Edge edge, const EState* estate, SgFunctionCallExp* funCall);
    std::list<EState> transferForkFunctionWithExternalTargetFunction(Edge edge, const EState* estate, SgFunctionCallExp* funCall);
#if 0
    std::list<EState> transferTrueFalseEdge(SgNode* nextNodeToAnalyze2, Edge edge, const EState* estate);
#endif
    std::list<EState> elistify();
    std::list<EState> elistify(EState res);

    void initializeGlobalVariablesNew(SgProject* root, EState& estate);
    // modifies PState with written initializers
    EState analyzeVariableDeclaration(SgVariableDeclaration* decl,EState currentEState, Label targetLabel);
    PState analyzeSgAggregateInitializer(VariableId initDeclVarId, SgAggregateInitializer* aggregateInitializer,PState pstate, /* for evaluation only  */ EState currentEState);

    // uses ExprAnalyzer to compute the result. Limits the number of results to one result only. Does not permit state splitting.
    // requires normalized AST
    AbstractValue singleValevaluateExpression(SgExpression* expr,EState currentEState);
    
    // only used in hybrid prototype binding
    VariableId globalVarIdByName(std::string varName);
    
  protected:
    void setElementSize(VariableId variableId, SgType* elementType);
    CodeThorn::CTAnalysis* _analyzer;
    std::string _rersHybridOutputFunctionName="calculate_output"; // only used if rersmode is active
  private:
    // only used in RERS mapping for hybrid output function name (transferFunctionCallLocalEdge)
    std::map<std::string,VariableId> globalVarName2VarIdMapping;
  };
}

#endif
