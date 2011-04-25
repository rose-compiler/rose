#include "valueGraph.h"
#include <utilities/utilities.h>
#include <boost/assign/list_inserter.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/graph/graphviz.hpp>

namespace Backstroke
{

using namespace std;

#define foreach BOOST_FOREACH


void EventReverser::buildValueGraph()
{
    // First, build the basic part of the value graph.
    buildBasicValueGraph();

    // Process all variable of their last versions.
    processLastVersions();

    // Add all state saving edges.
    addStateSavingEdges();

    // Add path information to edges.
    //addPathsToEdges();
    
    // Add a reverse edge for every non-ordered edge, and add extra nodes for + and -.
    addExtraNodesAndEdges();

    // Remove useless nodes.
    //removeUselessNodes();

    // Assign a global unique name for each node in VG.
    assignNameToNodes();
    
    valueGraphToDot("VG.dot");
}

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
                VGVertex newNode = createValueNode(initName, NULL);
                valuesToRestore_.push_back(newNode);
                stateVariables_.insert(VarName(1, initName));
            }
        }
        
        // Add all parameters of the event to the value graph.
        SgInitializedNamePtrList& paraList = funcDef_->get_declaration()->get_args();
        foreach (SgInitializedName* initName, paraList)
            createValueNode(initName, NULL);
    }
    else
    {
        // Add all parameters of the event to the value graph.
        SgInitializedNamePtrList& paraList = funcDef_->get_declaration()->get_args();
        foreach (SgInitializedName* initName, paraList)
        {
            VGVertex newNode = createValueNode(initName, NULL);

            // FIXME State variable may not be parameters.
            // Add the variable into wanted set.
            valuesToRestore_.push_back(newNode);
            stateVariables_.insert(VarName(1, initName));
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
        
                // It is possible that this var ref is actually a def. For example, a = 0;
                if (varVertexMap_.count(var) > 0)
                    nodeVertexMap_[expr] = varVertexMap_[var];
                continue;
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
            
            else if (SgFunctionCallExp* funcCall = isSgFunctionCallExp(expr))
            {
                const SSA::NodeReachingDefTable& defTable =
                            ssa_->getReachingDefsAtNode(funcCall->get_parent());

                #if 1
                        cout << "Print def table:\n";
                        typedef map<VarName, SSA::ReachingDefPtr>::value_type PT;
                        foreach (const PT& pt, defTable)
                        cout << "!" << SSA::varnameToString(pt.first) << " " << pt.second->getRenamingNumber() << endl;
                #endif              
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


void EventReverser::assignNameToNodes()
{
    static int counter = 0;

    set<string> names;
    foreach (VGVertex node, boost::vertices(valueGraph_))
    {
        ValueNode* valNode = isValueNode(valueGraph_[node]);
        if (valNode == NULL) continue;
        if (valNode->isAvailable()) continue;

        string name;
        if (valNode->isTemp())
            name = "temp" + boost::lexical_cast<string>(counter++);
        else
            name = valNode->var.toString();
        
        while (names.count(name) > 0)
            name += "_";
        names.insert(name);
        valNode->str = name;
    }
}


void EventReverser::addPathsToEdges()
{    
    vector<VGVertex> phiNodes;

    foreach (const VGEdge& e, boost::edges(valueGraph_))
    {
        ValueGraphEdge* edge = valueGraph_[e];

        // The ordered edge (edge from operator node) does not have path info.
        if (isOrderedEdge(edge)) continue;

        VGVertex src = boost::source(e, valueGraph_);
        //VGVertex tar = boost::target(e, valueGraph_);
        //ValueGraphNode* srcNode = valueGraph_[src];
        //ValueGraphNode* tarNode = valueGraph_[tar];

        // Phi nodes are taken care specially.
        if (isPhiNode(valueGraph_[src]))
        {
            phiNodes.push_back(src);
            continue;
        }

        if (ValueNode* valNode = isValueNode(valueGraph_[src]))
        {
            boost::tie(edge->dagIndex, edge->paths) =
                    pathNumManager_->getPathNumbers(valNode->astNode);
        }
    }

#if 0
    set<VGVertex> processedPhiNodes;
    foreach (VGVertex phiNode, phiNodes)
    {
        ValueGraphEdge* stateSavingEdge = 0;
        int dagIdx;
        PathSet paths;
        
        foreach (const VGEdge& e, boost::out_edges(phiNode, valueGraph_))
        {
            VGVertex tar = boost::target(e, valueGraph_);
            ValueGraphEdge* edge = valueGraph_[e];
            ValueGraphNode* node = valueGraph_[tar];

            if (isPhiNode(node))
                addPathsForPhiNodes(tar, processedPhiNodes);
            else if (tar == root_)
                stateSavingEdge = edge;
            else
            {
                ValueNode* valNode = isValueNode(node);
                ROSE_ASSERT(valNode);
                boost::tie(edge->dagIndex, edge->paths) =
                    pathNumManager_->getPathNumbers(valNode->astNode);

                // Collect information for state saving edge from this phi node.
                if (paths.empty())
                {
                    dagIdx = edge->dagIndex;
                    paths = edge->paths;
                }
                else
                    paths |= edge->paths;
            }
        }

        // Add the path information to the state saving edge from phi node.
        ROSE_ASSERT(stateSavingEdge);
        stateSavingEdge->dagIndex = dagIdx;
        stateSavingEdge->paths = paths;
    }
#endif
}

#if 0
EventReverser::PathSetWithIndex EventReverser::addPathsForPhiNodes(
        EventReverser::VGVertex phiNode,
        set<EventReverser::VGVertex>& processedPhiNodes)
{
    // Suppose all paths are in the same set.
    //map<VGEdge, PathSetWithIndex> paths;

    foreach (VGVertex tar, boost::adjacent_vertices(phiNode, valueGraph_))
    {
        if (isPhiNode(valueGraph_[tar]))
        {
            if (processedPhiNodes.count(phiNode) == 0)
                addPathsForPhiNodes(tar, processedPhiNodes);
        }
        if (isValueNode(valueGraph_[tar]))
        {
            
        }

    }

    processedPhiNodes.insert(phiNode);
}
#endif

void EventReverser::processLastVersions()
{
    // Collect all variables visible at the end of the event. It is needed since
    // a data member is not shown from SSA::getLastVersions() for a member event
    // function. So if a state variable does not appear the following set, it is
    // still available.
    set<VarName> varNamesAtEventEnd;
    
    // At the end of the event, find the versions of all variables,
    // and determine which variables are avaiable during the search of VG.
    typedef SSA::NodeReachingDefTable::value_type VarNameDefPair;
    foreach (const VarNameDefPair& nameDef,
             ssa_->getLastVersions(funcDef_->get_declaration()))
    {
        VarName name = nameDef.first;
        VGVertex node;
        
        varNamesAtEventEnd.insert(name);

        // For every variable, if it is not added into VG, add it now.
        VersionedVariable var(name, nameDef.second->getRenamingNumber());
        //cout << "VersionedVariable:" << var.toString() << endl;
        //printVarVertexMap();
        if (varVertexMap_.count(var) == 0)
        {
            //pseudoDefMap_[var] = nameDef.second;

            // Currently an object's member access is not added to VG.
            if (var.name.size() == 1)
                node = createPhiNode(var, nameDef.second);
        }
        else
            node = varVertexMap_[var];

        if (isStateVariable(name))
        {
            // If the variable is a state variable, make it available.
            availableValues_.insert(node);
        }
        else
        {
            // If this variable is not the state, it will be killed at the end
            // of the function. Add it to the killed set, then a state saving edge
            // is added to it.
            varsKilledAtEventEnd_.insert(node);
        }
    }
    
    foreach (const VarName& name, stateVariables_)
    {
        if (varNamesAtEventEnd.count(name) == 0)
        {
            VersionedVariable var(name, 0);
            ROSE_ASSERT(varVertexMap_.count(var));
            availableValues_.insert(varVertexMap_[var]);
        }
    }
}

void EventReverser::addExtraNodesAndEdges()
{
    //! Add reverse edges for phi nodes.
    vector<VGEdge> edges;
    foreach (const VGEdge& edge, boost::edges(valueGraph_))
        edges.push_back(edge);

    foreach (const VGEdge& edge, edges)
    {
        VGVertex src = boost::source(edge, valueGraph_);
        VGVertex tar = boost::target(edge, valueGraph_);

        // State saving edge.
        if (tar == root_)
            continue;

        // If the edge is not connected to an operator node, make a reverse copy.
        if (isOperatorNode(valueGraph_[src]) || isOperatorNode(valueGraph_[tar]))
            continue;

        addValueGraphEdge(tar, src, valueGraph_[edge]);
    }

    //! Add + and - nodes and edges for + and - operations.
    vector<VGVertex> vertices;
    foreach (VGVertex v, boost::vertices(valueGraph_))
        vertices.push_back(v);

    foreach (VGVertex node, vertices)
    {
        OperatorNode* opNode = isOperatorNode(valueGraph_[node]);
        if (opNode == NULL)
            continue;

        if (boost::in_degree(node, valueGraph_) == 0)
            continue;
        
        // The ingoing edge to this operation node.
        VGEdge inEdge = *(boost::in_edges(node, valueGraph_).first);
        
        // For + and - operations.
        if (opNode->type == V_SgAddOp || opNode->type == V_SgSubtractOp)
        {
            // Get all 3 operands first.
            VGVertex result, lhs, rhs;
            result = boost::source(inEdge, valueGraph_);

            foreach (const VGEdge& e, boost::out_edges(node, valueGraph_))
            {
                if (isOrderedEdge(valueGraph_[e])->index == 0)
                    lhs = boost::target(e, valueGraph_);
                else
                    rhs = boost::target(e, valueGraph_);
            }

            // a = b + b cannot be transformed to b = a - b
            if (lhs == rhs)
                continue;

            ValueGraphNode* lhsNode = valueGraph_[lhs];
            ValueGraphNode* rhsNode = valueGraph_[rhs];

            ValueNode* lhsValNode = isValueNode(lhsNode);
            ValueNode* rhsValNode = isValueNode(rhsNode);

            // If the operand is a constant, we don't have to build the relationship.
            if (!(lhsValNode && lhsValNode->isAvailable()))
            {
                VariantT tRev = (opNode->type == V_SgAddOp) ? V_SgSubtractOp : V_SgAddOp;
                VGVertex newNode = createOperatorNode(
                        tRev, lhs, result, rhs, valueGraph_[inEdge]);

                VGEdge newEdge = boost::edge(lhs, newNode, valueGraph_).first;
                VGEdge e = *(boost::in_edges(node, valueGraph_).first);
                *valueGraph_[newEdge] = *valueGraph_[e];
            }
            if (!(rhsValNode && rhsValNode->isAvailable()))
            {
                VGVertex newNode = createOperatorNode(
                        V_SgSubtractOp, rhs, result, lhs, valueGraph_[inEdge]);

                VGEdge newEdge = boost::edge(rhs, newNode, valueGraph_).first;
                VGEdge e = *(boost::in_edges(node, valueGraph_).first);
                *valueGraph_[newEdge] = *valueGraph_[e];
            }
        }
        // For ++ and -- operations.
        else if (opNode->type == V_SgPlusPlusOp || opNode->type == V_SgMinusMinusOp)
        {
            // Get both 2 operands first.
            VGVertex result, operand;
            result = *(boost::inv_adjacent_vertices(node, valueGraph_).first);
            operand = *(boost::adjacent_vertices(node, valueGraph_).first);

            VariantT tRev = (opNode->type == V_SgPlusPlusOp) ?
                V_SgMinusMinusOp : V_SgPlusPlusOp;
            VGVertex newNode = createOperatorNode(
                    tRev, operand, result, nullVertex(), valueGraph_[inEdge]);

            VGEdge newEdge = boost::edge(operand, newNode, valueGraph_).first;
            VGEdge e = *(boost::in_edges(node, valueGraph_).first);
            *valueGraph_[newEdge] = *valueGraph_[e];
        }
    }
}

EventReverser::VGVertex 
EventReverser::createPhiNode(VersionedVariable& var, SSA::ReachingDefPtr reachingDef)
{
    //cout << var.toString() << endl;
    
    //ROSE_ASSERT(pseudoDefMap_.count(var) > 0);
    //SSA::ReachingDefPtr reachingDef = pseudoDefMap_[var];
    SgNode* astNode = reachingDef->getDefinitionNode();

    // Add the phi node.
    VGVertex node = addValueGraphNode(new PhiNode(var, astNode));
    varVertexMap_[var] = node;

    // For every phi function parameter, chech if it is also a pseudo def.
    // If it is, add another phi node and connect them. Else, add an edge.
    typedef pair<SSA::ReachingDefPtr, set<ReachingDef::FilteredCfgEdge> > PairT;
	//pair<SSA::ReachingDefPtr, set<CFGEdge> > defEdgePair;
    foreach (const PairT& defEdgePair, reachingDef->getJoinedDefs())
    {
		SSA::ReachingDefPtr def = defEdgePair.first;
        const set<ReachingDef::FilteredCfgEdge>& cfgEdges = defEdgePair.second;
        int version = def->getRenamingNumber();

        //VGEdge newEdge;

        // If this def is also a phi node, add a varWithVersin entry
        // to the varReachingDefMap_ table.
        if (def->isPhiFunction())
        {
            VersionedVariable phiVar(var.name, version, true);
            // If this phi node is not added to VG.
            if (varVertexMap_.count(phiVar) == 0)
            //if (pseudoDefMap_.count(phiVar) == 0)
            {
                //pseudoDefMap_[phiVar] = def;
                VGVertex phiNode = createPhiNode(phiVar, def);
                addValueGraphPhiEdge(node, phiNode, cfgEdges);
            }
            else
            {
                ROSE_ASSERT(varVertexMap_.count(phiVar) > 0);
                addValueGraphPhiEdge(node, varVertexMap_[phiVar], cfgEdges);
            }
        }
        else
        {
            VersionedVariable defVar(var.name, version);
            ROSE_ASSERT(varVertexMap_.count(defVar) > 0);

            addValueGraphPhiEdge(node, varVertexMap_[defVar], cfgEdges);
        }

#if 0
        // Add path information for phi node here???
        ValueGraphEdge* edge = valueGraph_[newEdge];
        const set<ReachingDef::FilteredCfgEdge>& cfgEdges = defEdgePair.second;
        bool flag = true;
        foreach (const ReachingDef::FilteredCfgEdge& cfgEdge, cfgEdges)
        {
            cout << "!!!" << cfgEdge.toString() << endl;
            SgNode* node1 = cfgEdge.source().getNode();
            SgNode* node2 = cfgEdge.target().getNode();

            cout << node1->unparseToString() << endl;
            cout << node2->unparseToString() << endl;
            if (flag)
            {
                boost::tie(edge->dagIndex, edge->paths) =
                    pathNumManager_->getPathNumbers(node1, node2);
                flag = false;
                continue;
            }
            edge->paths |= pathNumManager_->getPathNumbers(node1, node2).second;
        }
        
#endif
        // If this def is not in the value graph, add it.

        //var.phiVersions.push_back(PhiNodeDependence(v));
    }

#if 0
    // Connect the pseudo def to real defs.
    foreach (const PhiNodeDependence& def, var.phiVersions)
    {
        // We cannot build a var like this since it may be another phi node.
        VersionedVariable varInPhi(var.name, def.version);

        cout << "Phi node dependence:" << varInPhi << endl;

        // If the node which defines this phi node is not in the table,
        // it should be another phi node.
        if (varVertexMap_.count(varInPhi) > 0)
        {
            // If this node is also a pseudo node.
            if (pseudoDefMap_.count(varInPhi) > 0)
            {
                varInPhi.isPseudoDef = true;
                createPhiNode(varInPhi);
            }
            else
                ROSE_ASSERT(false);
            ROSE_ASSERT(varVertexMap_.count(varInPhi) > 0);
        }

        addValueGraphEdge(node, varVertexMap_[varInPhi], 0);
    }
#endif

    return node;
}

EventReverser::VGVertex EventReverser::addValueGraphNode(ValueGraphNode* newNode)
{
#if 0
    if (VariableNode* varNode = isVariableNode(newNode))
        cout << "New var added:" << varNode->var << endl;
#endif

    VGVertex v = boost::add_vertex(valueGraph_);
    valueGraph_[v] = newNode;
    //	if (sgNode)
    //		nodeVertexMap_[sgNode] = v;
    return v;
}

EventReverser::VGEdge EventReverser::addValueGraphEdge(
        EventReverser::VGVertex src, 
        EventReverser::VGVertex tar,
        ValueGraphEdge* edgeToCopy)
{
    VGEdge newEdge = boost::add_edge(src, tar, valueGraph_).first;

    if (edgeToCopy)
    {
        valueGraph_[newEdge] = new ValueGraphEdge(*edgeToCopy);
        return newEdge;
    }

    ValueNode* valNode = isValueNode(valueGraph_[src]);
    ROSE_ASSERT(valNode);

    // Get the path information of this edge.
    int dagIndex;
    PathSet paths;
    boost::tie(dagIndex, paths) =
            pathNumManager_->getPathNumbers(valNode->astNode);
    
    //valueGraph_[e] = new ValueGraphEdge(valNode->getCost(), dagIndex, paths);
    valueGraph_[newEdge] = new ValueGraphEdge(0, dagIndex, paths);
    return newEdge;
}

void EventReverser::addValueGraphPhiEdge(
        EventReverser::VGVertex src, EventReverser::VGVertex tar,
        const std::set<ReachingDef::FilteredCfgEdge>& cfgEdges)
{
    // For every CFG edge, we add a VG edge. This is because we want each VG
    // edge to correspond a CFG edge.
    foreach (const ReachingDef::FilteredCfgEdge& cfgEdge, cfgEdges)
    {
        //cout << "!!!" << cfgEdge.toString() << endl;
        SgNode* node1 = cfgEdge.source().getNode();
        SgNode* node2 = cfgEdge.target().getNode();

        int dagIndex;
        PathSet paths;
        boost::tie(dagIndex, paths) =
            pathNumManager_->getPathNumbers(node1, node2);

        VGEdge newEdge = boost::add_edge(src, tar, valueGraph_).first;
        valueGraph_[newEdge] = new ValueGraphEdge(0, dagIndex, paths);
    }

    //valueGraph_[e] = new ValueGraphEdge(valNode->getCost(), dagIndex, paths);
    //valueGraph_[newEdge] = new ValueGraphEdge(0, dagIndex, paths);
    //return newEdge;
}

EventReverser::VGEdge EventReverser::addValueGraphOrderedEdge(
        EventReverser::VGVertex src, EventReverser::VGVertex tar, int index)
{
    VGEdge newEdge = boost::add_edge(src, tar, valueGraph_).first;
    valueGraph_[newEdge] = new OrderedEdge(index);
    return newEdge;
}

void EventReverser::addValueGraphStateSavingEdges(VGVertex src, SgNode* killer)
{
    // Get the cost to save this node.
    int cost = 0;
    if (availableValues_.count(src) == 0)
        cost = valueGraph_[src]->getCost();
    
    // Get the path information of this edge.
    int dagIndex;
    map<int, PathSet> paths;
    boost::tie(dagIndex, paths) =
            pathNumManager_->getVisiblePathNumbers(killer);
    
    ///cout << "***" << paths.size() << endl;

    typedef map<int, PathSet>::value_type NumPathPair;
    foreach (const NumPathPair& numPath, paths)
    {
        VGEdge newEdge = boost::add_edge(src, root_, valueGraph_).first;
        valueGraph_[newEdge] = new StateSavingEdge(
                cost, dagIndex, numPath.first, numPath.second, killer);
    }
}

vector<EventReverser::VGEdge>
EventReverser::addValueGraphStateSavingEdges(VGVertex src)
{
    // Get the cost to save this node.
    int cost = 0;
    if (availableValues_.count(src) == 0)
        cost = valueGraph_[src]->getCost();

    vector<VGEdge> newEdges;

    VGEdge newEdge = boost::add_edge(src, root_, valueGraph_).first;

    SgNode* astNode = NULL;
    if (ValueNode* valNode = isValueNode(valueGraph_[src]))
        astNode = valNode->astNode;
    ROSE_ASSERT(astNode);

    // Get the path information of this edge.
    int dagIndex;
    PathSet paths;
    boost::tie(dagIndex, paths) = pathNumManager_->getPathNumbers(astNode);

    valueGraph_[newEdge] = new ValueGraphEdge(cost, dagIndex, paths);

    // If the variable is killed at the exit of a scope, add a state saving edge to it.

    return newEdges;
}

set<EventReverser::VGVertex> EventReverser::getKillers(VGVertex killedNode)
{
    return set<VGVertex>();
}

EventReverser::VGVertex EventReverser::createValueNode(SgNode* lhsNode, SgNode* rhsNode)
{
    VGVertex lhsVertex;
    VGVertex rhsVertex;
    
    // Add state saving node here
    // If the lhs node is a declaration, no state saving is added here.
    if (lhsNode && !isSgInitializedName(lhsNode))
    {
        // Once a variable is defined, it may kill it previous def. Here we detect
        // all it killed defs then add state saving edges for them in this specific
        VersionedVariable var = getVersionedVariable(lhsNode, false);
        //cout << "New Var Defined: " << var.toString() << endl;
        SSA::NodeReachingDefTable defTable = ssa_->getReachingDefsAtNode(lhsNode);
        typedef SSA::NodeReachingDefTable::value_type reachingDefPair;
        foreach (const reachingDefPair& def, defTable)
        {
            if (def.first == var.name)
            {
                //cout << "Killed: " << def.second->getRenamingNumber() << endl;
                
                int version = def.second->getRenamingNumber();
                VersionedVariable killedVar(var.name, version);
                
#if 1
                // It is possible that the phi node is not built at this point.
                if (varVertexMap_.count(killedVar) == 0)
                {
                    ROSE_ASSERT(def.second->isPhiFunction());
                    createPhiNode(killedVar, def.second);
                }
#endif
                ROSE_ASSERT(varVertexMap_.count(killedVar));
                
                addValueGraphStateSavingEdges(varVertexMap_[killedVar], lhsNode);
            }
        }
    }

    if (rhsNode)
    {
        // If rhs node is not created yet, create it first.
        if (nodeVertexMap_.count(rhsNode) == 0)
        {
            //ROSE_ASSERT(!"Infeasible path???");
            rhsVertex = addValueGraphNode(new ValueNode(rhsNode));
            nodeVertexMap_[rhsNode] = rhsVertex;
        }
        else
            rhsVertex = nodeVertexMap_[rhsNode];
    }

    // If rhsNode just contains a rvalue, combine those two nodes.
    if (lhsNode && rhsNode)
    {
        ValueNode* rhsValNode = isValueNode(valueGraph_[rhsVertex]);
        ROSE_ASSERT(rhsValNode);
        if (rhsValNode->var.isNull())
        {
            rhsValNode->var = getVersionedVariable(lhsNode, false);
            varVertexMap_[rhsValNode->var] = rhsVertex;
            nodeVertexMap_[lhsNode] = rhsVertex;
            return rhsVertex;
        }
    }

    if (lhsNode)
    {
        VersionedVariable var = getVersionedVariable(lhsNode, false);
        ValueNode* valNode = new ValueNode(var, lhsNode);
        lhsVertex = addValueGraphNode(valNode);

        varVertexMap_[var] = lhsVertex;
        nodeVertexMap_[lhsNode] = lhsVertex;

        // If rhs node is not NULL, add an edge.
        if (rhsNode)
            addValueGraphEdge(lhsVertex, rhsVertex);
        
        return lhsVertex;
    }

    return rhsVertex;
}

EventReverser::VGVertex EventReverser::createOperatorNode(
        VariantT t,
        EventReverser::VGVertex result,
        EventReverser::VGVertex lhs,
        EventReverser::VGVertex rhs,
        ValueGraphEdge* edgeToCopy)
{
    // Add an operator node to VG.
    VGVertex op = addValueGraphNode(new OperatorNode(t));

    addValueGraphEdge(result, op, edgeToCopy);
    addValueGraphOrderedEdge(op, lhs, 0);
    if (rhs != nullVertex())
        addValueGraphOrderedEdge(op, rhs, 1);

    return op;
}

void EventReverser::addStateSavingEdges()
{
    foreach (VGVertex v, boost::vertices(valueGraph_))
    {
        ValueGraphNode* node = valueGraph_[v];

        // SS edges are only added to phi nodes and value nodes.
        ValueNode* valNode = isValueNode(node);
        if (valNode == NULL) continue;
        
#if 0
        // If the variable in this node is a stack variable, find its scope
        // and add a state saving edge for it containing the scope.
        SgNode* astNode = valNode->astNode;
        if (SgInitializedName* initName = isSgInitializedName(astNode))
        {
            // Note that the scope of a variable may be class (data member) or
            // function parameters, in which case we set its scope as function body.
            SgScopeStatement* scope = initName->get_scope();
            SgBasicBlock* funcBody = funcDef_->get_body();
            if (!SageInterface::isAncestor(funcBody, scope))
                scope = funcBody;
            
            // Find the last def of this variable in its definition scope.
            const SSA::NodeReachingDefTable& defTable = ssa_->getReachingDefsAtNode(scope);
            
            const VarName& varName = valNode->var.name;
            SSA::NodeReachingDefTable::const_iterator iter = defTable.find(varName);
            //ROSE_ASSERT(iter != defTable.end());
            if (iter != defTable.end())
            {
                SSA::ReachingDefPtr reachingDef = defTable.find(varName)->second;
                int version = reachingDef->getRenamingNumber();
                VersionedVariable var(varName, version);
                ROSE_ASSERT(varVertexMap_.count(var));
            
                addValueGraphStateSavingEdges(varVertexMap_[var], scope);
            }
        }
        
#endif
        
        // A temporary work-around for lacking of getting last defs for a variable.
        if (availableValues_.count(v) > 0)
        {
            addValueGraphStateSavingEdges(v, funcDef_->get_body());
        }
        
        // Add 
        
#if 0

        addValueGraphStateSavingEdges(v);
        
        // for backup
#if 0
        int cost  = 0;
        if (availableValues_.count(v) == 0)
            cost = node->getCost();
        addStateSavingEdge(v, cost);
#endif
        
#endif
    }
}

VersionedVariable EventReverser::getVersionedVariable(SgNode* node, bool isUse)
{
    //cout << node->class_name() << endl;
    VarName varName = SSA::getVarName(node);
    int version = -1;
#if 1
    SgInitializedName* initName = isSgInitializedName(node);
    if (initName && isSgClassDefinition(initName->get_declaration()->get_parent()))
    {
        varName = VarName(1, initName);
        version = 0;
    }
#endif
    ROSE_ASSERT(!varName.empty());

    // First, check if there is any use at the given node. If not, it should be a def.
    if (isUse)
    {
        const SSA::NodeReachingDefTable& defTable = ssa_->getUsesAtNode(node);

#if 0
        cout << "Print use table:\n";
        typedef map<VarName, SSA::ReachingDefPtr>::value_type PT;
        foreach (const PT& pt, defTable)
        cout << "!" << SSA::varnameToString(pt.first) << " " << pt.second->getRenamingNumber() << endl;
#endif
        
        SSA::NodeReachingDefTable::const_iterator iter = defTable.find(varName);
        if (iter != defTable.end())
        {
            //ROSE_ASSERT(defTable.count(varName) > 0);
            SSA::ReachingDefPtr reachingDef = iter->second;
            version = reachingDef->getRenamingNumber();

            // If its reaching def is a phi function, it's a pseudo def.
            // Note why we capture the phi node here is because we cannot access phi
            // nodes directly now from SSA.
            if (reachingDef->isPhiFunction())
            {
                VersionedVariable var(varName, version, true);
#if 0
                cout << "Found a phi node: " << var.toString() << "\n\n";
#endif
                //pseudoDefMap_[var] = reachingDef;
                createPhiNode(var, reachingDef);
                return var;
            }
        }
    }
    else
    {
#if 0
        cout << node->class_name() << endl;
        cout << node->get_parent()->class_name() << endl;
        cout << node->get_parent()->get_parent()->class_name() << endl;
#endif
        const SSA::NodeReachingDefTable& defTable =
            ssa_->getReachingDefsAtNode(node->get_parent());
            //ssa_->getReachingDefsAtNode(node->get_parent()->get_parent());

#if 0
        cout << "Print def table:\n";
        typedef map<VarName, SSA::ReachingDefPtr>::value_type PT;
        foreach (const PT& pt, defTable)
        cout << "!" << SSA::varnameToString(pt.first) << " " << pt.second->getRenamingNumber() << endl;
#endif

        SSA::NodeReachingDefTable::const_iterator iter = defTable.find(varName);
        if (iter != defTable.end())
        {
            SSA::ReachingDefPtr reachingDef = defTable.find(varName)->second;
            version = reachingDef->getRenamingNumber();
        }
        else
        {
#if 0
            // If the AST node is a variable declaration in a class, assign version
            // 0 to it.
            SgInitializedName* initName = isSgInitializedName(node);
            if (initName == NULL)
            {
                ROSE_ASSERT(!"Cannot find the given node in the def table.");
            }

            if (isSgClassDefinition(initName->get_declaration()->get_parent()))
            {
                //VersionedVariable vv(varName, 1);
                //cout << vv.toString() << endl;
                // Due to the bug in SSA, return a versioned variable with initname
                // with version 1 here.
                return VersionedVariable(VarName(1, initName), 1);
            }
#endif
        }
    }

    return VersionedVariable(varName, version);
}

void EventReverser::removeUselessEdges()
{
    vector<VGEdge> edgesToRemove;
    foreach (const VGEdge& e, boost::edges(valueGraph_))
    {
        // We do an optimization here by removing all out edges except state
        // saving one from an available node.
        VGVertex src = boost::source(e, valueGraph_);
        if (availableValues_.count(src) > 0)
        {
            VGVertex tar = boost::target(e, valueGraph_);
            if (tar != root_)
                edgesToRemove.push_back(e);
        }
    }

    foreach (const VGEdge& edge, edgesToRemove)
    {
        delete valueGraph_[edge];
        boost::remove_edge(edge, valueGraph_);
    }
}

void EventReverser::removeUselessNodes()
{
    // Remove redundant edges first, then more nodes can be removed.
    removeUselessEdges();
    
    set<VGVertex> usableNodes;

    stack<VGVertex> nodes;
    foreach(VGVertex node, valuesToRestore_)
    {
        usableNodes.insert(node);
        nodes.push(node);
    }

    while (!nodes.empty())
    {
        VGVertex n = nodes.top();
        nodes.pop();

        foreach (VGVertex tar, boost::adjacent_vertices(n, valueGraph_))
        {
            if (usableNodes.count(tar) == 0)
            {
                usableNodes.insert(tar);
                nodes.push(tar);
            }
        }
    }

    vector<VGEdge> edgesToRemove;

    // Remove edges first.
    foreach (const VGEdge& e, boost::edges(valueGraph_))
    {
        if (usableNodes.count(boost::source(e, valueGraph_)) == 0)
        {
            edgesToRemove.push_back(e);
            continue;
        }
    }

    foreach (const VGEdge& edge, edgesToRemove)
    {
        delete valueGraph_[edge];
        boost::remove_edge(edge, valueGraph_);
    }

    vector<VGVertex> verticesToRemove;

    // Then remove nodes.
    foreach (VGVertex v, boost::vertices(valueGraph_))
    {
        if (usableNodes.count(v) == 0)
            verticesToRemove.push_back(v);
    }

    foreach (VGVertex v, verticesToRemove)
    {
        delete valueGraph_[v];
        boost::remove_vertex(v, valueGraph_);
    }
}

void EventReverser::valueGraphToDot(const std::string& filename) const
{
    // Since the vetices are stored in a list, we have to give each vertex
    // a unique id here.
    int counter = 0;
    map<VGVertex, int> vertexIDs;
    foreach (VGVertex v, boost::vertices(valueGraph_))
        vertexIDs[v] = counter++;

    // Turn a std::map into a property map.
    boost::associative_property_map<map<VGVertex, int> > vertexIDMap(vertexIDs);

    ofstream ofile(filename.c_str(), std::ios::out);
    boost::write_graphviz(ofile, valueGraph_,
            boost::bind(&EventReverser::writeValueGraphNode, this, ::_1, ::_2),
            boost::bind(&EventReverser::writeValueGraphEdge, this, ::_1, ::_2),
            boost::default_writer(), vertexIDMap);
}

void EventReverser::routeGraphToDot(const std::string& filename) const
{
    // Since the vetices are stored in a list, we have to give each vertex
    // a unique id here.
    int counter = 0;
    map<VGVertex, int> vertexIDs;
    foreach (VGVertex v, boost::vertices(routeGraph_))
        vertexIDs[v] = counter++;

    // Turn a std::map into a property map.
    boost::associative_property_map<map<VGVertex, int> > vertexIDMap(vertexIDs);

    ofstream ofile(filename.c_str(), std::ios::out);
    boost::write_graphviz(ofile, routeGraph_,
            boost::bind(&EventReverser::writeValueGraphNode, this, ::_1, ::_2),
            boost::bind(&EventReverser::writeValueGraphEdge, this, ::_1, ::_2),
            boost::default_writer(), vertexIDMap);
}


} // End of namespace Backstroke
