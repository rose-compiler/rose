#if __cplusplus >= 201402L

static const char *purpose = "demonstrate how to create an instruction-level CFG";
static const char *description =
    "ROSE control flow graphs normally have vertices that are basic blocks, where a basic block is defined as a sequence of "
    "instructions that always starts at the first instruction and ends at the last instruction (a basic block doesn't "
    "necessarily need to occupy contiguous memory).  Flow of control neither enters nor exits from the interior of a basic block.";

#include <rose.h>
#include <Rose/BinaryAnalysis/Partitioner2/Engine.h>
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>
#include <Sawyer/GraphAlgorithm.h>

namespace P2 = Rose::BinaryAnalysis::Partitioner2;

int main(int argc, char *argv[]) {
    ROSE_INITIALIZE;

    P2::Engine engine;
    std::vector<std::string> specimen = engine.parseCommandLine(argc, argv, purpose, description).unreachedArgs();
    P2::Partitioner partitioner = engine.partition(specimen);

    // Get the standard basic block CFG and empty instruction CFG. The instruction CFG is as simple as possible: each vertex is
    // just a pointer to an instruction AST (with ill-defined ownership and all) and we're not storing any data on the
    // edges. We're also going to ignore all the CFG vertices that aren't basic blocks.
    const P2::ControlFlowGraph &cfg = partitioner.cfg();
    using InsnCfg = Sawyer::Container::Graph<SgAsmInstruction*>;
    InsnCfg insnCfg;

    // A mapping from each original basic block to its first instruction in the new CFG, which we'll need when creating the
    // inter-block edges. We'll use vertex IDs as both the domain and range since they're stable across vertex insertions (just
    // not stable across erasures).
    std::vector<size_t> bbToInsn(cfg.nVertices(), Rose::INVALID_INDEX);

    // For each basic block in the original CFG, create vertices for all its instructions in the new CFG. Also, remember the
    // mapping from basic blocks to the first instruction of each basic block in the new CFG since we'll need this later to
    // create the inter-block edges.
    for (const P2::ControlFlowGraph::Vertex &origVertex: cfg.vertices()) {
        if (origVertex.value().type() == P2::V_BASIC_BLOCK) {
            P2::BasicBlock::Ptr bb = origVertex.value().bblock();
            ASSERT_not_null(bb);
            InsnCfg::VertexIterator prev = insnCfg.vertices().end();
            for (SgAsmInstruction *insn: bb->instructions()) {
                InsnCfg::VertexIterator cur = insnCfg.insertVertex(insn);
                if (prev == insnCfg.vertices().end()) {
                    bbToInsn[origVertex.id()] = cur->id();
                } else {
                    insnCfg.insertEdge(prev, cur);     // intra-block edge
                }
                prev = cur;
            }
        }
    }

    // Now that we have all the vertices in the new graph, create the inter-block edges.
    for (const P2::ControlFlowGraph::Edge &edge: cfg.edges()) {
        if (edge.source()->value().type() == P2::V_BASIC_BLOCK && edge.target()->value().type() == P2::V_BASIC_BLOCK) {
            InsnCfg::VertexIterator newSrc = insnCfg.findVertex(bbToInsn[edge.source()->id()]);
            InsnCfg::VertexIterator newTgt = insnCfg.findVertex(bbToInsn[edge.target()->id()]);
            insnCfg.insertEdge(newSrc, newTgt);
        }
    }

    // Example depth-first reverse traversals of an instruction CFG, starting from each vertex. This time we'll iterate
    // over the starting vertices by hand to show how it's done, and we'll use a traversal algorithm for the DFS part.
    for (InsnCfg::VertexIterator vertex = insnCfg.vertices().begin(); vertex != insnCfg.vertices().end(); ++vertex) {
        std::cout <<"starting from vertex " <<vertex->id() <<"\n";
        using T = Sawyer::Container::Algorithm::DepthFirstReverseEdgeTraversal<InsnCfg>;
        for (T t(insnCfg, vertex); t; ++t) {
            const InsnCfg::Edge &edge = *t;
            std::cout <<"  edge " <<edge.id()
                      <<" from vertex " <<edge.source()->id()
                      <<" to vertex " <<edge.target()->id() <<"\n";
        }
    }
}

#else

#include <iostream>
int main() {
    std::cerr <<"not supported in this ROSE configuration\n";
    return 1;
}

#endif
