#include "sage3basic.h"

#include <AsmUnparser_compat.h>
#include <Color.h>
#include <MemoryCellList.h>
#include <Partitioner2/DataFlow.h>
#include <Partitioner2/GraphViz.h>
#include <Partitioner2/ModulesElf.h>
#include <Partitioner2/Partitioner.h>
#include <Sawyer/GraphTraversal.h>
#include <SymbolicSemantics2.h>
#include <sstream>

using namespace Sawyer::Container;
using namespace Sawyer::Container::Algorithm;

namespace Rose {
namespace BinaryAnalysis {
namespace Partitioner2 {
namespace DataFlow {

NotInterprocedural NOT_INTERPROCEDURAL;

// private class
class DfCfgBuilder {
public:
    const Partitioner &partitioner;
    const ControlFlowGraph &cfg;                             // global control flow graph
    DfCfg dfCfg;                                             // dataflow control flow graph we are building
    InterproceduralPredicate &interproceduralPredicate;      // returns true when a call should be inlined

    // maps CFG vertex ID to dataflow vertex
    typedef Sawyer::Container::GraphIteratorMap<ControlFlowGraph::ConstVertexIterator, DfCfg::VertexIterator> VertexMap;

    // Info about one function call
    struct CallFrame {
        Function::Ptr function;
        size_t inliningId;
        VertexMap vmap;
        DfCfg::VertexIterator functionEntryVertex;
        DfCfg::VertexIterator functionReturnVertex;
        CallFrame(DfCfg &dfCfg, const Function::Ptr &function, size_t inliningId)
            : function(function), inliningId(inliningId),
              functionEntryVertex(dfCfg.vertices().end()),
              functionReturnVertex(dfCfg.vertices().end()) {}
    };

    typedef std::list<CallFrame> CallStack;             // we use a list since there's no default constructor for an iterator
    CallStack callStack;
    size_t maxCallStackSize;                            // safety to prevent infinite recursion
    size_t nextInliningId;                              // next ID when crating a CallFrame
    
    DfCfgBuilder(const Partitioner &partitioner, const ControlFlowGraph &cfg, InterproceduralPredicate &predicate)
        : partitioner(partitioner), cfg(cfg), interproceduralPredicate(predicate),
          maxCallStackSize(10), nextInliningId(0) {}

    typedef DepthFirstForwardGraphTraversal<const ControlFlowGraph> CfgTraversal;

    // Given a CFG vertex, find the corresponding data-flow vertex. Since function CFGs are inlined into the dfCFG repeatedly,
    // this method looks only at the top of the virtual function call stack.  Returns the end dfCFG vertex if the top of the
    // call stack doesn't have this CFG vertex in the dfCFG yet.
    DfCfg::VertexIterator findVertex(const ControlFlowGraph::ConstVertexIterator cfgVertex) {
        ASSERT_require(!callStack.empty());
        CallFrame &callFrame = callStack.back();
        return callFrame.vmap.find(cfgVertex).orElse(dfCfg.vertices().end());
    }

    // Insert the specified dfVertex into the data-flow graph and associate it with the specified cfgVertex. The mapping is
    // entered into the top of the virtual function call stack (the mapping must not already exist).
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

    // Insert the specified dfVertex into the data-flow graph without associating it with any control flow vertex.  This is for
    // things like function return points in the data-flow, which have no corresponding vertex in the CFG.
    DfCfg::VertexIterator insertVertex(const DfCfgVertex &dfVertex) {
        return dfCfg.insertVertex(dfVertex);
    }

    // Insert a data-flow vertex of specified type, associating it with a control flow vertex.
    DfCfg::VertexIterator insertVertex(DfCfgVertex::Type type, const ControlFlowGraph::ConstVertexIterator &cfgVertex,
                                       const Function::Ptr &parentFunction, size_t inliningId) {
        return insertVertex(DfCfgVertex(type, parentFunction, inliningId), cfgVertex);
    }

    // Insert a data-flow vertex of specified type. Don't use this for inserting a df vertex that needs to be associated with a
    // control flow vertex.
    DfCfg::VertexIterator insertVertex(DfCfgVertex::Type type, const Function::Ptr &parentFunction, size_t inliningId) {
        return insertVertex(DfCfgVertex(type, parentFunction, inliningId));
    }

    // Insert basic block into the dfCFG if it hasn't been already. This only looks at the top-most function on the virtual
    // function call graph to decide whether to insert the basic block.
    DfCfg::VertexIterator findOrInsertBasicBlockVertex(const ControlFlowGraph::ConstVertexIterator &cfgVertex,
                                                       const Function::Ptr &parentFunction, size_t inliningId) {
        ASSERT_require(!callStack.empty());
        CallFrame &callFrame = callStack.back();
        ASSERT_require(cfgVertex != cfg.vertices().end());
        ASSERT_require(cfgVertex->value().type() == V_BASIC_BLOCK);
        DfCfg::VertexIterator retval = dfCfg.vertices().end();
        if (!callFrame.vmap.find(cfgVertex).assignTo(retval)) {
            BasicBlock::Ptr bblock = cfgVertex->value().bblock();
            ASSERT_not_null(bblock);
            retval = insertVertex(DfCfgVertex(bblock, parentFunction, inliningId), cfgVertex);

            // All function return basic blocks will point only to the special FUNCRET vertex.
            if (partitioner.basicBlockIsFunctionReturn(bblock)) {
                if (!dfCfg.isValidVertex(callFrame.functionReturnVertex))
                    callFrame.functionReturnVertex = insertVertex(DfCfgVertex::FUNCRET, parentFunction, inliningId);
                dfCfg.insertEdge(retval, callFrame.functionReturnVertex);
            }
        }
        return retval;
    }

    // Returns the dfCfg vertex for a CALL's return-to vertex, creating it if necessary.  There might be none, in which case the
    // vertex end iterator is returned.
    DfCfg::VertexIterator findOrInsertCallReturnVertex(const ControlFlowGraph::ConstVertexIterator &cfgCallSite,
                                                       const Function::Ptr &parentFunction, size_t inliningId) {
        ASSERT_require(cfgCallSite != cfg.vertices().end());
        ASSERT_require(cfgCallSite->value().type() == V_BASIC_BLOCK);
        DfCfg::VertexIterator dfReturnSite = dfCfg.vertices().end();
        BOOST_FOREACH (const ControlFlowGraph::Edge &edge, cfgCallSite->outEdges()) {
            if (edge.value().type() == E_CALL_RETURN) {
                ASSERT_require(edge.target()->value().type() == V_BASIC_BLOCK);
                ASSERT_require2(dfReturnSite == dfCfg.vertices().end(),
                                edge.target()->value().bblock()->printableName() + " has multiple call-return edges");
                dfReturnSite = findOrInsertBasicBlockVertex(edge.target(), parentFunction, inliningId);
            }
        }
        return dfReturnSite;
    }

    // top-level build function.
    DfCfgBuilder& build(const ControlFlowGraph::ConstVertexIterator &startVertex) {
        Function::Ptr parentFunction = partitioner.functionExists(startVertex->value().address());
        ASSERT_not_null(parentFunction);
        callStack.push_back(CallFrame(dfCfg, parentFunction, nextInliningId++));
        buildRecursively(startVertex);
        ASSERT_require(callStack.size() == 1);
        return *this;
    }

    // Add vertices to the dfCFG based on the CFG.  This function is recursive, with each invocation handling the insertion of
    // one CFG function into the dfCFG -- the function whose entry is at the top of the virtual function call stack.
    void buildRecursively(const ControlFlowGraph::ConstVertexIterator &functionEntryVertex) {
        ASSERT_forbid(callStack.empty());
        ASSERT_require(cfg.isValidVertex(functionEntryVertex));

        for (CfgTraversal t(cfg, functionEntryVertex, ENTER_EVENTS|LEAVE_EDGE); t; ++t) {
            if (t.event() == ENTER_VERTEX) {
                // Every CFG vertex we enter needs to have a corresponding dfCFG vertex created. Due to the t.skipChildren
                // invocations below (during edge traversals), we will only ever enter the root vertex and those vertices that
                // are reachable from non-call, non-return edges. I.e., we're visiting vertices that belong to a single
                // function.
                if (t.vertex()->value().type() == V_BASIC_BLOCK) {
                    findOrInsertBasicBlockVertex(t.vertex(), callStack.back().function, callStack.back().inliningId);
                    if (partitioner.basicBlockIsFunctionReturn(t.vertex()->value().bblock()))
                        t.skipChildren();               // we're handling return successors explicitly
                } else {
                    insertVertex(DfCfgVertex::INDET, t.vertex(), callStack.back().function, callStack.back().inliningId);
                }

            } else if (t.edge()->value().type() == E_FUNCTION_CALL) {
                // Function calls are either recursively inlined into the dfCFG or replaced by a special "faked call" vertex
                // that refers to the function but does not create all its vertices in the dfCFG.  In either case, the
                // traversal does not flow into the function -- either we insert the faked vertex explicitly or we invoke
                // buildRecurisvely to inline the called function.
                if (t.event() != ENTER_EDGE)
                    continue;
                t.skipChildren();
                Function::Ptr callerFunc = callStack.back().function;
                size_t callerInliningId = callStack.back().inliningId;
                DfCfg::VertexIterator callFrom = findVertex(t.edge()->source());
                ASSERT_require(dfCfg.isValidVertex(callFrom));

                // Create an optional vertex to which this inlined or faked function call will return. This will be an end
                // iterator if the call apparently doesn't return.
                DfCfg::VertexIterator returnTo = findOrInsertCallReturnVertex(t.edge()->source(), callerFunc, callerInliningId);

                // Function being called
                Function::Ptr calleeFunc;
                if (t.edge()->target()->value().type() == V_BASIC_BLOCK)
                    calleeFunc = bestSummaryFunction(t.edge()->target()->value().owningFunctions());

                // Insert either a summary vertex or recursively inline the callee's body
                callStack.push_back(CallFrame(dfCfg, calleeFunc, nextInliningId++)); {
                    bool doInline = true;
                    if (callStack.size() > maxCallStackSize) {
                        doInline = false;               // too much recursive inlining
                    } else if (t.edge()->target()->value().type() != V_BASIC_BLOCK) {
                        doInline = false;               // e.g., call to indeterminate address
                    } else if (!interproceduralPredicate(cfg, t.edge(), callStack.size())) {
                        doInline = false;               // user says no inlining
                    } else if (ModulesElf::isUnlinkedImport(partitioner, calleeFunc)) {
                        doInline = false;               // callee is not actually present (not linked in yet)
                    }

                    if (doInline) {
                        // Inline the called function into the dfCFG
                        buildRecursively(t.edge()->target());
                    } else {
                        // Insert a "faked" call, i.e. a vertex that summarizes the call by referencing the callee function.
                        // The function pointer will be null if the address is indeterminate.
                        callStack.back().functionEntryVertex =
                            insertVertex(DfCfgVertex(calleeFunc, callerFunc, callerInliningId));
                        callStack.back().functionReturnVertex = callStack.back().functionEntryVertex;
                    }

                    // Create the edge from the call site to the callee's entry vertex.
                    DfCfg::VertexIterator calleeVertex = callStack.back().functionEntryVertex;
                    ASSERT_require(dfCfg.isValidVertex(calleeVertex));
                    dfCfg.insertEdge(callFrom, calleeVertex);

                    // Create the edge from the callee's returning vertex to the CALL's return point.
                    if (dfCfg.isValidVertex(callStack.back().functionReturnVertex) && dfCfg.isValidVertex(returnTo))
                        dfCfg.insertEdge(callStack.back().functionReturnVertex, returnTo);
                } callStack.pop_back();

            } else if (t.edge()->value().type() == E_CALL_RETURN) {
                // Edges from CALL to the return point are handled in the E_FUNCTION_CALL case above, so we don't need to insert
                // an edge here. However, we must traverse these edges in order to reach the rest of the CFG.

            } else {
                // All other edge types in the CFG have a corresponding edge in the dfCFG provided there's a corresponding
                // source and target vertex in the dfCFG.
                if (t.event() != LEAVE_EDGE)
                    continue;
                DfCfg::VertexIterator dfSource = findVertex(t.edge()->source());
                DfCfg::VertexIterator dfTarget = findVertex(t.edge()->target());
                if (dfCfg.isValidVertex(dfSource) && dfCfg.isValidVertex(dfTarget))
                    dfCfg.insertEdge(dfSource, dfTarget);
            }
        }
        callStack.back().functionEntryVertex = findVertex(functionEntryVertex);
        ASSERT_require(dfCfg.isValidVertex(callStack.back().functionEntryVertex));
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
    return DfCfgBuilder(partitioner, cfg, predicate).build(startVertex).dfCfg;
}

void
dumpDfCfg(std::ostream &out, const DfCfg &dfCfg) {
    const Color::HSV entryColor(0.33, 1.0, 0.9);        // light green
    const Color::HSV indetColor(0.00, 1.0, 0.8);        // light red
    const Color::HSV returnColor(0.67, 1.0, 0.9);       // light blue

    // How many subgraphs?
    Sawyer::Container::Map<size_t, std::string> subgraphs;
    BOOST_FOREACH (const DfCfg::Vertex &vertex, dfCfg.vertices()) {
        if (vertex.value().parentFunction()) {
            subgraphs.insert(vertex.value().inliningId(), vertex.value().parentFunction()->printableName());
        } else {
            subgraphs.insertMaybe(vertex.value().inliningId(), "no function");
        }
    }
    
    out <<"digraph dfCfg {\n";
    BOOST_FOREACH (size_t subgraphId, subgraphs.keys()) {
        if (subgraphs.size() > 1) {
            out <<"subgraph cluster_" <<subgraphId <<" {\n"
                <<" graph ["
                <<" label=<subgraph " <<subgraphId <<"<br/>" <<GraphViz::htmlEscape(subgraphs[subgraphId]) <<">"
                <<" ];\n";
        }
        
        BOOST_FOREACH (const DfCfg::Vertex &vertex, dfCfg.vertices()) {
            if (vertex.value().inliningId() == subgraphId) {
                out <<vertex.id() <<" [";
                if (0 == vertex.id())
                    out <<" shape=box style=filled fillcolor=\"" <<entryColor.toHtml() <<"\"";

                out <<" label=<<b>Vertex " <<vertex.id() <<"</b>";
                switch (vertex.value().type()) {
                    case DfCfgVertex::BBLOCK:
                        BOOST_FOREACH (SgAsmInstruction *insn, vertex.value().bblock()->instructions())
                            out <<"<br align=\"left\"/>" <<GraphViz::htmlEscape(unparseInstructionWithAddress(insn));
                        out <<"<br align=\"left\"/>> shape=box fontname=Courier";
                        break;
                    case DfCfgVertex::FAKED_CALL:
                        if (Function::Ptr callee = vertex.value().callee()) {
                            out <<"<br/>fake call to<br/>" <<GraphViz::htmlEscape(callee->printableName()) <<">";
                        } else {
                            out <<"<br/>fake call to<br/>indeterminate function>";
                            out <<" style=filled fillcolor=\"" <<indetColor.toHtml() <<"\"";
                        }
                        break;
                    case DfCfgVertex::FUNCRET:
                        out <<"<br/>function return>";
                        out <<" style=filled fillcolor=\"" <<returnColor.toHtml() <<"\"";
                        break;
                    case DfCfgVertex::INDET:
                        out <<"<br/>indeterminate> style=filled fillcolor=\"" <<indetColor.toHtml() <<"\"";
                        break;
                }
                out <<" ];\n";
            }
        }

        if (subgraphs.size() > 1)
            out <<"}\n";
    }

    BOOST_FOREACH (const DfCfg::Edge &edge, dfCfg.edges()) {
        out <<edge.source()->id() <<" -> " <<edge.target()->id() <<";\n";
    }

    out <<"}\n";
}

// If the expression is an offset from the initial stack register then return the offset, else nothing.
static Sawyer::Optional<int64_t>
isStackAddress(const Rose::BinaryAnalysis::SymbolicExpr::Ptr &expr,
               const BaseSemantics::SValuePtr &initialStackPointer, const SmtSolverPtr &solver) {
    using namespace Rose::BinaryAnalysis::InstructionSemantics2;

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
    if (!constant || !constant->isIntegerConstant())
        std::swap(variable, constant);
    if (!constant || !constant->isIntegerConstant())
        return Sawyer::Nothing();
    if (!variable || !variable->isIntegerVariable())
        return Sawyer::Nothing();

    if (!variable->mustEqual(initialStack, solver))
        return Sawyer::Nothing();

    int64_t val = constant->toSigned().get();
    return val;
}

// Info about stack variables that is distributed across function frame offsets using a Sawyer::IntervalMap.
struct StackVariableMeta {
    InstructionSemantics2::BaseSemantics::MemoryCellList::AddressSet writers;
    InstructionSemantics2::BaseSemantics::InputOutputPropertySet ioProperties;

    StackVariableMeta(const InstructionSemantics2::BaseSemantics::MemoryCellList::AddressSet &writers,
                      const InstructionSemantics2::BaseSemantics::InputOutputPropertySet &io)
        : writers(writers), ioProperties(io) {}

    bool operator==(const StackVariableMeta &other) const {
        return writers == other.writers && ioProperties == other.ioProperties;
    }
};

Variables::StackVariables
findStackVariables(const Function::Ptr &function, const BaseSemantics::RiscOperatorsPtr &ops,
                   const BaseSemantics::SValuePtr &initialStackPointer) {
    using namespace Rose::BinaryAnalysis::InstructionSemantics2;
    ASSERT_not_null(ops);
    ASSERT_not_null(initialStackPointer);
    BaseSemantics::StatePtr state = ops->currentState();
    ASSERT_not_null(state);
    SmtSolverPtr solver = ops->solver();                // might be null

    // What is the word size for this architecture?  We'll assume the word size is the same as the width of the stack pointer,
    // whose value we have in initialStackPointer.
    ASSERT_require2(initialStackPointer->get_width() % 8 == 0, "stack pointer width is not an integral number of bytes");
    int64_t wordNBytes = initialStackPointer->get_width() / 8;
    ASSERT_require2(wordNBytes>0, "overflow");

    // Find groups of consecutive addresses that were written to by the same instruction(s) and which have the same I/O
    // properties. This is how we coalesce adjacent bytes into larger variables.
    typedef Sawyer::Container::IntervalMap<Variables::OffsetInterval, StackVariableMeta> CellCoalescer;
    CellCoalescer cellCoalescer;
    typedef Sawyer::Container::Map<int64_t, BaseSemantics::SValuePtr> OffsetAddress; // full address per stack offset
    OffsetAddress offsetAddresses;
    BaseSemantics::MemoryCellStatePtr mem = BaseSemantics::MemoryCellState::promote(state->memoryState());
    BOOST_REVERSE_FOREACH (const BaseSemantics::MemoryCellPtr &cell, mem->allCells()) {
        SymbolicSemantics::SValuePtr address = SymbolicSemantics::SValue::promote(cell->get_address());
        ASSERT_require2(0 == cell->get_value()->get_width() % 8, "memory must be byte addressable");
        size_t nBytes = cell->get_value()->get_width() / 8;
        ASSERT_require(nBytes > 0);
        if (Sawyer::Optional<int64_t> stackOffset = isStackAddress(address->get_expression(), initialStackPointer, solver)) {
            Variables::OffsetInterval location = Variables::OffsetInterval::baseSize(*stackOffset, nBytes);
            StackVariableMeta meta(cell->getWriters(), cell->ioProperties());
            cellCoalescer.insert(location, meta);
            for (size_t i=0; i<nBytes; ++i) {
                BaseSemantics::SValuePtr byteAddr = ops->add(address, ops->number_(address->get_width(), i));
                offsetAddresses.insert(*stackOffset+i, byteAddr);
            }
        }
    }

    // The cellCoalescer has automatically organized the individual bytes into the largest possible intervals that have the
    // same set of writers and I/O properties.  We just need to pick them off in order to build the return value.
    Variables::StackVariables retval;
    BOOST_FOREACH (const Variables::OffsetInterval &interval, cellCoalescer.intervals()) {
        int64_t offset = interval.least();
        int64_t nRemaining = interval.size();
        ASSERT_require2(nRemaining > 0, "overflow");
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
            ASSERT_require(nBytes > 0 && nBytes <= nRemaining);

            // Create the stack variable.
            const StackVariableMeta &meta = cellCoalescer[offset];
            Variables::StackVariable var(function, offset, nBytes, meta.writers);
            var.ioProperties(meta.ioProperties);
            var.setDefaultName();
            retval.insert(var.interval(), var);

            // Advance to next chunk of bytes within this interval
            offset += nBytes;
            nRemaining -= nBytes;
        }
    }
    return retval;
}

Variables::StackVariables
findLocalVariables(const Function::Ptr &function, const BaseSemantics::RiscOperatorsPtr &ops,
                   const BaseSemantics::SValuePtr &initialStackPointer) {
    Variables::StackVariables vars = findStackVariables(function, ops, initialStackPointer);
    Variables::StackVariables retval;
    BOOST_FOREACH (const Variables::StackVariable &var, vars.values()) {
        if (var.frameOffset() < 0)
            retval.insert(var.interval(), var);
    }
    return retval;
}

Variables::StackVariables
findFunctionArguments(const Function::Ptr &function, const BaseSemantics::RiscOperatorsPtr &ops,
                      const BaseSemantics::SValuePtr &initialStackPointer) {
    Variables::StackVariables vars = findStackVariables(function, ops, initialStackPointer);
    Variables::StackVariables retval;
    BOOST_FOREACH (const Variables::StackVariable &var, vars.values()) {
        if (var.frameOffset() >= 0)
            retval.insert(var.interval(), var);
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
    BaseSemantics::MemoryCellStatePtr mem = BaseSemantics::MemoryCellState::promote(state->memoryState());
    BOOST_REVERSE_FOREACH (const BaseSemantics::MemoryCellPtr &cell, mem->allCells()) {
        ASSERT_require2(0 == cell->get_value()->get_width() % 8, "memory must be byte addressable");
        size_t nBytes = cell->get_value()->get_width() / 8;
        ASSERT_require(nBytes > 0);
        if (cell->get_address()->is_number() && cell->get_address()->get_width()<=64) {
            rose_addr_t va = cell->get_address()->get_number();

            // There may have been many writers for an address. Rather than write an algorithm to find the largest sets of
            // addresses written by the same writer, we'll just arbitrarily choose the least address.
            const BaseSemantics::MemoryCell::AddressSet &allWriters = cell->getWriters();
            rose_addr_t leastWriter = 0;
            if (!allWriters.isEmpty())
                leastWriter = allWriters.least();
            stackWriters.insert(AddressInterval::baseSize(va, nBytes), leastWriter);
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
        BaseSemantics::RegisterStateGeneric::promote(newState->registerState());

    // Any register for which we need its initial state must be initialized rather than just springing into existence. We could
    // initialize all registers, but that makes output a bit verbose--users usually don't want to see values for registers that
    // weren't accessed by the dataflow, and omitting their initialization is one easy way to hide them.
#if 0 // [Robb Matzke 2015-01-14]
    regState->initialize_large();
#else
    regState->writeRegister(STACK_POINTER_REG, ops->undefined_(STACK_POINTER_REG.nBits()), ops.get());
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
        CallingConvention::Definition::Ptr ccDefn;
        Function::Ptr callee = vertex->value().callee();
        BaseSemantics::RegisterStateGenericPtr genericRegState =
            boost::dynamic_pointer_cast<BaseSemantics::RegisterStateGeneric>(retval->registerState());
        BaseSemantics::SValuePtr origStackPtr = ops->peekRegister(STACK_POINTER_REG);

        BaseSemantics::SValuePtr stackDelta;            // non-null if a stack delta is known for the callee
        if (callee)
            stackDelta = callee->stackDelta();

        // Clobber registers that are modified by the callee. The extra calls to updateWriteProperties is because most
        // RiscOperators implementation won't do that if they don't have a current instruction (which we don't).
        if (callee && callee->callingConventionAnalysis().didConverge()) {
            // A previous calling convention analysis knows what registers are clobbered by the call.
            const CallingConvention::Analysis &ccAnalysis = callee->callingConventionAnalysis();
            ccDefn = ccAnalysis.defaultCallingConvention();
            BOOST_FOREACH (RegisterDescriptor reg, ccAnalysis.outputRegisters().listAll(regDict)) {
                ops->writeRegister(reg, ops->undefined_(reg.nBits()));
                if (genericRegState)
                    genericRegState->insertProperties(reg, BaseSemantics::IO_WRITE);
            }
            if (ccAnalysis.stackDelta())
                stackDelta = ops->number_(STACK_POINTER_REG.nBits(), *ccAnalysis.stackDelta());

        } else if ((ccDefn = defaultCallingConvention_)) {
            // Use a default calling convention definition to decide what registers should be clobbered. Don't clobber the
            // stack pointer because we might be able to adjust it more intelligently below.
            BOOST_FOREACH (const CallingConvention::ParameterLocation &loc, ccDefn->outputParameters()) {
                if (loc.type() == CallingConvention::ParameterLocation::REGISTER && loc.reg() != STACK_POINTER_REG) {
                    ops->writeRegister(loc.reg(), ops->undefined_(loc.reg().nBits()));
                    if (genericRegState)
                        genericRegState->updateWriteProperties(loc.reg(), BaseSemantics::IO_WRITE);
                }
            }
            BOOST_FOREACH (RegisterDescriptor reg, ccDefn->scratchRegisters()) {
                if (reg != STACK_POINTER_REG) {
                    ops->writeRegister(reg, ops->undefined_(reg.nBits()));
                    if (genericRegState)
                        genericRegState->updateWriteProperties(reg, BaseSemantics::IO_WRITE);
                }
            }

            // Use the stack delta from the default calling convention only if we don't already know the stack delta from a
            // stack delta analysis, and only if the default CC is caller-cleanup.
            if (!stackDelta || !stackDelta->is_number() || stackDelta->get_width() > 64) {
                if (ccDefn->stackCleanup() == CallingConvention::CLEANUP_BY_CALLER) {
                    stackDelta = ops->number_(origStackPtr->get_width(), ccDefn->nonParameterStackSize());
                    if (ccDefn->stackDirection() == CallingConvention::GROWS_UP)
                        stackDelta = ops->negate(stackDelta);
                }
            }

        } else {
            // We have not performed a calling convention analysis and we don't have a default calling convention definition. A
            // conservative approach would need to set all registers to bottom.  We'll only adjust the stack pointer (below).
        }

        BaseSemantics::SValuePtr newStack;
        if (stackDelta && stackDelta->is_number() && stackDelta->get_width() <= 64) {
            newStack = ops->add(origStackPtr, stackDelta);
        } else {
            // We don't know the callee's delta, therefore we don't know how to adjust the delta for the callee's effect.
            newStack = ops->undefined_(STACK_POINTER_REG.nBits());
        }
        ASSERT_not_null(newStack);
        ops->writeRegister(STACK_POINTER_REG, newStack);
        if (genericRegState)
            genericRegState->updateWriteProperties(STACK_POINTER_REG, BaseSemantics::IO_WRITE);

        // Adjust the instruction pointer since it probably points to the entry block of the called function. We need it to
        // now point to the return-to site.
        if (vertex->nOutEdges() == 1) {
            if (BasicBlock::Ptr returnToBlock = vertex->outEdges().begin()->target()->value().bblock())
                ops->writeRegister(INSN_POINTER_REG, ops->number_(INSN_POINTER_REG.nBits(), returnToBlock->address()));
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

std::string
TransferFunction::printState(const BaseSemantics::StatePtr &state) {
    if (!state)
        return "null state";
    std::ostringstream ss;
    ss <<*state;
    return ss.str();
}

} // namespace
} // namespace
} // namespace
} // namespace
