
#include <iostream>
#include <fstream>
#include <list>
using namespace std;

#include "rose.h"
#include "cfgToDot.h"
//#include "VirtualCFGIterator.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define SgDefaultFile Sg_File_Info::generateDefaultFileInfoForTransformationNode()

void convertOMPSingle(SgProject* project);
SgProject* project;

SgFunctionDeclaration* fooDecl=NULL;

SgStatement* fooCallStmtCreate()
{
	SgFunctionSymbol* funcSymb = new SgFunctionSymbol(fooDecl);
	SgFunctionRefExp* funcRefExp = new SgFunctionRefExp(SgDefaultFile, funcSymb);
	funcSymb->set_parent(funcRefExp);
	SgExprListExp* args = new SgExprListExp(SgDefaultFile);
	SgFunctionCallExp* funcCall = new SgFunctionCallExp(SgDefaultFile, funcRefExp, args);
	funcRefExp->set_parent(funcCall);
	args->set_parent(funcCall);
	SgExprStatement* callStmt = new SgExprStatement(SgDefaultFile, funcCall);
	funcCall->set_parent(callStmt);
	
	return callStmt;
}

int main(int argc, char * argv[]) 
{	
	// Build the AST used by ROSE
	project = frontend(argc,argv);
	
	// find a declaration for foo()
	Rose_STL_Container<SgNode*> funcDecls = NodeQuery::querySubTree(project, V_SgFunctionDeclaration);	
	for(Rose_STL_Container<SgNode*>::iterator it = funcDecls.begin(); it!=funcDecls.end(); it++)
	{
		SgFunctionDeclaration* decl = isSgFunctionDeclaration(*it); ROSE_ASSERT(decl);
		if(decl->get_name().getString() == "foo")
		{
			fooDecl = decl;
			break;
		}
	}
	if(!fooDecl) { printf("ERROR: could not find declaration of function foo()!\n"); }

	convertOMPSingle(project);

	// Run internal consistancy tests on AST
	AstTests::runAllTests(project);	
	
	Rose_STL_Container<SgNode*> functions = NodeQuery::querySubTree(project, V_SgFunctionDefinition);
	for (Rose_STL_Container<SgNode*>::const_iterator i = functions.begin(); i != functions.end(); ++i) {
		SgFunctionDefinition* func = isSgFunctionDefinition(*i);
		ROSE_ASSERT(func);
	
		printf("func = %s\n", func->unparseToString().c_str());
		
		// output the CFG to a file
		ofstream fileCFG;
		fileCFG.open((func->get_declaration()->get_name().getString()+"_cfg.dot").c_str());
		cout << "    writing to file "<<(func->get_declaration()->get_name().getString()+"_cfg.dot")<<"\n";
		cfgToDot(fileCFG, func->get_declaration()->get_name(), func->cfgForBeginning());
		fileCFG.close();
		
		// iterate over all the nodes in the CFG
		/*for(VirtualCFG::iterator it(func->cfgForBeginning()); it!=VirtualCFG::iterator::end(); it++)
		{
			SgNode *n = (*it).getNode();
			printf("n = <%s | %s>\n", n->class_name().c_str(), n->unparseToString().c_str());
		}*/
	}

	return backend(project);	
}

void convertOMPSingle(SgProject* project)
{	
	// look through all the pragma declarations inside of function bodies
	Rose_STL_Container<SgNode*> functions = NodeQuery::querySubTree(project, V_SgFunctionDefinition);
	for (Rose_STL_Container<SgNode*>::const_iterator i = functions.begin(); i != functions.end(); ++i)
	{
		SgFunctionDefinition* func = isSgFunctionDefinition(*i);
		ROSE_ASSERT(func);
		
		SgBasicBlock* funcBody = func->get_body();
		
		/*Rose_STL_Container<SgNode*> pragmas = NodeQuery::querySubTree(funcBody, V_SgPragmaDeclaration);
		for (Rose_STL_Container<SgNode*>::const_iterator it = pragmas.begin(); it != pragmas.end(); ++it)
		{
			SgPragmaDeclaration* pragma = isSgPragmaDeclaration(*it);
			myStatementInsert(pragma, fooCallStmtCreate(), true, true);
		}*/
		
		Rose_STL_Container<SgNode*> returns = NodeQuery::querySubTree(funcBody, V_SgReturnStmt);
		for (Rose_STL_Container<SgNode*>::const_iterator it = returns.begin(); it != returns.end(); ++it)
		{
			SgReturnStmt* curRet = isSgReturnStmt(*it);
			myStatementInsert(curRet, fooCallStmtCreate(), true, true);
		}
		
		
	}
}
