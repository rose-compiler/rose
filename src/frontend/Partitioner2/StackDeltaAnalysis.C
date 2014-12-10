#include "sage3basic.h"

#include <BinaryDataFlow.h>
#include <Partitioner2/Partitioner.h>
#include <sawyer/DistinctList.h>
#include <sawyer/GraphTraversal.h>
#include <sawyer/SharedPointer.h>

using namespace rose::Diagnostics;

namespace rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

// Vertex type for the CFG used by dataflow.
struct DfCfgVertex {
    enum Type { BBLOCK, CALLRET, FUNCRET };
    Type type;
    BasicBlock::Ptr bblock;                             // attached to BBLOCK vertices
    std::vector<Function::Ptr> callees;                 // attached to CALLRET vertices
    DfCfgVertex(Type type, const BasicBlock::Ptr &bblock = BasicBlock::Ptr()): type(type), bblock(bblock) {}
};

// Control flow graph used by dataflow analysis. See buildDfCfg for details.
typedef Sawyer::Container::Graph<DfCfgVertex> DfCfg;

// Build the CFG that will be used for dataflow analysis.  The dfCfg will contain vertices and edges within a single function
// and only those which are reachable from the starting vertex.  It does so by using a depth-first traversal of the whole CFG
// and trimming away E_FUNCTION_CALL and E_FUNCTION_XFER edges.  Additionally, an E_CALL_RETURN edge in the CFG becomes an
// edge-vertex-edge sequence in the dfCfg, where the vertex is of type CALLRET.  The transfer function handles CALLRET vertices
// in a special way since they have no instructions.  Also, any basic block which is a function return block will be rewritten
// so its only outgoing edge is to a special FUNCRET vertex, which is a common method for handling dataflow in functions that
// can return from multiple locations.
static DfCfg
buildDfCfg(const ControlFlowGraph &cfg, const ControlFlowGraph::ConstVertexNodeIterator &startVertex,
           const Partitioner &partitioner) {
    using namespace Sawyer::Container;
    using namespace Sawyer::Container::Algorithm;
    Sawyer::Container::Map<size_t, DfCfg::VertexNodeIterator> vmap;
    DfCfg dfCfg;
    DfCfg::VertexNodeIterator functionReturnVertex = dfCfg.vertices().end();
    for (DepthFirstForwardGraphTraversal<const ControlFlowGraph> t(cfg, startVertex, ENTER_EVENTS|LEAVE_EDGE); t; ++t) {
        if (t.event() == ENTER_VERTEX) {
            ControlFlowGraph::ConstVertexNodeIterator v1 = t.vertex();
            if (v1->value().type() == V_BASIC_BLOCK) {
                DfCfg::VertexNodeIterator v2 = dfCfg.insertVertex(DfCfgVertex(DfCfgVertex::BBLOCK, v1->value().bblock()));
                vmap.insert(v1->id(), v2);
                if (partitioner.basicBlockIsFunctionReturn(v1->value().bblock())) {
                    if (functionReturnVertex == dfCfg.vertices().end())
                        functionReturnVertex = dfCfg.insertVertex(DfCfgVertex(DfCfgVertex::FUNCRET));
                    dfCfg.insertEdge(v2, functionReturnVertex);
                    t.skipChildren();
                }
            }
        } else if (t.event() == ENTER_EDGE) {
            if (t.edge()->value().type() == E_FUNCTION_CALL || t.edge()->value().type() == E_FUNCTION_XFER)
                t.skipChildren();
        } else {
            ASSERT_require(t.event() == LEAVE_EDGE);
            ControlFlowGraph::ConstEdgeNodeIterator edge = t.edge();
            DfCfg::VertexNodeIterator source = vmap.getOrElse(edge->source()->id(), dfCfg.vertices().end());
            DfCfg::VertexNodeIterator target = vmap.getOrElse(edge->target()->id(), dfCfg.vertices().end());
            if (source!=dfCfg.vertices().end() && target!=dfCfg.vertices().end()) {
                if (edge->value().type() == E_CALL_RETURN) {
                    DfCfg::VertexNodeIterator crv = dfCfg.insertVertex(DfCfgVertex(DfCfgVertex::CALLRET));
                    dfCfg.insertEdge(source, crv);
                    dfCfg.insertEdge(crv, target);
                    ControlFlowGraph::ConstVertexNodeIterator cfgCaller = t.edge()->source();
                    BOOST_FOREACH (const ControlFlowGraph::EdgeNode &callEdge, cfgCaller->outEdges()) {
                        if (callEdge.value().type()==E_FUNCTION_CALL && callEdge.target()->value().type() == V_BASIC_BLOCK) {
                            if (Function::Ptr callee = callEdge.target()->value().function()) {
                                crv->value().callees.push_back(callee);
                            }
                        }
                    }
                } else {
                    dfCfg.insertEdge(source, target);
                }
            }
        }
    }
    return dfCfg;
}

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
        if (!ours && !theirs) {
            return false;                               // both are BOTTOM (not calculated)
        } else if (!ours) {
            ours = theirs;
            return true;                                // ours is BOTTOM, theirs is not
        } else if (!theirs) {
            return false;                               // theirs is BOTTOM, ours is not
        } else if (!ours->is_number()) {
            ours = ops->undefined_(ours->get_width());  // make sure its a simple TOP, not some arbitrarily complex expression
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
        BOOST_FOREACH (const RegPair &reg_val, get_stored_registers()) {
            const RegisterDescriptor &reg = reg_val.desc;
            const BaseSemantics::SValuePtr &theirs = reg_val.value;
            if (!is_partly_stored(reg)) {
                changed = true;
                writeRegister(reg, theirs, ops_.get());
            } else {
                BaseSemantics::SValuePtr ours = readRegister(reg, ops_.get());
                changed = mergeSValues(ours /*in,out*/, theirs, ops_);
                if (changed)
                    writeRegister(reg, ours, ops_.get());
            }
        }
        return changed;
    }
};

// Dataflow transfer function
class TransferFunction {
    BaseSemantics::DispatcherPtr cpu_;
    BaseSemantics::SValuePtr callRetAdjustment_;
    const RegisterDescriptor STACK_POINTER;
public:
    explicit TransferFunction(const BaseSemantics::DispatcherPtr &cpu, const RegisterDescriptor &stackPointerRegister)
        : cpu_(cpu), STACK_POINTER(stackPointerRegister) {
        size_t adjustment = STACK_POINTER.get_nbits() / 8; // sizeof return address on top of stack
        callRetAdjustment_ = cpu->number_(STACK_POINTER.get_nbits(), adjustment);
    }

    State::Ptr initialState() const {
        BaseSemantics::RiscOperatorsPtr ops = cpu_->get_operators();
        State::Ptr retval = State::instance(ops);
        retval->writeRegister(STACK_POINTER, ops->number_(STACK_POINTER.get_nbits(), 0), ops.get());
        return retval;
    }
    
    State::Ptr operator()(const DfCfg &dfCfg, size_t vertexId, const State::Ptr &incomingState) const {
        State::Ptr retval = State::promote(incomingState->clone());
        DfCfg::ConstVertexNodeIterator vertex = dfCfg.findVertex(vertexId);
        ASSERT_require(vertex != dfCfg.vertices().end());
        if (DfCfgVertex::CALLRET == vertex->value().type) {
            // Adjust the stack pointer as if the function call returned.  If we know the function delta then use it, otherwise
            // assume it just pops the return value.
            BaseSemantics::RiscOperatorsPtr ops = cpu_->get_operators();
            BaseSemantics::SValuePtr delta;
            for (size_t i=0; i<vertex->value().callees.size(); ++i) {
                Function::Ptr callee = vertex->value().callees[i];
                BaseSemantics::SValuePtr calleeDelta = callee->stackDelta().getOptional().orDefault();
                if (0==i) {
                    delta = calleeDelta;
                } else {
                    State::mergeSValues(delta, calleeDelta, ops);
                }
            }
            BaseSemantics::SValuePtr oldStack = incomingState->readRegister(STACK_POINTER, ops.get());
            BaseSemantics::SValuePtr newStack = ops->add(oldStack, delta ? delta : callRetAdjustment_);
            retval->writeRegister(STACK_POINTER, newStack, ops.get());
        } else if (DfCfgVertex::FUNCRET == vertex->value().type) {
            // identity semantics; this vertex just merges all the various return blocks in the function
        } else {
            // Build a new state using the retval created above, then execute instructions to update it.
            ASSERT_require(vertex->value().type == DfCfgVertex::BBLOCK);
            BaseSemantics::RiscOperatorsPtr ops = cpu_->get_operators();
            BaseSemantics::SValuePtr addrProtoval = ops->get_state()->get_memory_state()->get_addr_protoval();
            BaseSemantics::SValuePtr valProtoval = ops->get_state()->get_memory_state()->get_val_protoval();
            BaseSemantics::MemoryStatePtr memState = ops->get_state()->get_memory_state()->create(addrProtoval, valProtoval);
            BaseSemantics::StatePtr fullState = ops->get_state()->create(retval, memState);
            ops->set_state(fullState);
            BOOST_FOREACH (SgAsmInstruction *insn, vertex->value().bblock->instructions()) {
                BaseSemantics::SValuePtr v = ops->readRegister(STACK_POINTER);
                if (v->is_number() && v->get_width() <= 64) {
                    int64_t delta = IntegerOps::signExtend2<uint64_t>(v->get_number(), v->get_width(), 64);
                    insn->set_stackDelta(delta);
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
    DfCfg dfCfg = buildDfCfg(cfg_, cfgStart, *this);
    DfCfg::VertexNodeIterator dfCfgStart = dfCfg.findVertex(0);
    BaseSemantics::DispatcherPtr cpu = newDispatcher(ops);
    DataFlow df(cpu);
    if (mlog[DEBUG]) {
        using namespace Sawyer::Container::Algorithm;
        typedef DepthFirstForwardGraphTraversal<DfCfg> Traversal;
        for (Traversal t(dfCfg, dfCfgStart, ENTER_VERTEX|ENTER_EDGE); t; ++t) {
            if (t.event() == ENTER_VERTEX) {
                mlog[DEBUG] <<"  #" <<t.vertex()->id() <<" [";
                if (BasicBlock::Ptr bb = t.vertex()->value().bblock)
                    mlog[DEBUG] <<" " <<bb->printableName();
                if (t.vertex()->value().type == DfCfgVertex::CALLRET) {
                    mlog[DEBUG] <<" call-ret";
                } else if (t.vertex()->value().type == DfCfgVertex::FUNCRET) {
                    mlog[DEBUG] <<" func-ret";
                }
                mlog[DEBUG] <<" ]\n";
                if (BasicBlock::Ptr bb = t.vertex()->value().bblock) {
                    BOOST_FOREACH (SgAsmInstruction *insn, bb->instructions())
                        mlog[DEBUG] <<"    " <<unparseInstructionWithAddress(insn) <<"\n";
                }
                BOOST_FOREACH (const Function::Ptr &callee, t.vertex()->value().callees)
                    mlog[DEBUG] <<"    returns from " <<callee->printableName() <<"\n";
            } else {
                mlog[DEBUG] <<"  #" <<t.edge()->source()->id() <<" -> #" <<t.edge()->target()->id() <<"\n";
            }
        }
    }

    // Run the dataflow until it reaches a fixed point or fails.  The nature of the state (finite number of registers) and
    // merge function (values form a lattice merged in one direction) ensures that the analysis reaches a fixed point.
    TransferFunction xfer(cpu, instructionProvider_->stackPointerRegister());
    typedef DataFlow::Engine<DfCfg, State::Ptr, TransferFunction> Engine;
    Engine engine(dfCfg, xfer);
    try {
        engine.runToFixedPoint(dfCfgStart->id(), xfer.initialState());
    } catch (const BaseSemantics::Exception&) {
        SAWYER_MESG(trace) <<" = BOTTOM (semantics exception)\n";
        retval = ops->undefined_(bitsPerWord);
        function->stackDelta() = retval;
        return retval;
    }

    // Reset the cached stack delta for all basic blocks
    typedef Sawyer::Container::Algorithm::DepthFirstForwardVertexTraversal<DfCfg> Traversal;
    for (Traversal t(dfCfg, dfCfgStart); t; ++t) {
        State::Ptr stateIn = engine.getInitialState(t.vertex()->id());
        ASSERT_not_null(stateIn);
        State::Ptr stateOut = engine.getFinalState(t.vertex()->id());
        ASSERT_not_null(stateOut);
        BaseSemantics::SValuePtr deltaIn = stateIn->readRegister(instructionProvider_->stackPointerRegister(), ops.get());
        BaseSemantics::SValuePtr deltaOut = stateOut->readRegister(instructionProvider_->stackPointerRegister(), ops.get());
        if (BasicBlock::Ptr bb = t.vertex()->value().bblock) {
            bb->stackDeltaIn() = deltaIn;
            bb->stackDeltaOut() = deltaOut;
        }
        if (DfCfgVertex::FUNCRET == t.vertex()->value().type)
            retval = deltaIn;
    }
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
    for (size_t cgVertexId=0; cgVertexId<nFunctions; ++cgVertexId) {
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
