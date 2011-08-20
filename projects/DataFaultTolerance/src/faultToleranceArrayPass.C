/*
 * Kamal Sharma sharma9@llnl.gov
 */

#include <string>
#include <vector>
#include <set>

#include "rose.h"

#include "pragmaHandling.C"

#define NEW_FILE_INFO Sg_File_Info::generateDefaultFileInfoForTransformationNode()
#define VALIDATE_ARRAY_CALL "validateArray"
#define UPDATE_ARRAY_CALL "updateArray"
#define UPDATE_ELEM_CALL "updateElem"
#define HASHTABLE_CLEAR_CALL "clearHashTable"
#define FAULT_LIB_HEADER "faultToleranceArrayLib.h"
#define TEMP_LABEL "_memTemp"

using namespace std;
using namespace SageBuilder;
using namespace SageInterface;

class MemFaultTolerance {
public:
	MemFaultTolerance() {
		nameCount = 0;
	}

	void visitLoops(vector<SgForStatement*> loops);

private:
	/* Private Methods */
	void processLoop(SgForStatement* loop);
	void insertWriteCheck(SgExpression *lhs, SgExprStatement *curExprStatement);
	void insertUpdateValidateCheck(SgForStatement* loop);
	void insertClearHashTable(SgStatement* statement);
	void gatherArrayReads(SgExpression *rhs);
	void processArrayRead(SgPntrArrRefExp* arrayRef);
	void processArrayRefs(SgForStatement* loop);
	SgAddOp* createArrayBaseAddr(SgForStatement* loop, string arrayName,
			int dimensions);

	/* Private variables */
	ArrayList* curArrayList;
	set<string> arrayReadList;
	int nameCount;
};

/*
 * Inserts an update element call for every write to an array reference
 */
void MemFaultTolerance::insertWriteCheck(SgExpression *lhs,
		SgExprStatement *curExprStatement) {
	SgExpression *arrayNameExp;
	vector<SgExpression *> subscripts;
	vector<SgExpression *>* ptrsubscripts = &subscripts;
	vector<SgExpression *>::iterator subscriptsIter;


	/* Check if the current lhsExpr is an array reference */
	if (!isArrayReference(lhs, &arrayNameExp, &ptrsubscripts))
		return;

	string arrayName = arrayNameExp->unparseToString();
	ArrayInfo* arrayInfo = curArrayList->getArrayInfo(arrayName);
	if (arrayInfo == NULL)
		return;

	// Create temp variable to store old value
	stringstream out;
	out << nameCount++;
	string tempName = TEMP_LABEL + out.str();
	SgVariableDeclaration* tempDecl = buildVariableDeclaration(tempName,
			buildIntType(), buildAssignInitializer(lhs));
	insertStatementBefore(curExprStatement, tempDecl);

	//Create parameter list for function call
	SgExprListExp* parameters = new SgExprListExp(NEW_FILE_INFO);
	parameters->append_expression(buildStringVal(arrayName)); // Array name
	//parameters->append_expression(arrayNameExp); // Array Reference
	parameters->append_expression(lhs); // Array Value

	// Insert subscripts
	for (subscriptsIter = subscripts.end() - 1; subscriptsIter
			>= subscripts.begin(); subscriptsIter--)
		parameters->append_expression(*subscriptsIter);

	// Insert Old Value in parameter list
	parameters->append_expression(
			buildVarRefExp(tempName, curExprStatement->get_scope()));

	// Insert LB and UB
	//	for(int i=0; i<arrayInfo->getNoDims(); i++)
	//	{
	//		parameters->append_expression(buildIntVal(arrayInfo->getDimLB(i))); //LB
	//		parameters->append_expression(buildIntVal(arrayInfo->getDimUB(i))); //UB
	//	}

	// Insert Chunksize
	parameters->append_expression(buildIntVal(curArrayList->getChunksize()));

	// Insert Update Function Call
	SgName name1;
	SgExprStatement* callStmt;
	name1 = SgName(UPDATE_ELEM_CALL);
	callStmt = buildFunctionCallStmt(name1, buildVoidType(), parameters,
			curExprStatement->get_scope());
	insertStatementAfter(curExprStatement, callStmt);
}

/*
 * Insert Array Reads into set list
 */
void MemFaultTolerance::processArrayRead(SgPntrArrRefExp* arrayRef) {
	SgExpression *arrayNameExp;
	vector<SgExpression *> subscripts;
	vector<SgExpression *>* ptrsubscripts = &subscripts;

	/* Check if the current arrayRef is an array reference */
	if (!isArrayReference(arrayRef, &arrayNameExp, &ptrsubscripts))
		return;

	/* Insert Array Name in the list */
	string arrayName = arrayNameExp->unparseToString();
	arrayReadList.insert(arrayName);
}

/*
 * Process all array reads in a statement
 */
void MemFaultTolerance::gatherArrayReads(SgExpression *rhs) {
	vector<SgPntrArrRefExp*> arrayRefList = querySubTree<SgPntrArrRefExp> (rhs,
			V_SgPntrArrRefExp);

	for (vector<SgPntrArrRefExp*>::iterator iter = arrayRefList.begin(); iter
			!= arrayRefList.end(); iter++) {
		SgPntrArrRefExp *arrayRef = *iter;

		processArrayRead(arrayRef);
	}
}

/*
 * Process an expression statement to look for array references
 */
void MemFaultTolerance::processArrayRefs(SgForStatement* loop) {

	vector<SgExprStatement*> exprStmtList = querySubTree<SgExprStatement> (
			loop->get_loop_body(), V_SgExprStatement);

	// Visit each expression statement
	for (vector<SgExprStatement*>::iterator iter = exprStmtList.begin(); iter
			!= exprStmtList.end(); iter++) {
		SgAssignOp *assignOp = isSgAssignOp((*iter)->get_expression());
		if (assignOp != NULL) {
			SgExpression* lhs = assignOp->get_lhs_operand();
			SgExpression* rhs = assignOp->get_rhs_operand();

			insertWriteCheck(lhs, *iter);
			gatherArrayReads(rhs);
		}
	}
}

/*
 * Creates a base address for an array
 * e.g. A[0] + 0 which is equivalent to &A[0][0]
 */
SgAddOp* MemFaultTolerance::createArrayBaseAddr(SgForStatement* loop,
		string arrayName, int dimensions) {
	SgVarRefExp* varRefExp = buildVarRefExp(arrayName, loop->get_scope());

	SgPntrArrRefExp* baseRefExp = new SgPntrArrRefExp(NEW_FILE_INFO, varRefExp,
			buildIntVal(0), varRefExp->get_type());


	for (int i = 1; i < dimensions - 1; i++) {
		SgPntrArrRefExp *pntrRefExp = new SgPntrArrRefExp(NEW_FILE_INFO,
				baseRefExp, buildIntVal(0), varRefExp->get_type());
		baseRefExp = pntrRefExp;
	}

	SgAddOp* baseAddr;

	if (dimensions > 1)
		baseAddr = new SgAddOp(NEW_FILE_INFO, baseRefExp, buildIntVal(0),
				varRefExp->get_type());
	else
		baseAddr = new SgAddOp(NEW_FILE_INFO, varRefExp, buildIntVal(0),
				varRefExp->get_type());

	return baseAddr;
}

/*
 * Inserts validate and update checks before and after the pragma
 * Using set guarantees that only unique array references are inserted
 * in these checks
 */
void MemFaultTolerance::insertUpdateValidateCheck(SgForStatement* loop) {

	SgStatement* lastStatement = loop;

	for (set<string>::iterator it = arrayReadList.begin(); it
			!= arrayReadList.end(); it++) {
		string arrayName = *it;

		ArrayInfo* arrayInfo = curArrayList->getArrayInfo(arrayName);
		if (arrayInfo == NULL)
			return;

		//Create parameter list for function call
		SgExprListExp* parameters = new SgExprListExp(NEW_FILE_INFO);
		parameters->append_expression(buildStringVal(arrayName)); // Array name
		parameters->append_expression(
				createArrayBaseAddr(loop, arrayName, arrayInfo->getNoDims())); // Array base address


		// Insert LB and UB
		for (int i = 0; i < arrayInfo->getNoDims(); i++) {
			parameters->append_expression(buildIntVal(arrayInfo->getDimLB(i))); //LB
			parameters->append_expression(buildIntVal(arrayInfo->getDimUB(i))); //UB
		}

		// Insert Chunksize
		parameters->append_expression(buildIntVal(curArrayList->getChunksize()));

		// Insert Update Function Call
		SgName name1 = SgName(UPDATE_ARRAY_CALL);
		SgExprStatement* callStmt1 = buildFunctionCallStmt(name1,
				buildVoidType(), parameters, loop->get_scope());
		insertStatementBefore(loop, callStmt1);

		// Insert Validate Array Call
		SgName name2 = SgName(VALIDATE_ARRAY_CALL);
		SgExprStatement* callStmt2 = buildFunctionCallStmt(name2,
				buildVoidType(), parameters, lastStatement->get_scope());
		insertStatementAfter(lastStatement, callStmt2);

		lastStatement = callStmt2;
	}

	insertClearHashTable(lastStatement);
}

/*
 * Inserts the clear hash table method call at the end of the loop
 */
void MemFaultTolerance::insertClearHashTable(SgStatement* statement) {

	SgName name = SgName(HASHTABLE_CLEAR_CALL);
	SgExprStatement* callStmt = buildFunctionCallStmt(name,
			buildVoidType(), NULL, statement->get_scope());
	insertStatementAfter(statement, callStmt);
}

void MemFaultTolerance::processLoop(SgForStatement* loop) {

	processArrayRefs(loop);
	insertUpdateValidateCheck(loop);
	//insertClearHashTable(loop);
}

/*
 * This method visit the loops present in a function
 * and checks the pragma's associated with it
 */
void MemFaultTolerance::visitLoops(vector<SgForStatement*> loops) {
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
		ArrayList* arrayList = new ArrayList();
		if (!recognizePragma(pragmaDec, arrayList))
			continue;

		curArrayList = arrayList;
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

			MemFaultTolerance* memTolerance = new MemFaultTolerance();
			memTolerance->visitLoops(loops);
		}

		// Generate source code from AST and call the vendor's compiler
		return backend(sageProject);
	}

}
