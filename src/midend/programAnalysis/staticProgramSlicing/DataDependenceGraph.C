#include "rose.h"

#include "DependenceGraph.h"

#include <list>
using namespace std;

DataDependenceGraph::DataDependenceGraph(SgNode * head, InterproceduralInfo * ii) 
  : _head(isSgFunctionDefinition(head))
{
  _interprocedural = ii;
  _buildDefUseChains(_head);
  _buildDDG();
}

void DataDependenceGraph::_buildDefUseChains(SgFunctionDefinition * fD) {

  SgBasicBlock * stmts = fD->get_body();
  AstInterface astInt(stmts);
  StmtVarAliasCollect alias;
  alias(astInt, fD);
  ReachingDefinitionAnalysis r;
  r(astInt, fD);
  _defuse.build(astInt, r, alias);
}

void DataDependenceGraph::_buildDDG() {

	//Iterate through the edges
	//CI (12/28/2006): Rewrote the edge iterator to be a double loop using only know iterators
	for (DefaultDUchain::NodeIterator nodeItr = _defuse.GetNodeIterator();!nodeItr.ReachEnd();nodeItr++)
	// iterat over all outgoing edges of that node
	for (DefaultDUchain::EdgeIterator edgeItr = _defuse.GetNodeEdgeIterator(*nodeItr,DefaultDUchain::EdgeOut);!edgeItr.ReachEnd();edgeItr++)
	{
//	for (DefaultDUchain::EdgeIterator edges = _defuse.GetEdgeIterator();			!edges.ReachEnd(); ++edges) 
	{

		//Get the edge
	//	GraphEdge * e = *edges;

		//get the "def" node and the "use" node
		DefUseChainNode * defnode =*nodeItr;// _defuse.GetEdgeEndPoint(*edges, DefaultDUchain::EdgeOut);
		DefUseChainNode * usenode =_defuse.GetEdgeEndPoint(*edgeItr,DefaultDUchain::EdgeIn);// _defuse.GetEdgeEndPoint(*edges, DefaultDUchain::EdgeIn);

		SgNode * defref = defnode->get_ref();
		SgNode * useref = usenode->get_ref();
		//     printf("Found Def-Use edge\n");
		//     printf("Def: %p(%s) Use: %p(%s)\n",
		// 	   defref,
		// 	   defref?defref->sage_class_name():"NULL",
		// 	   useref,
		// 	   useref?useref->sage_class_name():"NULL");

		SgNode * def = defnode->get_stmt();
		SgNode * use = usenode->get_stmt();

		DependenceNode * dep_from;
		DependenceNode * dep_to;
		list<DependenceNode *> dep_from_list;
		dep_from_list.clear();
		if (!def) 
		{
			//if the def statement is null, the def comes from the function
			//parameters. If we're not doing interprocedural, then we want
			//to ignore this. Otherwise we want to build the appropriate def
			//links.
			SgInitializedName * function_in = isSgInitializedName(defref);
			ROSE_ASSERT(function_in != NULL);

			//we only want to link these up if interprocedural is being done
			if (_interprocedural != NULL) 
			{
				//set dep_from to actual_in node that matches it
				dep_from = createNode(_interprocedural->procedureEntry.formal_in[function_in]);
			} 
			else 
			{
				dep_from = NULL;
			} 

		}
		else 
		{
			//the def statement isn't null, so we create a new dependence node for it
			dep_from = createNode(def);
			if (_interprocedural != NULL) 
			{
				//if the def statement has function calls in it, we have to handle this carefully
				list<SgFunctionCallExp *> funcCalls = InterproceduralInfo::extractFunctionCalls(def);
				if (!funcCalls.empty()) 
				{
					//@todo This probably needs to do a better job of
					//determining which actual_out return nodes need to be
					//linked to the uses.

					//determine whether this def is from the argument of a
					//function call or if it's defined in the statement itself
					//
					//Eg: x = foo(y); x is defined by the statement. y (if pass
					//by reference) is defined by the function call
					SgNode * funcArg = defnode->get_ref();
					SgFunctionCallExp * funcCall = _findArgExprFromRef(funcArg, def);
					if (funcCall == NULL) 
					{ 
						//the def is part of the statement itself, not a function
						//call within it 

						//in this case, we want to link the actual_out return
						//nodes for each function call to the use (we don't have
						//better information about which function calls are
						//important to the use, unfortunately).
						for (list<SgFunctionCallExp *>::iterator calls = funcCalls.begin(); calls != funcCalls.end(); calls++) 
						{
							dep_from_list.push_back(createNode(_interprocedural->callsite_map[*calls].actual_return));
						}
					} 
					else 
					{
						//need to relate the "out" node for the specific argument to the use
						SgExpression * funcExp = isSgExpression(funcArg);
						ROSE_ASSERT(funcExp != NULL);
						dep_from = createNode(_interprocedural->callsite_map[funcCall].actual_out[funcExp]);
					}
				}
			}
		}
		if (dep_from) 
		{ //if dep_from isn't set, we don't want to build the link
			if (use == NULL) 
			{//If there's no statement, we want to find the SgExpressionRoot that matches it
				use = usenode->get_ref();
				while (!isSgExpressionRoot(use)) 
				{
					use = use->get_parent();
				}
			}

			//determine if this use is connected to a function call
			list<SgFunctionCallExp *> hasCalls = InterproceduralInfo::extractFunctionCalls(use);
			if (!hasCalls.empty()) 
			{
				//there is a function call. First, figure out if the use comes
				//from an argument to the call:

				//find the function call (and argument) that the use was in
				SgNode * funcArg = usenode->get_ref();
				SgFunctionCallExp * funcCall = _findArgExprFromRef(funcArg, use);
				if (funcCall == NULL) 
				{
					//the statement had a function call, but the use was in something else
					dep_to = createNode(use);
				} 
				else 
				{
					SgExpression * funcExp = isSgExpression(funcArg);
					ROSE_ASSERT(funcExp != NULL);
					//we want to point from the def to the actual_in for the function call:
					dep_to = createNode(_interprocedural->callsite_map[funcCall].actual_in[funcExp]);
				} 
			} 
			else 
			{
				//no function calls, so the use is already set correctly
				dep_to = createNode(use);
			}

			//link them
			if (dep_from_list.empty())
			{ //if we aren't supposed to be creating multiple links
				establishEdge(dep_from, dep_to);
			} 
			else
			{
				list<DependenceNode *>::iterator deps;
				for (deps = dep_from_list.begin(); deps != dep_from_list.end(); deps++) 
				{
					establishEdge(*deps, dep_to);
				}
			}
		}
	}
}
	if (_interprocedural)
		_processReturns();
}

void DataDependenceGraph::_processReturns() {

  //go through all the nodes of the graph. If any of them are return
  //statements, the formal_out return node should be linked to
  set<SimpleDirectedGraphNode *>::iterator node_iterator;
  for (node_iterator = _nodes.begin(); node_iterator != _nodes.end(); node_iterator++) {
    DependenceNode * node = dynamic_cast<DependenceNode *>(* node_iterator);
    ROSE_ASSERT(node != NULL);
    if (node->getType() == DependenceNode::SGNODE) {
      SgNode * sgnode = node->getNode();
      if (isSgReturnStmt(sgnode)) {
	DependenceNode * return_node = createNode(_interprocedural->procedureEntry.formal_return);
	establishEdge(node, return_node);
      }
    }
  }
}

SgFunctionCallExp * DataDependenceGraph::_findArgExprFromRef(SgNode * &funcArg, SgNode * use) {

  //move up the chain from funcArg until either the parent is an
  //SgExprListExp (i.e. the argument list) or is == use (i.e. funcArg
  //wasn't part of a function call)
  while (!isSgExprListExp(funcArg->get_parent()) && (funcArg->get_parent() != use)) {
    funcArg = funcArg->get_parent();
  }

  //if it's == use, then return NULL
  if (funcArg->get_parent() == use) return NULL;

  //otherwise figure out which functioncallexp the argument is in
  SgFunctionCallExp * funcCall = isSgFunctionCallExp(funcArg->get_parent()->get_parent());
  ROSE_ASSERT(funcCall != NULL);

  return funcCall;

}

#define DEFUSECHAIN_TEMPLATE_ONLY
#include <DefUseChain.C>
template class DefUseChain<DefUseChainNode>;
