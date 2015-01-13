#include "sage3basic.h"

#include <Partitioner2/DataFlow.h>
#include <Partitioner2/GraphViz.h>
#include <Partitioner2/Partitioner.h>
#include <sawyer/GraphTraversal.h>
#include <SymbolicSemantics2.h>

using namespace Sawyer::Container;
using namespace Sawyer::Container::Algorithm;

namespace rose {
namespace BinaryAnalysis {
namespace Partitioner2 {
namespace DataFlow {

NotInterprocedural NOT_INTERPROCEDURAL;

// private class
class DfCfgBuilder {
public:
    const Partitioner &partitioner;
    const ControlFlowGraph &cfg;                                 // global control flow graph
    const ControlFlowGraph::ConstVertexNodeIterator startVertex; // where to start in the global CFG
    DfCfg dfCfg;                                                 // dataflow control flow graph we are building
    InterproceduralPredicate &interproceduralPredicate;          // returns true when a call should be inlined

    // maps CFG vertex ID to dataflow vertex
    typedef Sawyer::Container::Map<ControlFlowGraph::ConstVertexNodeIterator, DfCfg::VertexNodeIterator> VertexMap;

    // Info about one function call
    struct CallFrame {
        VertexMap vmap;
        DfCfg::VertexNodeIterator functionReturnVertex;
        bool wasFaked;
        CallFrame(DfCfg &dfCfg): functionReturnVertex(dfCfg.vertices().end()), wasFaked(false) {}
    };

    typedef std::list<CallFrame> CallStack;             // we use a list since there's no default constructor for an iterator
    CallStack callStack;
    size_t maxCallStackSize;
    

    DfCfgBuilder(const Partitioner &partitioner, const ControlFlowGraph &cfg,
                 const ControlFlowGraph::ConstVertexNodeIterator &startVertex, InterproceduralPredicate &predicate)
        : partitioner(partitioner), cfg(cfg), startVertex(startVertex), interproceduralPredicate(predicate),
          maxCallStackSize(4) {}
    
    typedef DepthFirstForwardGraphTraversal<const ControlFlowGraph> CfgTraversal;

    DfCfg::VertexNodeIterator findVertex(const ControlFlowGraph::ConstVertexNodeIterator cfgVertex) {
        ASSERT_require(!callStack.empty());
        CallFrame &callFrame = callStack.back();
        return callFrame.vmap.getOrElse(cfgVertex, dfCfg.vertices().end());
    }

    bool isValidVertex(const DfCfg::VertexNodeIterator &dfVertex) {
        return dfVertex != dfCfg.vertices().end();
    }

    DfCfg::VertexNodeIterator insertVertex(const DfCfgVertex &dfVertex,
                                           const ControlFlowGraph::ConstVertexNodeIterator &cfgVertex) {
        ASSERT_require(!callStack.empty());
        CallFrame &callFrame = callStack.back();
        ASSERT_require(cfgVertex != cfg.vertices().end());
        ASSERT_require(!callFrame.vmap.exists(cfgVertex));
        DfCfg::VertexNodeIterator dfVertexIter = dfCfg.insertVertex(dfVertex);
        callFrame.vmap.insert(cfgVertex, dfVertexIter);
        return dfVertexIter;
    }

    DfCfg::VertexNodeIterator insertVertex(const DfCfgVertex &dfVertex) {
        return dfCfg.insertVertex(dfVertex);
    }

    DfCfg::VertexNodeIterator insertVertex(DfCfgVertex::Type type) {
        return insertVertex(DfCfgVertex(type));
    }

    DfCfg::VertexNodeIterator insertVertex(DfCfgVertex::Type type, const ControlFlowGraph::ConstVertexNodeIterator &cfgVertex) {
        return insertVertex(DfCfgVertex(type), cfgVertex);
    }

    // Insert basic block if it hasn't been already
    DfCfg::VertexNodeIterator findOrInsertBasicBlockVertex(const ControlFlowGraph::ConstVertexNodeIterator &cfgVertex) {
        ASSERT_require(!callStack.empty());
        CallFrame &callFrame = callStack.back();
        ASSERT_require(cfgVertex != cfg.vertices().end());
        ASSERT_require(cfgVertex->value().type() == V_BASIC_BLOCK);
        DfCfg::VertexNodeIterator retval = dfCfg.vertices().end();
        if (!callFrame.vmap.getOptional(cfgVertex).assignTo(retval)) {
            BasicBlock::Ptr bblock = cfgVertex->value().bblock();
            ASSERT_not_null(bblock);
            retval = insertVertex(DfCfgVertex(bblock), cfgVertex);

            // All function return basic blocks will point only to the special FUNCRET vertex.
            if (partitioner.basicBlockIsFunctionReturn(bblock)) {
                if (!isValidVertex(callFrame.functionReturnVertex))
                    callFrame.functionReturnVertex = insertVertex(DfCfgVertex::FUNCRET);
                dfCfg.insertEdge(retval, callFrame.functionReturnVertex);
            }
        }
        return retval;
    }

    // Returns the dfCfg vertex for a CALL return-to vertex, creating it if necessary.  There might be none, in which case the
    // vertex end iterator is returned.
    DfCfg::VertexNodeIterator findOrInsertCallReturnVertex(const ControlFlowGraph::ConstVertexNodeIterator &cfgVertex) {
        ASSERT_require(cfgVertex != cfg.vertices().end());
        ASSERT_require(cfgVertex->value().type() == V_BASIC_BLOCK);
        DfCfg::VertexNodeIterator retval = dfCfg.vertices().end();
        BOOST_FOREACH (const ControlFlowGraph::EdgeNode &edge, cfgVertex->outEdges()) {
            if (edge.value().type() == E_CALL_RETURN) {
                ASSERT_require(edge.target()->value().type() == V_BASIC_BLOCK);
                ASSERT_require2(retval == dfCfg.vertices().end(),
                                edge.target()->value().bblock()->printableName() + " has multiple call-return edges");
                retval = findOrInsertBasicBlockVertex(edge.target());
            }
        }
        return retval;
    }

    // top-level build function.
    DfCfgBuilder& build() {
        callStack.push_back(CallFrame(dfCfg));
        for (CfgTraversal t(cfg, startVertex, ENTER_EVENTS|LEAVE_EDGE); t; ++t) {
            if (t.event() == ENTER_VERTEX) {
                if (t.vertex()->value().type() == V_BASIC_BLOCK) {
                    findOrInsertBasicBlockVertex(t.vertex());
                    if (partitioner.basicBlockIsFunctionReturn(t.vertex()->value().bblock()))
                        t.skipChildren();               // we're handling return successors explicitly
                } else {
                    insertVertex(DfCfgVertex::INDET, t.vertex());
                }
            } else {
                ASSERT_require(t.event()==ENTER_EDGE || t.event()==LEAVE_EDGE);
                ControlFlowGraph::ConstEdgeNodeIterator edge = t.edge();

                if (edge->value().type() == E_CALL_RETURN) {
                    // Do nothing; we handle call-return edges as part of function calls.

                } else if (edge->value().type() == E_FUNCTION_CALL) {
                    if (t.event() == ENTER_EDGE) {
                        DfCfg::VertexNodeIterator callFrom = findVertex(edge->source());
                        ASSERT_require(isValidVertex(callFrom));
                        callStack.push_back(CallFrame(dfCfg));
                        if (callStack.size() <= maxCallStackSize && edge->target()->value().type()==V_BASIC_BLOCK &&
                            interproceduralPredicate(cfg, edge)) {
                            // Incorporate the call into the dfCfg
                            DfCfg::VertexNodeIterator callTo = findOrInsertBasicBlockVertex(edge->target());
                            ASSERT_require(isValidVertex(callTo));
                            dfCfg.insertEdge(callFrom, callTo);
                        } else {
                            callStack.back().wasFaked = true;
                            t.skipChildren();
                        }
                    } else {
                        ASSERT_require(t.event() == LEAVE_EDGE);
                        ASSERT_require(callStack.size()>1);

                        if (!callStack.back().wasFaked) {
                            // Wire up the return from the called function back to the return-to point in the caller.
                            DfCfg::VertexNodeIterator returnFrom = callStack.back().functionReturnVertex;
                            callStack.pop_back();
                            DfCfg::VertexNodeIterator returnTo = findOrInsertCallReturnVertex(edge->source());
                            if (isValidVertex(returnFrom) && isValidVertex(returnTo))
                                dfCfg.insertEdge(returnFrom, returnTo);
                            ASSERT_require(!callStack.empty());
                        } else {
                            // Build the faked-call vertex and wire it up so the CALL goes to the faked-call vertex, which then
                            // flows to the CALL's return-point.
                            callStack.pop_back();
                            Function::Ptr callee;
                            if (edge->target()->value().type() == V_BASIC_BLOCK)
                                callee = edge->target()->value().function();
                            DfCfg::VertexNodeIterator dfSource = findVertex(edge->source());
                            ASSERT_require(isValidVertex(dfSource));
                            DfCfg::VertexNodeIterator faked = insertVertex(DfCfgVertex(callee));
                            dfCfg.insertEdge(dfSource, faked);
                            DfCfg::VertexNodeIterator returnTo = findOrInsertCallReturnVertex(edge->source());
                            if (isValidVertex(returnTo))
                                dfCfg.insertEdge(faked, returnTo);
                        }
                    }
                    
                } else {
                    // Generic edges
                    if (t.event() == LEAVE_EDGE) {
                        DfCfg::VertexNodeIterator dfSource = findVertex(edge->source());
                        ASSERT_require(isValidVertex(dfSource));
                        DfCfg::VertexNodeIterator dfTarget = findVertex(edge->target()); // the called function
                        if (isValidVertex(dfTarget))
                            dfCfg.insertEdge(dfSource, dfTarget);
                    }
                }
            }
        }
        return *this;
    }
};
        
DfCfg
buildDfCfg(const Partitioner &partitioner,
           const ControlFlowGraph &cfg, const ControlFlowGraph::ConstVertexNodeIterator &startVertex,
           InterproceduralPredicate &predicate) {
    return DfCfgBuilder(partitioner, cfg, startVertex, predicate).build().dfCfg;
}

void
dumpDfCfg(std::ostream &out, const DfCfg &dfCfg) {
    out <<"digraph dfCfg {\n";

    BOOST_FOREACH (const DfCfg::VertexNode &vertex, dfCfg.vertices()) {
        out <<vertex.id() <<" [ label=";
        switch (vertex.value().type()) {
            case DfCfgVertex::BBLOCK:
                out <<"\"" <<vertex.value().bblock()->printableName() <<"\"";
                break;
            case DfCfgVertex::FAKED_CALL:
                if (Function::Ptr callee = vertex.value().callee()) {
                    out <<"<fake call to " <<GraphViz::htmlEscape(vertex.value().callee()->printableName()) <<">";
                } else {
                    out <<"\"fake call to indeterminate function\"";
                }
                break;
            case DfCfgVertex::FUNCRET:
                out <<"\"function return\"";
                break;
            case DfCfgVertex::INDET:
                out <<"\"indeterminate\"";
                break;
        }
        out <<" ];\n";
    }

    BOOST_FOREACH (const DfCfg::EdgeNode &edge, dfCfg.edges()) {
        out <<edge.source()->id() <<" -> " <<edge.target()->id() <<";\n";
    }
    
    out <<"}\n";
}

void
State::init() {
    BaseSemantics::SValuePtr protoval = ops_->get_protoval();
    const RegisterDictionary *regdict = ops_->get_state()->get_register_state()->get_register_dictionary();
    BaseSemantics::RegisterStatePtr regs = ops_->get_state()->get_register_state()->create(protoval, regdict);
    BaseSemantics::MemoryStatePtr mem = ops_->get_state()->get_memory_state()->create(protoval, protoval);
    semanticState_ = ops_->get_state()->create(regs, mem);
}

std::ostream&
operator<<(std::ostream &out, const State &state) {
    out <<*state.semanticState();
    return out;
}

bool
State::merge(const Ptr &other) {
    using namespace rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics;
    RegisterStateGenericPtr reg1 = RegisterStateGeneric::promote(semanticState_->get_register_state());
    RegisterStateGenericPtr reg2 = RegisterStateGeneric::promote(other->semanticState_->get_register_state());
    bool registersChanged = mergeRegisterStates(reg1, reg2);

    MemoryCellListPtr mem1 = MemoryCellList::promote(semanticState_->get_memory_state());
    MemoryCellListPtr mem2 = MemoryCellList::promote(other->semanticState_->get_memory_state());
    bool memoryChanged = mergeMemoryStates(mem1, mem2);

    return registersChanged || memoryChanged;
}

bool
State::mergeSValues(BaseSemantics::SValuePtr &dstValue /*in,out*/, const BaseSemantics::SValuePtr &srcValue) const {
    // The calls to ops->undefined(...) are mostly so that we're simplifying TOP as much as possible. We want each TOP
    // value to be distinct from the others so we don't encounter things like "TOP xor TOP = 0", but we also don't want TOP
    // to be arbitrarily complex since that just makes things slower.
    if (!dstValue && !srcValue) {
        return false;                               // both are BOTTOM (not calculated)
    } else if (!dstValue) {
        ASSERT_not_null(srcValue);
        if (srcValue->is_number()) {
            dstValue = srcValue;
        } else {
            dstValue = ops_->undefined_(srcValue->get_width());
        }
        return true;                                // dstValue is BOTTOM, srcValue is not
    } else if (!srcValue) {
        return false;                               // srcValue is BOTTOM, dstValue is not
    } else if (!dstValue->is_number()) {
        dstValue = ops_->undefined_(dstValue->get_width());
        return false;                               // dstValue was already TOP
    } else if (dstValue->must_equal(srcValue)) {
        return false;                               // dstValue == srcValue
    } else {
        dstValue = ops_->undefined_(dstValue->get_width());
        return true;                                // dstValue became TOP
    }
}

bool
State::mergeRegisterStates(const BaseSemantics::RegisterStateGenericPtr &dstState,
                           const BaseSemantics::RegisterStateGenericPtr &srcState) const {
    bool changed = false;
    BOOST_FOREACH (const BaseSemantics::RegisterStateGeneric::RegPair &reg_val, srcState->get_stored_registers()) {
        const RegisterDescriptor &reg = reg_val.desc;
        const BaseSemantics::SValuePtr &srcValue = reg_val.value;
        if (!dstState->is_partly_stored(reg)) {
            changed = true;
            dstState->writeRegister(reg, srcValue, ops_.get());
        } else {
            BaseSemantics::SValuePtr dstValue = dstState->readRegister(reg, ops_.get());
            if (mergeSValues(dstValue /*in,out*/, srcValue)) {
                dstState->writeRegister(reg, dstValue, ops_.get());
                changed = true;
            }
        }
    }
    return changed;
}

bool
State::mergeMemoryStates(const BaseSemantics::MemoryCellListPtr &dstState,
                         const BaseSemantics::MemoryCellListPtr &srcState) const {
    // FIXME[Robb P. Matzke 2015-01-08]: not implemented yet
    dstState->clear();
    return false;
}

// If the expression is an offset from the initial stack register then return the offset, else nothing.
static Sawyer::Optional<int64_t>
isStackAddress(const rose::BinaryAnalysis::InsnSemanticsExpr::TreeNodePtr &expr,
               const BaseSemantics::SValuePtr &initialStackPointer, SMTSolver *solver) {
    using namespace rose::BinaryAnalysis::InsnSemanticsExpr;
    using namespace rose::BinaryAnalysis::InstructionSemantics2;

    if (!initialStackPointer)
        return Sawyer::Nothing();

    InternalNodePtr inode = expr->isInternalNode();
    if (!inode || inode->get_operator() != OP_ADD || inode->nchildren()!=2)
        return Sawyer::Nothing();

    LeafNodePtr variable = inode->child(0)->isLeafNode();
    LeafNodePtr constant = inode->child(1)->isLeafNode();
    if (!constant || !constant->is_known())
        std::swap(variable, constant);
    if (!constant || !constant->is_known())
        return Sawyer::Nothing();
    if (!variable || !variable->is_variable())
        return Sawyer::Nothing();

    TreeNodePtr initialStack = SymbolicSemantics::SValue::promote(initialStackPointer)->get_expression();
    if (!variable->must_equal(initialStack, solver))
        return Sawyer::Nothing();

    int64_t val = IntegerOps::signExtend2(constant->get_value(), constant->get_nbits(), 64);
    return val;
}

StackVariables
State::findStackVariables(const BaseSemantics::SValuePtr &initialStackPointer) const {
    using namespace rose::BinaryAnalysis::InstructionSemantics2;
    ASSERT_not_null(initialStackPointer);
    SMTSolver *solver = ops_->get_solver();             // might be null

    // What is the word size for this architecture?  We'll assume the word size is the same as the width of the stack pointer,
    // whose value we have in initialStackPointer.
    ASSERT_require2(initialStackPointer->get_width() % 8 == 0, "stack pointer width is not an integral number of bytes");
    size_t wordNBytes = initialStackPointer->get_width() / 8;

    // Find groups of consecutive addresses that were written to by the same instruction. This is how we coalesce adjacent
    // bytes into larger variables.
    typedef Sawyer::Container::Interval<int64_t> OffsetInterval;
    typedef Sawyer::Container::IntervalMap<OffsetInterval /*stack_offset*/, rose_addr_t /*writer_addr*/> StackWriters;
    StackWriters stackWriters;
    typedef Sawyer::Container::Map<int64_t, BaseSemantics::SValuePtr> OffsetAddress;
    OffsetAddress offsetAddress;                        // symbolic address for every byte found
    BaseSemantics::MemoryCellListPtr memState = BaseSemantics::MemoryCellList::promote(semanticState_->get_memory_state());
    BOOST_REVERSE_FOREACH (const BaseSemantics::MemoryCellPtr &cell, memState->get_cells()) {
        SymbolicSemantics::SValuePtr addr = SymbolicSemantics::SValue::promote(cell->get_address());
        ASSERT_require2(0 == cell->get_value()->get_width() % 8, "memory must be byte addressable");
        size_t nBytes = cell->get_value()->get_width() / 8;
        ASSERT_require(nBytes > 0);
        if (Sawyer::Optional<int64_t> stackOffset = isStackAddress(addr->get_expression(), initialStackPointer, solver)) {
            stackWriters.insert(OffsetInterval::baseSize(*stackOffset, nBytes), cell->latestWriter().orElse(0));
            offsetAddress.insert(*stackOffset, addr);
        }
    }

    // Organize the intervals into a list of abstract locations.
    std::vector<StackVariable> retval;
    BOOST_FOREACH (const OffsetInterval &interval, stackWriters.intervals()) {
        int64_t offset=interval.least();
        size_t nRemaining = interval.size();
        while (nRemaining > 0) {
            BaseSemantics::SValuePtr addr = offsetAddress.get(interval.least());
            size_t nBytes = 0;
            if (offset < 0 && offset + nRemaining > 0) {
                // Never create a variable that spans memory below and above (or equal to) the initial stack pointer. The
                // initial stack pointer is generally the boundary between local variables and function arguments (we're
                // considering the call-return address to be an argument on machines that pass it on the stack).
                nBytes = -offset;
            } else if (nBytes > wordNBytes) {
                nBytes = wordNBytes;
            } else {
                nBytes = nRemaining;
            }
            ASSERT_require(nBytes>0 && nBytes<=nRemaining);
            retval.push_back(StackVariable(offset, nBytes, addr));
            offset += nBytes;
            nRemaining -= nBytes;
        }
    }
    return retval;
}

StackVariables
State::findLocalVariables(const BaseSemantics::SValuePtr &initialStackPointer) const {
    StackVariables vars = findStackVariables(initialStackPointer);
    StackVariables retval;
    BOOST_FOREACH (const StackVariable &var, vars) {
        if (var.offset < 0)
            retval.push_back(var);
    }
    return retval;
}

StackVariables
State::findFunctionArguments(const BaseSemantics::SValuePtr &initialStackPointer) const {
    StackVariables vars = findStackVariables(initialStackPointer);
    StackVariables retval;
    BOOST_FOREACH (const StackVariable &var, vars) {
        if (var.offset >= 0)
            retval.push_back(var);
    }
    return retval;
}
    
// Construct a new state from scratch
State::Ptr
TransferFunction::initialState() const {
    BaseSemantics::RiscOperatorsPtr ops = cpu_->get_operators();
    State::Ptr state = State::instance(ops);
    BaseSemantics::RegisterStateGeneric::promote(state->semanticState()->get_register_state())->initialize_large();
    return state;
}

// Required by dataflow engine: compute new output state given a vertex and input state.
State::Ptr
TransferFunction::operator()(const DfCfg &dfCfg, size_t vertexId, const State::Ptr &incomingState) const {
    BaseSemantics::RiscOperatorsPtr ops = cpu_->get_operators();
    State::Ptr retval = incomingState->clone();
    ops->set_state(retval->semanticState());

    DfCfg::ConstVertexNodeIterator vertex = dfCfg.findVertex(vertexId);
    ASSERT_require(vertex != dfCfg.vertices().end());
    if (DfCfgVertex::FAKED_CALL == vertex->value().type()) {
        // Adjust the stack pointer as if the function call returned.  If we know the function delta then use it, otherwise
        // assume it just pops the return value.
        BaseSemantics::SValuePtr delta;
        if (Function::Ptr callee = vertex->value().callee())
            delta = callee->stackDelta().getOptional().orDefault();

        // Update the result state
        BaseSemantics::SValuePtr newStack;
        if (delta) {
            BaseSemantics::SValuePtr oldStack = ops->readRegister(STACK_POINTER_REG);
            newStack = ops->add(oldStack, delta);
        } else if (false) { // FIXME[Robb P. Matzke 2014-12-15]: should only apply if caller cleans up arguments
            // We don't know the callee's delta, so assume that the callee pops only its return address. This is usually
            // the correct for caller-cleanup ABIs common on Unix/Linux, but not usually correct for callee-cleanup ABIs
            // common on Microsoft systems.
            BaseSemantics::SValuePtr oldStack = ops->readRegister(STACK_POINTER_REG);
            newStack = ops->add(oldStack, callRetAdjustment_);
        } else {
            // We don't know the callee's delta, therefore we don't know how to adjust the delta for the callee's effect.
            newStack = ops->undefined_(STACK_POINTER_REG.get_nbits());
        }
        ASSERT_not_null(newStack);

        // FIXME[Robb P. Matzke 2014-12-15]: We should also reset any part of the state that might have been modified by
        // the called function(s). Unfortunately we don't have good ABI information at this time, so be permissive and
        // assume that the callee doesn't have any effect on registers except the stack pointer.
        ops->writeRegister(STACK_POINTER_REG, newStack);

    } else if (DfCfgVertex::FUNCRET == vertex->value().type()) {
        // Identity semantics; this vertex just merges all the various return blocks in the function.

    } else if (DfCfgVertex::INDET == vertex->value().type()) {
        // We don't know anything about the vertex, therefore we don't know anything about its semantics
        retval->clear();

    } else {
        // Build a new state using the retval created above, then execute instructions to update it.
        ASSERT_require(vertex->value().type() == DfCfgVertex::BBLOCK);
        ASSERT_not_null(vertex->value().bblock());
        BOOST_FOREACH (SgAsmInstruction *insn, vertex->value().bblock()->instructions())
            cpu_->processInstruction(insn);
    }
    return retval;
}

} // namespace
} // namespace
} // namespace
} // namespace
