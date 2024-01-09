static const char *purpose = "demonstrate how to create a CFG with return edges";
static const char *description =
    "ROSE global control flow graphs don't generally have edges from function return statements to all the places to which that "
    "function might return. Instead, the return statements point to an indeterminate vertex and the return-to information is "
    "stored at the call site as an E_CALL_RETURN edge. For instance, if function A calls function B then the call site in "
    "function A has two outgoing edges: an E_FUNCTION_CALL edge that points to the entry address of function B, and an "
    "E_CALL_RETURN edge that points to the address to which this function B call is expected to return. Within function B, there "
    "is an E_FUNCTION_RETURN edge from that function's return statements to the special indeterminate vertex.\n\n"

    "This tutorial shows how the default global CFG can be modified so that each return statement points to all the addresses to "
    "which the function is known to return. In essence, it removes all E_CALL_RETURN edges and replaces each E_FUNCTION_RETURN "
    "edge with a list of E_FUNCTION_RETURN edges that point to the locations to which the original E_CALL_RETURN edges pointed.";

#include <rose.h>                                       // must be first
#include <Rose/BinaryAnalysis/Partitioner2/BasicBlock.h>
#include <Rose/BinaryAnalysis/Partitioner2/ControlFlowGraph.h>
#include <Rose/BinaryAnalysis/Partitioner2/EngineBinary.h>
#include <Rose/BinaryAnalysis/Partitioner2/Function.h>
#include <Rose/BinaryAnalysis/Partitioner2/GraphViz.h>
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>
#include <Rose/StringUtility/NumberToString.h>

#include <boost/lexical_cast.hpp>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

using namespace Rose;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;

static std::string
vertexName(const P2::ControlFlowGraph::Vertex &vertex) {
    std::ostringstream ss;
    ss <<"vertex " <<vertex.id();
    if (const auto addr = vertex.value().optionalAddress())
        ss <<" at " <<StringUtility::addrToString(*addr);
    return ss.str();
}

static std::string
edgeName(const P2::ControlFlowGraph::Edge &edge) {
    std::ostringstream ss;
    ss <<"edge " <<edge.id() <<" from " <<vertexName(*edge.source()) <<" to " <<vertexName(*edge.target());
    return ss.str();
}

int main(int argc, char *argv[]) {
    ROSE_INITIALIZE;

    auto engine = P2::EngineBinary::instance();
    std::vector<std::string> specimen = engine->parseCommandLine(argc, argv, purpose, description).unreachedArgs();
    P2::Partitioner::Ptr partitioner = engine->partition(specimen);

    // Make a copy of the global control flow graph, which we will modify.
    P2::ControlFlowGraph cfg = partitioner->cfg();

    // A mapping from each function to all of the places to which it might return. It would have been nice to be able to store
    // vertex pointers (i.e., VertexIterator) here in the set, but unfortunately they don't have an operator< which is required
    // for sets. Instead, we'll store vertex ID numbers (size_t) and look up the vertex later when we need it.
    using ReturnToSites = std::map<P2::Function::Ptr, std::set<size_t>>;
    ReturnToSites returnToSites;

    // Find the function call sites by looking for call-return edges. These edges point from function call sites to the vertex to
    // which the call(s) are expected to return.
    for (P2::ControlFlowGraph::Edge &callRetEdge: cfg.edges()) {
        if (callRetEdge.value().type() == P2::E_CALL_RETURN) {
            std::cerr <<"found call-return " <<edgeName(callRetEdge) <<"\n";
            P2::ControlFlowGraph::VertexIterator callSite = callRetEdge.source(); // vertex from which a function(s) is called
            P2::ControlFlowGraph::VertexIterator retToSite = callRetEdge.target(); // vertex to which the calls will probably return

            // Now that we found the call sites, find the functions that the call site calls. The edges of type E_FUNCTION_CALL are
            // the ones pointing into called functions (usually just one such edge per call site). Each pointed-to vertex might be
            // owned by multiple functions (but usually just one). For each of the functions called from this call site, add this
            // `retToSite` to its set of return-to sites.
            for (P2::ControlFlowGraph::Edge &callEdge: callSite->outEdges()) {
                if (callEdge.value().type() == P2::E_FUNCTION_CALL) {
                    std::cerr <<"  call " <<edgeName(callEdge) <<"\n";
                    P2::ControlFlowGraph::VertexIterator callee = callEdge.target();
                    for (const P2::Function::Ptr &function: callee->value().owningFunctions().values()) {
                        std::cerr <<"    calls " <<function->printableName() <<"\n";
                        returnToSites[function].insert(retToSite->id());
                    }
                }
            }
        }
    }

    // Debugging
    for (const auto &entry: returnToSites) {
        std::cerr <<"call return-to sites for " <<entry.first->printableName() <<" are:\n";
        for (const size_t vertexId: entry.second)
            std::cerr <<"  " <<vertexName(*cfg.findVertex(vertexId)) <<"\n";
    }

    // Find all the function return edges that point to the indeterminate vertex and insert additional return edges that point to
    // all the locations to which this function can return. The indeterminate check is so that we don't end up processing the new
    // function return edges we're adding here (since they don't point to the indeterminate vertex).
    for (P2::ControlFlowGraph::Edge &returnEdge: cfg.edges()) {
        if (returnEdge.value().type() == P2::E_FUNCTION_RETURN && returnEdge.target()->value().type() == P2::V_INDETERMINATE) {
            for (const P2::Function::Ptr &function: returnEdge.source()->value().owningFunctions().values()) {
                for (size_t returnToSiteId: returnToSites[function]) {
                    auto newEdge = cfg.insertEdge(returnEdge.source(), cfg.findVertex(returnToSiteId), P2::E_FUNCTION_RETURN);
                    std::cerr <<"inserted function return " <<edgeName(*newEdge) <<"\n";
                }
            }
        }
    }

    // Now that we know to where every function can return, we can delete all the original edges that we don't need.
    for (P2::ControlFlowGraph::EdgeIterator edge = cfg.edges().begin(); edge != cfg.edges().end(); /*void*/) {
        if (edge->value().type() == P2::E_CALL_RETURN) {
            // Delete call-return edges if the return-to site has at least one incoming function-return edge.
            P2::ControlFlowGraph::VertexIterator returnToSite = edge->target();
            bool hasFunctionReturn = false;
            for (const P2::ControlFlowGraph::Edge &e: returnToSite->inEdges()) {
                if (e.value().type() == P2::E_FUNCTION_RETURN) {
                    hasFunctionReturn = true;
                    break;
                }
            }
            if (hasFunctionReturn) {
                std::cerr <<"deleting call return " <<edgeName(*edge) <<"\n";
                edge = cfg.eraseEdge(edge);
            } else {
                ++edge;
            }

        } else if (edge->value().type() == P2::E_FUNCTION_RETURN && edge->target()->value().type() == P2::V_INDETERMINATE) {
            // Delete function-return edges to the indeterminate vertex if the return site also has function-return edges that don't
            // point to the indeterminate vertex.
            P2::ControlFlowGraph::VertexIterator returnFromSite = edge->source();
            bool hasConcreteReturn = false;
            for (const P2::ControlFlowGraph::Edge &e: returnFromSite->outEdges()) {
                if (e.value().type() == P2::E_FUNCTION_RETURN && e.target()->value().type() != P2::V_INDETERMINATE) {
                    hasConcreteReturn = true;
                    break;
                }
            }
            if (hasConcreteReturn) {
                std::cerr <<"deleting function return " <<edgeName(*edge) <<"\n";
                edge = cfg.eraseEdge(edge);
            } else {
                ++edge;
            }

        } else {
            ++edge;
        }
    }

    // Emit the new CFG as a graphviz file
    P2::GraphViz::CfgEmitter gv(partitioner, cfg);
    gv.defaultGraphAttributes().insert("overlap", "scale");
    gv.useFunctionSubgraphs(true);
    gv.showInstructions(true);
    gv.showReturnEdges(true);
    gv.emitWholeGraph(std::cout);
}
