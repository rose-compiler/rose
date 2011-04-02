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
    foreach (DAGVertex v, boost::vertices(cfg_))
    {
        SgNode* node = cfg_[v]->getNode();
        nodeCFGVertexMap_[node] = v;
    }
}

void PathNumManager::generatePathNumbers()
{
    dags_.push_back(DAG());
    DAG& dag = dags_[0];

    foreach (DAGVertex v, boost::vertices(cfg_))
    {
        DAGVertex dagNode = boost::add_vertex(dag);
        dag[dagNode] = v;
        vertexToDagIndex_[v] = make_pair(0, dagNode);
    }

    foreach (const DAGEdge& e, boost::edges(cfg_))
    {
        ROSE_ASSERT(vertexToDagIndex_.count(boost::source(e, cfg_)) > 0);
        ROSE_ASSERT(vertexToDagIndex_.count(boost::target(e, cfg_)) > 0);

        DAGEdge edge = boost::add_edge(
                vertexToDagIndex_[boost::source(e, cfg_)].second,
                vertexToDagIndex_[boost::target(e, cfg_)].second, dag).first;
        dag[edge] = e;
    }

    DAGVertex entry = vertexToDagIndex_[cfg_.getEntry()].second;
    DAGVertex exit = vertexToDagIndex_[cfg_.getExit()].second;

    foreach (const DAG& dag, dags_)
    {
        PathNumGenerator* pathNumGen = new PathNumGenerator(dag, entry, exit);
        pathNumGen->getEdgeValues();
        pathNumGen->getAllPaths();
        pathNumGen->getAllPathNumbersForEachNode();

        pathNumGenerators_.push_back(pathNumGen);
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

    BackstrokeCFG::Vertex cfgNode = nodeCFGVertexMap_.find(node)->second;
    ROSE_ASSERT(vertexToDagIndex_.count(cfgNode) > 0);

    int idx;
    DAGVertex dagNode;
    boost::tie(idx, dagNode) = (vertexToDagIndex_.find(cfgNode))->second;

    return std::make_pair(idx, pathNumGenerators_[idx]->getPaths(dagNode));
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

#if 1
        cout << val << " : ";
        foreach (const Edge& e, paths_[val])
            cout << e << ' ';
        cout << '\n';
#endif
    }
}

void PathNumGenerator::getAllPathNumbersForEachNode()
{
    int pathNumber = pathNumbers_[entry_];
    foreach (Vertex v, boost::vertices(dag_))
        pathsForNode_[v].resize(pathNumber);

    int i = 0;
    foreach (const Path& path, paths_)
    {
        foreach (const Edge& edge, path)
            pathsForNode_[boost::target(edge, dag_)][i] = 1;
        ++i;
    }

    // For each node, sort the paths it has.
    typedef map<Vertex, PathSet>::value_type VV;
    foreach (VV& vv, pathsForNode_)
    {
#if 1
        cout << vv.first << " : " << vv.second << "\n";
#endif
    }
}


} // end of Backstroke
