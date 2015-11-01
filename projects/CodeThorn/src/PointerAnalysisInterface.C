#include "PointerAnalysisInterface.h"

SPRAY::PointerAnalysisInterface::PointerAnalysisInterface() {
}

SPRAY::PointerAnalysisInterface::~PointerAnalysisInterface() {
}

bool SPRAY::PointerAnalysisInterface::hasDereferenceOperation(SgExpression* exp) {
  RoseAst ast(exp);
  for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
    if(SgExpression* exp=isSgExpression(*i)) {
      if(isSgArrowExp(exp)
         ||isSgPointerDerefExp(exp)
         ||(isSgVarRefExp(exp)&&isSgReferenceType(exp->get_type()))
         ) {
        return true;
      }
    }
  }
  return false;
}

SPRAY::PointerAnalysisEmptyImplementation::PointerAnalysisEmptyImplementation(VariableIdMapping* vim) {
  _variableIdMapping=vim;
}

void
SPRAY::PointerAnalysisEmptyImplementation::initialize() {
  // nothing to be done
}

void 
SPRAY::PointerAnalysisEmptyImplementation::run() {
  ROSE_ASSERT(_variableIdMapping);
  // nothing to be done
}

SPRAY::VariableIdSet SPRAY::PointerAnalysisEmptyImplementation::getModByPointer() {
  ROSE_ASSERT(_variableIdMapping);
  return _variableIdMapping->getVariableIdSet();
}
