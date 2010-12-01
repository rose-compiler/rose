#pragma once

#include <map>
#include <utility>
#include "sage3basic.h"
#include "staticSingleAssignment.h"
#include <boost/foreach.hpp>

namespace ssa_private
{
	using namespace boost;
	using namespace std;

	/**
	 * @param dominatorTree map from each node in the dom tree to its childrenn
	 * @param iDominatorMap map from each node to its immediate dominator. */
	template<class CfgNodeT, class CfgEdgeT>
	multimap< CfgNodeT, pair<CfgNodeT, CfgEdgeT> >
		calculateControlDependence(SgFunctionDefinition* function, const map<CfgNodeT, CfgNodeT>& iPostDominatorMap)
	{
		//Map from each node to the nodes it's control dependent on (and corresponding edges)
		multimap< CfgNodeT, pair<CfgNodeT, CfgEdgeT> > controlDepdendences;

		//Let's iterate the control flow graph and stop every time we hit an edge with a condition
		set<CfgNodeT> visited;
		set<CfgNodeT> worklist;

		CfgNodeT sourceNode = function->cfgForBeginning();
		worklist.insert(sourceNode);

		while (!worklist.empty())
		{
			//Get the node to work on
			sourceNode = *worklist.begin();
			worklist.erase(worklist.begin());
			visited.insert(sourceNode);

			//For every edge, add it to the worklist 
			BOOST_FOREACH(const CfgEdgeT& edge, sourceNode.outEdges())
			{
				CfgNodeT targetNode = edge.target();

				//Insert the child in the worklist if the it hasn't been visited yet
				if (visited.count(targetNode) == 0)
				{
					worklist.insert(targetNode);
				}

				//Check if we need to process this edge in control dependence calculation
				if (edge.condition() == VirtualCFG::eckUnconditional)
					continue;

				//We traverse from nextNode up in the postdominator tree until we reach the parent of currNode.
				CfgNodeT parent;
				typename map<CfgNodeT, CfgNodeT>::const_iterator parentIter = iPostDominatorMap.find(sourceNode);
				ROSE_ASSERT (parentIter != iPostDominatorMap.end());
				parent = parentIter->second;

				//This is the node that we'll be marking as control dependent
				CfgNodeT currNode = targetNode;
				
				while (true)
				{
					//If we reach the parent of the source, stop
					if (currNode == parent)
					{
						break;
					}

					//Add a control dependence from the source to the new node
					controlDepdendences.insert(make_pair(currNode, make_pair(sourceNode, edge)));

					if (StaticSingleAssignment::getDebug())
					{
						printf("%s is control-dependent on %s - %s \n", currNode.toStringForDebugging().c_str(),
							sourceNode.toStringForDebugging().c_str(), edge.condition() == VirtualCFG::eckTrue ? "true" : "false");
					}

					//Move to the parent of the current node
					parentIter = iPostDominatorMap.find(currNode);
					ROSE_ASSERT (parentIter != iPostDominatorMap.end());
					currNode = parentIter->second;
				}
			}
		}

		return controlDepdendences;
	}
}