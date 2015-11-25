#include "sage3basic.h"

#include <AsmUnparser_compat.h>
#include <BinaryDataFlow.h>
#include <BinaryStackDelta.h>
#include <Partitioner2/DataFlow.h>
#include <Partitioner2/Partitioner.h>
#include <Sawyer/GraphAlgorithm.h>
#include <Sawyer/ProgressBar.h>
#include <Sawyer/SharedPointer.h>
#include <Sawyer/Stopwatch.h>
#include <Sawyer/ThreadWorkers.h>
#include <SymbolicSemantics2.h>

using namespace rose::Diagnostics;
using namespace rose::BinaryAnalysis::InstructionSemantics2;
namespace P2 = rose::BinaryAnalysis::Partitioner2;

namespace rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

void
Partitioner::forgetStackDeltas() const {
    BOOST_FOREACH (const BasicBlock::Ptr &bb, basicBlocks()) {
        bb->stackDeltaIn(BaseSemantics::SValuePtr());
        bb->stackDeltaOut(BaseSemantics::SValuePtr());
        bb->stackDelta(BaseSemantics::SValuePtr());
        BOOST_FOREACH (SgAsmInstruction *insn, bb->instructions())
            insn->set_stackDeltaIn(SgAsmInstruction::INVALID_STACK_DELTA);
    }
    BOOST_FOREACH (const Function::Ptr &func, functions())
        func->stackDelta(BaseSemantics::SValuePtr());
}

void
Partitioner::forgetStackDeltas(const Function::Ptr &function) const {
    ASSERT_not_null(function);
    BOOST_FOREACH (rose_addr_t va, function->basicBlockAddresses()) {
        ControlFlowGraph::ConstVertexIterator vertex = findPlaceholder(va);
        if (vertex != cfg().vertices().end() && vertex->value().type() == V_BASIC_BLOCK) {
            if (BasicBlock::Ptr bb = vertex->value().bblock()) {
                bb->stackDeltaIn(BaseSemantics::SValuePtr());
                bb->stackDeltaOut(BaseSemantics::SValuePtr());
                bb->stackDelta(BaseSemantics::SValuePtr());
                BOOST_FOREACH (SgAsmInstruction *insn, bb->instructions())
                    insn->set_stackDeltaIn(SgAsmInstruction::INVALID_STACK_DELTA);
            }
        }
    }
    function->stackDelta(BaseSemantics::SValuePtr());
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
        Function::Ptr function = callEdge->target()->value().function();
        return function && !function->stackDelta();
    }
};

BaseSemantics::SValuePtr
Partitioner::functionStackDelta(const Function::Ptr &function) const {
    ASSERT_not_null(function);
    BaseSemantics::SValuePtr retval;
    size_t bitsPerWord = instructionProvider().stackPointerRegister().get_nbits();

    // If a stack delta is defined for this function then use it (and cache it for next time).
    BaseSemantics::RiscOperatorsPtr ops = newOperators();
    if (Sawyer::Optional<int64_t> delta = config_.functionStackDelta(function)) {
        retval = ops->number_(bitsPerWord, *delta);
        function->stackDelta(retval);
        return retval;
    }
    
    // If a stack delta is known from some previous analysis, then return it.
    if ((retval = function->stackDelta()))
        return retval; 

    // Create the CFG that we'll use for dataflow.
    ControlFlowGraph::ConstVertexIterator functionVertex = findPlaceholder(function->address());
    if (functionVertex == cfg_.vertices().end()) {
        SAWYER_MESG(mlog[ERROR]) <<"functionStackDeltas: " <<function->printableName()
                                 <<" entry block is not attached to the CFG/AUM\n";
        return retval;
    }

    // (Re)run the analysis. Even if we ran the analysis already and it failed to determine the stack delta, things might have
    // changed in the CFG that allows it to succeed this time.  We provide our own dispatcher because we want the analysis to
    // be fast by not using any memory state (stack pointers are rarely saved and restored).
    BaseSemantics::DispatcherPtr cpu = newDispatcher(newOperators());
    if (cpu == NULL) {
        SAWYER_MESG(mlog[DEBUG]) <<"  no instruction semantics for this architecture\n";
        return retval;
    }
    Semantics::MemoryState::promote(cpu->get_operators()->get_state()->get_memory_state())->enabled(false);
    StackDelta::Analysis sdAnalysis(cpu);
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
#if 1 // [Robb Matzke 2015-11-18]
    if (retval==NULL &&                                                         // analysis failed.
        function->nBasicBlocks() == 1 &&                                        // thunks have a single basic block...
        functionVertex->nOutEdges() == 1 &&                                     // ...with a single outgoing edge which ...
        (functionVertex->outEdges().begin()->target() == nonexistingVertex_ ||  // ...branches to non-existing memory or...
         functionVertex->outEdges().begin()->target() == indeterminateVertex_) && // ...an unknown location.
        sdAnalysis.basicBlockStackDeltaConcrete(function->address()) == 0) {    // thunks don't push/pop
        SAWYER_MESG(mlog[DEBUG]) <<"  assuming " <<function->printableName()
                                 <<" (thunk) stack delta is " <<(bitsPerWord/8) <<"\n";
        retval = ops->number_(bitsPerWord, bitsPerWord/8);                      // size of return address on stack
    }
#endif

    // Cache stack information in the function.
    function->stackDelta(retval);

    // Cache stack information in function's basic blocks. Compress the expressions to a single node to save memory.
    BaseSemantics::SValuePtr functionInitialStackPtr = sdAnalysis.functionStackPointers().first;
    BOOST_FOREACH (rose_addr_t va, function->basicBlockAddresses()) {
        if (BasicBlock::Ptr bb = basicBlockExists(va)) {
            if (BaseSemantics::SValuePtr delta = sdAnalysis.basicBlockStackDelta(va)) {
                if (!SymbolicSemantics::SValue::promote(delta)->get_expression()->isLeafNode())
                    delta = ops->undefined_(delta->get_width());
                bb->stackDelta(delta);
            }
            if (functionInitialStackPtr) {
                if (BaseSemantics::SValuePtr bbSp = sdAnalysis.basicBlockStackPointers(va).first) {
                    BaseSemantics::SValuePtr delta = ops->subtract(bbSp, functionInitialStackPtr);
                    if (!SymbolicSemantics::SValue::promote(delta)->get_expression()->isLeafNode())
                        delta = ops->undefined_(delta->get_width());
                    bb->stackDeltaIn(delta);
                }
                if (BaseSemantics::SValuePtr bbSp = sdAnalysis.basicBlockStackPointers(va).second) {
                    BaseSemantics::SValuePtr delta = ops->subtract(bbSp, functionInitialStackPtr);
                    if (!SymbolicSemantics::SValue::promote(delta)->get_expression()->isLeafNode())
                        delta = ops->undefined_(delta->get_width());
                    bb->stackDeltaOut(delta);
                }
            }
        }
    }
    
    // Cache stack information in the function's instructions.
    if (functionInitialStackPtr) {
        BOOST_FOREACH (rose_addr_t va, function->basicBlockAddresses()) {
            if (BasicBlock::Ptr bb = basicBlockExists(va)) {
                BOOST_FOREACH (SgAsmInstruction *insn, bb->instructions()) {
                    if (BaseSemantics::SValuePtr insnSp = sdAnalysis.instructionStackPointers(insn).first) {
                        BaseSemantics::SValuePtr delta = ops->subtract(insnSp, functionInitialStackPtr);
                        insn->set_stackDeltaIn(sdAnalysis.toInt(delta));
                    }
                }
            }
        }
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

        // Show some results. We're using rose::BinaryAnalysis::StackDelta::mlog[TRACE] for the messages, so the mutex here
        // doesn't really protect it. However, since that analysis doesn't produce much output on that stream, this mutex helps
        // keep the output lines separated from one another, especially when they're all first starting up.
        if (StackDelta::mlog[TRACE]) {
            static boost::mutex mutex;
            boost::lock_guard<boost::mutex> lock(mutex);
            Sawyer::Message::Stream trace(StackDelta::mlog[TRACE]);
            trace <<"stack-delta for " <<function->printableName() <<" took " <<t <<" seconds\n";
        }

        ++progress;
    }
};

// Compute stack deltas for all basic blocks in all functions, and for functions overall. Functions are processed in an order
// so that callees are before callers.
void
Partitioner::allFunctionStackDelta() const {
    size_t nThreads = CommandlineProcessing::genericSwitchArgs.threads;
    FunctionCallGraph::Graph cg = functionCallGraph().graph();
    Sawyer::Container::Algorithm::graphBreakCycles(cg);
    Sawyer::ProgressBar<size_t> progress(cg.nVertices(), mlog[MARCH], "stack-delta analysis");
    Sawyer::Message::FacilitiesGuard guard();
    if (nThreads != 1)                                  // lots of threads doing progress reports won't look too good!
        rose::BinaryAnalysis::StackDelta::mlog[MARCH].disable();
    Sawyer::workInParallel(cg, nThreads, StackDeltaWorker(*this, progress));
}

} // namespace
} // namespace
} // namespace
