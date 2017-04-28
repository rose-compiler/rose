#include <sage3basic.h>

#include <BinaryReachability.h>
#include <BinaryDataFlow.h>
#include <Partitioner2/Partitioner.h>

namespace P2 = rose::BinaryAnalysis::Partitioner2;

namespace rose {
namespace BinaryAnalysis {

void
Reachability::clearReachability() {
    intrinsicReachability_ = reachability_ = std::vector<unsigned>(cfg_.nVertices(), NOT_REACHABLE);
}

void
Reachability::clear() {
    cfg_ = P2::ControlFlowGraph();
    clearReachability();
}

void
Reachability::cfg(const P2::ControlFlowGraph &g) {
    cfg_ = g;
    clearReachability();
}

unsigned
Reachability::isIntrinsicallyReachable(size_t vertexId) const {
    ASSERT_require(vertexId < cfg_.nVertices());
    return intrinsicReachability_[vertexId];
}

unsigned
Reachability::isReachable(size_t vertexId) const {
    ASSERT_require(vertexId < cfg_.nVertices());
    return reachability_[vertexId];
}

struct TransferFunction {
    unsigned operator()(const P2::ControlFlowGraph&, size_t vertexId, unsigned state) {
        return state;
    }

    std::string printState(unsigned state) {
        return StringUtility::toHex2(state, 8*sizeof(state), false, false);
    }
};

struct MergeFunction {
    bool operator()(unsigned &a, unsigned b) {
        if (a == b)
            return false;
        a |= b;
        return true;
    }
};

void
Reachability::propagate() {
    typedef DataFlow::Engine<const P2::ControlFlowGraph, unsigned, TransferFunction, MergeFunction> DfEngine;
    TransferFunction xfer;
    MergeFunction merge;
    DfEngine engine(cfg_, xfer, merge);
    engine.reset(NOT_REACHABLE);

    // Initialize the data-flow states for vertices with intrinsic reachability.
    for (size_t i=0; i<cfg_.nVertices(); ++i) {
        if (unsigned r = intrinsicReachability_[i])
            engine.insertStartingVertex(i, r);
    }

    // Run a data-flow analysis to propagate reachability from intrinsically reachable vertices to other vertices.
    engine.runToFixedPoint();

    // Copy out all the vertex final states from the data-flow engine.
    for (size_t i=0; i<cfg_.nVertices(); ++i)
        reachability_[i] = engine.getFinalState(i);
}

void
Reachability::intrinsicallyReachable(size_t vertexId, unsigned how, bool doPropagate) {
    ASSERT_require(vertexId < cfg_.nVertices());
    if (how == intrinsicReachability_[vertexId])
        return;
    intrinsicReachability_[vertexId] = how;
    if (doPropagate)
        propagate();
}

const std::vector<unsigned>&
Reachability::reachability() const {
    return reachability_;
}

void
Reachability::markSpecialFunctions(const P2::Partitioner &partitioner) {
    bool changed = false;
    BOOST_FOREACH (const P2::Function::Ptr &func, partitioner.functions()) {
        P2::ControlFlowGraph::ConstVertexIterator vertex = partitioner.findPlaceholder(func->address());
        ASSERT_require(partitioner.cfg().isValidVertex(vertex));

        unsigned wasReachable = isIntrinsicallyReachable(vertex->id());
        unsigned reachable = wasReachable;
        
        if (0 != (func->reasons() & SgAsmFunction::FUNC_ENTRY_POINT))
            reachable |= ENTRY_POINT;
        if (0 != (func->reasons() & SgAsmFunction::FUNC_EXPORT))
            reachable |= EXPORTED_FUNCTION;

        if (reachable != wasReachable) {
            intrinsicallyReachable(vertex->id(), reachable, false /*do_not_propagate*/);
            changed = true;
        }
    }
    if (changed)
        propagate();
}

} // namespace
} // namespace
