
#include "sage3basic.h"
#include "UDAstAttribute.h"

using namespace std;
using namespace CodeThorn;

UDAstAttribute::UDAstAttribute(RDAstAttribute* rdAttr, SgNode* nodeForUseVarQuery)
  :_rdAttr(rdAttr),
   _node(nodeForUseVarQuery)
{
}

/*! 
  * \author Markus Schordan
  * \date 2013.
 */
VariableIdSet UDAstAttribute::useVariables(VariableIdMapping& vidm) {
  // labels are associated with statements/exprstatements.
  SgNode* exprNode=_node;
  if(isSgExprStatement(exprNode))
    exprNode=SgNodeHelper::getExprStmtChild(exprNode);
  if(SgFunctionCallExp* callExp=isSgFunctionCallExp(exprNode)) {
    return AnalysisAbstractionLayer::useVariables(callExp->get_args(),vidm);
  }
  if(!isSgExpression(exprNode)&&!isSgInitializedName(exprNode)&&!isSgVariableDeclaration(exprNode)) {
    //cerr<<"HELLO:"<<exprNode->class_name()<<":"<<exprNode->unparseToString()<<endl;
    return VariableIdSet();
  }
  return AnalysisAbstractionLayer::useVariables(exprNode,vidm);
}

/*! 
  * \author Markus Schordan
  * \date 2013.
 */
LabelSet UDAstAttribute::definitionsOfVariable(VariableId var) {
  return _rdAttr->definitionsOfVariableId(var);
}

void UDAstAttribute::toStream(ostream& os, VariableIdMapping* vim) {
  // TODO: proper output of UD data
  os<<"@"<<_node<<"{#UseVars:"<<useVariables(*vim).size()<<"}"<<endl;
}
