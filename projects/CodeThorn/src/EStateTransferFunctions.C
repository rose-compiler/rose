#include "sage3basic.h"
#include "EStateTransferFunctions.h"
#include "Analyzer.h"

using namespace std;
using namespace Sawyer::Message;

Sawyer::Message::Facility CodeThorn::EStateTransferFunctions::logger;

namespace CodeThorn {

  EStateTransferFunctions::EStateTransferFunctions () {
    initDiagnostics();
  }
void EStateTransferFunctions::initDiagnostics() {
  static bool initialized = false;
  if (!initialized) {
    initialized = true;
    logger = Sawyer::Message::Facility("CodeThorn::EStateTransferFunctions", Rose::Diagnostics::destination);
    Rose::Diagnostics::mfacilities.insertAndAdjust(logger);
  }
}

  void EStateTransferFunctions::setAnalyzer(Analyzer* analyzer) {
    _analyzer=analyzer;
  }

  Analyzer* EStateTransferFunctions::getAnalyzer() {
    return _analyzer;
  }

  ExprAnalyzer* EStateTransferFunctions::getExprAnalyzer() {
    ROSE_ASSERT(_analyzer);
    return getAnalyzer()->getExprAnalyzer();
  }

  EState EStateTransferFunctions::createEState(Label label, CallString cs, PState pstate, ConstraintSet cset) {
    return getAnalyzer()->createEState(label,cs,pstate,cset);
  }

  EState EStateTransferFunctions::createEState(Label label, CallString cs, PState pstate, ConstraintSet cset, InputOutput io) {
    return getAnalyzer()->createEState(label,cs,pstate,cset,io);
  }

  std::list<EState> EStateTransferFunctions::transferFunctionCallLocalEdge(Edge edge, const EState* estate) {
    Label lab=estate->label();
    EState currentEState=*estate;
    PState currentPState=*currentEState.pstate();
    ConstraintSet cset=*currentEState.constraints();
    if(_analyzer->getOptionsRef().rers.rersBinary) {
      // logger[DEBUG]<<"ESTATE: "<<estate->toString(&variableIdMapping)<<endl;
      SgNode* nodeToAnalyze=getLabeler()->getNode(edge.source());
      if(SgFunctionCallExp* funCall=SgNodeHelper::Pattern::matchFunctionCall(nodeToAnalyze)) {
        ROSE_ASSERT(funCall);
        string funName=SgNodeHelper::getFunctionName(funCall);
        if(funName=="calculate_outputFP") {
          // RERS global vars binary handling
          PState _pstate=*estate->pstate();
          RERS_Problem::rersGlobalVarsCallInitFP(getAnalyzer(),_pstate, omp_get_thread_num());
#if 0
          //input variable passed as a parameter (obsolete since usage of script "transform_globalinputvar")
          int rers_result=RERS_Problem::calculate_outputFP(argument);
          (void) RERS_Problem::calculate_outputFP(argument);
#else
          (void) RERS_Problem::calculate_outputFP( omp_get_thread_num() );
          int rers_result=RERS_Problem::output[omp_get_thread_num()];
#endif
          //logger[DEBUG]<< "Called calculate_outputFP("<<argument<<")"<<" :: result="<<rers_result<<endl;
          if(rers_result<=-100) {
            // we found a failing assert
            // = rers_result*(-1)-100 : rers error-number
            // = -160 : rers globalError (2012 only)
            int index=((rers_result+100)*(-1));
            ROSE_ASSERT(index>=0 && index <=99);
            getAnalyzer()->binaryBindingAssert[index]=true;
            //reachabilityResults.reachable(index); //previous location in code
            //logger[DEBUG]<<"found assert Error "<<index<<endl;
            ConstraintSet _cset=*estate->constraints();
            InputOutput _io;
            _io.recordFailedAssert();
            // error label encoded in the output value, storing it in the new failing assertion EState
            PState newPstate  = _pstate;
            //newPstate[globalVarIdByName("output")]=CodeThorn::AbstractValue(rers_result);
            getExprAnalyzer()->writeToMemoryLocation(lab,&newPstate,
                                                     AbstractValue::createAddressOfVariable(getAnalyzer()->globalVarIdByName("output")),
                                                     CodeThorn::AbstractValue(rers_result));
            EState _eState=getAnalyzer()->createEState(edge.target(),estate->callString,newPstate,_cset,_io);
            return getAnalyzer()->elistify(_eState);
          }
          RERS_Problem::rersGlobalVarsCallReturnInitFP(getAnalyzer(),_pstate, omp_get_thread_num());
          InputOutput newio;
          if (rers_result == -2) {
            newio.recordVariable(InputOutput::STDERR_VAR,getAnalyzer()->globalVarIdByName("input"));
          }
          // TODO: _pstate[VariableId(output)]=rers_result;
          // matches special case of function call with return value, otherwise handles call without return value (function call is matched above)
          if(SgNodeHelper::Pattern::matchExprStmtAssignOpVarRefExpFunctionCallExp(nodeToAnalyze)) {
            SgNode* lhs=SgNodeHelper::getLhs(SgNodeHelper::getExprStmtChild(nodeToAnalyze));
            VariableId lhsVarId;
            bool isLhsVar=getAnalyzer()->exprAnalyzer.checkIfVariableAndDetermineVarId(lhs,lhsVarId);
            ROSE_ASSERT(isLhsVar); // must hold
            // logger[DEBUG]<< "lhsvar:rers-result:"<<lhsVarId.toString()<<"="<<rers_result<<endl;
            //_pstate[lhsVarId]=AbstractValue(rers_result);
            getExprAnalyzer()->writeToMemoryLocation(lab,&_pstate,AbstractValue::createAddressOfVariable(lhsVarId),AbstractValue(rers_result));
            ConstraintSet _cset=*estate->constraints();
            //_cset.removeAllConstraintsOfVar(lhsVarId);
            EState _eState=createEState(edge.target(),estate->callString,_pstate,_cset,newio);
            return getAnalyzer()->elistify(_eState);
          } else {
            ConstraintSet _cset=*estate->constraints();
            EState _eState=createEState(edge.target(),estate->callString,_pstate,_cset,newio);
            return getAnalyzer()->elistify(_eState);
          }
          cout <<"PState:"<< _pstate<<endl;
          logger[ERROR] <<"RERS-MODE: call of unknown function."<<endl;
          exit(1);
          // _pstate now contains the current state obtained from the binary
        }
        // logger[DEBUG]<< "@LOCAL_EDGE: function call:"<<SgNodeHelper::nodeToString(funCall)<<endl;
      }
    }
    return getAnalyzer()->elistify();
  }
  
}
