#ifndef ESTATE_TRANSFER_FUNCTIONS
#define ESTATE_TRANSFER_FUNCTIONS

#include "EState.h"
#include "DFTransferFunctions.h"

namespace CodeThorn {
  class Analyzer;
  class ExprAnalyzer;
  
  class EStateTransferFunctions : public DFTransferFunctions {
  public:
    EStateTransferFunctions();
    static void initDiagnostics();
    std::list<EState> transferFunctionCallLocalEdge(Edge edge, const EState* estate);
    void setAnalyzer(CodeThorn::Analyzer* analyzer);
    Analyzer* getAnalyzer();
    ExprAnalyzer* getExprAnalyzer();
    EState createEState(Label label, CallString cs, PState pstate, ConstraintSet cset);
    EState createEState(Label label, CallString cs, PState pstate, ConstraintSet cset, InputOutput io);
    static Sawyer::Message::Facility logger;
  protected:
    CodeThorn::Analyzer* _analyzer;
  };
}

#endif
