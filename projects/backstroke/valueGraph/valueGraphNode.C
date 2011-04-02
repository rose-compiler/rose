#include "valueGraphNode.h"
#include <ssa/staticSingleAssignment.h>

#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>


namespace Backstroke
{
	
using namespace std;
using namespace boost;

#define foreach BOOST_FOREACH

namespace
{
    //! An internal function to return a cost for state saving depending on the type.
    int getCostFromType(SgType* t)
    {
        ROSE_ASSERT(t);

        if (SgReferenceType* refType = isSgReferenceType(t))
        {
            t = refType->get_base_type();
        }

        switch (t->variantT())
        {
            case V_SgTypeBool:
                return sizeof(bool);
            case V_SgTypeInt:
                return sizeof(int);
            case V_SgTypeChar:
                return sizeof(char);
            case V_SgTypeShort:
                return sizeof(short);
            case V_SgTypeFloat:
                return sizeof(float);
            case V_SgTypeDouble:
                return sizeof(double);
            case V_SgClassType:
                ROSE_ASSERT(false);
                return 100;
            default:
                ROSE_ASSERT(!"Unknow type.");
                return 100;
        }

        ROSE_ASSERT(false);
        return 0;
    }
}

std::string VersionedVariable::toString() const
{
	if (name.empty())
		return "TEMP";
	return StaticSingleAssignment::varnameToString(name) +
			"_" + lexical_cast<string>(version);
}

int PhiNode::getCost() const
{
    return getCostFromType(getType());
}

std::string ValueNode::toString() const
{
	ostringstream os;
	if (SgValueExp* valueExp = isSgValueExp(astNode))
		os << valueExp->unparseToString() + "\\n";

	if (isTemp())
		os << "TEMP";
    else
    {
        foreach (const VersionedVariable& var, vars)
            os << var << " ";
    }
	return os.str();
}

int ValueNode::getCost() const
{
    int cost = 0;
    if (!isAvailable())
    {
        if (isTemp())
            cost = getCostFromType(getType());
        else
        {
            // The cost is the minimum one in all variables inside.
            cost = INT_MAX;
            foreach (const VersionedVariable& var, vars)
                cost = min(cost, getCostFromType(var.name.back()->get_type()));
        }
    }
    return cost;
}

SgType* ValueNode::getType() const
{
    if (SgExpression* expr = isSgExpression(astNode))
        return expr->get_type();
    if (SgInitializedName* initName = isSgInitializedName(astNode))
        return initName->get_type();
    return NULL;
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
    string s;
    boost::to_string(paths, s);
    str += s;
    return str;
}


} // End of namespace Backstroke
