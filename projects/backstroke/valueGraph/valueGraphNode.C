#include "valueGraphNode.h"

using namespace std;
using namespace boost;

#define foreach BOOST_FOREACH


namespace Backstroke
{

OperatorNode::OperatorType OperatorNode::getOperatorType(VariantT t)
{
	switch (t)
	{
	case V_SgAddOp:
	case V_SgPlusAssignOp:
		return otAdd;

	case V_SgSubtractOp:
	case V_SgMinusAssignOp:
		return otSubtract;

	case V_SgMultiplyOp:
		return otMultiply;

	case V_SgDivideOp:
		return otDivide;

	case V_SgGreaterThanOp:
		return otGreaterThan;

	default:
		ROSE_ASSERT(false);
	}

	ROSE_ASSERT(false);
	return otUnknown;
}

void OperatorNode::writeDotString(std::ostream& out) const
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

	out << "[label=\""  << label << "\"]";
}


} // End of namespace Backstroke
