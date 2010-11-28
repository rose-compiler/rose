#include "ReachingDef.h"
#include <algorithm>
#include <boost/foreach.hpp>
#include <boost/unordered_set.hpp>
#include <stack>

#define foreach BOOST_FOREACH

using namespace std;
using namespace boost;

ReachingDef::ReachingDef(SgNode* defNode, Type type) : defType(type), thisNode(defNode)
{
}

bool ReachingDef::isPhiFunction() const
{
	return defType == PHI_FUNCTION;
}

const vector < shared_ptr<ReachingDef> >& ReachingDef::getJoinedDefs() const
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
		unordered_set< shared_ptr<ReachingDef> > visited;
		vector< shared_ptr<ReachingDef> > worklist(parentDefs);

		while (!worklist.empty())
		{
			shared_ptr<ReachingDef> parentDef = worklist.back();
			worklist.pop_back();
			visited.insert(parentDef);

			if (!parentDef->isPhiFunction())
			{
				result.insert(parentDef->getDefinitionNode());
			}
			else
			{
				foreach(shared_ptr<ReachingDef> def, parentDef->getJoinedDefs())
				{
					if (visited.count(def) == 0)
					{
						worklist.push_back(def);
					}
				}
			}
		}
	}
	return result;
}

bool ReachingDef::operator==(const ReachingDef& other) const
{
	return (defType == other.defType) && (thisNode == other.thisNode) &&
			(parentDefs == other.parentDefs);
}

void ReachingDef::setDefinitionNode(SgNode* defNode)
{
	thisNode = defNode;
}

bool ReachingDef::addJoinedDef(shared_ptr<ReachingDef> newDef)
{
	ROSE_ASSERT(isPhiFunction());
	if (find(parentDefs.begin(), parentDefs.end(), newDef) == parentDefs.end())
	{
		parentDefs.push_back(newDef);
		return true;
	}
	else
	{
		return false;
	}
}
