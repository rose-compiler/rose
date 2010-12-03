#pragma once

#include <vector>
#include "sage3basic.h"
#include <string>
#include <sstream>

namespace ssa_private
{
using namespace std;

template<class CfgEdgeT>
class ControlPredicate
{
public:
	//Can either be a conjuction or disjuction
	enum PredicateType
	{
		AND, OR, LITERAL, NONE
	};

private:

	PredicateType type;

	/** If the predicate is AND or OR. */
	vector<ControlPredicate> children;

	/** If the predicate is literal. */
	CfgEdgeT literalValue;

public:

	//--------CONSTRUCTORS--------

	/** Create a new literal. */
	ControlPredicate(CfgEdgeT literal) : type(LITERAL), literalValue(literal)
	{
	}

	/** Create a predicate. */
	ControlPredicate(PredicateType type) : type(type)
	{
	}

	//--------ACCESSORS----------

	PredicateType getPredicateType() const
	{
		return type;
	}

	const vector<ControlPredicate>& getChildren() const
	{
		ROSE_ASSERT(type == AND || type == OR);
		return children;
	}

	string toString() const
	{
		if (type == NONE)
			return "";
		else if (type == LITERAL)
		{
			string result = literalValue.source().toStringForDebugging() + " == ";
			switch (literalValue.condition())
			{
				case VirtualCFG::eckTrue:
					result += "true";
					break;
				case VirtualCFG::eckFalse:
					result += "false";
					break;
				case VirtualCFG::eckCaseLabel:
					result += literalValue.caseLabel()->unparseToString();
					break;
				case eckUnconditional:
					ROSE_ASSERT(false);
				default:
					result += "unknown";
			}
			return result;
		}

		//We are a conjunction or disjunction
		string joinText = (type == AND) ? " AND " : " OR ";
		string result;

		for (size_t i = 0; i < children.size(); i++)
		{
			result += "(" + children[i].toString() + ")";
			if (i < children.size() - 1)
				result += joinText;
		}

		return result;
	}

	//---------MODIFIERS------------

	void addChildPredicate(const ControlPredicate& p)
	{
		ROSE_ASSERT(type == AND || type == OR);
		children.push_back(p);
	}
};

} //End namespace ssa_private