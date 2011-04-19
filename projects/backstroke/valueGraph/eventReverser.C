#include "valueGraph.h"
#include "eventReverser.h"
#include <normalizations/expNormalization.h>
#include <boost/graph/topological_sort.hpp>

namespace Backstroke
{

using namespace std;

#define foreach BOOST_FOREACH

EventReverser::EventReverser(SgFunctionDefinition* funcDef)
:   funcDef_(funcDef)
{
    // Normalize the function.
    BackstrokeNorm::normalizeEvent(funcDef_->get_declaration());

    // Three new functions are built. Backup the original function here.
    buildFunctionBodies();

    cfg_ = new BackstrokeCFG(funcDef_);
    ssa_ = new SSA(SageInterface::getProject());
    ssa_->run(false);

    pathNumManager_ = new PathNumManager(cfg_);
}

EventReverser::~EventReverser()
{
    delete cfg_;
    delete ssa_;
    delete pathNumManager_;
}

SgVariableDeclaration* getStackVar(const string& name)
{
	//string typeName;

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

void EventReverser::generateCode()
{
    // First, build the value graph.
    buildValueGraph();
#if 1

    // Insert the declaration of the path number in the front of reverse function,
    // and define its value from a pop function call.
    string pathNumName = "__num__";
    using namespace SageBuilder;

    pushScopeStack(rvsFuncDef_->get_body());
    SgVariableDeclaration* pathNumDecl =
            SageBuilder::buildVariableDeclaration(
                pathNumName,
                SageBuilder::buildIntType(),
                SageBuilder::buildAssignInitializer(
                    buildPopFunctionCall(
                        buildIntType())));
    popScopeStack();

    SageInterface::prependStatement(pathNumDecl, rvsFuncDef_->get_body());

    size_t pathNum = pathNumManager_->getPathNum(0);
    for (size_t i = 0; i < pathNum; ++i)
    {
        // Build a if branch for each path.
        SgBasicBlock* rvsFuncBody = rvsFuncDef_->get_body();

        SgBasicBlock* ifBody = SageBuilder::buildBasicBlock();
        SgStatement* condition = SageBuilder::buildExprStatement(
                SageBuilder::buildIntVal(i));
        SgIfStmt* ifStmt = SageBuilder::buildIfStmt(condition, ifBody, NULL);

        SageInterface::appendStatement(ifStmt, rvsFuncBody);
        getSubGraph(ifBody, 0, i);
    }
    pathNumManager_->instrumentFunction(pathNumName);

    // Finally insert all functions in the code.
    insertFunctions();
#endif
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
                instrumentPushFunction(valNode, funcDef_);
                rvsStmt = buildRestorationStmt(valNode);
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

void EventReverser::insertFunctions()
{
    using namespace SageInterface;

    // Switch the bodies of original and forward functions. This is because the
    // previous instrumentation happens on original one but should be placed in
    // the forward function.
    SgBasicBlock* body = funcDef_->get_body();
    funcDef_->set_body(fwdFuncDef_->get_body());
    fwdFuncDef_->set_body(body);

    SgFunctionDeclaration* funcDecl    = funcDef_->get_declaration();
    SgFunctionDeclaration* fwdFuncDecl = fwdFuncDef_->get_declaration();
    SgFunctionDeclaration* rvsFuncDecl = rvsFuncDef_->get_declaration();
    SgFunctionDeclaration* cmtFuncDecl = cmtFuncDef_->get_declaration();

    insertStatementAfter(funcDecl,    fwdFuncDecl);
    insertStatementAfter(fwdFuncDecl, rvsFuncDecl);
    insertStatementAfter(rvsFuncDecl, cmtFuncDecl);
}


} // End of namespace Backstroke
