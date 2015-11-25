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
            
// Association between dataflow variables and their values.  We're only interested in the register state because the stack
// pointer is seldom spilled to memory by normal functions, and using only registers simplifies things substantially. For
// instance, a machine has a finite number of registers, so there's no need to build a variable list before we run the
// dataflow--we just instantiate the variables as we encounter them.  Secondly, the variable list is shorter and therefore
// faster, and comparing one register with another is substantially faster than comparing two memory address expressions,
// especially when an SMT solver becomes involved.  Values stored in the variables form an infinite lattice with three depths:
// not-calculated, concrete, and abstract.  Not-calculated is represented by not storing any value for a register; concrete is
// an expression whose is_number predicate is true; abstract is any other symbolic expression.
class State: public BaseSemantics::RegisterStateGeneric {
    BaseSemantics::RiscOperatorsPtr ops_;
public:
    typedef boost::shared_ptr<State> Ptr;

protected:
    explicit State(const BaseSemantics::RiscOperatorsPtr &ops)
        : BaseSemantics::RegisterStateGeneric(ops->get_protoval(),
                                              ops->get_state()->get_register_state()->get_register_dictionary()),
          ops_(ops) {}

    State(const State &other)
        : BaseSemantics::RegisterStateGeneric(other), ops_(other.ops_) {}

public:
    static State::Ptr instance(const BaseSemantics::RiscOperatorsPtr &ops) {
        return State::Ptr(new State(ops));
    }

    static State::Ptr instance(const State::Ptr &other) {
        return State::Ptr(new State(*other));
    }

    virtual BaseSemantics::RegisterStatePtr create(const BaseSemantics::SValuePtr &protoval,
                                                   const RegisterDictionary *regdict) const ROSE_OVERRIDE {
        ASSERT_not_implemented("[Robb P. Matzke 2014-11-21] shouldn't be needed since this class is effectively final");
    }

    virtual BaseSemantics::RegisterStatePtr clone() const ROSE_OVERRIDE {
        return State::Ptr(new State(*this));
    }

    static State::Ptr promote(const BaseSemantics::RegisterStatePtr &from) {
        State::Ptr retval = boost::dynamic_pointer_cast<State>(from);
        ASSERT_not_null(retval);
        return retval;
    }

    static bool
    mergeSValues(BaseSemantics::SValuePtr &ours /*in,out*/, const BaseSemantics::SValuePtr &theirs,
                 const BaseSemantics::RiscOperatorsPtr &ops) {
        // The calls to ops->undefined(...) are mostly so that we're simplifying TOP as much as possible. We want each TOP
        // value to be distinct from the others so we don't encounter things like "TOP xor TOP = 0", but we also don't want TOP
        // to be arbitrarily complex since that just makes things slower.
        if (!ours && !theirs) {
            return false;                               // both are BOTTOM (not calculated)
        } else if (!ours) {
            ASSERT_not_null(theirs);
            if (theirs->is_number()) {
                ours = theirs;
            } else {
                ours = ops->undefined_(theirs->get_width());
            }
            return true;                                // ours is BOTTOM, theirs is not
        } else if (!theirs) {
            return false;                               // theirs is BOTTOM, ours is not
        } else if (!ours->is_number()) {
            ours = ops->undefined_(ours->get_width());
            return false;                               // ours was already TOP
        } else if (ours->must_equal(theirs)) {
            return false;                               // ours == theirs
        } else {
            ours = ops->undefined_(ours->get_width());
            return true;                                // ours became TOP
        }
    }
    
    // Merge other into this, returning true if this changed.
    bool merge(const State::Ptr &other) {
        bool changed = false;
        if (mlog[DEBUG]) {
            InstructionSemantics2::BaseSemantics::Formatter fmt;
            fmt.set_line_prefix("    ");
            mlog[DEBUG] <<"Stack delta merge states\n";
            mlog[DEBUG] <<"  this state:\n" <<(*this+fmt);
            mlog[DEBUG] <<"  other state:\n" <<(*other+fmt);
        }
        
        BOOST_FOREACH (const RegPair &reg_val, other->get_stored_registers()) {
            const RegisterDescriptor &reg = reg_val.desc;
            const BaseSemantics::SValuePtr &theirs = reg_val.value;
            if (mlog[DEBUG]) {
                mlog[DEBUG] <<"  register " <<reg <<"\n";
                mlog[DEBUG] <<"    their value = " <<*theirs <<"\n";
            }

            if (!is_partly_stored(reg)) {
                SAWYER_MESG(mlog[DEBUG]) <<"    our value   = <none>\n";
                changed = true;
                writeRegister(reg, theirs, ops_.get());
            } else {
                BaseSemantics::SValuePtr ours = readRegister(reg, ops_.get());
                SAWYER_MESG(mlog[DEBUG]) <<"    our value   = " <<*ours <<"\n";
                if (mergeSValues(ours /*in,out*/, theirs, ops_)) {
                    SAWYER_MESG(mlog[DEBUG]) <<"      state changed\n";
                    writeRegister(reg, ours, ops_.get());
                    changed = true;
                }
            }
        }
        return changed;
    }
};

// Dataflow transfer function
class TransferFunction {
    BaseSemantics::DispatcherPtr cpu_;
    BaseSemantics::SValuePtr callRetAdjustment_;
    const RegisterDescriptor STACK_POINTER_REG;
    static const rose_addr_t initialStackPointer = 0x7fff0000; // arbitrary stack pointer value at start of function
public:
    explicit TransferFunction(const BaseSemantics::DispatcherPtr &cpu, const RegisterDescriptor &stackPointerRegister)
        : cpu_(cpu), STACK_POINTER_REG(stackPointerRegister) {
        size_t adjustment = STACK_POINTER_REG.get_nbits() / 8; // sizeof return address on top of stack
        callRetAdjustment_ = cpu->number_(STACK_POINTER_REG.get_nbits(), adjustment);
    }

    State::Ptr initialState() const {
        BaseSemantics::RiscOperatorsPtr ops = cpu_->get_operators();
        State::Ptr retval = State::instance(ops);
        retval->writeRegister(STACK_POINTER_REG, ops->number_(STACK_POINTER_REG.get_nbits(), initialStackPointer), ops.get());
        return retval;
    }

    // Required by dataflow engine: should return a deep copy of the state
    State::Ptr operator()(const State::Ptr &incomingState) const {
        return incomingState ? State::promote(incomingState->clone()) : State::Ptr();
    }

    // Required by dataflow engine: compute new output state given a vertex and input state.
    State::Ptr operator()(const P2::DataFlow::DfCfg &dfCfg, size_t vertexId, const State::Ptr &incomingState) const {
        State::Ptr retval = State::promote(incomingState->clone());
        P2::DataFlow::DfCfg::ConstVertexIterator vertex = dfCfg.findVertex(vertexId);
        ASSERT_require(vertex != dfCfg.vertices().end());
        if (P2::DataFlow::DfCfgVertex::FAKED_CALL == vertex->value().type()) {
            // Adjust the stack pointer as if the function call returned.  If we know the function delta then use it, otherwise
            // assume it just pops the return value.
            BaseSemantics::SValuePtr delta;
            if (P2::Function::Ptr callee = vertex->value().callee())
                delta = callee->stackDelta().getOptional().orDefault();

            // Update the result state
            BaseSemantics::RiscOperatorsPtr ops = cpu_->get_operators();
            BaseSemantics::SValuePtr newStack;
            SymbolicExpr::LeafPtr leaf;
            if (delta && (leaf = Semantics::SValue::promote(delta)->get_expression()->isLeafNode()) && leaf->isVariable()) {
                // We tried to compute stack delta but were unable to come up with anything meaningful.
                delta = BaseSemantics::SValuePtr();
            }
            
            if (delta) {
                BaseSemantics::SValuePtr oldStack = incomingState->readRegister(STACK_POINTER_REG, ops.get());
                newStack = ops->add(oldStack, delta);
            } else if (true) { // FIXME[Robb P. Matzke 2014-12-15]: should only apply if caller cleans up arguments
                // We don't know the callee's delta, so assume that the callee pops only its return address. This is usually
                // the correct for caller-cleanup ABIs common on Unix/Linux, but not usually correct for callee-cleanup ABIs
                // common on Microsoft systems.
                BaseSemantics::SValuePtr oldStack = incomingState->readRegister(STACK_POINTER_REG, ops.get());
                newStack = ops->add(oldStack, callRetAdjustment_);
            } else {
                // We don't know the callee's delta, therefore we don't know how to adjust the delta for the callee's effect.
                newStack = ops->undefined_(STACK_POINTER_REG.get_nbits());
            }
            ASSERT_not_null(newStack);

            // FIXME[Robb P. Matzke 2014-12-15]: We should also reset any part of the state that might have been modified by
            // the called function(s). Unfortunately we don't have good ABI information at this time, so be permissive and
            // assume that the callee doesn't have any effect on registers except the stack pointer.
            retval->writeRegister(STACK_POINTER_REG, newStack, ops.get());

        } else if (P2::DataFlow::DfCfgVertex::FUNCRET == vertex->value().type()) {
            // Identity semantics; this vertex just merges all the various return blocks in the function.

        } else if (P2::DataFlow::DfCfgVertex::INDET == vertex->value().type()) {
            // We don't know anything about the vertex, therefore we don't know anything about its semantics
            retval->clear();

        } else {
            // Build a new state using the retval created above, then execute instructions to update it.
            ASSERT_require(vertex->value().type() == P2::DataFlow::DfCfgVertex::BBLOCK);
            BaseSemantics::RiscOperatorsPtr ops = cpu_->get_operators();
            BaseSemantics::SValuePtr addrProtoval = ops->get_state()->get_memory_state()->get_addr_protoval();
            BaseSemantics::SValuePtr valProtoval = ops->get_state()->get_memory_state()->get_val_protoval();
            BaseSemantics::MemoryStatePtr memState = ops->get_state()->get_memory_state()->clone();
            memState->clear();
            BaseSemantics::StatePtr fullState = ops->get_state()->create(retval, memState);
            ops->set_state(fullState);
            BOOST_FOREACH (SgAsmInstruction *insn, vertex->value().bblock()->instructions()) {
                BaseSemantics::SValuePtr v = ops->readRegister(STACK_POINTER_REG);
                if (v->is_number() && v->get_width() <= 64) {
                    int64_t delta = IntegerOps::signExtend2<uint64_t>(v->get_number(), v->get_width(), 64);
                    insn->set_stackDelta(delta - initialStackPointer);
                } else {
                    insn->set_stackDelta(SgAsmInstruction::INVALID_STACK_DELTA);
                }
                cpu_->processInstruction(insn);
            }
        }
        return retval;
>>>>>>> Port to VS 2014.
    }
};

BaseSemantics::SValuePtr
Partitioner::functionStackDelta(const Function::Ptr &function) const {
    ASSERT_not_null(function);
    BaseSemantics::SValuePtr retval;
    size_t bitsPerWord = instructionProvider().stackPointerRegister().get_nbits();

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
    BaseSemantics::MemoryStatePtr mem = cpu->get_operators()->get_state()->get_memory_state();
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
        function->stackDeltaOverride(retval);
    }
#endif

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
