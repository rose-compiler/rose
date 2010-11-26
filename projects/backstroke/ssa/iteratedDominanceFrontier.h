#pragma once

#include "backstrokeCFG.h"
#include "sage3basic.h"
#include <vector>
#include <set>
#include <map>
#include <iterator>
#include <boost/foreach.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/topological_sort.hpp>

#define foreach BOOST_FOREACH
#define reverse_foreach BOOST_REVERSE_FOREACH

namespace ssa_private
{
	using namespace Backstroke;
	using namespace std;
	using namespace boost;

	template<class CfgNodeT, class CfgEdgeT>
	void iteratedDominanceFrontier(SgFunctionDefinition* func, const vector<CfgNodeT> startNodes )
	{
		typedef CFG<CfgNodeT, CfgEdgeT> ControlFlowGraph;
		//Build a CFG first
		ControlFlowGraph functionCfg(func);

		functionCfg.toDot("boost_filtered_cfg.dot");

		//Build the dominator tree
		typename ControlFlowGraph::VertexVertexMap dominatorTreeMap = functionCfg.buildDominatorTree();

		//TODO: This code converts a VertexVertex Map to a  boost graph. Should be factored out
		typedef adjacency_list<vecS, vecS, bidirectionalS, CfgNodeT> TreeType;
		TreeType domTree;
		typedef typename graph_traits<TreeType>::vertex_descriptor TreeVertex;
		
		set<CfgNodeT> addedNodes;
		map<CfgNodeT, TreeVertex> cfgNodeToVertex;

		foreach(typename ControlFlowGraph::VertexVertexMap::value_type& nodeDominatorPair, dominatorTreeMap)
		{
			CfgNodeT node = *functionCfg[nodeDominatorPair.first];
			CfgNodeT dominator = *functionCfg[nodeDominatorPair.second];
			printf("%s is immediately dominated by %s.\n", node.toStringForDebugging().c_str(),
				dominator.toStringForDebugging().c_str());

			if (addedNodes.count(dominator) == 0)
			{
				TreeVertex newVertex = add_vertex(domTree);
				cfgNodeToVertex[dominator] = newVertex;
				domTree[newVertex] = dominator;
				addedNodes.insert(dominator);
			}

			if (addedNodes.count(node) == 0)
			{
				TreeVertex newVertex = add_vertex(domTree);
				cfgNodeToVertex[node] = newVertex;
				domTree[newVertex] = node;
				addedNodes.insert(node);
			}

			//Add the edge from dominator to node
			add_edge(cfgNodeToVertex[dominator], cfgNodeToVertex[node], domTree);
		}

		//Get a topological ordering of the vertices
		vector<TreeVertex> reverseTopological;
		topological_sort(domTree, back_inserter(reverseTopological));

		printf("Nodes in topological order:\n");
		reverse_foreach(TreeVertex v, reverseTopological)
		{
			printf("%s\n", domTree[v].toStringForDebugging().c_str());
		}
	}

}