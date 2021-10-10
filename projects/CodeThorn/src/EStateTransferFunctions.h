#ifndef ESTATE_TRANSFER_FUNCTIONS
#define ESTATE_TRANSFER_FUNCTIONS

#include "EState.h"
#include "DFTransferFunctions.h"
#include "CTIOLabeler.h"
#include <limits.h>
#include <string>
#include <unordered_set>
#include "Labeler.h"
#include "EState.h"
#include "VariableIdMapping.h"
#include "AbstractValue.h"
#include "AstTerm.h"
#include "ProgramLocationsReport.h"
#include "TypeSizeMapping.h"
#include "CodeThornOptions.h"
#include "ReadWriteListener.h"
#include "MemoryViolationAnalysis.h"

namespace CodeThorn {
  class CTAnalysis;
  
    class SingleBoolEvalResult {
  public:
    EState estate;
    CodeThorn::BoolLattice result;
    bool isTop() {return result.isTop();}
    bool isTrue() {return result.isTrue();}
    bool isFalse() {return result.isFalse();}
    bool isBot() {return result.isBot();}
  };

  class SingleEvalResult {
  public:
    void init(EState estate, AbstractValue result);
    EState estate;
    AbstractValue result;
    AbstractValue value() {return result;}
    bool isConstInt() {return result.isConstInt();}
    bool isTop() {return result.isTop();}
    bool isTrue() {return result.isTrue();}
    bool isFalse() {return result.isFalse();}
    bool isBot() {return result.isBot();}
  };

  enum InterpreterMode { IM_DISABLED, IM_ENABLED };
  // ACCESS_ERROR is null pointer dereference is detected. ACCESS_NON_EXISTING if pointer is lattice bottom element.
  enum MemoryAccessBounds {ACCESS_ERROR,ACCESS_DEFINITELY_NP, ACCESS_DEFINITELY_INSIDE_BOUNDS, ACCESS_POTENTIALLY_OUTSIDE_BOUNDS, ACCESS_DEFINITELY_OUTSIDE_BOUNDS, ACCESS_NON_EXISTING};

  class EStateTransferFunctions : public DFTransferFunctions {
  public:
    EStateTransferFunctions();
    static void initDiagnostics();
    // must be set
    void setAnalyzer(CodeThorn::CTAnalysis* analyzer);
    CTAnalysis* getAnalyzer();

    AbstractValue::Operator sgNodeToAbstractValueOperator(SgNode* node);
    
    // obtained from analyzer
    Labeler* getLabeler() override;
    // obtained from analyzer
    VariableIdMappingExtended* getVariableIdMapping() override;

    enum TransferFunctionCode
      {
       Unknown, // for internal error detection only
       FunctionCall,
       FunctionCallLocalEdge,
       FunctionCallExternal,
       FunctionCallReturn,
       FunctionEntry,
       FunctionExit,
       ReturnStmt,
       FailedAssert,
       AsmStmt,
       ExprStmt,
       GnuExtensionStmtExpr,
       Identity,
       VariableDeclaration,
       CaseOptionStmt,
       DefaultOptionStmt,
       Assign,
       IncDec,
       ForkFunction,
       ForkFunctionWithExternalTargetFunction,
      };

    enum EvalMode { MODE_ADDRESS, MODE_VALUE, MODE_EMPTY_STATE };

    EState createEState(Label label, CallString cs, PState pstate);
    EState createEState(Label label, CallString cs, PState pstate, InputOutput io);
    EState createEStateInternal(Label label, PState pstate);

    bool isApproximatedBy(const EState* es1, const EState* es2);
    EState combine(const EState* es1, const EState* es2);
    std::string transferFunctionCodeToString(TransferFunctionCode tfCode);
    
    /* determines transfer function code from CFG and AST-matching and calls transferEdgeEStateDispatch
       ultimately this function can be used to operate on its own IR */
    std::list<EState> transferEdgeEState(Edge edge, const EState* estate);

    // determines transfer function code based on ICFG and AST patterns
    std::pair<TransferFunctionCode,SgNode*> determineTransferFunctionCode(Edge edge, const EState* estate);
    // calls transfer function based on TransferFunctionCode. No additional tests are performed.
    std::list<EState> transferEdgeEStateDispatch(TransferFunctionCode tfCode, SgNode* node, Edge edge, const EState* estate);
    void printTransferFunctionInfo(TransferFunctionCode tfCode, SgNode* node, Edge edge, const EState* estate);
    void printEvaluateExpressionInfo(SgNode* node,EState& estate, EvalMode mode);

  protected:
    void fatalErrorExit(SgNode* node, std::string errorMessage);
    void transferFunctionEntryPrintStatus(Edge edge, const EState* estate, std::string fileName, std::string functionName);
    
    std::list<EState> transferFunctionCallLocalEdge(Edge edge, const EState* estate);
    std::list<EState> transferFunctionCall(Edge edge, const EState* estate);
    std::list<EState> transferFunctionCallExternal(Edge edge, const EState* estate);
    std::list<EState> transferFunctionCallReturn(Edge edge, const EState* estate);
    std::list<EState> transferFunctionEntry(Edge edge, const EState* estate);
    std::list<EState> transferFunctionExit(Edge edge, const EState* estate);
    std::list<EState> transferReturnStmt(Edge edge, const EState* estate);
    std::list<EState> transferAsmStmt(Edge edge, const EState* estate);
    std::list<EState> transferIdentity(Edge edge, const EState* estate);
    std::list<EState> transferFailedAssert(Edge edge, const EState* estate);

    std::list<EState> transferCaseOptionStmt(SgCaseOptionStmt* stmt,Edge edge, const EState* estate);
    std::list<EState> transferDefaultOptionStmt(SgDefaultOptionStmt* stmt,Edge edge, const EState* estate);
    std::list<EState> transferVariableDeclaration(SgVariableDeclaration* decl,Edge edge, const EState* estate);

    std::list<EState> transferExprStmt(SgNode* nextNodeToAnalyze1, Edge edge, const EState* estate);
    // wrapper function for evalAssignOp
    std::list<EState> transferAssignOp(SgAssignOp* assignOp, Edge edge, const EState* estate);
    // used at stmt level and for StmtExpr
    std::list<EState> transferIncDecOp(SgNode* nextNodeToAnalyze2, Edge edge, const EState* estate);
    std::list<EState> transferGnuExtensionStmtExpr(SgNode* nextNodeToAnalyze1, Edge edge, const EState* estate);
    
    // not used yet
    std::list<EState> transferIncDecOpEvalWrapper(SgNode* nextNodeToAnalyze2, Edge edge, const EState* estate);

    // special case, called from transferFunctionCall
    std::list<EState> transferForkFunction(Edge edge, const EState* estate, SgFunctionCallExp* funCall);
    std::list<EState> transferForkFunctionWithExternalTargetFunction(Edge edge, const EState* estate, SgFunctionCallExp* funCall);

    std::list<EState> transferTrueFalseEdge(SgNode* nextNodeToAnalyze2, Edge edge, const EState* estate);

  public:
    static std::list<EState> elistify();
    static std::list<EState> elistify(EState res);

    // logger facility
    static Sawyer::Message::Facility logger;

    // used to  create a new estate (shallow copy, PState copied as pointer)
    EState cloneEState(const EState* estate);

    // used by transferAssignOp to seperate evaluation from memory updates (i.e. state modifications)
    typedef std::pair<AbstractValue,AbstractValue> MemoryUpdatePair;
    typedef std::list<std::pair<EState,MemoryUpdatePair> > MemoryUpdateList;
    MemoryUpdateList evalAssignOpMemUpdates(SgAssignOp* assignOp, const EState* estate);

    // functions for handling callstring contexts
    CallString transferFunctionCallContext(CallString cs, Label lab);
    bool isFeasiblePathContext(CallString& cs,Label lab);

    CodeThorn::VariableIdSet determineUsedGlobalVars(SgProject* root, CodeThorn::VariableIdSet& setOfGlobalVars);
    void initializeGlobalVariables(SgProject* root, EState& estate);
    // modifies PState with written initializers
    EState transferVariableDeclarationEState(SgVariableDeclaration* decl,EState currentEState, Label targetLabel);
    EState transferVariableDeclarationWithInitializerEState(SgVariableDeclaration* decl, SgInitializedName* initName, SgInitializer* initializer, VariableId initDeclVarId, EState& currentEState, Label targetLabel);
    EState transferVariableDeclarationWithoutInitializerEState(SgVariableDeclaration* decl, SgInitializedName* initName, VariableId initDeclVarId, EState& currentEState, Label targetLabel);
    
    PState analyzeSgAggregateInitializer(VariableId initDeclVarId, SgAggregateInitializer* aggregateInitializer,PState pstate, /* for evaluation only  */ EState currentEState);
  private:
    // auxiliary semantic functions
    void declareUninitializedStruct(Label label,PState* pstate,AbstractValue structAddress, VariableId memVarId);
    AbstractValue createStructDataMemberAddress(AbstractValue structAddress,VariableId varId);
    bool isGlobalAddress(AbstractValue memLoc); 
  public:
    // determines whether lab is a function call label of a function
    // call of the form 'x=f(...)' and returns the varible-id of the
    // lhs, if a valid pointer is provided
    bool isFunctionCallWithAssignment(Label lab,VariableId* varId=0);
    // this function uses the respective function of ExprAnalyzer and
    // extracts the result from the ExprAnalyzer data structure.
    std::list<EState> evaluateFunctionCallArguments(Edge edge, SgFunctionCallExp* funCall, EState estate, bool useConstraints);


    // Limits the number of results to one result only. Does not permit state splitting.
    // requires normalized AST
    AbstractValue evaluateExpressionAV(SgExpression* expr,EState currentEState);

    // only used in hybrid prototype binding
    VariableId globalVarIdByName(std::string varName);

  protected:
    void setElementSize(VariableId variableId, SgType* elementType);
    CodeThorn::CTAnalysis* _analyzer=nullptr;
    std::string _rersHybridOutputFunctionName="calculate_output"; // only used if rersmode is active
  private:
    // only used in RERS mapping for hybrid output function name (transferFunctionCallLocalEdge)
    std::map<std::string,VariableId> globalVarName2VarIdMapping;

    // integrated ExprAnalyzer
  public:

    //SingleEvalResult eval(SgNode* node,EState estate);
    //! compute abstract lvalue
    SingleEvalResult evaluateLExpression(SgNode* node,EState estate);
    //! Evaluates an expression using AbstractValue and returns a list
    //! of all evaluation-results.  There can be multiple results if
    //! one of the variables was bound to top and branching constructs
    //! are inside the expression.
    SingleEvalResult evaluateExpression(SgNode* node,EState estate, EvalMode mode=MODE_VALUE);
    //! uses AbstractValue::getVariableIdMapping()
    AbstractValue evaluateExpressionWithEmptyState(SgExpression* expr);

    void setVariableIdMapping(VariableIdMappingExtended* variableIdMapping);

    void setSkipUnknownFunctionCalls(bool skip);
    bool getSkipUnknownFunctionCalls();
    void setSkipArrayAccesses(bool skip);
    bool getSkipArrayAccesses();

    // obtained from ctOpt
    bool getIgnoreUndefinedDereference();
    bool getIgnoreFunctionPointers();

    void setSVCompFunctionSemantics(bool flag);
    bool getSVCompFunctionSemantics();
    // deprecated
    bool stdFunctionSemantics();
    bool getStdFunctionSemantics();
    void setStdFunctionSemantics(bool flag);

    bool getPrintDetectedViolations();
    void setPrintDetectedViolations(bool flag);

    enum MemoryAccessBounds checkMemoryAccessBounds(AbstractValue address);

    // record detected errors in programs
    ProgramLocationsReport getProgramLocationsReport(enum AnalysisSelector analysisSelector);

    // record detected errors in programs
    void recordDefinitiveViolatingLocation(enum AnalysisSelector analysisSelector, Label lab);
    void recordPotentialViolatingLocation(enum AnalysisSelector analysisSelector, Label lab);
    std::string analysisSelectorToString(AnalysisSelector sel);

    // deprecated (still being used here)
    void recordDefinitiveNullPointerDereferenceLocation(Label lab);
    void recordPotentialNullPointerDereferenceLocation(Label lab);
    void recordDefinitiveOutOfBoundsAccessLocation(Label lab);
    void recordPotentialOutOfBoundsAccessLocation(Label lab);
    void recordDefinitiveUninitializedAccessLocation(Label lab);
    void recordPotentialUninitializedAccessLocation(Label lab);

    bool definitiveErrorDetected();
    bool potentialErrorDetected();

    void setOptionOutputWarnings(bool flag);
    bool getOptionOutputWarnings();

    //! returns true if node is a VarRefExp and sets varId=id, otherwise false and varId=0.
    bool checkIfVariableAndDetermineVarId(SgNode* node,VariableId& varId); // only used by CTAnalysis

    SingleEvalResult evalFunctionCallArguments(SgFunctionCallExp* funCall, EState estate);
    SingleEvalResult evalFunctionCall(SgFunctionCallExp* node, EState estate);
    bool isLValueOp(SgNode* node);
    // requires StructureAccessLookup to be initialized.
    bool isMemberVariable(CodeThorn::VariableId varId);
    // checks if value is a null pointer. If it is 0 it records a null pointer violation at provided label.
    // returns true if execution may continue, false if execution definitely does not continue.
    bool checkAndRecordNullPointer(AbstractValue value, Label label);

    enum InterpreterMode getInterpreterMode();
    void setInterpreterMode(enum InterpreterMode);
    std::string getInterpreterModeFileName();
    void setInterpreterModeFileName(std::string);

    // converts memLoc if array abstraction is active
    // remaps index if necessary and sets summary flag
    AbstractValue conditionallyApplyArrayAbstraction(AbstractValue memLoc);
    // reserves memory location at address memLoc and sets as value 'undef'
    void reserveMemoryLocation(Label lab, PState* pstate, AbstractValue memLoc);
    // reserves and initializes memory location at address memLoc with newValue
    void initializeMemoryLocation(Label lab, PState* pstate, AbstractValue memLoc, AbstractValue newValue);
    // handles addresses only
    AbstractValue readFromMemoryLocation(Label lab, PStatePtr pstate, AbstractValue memLoc);
    // handles only references (models indirection)
    AbstractValue readFromReferenceMemoryLocation(Label lab, PStatePtr pstate, AbstractValue memLoc);
    // handles both addresses and references
    AbstractValue readFromAnyMemoryLocation(Label lab, PStatePtr pstate, AbstractValue memLoc);
    // handles addresses only
    void writeToMemoryLocation(Label lab, PState* pstate, AbstractValue memLoc, AbstractValue newValue);
    // handles only references (models indirection)
    void writeToReferenceMemoryLocation(Label lab, PState* pstate, AbstractValue memLoc, AbstractValue newValue);
    // handles both addresses and references
    void writeToAnyMemoryLocation(Label lab, PState* pstate, AbstractValue memLoc, AbstractValue newValue);

    // memory must already be reserved (hence, this function is redundant if reserves is used before)
    void writeUndefToMemoryLocation(Label lab, PState* pstate, AbstractValue memLoc);
    void writeUndefToMemoryLocation(PState* pstate, AbstractValue memLoc);

    // utilify functions
    AbstractValue getMemoryRegionAbstractElementSize(CodeThorn::AbstractValue regionAddress);
    AbstractValue getMemoryRegionAbstractNumElements(CodeThorn::AbstractValue regionAddress);
    int getMemoryRegionNumElements(CodeThorn::AbstractValue regionAddress);
    int getMemoryRegionElementSize(CodeThorn::AbstractValue regionAddress);

    // if set to 0 then no listner active. By default it is 0.
    size_t numberOfReadWriteListeners();
    void registerReadWriteListener(ReadWriteListener* listener, std::string name="");
    ReadWriteListener* getReadWriteListener(std::string name);

    // initialize command line arguments provided by option "--cl-options" in PState
    void initializeCommandLineArgumentsInState(Label lab, PState& initialPState);
    void initializeStringLiteralInState(Label lab, PState& initialPState,SgStringVal* stringValNode, VariableId stringVarId);
    void initializeStringLiteralsInState(Label lab, PState& initialPState);

  protected:
    AbstractValue abstractValueFromSgValueExp(SgValueExp* valueExp, EvalMode mode);
    std::map<std::string,ReadWriteListener*> _readWriteListenerMap;
    
    // general evaluation function for abstract value operators
    SingleEvalResult evalOp(SgNode* node,
					       SingleEvalResult lhsResult,
					       SingleEvalResult rhsResult,
					       EState estate, EvalMode mode);

    // evaluation functions
    SingleEvalResult evalConditionalExpr(SgConditionalExp* node, EState estate, EvalMode mode=MODE_VALUE);
    SingleEvalResult evaluateShortCircuitOperators(SgNode* node,EState estate, EvalMode mode=MODE_VALUE);

    SingleEvalResult evalEqualOp(SgEqualityOp* node,
                                               SingleEvalResult lhsResult,
                                               SingleEvalResult rhsResult,
                                               EState estate, EvalMode mode=MODE_VALUE);
    SingleEvalResult evalNotEqualOp(SgNotEqualOp* node,
                                                  SingleEvalResult lhsResult,
                                                  SingleEvalResult rhsResult,
                                                  EState estate, EvalMode mode=MODE_VALUE);
    SingleEvalResult evalAndOp(SgAndOp* node,
                                             SingleEvalResult lhsResult,
                                             SingleEvalResult rhsResult,
                                             EState estate, EvalMode mode=MODE_VALUE);
    SingleEvalResult evalOrOp(SgOrOp* node,
                                            SingleEvalResult lhsResult,
                                            SingleEvalResult rhsResult,
                                            EState estate, EvalMode mode=MODE_VALUE);
    SingleEvalResult evalAddOp(SgAddOp* node,
                                             SingleEvalResult lhsResult,
                                             SingleEvalResult rhsResult,
                                             EState estate, EvalMode mode=MODE_VALUE);
    SingleEvalResult evalSubOp(SgSubtractOp* node,
                                             SingleEvalResult lhsResult,
                                             SingleEvalResult rhsResult,
                                             EState estate, EvalMode mode=MODE_VALUE);
    SingleEvalResult evalMulOp(SgMultiplyOp* node,
                                             SingleEvalResult lhsResult,
                                             SingleEvalResult rhsResult,
                                             EState estate, EvalMode mode=MODE_VALUE);
    SingleEvalResult evalDivOp(SgDivideOp* node,
                                             SingleEvalResult lhsResult,
                                             SingleEvalResult rhsResult,
                                             EState estate, EvalMode mode=MODE_VALUE);
    SingleEvalResult evalModOp(SgModOp* node,
                                             SingleEvalResult lhsResult,
                                             SingleEvalResult rhsResult,
                                             EState estate, EvalMode mode=MODE_VALUE);
    SingleEvalResult evalBitwiseAndOp(SgBitAndOp* node,
                                                    SingleEvalResult lhsResult,
                                                    SingleEvalResult rhsResult,
                                                    EState estate, EvalMode mode=MODE_VALUE);
    SingleEvalResult evalBitwiseOrOp(SgBitOrOp* node,
                                                   SingleEvalResult lhsResult,
                                                   SingleEvalResult rhsResult,
                                                   EState estate, EvalMode mode=MODE_VALUE);
    SingleEvalResult evalBitwiseXorOp(SgBitXorOp* node,
                                                    SingleEvalResult lhsResult,
                                                    SingleEvalResult rhsResult,
                                                    EState estate, EvalMode mode=MODE_VALUE);

    SingleEvalResult evalGreaterOrEqualOp(SgGreaterOrEqualOp* node,
                                                        SingleEvalResult lhsResult,
                                                        SingleEvalResult rhsResult,
                                                        EState estate, EvalMode mode=MODE_VALUE);
    SingleEvalResult evalGreaterThanOp(SgGreaterThanOp* node,
                                                     SingleEvalResult lhsResult,
                                                     SingleEvalResult rhsResult,
                                                     EState estate, EvalMode mode=MODE_VALUE);
    SingleEvalResult evalLessOrEqualOp(SgLessOrEqualOp* node,
                                                     SingleEvalResult lhsResult,
                                                     SingleEvalResult rhsResult,
                                                     EState estate, EvalMode mode=MODE_VALUE);
    SingleEvalResult evalLessThanOp(SgLessThanOp* node,
                                                  SingleEvalResult lhsResult,
                                                  SingleEvalResult rhsResult,
                                                  EState estate, EvalMode mode=MODE_VALUE);
    SingleEvalResult evalBitwiseShiftLeftOp(SgLshiftOp* node,
                                                          SingleEvalResult lhsResult,
                                                          SingleEvalResult rhsResult,
                                                          EState estate, EvalMode mode=MODE_VALUE);
    SingleEvalResult evalBitwiseShiftRightOp(SgRshiftOp* node,
                                                           SingleEvalResult lhsResult,
                                                           SingleEvalResult rhsResult,
                                                           EState estate, EvalMode mode=MODE_VALUE);

    SingleEvalResult evalAssignOp(SgAssignOp* node,
						     SingleEvalResult lhsResult,
						     SingleEvalResult rhsResult,
						     Label targetLabel, EState estate, EvalMode mode);
    std::list<EState> evalAssignOp3(SgAssignOp* node, Label targetLabel, const EState* estate);
    
    SingleEvalResult evalArrayReferenceOp(SgPntrArrRefExp* node,
                                                        SingleEvalResult lhsResult,
                                                        SingleEvalResult rhsResult,
                                                        EState estate, EvalMode mode=MODE_VALUE);
    SingleEvalResult evalCommaOp(SgCommaOpExp* node,
                                                        SingleEvalResult lhsResult,
                                                        SingleEvalResult rhsResult,
                                                        EState estate, EvalMode mode=MODE_VALUE);

    SingleEvalResult evalNotOp(SgNotOp* node,
                                             SingleEvalResult operandResult,
                                             EState estate, EvalMode mode=MODE_VALUE);
    SingleEvalResult evalUnaryMinusOp(SgMinusOp* node,
                                                    SingleEvalResult operandResult,
                                                    EState estate, EvalMode mode=MODE_VALUE);
    SingleEvalResult evalArrowOp(SgArrowExp* node,
                                               SingleEvalResult lhsResult,
                                               SingleEvalResult rhsResult,
                                               EState estate, EvalMode mode=MODE_VALUE);
    SingleEvalResult evalDotOp(SgDotExp* node,
                                             SingleEvalResult lhsResult,
                                             SingleEvalResult rhsResult,
                                             EState estate, EvalMode mode=MODE_VALUE);
    SingleEvalResult evalCastOp(SgCastExp* node,
                                              SingleEvalResult operandResult,
                                              EState estate, EvalMode mode=MODE_VALUE);
    SingleEvalResult evalDereferenceOp(SgPointerDerefExp* node,
                                                     SingleEvalResult operandResult,
                                                     EState estate, EvalMode mode=MODE_VALUE);
    SingleEvalResult semanticEvalDereferenceOp(SingleEvalResult operandResult,
                                                             EState estate, EvalMode mode=MODE_VALUE);
    SingleEvalResult evalAddressOfOp(SgAddressOfOp* node,
                                                   SingleEvalResult operandResult,
                                                   EState estate, EvalMode mode=MODE_VALUE);
    SingleEvalResult evalPreComputationOp(EState estate, AbstractValue address, AbstractValue change);
    SingleEvalResult evalPreIncrementOp(SgPlusPlusOp* node,
                  SingleEvalResult operandResult,
                  EState estate, EvalMode mode=MODE_VALUE);
    SingleEvalResult evalPostIncrementOp(SgPlusPlusOp* node,
                   SingleEvalResult operandResult,
                   EState estate, EvalMode mode=MODE_VALUE);
    SingleEvalResult evalPostComputationOp(EState estate, AbstractValue address, AbstractValue change);
    SingleEvalResult evalPreDecrementOp(SgMinusMinusOp* node,
                  SingleEvalResult operandResult,
                  EState estate, EvalMode mode=MODE_VALUE);
    SingleEvalResult evalPostDecrementOp(SgMinusMinusOp* node,
                   SingleEvalResult operandResult,
                   EState estate, EvalMode mode=MODE_VALUE);

    // dispatch function
    SingleEvalResult evalMinusMinusOp(SgMinusMinusOp* node,
                                                    SingleEvalResult operandResult,
                                                    EState estate, EvalMode mode=MODE_VALUE);
    // dispatch function
    SingleEvalResult evalPlusPlusOp(SgPlusPlusOp* node,
                                                  SingleEvalResult operandResult,
                                                  EState estate, EvalMode mode=MODE_VALUE);
    SingleEvalResult evalBitwiseComplementOp(SgBitComplementOp* node,
                                                           SingleEvalResult operandResult,
                                                           EState estate, EvalMode mode=MODE_VALUE);

    // special case of sizeof operator (operates on types and types of expressions)
    SingleEvalResult evalSizeofOp(SgSizeOfOp* node,
                                                EState estate, EvalMode mode=MODE_VALUE);

    SingleEvalResult evalLValuePntrArrRefExp(SgPntrArrRefExp* node, EState estate, EvalMode mode=MODE_VALUE);
    SingleEvalResult evalLValueVarRefExp(SgVarRefExp* node, EState estate, EvalMode mode=MODE_VALUE);
    SingleEvalResult evalLValuePointerDerefExp(SgPointerDerefExp* node, EState estate);
    // handles DotExp and ArrowExp
    SingleEvalResult evalLValueDotOrArrowExp(SgNode* node, EState estate, EvalMode mode=MODE_VALUE);

    SingleEvalResult evalRValueVarRefExp(SgVarRefExp* node, EState estate, EvalMode mode=MODE_VALUE);
    SingleEvalResult evalValueExp(SgValueExp* node, EState estate, EvalMode mode);

    SingleEvalResult evalFunctionRefExp(SgFunctionRefExp* node, EState estate, EvalMode mode=MODE_VALUE);
    // supported system functions
    SingleEvalResult evalFunctionCallMalloc(SgFunctionCallExp* funCall, EState estate);
    SingleEvalResult evalFunctionCallFree(SgFunctionCallExp* funCall, EState estate);
    SingleEvalResult evalFunctionCallMemCpy(SgFunctionCallExp* funCall, EState estate);
    SingleEvalResult evalFunctionCallStrLen(SgFunctionCallExp* funCall, EState estate);

    // supported functions to be executed (interpreter mode)
    SingleEvalResult execFunctionCallPrintf(SgFunctionCallExp* funCall, EState estate);
    SingleEvalResult execFunctionCallScanf(SgFunctionCallExp* funCall, EState estate);
    std::string sourceLocationAndNodeToString(Label lab);
  private:
    // outdated function, to be eliminated
    int computeNumberOfElements(SgVariableDeclaration* decl);

    void printLoggerWarning(EState& estate);
    void initViolatingLocations();
    VariableIdMappingExtended* _variableIdMapping=nullptr;
    std::vector<ProgramLocationsReport> _violatingLocations;
    // Options
    bool _skipSelectedFunctionCalls=false;
    bool _skipArrayAccesses=false;
    bool _stdFunctionSemantics=true;
    bool _svCompFunctionSemantics=false;
    bool _ignoreUndefinedDereference=false;
    bool _ignoreFunctionPointers=false;
    bool _printDetectedViolations=false;
    enum InterpreterMode _interpreterMode=IM_DISABLED;
    std::string _interpreterModeFileName;
    bool _optionOutputWarnings=false;
  };
}

#endif