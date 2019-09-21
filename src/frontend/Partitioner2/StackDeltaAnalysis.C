#include "sage3basic.h"

#include <AsmUnparser_compat.h>
#include <BinaryDataFlow.h>
#include <BinaryStackDelta.h>
#include <CommandLine.h>
#include <Partitioner2/DataFlow.h>
#include <Partitioner2/Partitioner.h>
#include <Sawyer/GraphAlgorithm.h>
#include <Sawyer/ProgressBar.h>
#include <Sawyer/SharedPointer.h>
#include <Sawyer/Stopwatch.h>
#include <Sawyer/ThreadWorkers.h>
#include <SymbolicSemantics2.h>

using namespace Rose::Diagnostics;
using namespace Rose::BinaryAnalysis::InstructionSemantics2;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;

namespace Rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

void
Partitioner::forgetStackDeltas() const {
    BOOST_FOREACH (const Function::Ptr &function, functions())
        forgetStackDeltas(function);
}

void
Partitioner::forgetStackDeltas(const Function::Ptr &function) const {
    ASSERT_not_null(function);
    function->stackDeltaAnalysis().clearResults();
}

// Determines when to perform interprocedural dataflow.  We want stack delta analysis to be interprocedural only if the called
// function has no stack delta.
struct InterproceduralPredicate: P2::DataFlow::InterproceduralPredicate {
    const Partitioner &partitioner;
    InterproceduralPredicate(const Partitioner &partitioner): partitioner(partitioner) {}
    bool operator()(const ControlFlowGraph &cfg, const ControlFlowGraph::ConstEdgeIterator &callEdge, size_t depth) {
        if (depth > partitioner.stackDeltaInterproceduralLimit())
            return false;
        ASSERT_require(callEdge != cfg.edges().end());
        ASSERT_require(callEdge->target()->value().type() == V_BASIC_BLOCK);
        BOOST_FOREACH (const Function::Ptr &function, callEdge->target()->value().owningFunctions().values()) {
            if (function->stackDelta())
                return false;
        }
        return true;                                    // no called function has a computed stack delta
    }
};

BaseSemantics::SValuePtr
Partitioner::functionStackDelta(const Function::Ptr &function) const {
    ASSERT_not_null(function);
    BaseSemantics::SValuePtr retval;
    size_t bitsPerWord = instructionProvider().stackPointerRegister().nBits();

    // If a stack delta is defined for this function then use it
    BaseSemantics::RiscOperatorsPtr ops = newOperators();
    if (Sawyer::Optional<int64_t> delta = config_.functionStackDelta(function))
        return ops->number_(bitsPerWord, *delta);

    // If an analysis has already been run or an override value has been set, return it.
    if ((retval = function->stackDelta()))
        return retval;

    // If a stack delta has run already for this function return it even if it had failed to find a delta.
    if (function->stackDeltaAnalysis().hasResults())
        return function->stackDelta();

    // Create the CFG that we'll use for dataflow.
    ControlFlowGraph::ConstVertexIterator functionVertex = findPlaceholder(function->address());
    if (functionVertex == cfg_.vertices().end()) {
        SAWYER_MESG(mlog[ERROR]) <<"functionStackDeltas: " <<function->printableName()
                                 <<" entry block is not attached to the CFG/AUM\n";
        return retval;
    }

    // Run the analysis.
    BaseSemantics::DispatcherPtr cpu = newDispatcher(newOperators());
    if (cpu == NULL) {
        SAWYER_MESG(mlog[DEBUG]) <<"  no instruction semantics for this architecture\n";
        return retval;
    }
    BaseSemantics::MemoryStatePtr mem = cpu->get_operators()->currentState()->memoryState();
    if (Semantics::MemoryListStatePtr ml = boost::dynamic_pointer_cast<Semantics::MemoryListState>(mem)) {
        ml->enabled(false);
    } else if (Semantics::MemoryMapStatePtr mm = boost::dynamic_pointer_cast<Semantics::MemoryMapState>(mem)) {
        mm->enabled(false);
    }
    StackDelta::Analysis &sdAnalysis = function->stackDeltaAnalysis() = StackDelta::Analysis(cpu);
    sdAnalysis.initialConcreteStackPointer(0x7fff0000); // optional: helps reach more solutions
    InterproceduralPredicate ip(*this);
    sdAnalysis.analyzeFunction(*this, function, ip);
    retval = sdAnalysis.functionStackDelta();

#if 0 // [Robb Matzke 2015-11-17]
    // If any basic blocks branched to an indeterminate location then we cannot know the stack delta. The indeterminate
    // location might have eventually branched back into this function with an arbitrary stack delta that should have poisoned
    // the result.
    if (branchedToIndeterminate) {
        SAWYER_MESG(trace) <<"  saw an indeterminate edge, therefore result must be TOP\n";
        retval = ops->undefined_(instructionProvider_->stackPointerRegister().get_nbits());
    }
#endif

    // If the analysis failed to find this function's stack delta and this function looks like a dynamic linking thunk, and the
    // thunk points to a nonexisting address, then assume that the (not yet linked) destination function pops the return
    // value. FIXME[Robb Matzke 2015-11-18]: This isn't correct in callee-cleanup situations (but it's what the previous
    // implementation did, so I don't want to change it just yet)!
    if (retval==NULL &&                                                         // analysis failed.
        function->nBasicBlocks() == 1 &&                                        // thunks have a single basic block...
        functionVertex->nOutEdges() == 1 &&                                     // ...with a single outgoing edge which ...
        (functionVertex->outEdges().begin()->target() == nonexistingVertex_ ||  // ...branches to non-existing memory or...
         functionVertex->outEdges().begin()->target() == indeterminateVertex_) && // ...an unknown location.
        sdAnalysis.basicBlockStackDeltaConcrete(function->address()) == 0) {    // thunks don't push/pop

        std::string reason;
        if (CallingConvention::Definition::Ptr ccDefn = function->callingConventionDefinition()) {
            reason = "calling convention";
            retval = ops->number_(bitsPerWord, ccDefn->nonParameterStackSize());
            if (ccDefn->stackDirection() == CallingConvention::/*StackDirection::*/GROWS_UP)
                retval = ops->invert(retval);

        } else if (!instructionProvider().callReturnRegister().isEmpty()) {
            reason = "typical of register-based function calls";
            retval = ops->number_(bitsPerWord, 0);

        } else {
            // FIXME[Robb Matzke 2019-09-18]: assumes stack grows down
            reason = "typical of stack-based function calls";
            retval = ops->number_(bitsPerWord, bitsPerWord/8); // size of return address on stack

        }
        ASSERT_not_null(retval);
        function->stackDeltaOverride(retval);
        SAWYER_MESG(mlog[DEBUG]) <<"  assuming " <<function->printableName()
                                 <<" (thunk) stack delta is " <<*retval <<" (" <<reason <<")\n";
    }

    return retval;
}

struct StackDeltaWorker {
    const Partitioner &partitioner;
    Sawyer::ProgressBar<size_t> &progress;

    StackDeltaWorker(const Partitioner &partitioner, Sawyer::ProgressBar<size_t> &progress)
        : partitioner(partitioner), progress(progress) {}

    void operator()(size_t workId, const Function::Ptr &function) {
        Sawyer::Stopwatch t;
        partitioner.functionStackDelta(function);

        // Show some results. We're using Rose::BinaryAnalysis::StackDelta::mlog[TRACE] for the messages, so the mutex here
        // doesn't really protect it. However, since that analysis doesn't produce much output on that stream, this mutex helps
        // keep the output lines separated from one another, especially when they're all first starting up.
        if (StackDelta::mlog[TRACE]) {
            static boost::mutex mutex;
            boost::lock_guard<boost::mutex> lock(mutex);
            Sawyer::Message::Stream trace(StackDelta::mlog[TRACE]);
            trace <<"stack-delta for " <<function->printableName() <<" took " <<t <<" seconds\n";
        }

        // Progress reports
        ++progress;
        partitioner.updateProgress("stack-delta", progress.ratio());
    }
};

// Compute stack deltas for all basic blocks in all functions, and for functions overall. Functions are processed in an order
// so that callees are before callers.
void
Partitioner::allFunctionStackDelta() const {
    size_t nThreads = Rose::CommandLine::genericSwitchArgs.threads;
    FunctionCallGraph::Graph cg = functionCallGraph(AllowParallelEdges::NO).graph();
    Sawyer::Container::Algorithm::graphBreakCycles(cg);
    Sawyer::ProgressBar<size_t> progress(cg.nVertices(), mlog[MARCH], "stack-delta analysis");
    progress.suffix(" functions");
    Sawyer::Message::FacilitiesGuard guard;
    if (nThreads != 1)                                  // lots of threads doing progress reports won't look too good!
        Rose::BinaryAnalysis::StackDelta::mlog[MARCH].disable();
    Sawyer::workInParallel(cg, nThreads, StackDeltaWorker(*this, progress));
}

} // namespace
} // namespace
} // namespace
