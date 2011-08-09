#include "valueGraph.h"
#include "functionReverser.h"
#include <normalizations/expNormalization.h>
#include <utilities/utilities.h>

#include <boost/graph/graphviz.hpp>
#include <boost/graph/reverse_graph.hpp>
#include <boost/graph/topological_sort.hpp>

namespace Backstroke
{

using namespace std;

#define foreach BOOST_FOREACH
#define reverse_foreach BOOST_REVERSE_FOREACH

EventReverser::EventReverser(SgFunctionDefinition* funcDef)
:   funcDef_(funcDef)
{
    // Normalize the function.
    BackstrokeNorm::normalizeEvent(funcDef_->get_declaration());

    // Three new functions are built. Backup the original function here.
    buildFunctionBodies();

    cfg_ = new BackstrokeCFG(funcDef_);
    ssa_ = new SSA(SageInterface::getProject());
    ssa_->run(true, true);

    pathNumManager_ = new PathNumManager(cfg_);
}

EventReverser::~EventReverser()
{
    delete cfg_;
    delete ssa_;
    delete pathNumManager_;
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

#if 0
    // Declare all temporary variables at the beginning of the reverse events.
    foreach (VGVertex node, boost::vertices(valueGraph_))
    {
        ValueNode* valNode = isValueNode(valueGraph_[node]);
        if (valNode == NULL) continue;
        if (valNode->isAvailable()) continue;

        SgStatement* varDecl = buildVarDeclaration(valNode);
        SageInterface::appendStatement(varDecl, rvsFuncDef_->get_body());
    }
#endif

    map<VGEdge, PathSet> routes;

    // Just for DAG 0.
    size_t pathNum = pathNumManager_->getNumberOfPath(0);
    for (size_t i = 0; i < pathNum; ++i)
    {
        set<VGEdge> route = getRouteFromSubGraph(0, i);

        foreach (const VGEdge& edge, route)
        {
            if (routes.count(edge) == 0)
                routes[edge].resize(pathNum);
            routes[edge].set(i);
        }
    }
    
    buildRouteGraph(routes);
    routeGraphToDot("routeGraph.dot");

#if 0
    typedef map<VGEdge, PathSet>::value_type EdgePathPair;
    foreach (const EdgePathPair& edgePath, routes)
    {
        cout << edgePath.second << ' ';
        writeValueGraphEdge(cout, edgePath.first);
        cout << '\n';
    }
#endif

    // Generate the reverse code for rvs event.
    ReverseCFG rvsCFG;
    buildReverseCFG(0, rvsCFG);
    
#ifdef VG_DEBUG
    ofstream ofile("rvsCFG.dot", std::ios::out);
    boost::write_graphviz(ofile, rvsCFG);
    ofile.close();
#endif

    generateCode(0, rvsCFG, rvsFuncDef_->get_body(), pathNumName);

    pathNumManager_->instrumentFunction(pathNumName);

    // Finally insert all functions in the code.
    insertFunctions();
    
    // It is likely that there are lots of empty if statements in reverse event.
    // Remove them here.
    removeEmptyIfStmt();
#endif
}

void EventReverser::buildRouteGraph(const map<VGEdge, PathSet>& routes)
{
    map<VGVertex, VGVertex> nodeTable;
    
    typedef map<VGEdge, PathSet>::value_type EdgePathPair;
    foreach (const EdgePathPair& edgePath, routes)
    {
        const VGEdge& edge = edgePath.first;
        const PathSet& paths = edgePath.second;
        
        VGVertex src = boost::source(edge, valueGraph_);
        VGVertex tgt = boost::target(edge, valueGraph_);
        
        VGVertex newSrc, newTgt;
        
        map<VGVertex, VGVertex>::iterator iter;
        
        iter = nodeTable.find(src);
        if (iter == nodeTable.end())
        {
            newSrc = boost::add_vertex(routeGraph_);
            routeGraph_[newSrc] = valueGraph_[src];
            nodeTable[src] = newSrc;
        }
        else
            newSrc = iter->second;
                
        iter = nodeTable.find(tgt);
        if (iter == nodeTable.end())
        {
            newTgt = boost::add_vertex(routeGraph_);
            routeGraph_[newTgt] = valueGraph_[tgt];
            nodeTable[tgt] = newTgt;
        }
        else
            newTgt = iter->second;
        
        VGEdge e = boost::add_edge(newSrc, newTgt, routeGraph_).first;
        
        ValueGraphEdge* newEdge = valueGraph_[edge]->clone();
        newEdge->paths = paths;
        routeGraph_[e] = newEdge;
    }
    
    // Set the root of the route graph.
    routeGraphRoot_ = nodeTable[root_];
    
    // Switch all nodes in valuesToRestore_ to those in route graph.
    // It is important since the code generation is performed on route graph, not
    // value graph.
    vector<VGVertex> newValuesToRestore;
    foreach (VGVertex node, valuesToRestore_)
    {
        if (nodeTable.count(node) > 0)
            newValuesToRestore.push_back(nodeTable[node]);
    }
    valuesToRestore_.swap(newValuesToRestore);
    
    removePhiNodesFromRouteGraph();
}

void EventReverser::removePhiNodesFromRouteGraph()
{
    set<VGVertex> nodesToRemove;
    
    // Then remove edges on phi nodes to ease the source code generation.
    foreach (VGVertex node, boost::vertices(routeGraph_))
    {
        if (isPhiNode(routeGraph_[node]))
        {
            // If there exists a pair of in and out edges for this phi node and 
            // the paths of one of them is a subset of the other one, we can forward
            // an edge by passing this phi node.
            foreach (const VGEdge& outEdge, boost::out_edges(node, routeGraph_))
            //if (boost::out_degree(node, routeGraph_) == 1)
            {
                //VGEdge outEdge = *(boost::out_edges(node, routeGraph_).first);
                VGVertex tgt = boost::target(outEdge, routeGraph_);
                const PathSet& pathsOnOutEdge = routeGraph_[outEdge]->paths;
                
                foreach (const VGEdge& inEdge, boost::in_edges(node, routeGraph_))
                {
                    const PathSet& pathsOnInEdge =  routeGraph_[inEdge]->paths;
                    
                    //ROSE_ASSERT(pathsOnInEdge.is_subset_of(pathsOnOutEdge) ||
                    //        pathsOnOutEdge.is_subset_of(pathsOnInEdge));
                    
                    // Add a new edge first, then attach the information in which
                    // the cost is the one on the out edge and the path set is the one
                    // on the in edge.
                    PathSet newPaths = pathsOnInEdge & pathsOnOutEdge;
                    if (!newPaths.any())
                        continue;
                    
                    VGVertex src = boost::source(inEdge, routeGraph_);
                    
                    // Multiple state saving edges with cost 0 can be merged.
                    // Then we don't have to add a new edge.
                    StateSavingEdge* ssEdge = isStateSavingEdge(routeGraph_[outEdge]);
                    if (ssEdge && ssEdge->cost == 0)
                    {
                        bool flag = false;
                        foreach (const VGEdge& e, boost::out_edges(src, routeGraph_))
                        {
                            // Their targets must be the same.
                            if (boost::target(e, routeGraph_) != tgt)
                                continue;
                            
                            StateSavingEdge* ssEdge2 = isStateSavingEdge(routeGraph_[e]);
                            if (ssEdge2 && ssEdge2->cost == 0)
                            {
                                ssEdge2->paths |= newPaths;
                                flag = true;
                                break;
                            }
                        }
                        if (flag) continue;
                    }
                    
                    ValueGraphEdge* newEdge = routeGraph_[outEdge]->clone();
                    newEdge->paths = newPaths;
                    VGEdge e = boost::add_edge(src, tgt, routeGraph_).first;
                    routeGraph_[e] = newEdge;
                    
#if 0
                    if (pathsOnInEdge.is_subset_of(pathsOnOutEdge))
                        newEdge->paths = pathsOnInEdge;
                    else if (pathsOnOutEdge.is_subset_of(pathsOnInEdge))
                        newEdge->paths = pathsOnOutEdge;
                    else
                    {
                        //ROSE_ASSERT(false);
                        continue;
                    }
#endif
                    
                    //newEdge->paths = routeGraph_[inEdge]->paths;
                }
            }
            
            nodesToRemove.insert(node);
        }        
    }

    
    // Remove phi nodes.
    foreach (VGVertex node, nodesToRemove)
    {
        boost::clear_vertex(node, routeGraph_);
        boost::remove_vertex(node, routeGraph_);
    }
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
EventReverser::getOperands(VGVertex opNode) const
{
    ValueNode* lhsNode = NULL;
    ValueNode* rhsNode = NULL;

    // If it's a unary operation.
    if (boost::out_degree(opNode, valueGraph_) == 1)
    {
        VGVertex lhs = *(boost::adjacent_vertices(opNode, valueGraph_).first);
        lhsNode = isValueNode(valueGraph_[lhs]);
    }
    else
    {
        foreach (const VGEdge& edge, boost::out_edges(opNode, valueGraph_))
        {
            VGVertex tar = boost::target(edge, valueGraph_);
            if (isOrderedEdge(valueGraph_[edge])->index == 0)
                lhsNode = isValueNode(valueGraph_[tar]);
            else
                rhsNode = isValueNode(valueGraph_[tar]);
        }
    }
    return make_pair(lhsNode, rhsNode);
}

void EventReverser::getRouteGraphEdgesInProperOrder(int dagIndex, vector<VGEdge>& result)
{
    map<PathSet, int> pathsIndexTable = pathNumManager_->getPathsIndices(dagIndex);
    
    foreach (const VGEdge& edge, boost::edges(routeGraph_))
    {
        PathSet paths = routeGraph_[edge]->paths;
        if (pathsIndexTable.count(paths) == 0)
            pathsIndexTable[paths] = INT_MAX;
    }
    
#if 0
    for (map<PathSet, int>::iterator it = pathsIndexTable.begin(), itEnd = pathsIndexTable.end();
            it != itEnd; ++it)
        cout << it->first << " : " << it->second << endl;
#endif
    
    // A priority queue, in which elements are sorted by its index from edgeIndexTable.
    priority_queue<VGEdge, vector<VGEdge>, RouteGraphEdgeComp>  
    nextEdgeCandidates(RouteGraphEdgeComp(routeGraph_, pathsIndexTable));
    
    map<VGVertex, PathSet> nodePathsTable;
    foreach (VGVertex node, boost::vertices(routeGraph_))
        nodePathsTable[node].resize(pathNumManager_->getNumberOfPath(dagIndex));
    
    set<VGEdge> traversedEdges;
    
    foreach (const VGEdge& inEdge, boost::in_edges(routeGraphRoot_, routeGraph_))
    {
        //cout << "Paths on edge: " << routeGraph_[inEdge]->paths << endl;
        nextEdgeCandidates.push(inEdge);
    }
    
    while (!nextEdgeCandidates.empty())
    {
        VGEdge edge = nextEdgeCandidates.top();
        result.push_back(edge);
        nextEdgeCandidates.pop();
        
        VGVertex src = boost::source(edge, routeGraph_);
        PathSet& pathsOnSrc = nodePathsTable[src];
        pathsOnSrc |= routeGraph_[edge]->paths;
        
        // After updating the traversed path set of the source node of the edge
        // just added to candidates, check all in edges of this source node to see
        // if any of it can be added to candidates.
        foreach (const VGEdge& inEdge, boost::in_edges(src, routeGraph_))
        {
            if (routeGraph_[inEdge]->paths.is_subset_of(pathsOnSrc))
            {
                if (traversedEdges.count(inEdge) == 0)
                {
                    nextEdgeCandidates.push(inEdge);
                    traversedEdges.insert(inEdge);
                }
            }
        }
    }    
}

void EventReverser::buildReverseCFG(
        int dagIndex, ReverseCFG& rvsCFG)
{
    set<VGEdge> visitedEdges;
    map<PathSet, RvsCFGVertex> rvsCFGBasicBlock;
    map<PathSet, PathSet> parentTable;
    
    
    // Add a initial node to reverse CFG indicating the entry.
    //RvsCFGVertex entry = boost::add_vertex(rvsCFG);
    PathSet allPaths = pathNumManager_->getAllPaths(dagIndex);
    parentTable[allPaths] = allPaths;
    
    addReverseCFGNode(allPaths, NULL, rvsCFG, rvsCFGBasicBlock, parentTable);
    //rvsCFG[entry].first = allPaths;
    //rvsCFGBasicBlock[allPaths] = entry;

    vector<VGEdge> result;
    getRouteGraphEdgesInProperOrder(dagIndex, result);
    
#if 1
    foreach (const VGEdge& edge, result)
        cout << "!!!" << routeGraph_[edge]->toString() << endl;
#endif
    
    
    reverse_foreach (const VGEdge& edge, result)
    {
        const PathSet& paths = routeGraph_[edge]->paths;
        addReverseCFGNode(paths, &edge, rvsCFG, rvsCFGBasicBlock, parentTable);
    }
    
    // Add the exit node.
    addReverseCFGNode(allPaths, NULL, rvsCFG, rvsCFGBasicBlock, parentTable);
    
    //rvsCFG = boost::make_reverse_graph(rvsCFG);
}

void EventReverser::addReverseCFGNode(
        const PathSet& paths, const VGEdge* edge, ReverseCFG& rvsCFG,
        map<PathSet, RvsCFGVertex>& rvsCFGBasicBlock,
        map<PathSet, PathSet>& parentTable)
{
    //const PathSet& paths = routeGraph_[edge]->paths;
        
    if (edge)
    {
//        // If a state saving edge has cost 0 (which means it does not need a state
//        // saving), we don't add it to reverse CFG.
//        ValueGraphEdge* vgEdge = routeGraph_[*edge];
//        if (isStateSavingEdge(vgEdge) && vgEdge->cost == 0)
//            return;
            
        map<PathSet, RvsCFGVertex>::iterator iter = rvsCFGBasicBlock.find(paths);

        // If the rvs CFG does not have a node for this path now, or if it has, but
        // that basic block node alreay has out edges (which means that basic block
        // is finished), create a new one.
        if (iter != rvsCFGBasicBlock.end() &&
                boost::out_degree(iter->second, rvsCFG) == 0)
        {
            rvsCFG[iter->second].second.push_back(*edge);
            return;
        }
    }

#if 1
        cout << "\nNow add the following edge to reverse CFG:\n";
        cout << paths << " : ";
        if (edge)
            cout << edgeToString(*edge);
        cout << endl;
        
        foreach (RvsCFGVertex node, boost::vertices(rvsCFG))
        {
            cout << node << "=>" << rvsCFG[node].first << endl;
        }
#endif

    // Note that we are building the reverse CFG in topological order, so
    // all predecessors of each new node already exist in the CFG.
    // Also, once a basic block has a successor, this basic block must be
    // finished, since nodes on every path are searched in topological order.

    typedef map<PathSet, RvsCFGVertex>::iterator PathVertexIter;
    typedef pair<PathSet, RvsCFGVertex> PathSetVertexPair;
    vector<PathVertexIter> nodesToRemove;
    vector<PathSetVertexPair> newNodes;
    vector<pair<PathSet, RvsCFGVertex> > newEdges;

//NEXT:
    // For each visible CFG node, connect an edge to from previous nodes
    // to the new node if possible.
    for (PathVertexIter it = rvsCFGBasicBlock.begin(),
            itEnd = rvsCFGBasicBlock.end();
            it != itEnd; ++it)
    {
        //if (it->second == newNode) continue;
        PathSet pathOnEdge = it->first & paths;
        if (!pathOnEdge.any())
            continue;
        
        // Full paths are all paths on that CFG node.
        PathSet fullPaths = rvsCFG[it->second].first;
        
        // Visible paths are paths after being updated.
        PathSet visiblePaths = it->first;

        // If the new node is a child of an existing node.
        // Branch node.
        if (paths.is_subset_of(visiblePaths))
        {
            cout << "Parent Paths: " << paths << " : " << fullPaths << endl;;
            parentTable[paths] = fullPaths;
        }
        
#if 0
        else (it->first.is_subset_of(paths))
        {
            PathSet parentPaths = parentTable[it->first];
            if (parentPaths != paths)
            {
                if (parentPaths.is_subset_of(paths))
                {
                    // OK
                }
                else if (!paths.is_subset_of(parentPaths))
                {
                    ROSE_ASSERT(!"Unstructured code!");
                    return;
                }
                
                
            }
        }
#endif
        // Join node.
        else if (visiblePaths.is_subset_of(paths) && parentTable[visiblePaths] == paths)
        {
            cout << "Parent Paths: " << paths << " : " << 
                    parentTable[parentTable[fullPaths]] << endl;
            
            ROSE_ASSERT(parentTable.count(visiblePaths));
            ROSE_ASSERT(parentTable.count(parentTable[visiblePaths]));
            parentTable[paths] = parentTable[parentTable[visiblePaths]];
        }

        //if (pathOnEdge != it->first && pathOnEdge != paths)
        else
        {
            // If the code is structured, we will add a join node here.
            cout << "***" << visiblePaths << endl;
            ROSE_ASSERT(parentTable.count(visiblePaths) && parentTable[visiblePaths].size());
            
            PathSet newPaths = parentTable[visiblePaths];
            cout << newPaths << endl;
            //getchar();
            //if (paths.is_subset_of(newPaths))
            {
                addReverseCFGNode(newPaths, NULL, rvsCFG, rvsCFGBasicBlock, parentTable);
                //goto NEXT;
                addReverseCFGNode(paths, edge, rvsCFG, rvsCFGBasicBlock, parentTable);
                return;
            }
        }

        // If this path set contains the same path with the current one,
        // connect an edge.
        newEdges.push_back(make_pair(pathOnEdge, it->second));

        // Insert a new node but not modify the current one.
        PathSet oldPaths = it->first;
        oldPaths -= pathOnEdge;
        
        // Update parent paths.
        parentTable[oldPaths] = fullPaths;
        
        ROSE_ASSERT(rvsCFGBasicBlock.count(oldPaths) == 0);

        // Once the paths info is modified, we modified it by removing
        // the old one and adding a new one entry.
        nodesToRemove.push_back(it);
        if (oldPaths.any())
            newNodes.push_back(make_pair(oldPaths, it->second));
    }

    // Add the new node.
    RvsCFGVertex newNode = boost::add_vertex(rvsCFG);
    rvsCFG[newNode].first = paths;
    if (edge)
        rvsCFG[newNode].second.push_back(*edge);

    // Add new edges.
    foreach (const PathSetVertexPair& pathsVertex, newEdges)
    {
        RvsCFGEdge newEdge = boost::add_edge(pathsVertex.second, newNode, rvsCFG).first;
        rvsCFG[newEdge] = pathsVertex.first;
    }

    // Remove all path-empty nodes (since we have finished their out edges).
    foreach (PathVertexIter it, nodesToRemove)
        rvsCFGBasicBlock.erase(it);

    // Add new visible nodes replacing those removed nodes above.
    foreach (const PathSetVertexPair& pathsNode, newNodes)
        rvsCFGBasicBlock.insert(pathsNode);

    // Update rvsCFGBasicBlock.
    rvsCFGBasicBlock[paths] = newNode;
}

void EventReverser::generateCodeForBasicBlock(
        const vector<VGEdge>& edges,
        SgScopeStatement* scope)
{
    SageBuilder::pushScopeStack(scope);

#if 0
    // First, declare all temporary variables at the beginning of the reverse events.
    foreach (VGVertex node, boost::vertices(route))
    {
        ValueNode* valNode = isValueNode(route[node]);
        if (valNode == NULL) continue;
        if (valNode->isAvailable()) continue;

        SgStatement* varDecl = buildVarDeclaration(valNode);
        SageInterface::appendStatement(varDecl, scope);
    }
#endif

    // Generate the reverse code in reverse topological order of the route DAG.
    foreach (const VGEdge& edge, edges)
    {
        //VGVertex node;
        VGVertex src = boost::source(edge, routeGraph_);
        VGVertex tgt = boost::target(edge, routeGraph_);

        ValueNode* valNode = isValueNode(routeGraph_[src]);
        if (valNode == NULL)        continue;
        if (valNode->isAvailable()) continue;

        SgStatement* rvsStmt = NULL;

        if (StateSavingEdge* ssEdge = isStateSavingEdge(routeGraph_[edge]))
        {
            // State saving edge.
            if (ssEdge->cost == 0)
                ;//rvsStmt = buildAssignOpertaion(valNode);
            else
            {
                // State saving here.
                // For forward event, we instrument a push function after the def.
                instrumentPushFunction(valNode, ssEdge->killer);
                //instrumentPushFunction(valNode, funcDef_);
                rvsStmt = buildRestorationStmt(valNode);
            }
        }
        else if (ValueNode* rhsValNode = isValueNode(routeGraph_[tgt]))
        {
            // Simple assignment.
            rvsStmt = buildAssignOpertaion(valNode, rhsValNode);
        }
        else if (OperatorNode* opNode = isOperatorNode(routeGraph_[tgt]))
        {
            // Rebuild the operation.
            ValueNode* lhsNode = NULL;
            ValueNode* rhsNode = NULL;
            boost::tie(lhsNode, rhsNode) = getOperands(tgt);

            rvsStmt = buildOperation(valNode, opNode->type, lhsNode, rhsNode);
        }

        // Add the generated statement to the scope.
        if (rvsStmt)
            SageInterface::appendStatement(rvsStmt, scope);
    }

#if 0
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
#endif

    SageBuilder::popScopeStack();
}

void EventReverser::generateCode(
        size_t dagIndex,
        const ReverseCFG& rvsCFG,
        SgBasicBlock* rvsFuncBody,
        const string& pathNumName)
{
    using namespace SageBuilder;
    
    map<PathSet, SgScopeStatement*> scopeTable;

    foreach (RvsCFGVertex node, boost::vertices(boost::make_reverse_graph(rvsCFG)))
    {
        SgScopeStatement* scope = NULL;

        PathSet paths = rvsCFG[node].first;
        cout << "&&& " << paths << endl;
        // If this node contains all paths, its scope is the reverse function body.
        if (paths.flip().none())
            scope = rvsFuncBody;
        else
        {
            ROSE_ASSERT(scopeTable.count(rvsCFG[node].first));
            scope = scopeTable[rvsCFG[node].first];
        }

        // Generate all code in the scope of this node.
        generateCodeForBasicBlock(rvsCFG[node].second, scope);

        vector<RvsCFGEdge> outEdges;
        foreach (const RvsCFGEdge& outEdge, boost::out_edges(node, rvsCFG))
            outEdges.push_back(outEdge);
        
        // Sort the out edges according to the number of paths on it.
//        sort(outEdges.begin(), outEdges.end(), 
//                rvsCFG[::_1].count() > rvsCFG[::_2].count());

//        if (outEdges.size() == 1)
//            continue;

        // If there are 2 out edges, build an if statement.
        //if (outEdges.size() == 2)
        while (!outEdges.empty())
        {
            PathSet condPaths = rvsCFG[outEdges.back()];
            outEdges.pop_back();
            
            // The last edge can get the current scope.
            if (outEdges.empty())
            {
                cout << "Path added: " << condPaths << endl;
                scopeTable[condPaths] = scope;
                break;
            }
            
//            PathSet paths1 = rvsCFG[outEdges[0]];
//            PathSet paths2 = rvsCFG[outEdges[1]];
//            PathSet condPaths; // = paths1;
//
//            // Force the unique result from the comparison between two path sets.
//            if ((paths1.count() < paths2.count()) ||
//                    paths1.count() == paths2.count() && paths1 < paths2)
//                condPaths = paths1;
//            else
//                condPaths = paths2;

            // Build every thing in the scope of this node.
            pushScopeStack(scope);

            vector<SgExpression*> conditions;

            // Find the path set with the minimum number of paths, then build a
            // logical or expression.
            // Note this part will be optimized later.
            for (size_t i = 0, s = condPaths.size(); i < s; ++i)
            {
                if (condPaths[i])
                {
                    size_t val = pathNumManager_->getPathNumber(dagIndex, i);
                    SgVarRefExp* numVar = buildVarRefExp(pathNumName);
                    SgIntVal* intVal = buildIntVal(val);
                    SgExpression* cond = buildEqualityOp(numVar, intVal);
                    conditions.push_back(cond);
                }
            }

            SgExpression* condition = NULL;
            foreach (SgExpression* cond, conditions)
            {
                if (condition == NULL)
                    condition = cond;
                else
                    condition = buildOrOp(condition, cond);
            }

            SgBasicBlock* trueBody = buildBasicBlock();
            SgBasicBlock* falseBody = buildBasicBlock();
            SgIfStmt* ifStmt = 
                    buildIfStmt(buildExprStatement(condition), trueBody, falseBody);

            // Add this if statement to the scope.
            SageInterface::appendStatement(ifStmt, scope);

            // Assign the scope to successors.
            cout << "Path added: " << condPaths << endl;
            scopeTable[condPaths] = trueBody;
            scope = falseBody;
//            
//            if (condPaths == paths1)
//            {
//                scopeTable[paths1] = trueBody;
//                scopeTable[paths2] = falseBody;
//            }
//            else
//            {
//                scopeTable[paths2] = trueBody;
//                scopeTable[paths1] = falseBody;
//            }

            popScopeStack();
            //continue;
        }
//        if (outEdges.size() > 2)
//        {
//            ROSE_ASSERT(!"Have not handled this case!");
//        }
    }

}


#if 0
void EventReverser::generateReverseFunction(
        SgScopeStatement* scope,
        const SubValueGraph& route)
{
    SageBuilder::pushScopeStack(scope);

#if 0
    // First, declare all temporary variables at the beginning of the reverse events.
    foreach (VGVertex node, boost::vertices(route))
    {
        ValueNode* valNode = isValueNode(route[node]);
        if (valNode == NULL) continue;
        if (valNode->isAvailable()) continue;

        SgStatement* varDecl = buildVarDeclaration(valNode);
        SageInterface::appendStatement(varDecl, scope);
    }
#endif

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
#endif

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

void EventReverser::removeEmptyIfStmt()
{    
    vector<SgIfStmt*> ifStmts = BackstrokeUtility::querySubTree<SgIfStmt>(rvsFuncDef_);
    foreach (SgIfStmt* ifStmt, ifStmts)
    {
        SgBasicBlock* trueBody = isSgBasicBlock(ifStmt->get_true_body());
        SgBasicBlock* falseBody = isSgBasicBlock(ifStmt->get_false_body());
        
        if (trueBody && falseBody &&
                trueBody->get_statements().empty() && 
                falseBody->get_statements().empty())
            SageInterface::removeStatement(ifStmt);
    }
}


} // End of namespace Backstroke
