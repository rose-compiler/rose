#include "sage3basic.h"
#include "EStateTransferFunctions.h"
#include "CTAnalysis.h"
#include "AstUtility.h"
#include "Miscellaneous2.h"

#include "CodeThornException.h"
#include "CTAnalysis.h" // dependency on analyzer->transferAssignOp 
#include "CppStdUtilities.h"
#include "CodeThornCommandLineOptions.h"
#include "CodeThornLib.h"
#include "PredefinedSemanticFunctions.h"
#include "AstTerm.h"
#include <map>
#include <iomanip>

using namespace std;
using namespace CodeThorn;
using namespace Sawyer::Message;

Sawyer::Message::Facility CodeThorn::EStateTransferFunctions::logger;

namespace CodeThorn {

  AbstractValue::Operator EStateTransferFunctions::sgNodeToAbstractValueOperator(SgNode* node) {
    VariantT variant=node->variantT();
    static std::map<VariantT,AbstractValue::Operator>
      mapping={
	       {V_SgMinusOp,AbstractValue::Operator::UnaryMinus},
	       {V_SgNotOp,AbstractValue::Operator::Not},
	       {V_SgBitComplementOp,AbstractValue::Operator::BitwiseComplement},
	       {V_SgAddOp,AbstractValue::Operator::Add},
	       {V_SgSubtractOp,AbstractValue::Operator::Sub},
	       {V_SgMultiplyOp,AbstractValue::Operator::Mul},
	       {V_SgDivideOp,AbstractValue::Operator::Div},
	       {V_SgModOp,AbstractValue::Operator::Mod},
	       {V_SgBitAndOp,AbstractValue::Operator::BitwiseAnd},
	       {V_SgBitOrOp,AbstractValue::Operator::BitwiseOr},
	       {V_SgBitXorOp,AbstractValue::Operator::BitwiseXor},
	       {V_SgLshiftOp,AbstractValue::Operator::BitwiseShiftLeft},
	       {V_SgRshiftOp,AbstractValue::Operator::BitwiseShiftRight},
	       {V_SgEqualityOp,AbstractValue::Operator::Eq},
	       {V_SgNotEqualOp,AbstractValue::Operator::NotEq},
	       {V_SgLessThanOp,AbstractValue::Operator::Less},
	       {V_SgLessOrEqualOp,AbstractValue::Operator::LessOrEq},
	       {V_SgGreaterOrEqualOp,AbstractValue::Operator::MoreOrEq},
	       {V_SgGreaterThanOp,AbstractValue::Operator::More},

    };
    return mapping[variant];

  }
  
  EStateTransferFunctions::EStateTransferFunctions () {
    initDiagnostics();
    initViolatingLocations();
  }
  void EStateTransferFunctions::initDiagnostics() {
    static bool initialized = false;
    if (!initialized) {
      initialized = true;
      logger = Sawyer::Message::Facility("CodeThorn::EStateTransferFunctions", Rose::Diagnostics::destination);
      Rose::Diagnostics::mfacilities.insertAndAdjust(logger);
    }
  }
  CTIOLabeler* EStateTransferFunctions::getLabeler() {
    ROSE_ASSERT(_analyzer);
    return _analyzer->getLabeler();
  }
  VariableIdMappingExtended* EStateTransferFunctions::getVariableIdMapping() {
    ROSE_ASSERT(_analyzer);
    return _analyzer->getVariableIdMapping();
  }

  void EStateTransferFunctions::setOptionOutputWarnings(bool flag) {
    _optionOutputWarnings=flag;
  }

  bool EStateTransferFunctions::getOptionOutputWarnings() {
    return _optionOutputWarnings;
  }

  void EStateTransferFunctions::setAnalyzer(CTAnalysis* analyzer) {
    _analyzer=analyzer;
  }

  CTAnalysis* EStateTransferFunctions::getAnalyzer() {
    return _analyzer;
  }
  
  EState EStateTransferFunctions::createEState(Label label, CallString cs, PState pstate, ConstraintSet cset) {
    EState estate=createEStateInternal(label,pstate,cset);
    estate.callString=cs;
    estate.io.recordNone(); // create NONE not bot by default
    return estate;
  }

  EState EStateTransferFunctions::createEState(Label label, CallString cs, PState pstate, ConstraintSet cset, InputOutput io) {
    EState estate=createEStateInternal(label,pstate,cset);
    estate.callString=cs;
    estate.io=io;
    return estate;
  }

  // does not use a context
  EState EStateTransferFunctions::createEStateInternal(Label label, PState pstate, ConstraintSet cset) {
    // here is the best location to adapt the analysis results to certain global restrictions
    if(_analyzer->isActiveGlobalTopify()) {
#if 1
      AbstractValueSet varSet=pstate.getVariableIds();
      for(AbstractValueSet::iterator i=varSet.begin();i!=varSet.end();++i) {
	if(_analyzer->variableValueMonitor.isHotVariable(_analyzer,*i)) {
	  //ROSE_ASSERT(false); // this branch is live
	  _analyzer->topifyVariable(pstate, cset, *i);
	}
      }
#else
      //pstate.topifyState();
#endif
      // set cset in general to empty cset, otherwise cset can grow again arbitrarily
      ConstraintSet cset0;
      cset=cset0;
    }
    const PState* newPStatePtr=_analyzer->processNewOrExisting(pstate);
    const ConstraintSet* newConstraintSetPtr=_analyzer->processNewOrExisting(cset);
    EState estate=EState(label,newPStatePtr,newConstraintSetPtr);
    estate.io.recordNone();
    return estate;
  }

  bool EStateTransferFunctions::isApproximatedBy(const EState* es1, const EState* es2) {
    return es1->isApproximatedBy(es2);
  }

  EState EStateTransferFunctions::combine(const EState* es1, const EState* es2) {
    ROSE_ASSERT(es1->label()==es2->label());
    ROSE_ASSERT(es1->constraints()==es2->constraints()); // pointer equality
    if(es1->callString!=es2->callString) {
      if(_analyzer->getOptionOutputWarnings()) {
	SAWYER_MESG(logger[WARN])<<"combining estates with different callstrings at label:"<<es1->label().toString()<<endl;
	SAWYER_MESG(logger[WARN])<<"cs1: "<<es1->callString.toString()<<endl;
	SAWYER_MESG(logger[WARN])<<"cs2: "<<es2->callString.toString()<<endl;
      }
    }
    PState ps1=*es1->pstate();
    PState ps2=*es2->pstate();

    InputOutput io;
    if(es1->io.isBot()) {
      io=es2->io;
    } else if(es2->io.isBot()) {
      io=es1->io;
    } else {
      ROSE_ASSERT(es1->io==es2->io);
      io=es1->io;
    }

    return createEState(es1->label(),es1->callString,PState::combine(ps1,ps2),*es1->constraints(),io);
  }
  
  std::list<EState> EStateTransferFunctions::elistify() {
    std::list<EState> resList;
    return resList;
  }

  std::list<EState> EStateTransferFunctions::elistify(EState res) {
    std::list<EState> resList;
    resList.push_back(res);
    return resList;
  }
  
  std::list<EState> EStateTransferFunctions::transferFunctionCallLocalEdge(Edge edge, const EState* estate) {
    Label lab=estate->label();
    EState currentEState=*estate;
    PState currentPState=*currentEState.pstate();
    ConstraintSet cset=*currentEState.constraints();
    if(_analyzer->getOptionsRef().rers.rersBinary) {
      SgNode* nodeToAnalyze=getLabeler()->getNode(edge.source());
      if(SgFunctionCallExp* funCall=SgNodeHelper::Pattern::matchFunctionCall(nodeToAnalyze)) {
        ROSE_ASSERT(funCall);
        string funName=SgNodeHelper::getFunctionName(funCall);
        if(funName==_rersHybridOutputFunctionName) {
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
            writeToMemoryLocation(lab,&newPstate,
				  AbstractValue::createAddressOfVariable(globalVarIdByName("output")),
				  CodeThorn::AbstractValue(rers_result));
            EState _eState=createEState(edge.target(),estate->callString,newPstate,_cset,_io);
            return getAnalyzer()->elistify(_eState);
          }
          RERS_Problem::rersGlobalVarsCallReturnInitFP(getAnalyzer(),_pstate, omp_get_thread_num());
          InputOutput newio;
          if (rers_result == -2) {
            newio.recordVariable(InputOutput::STDERR_VAR,globalVarIdByName("input"));
          }
          // TODO: _pstate[VariableId(output)]=rers_result;
          // matches special case of function call with return value, otherwise handles call without return value (function call is matched above)
          if(SgNodeHelper::Pattern::matchExprStmtAssignOpVarRefExpFunctionCallExp(nodeToAnalyze)) {
            SgNode* lhs=SgNodeHelper::getLhs(SgNodeHelper::getExprStmtChild(nodeToAnalyze));
            VariableId lhsVarId;
            bool isLhsVar=checkIfVariableAndDetermineVarId(lhs,lhsVarId);
            ROSE_ASSERT(isLhsVar); // must hold
            // logger[DEBUG]<< "lhsvar:rers-result:"<<lhsVarId.toString()<<"="<<rers_result<<endl;
            //_pstate[lhsVarId]=AbstractValue(rers_result);
            writeToMemoryLocation(lab,&_pstate,AbstractValue::createAddressOfVariable(lhsVarId),AbstractValue(rers_result));
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
  
  std::list<EState> EStateTransferFunctions::transferFunctionCall(Edge edge, const EState* estate) {
    // 1) obtain actual parameters from source
    // 2) obtain formal parameters from target
    // 3) eval each actual parameter and assign result to formal parameter in state
    // 4) create new estate and update callstring (context sensitive analysis)
    SAWYER_MESG(logger[TRACE])<<"transferFunctionCall: "<<getLabeler()->getNode(edge.source())->unparseToString()<<endl;
    Label currentLabel=estate->label();
    EState currentEState=*estate;
    PState currentPState=*currentEState.pstate();
    ConstraintSet cset=*currentEState.constraints();

    // ad 1)
    SgFunctionCallExp* funCall=SgNodeHelper::Pattern::matchFunctionCall(getLabeler()->getNode(edge.source()));
    ROSE_ASSERT(funCall);
    //FUNCTION CALL:funcPtr(data) L:53:SgFunctionCallExp(SgVarRefExp:funcPtr,SgExprListExp(SgVarRefExp:data))
    //cout<<"DEBUG: FUNCTION CALL:"<<funCall->unparseToString()<<" L:"<<edge.target().toString()<<":"<<AstTerm::astTermWithNullValuesToString(funCall)<<endl;
    if(_analyzer->getOptionsRef().rers.rersBinary) {
      // if rers-binary function call is selected then we skip the static analysis for this function (specific to rers)
      string funName=SgNodeHelper::getFunctionName(funCall);
      if(funName==_rersHybridOutputFunctionName) {
        // logger[DEBUG]<< "rers-binary mode: skipped static-analysis call."<<endl;
        return elistify();
      }
    }

    // cases: get_function() :
    // C function call        : FunctionRefExp
    // C function pointer call: VarRefExp
    // member function call (stack)      : SgDotExp(SgVarRefExp:a,SgMemberFunctionRefExp:m)
    // member function call (reference)  : SgDotExp(SgVarRefExp:r,SgMemberFunctionRefExp:m)
    // member functino call (pointer)    : SgArrowExp(SgVarRefExp:p,SgMemberFunctionRefExp:m)

    // check if function pointer call (function=VarRefExp) [otherwise it is a direct function call: function=FunctionRefExp]
    if(SgExpression* funExp=funCall->get_function()) {
      if(SgVarRefExp* varRefExp=isSgVarRefExp(funExp)) {
        //cout<<"Function pointer call: VarRefExp: "<<varRefExp->unparseToString()<<endl;
        // determine if the edge leads to the called function
        Label targetLabel=edge.target();
        VariableId varId=getVariableIdMapping()->variableId(varRefExp);
        //cout<<"DEBUG: function pointer var id: "<<varId.toString(getVariableIdMapping())<<endl;
        AbstractValue varAddress=AbstractValue::createAddressOfVariable(varId);
        //cout<<"DEBUG: function pointer var address: "<<varAddress.toString(getVariableIdMapping())<<endl;
        AbstractValue funcPtrVal=estate->pstate()->readFromMemoryLocation(varAddress);
        //cout<<"DEBUG: function pointer value: "<<funcPtrVal.toString(getVariableIdMapping())<<": isFunPtr:"<<funcPtrVal.isFunctionPtr()<<endl;
        if(funcPtrVal.isFunctionPtr()&&!funcPtrVal.isTop()&&!funcPtrVal.isBot()) {
          Label  funTargetLabel=funcPtrVal.getLabel();
          if(funTargetLabel!=targetLabel) {
            //infeasable path
            //cout<<"DEBUG: infeasable path: "<<funCall->unparseToString()<<":"<<funcPtrVal.toString(getVariableIdMapping())<<endl;
            return elistify();
          } else {
            // continue on this path, function pointer is referring to the target label's function entry
            //cout<<"Resolved function pointer"<<endl;
          }
        } else {
          //cerr<<"INFO: function pointer is top or bot. Not supported: "<<funCall->unparseToString()<<":"<<funcPtrVal.toString(getVariableIdMapping())<<endl;
          // continue but pass the information now to *all* outgoing static edges (maximum imprecision)
        }
      }
    }
  
    SgExpressionPtrList& actualParameters=SgNodeHelper::getFunctionCallActualParameterList(funCall);
    // ad 2)
    // check for function pointer label

    SgFunctionDefinition* funDef=isSgFunctionDefinition(getLabeler()->getNode(edge.target()));
    SgInitializedNamePtrList& formalParameters=SgNodeHelper::getFunctionDefinitionFormalParameterList(funDef);
    ROSE_ASSERT(funDef);
    // ad 3)
    PState newPState=currentPState;
    SgInitializedNamePtrList::iterator i=formalParameters.begin();
    SgExpressionPtrList::iterator j=actualParameters.begin();
    while(i!=formalParameters.end() || j!=actualParameters.end()) {
      SgInitializedName* formalParameterName=*i;
      ROSE_ASSERT(formalParameterName);
      // test formal parameter (instead of argument type) to allow for expressions in arguments
      VariableId formalParameterVarId=_analyzer->getVariableIdMapping()->variableId(formalParameterName);
      AbstractValue evalResultValue;
      if(_analyzer->getVariableIdMapping()->isOfClassType(formalParameterVarId)) {
        if(getOptionOutputWarnings()) {
          cout<<"Warning: imprecision: "<<SgNodeHelper::sourceLineColumnToString(funCall)<< ": passing of class/Struct/Union types per value as function parameters (assuming top)."<<endl;
        }
        evalResultValue=AbstractValue::createTop();
      } else {
        // VariableName varNameString=name->get_name();
        SgExpression* actualParameterExpr=*j;
        ROSE_ASSERT(actualParameterExpr);
        // check whether the actualy parameter is a single variable: In this case we can propagate the constraints of that variable to the formal parameter.
        // pattern: call: f(x), callee: f(int y) => constraints of x are propagated to y
        VariableId actualParameterVarId;
        ROSE_ASSERT(actualParameterExpr);
#if 0
        if(getExprAnalyzer.checkIfVariableAndDetermineVarId(actualParameterExpr,actualParameterVarId)) {
          // propagate constraint from actualParamterVarId to formalParameterVarId
          cset.addAssignEqVarVar(formalParameterVarId,actualParameterVarId);
        }
#endif
        // general case: the actual argument is an arbitrary expression (including a single variable)
        list<SingleEvalResultConstInt> evalResultList=evaluateExpression(actualParameterExpr,currentEState);
        if(evalResultList.size()==0) {
          SAWYER_MESG(logger[FATAL])<<"Internal error: no state computed for argument evaluation at: "<<SgNodeHelper::sourceLineColumnToString(getLabeler()->getNode(edge.source()))<<endl;
          SAWYER_MESG(logger[FATAL])<<"Argument expression: "<<actualParameterExpr->unparseToString()<<endl;
          SAWYER_MESG(logger[FATAL])<<"EState: "<<currentEState.toString(getVariableIdMapping())<<endl;
          exit(1);
        }
        list<SingleEvalResultConstInt>::iterator resultListIter=evalResultList.begin();
        SingleEvalResultConstInt evalResult=*resultListIter;
        if(evalResultList.size()>1) {
          SAWYER_MESG(logger[ERROR]) <<"multi-state generating operators in function call parameters not supported."<<endl;
          exit(1);
        }
        evalResultValue=evalResult.value();
      }
      // above evaluateExpression does not use constraints (par3==false). Therefore top vars remain top vars
      initializeMemoryLocation(currentLabel,&newPState,AbstractValue::createAddressOfVariable(formalParameterVarId),evalResultValue);
      ++i;++j;
    }
    // assert must hold if #formal-params==#actual-params (TODO: default values)
    ROSE_ASSERT(i==formalParameters.end() && j==actualParameters.end());

    // ad 4
    CallString cs=currentEState.callString;
    if(_analyzer->getOptionContextSensitiveAnalysis()) {
      cs=transferFunctionCallContext(cs, currentEState.label());
    }
    EState newEState=createEState(edge.target(),cs,newPState,cset);
    return elistify(newEState);
  }

  std::list<EState> EStateTransferFunctions::transferReturnStmt(Edge edge, const EState* estate) {
    Label lab=estate->label();
    EState currentEState=*estate;
    CallString cs=currentEState.callString;
    PState currentPState=*currentEState.pstate();
    ConstraintSet cset=*currentEState.constraints();
    ROSE_ASSERT(_analyzer);
    ROSE_ASSERT(_analyzer->getCFAnalyzer());
    SgNode* nextNodeToAnalyze1=_analyzer->getCFAnalyzer()->getNode(edge.source());
    ROSE_ASSERT(nextNodeToAnalyze1);
    SgNode* expr=SgNodeHelper::getFirstChild(nextNodeToAnalyze1);

    if(isSgNullExpression(expr)) {
      // null expr is a no-op
      return elistify(createEState(edge.target(),cs,currentPState,cset));
    } else {
      VariableId returnVarId;
#pragma omp critical(VAR_ID_MAPPING)
      {
	returnVarId=_analyzer->getVariableIdMapping()->createUniqueTemporaryVariableId(string("$return"));
      }

      // MS 04/09/2020: removed all old code here and function analyzeAssignRhs. Replaced it with function call 'evaluateExpression'
      // old version available in commit: 9645ce260b0bd44207d342a760afc27620380140
      list<SingleEvalResultConstInt> rhsResList=evaluateExpression(expr,currentEState, CodeThorn::EStateTransferFunctions::MODE_VALUE);
      AbstractValue rhsResultValue;
      if(rhsResList.size()==0) {
	// analyzed program execution ends, return empty set to indicate that execution does not continue
	// TODO: may also return an error state here
	std::list<EState> emptyList;
	return emptyList;
      } else if(rhsResList.size()>1) {
	cerr<<"Error: return expression gives more than one result. Not normalized."<<endl;
	exit(1);
      } else {
	ROSE_ASSERT(rhsResList.size()==1);
	rhsResultValue=(*rhsResList.begin()).value();
      }
      PState newPState=currentPState;
      initializeMemoryLocation(lab,&newPState,returnVarId,rhsResultValue);
      return elistify(createEState(edge.target(),cs,newPState,cset));
    }
  }

  std::list<EState> EStateTransferFunctions::transferFunctionCallReturn(Edge edge, const EState* estate) {
    EState currentEState=*estate;
    PState currentPState=*currentEState.pstate();
    ConstraintSet cset=*currentEState.constraints();

    // determine functionCallLabel corresponding to functioncallReturnLabel.
    Label functionCallReturnLabel=edge.source();
    SgNode* node=getLabeler()->getNode(functionCallReturnLabel);
    Label functionCallLabel=getLabeler()->functionCallLabel(node);
    SAWYER_MESG(logger[TRACE])<<"FunctionCallReturnTransfer: "<<functionCallLabel.toString()<<":"<<functionCallReturnLabel.toString()<<" cs: "<<estate->callString.toString()<<endl;

    CallString cs=currentEState.callString;
    if(_analyzer->getOptionContextSensitiveAnalysis()) {
      if(getLabeler()->isExternalFunctionCallLabel(functionCallLabel)) {
	// nothing to do for external function call (label is not added
	// to callstring by external function call)
      } else {
	if(isFeasiblePathContext(cs,functionCallLabel)) {
	  cs.removeLastLabel();
	} else {
	  if(cs.isEmpty()) {
	    SAWYER_MESG(logger[WARN])<<"Empty context on non-feasable path at label "<<functionCallLabel.toString()<<endl;
	  }
	  // definitely not feasible path, do not return a state
	  SAWYER_MESG(logger[TRACE])<<"definitly on non-feasable path at label (no next state)"<<functionCallLabel.toString()<<endl;
	  std::list<EState> emptyList;
	  return emptyList;
	}
      }
    }

    // 1. handle the edge as outgoing edge
    SgNode* nextNodeToAnalyze1=_analyzer->getCFAnalyzer()->getNode(edge.source());
    ROSE_ASSERT(nextNodeToAnalyze1);

    if(SgNodeHelper::Pattern::matchReturnStmtFunctionCallExp(nextNodeToAnalyze1)) {
      // case 1: return f(); pass estate through
      EState newEState=currentEState;
      newEState.setLabel(edge.target());
      return elistify(newEState);
    } else if(SgNodeHelper::Pattern::matchExprStmtAssignOpVarRefExpFunctionCallExp(nextNodeToAnalyze1)) {
      // case 2a: x=f(); bind variable x to value of $return
      if(_analyzer->getOptionsRef().rers.rersBinary) {
	if(SgFunctionCallExp* funCall=SgNodeHelper::Pattern::matchFunctionCall(nextNodeToAnalyze1)) {
	  string funName=SgNodeHelper::getFunctionName(funCall);
	  if(funName==_rersHybridOutputFunctionName) {
	    EState newEState=currentEState;
	    newEState.setLabel(edge.target());
	    newEState.callString=cs;
	    return elistify(newEState);
	  }
	}
      }
      SgNode* lhs=SgNodeHelper::getLhs(SgNodeHelper::getExprStmtChild(nextNodeToAnalyze1));
      VariableId lhsVarId;
      bool isLhsVar=checkIfVariableAndDetermineVarId(lhs,lhsVarId);
      ROSE_ASSERT(isLhsVar); // must hold
      PState newPState=*currentEState.pstate();
      // we only create this variable here to be able to find an existing $return variable!
      VariableId returnVarId;
#pragma omp critical(VAR_ID_MAPPING)
      {
	returnVarId=_analyzer->getVariableIdMapping()->createUniqueTemporaryVariableId(string("$return"));
      }

      if(newPState.varExists(returnVarId)) {
	AbstractValue evalResult=readFromMemoryLocation(currentEState.label(),&newPState,returnVarId);
	initializeMemoryLocation(currentEState.label(),&newPState,lhsVarId,evalResult);
	newPState.deleteVar(returnVarId); // remove $return from state
	return elistify(createEState(edge.target(),cs,newPState,cset));
      } else {
	// no $return variable found in state. This can be the case for an extern function.
	// alternatively a $return variable could be added in the external function call to
	// make this handling here uniform
	// for external functions no constraints are generated in the call-return node
	return elistify(createEState(edge.target(),cs,newPState,cset));
      }
    } else if(SgNodeHelper::Pattern::matchFunctionCallExpInVariableDeclaration(nextNodeToAnalyze1)) {
      // case 2b: Type x=f(); bind variable x to value of $return for function call in declaration
      if(_analyzer->getOptionsRef().rers.rersBinary) {
	if(SgFunctionCallExp* funCall=SgNodeHelper::Pattern::matchFunctionCall(nextNodeToAnalyze1)) {
	  string funName=SgNodeHelper::getFunctionName(funCall);
	  if(funName==_rersHybridOutputFunctionName) {
	    EState newEState=currentEState;
	    newEState.setLabel(edge.target());
	    newEState.callString=cs;
	    return elistify(newEState);
	  }
	}
      }

      // these two lines are different to x=f();
      SgVariableDeclaration* varDecl=SgNodeHelper::Pattern::matchVariableDeclarationWithFunctionCall(nextNodeToAnalyze1);
      VariableId lhsVarId=getVariableIdMapping()->variableId(varDecl);
      ROSE_ASSERT(lhsVarId.isValid());

      PState newPState=*currentEState.pstate();
      // this variable is created here only to be able to find an existing $return variable in the state
      VariableId returnVarId;
#pragma omp critical(VAR_ID_MAPPING)
      {
	returnVarId=_analyzer->getVariableIdMapping()->createUniqueTemporaryVariableId(string("$return"));
      }

      if(newPState.varExists(returnVarId)) {
	AbstractValue evalResult=readFromMemoryLocation(currentEState.label(),&newPState,returnVarId);
	initializeMemoryLocation(currentEState.label(),&newPState,lhsVarId,evalResult);
	newPState.deleteVar(returnVarId); // remove $return from state
	SAWYER_MESG(logger[TRACE])<<"transferFunctionCallReturn(initialization): LHSVAR:"<<getVariableIdMapping()->variableName(lhsVarId)<<" value: "<<evalResult.toString()<<endl;
	return elistify(createEState(edge.target(),cs,newPState,cset));
      } else {
	// no $return variable found in state. This can be the case for an extern function.
	// alternatively a $return variable could be added in the external function call to
	// make this handling here uniform
	// for external functions no constraints are generated in the call-return node
	SAWYER_MESG(logger[TRACE])<<"-------------------------------------------------"<<endl;
	SAWYER_MESG(logger[TRACE])<<"transferFunctionCallReturn: Variable declaration with function call: no function-return variable $return found! @ "<<SgNodeHelper::sourceLineColumnToString(nextNodeToAnalyze1)<<":"<<nextNodeToAnalyze1->unparseToString()<<endl;
	SAWYER_MESG(logger[TRACE])<<estate->toString(getVariableIdMapping())<<endl;
	SAWYER_MESG(logger[TRACE])<<"-------------------------------------------------"<<endl;
	return elistify(createEState(edge.target(),cs,newPState,cset));
      }
    } else  if(SgNodeHelper::Pattern::matchExprStmtFunctionCallExp(nextNodeToAnalyze1)) {
      // case 3: f(); remove $return from state (discard value)
      PState newPState=*currentEState.pstate();
      VariableId returnVarId;
#pragma omp critical(VAR_ID_MAPPING)
      {
	returnVarId=_analyzer->getVariableIdMapping()->createUniqueTemporaryVariableId(string("$return"));
      }
      // no effect if $return does not exist
      newPState.deleteVar(returnVarId);
      return elistify(createEState(edge.target(),cs,newPState,cset));
    } else if (SgNodeHelper::matchExtendedNormalizedCall(nextNodeToAnalyze1)) {
      // Handles Constructor Calls
      // \pp TEMPORARY SOLUTION TO MAKE CODE PASS THROUGH
      // \todo THIS IS WRONG 
      // (1) matchExtendedNormalizedCall is too coarse grain, as it covers
      //     both constructor and function calls
      // (2) distinction between constructor call of a base class and an
      //     variable initialization needs to be made. 
      EState newEState=currentEState;
      newEState.setLabel(edge.target());
      return elistify(newEState);
    } else {
      logger[FATAL] << "function call-return from unsupported call type:"<<nextNodeToAnalyze1->unparseToString()<<endl;
      exit(1);
    }
  }

  std::list<EState> EStateTransferFunctions::transferAsmStmt(Edge edge, const EState* estate) {
    // ignore AsmStmt
    return transferIdentity(edge,estate);
  }
  
  std::list<EState> EStateTransferFunctions::transferFunctionEntry(Edge edge, const EState* estate) {
    Label lab=estate->label();
    SgNode* node=_analyzer->getLabeler()->getNode(lab);
    SgFunctionDefinition* funDef=isSgFunctionDefinition(node);
    if(funDef) {
      string functionName=SgNodeHelper::getFunctionName(node);
      string fileName=SgNodeHelper::sourceFilenameToString(node);
      SAWYER_MESG(logger[INFO])<<"Analyzing Function:"<<fileName<<" : "<<functionName<<endl;
      SgInitializedNamePtrList& formalParameters=SgNodeHelper::getFunctionDefinitionFormalParameterList(funDef);
      SAWYER_MESG(logger[TRACE])<<"Function:"<<functionName<<" Parameters: ";
      for(auto fParam : formalParameters) {
        SAWYER_MESG(logger[TRACE])<<fParam->unparseToString()<<" sym:"<<fParam->search_for_symbol_from_symbol_table()<<endl;
      }
      SAWYER_MESG(logger[TRACE])<<endl;
    }
    return transferIdentity(edge,estate);
  }
  
  std::list<EState> EStateTransferFunctions::transferFunctionExit(Edge edge, const EState* estate) {
    EState currentEState=*estate;
    if(SgFunctionDefinition* funDef=isSgFunctionDefinition(getLabeler()->getNode(edge.source()))) {
      // 1) determine all local variables (including formal parameters) of function
      // 2) delete all local variables from state
      // 2a) remove variable from state
      // 2b) remove all constraints concerning this variable
      // 3) create new EState and return

      // ad 1)
      set<SgVariableDeclaration*> varDecls=SgNodeHelper::localVariableDeclarationsOfFunction(funDef);
      // ad 2)
      ConstraintSet cset=*currentEState.constraints();
      PState newPState=*(currentEState.pstate());
      VariableIdMapping::VariableIdSet localVars=_analyzer->getVariableIdMapping()->determineVariableIdsOfVariableDeclarations(varDecls);
      SgInitializedNamePtrList& formalParamInitNames=SgNodeHelper::getFunctionDefinitionFormalParameterList(funDef);
      VariableIdMapping::VariableIdSet formalParams=_analyzer->getVariableIdMapping()->determineVariableIdsOfSgInitializedNames(formalParamInitNames);
      VariableIdMapping::VariableIdSet vars=localVars+formalParams;
      set<string> names=_analyzer->variableIdsToVariableNames(vars);

      for(VariableIdMapping::VariableIdSet::iterator i=vars.begin();i!=vars.end();++i) {
	VariableId varId=*i;
	newPState.deleteVar(varId);
	//cset.removeAllConstraintsOfVar(varId);
      }
      // ad 3)
      return elistify(createEState(edge.target(),estate->callString,newPState,cset));
    } else {
      logger[FATAL] << "no function definition associated with function exit label."<<endl;
      exit(1);
    }
  }

  // called from transferForkFunction
  std::list<EState> EStateTransferFunctions::transferForkFunctionWithExternalTargetFunction(Edge edge, const EState* estate, SgFunctionCallExp* funCall) {
    //_analyzer->recordExternalFunctionCall(funCall); funcall would be forkFunction
    // arg5 is expression with functino pointer to external function
    list<EState> estateList;
    EState estate1=*estate;
    estate1.setLabel(edge.target());
    // no forked state
    estateList.push_back(estate1);
    return estateList;
  }

  std::list<EState> EStateTransferFunctions::transferForkFunction(Edge edge, const EState* estate, SgFunctionCallExp* funCall) {
    EState currentEState=*estate;
    CallString cs=currentEState.callString;
    PState currentPState=*currentEState.pstate();
    ConstraintSet cset=*currentEState.constraints();

    SgExpressionPtrList& actualParameters=SgNodeHelper::getFunctionCallActualParameterList(funCall);
    SAWYER_MESG(logger[TRACE])<<getAnalyzer()->_ctOpt.forkFunctionName<<" #args:"<<actualParameters.size()<<endl;
    // get 5th argument
    SgExpressionPtrList::iterator pIter=actualParameters.begin();
    for(int j=1;j<5;j++) {
      ++pIter;
    }        
    SgExpression* actualParameterExpr=*pIter;
    // general case: the actual argument is an arbitrary expression (including a single variable)
    list<SingleEvalResultConstInt> evalResultList=evaluateExpression(actualParameterExpr,currentEState);
    if(evalResultList.size()==0) {
      SAWYER_MESG(logger[FATAL])<<"Internal error: no state computed for argument evaluation at: "<<SgNodeHelper::sourceLineColumnToString(getLabeler()->getNode(edge.source()))<<endl;
      SAWYER_MESG(logger[FATAL])<<"Argument expression: "<<actualParameterExpr->unparseToString()<<endl;
      SAWYER_MESG(logger[FATAL])<<"EState: "<<currentEState.toString(getVariableIdMapping())<<endl;
      exit(1);
    }
    list<SingleEvalResultConstInt>::iterator resultListIter=evalResultList.begin();
    SingleEvalResultConstInt evalResult=*resultListIter;
    if(evalResultList.size()>1) {
      SAWYER_MESG(logger[ERROR]) <<"multi-state generating operators in function call parameters not supported."<<endl;
      exit(1);
    }
    AbstractValue arg5Value=evalResult.value();
    // this result value has to be a function pointer value, create a state (representing the fork), and continue with current state
    if(!arg5Value.isFunctionPtr()) {
      // case where no source exists for function pointer
      // handle like any other external function call
      return transferForkFunctionWithExternalTargetFunction(edge,estate,funCall);
    }
    // TODO: create state with this function label as start state
    EState forkedEState=*estate;
    // set target label in new state to function pointer label
    forkedEState.setLabel(arg5Value.getLabel());
        
    // allow both formats x=f(...) and f(...)
    SgAssignOp* assignOp=isSgAssignOp(AstUtility::findExprNodeInAstUpwards(V_SgAssignOp,funCall));
    if(assignOp) {
      list<EState> estateList1=transferAssignOp(assignOp,edge,estate); // use current estate, do not mix with forked state
      ROSE_ASSERT(estateList1.size()==1);
      EState estate1=*estateList1.begin();
      estate1.setLabel(edge.target());
      list<EState> estateList2;
      estateList2.push_back(estate1);
      estateList2.push_back(forkedEState);
      return estateList2;
    } else {
      list<EState> estateList;
      EState estate1=*estate;
      estate1.setLabel(edge.target());
      estateList.push_back(estate1);
      estateList.push_back(forkedEState);
      return estateList;
    }
  }

  std::list<EState> EStateTransferFunctions::transferFunctionCallExternal(Edge edge, const EState* estate) {
    EState currentEState=*estate;
    CallString cs=currentEState.callString;
    PState currentPState=*currentEState.pstate();
    ConstraintSet cset=*currentEState.constraints();

    // 1. we handle the edge as outgoing edge
    SgNode* nextNodeToAnalyze1=_analyzer->getCFAnalyzer()->getNode(edge.source());
    ROSE_ASSERT(nextNodeToAnalyze1);

    SAWYER_MESG(logger[TRACE]) << "transferFunctionCallExternal: "<<nextNodeToAnalyze1->unparseToString()<<endl;

    //cout<<"DEBUG: transferFunctionCallExternal: "<<nextNodeToAnalyze1->unparseToString()<<endl;

    InputOutput newio;
    Label lab=_analyzer->getLabeler()->getLabel(nextNodeToAnalyze1);

    VariableId varId;
    bool isFunctionCallWithAssignmentFlag=isFunctionCallWithAssignment(lab,&varId);

    SgFunctionCallExp* funCall=SgNodeHelper::Pattern::matchFunctionCall(nextNodeToAnalyze1);
    _analyzer->recordExternalFunctionCall(funCall);
    evaluateFunctionCallArguments(edge,funCall,*estate,false);

    // TODO: check whether the following test is superfluous meanwhile, since isStdInLabel does take NonDetX functions into account
    bool isExternalNonDetXFunction=false;
    if(_analyzer->svCompFunctionSemantics()) {
      if(funCall) {
	string externalFunctionName=SgNodeHelper::getFunctionName(funCall);
	if(externalFunctionName==_analyzer->_externalNonDetIntFunctionName||externalFunctionName==_analyzer->_externalNonDetLongFunctionName) {
	  isExternalNonDetXFunction=true;
	}
      }
    }
    if(isExternalNonDetXFunction || _analyzer->getLabeler()->isStdInLabel(lab,&varId)) {
      if(_analyzer->_inputSequence.size()>0) {
	PState newPState=*currentEState.pstate();
	ConstraintSet newCSet=*currentEState.constraints();
	//newCSet.removeAllConstraintsOfVar(varId);
	list<EState> resList;
	int newValue;
	if(_analyzer->_inputSequenceIterator!=_analyzer->_inputSequence.end()) {
	  newValue=*_analyzer->_inputSequenceIterator;
	  ++_analyzer->_inputSequenceIterator;
	  //cout<<"INFO: input sequence value: "<<newValue<<endl;
	} else {
	  return resList; // return no state (this ends the analysis)
	}
	if(_analyzer->getOptionsRef().inputValuesAsConstraints) {
	  SAWYER_MESG(logger[FATAL])<<"Option input-values-as-constraints no longer supported."<<endl;
	  exit(1);
	  //newCSet.removeAllConstraintsOfVar(varId);
	  //newPState[varId]=CodeThorn::Top();
	  //newPState.writeTopToMemoryLocation(varId);
	  //newCSet.addConstraint(Constraint(Constraint::EQ_VAR_CONST,varId,AbstractValue(newValue)));
	  //ROSE_ASSERT(newCSet.size()>0);
	} else {
	  //newCSet.removeAllConstraintsOfVar(varId);
	  //newPState[varId]=AbstractValue(newValue);
	  writeToMemoryLocation(currentEState.label(),&newPState,AbstractValue::createAddressOfVariable(varId),AbstractValue(newValue));
	}
	newio.recordVariable(InputOutput::STDIN_VAR,varId);
	EState newEState=createEState(edge.target(),cs,newPState,newCSet,newio);
	resList.push_back(newEState);
	return resList;
      } else {
	if(_analyzer->_inputVarValues.size()>0) {
	  // update state (remove all existing constraint on that variable and set it to top)
	  PState newPState=*currentEState.pstate();
	  ConstraintSet newCSet=*currentEState.constraints();
	  //newCSet.removeAllConstraintsOfVar(varId);
	  list<EState> resList;
	  for(set<int>::iterator i=_analyzer->_inputVarValues.begin();i!=_analyzer->_inputVarValues.end();++i) {
	    PState newPState=*currentEState.pstate();
	    if(_analyzer->getOptionsRef().inputValuesAsConstraints) {
	      SAWYER_MESG(logger[FATAL])<<"Option input-values-as-constraints no longer supported."<<endl;
	      exit(1);
	      //newCSet.removeAllConstraintsOfVar(varId);
	      //newPState[varId]=CodeThorn::Top();
	      //newPState.writeTopToMemoryLocation(varId);
	      //newCSet.addConstraint(Constraint(Constraint::EQ_VAR_CONST,varId,AbstractValue(*i)));
	      //assert(newCSet.size()>0);
	    } else {
	      //newCSet.removeAllConstraintsOfVar(varId);
	      // new input value must be const (otherwise constraints must be used)
	      //newPState[varId]=AbstractValue(*i);
	      writeToMemoryLocation(currentEState.label(),&newPState,AbstractValue::createAddressOfVariable(varId),AbstractValue(*i));
	    }
	    newio.recordVariable(InputOutput::STDIN_VAR,varId);
	    EState newEState=createEState(edge.target(),estate->callString,newPState,newCSet,newio);
	    resList.push_back(newEState);
	  }
	  return resList;
	} else {
	  // without specified input values (default mode: analysis performed for all possible input values)
	  // update state (remove all existing constraint on that variable and set it to top)
	  PState newPState=*currentEState.pstate();
	  ConstraintSet newCSet=*currentEState.constraints();
	  // update input var
	  //newCSet.removeAllConstraintsOfVar(varId);
	  newPState.writeTopToMemoryLocation(varId);
	  newio.recordVariable(InputOutput::STDIN_VAR,varId);

	  // external call context
	  // call string is reused from input-estate. An external function call does not change the call string
	  // callReturn node must check for being an external call
	  return elistify(createEState(edge.target(),cs,newPState,newCSet,newio));
	}
      }
    }

    if(_analyzer->getInterpreterMode()!=IM_ENABLED) {
      int constvalue=0;
      if(_analyzer->getLabeler()->isStdOutVarLabel(lab,&varId)) {
	newio.recordVariable(InputOutput::STDOUT_VAR,varId);
	ROSE_ASSERT(newio.var==varId);
	return elistify(createEState(edge.target(),cs,*currentEState.pstate(),*currentEState.constraints(),newio));
      } else if(_analyzer->getLabeler()->isStdOutConstLabel(lab,&constvalue)) {
	newio.recordConst(InputOutput::STDOUT_CONST,constvalue);
	return elistify(createEState(edge.target(),cs,*currentEState.pstate(),*currentEState.constraints(),newio));
      } else if(_analyzer->getLabeler()->isStdErrLabel(lab,&varId)) {
	newio.recordVariable(InputOutput::STDERR_VAR,varId);
	ROSE_ASSERT(newio.var==varId);
	return elistify(createEState(edge.target(),cs,*currentEState.pstate(),*currentEState.constraints(),newio));
      }
    }

    /* handling of specific semantics for external function */
    if(funCall) {
      string funName=SgNodeHelper::getFunctionName(funCall);
      if(_analyzer->svCompFunctionSemantics()) {
	if(funName==_analyzer->_externalErrorFunctionName) {
	  //cout<<"DETECTED error function: "<<_externalErrorFunctionName<<endl;
	  return elistify(_analyzer->createVerificationErrorEState(currentEState,edge.target()));
	} else if(funName==_analyzer->_externalExitFunctionName) {
	  /* the exit function is modeled to terminate the program
	     (therefore no successor state is generated)
	  */
	  return elistify();
	} else {
	  // dispatch all other external function calls to the other
	  // transferFunctions where the external function call is handled as an expression
	  if(isFunctionCallWithAssignmentFlag) {
	    // here only the specific format x=f(...) can exist
	    SgAssignOp* assignOp=isSgAssignOp(AstUtility::findExprNodeInAstUpwards(V_SgAssignOp,funCall));
	    ROSE_ASSERT(assignOp);
	    return transferAssignOp(assignOp,edge,estate);
	  } else {
	    // special case: void function call f(...);
	    list<SingleEvalResultConstInt> res=evalFunctionCall(funCall,currentEState);
	    // build new estate(s) from single eval result list
	    list<EState> estateList;
	    for(list<SingleEvalResultConstInt>::iterator i=res.begin();i!=res.end();++i) {
	      EState estate=(*i).estate;
	      PState newPState=*estate.pstate();
	      ConstraintSet cset=*estate.constraints();
	      estateList.push_back(createEState(edge.target(),cs,newPState,cset));
	    }
	    return estateList;
	  }
	}
      }

      if(getAnalyzer()->_ctOpt.forkFunctionEnabled) {
	if(funName==getAnalyzer()->_ctOpt.forkFunctionName) {
	  return transferForkFunction(edge,estate,funCall);
	}
      }

      if(isFunctionCallWithAssignmentFlag) {
	// here only the specific format x=f(...) can exist
	SgAssignOp* assignOp=isSgAssignOp(AstUtility::findExprNodeInAstUpwards(V_SgAssignOp,funCall));
	ROSE_ASSERT(assignOp);
	return transferAssignOp(assignOp,edge,estate);
      } else {
	// all other cases, evaluate function call as expression
	//cout<<"DEBUG: external function call: "<<funCall->unparseToString()<<"; evaluating as expression."<<endl;
	list<SingleEvalResultConstInt> res2=evaluateExpression(funCall,currentEState);
	ROSE_ASSERT(res2.size()==1);
	SingleEvalResultConstInt evalResult2=*res2.begin();
	SAWYER_MESG(logger[TRACE])<<"EXTERNAL FUNCTION: "<<SgNodeHelper::getFunctionName(funCall)<<" result(added to state):"<<evalResult2.result.toString()<<endl;

	//EState estate2=evalResult2.estate;
	// create new estate with added return variable (for inter-procedural analysis)
	CallString cs=evalResult2.estate.callString;
	PState newPState=*evalResult2.estate.pstate();
	ConstraintSet cset=*evalResult2.estate.constraints();
	VariableId returnVarId;
#pragma omp critical(VAR_ID_MAPPING)
	{
	  returnVarId=_analyzer->getVariableIdMapping()->createUniqueTemporaryVariableId(string("$return"));
	}
	// added function call result value to state. The returnVarId does not correspond to a declaration, and therefore it ise
	// treated as being initialized (and declared).
	initializeMemoryLocation(currentEState.label(),&newPState,returnVarId,evalResult2.result);
	//estate2.setLabel(edge.target());
	return elistify(createEState(edge.target(),cs,newPState,cset,evalResult2.estate.io));
      }
    }
    //cout<<"DEBUG: identity: "<<funCall->unparseToString()<<endl; // fflush is an example in the test cases
    // for all other external functions we use identity as transfer function
    EState newEState=currentEState;
    newEState.io=newio;
    newEState.setLabel(edge.target());
    return elistify(newEState);
  }

  std::list<EState> EStateTransferFunctions::transferDefaultOptionStmt(SgDefaultOptionStmt* defaultStmt,Edge edge, const EState* estate) {
    SAWYER_MESG(logger[TRACE])<<"DEBUG: DEFAULTSTMT: "<<defaultStmt->unparseToString()<<endl;

    Label targetLabel=edge.target();
    CallString cs=estate->callString;
    PState newPState=*estate->pstate();
    ConstraintSet cset=*estate->constraints();
    SgStatement* blockStmt=isSgBasicBlock(defaultStmt->get_parent());
    ROSE_ASSERT(blockStmt);
    SgSwitchStatement* switchStmt=isSgSwitchStatement(blockStmt->get_parent());
    ROSE_ASSERT(switchStmt);
    SgStatement* condStmt=isSgStatement(SgNodeHelper::getCond(switchStmt));
    ROSE_ASSERT(condStmt);
    SgExpression* condExpr=isSgExpression(SgNodeHelper::getExprStmtChild(condStmt));

    EState currentEState=*estate;

    // value of switch expression
    AbstractValue switchCondVal=singleValevaluateExpression(condExpr,currentEState);

    // if there is at least one case that is definitely reachable, then
    // the default (label) is non-reachable (fall-through still
    // applies), otherwise default is may-reachable
    bool defaultReachable=true;

    // create filter for all case labels (TODO: precompute this set)
    set<SgCaseOptionStmt*> caseStmtSet=SgNodeHelper::switchRelevantCaseStmtNodes(blockStmt); // argument must be the blockStmt of swithStmt
    SAWYER_MESG(logger[TRACE])<<"castStmtSet.size(): "<<caseStmtSet.size()<<endl;
    for(set<SgCaseOptionStmt*>::iterator i=caseStmtSet.begin();i!=caseStmtSet.end();++i) {
      SAWYER_MESG(logger[TRACE])<<"switch-stmt (@default): analyzing case "<<(*i)->unparseToString()<<endl;
      SgCaseOptionStmt* caseStmt=*i;
      SgExpression* caseExpr=caseStmt->get_key();
      SgExpression* caseExprOptionalRangeEnd=caseStmt->get_key_range_end();
      if(caseExprOptionalRangeEnd) {
	AbstractValue caseValRangeBegin=singleValevaluateExpression(caseExpr,currentEState);
	AbstractValue caseValRangeEnd=singleValevaluateExpression(caseExprOptionalRangeEnd,currentEState);
	AbstractValue comparisonValBegin=caseValRangeBegin.operatorLessOrEq(switchCondVal);
	AbstractValue comparisonValEnd=caseValRangeEnd.operatorMoreOrEq(switchCondVal);
	if(comparisonValBegin.isTrue()&&comparisonValEnd.isTrue()) {
	  SAWYER_MESG(logger[TRACE])<<"switch-default: non-reachable=false."<<endl;
	  defaultReachable=false;
	  break;
	}
      }
      // value of constant case value
      AbstractValue caseVal=singleValevaluateExpression(caseExpr,currentEState);
      // compare case constant with switch expression value
      SAWYER_MESG(logger[TRACE])<<"switch-default filter cmp: "<<switchCondVal.toString(getVariableIdMapping())<<"=?="<<caseVal.toString(getVariableIdMapping())<<endl;
      // check that not any case label may be equal to the switch-expr value (exact for concrete values)
      AbstractValue comparisonVal=caseVal.operatorEq(switchCondVal);
      // top is *not* considered here, because only a definitive case makes default non-reachable
      if(comparisonVal.isTrue()) {
	// determined that at least one case may be reachable
	SAWYER_MESG(logger[TRACE])<<"switch-default: continuing."<<endl;
	//return elistify(createEState(targetLabel,cs,newPState,cset));
	SAWYER_MESG(logger[TRACE])<<"switch-default: non-reachable=false."<<endl;
	defaultReachable=false;
	break;
      }
    }
    if(defaultReachable) {
      SAWYER_MESG(logger[TRACE])<<"switch-default: reachable."<<endl;
      return elistify(createEState(targetLabel,cs,newPState,cset));
    } else {
      // detected infeasable path (default is not reachable)
      SAWYER_MESG(logger[TRACE])<<"switch-default: infeasable path."<<endl;
      list<EState> emptyList;
      return emptyList;
    }
  }

  std::list<EState> EStateTransferFunctions::transferCaseOptionStmt(SgCaseOptionStmt* caseStmt,Edge edge, const EState* estate) {
    SAWYER_MESG(logger[TRACE])<<"CASESTMT: "<<caseStmt->unparseToString()<<endl;
    Label targetLabel=edge.target();
    CallString cs=estate->callString;
    PState newPState=*estate->pstate();
    ConstraintSet cset=*estate->constraints();
    SgStatement* blockStmt=isSgBasicBlock(caseStmt->get_parent());
    ROSE_ASSERT(blockStmt);
    SgSwitchStatement* switchStmt=isSgSwitchStatement(blockStmt->get_parent());
    ROSE_ASSERT(switchStmt);
    SgStatement* condStmt=isSgStatement(SgNodeHelper::getCond(switchStmt));
    ROSE_ASSERT(condStmt);
    SgExpression* condExpr=isSgExpression(SgNodeHelper::getExprStmtChild(condStmt));

    EState currentEState=*estate;

    // value of switch expression
    AbstractValue switchCondVal=singleValevaluateExpression(condExpr,currentEState);

    SgExpression* caseExpr=caseStmt->get_key();
    SgExpression* caseExprOptionalRangeEnd=caseStmt->get_key_range_end();
    if(caseExprOptionalRangeEnd) {
      AbstractValue caseValRangeBegin=singleValevaluateExpression(caseExpr,currentEState);
      AbstractValue caseValRangeEnd=singleValevaluateExpression(caseExprOptionalRangeEnd,currentEState);
      AbstractValue comparisonValBegin=caseValRangeBegin.operatorLessOrEq(switchCondVal);
      AbstractValue comparisonValEnd=caseValRangeEnd.operatorMoreOrEq(switchCondVal);
      if(comparisonValBegin.isTop()||comparisonValEnd.isTop()||(comparisonValBegin.isTrue()&&comparisonValEnd.isTrue())) {
	SAWYER_MESG(logger[TRACE])<<"switch-case GNU Range: continuing."<<endl;
	return elistify(createEState(targetLabel,cs,newPState,cset));
      } else {
	SAWYER_MESG(logger[TRACE])<<"switch-case GNU Range: infeasable path."<<endl;
	// infeasable path
	list<EState> emptyList;
	return emptyList;
      }
    }
    // value of constant case value
    AbstractValue caseVal=singleValevaluateExpression(caseExpr,currentEState);
    // compare case constant with switch expression value
    SAWYER_MESG(logger[TRACE])<<"switch cmp: "<<switchCondVal.toString(getVariableIdMapping())<<"=?="<<caseVal.toString(getVariableIdMapping())<<endl;
    AbstractValue comparisonVal=caseVal.operatorEq(switchCondVal);
    if(comparisonVal.isTop()||comparisonVal.isTrue()) {
      SAWYER_MESG(logger[TRACE])<<"switch-case: continuing."<<endl;
      return elistify(createEState(targetLabel,cs,newPState,cset));
    } else {
      // infeasable path
      SAWYER_MESG(logger[TRACE])<<"switch-case: infeasable path."<<endl;
      list<EState> emptyList;
      return emptyList;
    }
  }

  std::list<EState> EStateTransferFunctions::transferVariableDeclaration(SgVariableDeclaration* decl, Edge edge, const EState* estate) {
    return elistify(analyzeVariableDeclaration(decl,*estate, edge.target()));
  }

  std::list<EState> EStateTransferFunctions::transferGnuExtensionStmtExpr(SgNode* nextNodeToAnalyze1, Edge edge, const EState* estate) {
    //cout<<"WARNING: ignoring GNU extension StmtExpr (EStateTransferFunctions::transferGnuExtensionStmtExpr)"<<endl;
    return elistify(*estate);
  }

  std::list<EState> EStateTransferFunctions::transferExprStmt(SgNode* nextNodeToAnalyze1, Edge edge, const EState* estate) {
    SgNode* nextNodeToAnalyze2=0;
    if(isSgExprStatement(nextNodeToAnalyze1))
      nextNodeToAnalyze2=SgNodeHelper::getExprStmtChild(nextNodeToAnalyze1);
    if(SgNodeHelper::isForIncExpr(nextNodeToAnalyze1)) {
      nextNodeToAnalyze2=nextNodeToAnalyze1;
    }
    ROSE_ASSERT(nextNodeToAnalyze2);

    //Label newLabel;
    //PState newPState;
    //ConstraintSet newCSet;
    if(edge.isType(EDGE_TRUE) || edge.isType(EDGE_FALSE)) {
      return transferTrueFalseEdge(nextNodeToAnalyze2, edge, estate);
    } else if(SgNodeHelper::isPrefixIncDecOp(nextNodeToAnalyze2) || SgNodeHelper::isPostfixIncDecOp(nextNodeToAnalyze2)) {
      return transferIncDecOp(nextNodeToAnalyze2,edge,estate);
    } else if(SgAssignOp* assignOp=isSgAssignOp(nextNodeToAnalyze2)) {
      return transferAssignOp(assignOp, edge, estate);
    } else if(isSgCompoundAssignOp(nextNodeToAnalyze2)) {
      logger[ERROR] <<"compound assignment operators not supported. Use normalization to eliminate these operators."<<endl;
      logger[ERROR] <<"expr: "<<nextNodeToAnalyze2->unparseToString()<<endl;
      exit(1);
    } else if(isSgConditionalExp(nextNodeToAnalyze2)) {
      // this is meanwhile modeled in the ExprAnalyzer - TODO: utilize as expr-stmt.
      logger[ERROR] <<"found conditional expression outside expression (should have been dispatched to ExprAnalyzer)."<<endl;
      exit(1);
    } else {
      return transferIdentity(edge,estate);
    }
  }

  list<EState> EStateTransferFunctions::transferIdentity(Edge edge, const EState* estate) {
    // nothing to analyze, just create new estate (from same State) with target label of edge
    // can be same state if edge is a backedge to same cfg node
    EState newEState=*estate;
    newEState.setLabel(edge.target());
    return elistify(newEState);
  }


  std::list<EState> EStateTransferFunctions::transferAssignOp(SgAssignOp* nextNodeToAnalyze2, Edge edge, const EState* estate) {
    SAWYER_MESG(logger[TRACE]) << "transferAssignOp:"<<nextNodeToAnalyze2->unparseToString()<<endl;
    auto pList=evalAssignOpMemUpdates(nextNodeToAnalyze2, edge, estate);
    std::list<EState> estateList;
    for (auto p : pList) {
      EState estate=p.first;
      AbstractValue lhsAddress=p.second.first;
      AbstractValue rhsValue=p.second.second;
      Label label=estate.label();
      PState newPState=*estate.pstate();
      ConstraintSet cset=*estate.constraints();
      if(lhsAddress.isReferenceVariableAddress())
	writeToReferenceMemoryLocation(label,&newPState,lhsAddress,rhsValue);
      else
	writeToMemoryLocation(label,&newPState,lhsAddress,rhsValue);
      CallString cs=estate.callString;
      estateList.push_back(createEState(edge.target(),cs,newPState,cset));
    }
    SAWYER_MESG(logger[TRACE]) << "transferAssignOp: finished"<<endl;
    return estateList;
  }


  list<EState> EStateTransferFunctions::transferFailedAssert(Edge edge, const EState* estate) {
    return elistify(_analyzer->createFailedAssertEState(*estate,edge.target()));
  }
  
  list<EState> EStateTransferFunctions::transferIncDecOp(SgNode* nextNodeToAnalyze2, Edge edge, const EState* estate) {
    EState currentEState=*estate;
    CallString cs=estate->callString;

    SgNode* nextNodeToAnalyze3=SgNodeHelper::getUnaryOpChild(nextNodeToAnalyze2);
    VariableId var;
    if(checkIfVariableAndDetermineVarId(nextNodeToAnalyze3,var)) {
      list<SingleEvalResultConstInt> res=evaluateExpression(nextNodeToAnalyze3,currentEState);
      ROSE_ASSERT(res.size()==1); // must hold for currently supported limited form of ++,--
      list<SingleEvalResultConstInt>::iterator i=res.begin();
      EState estate=(*i).estate;
      PState newPState=*estate.pstate();
      ConstraintSet cset=*estate.constraints();

      AbstractValue oldVarVal=readFromMemoryLocation(estate.label(),&newPState,var);
      AbstractValue newVarVal;
      AbstractValue const1=1;
      switch(nextNodeToAnalyze2->variantT()) {
      case V_SgPlusPlusOp:
	newVarVal=AbstractValue::operatorAdd(oldVarVal,const1); // overloaded binary + operator
	break;
      case V_SgMinusMinusOp:
	newVarVal=AbstractValue::operatorSub(oldVarVal,const1); // overloaded binary - operator
	break;
      default:
	logger[ERROR] << "Operator-AST:"<<AstTerm::astTermToMultiLineString(nextNodeToAnalyze2,2)<<endl;
	logger[ERROR] << "Operator:"<<SgNodeHelper::nodeToString(nextNodeToAnalyze2)<<endl;
	logger[ERROR] << "Operand:"<<SgNodeHelper::nodeToString(nextNodeToAnalyze3)<<endl;
	logger[ERROR] <<"programmatic error in handling of inc/dec operators."<<endl;
	exit(1);
      }
      //newPState[var]=varVal;
      // TODOREF
      writeToMemoryLocation(estate.label(),&newPState,AbstractValue::createAddressOfVariable(var),newVarVal);

#if 0
      if(!(*i).result.isTop())
	cset.removeAllConstraintsOfVar(var);
#endif
      list<EState> estateList;
      estateList.push_back(createEState(edge.target(),cs,newPState,cset));
      return estateList;
    } else {
      throw CodeThorn::Exception("Error: currently inc/dec operators are only supported for variables.");
    }
  }

  EState EStateTransferFunctions::analyzeVariableDeclaration(SgVariableDeclaration* decl,EState currentEState, Label targetLabel) {

    /*
      1) declaration of variable or array
      - AggregateInitializer (e.g. T a[]={1,2,3};)
      - AggregateInitializer (e.g. T a[5]={1,2,3};)
      - AssignInitializer (e.g. T x=1+2;)
      2) if array, determine size of array (from VariableIdMapping)
      3) if no size is provided, determine it from the initializer list (and add this information to the variableIdMapping - or update the variableIdMapping).
    */

    SAWYER_MESG(logger[TRACE])<<"analyzeVariableDeclaration:"<<decl->unparseToString()<<" : "<<AstTerm::astTermWithNullValuesToString(decl)<<endl;
    CallString cs=currentEState.callString;
    Label label=currentEState.label();

    const SgInitializedNamePtrList& initNameList=decl->get_variables();
    SgNode* initName0;
    if(initNameList.size()>1) {
      SAWYER_MESG(logger[ERROR])<<"Error: variable declaration contains more than one variable. Normalization required."<<endl;
      SAWYER_MESG(logger[ERROR])<<"Error: "<<decl->unparseToString()<<endl;

      exit(1);
    } else {
      //SgNode* initName0=decl->get_traversalSuccessorByIndex(1); // get-InitializedName
      ROSE_ASSERT(initNameList.size()==1);
      initName0=*initNameList.begin();
    }
    if(initName0!=nullptr) {
      if(SgInitializedName* initName=isSgInitializedName(initName0)) {
	VariableId initDeclVarId=getVariableIdMapping()->variableId(initName);
	// not possible to support yet. getIntValue must succeed on declarations.
	/*
	  if(false && variableValueMonitor.isHotVariable(this,initDeclVarId)) {
	  // DEAD CODE
	  PState newPState=*currentEState.pstate();
	  newPState.reserveMemoryLocation(initDeclVarId);
	  ConstraintSet cset=*currentEState.constraints();
	  InputOutput io;
	  io.recordNone();
	  return createEState(targetLabel,cs,newPState,cset);
	  }
	*/
	// deactivated 05/20/2020
	//      if(getVariableIdMapping()->isOfArrayType(initDeclVarId) && _ctOpt.explicitArrays==false) {
        // in case of a constant array the array (and its members) are not added to the state.
        // they are considered to be determined from the initializer without representing them
        // in the state
        // logger[DEBUG] <<"not adding array to PState."<<endl;
	//  PState newPState=*currentEState.pstate();
	//  ConstraintSet cset=*currentEState.constraints();
	//  return createEState(targetLabel,cs,newPState,cset);
	//}

	ConstraintSet cset=*currentEState.constraints();
	SgInitializer* initializer=initName->get_initializer();
	if(initializer) {
	  //cout<<"DEBUG: decl-init: "<<decl->unparseToString()<<":AST:"<<AstTerm::astTermWithNullValuesToString(initializer)<<endl;
	  // special case that initializer is an assignment (reusing transferAssignOp)
	  if(SgAssignInitializer* assignInit=isSgAssignInitializer(initializer)) {
	    SgExpression* assignInitOperand=assignInit->get_operand_i();
	    ROSE_ASSERT(assignInitOperand);
	    if(SgAssignOp* assignOp=isSgAssignOp(assignInitOperand)) {
	      SAWYER_MESG(logger[TRACE])<<"assignment in initializer: "<<decl->unparseToString()<<endl;
	      //cout<<"DEBUG: assignment in initializer: "<<decl->unparseToString()<<endl;
	      Edge dummyEdge(targetLabel,EDGE_FORWARD,targetLabel); // only target label is used in transferAssignOp
	      //std::list<EState> estateList=transferAssignOp(assignOp, dummyEdge, &currentEState);
	      CodeThorn::EStateTransferFunctions::MemoryUpdateList memUpdList=evalAssignOpMemUpdates(assignOp,dummyEdge,&currentEState);
	      std::list<EState> estateList;
	      ROSE_ASSERT(memUpdList.size()==1);
	      auto memUpd=*memUpdList.begin();
	      // code is normalized, lhs must be a variable : tmpVar= var=val;
	      // store result of assignment in declaration variable
	      EState estate=memUpd.first;
	      PState newPState=*estate.pstate();
	      AbstractValue initDeclVarAddr=AbstractValue::createAddressOfVariable(initDeclVarId);
	      AbstractValue lhsAddr=memUpd.second.first;
	      AbstractValue rhsValue=memUpd.second.second;
	      writeToMemoryLocation(label,&newPState,lhsAddr,rhsValue); // assignment in initializer
	      initializeMemoryLocation(label,&newPState,initDeclVarAddr,rhsValue); // initialization of declared var
	      return createEState(targetLabel,cs,newPState,cset);
	    } else if(SgFunctionRefExp* funRefExp=isSgFunctionRefExp(assignInitOperand)) {
	      //cout<<"DEBUG: DETECTED isSgFunctionRefExp: evaluating expression ..."<<endl;
	      list<SingleEvalResultConstInt> res=evaluateExpression(funRefExp,currentEState);
	      if(res.size()!=1) {
		if(res.size()>1) {
		  SAWYER_MESG(logger[ERROR])<<"Error: multiple results in rhs evaluation."<<endl;
		  SAWYER_MESG(logger[ERROR])<<"expr: "<<SgNodeHelper::sourceLineColumnToString(decl)<<": "<<decl->unparseToString()<<endl;
		  exit(1);
		} else {
		  ROSE_ASSERT(res.size()==0);
		  SAWYER_MESG(logger[TRACE])<<"no results in rhs evaluation (returning top): "<<decl->unparseToString()<<endl;
		}
	      } else {
		ROSE_ASSERT(res.size()==1);
		SingleEvalResultConstInt evalResult=*res.begin();
		SAWYER_MESG(logger[TRACE])<<"rhs eval result 2: "<<evalResult.result.toString()<<endl;
              
		EState estate=evalResult.estate;
		PState newPState=*estate.pstate();
		AbstractValue initDeclVarAddr=AbstractValue::createAddressOfVariable(initDeclVarId);
		initializeMemoryLocation(label,&newPState,initDeclVarAddr,evalResult.value());
		ConstraintSet cset=*estate.constraints();
		return createEState(targetLabel,cs,newPState,cset);
	      }
	    }
	  }
	  if(getVariableIdMapping()->isOfClassType(initDeclVarId)) {
	    SAWYER_MESG(logger[WARN])<<"initialization of structs not supported yet (not added to state) "<<SgNodeHelper::sourceFilenameLineColumnToString(decl)<<endl;
	    // TODO: for(offset(membervar) : membervars {initialize(address(initDeclVarId)+offset,eval(initializer+));}
	    //AbstractValue pointerVal=AbstractValue::createAddressOfVariable(initDeclVarId);
	    // TODO: STRUCT VARIABLE DECLARATION
	    //reserveMemoryLocation(label,&newPState,pointerVal);
	    PState newPState=*currentEState.pstate();
	    return createEState(targetLabel,cs,newPState,cset);
	  }
	  if(getVariableIdMapping()->isOfReferenceType(initDeclVarId)) {
	    SAWYER_MESG(logger[TRACE])<<"initialization of reference:"<<SgNodeHelper::sourceFilenameLineColumnToString(decl)<<endl;
	    SgAssignInitializer* assignInit=isSgAssignInitializer(initializer);
	    ROSE_ASSERT(assignInit);
	    SgExpression* assignInitOperand=assignInit->get_operand_i();
	    ROSE_ASSERT(assignInitOperand);
	    list<SingleEvalResultConstInt> res=evaluateLExpression(assignInitOperand,currentEState);

	    SAWYER_MESG(logger[TRACE])<<"initialization of reference:"<<AstTerm::astTermWithNullValuesToString(assignInitOperand)<<endl;
	    if(res.size()>1) {
	      SAWYER_MESG(logger[ERROR])<<"Error: multiple results in rhs evaluation."<<endl;
	      SAWYER_MESG(logger[ERROR])<<"expr: "<<SgNodeHelper::sourceLineColumnToString(decl)<<": "<<decl->unparseToString()<<endl;
	      exit(1);
	    } else if(res.size()==0) {
	      // TODO: remove this case once initialization is fully supported
	      SAWYER_MESG(logger[TRACE])<<"no results in rhs evaluation (returning top): "<<decl->unparseToString()<<endl;
	      AbstractValue result=AbstractValue::createTop();
	      EState estate=currentEState;
	      PState newPState=*estate.pstate();
	      AbstractValue initDeclVarAddr=AbstractValue::createAddressOfVariable(initDeclVarId);
	      //initDeclVarAddr.setRefType(); // known to be ref from isOfReferenceType above
	      // creates a memory cell in state that contains the address of the referred memory cell
	      initializeMemoryLocation(label,&newPState,initDeclVarAddr,result);
	      ConstraintSet cset=*estate.constraints();
	      return createEState(targetLabel,cs,newPState,cset);
	    }
	    SingleEvalResultConstInt evalResult=*res.begin();
	    SAWYER_MESG(logger[TRACE])<<"rhs (reference init) result: "<<evalResult.result.toString()<<endl;
            
	    EState estate=evalResult.estate;
	    PState newPState=*estate.pstate();
	    AbstractValue initDeclVarAddr=AbstractValue::createAddressOfVariable(initDeclVarId);
	    //initDeclVarAddr.setRefType(); // known to be ref from isOfReferenceType above
	    // creates a memory cell in state that contains the address of the referred memory cell
	    initializeMemoryLocation(label,&newPState,initDeclVarAddr,evalResult.value());
	    ConstraintSet cset=*estate.constraints();
	    return createEState(targetLabel,cs,newPState,cset);
	  }
	  // has aggregate initializer
	  if(SgAggregateInitializer* aggregateInitializer=isSgAggregateInitializer(initializer)) {
	    if(SgArrayType* arrayType=isSgArrayType(aggregateInitializer->get_type())) {
	      SgType* arrayElementType=arrayType->get_base_type();
	      setElementSize(initDeclVarId,arrayElementType);
	      // only set size from aggregate initializer if not known from type
	      if(getVariableIdMapping()->getNumberOfElements(initDeclVarId)==0) {
		getVariableIdMapping()->setNumberOfElements(initDeclVarId, _analyzer->computeNumberOfElements(decl));
	      }
	      PState newPState=*currentEState.pstate();
	      newPState=analyzeSgAggregateInitializer(initDeclVarId, aggregateInitializer,newPState, currentEState);
	      return createEState(targetLabel,cs,newPState,cset);
	    } else {
	      // type not supported yet
	      SAWYER_MESG(logger[WARN])<<"aggregate initializer: unsupported type at: "<<SgNodeHelper::sourceFilenameLineColumnToString(decl)<<" : "<<aggregateInitializer->get_type()->unparseToString()<<endl;
	      // do not modify state. Value remains top.
	      PState newPState=*currentEState.pstate();
	      ConstraintSet cset=*currentEState.constraints();
	      return createEState(targetLabel,cs,newPState,cset);
	    }
	  } else if(SgAssignInitializer* assignInitializer=isSgAssignInitializer(initializer)) {
	    SgExpression* rhs=assignInitializer->get_operand_i();
	    ROSE_ASSERT(rhs);
	    SAWYER_MESG(logger[TRACE])<<"declaration with assign initializer:"<<" lhs:"<<initDeclVarId.toString(getVariableIdMapping())<<" rhs:"<<assignInitializer->unparseToString()<<" decl-term:"<<AstTerm::astTermWithNullValuesToString(initName)<<endl;

	    // only create string in state with variable as pointer-address if it is an array (not for the case it is a char* pointer)
	    // in the case of char* it is handled as a pointer initializer (and the string-pointer is already available in state)
	    if(SgStringVal* stringValNode=isSgStringVal(assignInitializer->get_operand())) {
	      if(isSgArrayType(initName->get_type())) {
		// handle special cases of: char a[]="abc"; char a[4]="abc";
		// TODO: a[5]="ab";
		SAWYER_MESG(logger[TRACE])<<"Initalizing (array) with string: "<<stringValNode->unparseToString()<<endl;
		//size_t stringLen=stringValNode->get_value().size();
		if(getVariableIdMapping()->getNumberOfElements(initDeclVarId)==0) {
		  VariableId stringLiteralId=getVariableIdMapping()->getStringLiteralVariableId(stringValNode);
		  size_t stringLiteralMemoryRegionSize=getVariableIdMapping()->getNumberOfElements(stringLiteralId);
		  getVariableIdMapping()->setNumberOfElements(initDeclVarId,stringLiteralMemoryRegionSize);
		  SAWYER_MESG(logger[TRACE])<<"Determined size of array from literal string memory region size: "<<stringLiteralMemoryRegionSize<<endl;
		} else {
		  SAWYER_MESG(logger[TRACE])<<"Determined size of array from array variable (containing string memory region) size: "<<getVariableIdMapping()->getNumberOfElements(initDeclVarId)<<endl;
		}
		SgType* variableType=initializer->get_type(); // for char and wchar
		setElementSize(initDeclVarId,variableType); // this must be a pointer, if it's not an array
		PState newPState=*currentEState.pstate();
		_analyzer->initializeStringLiteralInState(newPState,stringValNode,initDeclVarId);
		ConstraintSet cset=*currentEState.constraints();
		return createEState(targetLabel,cs,newPState,cset);
	      }
	    }
	    // set type info for initDeclVarId
	    getVariableIdMapping()->setNumberOfElements(initDeclVarId,1); // single variable
	    SgType* variableType=initializer->get_type();
	    setElementSize(initDeclVarId,variableType);

	    // build lhs-value dependent on type of declared variable
	    //AbstractValue lhsAbstractAddress=AbstractValue(initDeclVarId); // creates a pointer to initDeclVar
	    AbstractValue lhsAbstractAddress=AbstractValue::createAddressOfVariable(initDeclVarId); // creates a pointer to initDeclVar
	    list<SingleEvalResultConstInt> res=evaluateExpression(rhs,currentEState);
	    if(res.size()!=1) {
	      if(res.size()>1) {
		SAWYER_MESG(logger[ERROR])<<"Error: multiple results in rhs evaluation."<<endl;
		SAWYER_MESG(logger[ERROR])<<"expr: "<<SgNodeHelper::sourceLineColumnToString(decl)<<": "<<decl->unparseToString()<<endl;
		exit(1);
	      } else {
		ROSE_ASSERT(res.size()==0);
		SAWYER_MESG(logger[TRACE])<<"no results in rhs evaluation (returning top): "<<rhs->unparseToString()<<endl;
		EState estate=currentEState;
		PState newPState=*estate.pstate();
		// writeToMemoryLocation(label,&newPState,lhsAbstractAddress,CodeThorn::Top());
		reserveMemoryLocation(label,&newPState,lhsAbstractAddress);
		ConstraintSet cset=*estate.constraints();
		return createEState(targetLabel,cs,newPState,cset);
	      }
	    }
	    ROSE_ASSERT(res.size()==1);
	    SingleEvalResultConstInt evalResult=*res.begin();
	    SAWYER_MESG(logger[TRACE])<<"rhs eval result 1: "<<evalResult.result.toString()<<endl;

	    EState estate=evalResult.estate;
	    PState newPState=*estate.pstate();
	    initializeMemoryLocation(label,&newPState,lhsAbstractAddress,evalResult.value());
	    ConstraintSet cset=*estate.constraints();
	    return createEState(targetLabel,cs,newPState,cset);
	  } else {
	    SAWYER_MESG(logger[WARN]) << "unsupported initializer in declaration: "<<decl->unparseToString()<<" not adding to state (assuming arbitrary value)"<<endl;
	    PState newPState=*currentEState.pstate();
	    return createEState(targetLabel,cs,newPState,cset);
	  }
	} else {
	  // no initializer (model default cases)
	  ROSE_ASSERT(initName!=nullptr);
	  SgArrayType* arrayType=isSgArrayType(initName->get_type());
	  if(arrayType) {
	    SgType* arrayElementType=arrayType->get_base_type();
	    setElementSize(initDeclVarId,arrayElementType);
	    int numElements=getVariableIdMapping()->getArrayElementCount(arrayType);
	    if(numElements==0) {
	      SAWYER_MESG(logger[TRACE])<<"Number of elements in array is 0 (from variableIdMapping) - evaluating expression"<<endl;
	      std::vector<SgExpression*> arrayDimExps=SageInterface::get_C_array_dimensions(*arrayType);
	      if(arrayDimExps.size()>1) {
		if(_analyzer->getAbstractionMode()==3) {
		  CodeThorn::Exception("multi-dimensional arrays not supported yet.");
		} else {
		  SAWYER_MESG(logger[WARN])<<"multi-dimensional arrays not supported yet. Only linear arrays are supported. Not added to state (assuming arbitrary value)."<<endl;
		}
		// not adding it to state. Will be used as unknown.
		PState newPState=*currentEState.pstate();
		return createEState(targetLabel,cs,newPState,cset);
	      }
	      ROSE_ASSERT(arrayDimExps.size()==1);
	      SgExpression* arrayDimExp=*arrayDimExps.begin();
	      SAWYER_MESG(logger[TRACE])<<"Array dimension expression: "<<arrayDimExp->unparseToString()<<endl;
	      list<SingleEvalResultConstInt> evalResultList=evaluateExpression(arrayDimExp,currentEState);
	      ROSE_ASSERT(evalResultList.size()==1);
	      SingleEvalResultConstInt evalRes=*evalResultList.begin();
	      AbstractValue arrayDimAVal=evalRes.result;
	      SAWYER_MESG(logger[TRACE])<<"Computed array dimension: "<<arrayDimAVal.toString()<<endl;
	      if(arrayDimAVal.isConstInt()) {
		numElements=arrayDimAVal.getIntValue();
		getVariableIdMapping()->setNumberOfElements(initDeclVarId,numElements);
	      } else {
		if(_analyzer->getAbstractionMode()==3) {
		  CodeThorn::Exception("Could not determine size of array (non-const size).");
		}
		// TODO: size of array remains 1?
	      }
	    } else {
	      getVariableIdMapping()->setNumberOfElements(initDeclVarId,numElements);
	    }
	  } else {
	    // set type info for initDeclVarId
	    getVariableIdMapping()->setNumberOfElements(initDeclVarId,1); // single variable
	    SgType* variableType=initName->get_type();
	    setElementSize(initDeclVarId,variableType);
	  }

	  SAWYER_MESG(logger[TRACE])<<"Creating new PState"<<endl;
	  PState newPState=*currentEState.pstate();
	  if(getVariableIdMapping()->isOfArrayType(initDeclVarId)) {
	    SAWYER_MESG(logger[TRACE])<<"PState: upd: array"<<endl;
	    // add default array elements to PState
	    auto length=getVariableIdMapping()->getNumberOfElements(initDeclVarId);
	    if(length>0) {
	      SAWYER_MESG(logger[TRACE])<<"DECLARING ARRAY of size: "<<decl->unparseToString()<<":"<<length<<endl;
	      for(CodeThorn::TypeSize elemIndex=0;elemIndex<length;elemIndex++) {
		AbstractValue newArrayElementAddr=AbstractValue::createAddressOfArrayElement(initDeclVarId,AbstractValue(elemIndex));
		// set default init value
		reserveMemoryLocation(label,&newPState,newArrayElementAddr);
	      }
	    } else {
	      SAWYER_MESG(logger[TRACE])<<"DECLARING ARRAY of unknown size: "<<decl->unparseToString()<<":"<<length<<endl;
	      AbstractValue newArrayElementAddr=AbstractValue::createAddressOfArrayElement(initDeclVarId,AbstractValue(0)); // use elem index 0
	      // set default init value
	      reserveMemoryLocation(label,&newPState,newArrayElementAddr); // TODO: reserve summary memory location	    
	    }

	  } else if(getVariableIdMapping()->isOfClassType(initDeclVarId)) {
	    SAWYER_MESG(logger[TRACE])<<"PState: upd: class"<<endl;
	    // create only address start address of struct (on the
	    // stack) alternatively addresses for all member variables
	    // can be created; however, a member var can only be
	    // assigned by denoting an element relative to the start of
	    // the struct, similar only a pointer can be created. the
	    // value is actually uninitialized and therefore is
	    // implicitly bot.
	    AbstractValue pointerVal=AbstractValue::createAddressOfVariable(initDeclVarId);
	    SAWYER_MESG(logger[TRACE])<<"declaration of struct: "<<getVariableIdMapping()->getVariableDeclaration(initDeclVarId)->unparseToString()<<" : "<<pointerVal.toString(getVariableIdMapping())<<endl;
	    // TODO: STRUCT VARIABLE DECLARATION
	    reserveMemoryLocation(label,&newPState,pointerVal);
	  } else if(getVariableIdMapping()->isOfPointerType(initDeclVarId)) {
	    SAWYER_MESG(logger[TRACE])<<"PState: upd: pointer"<<endl;
	    // create pointer value and set it to top (=any value possible (uninitialized pointer variable declaration))
	    AbstractValue pointerVal=AbstractValue::createAddressOfVariable(initDeclVarId);
	    writeUndefToMemoryLocation(&newPState,pointerVal);
	  } else {
	    // set it to top (=any value possible (uninitialized)) for
	    // all remaining cases. It will become an error-path once
	    // all cases are addressed explicitly above.
	    SAWYER_MESG(logger[TRACE])<<"declaration of variable (other): "<<getVariableIdMapping()->getVariableDeclaration(initDeclVarId)->unparseToString()<<endl;
	    reserveMemoryLocation(label,&newPState,AbstractValue::createAddressOfVariable(initDeclVarId));
	  }
	  SAWYER_MESG(logger[TRACE])<<"Creating new EState"<<endl;
	  return createEState(targetLabel,cs,newPState,cset);
	}
      } else {
	logger[ERROR] << "in declaration (@initializedName) no variable found ... bailing out."<<endl;
	exit(1);
      }
    } else {
      logger[ERROR] << "in declaration: no variable found ... bailing out."<<endl;
      exit(1);
    }
    ROSE_ASSERT(false); // non-reachable
  }

  // set the size of an element determined by this type
  void EStateTransferFunctions::setElementSize(VariableId variableId, SgType* elementType) {
    unsigned int typeSize=getVariableIdMapping()->getTypeSize(elementType);
    if(getVariableIdMapping()->getElementSize(variableId)!=0
       && getVariableIdMapping()->getElementSize(variableId)!=typeSize) {
      SAWYER_MESG(logger[WARN])<<"Element type size mismatch: "
			       <<"EStateTransferFunctions::setElementSize: variableId name: "<<getVariableIdMapping()->variableName(variableId)
			       <<"typesize(from VID):"<<getVariableIdMapping()->getElementSize(variableId)
			       <<" typeSize(fromtype): "<<typeSize<<" of "<<elementType->unparseToString()
			       <<endl;
      // keep existing size from variableIdMapping
      return;
    }
    getVariableIdMapping()->setElementSize(variableId,typeSize);
  }

  void EStateTransferFunctions::initializeGlobalVariablesNew(SgProject* root, EState& estate) {
    if(SgProject* project=isSgProject(root)) {
      ROSE_ASSERT(getVariableIdMapping());
      CodeThorn::VariableIdSet setOfGlobalVars=getVariableIdMapping()->getSetOfGlobalVarIds();
      CodeThorn::VariableIdSet setOfUsedVars=AstUtility::usedVariablesInsideFunctions(project,getVariableIdMapping());
      std::set<VariableId> setOfFilteredGlobalVars=CodeThorn::setIntersect(setOfGlobalVars, setOfUsedVars);
      uint32_t numFilteredVars=setOfGlobalVars.size()-setOfFilteredGlobalVars.size();
      if(getAnalyzer()->_ctOpt.status) {
	cout<< "STATUS: Number of global variables: "<<setOfGlobalVars.size()<<endl;
	cout<< "STATUS: Number of used variables: "<<setOfUsedVars.size()<<endl;
      }
      std::list<SgVariableDeclaration*> relevantGlobalVariableDecls=(getAnalyzer()->_ctOpt.initialStateFilterUnusedVariables)?
	getVariableIdMapping()->getVariableDeclarationsOfVariableIdSet(setOfFilteredGlobalVars)
	: getVariableIdMapping()->getVariableDeclarationsOfVariableIdSet(setOfGlobalVars)
	;
      uint32_t declaredInGlobalState=0;
      for(auto decl : relevantGlobalVariableDecls) {
	estate=analyzeVariableDeclaration(decl,estate,estate.label());
	declaredInGlobalState++;
	// this data is only used by globalVarIdByName to determine rers 'output' variable name in binary mode
	globalVarName2VarIdMapping[getVariableIdMapping()->variableName(getVariableIdMapping()->variableId(decl))]=getVariableIdMapping()->variableId(decl);
      }
      if(getAnalyzer()->_ctOpt.status) {
	cout<< "STATUS: Number of unused variables filtered in initial state: "<<numFilteredVars<<endl;
	cout<< "STATUS: Number of global variables declared in initial state: "<<declaredInGlobalState<<endl;
      }
    } else {
      cout<< "STATUS: no global scope. Global state remains without entries.";
    }
  }
  
  VariableId EStateTransferFunctions::globalVarIdByName(std::string varName) {
    return globalVarName2VarIdMapping[varName];
  }

  // sets all elements in PState according to aggregate
  // initializer. Also models default values of Integers (floats not
  // supported yet). EState is only used for lookup (modified is only
  // the PState object).
  PState EStateTransferFunctions::analyzeSgAggregateInitializer(VariableId initDeclVarId, SgAggregateInitializer* aggregateInitializer,PState pstate, /* for evaluation only  */ EState currentEState) {
    //cout<<"DEBUG: AST:"<<AstTerm::astTermWithNullValuesToString(aggregateInitializer)<<endl;
    ROSE_ASSERT(aggregateInitializer);
    SAWYER_MESG(logger[TRACE])<<"analyzeSgAggregateInitializer::array-initializer:"<<aggregateInitializer->unparseToString()<<endl;
    Label label=currentEState.label();
    PState newPState=pstate;
    int elemIndex=0;
    SgExprListExp* initListObjPtr=aggregateInitializer->get_initializers();
    SgExpressionPtrList& initList=initListObjPtr->get_expressions();

    for(SgExpressionPtrList::iterator i=initList.begin();i!=initList.end();++i) {
      AbstractValue arrayElemAddr=AbstractValue::createAddressOfArrayElement(initDeclVarId,AbstractValue(elemIndex));
      SgExpression* exp=*i;
      SgAssignInitializer* assignInit=isSgAssignInitializer(exp);
      if(assignInit==nullptr) {
	SAWYER_MESG(logger[WARN])<<"expected assign initializer but found "<<exp->unparseToString();
	SAWYER_MESG(logger[WARN])<<"AST: "<<AstTerm::astTermWithNullValuesToString(exp)<<endl;
	AbstractValue newVal=AbstractValue::createTop();
	initializeMemoryLocation(label,&newPState,arrayElemAddr,newVal);
      } else {
	// initialize element of array initializer in state
	SgExpression* assignInitExpr=assignInit->get_operand();
	// currentEState from above, newPState must be the same as in currentEState.
	AbstractValue newVal=singleValevaluateExpression(assignInitExpr,currentEState);
	initializeMemoryLocation(label,&newPState,arrayElemAddr,newVal);
      }
      elemIndex++;
    }
    // initialize remaining elements (if there are any) with default value
    int aggregateSize=(int)getVariableIdMapping()->getNumberOfElements(initDeclVarId);
    // if array size is not 0 then it was determined from the type and remaining elements are initialized
    if(aggregateSize==0) {
      // update aggregate size if it was not set yet
      aggregateSize=initList.size();
      getVariableIdMapping()->setNumberOfElements(initDeclVarId,aggregateSize);
    }

    // otherwise the size is determined from the aggregate initializer itself (done above)
    if(aggregateSize!=0) {
      for(int i=elemIndex;i<aggregateSize;i++) {
	AbstractValue arrayElemAddr=AbstractValue::createAddressOfArrayElement(initDeclVarId,AbstractValue(i));
	SAWYER_MESG(logger[TRACE])<<"Init aggregate default value: "<<arrayElemAddr.toString()<<endl;
	// there must be a default value because an aggregate initializer exists (it is never undefined in this case)
	// note: int a[3]={}; also forces the array to be initialized with {0,0,0}, the list can be empty.
	// whereas with int a[3]; all 3 are undefined
	AbstractValue defaultValue=AbstractValue(0); // TODO: cases where default value is not 0.
	initializeMemoryLocation(label,&newPState,arrayElemAddr,defaultValue);
      }
    } else {
      // if aggregate size is 0 there is nothing to do
    }
    return newPState;
  }

  AbstractValue EStateTransferFunctions::singleValevaluateExpression(SgExpression* expr,EState currentEState) {
    list<SingleEvalResultConstInt> resultList=evaluateExpression(expr,currentEState);
    ROSE_ASSERT(resultList.size()==1);
    SingleEvalResultConstInt valueResult=*resultList.begin();
    AbstractValue val=valueResult.result;
    return val;
  }

  list<EState> EStateTransferFunctions::transferTrueFalseEdge(SgNode* nextNodeToAnalyze2, Edge edge, const EState* estate) {
    EState currentEState=*estate;
    CallString cs=estate->callString;
    Label newLabel;
    PState newPState;
    ConstraintSet newCSet;
    list<SingleEvalResultConstInt> evalResultList=evaluateExpression(nextNodeToAnalyze2,currentEState);
    list<EState> newEStateList;
    for(list<SingleEvalResultConstInt>::iterator i=evalResultList.begin();
	i!=evalResultList.end();
	++i) {
      SingleEvalResultConstInt evalResult=*i;
      if(evalResult.isBot()) {
	SAWYER_MESG(logger[WARN])<<"PSTATE: "<<estate->pstate()->toString(getVariableIdMapping())<<endl;
	SAWYER_MESG(logger[WARN])<<"CONDITION EVALUATES TO BOT : "<<nextNodeToAnalyze2->unparseToString()<<endl;
	SAWYER_MESG(logger[WARN])<<"CONDITION EVALUATES TO BOT at: "
				 <<ProgramLocationsReport::programLocation(getLabeler(),estate->label())
				 <<endl;
	newLabel=edge.target();
	newPState=*evalResult.estate.pstate();
	EState newEstate=createEState(newLabel,cs,newPState,newCSet);
	newEStateList.push_back(newEstate);
      } else if((evalResult.isTrue() && edge.isType(EDGE_TRUE)) || (evalResult.isFalse() && edge.isType(EDGE_FALSE)) || evalResult.isTop()) {
	// pass on EState
	newLabel=edge.target();
	newPState=*evalResult.estate.pstate();
	if(ReadWriteListener* readWriteListener=getReadWriteListener()) {
	  readWriteListener->trueFalseEdgeEvaluation(edge,evalResult,estate);
	}
	// use new empty cset instead of computed cset
	ROSE_ASSERT(newCSet.size()==0);
	EState newEstate=createEState(newLabel,cs,newPState,newCSet);
	newEStateList.push_back(newEstate);
      } else if((evalResult.isFalse() && edge.isType(EDGE_TRUE)) || (evalResult.isTrue() && edge.isType(EDGE_FALSE))) {
	// we determined not to be on an execution path, therefore do nothing (do not add any result to resultlist)
	//cout<<"DEBUG: not on feasable execution path. skipping."<<endl;
      } else {
	// all other cases (assume evaluating to true or false, sane as top)
	// pass on EState
	newLabel=edge.target();
	newPState=*evalResult.estate.pstate();
	if(ReadWriteListener* readWriteListener=getReadWriteListener()) {
	  readWriteListener->trueFalseEdgeEvaluation(edge,evalResult,estate);
	}
	// use new empty cset instead of computed cset
	ROSE_ASSERT(newCSet.size()==0);
	EState newEstate=createEState(newLabel,cs,newPState,newCSet);
	newEStateList.push_back(newEstate);
      }
    }
    return newEStateList;
  }

  CodeThorn::EStateTransferFunctions::MemoryUpdateList
  CodeThorn::EStateTransferFunctions::evalAssignOpMemUpdates(SgAssignOp* nextNodeToAnalyze2, Edge edge, const EState* estatePtr) {
    MemoryUpdateList memoryUpdateList;
    CallString cs=estatePtr->callString;
    EState currentEState=*estatePtr;
    SgNode* lhs=SgNodeHelper::getLhs(nextNodeToAnalyze2);
    SgNode* rhs=SgNodeHelper::getRhs(nextNodeToAnalyze2);
    list<SingleEvalResultConstInt> res=evaluateExpression(rhs,currentEState, CodeThorn::EStateTransferFunctions::MODE_VALUE);
    for(list<SingleEvalResultConstInt>::iterator i=res.begin();i!=res.end();++i) {
      VariableId lhsVar;
      bool isLhsVar=checkIfVariableAndDetermineVarId(lhs,lhsVar);
      if(isLhsVar) {
	EState estate=(*i).estate;
	if(getVariableIdMapping()->isOfClassType(lhsVar)) {
	  // assignments to struct variables are not supported yet (this test does not detect s1.s2 (where s2 is a struct, see below)).
	  SAWYER_MESG(logger[WARN])<<"assignment of structs (copy constructor) is not supported yet. Target update ignored! (unsound)"<<endl;
	} else if(getVariableIdMapping()->isOfCharType(lhsVar)) {
	  memoryUpdateList.push_back(make_pair(estate,make_pair(lhsVar,(*i).result)));
	} else if(getVariableIdMapping()->isOfIntegerType(lhsVar)) {
	  memoryUpdateList.push_back(make_pair(estate,make_pair(lhsVar,(*i).result)));
	} else if(getVariableIdMapping()->isOfEnumType(lhsVar)) /* PP */ {
	  memoryUpdateList.push_back(make_pair(estate,make_pair(lhsVar,(*i).result)));
	} else if(getVariableIdMapping()->isOfFloatingPointType(lhsVar)) {
	  memoryUpdateList.push_back(make_pair(estate,make_pair(lhsVar,(*i).result)));
	} else if(getVariableIdMapping()->isOfBoolType(lhsVar)) {
	  memoryUpdateList.push_back(make_pair(estate,make_pair(lhsVar,(*i).result)));
	} else if(getVariableIdMapping()->isOfPointerType(lhsVar)) {
	  // assume here that only arrays (pointers to arrays) are assigned
	  memoryUpdateList.push_back(make_pair(estate,make_pair(lhsVar,(*i).result)));
	} else if(SgTypeString* lhsTypeTypeString=isSgTypeString(getVariableIdMapping()->getType(lhsVar))) {
	  // assume here that only arrays (pointers to arrays) are assigned
	  SAWYER_MESG(logger[WARN])<<"DEBUG: LHS assignment: typestring band aid"<<endl;
	  memoryUpdateList.push_back(make_pair(estate,make_pair(lhsVar,(*i).result)));
	} else if(getVariableIdMapping()->isOfReferenceType(lhsVar)) {
	  memoryUpdateList.push_back(make_pair(estate,make_pair(lhsVar,(*i).result)));
	} else {
	  // other types (e.g. function pointer type)
	  memoryUpdateList.push_back(make_pair(estate,make_pair(lhsVar,(*i).result)));
#if 0
	  SAWYER_MESG(logger[ERROR])<<"Error at "<<SgNodeHelper::sourceFilenameLineColumnToString(nextNodeToAnalyze2)<<endl;
	  SAWYER_MESG(logger[ERROR])<<"Unsupported type on LHS side of assignment: "
				    <<"type lhs: '"<<getVariableIdMapping()->getType(lhsVar)->unparseToString()<<"'"
				    <<", "
				    <<"AST type node: "<<getVariableIdMapping()->getType(lhsVar)->class_name()
				    <<", Expr:"<<nextNodeToAnalyze2->unparseToString()
				    <<endl;
	  exit(1);
#endif
	}
      } else if(isSgDotExp(lhs)) {
	SAWYER_MESG(logger[TRACE])<<"detected dot operator on lhs "<<lhs->unparseToString()<<"."<<endl;
	list<SingleEvalResultConstInt> lhsRes=evaluateExpression(lhs,currentEState, CodeThorn::EStateTransferFunctions::MODE_ADDRESS);
	for (auto lhsAddressResult : lhsRes) {
	  EState estate=(*i).estate;
	  AbstractValue lhsAddress=lhsAddressResult.result;
	  SAWYER_MESG(logger[TRACE])<<"detected dot operator on lhs: writing to "<<lhsAddress.toString(getVariableIdMapping())<<"."<<endl;
	  memoryUpdateList.push_back(make_pair(estate,make_pair(lhsAddress,(*i).result)));
	}
      } else if(isSgArrowExp(lhs)) {
	SAWYER_MESG(logger[TRACE])<<"detected arrow operator on lhs "<<lhs->unparseToString()<<"."<<endl;
	list<SingleEvalResultConstInt> lhsRes=evaluateExpression(lhs,currentEState, CodeThorn::EStateTransferFunctions::MODE_ADDRESS);
	for (auto lhsAddressResult : lhsRes) {
	  EState estate=(*i).estate;
	  AbstractValue lhsAddress=lhsAddressResult.result;
	  SAWYER_MESG(logger[TRACE])<<"detected arrow operator on lhs: writing to "<<lhsAddress.toString(getVariableIdMapping())<<"."<<endl;
	  memoryUpdateList.push_back(make_pair(estate,make_pair(lhsAddress,(*i).result)));
	}
      } else if(isSgPntrArrRefExp(lhs)) {
	EState estate=(*i).estate;
	PState oldPState=*estate.pstate();
	if(_analyzer->getSkipArrayAccesses()) {
	  // estateList.push_back(createEState(edge.target(),cs,oldPState,oldcset));
	  // nothing to do (no memory access to add to list)
	} else {
	  SgExpression* arrExp=isSgExpression(SgNodeHelper::getLhs(lhs));
	  SgExpression* indexExp=isSgExpression(SgNodeHelper::getRhs(lhs));
	  if(SgVarRefExp* varRefExp=isSgVarRefExp(arrExp)) {
	    PState pstate2=oldPState;
	    VariableId arrayVarId=getVariableIdMapping()->variableId(varRefExp);
	    AbstractValue arrayPtrValue;
	    // two cases
	    if(getVariableIdMapping()->isOfArrayType(arrayVarId)) {
	      // create array element 0 (in preparation to have index added, or, if not index is used, it is already the correct index (=0).
	      arrayPtrValue=AbstractValue::createAddressOfArray(arrayVarId);
	    } else if(getVariableIdMapping()->isOfPointerType(arrayVarId)) {
	      // in case it is a pointer retrieve pointer value
	      AbstractValue ptr=AbstractValue::createAddressOfArray(arrayVarId);
	      if(pstate2.varExists(ptr)) {
		arrayPtrValue=readFromMemoryLocation(estate.label(),&pstate2,ptr);
		//cout<<"DEBUG: arrayPtrValue: "<<arrayPtrValue.toString(getVariableIdMapping())<<endl;
		// convert integer to VariableId
		if(arrayPtrValue.isTop()||arrayPtrValue.isBot()) {
		  if(getOptionOutputWarnings())
		    cout<<"Warning: "<<nextNodeToAnalyze2->unparseToString()<<arrayPtrValue.toString(getVariableIdMapping())<<" array index is top or bot. Not supported yet."<<endl;
		}
		// logger[DEBUG]<<"defering pointer-to-array: ptr:"<<getVariableIdMapping()->variableName(arrayVarId);
	      } else {
		if(getOptionOutputWarnings())
		  cout<<"Warning: lhs array access: pointer variable does not exis2t in PState:"<<ptr.toString()<<endl;
		arrayPtrValue=AbstractValue::createTop();
	      }
	    } else if(getVariableIdMapping()->isOfReferenceType(arrayVarId)) {
	      AbstractValue ptr=AbstractValue::createAddressOfArray(arrayVarId);
	      if(pstate2.varExists(ptr)) {
		arrayPtrValue=readFromReferenceMemoryLocation(estate.label(),&pstate2,ptr);
	      } else {
		arrayPtrValue=AbstractValue::createTop();
	      }
	    } else {
	      SAWYER_MESG(logger[ERROR]) <<"lhs array access: unknown type of array or pointer."<<endl;
	      exit(1);
	    }

	    list<SingleEvalResultConstInt> res=evaluateExpression(indexExp,currentEState);
	    ROSE_ASSERT(res.size()==1); // this should always hold for normalized ASTs
	    AbstractValue indexValue=(*(res.begin())).value();
	    AbstractValue arrayPtrPlusIndexValue=AbstractValue::operatorAdd(arrayPtrValue,indexValue);
	    AbstractValue arrayElementAddr=arrayPtrPlusIndexValue;
	    //cout<<"DEBUG: arrayElementId: "<<arrayElementId.toString(getVariableIdMapping())<<endl;
	    //SAWYER_MESG(logger[TRACE])<<"arrayElementVarId:"<<arrayElementId.toString()<<":"<<getVariableIdMapping()->variableName(arrayVarId)<<" Index:"<<index<<endl;
	    if(arrayElementAddr.isBot()) {
	      // inaccessible memory location, return empty estate list
	      return memoryUpdateList;
	    }
	    // read value of variable var id (same as for VarRefExp - TODO: reuse)
	    // TODO: check whether arrayElementId (or array) is a constant array (arrayVarId)
	    //TODO: writeToMemoryLocation(label,&pstate2,arrayElementId,(*i).value()); // *i is assignment-rhs evaluation result
	    memoryUpdateList.push_back(make_pair(estate,make_pair(arrayElementAddr,(*i).result)));
	  } else if(SgAddressOfOp* addressOfOp=isSgAddressOfOp(lhs)) {
	    // address of op, need to compute an l-value and use as address
          
	  } else {
	    list<SingleEvalResultConstInt> arrExpRes=evaluateExpression(arrExp,currentEState);
	    ROSE_ASSERT(arrExpRes.size()==1); // this should always hold for normalized ASTs
	    AbstractValue arrPtrValue=(*(arrExpRes.begin())).value();
	    list<SingleEvalResultConstInt> indexExpRes=evaluateExpression(indexExp,currentEState);
	    ROSE_ASSERT(indexExpRes.size()==1); // this should always hold for normalized ASTs
	    AbstractValue indexValue=(*(indexExpRes.begin())).value();
	    AbstractValue arrayPtrPlusIndexValue=AbstractValue::operatorAdd(arrPtrValue,indexValue);
	    AbstractValue arrayElementAddr=arrayPtrPlusIndexValue;
	    if(arrayElementAddr.isBot()) {
	      return memoryUpdateList; // inaccessible memory location, return empty estate list
	    }
	    memoryUpdateList.push_back(make_pair(estate,make_pair(arrayElementAddr,(*i).result)));
	    //above evaluation covers all cases now
	    //SAWYER_MESG(logger[ERROR]) <<"array-access uses expr for denoting the array. Normalization missing."<<endl;
	    //SAWYER_MESG(logger[ERROR]) <<"lhs AST: "<<AstTerm::astTermWithNullValuesToString(lhs)<<endl;
	    //SAWYER_MESG(logger[ERROR]) <<"expr: "<<lhs->unparseToString()<<endl;
	    //SAWYER_MESG(logger[ERROR]) <<"arraySkip: "<<_analyzer->getSkipArrayAccesses()<<endl;
	    //exit(1);
	  }
	}
      } else if(SgPointerDerefExp* lhsDerefExp=isSgPointerDerefExp(lhs)) {
	SgExpression* lhsOperand=lhsDerefExp->get_operand();
	SAWYER_MESG(logger[TRACE])<<"lhsOperand: "<<lhsOperand->unparseToString()<<endl;
	list<SingleEvalResultConstInt> resLhs=evaluateExpression(lhsOperand,currentEState);
	if(resLhs.size()>1) {
	  throw CodeThorn::Exception("more than 1 execution path (probably due to abstraction) in operand's expression of pointer dereference operator on lhs of "+nextNodeToAnalyze2->unparseToString());
	}
	ROSE_ASSERT(resLhs.size()==1);
	AbstractValue lhsPointerValue=(*resLhs.begin()).result;
	SAWYER_MESG(logger[TRACE])<<"lhsPointerValue: "<<lhsPointerValue.toString(getVariableIdMapping())<<endl;
	if(lhsPointerValue.isNullPtr()) {
	  recordDefinitiveNullPointerDereferenceLocation(estatePtr->label());
	  //return estateList;
	  if(_analyzer->getOptionsRef().nullPointerDereferenceKeepGoing) {
	    // no state can follow, but as requested keep going without effect
	  } else {
	    // no state can follow
	    return memoryUpdateList;
	  }
	} else if(lhsPointerValue.isTop()) {
	  recordPotentialNullPointerDereferenceLocation(estatePtr->label());
	  // specific case a pointer expr evaluates to top. Dereference operation
	  // potentially modifies any memory location in the state.
	  AbstractValue lhsAddress=lhsPointerValue;
	  memoryUpdateList.push_back(make_pair(*estatePtr,make_pair(lhsAddress,(*i).result))); // lhsAddress is TOP!!!
	} else if(!(lhsPointerValue.isPtr())) {
	  // changed isUndefined to isTop (2/17/20)
	  if(lhsPointerValue.isTop() && _analyzer->getIgnoreUndefinedDereference()) {
	    //cout<<"DEBUG: lhsPointerValue:"<<lhsPointerValue.toString(getVariableIdMapping())<<endl;
	    // skip write access, just create new state (no effect)
	    // nothing to do, as it request to not update the state in this case
	  } else {
	    SAWYER_MESG(logger[WARN])<<"not a pointer value (or top) in dereference operator: lhs-value:"<<lhsPointerValue.toLhsString(getVariableIdMapping())<<" lhs: "<<lhs->unparseToString()<<" : assuming change of any memory location."<<endl;
	    recordPotentialNullPointerDereferenceLocation(estatePtr->label());
	    // specific case a pointer expr evaluates to top. Dereference operation
	    // potentially modifies any memory location in the state.
	    // iterate over all elements of the state and merge with rhs value
	    memoryUpdateList.push_back(make_pair(*estatePtr,make_pair(lhsPointerValue,(*i).result))); // lhsPointerValue is TOP!!!
	  }
	} else {
	  //cout<<"DEBUG: lhsPointerValue:"<<lhsPointerValue.toString(getVariableIdMapping())<<endl;
	  memoryUpdateList.push_back(make_pair(*estatePtr,make_pair(lhsPointerValue,(*i).result)));
	}
      } else {
	//cout<<"DEBUG: else (no var, no ptr) ... "<<endl;
	if(_analyzer->getSkipArrayAccesses()&&isSgPointerDerefExp(lhs)) {
	  SAWYER_MESG(logger[WARN])<<"skipping pointer dereference: "<<lhs->unparseToString()<<endl;
	} else {
	  SAWYER_MESG(logger[ERROR]) << "transferfunction:SgAssignOp: unrecognized expression on lhs."<<endl;
	  SAWYER_MESG(logger[ERROR]) << "expr: "<< lhs->unparseToString()<<endl;
	  SAWYER_MESG(logger[ERROR]) << "type: "<<lhs->class_name()<<endl;
	  //cerr << "performing no update of state!"<<endl;
	  exit(1);
	}
      }
    }
    return memoryUpdateList;
  }

  // value semantics for upates
  CallString EStateTransferFunctions::transferFunctionCallContext(CallString cs, Label lab) {
    SAWYER_MESG(logger[TRACE])<<"FunctionCallTransfer: adding "<<lab.toString()<<" to cs: "<<cs.toString()<<endl;
    cs.addLabel(lab);
    return cs;
  }
  bool EStateTransferFunctions::isFeasiblePathContext(CallString& cs,Label lab) {
    return cs.getLength()==0||cs.isLastLabel(lab);
  }

  std::string EStateTransferFunctions::transerFunctionCodeToString(TransferFunctionCode tfCode) {
    static std::map<TransferFunctionCode,string> tfCodeInfo=
      {
       {Unknown,"unknown"},
       {FunctionCall,"FunctionCall"},
       {FunctionCallLocalEdge,"FunctionCallLocalEdge"},
       {FunctionCallExternal,"FunctionCallExternal"},
       {FunctionCallReturn,"FunctionCallReturn"},
       {FunctionEntry,"FunctionEntry"},
       {FunctionExit,"FunctionExit"},
       {ReturnStmt,"ReturnStmt"},
       {FailedAssert,"FailedAssert"},
       {AsmStmt,"AsmStmt"},
       {ExprStmt,"ExprStmt"},
       {GnuExtensionStmtExpr,"GnuExtensionStmtExpr"},
       {Identity,"Identity"},
       {VariableDeclaration,"VariableDeclaration"},
       {CaseOptionStmt,"CaseOptionStmt"},
       {DefaultOptionStmt,"DefaultOptionStmt"},
       {Assign,"Assign"},
       {IncDec,"IncDec"},
       {ForkFunction,"ForkFunction"},
       {ForkFunctionWithExternalTargetFunction,"ForkFunctionWithExternalTargetFunction"},
      };
    ROSE_ASSERT(tfCode<tfCodeInfo.size());
    return tfCodeInfo[tfCode];
  }

  void EStateTransferFunctions::printTransferFunctionInfo(TransferFunctionCode tfCode, SgNode* node, Edge edge, const EState* estate) {
    cout<<"transfer: L"<<estate->label().toString()<<": "<<std::setw(22)<<std::left<<transerFunctionCodeToString(tfCode)<<": ";
    if(getLabeler()->isFunctionEntryLabel(edge.source())||getLabeler()->isFunctionExitLabel(edge.source())) {
      cout<<SgNodeHelper::getFunctionName(node);
    } else {
      cout<<node->unparseToString();
    }
    cout<<endl;
  }

  void EStateTransferFunctions::printEvaluateExpressionInfo(SgNode* node,EState& estate, EvalMode mode) {
    cout<<"          "<<"      evaluateExpression    : "<<node->unparseToString()<<" [evalmode"<<mode<<": "<<AstTerm::astTermWithNullValuesToString(node)<<"]"<<endl;
  }

  list<EState> EStateTransferFunctions::transferEdgeEStateDispatch(TransferFunctionCode tfCode, SgNode* node, Edge edge, const EState* estate) {
    if(_analyzer->getOptionsRef().info.printTransferFunctionInfo) {
      printTransferFunctionInfo(tfCode,node,edge,estate);
    }
    switch(tfCode) {
    case FunctionCall: return transferFunctionCall(edge,estate);
    case FunctionCallLocalEdge: return transferFunctionCallLocalEdge(edge,estate);
    case FunctionCallExternal: return transferFunctionCallExternal(edge,estate);
    case FunctionCallReturn: return transferFunctionCallReturn(edge,estate);
    case FunctionEntry: return transferFunctionEntry(edge,estate);
    case FunctionExit: return transferFunctionExit(edge,estate);
    case ReturnStmt: return transferReturnStmt(edge,estate);
    case AsmStmt: return transferAsmStmt(edge,estate);
    case FailedAssert: return transferFailedAssert(edge,estate);
    case ExprStmt: return transferExprStmt(node,edge,estate);
    case GnuExtensionStmtExpr: return transferGnuExtensionStmtExpr(node,edge,estate);
    case Identity: return transferIdentity(edge,estate);
    case VariableDeclaration: return transferVariableDeclaration(isSgVariableDeclaration(node),edge,estate);
    case CaseOptionStmt: return transferCaseOptionStmt(isSgCaseOptionStmt(node),edge,estate);
    case DefaultOptionStmt: return transferDefaultOptionStmt(isSgDefaultOptionStmt(node),edge,estate);
    case Assign: return transferAssignOp(isSgAssignOp(node),edge,estate);
    case IncDec: return transferIncDecOp(node,edge,estate);
    case ForkFunction: return transferForkFunction(edge,estate,isSgFunctionCallExp(node));
    case ForkFunctionWithExternalTargetFunction: return transferForkFunctionWithExternalTargetFunction(edge,estate,isSgFunctionCallExp(node));
    default:
      return transferIdentity(edge,estate);
    }
    //unreachable
  }

  list<EState> EStateTransferFunctions::transferEdgeEState(Edge edge, const EState* estate) {
    pair<TransferFunctionCode,SgNode*> tfCodeNodePair=determineTransferFunctionCode(edge,estate);
    EStateTransferFunctions::TransferFunctionCode tfCode=tfCodeNodePair.first;
    SgNode* nextNodeToAnalyze=tfCodeNodePair.second;
    return transferEdgeEStateDispatch(tfCode,nextNodeToAnalyze,edge,estate);
  }

  std::pair<EStateTransferFunctions::TransferFunctionCode,SgNode*> EStateTransferFunctions::determineTransferFunctionCode(Edge edge, const EState* estate) {
    ROSE_ASSERT(edge.source()==estate->label());
    EState currentEState=*estate;
    PState currentPState=*currentEState.pstate();
    ConstraintSet cset=*currentEState.constraints();
    // handle the edge as outgoing edge
    ROSE_ASSERT(_analyzer->getCFAnalyzer());
    SgNode* nextNodeToAnalyze=_analyzer->getCFAnalyzer()->getNode(edge.source());
    ROSE_ASSERT(nextNodeToAnalyze);
    TransferFunctionCode tfCode=TransferFunctionCode::Unknown;
    if(edge.isType(EDGE_LOCAL)) {
      tfCode=TransferFunctionCode::FunctionCallLocalEdge;
    } else if(SgNodeHelper::Pattern::matchAssertExpr(nextNodeToAnalyze)) {
      // handle assert(0)
      tfCode=TransferFunctionCode::FailedAssert;
    } else if(edge.isType(EDGE_CALL) && SgNodeHelper::Pattern::matchFunctionCall(nextNodeToAnalyze)) {
      tfCode=TransferFunctionCode::FunctionCall;
    } else if(edge.isType(EDGE_EXTERNAL) && SgNodeHelper::Pattern::matchFunctionCall(nextNodeToAnalyze)) {
      // \todo the && condition excludes constructor calls
      if(ReadWriteListener* listener=getReadWriteListener()) {
	listener->functionCallExternal(edge,estate);
      }
      tfCode=TransferFunctionCode::FunctionCallExternal;
    } else if(isSgReturnStmt(nextNodeToAnalyze) && !SgNodeHelper::Pattern::matchReturnStmtFunctionCallExp(nextNodeToAnalyze)) {
      // "return x;": add $return=eval() [but not for "return f();"]
      tfCode=TransferFunctionCode::ReturnStmt;
    } else if(isSgAsmStmt(nextNodeToAnalyze)) {
      tfCode=TransferFunctionCode::AsmStmt;
    } else if(getLabeler()->isFunctionEntryLabel(edge.source())) {
      tfCode=TransferFunctionCode::FunctionEntry;
    } else if(getLabeler()->isFunctionExitLabel(edge.source())) {
      tfCode=TransferFunctionCode::FunctionExit;
    } else if(getLabeler()->isFunctionCallReturnLabel(edge.source())) {
      tfCode=TransferFunctionCode::FunctionCallReturn;
    } else if(isSgCaseOptionStmt(nextNodeToAnalyze)) {
      tfCode=TransferFunctionCode::CaseOptionStmt;
    } else if(isSgDefaultOptionStmt(nextNodeToAnalyze)) {
      tfCode=TransferFunctionCode::DefaultOptionStmt;
    } else if(isSgVariableDeclaration(nextNodeToAnalyze)) {
      tfCode=TransferFunctionCode::VariableDeclaration;
    } else if(isSgExprStatement(nextNodeToAnalyze) || SgNodeHelper::isForIncExpr(nextNodeToAnalyze)) {
      tfCode=TransferFunctionCode::ExprStmt;      
    } else if(isSgStatementExpression(nextNodeToAnalyze)) {
      // GNU extension
      tfCode=TransferFunctionCode::GnuExtensionStmtExpr;      
    } else if(SgFunctionCallExp* funCall=SgNodeHelper::Pattern::matchFunctionCall(nextNodeToAnalyze)) {
      // TODO: this case should be handled as part of transferExprStmt (or ExpressionRoot)
      //cout<<"DEBUG: function call"<<(isCondition?" (inside condition) ":"")<<nextNodeToAnalyze->unparseToString()<<endl;
      // this case cannot happen for normalized code
      SAWYER_MESG(logger[ERROR])<<"Function call detected not represented in ICFG. Normalization required:"<<SgNodeHelper::sourceLineColumnToString(funCall)<<":"<<funCall->unparseToString()<<endl;
      exit(1);
    } else {
      ROSE_ASSERT(!edge.isType(EDGE_EXTERNAL));
      ROSE_ASSERT(!edge.isType(EDGE_CALLRETURN));
      tfCode=TransferFunctionCode::Identity;
    }
    return std::make_pair(tfCode,nextNodeToAnalyze);
  }

  // ExprAnalyzer functions
  void EStateTransferFunctions::setVariableIdMapping(VariableIdMappingExtended* variableIdMapping) {
    _variableIdMapping=variableIdMapping;
  }
  CodeThorn::InterpreterMode EStateTransferFunctions::getInterpreterMode() {
    return _interpreterMode;
  }
  void EStateTransferFunctions::setInterpreterMode(CodeThorn::InterpreterMode im) {
    _interpreterMode=im;
  }

  string EStateTransferFunctions::getInterpreterModeFileName() {
    return _interpreterModeFileName;
  }
  void EStateTransferFunctions::setInterpreterModeFileName(string imFileName) {
    _interpreterModeFileName=imFileName;
  }

  bool EStateTransferFunctions::getIgnoreUndefinedDereference() {
    return _analyzer->_ctOpt.ignoreUndefinedDereference;
  }

  bool EStateTransferFunctions::getIgnoreFunctionPointers() {
    return _analyzer->_ctOpt.ignoreFunctionPointers;
  }

  // TODO: all following options to read from ctOpt (as above)
  void EStateTransferFunctions::setSkipUnknownFunctionCalls(bool skip) {
    _skipSelectedFunctionCalls=skip;
  }

  bool EStateTransferFunctions::getSkipUnknownFunctionCalls() {
    return _skipSelectedFunctionCalls;
  }

  void EStateTransferFunctions::setSkipArrayAccesses(bool skip) {
    _skipArrayAccesses=skip;
  }

  bool EStateTransferFunctions::getSkipArrayAccesses() {
    return _skipArrayAccesses;
  }

  void EStateTransferFunctions::setSVCompFunctionSemantics(bool flag) {
    _svCompFunctionSemantics=flag;
  }

  bool EStateTransferFunctions::getSVCompFunctionSemantics() {
    return _svCompFunctionSemantics;
  }

  bool EStateTransferFunctions::stdFunctionSemantics() {
    return _stdFunctionSemantics;
  }

  bool EStateTransferFunctions::getStdFunctionSemantics() {
    return _stdFunctionSemantics;
  }

  void EStateTransferFunctions::setStdFunctionSemantics(bool flag) {
    _stdFunctionSemantics=flag;
  }

  bool EStateTransferFunctions::checkIfVariableAndDetermineVarId(SgNode* node, VariableId& varId) {
    assert(node);
    if(SgNodeHelper::isArrayAccess(node)) {
      // 1) array variable id
      // 2) eval array-index expr
      // 3) if const then compute variable id otherwise return non-valid var id (would require set)
      return false;
    }
    if(SgVarRefExp* varref=isSgVarRefExp(node)) {
      // found variable
      ROSE_ASSERT(_variableIdMapping);
      varId=_variableIdMapping->variableId(varref);
      return true;
    } else {
      VariableId defaultVarId;
      varId=defaultVarId;
      return false;
    }
  }

  // macro for repeating code pattern to get correctly typed values from value expressions
#define CASE_ABSTRACT_VALUE_FROM_SG_VALUE_EXP(SG_VALUE_EXP_TYPE, CPP_TYPE) \
  case V_ ## SG_VALUE_EXP_TYPE: {					\
    SG_VALUE_EXP_TYPE* exp=is ## SG_VALUE_EXP_TYPE(valueExp);		\
    ROSE_ASSERT(exp);							\
    CPP_TYPE val=exp->get_value();					\
    return AbstractValue(val); }

  AbstractValue EStateTransferFunctions::abstractValueFromSgValueExp(SgValueExp* valueExp, EvalMode mode) {
    ROSE_ASSERT(valueExp);
    switch(valueExp->variantT()) {
      CASE_ABSTRACT_VALUE_FROM_SG_VALUE_EXP(SgFloatVal, float);
      CASE_ABSTRACT_VALUE_FROM_SG_VALUE_EXP(SgDoubleVal, double);
      //    CASE_ABSTRACT_VALUE_FROM_SG_VALUE_EXP(SgLongDoubleVal, long double);
      CASE_ABSTRACT_VALUE_FROM_SG_VALUE_EXP(SgShortVal, short int);
      CASE_ABSTRACT_VALUE_FROM_SG_VALUE_EXP(SgIntVal, int);
      CASE_ABSTRACT_VALUE_FROM_SG_VALUE_EXP(SgLongIntVal, long int);
      CASE_ABSTRACT_VALUE_FROM_SG_VALUE_EXP(SgLongLongIntVal, long long int);
      CASE_ABSTRACT_VALUE_FROM_SG_VALUE_EXP(SgUnsignedCharVal, unsigned char);
      CASE_ABSTRACT_VALUE_FROM_SG_VALUE_EXP(SgUnsignedShortVal, unsigned short);
      CASE_ABSTRACT_VALUE_FROM_SG_VALUE_EXP(SgUnsignedIntVal, unsigned int);
      CASE_ABSTRACT_VALUE_FROM_SG_VALUE_EXP(SgUnsignedLongVal, unsigned long int);
      CASE_ABSTRACT_VALUE_FROM_SG_VALUE_EXP(SgUnsignedLongLongIntVal, unsigned long long int);
      // following are all cases that require specific handling in ROSE AST
    case  V_SgCharVal: {
      SgCharVal* exp=isSgCharVal(valueExp);
      unsigned char val=(unsigned char)(signed char)exp->get_value();
      return AbstractValue(val);
    }
    case V_SgWcharVal: {
      SgWcharVal* exp=isSgWcharVal(valueExp);
      long int val=exp->get_value();
      return AbstractValue(val);
    }
    case V_SgComplexVal: {
      return AbstractValue(CodeThorn::Top());
    }
    case V_SgNullptrValExp: {
      return AbstractValue((int)0); // null pointer value
    }
    case V_SgEnumVal: {
      SgEnumVal* exp=isSgEnumVal(valueExp);
      int val=exp->get_value();
      return AbstractValue(val);
    }
    case V_SgStringVal: {
      SgStringVal* stringVal=isSgStringVal(valueExp);
      if(mode==MODE_EMPTY_STATE) {
	// string val addresses are only available if variable ids are available
	return AbstractValue::createTop();
      }
      // handle string literals
      std::string s=stringVal->get_value();
      VariableId stringValVarId=_variableIdMapping->getStringLiteralVariableId(stringVal);
      AbstractValue val=AbstractValue::createAddressOfVariable(stringValVarId);
      SAWYER_MESG(logger[TRACE])<<"Created abstract string literal value: "<<"\""<<s<<"\""<<": abstract value: "<<val.toString(_variableIdMapping)<<endl;
      return val;
    }
    case V_SgBoolValExp: {
      SgBoolValExp* exp=isSgBoolValExp(valueExp);
      // ROSE uses an integer for a bool
      int val=exp->get_value();
      if(val==0)
	return AbstractValue(false);
      else if(val==1)
	return AbstractValue(true);
      else {
	SAWYER_MESG(logger[ERROR])<<"Error: unknown bool value (not 0 or 1): SgBoolExp::get_value()=="<<val<<endl;
	exit(1);
      }
    }
    default:
      return AbstractValue::createTop();
    }
  }

  //////////////////////////////////////////////////////////////////////
  // EVAL CONSTINT
  //////////////////////////////////////////////////////////////////////
  list<SingleEvalResultConstInt> EStateTransferFunctions::listify(SingleEvalResultConstInt res) {
    list<SingleEvalResultConstInt> resList;
    resList.push_back(res);
    return resList;
  }

  void SingleEvalResultConstInt::init(EState estate, AbstractValue result) {
    this->estate=estate;
    this->result=result;
  }

#define CASE_EXPR_ANALYZER_EVAL(ROSENODENAME,EVALFUNCTIONNAME) case V_ ## ROSENODENAME: resultList.splice(resultList.end(),EVALFUNCTIONNAME(is ## ROSENODENAME(node),lhsResult,rhsResult,estate,mode));break

#define CASE_EXPR_ANALYZER_EVAL_UNARY_OP(ROSENODENAME,EVALFUNCTIONNAME) case V_ ## ROSENODENAME: resultList.splice(resultList.end(),EVALFUNCTIONNAME(is ## ROSENODENAME(node),operandResult,estate,mode));break

  list<SingleEvalResultConstInt> EStateTransferFunctions::evaluateLExpression(SgNode* node,EState estate) {
    list<SingleEvalResultConstInt> resList;
    AbstractValue result;
    if(SgVarRefExp* varExp=isSgVarRefExp(node)) {
      return evalLValueVarRefExp(varExp,estate);
    } else if(SgPntrArrRefExp* arrRef=isSgPntrArrRefExp(node)) {
      return evalLValuePntrArrRefExp(arrRef,estate);
    } else if(SgDotExp* dotExp=isSgDotExp(node)) {
      return evalLValueExp(dotExp,estate);
    } else if(SgArrowExp* arrowExp=isSgArrowExp(node)) {
      return evalLValueExp(arrowExp,estate);
    } else if(SgPointerDerefExp* ptrDerefExp=isSgPointerDerefExp(node)) {
      return evalLValuePointerDerefExp(ptrDerefExp,estate);
    } else {
      cerr<<"Error: unsupported lvalue expression: "<<node->unparseToString()<<endl;
      cerr<<"     : "<<SgNodeHelper::sourceLineColumnToString(node)<<" : "<<AstTerm::astTermWithNullValuesToString(node)<<endl;
      exit(1);
    }
    // unreachable
  }

  bool EStateTransferFunctions::isLValueOp(SgNode* node) {
    // assign operators not included yet
    return isSgAddressOfOp(node)
      || SgNodeHelper::isPrefixIncDecOp(node)
      || SgNodeHelper::isPostfixIncDecOp(node)
      ;
  }

  list<SingleEvalResultConstInt> EStateTransferFunctions::evaluateShortCircuitOperators(SgNode* node,EState estate, EvalMode mode) {
    SgNode* lhs=SgNodeHelper::getLhs(node);
    list<SingleEvalResultConstInt> lhsResultList=evaluateExpression(lhs,estate,mode);
    list<SingleEvalResultConstInt> resultList;
    for(list<SingleEvalResultConstInt>::iterator liter=lhsResultList.begin();
	liter!=lhsResultList.end();
	++liter) {
      switch(node->variantT()) {
      case V_SgAndOp: {
	SingleEvalResultConstInt lhsResult=*liter;
	// short circuit semantics
	if(lhsResult.isTrue()||lhsResult.isTop()||lhsResult.isBot()) {
	  SgNode* rhs=SgNodeHelper::getRhs(node);
	  list<SingleEvalResultConstInt> rhsResultList=evaluateExpression(rhs,estate,mode);
	  for(list<SingleEvalResultConstInt>::iterator riter=rhsResultList.begin();
	      riter!=rhsResultList.end();
	      ++riter) {
	    SingleEvalResultConstInt rhsResult=*riter;
	    resultList.splice(resultList.end(),evalAndOp(isSgAndOp(node),lhsResult,rhsResult,estate,mode));
	  }
	} else {
	  // rhs not executed
	  ROSE_ASSERT(lhsResult.isFalse());
	  // result must be zero (=false)
	  resultList.push_back(lhsResult);
	}
	break;
      }
      case V_SgOrOp: {
	SingleEvalResultConstInt lhsResult=*liter;
	if(lhsResult.isFalse()||lhsResult.isTop()||lhsResult.isBot()) {
	  SgNode* rhs=SgNodeHelper::getRhs(node);
	  list<SingleEvalResultConstInt> rhsResultList=evaluateExpression(rhs,estate,mode);
	  for(list<SingleEvalResultConstInt>::iterator riter=rhsResultList.begin();
	      riter!=rhsResultList.end();
	      ++riter) {
	    SingleEvalResultConstInt rhsResult=*riter;
	    resultList.splice(resultList.end(),evalOrOp(isSgOrOp(node),lhsResult,rhsResult,estate,mode));
	  }
	} else {
	  // rhs not executed
	  ROSE_ASSERT(lhsResult.isTrue());
	  // bugfix MS 50/06/2020: convert any true value to true/1: (10||x)=>1
	  lhsResult.result=AbstractValue(true);
	  resultList.push_back(lhsResult);
	}
	break;
      }
      default:
	cerr << "Binary short circuit op:"<<SgNodeHelper::nodeToString(node)<<"(nodetype:"<<node->class_name()<<")"<<endl;
	throw CodeThorn::Exception("Error: evaluateExpression::unknown binary short circuit operation.");
      }
    }
    return resultList;
  }

  AbstractValue EStateTransferFunctions::evaluateExpressionWithEmptyState(SgExpression* expr) {
    SAWYER_MESG(logger[TRACE])<<"evaluateExpressionWithEmptyState(1):"<<expr->unparseToString()<<endl;
    ROSE_ASSERT(AbstractValue::getVariableIdMapping());
    EState emptyEState;
    PState emptyPState;
    emptyEState.setPState(&emptyPState);
    EStateTransferFunctions::EvalMode evalMode=EStateTransferFunctions::MODE_EMPTY_STATE;
    list<SingleEvalResultConstInt> resList=evaluateExpression(expr,emptyEState,evalMode);
    SAWYER_MESG(logger[TRACE])<<"evaluateExpressionWithEmptyState(2):"<<expr->unparseToString()<<endl;
    if(resList.size()!=1) {
      return AbstractValue::createTop();
    } else {
      return (*resList.begin()).value();
    }
  }
  
  list<SingleEvalResultConstInt> EStateTransferFunctions::evalOp(SgNode* node,
								 SingleEvalResultConstInt lhsResult,
								 SingleEvalResultConstInt rhsResult,
								 EState estate, EvalMode mode) {
    list<SingleEvalResultConstInt> resultList;
    SingleEvalResultConstInt res;
    AbstractValue::Operator op=sgNodeToAbstractValueOperator(node);
    res.estate=estate;
    res.result=AbstractValue::applyOperator(op,lhsResult.result,rhsResult.result);
    resultList.push_back(res);
    return resultList;
  }
  
  list<SingleEvalResultConstInt> EStateTransferFunctions::evaluateExpression(SgNode* node,EState estate, EvalMode mode) {
    ROSE_ASSERT(estate.pstate()); // ensure state exists
    if(_analyzer->getOptionsRef().info.printTransferFunctionInfo) {
      printEvaluateExpressionInfo(node,estate,mode);
    }
    
  
    // initialize with default values from argument(s)
    SingleEvalResultConstInt res;
    res.estate=estate;
    res.result=AbstractValue::createBot();

    if(mode==EStateTransferFunctions::MODE_EMPTY_STATE) {
      if(isSgAssignOp(node)||isSgFunctionCallExp(node)||isSgVarRefExp(node)||isSgPlusPlusOp(node)||isSgMinusMinusOp(node)||isSgCompoundAssignOp(node)||isSgConditionalExp(node)) {
	res.result=AbstractValue::createTop();
	return listify(res);
      }
      //cout<<"DEBUG: evalExp at: "<<AstTerm::astTermWithNullValuesToString(node)<<endl;
    }

#if 0
    if(SgNodeHelper::isPostfixIncDecOp(node)) {
      cerr << "Error: incdec-op not supported in conditions."<<endl;
      exit(1);
    }
#endif
    if(SgStatementExpression* gnuExtensionStmtExpr=isSgStatementExpression(node)) {
      //cout<<"WARNING: ignoring GNU extension StmtExpr."<<endl;
      res.result=AbstractValue::createTop();
      return listify(res);
    }
  
    if(SgConditionalExp* condExp=isSgConditionalExp(node)) {
      return evalConditionalExpr(condExp,estate,mode);
    }

    if(dynamic_cast<SgBinaryOp*>(node)) {
      // special handling of short-circuit operators
      if(isSgAndOp(node)||isSgOrOp(node)) {
	return evaluateShortCircuitOperators(node,estate,mode);
      }
      SgNode* lhs=SgNodeHelper::getLhs(node);
      list<SingleEvalResultConstInt> lhsResultList=evaluateExpression(lhs,estate,mode);
      SgNode* rhs=SgNodeHelper::getRhs(node);
      list<SingleEvalResultConstInt> rhsResultList=evaluateExpression(rhs,estate,mode);
      list<SingleEvalResultConstInt> resultList;
      for(list<SingleEvalResultConstInt>::iterator liter=lhsResultList.begin();
	  liter!=lhsResultList.end();
	  ++liter) {
	for(list<SingleEvalResultConstInt>::iterator riter=rhsResultList.begin();
	    riter!=rhsResultList.end();
	    ++riter) {
	  SingleEvalResultConstInt lhsResult=*liter;
	  SingleEvalResultConstInt rhsResult=*riter;

	  // handle binary pointer operators
	  switch(node->variantT()) {
	    CASE_EXPR_ANALYZER_EVAL(SgArrowExp,evalArrowOp);
	    CASE_EXPR_ANALYZER_EVAL(SgDotExp,evalDotOp);
	  default:
	    // fall through;
	    ;
	  }
	  if(node->variantT()==V_SgArrowExp||node->variantT()==V_SgDotExp) {
	    return resultList;
	  }

	  switch(node->variantT()) {
	    CASE_EXPR_ANALYZER_EVAL(SgEqualityOp,evalEqualOp);
	    CASE_EXPR_ANALYZER_EVAL(SgNotEqualOp,evalNotEqualOp);
	    CASE_EXPR_ANALYZER_EVAL(SgAddOp,evalAddOp);
	    CASE_EXPR_ANALYZER_EVAL(SgSubtractOp,evalSubOp);
	    CASE_EXPR_ANALYZER_EVAL(SgMultiplyOp,evalMulOp);
	    CASE_EXPR_ANALYZER_EVAL(SgDivideOp,evalDivOp);
	    CASE_EXPR_ANALYZER_EVAL(SgModOp,evalModOp);
	    CASE_EXPR_ANALYZER_EVAL(SgBitAndOp,evalBitwiseAndOp);
	    CASE_EXPR_ANALYZER_EVAL(SgBitOrOp,evalBitwiseOrOp);
	    CASE_EXPR_ANALYZER_EVAL(SgBitXorOp,evalBitwiseXorOp);
	    CASE_EXPR_ANALYZER_EVAL(SgGreaterOrEqualOp,evalGreaterOrEqualOp);
	    CASE_EXPR_ANALYZER_EVAL(SgGreaterThanOp,evalGreaterThanOp);
	    CASE_EXPR_ANALYZER_EVAL(SgLessThanOp,evalLessThanOp);
	    CASE_EXPR_ANALYZER_EVAL(SgLessOrEqualOp,evalLessOrEqualOp);
	    CASE_EXPR_ANALYZER_EVAL(SgLshiftOp,evalBitwiseShiftLeftOp);
	    CASE_EXPR_ANALYZER_EVAL(SgRshiftOp,evalBitwiseShiftRightOp);

	    CASE_EXPR_ANALYZER_EVAL(SgPntrArrRefExp,evalArrayReferenceOp);
	    CASE_EXPR_ANALYZER_EVAL(SgCommaOpExp,evalCommaOp);
	  case V_SgAssignOp: {
	    list<SingleEvalResultConstInt> l=evaluateLExpression(lhs,estate);
	    ROSE_ASSERT(l.size()==1);
	    auto lhsAddress=*l.begin();
	    resultList.splice(resultList.end(),evalAssignOp(isSgAssignOp(node),lhsResult /*ignored*/,rhsResult,estate,mode));
	    break;
	  }
	  default:
	    cerr << "Binary Op:"<<SgNodeHelper::nodeToString(node)<<"(nodetype:"<<node->class_name()<<")"<<endl;
	    throw CodeThorn::Exception("Error: evaluateExpression::unknown binary operation.");
	  }
	}
      }
      return resultList;
    }

    if(isLValueOp(node)) {
      if(mode==EStateTransferFunctions::MODE_EMPTY_STATE) {
	// arbitrary value
	res.init(estate,CodeThorn::Top());
	return listify(res);
      }
      SgNode* child=SgNodeHelper::getFirstChild(node);
      list<SingleEvalResultConstInt> operandResultList=evaluateLExpression(child,estate);
      //assert(operandResultList.size()==1);
      list<SingleEvalResultConstInt> resultList;
      for(auto oiter:operandResultList) {
	SingleEvalResultConstInt operandResult=oiter;
	switch(node->variantT()) {
	  // covers same operators as isLValueOp
	  CASE_EXPR_ANALYZER_EVAL_UNARY_OP(SgAddressOfOp,evalAddressOfOp);
	  CASE_EXPR_ANALYZER_EVAL_UNARY_OP(SgPlusPlusOp,evalPlusPlusOp);
	  CASE_EXPR_ANALYZER_EVAL_UNARY_OP(SgMinusMinusOp,evalMinusMinusOp);
	  // SgPointerDerefExp??
	default:
	  ; // nothing to do, fall through to next loop on unary ops
	}
      }
      return resultList;
    }

    if(dynamic_cast<SgUnaryOp*>(node)) {
      SgNode* child=SgNodeHelper::getFirstChild(node);
      list<SingleEvalResultConstInt> operandResultList=evaluateExpression(child,estate,mode);
      list<SingleEvalResultConstInt> resultList;
      for(auto oiter:operandResultList) {
	SingleEvalResultConstInt operandResult=oiter;
	switch(node->variantT()) {
	  CASE_EXPR_ANALYZER_EVAL_UNARY_OP(SgNotOp,evalNotOp);
	  CASE_EXPR_ANALYZER_EVAL_UNARY_OP(SgCastExp,evalCastOp);
	  CASE_EXPR_ANALYZER_EVAL_UNARY_OP(SgBitComplementOp,evalBitwiseComplementOp);
	  CASE_EXPR_ANALYZER_EVAL_UNARY_OP(SgMinusOp,evalUnaryMinusOp);
	  CASE_EXPR_ANALYZER_EVAL_UNARY_OP(SgPointerDerefExp,evalDereferenceOp);
	default:
	  SAWYER_MESG(logger[ERROR])<<"evaluateExpression::unknown unary operation @"<<node->sage_class_name()<<endl;
	  exit(1);
	} // end switch
      }
      return  resultList;
    }

    ROSE_ASSERT(!dynamic_cast<SgBinaryOp*>(node) && !dynamic_cast<SgUnaryOp*>(node));

    // ALL REMAINING CASES DO NOT GENERATE CONSTRAINTS
    // EXPRESSION LEAF NODES
    // this test holds for all subclasses of SgValueExp

    // special case sizeof operator (operates on types and types of expressions)
    if(SgSizeOfOp* sizeOfOp=isSgSizeOfOp(node)) {
      return evalSizeofOp(sizeOfOp,estate);
    }
    if(SgValueExp* exp=isSgValueExp(node)) {
      ROSE_ASSERT(exp!=nullptr);
      return evalValueExp(exp,estate,mode);
    }
    switch(node->variantT()) {
    case V_SgVarRefExp:
      return evalRValueVarRefExp(isSgVarRefExp(node),estate,mode);
    case V_SgFunctionCallExp: {
      return evalFunctionCall(isSgFunctionCallExp(node),estate);
    }
    case V_SgNullExpression: {
      list<SingleEvalResultConstInt> resultList;
      res.result=AbstractValue::createTop();
      resultList.push_front(res);
      return resultList;
    }
    case V_SgFunctionRefExp: {
      if(getIgnoreFunctionPointers()) {
	// just ignore the call (this is unsound and only for testing)
	list<SingleEvalResultConstInt> resultList;
	res.result=AbstractValue::createTop();
	resultList.push_front(res);
	return resultList;
      } else {
	// use of function addresses as values. Being implemented now.
	//SAWYER_MESG(logger[WARN])<<"Imprecision: function pointer value: evaluating SgFunctionRefExp as top: "<<SgNodeHelper::sourceLineColumnToString(node)<<": "<<node->unparseToString()<<endl;
	return evalFunctionRefExp(isSgFunctionRefExp(node),estate,mode);
      }
    }
    case V_SgThisExp: {
      res.result=AbstractValue::createTop();
      printLoggerWarning(estate);
      return listify(res);
    }
    case V_SgNewExp: {
      res.result=AbstractValue::createTop();
      printLoggerWarning(estate);
      return listify(res);
    }
    case V_SgDeleteExp: {
      res.result=AbstractValue::createTop();
      printLoggerWarning(estate);
      return listify(res);
    }
    case V_SgAssignInitializer: {
      res.result=AbstractValue::createTop();
      printLoggerWarning(estate);
      return listify(res);
    }
    case V_SgConstructorInitializer: {
      res.result=AbstractValue::createTop();
      printLoggerWarning(estate);
      return listify(res);
    }
    case V_SgAggregateInitializer: {
      res.result=AbstractValue::createTop();
      SAWYER_MESG(logger[WARN])<<"AggregateInitializer in expression (only handled at stmt level, returning top)"<<endl;
      return listify(res);
    }
    case V_SgExprListExp: {
      res.result=AbstractValue::createTop();
      SAWYER_MESG(logger[WARN])<<"AggregateInitializer's ExprListExp in expression (only handled at stmt level, returning top)"<<endl;
      return listify(res);
    }
    default:
      throw CodeThorn::Exception("Error: evaluateExpression::unknown node in expression: "+string(node->sage_class_name())+" at "+SgNodeHelper::sourceFilenameToString(node)+" in file "+SgNodeHelper::sourceFilenameToString(node)+string(":")+node->unparseToString());
    } // end of switch
    throw CodeThorn::Exception("Error: evaluateExpression failed.");
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////
  // EVAL FUNCTIONS
  /////////////////////////////////////////////////////////////////////////////////////////////////

  // evaluation functions
  list<SingleEvalResultConstInt> EStateTransferFunctions::evalConditionalExpr(SgConditionalExp* condExp, EState estate, EvalMode mode) {
    list<SingleEvalResultConstInt> resultList;
    SgExpression* cond=condExp->get_conditional_exp();
    list<SingleEvalResultConstInt> condResultList=evaluateExpression(cond,estate);
    if(condResultList.size()==0) {
      cerr<<"Error: evaluating condition of conditional operator inside expressions gives no result."<<endl;
      exit(1);
    }
    if(condResultList.size()==2) {
      list<SingleEvalResultConstInt>::iterator i=condResultList.begin();
      SingleEvalResultConstInt singleResult1=*i;
      ++i;
      SingleEvalResultConstInt singleResult2=*i;
      if((singleResult1.value().operatorEq(singleResult2.value())).isTrue()) {
	SAWYER_MESG(logger[WARN])<<"evaluating condition of conditional operator gives two equal results"<<endl;
      }
    }
    if(condResultList.size()>1) {
      SAWYER_MESG(logger[ERROR])<<"evaluating condition of conditional operator gives more than one result. Normalization required."<<endl;
      exit(1);
    }
    SingleEvalResultConstInt singleResult=*condResultList.begin();
    if(singleResult.result.isTop()) {
      SgExpression* trueBranch=condExp->get_true_exp();
      list<SingleEvalResultConstInt> trueBranchResultList=evaluateExpression(trueBranch,estate);
      SgExpression* falseBranch=condExp->get_false_exp();
      list<SingleEvalResultConstInt> falseBranchResultList=evaluateExpression(falseBranch,estate);
      // append falseBranchResultList to trueBranchResultList (moves elements), O(1).
      trueBranchResultList.splice(trueBranchResultList.end(), falseBranchResultList);
      return trueBranchResultList;
    } else if(singleResult.result.isTrue()) {
      SgExpression* trueBranch=condExp->get_true_exp();
      list<SingleEvalResultConstInt> trueBranchResultList=evaluateExpression(trueBranch,estate);
      return trueBranchResultList;
    } else if(singleResult.result.isFalse()) {
      SgExpression* falseBranch=condExp->get_false_exp();
      list<SingleEvalResultConstInt> falseBranchResultList=evaluateExpression(falseBranch,estate);
      return falseBranchResultList;
    } else {
      cerr<<"Error: evaluating conditional operator inside expressions - unknown behavior (condition may have evaluated to bot)."<<endl;
      exit(1);
    }
  }

  list<SingleEvalResultConstInt> EStateTransferFunctions::evalCommaOp(SgCommaOpExp* node,
								      SingleEvalResultConstInt lhsResult,
								      SingleEvalResultConstInt rhsResult,
								      EState estate, EvalMode mode) {
    list<SingleEvalResultConstInt> resultList;
    SingleEvalResultConstInt res;
    res.estate=estate;
    // lhsResult is ignored in comma op
    // result of the expression is the rhs's value
    res.result=rhsResult.result;
    resultList.push_back(res);
    return resultList;
  }

  list<SingleEvalResultConstInt> EStateTransferFunctions::evalEqualOp(SgEqualityOp* node,
								      SingleEvalResultConstInt lhsResult,
								      SingleEvalResultConstInt rhsResult,
								      EState estate, EvalMode mode) {
    list<SingleEvalResultConstInt> resultList;
    SingleEvalResultConstInt res;
    res.estate=estate;
    res.result=(lhsResult.result.operatorEq(rhsResult.result));
    resultList.push_back(res);
    return resultList;
  }

  list<SingleEvalResultConstInt> EStateTransferFunctions::evalNotEqualOp(SgNotEqualOp* node,
									 SingleEvalResultConstInt lhsResult,
									 SingleEvalResultConstInt rhsResult,
									 EState estate, EvalMode mode) {
    list<SingleEvalResultConstInt> resultList;
    SingleEvalResultConstInt res;
    res.estate=estate;
    res.result=(lhsResult.result.operatorNotEq(rhsResult.result));
    resultList.push_back(res);
    return resultList;
  }

  list<SingleEvalResultConstInt> EStateTransferFunctions::evalAndOp(SgAndOp* node,
								    SingleEvalResultConstInt lhsResult,
								    SingleEvalResultConstInt rhsResult,
								    EState estate, EvalMode mode) {
    list<SingleEvalResultConstInt> resultList;
    SingleEvalResultConstInt res;
    res.estate=estate;
    res.result=(lhsResult.result.operatorAnd(rhsResult.result));
    resultList.push_back(res);
    return resultList;
  }

  list<SingleEvalResultConstInt> EStateTransferFunctions::evalOrOp(SgOrOp* node,
								   SingleEvalResultConstInt lhsResult,
								   SingleEvalResultConstInt rhsResult,
								   EState estate, EvalMode mode) {
    list<SingleEvalResultConstInt> resultList;
    SingleEvalResultConstInt res;
    res.estate=estate;
    res.result=lhsResult.result.operatorOr(rhsResult.result);

    // encode short-circuit CPP-OR-semantics
    if(lhsResult.result.isTrue()) {
      res.result=lhsResult.result;
    }
    resultList.push_back(res);
    return resultList;
  }

  list<SingleEvalResultConstInt> EStateTransferFunctions::evalAddOp(SgAddOp* node,
								    SingleEvalResultConstInt lhsResult,
								    SingleEvalResultConstInt rhsResult,
								    EState estate, EvalMode mode) {
    list<SingleEvalResultConstInt> resultList;
    SingleEvalResultConstInt res;
    res.estate=estate;
    res.result=AbstractValue::operatorAdd(lhsResult.result,rhsResult.result);
    resultList.push_back(res);
    return resultList;
  }
  list<SingleEvalResultConstInt> EStateTransferFunctions::evalSubOp(SgSubtractOp* node,
								    SingleEvalResultConstInt lhsResult,
								    SingleEvalResultConstInt rhsResult,
								    EState estate, EvalMode mode) {
    list<SingleEvalResultConstInt> resultList;
    SingleEvalResultConstInt res;
    res.estate=estate;
    res.result=AbstractValue::operatorSub(lhsResult.result,rhsResult.result);
    resultList.push_back(res);
    return resultList;
  }
  list<SingleEvalResultConstInt> EStateTransferFunctions::evalMulOp(SgMultiplyOp* node,
								    SingleEvalResultConstInt lhsResult,
								    SingleEvalResultConstInt rhsResult,
								    EState estate, EvalMode mode) {

    list<SingleEvalResultConstInt> resultList;
    SingleEvalResultConstInt res;
    res.estate=estate;
    res.result=AbstractValue::operatorMul(lhsResult.result,rhsResult.result);
    resultList.push_back(res);
    return resultList;
  }
  list<SingleEvalResultConstInt> EStateTransferFunctions::evalDivOp(SgDivideOp* node,
								    SingleEvalResultConstInt lhsResult,
								    SingleEvalResultConstInt rhsResult,
								    EState estate, EvalMode mode) {

    list<SingleEvalResultConstInt> resultList;
    SingleEvalResultConstInt res;
    res.estate=estate;
    res.result=AbstractValue::operatorDiv(lhsResult.result,rhsResult.result);
    resultList.push_back(res);
    return resultList;
  }

  list<SingleEvalResultConstInt> EStateTransferFunctions::evalModOp(SgModOp* node,
								    SingleEvalResultConstInt lhsResult,
								    SingleEvalResultConstInt rhsResult,
								    EState estate, EvalMode mode) {

    list<SingleEvalResultConstInt> resultList;
    SingleEvalResultConstInt res;
    res.estate=estate;
    res.result=AbstractValue::operatorMod(lhsResult.result,rhsResult.result);
    resultList.push_back(res);
    return resultList;
  }

  list<SingleEvalResultConstInt> EStateTransferFunctions::evalBitwiseAndOp(SgBitAndOp* node,
									   SingleEvalResultConstInt lhsResult,
									   SingleEvalResultConstInt rhsResult,
									   EState estate, EvalMode mode) {
    list<SingleEvalResultConstInt> resultList;
    SingleEvalResultConstInt res;
    res.estate=estate;
    res.result=(lhsResult.result.operatorBitwiseAnd(rhsResult.result));
    resultList.push_back(res);
    return resultList;
  }

  list<SingleEvalResultConstInt> EStateTransferFunctions::evalBitwiseOrOp(SgBitOrOp* node,
									  SingleEvalResultConstInt lhsResult,
									  SingleEvalResultConstInt rhsResult,
									  EState estate, EvalMode mode) {
    list<SingleEvalResultConstInt> resultList;
    SingleEvalResultConstInt res;
    res.estate=estate;
    res.result=(lhsResult.result.operatorBitwiseOr(rhsResult.result));
    resultList.push_back(res);
    return resultList;
  }

  list<SingleEvalResultConstInt> EStateTransferFunctions::evalBitwiseXorOp(SgBitXorOp* node,
									   SingleEvalResultConstInt lhsResult,
									   SingleEvalResultConstInt rhsResult,
									   EState estate, EvalMode mode) {
    list<SingleEvalResultConstInt> resultList;
    SingleEvalResultConstInt res;
    res.estate=estate;
    res.result=(lhsResult.result.operatorBitwiseXor(rhsResult.result));
    resultList.push_back(res);
    return resultList;
  }

  list<SingleEvalResultConstInt>
  EStateTransferFunctions::evalGreaterOrEqualOp(SgGreaterOrEqualOp* node,
						SingleEvalResultConstInt lhsResult,
						SingleEvalResultConstInt rhsResult,
						EState estate, EvalMode mode) {
    list<SingleEvalResultConstInt> resultList;
    SingleEvalResultConstInt res;
    res.estate=estate;
    res.result=(lhsResult.result.operatorMoreOrEq(rhsResult.result));
    resultList.push_back(res);
    return resultList;
  }

  list<SingleEvalResultConstInt>
  EStateTransferFunctions::evalGreaterThanOp(SgGreaterThanOp* node,
					     SingleEvalResultConstInt lhsResult,
					     SingleEvalResultConstInt rhsResult,
					     EState estate, EvalMode mode) {
    list<SingleEvalResultConstInt> resultList;
    SingleEvalResultConstInt res;
    res.estate=estate;
    res.result=(lhsResult.result.operatorMore(rhsResult.result));
    resultList.push_back(res);
    return resultList;
  }

  list<SingleEvalResultConstInt>
  EStateTransferFunctions::evalAssignOp(SgAssignOp* node,
					SingleEvalResultConstInt lhsResult,
					SingleEvalResultConstInt rhsResult,
					EState estate, EvalMode mode) {
    list<SingleEvalResultConstInt> resultList;
    SingleEvalResultConstInt res;
    Edge fakeEdge;
    ROSE_ASSERT(_analyzer);
    ROSE_ASSERT(_analyzer->getEStateTransferFunctions());
    std::list<EState> estateList=_analyzer->getEStateTransferFunctions()->transferAssignOp(node,fakeEdge,&estate);
    ROSE_ASSERT(estateList.size()==1);
    res.result=rhsResult.result; // value result of assignment
    res.estate=*estateList.begin();
    resultList.push_back(res);
    return resultList;
  }

  list<SingleEvalResultConstInt>
  EStateTransferFunctions::evalLessOrEqualOp(SgLessOrEqualOp* node,
					     SingleEvalResultConstInt lhsResult,
					     SingleEvalResultConstInt rhsResult,
					     EState estate, EvalMode mode) {
    list<SingleEvalResultConstInt> resultList;
    SingleEvalResultConstInt res;
    res.estate=estate;
    res.result=(lhsResult.result.operatorLessOrEq(rhsResult.result));
    resultList.push_back(res);
    return resultList;
  }

  list<SingleEvalResultConstInt>
  EStateTransferFunctions::evalLessThanOp(SgLessThanOp* node,
					  SingleEvalResultConstInt lhsResult,
					  SingleEvalResultConstInt rhsResult,
					  EState estate, EvalMode mode) {
    list<SingleEvalResultConstInt> resultList;
    SingleEvalResultConstInt res;
    res.estate=estate;
    res.result=(lhsResult.result.operatorLess(rhsResult.result));
    resultList.push_back(res);
    return resultList;
  }

  list<SingleEvalResultConstInt>
  EStateTransferFunctions::evalBitwiseShiftLeftOp(SgLshiftOp* node,
						  SingleEvalResultConstInt lhsResult,
						  SingleEvalResultConstInt rhsResult,
						  EState estate, EvalMode mode) {
    list<SingleEvalResultConstInt> resultList;
    SingleEvalResultConstInt res;
    res.estate=estate;
    res.result=(lhsResult.result.operatorBitwiseShiftLeft(rhsResult.result));
    resultList.push_back(res);
    return resultList;
  }

  list<SingleEvalResultConstInt>
  EStateTransferFunctions::evalBitwiseShiftRightOp(SgRshiftOp* node,
						   SingleEvalResultConstInt lhsResult,
						   SingleEvalResultConstInt rhsResult,
						   EState estate, EvalMode mode) {
    list<SingleEvalResultConstInt> resultList;
    SingleEvalResultConstInt res;
    res.estate=estate;
    res.result=(lhsResult.result.operatorBitwiseShiftRight(rhsResult.result));
    resultList.push_back(res);
    return resultList;
  }

  list<SingleEvalResultConstInt>
  EStateTransferFunctions::evalArrayReferenceOp(SgPntrArrRefExp* node,
						SingleEvalResultConstInt arrayExprResult,
						SingleEvalResultConstInt indexExprResult,
						EState estate, EvalMode mode) {
    SAWYER_MESG(logger[TRACE])<<"evalArrayReferenceOp: "<<node->unparseToString()<<endl;
    SAWYER_MESG(logger[TRACE])<<"evalArrayReferenceOp: AST:"<<AstTerm::astTermWithNullValuesToString(node)<<endl;
    list<SingleEvalResultConstInt> resultList;
    SingleEvalResultConstInt res;
    res.estate=estate;

    if(mode==EStateTransferFunctions::MODE_EMPTY_STATE) {
      // arbitrary value
      res.result=CodeThorn::Top();
      resultList.push_back(res);
      return resultList;
    }

    SgNode* arrayExpr=SgNodeHelper::getLhs(node);
    SAWYER_MESG(logger[TRACE])<<"arrayExpr: "<<arrayExpr->unparseToString()<<endl;

    if(indexExprResult.value().isTop()||getSkipArrayAccesses()==true) {
      // set result to top when index is top [imprecision]
      // assume top for array elements if skipped
      // Precision: imprecise
      SAWYER_MESG(logger[TRACE])<<"EStateTransferFunctions::evalArrayReferenceOp: returns top"<<endl;
      res.result=CodeThorn::Top();
      resultList.push_back(res);
      return resultList;
    } else {
      if(SgVarRefExp* varRefExp=isSgVarRefExp(arrayExpr)) {
	AbstractValue arrayPtrValue=arrayExprResult.result;
	const PState* const_pstate=estate.pstate();
	PState pstate2=*const_pstate; // also removes constness
	VariableId arrayVarId=_variableIdMapping->variableId(varRefExp);
	// two cases
	if(_variableIdMapping->isOfArrayType(arrayVarId)) {
	  if(_variableIdMapping->isFunctionParameter(arrayVarId)) {
	    // function parameter of array type contains a pointer value in C/C++
	    arrayPtrValue=readFromMemoryLocation(estate.label(),&pstate2,arrayVarId); // pointer value of array function paramter only (without index)
	    SAWYER_MESG(logger[TRACE])<<"evalArrayReferenceOp:"<<" arrayPtrValue (of function parameter) read from memory, arrayPtrValue: "<<arrayPtrValue.toString(_variableIdMapping)<<endl;
	  } else {
	    arrayPtrValue=AbstractValue::createAddressOfArray(arrayVarId);
	    SAWYER_MESG(logger[TRACE])<<"evalArrayReferenceOp: created array address (from array type): "<<arrayPtrValue.toString(_variableIdMapping)<<endl;
	  }
	} else if(_variableIdMapping->isOfPointerType(arrayVarId)) {
	  // in case it is a pointer retrieve pointer value
	  SAWYER_MESG(logger[DEBUG])<<"pointer-array access."<<endl;
	  if(pstate2.varExists(arrayVarId)) {
	    arrayPtrValue=readFromMemoryLocation(estate.label(),&pstate2,arrayVarId); // pointer value (without index)
	    SAWYER_MESG(logger[TRACE])<<"evalArrayReferenceOp:"<<" arrayPtrValue read from memory (in state), arrayPtrValue:"<<arrayPtrValue.toString(_variableIdMapping)<<endl;
	    if(!(arrayPtrValue.isTop()||arrayPtrValue.isBot()||arrayPtrValue.isPtr()||arrayPtrValue.isNullPtr())) {
	      logger[ERROR]<<"@"<<SgNodeHelper::lineColumnNodeToString(node)<<": value not a pointer value: "<<arrayPtrValue.toString()<<endl;
	      logger[ERROR]<<estate.toString(_variableIdMapping)<<endl;
	      exit(1);
	    }
	  } else {
	    //cerr<<"Error: pointer variable does not exist in PState: "<<arrayVarId.toString()<<endl  ;
	    // TODO PRECISION 2
	    // variable may have been not written because abstraction is too coarse (subsummed in write to top)
	    // => reading from anywhere, returning any value
	    res.result=CodeThorn::Top();
	    resultList.push_back(res);
	    return resultList;
	  }
	} else if(_variableIdMapping->isOfReferenceType(arrayVarId)) {
	  SAWYER_MESG(logger[TRACE])<<"reference array variable access"<<endl;
	  arrayPtrValue=readFromReferenceMemoryLocation(estate.label(),&pstate2,arrayVarId);
	  if(arrayPtrValue.isBot()) {
	    // if referred memory location is not in state
	    res.result=CodeThorn::Top();
	    resultList.push_back(res);
	    return resultList;
	  }
	  //cout<<"DEBUG: array reference value: "<<arrayPtrValue.toString()<<endl;
	  //cout<<"PSTATE:"<<pstate2.toString(_variableIdMapping)<<endl;
	  //cerr<<node->unparseToString()<<" of type "<<node->get_type()->unparseToString()<<endl;
	} else {
	  cerr<<"Error: unknown type of array or pointer."<<endl;
	  cerr<<node->unparseToString()<<" of type "<<node->get_type()->unparseToString()<<endl;
	  exit(1);
	}
	AbstractValue indexExprResultValue=indexExprResult.value();
	AbstractValue arrayPtrPlusIndexValue=AbstractValue::operatorAdd(arrayPtrValue,indexExprResultValue);
	//cout<<"DEBUG: array reference value + index val: "<<arrayPtrPlusIndexValue.toString(_variableIdMapping)<<endl;
	if(arrayPtrPlusIndexValue.isNullPtr()) {
	  recordDefinitiveViolatingLocation(ANALYSIS_NULL_POINTER,estate.label()); // NP_SOUNDNESS
	  // there is no state following a definitive null pointer
	  // dereference. An error-state recording this property is
	  // created to allow analysis of errors on the programs
	  // transition graph. In addition the property is also recorded in the _nullPointerDereferenceLocations list.
	  res.result=CodeThorn::Top(); // consider returning bot here?
	  // verification error states are detected in the solver and no successor states are computed.
	  res.estate.io.recordVerificationError();
	  resultList.push_back(res);
	  return resultList;
	}
	if(pstate2.varExists(arrayPtrValue)) {
	  // required for the following index computation (nothing to do here)
	} else {
	  if(arrayPtrValue.isTop()) {
	    //logger[ERROR]<<"@"<<SgNodeHelper::lineColumnNodeToString(node)<<" evalArrayReferenceOp: pointer is top. Pointer abstraction too coarse."<<endl;
	    // TODO: PRECISION 1
	    res.result=CodeThorn::Top();
	    recordPotentialNullPointerDereferenceLocation(estate.label()); // NP_SOUNDNESS
	    //if(_analyzer->getAbstractionMode()!=3) recordPotentialOutOfBoundsAccessLocation(estate.label());
	    recordPotentialViolatingLocation(ANALYSIS_UNINITIALIZED,estate.label()); // UNINIT_SOUNDNESS
	    resultList.push_back(res);
	    return resultList;
	  } else {
	    res.result=CodeThorn::Top();
	    resultList.push_back(res);
	    return resultList;
	  }
	  exit(1); // not reachable
	}
	if(pstate2.varExists(arrayPtrPlusIndexValue)) {
	  // address of denoted memory location
	  switch(mode) {
	  case MODE_VALUE:
	    res.result=readFromMemoryLocation(estate.label(),&pstate2,arrayPtrPlusIndexValue);
	    SAWYER_MESG(logger[TRACE])<<"retrieved array element value:"<<res.result<<endl;
	    return listify(res);
	  case MODE_ADDRESS:
	    res.result=arrayPtrPlusIndexValue;
	    return listify(res);
	  default:
	    SAWYER_MESG(logger[FATAL])<<"Internal error: evalArrayReferenceOp: unsupported EvalMode."<<endl;
	    exit(1);
	  }
	} else {
	  SAWYER_MESG(logger[WARN])<<"evalArrayReferenceOp:"<<" memory location not in state: "<<arrayPtrPlusIndexValue.toString(_variableIdMapping)<<endl;
	  SAWYER_MESG(logger[WARN])<<"evalArrayReferenceOp:"<<pstate2.toString(_variableIdMapping)<<endl;

	  if(mode==MODE_ADDRESS) {
	    SAWYER_MESG(logger[WARN])<<"EStateTransferFunctions::evalArrayReferenceOp: address mode not possible for variables not in state."<<endl;
	    res.result=CodeThorn::Top();
	    resultList.push_back(res);
	    return resultList;
	  }
	  // array variable NOT in state. Special space optimization case for constant array.
	  if(_variableIdMapping->isOfArrayType(arrayVarId)) {
	    if(_variableIdMapping->hasAssignInitializer(arrayVarId)) {
	      // special case of string initializer: x[2]="";
	      SAWYER_MESG(logger[WARN])<<"array assign initializer is not supported yet (assuming any value):"<<node->unparseToString()<<" AST:"<<AstTerm::astTermWithNullValuesToString(node)<<endl;
	      AbstractValue val=AbstractValue::createTop();
	      res.result=val;
	      return listify(res);
	    }
	    if(_variableIdMapping->isAggregateWithInitializerList(arrayVarId)) {
	      SgExpressionPtrList& initList=_variableIdMapping->getInitializerListOfArrayVariable(arrayVarId);
	      int elemIndex=0;
	      // TODO: slow linear lookup (TODO: pre-compute all values and provide access function)
	      for(SgExpressionPtrList::iterator i=initList.begin();i!=initList.end();++i) {
		SgExpression* exp=*i;
		SgAssignInitializer* assignInit=isSgAssignInitializer(exp);
		if(assignInit) {
		  SgExpression* initExp=assignInit->get_operand_i();
		  ROSE_ASSERT(initExp);
		  if(SgIntVal* intValNode=isSgIntVal(initExp)) {
		    int intVal=intValNode->get_value();
		    //newPState.writeToMemoryLocation(arrayElemId,CodeThorn::AbstractValue(AbstractValue(intVal)));
		    int index2=arrayPtrPlusIndexValue.getIndexIntValue();
		    if(elemIndex==index2) {
		      AbstractValue val=AbstractValue(intVal); // TODO BYTEMODE
		      res.result=val;
		      return listify(res);
		    }
		  } else {
		    SAWYER_MESG(logger[WARN])<<"unsupported array initializer value (assuming any value):"<<exp->unparseToString()<<" AST:"<<AstTerm::astTermWithNullValuesToString(exp)<<endl;
		    AbstractValue val=AbstractValue::createTop();
		    res.result=val;
		    return listify(res);
		  }
		} else {
		  SAWYER_MESG(logger[WARN])<<"no assign initialize:"<<exp->unparseToString()<<" AST:"<<AstTerm::astTermWithNullValuesToString(exp)<<endl;
		  AbstractValue val=AbstractValue::createTop();
		  res.result=val;
		  return listify(res);
		}
		elemIndex++;
	      }
	    } else {
	      // no initializer, initialize with top
	      AbstractValue val=AbstractValue::createTop();
	      res.result=val;
	      return listify(res);
	    }
	  } else if(_variableIdMapping->isStringLiteralAddress(arrayVarId)) {
	    SAWYER_MESG(logger[ERROR])<<"Error: Found string literal address, but data not present in state."<<endl;
	    exit(1);
	  } else {
	    //cout<<estate.toString(_variableIdMapping)<<endl;
	    SAWYER_MESG(logger[TRACE])<<"Program error detected: potential out of bounds access (P1) : array: "<<arrayPtrValue.toString(_variableIdMapping)<<", access: address: "<<arrayPtrPlusIndexValue.toString(_variableIdMapping)<<endl;
	    //cout<<"DEBUG: array-element: "<<arrayPtrPlusIndexValue.toString(_variableIdMapping)<<endl;
	    //cerr<<"PState: "<<pstate->toString(_variableIdMapping)<<endl;
	    //cerr<<"AST: "<<node->unparseToString()<<endl;
	    recordPotentialViolatingLocation(ANALYSIS_NULL_POINTER,estate.label()); // NP_SOUNDNESS
	    // continue after potential out-of-bounds access (assume any value can have been read)
	    AbstractValue val=AbstractValue::createTop();
	    res.result=val;
	    return listify(res);
	  }
	}
      } else {
	SAWYER_MESG(logger[WARN])<<"Array-access uses expr for denoting the array (not supported yet) ";
	SAWYER_MESG(logger[WARN])<<"@"<<SgNodeHelper::lineColumnNodeToString(node)<<" ";
	SAWYER_MESG(logger[WARN])<<"expr: "<<arrayExpr->unparseToString()<<" ";
	SAWYER_MESG(logger[WARN])<<"arraySkip: "<<getSkipArrayAccesses()<<endl;
      }
      return resultList;
    }
    ROSE_ASSERT(false); // not reachable
  }

  list<SingleEvalResultConstInt> EStateTransferFunctions::evalNotOp(SgNotOp* node,
								    SingleEvalResultConstInt operandResult,
								    EState estate, EvalMode mode) {
    SingleEvalResultConstInt res;
    res.estate=estate;
    res.result=operandResult.result.operatorNot();
    return listify(res);
  }
  list<SingleEvalResultConstInt> EStateTransferFunctions::evalUnaryMinusOp(SgMinusOp* node,
									   SingleEvalResultConstInt operandResult,
									   EState estate, EvalMode mode) {
    SingleEvalResultConstInt res;
    res.estate=estate;
    res.result=operandResult.result.operatorUnaryMinus();
    return listify(res);
  }

  list<SingleEvalResultConstInt> EStateTransferFunctions::evalSizeofOp(SgSizeOfOp* node,
								       EState estate, EvalMode mode) {
    SAWYER_MESG(logger[TRACE])<<"evalSizeofOp(started):"<<node->unparseToString()<<endl;
    SgType* operandType=node->get_operand_type();
    CodeThorn::TypeSize typeSize=0; // remains zero if no size can be determined
    AbstractValue sizeValue=AbstractValue::createTop();

    if(operandType) {
      typeSize=_variableIdMapping->getTypeSize(operandType);
    } else if(SgExpression* exp=node->get_operand_expr()) {
      if(SgVarRefExp* varRefExp=isSgVarRefExp(exp)) {
	typeSize=_variableIdMapping->getTypeSize(_variableIdMapping->variableId(varRefExp));
      } else if(SgType* expType=exp->get_type()) {
	typeSize=_variableIdMapping->getTypeSize(expType);
      } else {
	SAWYER_MESG(logger[WARN])<<"sizeof: could not determine any type of sizeof argument and unsupported argument expression: "<<SgNodeHelper::sourceLineColumnToString(exp)<<": "<<exp->unparseToString()<<endl<<AstTerm::astTermWithNullValuesToDot(exp)<<endl;
      }
    } else {
      SAWYER_MESG(logger[WARN]) <<"sizeof: could not determine any type of sizeof argument and no expression found either: "<<SgNodeHelper::sourceLineColumnToString(exp)<<": "<<exp->unparseToString()<<endl;
    }
    logger[TRACE]<<"evalSizeofOp(6):"<<node->unparseToString()<<endl;

    // determines sizeValue based on typesize
    if(typeSize==0) {
      SAWYER_MESG(logger[WARN])<<"sizeof: could not determine size (= zero) of argument, assuming top "<<SgNodeHelper::sourceLineColumnToString(node)<<": "<<node->unparseToString()<<endl;
      sizeValue=AbstractValue::createTop();
    } else {
      SAWYER_MESG(logger[TRACE])<<"DEBUG: @"<<SgNodeHelper::sourceLineColumnToString(node)<<": sizeof("<<typeSize<<")"<<endl;
      sizeValue=AbstractValue(typeSize);
      SAWYER_MESG(logger[TRACE])<<"DEBUG: @"<<SgNodeHelper::sourceLineColumnToString(node)<<": sizevalue of sizeof("<<typeSize<<"):"<<sizeValue.toString()<<endl;
    }
    SingleEvalResultConstInt res;
    res.init(estate,sizeValue);
    SAWYER_MESG(logger[TRACE])<<"evalSizeofOp(finished):"<<node->unparseToString()<<endl;
    return listify(res);
  }

  list<SingleEvalResultConstInt> EStateTransferFunctions::evalCastOp(SgCastExp* node,
								     SingleEvalResultConstInt operandResult,
								     EState estate, EvalMode mode) {
    // TODO: truncation of values
    // TODO: adapt pointer value element size
    SgType* targetType=node->get_type();
    targetType=targetType->stripType(SgType::STRIP_TYPEDEF_TYPE|SgType::STRIP_MODIFIER_TYPE);
    if(AbstractValue::byteMode) {
      if(SgPointerType* ptrType=isSgPointerType(targetType)) {
	SgType* elementType=ptrType->get_base_type();
	long int elementTypeSize=_variableIdMapping->getTypeSize(elementType);
	SAWYER_MESG(logger[DEBUG])<<"casting pointer to element type size: "<<elementTypeSize<<":"<<elementType->unparseToString()<<endl;
	if(AbstractValue::byteMode) {
	  operandResult.result.setElementTypeSize(elementTypeSize);
	}
      }
    }
    SingleEvalResultConstInt res;
    res.init(estate,operandResult.result);
    return listify(res);
  }

  list<SingleEvalResultConstInt> EStateTransferFunctions::evalBitwiseComplementOp(SgBitComplementOp* node,
										  SingleEvalResultConstInt operandResult,
										  EState estate, EvalMode mode) {
    SingleEvalResultConstInt res;
    res.estate=estate;
    res.result=operandResult.result.operatorBitwiseComplement();
    return listify(res);
  }

  AbstractValue EStateTransferFunctions::computeAbstractAddress(SgVarRefExp* varRefExp) {
    VariableId varId=_variableIdMapping->variableId(varRefExp);
    return AbstractValue::createAddressOfVariable(varId);
  }

  list<SingleEvalResultConstInt> EStateTransferFunctions::evalArrowOp(SgArrowExp* node,
								      SingleEvalResultConstInt lhsResult,
								      SingleEvalResultConstInt rhsResult,
								      EState estate, EvalMode mode) {
    list<SingleEvalResultConstInt> resultList;
    SingleEvalResultConstInt res;
    res.estate=estate;

    if(mode==EStateTransferFunctions::MODE_EMPTY_STATE) {
      // arbitrary value
      res.result=CodeThorn::Top();
      resultList.push_back(res);
      return resultList;
    }

    // L->R : L evaluates to pointer value (address), R evaluates to offset value (a struct member always evaluates to an offset)
    //AbstractValue address=lhsResult.result;
    //cout<<"DEBUG: ArrowOp: address(lhs):"<<address.toString(_variableIdMapping)<<endl;
    //AbstractValue referencedAddress=estate.pstate()->readFromMemoryLocation(address);
    AbstractValue referencedAddress=lhsResult.result;
    bool continueExec=checkAndRecordNullPointer(referencedAddress, estate.label());
    if(continueExec) {
      SAWYER_MESG(logger[TRACE])<<"ArrowOp: referencedAddress(lhs):"<<referencedAddress.toString(_variableIdMapping)<<endl;
      AbstractValue offset=rhsResult.result;
      AbstractValue denotedAddress=AbstractValue::operatorAdd(referencedAddress,offset);
      SAWYER_MESG(logger[TRACE])<<"ArrowOp: denoted Address(lhs):"<<denotedAddress.toString(_variableIdMapping)<<endl;

      switch(mode) {
      case MODE_VALUE:
	SAWYER_MESG(logger[TRACE])<<"Arrow op: reading value from arrowop-struct location."<<denotedAddress.toString(_variableIdMapping)<<endl;
	res.result=readFromMemoryLocation(estate.label(),estate.pstate(),denotedAddress);
	break;
      case MODE_ADDRESS:
	res.result=denotedAddress;
	break;
      case MODE_EMPTY_STATE:
	throw CodeThorn::Exception("Interal error: EStateTransferFunctions::evalArrowOp: empty state mode.");
	// intentionally no default case, all cases must be represented
      }
      resultList.push_back(res);
      return resultList;
    } else {
      list<SingleEvalResultConstInt> empty;
      return empty;
    }
  }

  list<SingleEvalResultConstInt> EStateTransferFunctions::evalDotOp(SgDotExp* node,
								    SingleEvalResultConstInt lhsResult,
								    SingleEvalResultConstInt rhsResult,
								    EState estate, EvalMode mode) {
    list<SingleEvalResultConstInt> resultList;
    SingleEvalResultConstInt res;
    res.estate=estate;
    // L.R : L evaluates to address, R evaluates to offset value (a struct member always evaluates to an offset)
    //SAWYER_MESG(logger[DEBUG])<<"DotOp: lhs:"<<lhsResult.result.toString(_variableIdMapping)<<" rhs: "<<rhsResult.result.toString(_variableIdMapping)<<endl;
    //cout<<"DotOp: "<<node->unparseToString()<<":"<<lhsResult.result.toString(_variableIdMapping)<<"."<<rhsResult.result.toString(_variableIdMapping)<<endl;
  
    if(mode==EStateTransferFunctions::MODE_EMPTY_STATE) {
      // arbitrary value
      res.result=CodeThorn::Top();
      resultList.push_back(res);
      return resultList;
    }

    // lhs of dot-op should never be 0 (except if uninitialized memory is used)
    bool continueExec=checkAndRecordNullPointer(lhsResult.result, estate.label());
    if(continueExec) {
      AbstractValue address=AbstractValue::operatorAdd(lhsResult.result,rhsResult.result);
      // only if rhs is *not* a dot-operator, needs the value be
      // read. Otherwise this is not the end of the access path and only the address is computed.
      if(true||!isSgDotExp(SgNodeHelper::getRhs(node))) {
	// reached end of dot sequence (a.b.<here>c)
	switch(mode) {
	case MODE_VALUE:
	  SAWYER_MESG(logger[TRACE])<<"Dot op: reading from struct location."<<address.toString(_variableIdMapping)<<endl;
	  res.result=readFromMemoryLocation(estate.label(),estate.pstate(),address);
	  break;
	case MODE_ADDRESS:
	  res.result=address;
	  break;
	case MODE_EMPTY_STATE:
	  throw CodeThorn::Exception("Interal error: EStateTransferFunctions::evalArrowOp: empty state mode.");
	  // intentionally no default case, all cases must be represented
	} 
      } else {
	// evaluation of dot sequence (a.<here>b.c)
	res.result=address;
      }
      resultList.push_back(res);
      return resultList;
    } else {
      list<SingleEvalResultConstInt> empty;
      return empty;
    }
  }

  list<SingleEvalResultConstInt> EStateTransferFunctions::evalAddressOfOp(SgAddressOfOp* node,
									  SingleEvalResultConstInt operandResult,
									  EState estate, EvalMode mode) {
    SingleEvalResultConstInt res;
    res.estate=estate;
    AbstractValue operand=operandResult.result;
    SAWYER_MESG(logger[TRACE])<<"AddressOfOp: "<<node->unparseToString()<<" - operand: "<<operand.toString(_variableIdMapping)<<endl;
    if(operand.isTop()||operand.isBot()) {
      res.result=operand;
    } else {
      res.result=operand;
    }
    return listify(res);
  }

  bool EStateTransferFunctions::checkAndRecordNullPointer(AbstractValue derefOperandValue, Label label) {
    if(derefOperandValue.isTop()) {
      recordPotentialNullPointerDereferenceLocation(label);
      return true;
    } else if(derefOperandValue.isConstInt()) {
      int ptrIntVal=derefOperandValue.getIntValue();
      if(ptrIntVal==0) {
	recordDefinitiveNullPointerDereferenceLocation(label);
	return false;
      }
    }
    return true;
  }

  list<SingleEvalResultConstInt> EStateTransferFunctions::semanticEvalDereferenceOp(SingleEvalResultConstInt operandResult,
										    EState estate, EvalMode mode) {
    SingleEvalResultConstInt res;
    res.estate=estate;
    AbstractValue derefOperandValue=operandResult.result;
    SAWYER_MESG(logger[DEBUG])<<"derefOperandValue: "<<derefOperandValue.toRhsString(_variableIdMapping);

    if(mode==EStateTransferFunctions::MODE_EMPTY_STATE) {
      // arbitrary value
      res.result=CodeThorn::Top();
      return listify(res);
    }

    // null pointer check
    bool continueExec=checkAndRecordNullPointer(derefOperandValue, estate.label());
    if(continueExec) {
      switch(mode) {
      case EStateTransferFunctions::MODE_VALUE :res.result=readFromMemoryLocation(estate.label(),estate.pstate(),derefOperandValue);break;
      case EStateTransferFunctions::MODE_ADDRESS:res.result=derefOperandValue;;break;
      default:
	cerr<<"Error: EStateTransferFunctions::semanticEvalDereferenceOp: unknown evaluation mode: "<<mode<<endl;
	exit(1);
      }
      return listify(res);
    } else {
      // Alternative to above null pointer dereference recording: build
      // proper error state and check error state in solver.  once this
      // is added above null pointer recording should be adapated to use
      // the generated error state.
      // TODO: create null-pointer deref error state
      list<SingleEvalResultConstInt> empty;
      return empty;
    }
  }

  list<SingleEvalResultConstInt> EStateTransferFunctions::evalDereferenceOp(SgPointerDerefExp* node,
									    SingleEvalResultConstInt operandResult,
									    EState estate, EvalMode mode) {
    return semanticEvalDereferenceOp(operandResult,estate,mode);
  }

  list<SingleEvalResultConstInt> EStateTransferFunctions::evalPreComputationOp(EState estate, AbstractValue address, AbstractValue change) {
    SingleEvalResultConstInt res;
    AbstractValue oldValue=readFromMemoryLocation(estate.label(),estate.pstate(),address);
    AbstractValue newValue=AbstractValue::operatorAdd(oldValue,change);
    CallString cs=estate.callString;
    PState newPState=*estate.pstate();
    writeToMemoryLocation(estate.label(),&newPState,address,newValue);
    ConstraintSet cset; // use empty cset (in prep to remove it)
    ROSE_ASSERT(_analyzer);
    res.init(createEState(estate.label(),cs,newPState,cset),newValue);
    return listify(res);
  }

  list<SingleEvalResultConstInt> EStateTransferFunctions::evalPostComputationOp(EState estate, AbstractValue address, AbstractValue change) {
    // TODO change from precomp to postcomp
    SingleEvalResultConstInt res;
    AbstractValue oldValue=readFromMemoryLocation(estate.label(),estate.pstate(),address);
    AbstractValue newValue=AbstractValue::operatorAdd(oldValue,change);
    CallString cs=estate.callString;
    PState newPState=*estate.pstate();
    writeToMemoryLocation(estate.label(),&newPState,address,newValue);
    ConstraintSet cset; // use empty cset (in prep to remove it)
    ROSE_ASSERT(_analyzer);
    res.init(createEState(estate.label(),cs,newPState,cset),oldValue);
    return listify(res);
  }

  list<SingleEvalResultConstInt> EStateTransferFunctions::evalPreIncrementOp(SgPlusPlusOp* node,
									     SingleEvalResultConstInt operandResult,
									     EState estate, EvalMode mode) {
    AbstractValue address=operandResult.result;
    ROSE_ASSERT(address.isPtr()||address.isTop());
    AbstractValue change=1;
    return evalPreComputationOp(estate,address,change);
  }

  list<SingleEvalResultConstInt> EStateTransferFunctions::evalPreDecrementOp(SgMinusMinusOp* node,
									     SingleEvalResultConstInt operandResult,
									     EState estate, EvalMode mode) {
    AbstractValue address=operandResult.result;
    ROSE_ASSERT(address.isPtr()||address.isTop());
    AbstractValue change=-1;
    return evalPreComputationOp(estate,address,change);
  }

  list<SingleEvalResultConstInt> EStateTransferFunctions::evalPostIncrementOp(SgPlusPlusOp* node,
									      SingleEvalResultConstInt operandResult,
									      EState estate, EvalMode mode) {
    AbstractValue address=operandResult.result;
    ROSE_ASSERT(address.isPtr()||address.isTop());
    AbstractValue change=1;
    return evalPostComputationOp(estate,address,change);
  }


  list<SingleEvalResultConstInt> EStateTransferFunctions::evalPostDecrementOp(SgMinusMinusOp* node,
									      SingleEvalResultConstInt operandResult,
									      EState estate, EvalMode mode) {
    AbstractValue address=operandResult.result;
    ROSE_ASSERT(address.isPtr()||address.isTop());
    AbstractValue change=-1;
    return evalPostComputationOp(estate,address,change);
  }

  list<SingleEvalResultConstInt> EStateTransferFunctions::evalPlusPlusOp(SgPlusPlusOp* node,
									 SingleEvalResultConstInt operandResult,
									 EState estate, EvalMode mode) {
    SingleEvalResultConstInt res;
    res.estate=estate;
    if(SgNodeHelper::isPrefixIncDecOp(node)) {
      // preincrement ++E
      return evalPreIncrementOp(node,operandResult,estate);
    } else if(SgNodeHelper::isPostfixIncDecOp(node)) {
      // postincrement E++
      return evalPostIncrementOp(node,operandResult,estate);
    }
    throw CodeThorn::Exception("Interal error: EStateTransferFunctions::evalPlusPlusOp: "+node->unparseToString());
  }

  list<SingleEvalResultConstInt> EStateTransferFunctions::evalMinusMinusOp(SgMinusMinusOp* node,
									   SingleEvalResultConstInt operandResult,
									   EState estate, EvalMode mode) {
    SingleEvalResultConstInt res;
    res.estate=estate;
    if(SgNodeHelper::isPrefixIncDecOp(node)) {
      // predecrement --E
      return evalPreDecrementOp(node,operandResult,estate);
    } else if(SgNodeHelper::isPostfixIncDecOp(node)) {
      // postdecrement E--
      return evalPostDecrementOp(node,operandResult,estate);
    }
    throw CodeThorn::Exception("Internal error: EStateTransferFunctions::evalMinusMinusOp: "+node->unparseToString());
  }


  // for evaluating LValue Arrow, Dot,
  list<SingleEvalResultConstInt> EStateTransferFunctions::evalLValueExp(SgNode* node, EState estate, EvalMode mode) {
    ROSE_ASSERT(isSgDotExp(node)||isSgArrowExp(node));
    PState oldPState=*estate.pstate();
    SingleEvalResultConstInt res;
    res.init(estate,AbstractValue::createBot());

    SgExpression* arrExp=isSgExpression(SgNodeHelper::getLhs(node));
    SgExpression* indexExp=isSgExpression(SgNodeHelper::getRhs(node));

    list<SingleEvalResultConstInt> lhsResultList=evaluateExpression(arrExp,estate,MODE_VALUE);
    list<SingleEvalResultConstInt> rhsResultList=evaluateExpression(indexExp,estate,MODE_VALUE);
    list<SingleEvalResultConstInt> resultList;
    for(list<SingleEvalResultConstInt>::iterator riter=rhsResultList.begin();
	riter!=rhsResultList.end();
	++riter) {
      for(list<SingleEvalResultConstInt>::iterator liter=lhsResultList.begin();
	  liter!=lhsResultList.end();
	  ++liter) {
	SAWYER_MESG(logger[DEBUG])<<"lhs-val: "<<(*liter).result.toString()<<endl;
	SAWYER_MESG(logger[DEBUG])<<"rhs-val: "<<(*riter).result.toString()<<endl;
	list<SingleEvalResultConstInt> intermediateResultList;
	if(SgDotExp* dotExp=isSgDotExp(node)) {
	  intermediateResultList=evalDotOp(dotExp,*liter,*riter,estate,MODE_ADDRESS);
	} else if(SgArrowExp* arrowExp=isSgArrowExp(node)) {
	  intermediateResultList=evalArrowOp(arrowExp,*liter,*riter,estate,MODE_ADDRESS);
	} else {
	  cerr<<"Internal error: EStateTransferFunctions::evalLValueExp: wrong oeprator node type: "<<node->class_name()<<endl;
	  exit(1);
	}
	// move elements from intermediateResultList to resultList
	resultList.splice(resultList.end(), intermediateResultList);
      }
    }
    return resultList;
  }

  list<SingleEvalResultConstInt> EStateTransferFunctions::evalLValuePntrArrRefExp(SgPntrArrRefExp* node, EState estate, EvalMode mode) {
    // for now we ignore array refs on lhs
    // TODO: assignments in index computations of ignored array ref
    // see ExprAnalyzer.C: case V_SgPntrArrRefExp:
    // since nothing can change (because of being ignored) state remains the same
    SAWYER_MESG(logger[DEBUG])<<"evalLValuePntrArrRefExp"<<endl;
    PState oldPState=*estate.pstate();
    SingleEvalResultConstInt res;
    res.init(estate,AbstractValue::createBot());
    if(getSkipArrayAccesses()) {
      res.result=CodeThorn::Top();
      return listify(res);
    } else {
      SgExpression* arrExp=isSgExpression(SgNodeHelper::getLhs(node));
      SgExpression* indexExp=isSgExpression(SgNodeHelper::getRhs(node));

      list<SingleEvalResultConstInt> lhsResultList=evaluateExpression(arrExp,estate,MODE_VALUE);
      list<SingleEvalResultConstInt> rhsResultList=evaluateExpression(indexExp,estate,MODE_VALUE);
      list<SingleEvalResultConstInt> resultList;
      for(list<SingleEvalResultConstInt>::iterator riter=rhsResultList.begin();
	  riter!=rhsResultList.end();
	  ++riter) {
	for(list<SingleEvalResultConstInt>::iterator liter=lhsResultList.begin();
	    liter!=lhsResultList.end();
	    ++liter) {
	  //cout<<"DEBUG: lhs-val: "<<(*liter).result.toString()<<endl;
	  //cout<<"DEBUG: rhs-val: "<<(*riter).result.toString()<<endl;
	  list<SingleEvalResultConstInt> intermediateResultList=evalArrayReferenceOp(node,*liter,*riter,estate,MODE_ADDRESS);
	  // move elements from intermediateResultList to resultList
	  resultList.splice(resultList.end(), intermediateResultList);
	}
      }
      return resultList;  
    }
    // unreachable
    ROSE_ASSERT(false);
  }

  list<SingleEvalResultConstInt> EStateTransferFunctions::evalLValueVarRefExp(SgVarRefExp* node, EState estate, EvalMode mode) {
    SAWYER_MESG(logger[TRACE])<<"DEBUG: evalLValueVarRefExp: "<<node->unparseToString()<<" label:"<<estate.label().toString()<<endl;
    SingleEvalResultConstInt res;
    res.init(estate,AbstractValue::createBot());
    const PState* pstate=estate.pstate();
    VariableId varId=_variableIdMapping->variableId(node);
    if(isMemberVariable(varId)) {
      ROSE_ASSERT(_variableIdMapping);
      int offset=AbstractValue::getVariableIdMapping()->getOffset(varId);
      SAWYER_MESG(logger[TRACE])<<"DEBUG: evalLValueVarRefExp found STRUCT member: "<<_variableIdMapping->variableName(varId)<<" offset: "<<offset<<endl;
      //res.result=AbstractValue(offset);
      //return listify(res);
    }
    if(pstate->varExists(varId)) {
      if(_variableIdMapping->isOfArrayType(varId)) {
	SAWYER_MESG(logger[TRACE])<<"DEBUG: lvalue array address(?): "<<node->unparseToString()<<"EState label:"<<estate.label().toString()<<endl;
	res.result=AbstractValue::createAddressOfArray(varId);
      } else {
	res.result=AbstractValue::createAddressOfVariable(varId);
      }
      return listify(res);
    } else {
      // special mode to represent information not stored in the state
      // i) unmodified arrays: data can be stored outside the state
      // ii) undefined variables mapped to 'top' (abstraction by removing variables from state)
      if(_variableIdMapping->isOfArrayType(varId) && _analyzer->getOptionsRef().arraysNotInState==true) {
	// variable is used on the rhs and it has array type implies it avalates to a pointer to that array
	//res.result=AbstractValue(varId.getIdCode());
	SAWYER_MESG(logger[TRACE])<<"DEBUG: lvalue array address (non-existing in state)(?): "<<node->unparseToString()<<endl;
	res.result=AbstractValue::createAddressOfArray(varId);
	return listify(res);
      } else {
	Label lab=estate.label();
	res.result=CodeThorn::Top();
	SAWYER_MESG(logger[WARN]) << "at label "<<lab<<": "<<(_analyzer->getLabeler()->getNode(lab)->unparseToString())<<": variable not in PState (LValue VarRefExp) (var="<<_variableIdMapping->uniqueVariableName(varId)<<"). Initialized with top."<<endl;
	//cerr << "WARNING: estate: "<<estate.toString(_variableIdMapping)<<endl;
	return listify(res);
      }
    }
    // unreachable
  }

  list<SingleEvalResultConstInt> EStateTransferFunctions::evalLValuePointerDerefExp(SgPointerDerefExp* node, EState estate) {
    SAWYER_MESG(logger[TRACE])<<"DEBUG: evalLValuePtrDerefExp: "<<node->unparseToString()<<" label:"<<estate.label().toString()<<endl;
    // abstract_value(*p) = abstract_eval(p) : the value of 'p' is an abstract address stored in p, which is the lvalue of *p
    SgExpression* operand=node->get_operand_i();
    list<SingleEvalResultConstInt> operandResultList=evaluateExpression(operand,estate,EStateTransferFunctions::MODE_VALUE);
    return operandResultList;
  }


  std::list<SingleEvalResultConstInt> EStateTransferFunctions::evalFunctionRefExp(SgFunctionRefExp* node, EState estate, EvalMode mode) {
    //cout<<"DEBUG: evalFunctionRefExp:"<<node->unparseToString()<<" : "<<AstTerm::astTermWithNullValuesToString(node)<<endl;
    if(mode==EStateTransferFunctions::MODE_EMPTY_STATE) {
      SingleEvalResultConstInt res;
      res.init(estate,AbstractValue::createTop());
      return listify(res);
    }  
    // create address of function
    SingleEvalResultConstInt res;
    ROSE_ASSERT(_analyzer);

    SgFunctionSymbol* functionSym=node->get_symbol_i();
    ROSE_ASSERT(functionSym);
    SgFunctionDeclaration* funDecl=functionSym->get_declaration();
    //SgFunctionDeclaration* funDecl=SgNodeHelper::findFunctionDeclarationWithFunctionSymbol(functionSym);
    SgDeclarationStatement* defFunDecl=funDecl->get_definingDeclaration();
    if(defFunDecl) {
      // if not resolved, funDef will be 0, and functionEntryLabel will be an invalid label id.
      funDecl=isSgFunctionDeclaration(defFunDecl);
    }
    ROSE_ASSERT(funDecl);
    //cout<<"DEBUG: isForwardDecl:"<<SgNodeHelper::isForwardFunctionDeclaration(funDecl)<<endl;
    //cout<<"DEBUG: fundecl:"<<funDecl->unparseToString()<<endl;
    SgFunctionDefinition* funDef=funDecl->get_definition();
    if(funDef) {
      Label funLab=_analyzer->getLabeler()->functionEntryLabel(funDef);
  
      // label of corresponding entry label of function of node; if function is external, then label is an invalid label.
      //cout<<"DEBUG: evalFunctionRefExp: label:"<<funLab.toString()<<endl;
      res.init(estate,AbstractValue::createAddressOfFunction(funLab));
      return listify(res);
    } else {
      SAWYER_MESG(logger[WARN])<<"EStateTransferFunctions::evalFunctionRefExp: funRefExp==0 (function pointer? - creating top)"<<endl;
      res.init(estate,AbstractValue::createTop());
      return listify(res);
    }
  }
  list<SingleEvalResultConstInt> EStateTransferFunctions::evalRValueVarRefExp(SgVarRefExp* node, EState estate, EvalMode mode) {
    if(mode==EStateTransferFunctions::MODE_EMPTY_STATE) {
      SingleEvalResultConstInt res;
      res.init(estate,AbstractValue::createTop());
      return listify(res);
    }  
    SAWYER_MESG(logger[TRACE])<<"evalRValueVarRefExp: "<<node->unparseToString()<<" id:"<<_variableIdMapping->variableId(isSgVarRefExp(node)).toString()<<" MODE:"<<mode<<" Symbol:"<<node->get_symbol()<<endl;
    SingleEvalResultConstInt res;
    res.init(estate,AbstractValue::createBot());

    const PState* pstate=estate.pstate();
    VariableId varId=_variableIdMapping->variableId(node);
    if(!varId.isValid()) {
      SAWYER_MESG(logger[ERROR])<<"Invalid varId: "<<node->unparseToString()<<" Parent: "<<node->get_parent()->unparseToString()<<" Symbol:"<<node->get_symbol()<<endl;
      ROSE_ASSERT(varId.isValid());
    }
    // check if var is a struct member. if yes return struct-offset.
    //SAWYER_MESG(logger[TRACE])<<"VarRefExp name: "<<_variableIdMapping->variableName(varId)<<endl;
    if(isMemberVariable(varId)) {
      int offset=AbstractValue::getVariableIdMapping()->getOffset(varId);
      ROSE_ASSERT(_variableIdMapping);
      SAWYER_MESG(logger[TRACE])<<"evalRValueVarRefExp STRUCT member: "<<_variableIdMapping->variableName(varId)<<" offset: "<<offset<<endl;
      res.result=AbstractValue(offset); // TODO BYTEMODE ?
      //cout<<"DEBUG1 STRUCT MEMBER: "<<_variableIdMapping->variableName(varId)<<" offset: "<<offset<<endl;
      return listify(res);
    }
    // TODO: as rvalue it represents the entire class
    if(_variableIdMapping->isOfClassType(varId)) {
      res.result=AbstractValue::createAddressOfVariable(varId);
      return listify(res);
    }
    if(pstate->varExists(varId)) {
      if(_variableIdMapping->isOfArrayType(varId)) {
	res.result=AbstractValue::createAddressOfArray(varId);
      } else {
	if(_variableIdMapping->isOfReferenceType(varId)) {
	  res.result=readFromReferenceMemoryLocation(estate.label(),pstate,varId);
	} else {
	  res.result=readFromMemoryLocation(estate.label(),pstate,varId);
	}
      }
      return listify(res);
    } else {
      // special mode to represent information not stored in the state
      // i) unmodified arrays: data can be stored outside the state
      // ii) undefined variables mapped to 'top' (abstraction by removing variables from state)
      if(_variableIdMapping->isOfArrayType(varId) && _analyzer->getOptionsRef().arraysNotInState==true) {
	// variable is used on the rhs and it has array type implies it avalates to a pointer to that array
	//res.result=AbstractValue(varId.getIdCode());
	res.result=AbstractValue::createAddressOfArray(varId);
	return listify(res);
      } else {
	res.result=CodeThorn::Top();
	//cerr << "WARNING: variable not in PState (var="<<_variableIdMapping->uniqueVariableName(varId)<<"). Initialized with top."<<endl;
	Label lab=estate.label();
	SAWYER_MESG(logger[WARN]) << "at label "<<lab<<": "<<(_analyzer->getLabeler()->getNode(lab)->unparseToString())<<": variable not in PState (RValue VarRefExp) (var="<<_variableIdMapping->uniqueVariableName(varId)<<"). Initialized with top."<<endl;

	return listify(res);
      }
    }
    // unreachable
    ROSE_ASSERT(false);
  }

  list<SingleEvalResultConstInt> EStateTransferFunctions::evalValueExp(SgValueExp* node, EState estate, EvalMode mode) {
    ROSE_ASSERT(node);
    SingleEvalResultConstInt res;
    res.init(estate,AbstractValue::createBot());
    res.result=abstractValueFromSgValueExp(node,mode);
    return listify(res);
  }

  list<SingleEvalResultConstInt> EStateTransferFunctions::evalFunctionCallArguments(SgFunctionCallExp* funCall, EState estate) {
    SgExpressionPtrList& argsList=SgNodeHelper::getFunctionCallActualParameterList(funCall);
    for (auto arg : argsList) {
      SAWYER_MESG(logger[TRACE])<<"evaluating function call argument: "<<arg->unparseToString()<<endl;
      // Requirement: code is normalized, does not contain state modifying operations in function arguments
      list<SingleEvalResultConstInt> resList=evaluateExpression(arg,estate);
    }
    SingleEvalResultConstInt res;
    AbstractValue evalResultValue=CodeThorn::Top();
    res.init(estate,evalResultValue);

    return listify(res);
  }

  list<SingleEvalResultConstInt> EStateTransferFunctions::evalFunctionCall(SgFunctionCallExp* funCall, EState estate) {
    SingleEvalResultConstInt res;
    res.init(estate,AbstractValue::createTop());
    SAWYER_MESG(logger[TRACE])<<"Evaluating function call: "<<funCall->unparseToString()<<endl;
    SAWYER_MESG(logger[TRACE])<<"AST function call: "<<AstTerm::astTermWithNullValuesToString(funCall)<<endl;
    if(getStdFunctionSemantics()) {
      string funName=SgNodeHelper::getFunctionName(funCall);
      if(funName=="malloc") {
	return evalFunctionCallMalloc(funCall,estate);
      } else if(funName=="free") {
	return evalFunctionCallFree(funCall,estate);
      } else if(funName=="strlen") {
	return PredefinedSemanticFunctions::evalFunctionCallStrLen(this,funCall,estate);
      } else if(funName=="memcpy") {
	return PredefinedSemanticFunctions::evalFunctionCallMemCpy(this,funCall,estate);
      } else if(funName=="fflush") {
	// ignoring fflush
	// res initialized above
	return evalFunctionCallArguments(funCall,estate);
      } else if(funName=="time"||funName=="srand"||funName=="rand") {
	// arguments must already be analyzed (normalized code) : TODO check that it is a single variable
	// result is top (time/srand/rand return any value)
	return listify(res); // return top (initialized above (res.init))
      } else if(funName=="__assert_fail") {
	// TODO: create state
	evalFunctionCallArguments(funCall,estate);
	estate.io.recordVerificationError();
	return listify(res);
      } else if(funName=="printf" && (getInterpreterMode()==IM_ENABLED)) {
	// call fprint function in mode CONCRETE and generate output
	// (1) obtain arguments from estate
	// (2) marshall arguments
	// (3) perform function call (causing side effect on stdout (or written to provided file))
	return execFunctionCallPrintf(funCall,estate);
      } else if(funName=="scanf" && (getInterpreterMode()==IM_ENABLED)) {
	// call scanf function in mode CONCRETE and generate output
	// (1) obtain arguments from estate
	// (2) marshall arguments
	// (3) perform function call (causing side effect on stdin)
	return execFunctionCallScanf(funCall,estate);
      }
    }
    if(getSkipUnknownFunctionCalls()) {
      return evalFunctionCallArguments(funCall,estate);
    } else {
      string s=funCall->unparseToString();
      throw CodeThorn::Exception("unknown semantics of function call inside expression: "+s);
    }
  }

  list<SingleEvalResultConstInt> EStateTransferFunctions::execFunctionCallPrintf(SgFunctionCallExp* funCall, EState estate) {
    //cout<<"DEBUG: EStateTransferFunctions::execFunctionCallPrintf"<<endl;
    SingleEvalResultConstInt res;
    res.init(estate,AbstractValue::createTop()); // default value for void function call
    ROSE_ASSERT(_variableIdMapping);
    SgExpressionPtrList& argsList=SgNodeHelper::getFunctionCallActualParameterList(funCall);
    auto iter=argsList.begin();
    ROSE_ASSERT(iter!=argsList.end());
    SgNode* arg=*iter++;
    while(SgCastExp* castExp=isSgCastExp(arg)) {
      arg=castExp->get_operand_i();
    }
    if(SgStringVal* formatStringVal=isSgStringVal(arg)) {
      ROSE_ASSERT(formatStringVal);
      string formatString=formatStringVal->get_value();
      vector<string> avStringVector;
      for(size_t i=1;i<argsList.size();i++) {
	SgExpression* arg=*iter++;
	list<SingleEvalResultConstInt> argResList=evaluateExpression(arg,estate);
	if(argResList.size()!=1) {
	  cerr<<"Error: conditional control-flow in printf argument not supported. Expression normalization required."<<endl;
	  exit(1);
	} else {
	  AbstractValue av=(*argResList.begin()).value();
	  avStringVector.push_back(av.toString(_variableIdMapping));
	}
      }
      // replace all uses of %? with respective AVString
      string concAVString;
      size_t j=0;
      for(size_t i=0;i<formatString.size();++i) {
	if(formatString[i]=='%') {
	  i++; // skip next character
	  if(j>=avStringVector.size()) {
	    // number of arguments and uses of '%' don't match in input
	    // program. This could be reported as program error.  For now
	    // we just do not produce an output (as the original program
	    // does not either)
	    // TODO: report input program error
	    continue; // continue to print other characters
	  }
	  concAVString+=avStringVector[j++];
	} else if(formatString[i]=='\\') {
	  if(i+1<=formatString.size()-1 && formatString[i+1]=='%') {
	    i+=1; // process additional character '%'
	    concAVString+="\%";
	  } else if(i+1<=formatString.size()-1 && formatString[i+1]=='n') {
	    concAVString+='\n'; // this generates a proper newline
	    i+=1; // process additional character 'n'
	  } else {
	    concAVString+='\\';
	  }
	} else {
	  // any other character
	  concAVString+=formatString[i];
	}
      }
      string fileName=getInterpreterModeFileName();
      if(fileName!="") {
	bool ok=CppStdUtilities::appendFile(fileName,concAVString);
	if(!ok) {
	  cerr<<"Error: could not open output file "<<fileName<<endl;
	  exit(1);
	}
      } else {
	cout<<concAVString;
      }
    } else {
      // first argument is not a string, must be some constant
      // since it is only printed, it is not modifying the state
      // TODO: it could be recorded as output state
    
      cout<<"WARNING: unsupported printf argument: "<<(arg)->class_name()<<" : "<<(arg)->unparseToString()<<endl;
    }
    return listify(res);
  }

  list<SingleEvalResultConstInt> EStateTransferFunctions::execFunctionCallScanf(SgFunctionCallExp* funCall, EState estate) {
    //cout<<"DEBUG: EStateTransferFunctions::execFunctionCallScanf"<<endl;
    SingleEvalResultConstInt res;
    res.init(estate,AbstractValue::createTop()); // default value for void function call
    ROSE_ASSERT(_variableIdMapping);
    SgExpressionPtrList& argsList=SgNodeHelper::getFunctionCallActualParameterList(funCall);
    auto iter=argsList.begin();
    ROSE_ASSERT(iter!=argsList.end());
    SgStringVal* formatStringVal=isSgStringVal(*iter++);
    ROSE_ASSERT(formatStringVal);
    string formatString=formatStringVal->get_value();
    vector<string> avStringVector;
    for(size_t i=1;i<argsList.size();i++) {
      SgExpression* arg=*iter++;
      list<SingleEvalResultConstInt> argResList=evaluateExpression(arg,estate);
      if(argResList.size()!=1) {
	cerr<<"Error: conditional control-flow in printf argument not supported. Expression normalization required."<<endl;
        exit(1);
      } else {
	AbstractValue av=(*argResList.begin()).value();
	avStringVector.push_back(av.toString(_variableIdMapping));
      }
    }
    // replace all uses of %? with respective AVString
    string concAVString;
    size_t j=0;
    for(size_t i=0;i<formatString.size();++i) {
      if(formatString[i]=='%') {
	char controlChar=formatString[i];
	i++; // skip next character
	if(j>=avStringVector.size()) {
	  // number of arguments and uses of '%' don't match in input
	  // program. This could be reported as program error.  For now
	  // we just do not produce an output (as the original program
	  // does not either)
	  // TODO: report input program error
	  continue; // continue to print other characters
	}
	switch(controlChar) {
	case 'd': {
	  int val;
	  int numParsed=scanf("%d",&val); // read integer from stdin
	  AbstractValue av=((numParsed==1)?argsList[j]:AbstractValue::createTop()); // assume any value in case of error
	  // write val into state at address argsList[j]
	  if(av.isPtr()) {
	    PState pstate=*estate.pstate();
	    writeToMemoryLocation(estate.label(),&pstate,av,val);
	    // TODO: pstate is not used yet, because estate is only read but not returned (hence this is a noop and not an update)
	    cout<<"Warning: interpreter mode: scanf: memory location "<<av.toString(_variableIdMapping)<<" not updated (not implemented yet)."<<endl;
	  } else {
	    cerr<<"Warning: interpreter mode: scanf writing to non-address value (ignored)"<<endl;
	  }
	  break;
	}
	default:
	  cerr<<"Warning: interpreter mode: scanf using unknown type "<<endl;
	}
      } else if(formatString[i]=='\\') {
	if(i+1<=formatString.size()-1 && formatString[i+1]=='%') {
	  i+=1; // process additional character '%'
	  concAVString+="\%";
	} else if(i+1<=formatString.size()-1 && formatString[i+1]=='n') {
	  i+=1; // process additional character 'n'
	} else {
	  // nothing to do (do not match)
	}
      } else {
	// any other character
      }
    }
    return listify(res);
  }

  list<SingleEvalResultConstInt> EStateTransferFunctions::evalFunctionCallMalloc(SgFunctionCallExp* funCall, EState estate) {
    // create two cases: (i) allocation successful, (ii) allocation fails (null pointer is returned, and no memory is allocated).
    SingleEvalResultConstInt res;
    static int memorylocid=0; // to be integrated in VariableIdMapping
    memorylocid++;
    stringstream ss;
    ss<<"$MEM"<<memorylocid;
    ROSE_ASSERT(_variableIdMapping);
    SgExpressionPtrList& argsList=SgNodeHelper::getFunctionCallActualParameterList(funCall);
    if(argsList.size()==1) {
      // (i) create state for successful allocation of memory (do not reserve memory yet, only pointer is reserved and size of memory is recorded)
      // memory is allocated when written to it. Otherwise it is assumed to be uninitialized
      SgExpression* arg1=*argsList.begin();
      list<SingleEvalResultConstInt> resList=evaluateExpression(arg1,estate);
      if(resList.size()!=1) {
	cerr<<"Error: conditional control-flow in function argument expression not supported. Expression normalization required."<<endl;
	exit(1);
      }
      SingleEvalResultConstInt sres=*resList.begin();
      AbstractValue arg1val=sres.result;
      VariableId memLocVarId;
      int memoryRegionSize;
      if(arg1val.isConstInt()) {
	memoryRegionSize=arg1val.getIntValue();
      } else {
	// unknown size
	memoryRegionSize=0;
      }
      list<SingleEvalResultConstInt> resList2;
      memLocVarId=_variableIdMapping->createAndRegisterNewMemoryRegion(ss.str(),memoryRegionSize);
      AbstractValue allocatedMemoryPtr=AbstractValue::createAddressOfArray(memLocVarId);
      logger[TRACE]<<"function call malloc: allocated at: "<<allocatedMemoryPtr.toString()<<endl;
      res.init(estate,allocatedMemoryPtr);
      //cout<<"DEBUG: evaluating function call malloc:"<<funCall->unparseToString()<<endl;
      ROSE_ASSERT(allocatedMemoryPtr.isPtr());
      //cout<<"Generated malloc-allocated mem-chunk pointer is OK."<<endl;
      // create resList with two states now
      resList2.push_back(res);
#if 0
      // (ii) add memory allocation case: null pointer (allocation failed)
      SingleEvalResultConstInt resNullPtr;
      AbstractValue nullPtr=AbstractValue::createNullPtr();
      resNullPtr.init(estate,nullPtr);
      resList2.push_back(resNullPtr);
#endif
#if 0
      // TODO: update of state (not allowed here)
      // generate representation of allocated memory (we only have the address so far)
      if(memoryRegionSize>0 && memoryRegionSize<=_analyzer->getOptionsRef().maxExactMemorySizeRepresentation) {
	// reserve memory
	for(int i=0;i<memoryRegionSize;++i) {
	  AbstractValue arrayElemAddr=AbstractValue::createAddressOfArrayElement(memLocVarId,AbstractValue::createIntegerValue(CodeThorn::BITYPE_UINT,i));
	  AbstractValue undefVal=AbstractValue::createUndefined();
	  PState* pstate=estate.pstate();
	  pstate->writeToMemoryLocation(arrayElemAddr,undefVal);
	}
      }
#endif
      return resList2;
    } else {
      // this will become an error in future
      cerr<<"WARNING: unknown malloc function "<<funCall->unparseToString()<<endl;
    }
    return listify(res);
  }

  list<SingleEvalResultConstInt> EStateTransferFunctions::evalFunctionCallFree(SgFunctionCallExp* funCall, EState estate) {
    SingleEvalResultConstInt res;
    SgExpressionPtrList& argsList=SgNodeHelper::getFunctionCallActualParameterList(funCall);
    if(argsList.size()==1) {
      SgExpression* arg1=*argsList.begin();
      list<SingleEvalResultConstInt> resList=evaluateExpression(arg1,estate);
      if(resList.size()!=1) {
	cerr<<"Error: conditional control-flow in function argument expression not supported. Expression normalization required."<<endl;
	exit(1);
      }
      SingleEvalResultConstInt sres=*resList.begin();
      AbstractValue arg1val=sres.result;
      if(arg1val.isPtr()) {
	int memoryRegionSize=getMemoryRegionNumElements(arg1val);
	// can be marked as deallocated (currently this does not impact the analysis)
	//variableIdMapping->setSize(arg1Val.getVariableId(),-1);
	// top maps to -1
	ROSE_ASSERT(memoryRegionSize>=-1);
      }
      res.init(estate,AbstractValue::createTop()); // void result (using top here)
    } else {
      // this will become an error in future
      cerr<<"WARNING: unknown free function "<<funCall->unparseToString()<<endl;
    }
    return listify(res);
  }

  // returns size, or -1 (=any) in case pointer is top.
  int EStateTransferFunctions::getMemoryRegionNumElements(CodeThorn::AbstractValue ptrToRegion) {
    if(ptrToRegion.isTop()) {
      return -1;
    }
    ROSE_ASSERT(ptrToRegion.isPtr());
    VariableId ptrVariableId=ptrToRegion.getVariableId();
    int size=_variableIdMapping->getNumberOfElements(ptrVariableId);
    SAWYER_MESG(logger[TRACE])<<"getMemoryRegionNumElements(ptrToRegion): ptrToRegion with ptrVariableId:"<<ptrVariableId<<" "<<_variableIdMapping->variableName(ptrVariableId)<<" numberOfElements: "<<size<<endl;
    return size;
  }

  int EStateTransferFunctions::getMemoryRegionElementSize(CodeThorn::AbstractValue ptrToRegion) {
    if(ptrToRegion.isTop()) {
      return -1;
    }
    ROSE_ASSERT(ptrToRegion.isPtr());
    VariableId ptrVariableId=ptrToRegion.getVariableId();
    int size=_variableIdMapping->getElementSize(ptrVariableId);
    SAWYER_MESG(logger[TRACE])<<"getMemoryRegionNumElements(ptrToRegion): ptrToRegion with ptrVariableId:"<<ptrVariableId<<" "<<_variableIdMapping->variableName(ptrVariableId)<<" numberOfElements: "<<size<<endl;
    return size;
  }


  // true if access is correct. false if out-of-bounds access.
  // TODO: rewrite using new abstract values with array address references
  bool EStateTransferFunctions::accessIsWithinArrayBounds(VariableId arrayVarId,int accessIndex) {
    // check array bounds
    int arraySize=_variableIdMapping->getNumberOfElements(arrayVarId);
    return !(accessIndex<0||accessIndex>=arraySize);
  }

  enum MemoryAccessBounds EStateTransferFunctions::checkMemoryAccessBounds(AbstractValue address) {
    if(address.isTop()) {
      return ACCESS_POTENTIALLY_OUTSIDE_BOUNDS;
    } if(address.isBot()) {
      return ACCESS_NON_EXISTING;
    } if(address.isNullPtr()) {
      return ACCESS_DEFINITELY_NP;
    } else {
      AbstractValue offset=address.getIndexValue();
      if(offset.isTop()) {
	return ACCESS_POTENTIALLY_OUTSIDE_BOUNDS;
      } else if(offset.isBot()) {
	return ACCESS_NON_EXISTING;
      } else {
	VariableId memId=address.getVariableId();
	// this must be the only remaining case
	if(offset.isConstInt()) {
	  // check array bounds
	  int memRegionSize=_variableIdMapping->getNumberOfElements(memId);
	  if(memRegionSize==0)
	    return ACCESS_POTENTIALLY_OUTSIDE_BOUNDS; // will become ACCESS_DEFINITELY_OUTSIDE_BOUNDS;
	  int accessIndex=offset.getIntValue();
	  if(!(accessIndex<0||accessIndex>=memRegionSize)) {
	    return ACCESS_DEFINITELY_INSIDE_BOUNDS;
	  } else {
	    return ACCESS_DEFINITELY_OUTSIDE_BOUNDS;
	  }
	} else {
	  return ACCESS_POTENTIALLY_OUTSIDE_BOUNDS;
	}
      }
    }
  }    

  ProgramLocationsReport EStateTransferFunctions::getProgramLocationsReport(enum AnalysisSelector analysisSelector) {
    ProgramLocationsReport report;
#pragma omp critical(VIOLATIONRECORDING)
    report=_violatingLocations.at(analysisSelector);
    return report;
  }

  // deprecated
  ProgramLocationsReport EStateTransferFunctions::getViolatingLocations(enum AnalysisSelector analysisSelector) {
    return getProgramLocationsReport(analysisSelector);
  }

  void EStateTransferFunctions::initViolatingLocations() {
    for(int i=ANALYSIS_NULL_POINTER;i<ANALYSIS_NUM;i++) {
      _violatingLocations.push_back(ProgramLocationsReport());
    }
  }
  std::string EStateTransferFunctions::analysisSelectorToString(AnalysisSelector sel) {
    switch(sel) {
    case ANALYSIS_NULL_POINTER: return "null-pointer";
    case ANALYSIS_OUT_OF_BOUNDS: return "out-of-bounds";
    case ANALYSIS_UNINITIALIZED: return "unitialized";
    case ANALYSIS_DEAD_CODE: return "dead-code";
    case ANALYSIS_OPAQUE_PREDICATE: return "opaque-predicate";
    default:
      SAWYER_MESG(logger[FATAL])<<"EStateTransferFunctions::analysisSelectorToString: unknown selector."<<endl;
      exit(1);
    }
  }

  std::string EStateTransferFunctions::sourceLocationAndNodeToString(Label lab) {
    SgNode* node=_analyzer->getLabeler()->getNode(lab);
    return SgNodeHelper::sourceLocationAndNodeToString(node);
  }

  void EStateTransferFunctions::recordDefinitiveViolatingLocation(enum AnalysisSelector analysisSelector, Label label) {
#pragma omp critical(VIOLATIONRECORDING)
    {
      _violatingLocations.at(analysisSelector).recordDefinitiveLocation(label);
      if(_printDetectedViolations) {
	cout<<analysisSelectorToString(analysisSelector)
	    <<": definitive violation at L"<<label.toString()<<":"
	    <<sourceLocationAndNodeToString(label)
	    <<endl;
      }
    }
  }

  void EStateTransferFunctions::recordPotentialViolatingLocation(enum AnalysisSelector analysisSelector, Label label) {
#pragma omp critical(VIOLATIONRECORDING)
    {
      _violatingLocations.at(analysisSelector).recordPotentialLocation(label);
      if(_printDetectedViolations)
	cout<<analysisSelectorToString(analysisSelector)<<": potential violation at label "<<label.toString()<<endl;
    }
  }

  void EStateTransferFunctions::recordDefinitiveNullPointerDereferenceLocation(Label label) {
    recordDefinitiveViolatingLocation(ANALYSIS_NULL_POINTER,label);
  }

  void EStateTransferFunctions::recordPotentialNullPointerDereferenceLocation(Label label) {
    recordPotentialViolatingLocation(ANALYSIS_NULL_POINTER,label);
  }

  void EStateTransferFunctions::recordDefinitiveOutOfBoundsAccessLocation(Label label) {
    recordDefinitiveViolatingLocation(ANALYSIS_OUT_OF_BOUNDS,label);
    if(_printDetectedViolations)
      cout<<"Violation detected: definitive out of bounds access at label "<<label.toString()<<endl;
  }

  void EStateTransferFunctions::recordPotentialOutOfBoundsAccessLocation(Label label) {
    recordPotentialViolatingLocation(ANALYSIS_OUT_OF_BOUNDS,label);
    if(_printDetectedViolations)
      cout<<"Violation detected: potential out of bounds access at label "<<label.toString()<<endl;
  }

  void EStateTransferFunctions::recordDefinitiveUninitializedAccessLocation(Label label) {
    recordDefinitiveViolatingLocation(ANALYSIS_UNINITIALIZED,label);
    if(_printDetectedViolations)
      cout<<"Violation detected: definitive uninitialized value read at label "<<label.toString()<<endl;
  }

  void EStateTransferFunctions::recordPotentialUninitializedAccessLocation(Label label) {
    recordPotentialViolatingLocation(ANALYSIS_UNINITIALIZED,label);
    if(_printDetectedViolations)
      cout<<"Violation detected: potential uninitialized value read at label "<<label.toString()<<endl;
  }

  bool EStateTransferFunctions::getPrintDetectedViolations() {
    return _printDetectedViolations;
  }
  void EStateTransferFunctions::setPrintDetectedViolations(bool flag) {
    _printDetectedViolations=flag;
  }

  bool EStateTransferFunctions::isMemberVariable(CodeThorn::VariableId varId) {
    return AbstractValue::getVariableIdMapping()->isMemberVariable(varId);
  }

  bool EStateTransferFunctions::definitiveErrorDetected() {
    for(int i=ANALYSIS_NULL_POINTER;i<ANALYSIS_NUM;i++) {
      if(_violatingLocations.at(i).numDefinitiveLocations()>0)
	return true;
    }
    return false;
  }

  bool EStateTransferFunctions::potentialErrorDetected() {
    for(int i=ANALYSIS_NULL_POINTER;i<ANALYSIS_NUM;i++) {
      if(_violatingLocations.at(i).numPotentialLocations()>0)
	return true;
    }
    return false;
  }

  AbstractValue EStateTransferFunctions::readFromMemoryLocation(Label lab, const PState* pstate, AbstractValue memLoc) {
    // inspect memory location here
    if(memLoc.isNullPtr()) {
      recordDefinitiveNullPointerDereferenceLocation(lab);
      //return AbstractValue::createBot();
      return AbstractValue::createTop();
    }
    if(memLoc.isTop()) {
      recordPotentialNullPointerDereferenceLocation(lab);
      recordPotentialOutOfBoundsAccessLocation(lab);
      recordPotentialViolatingLocation(ANALYSIS_UNINITIALIZED,lab);
    } else if(!pstate->memLocExists(memLoc)) {
      //recordPotentialOutOfBoundsAccessLocation(lab);
      recordPotentialUninitializedAccessLocation(lab);
    }
    AbstractValue val=pstate->readFromMemoryLocation(memLoc);
    // investigate value here
    if(val.isUndefined()) {
      recordPotentialUninitializedAccessLocation(lab);
    }
    if(_readWriteListener) {
      _readWriteListener->readingFromMemoryLocation(lab,pstate,memLoc,val);
    }

    return val;
  }

  void EStateTransferFunctions::writeToMemoryLocation(Label lab, PState* pstate, AbstractValue memLoc, AbstractValue newValue) {
    // inspect everything here
    SAWYER_MESG(logger[TRACE])<<"EStateTransferFunctions::writeToMemoryLocation1:started"<<endl;
    SAWYER_MESG(logger[TRACE])<<"EStateTransferFunctions::writeToMemoryLocation1:"<<memLoc.toString()<<endl;
    SAWYER_MESG(logger[TRACE])<<"EStateTransferFunctions::writeToMemoryLocation1:cont"<<endl;
    if(memLoc.isTop()) {
      SAWYER_MESG(logger[WARN])<<"writing to arbitrary memloc: "<<lab.toString()<<":"<<memLoc.toString(_variableIdMapping)<<":="<<newValue.toString(_variableIdMapping)<<endl;
      recordPotentialOutOfBoundsAccessLocation(lab);
    } else if(!pstate->memLocExists(memLoc)) {
      SAWYER_MESG(logger[TRACE])<<"EStateTransferFunctions::writeToMemoryLocation1: memloc does not exist"<<endl;
      if(!newValue.isUndefined()) {
	recordPotentialOutOfBoundsAccessLocation(lab);
	SAWYER_MESG(logger[WARN])<<"writing defined value to memloc not in state: "<<lab.toString()<<":"<<memLoc.toString(_variableIdMapping)<<":="<<newValue.toString(_variableIdMapping)<<endl;
      }
    }
    SAWYER_MESG(logger[TRACE])<<"EStateTransferFunctions::writeToMemoryLocation1: before write"<<endl;
    if(_readWriteListener) {
      _readWriteListener->writingToMemoryLocation(lab,pstate,memLoc,newValue);
    }
    pstate->writeToMemoryLocation(memLoc,newValue);
    SAWYER_MESG(logger[TRACE])<<"EStateTransferFunctions::writeToMemoryLocation1:done"<<endl;
  }

  AbstractValue EStateTransferFunctions::readFromReferenceMemoryLocation(Label lab, const PState* pstate, AbstractValue memLoc) {
    AbstractValue referedMemLoc=readFromMemoryLocation(lab,pstate,memLoc);
    return readFromMemoryLocation(lab,pstate,referedMemLoc);
  }

  void EStateTransferFunctions::writeToReferenceMemoryLocation(Label lab, PState* pstate, AbstractValue memLoc, AbstractValue newValue) {
    AbstractValue referedMemLoc=readFromMemoryLocation(lab,pstate,memLoc);
    writeToMemoryLocation(lab,pstate,referedMemLoc,newValue);
  }

  void EStateTransferFunctions::initializeMemoryLocation(Label lab, PState* pstate, AbstractValue memLoc, AbstractValue newValue) {
    SAWYER_MESG(logger[TRACE])<<"DEBUG: init memory location:start"<<endl;
    reserveMemoryLocation(lab,pstate,memLoc);
    SAWYER_MESG(logger[TRACE])<<"DEBUG: init memory location:p0"<<endl;
    writeToMemoryLocation(lab,pstate,memLoc,newValue);
    SAWYER_MESG(logger[TRACE])<<"DEBUG: init memory location:end"<<endl;
  }

  void EStateTransferFunctions::reserveMemoryLocation(Label lab, PState* pstate, AbstractValue memLoc) {
    writeUndefToMemoryLocation(lab,pstate,memLoc);
  }

  void EStateTransferFunctions::writeUndefToMemoryLocation(Label lab, PState* pstate, AbstractValue memLoc) {
#if 0
    if(AbstractValue::byteMode) {
      AbstractValue undefValue=AbstractValue::createUndefined();
      VariableId varId=memLoc.getVariableId();
      memLoc.setElementTypeSize(_variableIdMapping->getElementSize(varId)); // TODO: structs vars?
    }
#endif
    pstate->writeUndefToMemoryLocation(memLoc);
    //writeToMemoryLocation(lab,pstate,memLoc,undefValue);
  }

  void EStateTransferFunctions::writeUndefToMemoryLocation(PState* pstate, AbstractValue memLoc) {
#if 0
    if(AbstractValue::byteMode && memLoc.getElementTypeSize()==0) {
      ROSE_ASSERT(memLoc.isPtr());
      VariableId varId=memLoc.getVariableId();
      memLoc.setElementTypeSize(_variableIdMapping->getElementSize(varId)); // TODO: structs vars?
    }
#endif
    pstate->writeUndefToMemoryLocation(memLoc);
  }

  void EStateTransferFunctions::printLoggerWarning(EState& estate) {
    Label lab=estate.label();
    if(_analyzer) {
      SAWYER_MESG(logger[WARN]) << "at label "<<lab<<": "<<(_analyzer->getLabeler()->getNode(lab)->unparseToString())<<": this pointer set to top."<<endl;
    } else {
      SAWYER_MESG(logger[WARN]) << "at label "<<lab<<": "<<": this pointer set to top."<<endl;
    }
  }

  void EStateTransferFunctions::setReadWriteListener(ReadWriteListener* rwl) {
    _readWriteListener=rwl;
  }

  ReadWriteListener* EStateTransferFunctions::getReadWriteListener() {
    return _readWriteListener;
  }

  bool EStateTransferFunctions::isFunctionCallWithAssignment(Label lab,VariableId* varIdPtr){
    //return _labeler->isFunctionCallWithAssignment(lab,varIdPtr);
    SgNode* node=getLabeler()->getNode(lab);
    if(getLabeler()->isFunctionCallLabel(lab)) {
      std::pair<SgVarRefExp*,SgFunctionCallExp*> p=SgNodeHelper::Pattern::matchExprStmtAssignOpVarRefExpFunctionCallExp2(node);
      if(p.first) {
	if(varIdPtr) {
	  *varIdPtr=getVariableIdMapping()->variableId(p.first);
	}
	return true;
      }
    }
    return false;
  }

  // wrapper function for reusing exprAnalyzer's function. This function
  // is only relevant for external functions (when the implementation
  // does not exist in input program and the semantics are available in
  // the analyzer (e.g. malloc, strlen, etc.))
  list<EState> EStateTransferFunctions::evaluateFunctionCallArguments(Edge edge, SgFunctionCallExp* funCall, EState currentEState, bool useConstraints) {
    CallString cs=currentEState.callString;
    SAWYER_MESG(logger[TRACE]) <<"evaluating arguments of function call:"<<funCall->unparseToString()<<endl;
    list<SingleEvalResultConstInt> evalResultList=evalFunctionCallArguments(funCall, currentEState);
    ROSE_ASSERT(evalResultList.size()>0);
    list<SingleEvalResultConstInt>::iterator resultListIter=evalResultList.begin();
    SingleEvalResultConstInt evalResult=*resultListIter;
    if(evalResultList.size()>1) {
      SAWYER_MESG(logger[ERROR]) <<"multi-state generating operators in function call parameters not supported."<<endl;
      exit(1);
    }
    PState newPState=*evalResult.estate.pstate();
    ConstraintSet cset=*evalResult.estate.constraints();
    return elistify(createEState(edge.target(),cs,newPState,cset));
  }

  
} // end of namespace
