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
//  cgBuilder->classifyCallGraph();
}

#if 0
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
#else
bool CallGraphQuery::pathExist(SgGraphNode* from, SgGraphNode* to)
{
    return cgBuilder->getGraph()->checkIfDirectedGraphEdgeExists(from, to);
}

#endif
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
#if 1
	  SgGraphNode* mpiCall = NULL;
	  if(!callExpr->get_symbol_i())
		continue;

	  SgFunctionDeclaration* fDef = callExpr->get_symbol_i()->get_declaration();

          rose_graph_integer_node_hash_map & nodes =
            cgBuilder->getGraph()->get_node_index_to_node_map ();


          for( rose_graph_integer_node_hash_map::iterator it = nodes.begin();
              it != nodes.end(); ++it )
          {

            SgGraphNode* current = it->second;
            if(current->get_SgNode() == fDef)
            {
              mpiCall = current;
            }
          }

	  if(mpiCall != NULL)
	  {
		DFS(mpiCall, isSgFunctionDeclaration(mpiCall->get_SgNode()));
	  }

#else
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
#endif
	}
  }
}

void CallGraphQuery::DFS(SgGraphNode* p, SgFunctionDeclaration* mpiCall)
{
  //I think this function searches over function returns all functions
  //that points to this function
  rose_graph_integer_edge_hash_multimap & inEdges
    = cgBuilder->getGraph()->get_node_index_to_edge_multimap_edgesIn ();

  for( rose_graph_integer_edge_hash_multimap::const_iterator inEdgeIt = inEdges.find(p->get_index());
      inEdgeIt != inEdges.end(); ++inEdgeIt )
  {

    SgDirectedGraphEdge* graphEdge = isSgDirectedGraphEdge(inEdgeIt->second);
    ROSE_ASSERT(graphEdge!=NULL);
    DFS(graphEdge->get_from(),mpiCall);
  }
#if 0
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
#endif
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



