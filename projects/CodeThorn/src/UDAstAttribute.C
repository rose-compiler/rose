
#include "sage3basic.h"
#include "UDAstAttribute.h"

UDAstAttribute::UDAstAttribute(RDAnalysisAstAttribute* rdAttr, SgNode* nodeForUseVarQuery)
  :_rdAttr(rdAttr),
   _node(nodeForUseVarQuery)
{
}

VariableIdSet UDAstAttribute::useVariables(VariableIdMapping& vidm) {
  // labels are associated with statements/exprstatements.
  SgNode* exprNode=_node;
  if(isSgExprStatement(exprNode))
	exprNode=SgNodeHelper::getExprStmtChild(exprNode);
  return AnalysisAbstractionLayer::useVariablesInExpression(exprNode,vidm);
}

LabelSet UDAstAttribute::definitionsOfVariable(VariableId var) {
  return _rdAttr->definitionsOfVariableId(var);
}

void UDAstAttribute::toStream(ostream& os, VariableIdMapping* vim) {
  // TODO: proper output of UD data
  os<<"@"<<_node<<"{#UseVars:"<<useVariables(*vim).size()<<"}"<<endl;
}
