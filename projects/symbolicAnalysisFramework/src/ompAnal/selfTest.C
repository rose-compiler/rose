#include <iostream>

#include "ompUtils.h"
#include "ompNormalForm.h"
#include "cfgUtils.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define SgDefaultFile Sg_File_Info::generateDefaultFileInfoForTransformationNode()

void testOMPForSensitiveInsertion();
void testParsing();

int numFails=0;
SgProject* project;
SgFunctionDeclaration* fooDecl=NULL;

SgFunctionCallExp* fooCallCreate()
{
	SgFunctionSymbol* funcSymb = new SgFunctionSymbol(fooDecl);
	SgFunctionRefExp* funcRefExp = new SgFunctionRefExp(SgDefaultFile, funcSymb);
	funcSymb->set_parent(funcRefExp);
	SgExprListExp* args = new SgExprListExp(SgDefaultFile);
	SgFunctionCallExp* funcCall = new SgFunctionCallExp(SgDefaultFile, funcRefExp, args);
	funcRefExp->set_parent(funcCall);
	args->set_parent(funcCall);
	
	return funcCall;
}

SgStatement* fooCallStmtCreate()
{
	SgFunctionCallExp* funcCall = fooCallCreate();
	SgExprStatement* callStmt = new SgExprStatement(SgDefaultFile, funcCall);
	funcCall->set_parent(callStmt);
	return callStmt;
}



int main(int argc, char * argv[]) 
{	
	// Build the AST used by ROSE
	project = frontend(argc,argv);
	
	/*convertToOMPNormalForm(project, project);
	
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
	}
	
	backend(project);*/
	
	//generatePDF ( *project );
	
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
	if(!fooDecl) { printf("ERROR: could not find declaration of function foo()!\n"); numFails++; }
	
	testParsing();
		
	convertToOMPNormalForm(project, project);
	
	testOMPForSensitiveInsertion();
	
	AstTests::runAllTests(project);
	
	insertTopBottomOmpDirectives(project, ompUtils::omp_critical, true, &fooCallStmtCreate);
	insertTopBottomOmpDirectives(project, ompUtils::omp_single, false, &fooCallStmtCreate);
	
	// Run internal consistancy tests on AST
	//
	
	// Generate the CFGs of all the functions to ensure that all CFG data is good
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
	}
	
	backend(project);
	system("diff rose_test_example.c test_example.valid_rose_output.c > selfTest.out");
	struct stat file;
   stat("selfTest.out",&file);
	if(file.st_size!=0)
	{
		printf("Error: found differences between rose_test_example.c and the canonical test_example.valid_rose_output.c! Details in selfTest.out.\n");
		numFails++;
	}
	
	if(numFails==0) 
		cout << "PASSED\n";
	else
		cout << "FAILED!\n";
}

void testParsing()
{
	string par1 = " #pragma 	omp parallel reduction(+: a, b, c, d, e) private(q, 	  k, greg)  lastprivate(	roger, 	  k, raga_k_123) default ( none)";
	string par1_correct = "#pragma omp parallel reduction(+:a, b, c, d, e) private(q, k, greg) lastprivate(roger, k, raga_k_123) default(none)";
	string par2 = " #pragma 	omp parallel reduction(||: a, b, c, d, e) reduction(/: a, b, c, d, e) firstprivate(q, 	  k, greg)  copyin(	roger, 	  k, raga_k_123) default ( shared)";
	string par2_correct = "#pragma omp parallel reduction(||:a, b, c, d, e) firstprivate(q, k, greg) copyin(roger, k, raga_k_123) default(shared)";
	string par3 = " #pragma 	omp parallel reduction(&&: a, b, c, d, e) reduction(&&: a) reduction(*: a1, b2, c3) firstprivate(q, 	  k, greg)  blah(	roger, 	  k, raga_k_123)";
	string par3_correct = "#pragma omp parallel reduction(&&:a, b, c, d, e) reduction(&&:a) reduction(*:a1, b2, c3) firstprivate(q, k, greg) ";
	string par4 = " #pragma 	omp parallel firstprivate(q, 	  k, greg)  firstprivate(	roger, 	  k, raga_k_123)";
	string par4_correct = "#pragma omp parallel firstprivate(q, k, greg, roger, k, raga_k_123) ";
		
	if(ompUtils::getOmpType(par1) != ompUtils::omp_parallel) { cout << "Error: par1 is supposed to be a "<<ompUtils::ompTypeStr(ompUtils::omp_parallel)<<" but is instead "<<ompUtils::ompTypeStr(ompUtils::getOmpType(par1))<<"!\n"; numFails++; }
	if(ompUtils::getOmpType(par2) != ompUtils::omp_parallel) { cout << "Error: par2 is supposed to be a "<<ompUtils::ompTypeStr(ompUtils::omp_parallel)<<" but is instead "<<ompUtils::ompTypeStr(ompUtils::getOmpType(par2))<<"!\n"; numFails++; }
	if(ompUtils::getOmpType(par3) != ompUtils::omp_parallel) { cout << "Error: par3 is supposed to be a "<<ompUtils::ompTypeStr(ompUtils::omp_parallel)<<" but is instead "<<ompUtils::ompTypeStr(ompUtils::getOmpType(par3))<<"!\n"; numFails++; }
	if(ompUtils::getOmpType(par4) != ompUtils::omp_parallel) { cout << "Error: par4 is supposed to be a "<<ompUtils::ompTypeStr(ompUtils::omp_parallel)<<" but is instead "<<ompUtils::ompTypeStr(ompUtils::getOmpType(par4))<<"!\n"; numFails++; }
		
	ompParallel ompPar1(par1);
	//cout << par1 << "\n"; cout << ompPar1.str() << "\n";
	if(par1_correct != ompPar1.str()) { cout << "ERROR parsing par1! \n\tShould be: \""<<par1_correct<<"\" but\n\tactually got: \""<<ompPar1.str()<<"\".\n"; numFails++; }
	ompParallel ompPar2(par2);
	//cout << par2 << "\n"; cout << ompPar2.str() << "\n";
	if(par2_correct != ompPar2.str()) { cout << "ERROR parsing par2! \n\tShould be: \""<<par2_correct<<"\" but\n\tactually got: \""<<ompPar2.str()<<"\".\n"; numFails++; }
	ompParallel ompPar3(par3);
	//cout << par3 << "\n"; cout << ompPar3.str() << "\n";
	if(par3_correct != ompPar3.str()) { cout << "ERROR parsing par3! \n\tShould be: \""<<par3_correct<<"\" but\n\tactually got: \""<<ompPar3.str()<<"\".\n"; numFails++; }
	ompParallel ompPar4(par4);
	//cout << par4 << "\n"; cout << ompPar4.str() << "\n";
	if(par4_correct != ompPar4.str()) { cout << "ERROR parsing par4! \n\tShould be: \""<<par4_correct<<"\" but\n\tactually got: \""<<ompPar4.str()<<"\".\n"; numFails++; }
	
	string parfor1 = " #pragma 	omp parallel       for reduction(+: a, b, c, d, e) private(q, 	  k, greg)  lastprivate(	roger, 	  k, raga_k_123) default ( none) ordered";
	string parfor1_correct = "#pragma omp parallel for reduction(+:a, b, c, d, e) private(q, k, greg) lastprivate(roger, k, raga_k_123) ordered default(none)";
	string parfor2 = " #pragma 	omp parallel 	for reduction(||: a, b, c, d, e) reduction(/: a, b, c, d, e) ordered		firstprivate(q, 	  k, greg)  nowait	copyin(	roger, 	  k, raga_k_123) default ( shared)";
	string parfor2_correct = "#pragma omp parallel for reduction(||:a, b, c, d, e) firstprivate(q, k, greg) copyin(roger, k, raga_k_123) ordered default(shared)";
	string parfor3 = " #pragma 	omp parallel for nowait reduction(&&: a, b, c, d, e) reduction(&&: a) reduction(*: a1, b2, c3) firstprivate(q, 	  k, greg)  blah(	roger, 	  k, raga_k_123)";
	string parfor3_correct = "#pragma omp parallel for reduction(&&:a, b, c, d, e) reduction(&&:a) reduction(*:a1, b2, c3) firstprivate(q, k, greg) ";
	string parfor4 = " #pragma 	omp parallel for firstprivate(q, 	  k, greg)  firstprivate(	roger, 	  k, raga_k_123)";
	string parfor4_correct = "#pragma omp parallel for firstprivate(q, k, greg, roger, k, raga_k_123) ";
	
	if(ompUtils::getOmpType(parfor1) != ompUtils::omp_parallel_for) { cout << "Error: parfor1 is supposed to be a "<<ompUtils::ompTypeStr(ompUtils::omp_parallel_for)<<" but is instead "<<ompUtils::ompTypeStr(ompUtils::getOmpType(parfor1))<<"!\n"; numFails++; }
	if(ompUtils::getOmpType(parfor2) != ompUtils::omp_parallel_for) { cout << "Error: parfor2 is supposed to be a "<<ompUtils::ompTypeStr(ompUtils::omp_parallel_for)<<" but is instead "<<ompUtils::ompTypeStr(ompUtils::getOmpType(parfor2))<<"!\n"; numFails++; }
	if(ompUtils::getOmpType(parfor3) != ompUtils::omp_parallel_for) { cout << "Error: parfor3 is supposed to be a "<<ompUtils::ompTypeStr(ompUtils::omp_parallel_for)<<" but is instead "<<ompUtils::ompTypeStr(ompUtils::getOmpType(parfor3))<<"!\n"; numFails++; }
	if(ompUtils::getOmpType(parfor4) != ompUtils::omp_parallel_for) { cout << "Error: parfor4 is supposed to be a "<<ompUtils::ompTypeStr(ompUtils::omp_parallel_for)<<" but is instead "<<ompUtils::ompTypeStr(ompUtils::getOmpType(parfor4))<<"!\n"; numFails++; }
		
	ompParallelFor ompParFor1(parfor1);
	//cout << parfor1 << "\n"; cout << ompParFor1.str() << "\n";
	if(parfor1_correct != ompParFor1.str()) { cout << "ERROR parsing parfor1! \n\tShould be: \""<<parfor1_correct<<"\" but\n\tactually got: \""<<ompParFor1.str()<<"\".\n"; numFails++; }
	ompParallelFor ompParFor2(parfor2);
	//cout << parfor2 << "\n"; cout << ompParFor2.str() << "\n";
	if(parfor2_correct != ompParFor2.str()) { cout << "ERROR parsing parfor2! \n\tShould be: \""<<parfor2_correct<<"\" but\n\tactually got: \""<<ompParFor2.str()<<"\".\n"; numFails++; }
	ompParallelFor ompParFor3(parfor3);
	//cout << parfor3 << "\n"; cout << ompParFor3.str() << "\n";
	if(parfor3_correct != ompParFor3.str()) { cout << "ERROR parsing parfor3! \n\tShould be: \""<<parfor3_correct<<"\" but\n\tactually got: \""<<ompParFor3.str()<<"\".\n"; numFails++; }
	ompParallelFor ompParFor4(parfor4);
	//cout << parfor4 << "\n"; cout << ompParFor4.str() << "\n";
	if(parfor4_correct != ompParFor4.str()) { cout << "ERROR parsing parfor4! \n\tShould be: \""<<parfor4_correct<<"\" but\n\tactually got: \""<<ompParFor4.str()<<"\".\n"; numFails++; }

	string for1 = " #pragma 	omp        for reduction(+: a, b, c, d, e) private(q, 	  k, greg)  lastprivate(	roger, 	  k, raga_k_123) default ( none) ordered";
	string for1_correct = "#pragma omp for reduction(+:a, b, c, d, e) private(q, k, greg) lastprivate(roger, k, raga_k_123) ordered";
	string for2 = " #pragma 	omp for reduction(||: a, b, c, d, e) reduction(/: a, b, c, d, e) ordered		firstprivate(q, 	  k, greg)  nowait	copyin(	roger, 	  k, raga_k_123) default ( shared)";
	string for2_correct = "#pragma omp for reduction(||:a, b, c, d, e) firstprivate(q, k, greg) ordered nowait";
	string for3 = " #pragma 	omp for nowait reduction(&&: a, b, c, d, e) reduction(&&: a) reduction(*: a1, b2, c3) firstprivate(q, 	  k, greg)  blah(	roger, 	  k, raga_k_123)";
	string for3_correct = "#pragma omp for reduction(&&:a, b, c, d, e) reduction(&&:a) reduction(*:a1, b2, c3) firstprivate(q, k, greg) nowait";
	string for4 = " #pragma 	omp 	 for firstprivate(q, 	  k, greg)  firstprivate(	roger, 	  k, raga_k_123)";
	string for4_correct = "#pragma omp for firstprivate(q, k, greg, roger, k, raga_k_123)";
	
	if(ompUtils::getOmpType(for1) != ompUtils::omp_for) { cout << "Error: for1 is supposed to be a "<<ompUtils::ompTypeStr(ompUtils::omp_for)<<" but is instead "<<ompUtils::ompTypeStr(ompUtils::getOmpType(for1))<<"!\n"; numFails++; }
	if(ompUtils::getOmpType(for2) != ompUtils::omp_for) { cout << "Error: for2 is supposed to be a "<<ompUtils::ompTypeStr(ompUtils::omp_for)<<" but is instead "<<ompUtils::ompTypeStr(ompUtils::getOmpType(for2))<<"!\n"; numFails++; }
	if(ompUtils::getOmpType(for3) != ompUtils::omp_for) { cout << "Error: for3 is supposed to be a "<<ompUtils::ompTypeStr(ompUtils::omp_for)<<" but is instead "<<ompUtils::ompTypeStr(ompUtils::getOmpType(for3))<<"!\n"; numFails++; }
	if(ompUtils::getOmpType(for4) != ompUtils::omp_for) { cout << "Error: for4 is supposed to be a "<<ompUtils::ompTypeStr(ompUtils::omp_for)<<" but is instead "<<ompUtils::ompTypeStr(ompUtils::getOmpType(for4))<<"!\n"; numFails++; }
	
	ompFor ompfor1(for1);
	//cout << "for1 = " << for1 << "\n"; cout << "ompfor1.str() = " << ompfor1.str() << "\n";
	if(for1_correct != ompfor1.str()) { cout << "ERROR parsing for1! \n\tShould be: \""<<for1_correct<<"\" but\n\tactually got: \""<<ompfor1.str()<<"\".\n"; numFails++; }
	ompFor ompfor2(for2);
	//cout << for2 << "\n"; cout << ompfor2.str() << "\n";
	if(for2_correct != ompfor2.str()) { cout << "ERROR parsing for2! \n\tShould be: \""<<for2_correct<<"\" but\n\tactually got: \""<<ompfor2.str()<<"\".\n"; numFails++; }
	ompFor ompfor3(for3);
	//cout << for3 << "\n"; cout << ompfor3.str() << "\n";
	if(for3_correct != ompfor3.str()) { cout << "ERROR parsing for3! \n\tShould be: \""<<for3_correct<<"\" but\n\tactually got: \""<<ompfor3.str()<<"\".\n"; numFails++; }
	ompFor ompfor4(for4);
	//cout << for4 << "\n"; cout << ompfor4.str() << "\n";
	if(for4_correct != ompfor4.str()) { cout << "ERROR parsing for4! \n\tShould be: \""<<for4_correct<<"\" but\n\tactually got: \""<<ompfor4.str()<<"\".\n"; numFails++; }
}

void testOMPForSensitiveInsertion()
{
	annotateAllOmpFors(project);
	
	Rose_STL_Container<SgNode*> iteratorUses = NodeQuery::querySubTree(project, V_SgInitializedName);	
	OMPcfgRWTransaction trans;
	//VirtualCFG::cfgRWTransaction trans;
	trans.beginTransaction();
	for(Rose_STL_Container<SgNode*>::iterator it = iteratorUses.begin(); it!=iteratorUses.end(); it++)
	{
		SgInitializedName *initName = isSgInitializedName(*it); ROSE_ASSERT(initName);
		//printf("initialized Name <%s | %s>\n", initName->get_parent()->unparseToString().c_str(), initName->get_parent()->class_name().c_str());
		
		if(initName->get_name().getString() == "iterator")
		{
			//printf("   inserting1 at spot <%s | %s>\n", initName->get_parent()->unparseToString().c_str(), initName->get_parent()->class_name().c_str());
			trans.insertBefore(initName, fooCallCreate());
			trans.insertAfter(initName, fooCallCreate());
		}
	}
	
	iteratorUses = NodeQuery::querySubTree(project, V_SgVarRefExp);	
	for(Rose_STL_Container<SgNode*>::iterator it = iteratorUses.begin(); it!=iteratorUses.end(); it++)
	{
		SgVarRefExp *varRef = isSgVarRefExp(*it); ROSE_ASSERT(varRef);
		
		if(varRef->get_symbol()->get_name().getString() == "iterator")
		{
			//printf("   inserting2 at spot <%s | %s>\n", varRef->get_parent()->unparseToString().c_str(), varRef->get_parent()->class_name().c_str());
			trans.insertBefore(varRef->get_parent(), fooCallCreate());
			trans.insertAfter(varRef->get_parent(), fooCallCreate());
		}
	}
	
	trans.commitTransaction();
}
