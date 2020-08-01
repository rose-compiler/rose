#ifndef ESTATE_TRANSFER_FUNCTIONS
#define ESTATE_TRANSFER_FUNCTIONS

#include "EState.h"
#include "DFTransferFunctions.h"
#include "CTIOLabeler.h"

namespace CodeThorn {
  class Analyzer;
  class ExprAnalyzer;
  
  class EStateTransferFunctions : public DFTransferFunctions {
  public:
    EStateTransferFunctions();
    static void initDiagnostics();
    void setAnalyzer(CodeThorn::Analyzer* analyzer);
    Analyzer* getAnalyzer();
    ExprAnalyzer* getExprAnalyzer();
    CTIOLabeler* getLabeler() override;
    VariableIdMappingExtended* getVariableIdMapping() override;
    
    EState createEState(Label label, CallString cs, PState pstate, ConstraintSet cset);
    EState createEState(Label label, CallString cs, PState pstate, ConstraintSet cset, InputOutput io);
    static Sawyer::Message::Facility logger;
    bool getOptionOutputWarnings();
    
    std::list<EState> transferFunctionCallLocalEdge(Edge edge, const EState* estate);
    std::list<EState> transferFunctionCall(Edge edge, const EState* estate);
    std::list<EState> transferFunctionCallExternal(Edge edge, const EState* estate);
    std::list<EState> transferFunctionCallReturn(Edge edge, const EState* estate);
    std::list<EState> transferFunctionExit(Edge edge, const EState* estate);
    std::list<EState> transferReturnStmt(Edge edge, const EState* estate);
    std::list<EState> transferCaseOptionStmt(SgCaseOptionStmt* stmt,Edge edge, const EState* estate);
    std::list<EState> transferDefaultOptionStmt(SgDefaultOptionStmt* stmt,Edge edge, const EState* estate);
    std::list<EState> transferVariableDeclaration(SgVariableDeclaration* decl,Edge edge, const EState* estate);
    std::list<EState> transferExprStmt(SgNode* nextNodeToAnalyze1, Edge edge, const EState* estate);
    std::list<EState> transferIdentity(Edge edge, const EState* estate);
    std::list<EState> transferAssignOp(SgAssignOp* assignOp, Edge edge, const EState* estate);
    std::list<EState> transferIncDecOp(SgNode* nextNodeToAnalyze2, Edge edge, const EState* estate);
#if 0
    std::list<EState> transferTrueFalseEdge(SgNode* nextNodeToAnalyze2, Edge edge, const EState* estate);
#endif
    std::list<EState> elistify();
    std::list<EState> elistify(EState res);

  protected:
    CodeThorn::Analyzer* _analyzer;
  };
}

#endif
