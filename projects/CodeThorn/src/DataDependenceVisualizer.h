#ifndef DATA_DEPENDENCE_VISUALIZER
#define DATA_DEPENDENCE_VISUALIZER

// Author: Markus Schordan, 2013.

#include "Labeler.h"
#include "VariableIdMapping.h"

#include <set>
using std::set;
#include <string>
using std::string;

#include "RDAnalysisAstAttribute.h"
#include "AnalysisAbstractionLayer.h"

using namespace CodeThorn;

typedef pair<VariableId,LabelSet> VariableIdLabelSetPair;
typedef set<VariableIdLabelSetPair> UseDefInfo;

class UseDefInfoAttribute : public AstSgNodeAttribute {
 public:
 UseDefInfoAttribute(RDAnalysisAstAttribute* rdAttr, SgNode* nodeForUseVarQuery)
   :_rdAttr(rdAttr),
	_node(nodeForUseVarQuery)
 {
  }
  VariableIdSet useVariables(VariableIdMapping& vidm) {
	// labels are associated with statements.
	SgNode* exprNode=_node;
	if(isSgExprStatement(exprNode))
	  exprNode=SgNodeHelper::getExprStmtChild(exprNode);
	return AnalysisAbstractionLayer::useVariablesInExpression(exprNode,vidm);
  }
  LabelSet definitionsOfVariable(VariableId var) {
	return _rdAttr->definitionsOfVariableId(var);
  }
  void toStream(ostream& os, VariableIdMapping* vim) {
	os<<"@"<<_node<<"{#UseVars:"<<useVariables(*vim).size()<<"}"<<endl;
  }
 private:
  RDAnalysisAstAttribute* _rdAttr;
  SgNode* _node;
};

class DataDependenceVisualizer {
 public:
  DataDependenceVisualizer(Labeler* labeler, VariableIdMapping* varIdMapping, string useDefAttributeName);
  VariableIdSet useVars(SgNode* expr);
  LabelSet defLabels(SgNode* expr, VariableId useVar);
  Label getLabel(SgNode* stmt);
  SgNode* getNode(Label lab);
  void generateDot(SgNode* root, string fileName);
  bool _showSourceCode;
 private:
  UseDefInfoAttribute* getUseDefInfoAttribute(SgNode* expr,string attributeName);
  Labeler* _labeler;
  VariableIdMapping* _variableIdMapping;
  string  _useDefAttributeName;
};

#endif
