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
    size_t maxCallStackSize;                            // safety to prevent infinite recursion
    

    DfCfgBuilder(const Partitioner &partitioner, const ControlFlowGraph &cfg,
                 const ControlFlowGraph::ConstVertexNodeIterator &startVertex, InterproceduralPredicate &predicate)
        : partitioner(partitioner), cfg(cfg), startVertex(startVertex), interproceduralPredicate(predicate),
          maxCallStackSize(10) {}
    
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
                            interproceduralPredicate(cfg, edge, callStack.size())) {
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
    // clone+clear might be slower than creating a new state from scratch, but its simpler and it makes sure that any other
    // state configuration that might be present (like pointers to memory maps) will be initialized properly regardless of the
    // state subtype.
    semanticState_ = ops_->get_state()->clone();
    semanticState_->clear();
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
State::mergeDefiners(BaseSemantics::SValuePtr &dstValueBase /*in,out*/, const BaseSemantics::SValuePtr &srcValueBase) const {
    using namespace rose::BinaryAnalysis::InstructionSemantics2;
    SymbolicSemantics::SValuePtr dstValue = SymbolicSemantics::SValue::promote(dstValueBase);
    SymbolicSemantics::SValuePtr srcValue = SymbolicSemantics::SValue::promote(srcValueBase);
    SymbolicSemantics::InsnSet dstDefiners = dstValue->get_defining_instructions();
    SymbolicSemantics::InsnSet srcDefiners = srcValue->get_defining_instructions();
    if (dstDefiners.size() == srcDefiners.size() && std::equal(dstDefiners.begin(), dstDefiners.end(), srcDefiners.begin()))
        return false;                                   // no change
    dstValue->add_defining_instructions(srcDefiners);
    return true;
}

bool
State::mergeSValues(BaseSemantics::SValuePtr &dstValue /*in,out*/, const BaseSemantics::SValuePtr &srcValue) const {
    // The calls to ops->undefined(...) are mostly so that we're simplifying TOP as much as possible. We want each TOP
    // value to be distinct from the others so we don't encounter things like "TOP xor TOP = 0", but we also don't want TOP
    // to be arbitrarily complex since that just makes things slower.
    if (!dstValue && !srcValue) {                       // both are BOTTOM (not calculated)
        return false;
    } else if (!dstValue) {                             // dstValue is BOTTOM, srcValue is not
        ASSERT_not_null(srcValue);
        if (srcValue->is_number()) {
            dstValue = srcValue;
        } else {
            dstValue = ops_->undefined_(srcValue->get_width());
        }
        return true;
    } else if (!srcValue) {                             // srcValue is BOTTOM, dstValue is not
        return false;
    } else if (!dstValue->is_number()) {                // dstValue was already TOP
        BaseSemantics::SValuePtr merged = ops_->undefined_(dstValue->get_width());
        mergeDefiners(merged /*in,out*/, dstValue);
        dstValue = merged;
        return mergeDefiners(dstValue /*in,out*/, srcValue);
    } else if (dstValue->must_equal(srcValue)) {        // dstValue == srcValue
        return mergeDefiners(dstValue, srcValue);
    } else {                                            // dstValue becomes TOP
        BaseSemantics::SValuePtr merged = ops_->undefined_(dstValue->get_width());
        mergeDefiners(merged /*in,out*/, dstValue);
        dstValue = merged;
        mergeDefiners(dstValue /*in,out*/, srcValue);
        return true;
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
            dstState->writeRegister(reg, ops_->undefined_(reg.get_nbits()), ops_.get());
        } else {
            BaseSemantics::SValuePtr dstValue = dstState->readRegister(reg, ops_.get());
            if (mergeSValues(dstValue /*in,out*/, srcValue)) {
                dstState->writeRegister(reg, dstValue, ops_.get());
                changed = true;
            }
        }
        // We should adjust latestWriter also, but unfortunately we can only store one writer per bit of the register's value,
        // and it's hard to get information about the individual bits.  So we'll just clear all the writer info all the time so
        // users don't depend on it.
        dstState->clear_latest_writer(reg);
    }
    return changed;
}

bool
State::mergeMemoryStates(const BaseSemantics::MemoryCellListPtr &dstState,
                         const BaseSemantics::MemoryCellListPtr &srcState) const {
    using namespace rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics;
    bool changed = false;
    BOOST_REVERSE_FOREACH (const MemoryCellPtr &srcCell, srcState->get_cells()) {
        // Get the source and destination values
        SValuePtr srcValue = ops_->undefined_(8);
        srcValue = srcState->readMemory(srcCell->get_address(), srcValue, ops_.get(), ops_.get());
        bool shortCircuited = false;
        if (dstState->scan(srcCell->get_address(), 8, ops_.get(), ops_.get(), shortCircuited /*out*/).empty()) {
            // dstState doesn't even know about this address, so consider it undefined (i.e., TOP)
            dstState->writeMemory(srcCell->get_address(), ops_->undefined_(8), ops_.get(), ops_.get());
            changed = true;
        } else {
            // dstState has this address, so merge the src and dst values
            SValuePtr dstValue = dstState->readMemory(srcCell->get_address(), ops_->undefined_(8), ops_.get(), ops_.get());
            if (mergeSValues(dstValue /*in,out*/, srcValue)) {
                dstState->writeMemory(srcCell->get_address(), dstValue, ops_.get(), ops_.get());
                changed = true;
            }

            // Merge writer sets (but do not set changed=true because writer sets don't form a lattice and might not
            // converge to a fixed point during the dataflow loop).
            std::set<rose_addr_t> srcWriters = srcState->get_latest_writers(srcCell->get_address(), 8, ops_.get(), ops_.get());
            std::set<rose_addr_t> dstWriters = dstState->get_latest_writers(srcCell->get_address(), 8, ops_.get(), ops_.get());
            if (srcWriters.size()!=dstWriters.size() || !std::equal(srcWriters.begin(), srcWriters.end(), dstWriters.begin())) {
                MemoryCellList::CellList dstCells = dstState->scan(srcCell->get_address(), 8, ops_.get(), ops_.get(),
                                                                   shortCircuited /*out*/);
                BOOST_FOREACH (const MemoryCellPtr &dstCell, dstCells)
                    dstCell->clearLatestWriter();
            }
        }
    }
    return changed;
}

// If the expression is an offset from the initial stack register then return the offset, else nothing.
static Sawyer::Optional<int64_t>
isStackAddress(const rose::BinaryAnalysis::InsnSemanticsExpr::TreeNodePtr &expr,
               const BaseSemantics::SValuePtr &initialStackPointer, SMTSolver *solver) {
    using namespace rose::BinaryAnalysis::InsnSemanticsExpr;
    using namespace rose::BinaryAnalysis::InstructionSemantics2;

    if (!initialStackPointer)
        return Sawyer::Nothing();
    TreeNodePtr initialStack = SymbolicSemantics::SValue::promote(initialStackPointer)->get_expression();

    // Special case where (add SP0 0) is simplified to SP0
    LeafNodePtr variable = expr->isLeafNode();
    if (variable && variable->must_equal(initialStack, solver))
        return 0;

    // Otherwise the expression must be (add SP0 N) where N != 0
    InternalNodePtr inode = expr->isInternalNode();
    if (!inode || inode->get_operator() != OP_ADD || inode->nchildren()!=2)
        return Sawyer::Nothing();

    variable = inode->child(0)->isLeafNode();
    LeafNodePtr constant = inode->child(1)->isLeafNode();
    if (!constant || !constant->is_known())
        std::swap(variable, constant);
    if (!constant || !constant->is_known())
        return Sawyer::Nothing();
    if (!variable || !variable->is_variable())
        return Sawyer::Nothing();

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
    int64_t wordNBytes = initialStackPointer->get_width() / 8;
    ASSERT_require2(wordNBytes>0, "overflow");

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

    // Organize the intervals into a list of stack variables
    std::vector<StackVariable> retval;
    BOOST_FOREACH (const OffsetInterval &interval, stackWriters.intervals()) {
        int64_t offset = interval.least();
        int64_t nRemaining = interval.size();
        ASSERT_require2(nRemaining>0, "overflow");
        while (nRemaining > 0) {
            BaseSemantics::SValuePtr addr = offsetAddress.get(offset);
            int64_t nBytes = nRemaining;
            if (offset < 0 && offset + nBytes > 0) {
                // Never create a variable that spans memory below and above (or equal to) the initial stack pointer. The
                // initial stack pointer is generally the boundary between local variables and function arguments (we're
                // considering the call-return address to be an argument on machines that pass it on the stack).
                nBytes = -offset;
            }
            nBytes = std::min(nBytes, wordNBytes);
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

std::vector<AbstractLocation>
State::findGlobalVariables(size_t wordNBytes) const {
    using namespace rose::BinaryAnalysis::InstructionSemantics2;
    ASSERT_require(wordNBytes>0);
    BaseSemantics::MemoryCellListPtr memState = BaseSemantics::MemoryCellList::promote(semanticState_->get_memory_state());

    // Find groups of consecutive addresses that were written to by the same instruction.  This is how we coalesce adjacent
    // bytes into larger variables.
    typedef Sawyer::Container::IntervalMap<AddressInterval, rose_addr_t /*writer*/> StackWriters;
    typedef Sawyer::Container::Map<rose_addr_t, BaseSemantics::SValuePtr> SymbolicAddresses;
    StackWriters stackWriters;
    SymbolicAddresses symbolicAddrs;
    BOOST_REVERSE_FOREACH (const BaseSemantics::MemoryCellPtr &cell, memState->get_cells()) {
        ASSERT_require2(0 == cell->get_value()->get_width() % 8, "memory must be byte addressable");
        size_t nBytes = cell->get_value()->get_width() / 8;
        ASSERT_require(nBytes > 0);
        if (cell->get_address()->is_number() && cell->get_address()->get_width()<=64) {
            rose_addr_t va = cell->get_address()->get_number();
            stackWriters.insert(AddressInterval::baseSize(va, nBytes), cell->latestWriter().orElse(0));
            symbolicAddrs.insert(va, cell->get_address());
        }
    }

    // Organize the intervals into a list of global variables
    std::vector<AbstractLocation> retval;
    BOOST_FOREACH (const AddressInterval &interval, stackWriters.intervals()) {
        rose_addr_t va = interval.least();
        rose_addr_t nRemaining = interval.size();
        ASSERT_require2(nRemaining>0, "overflow");
        while (nRemaining > 0) {
            BaseSemantics::SValuePtr addr = symbolicAddrs.get(va);
            rose_addr_t nBytes = std::min(nRemaining, wordNBytes);
            retval.push_back(AbstractLocation(addr, nBytes));
            va += nBytes;
            nRemaining -= nBytes;
        }
    }
    return retval;
}

// Construct a new state from scratch
State::Ptr
TransferFunction::initialState() const {
    namespace BS = rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics;
    BS::RiscOperatorsPtr ops = cpu_->get_operators();
    State::Ptr state = State::instance(ops);
    BS::RegisterStateGenericPtr regState = BS::RegisterStateGeneric::promote(state->semanticState()->get_register_state());

    // Any register for which we need its initial state must be initialized rather than just sprining into existence. We could
    // initialize all registers, but that makes output a bit verbose--users usually don't want to see values for registers that
    // weren't accessed by the dataflow, and omitting their initialization is one easy way to hide them.
#if 0 // [Robb Matzke 2015-01-14]
    regState->initialize_large();
#else
    regState->writeRegister(STACK_POINTER_REG, ops->undefined_(STACK_POINTER_REG.get_nbits()), ops.get());
#endif
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
