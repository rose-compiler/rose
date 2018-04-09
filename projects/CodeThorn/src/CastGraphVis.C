#include "sage3basic.h"
#include "CastGraphVis.h"
#include "CppStdUtilities.h"
#include "AstTerm.h"
#include "SgNodeHelper.h"
#include "AstProcessing.h"
#include "RoseAst.h"

using namespace std;
using namespace CppStdUtilities;

static CastGraph castGraph;
static NodeVertexMapping nodeVertexMapping;
static stringstream ss;

string nodeString(SgNode* node) {
  stringstream tempss;
  tempss<<"x"<<node;
  return tempss.str();
}

string nodeId(SgExpression* node) {
  if(SgVarRefExp* varRef=isSgVarRefExp(node)) {
    SgSymbol* varSym=SgNodeHelper::getSymbolOfVariable(varRef);
    return nodeString(varSym);
  } else {
    return nodeString(node);
  }
}

string dotString(string s) {
  return SgNodeHelper::doubleQuotedEscapedString(s);
}

void addNode(SgInitializedName* node) {
  ROSE_ASSERT(node);
  cout<<"TODO: SgInitializedName: "<<endl;
}

void addNode(SgExpression* node, SgType* type);
void addNode(SgExpression* node) {
  SgType* type=node->get_type();
  addNode(node,type);
}

string typeColorName(SgType* type) {
  string color;
  switch(type->variantT()) {
  case V_SgTypeFloat: color="red";break;
  case V_SgTypeDouble: color="yellow";break;
  case V_SgTypeLongDouble: color="green";break;
  default: color="white";
  }
  return color;
}

void addNode(SgExpression* node, SgType* type) {
  ROSE_ASSERT(node);
  ROSE_ASSERT(type);

  if(nodeVertexMapping.find(node)==nodeVertexMapping.end()) {
    VertexIterType v=castGraph.insertVertex(VertexData(node,type));
    nodeVertexMapping[node]=v;
  
    string color=typeColorName(type);
    string labelInfo=string("\\n")+"type:"+type->unparseToString();
    
    if(isSgUnaryOp(node)||isSgBinaryOp(node)||isSgConditionalExp(node)||isSgCallExpression(node)) {
      ss<<nodeId(node)<<"[label=\"op:"<<node->class_name()+labelInfo<<"\" fillcolor="<<color<<" style=filled];"<<endl;
    } else if(isSgVarRefExp(node)) {
      ss<<nodeId(node)<<"[label=\"var:"<<node->unparseToString()+labelInfo<<"\" fillcolor="<<color<<" style=filled];"<<endl;
    } else if(isSgValueExp(node)) {
      ss<<nodeId(node)<<"[label=\"val:"<<dotString(node->unparseToString())+labelInfo<<"\" fillcolor="<<color<<" style=filled];"<<endl;
    } else {
      ss<<nodeId(node)<<"[label=\"node:"<<node->class_name()+labelInfo<<"\" fillcolor="<<color<<" style=filled];"<<endl;
    }
  }
}

void addEdge(SgExpression* from, SgExpression* to) {
  ROSE_ASSERT(nodeVertexMapping.find(from)!=nodeVertexMapping.end());
  ROSE_ASSERT(nodeVertexMapping.find(to)!=nodeVertexMapping.end());
  castGraph.insertEdge(nodeVertexMapping[from],nodeVertexMapping[to]);

  if(from->unparseToString()=="FE_UPWARD") {
    cout<<"DEBUG:   "<<from->unparseToString()<<endl;
    cout<<"DEBUG:p :"<<from->get_parent()->unparseToString()<<endl;
    cout<<"DEBUG:pp:"<<from->get_parent()->get_parent()->unparseToString()<<endl;
  }
  ss<<nodeId(to)<<" -> "<<nodeId(from)<<"[dir=back];"<<endl;
}

bool generateTypeGraph(SgProject* root, string dotFileName) {
  RoseAst ast(root);
  ss<<"digraph G {"<<endl;
  for (RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
    SgExpression* currentExpNode=isSgExpression(*i);
    if(currentExpNode) {
      SgExpression* parentExpNode=isSgExpression((*i)->get_parent());
      if(parentExpNode) {
	if(isSgAssignOp(parentExpNode)) {
	  // redirect assignment edge
	  SgExpression* lhs=isSgExpression(SgNodeHelper::getLhs(parentExpNode));
	  //SgExpression* rhs=isSgExpression(SgNodeHelper::getRhs(parentExpNode));
	  if(currentExpNode==lhs) {
	    std::swap(currentExpNode,parentExpNode); // invert direction of edge rhs<->assignop
	  }
	}
	if(SgFunctionCallExp* funCall=isSgFunctionCallExp(currentExpNode)) {
	  // generate edges for pairs (actual parameter (, formal parameter (var decl)).
	  SgExpressionPtrList& funActualArgs=SgNodeHelper::getFunctionCallActualParameterList(funCall);
	  if(SgFunctionDefinition* funDef=SgNodeHelper::determineFunctionDefinition(funCall)) {
	    /* for each formal parameter: 
	       create edge from the root-node of the actual argument expression to the
	       formal parameter declaration node
	    */
	    SgInitializedNamePtrList& funFormalArgs=SgNodeHelper::getFunctionDefinitionFormalParameterList(funDef);
	    // number of actual args and formal args must match (TODO: default parameters)
	    if(funActualArgs.size()!=funFormalArgs.size()) {
	      cerr<<"Error: number of function arguments do not match number of formal parameters."<<endl;
	      cerr<<"Function call: "<<funCall->unparseToString()<<endl;
	      exit(1);
	    }
	    SgExpressionPtrList::iterator funActualArgIter=funActualArgs.begin();
	    SgInitializedNamePtrList::iterator funFormalArgIter=funFormalArgs.begin();
#if 1
	    while(funActualArgIter!=funActualArgs.end() && funFormalArgIter!=funFormalArgs.end()) {
	      addNode(*funActualArgIter);
	      addNode(*funFormalArgIter);
	      //addEdge(*funActualArgIter,*funFormalArgIter); // TODO
	      ++funActualArgIter;
	      ++funFormalArgIter; // ensure: node used here, must be same as declnode linked in body with
	    }
	    ROSE_ASSERT(funFormalArgIter==funFormalArgs.end()); // must hold since arg lists are of same length
#endif
	    
	  } else {
	    // find first declaration. introduce dummy vars, if no variables are provided and link to those
	    // TODO
	  }
	}
	addNode(currentExpNode);
	addNode(parentExpNode);
	addEdge(currentExpNode,parentExpNode);
      } else {
	addNode(currentExpNode);
      }
    }
  }
  ss<<"}"<<endl;
  return writeFile(dotFileName,ss.str());
}
