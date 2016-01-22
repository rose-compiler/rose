#include "sage3basic.h"

#include <MemoryCellList.h>
#include <Partitioner2/DataFlow.h>
#include <Partitioner2/GraphViz.h>
#include <Partitioner2/Partitioner.h>
#include <Sawyer/GraphTraversal.h>
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
    const ControlFlowGraph &cfg;                             // global control flow graph
    const ControlFlowGraph::ConstVertexIterator startVertex; // where to start in the global CFG
    DfCfg dfCfg;                                             // dataflow control flow graph we are building
    InterproceduralPredicate &interproceduralPredicate;      // returns true when a call should be inlined

    // maps CFG vertex ID to dataflow vertex
    typedef Sawyer::Container::Map<ControlFlowGraph::ConstVertexIterator, DfCfg::VertexIterator> VertexMap;

    // Info about one function call
    struct CallFrame {
        VertexMap vmap;
        DfCfg::VertexIterator functionReturnVertex;
        bool wasFaked;
        CallFrame(DfCfg &dfCfg): functionReturnVertex(dfCfg.vertices().end()), wasFaked(false) {}
    };

    typedef std::list<CallFrame> CallStack;             // we use a list since there's no default constructor for an iterator
    CallStack callStack;
    size_t maxCallStackSize;                            // safety to prevent infinite recursion
    

    DfCfgBuilder(const Partitioner &partitioner, const ControlFlowGraph &cfg,
                 const ControlFlowGraph::ConstVertexIterator &startVertex, InterproceduralPredicate &predicate)
        : partitioner(partitioner), cfg(cfg), startVertex(startVertex), interproceduralPredicate(predicate),
          maxCallStackSize(10) {}
    
    typedef DepthFirstForwardGraphTraversal<const ControlFlowGraph> CfgTraversal;

    DfCfg::VertexIterator findVertex(const ControlFlowGraph::ConstVertexIterator cfgVertex) {
        ASSERT_require(!callStack.empty());
        CallFrame &callFrame = callStack.back();
        return callFrame.vmap.getOrElse(cfgVertex, dfCfg.vertices().end());
    }

    bool isValidVertex(const DfCfg::VertexIterator &dfVertex) {
        return dfVertex != dfCfg.vertices().end();
    }

    DfCfg::VertexIterator insertVertex(const DfCfgVertex &dfVertex,
                                       const ControlFlowGraph::ConstVertexIterator &cfgVertex) {
        ASSERT_require(!callStack.empty());
        CallFrame &callFrame = callStack.back();
        ASSERT_require(cfgVertex != cfg.vertices().end());
        ASSERT_require(!callFrame.vmap.exists(cfgVertex));
        DfCfg::VertexIterator dfVertexIter = dfCfg.insertVertex(dfVertex);
        callFrame.vmap.insert(cfgVertex, dfVertexIter);
        return dfVertexIter;
    }

    DfCfg::VertexIterator insertVertex(const DfCfgVertex &dfVertex) {
        return dfCfg.insertVertex(dfVertex);
    }

    DfCfg::VertexIterator insertVertex(DfCfgVertex::Type type) {
        return insertVertex(DfCfgVertex(type));
    }

    DfCfg::VertexIterator insertVertex(DfCfgVertex::Type type, const ControlFlowGraph::ConstVertexIterator &cfgVertex) {
        return insertVertex(DfCfgVertex(type), cfgVertex);
    }

    // Insert basic block if it hasn't been already
    DfCfg::VertexIterator findOrInsertBasicBlockVertex(const ControlFlowGraph::ConstVertexIterator &cfgVertex) {
        ASSERT_require(!callStack.empty());
        CallFrame &callFrame = callStack.back();
        ASSERT_require(cfgVertex != cfg.vertices().end());
        ASSERT_require(cfgVertex->value().type() == V_BASIC_BLOCK);
        DfCfg::VertexIterator retval = dfCfg.vertices().end();
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
    DfCfg::VertexIterator findOrInsertCallReturnVertex(const ControlFlowGraph::ConstVertexIterator &cfgVertex) {
        ASSERT_require(cfgVertex != cfg.vertices().end());
        ASSERT_require(cfgVertex->value().type() == V_BASIC_BLOCK);
        DfCfg::VertexIterator retval = dfCfg.vertices().end();
        BOOST_FOREACH (const ControlFlowGraph::Edge &edge, cfgVertex->outEdges()) {
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
                ControlFlowGraph::ConstEdgeIterator edge = t.edge();

                if (edge->value().type() == E_CALL_RETURN) {
                    // Do nothing; we handle call-return edges as part of function calls.

                } else if (edge->value().type() == E_FUNCTION_CALL) {
                    if (t.event() == ENTER_EDGE) {
                        DfCfg::VertexIterator callFrom = findVertex(edge->source());
                        ASSERT_require(isValidVertex(callFrom));
                        callStack.push_back(CallFrame(dfCfg));
                        if (callStack.size() <= maxCallStackSize && edge->target()->value().type()==V_BASIC_BLOCK &&
                            interproceduralPredicate(cfg, edge, callStack.size())) {
                            // Incorporate the call into the dfCfg
                            DfCfg::VertexIterator callTo = findOrInsertBasicBlockVertex(edge->target());
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
                            DfCfg::VertexIterator returnFrom = callStack.back().functionReturnVertex;
                            callStack.pop_back();
                            DfCfg::VertexIterator returnTo = findOrInsertCallReturnVertex(edge->source());
                            if (isValidVertex(returnFrom) && isValidVertex(returnTo))
                                dfCfg.insertEdge(returnFrom, returnTo);
                            ASSERT_require(!callStack.empty());
                        } else {
                            // Build the faked-call vertex and wire it up so the CALL goes to the faked-call vertex, which then
                            // flows to the CALL's return-point.
                            callStack.pop_back();
                            Function::Ptr callee;
                            if (edge->target()->value().type() == V_BASIC_BLOCK)
                                callee = bestSummaryFunction(edge->target()->value().owningFunctions());
                            DfCfg::VertexIterator dfSource = findVertex(edge->source());
                            ASSERT_require(isValidVertex(dfSource));
                            DfCfg::VertexIterator faked = insertVertex(DfCfgVertex(callee));
                            dfCfg.insertEdge(dfSource, faked);
                            DfCfg::VertexIterator returnTo = findOrInsertCallReturnVertex(edge->source());
                            if (isValidVertex(returnTo))
                                dfCfg.insertEdge(faked, returnTo);
                        }
                    }
                    
                } else {
                    // Generic edges
                    if (t.event() == LEAVE_EDGE) {
                        DfCfg::VertexIterator dfSource = findVertex(edge->source());
                        ASSERT_require(isValidVertex(dfSource));
                        DfCfg::VertexIterator dfTarget = findVertex(edge->target()); // the called function
                        if (isValidVertex(dfTarget))
                            dfCfg.insertEdge(dfSource, dfTarget);
                    }
                }
            }
        }
        return *this;
    }
};

Function::Ptr
bestSummaryFunction(const FunctionSet &functions) {
    Function::Ptr best;
    BOOST_FOREACH (const Function::Ptr &function, functions.values()) {
        // FIXME[Robb Matzke 2015-12-10]: for now, just choose any function
        best = function;
        break;
    }
    return best;
}

DfCfg
buildDfCfg(const Partitioner &partitioner,
           const ControlFlowGraph &cfg, const ControlFlowGraph::ConstVertexIterator &startVertex,
           InterproceduralPredicate &predicate) {
    return DfCfgBuilder(partitioner, cfg, startVertex, predicate).build().dfCfg;
}

void
dumpDfCfg(std::ostream &out, const DfCfg &dfCfg) {
    out <<"digraph dfCfg {\n";

    BOOST_FOREACH (const DfCfg::Vertex &vertex, dfCfg.vertices()) {
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

    BOOST_FOREACH (const DfCfg::Edge &edge, dfCfg.edges()) {
        out <<edge.source()->id() <<" -> " <<edge.target()->id() <<";\n";
    }
    
    out <<"}\n";
}

// If the expression is an offset from the initial stack register then return the offset, else nothing.
static Sawyer::Optional<int64_t>
isStackAddress(const rose::BinaryAnalysis::SymbolicExpr::Ptr &expr,
               const BaseSemantics::SValuePtr &initialStackPointer, SMTSolver *solver) {
    using namespace rose::BinaryAnalysis::InstructionSemantics2;

    if (!initialStackPointer)
        return Sawyer::Nothing();
    SymbolicExpr::Ptr initialStack = SymbolicSemantics::SValue::promote(initialStackPointer)->get_expression();

    // Special case where (add SP0 0) is simplified to SP0
    SymbolicExpr::LeafPtr variable = expr->isLeafNode();
    if (variable && variable->mustEqual(initialStack, solver))
        return 0;

    // Otherwise the expression must be (add SP0 N) where N != 0
    SymbolicExpr::InteriorPtr inode = expr->isInteriorNode();
    if (!inode || inode->getOperator() != SymbolicExpr::OP_ADD || inode->nChildren()!=2)
        return Sawyer::Nothing();

    variable = inode->child(0)->isLeafNode();
    SymbolicExpr::LeafPtr constant = inode->child(1)->isLeafNode();
    if (!constant || !constant->isNumber())
        std::swap(variable, constant);
    if (!constant || !constant->isNumber())
        return Sawyer::Nothing();
    if (!variable || !variable->isVariable())
        return Sawyer::Nothing();

    if (!variable->mustEqual(initialStack, solver))
        return Sawyer::Nothing();

    int64_t val = IntegerOps::signExtend2(constant->toInt(), constant->nBits(), 64);
    return val;
}

StackVariables
findStackVariables(const BaseSemantics::RiscOperatorsPtr &ops, const BaseSemantics::SValuePtr &initialStackPointer) {
    using namespace rose::BinaryAnalysis::InstructionSemantics2;
    ASSERT_not_null(ops);
    ASSERT_not_null(initialStackPointer);
    BaseSemantics::StatePtr state = ops->currentState();
    ASSERT_not_null(state);
    SMTSolver *solver = ops->get_solver();             // might be null

    // What is the word size for this architecture?  We'll assume the word size is the same as the width of the stack pointer,
    // whose value we have in initialStackPointer.
    ASSERT_require2(initialStackPointer->get_width() % 8 == 0, "stack pointer width is not an integral number of bytes");
    int64_t wordNBytes = initialStackPointer->get_width() / 8;
    ASSERT_require2(wordNBytes>0, "overflow");

    // Find groups of consecutive addresses that were written to by the same instruction(s) and which have the same I/O
    // properties. This is how we coalesce adjacent bytes into larger variables.
    typedef Sawyer::Container::IntervalMap<StackVariableLocation::Interval, StackVariableMeta> CellCoalescer;
    CellCoalescer cellCoalescer;
    typedef Sawyer::Container::Map<int64_t, BaseSemantics::SValuePtr> OffsetAddress; // full address per stack offset
    OffsetAddress offsetAddresses;
    BaseSemantics::MemoryCellStatePtr mem = BaseSemantics::MemoryCellState::promote(state->get_memory_state());
    BOOST_REVERSE_FOREACH (const BaseSemantics::MemoryCellPtr &cell, mem->allCells()) {
        SymbolicSemantics::SValuePtr address = SymbolicSemantics::SValue::promote(cell->get_address());
        ASSERT_require2(0 == cell->get_value()->get_width() % 8, "memory must be byte addressable");
        size_t nBytes = cell->get_value()->get_width() / 8;
        ASSERT_require(nBytes > 0);
        if (Sawyer::Optional<int64_t> stackOffset = isStackAddress(address->get_expression(), initialStackPointer, solver)) {
            StackVariableLocation location(*stackOffset, nBytes, address);
            StackVariableMeta meta(cell->getWriters(), cell->ioProperties());
            cellCoalescer.insert(location.interval(), meta);
            for (size_t i=0; i<nBytes; ++i) {
                BaseSemantics::SValuePtr byteAddr = ops->add(address, ops->number_(address->get_width(), i));
                offsetAddresses.insert(*stackOffset+i, byteAddr);
            }
        }
    }

    // The cellCoalescer has automatically organized the individual bytes into the largest possible intervals that have the
    // same set of writers and I/O properties.  We just need to pick them off in order to build the return value.
    std::vector<StackVariable> retval;
    BOOST_FOREACH (const StackVariableLocation::Interval &interval, cellCoalescer.intervals()) {
        int64_t offset = interval.least();
        int64_t nRemaining = interval.size();
        ASSERT_require2(nRemaining>0, "overflow");
        while (nRemaining > 0) {
            BaseSemantics::SValuePtr address = offsetAddresses[offset];
            int64_t nBytes = nRemaining;

            // Never create a variable that spans memory below and above (or equal to) the initial stack pointer. The initial
            // stack pointer is generally the boundary between local variables and function arguments (we're considering the
            // call-return address to be an argument on machines that pass it on the stack).
            if (offset < 0 && offset + nBytes > 0)
                nBytes = -offset;

            // Never create a variable that's wider than the architecture's natural word size.
            nBytes = std::min(nBytes, wordNBytes);
            ASSERT_require(nBytes>0 && nBytes<=nRemaining);

            // Create the stack variable.
            StackVariableLocation location(offset, nBytes, address);
            StackVariableMeta meta = cellCoalescer[offset];
            retval.push_back(StackVariable(location, meta));

            // Advance to next chunk of bytes within this interval
            offset += nBytes;
            nRemaining -= nBytes;
        }
    }
    return retval;
}

StackVariables
findLocalVariables(const BaseSemantics::RiscOperatorsPtr &ops, const BaseSemantics::SValuePtr &initialStackPointer) {
    StackVariables vars = findStackVariables(ops, initialStackPointer);
    StackVariables retval;
    BOOST_FOREACH (const StackVariable &var, vars) {
        if (var.location.offset < 0)
            retval.push_back(var);
    }
    return retval;
}

StackVariables
findFunctionArguments(const BaseSemantics::RiscOperatorsPtr &ops, const BaseSemantics::SValuePtr &initialStackPointer) {
    StackVariables vars = findStackVariables(ops, initialStackPointer);
    StackVariables retval;
    BOOST_FOREACH (const StackVariable &var, vars) {
        if (var.location.offset >= 0)
            retval.push_back(var);
    }
    return retval;
}

std::vector<AbstractLocation>
findGlobalVariables(const BaseSemantics::RiscOperatorsPtr &ops, size_t wordNBytes) {
    ASSERT_not_null(ops);
    BaseSemantics::StatePtr state = ops->currentState();
    ASSERT_not_null(state);
    ASSERT_require(wordNBytes>0);


    // Find groups of consecutive addresses that were written to by the same instruction.  This is how we coalesce adjacent
    // bytes into larger variables.
    typedef Sawyer::Container::IntervalMap<AddressInterval, rose_addr_t /*writer*/> StackWriters;
    typedef Sawyer::Container::Map<rose_addr_t, BaseSemantics::SValuePtr> SymbolicAddresses;
    StackWriters stackWriters;
    SymbolicAddresses symbolicAddrs;
    BaseSemantics::MemoryCellStatePtr mem = BaseSemantics::MemoryCellState::promote(state->get_memory_state());
    BOOST_REVERSE_FOREACH (const BaseSemantics::MemoryCellPtr &cell, mem->allCells()) {
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
            rose_addr_t nBytes = std::min(nRemaining, (rose_addr_t)wordNBytes);
            retval.push_back(AbstractLocation(addr, nBytes));
            va += nBytes;
            nRemaining -= nBytes;
        }
    }
    return retval;
}

// Construct a new state from scratch
BaseSemantics::StatePtr
TransferFunction::initialState() const {
    BaseSemantics::RiscOperatorsPtr ops = cpu_->get_operators();
    BaseSemantics::StatePtr newState = ops->currentState()->clone();
    newState->clear();

    BaseSemantics::RegisterStateGenericPtr regState =
        BaseSemantics::RegisterStateGeneric::promote(newState->get_register_state());

    // Any register for which we need its initial state must be initialized rather than just springing into existence. We could
    // initialize all registers, but that makes output a bit verbose--users usually don't want to see values for registers that
    // weren't accessed by the dataflow, and omitting their initialization is one easy way to hide them.
#if 0 // [Robb Matzke 2015-01-14]
    regState->initialize_large();
#else
    regState->writeRegister(STACK_POINTER_REG, ops->undefined_(STACK_POINTER_REG.get_nbits()), ops.get());
#endif
    return newState;
}

// Required by dataflow engine: compute new output state given a vertex and input state.
BaseSemantics::StatePtr
TransferFunction::operator()(const DfCfg &dfCfg, size_t vertexId, const BaseSemantics::StatePtr &incomingState) const {
    BaseSemantics::RiscOperatorsPtr ops = cpu_->get_operators();
    BaseSemantics::StatePtr retval = incomingState->clone();
    const RegisterDictionary *regDict = cpu_->get_register_dictionary();
    ops->currentState(retval);

    DfCfg::ConstVertexIterator vertex = dfCfg.findVertex(vertexId);
    ASSERT_require(vertex != dfCfg.vertices().end());
    if (DfCfgVertex::FAKED_CALL == vertex->value().type()) {
        Function::Ptr callee = vertex->value().callee();
        bool isStackPtrFixed = false;
        BaseSemantics::RegisterStateGenericPtr genericRegState =
            boost::dynamic_pointer_cast<BaseSemantics::RegisterStateGeneric>(retval->get_register_state());

        BaseSemantics::SValuePtr stackDelta;            // non-null if a stack delta is known for the callee
        if (callee)
            stackDelta = callee->stackDelta();

        // Clobber registers that are modified by the callee. The extra calls to updateWriteProperties is because most
        // RiscOperators implementation won't do that if they don't have a current instruction (which we don't).
        if (callee && callee->callingConventionAnalysis().hasResults()) {
            // A previous calling convention analysis knows what registers are clobbered by the call.
            const CallingConvention::Analysis &ccAnalysis = callee->callingConventionAnalysis();
            BOOST_FOREACH (const RegisterDescriptor &reg, ccAnalysis.outputRegisters().listAll(regDict)) {
                ops->writeRegister(reg, ops->undefined_(reg.get_nbits()));
                if (genericRegState)
                    genericRegState->insertProperties(reg, BaseSemantics::IO_WRITE);
            }
            if (ccAnalysis.stackDelta()) {
                ops->writeRegister(STACK_POINTER_REG,
                                   ops->add(ops->readRegister(STACK_POINTER_REG),
                                            ops->number_(STACK_POINTER_REG.get_nbits(), *ccAnalysis.stackDelta())));
                if (genericRegState)
                    genericRegState->updateWriteProperties(STACK_POINTER_REG, BaseSemantics::IO_WRITE);
                isStackPtrFixed = true;
            }
        } else if (defaultCallingConvention_) {
            // Use a default calling convention definition to decide what registers should be clobbered. Don't clobber the
            // stack pointer because we might be able to adjust it more intelligently below.
            BOOST_FOREACH (const CallingConvention::ParameterLocation &loc, defaultCallingConvention_->outputParameters()) {
                if (loc.type() == CallingConvention::ParameterLocation::REGISTER && loc.reg() != STACK_POINTER_REG) {
                    ops->writeRegister(loc.reg(), ops->undefined_(loc.reg().get_nbits()));
                    if (genericRegState)
                        genericRegState->updateWriteProperties(loc.reg(), BaseSemantics::IO_WRITE);
                }
            }
            BOOST_FOREACH (const RegisterDescriptor &reg, defaultCallingConvention_->scratchRegisters()) {
                if (reg != STACK_POINTER_REG) {
                    ops->writeRegister(reg, ops->undefined_(reg.get_nbits()));
                    if (genericRegState)
                        genericRegState->updateWriteProperties(reg, BaseSemantics::IO_WRITE);
                }
            }
            if (!stackDelta && // don't fix it here if we'll fix it below with more accurate information
                defaultCallingConvention_->stackCleanup() == CallingConvention::CLEANUP_BY_CALLER &&
                defaultCallingConvention_->nonParameterStackSize() != 0) {
                BaseSemantics::SValuePtr oldStack = ops->readRegister(STACK_POINTER_REG);
                BaseSemantics::SValuePtr delta =
                    ops->number_(oldStack->get_width(), defaultCallingConvention_->nonParameterStackSize());
                if (defaultCallingConvention_->stackDirection() == CallingConvention::GROWS_UP)
                    delta = ops->negate(delta);
                BaseSemantics::SValuePtr newStack = ops->add(oldStack, delta);
                ops->writeRegister(STACK_POINTER_REG, newStack);
                if (genericRegState)
                    genericRegState->updateWriteProperties(STACK_POINTER_REG, BaseSemantics::IO_WRITE);
                isStackPtrFixed = true;
            }
        } else {
            // We have not performed a calling convention analysis and we don't have a default calling convention definition. A
            // conservative approach would need to set all registers to bottom.  We'll only adjust the stack pointer (below).
        }

        // Adjust the stack pointer if we haven't already.
        if (!isStackPtrFixed) {
            BaseSemantics::SValuePtr newStack, delta;
            if (callee)
                delta = callee->stackDelta();
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
            ops->writeRegister(STACK_POINTER_REG, newStack);
            if (genericRegState)
                genericRegState->updateWriteProperties(STACK_POINTER_REG, BaseSemantics::IO_WRITE);
        }

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
