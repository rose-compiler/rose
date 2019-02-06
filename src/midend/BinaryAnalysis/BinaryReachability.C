#include <sage3basic.h>

#include <BinaryReachability.h>
#include <BinaryDataFlow.h>
#include <Partitioner2/Partitioner.h>

namespace P2 = Rose::BinaryAnalysis::Partitioner2;

namespace Rose {
namespace BinaryAnalysis {

void
Reachability::clearReachability() {
    intrinsicReachability_ = reachability_ = std::vector<ReasonFlags>(cfg_.nVertices());
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

void
Reachability::replaceCfg(const P2::ControlFlowGraph &g) {
    cfg_ = g;
}

Reachability::ReasonFlags
Reachability::isIntrinsicallyReachable(size_t vertexId) const {
    ASSERT_require(vertexId < cfg_.nVertices());
    return intrinsicReachability_[vertexId];
}

Reachability::ReasonFlags
Reachability::isReachable(size_t vertexId) const {
    ASSERT_require(vertexId < cfg_.nVertices());
    return reachability_[vertexId];
}

struct TransferFunction {
    Reachability::ReasonFlags operator()(const P2::ControlFlowGraph&, size_t vertexId, Reachability::ReasonFlags state) {
        return state;
    }

    std::string printState(Reachability::ReasonFlags state) {
        return StringUtility::toHex2(state.vector(), 32, false, false);
    }
};

struct MergeFunction {
    bool operator()(Reachability::ReasonFlags &a, Reachability::ReasonFlags b) {
        if (a == b)
            return false;
        a.set(b);
        return true;
    }
};

void
Reachability::propagate() {
    typedef DataFlow::Engine<const P2::ControlFlowGraph, ReasonFlags, TransferFunction, MergeFunction> DfEngine;
    TransferFunction xfer;
    MergeFunction merge;
    DfEngine engine(cfg_, xfer, merge);
    engine.reset(NOT_REACHABLE);

    // Initialize the data-flow states for vertices with intrinsic reachability.
    for (size_t i=0; i<cfg_.nVertices(); ++i) {
        ReasonFlags r = intrinsicReachability_[i];
        if (r.isAnySet())
            engine.insertStartingVertex(i, r);
    }

    // Run a data-flow analysis to propagate reachability from intrinsically reachable vertices to other vertices.
    engine.runToFixedPoint();

    // Copy out all the vertex final states from the data-flow engine.
    for (size_t i=0; i<cfg_.nVertices(); ++i)
        reachability_[i] = engine.getFinalState(i);
}

void
Reachability::intrinsicallyReachable(size_t vertexId, ReasonFlags how, Propagate::Boolean doPropagate) {
    ASSERT_require(vertexId < cfg_.nVertices());
    if (how == intrinsicReachability_[vertexId])
        return;
    intrinsicReachability_[vertexId] = how;
    if (doPropagate != Propagate::NO)
        propagate();
}

const std::vector<Reachability::ReasonFlags>&
Reachability::reachability() const {
    return reachability_;
}

void
Reachability::markSpecialFunctions(const P2::Partitioner &partitioner) {
    bool changed = false;
    BOOST_FOREACH (const P2::Function::Ptr &func, partitioner.functions()) {
        P2::ControlFlowGraph::ConstVertexIterator vertex = partitioner.findPlaceholder(func->address());
        ASSERT_require(partitioner.cfg().isValidVertex(vertex));

        ReasonFlags wasReachable = isIntrinsicallyReachable(vertex->id());
        ReasonFlags reachable = wasReachable;
        
        if (0 != (func->reasons() & SgAsmFunction::FUNC_ENTRY_POINT))
            reachable.set(ENTRY_POINT);
        if (0 != (func->reasons() & SgAsmFunction::FUNC_EXPORT))
            reachable.set(EXPORTED_FUNCTION);

        if (reachable != wasReachable) {
            intrinsicallyReachable(vertex->id(), reachable, Propagate::NO);
            changed = true;
        }
    }
    if (changed)
        propagate();
}

} // namespace
} // namespace
