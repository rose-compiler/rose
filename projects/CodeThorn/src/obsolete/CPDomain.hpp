#ifndef CPDOMAIN_HPP
#define CPDOMAIN_HPP

#include "Domain.hpp"

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

#endif
