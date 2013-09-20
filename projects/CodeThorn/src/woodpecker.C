// Author: Markus Schordan, 2013.

#include "rose.h"

#include "inliner.h"

#include <iostream>
#include "VariableIdMapping.h"
#include "Labeler.h"
#include "CFAnalyzer.h"
#include "RDLattice.h"
#include "DFAnalyzer.h"
#include "WorkList.h"
#include "RDAnalyzer.h"
#include "RDAstAttribute.h"
#include "AstAnnotator.h"
#include "DataDependenceVisualizer.h"
#include "Miscellaneous.h"
#include "ProgramStats.h"
#include "CommandLineOptions.h"

#include <list>

using namespace std;
using namespace CodeThorn;

bool trivialInline(SgFunctionCallExp* funCall) {
  /*
    0) check if it is a trivial function call (no return value, no params)
	1) find function to inline
	2) determine body of function to inline
	3) delete function call
	4) clone body of function to inline
	5) insert cloned body as block
  */
  bool success=false;
  string fname=SgNodeHelper::getFunctionName(funCall);
  SgFunctionDefinition* functionDef=isSgFunctionDefinition(SgNodeHelper::determineFunctionDefinition(funCall));
  if(!functionDef)
	return false;
  SgBasicBlock* functionBody=isSgBasicBlock(functionDef->get_body());
  if(!functionBody)
	return false;
  SgTreeCopy tc;
  SgBasicBlock* functionBodyClone=isSgBasicBlock(functionBody->copy(tc));
  // set current basic block as parent of body
  if(!functionBodyClone)
	return false;
  SgExprStatement* functionCallExprStmt=isSgExprStatement(funCall->get_parent());
  if(!functionCallExprStmt)
	return false;
  SgBasicBlock* functionCallBlock=isSgBasicBlock(functionCallExprStmt->get_parent());
  if(!functionCallBlock)
	return false;
  if(functionCallBlock->get_statements().size()>0) {
	SgStatement* oldStmt=functionCallExprStmt;
	SgStatement* newStmt=functionBodyClone;
	SageInterface::replaceStatement(oldStmt, newStmt,false);
	return true;
  }
  return false;
}

SgFunctionCallExp* isTrivialFunctionCall(SgNode* node) {
  if(SgFunctionCallExp* funCall=isSgFunctionCallExp(node)) {
	SgExpressionPtrList& args=SgNodeHelper::getFunctionCallActualParameterList(funCall);
	if(args.size()==0) {
	  if(SgFunctionDefinition* funDef=SgNodeHelper::determineFunctionDefinition(funCall)) {
		SgType* returnType=SgNodeHelper::getFunctionReturnType(funDef);
		if(isSgTypeVoid(returnType)) {
		  return funCall;
		}		 
	  }
	}
  }
  return 0;
}

list<SgFunctionCallExp*> trivialFunctionCalls(SgNode* node) {
  RoseAst ast(node);
  list<SgFunctionCallExp*> funCallList;
  for(RoseAst::iterator i=ast.begin();i!=ast.end();i++) {
	if(SgFunctionCallExp* funCall=isTrivialFunctionCall(*i)) {
	  funCallList.push_back(funCall);
	}
  }
  return funCallList;
}

size_t numberOfFunctions(SgNode* node) {
  RoseAst ast(node);
  size_t num=0;
  for(RoseAst::iterator i=ast.begin();i!=ast.end();i++) {
	if(isSgFunctionDefinition(*i))
	  num++;
  }
  return num;
}

size_t inlineFunctionCalls(list<SgFunctionCallExp*>& funCallList) {
  size_t num;
  for(list<SgFunctionCallExp*>::iterator i=funCallList.begin();i!=funCallList.end();i++) {
	SgFunctionCallExp* funCall=*i;
	cout<< "function call:"<<SgNodeHelper::nodeToString(*i)<<": ";
	bool success=trivialInline(funCall);
	if(success) {
	  cout<<"inlined."<<endl;
	  num++;
	}
	else
	  cout<<"not inlined."<<endl;
  }
  return num;
}

int main(int argc, char* argv[]) {
  cout << "INIT: Parsing and creating AST."<<endl;
  boolOptions.registerOption("semantic-fold",false); // temporary
  boolOptions.registerOption("post-semantic-fold",false); // temporary
  SgProject* root = frontend(argc,argv);
  //  AstTests::runAllTests(root);
#if 1
  // inline all functions
  list<SgFunctionCallExp*> funCallList=trivialFunctionCalls(root);
  cout<<"Number of trivial function calls (with existing function bodies): "<<funCallList.size()<<endl;

  std::string funtofind="main";
  RoseAst completeast(root);
  SgFunctionDefinition* mainFunctionRoot=completeast.findFunctionByName(funtofind);
  if(!mainFunctionRoot) {
	cerr<<"No main function available. "<<endl;
	exit(1);
  } else {
	cout << "Found main function."<<endl;
  }
  list<SgFunctionCallExp*> remainingFunCalls=trivialFunctionCalls(mainFunctionRoot);
  while(remainingFunCalls.size()>0) {
	size_t numFunCall=remainingFunCalls.size();
	cout<<"Remaing function calls in main function: "<<numFunCall<<endl;
	if(numFunCall>0)
	  inlineFunctionCalls(remainingFunCalls);
	remainingFunCalls=trivialFunctionCalls(mainFunctionRoot);
  }
#endif
  list<SgFunctionDefinition*> funDefs=SgNodeHelper::listOfFunctionDefinitions(root);
  for(list<SgFunctionDefinition*>::iterator i=funDefs.begin();i!=funDefs.end();i++) {
	string funName=SgNodeHelper::getFunctionName(*i);
	SgFunctionDeclaration* funDecl=(*i)->get_declaration();
	if(funName!="main") {
	  cout<<"Deleting function: "<<funName<<endl;
	  SgStatement* stmt=funDecl;
	  SageInterface::removeStatement (stmt, false);
	  //SageInterface::deleteAST(funDef);
	}
  }

#if 0
  rdAnalyzer->determineExtremalLabels(startFunRoot);
  rdAnalyzer->run();
#endif
  cout << "Remaining functions in program: "<<numberOfFunctions(root)<<endl;
  cout << "INFO: generating transformed source code."<<endl;
  root->unparse(0,0);
  return 0;
}
