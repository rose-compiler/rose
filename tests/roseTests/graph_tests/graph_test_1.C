// Test code to demonstrate the construction of graphs using the ROSE Graph IR nodes, or Sawyer::Container::Graph

#include "rose.h"
#include <boost/lexical_cast.hpp>
#include <Sawyer/CommandLine.h>
#include <Sawyer/Graph.h>

using namespace std;
using namespace boost;
using namespace StringUtility;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Graph types
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// ROSE Graph approach requires subclassing.  Vertex and edge names are built-in as part of the interface.
class MySageGraph {
public:
    SgIncidenceUndirectedGraph graph;
#ifdef ROSE_USING_GRAPH_IR_NODES_FOR_BACKWARD_COMPATABILITY
    typedef SgGraphNodeList::iterator node_iterator;
    typedef SgGraphEdgeList::iterator edge_iterator;
#endif
    MySageGraph( string name ) : graph(name) {}
    SgGraphNode* addNode( const string & name = "") {
        return graph.addNode(name);
    }
    SgGraphEdge* addEdge( SgGraphNode* x, SgGraphNode* y, const string & name = "") {
        return graph.addEdge(x, y, name);
    }
};

// Sawyer::Container::Graph approach is more STL-like in that vertex and edge types are specified by template arguments.
typedef Sawyer::Container::Graph<std::string /*vertex*/, std::string /*edge*/> MySawyerGraph;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Graph information
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void print_sage_graph_info() {
    printf("Sizes of Graph IR nodes in ROSE:\n");
    printf("  Size of SgGraphNode                                = %zu\n", sizeof(SgGraphNode));
    printf("  Size of SgGraphNodeList                            = %zu\n", sizeof(SgGraphNodeList));

    printf("  Size of SgGraphEdge                                = %zu\n", sizeof(SgGraphEdge));
    printf("  Size of SgDirectedGraphEdge                        = %zu\n", sizeof(SgDirectedGraphEdge));
    printf("  Size of SgUndirectedGraphEdge                      = %zu\n", sizeof(SgUndirectedGraphEdge));
    printf("  Size of SgGraphEdgeList                            = %zu\n", sizeof(SgGraphEdgeList));

    printf("  Size of SgGraph                                    = %zu\n", sizeof(SgGraph));
    printf("  Size of SgIncidenceDirectedGraph                   = %zu\n", sizeof(SgIncidenceDirectedGraph));
    printf("  Size of SgIncidenceUndirectedGraph                 = %zu\n", sizeof(SgIncidenceUndirectedGraph));
    printf("  Size of SgBidirectionalGraph                       = %zu\n", sizeof(SgBidirectionalGraph));
    printf("  Size of SgStringKeyedBidirectionalGraph            = %zu\n", sizeof(SgStringKeyedBidirectionalGraph));
    printf("  Size of SgIntKeyedBidirectionalGraph               = %zu\n", sizeof(SgIntKeyedBidirectionalGraph));

    printf("  Size of SgNode                                     = %zu\n", sizeof(SgNode));
    printf("  Size of SgSupport                                  = %zu\n", sizeof(SgSupport));
    printf("  Size of std::string                                = %zu\n", sizeof(std::string));
    printf("  Size of property map: std::map< int, std::string > = %zu\n", sizeof(std::map< int, std::string >));
    printf("  Size of rose_graph_hash_multimap                   = %zu\n", sizeof(rose_graph_hash_multimap));
    // printf ("Size of rose_directed_graph_hash_multimap          = %zu \n",sizeof(rose_directed_graph_hash_multimap));
    // printf ("Size of rose_undirected_graph_hash_multimap        = %zu \n",sizeof(rose_undirected_graph_hash_multimap));
    printf ("Size of rose_graph_node_edge_hash_multimap          = %zu\n", sizeof(rose_graph_node_edge_hash_multimap));

    printf("Size of AstAttributeMechanism (but we use a pointer) = %zu\n", sizeof(AstAttributeMechanism));
}

static void print_sawyer_graph_info() {
    printf("Sizes of Graph types in Sawyer:\n");
    printf("  Size of Graph                                      = %zu\n", sizeof(MySawyerGraph));
    printf("  Size of Graph::VertexValue (std::string)           = %zu\n", sizeof(MySawyerGraph::VertexValue));
    printf("  Size of Graph::EdgeValue (std::string)             = %zu\n", sizeof(MySawyerGraph::EdgeValue));
    printf("  Size of Graph::Vertex (the storage node)           = %zu\n", sizeof(MySawyerGraph::Vertex));
    printf("  Size of Graph::Edge (the storage node)             = %zu\n", sizeof(MySawyerGraph::Edge));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Tests for each graph type
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void test_sage_graph(size_t nverts, bool skipEdges) {
    // Build a graph using the new graph IR nodes in ROSE.
    // SgIncidenceUndirectedGraph graph ("demo graph");

    MySageGraph graph("demo graph");

    // Reset the size of the internal hash tables.
    graph.graph.resize_hash_maps(1000,1000000);

    printf ("Adding %zu vertices\n", nverts);
    SgGraphNode **nodes = new SgGraphNode*[nverts];
    for (size_t i=0; i < nverts; i++) {
        nodes[i] = new SgGraphNode();
        graph.graph.addNode(nodes[i]);                  // bypassing class so we don't need to provide a label?
    }

    if (!skipEdges) {
        printf("Adding edges\n");
        for (size_t i=0; i < nverts; i++) {
            for (size_t j=0; j < i; j++) {
                graph.addEdge(nodes[i],nodes[j],
                              "edge "+lexical_cast<string>(i)+"-"+lexical_cast<string>(j));
            }
        }
    }

    // Statistics
    size_t numberOfGraphNodes = graph.graph.numberOfGraphNodes();
    size_t numberOfGraphEdges = graph.graph.numberOfGraphEdges();
    size_t memoryUsage = graph.graph.memory_usage();
    std::cout <<"Graph size: " <<plural(numberOfGraphNodes, "vertices", "vertex")
              <<" and " <<plural(numberOfGraphEdges, "edges") <<"\n";
    std::cout <<"Approximate memory used: " <<plural(memoryUsage, "bytes") <<"\n";
}

static void test_sawyer_graph(size_t nverts, bool skipEdges) {
    MySawyerGraph graph;

    printf("Adding %zu vertices\n", nverts);
    for (size_t i=0; i<nverts; ++i)
        graph.insertVertex();   // don't provide a label, for consistency with Sage test above

    if (!skipEdges) {
        printf("Adding edges\n");
        for (size_t i=0; i<nverts; ++i) {
            for (size_t j=0; j<i; ++j) {
                graph.insertEdge(graph.findVertex(i), graph.findVertex(j),
                                 "edge "+lexical_cast<string>(i)+"-"+lexical_cast<string>(j));
            }
        }
    }

    // Statistics
    std::cout <<"Graph size: " <<plural(graph.nVertices(), "vertices", "vertex")
              <<" and " <<plural(graph.nEdges(), "edges") <<"\n";
    size_t memoryUsage = sizeof(MySawyerGraph) +
                         graph.nVertices()* sizeof(MySawyerGraph::Vertex) +
                         graph.nEdges() * sizeof(MySawyerGraph::Edge);
    std::cout <<"Approximate memory used: " <<plural(memoryUsage, "bytes") <<"\n";
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Main program
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


static void showHelpAndExit(const Sawyer::CommandLine::ParserResult &cmdline) {
    cmdline.parser().emitDocumentationToPager();
    exit(0);
}

int main(int argc, char *argv[]) {

    size_t nverts = 1000;                               // number of vertices to insert
    bool skipEdges = false;                             // if true, then skip the step that inserts edges
    
    // Parse command-line
    using Sawyer::CommandLine::Switch;
    Sawyer::CommandLine::SwitchGroup switches;
    switches.insert(Switch("help", 'h')
                    .action(userAction(showHelpAndExit))
                    .doc("Shows this documentation."));
    switches.insert(Switch("nverts", 'v')
                    .argument("n", Sawyer::CommandLine::integerParser(nverts))
                    .doc("Populate the graph with @v{n} vertices. The default is " + lexical_cast<string>(nverts) + "."));
    switches.insert(Switch("no-edges")
                    .intrinsicValue(true, skipEdges)
                    .doc("Do not add edges to the graph.  The default, when this switch is not present, is to add a "
                         "directed edge from vertex i to vertex j for all i<j."));
    Sawyer::CommandLine::Parser parser;
    parser.with(switches);
    parser.purpose("tests vertex and edge insertion for Sage vs. Sawyer graphs");
    parser.doc("Synopsis", "@prop{programName} [@v{switches}...] sage|sawyer...");
    Sawyer::CommandLine::ParserResult cmdline = parser.parse(argc, argv).apply();

    if (cmdline.unparsedArgs().size() != 1) {
        std::cerr <<"incorrect usage; see --help\n";
        exit(1);
    }
    bool use_sage = 0==cmdline.unparsedArgs().front().compare("sage");
    
    if (use_sage) {
        print_sage_graph_info();
        test_sage_graph(nverts, skipEdges);
    } else {
        print_sawyer_graph_info();
        test_sawyer_graph(nverts, skipEdges);
    }

    return 0;
}
