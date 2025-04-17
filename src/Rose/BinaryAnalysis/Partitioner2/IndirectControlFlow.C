
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Partitioner2/IndirectControlFlow.h>

#include <Rose/Affirm.h>
#include <Rose/As.h>
#include <Rose/BinaryAnalysis/Architecture/X86.h>
#include <Rose/BinaryAnalysis/Partitioner2/BasicBlock.h>
#include <Rose/BinaryAnalysis/Partitioner2/DataFlow.h>
#include <Rose/BinaryAnalysis/Partitioner2/GraphViz.h>
#include <Rose/BinaryAnalysis/Partitioner2/ModulesX86.h>
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>
#include <Rose/StringUtility/Diagnostics.h>
#include <Rose/StringUtility/NumberToString.h>

#include <SgAsmDirectRegisterExpression.h>
#include <SgAsmMemoryReferenceExpression.h>
#include <SgAsmX86Instruction.h>


namespace BS = Rose::BinaryAnalysis::InstructionSemantics::BaseSemantics;

using namespace Rose::BinaryAnalysis::InstructionSemantics;
using namespace Sawyer::Message::Common;
using Rose::StringUtility::addrToString;
using Rose::StringUtility::plural;

namespace Rose {
namespace BinaryAnalysis {
namespace Partitioner2 {
namespace IndirectControlFlow {

Sawyer::Message::Facility mlog;

// Called from the ROSE_INITIALIZE macro to configure diagnostics.
void
initDiagnostics() {
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        Rose::Diagnostics::initAndRegister(&mlog, "Rose::BinaryAnalysis::Partitioner2::IndirectControlFlow");
        mlog.comment("analyzing indirect control flow");
    }
}

Sawyer::CommandLine::SwitchGroup
commandLineSwitches(Settings &settings) {
    using namespace Sawyer::CommandLine;

    SwitchGroup sg("Indirect control flow recovery");
    sg.name("icf");
    sg.doc("These switches affect the indirect control flow recovery algorithms primarily used when partitioning a binary specimen "
           "into instructions, basic blocks, static data blocks, and functions to construct the control flow graph and related "
           "data structures.");

    sg.insert(Switch("df-max-reverse")
              .argument("n", nonNegativeIntegerParser(settings.maxReversePathLength))
              .doc("The maximum depth of the initial, function-skipping, reverse CFG traversal when constructing a dataflow "
                   "graph. The default is " + plural(settings.maxReversePathLength, "basic blocks") + "."));

    sg.insert(Switch("df-max-inline")
              .argument("n", nonNegativeIntegerParser(settings.maxInliningDepth))
              .doc("Maximum call depth for inlining functions during the forward CFG traversal when constructing a dataflow "
                   "graph. The default is a maximum depth of " + plural(settings.maxInliningDepth, "function calls") + "."));

    sg.insert(Switch("df-max-iter-factor")
              .argument("n", nonNegativeIntegerParser(settings.maxDataflowIterationFactor))
              .doc("Limits the dataflow analysis so it terminates even if the states don't converge to a fixed point. The limit "
                   "is computed by multiplying the number of vertices in the dataflow graph by @v{n}. The default for @v{n} is " +
                   boost::lexical_cast<std::string>(settings.maxDataflowIterationFactor) + "."));

    return sg;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Functions for analyzing static jump tables, such as what the C compiler generates for `switch` statements.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace StaticJumpTable {

// Create a special register that is used to store the execution path constraints.
static RegisterDescriptor
createPathRegister(const Partitioner::ConstPtr &partitioner) {
    ASSERT_not_null(partitioner);
    const unsigned maj = partitioner->architecture()->registerDictionary()->firstUnusedMajor();
    const unsigned min = partitioner->architecture()->registerDictionary()->firstUnusedMinor(maj);
    return RegisterDescriptor(maj, min, 0, 1);
}

// Initialize the state with a reasonable concrete initial stack pointer.
static void
initializeStackPointer(const Partitioner::ConstPtr &partitioner, const BS::RiscOperators::Ptr &ops) {
    ASSERT_not_null(partitioner);
    ASSERT_not_null(ops);
    Sawyer::Message::Stream debug(mlog[DEBUG]);

    const RegisterDescriptor SP = partitioner->architecture()->registerDictionary()->stackPointerRegister();
    ASSERT_require(SP);

    const size_t stackSize = 2 * 1024 * 1024;
    const size_t stackAlignment = 1024 * 1024;

    const AddressInterval search = AddressInterval::hull(0x80000000, 0xffffffff);
    const Sawyer::Optional<Address> stackBottom =
        partitioner->memoryMap()->findFreeSpace(stackSize, stackAlignment, search, Sawyer::Container::MATCH_BACKWARD);
    if (stackBottom) {
        const AddressInterval stackRegion = AddressInterval::baseSize(*stackBottom, stackSize);
        const Address stackPtr = *stackBottom + stackSize / 2;
        SAWYER_MESG(debug) <<"  initial state stack: region = " <<addrToString(stackRegion)
                           <<"; pointer = " <<addrToString(stackPtr) <<"\n";
        const BS::SValue::Ptr sp = ops->number_(SP.nBits(), stackPtr);
        ops->writeRegister(SP, sp);
    } else {
        SAWYER_MESG(debug) <<"initial state has abstract stack pointer\n";
    }
}

// matches "(add[u64] (sext 0x40, e1[u32]) c1)" and returns e1 and c1 if matched. If not matched, returns (nullptr, 0).
static std::pair<SymbolicExpression::Ptr, Address>
match1(const SymbolicExpression::Ptr &add) {
    using namespace SymbolicExpression;
    if (!add || !add->isOperator(OP_ADD) || add->nBits() != 64 || !add->isIntegerExpr())
        return {};

    const auto sext = add->child(0);
    if (!sext || !sext->isOperator(OP_SEXTEND) || sext->nBits() != 64)
        return {};

    const auto sixtyfour = sext->child(0);
    if (!sixtyfour || sixtyfour->toUnsigned().orElse(0) != 64)
        return {};

    const auto e1 = sext->child(1);
    if (!e1 || !e1->isIntegerVariable() || e1->nBits() != 32)
        return {};

    const auto c1 = add->child(1);
    if (!c1 || !c1->isIntegerConstant() || c1->nBits() != 64 || !c1->toUnsigned())
        return {};

    return {e1, *c1->toUnsigned()};
}

// matches (add[u32] e1[u32] c1[u32]) and returns e1 and c1 if matched. If not matched, returns (nullptr, 0).
static std::pair<SymbolicExpression::Ptr, Address>
match2(const SymbolicExpression::Ptr &expr) {
    using namespace SymbolicExpression;
    if (!expr || !expr->isOperator(OP_ADD) || expr->nBits() != 32 || !expr->isIntegerExpr())
        return std::make_pair(Ptr(), 0);

    const auto e1 = expr->child(0);
    if (!e1 || e1->nBits() != 32 || !e1->isIntegerExpr())
        return std::make_pair(Ptr(), 0);

    const auto c1 = expr->child(1);
    if (!c1 || c1->nBits() != 32 || !c1->toUnsigned())
        return std::make_pair(Ptr(), 0);

    return std::make_pair(e1, *c1->toUnsigned());
}

// matches e1[32] and if so, returns (e1, 0)
static std::pair<SymbolicExpression::Ptr, Address>
match3(const SymbolicExpression::Ptr &expr) {
    return std::make_pair(expr, 0);
}

// Looks at an instruction pointer and tries to figure out some information about the jump table. It returns the symbolic value
// read from the jump table, and a constant that needs to be added to each entry in the jump table in order to obtain a target
// address.
static std::pair<SymbolicExpression::Ptr, Address>
match(const SymbolicExpression::Ptr &expr) {
    const auto m1 = match1(expr);
    if (m1.first)
        return m1;

    const auto m2 = match2(expr);
    if (m2.first)
        return m2;

    return match3(expr);
}

// Find the memory address containing the specified value. The value can be more than one byte. The lowest address is returned.
static SymbolicExpression::Ptr
findAddressContaining(const BS::RiscOperators::Ptr &ops, const SymbolicExpression::Ptr &searchValue) {
    ASSERT_not_null(ops);
    ASSERT_not_null(searchValue);
    using namespace SymbolicExpression;

    if (auto mem = as<BS::MemoryCellState>(ops->currentState()->memoryState())) {
        // FIXME[Robb Matzke 2025-04-08]: need to handle big-endian also
        const auto searchByte = makeExtract(0, 8, searchValue);

        struct Visitor: public BS::MemoryCell::Visitor {
            Ptr searchValue, searchByte;
            Ptr foundAddr;

            Visitor(const Ptr &searchValue, const Ptr &searchByte)
                : searchValue(notnull(searchValue)), searchByte(notnull(searchByte)) {}

            void operator()(BS::MemoryCell::Ptr &cell) {
                auto cellValue = SymbolicSemantics::SValue::promote(cell->value())->get_expression();

                // Search for the byte directly. Unfortunately this doesn't always work due to minor differences. For instance, the
                // needle might be (extract[u8] 0[u32], 8[u32], expr[u64]) but the value in memory might be (extract[u8] 0[u64],
                // 8[u64], expr[u64]) -- a difference only in the size of the types used to extract the byte. Since memory cell
                // values are almost always extract expressions, we can also try looking "through" the extract to the value being
                // extracted.
                if (cellValue->isEquivalentTo(searchByte)) {
                    foundAddr = SymbolicSemantics::SValue::promote(cell->address())->get_expression();
                } else if (cellValue->isOperator(OP_EXTRACT) &&
                           cellValue->child(0)->toUnsigned().orElse(1) == 0 &&
                           cellValue->child(1)->toUnsigned().orElse(1) == 8 &&
                           cellValue->child(2)->isEquivalentTo(searchValue)) {
                    foundAddr = SymbolicSemantics::SValue::promote(cell->address())->get_expression();
                }
            }
        } visitor(searchValue, searchByte);

        mem->traverse(visitor);
        return visitor.foundAddr;
    }

    return {};
}

// Return the set of interesting constants found in an expression. Constants are interesting if:
//
//   1. A constant that is an operand of an `add` operation.
//
// The list may change in the future.
static std::set<Address>
findInterestingConstants(const SymbolicExpression::Ptr &expr) {
    using namespace SymbolicExpression;

    struct: Visitor {
        std::set<const Node*> seen;                     // avoid re-processing common subexpressions
        std::set<Address> found;

        VisitAction preVisit(const Node *expr) {
            if (seen.insert(expr).second) {
                if (expr->isOperator(OP_ADD)) {
                    for (const Ptr &child: expr->children()) {
                        if (const auto n = child->toUnsigned())
                            found.insert(*n);
                    }
                }
                return CONTINUE;
            } else {
                return TRUNCATE;
            }
        }

        VisitAction postVisit(const Node*) {
            return CONTINUE;
        }
    } visitor;
    expr->depthFirstTraversal(visitor);
    return visitor.found;
}

// True if the address is mapped and has all the required permissions and none of the prohibited permissions.
static bool
isMappedAccess(const MemoryMap::Ptr &map, const Address addr, const unsigned required, const unsigned prohibited) {
    ASSERT_not_null(map);
    return !map->at(addr).require(required).prohibit(prohibited).segments().empty();
}

// Return jump table target addresses for table entries whose addresses are satisfiable given the path constraints. The
// `entryAddrExpr` is the symbolic address that was read by the basic block and represents any table entry.
std::set<Address>
satisfiableTargets(const JumpTable::Ptr &table, const SymbolicExpression::Ptr &pathConstraint,
                   const SymbolicExpression::Ptr &entryAddrExpr) {
    using namespace SymbolicExpression;
    ASSERT_not_null(table);
    ASSERT_not_null(pathConstraint);
    Sawyer::Message::Stream debug(mlog[DEBUG]);

    SmtSolver::Ptr solver = SmtSolver::bestAvailable();
    if (!solver)
        return std::set<Address>(table->targets().begin(), table->targets().end());

    std::set<Address> retval;
    ASSERT_not_null(solver);
    solver->insert(pathConstraint);
    for (size_t i = 0; i < table->nEntries(); ++i) {
        const Address target = table->targets()[i];
        const Address entryAddr = table->location().least() + i * table->bytesPerEntry();
        const auto entryConstraint = makeEq(makeIntegerConstant(entryAddrExpr->nBits(), entryAddr), entryAddrExpr);
        SAWYER_MESG(debug) <<"    entry #" <<i <<" at " <<addrToString(entryAddr)
                           <<" has target " <<addrToString(target) <<"\n";
        solver->push();
        solver->insert(entryConstraint);
        const auto isSatisfiable = solver->check();
        solver->pop();

        switch (isSatisfiable) {
            case SmtSolver::SAT_UNKNOWN:
                SAWYER_MESG(debug) <<"      SMT solver failed (assuming satisfiable)\n";
                // fall through
            case SmtSolver::SAT_YES: {
                const Address target = table->targets()[i];
                SAWYER_MESG(debug) <<"      satisfiable table entry address\n";
                retval.insert(target);
                break;
            }
            case SmtSolver::SAT_NO:
                SAWYER_MESG(debug) <<"      entry address is not satisfiable\n";
                break;
        }
    }
    return retval;
}

class DfVertex {
public:
    const BasicBlock::Ptr bblock;
    const size_t inlineId = 0;

    DfVertex() = delete;

    DfVertex(const BasicBlock::Ptr &bb, const size_t inlineId)
        : bblock(notnull(bb)), inlineId(inlineId) {}

    Address address() const {
        return bblock->address();
    }

    std::string printableName() const {
        return bblock->printableName();
    }
};

class DfVertexKey {
public:
    const Address addr = 0;
    const size_t inlineId = 0;

    DfVertexKey() = delete;

    DfVertexKey(const DfVertex &vertex)
        : addr(notnull(vertex.bblock)->address()), inlineId(vertex.inlineId) {}

    bool operator<(const DfVertexKey &other) const {
        if (addr != other.addr) {
            return addr < other.addr;
        } else {
            return inlineId < other.inlineId;
        }
    }
};

using DfGraph = Sawyer::Container::Graph<
    DfVertex,
    size_t,                                             // CFG edge ID
    DfVertexKey                                         // vertices sorted by basic block starting address
    >;

class DfInline {
public:
    DfGraph::ConstVertexIterator dfCaller;              // dataflow vertex making a function call
    DfGraph::ConstVertexIterator dfCallRet;             // dataflow vertex where the function call will return
    ControlFlowGraph::ConstEdgeIterator cfgCall;        // CFG edge representing the function call
    size_t callerInlineId;                              // basic blocks may appear more than once, distinguished by this field

    DfInline(const DfGraph::ConstVertexIterator dfCaller, const DfGraph::ConstVertexIterator dfCallRet,
             const ControlFlowGraph::ConstEdgeIterator cfgCall, const size_t callerInlineId)
        : dfCaller(dfCaller), dfCallRet(dfCallRet), cfgCall(cfgCall), callerInlineId(callerInlineId) {}
};

class DfTransfer {
    Partitioner::ConstPtr partitioner_;
    BS::Dispatcher::Ptr cpu_;

public:
    DfTransfer(const Partitioner::ConstPtr &partitioner, const BS::Dispatcher::Ptr &cpu)
        : partitioner_(notnull(partitioner)), cpu_(notnull(cpu)) {}

    std::string toString(const BS::State::Ptr &state) const {
        if (!state) {
            return "null state";
        } else {
            return boost::lexical_cast<std::string>(*state);
        }
    }

    BS::State::Ptr initialState() const {
        BS::RiscOperators::Ptr ops = cpu_->operators();
        BS::State::Ptr retval = ops->currentState()->clone();
        ops->currentState(retval);

        retval->clear();
        cpu_->initializeState(retval);
        initializeStackPointer(partitioner_, ops);
        return retval;
    }

    BS::State::Ptr operator()(const DfGraph &dfGraph, size_t vertexId, const BS::State::Ptr &incomingState) const {
        ASSERT_not_null(incomingState);

        BS::State::Ptr retval = incomingState->clone();
        BS::RiscOperators::Ptr ops = cpu_->operators();
        ops->currentState(retval);

        BasicBlock::Ptr bb = dfGraph.findVertex(vertexId)->value().bblock;
        for (SgAsmInstruction *insn: bb->instructions()) {
            try {
                cpu_->processInstruction(insn);
            } catch (const BS::Exception&) {
            }
        }
        return retval;
    }
};

class DfMerge: public BinaryAnalysis::DataFlow::SemanticsMerge {
public:
    using Super = BinaryAnalysis::DataFlow::SemanticsMerge;

private:
    const RegisterDescriptor PATH, IP;
    const DfGraph &dfGraph;

public:
    explicit DfMerge(const Partitioner::ConstPtr &partitioner, const BS::RiscOperators::Ptr &ops, const DfGraph &dfGraph)
        : Super(ops),
          PATH(createPathRegister(partitioner)),
          IP(partitioner->architecture()->registerDictionary()->instructionPointerRegister()),
          dfGraph(dfGraph) {}

    bool operator()(size_t dstId, BS::State::Ptr &dst, size_t srcId, const BS::State::Ptr &src) const {
        using namespace SymbolicExpression;

        ASSERT_not_null(src);
        const auto dstVert = dfGraph.findVertex(dstId);
        ASSERT_require(dfGraph.isValidVertex(dstVert));
        const auto srcVert = dfGraph.findVertex(dstId);
        ASSERT_require(dfGraph.isValidVertex(srcVert));
        const BS::RiscOperators::Ptr ops = notnull(operators());

        // Symbolic expression for the expected successor(s)
        const auto srcIp = SymbolicSemantics::SValue::promote(src->peekRegister(IP, ops->undefined_(IP.nBits()), ops.get()))
                           ->get_expression();

        // Concrete address of the destination successor--the state we're merging into
        const SymbolicExpression::Ptr dstAddr = makeIntegerConstant(srcIp->nBits(), dstVert->value().address());

        // Initial path constraints for the destination (before merging)
        SymbolicExpression::Ptr origConstraints;
        if (dst) {
            origConstraints = SymbolicSemantics::SValue::promote(dst->peekRegister(PATH, ops->boolean_(true), ops.get()))
                              ->get_expression();
        }

        bool retval = Super::operator()(dstId, dst, srcId, src);
        ASSERT_not_null(dst);

        // New constraints for the destination
        if (dstAddr) {
            auto newConstraints = makeEq(srcIp, dstAddr);
            if (origConstraints)
                newConstraints = makeOr(newConstraints, origConstraints);
            auto newConstraintsSVal = SymbolicSemantics::SValue::instance_symbolic(newConstraints);
            dst->writeRegister(PATH, newConstraintsSVal, ops.get());
        }

        return retval;
    }
};

// Print a dataflow graph in Graphviz format for debugging.
static void
toGraphviz(std::ostream &out, const DfGraph &graph, const Partitioner::ConstPtr &partitioner) {
    ASSERT_not_null(partitioner);

    const Color::HSV entryColor(0.33, 1.0, 0.9);        // light green
    const Color::HSV indetColor(0.00, 1.0, 0.8);        // light red
    const Color::HSV returnColor(0.67, 1.0, 0.9);       // light blue

    // One subgraph per inline ID, and give them names.
    std::map<size_t, std::string> subgraphs;
    for (const auto &vertex: graph.vertices()) {
        auto &name = subgraphs[vertex.value().inlineId];
        if (name.empty()) {
            for (Function::Ptr &function: partitioner->functionsOverlapping(vertex.value().address())) {
                name = "inline #" + boost::lexical_cast<std::string>(vertex.value().inlineId) + " " + function->printableName();
                break;
            }
        }
    }
    for (auto &subgraph: subgraphs) {
        if (subgraph.second.empty())
            subgraph.second = "inline #" + boost::lexical_cast<std::string>(subgraph.first);
    }

    out <<"digraph dfCfg {\n";
    for (const auto &subgraph: subgraphs) {
        out <<"subgraph cluster_" <<subgraph.first <<" {\n"
            <<" graph ["
            <<" label=<subgraph " <<subgraph.first <<"<br/>" <<GraphViz::htmlEscape(subgraph.second) <<">"
            <<" ];\n";

        for (const auto &dfVertex: graph.vertices()) {
            if (dfVertex.value().inlineId == subgraph.first) {
                out <<dfVertex.id() <<" [";
                if (dfVertex.nInEdges() == 0)
                    out <<" shape=box style=filled fillcolor=\"" <<entryColor.toHtml() <<"\"";

                out <<" label=<<b>Vertex " <<dfVertex.id() <<"</b>";

                BasicBlock::Ptr bb = partitioner->basicBlockExists(dfVertex.value().address());
                ASSERT_not_null(bb);
                for (SgAsmInstruction *insn: bb->instructions())
                    out <<"<br align=\"left\"/>" <<GraphViz::htmlEscape(insn->toStringNoColor());
                out <<"<br align=\"left\"/>> shape=box fontname=Courier";
                out <<" ];\n";
            }
        }
        out <<"}\n";
    }

    for (const auto &edge: graph.edges()) {
        out <<edge.source()->id() <<" -> " <<edge.target()->id() <<";\n";
    }

    out <<"}\n";
}

// Inline into the dataflow graph a function called via CFG edge. Return a list of any new dataflow vertices that have subsequent
// function calls that were not inlined yet.
static std::vector<DfInline>
inlineFunctionCall(DfGraph &graph, const ControlFlowGraph &cfg, const DfInline &call, const size_t calleeInlineId) {
    std::vector<DfInline> retval;                       // next level for possible inlining

    // Insert the first basic block of the callee and create an edge from the caller to the callee.
    if (BasicBlock::Ptr entryBb = call.cfgCall->target()->value().bblock()) {
        auto dfCallee = graph.insertVertex(DfVertex(entryBb, calleeInlineId));
        graph.insertEdge(call.dfCaller, dfCallee, call.cfgCall->id());
    } else {
        return retval;
    }

    // Insert all the basic blocks for the callee
    using namespace Sawyer::Container::Algorithm;
    using Traversal = DepthFirstForwardGraphTraversal<const ControlFlowGraph>;
    for (Traversal t(cfg, call.cfgCall->target()); t; ++t) {
        if (t.event() == ENTER_EDGE) {
            BasicBlock::Ptr src = t.edge()->source()->value().bblock();
            BasicBlock::Ptr tgt = t.edge()->target()->value().bblock();
            if (src && tgt && t.edge()->value().type() == E_FUNCTION_CALL) {
                // The called function makes another function call. Skip this edge, but return the info for further inlining.
                const auto caller = graph.findVertexValue(DfVertex(src, call.callerInlineId));
                ASSERT_require(graph.isValidVertex(caller));
                const auto callret = graph.insertVertexMaybe(DfVertex(tgt, call.callerInlineId));
                ASSERT_require(graph.isValidVertex(callret));
                retval.push_back(DfInline(caller, callret, t.edge(), calleeInlineId));
                t.skipChildren();

            } else if (src && t.edge()->value().type() == E_FUNCTION_RETURN) {
                // The called function is returning. Create an edge from the returning vertex to the caller's callret vertex.
                const auto dfReturnFrom = graph.findVertexValue(DfVertex(src, calleeInlineId));
                ASSERT_require(graph.isValidVertex(dfReturnFrom));
                graph.insertEdge(dfReturnFrom, call.dfCallRet, t.edge()->id());
                t.skipChildren();

            } else if (src && tgt && t.edge()->value().type() != E_FUNCTION_CALL) {
                // Normal intra-function edge to be inlined into the dataflow graph
                graph.insertEdgeWithVertices(DfVertex(src, calleeInlineId), DfVertex(tgt, calleeInlineId), t.edge()->id());

            } else {
                t.skipChildren();
            }
        }
    }

    // Erase the old call-return edge(s) that goes from the call vertex to the vertex to which the call returns.
    std::vector<DfGraph::ConstEdgeIterator> toErase;
    for (const auto &dfEdge: call.dfCaller->outEdges()) {
        if (dfEdge.target() == call.dfCallRet) {
            const auto cfgEdge = cfg.findEdge(dfEdge.value());
            ASSERT_require(cfg.isValidEdge(cfgEdge));
            if (cfgEdge->value().type() == E_CALL_RETURN)
                toErase.push_back(graph.findEdge(dfEdge.id()));
        }
    }
    for (const auto &edge: toErase)
        graph.eraseEdge(edge);

    return retval;
}

static DfGraph::ConstVertexIterator
findCallRet(const DfGraph &graph, const ControlFlowGraph &cfg, const DfGraph::Vertex &dfVertex) {
    for (const auto &dfEdge: dfVertex.outEdges()) {
        const auto cfgEdge = cfg.findEdge(dfEdge.value());
        ASSERT_require(cfg.isValidEdge(cfgEdge));
        if (cfgEdge->value().type() == E_CALL_RETURN)
            return dfEdge.target();
    }
    return graph.vertices().end();
}

// Given a CFG vertex that has an indeterminate edge (i.e., a possible indirect branch), construct a dataflow graph that looks
// backward from that CFG vertex a certain distance. Return the dataflow graph and its starting vertex IDs.
static DfGraph
buildDfGraphInReverse(const Settings &settings, const Partitioner::ConstPtr &partitioner,
                      const ControlFlowGraph::Vertex &cfgVertex, const size_t maxDepth) {
    using namespace Sawyer::Container::Algorithm;

    ASSERT_not_null(partitioner);
    DfGraph graph;

    // Insert the ending vertex first so we're guaranteed that it has ID zero.
    if (BasicBlock::Ptr bb = cfgVertex.value().bblock()) {
        graph.insertVertex(DfVertex(bb, 0));
    } else {
        return graph;
    }

    // Work backward from the ending vertex
    size_t depth = 0;
    using Traversal = DepthFirstReverseGraphTraversal<const ControlFlowGraph>;
    for (Traversal t(partitioner->cfg(), partitioner->cfg().findVertex(cfgVertex.id())); t; ++t) {
        if (t.event() == ENTER_EDGE) {
            BasicBlock::Ptr src = t.edge()->source()->value().bblock();
            BasicBlock::Ptr tgt = t.edge()->target()->value().bblock();
            if (++depth <= maxDepth && src && tgt) {
                graph.insertEdgeWithVertices(DfVertex(src, 0), DfVertex(tgt, 0), t.edge()->id());
            } else {
                t.skipChildren();
            }
        } else if (t.event() == LEAVE_EDGE) {
            ASSERT_require(depth > 0);
            --depth;
        }
    }

    // Find vertices in the dataflow graph that call other functions.
    std::vector<DfInline> callsToInline;
    for (const auto &dfVertex: graph.vertices()) {
        const auto cfgVertex = partitioner->findPlaceholder(dfVertex.value().address());
        ASSERT_require(partitioner->cfg().isValidVertex(cfgVertex));
        for (const auto &cfgEdge: cfgVertex->outEdges()) {
            if (cfgEdge.value().type() == E_FUNCTION_CALL && cfgEdge.target()->value().type() == V_BASIC_BLOCK) {
                const auto dfCallRet = findCallRet(graph, partitioner->cfg(), dfVertex);
                if (graph.isValidVertex(dfCallRet)) {
                    const auto dfCallerIter = graph.findVertex(dfVertex.id());
                    const auto cfgEdgeIter = partitioner->cfg().findEdge(cfgEdge.id());
                    callsToInline.push_back(DfInline(dfCallerIter, dfCallRet, cfgEdgeIter, 0));
                }
            }
        }
    }

    // Repeatedly inline function calls to the specified depth. So far, all vertices have an inlineId of zero. Each time we inline
    // another function we'll use a new inlineId. This ensures that even if the same function is inlined more than once we can
    // uniquely identify each vertex in the dataflow graph by a combination of address and inlineId.
    size_t inlineId = 0;
    for (size_t i = 0; i < settings.maxInliningDepth && !callsToInline.empty(); ++i) {
        std::vector<DfInline> nextLevelCalls;
        for (const auto &call: callsToInline) {
            const auto next = inlineFunctionCall(graph, partitioner->cfg(), call, ++inlineId);
            nextLevelCalls.insert(nextLevelCalls.end(), next.begin(), next.end());
        }
        callsToInline = nextLevelCalls;
    }

    return graph;
}

static void
printGraph(std::ostream &out, const DfGraph &graph) {
    out <<"  vertices:\n";
    for (const auto &vertex: graph.vertices()) {
        out <<"    V" <<vertex.id() <<": " <<vertex.value().printableName() <<"\n";
        for (const auto &edge: vertex.outEdges()) {
            out <<"      E" <<edge.id() <<": cfg edge #" <<edge.value() <<" to " <<edge.target()->value().printableName() <<"\n";
        }
    }
}

// Test whether the specified vertex should be analyzed
static BasicBlock::Ptr
shouldAnalyze(const ControlFlowGraph::Vertex &vertex) {
    if (BasicBlock::Ptr bb = vertex.value().bblock()) {
        for (const auto &edge: vertex.outEdges()) {
            if (edge.target()->value().type() == V_INDETERMINATE &&
                edge.value().type() != E_FUNCTION_RETURN)
                return bb;
        }
    }
    return {};
}

static bool
useJumpTable(const Partitioner::Ptr &partitioner, const BasicBlock::Ptr &bb, const BS::RiscOperators::Ptr &ops) {
    ASSERT_not_null(partitioner);
    ASSERT_not_null(bb);
    ASSERT_not_null(ops);
    BS::State::Ptr state = ops->currentState();
    ASSERT_not_null(state);
    Sawyer::Message::Stream debug(mlog[DEBUG]);

    SAWYER_MESG(debug) <<"  results for " <<bb->printableName() <<"\n";
    const RegisterDescriptor IP = partitioner->architecture()->registerDictionary()->instructionPointerRegister();
    const SymbolicExpression::Ptr ip = SymbolicSemantics::SValue::promote(ops->peekRegister(IP))->get_expression();
    SAWYER_MESG(debug) <<"    ip = " <<*ip <<"\n";

    const RegisterDescriptor PATH = createPathRegister(partitioner);
    const SymbolicExpression::Ptr path = SymbolicSemantics::SValue::promote(ops->peekRegister(PATH))->get_expression();
    SAWYER_MESG(debug) <<"    path constraints = " <<*path <<"\n";

    // We only handle x86 for now (although this might work for other instruction sets too).
    if (as<const Architecture::X86>(partitioner->architecture())) {
        const auto matched = match(ip);
        const auto jumpTableEntry = matched.first;
        const Address perEntryOffset = matched.second;
        if (jumpTableEntry) {
            SAWYER_MESG(debug) <<"    value read from jump table: " <<*jumpTableEntry <<"\n";
            if (const auto jumpTableAddrExpr = findAddressContaining(ops, jumpTableEntry)) {
                SAWYER_MESG(debug) <<"    address from which it was read: " <<*jumpTableAddrExpr <<"\n";
                const std::set<Address> constants = findInterestingConstants(jumpTableAddrExpr);
                for (const Address tableAddr: constants) {
                    if (isMappedAccess(partitioner->memoryMap(), tableAddr, MemoryMap::READABLE, MemoryMap::WRITABLE)) {
                        SAWYER_MESG(debug) <<"    possible jump table at " <<addrToString(tableAddr) <<"\n"
                                           <<"    per-entry offset is " <<addrToString(perEntryOffset) <<"\n";

                        const auto tableLimits = AddressInterval::whole(); // will be refined
                        const size_t bytesPerEntry = (jumpTableEntry->nBits() + 7) / 8;
                        auto table = JumpTable::instance(partitioner, tableLimits, bytesPerEntry, perEntryOffset,
                                                         JumpTable::EntryType::ABSOLUTE);
                        table->maxPreEntries(0);
                        table->refineLocationLimits(bb, tableAddr);
                        SAWYER_MESG(debug) <<"    table limited to " <<addrToString(table->tableLimits()) <<"\n";

                        //table->refineTargetLimits(bb);
                        SAWYER_MESG(debug) <<"    targets limited to " <<addrToString(table->targetLimits()) <<"\n";

                        SAWYER_MESG(debug) <<"    scanning table at " <<addrToString(tableAddr)
                                           <<" within " <<addrToString(table->tableLimits()) <<"\n";
                        table->scan(partitioner->memoryMap()->require(MemoryMap::READABLE).prohibit(MemoryMap::WRITABLE), tableAddr);
                        if (table->location()) {
                            SAWYER_MESG(debug) <<"    parsed jump table with " <<plural(table->nEntries(), "entries") <<"\n";
                            std::set<Address> successors = satisfiableTargets(table, path, jumpTableAddrExpr);
                            if (debug) {
                                debug <<"    unique targets remaining: " <<successors.size() <<"\n";
                                for (const Address target: successors)
                                    debug <<"      target " <<addrToString(target) <<"\n";
                            }

                            partitioner->detachBasicBlock(bb);
                            bb->successors().clear();
                            const size_t bitsPerWord = partitioner->architecture()->bitsPerWord();
                            for (const Address successor: successors)
                                bb->insertSuccessor(successor, bitsPerWord);
                            table->attachTableToBasicBlock(bb);
                            partitioner->attachBasicBlock(bb);
                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}

bool
analyzeAllBlocks(const Settings &settings, const Partitioner::Ptr &partitioner) {
    ASSERT_not_null(partitioner);
    Sawyer::Message::Stream debug(mlog[DEBUG]);
    bool madeChanges = false;

    for (const auto &cfgVertex: partitioner->cfg().vertices()) {
        if (BasicBlock::Ptr bb = shouldAnalyze(cfgVertex)) {
            SAWYER_MESG(debug) <<"possible jump table for " <<bb->printableName() <<"\n";
            DfGraph dfGraph = buildDfGraphInReverse(settings, partitioner, cfgVertex, settings.maxReversePathLength);
            if (debug) {
                printGraph(debug, dfGraph);
                if (!dfGraph.isEmpty()) {
                    static size_t ncalls = 0;
                    const std::string fname = "dfgraph-" + addrToString(bb->address()).substr(2) +
                                              "-" + boost::lexical_cast<std::string>(++ncalls) + ".dot";
                    std::ofstream file(fname.c_str());
                    toGraphviz(file, dfGraph, partitioner);
                    debug <<"  also written to " <<fname <<"\n";
                }
            }
            if (dfGraph.isEmpty())
                continue;

            // Configure the dataflow engine
            BS::RiscOperators::Ptr ops = partitioner->newOperators();
            BS::Dispatcher::Ptr cpu = partitioner->newDispatcher(ops);
            if (!cpu)
                return false;

            DfTransfer xfer(partitioner, cpu);
            DfMerge merge(partitioner, ops, dfGraph);
            using DfEngine = BinaryAnalysis::DataFlow::Engine<DfGraph, BS::State::Ptr, DfTransfer, DfMerge>;
            DfEngine dfEngine(dfGraph, xfer, merge);
            dfEngine.maxIterations(dfGraph.nVertices() * settings.maxDataflowIterationFactor);

            // Choose dataflow starting points
            for (const auto &vertex: dfGraph.vertices()) {
                if (vertex.nInEdges() == 0)
                    dfEngine.insertStartingVertex(vertex.id(), xfer.initialState());
            }

            // Run the dataflow
            try {
                dfEngine.runToFixedPoint();     // probably won't reach a fixed point due to maxIterations set above
            } catch (const BinaryAnalysis::DataFlow::NotConverging&) {
            }

            // Examine the outgoing state for the basic block in question (dataflow vertex #0)
            if (BS::State::Ptr state = dfEngine.getFinalState(0)) {
                ops->currentState(state);
                std::cerr <<"ROBB: outgoing state:\n" <<*state;
                if (useJumpTable(partitioner, bb, ops))
                    madeChanges = true;
            }
        }
    }
    return madeChanges;
}

} // namespace
} // namespace
} // namespace
} // namespace
} // namespace

#endif
