#include "valueGraph.h"
#include "eventReverser.h"
#include <sageInterface.h>
#include <sageBuilder.h>
#include <utilities/utilities.h>
#include <boost/assign/list_inserter.hpp>
#include <boost/graph/topological_sort.hpp>

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

    /***************************************************************************/
    // We search state variables here. This part should use a functor to determine
    // which variables are state ones.
    // Currently, we assume the parameters are state variables in C style, while
    // all data members are state variables in C++ style.
    
    // First, add all parameters of the event to the value graph.
    SgInitializedNamePtrList& paraList = funcDef_->get_declaration()->get_args();
    foreach (SgInitializedName* initName, paraList)
    {
        VGVertex newNode = createValueNode(initName, NULL);

        // FIXME State variable may not be parameters.
        // Add the variable into wanted set.
        valuesToRestore_.push_back(newNode);
        stateVariables_.insert(VarName(1, initName));
    }

    SgFunctionDeclaration* funcDecl = funcDef_->get_declaration();
    // If the event is a member function.
    if (SgMemberFunctionDeclaration* memFuncDecl = isSgMemberFunctionDeclaration(funcDecl))
    {
        SgClassDefinition* classDef = memFuncDecl->get_class_scope();
        foreach (SgDeclarationStatement* decl, classDef->get_members())
        {
            SgVariableDeclaration* varDecl = isSgVariableDeclaration(decl);
            if (varDecl == NULL)
                continue;
            foreach (SgInitializedName* initName, varDecl->get_variables())
            {
                VarName varName(1, initName);
                cout << SSA::varnameToString(varName) << 1 << endl;
                VGVertex newNode = createValueNode(initName, NULL);
                valuesToRestore_.push_back(newNode);
                stateVariables_.insert(VarName(1, initName));
            }
        }
    }
    /***************************************************************************/

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
                // For data member access, ignore its corresponding "this" pointer.
                if (isSgThisExp(expr))
                    continue;

                if (SgArrowExp* arrowExp = isSgArrowExp(expr))
                {
                    if (isSgThisExp(arrowExp->get_lhs_operand()))
                    {
                        SgExpression* rhsExp = arrowExp->get_rhs_operand();
                        // It is possible that this var ref is actually a def.
                        // For example, this->a = 0;
                        if (nodeVertexMap_.count(rhsExp) > 0)
                            nodeVertexMap_[arrowExp] = nodeVertexMap_[rhsExp];
                        continue;
                    }
                }

                // A cast expression may be a variable reference.
                if (SgCastExp* castExp = isSgCastExp(expr))
                {
                    ROSE_ASSERT(nodeVertexMap_.count(castExp->get_operand()));
                    nodeVertexMap_[castExp] = nodeVertexMap_[castExp->get_operand()];
                    continue;
                }

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
                    nodeVertexMap_[expr] = varVertexMap_[var];
                }
                else
                {
                    cout << SageInterface::get_name(expr) << endl;
                    cout << "&" << var.toString() << " " << var.isPseudoDef << endl;

                    // If the variable is a use, it should already exist in the var->vertex table.
                    // Find it and update the node->vertex table.
                    // The def node is not added here. (added in assignment)
                    printVarVertexMap();
                    // It is possible that this var ref is actually a def. For example, a = 0;
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

vector<EventReverser::VGVertex>
EventReverser::getGraphNodesInTopologicalOrder(
        const SubValueGraph& subgraph) const
{
    // The following code is needed since the value graph has VertexList=ListS which
    // does not have a vertex_index property, which is needed by topological_sort.
    int counter = 0;
    map<VGVertex, int> vertexIDs;
    foreach (VGVertex v, boost::vertices(subgraph))
        vertexIDs[v] = counter++;
    // Turn a std::map into a property map.
    boost::associative_property_map<map<VGVertex, int> > vertexIDMap(vertexIDs);

    vector<VGVertex> nodes;
    boost::topological_sort(subgraph, back_inserter(nodes), vertex_index_map(vertexIDMap));
    return nodes;
}

pair<ValueNode*, ValueNode*>
EventReverser::getOperands(VGVertex opNode, const SubValueGraph& subgraph) const
{
    ValueNode* lhsNode = NULL;
    ValueNode* rhsNode = NULL;

    // If it's a unary operation.
    if (boost::out_degree(opNode, subgraph) == 1)
    {
        VGVertex lhs = *(boost::adjacent_vertices(opNode, subgraph).first);
        lhsNode = isValueNode(subgraph[lhs]);
    }
    else
    {
        foreach (const VGEdge& edge, boost::out_edges(opNode, subgraph))
        {
            VGVertex tar = boost::target(edge, subgraph);
            if (isOrderedEdge(subgraph[edge])->index == 0)
                lhsNode = isValueNode(subgraph[tar]);
            else
                rhsNode = isValueNode(subgraph[tar]);
        }
    }
    return make_pair(lhsNode, rhsNode);
}

void EventReverser::generateReverseFunction(
        SgScopeStatement* scope,
        const SubValueGraph& route)
{
    SageBuilder::pushScopeStack(scope);

    // First, declare all temporary variables at the beginning of the reverse events.
    foreach (VGVertex node, boost::vertices(route))
    {
        ValueNode* valNode = isValueNode(route[node]);
        if (valNode == NULL) continue;
        if (valNode->isAvailable()) continue;

        SgStatement* varDecl = buildVarDeclaration(valNode);
        SageInterface::appendStatement(varDecl, scope);
    }

    // Generate the reverse code in reverse topological order of the route DAG.
    foreach (VGVertex node, getGraphNodesInTopologicalOrder(route))
    {
        if (node == root_) continue;

        ValueNode* valNode = isValueNode(route[node]);
        if (valNode == NULL)        continue;
        if (valNode->isAvailable()) continue;

        ROSE_ASSERT(boost::out_degree(node, route) == 1);

        VGVertex tar = *(boost::adjacent_vertices(node, route).first);
        SgStatement* rvsStmt = NULL;

        if (tar == root_)
        {
            // State saving edge.
            VGEdge edge = boost::edge(node, tar, route).first;
            if (route[edge]->cost == 0)
                rvsStmt = buildAssignOpertaion(valNode);
            else
            {
                // State saving here.
                // For forward event, we instrument a push function after the def.
                instrumentPushFunction(valNode);
                rvsStmt = buildPopFunction(valNode);
            }
        }
        else if (ValueNode* rhsValNode = isValueNode(route[tar]))
        {
            // Simple assignment.
            rvsStmt = buildAssignOpertaion(valNode, rhsValNode);
        }
        else if (OperatorNode* opNode = isOperatorNode(route[tar]))
        {
            // Rebuild the operation.
            ValueNode* lhsNode = NULL;
            ValueNode* rhsNode = NULL;
            boost::tie(lhsNode, rhsNode) = getOperands(tar, route);

            rvsStmt = buildOperation(valNode, opNode->type, lhsNode, rhsNode);
        }

        // Add the generated statement to the scope.
        if (rvsStmt)
            SageInterface::appendStatement(rvsStmt, scope);
    }

    // At last, assign the value restored back to state variables.
    // Note that those values are held by temporary variables before.
    foreach (VGVertex node, valuesToRestore_)
    {
        ValueNode* valNode = isValueNode(route[node]);

        SgExpression* lhs = valNode->var.getVarRefExp();
        SgExpression* rhs = SageBuilder::buildVarRefExp(valNode->var.toString());

        SgExpression* rvsExpr = SageBuilder::buildAssignOp(lhs, rhs);
        SgStatement* rvsStmt = SageBuilder::buildExprStatement(rvsExpr);

        SageInterface::appendStatement(rvsStmt, scope);
    }
        //else if (valuesToRestore_.count(node))

    SageBuilder::popScopeStack();
}


} // End of namespace Backstroke
