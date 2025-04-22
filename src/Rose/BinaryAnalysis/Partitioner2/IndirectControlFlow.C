
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Diagnostics
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Settings and command-line parsing
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Create a parser that understands the command-line switches related to settings for this analysis.
Sawyer::CommandLine::SwitchGroup
commandLineSwitches(Settings &settings) {
    using namespace Sawyer::CommandLine;

    SwitchGroup sg("Indirect control flow recovery");
    sg.name("icf");
    sg.doc("These switches affect the indirect control flow recovery algorithms primarily used when partitioning a binary specimen "
           "into instructions, basic blocks, static data blocks, and functions to construct the control flow graph and related "
           "data structures.\n\n"

           "When analyzing indirect control flow from a particular basic block, the dataflow graph is constructed from the control "
           "flow graph as follows: first, starting at the basic block in question, CFG edges are followed in reverse up to a "
           "configurable maximum distance within the same function but skipping over calls to other functions. This step finds "
           "the starting points for the dataflow analysis. Second, starting from those points, a forward traversal attempts to "
           "reach the basic block in question, this time inlining called functions into the dataflow graph to a specified depth, "
           "or inserting a placeholder for the called function.");

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
// Functions for dataflow
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Create a special register that is used to store the execution path constraints.
static RegisterDescriptor
createPathRegister(const Partitioner::ConstPtr &partitioner) {
    ASSERT_not_null(partitioner);
    const unsigned maj = partitioner->architecture()->registerDictionary()->firstUnusedMajor();
    const unsigned min = partitioner->architecture()->registerDictionary()->firstUnusedMinor(maj);
    return RegisterDescriptor(maj, min, 0, 1);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Dataflow graph vertices nominally point to basic blocks. If `fakedCall` is not empty then the transfer function handles the
// vertex in a special way: instead of symbolically executing the basic block's instructions, it pretends that the function does
// something else entirely and a return-from-function is executed.
class DfVertex {
public:
    const BasicBlock::Ptr bblock;
    const size_t inlineId = 0;
    std::string fakedCall;                              // instad of executing the bblock, it represents an entire function

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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Dataflow vertices are indexed by address and inline ID. In a CFG, the basic block starting address is sufficient to uniquely
// identify a vertex (at least for those vertices that are basic blocks). But the dataflow graph is different in that a function
// might be inlined (copied) into the graph multiple times. Therefore, each copy of the function will get a unique inlining ID.
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// A dataflow graph contains vertices that nominally point to basic blocks and are sorted by their corresponding DfVertexKey, and
// edges that nominally reference a CFG edge by mentioning its edge ID.  Not all edges in the dataflow graph correspond to edges in
// the CFG. For instance, when the dataflow graph contains a faked function, there is probably no CFG edge that points from the
// first block of the faked function to the vertex to which that function would return.
using DfGraph = Sawyer::Container::Graph<
    DfVertex,
    Sawyer::Optional<size_t>,                           // CFG edge ID
    DfVertexKey                                         // vertices sorted by basic block starting address
    >;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Functions can be inlined (copied) into the dataflow graph from the CFG. This class holds information about function calls.
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The dataflow transfer function is responsible for taking an incoming state and a dataflow vertex (nominally a basic block) and
// producing an outgoing state by symbolically executing the instructions of the basic block. Some dataflow vertices serve special
// purposes known to the transfer function.
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

    // Initialize the state with a reasonable concrete initial stack pointer.
    void initializeStackPointer(const BS::RiscOperators::Ptr &ops) const {
        ASSERT_not_null(ops);
        Sawyer::Message::Stream debug(mlog[DEBUG]);

        const RegisterDescriptor SP = partitioner_->architecture()->registerDictionary()->stackPointerRegister();
        ASSERT_require(SP);

        const size_t stackSize = 2 * 1024 * 1024;
        const size_t stackAlignment = 1024 * 1024;

        const AddressInterval search = AddressInterval::hull(0x80000000, 0xffffffff);
        const Sawyer::Optional<Address> stackBottom =
            partitioner_->memoryMap()->findFreeSpace(stackSize, stackAlignment, search, Sawyer::Container::MATCH_BACKWARD);
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

    BS::State::Ptr initialState() const {
        BS::RiscOperators::Ptr ops = cpu_->operators();
        BS::State::Ptr retval = ops->currentState()->clone();
        ops->currentState(retval);

        retval->clear();
        cpu_->initializeState(retval);
        initializeStackPointer(ops);
        return retval;
    }

    BS::State::Ptr operator()(const DfGraph &dfGraph, size_t vertexId, const BS::State::Ptr &incomingState) const {
        ASSERT_not_null(incomingState);

        BS::State::Ptr retval = incomingState->clone();
        BS::RiscOperators::Ptr ops = cpu_->operators();
        ops->currentState(retval);

        const auto dfVertex = dfGraph.findVertex(vertexId);
        ASSERT_require(dfGraph.isValidVertex(dfVertex));

        const size_t bitsPerWord = partitioner_->architecture()->bitsPerWord();

        const CallingConvention::Definition::Ptr cc = [this]() {
            const auto ccDefs = partitioner_->architecture()->callingConventions();
            if (ccDefs.empty()) {
                return CallingConvention::Definition::Ptr();
            } else {
                return ccDefs[0];
            }
        }();

        if (dfVertex->value().fakedCall.empty()) {
            // A normal basic block. The new state is created by symbolically executing the block's instructions.
            BasicBlock::Ptr bb = dfGraph.findVertex(vertexId)->value().bblock;
            for (SgAsmInstruction *insn: bb->instructions()) {
                try {
                    //std::cerr <<"ROBB: " <<insn->toString() <<"\n";
                    cpu_->processInstruction(insn);
                    //std::cerr <<"ROBB: state after insn:\n" <<*retval;
                } catch (const BS::Exception&) {
                }
            }

        } else {
            // Simulate a function return
            const RegisterDescriptor IP = partitioner_->architecture()->registerDictionary()->instructionPointerRegister();
            const RegisterDescriptor SP = partitioner_->architecture()->registerDictionary()->stackPointerRegister();
            const RegisterDescriptor LR = partitioner_->architecture()->registerDictionary()->callReturnRegister();
            const BS::SValue::Ptr t = ops->boolean_(true);

            // Compute new instruction pointer and stack pointer values if necessary.
            BS::SValue::Ptr newIp, newSp;
            if (LR) {
                newIp = ops->readRegister(LR, ops->undefined_(LR.nBits()));
            } else {
                const auto sp = ops->readRegister(SP, ops->undefined_(SP.nBits()));
                newIp = ops->readMemory(RegisterDescriptor(), sp, ops->undefined_(IP.nBits()), t);
                newSp = ops->add(sp, ops->number_(SP.nBits(), SP.nBits()/8));
            }

            // Clobber registers that the callee is allowed to change. These might include some things we computed above, so we
            // clobber them before we write the results above.
            if (cc) {
                for (const RegisterDescriptor reg: cc->scratchRegisters())
                    ops->writeRegister(reg, ops->undefined_(reg.nBits()));

                for (const ConcreteLocation &loc: cc->outputParameters()) {
                    switch (loc.type()) {
                        case ConcreteLocation::REGISTER:
                            ops->writeRegister(loc.reg(), ops->undefined_(loc.reg().nBits()));
                            break;

                        case ConcreteLocation::RELATIVE: {
                            const auto t = ops->boolean_(true);
                            const auto base = ops->readRegister(loc.reg(), ops->undefined_(loc.reg().nBits()));
                            const auto addr = ops->add(base, ops->number_(base->nBits(), loc.offset()));
                            if (SymbolicSemantics::SValue::promote(addr)->get_expression()->isIntegerConstant())
                                ops->writeMemory(RegisterDescriptor(), addr, ops->undefined_(bitsPerWord), t);
                            break;
                        }

                        case ConcreteLocation::ABSOLUTE: {
                            const auto t = ops->boolean_(true);
                            const auto addr = ops->number_(IP.nBits(), loc.address());
                            ops->writeMemory(RegisterDescriptor(), addr, ops->undefined_(bitsPerWord), t);
                            break;
                        }

                        case ConcreteLocation::NO_LOCATION:
                            break;
                    }
                }
            }

            // Update the stuff we calculated above
            if (newIp)
                ops->writeRegister(IP, newIp);
            if (newSp)
                ops->writeRegister(SP, newSp);
        }
        return retval;
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The dataflow merge function is responsible for merging two symbolic states when control flow is joined from two or more edges
// entering a vertex. For instance, an `if` statement flows two directions (the `if` body when the condition is true, and skipping
// it when the condition is false) and therefore those two states will need to be merged at the block immediately following the `if`
// statement.
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Hash the dataflow graph
static uint64_t
hashGraph(const DfGraph &graph) {
    // Hash each vertex individually into the `hashedVertices` list
    std::vector<std::pair<BasicBlock::Ptr, uint64_t>> hashedVertices;
    hashedVertices.reserve(graph.nVertices());

    for (const auto &vertex: graph.vertices()) {
        // Hash this vertex
        Combinatorics::HasherSha256Builtin hasher;
        hasher.insert(vertex.value().bblock->address());
        hasher.insert(vertex.value().bblock->nInstructions());

        // Hash the outgoing edges in order of the target addresses
        std::vector<BasicBlock::Ptr> targets;
        targets.reserve(vertex.nOutEdges());
        for (const auto &edge: vertex.outEdges())
            targets.push_back(edge.target()->value().bblock);
        std::sort(targets.begin(), targets.end(), [](const BasicBlock::Ptr &a, const BasicBlock::Ptr &b) {
            return a->address() < b->address();
        });
        for (const auto &target: targets) {
            hasher.insert(target->address());
            hasher.insert(target->nInstructions());
        }

        // Save the results for this vertex because we'll need to process them in order by their address
        hashedVertices.push_back(std::make_pair(vertex.value().bblock, hasher.make64Bits()));
    }

    // Combine all the vertex hashes in order by the vertex address
    std::sort(hashedVertices.begin(), hashedVertices.end(),
              [](const std::pair<BasicBlock::Ptr, uint64_t> &a, const std::pair<BasicBlock::Ptr, uint64_t> &b) {
                  return a.first->address() < b.first->address();
              });
    Combinatorics::HasherSha256Builtin hasher;
    for (const auto &pair: hashedVertices) {
        hasher.insert(pair.first->address());
        hasher.insert(pair.first->nInstructions());
        hasher.insert(pair.second);
    }

    return hasher.make64Bits();
}

// Print a dataflow graph in Graphviz format for debugging.
static void
toGraphviz(std::ostream &out, const DfGraph &graph, const Partitioner::ConstPtr &partitioner) {
    ASSERT_not_null(partitioner);

    const Color::HSV entryColor(0.33, 1.0, 0.9);        // light green
    const Color::HSV targetColor(0.67, 1.0, 0.9);       // light blue

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
                if (dfVertex.id() == 0) {
                    out <<" shape=box style=filled fillcolor=\"" <<targetColor.toHtml() <<"\"";
                } else if (dfVertex.nInEdges() == 0) {
                    out <<" shape=box style=filled fillcolor=\"" <<entryColor.toHtml() <<"\"";
                }
                out <<" label=<<b>Vertex " <<dfVertex.id() <<"</b>";

                if (dfVertex.value().fakedCall.empty()) {
                    BasicBlock::Ptr bb = partitioner->basicBlockExists(dfVertex.value().address());
                    ASSERT_not_null(bb);
                    for (SgAsmInstruction *insn: bb->instructions())
                        out <<"<br align=\"left\"/>" <<GraphViz::htmlEscape(insn->toStringNoColor());
                } else {
                    out <<"<br align=\"left\"/>faked call to " <<GraphViz::htmlEscape(dfVertex.value().fakedCall);
                }
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

// Simple output function for debugging
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

// Inline into the dataflow graph a function called via CFG edge. Return a list of any new dataflow vertices that have subsequent
// function calls that were not inlined yet.
static std::vector<DfInline>
inlineFunctionCall(DfGraph &graph, const Partitioner::ConstPtr &partitioner, const DfInline &call, const size_t calleeInlineId) {
    ASSERT_not_null(partitioner);
    const ControlFlowGraph &cfg = partitioner->cfg();
    std::vector<DfInline> retval;                       // next level for possible inlining

#ifndef NDEBUG
    {
        ASSERT_require(graph.isValidVertex(call.dfCaller));
        ASSERT_require(call.dfCaller->value().inlineId == call.callerInlineId);
        ASSERT_not_null(call.dfCaller->value().bblock);

        ASSERT_require(graph.isValidVertex(call.dfCallRet));
        ASSERT_require(call.dfCallRet->value().inlineId == call.callerInlineId);
        ASSERT_not_null(call.dfCallRet->value().bblock);

        ASSERT_require(cfg.isValidEdge(call.cfgCall));
        ASSERT_require(call.cfgCall->source()->value().type() == V_BASIC_BLOCK);
        const auto callerAddr = call.cfgCall->source()->value().optionalAddress();
        ASSERT_require(callerAddr);
        const auto callerBb = partitioner->basicBlockExists(*callerAddr);
        ASSERT_not_null(callerBb);
        ASSERT_require(callerBb->address() == *callerAddr);
        const auto callerVertex = graph.findVertexValue(DfVertex(callerBb, call.callerInlineId));
        ASSERT_require(graph.isValidVertex(callerVertex));
    }
#endif

    // Insert the first basic block of the callee and create an edge from the caller to the callee.
    DfGraph::VertexIterator dfCallee = graph.vertices().end();
    if (BasicBlock::Ptr entryBb = call.cfgCall->target()->value().bblock()) {
        dfCallee = graph.insertVertex(DfVertex(entryBb, calleeInlineId));
        graph.insertEdge(call.dfCaller, dfCallee, call.cfgCall->id());
    } else {
        return retval;
    }
    ASSERT_require(graph.isValidVertex(dfCallee));

    // Handle calls to functions in shared libraries by faking the function.
    if (Function::Ptr function = partitioner->functionExists(dfCallee->value().address())) {
        if (boost::ends_with(function->name(), "@plt")) {
            dfCallee->value().fakedCall = function->name();
            graph.insertEdge(dfCallee, call.dfCallRet, Sawyer::Nothing());
        }
    }

    // Insert all the basic blocks for the callee
    if (dfCallee->value().fakedCall.empty()) {
        using namespace Sawyer::Container::Algorithm;
        using Traversal = DepthFirstForwardGraphTraversal<const ControlFlowGraph>;
        for (Traversal t(cfg, call.cfgCall->target()); t; ++t) {
            if (t.event() == ENTER_EDGE) {
                BasicBlock::Ptr src = t.edge()->source()->value().bblock();
                BasicBlock::Ptr tgt = t.edge()->target()->value().bblock();
                if (src && tgt && t.edge()->value().type() == E_FUNCTION_CALL) {
                    // The called function makes another function call. Skip this edge, but return the info for further inlining.
                    const auto caller = graph.findVertexValue(DfVertex(src, calleeInlineId));
                    ASSERT_require(graph.isValidVertex(caller));
                    const auto callret = graph.insertVertexMaybe(DfVertex(tgt, calleeInlineId));
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
    }

    // Erase the old call-return edge(s) that goes from the call vertex to the vertex to which the call returns.
    std::vector<DfGraph::ConstEdgeIterator> toErase;
    for (const auto &dfEdge: call.dfCaller->outEdges()) {
        if (dfEdge.target() == call.dfCallRet) {
            const auto cfgEdge = cfg.findEdge(dfEdge.value().orElse(UNLIMITED));
            if (cfg.isValidEdge(cfgEdge) && cfgEdge->value().type() == E_CALL_RETURN)
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
        const auto cfgEdge = cfg.findEdge(dfEdge.value().orElse(UNLIMITED));
        if (cfg.isValidEdge(cfgEdge) && cfgEdge->value().type() == E_CALL_RETURN)
            return dfEdge.target();
    }
    return graph.vertices().end();
}

// Modify the graph by removing all vertices that cannot reach the targetId vertex by any path.
static void
removeUnreachableReverse(DfGraph &graph, const size_t targetId) {
    using namespace Sawyer::Container::Algorithm;
    using Traversal = DepthFirstReverseGraphTraversal<DfGraph>;
    std::vector<bool> reachable(graph.nVertices(), false);
    for (Traversal t(graph, graph.findVertex(targetId)); t; ++t) {
        if (t.event() == ENTER_VERTEX)
            reachable[t.vertex()->id()] = true;
    }

    std::vector<DfGraph::VertexIterator> toRemove;
    for (size_t vertexId = 0; vertexId < reachable.size(); ++vertexId) {
        if (!reachable[vertexId])
            toRemove.push_back(graph.findVertex(vertexId));
    }

    for (const auto &vertex: toRemove)
        graph.eraseVertex(vertex);
}

// Given a CFG vertex that has an indeterminate edge (i.e., a possible indirect branch), construct a dataflow graph that looks
// backward from that CFG vertex a certain distance. Return the dataflow graph and its starting vertex IDs.
static DfGraph
buildDfGraph(const Settings &settings, const Partitioner::ConstPtr &partitioner, const ControlFlowGraph::Vertex &cfgVertex,
             const size_t maxDepth) {
    using namespace Sawyer::Container::Algorithm;

    ASSERT_not_null(partitioner);
    DfGraph graph;

    // Insert the ending vertex first so we're guaranteed that it has ID zero.
    BasicBlock::Ptr endBb = cfgVertex.value().bblock();
    if (endBb) {
        graph.insertVertex(DfVertex(endBb, 0));
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
            if (++depth <= maxDepth && src && tgt && src != endBb) {
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
            const auto next = inlineFunctionCall(graph, partitioner, call, ++inlineId);
            nextLevelCalls.insert(nextLevelCalls.end(), next.begin(), next.end());
        }
        callsToInline = nextLevelCalls;
    }

    // Remove parts of the graph that can't reach the target vertex (vertex #0)
    removeUnreachableReverse(graph, 0);

    return graph;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
//// ICF Recovery Strategy: Compiler-generated, static, read-only jump tables.
//// 
//// Functions for analyzing static jump tables, such as what the C compiler generates for `switch` statements.
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace StaticJumpTable {

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

/*  This analysis attempts to resolve the indirect control flow by discovering and using statically created jump tables,
 *  such as what a C compiler typically creates for `switch` statements.
 *
 *  For each such block having an indeterminate outgoing CFG edge, a dataflow graph is created from nearby basic blocks. The
 *  dataflow analysis uses instruction semantics to update the machine state associated with each dataflow graph vertex. A special
 *  merge operation merges states when control flow merges, such as at the end of an `if` statement. The analysis examines the final
 *  outgoing state for the basic block in question to ascertain whether a static jump table was used, what its starting address
 *  might be, and how entries in the table are used to compute target addresses for the jump instruction.
 *
 *  Besides containing the typical symbolic registers and memory, the state also contains constraints imposed by the execution path.
 *  These constraints are used to limit the size of the jump table. Ideally, even if the compiler emits two consecutive tables for
 *  two different `switch` statements, the basic block for each `switch` statement will have only successors that are relevant to
 *  that statement. */
static bool
useJumpTable(const Partitioner::Ptr &partitioner, const BasicBlock::Ptr &bb, const BS::RiscOperators::Ptr &ops) {
    ASSERT_not_null(partitioner);
    ASSERT_not_null(bb);
    ASSERT_not_null(ops);
    BS::State::Ptr state = ops->currentState();
    ASSERT_not_null(state);
    Sawyer::Message::Stream debug(mlog[DEBUG]);


    // We only handle x86 for now (although this might work for other instruction sets too).
    if (as<const Architecture::X86>(partitioner->architecture())) {
        SAWYER_MESG(debug) <<"  possible jump table for " <<bb->printableName() <<"\n";
        const RegisterDescriptor IP = partitioner->architecture()->registerDictionary()->instructionPointerRegister();
        const SymbolicExpression::Ptr ip = SymbolicSemantics::SValue::promote(ops->peekRegister(IP))->get_expression();
        SAWYER_MESG(debug) <<"    ip = " <<*ip <<"\n";

        const RegisterDescriptor PATH = createPathRegister(partitioner);
        const SymbolicExpression::Ptr path = SymbolicSemantics::SValue::promote(ops->peekRegister(PATH))->get_expression();
        SAWYER_MESG(debug) <<"    path constraints = " <<*path <<"\n";

        const auto matched = match(ip);
        const auto jumpTableEntry = matched.first;
        const Address perEntryOffset = matched.second;
        if (!jumpTableEntry) {
            SAWYER_MESG(debug) <<"    jump table IP expression not matched (no jump table)\n";
            return false;
        }
        SAWYER_MESG(debug) <<"    value read from jump table: " <<*jumpTableEntry <<"\n";

        const auto jumpTableAddrExpr = findAddressContaining(ops, jumpTableEntry);
        if (!jumpTableAddrExpr) {
            SAWYER_MESG(debug) <<"    cannot find jump table entry in memory state\n";
            return false;
        }
        SAWYER_MESG(debug) <<"    address from which it was read: " <<*jumpTableAddrExpr <<"\n";

        const std::set<Address> constants = findInterestingConstants(jumpTableAddrExpr);
        for (const Address tableAddr: constants) {
            SAWYER_MESG(debug) <<"    potential table address " <<addrToString(tableAddr) <<"\n";
            if (!isMappedAccess(partitioner->memoryMap(), tableAddr, MemoryMap::READABLE, MemoryMap::WRITABLE)) {
                SAWYER_MESG(debug) <<"      potential table entry is not readable, or is writable\n";
                continue;
            }
            SAWYER_MESG(debug) <<"      possible jump table at " <<addrToString(tableAddr) <<"\n"
                               <<"      per-entry offset is " <<addrToString(perEntryOffset) <<"\n";

            const auto tableLimits = AddressInterval::whole(); // will be refined
            const size_t bytesPerEntry = (jumpTableEntry->nBits() + 7) / 8;
            auto table = JumpTable::instance(partitioner, tableLimits, bytesPerEntry, perEntryOffset,
                                             JumpTable::EntryType::ABSOLUTE);
            table->maxPreEntries(0);
            table->refineLocationLimits(bb, tableAddr);
            SAWYER_MESG(debug) <<"      table limited to " <<addrToString(table->tableLimits()) <<"\n";
            SAWYER_MESG(debug) <<"      targets limited to " <<addrToString(table->targetLimits()) <<"\n";
            SAWYER_MESG(debug) <<"      scanning table at " <<addrToString(tableAddr)
                               <<" within " <<addrToString(table->tableLimits()) <<"\n";
            table->scan(partitioner->memoryMap()->require(MemoryMap::READABLE).prohibit(MemoryMap::WRITABLE), tableAddr);
            if (!table->location()) {
                SAWYER_MESG(debug) <<"      table not found at " <<addrToString(tableAddr) <<"\n";
                return false;
            }
            SAWYER_MESG(debug) <<"      parsed jump table at " <<addrToString(tableAddr)
                               <<" with " <<plural(table->nEntries(), "entries") <<"\n";

            std::set<Address> successors = satisfiableTargets(table, path, jumpTableAddrExpr);
            if (debug) {
                debug <<"      unique targets remaining: " <<successors.size() <<"\n";
                for (const Address target: successors)
                    debug <<"        target " <<addrToString(target) <<"\n";
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
    return false;
}

} // namespace

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
//// ICF Recovery strategy: final state already has a concrete instruction pointer.
////
//// If the dataflow itself can resolve the indirect control flow to a single value, then use that value.
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ConcreteIp {

static bool
useConcreteIp(const Partitioner::Ptr &partitioner, const BasicBlock::Ptr &bb, const BS::RiscOperators::Ptr &ops) {
    ASSERT_not_null(partitioner);
    ASSERT_not_null(bb);
    ASSERT_not_null(ops);

    const RegisterDescriptor IP = partitioner->architecture()->registerDictionary()->instructionPointerRegister();
    const SymbolicExpression::Ptr ip = SymbolicSemantics::SValue::promote(ops->peekRegister(IP))->get_expression();
    if (const auto addr = ip->toUnsigned()) {
        Sawyer::Message::Stream debug(mlog[DEBUG]);
        SAWYER_MESG(debug) <<"  IP is concrete: " <<*ip <<"\n";

        partitioner->detachBasicBlock(bb);
        bb->successors().clear();
        bb->insertSuccessor(*addr, partitioner->architecture()->bitsPerWord());
        partitioner->attachBasicBlock(bb);
        return true;
    }

    return false;
}

} // namespace

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
//// Top level functions employing various strategies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Analyze one basic block (and possibly its neighbors in the CFG) to try to resolve indirect control flow for that block. Return
// true if the CFG successors are changed for that block.
bool
analyzeBasicBlock(const Settings &settings, const Partitioner::Ptr &partitioner, const BasicBlock::Ptr &bb) {
    ASSERT_not_null(partitioner);
    ASSERT_not_null(bb);
    Sawyer::Message::Stream debug(mlog[DEBUG]);
    SAWYER_MESG(debug) <<"analyzing " <<bb->printableName() <<"\n";

    // Build the dataflow graph
    const ControlFlowGraph::VertexIterator cfgVertex = partitioner->findPlaceholder(bb->address());
    ASSERT_require(partitioner->cfg().isValidVertex(cfgVertex));
    DfGraph dfGraph = buildDfGraph(settings, partitioner, *cfgVertex, settings.maxReversePathLength);
    if (dfGraph.isEmpty()) {
        SAWYER_MESG(debug) <<"  dataflow graph is empty\n";
        return false;
    }
    const uint64_t hash = hashGraph(dfGraph);
    if (!partitioner->icf().hashedGraphs.insert(hash).second) {
        SAWYER_MESG(debug) <<"  previously analyzed this dataflow graph\n";
        return false;
    }
    if (debug) {
        printGraph(debug, dfGraph);
        if (!dfGraph.isEmpty()) {
            const std::string fname = "dfgraph-" + addrToString(bb->address()).substr(2) +
                                      "-H" + addrToString(hash).substr(2) + ".dot";
            std::ofstream file(fname.c_str());
            toGraphviz(file, dfGraph, partitioner);
            debug <<"  also written to " <<fname <<"\n";
        }
    }

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
        if (ConcreteIp::useConcreteIp(partitioner, bb, ops)) {
            SAWYER_MESG(debug) <<"  resolved indirect control flow using constant IP for " <<bb->printableName() <<"\n";
            return true;
        } else if (StaticJumpTable::useJumpTable(partitioner, bb, ops)) {
            SAWYER_MESG(debug) <<"  resolved indirect control flow using static jump table for " <<bb->printableName() <<"\n";
            return true;
        }
    }

    return false;
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

bool
analyzeAllBlocks(const Settings &settings, const Partitioner::Ptr &partitioner) {
    ASSERT_not_null(partitioner);
    bool madeChanges = false;

    for (const auto &cfgVertex: partitioner->cfg().vertices()) {
        if (BasicBlock::Ptr bb = shouldAnalyze(cfgVertex)) {
            if (analyzeBasicBlock(settings, partitioner, bb))
                madeChanges = true;
        }
    }
    return madeChanges;
}

} // namespace
} // namespace
} // namespace
} // namespace

#endif
