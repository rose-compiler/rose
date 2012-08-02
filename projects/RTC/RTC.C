#ifndef RTC_C
#define RTC_C
#include "CheckBase.h"
#include "QSTraversal.h"
#include "ArithCheck.h"
#include "BinarySimpleTraversal.h"


//CheckBase* checkbaseobject;


NodeContainer TraversalBase::MallocRefs;
NodeContainer TraversalBase::VarNames;
NodeContainer TraversalBase::VarRefs;


void insertTimingSupport(SgProject* project) {


	SgFilePtrList file_list = project->get_fileList();
	SgFilePtrList::iterator iter;
	for(iter = file_list.begin(); iter!=file_list.end(); iter++) {
		SgSourceFile* cur_file = isSgSourceFile(*iter);
		SgGlobal* global_scope = cur_file->get_globalScope();
		SageInterface::insertHeader("foo2_c.h", PreprocessingInfo::after,false,global_scope);
	}


    //hackyPtrCheck();
	//AnotherHackyPtrCheck();
	//YetAnotherHackyPtrCheck();
	//HackyPtrCheck4();

//    SgProject* project = CheckBase::CBProject;

	SgNode* TopNode = isSgNode(project);

	SgFunctionDeclaration* MainFn = SageInterface::findMain(TopNode);

	SgScopeStatement* scope = MainFn->get_scope();
    
	SgFunctionDefinition* MainFnDef = MainFn->get_definition();

	SgBasicBlock* FirstBlock = MainFnDef->get_body();

	SgStatementPtrList& Stmts = FirstBlock->get_statements();

	SgName s_name ("StartClock");
	SgExprListExp* parameter_list = SageBuilder::buildExprListExp();

	SgExprStatement* NewFunction = SageBuilder::buildFunctionCallStmt(s_name, 
													SgTypeVoid::createType(), 
													parameter_list, 
													scope); 



	SgStatementPtrList::iterator FirstStmt = Stmts.begin();
	// Equivalent to above, I guess
//	SgStatement* FirstStmt = SageInterface::getFirstStatement(scope);

	SageInterface::insertStatementBefore(*FirstStmt,
										NewFunction);



	#if 1
	SgName s_name2 ("EndClock");
	SgExprListExp* parameter_list2 = SageBuilder::buildExprListExp();

	SgExprStatement* NewFunction2 = SageBuilder::buildFunctionCallStmt(s_name2, 
													SgTypeVoid::createType(), 
													parameter_list2, 
													scope); 



	//SgStatementPtrList::iterator sit2 = Stmts.begin();
	//SgStatement* LastStatement = SageInterface::getLastStatement(scope);
	// FIXME: This is a dirty hack which uses the fact that the StatementPtrList
	// is an ROSE_STL_Container, which is a standard STL vector
	//SgStatement* LastStmt = Stmts[Stmts.size() - 1];
	SgStatement* LastStmt = Stmts.back();

	SageInterface::insertStatementBefore(LastStmt,
										NewFunction2);



	#endif
}


int main(int argc, char* argv[]) {

	printf("Hello World\n");

	Rose_STL_Container<std::string> ArgList = CommandlineProcessing::generateArgListFromArgcArgv(argc, argv); 

	SgProject* project = frontend(argc, argv);
	ROSE_ASSERT(project != NULL);

    CheckBase::CBProject = project;

	#if 1
	ArithCheck AC;
	AC.setProject(project);
	AC.instrument();
	#else
	// Create the Check and Traversal objects
	CheckBase* CB = new ArithCheck;
//	printf("%lu\n", &(*CB));


	#if 1
	CB->instrument();
	#else
	bool plainRun = false;

	for(Rose_STL_Container<std::string>::iterator rit = ArgList.begin(); rit != ArgList.end(); rit++) {
		//printf("Arg: %s\n", *rit);
		//std::cout << "Arg: " << *rit << "\n";

		if(*rit == "-plain") {
			plainRun = true;
			break;
		}
	}

	if(!plainRun) {
	
	//	checkbaseobject = CB;
	
		//CB->registerCheck();
		//CheckBase::RegisterBinaryOpCheck(CB);
	
//		TraversalBase* TB = new QSTraversal;
   		TraversalBase* TB = new BinarySimpleTraversal;

		// Let them know about each other
		CheckBase::TBObj = TB;
		TraversalBase::CBObj = CB;
	
		// Traverse the tree, starting from SgProject
			// Call CheckType if required
		// 	FIXME: Would be great if we knew exactly when to call
		// the process Node. Right now, we call it always -- 
		// This is a basic one. Very bad performance
		TB->nodeCollection(project);
	
		TB->nodeInstrumentation(project);
	}
	else {
		insertTimingSupport(project);	
	}
	#endif
	#endif

	#if 1
	printf("runAllTests\n");
	// FIXME: Removed Oct 18th 2011 12PM
	AstTests::runAllTests(project);
	printf("runAllTests - done\n");
	#endif

	printf("backend/unparser\n");
	// backend = unparse + compile
//	return backend(project);
	project->unparse();
	printf("backend/unparser - done\n");
	
	return 0;


}

#endif
