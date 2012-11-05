/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

#include "Labeler.h"
#include "SgNodeHelper.h"
#include "AstTerm.h"
#include <sstream>

using namespace CodeThorn;

Labeler::Labeler(SgNode* start) {
  createLabels(start);
}

// returns number of labels to be associated with node
int Labeler::isLabelRelevantNode(SgNode* node) {
  if(node==0) 
	return 0;

  /* special case: the incExpr in a SgForStatement is a raw SgExpression
   * hence, the root can be any unary or binary node. We therefore perform
   * a lookup in the AST to check whether we are inside a SgForStatement
   */
  if(SgNodeHelper::isForIncExpr(node))
	return 1;

  switch(node->variantT()) {
  case V_SgFunctionCallExp:
  case V_SgBasicBlock:
	return 2;
  case V_SgFunctionDefinition:
	return 2;
  case V_SgExprStatement:
	//special case of FunctionCall inside expression
	if(SgNodeHelper::Pattern::matchFunctionCall(node)) {
	  //cout << "DEBUG: Labeler: assigning 2 labels for SgFunctionCallExp"<<endl;
	  return 2;
	}
	else
	  return 1;
  case V_SgIfStmt:
  case V_SgWhileStmt:
  case V_SgDoWhileStmt:
  case V_SgForStatement:
	//  case V_SgForInitStatement: // TODO: investigate: we might not need this
  case V_SgBreakStmt:
  case V_SgVariableDeclaration:
  case V_SgLabelStatement:
  case V_SgFunctionDeclaration:
  case V_SgNullStatement:
  case V_SgReturnStmt:
	if(SgNodeHelper::Pattern::matchReturnStmtFunctionCallExp(node)) {
	  //cout << "DEBUG: Labeler: assigning 3 labels for SgReturnStmt(SgFunctionCallExp)"<<endl;
	  return 3;
	}
	else
	  return 1;
  default:
	return 0;
  }
}

void Labeler::createLabels(SgNode* node) {
  RoseAst ast(node);
  for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
	if(int num=isLabelRelevantNode(*i)) {
	  for(int j=0;j<num;j++) {
		labelNodeMapping.push_back(*i);
	  }
	}
   	if(isSgExprStatement(*i)||isSgReturnStmt(*i)||isSgVariableDeclaration(*i))
	  i.skipChildrenOnForward();
  }
  std::cout << "STATUS: Assigned "<<labelNodeMapping.size()<< " labels."<<std::endl;
}

string Labeler::labelToString(Label lab) {
  stringstream ss;
  ss<<lab;
  return ss.str();
}

/* this access function has O(n). This is OK as this function should only be used rarely, whereas
   the function getNode is used frequently (and has O(1)).
*/
Label Labeler::getLabel(SgNode* node) {
  //std::cout << "MappingSize:"<<labelNodeMapping.size()<<std::endl;
  for(int i=0;i<labelNodeMapping.size();++i) {
	if(labelNodeMapping[i]==node) {
	  return i;
	}
  }
  return Labeler::NO_LABEL;
}

long Labeler::numberOfLabels() {
  return labelNodeMapping.size();
}
Label Labeler::functionCallLabel(SgNode* node) {
  assert(SgNodeHelper::Pattern::matchFunctionCall(node));
  return getLabel(node);
}

Label Labeler::functionCallReturnLabel(SgNode* node) {
  assert(SgNodeHelper::Pattern::matchFunctionCall(node));
  // it its current implementation it is guaranteed that labels associated with the same node
  // are associated as an increasing sequence of labels
  Label lab=getLabel(node);
  if(lab==NO_LABEL) 
	return NO_LABEL;
  else
	return lab+1; 
}
Label Labeler::blockBeginLabel(SgNode* node) {
  assert(isSgBasicBlock(node));
  return getLabel(node);
}
Label Labeler::blockEndLabel(SgNode* node) {
  assert(isSgBasicBlock(node));
  Label lab=getLabel(node);
  if(lab==NO_LABEL) 
	return NO_LABEL;
  else
	return lab+1; 
}
Label Labeler::functionEntryLabel(SgNode* node) {
  assert(isSgFunctionDefinition(node));
  return getLabel(node);
}
Label Labeler::functionExitLabel(SgNode* node) {
  assert(isSgFunctionDefinition(node));
  Label lab=getLabel(node);
  if(lab==NO_LABEL) 
	return NO_LABEL;
  else
	return lab+1; 
}
bool Labeler::isFunctionEntryLabel(Label lab) {
  SgNode* node=getNode(lab);
  if(isSgFunctionDefinition(node))
	return functionEntryLabel(node)==lab;
  else
	return false;
}
bool Labeler::isFunctionExitLabel(Label lab) {
  SgNode* node=getNode(lab);
  if(isSgFunctionDefinition(node))
	return functionExitLabel(node)==lab;
  else
	return false;
}
bool Labeler::isBlockBeginLabel(Label lab) {
  SgNode* node=getNode(lab);
  if(isSgBasicBlock(node))
	return blockBeginLabel(node)==lab;
  else
	return false;
}
bool Labeler::isBlockEndLabel(Label lab) {
  SgNode* node=getNode(lab);
  if(isSgBasicBlock(node))
	return blockEndLabel(node)==lab;
  else
	return false;
}

bool Labeler::isFunctionCallLabel(Label lab) {
  //cout << "ISFCLAB("<<lab<<"): @";
  SgNode* node=getNode(lab);
  if(SgNodeHelper::Pattern::matchFunctionCall(node)) {
	//cout << "FCL="<<functionCallLabel(node) << " =?= " << lab<<endl;
	return functionCallLabel(node)==lab;
  } else {
	//cout << "NOT A FSgCallExpression. ("<<node->class_name()<<")"<<endl;
	return false;
  }
}

bool Labeler::isFunctionCallReturnLabel(Label lab) {
  SgNode* node=getNode(lab);
  if(SgNodeHelper::Pattern::matchFunctionCall(node))
	return functionCallReturnLabel(node)==lab;
  else
	return false;
}

LabelSet Labeler::getLabelSet(set<SgNode*>& nodeSet) {
  LabelSet lset;
  for(set<SgNode*>::iterator i=nodeSet.begin();i!=nodeSet.end();++i) {
	lset.insert(getLabel(*i));
  }
  return lset;
}

SgNode*	Labeler::getNode(Label label) {
  if(label>=labelNodeMapping.size() || label==Labeler::NO_LABEL)
	return 0; // we might want to replace this with throwing an exeception in future
  return labelNodeMapping[label];
}

std::string Labeler::toString() {
  std::stringstream ss;
  for(int i=0;i<labelNodeMapping.size();++i) {
	ss << i<< ":"<<labelNodeMapping[i]->sage_class_name()<<endl;
  }
  return ss.str();
}

bool Labeler::isStdOutLabel(Label label, VariableId* id) {
  if(SgVarRefExp* varRefExp=SgNodeHelper::Pattern::matchSingleVarPrintf(getNode(label))) {
	SgSymbol* sym=SgNodeHelper::getSymbolOfVariable(varRefExp);
	assert(sym);
	if(id)
	  *id=VariableId(sym);
	return true;
  }
  return false;
}

bool Labeler::isStdInLabel(Label label, VariableId* id) {
  if(SgVarRefExp* varRefExp=SgNodeHelper::Pattern::matchSingleVarScanf(getNode(label))) {
	SgSymbol* sym=SgNodeHelper::getSymbolOfVariable(varRefExp);
	assert(sym);
	if(id)
	  *id=VariableId(sym);
	return true;
  }
  return false;
}

bool Labeler::isStdErrLabel(Label label, VariableId* id) {
  if(SgVarRefExp* varRefExp=SgNodeHelper::Pattern::matchSingleVarFPrintf(getNode(label))) {
	SgSymbol* sym=SgNodeHelper::getSymbolOfVariable(varRefExp);
	assert(sym);
	if(id)
	  *id=VariableId(sym);
	return true;
  }
  return false;
}
