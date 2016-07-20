// Example how to use graph isomorphism
#include <Sawyer/Graph.h>
#include <Sawyer/GraphAlgorithm.h>
#include <boost/foreach.hpp>

using namespace Sawyer::Container::Algorithm;

// A graph with no user data, only connectivity info.
typedef Sawyer::Container::Graph<> MyGraph;

static void
printSolution(const MyGraph &g1, const std::vector<size_t> &g1VertIds,
              const MyGraph &g2, const std::vector<size_t> &g2VertIds) {
    std::cout <<"  solution graph has " <<g1.nVertices() <<" vertices\n";
    std::cout <<"    vertex pairs:";
    for (size_t i=0; i<g1VertIds.size(); ++i)
        std::cout <<" (" <<g1VertIds[i] <<"," <<g2VertIds[i] <<")";
    std::cout <<"\n";

    // We can also turn the vertex vector representation into a true graph
    // so we can more easily operate on it.
    MyGraph subgraph2 = graphCopySubgraph(g2, g2VertIds);
    std::cout <<"    subgraph2 has " <<subgraph2.nEdges() <<" edges"
              <<" and is " <<(graphIsConnected(subgraph2) ? "" : "not ") <<"connected\n";
}

int main() {
    // Ensure diagnostics are initialized
    Sawyer::initializeLibrary();

    // Create a couple graphs by inserting vertices and connecting them with edges.
    // This is just one of many ways to do this.
    MyGraph g1;
    for (size_t i=0; i<5; ++i)
        g1.insertVertex();
    g1.insertEdge(g1.findVertex(0), g1.findVertex(1));
    g1.insertEdge(g1.findVertex(1), g1.findVertex(2));
    g1.insertEdge(g1.findVertex(1), g1.findVertex(3));
    g1.insertEdge(g1.findVertex(2), g1.findVertex(3));
    g1.insertEdge(g1.findVertex(3), g1.findVertex(4));

    MyGraph g2;
    for (size_t i=0; i<7; ++i)
        g2.insertVertex();
    g2.insertEdge(g2.findVertex(0), g2.findVertex(1));
    g2.insertEdge(g2.findVertex(1), g2.findVertex(2));
    g2.insertEdge(g2.findVertex(1), g2.findVertex(3));
    g2.insertEdge(g2.findVertex(3), g2.findVertex(2));
    g2.insertEdge(g2.findVertex(2), g2.findVertex(4));
    g2.insertEdge(g2.findVertex(4), g2.findVertex(5));
    g2.insertEdge(g2.findVertex(4), g2.findVertex(6));
    g2.insertEdge(g2.findVertex(6), g2.findVertex(5));

    // Do some operations

    std::cout <<"Subgraphs of g2 that are isomorphic to g1\n";
    findIsomorphicSubgraphs(g1, g2, printSolution);

    std::cout <<"Subgraphs of both g1 and g2 that are isomorphic to each other\n";
    findCommonIsomorphicSubgraphs(g1, g2, printSolution);

    std::cout <<"Largest subgraphs of both g1 and g2 that are isomorphic\n";
    typedef std::vector<size_t> VertexIds;
    typedef std::pair<VertexIds, VertexIds> Solution;
    typedef std::vector<Solution> Solutions;
    Solutions solns = findMaximumCommonIsomorphicSubgraphs(g1, g2);
    BOOST_FOREACH (const Solution &soln, solns)
        printSolution(g1, soln.first, g2, soln.second);
}
