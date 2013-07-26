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

VariableIdMapping::VariableIdSet DataDependenceVisualizer::useVars(SgNode* expr){
  return getAstAttributeVariableIdSet(expr,"use-variableid-set");
}

VariableIdMapping::VariableIdSet DataDependenceVisualizer::defVars(SgNode* expr){
  return getAstAttributeVariableIdSet(expr,"def-variableid-set");
}

Label DataDependenceVisualizer::getLabel(SgNode* stmt){
  return _labeler->getLabel(stmt);
}

SgNode* DataDependenceVisualizer::getNode(Label label){
  return _labeler->getNode(label);
}

void DataDependenceVisualizer::generateDot(SgNode* root, string fileName){
}

// private

VariableIdMapping::VariableIdSet DataDependenceVisualizer::getAstAttributeVariableIdSet(SgNode* expr,string attributeName){
  if(expr->attributeExists(attributeName)) {
	VariableIdSetAttribute* varsAttr=dynamic_cast<VariableIdSetAttribute*>(expr->getAttribute(attributeName));
	return varsAttr->getVariableIdSet();
  } else {
	VariableIdMapping::VariableIdSet empty;
	return empty;
  }
}
