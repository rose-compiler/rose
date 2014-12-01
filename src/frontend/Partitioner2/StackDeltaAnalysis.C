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
    BasicBlock::Ptr bblock;
    std::vector<Function::Ptr> callees;
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
            if (t.edge()->value().type() == E_FUNCTION_CALL || t.edge()->value().type() == E_FUNCTION_XFER) {
                DfCfg::VertexNodeIterator dfCaller = vmap.getOrElse(t.edge()->source()->id(), dfCfg.vertices().end());
                ControlFlowGraph::ConstVertexNodeIterator cfgCallee = t.edge()->target();
                Function::Ptr callee;
                if (cfgCallee->value().type() == V_BASIC_BLOCK && (callee=cfgCallee->value().function()))
                    dfCaller->value().callees.push_back(callee);
                t.skipChildren();
            }
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
        if (!ours->is_number() || !theirs->is_number()) {
            if (ours->is_number()) {
                return true;
            } else {
                // Make sure non-concrete values are free variables otherwise it could affect subsequent computations
                // since we're not doing a proper merge.
                using namespace rose::BinaryAnalysis::InstructionSemantics2;
                SymbolicSemantics::SValuePtr oursSymbolic = SymbolicSemantics::SValue::promote(ours);
                InsnSemanticsExpr::LeafNodePtr leaf = oursSymbolic->get_expression()->isLeafNode();
                if (!leaf || !leaf->is_variable()) {
                    ours = ops->undefined_(ours->get_width());
                    return true;
                }
            }
        } else if (!ours->must_equal(theirs)) { // concrete equality (fast, no SMT solver involved)
            ours = ops->undefined_(ours->get_width());
            return true;
        }
        return false;
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
            BOOST_FOREACH (const Function::Ptr &callee, vertex->value().callees) {
                BaseSemantics::SValuePtr calleeDelta;
                if (callee->stackDelta().getOptional().assignTo(calleeDelta)) {
                    State::mergeSValues(delta, calleeDelta, ops);
                    if (!delta->is_number())
                        break;
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

BaseSemantics::SValuePtr
Partitioner::functionStackDelta(const Function::Ptr &function) const {
    ASSERT_not_null(function);

    BaseSemantics::SValuePtr retval;
    if (function->stackDelta().getOptional().assignTo(retval))
        return retval;                                  // already cached

    Sawyer::Message::Stream trace(mlog[TRACE]);
    SAWYER_MESG(trace) <<"functionCalculateStackDeltas(" <<function->printableName() <<")";

    // Create the CFG that we'll use for dataflow.
    ControlFlowGraph::ConstVertexNodeIterator cfgStart = findPlaceholder(function->address());
    if (cfgStart == cfg_.vertices().end()) {
        SAWYER_MESG(mlog[ERROR]) <<"functionStackDeltas: " <<function->printableName()
                                 <<" entry block is not attached to the CFG/AUM\n";
        function->stackDelta() = retval;                // null
        return retval;
    }
    DfCfg dfCfg = buildDfCfg(cfg_, cfgStart, *this);
    DfCfg::VertexNodeIterator dfCfgStart = dfCfg.findVertex(0);

    // Get the list of variables over which dataflow is calculated
    BaseSemantics::RiscOperatorsPtr ops = newOperators();
    BaseSemantics::DispatcherPtr cpu = newDispatcher(ops);
    DataFlow df(cpu);

    // Build the dataflow engine
    TransferFunction xfer(cpu, instructionProvider_->stackPointerRegister());
    typedef DataFlow::Engine<DfCfg, State::Ptr, TransferFunction> Engine;
    Engine engine(dfCfg, xfer);
    try {
        engine.runToFixedPoint(dfCfgStart->id(), xfer.initialState());
    } catch (const BaseSemantics::Exception&) {
        SAWYER_MESG(trace) <<" = BOTTOM (semantics exception)\n";
        function->stackDelta() = retval;                // null
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
