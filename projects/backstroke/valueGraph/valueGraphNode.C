#include "valueGraphNode.h"
#include <ssa/staticSingleAssignment.h>

#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>


namespace Backstroke
{
	
using namespace std;
using namespace boost;

#define foreach BOOST_FOREACH


std::string VersionedVariable::toString() const
{
	if (name.empty())
		return "TEMP";
	return StaticSingleAssignment::varnameToString(name) +
			"_" + lexical_cast<string>(version);
}

std::string ValueNode::toString() const
{
	if (isTemp())
		return "TEMP";

	ostringstream os;
	if (valueExp)
		os << valueExp->unparseToString() + "\\n";
	foreach (const VersionedVariable& var, vars)
		os << var << " ";
	return os.str();
}

OperatorNode::OperatorType OperatorNode::getOperatorType(VariantT t)
{
	switch (t)
	{
	case V_SgAddOp:
	case V_SgPlusAssignOp:
    case V_SgPlusPlusOp:
		return otAdd;

	case V_SgSubtractOp:
	case V_SgMinusAssignOp:
    case V_SgMinusMinusOp:
		return otSubtract;

	case V_SgMultiplyOp:
		return otMultiply;

	case V_SgDivideOp:
		return otDivide;

	case V_SgGreaterThanOp:
		return otGreaterThan;

	case V_SgLessThanOp:
		return otLessThan;

	default:
		ROSE_ASSERT(false);
	}

	ROSE_ASSERT(false);
	return otUnknown;
}

string OperatorNode::toString() const
{
	std::string label;
	switch (type)
	{
		case otAdd:
			label = "+";
			break;
		case otSubtract:
			label = "-";
			break;
		case otMultiply:
			label = "x";
			break;
		case otGreaterThan:
			label = ">";
			break;
		case otLessThan:
			label = "<";
			break;
		default:
			break;
	}

	return label;
}

std::string ValueGraphEdge::toString() const
{
    std::string str = "cost:" + boost::lexical_cast<std::string>(cost) + "\\n";
    str += boost::lexical_cast<std::string>(dagIndex) + " : ";
    foreach (int i, paths)
        str += boost::lexical_cast<std::string>(i) + " ";
    return str;
}


} // End of namespace Backstroke
