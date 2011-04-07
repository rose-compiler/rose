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
        createValueNode(var);

        // FIXME State variable may not be parameters.
        // Add the variable into wanted set.
        valuesToRestore_.push_back(nodeVertexMap_[var]);
        stateVariables_.insert(VarName(1, var));
    }

    vector<SgNode*> nodes = BackstrokeUtility::querySubTree<SgNode>(funcDef_);
    foreach (SgNode* node, nodes)
    {
        cout << node->class_name() << endl;

        // Statement case: variable declaration.
        if (SgStatement* stmt = isSgStatement(node))
        {
            if (SgVariableDeclaration* varDecl = isSgVariableDeclaration(stmt))
            {
                foreach (SgInitializedName* initName, varDecl->get_variables())
                {
                    SgInitializer* initalizer = initName->get_initializer();

                    // A declaration without definition
                    if (initalizer == NULL)
                    {
                        createValueNode(initName);
                    }
                    else if (SgAssignInitializer* assignInit =
                            isSgAssignInitializer(initalizer))
                    {
                        SgExpression* operand = assignInit->get_operand();

                        ROSE_ASSERT(nodeVertexMap_.count(operand) > 0);
                        VGVertex rhsVertex = nodeVertexMap_[operand];

                        addVariableToNode(rhsVertex, initName);
                    }
                    else
                    {
                        //cout << initalizer->class_name() << endl;
                        ROSE_ASSERT(!"Can only deal with assign initializer now!");
                    }
                }
            }
        }

        // Expression case.
        if (SgExpression* expr = isSgExpression(node))
        {
            //cout << ">>>>>>>" << expr->unparseToString() << " " << SageInterface::get_name(expr->get_type()) << endl;

            // For a variable reference, if its def is a phi node, we build this phi node here.
            if (BackstrokeUtility::isVariableReference(expr))
            {
                // Get the var name and version for lhs.
                // We don't know if this var is a use or def now.
                VersionedVariable var = getVersionedVariable(expr);

#if 1
                cout << "Variable node:" << var << endl;
#endif

                // If this variable is defined by a phi function.
                if (var.isPseudoDef)
                {
                    // If there is no node of var in the VG.
                    if (varVertexMap_.find(var) == varVertexMap_.end())
                    {
                        addValueGraphPhiNode(var);
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

                //VersionedVariable var(varName, defTable[varName]->getRenamingNumber());
                //varVertexMap_[var] = boost::add_vertexvalueGraph_;
                //valueGraph_[newNode] = new ValueGraphNode(varName, defTable[varName]->getRenamingNumber());
                //valueGraph_[newNode] = newNode;
                //varVertexMap_[var] = v;
            }

            // Value expression.
            else if (SgValueExp* valueExp = isSgValueExp(expr))
            {
                createValueNode(valueExp);
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

                        VGVertex result = createValueNode(unaryOp);
                        VGVertex oprdNode = nodeVertexMap_[operand];

                        createOperatorNode(t, result, oprdNode);
                        addVariableToNode(result, operand);

                        // For postfix ++ and --, we should assign the value node
                        // before modified to this expression.
                        if (unaryOp->get_mode() == SgUnaryOp::postfix)
                            nodeVertexMap_[unaryOp] = oprdNode;

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
                    {
                        if (BackstrokeUtility::isVariableReference(lhs))
                        {
                            ROSE_ASSERT(nodeVertexMap_.count(rhs) > 0);
                            VGVertex rhsVertex = nodeVertexMap_.find(rhs)->second;
                            addVariableToNode(rhsVertex, lhs);
                        }
                        else
                        {
                            ROSE_ASSERT(!"Only variable can be assigned now.");
                        }

                        break;
                    }

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
                    {
                        ROSE_ASSERT(nodeVertexMap_.count(lhs) > 0);
                        ROSE_ASSERT(nodeVertexMap_.count(rhs) > 0);

                        createOperatorNode(t, createValueNode(binOp),
                                nodeVertexMap_[lhs], nodeVertexMap_[rhs]);

                        break;
                    }

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

                        createOperatorNode(getOriginalType(t), createValueNode(expr),
                                varVertexMap_[use], nodeVertexMap_[rhs]);
                        break;
                    }


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

void EventReverser::buildForwardAndReverseEvent()
{
    SgFunctionDeclaration* funcDecl = funcDef_->get_declaration();

    SgScopeStatement* funcScope = funcDecl->get_scope();
    string funcName = funcDecl->get_name();

    //Create the function declaration for the forward body
    SgName fwdFuncName = funcName + "_forward";
    SgFunctionDeclaration* fwdFuncDecl = SageBuilder::buildDefiningFunctionDeclaration(
                    fwdFuncName, funcDecl->get_orig_return_type(),
                    isSgFunctionParameterList(
                        SageInterface::copyStatement(funcDecl->get_parameterList())),
                    funcScope);
    SgFunctionDefinition* fwdFuncDef = fwdFuncDecl->get_definition();
    //SageInterface::replaceStatement(fwdFuncDef->get_body(), isSgBasicBlock(stmt.fwd_stmt));

    //Create the function declaration for the reverse body
    SgName rvsFuncName = funcName + "_reverse";
    SgFunctionDeclaration* rvsFuncDecl = SageBuilder::buildDefiningFunctionDeclaration(
                    rvsFuncName, funcDecl->get_orig_return_type(),
                    isSgFunctionParameterList(
                        SageInterface::copyStatement(funcDecl->get_parameterList())),
                    funcScope);
    SgFunctionDefinition* rvsFuncDef = rvsFuncDecl->get_definition();
    //SageInterface::replaceStatement(rvsFuncDef->get_body(), isSgBasicBlock(stmt.rvs_stmt));

    //Create the function declaration for the commit method
    SgName cmtFuncName = funcName + "_commit";
    SgFunctionDeclaration* cmtFuncDecl = SageBuilder::buildDefiningFunctionDeclaration(
                    cmtFuncName, funcDecl->get_orig_return_type(),
                    isSgFunctionParameterList(
                        SageInterface::copyStatement(funcDecl->get_parameterList())),
                    funcScope);
    //SgFunctionDefinition* commitFunctionDefinition = commitFunctionDecl->get_definition();



    getSubGraph(rvsFuncDef->get_body(), 0, 0);

    SageInterface::insertStatementAfter(funcDecl, fwdFuncDecl);
    SageInterface::insertStatementAfter(fwdFuncDecl, rvsFuncDecl);
    SageInterface::insertStatementAfter(rvsFuncDecl, cmtFuncDecl);

    //SageInterface::insertStatementBefore(funcDecl, getStackVar(funcName));
	//return outputs;

#if 0
	map<OperatorNode*, tuple<VGVertex, VGVertex, VGVertex> > opReversals;
	foreach (VGEdge e, dagEdges_)
	{
		VGVertex src = source(e, valueGraph_);
		VGVertex tar = target(e, valueGraph_);


		// If the source is the root, then it is a state saving.
		if (src == root_)
		{
			// If the target is an available node, skip.
			if (valueGraph_[e]->cost == 0)
				continue;
			cout << "State Saving:" << valueGraph_[tar]->toString() << endl;
		}


		// If the target is an operator node, store it first.
		else if (OperatorNode* opNode = isOperatorNode(valueGraph_[tar]))
		{
			ROSE_ASSERT(isOrderedEdge(valueGraph_[e]));
			if (isOrderedEdge(valueGraph_[e])->index == 0)
				opReversals[opNode].get<1>() = src;
			else
				opReversals[opNode].get<2>() = src;
		}

		else if (OperatorNode* opNode = isOperatorNode(valueGraph_[src]))
		{
			VGVertex result = opReversals[opNode].get<0>() = tar;
			VGVertex operand1 = opReversals[opNode].get<1>();
			VGVertex operand2 = opReversals[opNode].get<2>();

			ROSE_ASSERT(operand1 != nullVertex() &&	operand2 != nullVertex());

			cout << "Reversal: " << valueGraph_[result]->toString() << " = " <<
					valueGraph_[operand1]->toString() << opNode->toString() <<
					valueGraph_[operand2]->toString() << endl;
		}

		// A normal assignment.
		else
		{
			cout << "Reversal: " << valueGraph_[tar]->toString() <<
					" = " << valueGraph_[src]->toString() << endl;
		}
	}
#endif
}



} // End of namespace Backstroke
