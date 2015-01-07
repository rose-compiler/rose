#include "sage3basic.h"

#include <BinaryDataFlow.h>
#include <Partitioner2/DataFlow.h>
#include <Partitioner2/Partitioner.h>
#include <sawyer/ProgressBar.h>
#include <sawyer/SharedPointer.h>

using namespace rose::Diagnostics;
namespace P2 = rose::BinaryAnalysis::Partitioner2;

namespace rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

// Determines when to perform interprocedural dataflow.  We want stack delta analysis to be interprocedural only if the called
// function has no stack delta.
struct InterproceduralPredicate: P2::DataFlow::InterproceduralPredicate {
    const Partitioner &partitioner;
    InterproceduralPredicate(const Partitioner &partitioner): partitioner(partitioner) {}
    bool operator()(const ControlFlowGraph &cfg, const ControlFlowGraph::ConstEdgeNodeIterator &callEdge) {
        ASSERT_require(callEdge != cfg.edges().end());
        ASSERT_require(callEdge->target()->value().type() == V_BASIC_BLOCK);
        Function::Ptr function = callEdge->target()->value().function();
        return function && !function->stackDelta().getOptional().orDefault();
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
        P2::DataFlow::DfCfg::ConstVertexNodeIterator vertex = dfCfg.findVertex(vertexId);
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
            if (delta) {
                BaseSemantics::SValuePtr oldStack = incomingState->readRegister(STACK_POINTER_REG, ops.get());
                newStack = ops->add(oldStack, delta);
            } else if (false) { // FIXME[Robb P. Matzke 2014-12-15]: should only apply if caller cleans up arguments
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
            BaseSemantics::MemoryStatePtr memState = ops->get_state()->get_memory_state()->create(addrProtoval, valProtoval);
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
    }
};

void
Partitioner::functionStackDelta(const std::string &functionName, int64_t delta) {
    if (delta == SgAsmInstruction::INVALID_STACK_DELTA) {
        stackDeltaMap_.erase(functionName);
    } else {
        stackDeltaMap_.insert(functionName, delta);
    }
}

int64_t
Partitioner::functionStackDelta(const std::string &functionName) const {
    int64_t delta = SgAsmInstruction::INVALID_STACK_DELTA;
    if (!stackDeltaMap_.getOptional(functionName).assignTo(delta))
        delta = instructionProvider_->instructionPointerRegister().get_nbits() / 8;
    return delta;
}

BaseSemantics::SValuePtr
Partitioner::functionStackDelta(const Function::Ptr &function) const {
    ASSERT_not_null(function);
    const size_t bitsPerWord = instructionProvider_->instructionPointerRegister().get_nbits();

    BaseSemantics::SValuePtr retval;
    if (function->stackDelta().getOptional().assignTo(retval))
        return retval;                                  // already cached

    Sawyer::Message::Stream trace(mlog[TRACE]);
    SAWYER_MESG(trace) <<"functionCalculateStackDeltas(" <<function->printableName() <<")";

    // Use (and cache) a predetermined delta if one's available.
    BaseSemantics::RiscOperatorsPtr ops = newOperators();
    if (Sawyer::Optional<int64_t> delta = stackDeltaMap_.getOptional(function->name())) {
        retval = ops->number_(bitsPerWord, *delta);
        function->stackDelta() = retval;
        SAWYER_MESG(trace) <<"  stack delta is defined as " <<*delta <<" in a lookup table\n";
        return retval;
    }
    
    // Create the CFG that we'll use for dataflow.
    ControlFlowGraph::ConstVertexNodeIterator cfgStart = findPlaceholder(function->address());
    if (cfgStart == cfg_.vertices().end()) {
        SAWYER_MESG(mlog[ERROR]) <<"functionStackDeltas: " <<function->printableName()
                                 <<" entry block is not attached to the CFG/AUM\n";
        function->stackDelta() = retval;                // null
        return retval;
    }
    if (cfgStart->nOutEdges()==1 && cfgStart->outEdges().begin()->target()==nonexistingVertex_) {
        // This is probably a library function that isn't linked and whose name didn't appear in the lookup table above. Assume
        // the function pops a return address.
        retval = ops->number_(bitsPerWord, bitsPerWord/8);
        function->stackDelta() = retval;
        SAWYER_MESG(mlog[DEBUG]) <<"  using stack delta default (" <<(bitsPerWord/8) <<") for non-existing function\n";
        return retval;
    }
    InterproceduralPredicate ip(*this);
    P2::DataFlow::DfCfg dfCfg = P2::DataFlow::buildDfCfg(*this, cfg_, cfgStart, ip);
#if 1 // DEBUGGING [Robb P. Matzke 2015-01-06]
    if (function->address() == 0x080480cf) {
        std::ofstream graphViz("x.dot");
        P2::DataFlow::dumpDfCfg(graphViz, dfCfg);
    }
#endif
    P2::DataFlow::DfCfg::VertexNodeIterator dfCfgStart = dfCfg.findVertex(0);
    BaseSemantics::DispatcherPtr cpu = newDispatcher(ops);
    BinaryAnalysis::DataFlow df(cpu);

    // Dump the CFG for debugging
    if (mlog[DEBUG]) {
        using namespace Sawyer::Container::Algorithm;
        BOOST_FOREACH (const P2::DataFlow::DfCfg::VertexNode &vertex, dfCfg.vertices()) {
            mlog[DEBUG] <<"  Vertex #" <<vertex.id() <<" [";
            if (BasicBlock::Ptr bb = vertex.value().bblock())
                mlog[DEBUG] <<" " <<bb->printableName();
            if (vertex.value().type() == P2::DataFlow::DfCfgVertex::FAKED_CALL) {
                mlog[DEBUG] <<" faked call to " <<
                    (vertex.value().callee() ? vertex.value().callee()->printableName() : std::string("indeterminate"));
            } else if (vertex.value().type() == P2::DataFlow::DfCfgVertex::FUNCRET) {
                mlog[DEBUG] <<" func-ret";
            } else if (vertex.value().type() == P2::DataFlow::DfCfgVertex::INDET) {
                mlog[DEBUG] <<" indeterminate";
            }
            mlog[DEBUG] <<" ]\n";
            if (BasicBlock::Ptr bb = vertex.value().bblock()) {
                BOOST_FOREACH (SgAsmInstruction *insn, bb->instructions())
                    mlog[DEBUG] <<"      " <<unparseInstructionWithAddress(insn) <<"\n";
            }
            mlog[DEBUG] <<"    successor vertices {";
            BOOST_FOREACH (const P2::DataFlow::DfCfg::EdgeNode &edge, vertex.outEdges())
                mlog[DEBUG] <<" " <<edge.target()->id();
            mlog[DEBUG] <<" }\n";
        }
    }

    // Run the dataflow until it reaches a fixed point or fails.  The nature of the state (finite number of registers) and
    // merge function (values form a lattice merged in one direction) ensures that the analysis reaches a fixed point.
    TransferFunction xfer(cpu, instructionProvider_->stackPointerRegister());
    typedef BinaryAnalysis::DataFlow::Engine<P2::DataFlow::DfCfg, State::Ptr, TransferFunction> Engine;
    Engine engine(dfCfg, xfer);
    try {
        engine.runToFixedPoint(dfCfgStart->id(), xfer.initialState());
    } catch (const BaseSemantics::Exception&) {
        SAWYER_MESG(trace) <<" = BOTTOM (semantics exception)\n";
        retval = ops->undefined_(bitsPerWord);
        function->stackDelta() = retval;
        return retval;
    }

    // Get the initial stack pointer for the function.  We can't get it from the input state for the starting vertex because
    // the the dataflow might have merged some output states into that also, so get it from a fresh initial state.  The initial
    // stack register should be concrete, otherwise we might have problems simplifying things.
    BaseSemantics::SValuePtr initialStackPointer = xfer.initialState()
                                                   ->readRegister(instructionProvider_->stackPointerRegister(), ops.get());
    ASSERT_require(initialStackPointer->is_number());

    // Reset the cached stack delta for all basic blocks
    bool branchedToIndeterminate = false;
    typedef Sawyer::Container::Algorithm::DepthFirstForwardVertexTraversal<P2::DataFlow::DfCfg> Traversal;
    for (Traversal t(dfCfg, dfCfgStart); t; ++t) {
        if (t.vertex()->value().type() == P2::DataFlow::DfCfgVertex::INDET)
            branchedToIndeterminate = true;

        // Get incoming and outgoing stack pointer for the vertex
        State::Ptr stateIn = engine.getInitialState(t.vertex()->id());
        ASSERT_not_null(stateIn);
        State::Ptr stateOut = engine.getFinalState(t.vertex()->id());
        ASSERT_not_null(stateOut);
        BaseSemantics::SValuePtr spIn = stateIn->readRegister(instructionProvider_->stackPointerRegister(), ops.get());
        BaseSemantics::SValuePtr spOut = stateOut->readRegister(instructionProvider_->stackPointerRegister(), ops.get());

        // Deltas are the stack pointers minus the initial stack pointer for the function.
        BaseSemantics::SValuePtr deltaIn = ops->subtract(spIn, initialStackPointer);
        BaseSemantics::SValuePtr deltaOut = ops->subtract(spOut, initialStackPointer);

        // Simplify abstract deltas so they're just a single variable. This could save a lot of memory.
        if (!deltaIn->is_number())
            deltaIn = ops->undefined_(deltaIn->get_width());
        if (!deltaOut->is_number())
            deltaOut = ops->undefined_(deltaOut->get_width());

        // Cache results
        if (BasicBlock::Ptr bb = t.vertex()->value().bblock()) {
            bb->stackDeltaIn() = deltaIn;
            bb->stackDeltaOut() = deltaOut;
        }
        if (P2::DataFlow::DfCfgVertex::FUNCRET == t.vertex()->value().type())
            retval = deltaIn;
    }

    // If any basic blocks branched to an indeterminate location then we cannot know the stack delta. The indeterminate
    // location might have eventually branched back into this function with an arbitrary stack delta that should have poisoned
    // the result.
    if (branchedToIndeterminate) {
        SAWYER_MESG(trace) <<"  saw an indeterminate edge, therefore result must be TOP\n";
        retval = ops->undefined_(instructionProvider_->stackPointerRegister().get_nbits());
    }

    // Debugging
    if (trace) {
        if (!retval) {
            trace <<" = BOTTOM\n";
        } else if (retval->is_number()) {
            int64_t offset = IntegerOps::signExtend2<uint64_t>(retval->get_number(), retval->get_width(), 64);
            trace <<" = " <<offset <<"\n";
        } else {
            trace <<" = TOP (" <<*retval <<")\n";
        }
    }

    function->stackDelta() = retval;
    return retval;
}

// Compute stack deltas for all basic blocks in all functions, and for functions overall. Functions are processed in an order
// so that callees are before callers.
void
Partitioner::allFunctionStackDelta() const {
    using namespace Sawyer::Container::Algorithm;
    FunctionCallGraph cg = functionCallGraph();
    size_t nFunctions = cg.graph().nVertices();
    std::vector<bool> visited(nFunctions, false);
    Sawyer::ProgressBar<size_t> progress(nFunctions, mlog[MARCH], "stack-delta analysis");
    for (size_t cgVertexId=0; cgVertexId<nFunctions; ++cgVertexId, ++progress) {
        if (!visited[cgVertexId]) {
            typedef DepthFirstForwardGraphTraversal<const FunctionCallGraph::Graph> Traversal;
            for (Traversal t(cg.graph(), cg.graph().findVertex(cgVertexId), ENTER_VERTEX|LEAVE_VERTEX); t; ++t) {
                if (t.event() == ENTER_VERTEX) {
                    if (visited[t.vertex()->id()])
                        t.skipChildren();
                } else {
                    ASSERT_require(t.event() == LEAVE_VERTEX);
                    functionStackDelta(t.vertex()->value());
                    visited[t.vertex()->id()] = true;
                }
            }
        }
    }
}

} // namespace
} // namespace
} // namespace
