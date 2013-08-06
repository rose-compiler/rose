// Author: Markus Schordan, 2013.

#include "rose.h"
#include "DataDependenceVisualizer.h"

class VariableIdSetAttribute;

// public

DataDependenceVisualizer::DataDependenceVisualizer(Labeler* labeler, VariableIdMapping* varIdMapping)
  : _labeler(labeler),
    _variableIdMapping(varIdMapping)
{
}

VariableIdSet DataDependenceVisualizer::useVars(SgNode* expr) {
  VariableIdSet set;
  // TODO
  return set;
}

VariableIdSet DataDependenceVisualizer::defVars(SgNode* expr) {
  VariableIdSet set;
  // TODO
  return set;
}

Label DataDependenceVisualizer::getLabel(SgNode* stmt) {
  return _labeler->getLabel(stmt);
}

SgNode* DataDependenceVisualizer::getNode(Label label) {
  return _labeler->getNode(label);
}

void DataDependenceVisualizer::generateDot(SgNode* root, string fileName) {
  // TODO
}

// private

UseDefInfoAttribute* DataDependenceVisualizer::getUseDefInfoAttribute(SgNode* expr,string attributeName){
  if(expr->attributeExists(attributeName)) {
    UseDefInfoAttribute* udAttr=dynamic_cast<UseDefInfoAttribute*>(expr->getAttribute(attributeName));
    return udAttr;
  } else {
    return 0;
  }
}
