#include "rose.h"
#include "patternRewrite.h"
#include "rewrite.h"
#include "CFGRewrite.h"
#include "variables.h"
#include <list>
#include <sstream>
#include <iostream>
#include <fstream>
using namespace std;

#include "common.h"
//#include "variables.h"
//#include "cfgUtils.h"
#include "ompUtils.h"

#define SgDefaultFile Sg_File_Info::generateDefaultFileInfoForTransformationNode()

// points to the declaration of omp_get_thread()
SgFunctionDeclaration *omp_get_num_threadsDecl=NULL;
SgFunctionDeclaration *omp_get_thread_numDecl=NULL;

void convertToOMPNormalForm(SgProject* project, SgNode* tree)
{
/*	SgFunctionDeclaration* genericForHeaderDecl = NULL;
	// look through all the function declarations
	Rose_STL_Container<SgNode*> functionDecls = NodeQuery::querySubTree(tree, V_SgFunctionDeclaration);
	for (Rose_STL_Container<SgNode*>::const_iterator i = functionDecls.begin(); i != functionDecls.end(); ++i)
	{
		SgFunctionDeclaration* decl = isSgFunctionDeclaration(*i);
		ROSE_ASSERT(decl);
		
		if(strcmp(decl->get_name().str(), "genericForHeader") == 0)
			genericForHeaderDecl = decl;
	}
	
	if(genericForHeaderDecl == NULL) { printf("Error: couldn't find declaration of genericForHeader()!\n"); }
	ROSE_ASSERT(genericForHeaderDecl);*/
	
	// omp_get_thread_numDecl hasn't been set yet, find its function and set it
	if(!omp_get_thread_numDecl || !omp_get_num_threadsDecl)
	{
		Rose_STL_Container<SgNode*> functions = NodeQuery::querySubTree(project, V_SgFunctionDeclaration);
		for (Rose_STL_Container<SgNode*>::const_iterator i = functions.begin(); i != functions.end(); ++i) {
			if(isSgFunctionDeclaration(*i)->get_name().getString() == "omp_get_num_threads")
				omp_get_num_threadsDecl = isSgFunctionDeclaration(*i);
			else if(isSgFunctionDeclaration(*i)->get_name().getString() == "omp_get_thread_num")
				omp_get_thread_numDecl = isSgFunctionDeclaration(*i);
		}
		if(!omp_get_num_threadsDecl) { fprintf(stderr, "Error: couldn't find declaration of omp_get_num_threads()!\n"); }
		if(!omp_get_thread_numDecl) { fprintf(stderr, "Error: couldn't find declaration of omp_get_thread_num()!\n"); }
	}
	
	// look through all the pragma declarations inside of function bodies
	Rose_STL_Container<SgNode*> functions = NodeQuery::querySubTree(tree, V_SgFunctionDefinition);
	for (Rose_STL_Container<SgNode*>::const_iterator i = functions.begin(); i != functions.end(); ++i)
	{
		SgFunctionDefinition* func = isSgFunctionDefinition(*i);
		ROSE_ASSERT(func);
		
		SgBasicBlock* funcBody = func->get_body();
		
		Rose_STL_Container<SgNode*> pragmas = NodeQuery::querySubTree(funcBody, V_SgPragmaDeclaration);
		for (Rose_STL_Container<SgNode*>::const_iterator it = pragmas.begin(); it != pragmas.end(); ++it)
		{
			SgPragmaDeclaration* pragma = isSgPragmaDeclaration(*it); ROSE_ASSERT(pragma);
			string pragmaStr(pragma->get_pragma()->get_pragma());
			string directive = "#pragma "+pragmaStr;
			int ompType = ompUtils::getOmpType(directive);
			
			// if this is is #pragma omp parallel
			if(ompType == ompUtils::omp_parallel)
			{
				// wrap the pragma's body with a SgBasicBlock
				wrapPragmaBody(pragma);
			}
			// if this is is #pragma omp parallel for
			else if(ompType == ompUtils::omp_parallel_for)
			{
				//printf("#pragma omp parallel for\n");
				ompParallelFor ompParFor(directive);
				
				// wrap the pragma's body with a SgBasicBlock
				SgStatement* oldPragmaBody;
				SgBasicBlock* newPragmaBody;
				wrapPragmaBody(pragma, &oldPragmaBody, &newPragmaBody);
				
				// break the #pragma omp parallel for into a #pragma omp parallel
				// and a #pragma omp for
				ompParallel ompPar(ompParFor);
				ompFor newFor(ompParFor.getOrdered(), false);
				
				char* pragmaText = new char[ompPar.str_nopragma().size()+1];
				strcpy(pragmaText, ompPar.str_nopragma().c_str());
				
				SgPragma* newParPragma = new SgPragma(pragmaText, SgDefaultFile);
				SgPragmaDeclaration* newParPragmaDecl = new SgPragmaDeclaration(SgDefaultFile, newParPragma);
				newParPragma->set_parent(newParPragmaDecl);
				//newParPragmaDecl->set_firstNondefiningDeclaration(newParPragmaDecl);
				newParPragmaDecl->set_definingDeclaration(newParPragmaDecl);
				myStatementInsert(pragma, newParPragmaDecl, true, true);
				
				pragmaText = new char[newFor.str_nopragma().size()+1];
				strcpy(pragmaText, newFor.str_nopragma().c_str());
				
				SgPragma* newForPragma = new SgPragma(pragmaText, SgDefaultFile);
				SgPragmaDeclaration* newForPragmaDecl = new SgPragmaDeclaration(SgDefaultFile, newForPragma);
				newForPragma->set_parent(newForPragmaDecl);
				//newForPragmaDecl->set_firstNondefiningDeclaration(newForPragmaDecl);
				newForPragmaDecl->set_definingDeclaration(newForPragmaDecl);
				myStatementInsert(oldPragmaBody, newForPragmaDecl, true, true);
				
				LowLevelRewrite::remove(pragma);
			}
			else if(ompType == ompUtils::omp_master)
			{
				SgStatement* masterBody = getPragmaBody(pragma);
				
				// remove the pragma
				LowLevelRewrite::remove(pragma);
					
				// create "omp_thread_num()"
				SgFunctionCallExp* threadNumCall;
				{
					SgFunctionSymbol* threadNumSymb = new SgFunctionSymbol(omp_get_thread_numDecl);
					SgFunctionRefExp* threadNumRef = new SgFunctionRefExp(SgDefaultFile, threadNumSymb);
					threadNumSymb->set_parent(threadNumRef);
					SgExprListExp* argList = new SgExprListExp(SgDefaultFile);
					threadNumCall = new SgFunctionCallExp(SgDefaultFile, threadNumRef, argList);
					argList->set_parent(threadNumCall);
					threadNumRef->set_parent(threadNumCall);
				}
				
				// arbitrary statement to be placed on the false branch of the if statement
				SgExprStatement* threadNumCallStmt;
				{
					SgFunctionSymbol* threadNumSymb = new SgFunctionSymbol(omp_get_thread_numDecl);
					SgFunctionRefExp* threadNumRef = new SgFunctionRefExp(SgDefaultFile, threadNumSymb);
					threadNumSymb->set_parent(threadNumRef);
					SgExprListExp* argList = new SgExprListExp(SgDefaultFile);
					SgFunctionCallExp* threadNumCall = new SgFunctionCallExp(SgDefaultFile, threadNumRef, argList);
					argList->set_parent(threadNumCall);
					threadNumRef->set_parent(threadNumCall);
					
					threadNumCallStmt = new SgExprStatement (SgDefaultFile, threadNumCall);
					threadNumCall->set_parent(threadNumCallStmt);
				}
				
				// create "omp_thread_num()==0"
				SgExprStatement* eqStmt;
				{
					SgIntVal* num0 = new SgIntVal(SgDefaultFile, 0);
					SgTypeInt* intType = new SgTypeInt();
					SgEqualityOp* eq = new SgEqualityOp(SgDefaultFile, threadNumCall, num0, intType);
					threadNumCall->set_parent(eq);
					num0->set_parent(eq);
					
					eqStmt = new SgExprStatement (SgDefaultFile, eq);
					eq->set_parent(eqStmt);
				}
				
				// create "if(omp_thread_num()==0){}"
				SgIfStmt* ifStmt;
				SgBasicBlock* trueBody;
				{
					trueBody = new SgBasicBlock(SgDefaultFile);
					SgBasicBlock* falseBody = new SgBasicBlock(SgDefaultFile);
					
					// fill the false branch
					falseBody->prepend_statement(threadNumCallStmt);
					threadNumCallStmt->set_parent(falseBody);
					
					ifStmt = new SgIfStmt(SgDefaultFile, eqStmt, trueBody, falseBody);
					trueBody->set_parent(ifStmt);
					falseBody->set_parent(ifStmt);
				}
				
				// place "if(omp_thread_num()==0){}" immediately before the single's body
				myStatementInsert(masterBody, ifStmt, true, true);
				
				// remove the single's body and place it into the true branch of the if statement
				LowLevelRewrite::remove(masterBody);
				trueBody->prepend_statement(masterBody);
				masterBody->set_parent(trueBody);
				
				/*// if this is a single
				if(ompType == ompUtils::omp_single)
				{
					ompSingle ompS(directive);
					
					// if this single is not a nowait
					if(!ompS.getNowait())
					{
						// add a #pragma omp barrier immediately after the if statement
						SgPragma* newBarPragma = new SgPragma("omp barrier", SgDefaultFile);
						SgPragmaDeclaration* newBarPragmaDecl = new SgPragmaDeclaration(SgDefaultFile, newBarPragma);
						newBarPragma->set_parent(newBarPragmaDecl);
						//newForPragmaDecl->set_firstNondefiningDeclaration(newForPragmaDecl);
						newBarPragmaDecl->set_definingDeclaration(newBarPragmaDecl);
						myStatementInsert(ifStmt, newBarPragmaDecl, true, false);
					}
				}*/
			}
			else if(ompType == ompUtils::omp_single)
			{
			   ompSingle ompSingle(directive);
				SgStatement* singleBody = getPragmaBody(pragma);
				ompReductionsSet forReductions;
				ompFor newFor(forReductions, ompSingle.privateVars, ompSingle.firstPrivateVars, 
				       ompSingle.copyPrivateVars, false, ompSingle.getNowait());
				
				// replace the single's pragma with the newFor pragma
				{
					char* pragmaText = new char[newFor.str_nopragma().size()+1];
					strcpy(pragmaText, newFor.str_nopragma().c_str());
					
					SgPragma* newForPragma = new SgPragma(pragmaText, SgDefaultFile);
					pragma->set_pragma(newForPragma);
					newForPragma->set_parent(pragma);
				}
				
				// replace the single's body with a single-iteration for loop with the same body
				{
					SgName name("i");
					
					// create the for's initialization expression
					SgForInitStatement* forInit;
					SgVariableDeclaration* varDecl;
					{
						SgType* inittype = new SgTypeInt();
						SgIntVal* num0 = new SgIntVal(SgDefaultFile, 0);
						SgAssignInitializer* init = new SgAssignInitializer(SgDefaultFile, num0, inittype);
						num0->set_parent(init);
						//inittype->set_parent(init);
						
						SgType* type = new SgTypeInt();
						varDecl = new SgVariableDeclaration(SgDefaultFile, name, type, init);
						//varDecl->set_firstNondefiningDeclaration(varDecl);
						varDecl->set_definingDeclaration(varDecl);
						//type->set_parent(forInit);
						//init->set_parent(varDecl);
						
						forInit = new SgForInitStatement(SgDefaultFile);
						forInit->append_init_stmt(varDecl);
						varDecl->set_parent(forInit);
					}
					
					// create the for's test expression
					SgExprStatement* forTest;
					{
						SgIntVal* num1 = new SgIntVal(SgDefaultFile, 1);
						SgType* type = new SgTypeInt();
						
						// create a reference to iterator variable
						SgVariableSymbol* sym = new SgVariableSymbol(varDecl->get_decl_item(name));
						SgVarRefExp *varRef = new SgVarRefExp(SgDefaultFile, sym);
						sym->set_parent(varRef);
						
						SgExpression* forTestExpr = new SgLessThanOp(SgDefaultFile, varRef, num1, type);
						num1->set_parent(forTestExpr);
						//type->set_parent(forTest);
						varRef->set_parent(forTestExpr);
						
						forTest = new SgExprStatement(SgDefaultFile, forTestExpr);
						forTestExpr->set_parent(forTest);
					}
					
					// create the for's increment expression
					SgExpression* forIncr;
					{
						SgType* type = new SgTypeInt();
						
						// create a reference to iterator variable
						SgVariableSymbol* sym = new SgVariableSymbol(varDecl->get_decl_item(name));
						SgVarRefExp *varRef = new SgVarRefExp(SgDefaultFile, sym);
						sym->set_parent(varRef);
						
						forIncr = new SgPlusPlusOp(SgDefaultFile, varRef, type);
						//type->set_parent(forIncr);
						varRef->set_parent(forIncr);
					}
				
					// create a new for loop with the an empty body
					SgBasicBlock* singleBodyBlock = new SgBasicBlock(SgDefaultFile);
					SgForStatement* forStmt = new SgForStatement(SgDefaultFile, forTest, forIncr, singleBodyBlock);
					forTest->set_parent(forStmt);
					forIncr->set_parent(forStmt);
					singleBodyBlock->set_parent(forStmt);
					forStmt->set_for_init_stmt(forInit);
					forInit->set_parent(forStmt);
					varDecl->get_decl_item(name)->set_scope(forStmt);
				
					// insert the new for loop immediately after the pragma
					myStatementInsert(pragma, forStmt, false, true);
					
					// remove the body of the single
					LowLevelRewrite::remove(singleBody);
					
					// place the single body inside the new for's body
					singleBodyBlock->append_statement(singleBody);
					singleBody->set_parent(singleBodyBlock);
				}
			}
		}
	}	
}
