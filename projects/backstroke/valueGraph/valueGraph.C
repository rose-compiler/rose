#include "valueGraph.h"
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
    addPathsToEdges();
    
    // Add a reverse edge for every non-ordered edge, and add extra nodes for + and -.
    addExtraNodesAndEdges();

    // Remove useless nodes.
    removeUselessNodes();

    // Assign a global unique name for each node in VG.
    assignNameToNodes();

    buildForwardAndReverseEvent();

    cout << "Number of nodes: " << boost::num_vertices(valueGraph_) << endl;
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
                    pathNumManager_.getPathNumbers(valNode->astNode);
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
                    pathNumManager_.getPathNumbers(valNode->astNode);

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

void EventReverser::processLastVersions()
{
    // At the end of the event, find the versions of all variables,
    // and determine which variables are avaiable during the search of VG.
    typedef SSA::NodeReachingDefTable::value_type VarNameDefPair;
    foreach (const VarNameDefPair& nameDef, ssa_.getLastVersions(funcDef_->get_declaration()))
    {
        VarName name = nameDef.first;

        // For every variable, if it is not added into VG, add it now
        VersionedVariable var(name, nameDef.second->getRenamingNumber());
        if (varVertexMap_.count(var) == 0)
        {
            pseudoDefMap_[var] = nameDef.second;
            createPhiNode(var);
        }

        if (isStateVariable(name))
        {
            //VersionedVariable var(name, nameDef.second->getRenamingNumber());

            ROSE_ASSERT(varVertexMap_.count(var) || (cout << var, 0));
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
        
        // For + and - operations.
        if (opNode->type == V_SgAddOp || opNode->type == V_SgSubtractOp)
        {
            // Get all 3 operands first.
            VGVertex result, lhs, rhs;
            result = *(boost::inv_adjacent_vertices(node, valueGraph_).first);

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
                VGVertex newNode = createOperatorNode(tRev, lhs, result, rhs);

                VGEdge newEdge = boost::edge(lhs, newNode, valueGraph_).first;
                VGEdge e = *(boost::in_edges(node, valueGraph_).first);
                *valueGraph_[newEdge] = *valueGraph_[e];
            }
            if (!(rhsValNode && rhsValNode->isAvailable()))
            {
                VGVertex newNode = createOperatorNode(V_SgSubtractOp, rhs, result, lhs);

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
            VGVertex newNode = createOperatorNode(tRev, operand, result);

            VGEdge newEdge = boost::edge(operand, newNode, valueGraph_).first;
            VGEdge e = *(boost::in_edges(node, valueGraph_).first);
            *valueGraph_[newEdge] = *valueGraph_[e];
        }
    }
}

EventReverser::VGVertex EventReverser::createPhiNode(VersionedVariable& var)
{
    // Add the phi node.
    VGVertex node = addValueGraphNode(new PhiNode(var));
    varVertexMap_[var] = node;

    ROSE_ASSERT(pseudoDefMap_.count(var) > 0);
    SSA::ReachingDefPtr reachingDef = pseudoDefMap_[var];

    // For every phi function parameter, chech if it is also a pseudo def.
    // If it is, add another phi node and connect them. Else, add an edge.
	pair<SSA::ReachingDefPtr, CFGEdge> defEdgePair;
    foreach (defEdgePair, reachingDef->getJoinedDefs())
    {
		SSA::ReachingDefPtr def = defEdgePair.first;
        int version = def->getRenamingNumber();

        // If this def is also a phi node, add a varWithVersin entry
        // to the varReachingDefMap_ table.
        if (def->isPhiFunction())
        {
            VersionedVariable phiVar(var.name, version, true);
            // If this phi node is not added to VG.
            if (pseudoDefMap_.count(phiVar) == 0)
            {
                pseudoDefMap_[phiVar] = def;
                VGVertex phiNode = createPhiNode(phiVar);
                addValueGraphEdge(node, phiNode);
            }
            else
            {
                ROSE_ASSERT(varVertexMap_.count(phiVar) > 0);
                addValueGraphEdge(node, varVertexMap_[phiVar]);
            }
        }
        else
        {
            VersionedVariable defVar(var.name, version);
            ROSE_ASSERT(varVertexMap_.count(defVar) > 0);

            addValueGraphEdge(node, varVertexMap_[defVar]);
#if 0
            cout << "--- Add an edge from " << var << endl;
            VGOutEdgeIter e, f;
            for (boost::tie(e, f) = boost::out_edges(node, valueGraph_); e != f; ++e)
                cout << "* ";
            cout << endl;
#endif
        }
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

        // If the node which defines this phi node is not in the table, it should be another phi node.
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
        EventReverser::VGVertex src, EventReverser::VGVertex tar, int cost)
{
    VGEdge e = boost::add_edge(src, tar, valueGraph_).first;
    valueGraph_[e] = new ValueGraphEdge(cost);
    return e;
}

EventReverser::VGEdge EventReverser::addValueGraphEdge(
        EventReverser::VGVertex src, 
        EventReverser::VGVertex tar,
        ValueGraphEdge* edge)
{
    VGEdge e = boost::add_edge(src, tar, valueGraph_).first;
    valueGraph_[e] = new ValueGraphEdge(*edge);
    return e;
}

EventReverser::VGEdge EventReverser::addValueGraphOrderedEdge(
        EventReverser::VGVertex src, EventReverser::VGVertex tar, int index)
{
    VGEdge e = boost::add_edge(src, tar, valueGraph_).first;
    valueGraph_[e] = new OrderedEdge(index);
    return e;
}

EventReverser::VGVertex EventReverser::createValueNode(SgNode* lhsNode, SgNode* rhsNode)
{
    VGVertex newVertex;

    if (lhsNode)
    {
        //ROSE_ASSERT(isSgVarRefExp(lhsNode) || isSgInitializedName(lhsNode));

        ValueNode* valNode = new ValueNode(lhsNode);
        newVertex = addValueGraphNode(valNode);

        valNode->var = getVersionedVariable(lhsNode, false);
        varVertexMap_[valNode->var] = newVertex;
        nodeVertexMap_[lhsNode] = newVertex;

        // If rhs node is NULL, add an edge.
        if (rhsNode)
        {
            VGVertex rhsVertex;
            // If rhs node is not created yet, create it first.
            if (nodeVertexMap_.count(rhsNode) == 0)
            {
                //ROSE_ASSERT(!"Infeasible path???");
                rhsVertex = addValueGraphNode(new ValueNode(rhsNode));
                nodeVertexMap_[rhsNode] = newVertex;
            }
            else
                rhsVertex = nodeVertexMap_[rhsNode];
            
            addValueGraphEdge(newVertex, rhsVertex);
        }
    }
    else if (rhsNode)
    {
        newVertex = addValueGraphNode(new ValueNode(rhsNode));
        nodeVertexMap_[rhsNode] = newVertex;
    }
    else
        ROSE_ASSERT(!"At least one parameter should not be NULL!");

    return newVertex;
}

//void EventReverser::addVariableToNode(EventReverser::VGVertex v, SgNode* node)
//{
//    ValueNode* valNode = isValueNode(valueGraph_[v]);
//    ROSE_ASSERT(valNode);
//
//    // Note that we add a variable from its def, so the second parameter below
//    // is false.
//    valNode->var = getVersionedVariable(node, false);
//    varVertexMap_[valNode->var] = nodeVertexMap_[node] = v;
//}

EventReverser::VGVertex EventReverser::createOperatorNode(
        VariantT t,
        EventReverser::VGVertex result,
        EventReverser::VGVertex lhs,
        EventReverser::VGVertex rhs)
{
    // Add an operator node to VG.
    VGVertex op = addValueGraphNode(new OperatorNode(t));

    addValueGraphEdge(result, op);
    addValueGraphOrderedEdge(op, lhs, 0);
    addValueGraphOrderedEdge(op, rhs, 1);

    return op;
}

EventReverser::VGVertex EventReverser::createOperatorNode(
        VariantT t,
        EventReverser::VGVertex result,
        EventReverser::VGVertex operand)
{
    // Add an operator node to VG.
    VGVertex op = addValueGraphNode(new OperatorNode(t));

    addValueGraphEdge(result, op);
    addValueGraphOrderedEdge(op, operand, 0);

    return op;
}

void EventReverser::addStateSavingEdges()
{
    foreach (VGVertex v, boost::vertices(valueGraph_))
    {
        ValueGraphNode* node = valueGraph_[v];

        if (!isPhiNode(node) && !isValueNode(node))
            continue;

        int cost = 0;
        if (availableValues_.count(v) == 0)
            cost = node->getCost();

        addValueGraphEdge(v, root_, cost);
    }
}

VersionedVariable EventReverser::getVersionedVariable(SgNode* node, bool isUse)
{
    VarName varName = SSA::getVarName(node);
    int version;

    // First, check if there is any use at the given node. If not, it should be a def.
    if (isUse)
    {
        const SSA::NodeReachingDefTable& defTable = ssa_.getUsesAtNode(node);
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
                pseudoDefMap_[var] = reachingDef;
                return var;
            }
        }
    }
    else
    {
        const SSA::NodeReachingDefTable& defTable =
            ssa_.getReachingDefsAtNode(node->get_parent());
        SSA::NodeReachingDefTable::const_iterator iter = defTable.find(varName);
        if (iter != defTable.end())
        {
            SSA::ReachingDefPtr reachingDef = defTable.find(varName)->second;
            version = reachingDef->getRenamingNumber();
        }
        else
        {
            ROSE_ASSERT(!"Cannot find the given node in the def table.");
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
    {
        vertexIDs[v] = counter++;

#if 0
        if (isPhiNode(valueGraph_[v]))
        {
            VGOutEdgeIter e, f;
            for (boost::tie(e, f) = boost::out_edges(*v, valueGraph_); e != f; ++e)
                cout << "* ";
            cout << endl;
        }
#endif
    }

    // Turn a std::map into a property map.
    boost::associative_property_map<map<VGVertex, int> > vertexIDMap(vertexIDs);

    ofstream ofile(filename.c_str(), std::ios::out);
    boost::write_graphviz(ofile, valueGraph_,
            boost::bind(&EventReverser::writeValueGraphNode, this, ::_1, ::_2),
            boost::bind(&EventReverser::writeValueGraphEdge, this, ::_1, ::_2),
            boost::default_writer(), vertexIDMap);
}


} // End of namespace Backstroke
