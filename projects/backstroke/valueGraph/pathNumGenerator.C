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

std::pair<int, PathSet> PathNumManager::getPathNumbers(SgNode* node) const
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

    int idx;
    DAGVertex dagNode;
    boost::tie(idx, dagNode) = (vertexToDagIndex_.find(cfgNode))->second;

    return std::make_pair(idx, pathNumGenerators_[idx]->getPaths(dagNode));
}

std::pair<int, PathSet> PathNumManager::getPathNumbers(
        SgNode* node1, SgNode* node2) const
{
    // Trace this node up to find which CFG vertex it belongs to.
    while (node1)
    {
        if (nodeCFGVertexMap_.find(node1) != nodeCFGVertexMap_.end())
            break;
        node1 = node1->get_parent();
    }
    while (node2)
    {
        if (nodeCFGVertexMap_.find(node2) != nodeCFGVertexMap_.end())
            break;
        node2 = node2->get_parent();
    }

    ROSE_ASSERT(node1 && node2);

    CFGVertex cfgNode1 = nodeCFGVertexMap_.find(node1)->second;
    CFGVertex cfgNode2 = nodeCFGVertexMap_.find(node2)->second;

    CFGEdge cfgEdge = boost::edge(cfgNode1, cfgNode2, cfg_).first;
    ROSE_ASSERT(edgeToDagIndex_.count(cfgEdge) > 0);

    int idx;
    DAGEdge dagEdge;
    boost::tie(idx, dagEdge) = (edgeToDagIndex_.find(cfgEdge))->second;

    return std::make_pair(idx, pathNumGenerators_[idx]->getPaths(dagEdge));
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

        foreach (const Edge& e, boost::out_edges(node, dag_))
        {
            Vertex tar = boost::target(e, dag_);
            Path path = pathsOnVertex[node];
            path.push_back(e);
            pathsOnVertex[tar].swap(path);
            
            nodes.push(tar);
        }

        if (node == exit_)
            paths.push_back(pathsOnVertex[node]);
    }

    paths_.resize(paths.size());

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
        pathsForNode_[v].resize(pathNumber);
    foreach (const Edge& e, boost::edges(dag_))
        pathsForEdge_[e].resize(pathNumber);

    int i = 0;
    foreach (const Path& path, paths_)
    {
        foreach (const Edge& edge, path)
        {
            pathsForNode_[boost::target(edge, dag_)][i] = 1;
            pathsForEdge_[edge][i] = 1;
        }
        ++i;
    }
}


} // end of Backstroke
