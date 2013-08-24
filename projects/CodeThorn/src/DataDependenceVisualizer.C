// Author: Markus Schordan, 2013.

#include "rose.h"
#include "DataDependenceVisualizer.h"
#include "addressTakenAnalysis/VariableIdUtils.h"

class VariableIdSetAttribute;

// public

DataDependenceVisualizer::DataDependenceVisualizer(Labeler* labeler, VariableIdMapping* varIdMapping, string useDefAttributeName)
  : _showSourceCode(true),
    _labeler(labeler),
    _variableIdMapping(varIdMapping),
	_useDefAttributeName(useDefAttributeName)
{
}

VariableIdSet DataDependenceVisualizer::useVars(SgNode* expr) {
  UDAstAttribute* useDefAttribute=getUDAstAttribute(expr,_useDefAttributeName);
  ROSE_ASSERT(useDefAttribute);
  return useDefAttribute->useVariables(*_variableIdMapping);
}

LabelSet DataDependenceVisualizer::defLabels(SgNode* expr, VariableId useVar) {
  UDAstAttribute* useDefAttribute=getUDAstAttribute(expr,_useDefAttributeName);
  ROSE_ASSERT(useDefAttribute);
  return useDefAttribute->definitionsOfVariable(useVar);
}

Label DataDependenceVisualizer::getLabel(SgNode* stmt) {
  return _labeler->getLabel(stmt);
}

SgNode* DataDependenceVisualizer::getNode(Label label) {
  return _labeler->getNode(label);
}

string DataDependenceVisualizer::nodeSourceCode(Label lab) {
  if(_labeler->isFunctionEntryLabel(lab))
	return "FunctionEntry";
  if(_labeler->isFunctionExitLabel(lab))
	return "FunctionExit";
  // all other cases
  return SgNodeHelper::doubleQuotedEscapedString(getNode(lab)->unparseToString());
}

void DataDependenceVisualizer::generateDefUseDotGraph(SgNode* root, string fileName) {
  _mode=DDVMODE_DEFUSE;
  generateDot(root,fileName);
}
void DataDependenceVisualizer::generateUseDefDotGraph(SgNode* root, string fileName) {
  _mode=DDVMODE_USEDEF;  
  generateDot(root,fileName);
}

void DataDependenceVisualizer::generateDot(SgNode* root, string fileName) {
  std::ofstream myfile;
  myfile.open(fileName.c_str(),std::ios::out);
  myfile<<"digraph DataDependence {"<<endl;
  myfile<<_dotFunctionClusters;
#if 1
  long labelNum=_labeler->numberOfLabels(); // may change this to Flow. (make it dependent, whether _flow exists)
#else
  long labelNum=_labeler->numberOfLabels();
#endif
  for(long i=0;i<labelNum;++i) {
	Label lab=i;
	SgNode* node=_labeler->getNode(i);
	if(!node)
	  continue;
	if(existsUDAstAttribute(node,_useDefAttributeName)) {
	  VariableIdSet useVarSet=useVars(node);
	  for(VariableIdSet::iterator i=useVarSet.begin();i!=useVarSet.end();++i) {
		VariableId useVar=*i;
		LabelSet defLabSet=defLabels(node,useVar);
		for(LabelSet::iterator i=defLabSet.begin();i!=defLabSet.end();++i) {
		  Label sourceNode=lab;
		  Label targetNode=*i;
		  VariableId edgeAnnotation=useVar;
		  string edgeAnnotationString=_variableIdMapping->uniqueShortVariableName(useVar);
		  switch(_mode) {
		  case DDVMODE_USEDEF:
			myfile<<sourceNode<<" -> "<<targetNode; break;
		  case DDVMODE_DEFUSE:
			myfile<<targetNode<<" -> "<<sourceNode; break;
		  default: 
			cerr<<"Error: unknown visualization mode."<<endl;
			exit(1);
		  }
		  if(_showSourceCode) {
			myfile<<"[label=\""<<edgeAnnotationString<<"\" color=darkgoldenrod4];";
		  }
		  myfile<<endl;
		  
		  if(_showSourceCode) {
			myfile<<sourceNode<<" [label=\""<<sourceNode<<":"<<nodeSourceCode(sourceNode)<<"\"];"<<endl;
			myfile<<targetNode<<" [label=\""<<targetNode<<":"<<nodeSourceCode(targetNode)<<"\"];"<<endl;
		  }
		  
		}
	  }
	}
  }
  if(_flow) {
	_flow->setDotOptionHeaderFooter(false);
	//_flow->setDotOptionDisplayLabel(true);
	//_flow->setDotOptionDisplayStmt(true);
	//_flow->setDotOptionFixedColor(true);
    myfile<<_flow->toDot(_labeler);
  }
  myfile<<"}"<<endl;
  myfile.close();
}

// private

UDAstAttribute* DataDependenceVisualizer::getUDAstAttribute(SgNode* expr,string attributeName){
  if(existsUDAstAttribute(expr,attributeName)) {
    UDAstAttribute* udAttr=dynamic_cast<UDAstAttribute*>(expr->getAttribute(attributeName));
    return udAttr;
  } else {
    return 0;
  }
}
bool DataDependenceVisualizer::existsUDAstAttribute(SgNode* expr,string attributeName){
  return expr->attributeExists(attributeName);
}
void DataDependenceVisualizer::generateDotFunctionClusters(LabelSetSet functionLabelSetSet) {
  string result;
  for(LabelSetSet::iterator i=functionLabelSetSet.begin();i!=functionLabelSetSet.end();++i) {
	result+="{ ";
	for(LabelSet::iterator j=(*i).begin();j!=(*i).end();++j) {
	  // xxx TODO
	}
	result+="}\n";
  }
  _dotFunctionClusters=result;
}
