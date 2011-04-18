#include "pathNumGenerator.h"

#include <sageInterface.h>
#include <boost/foreach.hpp>
#include <boost/graph/topological_sort.hpp>
#include <boost/tuple/tuple.hpp>


namespace Backstroke
{

using namespace std;

#define foreach BOOST_FOREACH


PathNumManager::PathNumManager(const BackstrokeCFG& cfg)
    : cfg_(cfg)
{
    generatePathNumbers();
    buildNodeCFGVertexMap();
}

void PathNumManager::buildNodeCFGVertexMap()
{
    foreach (CFGVertex v, boost::vertices(cfg_))
    {
        SgNode* node = cfg_[v]->getNode();
        nodeCFGVertexMap_[node] = v;
    }
}

void PathNumManager::generatePathNumbers()
{
    dags_.push_back(DAG());
    DAG& dag = dags_[0];

    foreach (CFGVertex v, boost::vertices(cfg_))
    {
        DAGVertex dagNode = boost::add_vertex(dag);
        dag[dagNode] = v;
        vertexToDagIndex_[v] = make_pair(0, dagNode);
    }

    foreach (const CFGEdge& e, boost::edges(cfg_))
    {
        ROSE_ASSERT(vertexToDagIndex_.count(boost::source(e, cfg_)) > 0);
        ROSE_ASSERT(vertexToDagIndex_.count(boost::target(e, cfg_)) > 0);

        DAGEdge dagEdge = boost::add_edge(
                vertexToDagIndex_[boost::source(e, cfg_)].second,
                vertexToDagIndex_[boost::target(e, cfg_)].second, dag).first;
        dag[dagEdge] = e;
        edgeToDagIndex_[e] = make_pair(0, dagEdge);
    }

    DAGVertex entry = vertexToDagIndex_[cfg_.getEntry()].second;
    DAGVertex exit  = vertexToDagIndex_[cfg_.getExit()].second;

    // For each DAG, generate its path information.
    foreach (const DAG& dag, dags_)
    {
        PathNumGenerator* pathNumGen = new PathNumGenerator(dag, entry, exit);
        pathNumGen->generatePathNumbers();

        pathNumGenerators_.push_back(pathNumGen);

        int parentIdx = 0;
        pathInfo_.push_back(make_pair(parentIdx, pathNumGen->getPathNum()));
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
    // If the given node is a data member of a class, set its CFG node to the entry.
    if (isDataMember(node))
        cfgNode = cfg_.getEntry();
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
    CFGVertex cfgNode1 = getCFGNode(node1);
    CFGVertex cfgNode2 = getCFGNode(node2);

    CFGEdge cfgEdge = boost::edge(cfgNode1, cfgNode2, cfg_).first;
    ROSE_ASSERT(edgeToDagIndex_.count(cfgEdge) > 0);

    int idx;
    DAGEdge dagEdge;
    boost::tie(idx, dagEdge) = (edgeToDagIndex_.find(cfgEdge))->second;

    return std::make_pair(idx, pathNumGenerators_[idx]->getPaths(dagEdge));
}

void PathNumManager::instrumentFunction()
{
    ROSE_ASSERT(pathNumGenerators_.size() == dags_.size());

    SgFunctionDefinition* funcDef =
            isSgFunctionDefinition(cfg_[cfg_.getEntry()]->getNode());
    ROSE_ASSERT(funcDef);

    string pathNumName = "__num__";
    SgVariableDeclaration* pathNumDecl =
            SageBuilder::buildVariableDeclaration(
                pathNumName,
                SageBuilder::buildIntType(),
                SageBuilder::buildAssignInitializer(
                    SageBuilder::buildIntVal(0)));
    SageInterface::prependStatement(pathNumDecl, funcDef->get_body());

    for (size_t i = 0, m = dags_.size(); i != m; ++i)
    {
        typedef map<DAGEdge, int>::value_type EdgeValuePair;
        foreach (const EdgeValuePair& edgeVal, pathNumGenerators_[i]->edgeValues_)
        {
            BackstrokeCFG::Edge cfgEdge = dags_[i][edgeVal.first];
            insertPathNumOnEdge(cfgEdge, edgeVal.second);
        }
    }

    SageInterface::fixVariableReferences(funcDef);
}

namespace 
{
    SgStatement* getBelongedStatement(SgNode* node)
    {
        SgStatement* stmt;
        while (!(stmt = isSgStatement(node)))
        {
            node = node->get_parent();
            if (node == NULL)
                return NULL;
        }
        return stmt;
    }

} // end of anonymous

void PathNumManager::insertPathNumOnEdge(const BackstrokeCFG::Edge& cfgEdge, int val)
{
    if (val == 0) return;
    
    using namespace SageBuilder;

    string pathNumName = "__num__";
    SgStatement* pathNumStmt = buildExprStatement(
                                    buildPlusAssignOp(
                                        buildVarRefExp(pathNumName),
                                        buildIntVal(val)));

    SgNode* src = cfg_[cfgEdge]->source().getNode();
    SgNode* tgt = cfg_[cfgEdge]->target().getNode();

    if (SgIfStmt* ifStmt = isSgIfStmt(src))
    {
        if (SageInterface::isAncestor(src, tgt))
        {
            SgStatement* stmt = getBelongedStatement(tgt);
            SageInterface::insertStatementBefore(stmt, pathNumStmt);
        }
        else
        {
            ROSE_ASSERT(ifStmt->get_false_body() == NULL);
            // This edge must represent false body here.
            SgBasicBlock* falseBody = buildBasicBlock(pathNumStmt);
            ifStmt->set_false_body(falseBody);
            falseBody->set_parent(ifStmt);
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
        // If this node is a leaf.
        if (boost::out_degree(v, dag_) == 0)
        {
            pathNumbers_[v] = 1;
            continue;
        }

        pathNumbers_[v] = 0;
        foreach (const Edge& e, boost::out_edges(v, dag_))
        {
            Vertex tar = boost::target(e, dag_);
            ROSE_ASSERT(pathNumbers_.count(tar) > 0);
            ROSE_ASSERT(pathNumbers_[tar] > 0);
            
            edgeValues_[e] = pathNumbers_[v];
            pathNumbers_[v] += pathNumbers_[tar];
        }
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

    paths_.resize(paths.size());

    // Get the path number as the index of each path.
    foreach (Path& path, paths)
    {
        int val = 0;
        foreach (const Edge& e, path)
            val += edgeValues_[e];
        paths_[val].swap(path);
    }
}

void PathNumGenerator::getAllPathNumForNodesAndEdges()
{
    int pathNumber = pathNumbers_[entry_];
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
            pathSetOnVertex.numToPath[visibleNum][i] = 1;

            pathSetOnVertex.allPath[i] = 1;
            pathsForEdge_[edge][i] = 1;
        }
        ++i;
    }
}


} // end of Backstroke
