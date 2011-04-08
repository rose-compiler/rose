#include "valueGraphNode.h"
#include <ssa/staticSingleAssignment.h>
#include <sageBuilder.h>
#include <boost/assign/list_inserter.hpp>
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
    string str;
    foreach (SgInitializedName* n, name)
        str += n->get_name() + "_";
    str += boost::lexical_cast<string>(version);
    return str;
}

SgExpression* VersionedVariable::getVarRefExp() const
{
    ROSE_ASSERT(!name.empty());
    return SageBuilder::buildVarRefExp(name.back()->get_name());
}

std::string ValueNode::toString() const
{
	ostringstream os;
	if (SgValueExp* valueExp = isSgValueExp(astNode))
		os << valueExp->unparseToString() + "\\n";

	if (isTemp())
		os << "TEMP";
    else
        os << var << " ";
	return os.str();
}


//void ValueNode::addVariable(const VersionedVariable& newVar)
//{
//    // If this value node has a value, its cost is zero.
//    if (isSgValueExp(astNode_))
//    {
//        vars_.push_back(newVar);
//        return;
//    }
//
//    int costOfNewVar = getCostFromType(newVar.name.back()->get_type());
//    if (vars_.empty() || costOfNewVar < cost)
//    {
//        cost = costOfNewVar;
//        vars_.push_back(newVar);
//    }
//    else
//    {
//        vars_.insert(vars_.end() - 1, newVar);
//    }
//}

int PhiNode::getCost() const
{
    getCostFromType(getType());
}

SgType* ValueNode::getType() const
{
    SgType* type;

    if (SgExpression* expr = isSgExpression(astNode))
        type = expr->get_type();
    if (SgInitializedName* initName = isSgInitializedName(astNode))
        type = initName->get_type();

    // Remove reference.
    if (SgReferenceType* refType = isSgReferenceType(type))
        type = refType->get_base_type();

    return type;
}

int ValueNode::getCost() const
{
    if (isAvailable())
        return 0;
    return getCostFromType(getType());
}

std::map<VariantT, std::string> OperatorNode::typeStringTable;

void OperatorNode::buildTypeStringTable()
{
    boost::assign::insert(typeStringTable)
    (V_SgAddOp,             "+" )
    (V_SgSubtractOp,        "-" )
    (V_SgPlusPlusOp,        "++")
    (V_SgMinusMinusOp,      "--")
    (V_SgMultiplyOp,        "x" )
    (V_SgDivideOp,          "/" )
    (V_SgGreaterThanOp,     ">" )
    (V_SgLessThanOp,        "<" )
    (V_SgEqualityOp,        "==")
    (V_SgNotEqualOp,        "!=")
    (V_SgGreaterOrEqualOp,  ">=")
    (V_SgLessOrEqualOp,     "<=")
    (V_SgModOp,             "%" );
}

OperatorNode::OperatorNode(VariantT t)
    : ValueGraphNode(), type(t)
{
    switch (t)
    {
	case V_SgPlusAssignOp:
		type = V_SgAddOp;
        break;

	case V_SgSubtractOp:
	case V_SgMinusAssignOp:
        type = V_SgSubtractOp;
        break;

    default:
        break;
    }
    buildTypeStringTable();
}

string OperatorNode::toString() const
{
    if (typeStringTable.count(type) > 0)
        return typeStringTable[type];
    return "OP";
}

std::string ValueGraphEdge::toString() const
{
    std::string str = "cost:" + boost::lexical_cast<std::string>(cost) + "\\n";
    str += boost::lexical_cast<std::string>(dagIndex) + ":";
    string s;
    boost::to_string(paths, s);
    str += s;
    return str;
}


} // End of namespace Backstroke
