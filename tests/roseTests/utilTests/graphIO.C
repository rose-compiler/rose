#include "GraphUtility.h"
#include "StringUtility.h"
#include "Diagnostics.h"
#include <sawyer/CommandLine.h>
#include <iostream>
#include <sstream>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Vertex and Edge value types
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Some arbitrary class type that's stored at each vertex of a graph.
struct VertexValue {
    int i1, i2;
    std::string name;

    // Default c'tor is required
    VertexValue()
        : i1(0), i2(0) {}

    // This c'tor is optional and only used in our testing.
    VertexValue(int i1, int i2, const std::string &name)
        : i1(i1), i2(i2), name(name) {}

    // Required; called by rose::GraphUtility::serialize()
    void serialize(std::ostream &output) const {
        output.write((const char*)&i1, sizeof i1);
        output.write((const char*)&i2, sizeof i2);
        size_t n = name.size();
        output.write((const char*)&n, sizeof n);
        output.write(name.data(), name.size());
    }

    // Required; called by rose::GraphUtility::deserialize()
    void deserialize(std::istream &input) {
        input.read((char*)&i1, sizeof i1);
        input.read((char*)&i2, sizeof i2);
        size_t n = 0;
        input.read((char*)&n, sizeof n);
        char *buf = new char[n];
        input.read(buf, n);
        name = std::string(buf, n);
        delete[] buf;
    }

    // Only used for our testing
    bool operator==(const VertexValue &other) const {
        return i1==other.i1 && i2==other.i2 && name==other.name;
    }
};

// Some arbitrary class type that's stored at each edge of the graph.
struct EdgeValue {
    double weight;

    // Required
    EdgeValue()
        : weight(0.0) {}

    // Optional; only used in our testing
    EdgeValue(double weight)
        : weight(weight) {}

    // Required by rose::GraphUtility::serialize
    void serialize(std::ostream &output) const {
        output.write((const char*)&weight, sizeof weight);
    }

    // Required by rose::GraphUtility::deserialize
    void deserialize(std::istream &input) {
        input.read((char*)&weight, sizeof weight);
    }

    // Only used for our testing. FP equality is desired here (must be bit-for-bit equal)
    bool operator==(const EdgeValue &other) const {
        return weight==other.weight;
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Printing, only used for our own debugging
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::ostream& operator<<(std::ostream &output, const VertexValue &vertex) {
    output <<vertex.i1 <<"\t" <<vertex.i2 <<"\t" <<vertex.name;
    return output;
}

std::ostream& operator<<(std::ostream &output, const EdgeValue &edge) {
    output <<edge.weight;
    return output;
}

template<class V, class E>
std::ostream& operator<<(std::ostream &output, const Sawyer::Container::Graph<V, E> &graph) {
    output <<"  vertices:\n";
    for (size_t i=0; i<graph.nVertices(); ++i)
        output <<"    [" <<i <<"]\t" <<graph.findVertex(i)->value() <<"\n";
    output <<"  edges:\n";
    for (size_t i=0; i<graph.nEdges(); ++i) {
        typename Sawyer::Container::Graph<V, E>::ConstEdgeIterator edge = graph.findEdge(i);
        output <<"    [" <<i <<"]\t" <<edge->source()->id() <<"\t" <<edge->target()->id() <<"\t" <<edge->value() <<"\n";
    }
    return output;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      The test: create graph, serialize, deserialize
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<class V, class E>
void buildGraph(Sawyer::Container::Graph<V, E> &graph, size_t nverts, size_t nedges) {
    typedef typename Sawyer::Container::Graph<V, E>::VertexIterator Vertex;
    graph.clear();
    std::vector<Vertex> verts;
    for (size_t i=0; i<nverts; ++i)
        verts.push_back(graph.insertVertex(VertexValue(100+2*i, 100+2*i+1, "vert "+StringUtility::numberToString(i))));
    for (size_t i=0; i<nedges; ++i) {
        Vertex src = verts[rand() % verts.size()];
        Vertex tgt = verts[rand() % verts.size()];
        graph.insertEdge(src, tgt, i+3.14);
    }
}

template<class V, class E>
void checkGraphs(const Sawyer::Container::Graph<V, E> &a, const Sawyer::Container::Graph<V, E> &b) {
    typedef typename Sawyer::Container::Graph<V, E>::ConstVertexIterator Vertex;
    typedef typename Sawyer::Container::Graph<V, E>::ConstEdgeIterator Edge;

    ASSERT_always_require(a.nVertices()==b.nVertices());
    ASSERT_always_require(a.nEdges()==b.nEdges());

    for (size_t i=0; i<a.nVertices(); ++i)
        ASSERT_always_require(a.findVertex(i)->value() == b.findVertex(i)->value());
    for (size_t i=0; i<a.nEdges(); ++i) {
        Edge ea = a.findEdge(i);
        Edge eb = b.findEdge(i);
        ASSERT_always_require(ea->source()->id() == eb->source()->id());
        ASSERT_always_require(ea->target()->id() == eb->target()->id());
        ASSERT_always_require(ea->value() == eb->value());
    }
}

static void showHelpAndExit(const Sawyer::CommandLine::ParserResult &cmdline) {
    cmdline.parser().emitDocumentationToPager();
    exit(0);
}

int main(int argc, char *argv[]) {
    using namespace rose;
    using namespace rose::Diagnostics;
    using namespace StringUtility;

    typedef Sawyer::Container::Graph<VertexValue, EdgeValue> Graph;
    typedef Graph::VertexIterator Vertex;
    typedef Graph::EdgeIterator Edge;

    Diagnostics::initialize();

    size_t nverts = 10000;
    size_t nedges = 10000000;
    bool showGraph = false;

    // Declare command-line switches
    Sawyer::CommandLine::SwitchGroup switches;
    switches.insert(Sawyer::CommandLine::Switch("help", 'h')
                    .action(userAction(showHelpAndExit))
                    .doc("Show this man page and exit."));
    switches.insert(Sawyer::CommandLine::Switch("nverts", 'V')
                    .argument("n", Sawyer::CommandLine::nonNegativeIntegerParser(nverts))
                    .doc("Create graphs with @v{n} vertices.  The default is " + plural(nverts, "vertices", "vertex") + "."));
    switches.insert(Sawyer::CommandLine::Switch("nedges", 'E')
                    .argument("n", Sawyer::CommandLine::nonNegativeIntegerParser(nedges))
                    .doc("Create graphs with @v{n} edges.  The default is " + plural(nedges, "edges") + "."));
    switches.insert(Sawyer::CommandLine::Switch("debug", 'd')
                    .intrinsicValue(true, showGraph)
                    .doc("Show the graph contents."));

    // Program documentation and commnad-line processing
    Sawyer::CommandLine::Parser parser;
    parser.with(switches);
    parser.purpose("tests graph serialization and deserialization");
    parser.doc("Description",
               "Creates a Sawyer graph with the specified number of vertices and edges. The edges are connected to "
               "randomly selected vertices.  Then uses sage::GraphUtility::serialize to serialize the graph to a "
               "string, then deserializes the string to create a new graph, then tests that both graphs are identical.");
    Sawyer::CommandLine::ParserResult cmdline = parser.parse(argc, argv).apply();
    ASSERT_always_require2(cmdline.unreachedArgs().empty(), "invalid program arguments; see --help");
    mlog[DEBUG].enable(showGraph);

    // Build and serialize the inital graph
    mlog[INFO] <<"building the initial graph: " <<plural(nverts, "vertices", "vertex") <<" and " <<plural(nedges, "edges") <<"\n";
    Graph g1;
    buildGraph(g1, nverts, nedges);
    SAWYER_MESG(mlog[DEBUG]) <<"Original graph:\n" <<g1;
    mlog[INFO] <<"serializing original graph\n";
    std::ostringstream oss;
    GraphUtility::serialize(oss, g1);
    mlog[INFO] <<"serialized to " <<plural(oss.str().size(), "bytes") <<"\n";

    // De-serialize to create a second graph
    mlog[INFO] <<"creating a new graph by de-serializing\n";
    Graph g2;
    std::istringstream iss(oss.str());
    GraphUtility::deserialize(iss, g2);
    ASSERT_always_require2(g2.nVertices()==nverts, "wrong number of vertices deserialized");
    ASSERT_always_require2(g2.nEdges()==nedges, "wrong number of edges deserialized");
    SAWYER_MESG(mlog[DEBUG]) <<"Reconstructed graph:\n" <<g2;

    // Check that both graphs are the same
    mlog[INFO] <<"comparing new graph with original\n";
    checkGraphs(g1, g2);
}
