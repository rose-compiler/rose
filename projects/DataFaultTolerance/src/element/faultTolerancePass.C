// Example ROSE Translator: used for testing ROSE infrastructure

#include "string.h"
#include "rose.h"

#define NEW_FILE_INFO Sg_File_Info::generateDefaultFileInfoForTransformationNode()
#define VERIFY_CALL "verifyCheckBits"
#define UPDATE_CALL "updateCheckBits"
#define FAULT_LIB_HEADER "faultToleranceLib.h"

using namespace std;
using namespace SageBuilder;
using namespace SageInterface;

/* Created enum for future extensions */
enum constructs_enum {
	mem_faultTolerance_enum, none_enum
};

/*
 * This method recognizes the different pragmas
 */
constructs_enum recognizePragma(SgPragmaDeclaration *pragmaDec) {
	string pragmaStr = pragmaDec->get_pragma()->get_pragma();

	if (pragmaStr.find("mem_fault_tolerance") != string::npos)
		return mem_faultTolerance_enum;

	return none_enum;
}

void insertChecksum(SgPntrArrRefExp *arrayReference,
		SgExprStatement *curExprStatement, bool isWrite) {

	if (arrayReference == NULL)
		return;

	// Create parameter list for function call
	SgExprListExp* parameters = new SgExprListExp(NEW_FILE_INFO);
	SgAddOp *addOp = new SgAddOp(NEW_FILE_INFO,
			arrayReference->get_lhs_operand(),
			arrayReference->get_rhs_operand(), arrayReference->get_type());
	parameters->append_expression(addOp); // Address
	//parameters->append_expression(arrayReference); // Value
	parameters->append_expression(
			buildStringVal(arrayReference->get_type()->unparseToString())); // type

	// Insert Function Call
	SgName name1;
	SgExprStatement* callStmt;

	if (!isWrite) {
		name1 = SgName(VERIFY_CALL);
		callStmt = buildFunctionCallStmt(name1, buildVoidType(), parameters,
				curExprStatement->get_scope());
		insertStatementBefore(curExprStatement, callStmt);
	} else {
		name1 = SgName(UPDATE_CALL);
		callStmt = buildFunctionCallStmt(name1, buildVoidType(), parameters,
				curExprStatement->get_scope());
		insertStatementAfter(curExprStatement, callStmt);
	}

}

/*
 * This method inserts verify check for all the array references
 * present below the expr node.
 */
void fetchAndVerifyArrayRefs(SgExpression *expr,
		SgExprStatement *curExprStatement, bool isWrite) {

	vector<SgPntrArrRefExp*> arrayRefList = querySubTree<SgPntrArrRefExp> (
			expr, V_SgPntrArrRefExp);
	vector<SgPntrArrRefExp*> uniqueArrayRefList;

	// Create a new list as nodes change due to new statements
	for (vector<SgPntrArrRefExp*>::iterator iter = arrayRefList.begin(); iter
			!= arrayRefList.end(); iter++) {
		SgPntrArrRefExp *arrayRef = *iter;
		SgNode *parentArrayRef = arrayRef->get_parent();

		if (isSgPntrArrRefExp(parentArrayRef) == NULL)
			uniqueArrayRefList.push_back(arrayRef);
	}

	// Visit each array reference
	for (vector<SgPntrArrRefExp*>::iterator iter = uniqueArrayRefList.begin(); iter
			!= uniqueArrayRefList.end(); iter++) {
		insertChecksum(*iter, curExprStatement, isWrite);
	}
}

void processArrayRefs(SgForStatement* loop) {
	vector<SgExprStatement*> exprStmtList = querySubTree<SgExprStatement> (
			loop->get_loop_body(), V_SgExprStatement);

	// Visit each expression statement
	for (vector<SgExprStatement*>::iterator iter = exprStmtList.begin(); iter
			!= exprStmtList.end(); iter++) {
		SgAssignOp *assignOp = isSgAssignOp((*iter)->get_expression());
		if (assignOp != NULL) {
			SgExpression* lhs = assignOp->get_lhs_operand();
			SgExpression* rhs = assignOp->get_rhs_operand();

			SgPntrArrRefExp *arrayReference = isSgPntrArrRefExp(lhs);
			insertChecksum(arrayReference, *iter, true);

			fetchAndVerifyArrayRefs(rhs, *iter, false);
		}
	}
}

void insertStartEndCheck(SgForStatement* loop) {
	SgName name1(VERIFY_CALL);
	SgExprStatement* callStmt_1 = buildFunctionCallStmt(name1, buildVoidType(),
			buildExprListExp(), loop->get_scope());

	insertStatementBefore(loop, callStmt_1);
	insertStatementAfter(loop, callStmt_1);

}

void processLoop(SgForStatement* loop) {
	//insertStartEndCheck(loop);

	processArrayRefs(loop);

}

/*
 * This method visit the loops present in a function
 * and checks the pragma's associated with it
 */
void visitLoops(vector<SgForStatement*> loops) {
	// Visit each loop
	for (vector<SgForStatement*>::iterator iter = loops.begin(); iter
			!= loops.end(); iter++) {
		SgForStatement* forloop = (*iter);

		SgStatement* prev_stmt = getPreviousStatement(forloop);
		SgPragmaDeclaration *pragmaDec = isSgPragmaDeclaration(prev_stmt);

		/* If no pragma present, skip to next loop */
		if (pragmaDec == NULL)
			continue;

		/* If pragma is not mem_fault_tolerance, ignore it */
		if (recognizePragma(pragmaDec) != mem_faultTolerance_enum)
			continue;

		processLoop(forloop);
	}
}

void insertHeader(SgSourceFile * sfile) {
	insertHeader(FAULT_LIB_HEADER, PreprocessingInfo::after, false,
			sfile->get_globalScope());
}

/*
 * Main Function
 */
int main(int argc, char * argv[]) {
	// Build the AST used by ROSE
	SgProject* sageProject = frontend(argc, argv);

	// For each source file in the project
	SgFilePtrList & ptr_list = sageProject->get_fileList();
	for (SgFilePtrList::iterator iter = ptr_list.begin(); iter
			!= ptr_list.end(); iter++) {
		SgFile* sageFile = (*iter);
		SgSourceFile * sfile = isSgSourceFile(sageFile);
		ROSE_ASSERT(sfile);
		SgGlobal *root = sfile->get_globalScope();
		SgDeclarationStatementPtrList& declList = root->get_declarations();

		// Insert header file
		insertHeader(sfile);

		//For each function body in the scope
		for (SgDeclarationStatementPtrList::iterator p = declList.begin(); p
				!= declList.end(); ++p) {
			SgFunctionDeclaration *func = isSgFunctionDeclaration(*p);
			if (func == 0)
				continue;
			SgFunctionDefinition *defn = func->get_definition();
			if (defn == 0)
				continue;
			//ignore functions in system headers, Can keep them to test robustness
			if (defn->get_file_info()->get_filename()
					!= sageFile->get_file_info()->get_filename())
				continue;
			SgBasicBlock *body = defn->get_body();
			ROSE_ASSERT(body);

			vector<SgForStatement*> loops = querySubTree<SgForStatement> (defn,
					V_SgForStatement);
			if (loops.size() == 0)
				continue;

			visitLoops(loops);
		}

		// Generate source code from AST and call the vendor's compiler
		return backend(sageProject);
	}

}
