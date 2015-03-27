/*********************************
 * Author: Markus Schordan, 2012 *
 *********************************/

#include "sage3basic.h"

#include <iostream>

#include "AstTerm.h"
#include "SgNodeHelper.h"

using namespace std;
using namespace SPRAY;

/*! 
 *  \author    Markus Schordan
 *  \date      2012
 */
std::string SPRAY::AstTerm::nodeTypeName(SgNode* node) {
  if(node==0) {
    return "null";
  } else {
    std::string tid=typeid(*node).name();
    int j=0;
    while(tid[j]>='0' && tid[j]<='9') j++;
    tid=tid.substr(j,tid.size()-j);
    return tid;
  }
}

std::string SPRAY::AstTerm::astTermToMultiLineString(SgNode* node,int tab, int pos) {
  std::string tabstring;
  for(int t=0;t<pos;t++) tabstring+=" ";

  if(node==0) 
    return tabstring+"null";

  std::string s=tabstring+nodeTypeName(node);
  
  // address debug output
  std::stringstream ss;
  ss<<node;
  //s+="@"+ss.str();

  int arity=node->get_numberOfTraversalSuccessors();
  if(arity>0) {
    s+="(\n";
    for(int i=0; i<arity;i++) {
      SgNode* child = node->get_traversalSuccessorByIndex(i);   
      if(i!=0) s+=",\n";
      s+=astTermToMultiLineString(child,tab,pos+tab);
    }
    s+="\n"+tabstring+")";
  }
  return s;
}

std::string SPRAY::AstTerm::astTermWithNullValuesToString(SgNode* node) {
  if(node==0)
    return "null";
  std::string s=nodeTypeName(node);
  int arity=node->get_numberOfTraversalSuccessors();
  if(arity>0) {
    s+="(";
    for(int i=0; i<arity;i++) {
      SgNode* child = node->get_traversalSuccessorByIndex(i);   
      if(i!=0) s+=",";
      s+=astTermWithNullValuesToString(child);
    }
    s+=")";
  }
  return s;
}

std::string SPRAY::AstTerm::pointerExprToString(SgNode* node) {
  // MS: TODO: type check is required to ensure the expression is indeed a pointer expression
  // e.g. for return 0 it produces "null" even if the integer 0 is returned.
#if 0
  // does not work anymore in new branch
  if(SgNodeHelper::isCond(node)) {
    return string("COND ")+pointerExprToString(SgNodeHelper::getFirstChild(node));
  }
#endif

  if(isSgExprListExp(node)) {
    return string("EXPRLIST ")+"["+pointerExprToString(SgNodeHelper::getFirstChild(node))+"]"; 
  }
  if(isSgDeleteExp(node)) {
    return string("DELETE ")+"["+pointerExprToString(SgNodeHelper::getFirstChild(node))+"]"; 
  }
  if(isSgReturnStmt(node)) {
    return string("RETURN ")+"["+pointerExprToString(SgNodeHelper::getFirstChild(node))+"]"; 
  }
  if(dynamic_cast<SgBinaryOp*>(node)) {
    string lhs=pointerExprToString(SgNodeHelper::getLhs(node));
    string rhs=pointerExprToString(SgNodeHelper::getRhs(node));
    string result="["+lhs+"]["+rhs+"]";
    if(isSgEqualityOp(node))
      return string("CMPEQ ")+result;
    else if(isSgNotEqualOp(node))
      return string("CMPNEQ ")+result;
    else if(isSgLessOrEqualOp(node))
      return string("CMPLEQ ")+result;
    else if(isSgLessThanOp(node))
      return string("CMPLT ")+result;
    else if(isSgGreaterOrEqualOp(node))
      return string("CMPGEQ ")+result;
    else if(isSgGreaterThanOp(node))
      return string("CMPGT ")+result;
  }

  if(node==0)
    return ""; // null
  if(isSgNewExp(node))
    return "\"new\"";
  if(SgIntVal* intVal=isSgIntVal(node)) {
    if(intVal->get_value()==0)
      return "\"null\"";
    else
      return "";
  }
  if(isSgReturnStmt(node)) {
    pointerExprToString(SgNodeHelper::getFirstChild(node));
  }
  if(SgVarRefExp* varRefExp=isSgVarRefExp(node))
    return "\""+SgNodeHelper::symbolToString(SgNodeHelper::getSymbolOfVariable(varRefExp))+"\" ";

  string s;
  if(SgVariableDeclaration* varDecl=isSgVariableDeclaration(node)) {
    SgInitializedName* initName=SgNodeHelper::getInitializedNameOfVariableDeclaration(varDecl);
    string lhs="\""+SgNodeHelper::symbolToString(SgNodeHelper::getSymbolOfInitializedName(initName))+"\"";
    
#if 1
    string rhs=pointerExprToString(SgNodeHelper::getFirstChild(initName));
#else
    // this fails in new branch (but should not?)
    assert(initName->get_initializer());
    SgExpression* initExp=SgNodeHelper::getInitializerExpressionOfVariableDeclaration(varDecl);
    string rhs=pointerExprToString(initExp);
#endif
    return string("DECLINIT ")+"["+lhs+"]["+rhs+"]";
  }

  int arity=node->get_numberOfTraversalSuccessors();
  if(arity>0) {
    if(isSgAssignOp(node)) {
      string lhs=pointerExprToString(SgNodeHelper::getLhs(node));
      string rhs=pointerExprToString(SgNodeHelper::getRhs(node));
      return string("ASSIGN ")+"["+lhs+"]["+rhs+"]";
    }
    for(int i=0; i<arity;i++) {
      SgNode* child = node->get_traversalSuccessorByIndex(i);   
      //if(i!=0) 
      //    s+=",";
      s+=pointerExprToString(child);
    }
  }
  return s;
}

std::string SPRAY::AstTerm::astTermWithoutNullValuesToDot(SgNode* root) {
  RoseAst ast(root);
  std::stringstream ss;
  ss << "digraph G {\n ordering=out;\n";
  for(RoseAst::iterator i=ast.begin().withoutNullValues();i!=ast.end();++i) {
    ss << "\"" << *i <<"\""<< "[label=\"" << nodeTypeName(*i)<< "\"];"<<std::endl;
    if(*i!=root) {
      ss << "\""<<i.parent() << "\"" << " -> " << "\"" << *i << "\""<<";" << std::endl; 
    }
  }
  ss<<"}\n";
  return ss.str();
}

std::string SPRAY::AstTerm::functionAstTermsWithNullValuesToDot(SgNode* root) {
  RoseAst ast(root);
  string fragments;
  for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
    if(isSgFunctionDefinition(*i)) {
      fragments+=astTermWithNullValuesToDotFragment(*i);
    }
  }
  return dotFragmentToDot(fragments);
}

std::string SPRAY::AstTerm::dotFragmentToDot(string fragment) {
  std::stringstream ss;
  ss << "digraph G {\n ordering=out;\n";
  ss << fragment;
  ss<<"}\n";
  return ss.str();
}
std::string SPRAY::AstTerm::astTermWithNullValuesToDot(SgNode* root) {
  std::stringstream ss;
  ss << "digraph G {\n ordering=out;\n";
  ss << astTermWithNullValuesToDotFragment(root);
  ss<<"}\n";
  return ss.str();
}

std::string SPRAY::AstTerm::astTermWithNullValuesToDotFragment(SgNode* root) {
  RoseAst ast(root);
  std::stringstream ss;
  for(RoseAst::iterator i=ast.begin().withNullValues();i!=ast.end();++i) {
    ss << "\"" << *i <<"\""
       << "[label=\"" << nodeTypeName(*i);
    if(*i && (*i)->attributeExists("info")) {
      AstAttribute* attr=(*i)->getAttribute("info");
      ss<<attr->toString();
      ss << "\""<<" style=filled color=lightblue ";
    } else {
      ss << "\"";
    }
    ss<< "];"<<std::endl;
    if(*i!=root) {
      ss << "\""<<i.parent() << "\"" << " -> " << "\"" << *i << "\""<<";" << std::endl; 
    }
  }
  return ss.str();
}

std::string SPRAY::AstTerm::astTermToDot(RoseAst::iterator start, RoseAst::iterator end) {
  std::stringstream ss;
  SgNode* root=*start;
  long int visitCnt=1;
  ss << "digraph G {\n";
  ss << "ordering=out;";
  std::string prevNode="";
  for(RoseAst::iterator i=start;i!=end;++i) {
    ss << "\"" << i.current_node_id() <<"\""<< "[label=\"" << visitCnt++ << ":";
    ss << nodeTypeName(*i);
    ss << "\"";
    if(*i==0)
      ss << ",shape=diamond";
    ss <<"];"<<std::endl;
    if(*i!=root) {
      ss << "\""<<i.parent_node_id() << "\"" << " -> " << "\"";
      ss << i.current_node_id();
      ss << "\""<<";" << std::endl; 
    }
    std::string currentNode=i.current_node_id();
    if(prevNode!="") {
      // add traversal edge
      ss << "\""<< prevNode << "\"" << " -> " << "\"" << currentNode << "\"" << "[color=\"red\", constraint=\"false\"];\n";      
    }
    prevNode=currentNode;

  }
  ss<<"}\n";
  return ss.str();
}
