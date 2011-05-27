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
            case V_SgTypeLong:
                return sizeof(long);
            case V_SgTypeLongLong:
                return sizeof(long long);
            case V_SgTypeUnsignedChar:
                return sizeof(unsigned char);
            case V_SgTypeUnsignedInt:
                return sizeof(unsigned int);
            case V_SgTypeUnsignedShort:
                return sizeof(unsigned short);
            case V_SgTypeUnsignedLongLong:
                return sizeof(unsigned long long);
            case V_SgTypeUnsignedLong:
                return sizeof(unsigned long);
            case V_SgTypeFloat:
                return sizeof(float);
            case V_SgTypeDouble:
                return sizeof(double);
            case V_SgClassType:
                //ROSE_ASSERT(false);
                return 100;
            case V_SgPointerType:
                return sizeof(void*);
            case V_SgTypedefType:
                return getCostFromType(t->stripTypedefsAndModifiers());
            case V_SgEnumType:
                return sizeof(int);
            default:
                cout << t->class_name() << endl;
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
    return SageBuilder::buildVarRefExp(name.back());//->get_name());
}

std::string ValueNode::toString() const
{
	ostringstream os;
	if (SgValueExp* valueExp = isSgValueExp(astNode))
		os << valueExp->unparseToString() + "\\n";

    if (isSgThisExp(astNode))
        os << "THIS";
    else if (isTemp())
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
    return getCostFromType(getType());
}

SgType* ValueNode::getType() const
{
    SgType* type;
    if (!var.isNull())
        type = var.name.back()->get_type();
    else if (SgExpression* expr = isSgExpression(astNode))
        type = expr->get_type();
    else if (SgInitializedName* initName = isSgInitializedName(astNode))
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

OperatorNode::OperatorNode(VariantT t, SgNode* node)
    : ValueGraphNode(node), type(t)
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

std::string OperatorNode::toString() const
{
    if (typeStringTable.count(type) > 0)
        return typeStringTable[type];
    return "OP";
}

FunctionCallNode::FunctionCallNode(SgFunctionCallExp* funcCall)
        : ValueGraphNode(funcCall), isVirtual(false)
{
    
#if 1
            
    SgFunctionDeclaration* funcDecl = funcCall->getAssociatedFunctionDeclaration();
    // In some cases, such as function pointers and virtual functions, the function 
    // called cannot be resolved statically; for those cases this function returns NULL.
    if (funcDecl == NULL)
    {
        //isVirtual = true;
        if (SgArrowExp* arrowExp = isSgArrowExp(funcCall->get_function()))
            if (isSgThisExp(arrowExp->get_lhs_operand()))
                isVirtual = true;
    }
    else
    {
        if (SgArrowExp* arrowExp = isSgArrowExp(funcCall->get_function()))
        {
            if (isSgThisExp(arrowExp->get_lhs_operand()))
            {
                SgMemberFunctionRefExp* funcRef = isSgMemberFunctionRefExp(arrowExp->get_rhs_operand());
                SgMemberFunctionDeclaration* funcDecl = funcRef->getAssociatedMemberFunctionDeclaration();
                //cout << funcDecl->get_name().str() << funcDecl->get_functionModifier().isVirtual() << endl;
                isVirtual = funcDecl->get_functionModifier().isVirtual();
            }
        }
        //isVirtual = true;
    }
    
#endif
}

std::string FunctionCallNode::toString() const
{
    string str;
    if (SgFunctionCallExp* funcCallExp = getFunctionCallExp())
    {
#if 0
        SgFunctionDeclaration* funcDecl = funcCallExp->getAssociatedFunctionDeclaration();
        if (funcDecl)
            str += funcDecl->get_name();
#endif
        str += funcCallExp->unparseToString();
    }
    
    if (str == "")
        str += "NO NAME";
    
    if (isVirtual)
        str += "\\nVIRTUAL";
    return str;
}

std::string ValueGraphEdge::toString() const
{
    std::string str;
    
#if 1
    str += "cost:" + boost::lexical_cast<std::string>(cost) + "\\n";
    str += boost::lexical_cast<std::string>(dagIndex) + ":";
    std::string s;
    boost::to_string(paths, s);
    str += s;
#endif
    
    if (controlDependences.empty())
        str += "Entry";
    
    foreach (const ControlDependence& cd, controlDependences)
    {
        str += cd.cdEdge.toString() + ":";
        
        if (SgIfStmt* ifStmt = isSgIfStmt(cd.cdNode))
        {
            str += ifStmt->get_conditional()->unparseToString();
        }
        else 
            str += cd.cdNode->class_name();
            
        str += "\\n";
    }
    
    return str;
}

std::string StateSavingEdge::toString() const
{ 
    std::string str = "SS:";// + boost::lexical_cast<std::string>(visiblePathNum) + "\\n";
    return str + ValueGraphEdge::toString();
}

} // End of namespace Backstroke
