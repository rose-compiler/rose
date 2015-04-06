#ifndef ROSE_GraphUtility_H
#define ROSE_GraphUtility_H

#include <boost/cstdint.hpp>
#include <sawyer/Graph.h>

namespace rose {

/** Graph utilities. */
namespace GraphUtility {

/** Serialize a graph into a stream of bytes. */
template<class Graph>
void
serialize(std::ostream &output, Graph &graph)
{
    boost::uint32_t n = 0;
    output.write((const char*)&n, 4);

    n = graph.nVertices();
    output.write((const char*)&n, 4);

    n = graph.nEdges();
    output.write((const char*)&n, 4);

    for (size_t i=0; i<graph.nVertices(); ++i) {
        typename Graph::ConstVertexIterator vertex = graph.findVertex(i);
        vertex->value().serialize(output);
    }

    for (size_t i=0; i<graph.nEdges(); ++i) {
        typename Graph::ConstEdgeIterator edge = graph.findEdge(i);
        n = edge->source()->id();
        output.write((const char*)&n, 4);
        n = edge->target()->id();
        output.write((const char*)&n, 4);
        edge->value().serialize(output);
    }
}

template<class Graph>
void
deserialize(std::istream &input, Graph &graph)
{
    graph.clear();
    boost::uint32_t n;

    input.read((char*)&n, 4);
    ASSERT_require2(n==0, "incorrect version number");

    input.read((char*)&n, 4);
    size_t nverts = n;

    input.read((char*)&n, 4);
    size_t nedges = n;

    for (size_t i=0; i<nverts; ++i) {
        typename Graph::VertexIterator vertex = graph.insertVertex();
        ASSERT_require2(vertex->id()==i, "unexpected vertex numbering");
        vertex->value().deserialize(input);
    }

    for (size_t i=0; i<nedges; ++i) {
        input.read((char*)&n, 4);
        typename Graph::VertexIterator source = graph.findVertex(n);
        input.read((char*)&n, 4);
        typename Graph::VertexIterator target = graph.findVertex(n);
        typename Graph::EdgeIterator edge = graph.insertEdge(source, target);
        ASSERT_require2(edge->id()==i, "unexpected edge numbering");
        edge->value().deserialize(input);
    }
}

} // namespace
} // namespace
#endif
