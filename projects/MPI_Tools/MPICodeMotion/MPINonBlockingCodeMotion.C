#include "rose.h"
#include "MPINonBlockingCodeMotion.h"

using namespace std;
using namespace MPIOptimization;


/**********************************************************************
* Class MPINonBlockingCodeMotion Implementation
*
*
**********************************************************************/

///////////////////////////////////////////////////////////////////////
// Constructor & Destructor
///////////////////////////////////////////////////////////////////////

MPINonBlockingCodeMotion::MPINonBlockingCodeMotion(SgProject* proj): project(proj)
{
	ROSE_ASSERT(project != NULL);
	sdg = new SystemDependenceGraph;

	cgQuery = new CallGraphQuery(project);
	//cout << endl << "Han: Done SDG parsing..." << endl;
}


MPINonBlockingCodeMotion::~MPINonBlockingCodeMotion()
{
	delete sdg;
}



///////////////////////////////////////////////////////////////////////
// public methods
///////////////////////////////////////////////////////////////////////


int MPINonBlockingCodeMotion::run()
{
	// 1. Retrieve MPI calls from source code
	retrieveMPICalls();

	// construction the mapping 
	// from function declarations 
	// to a bool value representing the function calls MPI calls
	traverseCallGraph();

	set<SgFunctionDeclaration*> mpiList = cgQuery->getFuncWithMPICallSet();
	for(set<SgFunctionDeclaration*>::iterator i = mpiList.begin(); i != mpiList.end(); i++)
	{
		SgFunctionDeclaration* fd = isSgFunctionDeclaration(*i);
		cout << "Han: fd " << fd->unparseToString() << endl;
	}

	// SDG is already generated when this class is constructed.
	ROSE_ASSERT(sdg != NULL);

	// generating SDG
	sdg->parseProject(project);

	cout << "Han: =========================================" << endl;
	cout << "Han: SystemDependenceGraph has parsed project" << endl;
	cout << "Han: Starting Code Motion..." << endl;
	cout << "Han: =========================================" << endl;

	// 1. First level
	// for each mpi communication call,
	for(std::list<SgExprStatement*>::iterator i = mpiCommFuncCallList.begin(); i != mpiCommFuncCallList.end(); i++)
	{
		ROSE_ASSERT(isSgExprStatement(*i));
		MotionInformation* info = new MotionInformation(isSgExprStatement(*i), cgQuery);

		processStatementMotion(info);
	}

	// 2. Second or higher level

	// now we consider blocks that wrap MPI calls, for example, 
	// if(rank%2==0) MPI_Irecv(...);
	// we assume that the previous step has moved the mpi calls to upward and
	// the first statement of the block is a mpi call OR
	// all the statements before the mpi call in the block define the parameter of the mpi call

	// *** for now, let's assume only the first case....

	cout << "Han: ============================================" << endl;
	cout << "Han: run()::block things started" << endl;
	cout << "Han: motionInfoList.size: " << motionInfoList.size() << endl;
	cout << "Han: ============================================" << endl;

	while(motionInfoList.size() > 0)
	{
		list<MotionInformation*>::iterator motionInfoListItr = motionInfoList.begin();

		// if it is block motion information,
		BlockMotionInformation* blockInfo = dynamic_cast<BlockMotionInformation*>(*motionInfoListItr);
		if(blockInfo != NULL)
		{
			motionInfoList.pop_front();
			processBlockMotion(blockInfo);
		} else {
			MotionInformation* motionInfo = dynamic_cast<MotionInformation*>(*motionInfoListItr);
			if(motionInfo != NULL)
			{
				motionInfoList.pop_front();
				processStatementMotion(motionInfo);
			}
		}
	}
		
	return 1;
}

///////////////////////////////////////////////////////////////////////
// Protected methods
///////////////////////////////////////////////////////////////////////

/* 
 	subroutine for run()
 */
void MPINonBlockingCodeMotion::processStatementMotion(MotionInformation* info)
{
	ROSE_ASSERT(info != NULL);
	ROSE_ASSERT(info->getSrc() != NULL);

	// 2. Find data dependencies between MPI call parameters and definitions
	//    using System Dependency Graph Analysis
	info->findDataDependence(sdg);

	// 3. Dependencies between MPI Calls
	info->findMPICallDependence(&mpiFunctionCalls);

	// 4. find block boundaries for each MPI calls
	info->findBlockDependence();

	// 5. find call site dependencies
	info->findCallSiteDependence(sdg);

	// we save outer scope for the second or higher level code motion
	SgBasicBlock* basicBlock = isSgBasicBlock(info->getSrc()->get_parent());

	if(basicBlock != NULL)
	{
		if(isScopeStatement(basicBlock->get_parent()))
		{
			BlockMotionInformation *blockInfo = new BlockMotionInformation(info, cgQuery, basicBlock->get_parent());
			motionInfoList.push_back(blockInfo);
		} 
		else if (isSgBasicBlock(basicBlock->get_parent()) ) //||
				//isSgFunctionDefinition(basicBlock->get_parent()))
		{
			BlockMotionInformation *blockInfo = new BlockMotionInformation(info, cgQuery, basicBlock);
			motionInfoList.push_back(blockInfo);
		} 
		else if(isSgFunctionDefinition(basicBlock->get_parent()))
		{
			cout << "Han: found a function definition:" << basicBlock->get_parent()->unparseToString() << endl;
			// find call sites and create a MotionInformation instance
			InterproceduralInfo* ii = sdg->getInterproceduralInformation(isSgFunctionDefinition(basicBlock->get_parent())->get_declaration());

			cout << "Han: function entry: " << ii->getFunctionEntry()->unparseToString() << endl;
			SgNode* entry = ii->getFunctionEntry();
			DependenceNode* callee = sdg->getExistingNode(DependenceNode::ENTRY, entry);

			set<SimpleDirectedGraphNode*> callerSet = callee->getPredecessors();

			for(set<SimpleDirectedGraphNode*>::iterator i = callerSet.begin(); i != callerSet.end(); i++)
			{
				DependenceNode* caller = dynamic_cast<DependenceNode*>(*i);

				if(caller != NULL && sdg->edgeExists(caller, callee, DependenceGraph::CALL))
				{
					SgExprStatement* callSiteExpr = isSgExprStatement(caller->getSgNode());
					if(callSiteExpr != NULL)
					{
						MotionInformation* callSiteInfo = new MotionInformation(callSiteExpr, cgQuery);
						motionInfoList.push_back(callSiteInfo);
					}
				}
			}

			/*
			int callSiteCount = ii->callSiteCount();

			for(int i = 0; i < callSiteCount; i++)
			{
				SgFunctionCallExp* callSite = isSgFunctionCallExp(ii->getFunctionCallExpNode(i));
				cout << "Han: call site of this definition is : " << callSite->unparseToString() << endl;

				MotionInformation* callSiteInfo = new MotionInformation(isSgExprStatement(callSite->get_parent()));
				cout << "Han: push back a new motion info for call sites: " << callSite->unparseToString() << endl;
				motionInfoList.push_back(callSiteInfo);
			}
			*/
		}
	} else {
		// can be a test case in a for statement.
		// but we are not interested in moving for loop test statements.
		cout << "Han: SgExprStatement's parent is not SgBasicBlock " << info->getSrc()->class_name() << endl;
	}

	///////////////////////////////////////////////////////////////////////////
	// Up to this point, 
	// we should know to which point of the code the MPI_Recvs/Sends can be hoisted
	///////////////////////////////////////////////////////////////////////////
	info->setDest();
	moveCode(info->getSrc(), info->getDest(), info->getInsertAfterDest());

	//delete info;
	cout << "Han: moved one statement" << endl;
}

void MPINonBlockingCodeMotion::processBlockMotion(BlockMotionInformation* blockInfo)
{

	// In order to move the block, 
	// first, we have to decide whether we can slice out the first statement
	// the counter example will be 
	// for(i=0;i<10;i++) { MPI_Irecv(k, ...); k++ }

	// if possible, slice out the block

	cout << "Han: run()::slice out: " << blockInfo->getSliceStart()->unparseToString() << endl;
	cout << "Han: run()::source: " << blockInfo->getSrc()->unparseToString() << endl;
	cout << "Han: run()::top: " << blockInfo->getSliceStart()->class_name() << endl;

	// TODO: fix this function..
	//sliceOutAsNewBlock(blockInfo);

	// second, we collect dependence information for that block
	// TODO: be careful, badn performance!!
	if(sdg != NULL)
		delete sdg;
	sdg = new SystemDependenceGraph();
	sdg->parseProject(project);

	// TODO: be careful, bad performance!!
	retrieveMPICalls();

	// compute dependencies all over again!!! 
	// (very expensive.. 
	// but not sure whether or notit is possible to reuse the previous information,
	// especially after moving (adding and deleting) codes, possibly dangling pointers)
	blockInfo->findDataDependence(sdg);
	blockInfo->findMPICallDependence(&mpiFunctionCalls);
	blockInfo->findBlockDependence();

	blockInfo->setDest();


		// third, move the block where possible
	cout << "Han: run()::move FROM " << blockInfo->getSliceStart()->unparseToString() << " ** TO ** " << blockInfo->getDest()->unparseToString() << endl;

	moveCode(blockInfo->getSliceStart(), blockInfo->getDest(), blockInfo->getInsertAfterDest());


	// again, the moved block and the the closest outer block forms a new block
	// if the new block satisfies certain conditions,
	// put it into a queue so that it can be analyzed later.
	SgNode* sliceStart = blockInfo->getSliceStart();
	SgNode* sliceParent = sliceStart->get_parent();

	if(sliceParent && sliceParent->get_parent() && 
			isScopeStatement(sliceParent->get_parent()))
	{
		BlockMotionInformation* newBlockInfo = new BlockMotionInformation(blockInfo, cgQuery, sliceParent->get_parent());
		motionInfoList.push_back(newBlockInfo);
	}
	else if(sliceParent && sliceParent->get_parent() && 
			isSgBasicBlock(sliceParent->get_parent()) ) //||
			//isSgFunctionDefinition(sliceParent->get_parent()))
	{
		BlockMotionInformation* newBlockInfo = new BlockMotionInformation(blockInfo, cgQuery, sliceParent);
		motionInfoList.push_back(newBlockInfo);
	}
	else if(sliceParent && sliceParent->get_parent() &&
			isSgFunctionDefinition(sliceParent->get_parent()))
	{
		cout << "Han: callSite: " << sliceParent->get_parent()->unparseToString() << endl;
		// interprocedural analysis

		// find the entry node

		// find CALL edges that end in the entry node

		// for each call site, we add a CallSiteInformation..
		//CallSiteInformation* newCallSiteInfo = new CallSiteInformation(blockInfo,


	}
	else
	{
		cout << "Han: run()::you've reached at the outer most block!!" << endl;
		if(sliceParent)
			cout << "Han: " << sliceParent->class_name() << endl;
		else
			cout << "Han: run():: sliceParent is NULL" << endl;
	}


}

/*
void MPINonBlockingCodeMotion::processCallSiteMotion(MotionInformation* info)
{
	// 1. make a list of variables passed as parameters


	// 2. find data dependencies for each parameters

	// 3. find control dependencies for the call site

	// 4. move code

	// 5. create a new block information for the block containing the call site..


}
*/


// end of subroutines for run()
////////////////////////////////////////////////////////////////////////////////


// Step 1.
// Retrieves all the MPI library calls
// can be reimplemented using more rigorous fashion, 
// e.g., check if the function definition is in mpi.h
void MPINonBlockingCodeMotion::retrieveMPICalls()
{
	mpiFunctionCalls.clear();
	mpiCommFuncCallList.clear();
	mpiRecvFunctionCallList.clear();
	mpiSendFunctionCallList.clear();

	//cout << "Han: list size: " << mpiCommFuncCallList.size() << " " << mpiRecvFunctionCallList.size() << " " << mpiSendFunctionCallList.size() << endl;

	// 1. Get MPI calls
	Rose_STL_Container<SgNode*> expressionList = NodeQuery::querySubTree(project, V_SgExprStatement);

	for(Rose_STL_Container<SgNode*>::iterator i = expressionList.begin(); i != expressionList.end(); i++)
	{
		SgExprStatement* callSite = isSgExprStatement(*i);

		if(callSite->get_expression() == NULL)
			continue;
		SgFunctionCallExp* functionExpression = isSgFunctionCallExp(callSite->get_expression());

		if(functionExpression == NULL || functionExpression->get_function() == NULL)
			continue;
		ROSE_ASSERT(functionExpression->get_function() != NULL);
		SgFunctionRefExp* functionRefExp = isSgFunctionRefExp(functionExpression->get_function());

		if(functionRefExp != NULL)
		{
			SgFunctionSymbol* functionSymbol = NULL;

			functionSymbol = functionRefExp->get_symbol();

			// 1) if a function name starts with MPI_, then store into a list
			string functionName = functionSymbol->get_name().getString();

			if(functionName.find("MPI_") != string::npos)
			{
				//cout << "Han: MPI call: " << functionSymbol->get_name().getString() << endl;
				mpiFunctionCalls.push_back(callSite);

				// 2) MPI Recv calls
				if(functionName.find("MPI_Irecv") != string::npos ||
						functionName.find("MPI_Recv") != string::npos)
				{
					// save information for system dependence graph analysis
					mpiRecvFunctionCallList.push_back(callSite);
					mpiCommFuncCallList.push_back(callSite);
					//cout << "Han: MPI Recv Call: " << functionSymbol->get_name().getString() << endl;
				}

				// 3) MPI Send calls
				if(functionName.find("MPI_Isend") != string::npos ||
						functionName.find("MPI_Send") != string::npos)
				{
					// save information for system dependence graph analysis
					mpiSendFunctionCallList.push_back(callSite);
					mpiCommFuncCallList.push_back(callSite);
					//cout << "Han: MPI Send Call: " << functionSymbol->get_name().getString() << endl;
				}
			}
		}

	}
}

void MPINonBlockingCodeMotion::retrieveMPICalls2()
{
	mpiFunctionCalls.clear();
	mpiCommFuncCallList.clear();
	mpiRecvFunctionCallList.clear();
	mpiSendFunctionCallList.clear();

	Rose_STL_Container<SgNode*> funcDefList = NodeQuery::querySubTree(project, V_SgFunctionDefinition);

	for(Rose_STL_Container<SgNode*>::iterator i = funcDefList.begin(); i != funcDefList.end(); i++)
	{
		SgFunctionDefinition* fDef = isSgFunctionDefinition(*i);
		ROSE_ASSERT(fDef != NULL);

		Rose_STL_Container<SgNode*> callSiteList = NodeQuery::querySubTree(fDef, V_SgExprStatement);
		for(Rose_STL_Container<SgNode*>::iterator j = callSiteList.begin(); j != callSiteList.end(); j++)
		{
			SgExprStatement* callSite = isSgExprStatement(*j);

			if(callSite->get_expression() == NULL)
				continue;
			SgFunctionCallExp* functionExpression = isSgFunctionCallExp(callSite->get_expression());

			if(functionExpression == NULL || functionExpression->get_function() == NULL)
				continue;
			ROSE_ASSERT(functionExpression->get_function() != NULL);
			SgFunctionRefExp* functionRefExp = isSgFunctionRefExp(functionExpression->get_function());

			if(functionRefExp != NULL)
			{
				SgFunctionSymbol* functionSymbol = NULL;

				functionSymbol = functionRefExp->get_symbol();

				// 1) if a function name starts with MPI_, then store into a list
				string functionName = functionSymbol->get_name().getString();

				if(functionName.find("MPI_") != string::npos)
				{
					//cout << "Han: MPI call: " << functionSymbol->get_name().getString() << endl;
					mpiFunctionCalls.push_back(callSite);

					// 2) MPI Recv calls
					if(functionName.find("MPI_Irecv") != string::npos ||
							functionName.find("MPI_Recv") != string::npos)
					{
						// save information for system dependence graph analysis
						mpiRecvFunctionCallList.push_back(callSite);
						mpiCommFuncCallList.push_back(callSite);
						//cout << "Han: MPI Recv Call: " << functionSymbol->get_name().getString() << endl;
					}

					// 3) MPI Send calls
					if(functionName.find("MPI_Isend") != string::npos ||
							functionName.find("MPI_Send") != string::npos)
					{
						// save information for system dependence graph analysis
						mpiSendFunctionCallList.push_back(callSite);
						mpiCommFuncCallList.push_back(callSite);
						//cout << "Han: MPI Send Call: " << functionSymbol->get_name().getString() << endl;
					}
				}
			}
		}

	}
}

void MPINonBlockingCodeMotion::traverseCallGraph()
{
	cgQuery->initialize();

	cgQuery->constructFuncWithMPICallSet(mpiFunctionCalls);
}


///////////////////////////////////////////////////////////////////////////////////
// MPINonBlockingCodeMotion
// protected function implementations
///////////////////////////////////////////////////////////////////////////////////

void MPINonBlockingCodeMotion::moveCode(SgNode* _from, SgNode* _to, bool insertAfterDest)
{
	// what this method is doing is removing the AST subtree of which root is the SgExprStatement of MPI_recv call
	// and inserting a subtree at "to" location
	SgStatement* from = isSgStatement(_from);
	SgNode* to = _to;

	cout << "Han: moving statements" << endl;
	if(insertAfterDest)
	{
		cout << "Han: Moving the statement: from " << from->unparseToString() << endl;
		cout << "Han: after " << to->unparseToString() << endl;

		SgBasicBlock* parent = isSgBasicBlock(to->get_parent());
		if(parent != NULL)
		{
			SgStatementPtrList& basicBlockStmtList = parent->get_statements();

			// find the locations of the two statements in its parent basic block
			Rose_STL_Container<SgStatement*>::iterator toLoc = find(basicBlockStmtList.begin(), basicBlockStmtList.end(), to);
			Rose_STL_Container<SgStatement*>::iterator fromLoc = find(basicBlockStmtList.begin(), basicBlockStmtList.end(), from);

			if(toLoc!= basicBlockStmtList.end() && fromLoc != basicBlockStmtList.end())
			{
         // delete the previous statement

         // DQ (9/26/2007): Moved from std::list to std::vector uniformly in ROSE.
         // basicBlockStmtList.remove(*fromLoc);
				basicBlockStmtList.erase(fromLoc);

				// insert a new statement in a new location
				basicBlockStmtList.insert(++toLoc, from);
			}
		}
	}
	else
	{
		ROSE_ASSERT(isSgBasicBlock(to) != NULL);
		cout << "Han: Moving the statement: from " << from->unparseToString() << endl;
		cout << "Han: in " << to->unparseToString() << endl;

		SgStatementPtrList& basicBlockStmtList = isSgBasicBlock(to)->get_statements();
		Rose_STL_Container<SgStatement*>::iterator fromLoc = find(basicBlockStmtList.begin(), basicBlockStmtList.end(), from);

		if(fromLoc != basicBlockStmtList.end())
		{
			//cout << "Han: hey" << endl;

      // DQ (9/26/2007): Moved from std::list to std::vector uniformly in ROSE.
      // basicBlockStmtList.remove(*fromLoc);
			basicBlockStmtList.erase(fromLoc);

      // DQ (9/26/2007): Moved from std::list to std::vector uniformly in ROSE.
      // basicBlockStmtList.push_front(from);
         basicBlockStmtList.insert(basicBlockStmtList.begin(),from);
		}
	}
}

void MPINonBlockingCodeMotion::sliceOutAsNewBlock(BlockMotionInformation* blockInfo)
{
#if 0
	SgNode* endPath = blockInfo->getSrc();
	SgNode* startPath = blockInfo->getSliceStart();

	// TODO: if the src node is the only statement in it, you can skip slicing..


	// from endPath..
	// make a copy of childrens of the endPath
	Sg_File_Info* fileInfo = Sg_File_Info::generateDefaultFileInfoForTransformationNode();

	SgNode* p = endPath;
	SgNode* child = endPath;
	SgNode* parent = endPath->get_parent();

	SgNode* newTree; // = new_exprStatement;

	while(p != NULL) // && p != startPath)
	{
		// base case
		if(isSgExprStatement(p))
		{
			SgExprStatement* new_exprStatement = isSgExprStatement(p->copy(SgTreeCopy::static_instance()));
			newTree = new_exprStatement;
			blockInfo->setSrc(new_exprStatement);

			// delete this statement from old AST tree
			if(isSgBasicBlock(parent) && p != startPath)
			{
				SgStatementPtrList& stmt_list = isSgBasicBlock(parent)->get_statements();
				stmt_list.remove(isSgStatement(p));
				cout << "Han: after deletion " << parent->unparseToString() << endl;
			}
		}
		else if(isSgBasicBlock(p))
		{
			cout << "Han: adding SgBasicBlock" << endl;
			SgBasicBlock* current = isSgBasicBlock(p);
			ROSE_ASSERT(current != NULL);

			// TODO: you should include all the statements that is placed before newTree node
			SgStatement* newChild = isSgStatement(newTree);
			ROSE_ASSERT(newChild != NULL);

			SgBasicBlock* newBlock = new SgBasicBlock(fileInfo, NULL);
			SgStatementPtrList& statements = current->get_statements();
			SgStatementPtrList::iterator loc = find(statements.begin(), statements.end(), child);

			// copy all the statements up to 
			for(SgStatementPtrList::iterator i = statements.begin(); i != loc; i++)
			{
				SgStatement* aStmt = (*i)->copy(SgTreeCopy::static_instance());
				newBlock->append_statement(aStmt);
				aStmt->set_parent(newBlock);
			}

			newBlock->append_statement(newChild);
			newChild->set_parent(newBlock);

#if 0
			SgBasicBlock* newBlock = new SgBasicBlock(fileInfo, newChild);
			//newBlock->append_statement(child);
			newChild->set_parent(newBlock);
#endif

			newTree = newBlock;

			// delete this block if this is included inside another basic block
			if(isSgBasicBlock(parent) && p != startPath)
			{
				SgStatementPtrList& stmt_list = isSgBasicBlock(parent)->get_statements();
				stmt_list.remove(isSgStatement(p));
				cout << "Han: after deletion " << parent->unparseToString() << endl;
			}
		} 
		else if(isSgIfStmt(p))
		{
			cout << "Han: adding SgIfStmt" << endl;
			SgIfStmt* current = isSgIfStmt(p);
			ROSE_ASSERT(current != NULL);

			SgBasicBlock* newChild = isSgBasicBlock(newTree);
			ROSE_ASSERT(newTree != NULL);

			SgStatement* newConditional = isSgStatement(current->get_conditional()->copy(SgTreeCopy::static_instance()));
			ROSE_ASSERT(newConditional != NULL);

			SgBasicBlock* newTrueBody;
			SgBasicBlock* newFalseBody;

			if(current->get_true_body() == child)
			{
				newTrueBody = newChild;
				newFalseBody = new SgBasicBlock(fileInfo, NULL);
			}
			else
			{
				newTrueBody = new SgBasicBlock(fileInfo, NULL);
				newFalseBody = newChild;
			}

			SgIfStmt* newIfStmt = new SgIfStmt(fileInfo, newConditional, newTrueBody, newFalseBody);
			newTrueBody->set_parent(newIfStmt);
			newFalseBody->set_parent(newIfStmt);
			newTree = newIfStmt;

			// delete current if it's empty
			bool simple_conditional = false;
			bool true_body_empty = false;
			bool false_body_empty = false;

			SgStatement* leftOverCond = current->get_conditional();
			list<SgNode*> assignmentList = NodeQuery::querySubTree(leftOverCond, V_SgAssignOp);
			list<SgNode*> callSiteList = NodeQuery::querySubTree(leftOverCond, V_SgFunctionCallExp);

			if(assignmentList.empty() && callSiteList.empty())
				simple_conditional = true;

			SgBasicBlock* leftOverTrueBody = current->get_true_body();
			SgStatementPtrList leftOverTrueBodyStmtList = leftOverTrueBody->get_statements();
			if(leftOverTrueBodyStmtList.empty())
				true_body_empty = true;

			SgBasicBlock* leftOverFalseBody = current->get_false_body();
			SgStatementPtrList leftOverFalseBodyStmtList = leftOverFalseBody->get_statements();
			if(leftOverFalseBodyStmtList.empty())
				false_body_empty = true;

			if(simple_conditional && true_body_empty && false_body_empty && 
					isSgBasicBlock(parent) && p != startPath)
			{
				SgStatementPtrList& stmtList = isSgBasicBlock(parent)->get_statements();
				stmtList.remove(isSgStatement(p));
			}
		}
		else if(isSgForStatement(p))
		{
			cout << "Han: adding SgForStatement" << endl;
			SgForStatement* current = isSgForStatement(p);
			ROSE_ASSERT(current != NULL);




			// TODO: can we split the loop????




			// we only move loop_body
			SgBasicBlock* newChild = isSgBasicBlock(newTree);
			ROSE_ASSERT(newTree != NULL);

			SgForInitStatement* newInitStmt = isSgForInitStatement(current->get_for_init_stmt()->copy(SgTreeCopy::static_instance()));
			ROSE_ASSERT(newInitStmt != NULL);

			SgStatement* newTest = isSgStatement(current->get_test()->copy(SgTreeCopy::static_instance()));
			ROSE_ASSERT(newTest != NULL);

			SgExpression* newIncrement = isSgExpression(current->get_increment()->copy(SgTreeCopy::static_instance()));

			SgForStatement* newForStmt = new SgForStatement(fileInfo, newTest, newIncrement, newChild);
			newForStmt->append_init_stmt(newInitStmt);

			newInitStmt->set_parent(newForStmt);
			newTest->set_parent(newForStmt);
			newIncrement->set_parent(newForStmt);
			newChild->set_parent(newForStmt);

			newTree = newForStmt;

			// delete the for loop?
		}
		else if(isSgWhileStmt(p))
		{
			SgWhileStmt* current = isSgWhileStmt(p);





			// TODO: can we split the loop????






			// delete this while statement if it's empty
			bool simple_condition = false;
			bool empty_body = false;

			// does condition not have assignment and function call?
			SgStatement* condition = current->get_condition();
			list<SgNode*> assignmentList = NodeQuery::querySubTree(condition, V_SgAssignOp);
			list<SgNode*> callSiteList = NodeQuery::querySubTree(condition, V_SgFunctionCallExp);

			if(assignmentList.empty() && callSiteList.empty())
				simple_condition = true;

			// is body empty?
			SgBasicBlock* leftOverBody = current->get_body();
			SgStatementPtrList leftOverBodyList = leftOverBody->get_statements();
			if(leftOverBodyList.empty())
				empty_body = true;

			if(simple_condition && empty_body && isSgBasicBlock(parent) && p != startPath)
			{
				SgStatementPtrList& stmtList = isSgBasicBlock(parent)->get_statements();
				stmtList.remove(isSgStatement(p));
			}
		}
		else if(isSgDoWhileStmt(p))
		{
			SgDoWhileStmt* current = isSgDoWhileStmt(p);





			// TODO: can we split the loop????




			// delete this do-while statement if it's empty
			bool simple_condition = false;
			bool empty_body = false;

			// does condition not have assignment and function call?
			SgStatement* condition = current->get_condition();

			list<SgNode*> assignmentList = NodeQuery::querySubTree(condition, V_SgAssignOp);
			list<SgNode*> callSiteList = NodeQuery::querySubTree(condition, V_SgFunctionCallExp);

			if(assignmentList.empty() && callSiteList.empty())
				simple_condition = true;

			// is body empty?
			SgBasicBlock* leftOverBody = current->get_body();
			SgStatementPtrList leftOverBodyList = leftOverBody->get_statements();
			if(leftOverBodyList.empty())
				empty_body = true;

			// then remove
			if(simple_condition && empty_body && isSgBasicBlock(parent) && p != startPath)
			{
				SgStatementPtrList& stmtList = isSgBasicBlock(parent)->get_statements();
				stmtList.remove(isSgStatement(p));
			}
		}
		else
		{
			cout << "Han: The name of this node is: " << p->class_name() << endl;
		}

		if(p == startPath)
			break;

		// iteration, update pointers upward
		child = p;
		if(p->get_parent())
			p = p->get_parent();

		if(p && p->get_parent())
			parent = p->get_parent();
	}

	cout << "Han: done creating a new nodes" << endl;

	SgBasicBlock *parentBlock = isSgBasicBlock(startPath->get_parent());
	ROSE_ASSERT(parentBlock != NULL);

	SgStatementPtrList& statementList = parentBlock->get_statements();
	SgStatementPtrList::iterator loc = find(statementList.begin(), statementList.end(), startPath);

	if(loc != statementList.end())
	{
		SgStatement* newStmt = isSgStatement(newTree);
		ROSE_ASSERT(newStmt != NULL);


		newStmt->set_parent(parentBlock);

		// set the start statement (outer most scope) of the block to a new statement
		blockInfo->setSliceStart(newStmt);
		blockInfo->setScopeDependence();
		blockInfo->setFunctionDefinition();


		// inserting new tree into the parent basic block
		statementList.insert(loc, newStmt);

		// if startPath is an empty block, 
		// then delete the block
		// here empty means, an if statement with no statements in both true and false body
		// and no function call exp or assignment in conditional.

		//cleanUpAfterMove(startPath);

		//statementList.erase(loc);

	} else {
		// can't reach here??
		cout << "Han: there is no startPath under its parent??" << endl;
		ROSE_ASSERT(0);
	}
#endif
}

void MPINonBlockingCodeMotion::cleanUpAfterMove(SgNode* startPath)
{
	ROSE_ASSERT(startPath != NULL);

	SgBasicBlock* parent = isSgBasicBlock(startPath->get_parent());
	cout << "Han: cleaning from " << parent->unparseToString() << endl;
	SgStatementPtrList& statementList = parent->get_statements();

	if(!parent)
	{
		cout << "Han: the parent of startPath is not SgBasicBlock" << endl;
		return;
	}

	if(isSgBasicBlock(startPath))
	{
		SgStatementPtrList& leftOverStmtList = isSgBasicBlock(startPath)->get_statements();
		if(leftOverStmtList.empty() == true)
		{
			// delete startPath
			//statementList.remove(isSgStatement(startPath));
			SgStatementPtrList::iterator removeLoc = find(statementList.begin(), statementList.end(), startPath);

      // DQ (9/26/2007): Moved from std::list to std::vector uniformly in ROSE.
      // statementList.remove(*removeLoc);
			statementList.erase(removeLoc);
		}
	}
	else if(isSgIfStmt(startPath))
	{
		bool simple_conditional = false;
		bool true_body_empty = false;
		bool false_body_empty = false;

		cout << "Han: cleaning up : " << startPath->unparseToString() << endl;

		// does conditional not have assignments and function calls?
		SgStatement* leftOverCond = isSgIfStmt(startPath)->get_conditional();
		Rose_STL_Container<SgNode*> assignmentList = NodeQuery::querySubTree(leftOverCond, V_SgAssignOp);
		Rose_STL_Container<SgNode*> callSiteList = NodeQuery::querySubTree(leftOverCond, V_SgFunctionCallExp);

		if(assignmentList.empty() && callSiteList.empty())
		{
			cout << "Han: simple conditional" << endl;
			simple_conditional = true;
		}

		// is true body empty?
		if(isSgBasicBlock(isSgIfStmt(startPath)->get_true_body()) && isSgBasicBlock(isSgIfStmt(startPath)->get_true_body())->get_statements().empty())
		{
			cout << "Han: empty true body" << endl;
			true_body_empty = true;
		}

		// is false body empty?
		if(isSgIfStmt(startPath)->get_false_body() == NULL || (isSgBasicBlock(isSgIfStmt(startPath)->get_false_body()) && isSgBasicBlock(isSgIfStmt(startPath)->get_false_body())->get_statements().empty()))
		{
			cout << "Han: empty false body" << endl;
			false_body_empty = true;
		}

		if(simple_conditional && true_body_empty && false_body_empty)
		{
			// delete startPath
			cout << "Han: removing " << startPath->unparseToString() << endl;
			cout << "Han: before " << statementList.size() << endl;
			//startPath->set_parent(NULL);
			//statementList.remove(isSgStatement(startPath));

			SgStatementPtrList::iterator removeLoc = find(statementList.begin(), statementList.end(), startPath);
			statementList.erase(removeLoc);
			cout << "Han: after " << statementList.size() << endl;
		}
	}
	else if(isSgForStatement(startPath))
	{
		// don't know what to do..
		// TODO: implement this....................................................

	}
	else if(isSgWhileStmt(startPath))
	{
		bool simple_condition = false;
		bool empty_body = false;

		// does condition not have assignment and function call?
		SgStatement* condition = isSgWhileStmt(startPath)->get_condition();
		Rose_STL_Container<SgNode*> assignmentList = NodeQuery::querySubTree(condition, V_SgAssignOp);
		Rose_STL_Container<SgNode*> callSiteList = NodeQuery::querySubTree(condition, V_SgFunctionCallExp);

		if(assignmentList.empty() && callSiteList.empty())
			simple_condition = true;

		// is body empty?
		SgStatement* leftOverBody = isSgWhileStmt(startPath)->get_body();
		if(isSgBasicBlock(leftOverBody) && isSgBasicBlock(leftOverBody)->get_statements().empty())
			empty_body = true;

		if(simple_condition && empty_body)
      {
     // statementList.remove(isSgStatement(startPath));
        statementList.erase(find(statementList.begin(),statementList.end(),isSgStatement(startPath)));
      }
	}
	else if(isSgDoWhileStmt(startPath))
	{
		bool simple_condition = false;
		bool empty_body = false;

		// does condition not have assignment and function call?
		SgStatement* condition = isSgWhileStmt(startPath)->get_condition();

		Rose_STL_Container<SgNode*> assignmentList = NodeQuery::querySubTree(condition, V_SgAssignOp);
		Rose_STL_Container<SgNode*> callSiteList = NodeQuery::querySubTree(condition, V_SgFunctionCallExp);

		if(assignmentList.empty() && callSiteList.empty())
			simple_condition = true;

		// is body empty?
		SgStatement* leftOverBody = isSgDoWhileStmt(startPath)->get_body();
		if(isSgBasicBlock(leftOverBody) && isSgBasicBlock(leftOverBody)->get_statements().empty())
			empty_body = true;

		// then remove
		if(simple_condition && empty_body)
      {
     // statementList.remove(isSgStatement(startPath));
        statementList.erase(find(statementList.begin(),statementList.end(),isSgStatement(startPath)));
      }
	}
}

bool MPINonBlockingCodeMotion::isMovableOutside(SgBasicBlock* block, SgNode* statement)
{
	// TODO: implement this function

	 return false;
}


std::string MPINonBlockingCodeMotion::getFunctionCallName(SgExprStatement* expr)
{
	ROSE_ASSERT(expr!= NULL);

	if(expr->get_expression() == NULL)
		return string("");

	SgFunctionCallExp* functionExpression = isSgFunctionCallExp(expr->get_expression());

	if(functionExpression == NULL || functionExpression->get_function() == NULL)
		return string("");

	SgFunctionRefExp* functionRefExp = isSgFunctionRefExp(functionExpression->get_function());

	if(functionRefExp != NULL)
	{
		SgFunctionSymbol* functionSymbol = functionRefExp->get_symbol();
		if(functionSymbol->get_name() != NULL)
			return functionSymbol->get_name().getString();
		else 
			return string("");
	} 
	else {
		return string("");
	}
}

bool MPINonBlockingCodeMotion::isScopeStatement(SgNode* node)
{
	return (isSgIfStmt(node) || isSgForStatement(node) || isSgDoWhileStmt(node) || isSgWhileStmt(node));
}


/*
MotionInformation* MPINonBlockingCodeMotion::getExistingMotionInfo(SgExprStatement* src)
{
	ROSE_ASSERT(src != NULL);

	std::list<MotionInformation*>::iterator infoItr;

	for(infoItr = motionInfoList.begin(); infoItr != motionInfoList.end(); infoItr++)
	{
		MotionInformation* info = (*infoItr);

		if(info->getSrc() == src)
			return info;
	}

	return NULL;
}


MotionInformation* MPINonBlockingCodeMotion::getMotionInfo(SgExprStatement* src)
{
	ROSE_ASSERT(src != NULL);

	MotionInformation* info = getExistingMotionInfo(src);

	if(info)
		return info;
	else
		return new MotionInformation(src);
}
*/




			//cout << "Han: Looking into an SDG node at " << SDGNode->getSgNode()->get_startOfConstruct()->get_line() << ", type: " << SDGNode->getType() << endl;
			//if(isSgExprStatement(SDGNode->getSgNode()) != NULL)
				//cout << "Han: The SDG node is an SgExprStatement" << endl;


			//cout << "Han: #parameter for a recv call: " << parameterNodeSet.size() << endl;
					//cout << "Han: ====================================" << endl;
					//cout << "Han: For a parameter at " << parameterNode->getSgNode()->get_startOfConstruct()->get_line() << endl;

								/*
								cout << "Han: ACTUALIN node" << endl;
								cout << "Han: Definition: "; parentNode->writeOut(cout);
								cout << endl << "Han: defined at " << parentNode->getSgNode()->get_startOfConstruct()->get_line() << endl;
								cout << "Han: Parameter: " << parameterNode->getName() << "(" << parameterNode->getType() << ")" << endl;
								*/

						/*
						cout << "Han: Not an ACTUALIN node" << endl;
						cout << "Han: Parameter Node Type: " << parameterNode->getType() << endl;
						*/

			/*
			SgNode* ancestor = leastCommonAncestor(recvCall, mpiCall);

			bool isInSameBlock = true;

			if(ancestor != NULL)
			{
				for(SgNode* p = recvCall; p != ancestor; p = p->get_parent())
				{
					if(p->class_name() == "SgBasicBlock")
					{
						cout << "Han: Different blocks" << endl;
						isInSameBlock = false;
						break;
					}
				}

				for(SgNode* p = mpiCall; p != ancestor; p = p->get_parent())
				{
					if(p->class_name() == "SgBasicBlock")
					{
						cout << "Han: Different blocks" << endl;
						isInSameBlock = false;
						break;
					}
				}
			}

			if(isInSameBlock == true)
			{
				// as two statements are in the same block, 
				// the second cannot be moved beyond the first statement
				// later it will be ordered which one is executed before the other
				// see MotionInformation::getMaximumNode()


				cout << "Han: recvCall at " << recvCall->get_startOfConstruct()->get_line() << " depends on the MPI call at " << mpiCall->get_startOfConstruct()->get_line() << endl;

				// add this information into a list
				info->addMPIDependence(mpiCall);
			}
		*/

			//cout << "Han: recvCall at " << commCall->get_startOfConstruct()->get_line() << " depends on the MPI call at " << mpiCall->get_startOfConstruct()->get_line() << endl;

#if 0
void BlockMotionInformation::computeDependence(SystemDependenceGraph* sdg)
{
	// here we find all the dependencies 
	// for the variables appearing under sliceStart, 

	// pick up variables
	ROSE_ASSERT(sdg != NULL);
	ROSE_ASSERT(sliceStart != NULL);
	ROSE_ASSERT(child != NULL);

	cout << "Han: sliceStart " << sliceStart->unparseToString() << endl;

	// copy all the dependencies to the current list
	findDataDependence(sdg);

	/*
	copy(child->getDataDependenceList()->begin(), 
			child->getDataDependenceList()->end(), 
			back_inserter(dataDependenceList));

	// 1. for data dependence list
	for(vector<SgNode*>::iterator i = dataDependenceList.begin(); 
			i != dataDependenceList.end(); i++)
	{
		SgNode* dependenceNode = isSgNode(*i);
		SgNode* commonAncestor = leastCommonAncestor(dependenceNode, sliceStart);

		// if dependenceNode is underneath sliceStart
		if(!commonAncestor || commonAncestor == sliceStart)
		{
			// delete it!
			cout << "Han: deleting from list: " << dependenceNode->unparseToString() << endl;
			dataDependenceList.erase(i);
		}
	}


			*/

	copy(child->getMPIDependenceList()->begin(), 
			child->getMPIDependenceList()->end(), 
			back_inserter(mpiDependenceList));

	copy(child->getBlockDependenceList()->begin(), 
			child->getBlockDependenceList()->end(), 
			back_inserter(blockDependenceList));


	// now that we just copied from child,
	// we have to filter out unnecessary information,
	// for example, dependencies inside the scope
	// 2. for mpi dependence list
	for(vector<SgNode*>::iterator i = mpiDependenceList.begin(); 
			i != mpiDependenceList.end(); i++)
	{
		SgNode* dependenceNode = isSgNode(*i);
		SgNode* commonAncestor = leastCommonAncestor(dependenceNode, sliceStart);

		// if dependenceNode is underneath sliceStart
		if(!commonAncestor || commonAncestor == sliceStart)
		{
			// delete it!
			cout << "Han: deleting from list: " << dependenceNode->unparseToString() << endl;
			mpiDependenceList.erase(i);
		}
	}

	// 3. for block dependence list
	for(vector<SgNode*>::iterator i = blockDependenceList.begin(); 
			i != blockDependenceList.end(); i++)
	{
		SgNode* dependenceNode = isSgNode(*i);
		SgNode* commonAncestor = leastCommonAncestor(dependenceNode, sliceStart);

		// if dependenceNode is underneath sliceStart
		if(!commonAncestor || commonAncestor == sliceStart)
		{
			// delete it!
			cout << "Han: deleting from list: " << dependenceNode->unparseToString() << endl;
			blockDependenceList.erase(i);
		}
	}





	// 4. we add up additional dependence list as we add statements wrapping the original MPI calls

	list<SgNode*> stmtList = NodeQuery::querySubTree(sliceStart, V_SgExprStatement);

	// find dependencies of the variable that are outer scope from sliceStart
	for(list<SgNode*>::iterator i = stmtList.begin(); i != stmtList.end(); i++)
	{
		cout << "Han: SgExprStatement: " << (*i)->unparseToString() << endl;
		DependenceNode* expr = sdg->getExistingNode(*i);

		// TODO: you have to consider if this statement is 
		// 1) a function call, 2) expression statement, 3) ??
		if(expr != NULL)
		{
			set<SimpleDirectedGraphNode*> parentOfExpr = expr->getPredecessors();

			for(set<SimpleDirectedGraphNode*>::iterator parentOfExprIt = parentOfExpr.begin();
					parentOfExprIt != parentOfExpr.end(); parentOfExprIt++)
			{
				DependenceNode* parentNode = dynamic_cast<DependenceNode*>(*parentOfExprIt);

				if(sdg->edgeExists(parentNode, expr, DependenceGraph::DATA))
				{
					// check whether this parent node is in outer scope than sliceStart
					SgNode* commonAncestor = leastCommonAncestor(parentNode->getSgNode(), sliceStart);
					
					if(commonAncestor && 
							commonAncestor != parentNode->getSgNode() && 
							commonAncestor != sliceStart)
					{
						cout << "Han: DD added: " << parentNode->getSgNode()->unparseToString() << endl;
						dataDependenceList.push_back(parentNode->getSgNode());
					}
					else
					{
						cout << "Han: the depen is inside the scope: " << parentNode->getSgNode()->unparseToString() << endl;
					}
				}
				else
				{
					cout << "Han: this is not data dep edge: " << parentNode->getSgNode()->unparseToString() << endl;
				}
			}
		} 
		else
		{
			cout << "Han: SgExprStatement does not exist in sdg" << endl;
		}
	}

	// TODO: if there is a function call, 



}
#endif


