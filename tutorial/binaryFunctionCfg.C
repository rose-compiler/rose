//! [prologue]
#include <rose.h>

#include <Rose/Diagnostics.h>
#include <Rose/BinaryAnalysis/Partitioner2/BasicBlock.h>
#include <Rose/BinaryAnalysis/Partitioner2/EngineBinary.h>
#include <Rose/BinaryAnalysis/Partitioner2/GraphViz.h>
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>
#include <Sawyer/CommandLine.h>

static const char* purpose = "obtains a function control flow graph from a binary specimen";
static const char* description =
    "This tool disassembles the specified file and generates prints a control flow "
    "graph to standard output for each function. It is intended as a demo of how to "
    "obtain a function CFG; there are better ways to print graphs.";

using namespace Rose::Diagnostics;
using namespace Rose::BinaryAnalysis;
//! [prologue]

int
main(int argc, char *argv[]) {
    //! [setup]
    ROSE_INITIALIZE;                                    // see Rose::initialize
    Partitioner2::Engine::Ptr engine = Partitioner2::EngineBinary::instance();
    std::vector<std::string> specimen = engine->parseCommandLine(argc, argv, purpose, description).unreachedArgs();
    if (specimen.empty()) {
        mlog[FATAL] <<"no binary specimen specified; see --help\n";
        exit(1);
    }
    //! [setup]
    
    //! [partition]
    Partitioner2::Partitioner::Ptr partitioner = engine->partition(specimen);
    //! [partition]

    //! [function cfg]
    for (const Partitioner2::Function::Ptr &function : partitioner->functions()) {
        // global control flow graph
        Partitioner2::ControlFlowGraph cfg = partitioner->cfg();

        // Erase all vertices that don't belong to the function of interest, and their incident edges
        Partitioner2::ControlFlowGraph::VertexIterator vi = cfg.vertices().begin();
        while (vi != cfg.vertices().end()) {
            if (!vi->value().isOwningFunction(function)) {
                cfg.eraseVertex(vi++);
            } else {
                ++vi;
            }
        }

        // Print the results
        std::cout <<"CFG for " <<function->printableName() <<"\n"
                  <<"  Vertices:\n";
        for (const Partitioner2::ControlFlowGraph::Vertex &v : cfg.vertices())
            std::cout <<"    " <<partitioner->vertexName(v) <<"\n";
        std::cout <<"  Edges:\n";
        for (const Partitioner2::ControlFlowGraph::Edge &e : cfg.edges())
            std::cout <<"    " <<partitioner->edgeName(e) <<"\n";
    }
    //! [function cfg]
}
