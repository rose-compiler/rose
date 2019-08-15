#include "PointerAnalysisInterface.h"

CodeThorn::PointerAnalysisInterface::PointerAnalysisInterface() {
}

CodeThorn::PointerAnalysisInterface::~PointerAnalysisInterface() {
}

bool CodeThorn::PointerAnalysisInterface::hasDereferenceOperation(SgExpression* exp) {
  RoseAst ast(exp);
  for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
    if(SgExpression* exp=isSgExpression(*i)) {
      if(   isSgArrowExp(exp)
         || isSgArrowStarOp(exp)
         || isSgDotStarOp(exp)
         || isSgPointerDerefExp(exp)
         || isSgPntrArrRefExp(exp)
         || isSgFunctionCallExp(exp)
         ||(isSgVarRefExp(exp)&&isSgReferenceType(exp->get_type()))
         ) {
        return true;
      }
    }
  }
  return false;
}

bool CodeThorn::PointerAnalysisInterface::hasAddressOfOperation(SgExpression* exp) {
  RoseAst ast(exp);
  for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
    if(isSgAddressOfOp(*i)) {
      return true;
    }
  }
  return false;
}

CodeThorn::PointerAnalysisEmptyImplementation::PointerAnalysisEmptyImplementation(VariableIdMapping* vim) {
  _variableIdMapping=vim;
}

void
CodeThorn::PointerAnalysisEmptyImplementation::initialize() {
  // nothing to be done
}

void 
CodeThorn::PointerAnalysisEmptyImplementation::run() {
  ROSE_ASSERT(_variableIdMapping);
  // nothing to be done
}

CodeThorn::VariableIdSet CodeThorn::PointerAnalysisEmptyImplementation::getModByPointer() {
  ROSE_ASSERT(_variableIdMapping);
  return _variableIdMapping->getVariableIdSet();
}
