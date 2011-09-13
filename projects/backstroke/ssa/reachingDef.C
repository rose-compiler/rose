#include "reachingDef.h"
#include <algorithm>
#include <boost/foreach.hpp>
#include <boost/unordered_set.hpp>
#include <boost/tuple/tuple.hpp>
#include <stack>

#define foreach BOOST_FOREACH

using namespace std;
using namespace boost;

ReachingDef::ReachingDef(const CFGNode& defNode, Type type) : defType(type), thisNode(defNode), renamingNumer(-1)
{
}

bool ReachingDef::isPhiFunction() const
{
	return defType == PHI_FUNCTION;
}

const map<ReachingDef::ReachingDefPtr, set<CFGEdge> >& ReachingDef::getJoinedDefs() const
{
	ROSE_ASSERT(isPhiFunction());
	return parentDefs;
}

const CFGNode& ReachingDef::getDefinitionNode() const
{
	return thisNode;
}

set<CFGNode> ReachingDef::getActualDefinitions() const
{
	set<CFGNode> result;
	if (!isPhiFunction())
	{
		result.insert(thisNode);
	}
	else
	{
		//Depth-first search of phi node graph
		unordered_set<ReachingDefPtr> visited;
		vector< ReachingDefPtr > worklist;
		ReachingDefPtr parentDef;
		set<CFGEdge> edges;
		foreach (tie(parentDef, edges), parentDefs)
		{
			worklist.push_back(parentDef);
		}

		while (!worklist.empty())
		{
			ReachingDefPtr parentDef = worklist.back();
			worklist.pop_back();
			visited.insert(parentDef);

			if (!parentDef->isPhiFunction())
			{
				result.insert(parentDef->getDefinitionNode());
			}
			else
			{
				pair<ReachingDefPtr, set<CFGEdge> > defEdgePair;
				foreach(defEdgePair, parentDef->getJoinedDefs())
				{
					if (visited.count(defEdgePair.first) == 0)
					{
						worklist.push_back(defEdgePair.first);
					}
				}
			}
		}
	}
	return result;
}

int ReachingDef::getRenamingNumber() const
{
	return renamingNumer;
}

bool ReachingDef::operator==(const ReachingDef& other) const
{
	return (defType == other.defType) && (thisNode == other.thisNode) &&
			(parentDefs == other.parentDefs) && (renamingNumer == other.renamingNumer);
}

void ReachingDef::setDefinitionNode(CFGNode defNode)
{
	thisNode = defNode;
}

void ReachingDef::addJoinedDef(shared_ptr<ReachingDef> newDef, CFGEdge edge)
{
	ROSE_ASSERT(isPhiFunction());
	parentDefs[newDef].insert(edge);
}

void ReachingDef::setRenamingNumber(int n)
{
	renamingNumer = n;
}
