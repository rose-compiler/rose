#include "valueGraph.h"
#include <boost/foreach.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/timer.hpp>

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

    // Add a reverse edge for every non-ordered edge.
    addReverseEdges();

    // Add all state saving edges.
    addStateSavingEdges();

    // Remove useless nodes.
    removeUselessNodes();

    // Add path information to edges.
    addPathsToEdges();

    cout << "Number of nodes: " << boost::num_vertices(valueGraph_) << endl;
}

void EventReverser::addPathsToEdges()
{    
    vector<VGVertex> phiNodes;

    VGEdgeIter ei, eiEnd;
    boost::tie(ei, eiEnd) = boost::edges(valueGraph_);
    for (; ei != eiEnd; ++ei)
    {
        ValueGraphEdge* edge = valueGraph_[*ei];

        // The ordered edge (edge from operator node) does not have path info.
        if (isOrderedEdge(edge)) continue;

        VGVertex src = boost::source(*ei, valueGraph_);
        VGVertex tar = boost::target(*ei, valueGraph_);
        //ValueGraphNode* srcNode = valueGraph_[src];
        //ValueGraphNode* tarNode = valueGraph_[tar];

        // Phi nodes are taken care specially.
        if (PhiNode* phiNode = isPhiNode(valueGraph_[src]))
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
        VGOutEdgeIter oei, oeiEnd;
        boost::tie(oei, oeiEnd) = boost::out_edges(phiNode, valueGraph_);
        for (; oei != oeiEnd; ++oei)
        {
            VGVertex tar = boost::target(*oei, valueGraph_);
            if (isPhiNode(valueGraph_[tar]))
                addPathsForPhiNodes(tar, processedPhiNodes);
        }

    }
#endif
}

EventReverser::PathSet EventReverser::addPathsForPhiNodes(
        EventReverser::VGVertex phiNode,
        set<EventReverser::VGVertex>& processedPhiNodes)
{
    // Suppose all paths are in the same set.
    map<VGEdge, PathSet> paths;

    VGOutEdgeIter oei, oeiEnd;
    boost::tie(oei, oeiEnd) = boost::out_edges(phiNode, valueGraph_);
    for (; oei != oeiEnd; ++oei)
    {
        VGVertex tar = boost::target(*oei, valueGraph_);
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
            addValueGraphPhiNode(var);
        }

        if (isStateVariable(name))
        {
            //VersionedVariable var(name, nameDef.second->getRenamingNumber());

            ROSE_ASSERT(varVertexMap_.count(var) || (cout << var, 0));
            availableValues_.insert(varVertexMap_[var]);
        }
    }
}

void EventReverser::addReverseEdges()
{
    vector<VGEdge> edges;
//    VGEdgeIter e, f;
//    for (boost::tie(e, f) = boost::edges(valueGraph_); e != f; ++e)
//        edges.push_back(*e);

    foreach (VGEdge e, boost::edges(valueGraph_))
    {
        edges.push_back(e);
    }

    foreach (VGEdge edge, edges)
    {
        VGVertex src = boost::source(edge, valueGraph_);
        VGVertex tar = boost::target(edge, valueGraph_);

        // If the edge is not connected to an operator node, make a reverse copy.
        if (!isOperatorNode(valueGraph_[src]) && !isOperatorNode(valueGraph_[tar]))
            addValueGraphEdge(tar, src, edge);
    }
}

EventReverser::VGVertex EventReverser::addValueGraphPhiNode(VersionedVariable& var)
{
    // Add the phi node.
    VGVertex node = addValueGraphNode(new PhiNode(var));
    varVertexMap_[var] = node;

    ROSE_ASSERT(pseudoDefMap_.count(var) > 0);
    SSA::ReachingDefPtr reachingDef = pseudoDefMap_[var];

    // For every phi function parameter, chech if it is also a pseudo def.
    // If it is, add another phi node and connect them. Else, add an edge.
    foreach (SSA::ReachingDefPtr def, reachingDef->getJoinedDefs())
    {
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
                VGVertex phiNode = addValueGraphPhiNode(phiVar);
                addValueGraphEdge(node, phiNode, 0);
            }
        }
        else
        {
            VersionedVariable defVar(var.name, version);
            ROSE_ASSERT(varVertexMap_.count(defVar) > 0);

            addValueGraphEdge(node, varVertexMap_[defVar], 0);
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
                addValueGraphPhiNode(varInPhi);
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
        EventReverser::VGEdge edge)
{
    VGEdge e = boost::add_edge(src, tar, valueGraph_).first;
    valueGraph_[e] = new ValueGraphEdge(*valueGraph_[edge]);
    return e;
}

EventReverser::VGEdge EventReverser::addValueGraphOrderedEdge(
        EventReverser::VGVertex src, EventReverser::VGVertex tar, int index)
{
    VGEdge e = boost::add_edge(src, tar, valueGraph_).first;
    valueGraph_[e] = new OrderedEdge(index);
    return e;
}

//void EventReverser::setNewDefNode(SgNode* defNode, VGVertex useVertex)
//{
//	// Get the var name and version for lhs.
//	VersionedVariable var = getVersionedVariable(defNode, false);
//
//	//// Find the node of rhs in the VG which should exist now.
//	//ROSE_ASSERT(nodeVertexMap_.count(rhs) > 0);
//	//Vertex rhsVertex = nodeVertexMap_.find(rhs)->second;
//
//	// Assign the node with the variable name and version.
//	//ROSE_ASSERT(node);
//
//	// If the node contains a temporary variable, replace it with a variable node,
//	// else build a new node and connect them.
//	VariableNode* varNode = isVariableNode(valueGraph_[useVertex]);
//	if (varNode && varNode->isTemp)
//	{
////			delete valueGraph_[useVertex];
////			valueGraph_[useVertex] = new VariableNode(var);
//		// If the node is a variable node, assign the var name to it.
//		varNode->setVariable(var);
//
//		// Update the node to vertex table.
//		nodeVertexMap_[defNode] = useVertex;
//		// Update the var to vertex map.
//		varVertexMap_[var] = useVertex;
//	}
////		else if (ValueNode* valNode = isValueNode(node))
////		{
////			// If the node is a value node, assign the var name to it.
////			valNode->var = var;
////
////			// Update the node to vertex table.
////			nodeVertexMap_[defNode] = useVertex;
////			// Update the var to vertex map.
////			varVertexMap_[var] = useVertex;
////		}
//	else
//	{
//		VGVertex newVertex = addValueGraphNode(new VariableNode(var), defNode);
//		addValueGraphEdge(newVertex, useVertex);
//
//		// Update tables.
//		nodeVertexMap_[defNode] = newVertex;
//		varVertexMap_[var] = newVertex;
//	}
//}

EventReverser::VGVertex EventReverser::createValueOneNode()
{
    SgValueExp* valueOne = SageBuilder::buildIntVal(1);
    return addValueGraphNode(new ValueNode(valueOne));
}


EventReverser::VGVertex EventReverser::createValueNode(SgNode* node)
{
    ValueNode* valNode = new ValueNode(node);

    // If it is a variable node.
    if (isSgVarRefExp(node) || isSgInitializedName(node))
    {
        VersionedVariable var = getVersionedVariable(node, false);
        valNode->vars.push_back(var);

        VGVertex newVertex = addValueGraphNode(valNode);
        varVertexMap_[var] = newVertex;
        nodeVertexMap_[node] = newVertex;

        return newVertex;
    }

    VGVertex newVertex = addValueGraphNode(valNode);
    nodeVertexMap_[node] = newVertex;

    return newVertex;
}

void EventReverser::addVariableToNode(EventReverser::VGVertex v, SgNode* node)
{
    ValueNode* valNode = isValueNode(valueGraph_[v]);
    ROSE_ASSERT(valNode);

    // Note that we add a variable from its def, so the second parameter below
    // is false.
    VersionedVariable var = getVersionedVariable(node, false);
    valNode->vars.push_back(var);

    //cout << ">>" << var <<endl;

    varVertexMap_[var] = nodeVertexMap_[node] = v;
}

EventReverser::VGVertex EventReverser::createOperatorNode(
        VariantT t,
        EventReverser::VGVertex result,
        EventReverser::VGVertex lhs,
        EventReverser::VGVertex rhs)
{
    // Add an operator node to VG.
    VGVertex op = addValueGraphNode(new OperatorNode(t));

    addValueGraphEdge(result, op, 0);
    addValueGraphOrderedEdge(op, lhs, 0);
    addValueGraphOrderedEdge(op, rhs, 1);

    return op;
}

void EventReverser::addStateSavingEdges()
{
    VGVertexIter v, w;
    for (boost::tie(v, w) = boost::vertices(valueGraph_); v != w; ++v)
    {
        ValueGraphNode* node = valueGraph_[*v];

        if (!isPhiNode(node) && !isValueNode(node))
            continue;

        int cost = 0;
        if (availableValues_.count(*v) == 0)
            cost = node->getCost();

        addValueGraphEdge(*v, root_, cost);
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
            SSA::ReachingDefPtr reachingDef = defTable.find(varName)->second;
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

void EventReverser::removeUselessNodes()
{
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

        VGOutEdgeIter e, f;
        for (boost::tie(e, f) = boost::out_edges(n, valueGraph_); e != f; ++e)
        {
            VGVertex tar = boost::target(*e, valueGraph_);
            if (usableNodes.count(tar) == 0)
            {
                usableNodes.insert(tar);
                nodes.push(tar);
            }
        }
    }

    // Remove edges first.
    VGEdgeIter ei, eiEnd, eiNext;
    boost::tie(ei, eiEnd) = boost::edges(valueGraph_);
    for (eiNext = ei; ei != eiEnd; ei = eiNext)
    {
        ++eiNext;
        if (usableNodes.count(boost::source(*ei, valueGraph_)) == 0)
        {
            delete valueGraph_[*ei];
            boost::remove_edge(*ei, valueGraph_);
        }
    }

    // Then remove nodes.
    VGVertexIter vi, viEnd, viNext;
    boost::tie(vi, viEnd) = boost::vertices(valueGraph_);
    for (viNext = vi; vi != viEnd; vi = viNext)
    {
        ++viNext;
        if (usableNodes.count(*vi) == 0)
        {
            delete valueGraph_[*vi];
            boost::remove_vertex(*vi, valueGraph_);
        }
    }
}

void EventReverser::valueGraphToDot(const std::string& filename) const
{
    // Since the vetices are stored in a list, we have to give each vertex
    // a unique id here.
    int counter = 0;
    map<VGVertex, int> vertexIDs;
    VGVertexIter v, w;
    for (boost::tie(v, w) = boost::vertices(valueGraph_); v != w; ++v)
    {
        vertexIDs[*v] = counter++;

#if 0
        if (isPhiNode(valueGraph_[*v]))
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



#if 0


struct VertexInfo
{
    VertexInfo(EventReverser::VGVertex ver = 0)
        : v(ver), dist(INT_MAX / 2), visited(false), joinNode(false)
    {
        predDist[0] = predDist[1] = INT_MAX / 2;
    }

    //	bool operator < (const VertexInfo& info) const
    //	{ return dist > info.dist; }

    EventReverser::VGVertex v;
    int dist;
    //vector<EventReverser::VGVertex> predecessors;
    vector<EventReverser::VGEdge> edges;
    bool visited;

    bool joinNode;
    int predDist[2];
};

void EventReverser::shortestPath()
{
    VGVertexIter v, w;
    vector<VertexInfo> verticesInfos(boost::num_vertices(valueGraph_));
    //priority_queue<VertexInfo> workList;
    typedef pair<int, VGVertex> DistVertexPair;
    priority_queue<DistVertexPair, vector<DistVertexPair>, greater<DistVertexPair> > workList;

    for (boost::tie(v, w) = vertices(valueGraph_); v != w; ++v)
    {
        VertexInfo info(*v);
        if (*v == root_)
            info.dist = 0;
        if (isOperatorNode(valueGraph_[*v]))
            info.joinNode = true;

        verticesInfos[info.v] = info;
    }
    workList.push(make_pair(0, root_));

    while (!workList.empty())
    {
        VGVertex r = workList.top().second;
        workList.pop();

        if (verticesInfos[r].visited)
            continue;

        VGOutEdgeIter e, f;
        for (boost::tie(e, f) = boost::out_edges(r, valueGraph_); e != f; ++e)
        {
            VGVertex v = boost::target(*e, valueGraph_);
            VertexInfo& info = verticesInfos[v];
            if (info.visited)
                continue;

            int newDist = verticesInfos[r].dist + valueGraph_[*e]->cost;
            if (!info.joinNode)
            {
                if (info.dist > newDist)
                {
                    info.dist = newDist;
                    info.edges = verticesInfos[r].edges;
                    info.edges.push_back(*e);
                    //					info.predecessors = verticesInfos[r].predecessors;
                    //					info.predecessors.push_back(r);
                }
            }
            else
            {
                ROSE_ASSERT(isOrderedEdge(valueGraph_[*e]));
                int idx = isOrderedEdge(valueGraph_[*e])->index;
                ROSE_ASSERT(idx >= 0 && idx <= 1);

                info.predDist[idx] = newDist;
                info.dist = info.predDist[0] + info.predDist[1];

                ROSE_ASSERT(!verticesInfos[r].edges.empty());

                info.edges.insert(info.edges.end(),
                        verticesInfos[r].edges.begin(), verticesInfos[r].edges.end());
                info.edges.push_back(*e);
            }

            workList.push(make_pair(info.dist, v));
        }
        verticesInfos[r].visited = true;
    }

    //	foreach (VertexInfo& info, verticesInfos)
    //	{
    //		cout << valueGraph_[info.v]->toString() << endl << info.v << ':' << info.dist << endl;
    //	}

    foreach (VGVertex v, valuesToRestore_)
        dagEdges_.insert(dagEdges_.end(),
                verticesInfos[v].edges.begin(), verticesInfos[v].edges.end());

    foreach (VGEdge e, dagEdges_)
        cout << e << endl;
}

namespace // anonymous namespace
{

// A local structure to help find all paths.
struct PathInfo
{
    set<EventReverser::VGVertex> nodes;
    set<OperatorNode*> operations;
    int cost;
    set<EventReverser::VGVertex> nodesRestored;
};

bool increaseIndex(vector<int>& indices, const vector<int>& maxIndices)
{
    int i = 0;
    while(1)
    {
        if (indices[i] == maxIndices[i])
        {
            if (i == indices.size() - 1)
                return false;
            indices[i] = 0;
            ++i;
        }
        else
        {
            ++indices[i];
            return true;
        }
    }
    return true;
}

} // end of anonymous

void EventReverser::getPath()
{
    // A map from each state variable to all its possible paths.
    map<VGVertex, vector<PathInfo> > allPaths;

    int maxCost = 0;
    foreach (VGVertex s, valuesToRestore_)
        maxCost += isValueNode(valueGraph_[s])->cost;

    foreach (VGVertex s, valuesToRestore_)
    {
        ValueNode* valNode = isValueNode(valueGraph_[s]);
        ROSE_ASSERT(valNode);

        vector<PathInfo> paths;
        PathInfo path;
        path.nodes.insert(s);
        path.cost = valNode->cost;
        path.nodesRestored.insert(s);

        paths.push_back(path);

        // Start to search all possible paths for this variable.

        for (size_t i = 0; i < paths.size(); ++i)
        {
            // Here we try to spread each node in the list.
            // Copy the list to avoid invalidation due to the push_back().
            set<VGVertex> nodes = paths[i].nodes;
            foreach (VGVertex v, nodes)
            {
                ROSE_ASSERT(isValueNode(valueGraph_[v]));

                // If the cost is 0, it is not necessary to go through this node.
                if (isValueNode(valueGraph_[v])->cost == 0)
                    continue;

                PathInfo p = paths[i];

                // Remove this node from the list, then spread through it.
                p.nodes.erase(p.nodes.find(v));
                p.nodesRestored.insert(v);

                VGOutEdgeIter e, f;
                for (boost::tie(e, f) = boost::out_edges(v, valueGraph_); e != f; ++e)
                {
                    VGVertex tar = boost::target(*e, valueGraph_);
                    if (OperatorNode* operNode = isOperatorNode(valueGraph_[tar]))
                    {
                        PathInfo newPath = p;

                        VGOutEdgeIter g, h;
                        vector<VGVertex> operands;
                        for (boost::tie(g, h) = boost::out_edges(tar, valueGraph_); g != h; ++g)
                        {
                            VGVertex operand = boost::target(*g, valueGraph_);
                            if (p.nodesRestored.count(operand) > 0)
                                goto NEXT;
                            operands.push_back(operand);
                        }
                        ROSE_ASSERT(operands.size() == 2);

                        foreach (VGVertex oper, operands)
                            newPath.nodes.insert(oper);
                        newPath.operations.insert(operNode);

                        // If the cost of this path is greater than the max cost, discard it.
                        newPath.cost = 0;
                        foreach (VGVertex n, newPath.nodes)
                            newPath.cost += isValueNode(valueGraph_[n])->cost;
                        if (newPath.cost >= maxCost)
                            continue;

                        paths.push_back(newPath);
                    }
NEXT:
                    ;
                }
            }
        }

        cout << "All paths for " << s << endl;
        foreach (const PathInfo& info, paths)
        {
            foreach (VGVertex v, info.nodes)
                cout << v << ' ';
            cout << info.cost << endl;
        }
        cout << endl << endl;


        allPaths[s].swap(paths);
    }

    // Find the optimal combination of paths of all state variables.
    vector<int> indices(valuesToRestore_.size(), 0);
    vector<int> maxIndices;
    foreach (VGVertex v, valuesToRestore_)
        maxIndices.push_back(allPaths[v].size() - 1);

    vector<int> results;
    int minCost = INT_MAX;
    do
    {
        set<VGVertex> allNodes;
        int i = 0;
        int cost = 0;
        foreach (VGVertex v, valuesToRestore_)
        {
            const set<VGVertex>& nodes = allPaths[v][indices[i++]].nodes;
            allNodes.insert(nodes.begin(), nodes.end());

            cost = 0;
            foreach (VGVertex w, allNodes)
                cost += isValueNode(valueGraph_[w])->cost;
            if (cost >= maxCost)
                goto NEXT2;
        }
        if (cost < minCost)
        {
            cout << cost << endl;
            results = indices;
            minCost = cost;
        }
NEXT2:
        ;
    } while (increaseIndex(indices, maxIndices));
}



#endif

} // End of namespace Backstroke
