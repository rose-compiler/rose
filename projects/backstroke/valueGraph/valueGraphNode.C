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
            {
                //SgPointerType* pType = isSgPointerType(t);
                //return getCostFromType(pType->get_base_type());
                return sizeof(void*);
            }
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
    
    SgExpression* var = SageBuilder::buildVarRefExp(name[0]);
    
    for (int i = 1, s = name.size(); i < s; ++i)
    {
        SgType* type = var->get_type();
        
        SgExpression* exp = SageBuilder::buildVarRefExp(name[i]);
        if (isSgPointerType(type))
            var = SageBuilder::buildArrowExp(var, exp);
        else
            var = SageBuilder::buildDotExp(var, exp);
    }
    
    return var;
}

std::string ValueNode::toString() const
{
	ostringstream os;
	if (SgValueExp* valueExp = isSgValueExp(astNode))
    {
        if (!isSgStringVal(astNode))
            os << valueExp->unparseToString() + "\\n";
    }

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

FunctionCallNode::FunctionCallNode(SgFunctionCallExp* funcCall, bool isRvs)
:   ValueGraphNode(funcCall), isReverse(isRvs), isVirtual(false), 
    isStd(false), canBeReversed(false)
{
    // If this function is declared as const.
    bool isConst = false;
    bool isInline = false;
    bool isMemberFunc = false;
    
#if 1
            
    funcDecl = funcCall->getAssociatedFunctionDeclaration();
    // In some cases, such as function pointers and virtual functions, the function 
    // called cannot be resolved statically; for those cases this function returns NULL.
    if (funcDecl == NULL)
    {
        SgMemberFunctionRefExp* funcRef = NULL;
        
        //isVirtual = true;
        if (SgBinaryOp* binExp = isSgBinaryOp(funcCall->get_function()))
            funcRef = isSgMemberFunctionRefExp(binExp->get_rhs_operand());
        
            //if (isSgThisExp(arrowExp->get_lhs_operand()))
        if (funcRef)
        {
            SgMemberFunctionDeclaration* memFuncDecl = funcRef->getAssociatedMemberFunctionDeclaration();
            //cout << funcDecl->get_name().str() << funcDecl->get_functionModifier().isVirtual() << endl;
            isVirtual = memFuncDecl->get_functionModifier().isVirtual();
            //cout << "@@@" << funcDecl->get_type()->unparseToString() << endl;
            //isConst = SageInterface::isConstType(funcDecl->isDefinedInClass());
            isInline = memFuncDecl->isDefinedInClass();

            funcDecl = memFuncDecl;
            //if (isVirtual)
            //cout << funcDecl->get_name().str() << "\t: VIRTUAL1\n\n";
            
            functionsToReverse.insert(memFuncDecl);
        }

        else 
        {
            //isVirtual = true;
            //cout << "UNKNOWN" << "\t: VIRTUAL2\n\n";
        }
        
        // TEMP
        //isVirtual = true;
        isMemberFunc =true;
    }
    
    else
    {
        SgMemberFunctionRefExp* funcRef = NULL;
        if (SgBinaryOp* binExp = isSgBinaryOp(funcCall->get_function()))
            funcRef = isSgMemberFunctionRefExp(binExp->get_rhs_operand());
        
        if (funcRef)
        {
            //SgMemberFunctionRefExp* funcRef = isSgMemberFunctionRefExp(arrowExp->get_rhs_operand());
            SgMemberFunctionDeclaration* memFuncDecl = funcRef->getAssociatedMemberFunctionDeclaration();
            //cout << funcDecl->get_name().str() << funcDecl->get_functionModifier().isVirtual() << endl;
            isVirtual = memFuncDecl->get_functionModifier().isVirtual();
            isConst = SageInterface::isConstType(memFuncDecl->get_type());
            isInline = memFuncDecl->get_functionModifier().isInline();

            SgNamespaceDefinitionStatement* nsDef = SageInterface::enclosingNamespaceScope(memFuncDecl);
            SgNamespaceDeclarationStatement* nsDecl = nsDef ? nsDef->get_namespaceDeclaration() : NULL;
            if (nsDecl && nsDecl->get_name() == "std")
            {
                cout << "\nFound a STD function: " << memFuncDecl->get_name() << "\n\n";
                isStd = true;
            }
            else if (memFuncDecl->get_specialFunctionModifier().isOperator() ||
                    memFuncDecl->get_specialFunctionModifier().isConversion())
            {
                cout << "\nFound a overloaded operator: " << memFuncDecl->get_name() << "\n\n";
            }
            else
            {
                //isVirtual = true;
                
                functionsToReverse.insert(memFuncDecl);
            }
            
            //if (isVirtual)
            //cout << funcDecl->get_name().str() << "\t: VIRTUAL3\n\n";
            //funcRef->get_file_info()->display();
            
            isMemberFunc = true;
        }
        //isVirtual = true;
    }
    
    ROSE_ASSERT(funcDecl);
    funcName = funcDecl->get_name();
    
    if (isVirtual)
        canBeReversed = true;
    else if (isConst || isInline || isStd)
        canBeReversed = false;
    else if (isMemberFunc)
        canBeReversed = true;


    os << funcDecl->get_name() << ":\n";
    os << funcDecl->get_functionModifier() << "\n";
    os << isMemberFunc << canBeReversed << "\n";
    os << funcDecl->get_specialFunctionModifier() << "\n\n";
    
#endif
}

FunctionCallNode::FunctionNamesT FunctionCallNode::getFunctionNames() const
{
    string fwdName, rvsName, cmtName;
    if (isStd)
    {
        if (funcName == "push_back")
        {
            fwdName = "push_back";
            rvsName = "pop_back";
            cmtName = "";
        }
        
    }
    else
    {
        fwdName = funcName + "_forward";
        rvsName = funcName + "_reverse";
        cmtName = funcName + "_commit";
    }
    
    return FunctionNamesT(fwdName, rvsName, cmtName);
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
        str += boost::lexical_cast<string>(astNode);
    
    if (isReverse)
        str += "\\nREVERSE";
    
    if (isVirtual)
        str += "\\nVIRTUAL";
    return str;
}

std::string ValueGraphEdge::toString() const
{
    std::string str;
    
#if 1
    str += "cost:" + boost::lexical_cast<std::string>(cost) + "\\n";
    str += paths.toString();
#endif
    
#if 0
    str += "\\n";
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
#endif
    
    return str;
}

std::string StateSavingEdge::toString() const
{ 
    std::string str = "SS: ";
    if (killer)
        str += killer->class_name() + "\\n";// + boost::lexical_cast<std::string>(visiblePathNum) + "\\n";
    return str + ValueGraphEdge::toString();
}

} // End of namespace Backstroke
