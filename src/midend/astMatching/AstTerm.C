/*********************************
 * Author: Markus Schordan, 2012 *
 *********************************/

#include "sage3basic.h"

#include <iostream>

#include "AstTerm.h"
#include "SgNodeHelper.h"

using namespace std;

/*! 
 *  \author    Markus Schordan
 *  \date      2012
 */
std::string AstTerm::nodeTypeName(SgNode* node) {
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

std::string AstTerm::astTermToMultiLineString(SgNode* node,int tab, int pos) {
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

std::string AstTerm::astTermWithNullValuesAndTypesToString(SgNode* node) {
  return astTermWithNullValuesToString(node,true,true);
}

std::string AstTerm::astTermWithNullValuesToString(SgNode* node) {
  return astTermWithNullValuesToString(node,true,false);
}

std::string AstTerm::astTermWithNullValuesToString(SgNode* node, bool withNumbers) {
  return astTermWithNullValuesToString(node,withNumbers,false);
}

std::string AstTerm::astTermWithNullValuesToString(SgNode* node, bool withNumbers, bool withTypes) {
  std::unordered_set<SgNode*> visited; // empty set
  return astTermWithNullValuesToString(node,withNumbers,withTypes,visited);
}
  
std::string AstTerm::astTermWithNullValuesToString(SgNode* node, bool withNumbers, bool withTypes, std::unordered_set<SgNode*>& visited) {
  if(node==0)
    return "null";
  std::string s=nodeTypeName(node);
  std::string typeTerm;
  if(withTypes) {
    // add type of expression
    if(SgExpression* exp=isSgExpression(node)) {
      SgType* expType=exp->get_type();
      if(expType) {
        if(visited.find(expType)==visited.end()) {
          visited.insert(expType); // for detecting sharing and/or cycle in type info
          typeTerm+="type:"+astTermWithNullValuesToString(expType,withNumbers,withTypes,visited);
          if(SgArrayType* arrayType=isSgArrayType(expType)) {
            typeTerm+=",basetype:"+astTermWithNullValuesToString(arrayType->get_base_type(),withNumbers,withTypes,visited);
          }
        } else {
          typeTerm+="type:"+expType->class_name()+"[shared]";
        }
      } else {
        typeTerm+="type:null";
      }
    }
  }
  int arity=node->get_numberOfTraversalSuccessors();
  if(arity>0) {
    s+="(";
    if(withTypes) {
      if(typeTerm.size()>0)
        s+=typeTerm+",";
    }
    for(int i=0; i<arity;i++) {
      SgNode* child = node->get_traversalSuccessorByIndex(i);
      if(i!=0)
        s+=",";
      s+=astTermWithNullValuesToString(child,withNumbers,withTypes,visited);
    }
    s+=")";
  }
  // unparse number leaf nodes if requested
  if(arity==0 && withNumbers) {
    if(SgExpression* exp=isSgExpression(node)) {
      s+=":"+exp->unparseToString();
    }
  }
  return s;
}

std::string AstTerm::pointerExprToString(SgNode* node) {
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

std::string AstTerm::astTermWithoutNullValuesToDot(SgNode* root) {
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

std::string AstTerm::functionAstTermsWithNullValuesToDot(SgNode* root) {
  RoseAst ast(root);
  string fragments;
  for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
    if(isSgFunctionDefinition(*i)) {
      fragments+=astTermWithNullValuesToDotFragment(*i);
    }
  }
  return dotFragmentToDot(fragments);
}

std::string AstTerm::dotFragmentToDot(string fragment) {
  std::stringstream ss;
  ss << "digraph G {\n ordering=out;\n";
  ss << fragment;
  ss<<"}\n";
  return ss.str();
}
std::string AstTerm::astTermWithNullValuesToDot(SgNode* root) {
  std::stringstream ss;
  ss << "digraph G {\n ordering=out;\n";
  ss << astTermWithNullValuesToDotFragment(root);
  ss<<"}\n";
  return ss.str();
}

std::string AstTerm::astTermWithNullValuesToDotFragment(SgNode* root) {
  RoseAst ast(root);
  std::stringstream ss;
  for(RoseAst::iterator i=ast.begin().withNullValues();i!=ast.end();++i) {
    if(nodeTypeName(*i)=="null")
      ss << "\"" << i.parent()<<"_null" <<"\"";
    else
      ss << "\"" << *i <<"\"";
    ss << "[label=\"" << nodeTypeName(*i);

    if(*i && (*i)->attributeExists("info")) {
      AstAttribute* attr=(*i)->getAttribute("info");
      ss<<attr->toString();
      ss << "\""<<" style=filled color=lightblue ";
    } else {
      ss << "\"";
    }
    ss<< "];"<<std::endl;
    if(*i!=root) {
      if(nodeTypeName(*i)=="null")
        ss << "\""<<i.parent() << "\"" << " -> " << "\"" << i.parent()<<"_null" << "\""<<";" << std::endl;
      else
        ss << "\""<<i.parent() << "\"" << " -> " << "\"" << *i << "\""<<";" << std::endl; 
    }
  }
  return ss.str();
}

std::string AstTerm::astTermToDot(RoseAst::iterator start, RoseAst::iterator end) {
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
