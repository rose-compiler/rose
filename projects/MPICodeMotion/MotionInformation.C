#include "rose.h"
#include "MotionInformation.h"
#include "virtualCFG.h"
#include "filteredCFG.h"
#include "DominatorTree.h"
#include "DominanceFrontier.h"


using namespace std;
using namespace MPIOptimization;
using namespace VirtualCFG;

/**********************************************************************
* Class MotionInformation Implementation
*
*
**********************************************************************/

struct IsStatementFilter
{
    bool operator() (CFGNode cfgn) const
    {
        // get rid of all beginning nodes
        if (!cfgn.isInteresting())
            return false;

        SgNode *n = cfgn.getNode();
        if (isSgExprStatement(n))
              return true;
        if (isSgReturnStmt(n))
              return true;

        // the following should not be necessary
        if (isSgFunctionDefinition(n))
              return true;

				// Han added
				if(isSgVariableDeclaration(n))
					return true;

				if(isSgScopeStatement(n))
					return true;

        // check for for-loop incremental expression
        if ((isSgBinaryOp(n) || isSgUnaryOp(n)) && isSgExpression(n))
        {
            // if the parent is a for statement, then true else false
            SgNode *current = n;
            while (isSgExpression(current->get_parent()))
            {
                current = current->get_parent();
            }
            if (isSgForStatement(current->get_parent()))
                  return true;
        }
        return false;
    }
};

using namespace DominatorTreesAndDominanceFrontiers;

MotionInformation::MotionInformation(SgExprStatement* _src, CallGraphQuery* _cgQuery)
{
	src = _src;
	SgNode* p = src;

	while(p && !isSgScopeStatement(p))
		p = p->get_parent();

	scopeDependence = isSgScopeStatement(p);

	p = src;
	while(p && !isSgFunctionDefinition(p))
		p = p->get_parent();

	fDef = isSgFunctionDefinition(p);

	insertAfterDest = true;

	ROSE_ASSERT(fDef != NULL);

	cgQuery = _cgQuery;
}

MotionInformation::~MotionInformation()
{
	dataDependenceList.clear();
	mpiDependenceList.clear();
	blockDependenceList.clear();
	callSiteDependenceList.clear();

	src = NULL;
	scopeDependence = NULL;
	fDef = NULL;
}


void MotionInformation::findDataDependence(SystemDependenceGraph* sdg)
{
	DependenceNode* SDGNode;
	set<SimpleDirectedGraphNode*> parameterNodeSet;
	set<SimpleDirectedGraphNode*>::iterator parameterNodeSetItr;

	// for each recv call, picks up only paramter nodes
	SDGNode = sdg->getExistingNode(src);

	if(SDGNode != NULL)
	{
		// get a set of successors of a SDGNode (=MPI Recv call)
		// it could contain parameters of the MPI recv call and also contain other nodes
		// so we have to filter out nodes that are not parameters
		parameterNodeSet = SDGNode->getSuccessors();

		// for each parameters
		for(parameterNodeSetItr=parameterNodeSet.begin(); parameterNodeSetItr != parameterNodeSet.end(); parameterNodeSetItr++)
		{
			// if there is a CONTROL edge between two nodes,
			// this is the way to filter out irrelevant nodes
			if(sdg->edgeExists(SDGNode, (dynamic_cast<DependenceNode*>(*parameterNodeSetItr)), DependenceGraph::CONTROL))
			{
				DependenceNode* parameterNode = sdg->getExistingNode(DependenceNode::ACTUALIN, (dynamic_cast<DependenceNode*>(*parameterNodeSetItr))->getSgNode());

				// and if the child node is an ACTUALIN node,
				if(parameterNode != NULL)
				{
					// find the definition of this parameter
					set<SimpleDirectedGraphNode*> parentOfParameter = parameterNode->getPredecessors();
					//cout << "Han: #Parent: " << parentOfParameter.size() << endl;

					for(set<SimpleDirectedGraphNode*>::iterator parentOfParameterIt = parentOfParameter.begin();
							parentOfParameterIt != parentOfParameter.end(); parentOfParameterIt++)
					{
						DependenceNode* parentNode = dynamic_cast<DependenceNode*>(*parentOfParameterIt);

						if(sdg->edgeExists(parentNode, parameterNode, DependenceGraph::DATA))
						{
							// definition!
							// store this information
							cout << "Han: Data Dependence " << parentNode->getSgNode()->unparseToString() << endl;
							dataDependenceList.push_back(parentNode->getSgNode());

						} else {
							// ignore other edges
							//cout << "Han: parent node type: " << parentNode->getType() << endl;
						}
					}
				} else {
					//Can't reach here because all the nodes reachable from a CONTROL egde is ACTUALIN??
				}
			} // end of if(sdg-> ...
		} // end of for(parameterNodeSetItr ...
	} // end of if(SDGNode != NULL)
	else {
		// can't be reached here, I guess..
		cerr << "Han: MPI receive call is not in SDG." << endl;
	}
}



// Step 2.
// find the data dependencies for each MPI communication calls using System Dependence Graph
// 
// for send calls, not implemented yet
void MotionInformation::findMPICallDependence(list<SgExprStatement*>* mpiFunctionCalls)
{
	// we check whether those two functions are defined in the same function definition
	SgNode* p = src;
	while(p && !isSgFunctionDefinition(p))
		p = p->get_parent();

	cout << "Han: mpiFunctionCalls size: " << mpiFunctionCalls->size() << endl;

	for(list<SgExprStatement*>::iterator mpiCallItr = mpiFunctionCalls->begin(); 
			mpiCallItr != mpiFunctionCalls->end(); mpiCallItr++)
	{
		SgNode* mpiCall = *mpiCallItr;
		SgNode* q = mpiCall;
		while(q && !isSgFunctionDefinition(q))
			q = q->get_parent();

		if(p && q && p == q)
		{
			// add this information into a list
			mpiDependenceList.push_back(mpiCall);
			//cout << "Han: push_back " << mpiCall->unparseToString() << endl;
			ROSE_ASSERT(mpiCall->get_parent() != NULL);
		}
	} // end of inner for-loop
}

void MotionInformation::findBlockDependence()
{
	// if any of statements that has a dependence with a MPI recv call,
	// and if the statement is inside a block, e.g. if statement, for statement
	// we set the block as a limit

	// for each MPI Send call
	std::vector<SgNode*> dependenceList;

	copy(dataDependenceList.begin(), dataDependenceList.end(), back_inserter(dependenceList));
	copy(mpiDependenceList.begin(), mpiDependenceList.end(), back_inserter(dependenceList));

	for(vector<SgNode*>::iterator j = dependenceList.begin(); j != dependenceList.end(); j++)
	{
		SgNode* dependence = isSgNode(*j);
		ROSE_ASSERT(dependence != NULL);

		// if there exists a if,for,dowhile and so on on the path from leasCommonAncestor to dependence,
		// we add block statement into control dependence list

		SgNode* ancestor = leastCommonAncestor(src, dependence);
		//cout << "Han: common ancestor from findBlockDependence() " << ancestor->unparseToString() << endl;

		/////////////////////////////////////////////////////
		// From dependence node
		SgNode* p = dependence;
		SgNode* blockDependence = NULL;
		while(p && p != ancestor)
		{
			if(isSgScopeStatement(p))
				blockDependence = p;

			p = p->get_parent();
		}

		if(blockDependence)
		{
			cout << "Han: adding block dependency from dep node: " << blockDependence->unparseToString() << endl;
			blockDependenceList.push_back(blockDependence);
		}
	}
}

void MotionInformation::findCallSiteDependence(SystemDependenceGraph* sdg)
{
	DependenceNode* srcSDGNode = sdg->getExistingNode(src);
	set<SimpleDirectedGraphNode*> srcParamSet = srcSDGNode->getPredecessors();

	Rose_STL_Container<SgNode*> callSiteList = NodeQuery::querySubTree(fDef, V_SgFunctionCallExp);

	cout << "Han: call site # " << callSiteList.size() << endl;



	for(Rose_STL_Container<SgNode*>::iterator i = callSiteList.begin(); i != callSiteList.end(); i++)
	{

		////////////////////////////////////////////////////////////
		// 1. Data dependence for each call site
		// - find function call that update variables inside the function
		//
		// (SgExprStatement)->:CONTROL->(ACTUALOUT)->:DATA->(ACTUALIN)->:CONTROL->(SgExprStatement)
		// 
		// The first SgExprStatement stands for a user defined call site, e.g. allocateBuffer(char*p);
		// The variable defined inside the function call has ACTUALOUT node.
		// ACTUALIN nodes are the actual parameter passed to MPI function call.
		// If there is a data edge between ACTUALOUT nodes and ACTUALIN nodes, then we can say
		// the user defined function call should be executed before the MPI call 
		// because a parameter needed in the MPI call is defined in the user defined functoin call.
		//
		////////////////////////////////////////////////////////////////


		// 
		// for each call site
		SgFunctionCallExp* callExp = isSgFunctionCallExp(*i);
		ROSE_ASSERT(callExp != NULL);

		SgExprStatement* callStmt = isSgExprStatement(callExp->get_parent());
		ROSE_ASSERT(callStmt != NULL);
		cout << "Han: call site statement:" << callStmt->unparseToString() << endl;

		// get list of variables that the function modifies
		DependenceNode* callSiteSDGNode = sdg->getExistingNode(callStmt);
		if(callSiteSDGNode == NULL)
		{
			cout << "Han: callSiteSDGNode is NULL" << endl;
			continue;
		}

		// get successors, among them there are ACTUALOUT
		set<SimpleDirectedGraphNode*> actualOutSet = callSiteSDGNode->getSuccessors();

		//DependenceNode* usageSDGNode = sdg->getExistingNode(src);

		for(set<SimpleDirectedGraphNode*>::iterator j = actualOutSet.begin(); j != actualOutSet.end(); j++)
		{
			if(sdg->edgeExists(callSiteSDGNode, (dynamic_cast<DependenceNode*>(*j)), DependenceGraph::CONTROL))
			{
				// pick only ACTUALOUT node
				// these nodes are the node 
				DependenceNode* actualOutNode = sdg->getExistingNode(DependenceNode::ACTUALOUT, (dynamic_cast<DependenceNode*>(*j))->getSgNode());

				for(set<SimpleDirectedGraphNode*>::iterator k = srcParamSet.begin(); k != srcParamSet.end(); k++)
				{
					DependenceNode* actualInNode = sdg->getExistingNode(DependenceNode::ACTUALIN, (dynamic_cast<DependenceNode*>(*k))->getSgNode());

					if(actualInNode &&
							sdg->edgeExists(actualInNode, srcSDGNode, DependenceGraph::CONTROL) &&
							actualOutNode &&
							sdg->edgeExists(actualOutNode, actualInNode, DependenceGraph::DATA))
					{
						cout << "Han: call site dependence " << callStmt->unparseToString() << endl;
						callSiteDependenceList.push_back(callStmt);
					} 
				}
			} else {
				//cout << "Han: not a control edge, skip: " << (dynamic_cast<DependenceNode*>(*j))->getSgNode()->unparseToString() << endl;
			}
		}

		///////////////////////////////////////////////////////////
		// 2. Find function calls that have MPI calls inside

		// find definition
		if(cgQuery->hasMPICall(callStmt))
			callSiteDependenceList.push_back(callStmt);
	}
}

void MotionInformation::setDest()
{
	ROSE_ASSERT(fDef != NULL);

	// source CFG node
	FilteredCFGNode<IsStatementFilter> srcCFGNode = FilteredCFGNode<IsStatementFilter> (src->cfgForEnd());
	cout << "Han: setDest()::src node: " << srcCFGNode.getNode()->unparseToString() << endl;

	// merge three list into one
	std::vector<SgNode*> mergedList;
	
	copy(dataDependenceList.begin(), dataDependenceList.end(), back_inserter(mergedList));
	copy(mpiDependenceList.begin(), mpiDependenceList.end(), back_inserter(mergedList));
	copy(blockDependenceList.begin(), blockDependenceList.end(), back_inserter(mergedList));
	copy(callSiteDependenceList.begin(), callSiteDependenceList.end(), back_inserter(mergedList));

	// build a dominance tree
	TemplatedDominatorTree<IsStatementFilter> dt(fDef, PRE_DOMINATOR);

	if(mergedList.size() > 0)
	{
		vector<SgNode*>::iterator i = mergedList.begin();

		// pick one node
		SgNode* resultExprStmt = getChildStatement(*i);
		FilteredCFGNode<IsStatementFilter> resultCFGNode = FilteredCFGNode<IsStatementFilter> (resultExprStmt->cfgForEnd());
		//cout << endl << "Han: resultCfgNode: " << resultCFGNode.getNode()->unparseToString() << endl;

		// compare the node and find the node dominated by all other dependence nodes
		for(i = mergedList.begin(); i != mergedList.end(); i++)
		{
			SgNode* aExprStmt = getChildStatement(*i);
			FilteredCFGNode<IsStatementFilter> aCFGNode = FilteredCFGNode<IsStatementFilter> (aExprStmt->cfgForEnd());
			//cout << endl << "Han: a: " << aCFGNode.getNode()->unparseToString() << endl;

			if(dt.dominates(resultCFGNode, aCFGNode) == true)
				cout << "Han: resultCFGNode dominates a" << endl;
			else
				cout << "Han: \"" << aCFGNode.getNode()->unparseToString() << "\" dominates \"" << resultCFGNode.getNode()->unparseToString() << "\"" << endl;

			if(dt.dominates(aCFGNode, srcCFGNode) == true)
				cout << "Han: " << aCFGNode.getNode()->unparseToString() << " dominates srcCFGNode" << endl;

			if(dt.dominates(resultCFGNode, aCFGNode) == true 
					&& resultCFGNode != aCFGNode 
					&& dt.dominates(aCFGNode, srcCFGNode) == true)
			{
				cout << "Han: changing dominator to: " << aCFGNode.getNode()->unparseToString() << endl;
				resultCFGNode = aCFGNode;
			}
		}

		dest = getParentStatement(resultCFGNode.getNode());
		/*
		cout << "Han: setDest()::combined dep " << resultCFGNode.getNode()->unparseToString() << endl;
		cout << "Han: setDest()::combined dep's parent " << resultCFGNode.getNode()->get_parent()->unparseToString() << endl;
		*/

		// ROSE_ASSERT(isSgBasicBlock(scope) != NULL);
		if(isSgBasicBlock(scopeDependence))
		{
			cout << "Han: scopeDependence: " << scopeDependence->unparseToString() << endl;
			SgNode* parent = scopeDependence->get_parent();

			FilteredCFGNode<IsStatementFilter> scopeCFGNode = FilteredCFGNode<IsStatementFilter> (scopeDependence->cfgForEnd());

			if(isSgIfStmt(parent))
			{
				scopeCFGNode = FilteredCFGNode<IsStatementFilter> (isSgIfStmt(parent)->get_conditional()->cfgForEnd());
			} 
			else if( isSgForStatement(parent) )
			{
				scopeCFGNode = FilteredCFGNode<IsStatementFilter> (isSgForStatement(parent)->get_test()->cfgForEnd());
			}
			else if(isSgDoWhileStmt(parent) )
			{
				scopeCFGNode = FilteredCFGNode<IsStatementFilter> (isSgDoWhileStmt(parent)->get_condition()->cfgForEnd());
			}
			else if(isSgWhileStmt(parent))
			{
				scopeCFGNode = FilteredCFGNode<IsStatementFilter> (isSgWhileStmt(parent)->get_condition()->cfgForEnd());
			} 

			// if the last dependency is outside of the innermost scope of the MPI call,
			// we move the MPI call before the first statement of its scope.
			if(dt.dominates(resultCFGNode, scopeCFGNode) == true)
			{
				insertAfterDest = false;
				dest = scopeDependence;

				//cout << "Han: resultCFGNode dominates firstCFGNode" << endl;
				cout << "Han: setDest()::final dest: first statement of " << dest->unparseToString() << endl;
				cout << "Han: parent of dest " << dest->get_parent()->unparseToString() << endl;
			}
		} else {
			cout << "Han: setDest()::scopeDependence is not SgBasicBlock" << endl;
		}
	}
	else
	{
		// can't reach here because parameteres of MPI calls should be defined above, 
		// which make the list size larger than 0
		dest = NULL;
		//return NULL;
	}


}



SgNode* MotionInformation::getChildStatement(SgNode* input)
{
	ROSE_ASSERT(input != NULL);

	if(isSgScopeStatement(input))
	{
		if(isSgIfStmt(input))
			return isSgIfStmt(input)->get_conditional();
		if(isSgForStatement(input))
			return isSgForStatement(input)->get_test();
		if(isSgWhileStmt(input))
			return isSgWhileStmt(input)->get_condition();
		if(isSgDoWhileStmt(input))
			return isSgDoWhileStmt(input)->get_condition();
	} else {
		// this can be a problem. You need to narrow down to specific nodes
		// SgExprStatement or SgVariableDeclaration but what else?
		return input;
	}

	return input;
}

SgNode* MotionInformation::getParentStatement(SgNode* child)
{
	ROSE_ASSERT(child != NULL);

	SgNode* parent = child->get_parent();
	//cout << "Han: parent=" << parent->unparseToString() << endl;

	if(parent)
	{
		if(isSgIfStmt(parent) || isSgForStatement(parent) ||
				isSgWhileStmt(parent) || isSgDoWhileStmt(parent))
		{
			//cout << "Han: returning parent" << endl;
			return parent;
		}
		else {
			return child;
		}
	}
	return child;
}

// reused from virtualCFG.C
SgNode* MotionInformation::leastCommonAncestor(SgNode* a, SgNode* b)
{
	if(a == NULL || b == NULL)
		return NULL;

	//cout << "Han: leastCommonAncestor: a=" << a->unparseToString() << endl;
	//cout << "Han: leastCommonAncestor: b=" << b->unparseToString() << endl;


	// Find the closest node which is an ancestor of both a and b
	vector<SgNode*> ancestorsOfA;
	for (SgNode* p = a; !isSgProject(p); p = p->get_parent()) 
	{
		//cout << "Han: leastcommonancestor p " << p->unparseToString() << endl;
		ancestorsOfA.push_back(p);
	}

	while (b) {
	        vector<SgNode*>::const_iterator i = std::find(ancestorsOfA.begin(), ancestorsOfA.end(), b);
                if (i != ancestorsOfA.end()) return *i;
                b = b->get_parent();
	}
	return NULL;
}


void MotionInformation::setScopeDependence()
{
	ROSE_ASSERT(src != NULL);

	SgNode* p = src;

	while(p && !isSgScopeStatement(p))
		p = p->get_parent();

	scopeDependence = isSgScopeStatement(p);
}

void MotionInformation::setFunctionDefinition()
{
	ROSE_ASSERT(src != NULL);

	SgNode* p = src;
	while(p && !isSgFunctionDefinition(p))
		p = p->get_parent();

	fDef = isSgFunctionDefinition(p);
}



/**********************************************************************
* Class BlockMotionInformation Implementation
*
*
**********************************************************************/
void BlockMotionInformation::findDataDependence(SystemDependenceGraph* sdg)
{
	Rose_STL_Container<SgNode*> exprStmtList = NodeQuery::querySubTree(sliceStart, V_SgExprStatement);

	for(Rose_STL_Container<SgNode*>::iterator i = exprStmtList.begin(); i != exprStmtList.end(); i++)
	{
		// 1. find all the dependence for expression statement,
		// e.g. for the expressoin statement, prev = rank + 1, 
		// we have to find the definition of rank variable

		// for all expression statements
		DependenceNode* stmtNode = sdg->getExistingNode(DependenceNode::SGNODE, *i);

		if(!stmtNode)
			continue;

		set<SimpleDirectedGraphNode*> predecessors = stmtNode->getPredecessors();

		// for each predecessors
		for(set<SimpleDirectedGraphNode*>::iterator j = predecessors.begin();
				j != predecessors.end(); j++)
		{
			DependenceNode* parentNode = dynamic_cast<DependenceNode*>(*j);

			// if there is an DATA edge,
			if(sdg->edgeExists(parentNode, stmtNode, DependenceGraph::DATA))
			{
				SgNode* dependence = NULL;
				SgNode* commonAncestor = NULL;

				// if predecessor is a function call, 
				if(sdg->getExistingNode(DependenceNode::ACTUALOUT, parentNode->getSgNode()))
				{
					if(isSgExprListExp(parentNode->getSgNode()))
						dependence = parentNode->getSgNode()->get_parent();
					else if(isSgFunctionCallExp(parentNode->getSgNode()))
						dependence = parentNode->getSgNode();
				}
				else
				{
					dependence = parentNode->getSgNode();
				}

				// we don't need dependence information inside this block
				commonAncestor = leastCommonAncestor(dependence, sliceStart);
				if(dependence && commonAncestor && commonAncestor != sliceStart)
				{
					dataDependenceList.push_back(dependence);
				}
			}
		}
		
		// 2. find all the dependence for parameters of a function call
		// if a expression statement is a call site, 
		// then we have to find all the definition of parameters used for the function call
		// this part is identical for MotionInformation::findDataDependence(..)

		set<SimpleDirectedGraphNode*> parameterNodeSet = stmtNode->getSuccessors();

		for(set<SimpleDirectedGraphNode*>::iterator j = parameterNodeSet.begin();
				j != parameterNodeSet.end(); j++)
		{
			if(sdg->edgeExists(stmtNode, (dynamic_cast<DependenceNode*>(*j)), DependenceGraph::CONTROL))
			{
				DependenceNode* parameterNode = sdg->getExistingNode(DependenceNode::ACTUALIN, (dynamic_cast<DependenceNode*>(*j))->getSgNode());

				if(parameterNode != NULL)
				{
					set<SimpleDirectedGraphNode*> parentOfParam = parameterNode->getPredecessors();

					for(set<SimpleDirectedGraphNode*>::iterator k = parentOfParam.begin();
							k != parentOfParam.end(); k++)
					{
						DependenceNode* parentNode = dynamic_cast<DependenceNode*>(*k);

						if(sdg->edgeExists(parentNode, parameterNode, DependenceGraph::DATA))
						{
							SgNode* commonAncestor = NULL;

							// again, filter out information inside the block
							commonAncestor = leastCommonAncestor(parentNode->getSgNode(), sliceStart);
							if(commonAncestor && commonAncestor != sliceStart)
								dataDependenceList.push_back(parentNode->getSgNode());
						}
					}
				}
			} // end of if(sdg->...)
		} // end of for(j)
	} // end of for(i..)



}

void BlockMotionInformation::findMPICallDependence(std::list<SgExprStatement*>* mpiFunctionCalls)
{
	MotionInformation::findMPICallDependence(mpiFunctionCalls);

	cout << "Han: mpiDependencelist size: " << mpiDependenceList.size() << endl;
	// filter out information that we don't need it
	for(vector<SgNode*>::iterator i = mpiDependenceList.begin(); i != mpiDependenceList.end(); i++)
	{
		SgNode* dependence = *i;
		//SgNode* commonAncestor = NULL;

		ROSE_ASSERT(dependence != NULL);
		//cout << "Han: mpi calls :" << dependence << endl;

		/*
		commonAncestor = leastCommonAncestor(dependence, sliceStart);
		if(!commonAncestor || commonAncestor == sliceStart)
			mpiDependenceList.erase(i);
			*/
	}
}

#if 0
void BlockMotionInformation::findBlockDependence()
{
	MotionInformation::findBlockDependence();

	// I'm not sure whether I really need this checking
	/*
	cout << "Han: block size: " << blockDependenceList.size() << endl;
	for(vector<SgNode*>::iterator i = blockDependenceList.begin(); i != blockDependenceList.end(); i++)
	{
		SgNode* dependence = *i;
		SgNode* commonAncestor = NULL;

		cout << "Han: " << ++j << endl;
		//cout << "Han: findBlockDependence(): dependence " << dependence->unparseToString() << endl;
		//cout << "Han: findBlockDependence(): sliceStart " << sliceStart->unparseToString() << endl;

		commonAncestor = leastCommonAncestor(dependence, sliceStart);
		if(!commonAncestor || commonAncestor == sliceStart)
			blockDependenceList.erase(i);
	}
	*/
}
#endif


void BlockMotionInformation::setDest()
{

	FilteredCFGNode<IsStatementFilter> sliceCFGNode = FilteredCFGNode<IsStatementFilter> (getChildStatement(sliceStart)->cfgForEnd());
	cout << "Han: setDest::sliceStart node: " << sliceCFGNode.getNode()->unparseToString() << endl;

	std::vector<SgNode*> mergedList;

	copy(dataDependenceList.begin(), dataDependenceList.end(), back_inserter(mergedList));
	copy(mpiDependenceList.begin(), mpiDependenceList.end(), back_inserter(mergedList));
	copy(blockDependenceList.begin(), blockDependenceList.end(), back_inserter(mergedList));

	TemplatedDominatorTree<IsStatementFilter> dt(fDef, PRE_DOMINATOR);

	if(mergedList.size() > 0)
	{
		vector<SgNode*>::iterator i = mergedList.begin();

		SgNode* resultExprStmt = getChildStatement(*i);
		FilteredCFGNode<IsStatementFilter> resultCFGNode = FilteredCFGNode<IsStatementFilter> (resultExprStmt->cfgForEnd());

		for(i = mergedList.begin(); i != mergedList.end(); i++)
		{
			SgNode* aExprStmt = getChildStatement(*i);
			FilteredCFGNode<IsStatementFilter> aCFGNode = FilteredCFGNode<IsStatementFilter> (aExprStmt->cfgForEnd());
			//cout << endl << "Han: a: " << aCFGNode.getNode()->unparseToString() << endl;

			/*
			if(dt.dominates(resultCFGNode, aCFGNode) == true)
				cout << "Han: resultCFGNode dominates a" << endl;
			if(dt.dominates(aCFGNode, sliceCFGNode) == true)
				cout << "Han: a dominates sliceCFGNode" << endl;
				*/

			if(dt.dominates(resultCFGNode, aCFGNode) == true 
					&& resultCFGNode != aCFGNode 
					&& dt.dominates(aCFGNode, sliceCFGNode) == true)
			{
				//cout << "Han: changing dominator to: " << aCFGNode.getNode()->unparseToString() << endl;
				resultCFGNode = aCFGNode;
			}
		}

		dest = getParentStatement(resultCFGNode.getNode());
		//cout << "Han: setDest()::combined dep " << resultCFGNode.getNode()->unparseToString() << endl;
		//cout << "Han: setDest()::combined dep's parent " << resultCFGNode.getNode()->get_parent()->unparseToString() << endl;

		// ROSE_ASSERT(isSgBasicBlock(scope) != NULL);
		//cout << "Han: scopeDependence: " << scopeDependence->unparseToString() << endl;
		if(isSgBasicBlock(scopeDependence))
		{
			SgNode* parent = scopeDependence->get_parent();

			FilteredCFGNode<IsStatementFilter> scopeCFGNode = FilteredCFGNode<IsStatementFilter> (scopeDependence->cfgForEnd());

			if(isSgIfStmt(parent))
			{
				scopeCFGNode = FilteredCFGNode<IsStatementFilter> (isSgIfStmt(parent)->get_conditional()->cfgForEnd());
			} 
			else if( isSgForStatement(parent) )
			{
				scopeCFGNode = FilteredCFGNode<IsStatementFilter> (isSgForStatement(parent)->get_test()->cfgForEnd());
			}
			else if(isSgDoWhileStmt(parent) )
			{
				scopeCFGNode = FilteredCFGNode<IsStatementFilter> (isSgDoWhileStmt(parent)->get_condition()->cfgForEnd());
			}
			else if(isSgWhileStmt(parent))
			{
				scopeCFGNode = FilteredCFGNode<IsStatementFilter> (isSgWhileStmt(parent)->get_condition()->cfgForEnd());
			} 

			// if the last dependency is outside of the innermost scope of the MPI call,
			// we move the MPI call before the first statement of its scope.
			if(dt.dominates(resultCFGNode, scopeCFGNode) == true)
			{
				insertAfterDest = false;
				dest = scopeDependence;

				//cout << "Han: resultCFGNode dominates firstCFGNode" << endl;
				cout << "Han: setDest()::final dest: first statement of " << dest->unparseToString() << endl;
				cout << "Han: parent of dest " << dest->get_parent()->unparseToString() << endl;
			}
		} else {
			cout << "Han: setDest()::scopeDependence is not SgBasicBlock" << endl;
		}


	} 
	else
	{
		dest = NULL;
	}
}

void BlockMotionInformation::setScopeDependence()
{
	ROSE_ASSERT(src != NULL);

	SgNode* p = sliceStart->get_parent();

	while(p && !isSgScopeStatement(p))
		p = p->get_parent();

	scopeDependence = isSgScopeStatement(p);
}


/**********************************************************************
* Class CallSiteMotionInformation Implementation
*
*
**********************************************************************/
