/*********************************
 * Author: Markus Schordan, 2012 *
 *********************************/

#include "SgNodeHelper.h"
#include "limits.h"
#include "MyAst.h"
#include <list>

using namespace std;


SgExpression* SgNodeHelper::getInitializerExpressionOfVariableDeclaration(SgVariableDeclaration* decl) {
  SgInitializedName* initName=SgNodeHelper::getInitializedNameOfVariableDeclaration(decl);
  SgInitializer* initializer=initName->get_initializer();
  if(!initializer)
	throw "Error: AST structure failure: no initializer for initialized name.";
  if(SgAssignInitializer* assignInitializer=isSgAssignInitializer(initializer)) {
	SgExpression* expr=assignInitializer->get_operand_i();
	return expr;
  } else {
	return 0;
  }
}

// returns the SgInitializedName of a variable declaration or throws an exception
SgInitializedName* SgNodeHelper::getInitializedNameOfVariableDeclaration(SgVariableDeclaration* decl) {
  SgNode* initName0=decl->get_traversalSuccessorByIndex(1); // get-InitializedName
  if(initName0) {
	if(SgInitializedName* initName=isSgInitializedName(initName0)) {
	  return initName;
	} else {
	  throw "Error: AST structure failure: no variable found (@initializedName).";
	}
  } else {
	throw "Error: AST structure failure: no variable found.";
  }
}

string SgNodeHelper::symbolToString(SgSymbol* symbol) {
  SgName nameObject=symbol->get_name();
  string nameString=nameObject.getString();
  return nameString;
}

list<SgGlobal*> SgNodeHelper::listOfSgGlobal(SgProject* project) {
  list<SgGlobal*> globalList;
  SgFileList* fileList=project->get_fileList_ptr();
  int numFiles=project->numberOfFiles();
  for(int i=0;i<numFiles;++i) {
	SgFile* file=(*project)[i];
	if(SgSourceFile* sourceFile=isSgSourceFile(file)) {
	  SgGlobal* global=sourceFile->get_globalScope();
	  globalList.push_back(global);
	} else {
	  throw "Error: Ast structure failure: file is not a source file.";
	}
  }
  return globalList;
}

list<SgVariableDeclaration*> SgNodeHelper::listOfGlobalVars(SgProject* project) {
  list<SgVariableDeclaration*> globalVarDeclList;
  list<SgGlobal*> globalList=SgNodeHelper::listOfSgGlobal(project);
  for(list<SgGlobal*>::iterator i=globalList.begin();i!=globalList.end();++i) {
	list<SgVariableDeclaration*> varDeclList=SgNodeHelper::listOfGlobalVars(*i);
	globalVarDeclList.splice(globalVarDeclList.end(),varDeclList); // we are *moving* objects (not copying)
  }
  return globalVarDeclList;
}

list<SgVariableDeclaration*> SgNodeHelper::listOfGlobalVars(SgGlobal* global) {
  list<SgVariableDeclaration*> varDeclList;
  SgDeclarationStatementPtrList& declStmtList=global->get_declarations();
  for(SgDeclarationStatementPtrList::iterator i=declStmtList.begin();i!=declStmtList.end();++i) {
	if(SgVariableDeclaration* varDecl=isSgVariableDeclaration(*i)) {
	  if(!varDecl->isForward()) {
		varDeclList.push_back(varDecl);
	  }
	}
  }
  return varDeclList;
}

list<SgFunctionDefinition*> SgNodeHelper::listOfGlobalFunctionDefinitions(SgGlobal* global) {
  list<SgFunctionDefinition*> funDefList;
  return funDefList;
}

SgSymbol*
SgNodeHelper::getSymbolOfVariableDeclaration(SgVariableDeclaration* decl) {  
  SgInitializedName* initName=SgNodeHelper::getInitializedNameOfVariableDeclaration(decl);
  SgSymbol* initDeclVar=initName->search_for_symbol_from_symbol_table();
  return initDeclVar;
}

// returns a unique UniqueVariableSymbol (SgSymbol*) for a variale in a variable declaration (can be used as ID)
SgSymbol*
SgNodeHelper::getSymbolOfVariable(SgVarRefExp* varRefExp) {
  SgVariableSymbol* varSym=varRefExp->get_symbol();
  SgInitializedName* varInitName=varSym->get_declaration();
  SgSymbol* symbol=getSymbolOfInitializedName(varInitName);
  return symbol;
}

SgSymbol*
SgNodeHelper::getSymbolOfInitializedName(SgInitializedName* initName) {
 SgSymbol* initDeclVar=initName->search_for_symbol_from_symbol_table();
}

string SgNodeHelper::uniqueLongVariableName(SgNode* node) {
  SgSymbol* sym;
  bool found=false;
  if(SgVariableDeclaration* varDecl=isSgVariableDeclaration(node)) {
	sym=SgNodeHelper::getSymbolOfVariableDeclaration(varDecl);
	found=true;
  }
  if(SgVarRefExp* varRef=isSgVarRefExp(node)) {
	sym=SgNodeHelper::getSymbolOfVariable(varRef);
	found=true;
  }
  if(found) {
	string name=SgNodeHelper::symbolToString(sym);
	// we search from the SgSymbol (which is somewhere found in the AST). Even if it is in the symbol table
	// we will still find the right function!
	SgFunctionDefinition* funDef=SgNodeHelper::correspondingSgFunctionDefinition(sym);
	string funName; // intentionally empty when we are in global scope and no funDef is found.
	if(funDef)
	  funName=SgNodeHelper::getFunctionName(funDef);
	stringstream ss;
	ss << SgNodeHelper::scopeNestingLevel(sym);
	string scopeLevel=ss.str();
	string longName=string("$")+funName+"$"+scopeLevel+"$"+name;
	return longName;
  } else {
	throw "SgNodeHelper::uniqueLongVariableName: improper node operation.";
  }
}


SgFunctionDefinition* SgNodeHelper::correspondingSgFunctionDefinition(SgNode* node) {
  if(node==0)
	return 0;
  while(!isSgFunctionDefinition(node)) {
	node=SgNodeHelper::getParent(node);
	if(node==0)
	  return 0;
  }
  return isSgFunctionDefinition(node);
}

int SgNodeHelper::scopeNestingLevel(SgNode* node) {
  if(node==0)
	return 0;
  while(!isSgBasicBlock(node)) {
	node=SgNodeHelper::getParent(node);
	if(node==0)
	  return 0;
  }
  return 1+scopeNestingLevel(SgNodeHelper::getParent(node));
}

bool SgNodeHelper::isForwardFunctionDeclaration(SgNode* node) {
  if(SgFunctionDeclaration* funDecl=isSgFunctionDeclaration(node))
	if(SgFunctionDefinition* funDef=funDecl->get_definition())
	  return false;
  return true;
}

SgFunctionDefinition* SgNodeHelper::determineFunctionDefinition(SgFunctionCallExp* funCall) {
  SgFunctionDeclaration* funDecl=funCall->getAssociatedFunctionDeclaration();
  assert(funDecl);
  SgFunctionDefinition* funDef=funDecl->get_definition();
  if(funDef==0) {
	// forward declaration (we have not found the function definition yet)
	// 1) use parent pointers and search for Root node (likely to be SgProject node)
	SgNode* root=funDecl;
	SgNode* parent=0;
	while(!SgNodeHelper::isAstRoot(root)) {
	  parent=SgNodeHelper::getParent(root);
	  root=parent;
	}
	assert(root);
	// 2) search in AST for the right definition now
	MyAst ast(root);
	for(MyAst::iterator i=ast.begin();i!=ast.end();++i) {
	  if(SgFunctionDeclaration* funDecl2=isSgFunctionDeclaration(*i)) {
		if(!SgNodeHelper::isForwardFunctionDeclaration(funDecl2)) {
		  if(funDecl2->search_for_symbol_from_symbol_table()
			 ==funDecl->search_for_symbol_from_symbol_table()) {
			SgFunctionDefinition* fundef2=funDecl2->get_definition();
			assert(fundef2);
			return fundef2;
		  }
		}
	  }
	}
	return 0;
  }
  assert(funDef!=0);
}

SgExpressionPtrList& SgNodeHelper::getFunctionCallActualParameterList(SgNode* node) {
  if(!isSgFunctionCallExp(node))
	throw "SgNodeHelper::getFunctionCallActualParameterList: improper node operation.";
  return isSgExprListExp(node->get_traversalSuccessorByIndex(1))->get_expressions();
}

SgInitializedNamePtrList& SgNodeHelper::getFunctionDefinitionFormalParameterList(SgNode* node) {
  SgFunctionDefinition* funDef=isSgFunctionDefinition(node);
  if(!funDef)
	throw "SgNodeHelper::getFunctionDefinitionFormalParameterList: improper node operation.";
  SgFunctionDeclaration* funDecl=funDef->get_declaration();
  return funDecl->get_args();
}

SgFunctionCallExp* SgNodeHelper::Pattern::matchFunctionCall(SgNode* node) {
  if(SgFunctionCallExp* fce=SgNodeHelper::Pattern::matchReturnStmtFunctionCallExp(node))
	return fce;
  if(SgFunctionCallExp* fce=SgNodeHelper::Pattern::matchExprStmtFunctionCallExp(node))
	return fce;
  if(SgFunctionCallExp* fce=SgNodeHelper::Pattern::matchExprStmtAssignOpVarRefExpFunctionCallExp(node))
	return fce;
  return 0;
}
SgFunctionCallExp* SgNodeHelper::Pattern::matchExprStmtFunctionCallExp(SgNode* node) {
  if(SgNode* sexp=isSgExprStatement(node))
	if(SgFunctionCallExp* fcp=isSgFunctionCallExp(SgNodeHelper::getExprStmtChild(sexp)))
	  return fcp;
  return 0;
}
SgFunctionCallExp* SgNodeHelper::Pattern::matchReturnStmtFunctionCallExp(SgNode* node) {
  if(SgNode* rexp=isSgReturnStmt(node))
	if(SgFunctionCallExp* fcp=isSgFunctionCallExp(SgNodeHelper::getFirstChild(rexp)))
	  return fcp;
  return 0;
}

SgFunctionCallExp* SgNodeHelper::Pattern::matchExprStmtAssignOpVarRefExpFunctionCallExp(SgNode* node) {
  if(SgNode* sexp=isSgExprStatement(node))
	if(SgNode* assignOp=isSgAssignOp(SgNodeHelper::getExprStmtChild(sexp)))
	  if(isSgVarRefExp(SgNodeHelper::getLhs(assignOp)))
		if(SgFunctionCallExp* fcp=isSgFunctionCallExp(SgNodeHelper::getRhs(assignOp)))
		  return fcp;
  return 0;
}

set<SgNode*> SgNodeHelper::LoopRelevantBreakStmtNodes(SgNode* node) {
  set<SgNode*> breakNodes;
  MyAst ast(node);
  MyAst::iterator i=ast.begin();
  ++i; // go to first child
  while(i!=ast.end()) {
	if(isSgBreakStmt(*i))
	  breakNodes.insert(*i);
	if(isSgForStatement(*i)||isSgWhileStmt(*i)||isSgDoWhileStmt(*i)||isSgSwitchStatement(*i))
	  i.skipChildrenOnForward();
	++i;
  }
  return breakNodes;
}

bool SgNodeHelper::isAstRoot(SgNode* node) {
  return !node->get_parent();
}

SgNode* SgNodeHelper::getParent(SgNode* node) {
  SgNode* origNode=node;
  node=node->get_parent();
  if(node==0 && !isSgProject(origNode)) {
	throw "SgNodeHelper::getParent: improper node operation (@"+origNode->class_name()+")";
  }
  return node;
}

bool SgNodeHelper::isCond(SgNode* node) {
  SgNode* parent=node->get_parent();
  if(isSgIfStmt(parent)||isSgWhileStmt(parent)||isSgDoWhileStmt(parent)||isSgForStatement(parent)||isSgConditionalExp(parent))
	return SgNodeHelper::getCond(parent)==node && node!=0;
  else
	return false;
}

SgNode* SgNodeHelper::getCond(SgNode* node) {
  if(SgConditionalExp*  condexp=isSgConditionalExp(node)) {
	return condexp->get_conditional_exp();
  }
  if(SgIfStmt* ifstmt=isSgIfStmt(node)) {
	return ifstmt->get_conditional();
  }
  if(SgWhileStmt* whilestmt=isSgWhileStmt(node)) {
	return whilestmt->get_condition();
  }
  if(SgDoWhileStmt* dowhilestmt=isSgDoWhileStmt(node)) {
	return dowhilestmt->get_condition();
  }
  if(SgForStatement* forstmt=isSgForStatement(node)) {
	return forstmt->get_test();
  }
  throw "SgNodeHelper::getCond: improper node operation.";
}

SgNode* SgNodeHelper::getTrueBranch(SgNode* node) {
  if(SgIfStmt* ifstmt=isSgIfStmt(node)) {
	return ifstmt->get_true_body();
  }
  if(SgConditionalExp*  condexp=isSgConditionalExp(node)) {
	return condexp->get_true_exp();
  }
  throw "SgNodeHelper::getTrueBranch: improper node operation.";
}

SgNode* SgNodeHelper::getFalseBranch(SgNode* node) {
  if(SgIfStmt* ifstmt=isSgIfStmt(node)) {
	return ifstmt->get_false_body();
  }
  if(SgConditionalExp*  condexp=isSgConditionalExp(node)) {
	return condexp->get_false_exp();
  }
  throw "SgNodeHelper::getFalseBranch: improper node operation.";
}

SgNode* SgNodeHelper::getLoopBody(SgNode* node) {
  if(SgWhileStmt* whilestmt=isSgWhileStmt(node)) {
	return whilestmt->get_body();
  }
  if(SgDoWhileStmt* dowhilestmt=isSgDoWhileStmt(node)) {
	return dowhilestmt->get_body();
  }
  if(SgForStatement* forstmt=isSgForStatement(node)) {
	return forstmt->get_loop_body();
  }
  throw "SgNodeHelper::getLoopBody: improper node operation.";
}


SgNode* SgNodeHelper::getFirstOfBlock(SgNode* node) {
  if(SgBasicBlock* block=isSgBasicBlock(node)) {
	int len=SgNodeHelper::numChildren(block);
	if(len>0)
	  return node->get_traversalSuccessorByIndex(0);
  }
  // MS: note, the child could be 0 as well. Therefore we do not return 0, but throw an exception.
  throw "SgNodeHelper::getFirstBlock: improper node operation.";
}

SgNode* SgNodeHelper::getLastOfBlock(SgNode* node) {
  if(SgBasicBlock* block=isSgBasicBlock(node)) {
	int len=SgNodeHelper::numChildren(block);
	if(len>0)
	  return node->get_traversalSuccessorByIndex(len-1);
  }
  // MS: note, the child could be 0 as well. Therefore we do not return 0, but throw an exception.
  throw "SgNodeHelper::getLastOfBlock: improper node operation.";
}

string SgNodeHelper::doubleQuotedEscapedString(string s1) {
  string s2;
  for(int i=0;i<s1.size();++i) {
	if(s1[i]=='"')
	  s2+="\\\"";
	else
	  s2+=s1[i];
  }
  return s2;
}

string SgNodeHelper::nodeToString(SgNode* node) {
  if(isSgBasicBlock(node))
	return "{"; // MS: TODO: that's temporary and will be removed.
  if(isSgFunctionDefinition(node)||isSgFunctionDeclaration(node))
	return SgNodeHelper::getFunctionName(node);
  string s=node->unparseToString();
  return doubleQuotedEscapedString(s);
}

string SgNodeHelper::getFunctionName(SgNode* node) {
  SgFunctionDeclaration* fundecl;
  if(SgFunctionDefinition* fundef=isSgFunctionDefinition(node)) {
	node=fundef->get_declaration();
  }
  if(SgFunctionDeclaration* tmpfundecl=isSgFunctionDeclaration(node)) {
	fundecl=tmpfundecl;
  }
  if(fundecl) {
	SgName fname=fundecl->get_name();
	return fname.getString();
  }
  throw "SgNodeHelper::getFunctionName: improper node operation.";
}

SgNode* SgNodeHelper::getExprStmtChild(SgNode* node) {
  if(!isSgExprStatement(node)) {
	cerr << "Error: improper type in getExprStmtChild ("<<node->class_name()<<")"<<endl;
	exit(1);
  }
  return SgNodeHelper::getFirstChild(node);
}

SgNode* SgNodeHelper::getFirstChild(SgNode* node) {
  int len=SgNodeHelper::numChildren(node);
  if(len>0)
	return node->get_traversalSuccessorByIndex(0);
  else {
	// MS: note, the child could be 0 as well. Therefore we do not return 0, but throw an exception.
	throw "SgNodeHelper::getFirstChild: improper node operation.";
  }
}

SgNode* SgNodeHelper::getLhs(SgNode* node) {
  if(dynamic_cast<SgBinaryOp*>(node)) 
	return node->get_traversalSuccessorByIndex(0);
  else 
	throw "SgNodeHelper::getLhs: improper node operation.";
}

SgNode* SgNodeHelper::getRhs(SgNode* node) {
  if(dynamic_cast<SgBinaryOp*>(node)) 
	return node->get_traversalSuccessorByIndex(1);
  else 
	throw "SgNodeHelper::getRhs: improper node operation.";
}

int SgNodeHelper::numChildren(SgNode* node) {
  if(node) {
	size_t len=node->get_numberOfTraversalSuccessors();
	if(len<=(size_t)INT_MAX)
	  return (int)len;
	else
	  throw "SgNodeHelper::numChildren: number of children beyond max int.";
  } else {
	return 0; // if node==0 we return 0 as number of children
  }
}

