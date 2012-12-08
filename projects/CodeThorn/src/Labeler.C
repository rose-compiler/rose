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

#define CHECK_LABEL 0

Labeler::Labeler(SgNode* start, VariableIdMapping* variableIdMapping) {
  _variableIdMapping=variableIdMapping;
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

void Labeler::createLabels(SgNode* root) {
  RoseAst ast(root);
  for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
	if(int num=isLabelRelevantNode(*i)) {
	  if(SgNodeHelper::Pattern::matchFunctionCall(*i)) {
		if(SgNodeHelper::Pattern::matchReturnStmtFunctionCallExp(*i)) {
		  assert(num==3);
		  labelNodeMapping.push_back(LabelProperty(*i,LabelProperty::LABEL_FUNCTIONCALL,_variableIdMapping));
		  labelNodeMapping.push_back(LabelProperty(*i,LabelProperty::LABEL_FUNCTIONCALLRETURN,_variableIdMapping));
		  labelNodeMapping.push_back(LabelProperty(*i,_variableIdMapping)); // return-stmt-label
		} else {
		  assert(num==2);
		  labelNodeMapping.push_back(LabelProperty(*i,LabelProperty::LABEL_FUNCTIONCALL,_variableIdMapping));
		  labelNodeMapping.push_back(LabelProperty(*i,LabelProperty::LABEL_FUNCTIONCALLRETURN,_variableIdMapping));
		}
	  } else if(isSgFunctionDefinition(*i)) {
		assert(num==2);
		labelNodeMapping.push_back(LabelProperty(*i,LabelProperty::LABEL_FUNCTIONENTRY,_variableIdMapping));
		labelNodeMapping.push_back(LabelProperty(*i,LabelProperty::LABEL_FUNCTIONEXIT,_variableIdMapping));
	  } else if(isSgBasicBlock(*i)) {
		assert(num==2);
		labelNodeMapping.push_back(LabelProperty(*i,LabelProperty::LABEL_BLOCKBEGIN,_variableIdMapping));
		labelNodeMapping.push_back(LabelProperty(*i,LabelProperty::LABEL_BLOCKEND,_variableIdMapping));
	  } else {
		// all other cases
		for(int j=0;j<num;j++) {
		  labelNodeMapping.push_back(LabelProperty(*i,_variableIdMapping));
		}
	  }
	}
   	if(isSgExprStatement(*i)||isSgReturnStmt(*i)||isSgVariableDeclaration(*i))
	  i.skipChildrenOnForward();
  }
  std::cout << "STATUS: Assigned "<<labelNodeMapping.size()<< " labels."<<std::endl;
  //std::cout << "DEBUG: labelNodeMapping:\n"<<this->toString()<<std::endl;
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
  static long labnum=0;
  //cout << "INFO: getLabel ("<<labnum<<")"<<endl;
  labnum++;
  for(Label i=0;i<labelNodeMapping.size();++i) {
	if(labelNodeMapping[i].getNode()==node) {
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
#if CHECK_LABEL
  bool res=false;
  SgNode* node=getNode(lab);
  if(isSgFunctionDefinition(node))
	res=(functionEntryLabel(node)==lab);
  else
	res=false;
  assert(res==labelNodeMapping[lab].isFunctionEntryLabel());  
#endif
  return labelNodeMapping[lab].isFunctionEntryLabel();
}

bool Labeler::isFunctionExitLabel(Label lab) {
#if CHECK_LABEL
  bool res=false;
  SgNode* node=getNode(lab);
  if(isSgFunctionDefinition(node))
	res=(functionExitLabel(node)==lab);
  else
	res=false;
  assert(res==labelNodeMapping[lab].isFunctionExitLabel());  
#endif
  return labelNodeMapping[lab].isFunctionExitLabel();
}
bool Labeler::isBlockBeginLabel(Label lab) {
#if CHECK_LABEL
  bool res=false;
  SgNode* node=getNode(lab);
  if(isSgBasicBlock(node))
	res=(blockBeginLabel(node)==lab);
  else
	res=false;
  assert(res==labelNodeMapping[lab].isBlockBeginLabel());    
#endif
  return labelNodeMapping[lab].isBlockBeginLabel();
}

bool Labeler::isBlockEndLabel(Label lab) {
#if CHECK_LABEL
  bool res=false;
  SgNode* node=getNode(lab);
  if(isSgBasicBlock(node))
	res=(blockEndLabel(node)==lab);
  else
	res=false;
  assert(res==labelNodeMapping[lab].isBlockEndLabel());    
#endif
  return labelNodeMapping[lab].isBlockEndLabel();
}

bool Labeler::isFunctionCallLabel(Label lab) {
#if CHECK_LABEL
  //cout << "ISFCLAB("<<lab<<"): @";
  bool res=false;
  SgNode* node=getNode(lab);
  if(SgNodeHelper::Pattern::matchFunctionCall(node)) {
	//cout << "FCL="<<functionCallLabel(node) << " =?= " << lab<<endl;
	res=(functionCallLabel(node)==lab);
  } else {
	//cout << "NOT A FSgCallExpression. ("<<node->class_name()<<")"<<endl;
	res=false;
  }
  if(res!=labelNodeMapping[lab].isFunctionCallLabel()) {
	cerr << "Error: label:"<<lab<<",res:"<<res<<"lnm:"<<labelNodeMapping[lab].isFunctionCallLabel()<<endl;
	exit(1);
  }
  assert(res==labelNodeMapping[lab].isFunctionCallLabel());
#endif
  return labelNodeMapping[lab].isFunctionCallLabel();
}

bool Labeler::isFunctionCallReturnLabel(Label lab) {
#if CHECK_LABEL
  bool res=false;
  SgNode* node=getNode(lab);
  if(SgNodeHelper::Pattern::matchFunctionCall(node))
	res=(functionCallReturnLabel(node)==lab);
  else
	res=false;
  assert(res==labelNodeMapping[lab].isFunctionCallReturnLabel());
#endif
  return labelNodeMapping[lab].isFunctionCallReturnLabel();
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
  return labelNodeMapping[label].getNode();
}

std::string Labeler::toString() {
  std::stringstream ss;
  for(Label i=0;i<labelNodeMapping.size();++i) {
	ss << i<< ":"<<labelNodeMapping[i].toString()<<endl;
  }
  return ss.str();
}

bool Labeler::isStdOutLabel(Label label, VariableId* id) {
  bool res=false;
#if CHECK_LABEL
  if(SgVarRefExp* varRefExp=SgNodeHelper::Pattern::matchSingleVarPrintf(getNode(label))) {
	SgSymbol* sym=SgNodeHelper::getSymbolOfVariable(varRefExp);
	assert(sym);
	if(id) {
	  *id=VariableId(sym);
	  assert(*id==labelNodeMapping[label].getIOVarId());
	}
	res=true;
  }
  assert(res==labelNodeMapping[label].isStdOutLabel());
#endif
  res=labelNodeMapping[label].isStdOutLabel();
  if(res&&id)
	*id=labelNodeMapping[label].getIOVarId();
  return res;
}

bool Labeler::isStdInLabel(Label label, VariableId* id) {
  bool res=false;
#if CHECK_LABEL
  if(SgVarRefExp* varRefExp=SgNodeHelper::Pattern::matchSingleVarScanf(getNode(label))) {
	SgSymbol* sym=SgNodeHelper::getSymbolOfVariable(varRefExp);
	assert(sym);
	if(id) {
	  *id=VariableId(sym);
	  assert(*id==labelNodeMapping[label].getIOVarId());
	}
	res=true;
  }
  assert(res==labelNodeMapping[label].isStdInLabel());
#endif
  res=labelNodeMapping[label].isStdInLabel();
  if(res&&id)
	*id=labelNodeMapping[label].getIOVarId();
  return res;
}

bool Labeler::isStdErrLabel(Label label, VariableId* id) {
  bool res=false;
#if CHECK_LABEL
  if(SgVarRefExp* varRefExp=SgNodeHelper::Pattern::matchSingleVarFPrintf(getNode(label))) {
	SgSymbol* sym=SgNodeHelper::getSymbolOfVariable(varRefExp);
	assert(sym);
	if(id) {
	  *id=VariableId(sym);
	  assert(*id==labelNodeMapping[label].getIOVarId());
	}
	res=true;
  }
  assert(res==labelNodeMapping[label].isStdErrLabel());
#endif
  res=labelNodeMapping[label].isStdErrLabel();
  if(res&&id)
	*id=labelNodeMapping[label].getIOVarId();
  return res;
}
