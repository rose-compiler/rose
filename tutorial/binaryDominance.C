// Extensive documentation for this program is in the Binary Analysis Tutorial in doxygen.
// Do not change the output format -- this tool is also used by tests/nonsmoke/functional/BinaryAnalysis

//! [headers]
static const char *programPurpose = "prints dominators for all function CFGs";
static const char *programDescription =
    "The purpose of this tool is to demonstrate how to calculate dominators (and by extension, post-dominators) for all "
    "vertices of a function control flow graph (and by extension, any Sawyer graph).  It parses, loads, disassembles, and "
    "partitions a binary specimen and then iterates over the functions. For each function, it obtains a function CFG, "
    "calculates the immediate dominator for each vertex, and shows the results.";

#include <rose.h>
#include <Partitioner2/Engine.h>
#include <Partitioner2/Partitioner.h>
#include <Sawyer/GraphAlgorithm.h>

namespace P2 = Rose::BinaryAnalysis::Partitioner2;
//! [headers]

//! [init]
int
main(int argc, char *argv[])
{
    ROSE_INITIALIZE;
    P2::Engine engine;
    engine.doingPostAnalysis(false);                    // not needed for this tool, and faster without
    engine.namingSystemCalls(false);                    // for consistent results w.r.t. the answer file since the system...
    engine.systemCallHeader("/dev/null");               // ...call mapping comes from run-time files.
    std::vector<std::string> specimen = engine.parseCommandLine(argc, argv, programPurpose, programDescription).unreachedArgs();
    P2::Partitioner partitioner = engine.partition(specimen);
    //! [init]

    //! [iterate]
    BOOST_FOREACH (P2::Function::Ptr function, partitioner.functions()) {
        std::cout <<"CFG dominators for function " <<Rose::StringUtility::addrToString(function->address()) <<":\n"; // or function->printableName()
        //! [iterate]

        //! [cfg_enter]
        P2::ControlFlowGraph cfg = partitioner.cfg();
        P2::ControlFlowGraph::VertexIterator entryVertex = cfg.findVertex(partitioner.findPlaceholder(function->address())->id());
        //! [cfg_enter]
        ASSERT_require(cfg.isValidVertex(entryVertex));

        //! [cfg_build]
        P2::ControlFlowGraph::VertexIterator vi = cfg.vertices().begin();
        while (vi != cfg.vertices().end()) {
            if (!vi->value().isOwningFunction(function)) { // a basic block can belong to multiple functions
                ASSERT_forbid(vi == entryVertex);
                cfg.eraseVertex(vi++);
            } else {
                ++vi;
            }
        }
        //! [cfg_build]

        //! [dominators]
        std::vector<P2::ControlFlowGraph::VertexIterator> idoms = Sawyer::Container::Algorithm::graphDominators(cfg, entryVertex);
        //! [dominators]
        ASSERT_require(idoms.size() == cfg.nVertices());

        //! [results]
        for (size_t i=0; i<idoms.size(); ++i) {
            std::cout <<"  function " <<Rose::StringUtility::addrToString(function->address())
                      <<" dominator of " <<P2::Partitioner::vertexName(*cfg.findVertex(i))
                      <<" is ";
            if (cfg.isValidVertex(idoms[i])) {
                std::cout <<P2::Partitioner::vertexName(*idoms[i]) <<"\n";
            } else {
                std::cout <<"none\n";
            }
        }
        //! [results]
    }
}
