#include "valueGraph.h"
#include "functionReverser.h"
#include <normalizations/expNormalization.h>
#include <utilities/utilities.h>

#include <boost/graph/graphviz.hpp>
#include <boost/graph/reverse_graph.hpp>
#include <boost/graph/topological_sort.hpp>
#include <boost/graph/filtered_graph.hpp>

namespace Backstroke
{

using namespace std;

#define foreach BOOST_FOREACH
#define reverse_foreach BOOST_REVERSE_FOREACH

EventReverser::EventReverser(SSA* ssa)
:   cfg_(NULL), cdg_(NULL), 
    ssa_(ssa), 
    pathNumManager_(NULL)
{
    // Note that all event functions must be normalized before creating an event reverser!!!
    //ssa_->run(true);
}

EventReverser::~EventReverser()
{
    delete cfg_;
    delete cdg_;
    delete pathNumManager_;
}


void EventReverser::reverseEvent(SgFunctionDefinition* funcDef)
{
    funcDef_ = funcDef;
    
    //// Normalize the function.
    //BackstrokeNorm::normalizeEvent(funcDef_->get_declaration());

    // Three new functions are built. Backup the original function here.
    buildFunctionBodies();
    
    cfg_ = new BackstrokeCFG(funcDef_);
    cdg_ = new BackstrokeCDG(*cfg_);
    pathNumManager_ = new PathNumManager(cfg_);

    // Get all backedges from this CFG.
    //backEdges_ = cfg_->getAllBackEdges();
    
    // Get the number of DAGs for this function.
    int dagNum = pathNumManager_->getDagNum();
    valuesToRestore_.resize(dagNum);
    availableValues_.resize(dagNum);
    
    generateCode();
}

void EventReverser::generateCode()
{
    // First, build the value graph.
    buildValueGraph();
        
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
    
    // Discard all available values here.
    availableValues_[0].clear();
    
    // Get all values which are needed or available for all DAGs here.
    foreach (VGVertex node, boost::vertices(valueGraph_))
    {
        if (MuNode* muNode = isMuNode(valueGraph_[node]))
        {
            if (muNode->isCopy)
                ;//availableValues_[muNode->dagIndex].insert(node);
            else
                valuesToRestore_[muNode->dagIndex].insert(node);
        }
        else if (ValueNode* valNode = isValueNode(valueGraph_[node]))
        {
            // Add all constants to available values.
            if (valNode->isAvailable())
                availableValues_[0].insert(node);
        }
    }
    // Put root as the available values.
    availableValues_[0].insert(root_);
    for (size_t i = 1, s = availableValues_.size(); i != s; ++i)
    {
        availableValues_[i].insert(
            availableValues_[0].begin(), availableValues_[0].end());
    }
    
    
    map<VGEdge, PathInfo> routes;

    int dagNum = pathNumManager_->getNumberOfDags();
    for (int dagIndex = 0; dagIndex < dagNum; ++dagIndex)
    {
        // Just for DAG 0.
        size_t pathNum = pathNumManager_->getNumberOfPath(dagIndex);
        for (size_t i = 0; i < pathNum; ++i)
        {
            // Search the subgraph then get the shorted paths.
            set<VGEdge> route = getRouteFromSubGraph(dagIndex, i);

            foreach (const VGEdge& edge, route)
            {
                PathSet& path = routes[edge][dagIndex];
                if (path.empty())
                    path.resize(pathNum);
                path.set(i);
            }
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
    vector<ReverseCFG> rvsCFGs(dagNum);
    for (int i = 0; i < dagNum; ++i)
    {
        buildReverseCFG(i, rvsCFGs[i]);
        
        char name[32];
        sprintf(name, "rvsCFG%d.dot", i);
        ofstream ofile(name, std::ios::out);
        boost::write_graphviz(ofile, rvsCFGs[i]);
    }
    
    //buildReverseCFG(0, rvsCFG);
    

    string pathNumName = "__num__";
    
    // If the number of path is 1, we don't have to use path numbers.
    //if (pathNum > 1)
    buildPathNumDeclaration(pathNumName);
    
    // Build all three functions.
    generateCode(0, rvsCFGs, rvsFuncDef_->get_body(), cmtFuncDef_->get_body(), pathNumName);


    // If the number of path is 1, we don't have to use path numbers.
    //if (pathNum > 1)
    pathNumManager_->insertPathNumberToEvents(pathNumName);

    // Finally insert all functions in the code.
    insertFunctions();
    
    // It is likely that there are lots of empty if statements in reverse event.
    // Remove them here.
    removeEmptyIfStmt(rvsFuncDef_);
    removeEmptyIfStmt(cmtFuncDef_);
}

void EventReverser::buildPathNumDeclaration(const string& pathNumName)
{
    using namespace SageBuilder;

    // Insert the declaration of the path number in the front of reverse function,
    // and define its value from a pop function call.
    pushScopeStack(rvsFuncDef_->get_body());
    SgVariableDeclaration* pathNumDecl =
            SageBuilder::buildVariableDeclaration(
                pathNumName,
                SageBuilder::buildIntType()/*,
                SageBuilder::buildAssignInitializer(
                    buildPopFunctionCall(
                        buildIntType()))*/);
    SgStatement* restoreNum = SageBuilder::buildExprStatement(
                buildRestoreFunctionCall(buildVarRefExp(pathNumDecl)));
    popScopeStack();

    SageInterface::prependStatement(restoreNum, rvsFuncDef_->get_body());
    SageInterface::prependStatement(pathNumDecl, rvsFuncDef_->get_body());
    SageInterface::prependStatement(SageInterface::copyStatement(restoreNum), cmtFuncDef_->get_body());
    SageInterface::prependStatement(SageInterface::copyStatement(pathNumDecl), cmtFuncDef_->get_body());
}

void EventReverser::buildRouteGraph(const map<VGEdge, PathInfo>& routes)
{
    map<VGVertex, VGVertex> nodeTable;
    
    typedef map<VGEdge, PathInfo>::value_type EdgePathPair;
    foreach (const EdgePathPair& edgePath, routes)
    {
        const VGEdge& edge = edgePath.first;
        const PathInfo& paths = edgePath.second;
        
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
    
    // It is possible that routeGraph_ has no node at this point.
    if (boost::num_vertices(routeGraph_) == 0)
    {
        routeGraphRoot_ = boost::add_vertex(routeGraph_);
        routeGraph_[routeGraphRoot_] = valueGraph_[root_];
    }
    else
    {
        // Set the root of the route graph.
        ROSE_ASSERT(nodeTable.count(root_));
        routeGraphRoot_ = nodeTable[root_];
    }
    
    // Switch all nodes in valuesToRestore_ to those in route graph.
    // It is important since the code generation is performed on route graph, not
    // value graph.
    set<VGVertex> newValuesToRestore;
    foreach (VGVertex node, valuesToRestore_[0])
    {
        if (nodeTable.count(node) > 0)
            newValuesToRestore.insert(nodeTable[node]);
    }
    valuesToRestore_[0].swap(newValuesToRestore);
    
    removePhiNodesFromRouteGraph();
}

void EventReverser::removePhiNodesFromRouteGraph()
{
    set<VGVertex> nodesToRemove;
    
    // Then remove edges on phi nodes to ease the source code generation.
    foreach (VGVertex node, boost::vertices(routeGraph_))
    {
        PhiNode* phiNode = isPhiNode(routeGraph_[node]);
        if (phiNode == NULL || isMuNode(phiNode))
            continue;
        
        // If there exists a pair of in and out edges for this phi node and 
        // the paths of one of them is a subset of the other one, we can forward
        // an edge by passing this phi node.
        foreach (const VGEdge& outEdge, boost::out_edges(node, routeGraph_))
        //if (boost::out_degree(node, routeGraph_) == 1)
        {
            //VGEdge outEdge = *(boost::out_edges(node, routeGraph_).first);
            VGVertex tgt = boost::target(outEdge, routeGraph_);
            const PathInfo& pathsOnOutEdge = routeGraph_[outEdge]->paths;

            foreach (const VGEdge& inEdge, boost::in_edges(node, routeGraph_))
            {
                const PathInfo& pathsOnInEdge =  routeGraph_[inEdge]->paths;

                //ROSE_ASSERT(pathsOnInEdge.is_subset_of(pathsOnOutEdge) ||
                //        pathsOnOutEdge.is_subset_of(pathsOnInEdge));

                // Add a new edge first, then attach the information in which
                // the cost is the one on the out edge and the path set is the one
                // on the in edge.
                PathInfo newPaths = pathsOnInEdge & pathsOnOutEdge;
                if (newPaths.empty())
                    continue;

                VGVertex src = boost::source(inEdge, routeGraph_);

                // Multiple state saving edges with cost 0 can be merged.
                // Then we don't have to add a new edge.
                StateSavingEdge* ssEdge = isStateSavingEdge(routeGraph_[outEdge]);
                if (ssEdge && ssEdge->cost == 0);
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
                            // This part is incorrect now.
                            //ROSE_ASSERT(false);
                            ssEdge2->paths = ssEdge2->paths | newPaths;
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
    
    if (/*SgMemberFunctionDeclaration* memFuncDecl = */isSgMemberFunctionDeclaration(funcDecl))
    {
        //SgMemberFunctionType* memFuncType = isSgMemberFunctionType(memFuncDecl->get_type());
        //ROSE_ASSERT(memFuncType);

        //Create the function declaration for the forward body
        SgName fwdFuncName = funcName + "_forward";
        SgFunctionDeclaration* fwdFuncDecl = buildDefiningMemberFunctionDeclaration(
                        fwdFuncName,
                        funcDecl->get_orig_return_type(),
                        isSgFunctionParameterList(
                            copyStatement(funcDecl->get_parameterList())),
                        funcScope);
        fwdFuncDef_ = fwdFuncDecl->get_definition();
        //SageInterface::replaceStatement(fwdFuncDef->get_body(), isSgBasicBlock(stmt.fwd_stmt));

        //Create the function declaration for the reverse body
        SgName rvsFuncName = funcName + "_reverse";
        SgFunctionDeclaration* rvsFuncDecl = buildDefiningMemberFunctionDeclaration(
                        rvsFuncName,
                        funcDecl->get_orig_return_type(),
                        //buildFunctionParameterList(),
                        isSgFunctionParameterList(
                            copyStatement(funcDecl->get_parameterList())),
                        funcScope);
        rvsFuncDef_ = rvsFuncDecl->get_definition();
        //SageInterface::replaceStatement(rvsFuncDef->get_body(), isSgBasicBlock(stmt.rvs_stmt));

        //Create the function declaration for the commit method
        SgName cmtFuncName = funcName + "_commit";
        SgFunctionDeclaration* cmtFuncDecl = buildDefiningMemberFunctionDeclaration(
                        cmtFuncName,
                        funcDecl->get_orig_return_type(),
                        //buildFunctionParameterList(),
                        isSgFunctionParameterList(
                            copyStatement(funcDecl->get_parameterList())),
                        funcScope);
        cmtFuncDef_ = cmtFuncDecl->get_definition();
    }
    else
    {
        ROSE_ASSERT(false);
    }

    // Copy the original function to forward function.
    replaceStatement(fwdFuncDef_->get_body(),
                     copyStatement(funcDef_->get_body()));
    
    // Print the name of the handler at the beginning of each event.
#if 0
    SgMemberFunctionDeclaration* memFuncDecl = isSgMemberFunctionDeclaration(funcDef_->get_declaration());
    string className;
    if (memFuncDecl)
        className += memFuncDecl->get_associatedClassDeclaration()->get_name().str();
    
    string funcNames[4];
    funcNames[0] = "_forward";
    funcNames[2] = "_reverse";
    funcNames[3] = "_commit";
    
    SgScopeStatement* scopes[4];
    scopes[0] = funcDef_->get_body();
    scopes[1] = fwdFuncDef_->get_body();
    scopes[2] = rvsFuncDef_->get_body();
    scopes[3] = cmtFuncDef_->get_body();
    
    for (int i = 0; i < 4; ++i)
    {
        string name = className + string("::") + funcName + funcNames[i] + string("\\n");
        SgExprListExp* exprList = buildExprListExp(buildStringVal(name));
    
        SgFunctionCallExp* printFuncName = buildFunctionCallExp(
            "printf", buildVoidType(), exprList, scopes[i]);
    
        prependStatement(buildExprStatement(printFuncName), scopes[i]);
    }
#endif
    
    ROSE_ASSERT(funcDef_->get_body()->get_parent() == funcDef_);
    ROSE_ASSERT(fwdFuncDef_->get_body()->get_parent() == fwdFuncDef_);
    ROSE_ASSERT(rvsFuncDef_->get_body()->get_parent() == rvsFuncDef_);
    ROSE_ASSERT(cmtFuncDef_->get_body()->get_parent() == cmtFuncDef_);

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
//    // This table is used to sort all path sets according to the topological order
//    // in the CFG.
//    map<PathSet, int> pathsIndexTable = pathNumManager_->getPathsIndices(dagIndex);
        
    // This table is used to sort all AST nodes according to the topological order
    // in the CFG.
    map<SgNode*, int> nodeIndexTable;
    pathNumManager_->getAstNodeIndices(dagIndex, nodeIndexTable);
    
//    foreach (const VGEdge& edge, boost::edges(routeGraph_))
//    {
//        PathSet paths = routeGraph_[edge]->paths;
//        if (pathsIndexTable.count(paths) == 0)
//            pathsIndexTable[paths] = INT_MAX;
//    }
    
#if 0
    for (map<PathSet, int>::iterator it = pathsIndexTable.begin(), itEnd = pathsIndexTable.end();
            it != itEnd; ++it)
        cout << it->first << " : " << it->second << endl;
#endif
    
    // A priority queue, in which elements are sorted by its index from edgeIndexTable.
    priority_queue<VGEdge, vector<VGEdge>, RouteGraphEdgeComp>  
    nextEdgeCandidates(RouteGraphEdgeComp(routeGraph_, nodeIndexTable));
    
    map<VGVertex, PathInfo> nodePathsTable;
    foreach (VGVertex node, boost::vertices(routeGraph_))
        nodePathsTable[node][dagIndex].resize(pathNumManager_->getNumberOfPath(dagIndex));
    
    set<VGEdge> traversedEdges;
    
#if 0
    foreach (VGVertex node, boost::vertices(routeGraph_))
    {
        MuNode* muNode = isMuNode(routeGraph_[node]);
        // The search start point is either root or Mu node (for loops).
        if ((muNode && !muNode->isCopy && muNode->dagIndex == dagIndex) 
                || node == routeGraphRoot_)
        {
            foreach (const VGEdge& inEdge, boost::in_edges(node, routeGraph_))
            {
                //cout << "Paths on edge: " << routeGraph_[inEdge]->paths << endl;
                nextEdgeCandidates.push(inEdge);
            }
        }
    }
#endif
    foreach (const VGEdge& inEdge, boost::in_edges(routeGraphRoot_, routeGraph_))
    {
        //cout << "Paths on edge: " << routeGraph_[inEdge]->paths << endl;
        nextEdgeCandidates.push(inEdge);
    }
    
    while (!nextEdgeCandidates.empty())
    {
        VGEdge edge = nextEdgeCandidates.top();
        nextEdgeCandidates.pop();
        
        // If this edge does not belong to the route of the given DAG.
        if (routeGraph_[edge]->paths.count(dagIndex) == 0)
            continue;
            
        result.push_back(edge);
        
        VGVertex src = boost::source(edge, routeGraph_);
        // The following paths are the paths on this edge.
        PathSet& pathsOnSrc = nodePathsTable[src][dagIndex];
        pathsOnSrc |= routeGraph_[edge]->paths[dagIndex];
        
        // After updating the traversed path set of the source node of the edge
        // just added to candidates, check all in edges of this source node to see
        // if any of it can be added to candidates.
        foreach (const VGEdge& inEdge, boost::in_edges(src, routeGraph_))
        {
            //// If this edge does not belong to the route of the given DAG.
            if (routeGraph_[inEdge]->paths.count(dagIndex) == 0)
                continue;
            
            if (routeGraph_[inEdge]->paths[dagIndex].is_subset_of(pathsOnSrc))
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
    // This set tracks which DAG has been added to the reverse CFG.
    set<int> dagAdded;
    
    
    // Add a initial node to reverse CFG indicating the entry.
    //RvsCFGVertex entry = boost::add_vertex(rvsCFG);
    PathSet allPaths = pathNumManager_->getAllPaths(dagIndex);
    parentTable[allPaths] = allPaths;
    
    addReverseCFGNode(allPaths, NULL, rvsCFG, 
            rvsCFGBasicBlock, parentTable, dagAdded);
    //rvsCFG[entry].first = allPaths;
    //rvsCFGBasicBlock[allPaths] = entry;

    // Proper order means topological order on both VG and CFG.
    vector<VGEdge> result;
    getRouteGraphEdgesInProperOrder(dagIndex, result);
    
#if 1
    foreach (const VGEdge& edge, result)
    {
        VGVertex src = boost::source(edge, valueGraph_);
        VGVertex tgt = boost::target(edge, valueGraph_);
        cout << valueGraph_[src]->toString() << " ==> " 
                << valueGraph_[tgt]->toString() << endl;
        //cout << "!!!" << routeGraph_[edge]->toString() << endl;
    }
    cout << "\n\n";
#endif
    
    
    foreach (const VGEdge& edge, result)
    {
        const PathSet& paths = routeGraph_[edge]->paths[dagIndex];
        addReverseCFGNode(paths, &edge, rvsCFG, 
                rvsCFGBasicBlock, parentTable, dagAdded);
    }
    
    // Add the exit node.
    addReverseCFGNode(allPaths, NULL, rvsCFG, 
            rvsCFGBasicBlock, parentTable, dagAdded);
    
    //rvsCFG = boost::make_reverse_graph(rvsCFG);
}

void EventReverser::addReverseCFGNode(
        const PathSet& paths, const VGEdge* edge, ReverseCFG& rvsCFG,
        map<PathSet, RvsCFGVertex>& rvsCFGBasicBlock,
        map<PathSet, PathSet>& parentTable,
        set<int>& dagAdded)
{
    //const PathSet& paths = routeGraph_[edge]->paths;
    int dagIndex = 0;
        
    if (edge)
    {
        // If the target of this edge is a Mu node, set a flag by DAG index then
        // we will insert the loop at the beginning of the new built basic block.
        
        VGVertex tar = boost::target(*edge, valueGraph_);
        if (MuNode* muNode = isMuNode(valueGraph_[tar]))
            dagIndex = muNode->dagIndex;
//        // If a state saving edge has cost 0 (which means it does not need a state
//        // saving), we don't add it to reverse CFG.
//        ValueGraphEdge* vgEdge = routeGraph_[*edge];
//        if (isStateSavingEdge(vgEdge) && vgEdge->cost == 0)
//            return;
            
        map<PathSet, RvsCFGVertex>::iterator iter = rvsCFGBasicBlock.find(paths);

        // If the rvs CFG does not have a node for this path now, or if it has, but
        // that basic block node already has out edges (which means that basic block
        // is finished), create a new one.
        if (iter != rvsCFGBasicBlock.end() &&
                boost::out_degree(iter->second, rvsCFG) == 0)
        {
            rvsCFG[iter->second].edges.push_back(*edge);
      
            // If the given DAG index is greater than 0, this is a MU node.
            // In this case, build a new vertex, and the loop will be inserted
            // in the front of this basic block later. Note that the dagAdded set
            // is used to prevent that a loop is added more than once due to several
            // MU nodes.
            if (dagIndex > 0 && dagAdded.count(dagIndex) == 0)
            {
                // Add the new node.
                RvsCFGVertex newNode = boost::add_vertex(rvsCFG);
                rvsCFG[newNode].paths = paths;
                rvsCFG[newNode].dagIndex = dagIndex;
                // Update rvsCFGBasicBlock.
                
                // Add an edge.
                RvsCFGEdge newEdge = boost::add_edge(iter->second, newNode, rvsCFG).first;
                rvsCFG[newEdge] = iter->first;
                
                rvsCFGBasicBlock[paths] = newNode;
                dagAdded.insert(dagIndex);
            }
            return;
        }
    }

#if 0
        cout << "\nNow add the following edge to reverse CFG:\n";
        cout << paths << " : ";
        if (edge)
            cout << edgeToString(*edge);
        cout << endl;
        
        foreach (RvsCFGVertex node, boost::vertices(rvsCFG))
        {
            cout << node << "=>" << rvsCFG[node].paths << endl;
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
    // For each visible CFG node, connect an edge from previous nodes
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
        PathSet fullPaths = rvsCFG[it->second].paths;
        
        // Visible paths are paths after being updated.
        PathSet visiblePaths = it->first;

        // If the new node is a child of an existing node.
        // Branch node.
        if (paths.is_subset_of(visiblePaths))
        {
            //cout << "Parent Paths: " << paths << " : " << fullPaths << endl;;
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
        else if (visiblePaths.is_subset_of(paths) 
                && parentTable[visiblePaths] == paths)
        {
            //cout << "Parent Paths: " << paths << " : " << 
            //        parentTable[parentTable[fullPaths]] << endl;
            
            ROSE_ASSERT(parentTable.count(visiblePaths));
            ROSE_ASSERT(parentTable.count(parentTable[visiblePaths]));
            parentTable[paths] = parentTable[parentTable[visiblePaths]];
        }

        //if (pathOnEdge != it->first && pathOnEdge != paths)
        else
        {
            // If the code is structured, we will add a join node here.
            //cout << "***" << visiblePaths << endl;
            ROSE_ASSERT(parentTable.count(visiblePaths) 
                    && parentTable[visiblePaths].size());
            
            PathSet newPaths = parentTable[visiblePaths];
            //cout << newPaths << endl;
            //getchar();
            //if (paths.is_subset_of(newPaths))
            {
                addReverseCFGNode(newPaths, NULL, rvsCFG, 
                        rvsCFGBasicBlock, parentTable, dagAdded);
                //goto NEXT;
                addReverseCFGNode(paths, edge, rvsCFG, 
                        rvsCFGBasicBlock, parentTable, dagAdded);
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
    rvsCFG[newNode].paths = paths;
    rvsCFG[newNode].dagIndex = dagIndex;
    if (edge)
        rvsCFG[newNode].edges.push_back(*edge);

    // Add new edges.
    foreach (const PathSetVertexPair& pathsVertex, newEdges)
    {
        RvsCFGEdge newEdge = 
                boost::add_edge(pathsVertex.second, newNode, rvsCFG).first;
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

namespace 
{
    SgNode* removeThisPointer(SgNode* node)
    {
        if (SgArrowExp* arrowExp = isSgArrowExp(node))
        {
            if (isSgThisExp(arrowExp->get_lhs_operand()))
                return arrowExp->get_rhs_operand();
        }
        return node;
    }
}

void EventReverser::generateCodeForBasicBlock(
        const vector<VGEdge>& edges,
        SgScopeStatement* rvsScope,
        SgScopeStatement* cmtScope)
{
    using namespace SageBuilder;
    using namespace SageInterface;
    
    pushScopeStack(rvsScope);

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
    
    // This table is used to locate where to push a value. A function call may 
    // modify several values, and the order of those pushes and pops should be correct.
    // Without this table, the order may be wrong.
    map<SgNode*, SgStatement*> pushLocations;

    // Generate the reverse code in reverse topological order of the route DAG.
    foreach (const VGEdge& edge, edges)
    {
        //VGVertex node;
        VGVertex src = boost::source(edge, routeGraph_);
        VGVertex tgt = boost::target(edge, routeGraph_);

        ValueNode* valNode = isValueNode(routeGraph_[src]);
        if (valNode == NULL)        continue;
        if (valNode->isAvailable()) continue;

        SgStatement* pushFuncStmt = NULL;
        SgStatement* rvsStmt = NULL;
        SgStatement* cmtStmt = NULL;

        if (StateSavingEdge* ssEdge = isStateSavingEdge(routeGraph_[edge]))
        {
#if 1
            // State saving edge.
            if (ssEdge->cost == 0)
                ;//rvsStmt = buildAssignOpertaion(valNode);
            else
            {
                // Find the correct location to put the push function call.
                SgNode* killer = ssEdge->killer;
                SgStatement* pushLocation = NULL;
                if (pushLocations.count(killer) == 0)
                {
                    pushLocation = SageInterface::getEnclosingStatement(killer);
                    pushLocations[killer] = pushLocation;
                }
                else
                    pushLocation = pushLocations[killer];
                
                
                pushFuncStmt = buildExprStatement(
                        buildStoreFunctionCall(valNode->var.getVarRefExp()));

                //instrumentPushFunction(valNode, funcDef_);
                rvsStmt = buildExprStatement(
                        buildRestoreFunctionCall(valNode->var.getVarRefExp()));

                // Build the commit statement.
                cmtStmt = buildPopStatement(valNode->getType());
                
#if 0
                // If the variable is a pointer, do a deep copy of it.
                if (isSgPointerType(valNode->getType()))
                {
                    pushFuncStmt = buildPushStatementForPointerType(valNode);
                    
                    //instrumentPushFunction(valNode, funcDef_);
                    rvsStmt = buildExprStatement(buildCommaOpExp(
                            buildDestroyFunctionCall(valNode->var.getVarRefExp()),
                            //buildDeleteExp(buildVariable(valNode), 0, 0, 0),
                            buildRestorationExp(valNode)));
                    
                    //rvsStmt = buildRestorationStmt(valNode);

                    // Build the commit statement.
                    cmtStmt = buildExprStatement(
                            buildDestroyFunctionCall(buildPopFunctionCall(valNode->getType())));
                            //buildDeleteExp(buildPopFunctionCall(valNode->getType()), 0, 0, 0));
                    //cmtStmt = buildExprStatement(buildPopFunctionCall(valNode->getType()));
                }
                else
                {
                    pushFuncStmt = buildPushStatement(valNode);
                    
                    
                    //instrumentPushFunction(valNode, funcDef_);
                    rvsStmt = buildRestorationStmt(valNode);

                    // Build the commit statement.
                    cmtStmt = buildPopStatement(valNode->getType());
                }
#endif
                
                // State saving here.
                // For forward event, we instrument a push function before the def.
                
                //instrumentPushFunction(valNode, ssEdge->killer);
                
                //SgStatement* pushFuncStmt = buildPushStatement(valNode);
                
                SageInterface::insertStatementBefore(pushLocation, pushFuncStmt); 
                // Update the location so that the next push is put before this push.
                pushLocations[killer] = pushFuncStmt;
                
                ////instrumentPushFunction(valNode, funcDef_);
                //rvsStmt = buildRestorationStmt(valNode);
                
                //// Build the commit statement.
                //cmtStmt = buildPopStatement(valNode->getType());
            }
#endif
        }
        else if (ValueNode* rhsValNode = isValueNode(routeGraph_[tgt]))
        {
            // Don't generate the expression like a = a.
            if (valNode->var.name == rhsValNode->var.name)
                continue;
            // Simple assignment.
            rvsStmt = buildAssignOpertaion(valNode, rhsValNode);
        }
        else if (OperatorNode* opNode = isOperatorNode(routeGraph_[tgt]))
        {
            // Rebuild the operation.
            ValueNode* lhsNode = NULL;
            ValueNode* rhsNode = NULL;
            boost::tie(lhsNode, rhsNode) = getOperands(tgt);

            rvsStmt = buildOperationStatement(valNode, opNode->type, lhsNode, rhsNode);
        }
        else if (FunctionCallNode* funcCallNode = isFunctionCallNode(routeGraph_[tgt]))
        {
#if 1
            // Virtual function call.
            SgFunctionCallExp* funcCallExp = funcCallNode->getFunctionCallExp();
            ROSE_ASSERT(funcCallExp);
            SgMemberFunctionRefExp* funcRef = NULL;
            if (SgBinaryOp* binExp = isSgBinaryOp(funcCallExp->get_function()))
                funcRef = isSgMemberFunctionRefExp(binExp->get_rhs_operand());
            ROSE_ASSERT(funcRef);
            
            SgMemberFunctionDeclaration* funcDecl = funcRef->getAssociatedMemberFunctionDeclaration();
            SgType* returnType = funcCallExp->get_type();
            
            string funcName = funcDecl->get_name().str();
            string fwdFuncName = funcName + "_forward";
            string rvsFuncName = funcName + "_reverse";
            string cmtFuncName = funcName + "_commit";
            
            if (SgClassDefinition* classDef = funcDecl->get_class_scope())
            {
                SgMemberFunctionSymbol* fwdFuncSymbol = NULL;
                SgMemberFunctionSymbol* rvsFuncSymbol = NULL;
                SgMemberFunctionSymbol* cmtFuncSymbol = NULL;

                //cout << "Class Name: " << classDef->get_declaration()->get_name().str() << endl;
                ROSE_ASSERT(classDef);
                
                foreach (SgDeclarationStatement* decl, classDef->get_members())
                {
                    SgMemberFunctionDeclaration* memFuncDecl = 
                            isSgMemberFunctionDeclaration(decl);
                    if (memFuncDecl == NULL)
                        continue;
                    
                    SgName funcName = memFuncDecl->get_name();
                    //cout << memFuncDecl->get_qualified_name().str() << endl;
                    
                    //cout << "FUNC:\t" << funcName.str() << endl;
                    //cout << fwdFuncName << " " << rvsFuncName << " " << cmtFuncName << endl;
                    
                    if (funcName == fwdFuncName)
                    {
                        fwdFuncSymbol = isSgMemberFunctionSymbol(
                                memFuncDecl->get_symbol_from_symbol_table());
                    }
                    else if (funcName == rvsFuncName)
                    {
                        rvsFuncSymbol = isSgMemberFunctionSymbol(
                                memFuncDecl->get_symbol_from_symbol_table());
                    }
                    else if (funcName == cmtFuncName)
                    {
                        cmtFuncSymbol = isSgMemberFunctionSymbol(
                                memFuncDecl->get_symbol_from_symbol_table());
                    }
                }
                
                cout << "Processing Function Call:\t" << funcName << endl;
                ROSE_ASSERT(fwdFuncSymbol && rvsFuncSymbol && cmtFuncSymbol);

                //SgThisExp* thisExp = isSgThisExp(arrowExp->get_lhs_operand());
                //ROSE_ASSERT(thisExp);
                //ROSE_ASSERT(copyExpression(thisExp));

                SgFunctionCallExp* fwdFuncCall = isSgFunctionCallExp(copyExpression(funcCallExp));
                if (SgBinaryOp* binExp = isSgBinaryOp(fwdFuncCall->get_function()))
                    funcRef = isSgMemberFunctionRefExp(binExp->get_rhs_operand());
                funcRef->set_symbol(fwdFuncSymbol);
                // FIXME The following method does not work!!
                //replaceExpression(arrowExp->get_rhs_operand(), fwdFuncRef);
                
                SgFunctionCallExp* rvsFuncCall = isSgFunctionCallExp(copyExpression(funcCallExp));
                replaceExpression(rvsFuncCall->get_args(), buildExprListExp());
                if (SgBinaryOp* binExp = isSgBinaryOp(rvsFuncCall->get_function()))
                    funcRef = isSgMemberFunctionRefExp(binExp->get_rhs_operand());
                funcRef->set_symbol(rvsFuncSymbol);
                //replaceExpression(arrowExp->get_rhs_operand(), rvsFuncRef);
                
                SgFunctionCallExp* cmtFuncCall = isSgFunctionCallExp(copyExpression(funcCallExp));
                replaceExpression(cmtFuncCall->get_args(), buildExprListExp());
                if (SgBinaryOp* binExp = isSgBinaryOp(cmtFuncCall->get_function()))
                    funcRef = isSgMemberFunctionRefExp(binExp->get_rhs_operand());
                funcRef->set_symbol(cmtFuncSymbol);
                //replaceExpression(arrowExp->get_rhs_operand(), cmtFuncRef);
                
#if 0
                SgExpression* fwdFuncCall = buildFunctionCallExp(buildArrowExp(
                        copyExpression(thisExp), fwdFuncRef));
                replaceExpression(funcCallExp, fwdFuncCall);

                SgExpression* rvsFuncCall = buildFunctionCallExp(buildArrowExp(
                        copyExpression(thisExp), rvsFuncRef));
                rvsStmt = buildExprStatement(rvsFuncCall);

                SgExpression* cmtFuncCall = buildFunctionCallExp(buildArrowExp(
                        copyExpression(thisExp), cmtFuncRef));
                cmtStmt = buildExprStatement(cmtFuncCall);
#endif
                
                replaceExpression(funcCallExp, fwdFuncCall);
                rvsStmt = buildExprStatement(rvsFuncCall);
                cmtStmt = buildExprStatement(cmtFuncCall);
            }
            else
            {
                ROSE_ASSERT(0);
                
                SgExpression* fwdFuncCall = buildFunctionCallExp(fwdFuncName, returnType);
                //fwdFuncCall = buildArrowExp(SageInterface::copyExpression(thisExp), fwdFuncCall);
                SageInterface::replaceExpression(funcCallExp, fwdFuncCall);

                SgExpression* rvsFuncCall = buildFunctionCallExp(rvsFuncName, returnType);
                //SgExpression* rvsFuncCall = buildFunctionCallExp(SageInterface::copyExpression(arrowExp));
                rvsStmt = buildExprStatement(rvsFuncCall);

                SgExpression* cmtFuncCall = buildFunctionCallExp(cmtFuncName, returnType);
                cmtStmt = buildExprStatement(cmtFuncCall);
            }
#endif
        }

        // Add the generated statement to the scope.
        if (rvsStmt)
            appendStatement(rvsStmt, rvsScope);
        
        if (cmtStmt)
            appendStatement(cmtStmt, cmtScope);
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

    popScopeStack();
}

namespace
{
    using namespace SageBuilder;
    
    SgForStatement* buildLoopForReverseFunction()
    {
        SgBasicBlock* rvsBody = buildBasicBlock();
        SgStatement* testStmt = buildExprStatement(buildIntVal(1));
        SgExpression* incrExpr = buildIntVal(1);
        SgForStatement* rvsForStmt = 
                buildForStatement(0, testStmt, incrExpr, rvsBody);
        return rvsForStmt;
    }
}

void EventReverser::generateCode(
        int dagIndex,
        const vector<ReverseCFG>& rvsCFGs,
        SgScopeStatement* rvsFuncBody,
        SgScopeStatement* cmtFuncBody,
        const string& pathNumName)
{
    using namespace SageBuilder;
    using namespace SageInterface;
    
    int counter = 0;
    
//    // Build an anonymous namespace.
//    SgGlobal* globalScope = getGlobalScope(fwdFuncDef_);
//    SgNamespaceDeclarationStatement* anonymousNamespaceDecl = buildNamespaceDeclaration("", globalScope);
//    SgNamespaceDefinitionStatement* anonymousNamespace = buildNamespaceDefinition(anonymousNamespaceDecl);
//    prependStatement(anonymousNamespaceDecl, globalScope);
    
    map<PathSet, SgScopeStatement*> rvsScopeTable;
    map<PathSet, SgScopeStatement*> cmtScopeTable;
    
#if 0
    for (int i = 1; i < rvsCFGs.size(); ++i)
    {
        foreach (RvsCFGVertex node, boost::vertices(rvsCFGs[i]))
            generateCodeForBasicBlock(rvsCFGs[i][node].second, rvsFuncBody, cmtFuncBody);
    }
#endif

    //boost::reverse_graph<ReverseCFG> rvsCFG 
    //        = boost::make_reverse_graph(rvsCFGs[0]);
    //foreach (RvsCFGVertex node, boost::vertices(boost::make_reverse_graph(rvsCFGs[dagIndex])))
    const ReverseCFG& rvsCFG = rvsCFGs[dagIndex];
    //reverse_foreach (RvsCFGVertex node, boost::vertices(rvsCFG))
    foreach (RvsCFGVertex node, boost::vertices(rvsCFG))
    {
        SgScopeStatement* rvsScope = NULL;
        SgScopeStatement* cmtScope = NULL;

        PathSet paths = rvsCFG[node].paths;
        //cout << "&&& " << paths << endl;
        // If this node contains all paths, its scope is the reverse function body.
        if (paths.flip().none())
        {
            rvsScope = rvsFuncBody;
            cmtScope = cmtFuncBody;
        }
        else
        {
            ROSE_ASSERT(rvsScopeTable.count(rvsCFG[node].paths));
            ROSE_ASSERT(cmtScopeTable.count(rvsCFG[node].paths));
            rvsScope = rvsScopeTable[rvsCFG[node].paths];
            cmtScope = cmtScopeTable[rvsCFG[node].paths];
        }
        
        // If a loop is needed at the beginning of this basic block.
        int dagIndexInBasicBlock = rvsCFG[node].dagIndex;
        if (dagIndexInBasicBlock != dagIndex && dagIndexInBasicBlock > 0)
        {
            // Build two loop bodies for reverse and commit events.
            SgForStatement* rvsForStmt = buildLoopForReverseFunction();
            appendStatement(rvsForStmt, rvsScope);
            
            SgForStatement* cmtForStmt = buildLoopForReverseFunction();
            appendStatement(cmtForStmt, cmtScope);
            
            generateCode(dagIndexInBasicBlock, rvsCFGs, 
                    isSgScopeStatement(rvsForStmt->get_loop_body()), 
                    isSgScopeStatement(cmtForStmt->get_loop_body()), 
                    pathNumName);
        }

        // Generate all code in the scope of this node.
        generateCodeForBasicBlock(rvsCFG[node].edges, rvsScope, cmtScope);

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
            //cout << "Path added: " << condPaths << endl;
            outEdges.pop_back();
            
            // The last edge can get the current scope.
            if (outEdges.empty())
            {
                rvsScopeTable[condPaths] = rvsScope;
                cmtScopeTable[condPaths] = cmtScope;
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
            pushScopeStack(rvsScope);

            vector<SgExpression*> conditions;
            vector<int> pathNums;

            // Find the path set with the minimum number of paths, then build a
            // logical or expression.
            // Note this part will be optimized later.
            for (size_t i = 0, s = condPaths.size(); i < s; ++i)
            {
                if (condPaths[i])
                {
                    size_t val = pathNumManager_->getPathNumber(dagIndex, i);
                    pathNums.push_back(val);
                    
                    SgVarRefExp* numVar = buildVarRefExp(pathNumName);
                    SgIntVal* intVal = buildIntVal(val);
                    SgExpression* cond = buildEqualityOp(numVar, intVal);
                    conditions.push_back(cond);
                }
            }

            // The if condition.
            SgExpression* condition = NULL;
            
            // Here we put all path numbers into an array then do a binary search
            // on this array. It can improve the performance.
            
            if (conditions.size() > 8)
            {
                // Build an array containing all path numbers.
                
                //pushScopeStack(anonymousNamespace);
                
                SgArrayType* pathNumArrayType = 
                        buildArrayType(buildIntType(), buildIntVal(pathNums.size()));
                
                vector<SgExpression*> pathNumExps;
                foreach (int num, pathNums)
                    pathNumExps.push_back(buildIntVal(num));
                
                SgInitializer* initList = 
                        buildAggregateInitializer(buildExprListExp(pathNumExps));
                
                string arrayName = "conditions" + boost::lexical_cast<string>(counter++);
                SgVariableDeclaration* pathNumArray = 
                        buildVariableDeclaration(arrayName, pathNumArrayType, initList);
                setStatic(pathNumArray);
                
                //anonymousNamespace->append_declaration(pathNumArray);
                prependStatement(pathNumArray, rvsFuncBody);
                prependStatement(copyStatement(pathNumArray), cmtFuncBody);
                
                //popScopeStack();
                
#if 0
                SgExprListExp* para = buildExprListExp(
                                            buildVarRefExp(pathNumArray), 
                                            buildUnsignedIntVal(conditions.size()), 
                                            buildVarRefExp(pathNumName));
                condition = buildFunctionCallExp("__check__", buildBoolType(), para);
#endif
                
                SgExprListExp* para = buildExprListExp(
                                        buildVarRefExp(pathNumArray), 
                                        buildAddOp(
                                            buildVarRefExp(pathNumArray), 
                                            buildUnsignedIntVal(conditions.size())),
                                        buildVarRefExp(pathNumName));
                condition = buildFunctionCallExp("std::binary_search", buildBoolType(), para);
            }
            else
            {
                // Build a logical or expression.
                foreach (SgExpression* cond, conditions)
                {
                    if (condition == NULL)
                        condition = cond;
                    else
                        condition = buildOrOp(condition, cond);
                } 
            }

            SgBasicBlock* rvsTrueBody = buildBasicBlock();
            SgBasicBlock* rvsFalseBody = buildBasicBlock();
            SgIfStmt* rvsIfStmt = 
                    buildIfStmt(buildExprStatement(condition), rvsTrueBody, rvsFalseBody);
            
            SgBasicBlock* cmtTrueBody = buildBasicBlock();
            SgBasicBlock* cmtFalseBody = buildBasicBlock();
            SgIfStmt* cmtIfStmt = 
                    buildIfStmt(buildExprStatement(
                            copyExpression(condition)), 
                            cmtTrueBody, cmtFalseBody);

            // Add this if statement to the scope.
            appendStatement(rvsIfStmt, rvsScope);
            appendStatement(cmtIfStmt, cmtScope);

            // Assign the scope to successors.
            //cout << "Path added: " << condPaths << endl;
            rvsScopeTable[condPaths] = rvsTrueBody;
            cmtScopeTable[condPaths] = cmtTrueBody;
            
            rvsScope = rvsFalseBody;
            cmtScope = cmtFalseBody;
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

            rvsStmt = buildOperationStatement(valNode, opNode->type, lhsNode, rhsNode);
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
    fwdFuncDef_->get_body()->set_parent(funcDef_);
    fwdFuncDef_->set_body(body);
    body->set_parent(fwdFuncDef_);

    SgDeclarationStatement* funcDecl   = funcDef_->get_declaration();//->get_definingDeclaration();
    SgFunctionDeclaration* fwdFuncDecl = fwdFuncDef_->get_declaration();
    SgFunctionDeclaration* rvsFuncDecl = rvsFuncDef_->get_declaration();
    SgFunctionDeclaration* cmtFuncDecl = cmtFuncDef_->get_declaration();
    
#if 0
    if (SgMemberFunctionDeclaration* memFuncDecl = isSgMemberFunctionDeclaration(funcDecl))
    {
        SgClassDefinition* classDef = memFuncDecl->get_class_scope();
        ROSE_ASSERT(classDef);
        classDef->append_member(fwdFuncDecl);
        classDef->append_member(rvsFuncDecl);
        classDef->append_member(cmtFuncDecl);
    }
#endif
    
#if 0
    cout << fwdFuncDecl->unparseToString() << "\n\n";
    cout << rvsFuncDecl->unparseToString() << "\n\n";
    cout << cmtFuncDecl->unparseToString() << "\n\n";
#endif 
    
#if 1
    insertStatementAfter(funcDecl,    fwdFuncDecl);
    insertStatementAfter(fwdFuncDecl, rvsFuncDecl);
    insertStatementAfter(rvsFuncDecl, cmtFuncDecl);
    
    
    ROSE_ASSERT(funcDef_->get_body()->get_parent() == funcDef_);
    ROSE_ASSERT(fwdFuncDef_->get_body()->get_parent() == fwdFuncDef_);
    ROSE_ASSERT(rvsFuncDef_->get_body()->get_parent() == rvsFuncDef_);
    ROSE_ASSERT(cmtFuncDef_->get_body()->get_parent() == cmtFuncDef_);
#else
    SgGlobal* globalScope = SageInterface::getGlobalScope(funcDef_);
    SageInterface::appendStatement(fwdFuncDecl, globalScope);
    SageInterface::appendStatement(rvsFuncDecl, globalScope);
    SageInterface::appendStatement(cmtFuncDecl, globalScope);
#endif
}

void EventReverser::removeEmptyIfStmt(SgNode* node)
{    
    vector<SgIfStmt*> ifStmts = BackstrokeUtility::querySubTree<SgIfStmt>(node);
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
