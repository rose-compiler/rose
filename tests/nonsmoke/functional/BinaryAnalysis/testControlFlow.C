static const char *purpose = "test binary control flow graphs";
static const char *description = "Reads a binary file, disassembles it, and spits out a control flow graph.";

#include "conditionalDisable.h"
#ifdef ROSE_BINARY_TEST_DISABLED
#include <iostream>
int main() { std::cout <<"disabled for " <<ROSE_BINARY_TEST_DISABLED <<"\n"; return 1; }
#else

#include <rose.h>
#include <Partitioner2/Engine.h>
#include <Partitioner2/GraphViz.h>
#include <Partitioner2/Partitioner.h>

using namespace Rose;
using namespace Rose::BinaryAnalysis;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;

int
main(int argc, char *argv[]) {
    /* Algorithm is first argument. */
    assert(argc>1);
    std::string algorithm = argv[1];
    memmove(argv+1, argv+2, argc-1); /* also copy null ptr */
    --argc;

    /* Parse the binary file */
    P2::Engine engine;
    std::vector<std::string> specimen = engine.parseCommandLine(argc, argv, purpose, description).unreachedArgs();
    P2::Partitioner partitioner = engine.partition(specimen);

    /* Calculate plain old CFG. */
    if (algorithm=="A") {
        P2::GraphViz::CfgEmitter emitter(partitioner);
        emitter.emitWholeGraph(std::cout);
    }

    /* Calculate a CFG with only function call edges.  Note that this is not quite the same as
     * FunctionCall::Graph. */
    if (algorithm=="B") {
        P2::GraphViz::CgEmitter emitter(partitioner);
        emitter.emitCallGraph(std::cout);
    }

    /* Build a pseudo call-graph by first building a CFG and then copying it to filter out non-call edges.  The result
     * should be the same as for algorithm B, assuming our edge filter is semantically equivalent. */
    if (algorithm=="C") {
        P2::ControlFlowGraph cfg = partitioner.cfg();
        P2::ControlFlowGraph::EdgeIterator ei = cfg.edges().begin();
        while (ei != cfg.edges().end()) {
            if (ei->value().type() == P2::E_FUNCTION_CALL) {
                ++ei;
            } else {
                ei = cfg.eraseEdge(ei);
            }
        }
        P2::GraphViz::CfgEmitter emitter(partitioner, cfg);
        emitter.emitWholeGraph(std::cout);
    }

    /* Build a pseudo call-graph by defining an edge filter before building a regular call graph.  The result should be the
     * same as for algorithm C since both use the same filter. */
    if (algorithm=="D") {
        std::cout <<"no longer supported\n";            // because CFGs can be easily modified now
    }

    /* Build an instruction-based control flow graph. */
    if (algorithm=="E") {
        P2::GraphViz::CfgEmitter emitter(partitioner);
        emitter.showInstructions(true);
        emitter.emitWholeGraph(std::cout);
    }
    
    return 0;
};

#endif
