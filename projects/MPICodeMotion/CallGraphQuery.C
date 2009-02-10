#include "rose.h"
#include "CallGraphQuery.h"

using namespace std;

CallGraphQuery::CallGraphQuery(SgProject* project)
{
  cgBuilder = new CallGraphBuilder(project);
}

void CallGraphQuery::initialize()
{
  cgBuilder->buildCallGraph();
  cgBuilder->classifyCallGraph();
}

bool CallGraphQuery::pathExist(CallGraphCreate::Node* from, CallGraphCreate::Node* to)
{
  //cout << "from " << from->functionDeclaration->unparseToString() << endl;
  //cout << "to " << to->functionDeclaration->unparseToString() << endl;

  // base case
  if(from == to)
  {
	//std::cout << "base case: " << from->functionDeclaration->unparseToString() << endl;
	return true;
  }
  // recursive case
  else
  {
	CallGraphCreate::Node* node;
	CallGraphCreate::EdgeIterator edgeItr;

	edgeItr = cgBuilder->getGraph()->GetNodeEdgeIterator(from, GraphAccess::EdgeOut);
	if(edgeItr.ReachEnd())
	{
	  // it's empty
	  //std::cout << "empty" << endl;
	  return false;
	}
	else
	{
	  bool ret = false;

	  while(!edgeItr.ReachEnd())
	  {
		node = cgBuilder->getGraph()->GetEdgeEndPoint(*edgeItr, GraphAccess::EdgeIn);
		//std::cout << "out edge " << node->functionDeclaration->unparseToString() << endl;

		if(node != from)
		  ret = ret || pathExist(node, to);

		if(ret)
		{
		  //cout << "returning true" << endl;
		  return true;
		}

		edgeItr++;
	  }

	  //cout << "no path exists" << endl;
	  return ret;
	}
  }
}

void CallGraphQuery::constructFuncWithMPICallSet(list<SgExprStatement*> mpiCallList)
{
  for(list<SgExprStatement*>::iterator i = mpiCallList.begin(); i != mpiCallList.end(); i++)
  {
	SgExprStatement* stmt = isSgExprStatement(*i);
	ROSE_ASSERT(stmt != NULL);

	SgFunctionCallExp* callSite = isSgFunctionCallExp(stmt->get_expression());
	SgFunctionRefExp* callExpr = isSgFunctionRefExp(callSite->get_function());

	if(callExpr != NULL)
	{
	  CallGraphCreate::Node* mpiCall = NULL;
	  if(!callExpr->get_symbol_i())
		continue;

	  SgFunctionDeclaration* fDef = callExpr->get_symbol_i()->get_declaration();

	  CallGraphCreate::NodeIterator nodeItr = cgBuilder->getGraph()->GetNodeIterator();
	  
	  while(!nodeItr.ReachEnd())
	  {
		CallGraphCreate::Node* current = *nodeItr;
		if(current->functionDeclaration == fDef)
		{
		  mpiCall = current;
		}

		++nodeItr;
	  }

	  if(mpiCall != NULL)
	  {
		DFS(mpiCall, mpiCall->functionDeclaration);
	  }
	}
  }
}

void CallGraphQuery::DFS(CallGraphCreate::Node* p, SgFunctionDeclaration* mpiCall)
{
  CallGraphCreate::EdgeIterator edgeItr = cgBuilder->getGraph()->GetNodeEdgeIterator(p, GraphAccess::EdgeIn);
  while(!edgeItr.ReachEnd())
  {
	CallGraphCreate::Node* node = cgBuilder->getGraph()->GetEdgeEndPoint(*edgeItr, GraphAccess::EdgeOut);

	if(node)
	{
	  funcWithMPICallSet.insert(node->functionDeclaration); 
	  DFS(node, mpiCall);
	}
	++edgeItr;
  }
}

bool CallGraphQuery::hasMPICall(SgExprStatement* callSite)
{
  ROSE_ASSERT(callSite != NULL);

  SgFunctionCallExp* callSiteExp = isSgFunctionCallExp(callSite->get_expression());
  if(callSiteExp == NULL || callSiteExp->get_function() == NULL)
	return true;
  SgFunctionRefExp* callSiteRefExp = isSgFunctionRefExp(callSiteExp->get_function());

  std::set<SgFunctionDeclaration*>::iterator ret = find(funcWithMPICallSet.begin(), funcWithMPICallSet.end(), callSiteRefExp->get_symbol_i()->get_declaration());

  if(ret != funcWithMPICallSet.end())
	return true;

  return false;
}



