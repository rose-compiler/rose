// Author: Markus Schordan, 2013.

#include "rose.h"
#include "DataDependenceVisualizer.h"

class VariableIdSetAttribute;

// public

DataDependenceVisualizer::DataDependenceVisualizer(Labeler* labeler, VariableIdMapping* varIdMapping, string useDefAttributeName)
  : _labeler(labeler),
    _variableIdMapping(varIdMapping),
	_useDefAttributeName(useDefAttributeName)
{
}

VariableIdSet DataDependenceVisualizer::useVars(SgNode* expr) {
  UseDefInfoAttribute* useDefAttribute=getUseDefInfoAttribute(expr,_useDefAttributeName);
  return useDefAttribute->useVariables(*_variableIdMapping);
}

LabelSet DataDependenceVisualizer::defLabels(SgNode* expr, VariableId useVar) {
  UseDefInfoAttribute* useDefAttribute=getUseDefInfoAttribute(expr,_useDefAttributeName);
  return useDefAttribute->definitionsOfVariable(useVar);
}

Label DataDependenceVisualizer::getLabel(SgNode* stmt) {
  return _labeler->getLabel(stmt);
}

SgNode* DataDependenceVisualizer::getNode(Label label) {
  return _labeler->getNode(label);
}

void DataDependenceVisualizer::generateDot(SgNode* root, string fileName) {
  std::ofstream myfile;
  myfile.open(fileName.c_str(),std::ios::out);
  myfile<<"digraph DataDependence {"<<endl;
  stringstream ss;
  long labelNum=_labeler->numberOfLabels();
  for(long i=0;i<labelNum;++i) {
	Label lab=i;
	SgNode* node=_labeler->getNode(i);
	VariableIdSet useVarSet=useVars(node);
	for(VariableIdSet::iterator i=useVarSet.begin();i!=useVarSet.end();++i) {
	  VariableId useVar=*i;
	  LabelSet defLabSet=defLabels(node,useVar);
	  for(LabelSet::iterator i=defLabSet.begin();i!=defLabSet.end();++i) {
		Label sourceNode=lab;
		Label targetNode=*i;
		VariableId edgeAnnotation=useVar;
		string edgeAnnotationString=_variableIdMapping->uniqueShortVariableName(useVar);
		myfile<<sourceNode<<" -> "<<targetNode<<"[label=\""<<edgeAnnotation.toString()<<"\"];"<<endl;
	  }
	}
  }
  myfile<<"}"<<endl;
  myfile.close();
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
