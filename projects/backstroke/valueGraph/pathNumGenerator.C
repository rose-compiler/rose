#include "pathNumGenerator.h"
#include "functionReverser.h"

#include <boost/foreach.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/graph/topological_sort.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/shared_ptr.hpp>


namespace Backstroke
{

using namespace std;

#define foreach BOOST_FOREACH
#define reverse_foreach BOOST_REVERSE_FOREACH


PathNumManager::PathNumManager(const BackstrokeCFG* cfg)
    : cfg_(cfg)
{
    generatePathNumbers();
    buildNodeCFGVertexMap();
}

void PathNumManager::buildNodeCFGVertexMap()
{
    foreach (CFGVertex v, boost::vertices(*cfg_))
    {
        SgNode* node = (*cfg_)[v]->getNode();
        nodeCFGVertexMap_[node] = v;
    }
}

void PathNumManager::generatePathNumbers()
{
    map<CFGVertex, set<CFGVertex> > loops = cfg_->getAllLoops();
    cout << cfg_->getAllBackEdges().size() << endl;
    cout << loops.begin()->second.size() << endl;
    getchar();
    
    
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

    // For each DAG, generate its path information.
    foreach (const DAG& dag, dags_)
    {
        PathNumGenerator* pathNumGen = new PathNumGenerator(dag, entry, exit);
        pathNumGen->generatePathNumbers();

        pathNumGenerators_.push_back(pathNumGen);

        int parentIdx = 0;
        pathInfo_.push_back(make_pair(parentIdx, pathNumGen->getNumberOfPath()));
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

std::pair<int, PathSet> PathNumManager::getPathNumbers(SgNode* node) const
{
    CFGVertex cfgNode;
    // If the given node is a data member of a class, set its CFG node to the exit.
    if (isDataMember(node))
        cfgNode = cfg_->getExit();
    else
        cfgNode = getCFGNode(node);

    int idx;
    DAGVertex dagNode;
    boost::tie(idx, dagNode) = (vertexToDagIndex_.find(cfgNode))->second;

    return std::make_pair(idx, pathNumGenerators_[idx]->getPaths(dagNode));
}

std::pair<int, std::map<int, PathSet> >
PathNumManager::getVisiblePathNumbers(SgNode* node) const
{
    CFGVertex cfgNode = getCFGNode(node);

    int idx;
    DAGVertex dagNode;
    boost::tie(idx, dagNode) = (vertexToDagIndex_.find(cfgNode))->second;

    return std::make_pair(idx,
                          pathNumGenerators_[idx]->getVisibleNumAndPaths(dagNode));
}

std::pair<int, PathSet> PathNumManager::getPathNumbers(
        SgNode* node1, SgNode* node2) const
{
    //cout << node1->unparseToString() << ' ' << node2->unparseToString() << endl;
    CFGVertex cfgNode1 = getCFGNode(node1);
    CFGVertex cfgNode2 = getCFGNode(node2);

    CFGEdge cfgEdge = boost::edge(cfgNode1, cfgNode2, *cfg_).first;
    ROSE_ASSERT(edgeToDagIndex_.count(cfgEdge) > 0);

    int idx;
    DAGEdge dagEdge;
    boost::tie(idx, dagEdge) = (edgeToDagIndex_.find(cfgEdge))->second;

    return std::make_pair(idx, pathNumGenerators_[idx]->getPaths(dagEdge));
}

void PathNumManager::getAstNodeIndices(size_t index, map<SgNode*, int>& nodeIndicesTable) const
{
    const DAG& dag = dags_[index];

    vector<DAGVertex> nodes;
    boost::topological_sort(dag, back_inserter(nodes));
    
    int num = 0;
    foreach (DAGVertex node, nodes)
    {
        SgNode* astNode = (*cfg_)[dag[node]]->getNode();
        nodeIndicesTable[astNode] = num++;
    }
    
    // Add a NULL entry to the table because it is possible that some VG node contains
    // a NULL AST node inside.
    nodeIndicesTable[NULL] = 0;
}

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

void PathNumManager::insertPathNumberToEvents(const string& pathNumName)
{
    ROSE_ASSERT(pathNumGenerators_.size() == dags_.size());

    SgFunctionDefinition* funcDef =
            isSgFunctionDefinition((*cfg_)[cfg_->getEntry()]->getNode());
    ROSE_ASSERT(funcDef);

    // Insert the declaration of the path number in the front of forward function.
    //string pathNumName = "__num__";
    SgVariableDeclaration* pathNumDecl =
            SageBuilder::buildVariableDeclaration(
                pathNumName,
                SageBuilder::buildIntType(),
                SageBuilder::buildAssignInitializer(
                    SageBuilder::buildIntVal(0)));
    SageInterface::prependStatement(pathNumDecl, funcDef->get_body());

    //! Insert path number update statement on CFG edges.
    for (size_t i = 0, m = dags_.size(); i != m; ++i)
    {
        typedef map<DAGEdge, int>::value_type EdgeValuePair;
        foreach (const EdgeValuePair& edgeVal, pathNumGenerators_[i]->edgeValues_)
        {
            BackstrokeCFG::Edge cfgEdge = dags_[i][edgeVal.first];

            // If the edge value is 0, no updating.
            if (edgeVal.second == 0) continue;

            // Insert the path num update on the CFG edge.
            insertPathNumberOnEdge(cfgEdge, pathNumName, edgeVal.second);
        }
    }

    // In the forward event, push the path number variable to the stack just before
    // the exit of the function.
    
    // Build a push function call.
    SageBuilder::pushScopeStack(funcDef->get_body());
    SgExpression* pathNumVar = SageBuilder::buildVarRefExp(pathNumDecl);
    SgStatement* pushPathNumFuncCall = SageBuilder::buildExprStatement(
            buildPushFunctionCall(pathNumVar));
    SageBuilder::popScopeStack();
    
    // For each in edges to exit node of CFG, insert the push functin on that edge.
    foreach (BackstrokeCFG::Vertex cfgNode, 
            boost::inv_adjacent_vertices(cfg_->getExit(), *cfg_))
    {
        SgNode* astNode = (*cfg_)[cfgNode]->getNode();
        
        // For each return statement, insert the push function before it.
        if (SgReturnStmt* returnStmt = isSgReturnStmt(astNode))
        {
            SgStatement* pushFuncCall = SageInterface::copyStatement(pushPathNumFuncCall);
            SageInterface::insertStatementBefore(returnStmt, pushFuncCall);
        }
    }
    SageInterface::appendStatement(pushPathNumFuncCall, funcDef->get_body());
    
    SageInterface::fixVariableReferences(funcDef);
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
    using namespace SageBuilder;
    SgStatement* pathNumStmt = buildExprStatement(
                                    buildPlusAssignOp(
                                        buildVarRefExp(pathNumName),
                                        buildIntVal(val)));
    
    SgNode* src = (*cfg_)[cfgEdge]->source().getNode();
    SgNode* tgt = (*cfg_)[cfgEdge]->target().getNode();

    if (SgIfStmt* ifStmt = isSgIfStmt(src))
    {
        if (SageInterface::isAncestor(src, tgt))
        {
            SgStatement* s = SageInterface::getEnclosingStatement(tgt);
            if (SgBasicBlock* body = isSgBasicBlock(s))
            {
                SageInterface::prependStatement(pathNumStmt, body);
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
        if (SgCaseOptionStmt* caseStmt = isSgCaseOptionStmt(tgt))
        {
            if (SgBasicBlock* body = isSgBasicBlock(caseStmt->get_body()))
            {
                SageInterface::prependStatement(pathNumStmt, body);
                
                // A trick to prevent that the previous case option does not have a break.
                SgStatement* pathNumStmt2 = buildExprStatement(
                                                buildMinusAssignOp(
                                                    buildVarRefExp(pathNumName),
                                                    buildIntVal(val)));

                SageInterface::insertStatementBefore(caseStmt, pathNumStmt2);
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
                SageInterface::prependStatement(pathNumStmt, body);
                
                // A trick to prevent that the previous case option does not have a break.
                SgStatement* pathNumStmt2 = buildExprStatement(
                                                buildMinusAssignOp(
                                                    buildVarRefExp(pathNumName),
                                                    buildIntVal(val)));

                SageInterface::insertStatementBefore(defaultStmt, pathNumStmt2);
            }
            else
            {
                ROSE_ASSERT(!"The target of the edge from a switch statement is not a basic block!");
            }
        }
        else if (!SageInterface::isAncestor(src, tgt))
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

        pathNum = 0;
        foreach (const Edge& e, boost::out_edges(v, dag_))
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
        pathsForNode_[v].allPath.resize(pathNumber);
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

            PathSetOnVertex& pathSetOnVertex = pathsForNode_[tar];

            if (pathSetOnVertex.numToPath.count(visibleNum) == 0)
                pathSetOnVertex.numToPath[visibleNum].resize(pathNumber);
            pathSetOnVertex.numToPath[visibleNum].set(i);

            pathSetOnVertex.allPath.set(i);
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
