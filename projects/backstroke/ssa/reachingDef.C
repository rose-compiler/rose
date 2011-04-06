#include "reachingDef.h"
#include <algorithm>
#include <boost/foreach.hpp>
#include <boost/unordered_set.hpp>
#include <stack>

#define foreach BOOST_FOREACH

using namespace std;
using namespace boost;

ReachingDef::ReachingDef(SgNode* defNode, Type type) : defType(type), thisNode(defNode), renamingNumer(-1)
{
}

bool ReachingDef::isPhiFunction() const
{
	return defType == PHI_FUNCTION;
}

const vector < pair<ReachingDef::ReachingDefPtr, CFGEdge> >& ReachingDef::getJoinedDefs() const
{
	ROSE_ASSERT(isPhiFunction());
	return parentDefs;
}

SgNode* ReachingDef::getDefinitionNode() const
{
	return thisNode;
}

set<SgNode*> ReachingDef::getActualDefinitions() const
{
	set<SgNode*> result;
	if (!isPhiFunction())
	{
		result.insert(thisNode);
	}
	else
	{
		//Depth-first search of phi node graph
		unordered_set<ReachingDefPtr> visited;
		vector< pair<ReachingDefPtr, CFGEdge> > worklist(parentDefs);

		while (!worklist.empty())
		{
			pair<ReachingDefPtr, CFGEdge> defEdgePair = worklist.back();
			worklist.pop_back();
			ReachingDefPtr parentDef = defEdgePair.first;
			visited.insert(parentDef);

			if (!parentDef->isPhiFunction())
			{
				result.insert(parentDef->getDefinitionNode());
			}
			else
			{
				pair<ReachingDefPtr, CFGEdge> defEdgePair;
				foreach(defEdgePair, parentDef->getJoinedDefs())
				{
					if (visited.count(defEdgePair.first) == 0)
					{
						worklist.push_back(defEdgePair);
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

void ReachingDef::setDefinitionNode(SgNode* defNode)
{
	thisNode = defNode;
}

bool ReachingDef::addJoinedDef(shared_ptr<ReachingDef> newDef, CFGEdge edge)
{
	ROSE_ASSERT(isPhiFunction());
	
	pair<ReachingDefPtr, CFGEdge> incomingDef(newDef, edge);
	
	if (find(parentDefs.begin(), parentDefs.end(), incomingDef) == parentDefs.end())
	{
		parentDefs.push_back(incomingDef);
		return true;
	}
	else
	{
		return false;
	}
}

void ReachingDef::setRenamingNumber(int n)
{
	renamingNumer = n;
}
