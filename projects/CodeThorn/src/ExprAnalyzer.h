#ifndef CT_EXPR_ANALYZER_H
#define CT_EXPR_ANALYZER_H

/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

#include <limits.h>
#include <string>
#include <unordered_set>
#include "Labeler.h"
#include "EState.h"
#include "VariableIdMapping.h"
#include "AbstractValue.h"
#include "AstTerm.h"
#include "ProgramLocationsReport.h"
#include "SgTypeSizeMapping.h"
#include "StructureAccessLookup.h"

using namespace std;

namespace CodeThorn {

  class Analyzer;
  /*! 
   * \author Markus Schordan
   * \date 2012.
   */
  class SingleEvalResult {
  public:
    EState estate;
    CodeThorn::BoolLattice result;
    bool isTop() {return result.isTop();}
    bool isTrue() {return result.isTrue();}
    bool isFalse() {return result.isFalse();}
    bool isBot() {return result.isBot();}
  };
  
  /*! 
   * \author Markus Schordan
   * \date 2012.
   */
  class SingleEvalResultConstInt {
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
  
  enum InterpretationMode { IM_ABSTRACT, IM_CONCRETE };
  // ACCESS_ERROR is null pointer dereference is detected. ACCESS_NON_EXISTING if pointer is lattice bottom element.
  enum MemoryAccessBounds {ACCESS_ERROR,ACCESS_DEFINITELY_NP, ACCESS_DEFINITELY_INSIDE_BOUNDS, ACCESS_POTENTIALLY_OUTSIDE_BOUNDS, ACCESS_DEFINITELY_OUTSIDE_BOUNDS, ACCESS_NON_EXISTING};

  /*! 
   * \author Markus Schordan
   * \date 2012.
   */
  //#define EXPR_VISITOR
  class ExprAnalyzer {
  public:
    enum EvalMode { MODE_ADDRESS, MODE_VALUE };
    ExprAnalyzer();
    void setAnalyzer(Analyzer* analyzer);
    //SingleEvalResult eval(SgNode* node,EState estate);
    //! compute abstract lvalue
    list<SingleEvalResultConstInt> evaluateLExpression(SgNode* node,EState estate);
    //! Evaluates an expression using AbstractValue and returns a list
    //! of all evaluation-results.  There can be multiple results if
    //! one of the variables was bound to top and branching constructs
    //! are inside the expression.
    list<SingleEvalResultConstInt> evaluateExpression(SgNode* node,EState estate, EvalMode mode=MODE_VALUE);
    void setVariableIdMapping(VariableIdMapping* variableIdMapping);
    void setSkipSelectedFunctionCalls(bool skip);
    bool getSkipSelectedFunctionCalls();
    void setSkipArrayAccesses(bool skip);
    bool getSkipArrayAccesses();
    void setIgnoreUndefinedDereference(bool skip);
    bool getIgnoreUndefinedDereference();
    void setIgnoreFunctionPointers(bool skip);
    bool getIgnoreFunctionPointers();
    void setSVCompFunctionSemantics(bool flag);
    bool getSVCompFunctionSemantics();
    // deprecated
    bool stdFunctionSemantics();
    bool getStdFunctionSemantics();
    void setStdFunctionSemantics(bool flag);

    bool getPrintDetectedViolations();
    void setPrintDetectedViolations(bool flag);
    
    // deprecated (superseded by checkMemoryAccessBounds
    bool accessIsWithinArrayBounds(VariableId arrayVarId,int accessIndex);
    // supersedes accessIsWithinArrayBounds
    enum MemoryAccessBounds checkMemoryAccessBounds(AbstractValue address);
    
    // deprecated
    //VariableId resolveToAbsoluteVariableId(AbstractValue abstrValue) const;
    AbstractValue computeAbstractAddress(SgVarRefExp* varRefExp);

    // record detected errors in programs
    void recordDefinitiveNullPointerDereferenceLocation(Label lab);
    void recordPotentialNullPointerDereferenceLocation(Label lab);
    ProgramLocationsReport getNullPointerDereferenceLocations();

    void recordDefinitiveOutOfBoundsAccessLocation(Label lab);
    void recordPotentialOutOfBoundsAccessLocation(Label lab);
    ProgramLocationsReport getOutOfBoundsAccessLocations();

    void recordDefinitiveUninitializedAccessLocation(Label lab);
    void recordPotentialUninitializedAccessLocation(Label lab);
    ProgramLocationsReport getUninitializedAccessLocations();

    bool definitiveErrorDetected();
    bool potentialErrorDetected();

    void setOptionOutputWarnings(bool flag);
    bool getOptionOutputWarnings();

    //! returns true if node is a VarRefExp and sets varName=name, otherwise false and varName="$".
    static bool variable(SgNode* node,VariableName& varName);
    //! returns true if node is a VarRefExp and sets varId=id, otherwise false and varId=0.
    bool variable(SgNode* node,VariableId& varId);
    list<SingleEvalResultConstInt> evalFunctionCallArguments(SgFunctionCallExp* funCall, EState estate);
    list<SingleEvalResultConstInt> evalFunctionCall(SgFunctionCallExp* node, EState estate);
    bool isLValueOp(SgNode* node);
    void initializeStructureAccessLookup(SgProject* node);
    // requires StructureAccessLookup to be initialized.
    bool isStructMember(CodeThorn::VariableId varId);
    // checks if value is a null pointer. If it is 0 it records a null pointer violation at provided label.
    // returns true if execution may continue, false if execution definitely does not continue.
    bool checkAndRecordNullPointer(AbstractValue value, Label label);

    enum InterpretationMode getInterpretationMode();
    void setInterpretationMode(enum InterpretationMode);

  protected:
    static void initDiagnostics();
    static Sawyer::Message::Facility logger;
    AbstractValue constIntLatticeFromSgValueExp(SgValueExp* valueExp);
    
    //! This function turn a single result into a one-elment list with
    //! this one result.
    static list<SingleEvalResultConstInt> listify(SingleEvalResultConstInt res);
    
    // evaluation state
#ifdef EXPR_VISITOR
    SingleEvalResultConstInt res;
#endif
    // evaluation functions
    list<SingleEvalResultConstInt> evalConditionalExpr(SgConditionalExp* node, EState estate, EvalMode mode=MODE_VALUE);
    list<SingleEvalResultConstInt> evaluateShortCircuitOperators(SgNode* node,EState estate, EvalMode mode=MODE_VALUE);

    list<SingleEvalResultConstInt> evalEqualOp(SgEqualityOp* node,
                                               SingleEvalResultConstInt lhsResult, 
                                               SingleEvalResultConstInt rhsResult,
                                               EState estate, EvalMode mode=MODE_VALUE);
    list<SingleEvalResultConstInt> evalNotEqualOp(SgNotEqualOp* node,
                                                  SingleEvalResultConstInt lhsResult, 
                                                  SingleEvalResultConstInt rhsResult,
                                                  EState estate, EvalMode mode=MODE_VALUE);
    list<SingleEvalResultConstInt> evalAndOp(SgAndOp* node,
                                             SingleEvalResultConstInt lhsResult, 
                                             SingleEvalResultConstInt rhsResult,
                                             EState estate, EvalMode mode=MODE_VALUE);
    list<SingleEvalResultConstInt> evalOrOp(SgOrOp* node,
                                            SingleEvalResultConstInt lhsResult, 
                                            SingleEvalResultConstInt rhsResult,
                                            EState estate, EvalMode mode=MODE_VALUE);
    list<SingleEvalResultConstInt> evalAddOp(SgAddOp* node,
                                             SingleEvalResultConstInt lhsResult, 
                                             SingleEvalResultConstInt rhsResult,
                                             EState estate, EvalMode mode=MODE_VALUE);
    list<SingleEvalResultConstInt> evalSubOp(SgSubtractOp* node,
                                             SingleEvalResultConstInt lhsResult, 
                                             SingleEvalResultConstInt rhsResult,
                                             EState estate, EvalMode mode=MODE_VALUE);
    list<SingleEvalResultConstInt> evalMulOp(SgMultiplyOp* node,
                                             SingleEvalResultConstInt lhsResult, 
                                             SingleEvalResultConstInt rhsResult,
                                             EState estate, EvalMode mode=MODE_VALUE);
    list<SingleEvalResultConstInt> evalDivOp(SgDivideOp* node,
                                             SingleEvalResultConstInt lhsResult, 
                                             SingleEvalResultConstInt rhsResult,
                                             EState estate, EvalMode mode=MODE_VALUE);
    list<SingleEvalResultConstInt> evalModOp(SgModOp* node,
                                             SingleEvalResultConstInt lhsResult, 
                                             SingleEvalResultConstInt rhsResult,
                                             EState estate, EvalMode mode=MODE_VALUE);
    list<SingleEvalResultConstInt> evalBitwiseAndOp(SgBitAndOp* node,
                                                    SingleEvalResultConstInt lhsResult, 
                                                    SingleEvalResultConstInt rhsResult,
                                                    EState estate, EvalMode mode=MODE_VALUE);
    list<SingleEvalResultConstInt> evalBitwiseOrOp(SgBitOrOp* node,
                                                   SingleEvalResultConstInt lhsResult, 
                                                   SingleEvalResultConstInt rhsResult,
                                                   EState estate, EvalMode mode=MODE_VALUE);
    list<SingleEvalResultConstInt> evalBitwiseXorOp(SgBitXorOp* node,
                                                    SingleEvalResultConstInt lhsResult, 
                                                    SingleEvalResultConstInt rhsResult,
                                                    EState estate, EvalMode mode=MODE_VALUE);
    
    list<SingleEvalResultConstInt> evalGreaterOrEqualOp(SgGreaterOrEqualOp* node,
                                                        SingleEvalResultConstInt lhsResult, 
                                                        SingleEvalResultConstInt rhsResult,
                                                        EState estate, EvalMode mode=MODE_VALUE);
    list<SingleEvalResultConstInt> evalGreaterThanOp(SgGreaterThanOp* node,
                                                     SingleEvalResultConstInt lhsResult, 
                                                     SingleEvalResultConstInt rhsResult,
                                                     EState estate, EvalMode mode=MODE_VALUE);
    list<SingleEvalResultConstInt> evalLessOrEqualOp(SgLessOrEqualOp* node,
                                                     SingleEvalResultConstInt lhsResult, 
                                                     SingleEvalResultConstInt rhsResult,
                                                     EState estate, EvalMode mode=MODE_VALUE);
    list<SingleEvalResultConstInt> evalLessThanOp(SgLessThanOp* node,
                                                  SingleEvalResultConstInt lhsResult, 
                                                  SingleEvalResultConstInt rhsResult,
                                                  EState estate, EvalMode mode=MODE_VALUE);
    list<SingleEvalResultConstInt> evalBitwiseShiftLeftOp(SgLshiftOp* node,
                                                          SingleEvalResultConstInt lhsResult, 
                                                          SingleEvalResultConstInt rhsResult,
                                                          EState estate, EvalMode mode=MODE_VALUE);
    list<SingleEvalResultConstInt> evalBitwiseShiftRightOp(SgRshiftOp* node,
                                                           SingleEvalResultConstInt lhsResult, 
                                                           SingleEvalResultConstInt rhsResult,
                                                           EState estate, EvalMode mode=MODE_VALUE);

    list<SingleEvalResultConstInt> evalArrayReferenceOp(SgPntrArrRefExp* node,
                                                        SingleEvalResultConstInt lhsResult, 
                                                        SingleEvalResultConstInt rhsResult,
                                                        EState estate, EvalMode mode=MODE_VALUE);
    list<SingleEvalResultConstInt> evalNotOp(SgNotOp* node, 
                                             SingleEvalResultConstInt operandResult, 
                                             EState estate, EvalMode mode=MODE_VALUE);
    list<SingleEvalResultConstInt> evalUnaryMinusOp(SgMinusOp* node, 
                                                    SingleEvalResultConstInt operandResult, 
                                                    EState estate, EvalMode mode=MODE_VALUE);
    list<SingleEvalResultConstInt> evalArrowOp(SgArrowExp* node,
                                               SingleEvalResultConstInt lhsResult, 
                                               SingleEvalResultConstInt rhsResult,
                                               EState estate, EvalMode mode=MODE_VALUE);
    list<SingleEvalResultConstInt> evalDotOp(SgDotExp* node,
                                             SingleEvalResultConstInt lhsResult, 
                                             SingleEvalResultConstInt rhsResult,
                                             EState estate, EvalMode mode=MODE_VALUE);
    list<SingleEvalResultConstInt> evalCastOp(SgCastExp* node, 
                                              SingleEvalResultConstInt operandResult, 
                                              EState estate, EvalMode mode=MODE_VALUE);
    list<SingleEvalResultConstInt> evalDereferenceOp(SgPointerDerefExp* node, 
                                                     SingleEvalResultConstInt operandResult, 
                                                     EState estate, EvalMode mode=MODE_VALUE);
    list<SingleEvalResultConstInt> semanticEvalDereferenceOp(SingleEvalResultConstInt operandResult, 
                                                             EState estate, EvalMode mode=MODE_VALUE);
    list<SingleEvalResultConstInt> evalAddressOfOp(SgAddressOfOp* node, 
                                                   SingleEvalResultConstInt operandResult, 
                                                   EState estate, EvalMode mode=MODE_VALUE);
    list<SingleEvalResultConstInt> evalPreComputationOp(EState estate, AbstractValue address, AbstractValue change);
    list<SingleEvalResultConstInt> evalPreIncrementOp(SgPlusPlusOp* node, 
						      SingleEvalResultConstInt operandResult, 
						      EState estate, EvalMode mode=MODE_VALUE);
    list<SingleEvalResultConstInt> evalPostIncrementOp(SgPlusPlusOp* node, 
						       SingleEvalResultConstInt operandResult, 
						       EState estate, EvalMode mode=MODE_VALUE);
    list<SingleEvalResultConstInt> evalPostComputationOp(EState estate, AbstractValue address, AbstractValue change);
    list<SingleEvalResultConstInt> evalPreDecrementOp(SgMinusMinusOp* node, 
						      SingleEvalResultConstInt operandResult, 
						      EState estate, EvalMode mode=MODE_VALUE);
    list<SingleEvalResultConstInt> evalPostDecrementOp(SgMinusMinusOp* node, 
						       SingleEvalResultConstInt operandResult, 
						       EState estate, EvalMode mode=MODE_VALUE);
    // dispatch function
    list<SingleEvalResultConstInt> evalMinusMinusOp(SgMinusMinusOp* node, 
                                                    SingleEvalResultConstInt operandResult, 
                                                    EState estate, EvalMode mode=MODE_VALUE);
    // dispatch function
    list<SingleEvalResultConstInt> evalPlusPlusOp(SgPlusPlusOp* node, 
                                                  SingleEvalResultConstInt operandResult, 
                                                  EState estate, EvalMode mode=MODE_VALUE);
    list<SingleEvalResultConstInt> evalBitwiseComplementOp(SgBitComplementOp* node, 
                                                           SingleEvalResultConstInt operandResult, 
                                                           EState estate, EvalMode mode=MODE_VALUE);
    
    // special case of sizeof operator (operates on types and types of expressions)
    list<SingleEvalResultConstInt> evalSizeofOp(SgSizeOfOp* node, 
                                                EState estate, EvalMode mode=MODE_VALUE);

    list<SingleEvalResultConstInt> evalLValuePntrArrRefExp(SgPntrArrRefExp* node, EState estate, EvalMode mode=MODE_VALUE);
    list<SingleEvalResultConstInt> evalLValueVarRefExp(SgVarRefExp* node, EState estate, EvalMode mode=MODE_VALUE);
    // handles DotExp and ArrowExp
    list<SingleEvalResultConstInt> evalLValueExp(SgNode* node, EState estate, EvalMode mode=MODE_VALUE);

    list<SingleEvalResultConstInt> evalRValueVarRefExp(SgVarRefExp* node, EState estate, EvalMode mode=MODE_VALUE);
    list<SingleEvalResultConstInt> evalValueExp(SgValueExp* node, EState estate);
    
    // supported system functions
    list<SingleEvalResultConstInt> evalFunctionCallMalloc(SgFunctionCallExp* funCall, EState estate);
    list<SingleEvalResultConstInt> evalFunctionCallFree(SgFunctionCallExp* funCall, EState estate);
    list<SingleEvalResultConstInt> evalFunctionCallMemCpy(SgFunctionCallExp* funCall, EState estate);
    list<SingleEvalResultConstInt> evalFunctionCallStrLen(SgFunctionCallExp* funCall, EState estate);

    // supported functions to be executed (interpreter mode)
    list<SingleEvalResultConstInt> execFunctionCallPrintf(SgFunctionCallExp* funCall, EState estate);

    // utilify functions
    int getMemoryRegionNumElements(CodeThorn::AbstractValue ptrToRegion);
    int getMemoryRegionElementSize(CodeThorn::AbstractValue);

  private:
    VariableIdMapping* _variableIdMapping=nullptr;
    ProgramLocationsReport _nullPointerDereferenceLocations;
    ProgramLocationsReport _outOfBoundsAccessLocations;
    ProgramLocationsReport _uninitializedAccessLocations;
   
    // Options
    bool _skipSelectedFunctionCalls=false;
    bool _skipArrayAccesses=false;
    bool _stdFunctionSemantics=true;
    bool _svCompFunctionSemantics=false;
    bool _ignoreUndefinedDereference=false;
    bool _ignoreFunctionPointers=false;
    Analyzer* _analyzer;
    bool _printDetectedViolations=false;
    enum InterpretationMode _interpretationMode=IM_ABSTRACT;
    bool _optionOutputWarnings=false;
  public:
    StructureAccessLookup structureAccessLookup;
  };
 
} // end of namespace CodeThorn

#endif
