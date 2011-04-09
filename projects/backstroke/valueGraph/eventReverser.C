#include "valueGraph.h"

#include <sageInterface.h>
#include <sageBuilder.h>
#include <utilities/utilities.h>
#include <boost/assign/list_inserter.hpp>

namespace Backstroke
{

using namespace std;

#define foreach BOOST_FOREACH

namespace
{
    struct OperationTypeTable
    {
        OperationTypeTable()
        {
            boost::assign::insert(table)
            (V_SgPlusAssignOp,      V_SgAddOp)
            (V_SgMinusAssignOp,     V_SgSubtractOp)
            (V_SgMultAssignOp,      V_SgMultiplyOp)
            (V_SgDivAssignOp,       V_SgDivideOp)
            (V_SgAndAssignOp,       V_SgAndOp)
            (V_SgIorAssignOp,       V_SgOrOp)
            (V_SgModAssignOp,       V_SgModOp)
            (V_SgXorAssignOp,       V_SgBitXorOp)
            (V_SgLshiftAssignOp,    V_SgLshiftOp)
            (V_SgRshiftAssignOp,    V_SgRshiftOp);
        }

        map<VariantT, VariantT> table;
    };

    VariantT getOriginalType(VariantT t)
    {
        static OperationTypeTable typeTable = OperationTypeTable();
        ROSE_ASSERT(typeTable.table.count(t) > 0);
        return typeTable.table[t];
    }

} // end of anonymous

void EventReverser::buildBasicValueGraph()
{
    // Build the SSA form of the given function.
    //SSA::NodeReachingDefTable defTable = getUsesAtNode(funcDef);

    // Build a vertex which is the start point of the search.
    //root_ = addValueGraphNode(new ValueGraphNode);
    root_ = addValueGraphNode(new ValueGraphNode);

    // First, add all parameters of the event to the value graph.
    SgInitializedNamePtrList& paraList = funcDef_->get_declaration()->get_args();
    foreach (SgInitializedName* var, paraList)
    {
        createValueNode(var, NULL);

        // FIXME State variable may not be parameters.
        // Add the variable into wanted set.
        valuesToRestore_.push_back(nodeVertexMap_[var]);
        stateVariables_.insert(VarName(1, var));
    }

    vector<SgNode*> nodes = BackstrokeUtility::querySubTree<SgNode>(funcDef_);
    foreach (SgNode* node, nodes)
    {
        //cout << node->class_name() << endl;

        // Statement case: variable declaration.
        if (SgStatement* stmt = isSgStatement(node))
        {
            SgVariableDeclaration* varDecl = isSgVariableDeclaration(stmt);
            if (varDecl == NULL) continue;
            
            foreach (SgInitializedName* initName, varDecl->get_variables())
            {
                SgInitializer* initalizer = initName->get_initializer();

                // A declaration without definition
                if (initalizer == NULL)
                {
                    createValueNode(initName, NULL);
                }
                else if (SgAssignInitializer* assignInit =
                        isSgAssignInitializer(initalizer))
                {
                    SgExpression* operand = assignInit->get_operand();
                    createValueNode(initName, operand);
                    //addVariableToNode(rhsVertex, initName);
                }
                else
                {
                    //cout << initalizer->class_name() << endl;
                    ROSE_ASSERT(!"Can only deal with assign initializer now!");
                }
            }
        }

        // Expression case.
        if (SgExpression* expr = isSgExpression(node))
        {
            // For a variable reference, if its def is a phi node, we build this phi node here.
            if (BackstrokeUtility::isVariableReference(expr))
            {
                // Get the var name and version for lhs.
                // We don't know if this var is a use or def now.
                VersionedVariable var = getVersionedVariable(expr);

                // If this variable is defined by a phi function.
                if (var.isPseudoDef)
                {
                    // If there is no node of var in the VG.
                    if (varVertexMap_.find(var) == varVertexMap_.end())
                    {
                        createPhiNode(var);
                    }

                    ROSE_ASSERT(varVertexMap_.find(var) != varVertexMap_.end());

                    // Add the node -> vertex to nodeVertexMap_
                    nodeVertexMap_[expr] = varVertexMap_.find(var)->second;
                }
                else
                {
                    //cout << var.version << " " << var.isPseudoDef << endl;

                    // If the variable is a use, it should already exist in the var->vertex table.
                    // Find it and update the node->vertex table.
                    // The def node is not added here. (added in assignment)
                    if (varVertexMap_.count(var) > 0)
                        nodeVertexMap_[expr] = varVertexMap_[var];
                }
            }

            // Value expression.
            if (SgValueExp* valueExp = isSgValueExp(expr))
            {
                createValueNode(NULL, valueExp);
                //addValueGraphNode(new ValueNode(valueExp), expr);
            }

            // Cast expression.
            else if (SgCastExp* castExp = isSgCastExp(expr))
            {
                ROSE_ASSERT(nodeVertexMap_.count(castExp->get_operand()));
                nodeVertexMap_[castExp] = nodeVertexMap_[castExp->get_operand()];
            }

            // Unary expressions.
            else if (SgUnaryOp* unaryOp = isSgUnaryOp(expr))
            {
                SgExpression* operand = unaryOp->get_operand();

                VariantT t = unaryOp->variantT();
                switch (t)
                {
                case V_SgPlusPlusOp:
                case V_SgMinusMinusOp:
                    {
                        ROSE_ASSERT(nodeVertexMap_.count(operand) > 0);

                        VGVertex operandNode = nodeVertexMap_[operand];
                        VGVertex result = createValueNode(operand, NULL);

                        createOperatorNode(t, result, operandNode);
                        //addVariableToNode(result, operand);

                        // For postfix ++ and --, we should assign the value node
                        // before modified to this expression.
                        if (unaryOp->get_mode() == SgUnaryOp::postfix)
                            nodeVertexMap_[unaryOp] = operandNode;
                        else
                            nodeVertexMap_[unaryOp] = result;

                        break;
                    }

                default:
                    break;
                }
            }

            // Binary expressions.
            else if (SgBinaryOp* binOp = isSgBinaryOp(expr))
            {
                SgExpression* lhs = binOp->get_lhs_operand();
                SgExpression* rhs = binOp->get_rhs_operand();

                VariantT t = binOp->variantT();
                switch (t)
                {
                // For assign op, we assign the node which is assigned to with a variable name.
                case V_SgAssignOp:
                    if (BackstrokeUtility::isVariableReference(lhs))
                    {
                        VGVertex result = createValueNode(lhs, rhs);
                        // Update the node-vertex table.
                        nodeVertexMap_[binOp] = result;
                    }
                    else
                    {
                        ROSE_ASSERT(!"Only variable can be assigned now.");
                    }

                    break;
                    
                case V_SgAddOp:
                case V_SgSubtractOp:
                case V_SgMultiplyOp:
                case V_SgDivideOp:
                case V_SgGreaterThanOp:
                case V_SgGreaterOrEqualOp:
                case V_SgLessThanOp:
                case V_SgLessOrEqualOp:
                case V_SgEqualityOp:
                case V_SgNotEqualOp:
                    ROSE_ASSERT(nodeVertexMap_.count(lhs) > 0);
                    ROSE_ASSERT(nodeVertexMap_.count(rhs) > 0);

                    createOperatorNode(t, createValueNode(NULL, binOp),
                            nodeVertexMap_[lhs], nodeVertexMap_[rhs]);
                    break;

                case V_SgPlusAssignOp:
                case V_SgMinusAssignOp:
                case V_SgMultAssignOp:
                case V_SgDivAssignOp:
                case V_SgAndAssignOp:
                case V_SgIorAssignOp:
                case V_SgModAssignOp:
                case V_SgXorAssignOp:
                case V_SgLshiftAssignOp:
                case V_SgRshiftAssignOp:
                    {
                        VersionedVariable use = getVersionedVariable(lhs);

                        ROSE_ASSERT(varVertexMap_.count(use) > 0);
                        ROSE_ASSERT(nodeVertexMap_.count(rhs) > 0);

                        VGVertex result = createValueNode(lhs, NULL);
                        createOperatorNode(
                                getOriginalType(t), result,
                                varVertexMap_[use], nodeVertexMap_[rhs]);

                        // Update the node-vertex table.
                        nodeVertexMap_[binOp] = result;
                        break;
                    }

                case V_SgCommaOpExp:
                    nodeVertexMap_[binOp] = nodeVertexMap_[rhs];
                    break;

                default:
                    break;
                }
            }
        }
    }
}

SgVariableDeclaration* getStackVar(const string& name)
{
	string typeName;

    vector<SgVariableDeclaration*> stackVars;
	string stackName = name + "_stack";

    SgClassDeclaration* stackTypeDeclaration = SageBuilder::buildStructDeclaration("std::deque<boost::any>");
    SgType* stackType = stackTypeDeclaration->get_type();
    ROSE_ASSERT(stackType);
    //delete stackTypeDeclaration;

    SgVariableDeclaration* stackDecl = SageBuilder::buildVariableDeclaration(stackName, stackType);
    stackVars.push_back(stackDecl);


    return stackDecl;
	//return SageBuilder::buildVarRefExp(stackVars[0]->get_variables()[0]);
}

void EventReverser::buildFunctionBodies()
{
    using namespace SageInterface;
    using namespace SageBuilder;

    SgFunctionDeclaration* funcDecl = funcDef_->get_declaration();

    SgScopeStatement* funcScope = funcDecl->get_scope();
    string funcName = funcDecl->get_name();

    //Create the function declaration for the forward body
    SgName fwdFuncName = funcName + "_forward";
    SgFunctionDeclaration* fwdFuncDecl = buildDefiningFunctionDeclaration(
                    fwdFuncName,
                    funcDecl->get_orig_return_type(),
                    isSgFunctionParameterList(
                        copyStatement(funcDecl->get_parameterList())),
                    funcScope);
    fwdFuncDef_ = fwdFuncDecl->get_definition();
    //SageInterface::replaceStatement(fwdFuncDef->get_body(), isSgBasicBlock(stmt.fwd_stmt));

    //Create the function declaration for the reverse body
    SgName rvsFuncName = funcName + "_reverse";
    SgFunctionDeclaration* rvsFuncDecl = buildDefiningFunctionDeclaration(
                    rvsFuncName,
                    funcDecl->get_orig_return_type(),
                    isSgFunctionParameterList(
                        copyStatement(funcDecl->get_parameterList())),
                    funcScope);
    rvsFuncDef_ = rvsFuncDecl->get_definition();
    //SageInterface::replaceStatement(rvsFuncDef->get_body(), isSgBasicBlock(stmt.rvs_stmt));

    //Create the function declaration for the commit method
    SgName cmtFuncName = funcName + "_commit";
    SgFunctionDeclaration* cmtFuncDecl = buildDefiningFunctionDeclaration(
                    cmtFuncName,
                    funcDecl->get_orig_return_type(),
                    isSgFunctionParameterList(
                        copyStatement(funcDecl->get_parameterList())),
                    funcScope);
    cmtFuncDef_ = cmtFuncDecl->get_definition();

    // Copy the original function to forward function.
    replaceStatement(fwdFuncDef_->get_body(),
                     copyStatement(funcDef_->get_body()));

    // Swap the following two function definitions. This is because currently there
    // is a problem on copying a function to another. We work around it by regard the
    // original event as the forward one, and the copy of the original event becomes
    // the original one at last.
    //swap(funcDef_, fwdFuncDef_);

//    insertStatementAfter(funcDecl,    fwdFuncDecl);
//    insertStatementAfter(fwdFuncDecl, rvsFuncDecl);
//    insertStatementAfter(rvsFuncDecl, cmtFuncDecl);

	//SageInterface::fixVariableReferences(funcScope);
    //AstTests::runAllTests(SageInterface::getProject());
    //SageInterface::insertStatementBefore(funcDecl, getStackVar(funcName));
	//return outputs;
}



} // End of namespace Backstroke
