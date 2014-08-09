#ifndef EVALUATOR_H
#define EVALUATOR_H

#include "AstProcessing.h"

class CppConstExprEvaluator;

class EvalResult {
 public:
 EvalResult():_constValue(0),_isConst(false){}
 EvalResult(int v):_constValue(v),_isConst(true){}
  bool isConst() { return _isConst; }
  int constValue() { ROSE_ASSERT(isConst()); return _constValue; }
  friend class CppConstExprEvaluator;
 private:
  int _constValue;
  bool _isConst;
};

class CppConstExprEvaluator : public AstBottomUpProcessing<EvalResult> {
 public:
  EvalResult add(EvalResult& e1, EvalResult& e2) {
    if(e1.isConst() && e2.isConst()) {
      return EvalResult(e1.constValue()+e2.constValue());
    } else {
      return EvalResult();
    }
  }
  EvalResult sub(EvalResult& e1, EvalResult& e2) {
    if(e1.isConst() && e2.isConst()) {
      return EvalResult(e1.constValue()-e2.constValue());
    } else {
      return EvalResult();
    }
  }
  EvalResult mul(EvalResult& e1, EvalResult& e2) {
    if(e1.isConst() && e2.isConst()) {
      return EvalResult(e1.constValue()*e2.constValue());
    } else {
      return EvalResult();
    }
  }
  EvalResult div(EvalResult& e1, EvalResult& e2) {
    if(e1.isConst() && e2.isConst()) {
      return EvalResult(e1.constValue()/e2.constValue());
    } else {
      return EvalResult();
    }
  }
  EvalResult mod(EvalResult& e1, EvalResult& e2) {
    if(e1.isConst() && e2.isConst()) {
      return EvalResult(e1.constValue()%e2.constValue());
    } else {
      return EvalResult();
    }
  }

  virtual EvalResult evaluateSynthesizedAttribute(SgNode* node, AstBottomUpProcessing<EvalResult>::SynthesizedAttributesList clist) {
    switch(node->variantT()) {
    case V_SgIntVal: return EvalResult(isSgIntVal(node)->get_value());
    case V_SgAddOp:  return add(clist[0],clist[1]);
    case V_SgSubtractOp: return sub(clist[0],clist[1]);
    case V_SgMultiplyOp: return mul(clist[0],clist[1]);
    case V_SgDivideOp: return div(clist[0],clist[1]);
    case V_SgModOp: return mod(clist[0],clist[1]);
    case V_SgMinusOp: {EvalResult v(0); return sub(v,clist[0]); }
    case V_SgVarRefExp: 
    default:
      return EvalResult();
    }
    return EvalResult();
  }
 
};

#endif
