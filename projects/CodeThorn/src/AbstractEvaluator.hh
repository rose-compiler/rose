#ifndef EVALUATOR_H
#define EVALUATOR_H

#include "AstProcessing.h"

class CppConstExprEvaluator;

class CPEvalResult {
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

template<typename EvalResult>
class Domain {
public:
  virtual EvalResult add(EvalResult& e1, EvalResult& e2)=0;
  virtual EvalResult sub(EvalResult& e1, EvalResult& e2)=0;
  virtual EvalResult mul(EvalResult& e1, EvalResult& e2)=0;
  virtual EvalResult div(EvalResult& e1, EvalResult& e2)=0;
  virtual EvalResult neg(EvalResult& e2)=0;
  virtual EvalResult intValue(EvalResult& e0)=0;
};

template<typename Lattice>
class CPDomain : public Domain<CPEvalResult> {
public:
  CPEvalResult add(CPEvalResult& e1, CPEvalResult& e2) {
    if(e1.isConst() && e2.isConst()) {
      return CPEvalResult(e1.constValue()+e2.constValue());
    } else {
      return CPEvalResult();
    }
  }
  CPEvalResult sub(CPEvalResult& e1, CPEvalResult& e2) {
    if(e1.isConst() && e2.isConst()) {
      return CPEvalResult(e1.constValue()-e2.constValue());
    } else {
      return CPEvalResult();
    }
  }
  CPEvalResult mul(CPEvalResult& e1, CPEvalResult& e2) {
    if(e1.isConst() && e2.isConst()) {
      return CPEvalResult(e1.constValue()*e2.constValue());
    } else {
      return CPEvalResult();
    }
  }
  CPEvalResult div(CPEvalResult& e1, CPEvalResult& e2) {
    if(e1.isConst() && e2.isConst()) {
      return CPEvalResult(e1.constValue()/e2.constValue());
    } else {
      return CPEvalResult();
    }
  }
  CPEvalResult mod(CPEvalResult& e1, CPEvalResult& e2) {
    if(e1.isConst() && e2.isConst()) {
      return CPEvalResult(e1.constValue()%e2.constValue());
    } else {
      return CPEvalResult();
    }
  }
  CPEvalResult neg(CPEvalResult& e2) {
    if(e2.isConst()) {
      return {CPEvalResult zero(0); return sub(zero,e2); }
    } else {
      return CPEvalResult();
    }
  }
  CPEvalResult intValue(int val) {
    return CPEvalResult(val);
  }
};

template<typename Domain<Lattice>, typename PropertyState >
class CppExprEvaluator : public AstBottomUpProcessing<Lattice> {
 public:
  virtual Lattice evaluateSynthesizedAttribute(SgNode* node, AstBottomUpProcessing<Lattice>::SynthesizedAttributesList clist) {
    switch(node->variantT()) {
    case V_SgIntVal: return domain->intValue(isSgIntVal(node)->get_value());
    case V_SgAddOp:  return domain->add(clist[0],clist[1]);
    case V_SgSubtractOp: return domain->sub(clist[0],clist[1]);
    case V_SgMultiplyOp: return domain->mul(clist[0],clist[1]);
    case V_SgDivideOp: return domain->div(clist[0],clist[1]);
    case V_SgModOp: return domain->mod(clist[0],clist[1]);
    case V_SgMinusOp: return domain->neg(clist[0]);
    case V_SgVarRefExp: {return propertyState->getVarValue(variableIdMapping->variableId(node));}
    default:
      return Lattice();
    }
    return Lattice();
  }
  void setDomain(Domain<Lattice>* domain) { this->domain=domain; }
  void setPropertyState(Domain<Lattice>* pstate) { this->propertyState=pstate; }
  void setVariableIdMapping(VariableIdMapping variableIdMapping) { this->variableIdMapping=variableIdMapping; }
private:
  Domain<Lattice>* domain;
  PropertyState* propertyState;
  VariableIdMapping* variableIdMapping;
};

template<AbstractLValue, AbstractRValue, PropertyState, Domain>
class AbstractEvaluator {
  void setDomain(Domain* domain) { _domain=domain; }
  virtual AbstractRValue evaluateRValueExpression(SgAddOp* node, PropertyState* pstate) { return defaultRValue(); }
  virtual AbstractLValue evaluateLValueExpression(SgVarRefExp* node, PropertyState* pstate) { return defaultLValue(); }
  virtual defautRValue() { return AbstractRValue(); }
  virtual defautLValue() { return AbstractLValue(); }
};

#endif
