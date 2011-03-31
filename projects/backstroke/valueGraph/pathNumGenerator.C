#include "pathNumGenerator.h"

#include <boost/foreach.hpp>
#include <boost/graph/topological_sort.hpp>
#include <boost/tuple/tuple.hpp>


namespace Backstroke
{

using namespace std;

#define foreach BOOST_FOREACH


PathNum::PathNum(const BackstrokeCFG& cfg)
{
    DAG dag;

    map<BackstrokeCFG::Vertex, Vertex> verticesTable;

    BackstrokeCFG::VertexIter v, w;
    for (boost::tie(v, w) = boost::vertices(cfg); v != w; ++v)
    {
        Vertex node = boost::add_vertex(dag);
        dag[node] = *v;
        verticesTable[*v] = node;
    }

    BackstrokeCFG::EdgeIter e, f;
    for (boost::tie(e, f) = boost::edges(cfg); e != f; ++e)
    {
        ROSE_ASSERT(verticesTable.count(boost::source(*e, cfg)) > 0);
        ROSE_ASSERT(verticesTable.count(boost::target(*e, cfg)) > 0);

        Edge edge = boost::add_edge(
                verticesTable[boost::source(*e, cfg)],
                verticesTable[boost::target(*e, cfg)], dag).first;
        dag[edge] = *e;
    }

    Vertex entry = verticesTable[cfg.getEntry()];
    Vertex exit = verticesTable[cfg.getExit()];

    PathNumGenerator pathNumGen(dag, entry, exit);
    pathNumGen.getEdgeValues();
    pathNumGen.getAllPaths();
    pathNumGen.getAllPathNumbersForEachNode();
}




void PathNumGenerator::getEdgeValues()
{
    vector<Vertex> nodes;
    boost::topological_sort(dag_, std::back_inserter(nodes));

    // This algorithm is from "Ball T, Larus JR. Efficient Path Profiling."
    foreach (Vertex v, nodes)
    {
        OutEdgeIter e, f;
        boost::tie(e, f) = boost::out_edges(v, dag_);
        if (e == f)
        {
            pathNumbers_[v] = 1;
            continue;
        }

        pathNumbers_[v] = 0;
        for (; e != f; ++e)
        {
            Vertex tar = boost::target(*e, dag_);
            ROSE_ASSERT(pathNumbers_.count(tar) > 0);
            ROSE_ASSERT(pathNumbers_[tar] > 0);
            
            edgeValues_[*e] = pathNumbers_[v];
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

        OutEdgeIter e, f;
        for (boost::tie(e, f) = boost::out_edges(node, dag_); e != f; ++e)
        {
            Vertex tar = boost::target(*e, dag_);
            Path path = pathsOnVertex[node];
            path.push_back(*e);
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
    int i = 0;
    foreach (const Path& path, paths_)
    {
        foreach (const Edge& edge, path)
            pathsForNode_[boost::target(edge, dag_)].push_back(i);
        ++i;
    }

    // For each node, sort the paths it has.
    typedef map<Vertex, vector<int> >::value_type VV;
    foreach (VV& vv, pathsForNode_)
    {
        sort(vv.second.begin(), vv.second.end());
#if 1
        cout << vv.first << " : ";
        copy(vv.second.begin(), vv.second.end(), ostream_iterator<int>(cout, " "));
        cout << "\n";
#endif
    }
}


} // end of Backstroke
