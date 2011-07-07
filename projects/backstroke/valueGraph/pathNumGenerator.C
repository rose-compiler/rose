#include "pathNumGenerator.h"
#include "functionReverser.h"

#include <boost/foreach.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/topological_sort.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/shared_ptr.hpp>


namespace Backstroke
{

using namespace std;
using namespace SageBuilder;
using namespace SageInterface;

#define foreach BOOST_FOREACH
#define reverse_foreach BOOST_REVERSE_FOREACH


PathNumManager::PathNumManager(const BackstrokeCFG* cfg)
    :   cfg_(cfg), 
        fullCfg_(cfg_->getFunctionDefinition()),
        backEdges_(cfg_->getAllBackEdges()), 
        loops_(cfg_->getAllLoops())
{    
    generatePathNumbers();
    buildNodeCFGVertexMap();
    
    // Work-around. May be removed in the future.
    buildAuxiliaryDags();
}

void PathNumManager::buildNodeCFGVertexMap()
{
    foreach (CFGVertex v, boost::vertices(*cfg_))
    {
        SgNode* node = (*cfg_)[v]->getNode();
        nodeCFGVertexMap_[node] = v;
    }
}

void PathNumManager::buildSuperDAG()
{
    foreach (CFGVertex v, boost::vertices(*cfg_))
    {
        //if (cfgNodesInLoop.count(v) > 0)
        //    continue;
        
        DAGVertex dagNode = boost::add_vertex(superDag_);
        superDag_[dagNode] = v;
        vertexToDagIndex_[v][0] = dagNode;
    }
    
    //cout << boost::num_vertices(*cfg_) << endl;
    //cout << boost::num_vertices(dag) << endl;

    foreach (const CFGEdge& e, boost::edges(*cfg_))
    {
        DAGVertex src = vertexToDagIndex_[boost::source(e, *cfg_)].begin()->second;
        DAGVertex tgt = vertexToDagIndex_[boost::target(e, *cfg_)].begin()->second;
        
        // Ignore back edges.
        if (backEdges_.count(e) > 0)
        {
            continue;
        }
        
        ROSE_ASSERT(vertexToDagIndex_.count(boost::source(e, *cfg_)) > 0);
        ROSE_ASSERT(vertexToDagIndex_.count(boost::target(e, *cfg_)) > 0);
        
        
        
        //// If both nodes are in a loop, don't add it to this DAG now.
        //if (cfgNodesInLoop.count(src) > 0 || cfgNodesInLoop.count(tgt) > 0)
        //    continue;
        

        DAGEdge dagEdge = boost::add_edge(src, tgt, superDag_).first;
        superDag_[dagEdge] = e;
        edgeToDagIndex_[e][0] = dagEdge;
    }
        
}

void PathNumManager::generatePathNumbers()
{
    // Get all loops in this CFG.
    set<CFGVertex> cfgNodesInLoop;
    
    // Get all CFG nodes in loop.
    typedef pair<CFGVertex, set<CFGVertex> > NodeToNodes;
    foreach (const NodeToNodes& loop, loops_)
        cfgNodesInLoop.insert(loop.second.begin(), loop.second.end());
    //cout << cfgNodesInLoop.size() << endl;
    
    dags_.resize(loops_.size() + 1);
    //dags_.resize(1);
        
    // Build the first DAG which contains the whole CFG without loops.
    // This is done by removing all back edges.
    
    DAG& dag = dags_[0];

    foreach (CFGVertex v, boost::vertices(*cfg_))
    {
        //if (cfgNodesInLoop.count(v) > 0)
        //    continue;
        
        DAGVertex dagNode = boost::add_vertex(dag);
        dag[dagNode] = v;
        vertexToDagIndex_[v][0] = dagNode;
    }
    
    //cout << boost::num_vertices(*cfg_) << endl;
    //cout << boost::num_vertices(dag) << endl;

    foreach (const CFGEdge& e, boost::edges(*cfg_))
    {
        // Ignore back edges.
        if (backEdges_.count(e) > 0)
            continue;
        
        ROSE_ASSERT(vertexToDagIndex_.count(boost::source(e, *cfg_)) > 0);
        ROSE_ASSERT(vertexToDagIndex_.count(boost::target(e, *cfg_)) > 0);
        
        DAGVertex src = vertexToDagIndex_[boost::source(e, *cfg_)].begin()->second;
        DAGVertex tgt = vertexToDagIndex_[boost::target(e, *cfg_)].begin()->second;
        
        
        //// If both nodes are in a loop, don't add it to this DAG now.
        //if (cfgNodesInLoop.count(src) > 0 || cfgNodesInLoop.count(tgt) > 0)
        //    continue;
        

        DAGEdge dagEdge = boost::add_edge(src, tgt, dag).first;
        dag[dagEdge] = e;
        edgeToDagIndex_[e][0] = dagEdge;
    }
        
    // Entries and exits of all DAGs.
    vector<DAGVertex> entries(dags_.size());
    vector<DAGVertex> exits(dags_.size());
    entries[0] = vertexToDagIndex_[cfg_->getEntry()].begin()->second;
    exits[0]   = vertexToDagIndex_[cfg_->getExit()].begin()->second;
    
    // Then build other dags for loops.
    int dagIdx = 1;
    foreach (NodeToNodes loop, loops_)
    {
        DAG& dag = dags_[dagIdx];
        CFGVertex header = loop.first;
        headerToDagIndex_[header] = dagIdx;
        loop.second.insert(header);
        
        foreach (CFGVertex cfgNode, loop.second)
        {
            DAGVertex dagNode = boost::add_vertex(dag);
            dag[dagNode] = cfgNode;
            vertexToDagIndex_[cfgNode][dagIdx] = dagNode;
        }
        // Set the exit of the CFG as the exit of this DAG.
        DAGVertex exit = boost::add_vertex(dag);
        dag[exit] = cfg_->getExit();
        vertexToDagIndex_[cfg_->getExit()][dagIdx] = exit;
        
        foreach (CFGVertex cfgNode, loop.second)
        {            
            foreach (const CFGEdge& edge, boost::out_edges(cfgNode, *cfg_))
            {
                DAGVertex dagSrc = vertexToDagIndex_[cfgNode][dagIdx];
                DAGVertex dagTgt;
                CFGVertex tgt = boost::target(edge, *cfg_);
                
                // If the edge is an exit edge, don't add it.
                if (loop.second.count(tgt) == 0)
                    continue;
                
                // If the edge is a back edge, set its target to the exit.
                // If the target is a node not in this loop, set it to exit.
                if (loop.second.count(tgt) == 0 || tgt == header)
                    dagTgt = exit;
                else
                    dagTgt = vertexToDagIndex_[tgt][dagIdx];
                
                DAGEdge dagEdge = boost::add_edge(dagSrc, dagTgt, dag).first;
                dag[dagEdge] = edge;
                edgeToDagIndex_[edge][dagIdx] = dagEdge;
            }
        }
        entries[dagIdx] = vertexToDagIndex_[header][dagIdx];
        exits[dagIdx] = exit;
        
        ++dagIdx;
    }

    pathNumGenerators_.resize(dags_.size());
    // For each DAG, generate its path information.
    for (int i = 0, n = dags_.size(); i != n; ++i)
    {
        PathNumGenerator* pathNumGen = 
                new PathNumGenerator(dags_[i], entries[i], exits[i], cfg_);
        pathNumGen->generatePathNumbers();
        //cout << pathNumGen->getNumberOfPath() << endl;
        pathNumGenerators_[i] = pathNumGen;
        pathInfo_.push_back(make_pair(i, pathNumGen->getNumberOfPath()));
        
        char filename[16];
        sprintf(filename, "dag%d.dot", i);
        dagToDot(dags_[i], filename);
    }
}

void PathNumManager::buildAuxiliaryDags()
{
    typedef Backstroke::FullCFG::Vertex CFGVertex;
    typedef Backstroke::FullCFG::Edge   CFGEdge;
    
    Backstroke::FullCFG& fullCFG = fullCfg_;
    
    map<CFGVertex, set<CFGVertex> > loops(fullCFG.getAllLoops());
    set<CFGEdge> backEdges(fullCFG.getAllBackEdges());
    
    map<CFGVertex, map<int, DAGVertex> > vertexToDagIndex;
    map<CFGEdge, map<int, DAGEdge> > edgeToDagIndex;
    
// Get all loops in this CFG.
    set<CFGVertex> cfgNodesInLoop;
    
    // Get all CFG nodes in loop.
    typedef pair<CFGVertex, set<CFGVertex> > NodeToNodes;
    foreach (const NodeToNodes& loop, loops)
        cfgNodesInLoop.insert(loop.second.begin(), loop.second.end());
    //cout << cfgNodesInLoop.size() << endl;
    
    auxDags_.resize(loops.size() + 1);
    //dags_.resize(1);
        
    // Build the first DAG which contains the whole CFG without loops.
    // This is done by removing all back edges.
    
    DAG& dag = auxDags_[0];

    foreach (CFGVertex v, boost::vertices(fullCFG))
    {
        //if (cfgNodesInLoop.count(v) > 0)
        //    continue;
        
        DAGVertex dagNode = boost::add_vertex(dag);
        dag[dagNode] = v;
        vertexToDagIndex[v][0] = dagNode;
    }
    
    //cout << boost::num_vertices(*cfg_) << endl;
    //cout << boost::num_vertices(dag) << endl;

    foreach (const CFGEdge& e, boost::edges(fullCFG))
    {
        // Ignore back edges.
        if (backEdges.count(e) > 0)
            continue;
        
        ROSE_ASSERT(vertexToDagIndex.count(boost::source(e, fullCFG)) > 0);
        ROSE_ASSERT(vertexToDagIndex.count(boost::target(e, fullCFG)) > 0);
        
        DAGVertex src = vertexToDagIndex[boost::source(e, fullCFG)].begin()->second;
        DAGVertex tgt = vertexToDagIndex[boost::target(e, fullCFG)].begin()->second;
        
        
        //// If both nodes are in a loop, don't add it to this DAG now.
        //if (cfgNodesInLoop.count(src) > 0 || cfgNodesInLoop.count(tgt) > 0)
        //    continue;
        

        DAGEdge dagEdge = boost::add_edge(src, tgt, dag).first;
        dag[dagEdge] = e;
        edgeToDagIndex[e][0] = dagEdge;
    }
        
    // Entries and exits of all DAGs.
    vector<DAGVertex> entries(auxDags_.size());
    vector<DAGVertex> exits(auxDags_.size());
    entries[0] = vertexToDagIndex[fullCFG.getEntry()].begin()->second;
    exits[0]   = vertexToDagIndex[fullCFG.getExit()].begin()->second;
    
    // Then build other dags for loops.
    int dagIdx = 1;
    foreach (NodeToNodes loop, loops)
    {
        DAG& dag = auxDags_[dagIdx];
        CFGVertex header = loop.first;
        //headerToDagIndex[header] = dagIdx;
        loop.second.insert(header);
        
        foreach (CFGVertex cfgNode, loop.second)
        {
            DAGVertex dagNode = boost::add_vertex(dag);
            dag[dagNode] = cfgNode;
            vertexToDagIndex[cfgNode][dagIdx] = dagNode;
        }
        // Set the exit of the CFG as the exit of this DAG.
        DAGVertex exit = boost::add_vertex(dag);
        dag[exit] = fullCFG.getExit();
        vertexToDagIndex[fullCFG.getExit()][dagIdx] = exit;
        
        foreach (CFGVertex cfgNode, loop.second)
        {            
            foreach (const CFGEdge& edge, boost::out_edges(cfgNode, fullCFG))
            {
                DAGVertex dagSrc = vertexToDagIndex[cfgNode][dagIdx];
                DAGVertex dagTgt;
                CFGVertex tgt = boost::target(edge, fullCFG);
                
                // If the edge is an exit edge, don't add it.
                if (loop.second.count(tgt) == 0)
                    continue;
                
                // If the edge is a back edge, set its target to the exit.
                // If the target is a node not in this loop, set it to exit.
                if (loop.second.count(tgt) == 0 || tgt == header)
                    dagTgt = exit;
                else
                    dagTgt = vertexToDagIndex[tgt][dagIdx];
                
                DAGEdge dagEdge = boost::add_edge(dagSrc, dagTgt, dag).first;
                dag[dagEdge] = edge;
                edgeToDagIndex[edge][dagIdx] = dagEdge;
            }
        }
        entries[dagIdx] = vertexToDagIndex[header][dagIdx];
        exits[dagIdx] = exit;
        
        ++dagIdx;
    }    
}


PathNumManager::~PathNumManager()
{
    foreach (PathNumGenerator* gen, pathNumGenerators_)
        delete gen;
}

PathNumManager::CFGVertex PathNumManager::getCFGNode(SgNode* node) const
{
    // Trace this node up to find which CFG vertex it belongs to.
    while (node)
    {
        if (nodeCFGVertexMap_.find(node) != nodeCFGVertexMap_.end())
            break;
        node = node->get_parent();
    }

    ROSE_ASSERT(node);

    CFGVertex cfgNode = nodeCFGVertexMap_.find(node)->second;
    ROSE_ASSERT(vertexToDagIndex_.count(cfgNode) > 0);

    return cfgNode;
}

bool PathNumManager::isDataMember(SgNode* node) const
{
    SgInitializedName* initName = isSgInitializedName(node);
    if (initName == NULL)
        return false;
    if (isSgClassDefinition(initName->get_declaration()->get_parent()))
        return true;
    return false;
}

PathInfo PathNumManager::getPathNumbers(SgNode* node) const
{
    CFGVertex cfgNode;
    // If the given node is a data member of a class, set its CFG node to the exit.
    if (isDataMember(node))
        cfgNode = cfg_->getExit();
    else
        cfgNode = getCFGNode(node);

    int idx;
    DAGVertex dagNode;
    PathInfo paths;
    
    typedef std::map<int, DAGVertex>::value_type IndexToDagVertex;
    foreach (const IndexToDagVertex& idxNode, vertexToDagIndex_.find(cfgNode)->second)
    {
        boost::tie(idx, dagNode) = idxNode;
        PathSet p = pathNumGenerators_[idx]->getPaths(dagNode);
        if (p.any())
            paths[idx] = p;
    }
    return paths;
}

std::pair<int, std::map<int, PathSet> >
PathNumManager::getVisiblePathNumbers(SgNode* node) const
{
#if 0
    CFGVertex cfgNode = getCFGNode(node);

    int idx;
    DAGVertex dagNode;
    boost::tie(idx, dagNode) = (vertexToDagIndex_.find(cfgNode))->second;

    //PathInfo paths;
    
    typedef map<int, DAGVertex>::value_type IndexNode;
    foreach (const IndexNode& idxNode, vertexToDagIndex_.find(cfgNode)->second)
    {
        boost::tie(idx, dagNode) = idxNode;
        paths[idx] = pathNumGenerators_[idx]->getVisibleNumAndPaths(dagNode);
        int idx = i
    }
    
    return std::make_pair(idx,
                          pathNumGenerators_[idx]->getVisibleNumAndPaths(dagNode));
#endif
}

PathInfo PathNumManager::getPathNumbers(
        SgNode* node1, SgNode* node2) const
{
    //cout << node1->unparseToString() << ' ' << node2->unparseToString() << endl;
    CFGVertex cfgNode1 = getCFGNode(node1);
    CFGVertex cfgNode2 = getCFGNode(node2);

    CFGEdge cfgEdge = boost::edge(cfgNode1, cfgNode2, *cfg_).first;
    ROSE_ASSERT(edgeToDagIndex_.count(cfgEdge) > 0);

    int idx;
    DAGEdge dagEdge;
    PathInfo paths;
    
    typedef std::map<int, DAGEdge>::value_type IndexToDagEdge;
    foreach (const IndexToDagEdge& idxEdge, edgeToDagIndex_.find(cfgEdge)->second)
    {
        boost::tie(idx, dagEdge) = idxEdge;
        paths[idx] = pathNumGenerators_[idx]->getPaths(dagEdge);
    }
    return paths;
}


PathInfo PathNumManager::getAllPaths() const
{
    PathInfo allPaths;
    for (size_t i = 0, s = dags_.size(); i < s; ++i)
    {
        PathSet paths(getNumberOfPath(i));
        paths.flip();
        allPaths[i] = paths;
    }
    return allPaths;
}

void PathNumManager::getAstNodeIndices(size_t index, map<SgNode*, int>& nodeIndicesTable) const
{
    const DAG& dag = auxDags_[index];

    vector<DAGVertex> nodes;
    boost::topological_sort(dag, back_inserter(nodes));
    
    int num = 0;
    reverse_foreach (DAGVertex node, nodes)
    {
        SgNode* astNode = fullCfg_[dag[node]]->getNode();
        nodeIndicesTable[astNode] = num * 10;
        num++;
    }
    
    // Add a NULL entry to the table because it is possible that some VG node contains
    // a NULL AST node inside.
    nodeIndicesTable[NULL] = INT_MAX;
}

#if 0
map<PathSet, int> PathNumManager::getPathsIndices(size_t index) const
{
    map<PathSet, int> pathsIndicesTable;

    vector<DAGVertex> nodes;
    boost::topological_sort(dags_[index], back_inserter(nodes));
    
    int num = 0;
    foreach (DAGVertex node, nodes)
    {
        PathSet paths = pathNumGenerators_[index]->getPaths(node);
        if (pathsIndicesTable.count(paths) == 0)
            pathsIndicesTable[paths] = num++;

        
        foreach (const DAGEdge& edge, boost::in_edges(node, dags_[index]))
        {
            PathSet paths = pathNumGenerators_[index]->getPaths(edge);
            if (pathsIndicesTable.count(paths) == 0)
                pathsIndicesTable[paths] = num++;
        }
    }
    
    reverse_foreach (DAGVertex node, nodes)
    {
        map<int, PathSet> visiblePaths = 
            pathNumGenerators_[index]->getVisibleNumAndPaths(node);
        PathSet paths = pathNumGenerators_[index]->getPaths(node);
        
        typedef map<int, PathSet>::value_type IntPathSetPair;
        foreach (const IntPathSetPair& intPaths, visiblePaths)
        {
            if (pathsIndicesTable.count(intPaths.second) == 0)
                pathsIndicesTable[intPaths.second] = pathsIndicesTable[paths];
        }
    }
    
    return pathsIndicesTable;
}
#endif

void PathNumManager::insertPathNumToFwdFunc()
{
    ROSE_ASSERT(pathNumGenerators_.size() == dags_.size());

    SgFunctionDefinition* funcDef =
            isSgFunctionDefinition((*cfg_)[cfg_->getEntry()]->getNode());
    ROSE_ASSERT(funcDef);

    // Insert the declaration of the path number in the front of forward function.
    string pathNumName = "__num0";
    SgVariableDeclaration* pathNumDecl =
            SageBuilder::buildVariableDeclaration(
                pathNumName,
                SageBuilder::buildIntType(),
                SageBuilder::buildAssignInitializer(
                    SageBuilder::buildIntVal(0)),
                funcDef->get_body());
    prependStatement(pathNumDecl, funcDef->get_body());

    //! Insert path number update statement on CFG edges.
    for (size_t i = 0, m = dags_.size(); i != m; ++i)
    {
        // Each DAG has a different path num name.
        pathNumName = string("__num") + boost::lexical_cast<string>(i);
        
        typedef pair<DAGEdge, int> EdgeValuePair;
        
        // Sort edges which will be instrumented by topological order of its DAG.
        // This makes sure instrumenting a parent prior to instrumenting its children.
        
        multimap<CFGVertex, EdgeValuePair> nodeToEdge;
        foreach (const EdgeValuePair& edgeVal, pathNumGenerators_[i]->edgeValues_)
        {
            CFGVertex src = boost::source(dags_[i][edgeVal.first], *cfg_);
            // If the edge value is 0, no updating.
            if (edgeVal.second == 0) continue;
            
            nodeToEdge.insert(make_pair(src, edgeVal));
        }

        vector<CFGVertex> nodes;
        boost::topological_sort(dags_[i], std::back_inserter(nodes));
        
        vector<EdgeValuePair> edgesToInstrument;
        foreach (CFGVertex node, nodes)
        {
            typedef pair<CFGVertex, EdgeValuePair> T;
            foreach (const T& nodeEdge, nodeToEdge.equal_range(node))
                edgesToInstrument.push_back(nodeEdge.second);
        }
        
        foreach (const EdgeValuePair& edgeVal, edgesToInstrument)
        {
            BackstrokeCFG::Edge cfgEdge = dags_[i][edgeVal.first];
            // Insert the path num update on the CFG edge.
            insertPathNumberOnEdge(cfgEdge, pathNumName, edgeVal.second);
        }
    }

    // In the forward event, push the path number variable to the stack just before
    // the exit of the function.
    
    // Build a push function call.
    pushScopeStack(funcDef->get_body());
    SgExpression* pathNumVar = SageBuilder::buildVarRefExp(pathNumDecl);
    SgStatement* pushPathNumFuncCall = SageBuilder::buildExprStatement(
            buildPushFunctionCall(pathNumVar));
    popScopeStack();
    
    // For each in edges to exit node of CFG, insert the push functin on that edge.
    foreach (BackstrokeCFG::Vertex cfgNode, 
            boost::inv_adjacent_vertices(cfg_->getExit(), *cfg_))
    {
        SgNode* astNode = (*cfg_)[cfgNode]->getNode();
        
        // For each return statement, insert the push function before it.
        if (SgReturnStmt* returnStmt = isSgReturnStmt(astNode))
        {
            SgStatement* pushFuncCall = copyStatement(pushPathNumFuncCall);
            insertStatementBefore(returnStmt, pushFuncCall);
        }
    }
    appendStatement(pushPathNumFuncCall, funcDef->get_body());
    
    fixVariableReferences(funcDef);
}

namespace
{
    
SgStatement* getLoopStatement(SgNode* header)
{
    SgNode* parent = header;
    while (parent)
    {
        switch (parent->variantT())
        {
            case V_SgWhileStmt:
            case V_SgDoWhileStmt:
            case V_SgForStatement:
                return isSgStatement(parent);
            default:
                parent = parent->get_parent();
        }
    }
    return NULL;
}

}

void PathNumManager::insertLoopCounterToFwdFunc(const set<SgNode*>& loopHeaders)
{    
    //map<CFGVertex, string> loopCounterNames;
    int counter = 1;
    
    typedef pair<CFGVertex, set<CFGVertex> > NodeToNodes;
    foreach (const NodeToNodes& headerAndLoopNodes, loops_)
    {
        CFGVertex header = headerAndLoopNodes.first;
        
        // If this loop is not in route graph, skip it.
        if (loopHeaders.count((*cfg_)[header]->getNode()) == 0)
            continue;
        
        set<CFGVertex> loopNodes = headerAndLoopNodes.second;
        loopNodes.insert(header);
        
        SgStatement* loopStmt = getLoopStatement((*cfg_)[header]->getNode());
        ROSE_ASSERT(loopStmt);
        
        // Insert the declaration of the path number before the loop stmt.
        string pathNumName = string("__num") + boost::lexical_cast<string>(counter);
        SgVariableDeclaration* pathNumDecl =
                buildVariableDeclaration(
                    pathNumName,
                    buildIntType(),
                    buildAssignInitializer(buildIntVal(0)),
                    getScope(loopStmt));
        insertStatementBefore(loopStmt, pathNumDecl);
        
        // Build the counter declaration.
        string loopCounterName = string("__counter") + boost::lexical_cast<string>(counter);
        SgStatement* loopCounterDecl =
                buildVariableDeclaration(
                    loopCounterName,
                    buildIntType(),
                    buildAssignInitializer(buildIntVal(0)),
                    getScope(loopStmt));
        insertStatementBefore(loopStmt, loopCounterDecl);
        
        foreach (CFGVertex cfgNode, loopNodes)
        {
            foreach (const CFGEdge& cfgEdge, boost::out_edges(cfgNode, *cfg_))
            {
                CFGVertex tgt = boost::target(cfgEdge, *cfg_);
                // Backedges
                if (tgt == header)
                    insertLoopCounterIncrOnEdge(cfgEdge, pathNumName, loopCounterName);
                // exit edges
                else if (loopNodes.count(tgt) == 0)
                    insertLoopCounterPushOnEdge(cfgEdge, loopCounterName);
            }
        }
        
        ++counter;
        //fixVariableReferences(getScope(loopStmt));
    }
    
}

//namespace 
//{
//    SgStatement* getAncestorStatement(SgNode* node)
//    {
//        SgStatement* stmt;
//        while (!(stmt = isSgStatement(node)))
//        {
//            node = node->get_parent();
//            if (node == NULL)
//                return NULL;
//        }
//        return stmt;
//    }
//
//} // end of anonymous

void PathNumManager::insertPathNumberOnEdge(
        const BackstrokeCFG::Edge& cfgEdge,
        const string& pathNumName,
        int val)
{
    SgExpression* pathNumExp = buildPlusAssignOp(
                                buildVarRefExp(pathNumName),
                                buildIntVal(val));

    SgNode* src = (*cfg_)[cfgEdge]->source().getNode();
    SgNode* tgt = (*cfg_)[cfgEdge]->target().getNode();

    if (SgIfStmt* ifStmt = isSgIfStmt(src))
    {
        SgStatement* pathNumStmt = buildExprStatement(pathNumExp);
        
        if (isAncestor(src, tgt))
        {
            SgStatement* s = getEnclosingStatement(tgt);
            if (SgBasicBlock* body = isSgBasicBlock(s))
            {
                prependStatement(pathNumStmt, body);
            }
            else
                ROSE_ASSERT(!"The target of the edge from a if statement is not a basic block!");
            //SageInterface::insertStatementBefore(stmt, pathNumStmt);
        }
        else
        {
            ROSE_ASSERT(ifStmt->get_false_body() == NULL);
            // This edge must represent false body here.
            SgBasicBlock* falseBody = SageBuilder::buildBasicBlock(pathNumStmt);
            ifStmt->set_false_body(falseBody);
            falseBody->set_parent(ifStmt);
        }
    }
    
    else if (SgSwitchStatement* switchStmt = isSgSwitchStatement(src))
    {
        SgStatement* pathNumStmt = buildExprStatement(pathNumExp);
        
        if (SgCaseOptionStmt* caseStmt = isSgCaseOptionStmt(tgt))
        {
            if (SgBasicBlock* body = isSgBasicBlock(caseStmt->get_body()))
            {
                prependStatement(pathNumStmt, body);
                
                // A trick to prevent that the previous case option does not have a break.
                SgStatement* pathNumStmt2 = buildExprStatement(
                                                buildMinusAssignOp(
                                                    buildVarRefExp(pathNumName),
                                                    buildIntVal(val)));

                insertStatementBefore(caseStmt, pathNumStmt2);
            }
            else
            {
                ROSE_ASSERT(!"The target of the edge from a switch statement is not a basic block!");
            }
        }
        else if (SgDefaultOptionStmt* defaultStmt = isSgDefaultOptionStmt(tgt))
        {
            if (SgBasicBlock* body = isSgBasicBlock(defaultStmt->get_body()))
            {
                prependStatement(pathNumStmt, body);
                
                // A trick to prevent that the previous case option does not have a break.
                SgStatement* pathNumStmt2 = buildExprStatement(
                                                buildMinusAssignOp(
                                                    buildVarRefExp(pathNumName),
                                                    buildIntVal(val)));

                insertStatementBefore(defaultStmt, pathNumStmt2);
            }
            else
            {
                ROSE_ASSERT(!"The target of the edge from a switch statement is not a basic block!");
            }
        }
        else if (!isAncestor(src, tgt))
        {
            // In this case, this switch statement does not have a default option.
            // We build one here.
            SgDefaultOptionStmt* defaultStmt = SageBuilder::buildDefaultOptionStmt(pathNumStmt);
            switchStmt->append_default(defaultStmt);
        }
        else
        {
            ROSE_ASSERT(0);
        }
    }
    
    else if (SgWhileStmt* whileStmt = isSgWhileStmt(src))
    {
        SgStatement* pathNumStmt = buildExprStatement(pathNumExp);
        
        // If the edge points to while body.
        if (isAncestor(src, tgt))
        {
            SgStatement* s = getEnclosingStatement(tgt);
            if (SgBasicBlock* body = isSgBasicBlock(s))
            {
                prependStatement(pathNumStmt, body);
            }
            else
                ROSE_ASSERT(!"The target of the edge from a if statement is not a basic block!");
            //SageInterface::insertStatementBefore(stmt, pathNumStmt);
        }
        else
        {
            insertStatementAfter(whileStmt, pathNumStmt);
            
            // For all breaks in this while statement, add a -= statement to offset the one
            // just added after while stmt.
            foreach (SgBreakStmt* breakStmt, 
                    querySubTree<SgBreakStmt>(whileStmt, V_SgBreakStmt))
            {
                SgStatement* pathNumStmt2 = buildExprStatement(
                                                    buildMinusAssignOp(
                                                        buildVarRefExp(pathNumName),
                                                        buildIntVal(val)));
                insertStatementBefore(breakStmt, pathNumStmt2);
            }
            
            foreach (SgGotoStatement* gotoStmt, 
                    querySubTree<SgGotoStatement>(whileStmt, V_SgGotoStatement))
            {
                ROSE_ASSERT(!"Cannot handle goto in while stmt!");
            }
        }
    }
    
    else if (SgForStatement* forStmt = isSgForStatement(src))
    {
        SgStatement* pathNumStmt = buildExprStatement(pathNumExp);
        
        // If the edge points to while body.
        if (isAncestor(src, tgt))
        {
            SgStatement* s = getEnclosingStatement(tgt);
            if (SgBasicBlock* body = isSgBasicBlock(s))
            {
                prependStatement(pathNumStmt, body);
            }
            else
                ROSE_ASSERT(!"The target of the edge from a if statement is not a basic block!");
            //SageInterface::insertStatementBefore(stmt, pathNumStmt);
        }
        else
        {
            insertStatementAfter(forStmt, pathNumStmt);
            
            // For all breaks in this while statement, add a -= statement to offset the one
            // just added after while stmt.
            foreach (SgBreakStmt* breakStmt, 
                    querySubTree<SgBreakStmt>(forStmt, V_SgBreakStmt))
            {
                SgStatement* pathNumStmt2 = buildExprStatement(
                                                    buildMinusAssignOp(
                                                        buildVarRefExp(pathNumName),
                                                        buildIntVal(val)));
                insertStatementBefore(breakStmt, pathNumStmt2);
            }
            
            foreach (SgGotoStatement* gotoStmt, 
                    querySubTree<SgGotoStatement>(forStmt, V_SgGotoStatement))
            {
                ROSE_ASSERT(!"Cannot handle goto in while stmt!");
            }
        }
    }
    
    else if (SgConditionalExp* condExp = isSgConditionalExp(src))
    {
        SgExpression* expr = isSgExpression(tgt);
        ROSE_ASSERT(expr);
        replaceExpression(expr, buildCommaOpExp(pathNumExp, copyExpression(expr)));
    }
    
    else
    {
        // To avoid that replacing an expression invalidates an expression which is 
        // used later, collect expressions replaces in the following table.
        static map<SgNode*, SgNode*> replaceTable;

        while (replaceTable.count(src))
            src = replaceTable[src];

        //    cout << src->class_name() << endl;
            
        SgNode* parent = src->get_parent();

        if (isSgAndOp(parent) || isSgOrOp(parent))
        {            
            SgExpression* expr = isSgBinaryOp(parent)->get_rhs_operand();
            ROSE_ASSERT(expr);

            SgExpression* newExpr = buildCommaOpExp(pathNumExp, copyExpression(expr));
            replaceTable[expr] = newExpr;

            replaceExpression(expr, newExpr);
        }
        else
        {
            ROSE_ASSERT(false);
        }
    }
}

void PathNumManager::insertLoopCounterIncrOnEdge(
            const BackstrokeCFG::Edge& cfgEdge,
            const std::string& pathNumName,
            const std::string& counterName)
{
    SgNode* src = (*cfg_)[cfgEdge]->source().getNode();
    //SgNode* tgt = (*cfg_)[cfgEdge]->target().getNode();
    pushScopeStack(getScope(src));
    
    SgStatement* counterIncrStmt = 
            buildExprStatement(buildPlusPlusOp(buildVarRefExp(counterName)));
    SgStatement* pushPathNumStmt = 
            buildExprStatement(buildPushFunctionCall(buildVarRefExp(pathNumName)));
    SgStatement* setPathNumZeroStmt = 
            buildExprStatement(buildAssignOp(buildVarRefExp(pathNumName), buildIntVal(0)));
    
    
    if (SgContinueStmt* contStmt = isSgContinueStmt(src))
    {
        insertStatementBefore(contStmt, counterIncrStmt);
        insertStatementBefore(contStmt, pushPathNumStmt);
        insertStatementBefore(contStmt, setPathNumZeroStmt);
    }
    else if (SgForStatement* forStmt = isSgForStatement(getLoopStatement(src)))
    {
        // For for statement, we can just put the counter increment before continues
        // and at the end of the for body
        foreach (SgContinueStmt* contStmt, 
                querySubTree<SgContinueStmt>(forStmt, V_SgContinueStmt))
        {
            // Make sure this continue statement is for this for statement.
            if (getLoopStatement(contStmt) != forStmt)
                continue;
            insertStatementBefore(contStmt, counterIncrStmt);
            insertStatementBefore(contStmt, pushPathNumStmt);
            insertStatementBefore(contStmt, setPathNumZeroStmt);
        }
        
        SgBasicBlock* loopBody = isSgBasicBlock(forStmt->get_loop_body());
        if (loopBody == NULL)
            ROSE_ASSERT(!"The body of the loop is not a basic block!");
        appendStatement(counterIncrStmt, loopBody);
        appendStatement(pushPathNumStmt, loopBody);
        appendStatement(setPathNumZeroStmt, loopBody);
    }
    else
    {
        SgStatement* stmt = getEnclosingStatement(src);
        if (isSgBasicBlock(stmt))
            stmt = isSgStatement(stmt->get_parent());
        insertStatementAfter(stmt, setPathNumZeroStmt);
        insertStatementAfter(stmt, pushPathNumStmt);
        insertStatementAfter(stmt, counterIncrStmt);
    }
    
    popScopeStack();
}

void PathNumManager::insertLoopCounterPushOnEdge(
            const BackstrokeCFG::Edge& cfgEdge,
            const std::string& counterName)
{
    SgNode* src = (*cfg_)[cfgEdge]->source().getNode();
    SgNode* tgt = (*cfg_)[cfgEdge]->target().getNode();
    
    pushScopeStack(getScope(src));
    
    SgStatement* counterPushStmt = 
            buildExprStatement(buildPushFunctionCall(buildVarRefExp(counterName)));
    
    //SgNode* tgt = (*cfg_)[cfgEdge]->target().getNode();
    
    if (SgWhileStmt* stmt = isSgWhileStmt(src))
        insertStatementAfter(stmt, counterPushStmt);
    else if (SgForStatement* stmt = isSgForStatement(src))
        insertStatementAfter(stmt, counterPushStmt);
    else if (isSgIfStmt(src))
    {
        if (SgBasicBlock* basicBlock = isSgBasicBlock(tgt))
        {
            // We only push the counter if the exit edge goes through return 
            // statement, not a break statement. That is because for break, there
            // is another push just after the loop statement.
            foreach (SgStatement* stmt, basicBlock->get_statements())
            {
                if (isSgReturnStmt(stmt))
                {
                    prependStatement(counterPushStmt, basicBlock);
                    break;
                }
                else if (isSgGotoStatement(stmt))
                    ROSE_ASSERT(!"Cannot handle goto here!");
            }
        }
    }
    else
    {
        cout << src->class_name() << endl;
        ROSE_ASSERT(false);
    }
    
    popScopeStack();
}

void PathNumManager::dagToDot(const DAG& dag, const std::string& filename)
{
    ofstream ofile(filename.c_str(), std::ios::out);
    boost::write_graphviz(ofile, dag);    
}

void PathNumGenerator::getEdgeValues()
{
    vector<Vertex> nodes;
    boost::topological_sort(dag_, std::back_inserter(nodes));

    // This algorithm is from "Ball T, Larus JR. Efficient Path Profiling."
    foreach (Vertex v, nodes)
    {
        size_t& pathNum = pathNumbersOnVertices_[v];
        
        // If this node is a leaf.
        if (boost::out_degree(v, dag_) == 0)
        {
            pathNum = 1;
            continue;
        }

        
        // Collect all out edges first then sort them. For && and || expression,
        // we choose the specific edge whose value is not zero.
        vector<Edge> outEdges;
        foreach (const Edge& e, boost::out_edges(v, dag_))
        {
            outEdges.push_back(e);
        }
        
        if (outEdges.size() == 2)
        {
            // Get the source CFG node.
            SgNode* src = (*cfg_)[dag_[v]]->getNode();
            SgNode* parent = src->get_parent();

            if (isSgAndOp(parent))
            {
                // Get the first edge see if it is a true edge.
                BackstrokeCFG::CFGEdgePtr edge = (*cfg_)[dag_[outEdges[0]]];
                if (edge->condition() == VirtualCFG::eckTrue)
                    swap(outEdges[0], outEdges[1]);
            }

            else if (isSgOrOp(parent))
            {
                // Get the first edge see if it is a true edge.
                BackstrokeCFG::CFGEdgePtr edge = (*cfg_)[dag_[outEdges[0]]];
                if (edge->condition() == VirtualCFG::eckFalse)
                    swap(outEdges[0], outEdges[1]);
            }
        }
        
        pathNum = 0;
        foreach (const Edge& e, outEdges)
        {
            Vertex tar = boost::target(e, dag_);
            ROSE_ASSERT(pathNumbersOnVertices_.count(tar) > 0);
            ROSE_ASSERT(pathNumbersOnVertices_[tar] > 0);
            
            edgeValues_[e] = pathNum;
            pathNum += pathNumbersOnVertices_[tar];
        }
        
#if 1
        // Make pathNumbersOnVertices_[v] power of 2.
        for (int i = 0; ; ++i)
        {
            size_t val = 1 << i;
            if (pathNum == val)
                break;
            if (pathNum < val)
            {
                pathNum = val;
                break;
            }
        }        
#endif
    }
}

void PathNumGenerator::getAllPaths()
{
    std::vector<Path> paths;
    map<Vertex, vector<Edge> > pathsOnVertex;

    stack<Vertex> nodes;
    nodes.push(entry_);
    while (!nodes.empty())
    {
        Vertex node = nodes.top();
        nodes.pop();

        // For each node, propagate its edges to its successors.
        foreach (const Edge& e, boost::out_edges(node, dag_))
        {
            Vertex tar = boost::target(e, dag_);
            Path path = pathsOnVertex[node];
            path.push_back(e);
            pathsOnVertex[tar].swap(path);
            
            nodes.push(tar);
        }

        // The exit is the last node of each path.
        if (node == exit_)
            paths.push_back(pathsOnVertex[node]);
    }
    
    // Use this map to sort all paths by their values.
    map<size_t, Path*> valuePathTable;

    // Get the value for each path. Each path has a unique value.
    foreach (Path& path, paths)
    {
        size_t val = 0;
        foreach (const Edge& e, path)
            val += edgeValues_[e];
        valuePathTable[val] = &path;
    }
    
    paths_.resize(paths.size());
    pathNumbers_.resize(paths.size());
    size_t idx = 0;
    
    typedef map<size_t, Path*>::value_type IntPathPair;
    foreach (const IntPathPair& valPath, valuePathTable)
    {
        // Assign the path values here.
        pathNumbers_[idx] = valPath.first;
        paths_[idx].swap(*(valPath.second));
        ++idx;
    }
}

void PathNumGenerator::getAllPathNumForNodesAndEdges()
{
    int pathNumber = getNumberOfPath();
    foreach (Vertex v, boost::vertices(dag_))
        //pathsForNode_[v].allPath.resize(pathNumber);
        pathsForNode_[v].resize(pathNumber);
    foreach (const Edge& e, boost::edges(dag_))
        pathsForEdge_[e].resize(pathNumber);

    int i = 0;
    foreach (const Path& path, paths_)
    {
        // The visible number is the path number on each vertex which
        // can see it in runtime.
        int visibleNum = 0;
        foreach (const Edge& edge, path)
        {
            visibleNum += edgeValues_[edge];
            Vertex tar = boost::target(edge, dag_);

#if 0
            PathSetOnVertex& pathSetOnVertex = pathsForNode_[tar];

            if (pathSetOnVertex.numToPath.count(visibleNum) == 0)
                pathSetOnVertex.numToPath[visibleNum].resize(pathNumber);
            pathSetOnVertex.numToPath[visibleNum].set(i);

            pathSetOnVertex.allPath.set(i);
#endif
            pathsForNode_[tar].set(i);
            pathsForEdge_[edge].set(i);
        }
        ++i;
    }
}

#if 0
PredicateManager::PredicateManager(const BackstrokeCFG* cfg, const BackstrokeCDG* cdg)
: cfg_(cfg), cdg_(cdg)
{
    dags_.push_back(DAG());
    DAG& dag = dags_[0];

    foreach (CFGVertex v, boost::vertices(*cfg_))
    {
        DAGVertex dagNode = boost::add_vertex(dag);
        dag[dagNode] = v;
        vertexToDagIndex_[v] = make_pair(0, dagNode);
    }

    foreach (const CFGEdge& e, boost::edges(*cfg_))
    {
        ROSE_ASSERT(vertexToDagIndex_.count(boost::source(e, *cfg_)) > 0);
        ROSE_ASSERT(vertexToDagIndex_.count(boost::target(e, *cfg_)) > 0);

        DAGEdge dagEdge = boost::add_edge(
                vertexToDagIndex_[boost::source(e, *cfg_)].second,
                vertexToDagIndex_[boost::target(e, *cfg_)].second, dag).first;
        dag[dagEdge] = e;
        edgeToDagIndex_[e] = make_pair(0, dagEdge);
    }

    DAGVertex entry = vertexToDagIndex_[cfg_->getEntry()].second;
    DAGVertex exit  = vertexToDagIndex_[cfg_->getExit()].second;
    
    vector<CFGVertex> branchNodes;

    // For each DAG, generate its path information.
    foreach (const DAG& dag, dags_)
    {
        foreach (DAGVertex dagNode, boost::vertices(dag))
        {
            // Get the number of out edges of this node.
            int outDegree = boost::out_degree(dagNode, dag);
            
            if (outDegree > 1)
            {
                // Get the number of bits needed to save the flag of each out edge.
                int bitsNum = 1;
                while ((1 << bitsNum) < outDegree)
                    ++bitsNum;
                
                
                branchNodes.push_back(dag[dagNode]);
            }
        }

    }    
}

#endif

} // end of Backstroke
