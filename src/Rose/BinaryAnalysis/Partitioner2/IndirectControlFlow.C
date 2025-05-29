
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
#include <Rose/CommandLine.h>
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

enum class Debugging {OFF, ON};

static Debugging
enableDiagnostics(Sawyer::Message::Stream &stream, const Settings &settings, const BasicBlock::Ptr &bb) {
    if (settings.debugAddress && bb && !bb->instructions().empty() &&
        bb->instructions().back()->get_address() == *settings.debugAddress) {
        stream.enable();
        return Debugging::ON;
    } else {
        return Debugging::OFF;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Settings and command-line parsing
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Create a parser that understands the command-line switches related to settings for this analysis.
Sawyer::CommandLine::SwitchGroup
commandLineSwitches(Settings &settings) {
    using namespace Sawyer::CommandLine;
    using Rose::CommandLine::insertBooleanSwitch;
    using Rose::CommandLine::insertEnableSwitch;

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

    insertEnableSwitch(sg, "", "indirect control flow recovery", settings.enabled,
                       "Indirect control flow (ICF) recovery is a collection of analyses that attempt to discover the control "
                       "flow graph successors for an instruction (or sequence of instructions) that calculates the successor "
                       "addresses at runtime. When disabled, none of the other ICF switches have any effect.");

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

    sg.insert(Switch("df-max-vertices")
              .argument("n", nonNegativeIntegerParser(settings.maxDataflowVertices))
              .doc("If the dataflow graph contains more than @v{n} vertices then the dataflow is skipped. Either some other "
                   "analysis will be performed, or the indirect control flow will be represented by a CFG edge that points to "
                   "an indeterminate address. The default is " + plural(settings.maxDataflowVertices, "vertices") + "."));

    sg.insert(Switch("df-max-valueset")
              .argument("n", nonNegativeIntegerParser(settings.maxDataflowSetSize))
              .doc("During dataflow, this is the maximum number of distinct values to save in a set when merging states from one "
                   "or more graph edges. The default is " + plural(settings.maxDataflowSetSize, "values") + "."));

    sg.insert(Switch("max-symbolic-size")
              .argument("n", nonNegativeIntegerParser(settings.maxSymbolicExprSize))
              .doc("Maximum number of nodes to permit in a symbolic expression before the expression is folded into a new "
                   "unconstrained symbolic variable. The default is " + plural(settings.maxSymbolicExprSize, "nodes") + "."));

    insertBooleanSwitch(sg, "reanalyze", settings.reanalyzeSomeBlocks,
                        "Perform a second pass after the CFG is recurisively constructed. This pass reanalyzes basic blocks that "
                        "were analyzed by the first phase during CFG construction. The second phase is a single pass over all "
                        "such blocks whose dataflow graph has changed due to having a more complete CFG available.");

    sg.insert(Switch("debug")
              .argument("address", nonNegativeIntegerParser(settings.debugAddress))
              .doc("Turn on extensive debugging for one particular address. The address is the instruction that has the "
                   "indirect control flow."));

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
    const BasicBlock::Ptr bblock_;                      // null for indeterminate vertices
    const size_t inlineId_ = 0;
    std::string fakedCall_;                             // instad of executing the bblock, it represents an entire function

    DfVertex() = delete;

public:
    // Vertex for a particular basic block with a distinct starting address.
    DfVertex(const BasicBlock::Ptr &bb, const size_t inlineId)
        : bblock_(notnull(bb)), inlineId_(inlineId) {}

    // Vertex for an indeterminate basic block at some unknown address.
    DfVertex(const std::string &name, const size_t inlineId)
        : inlineId_(inlineId), fakedCall_(name) {
        ASSERT_forbid(name.empty());
    }

    // Starting address for the vertex if one is known, nothing for vertices with indeterminate addresses.
    Sawyer::Optional<Address> address() const {
        if (bblock_) {
            return bblock_->address();
        } else {
            return {};
        }
    }

    // Basic block for this vertex if it has one. Indeterminate vertices have no basic block.
    BasicBlock::Ptr basicBlock() const {
        return bblock_;
    }

    // Printable name of this vertex.
    std::string printableName() const {
        if (bblock_) {
            return bblock_->printableName();
        } else {
            ASSERT_forbid(fakedCall_.empty());
            return fakedCall_;
        }
    }

    // Inline ID for this vertex. The base function has ID zero. Each subsequently inlined function increments the ID.
    size_t inlineId() const {
        return inlineId_;
    }

    // The name for a faked call, or the name for an indeterminate address.
    const std::string& fakedCall() const {
        return fakedCall_;
    }

    // Mark this vertex as a faked call
    void fakedCall(const std::string &name) {
        ASSERT_forbid(name.empty());
        fakedCall_ = name;
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Dataflow vertices are indexed by address (if they have one, otherwise faked name) and inline ID. In a CFG, the basic block
// starting address is sufficient to uniquely identify a vertex (at least for those vertices that are basic blocks). But the
// dataflow graph is different in that a function might be inlined (copied) into the graph multiple times. Therefore, each copy of
// the function will get a unique inlining ID.
class DfVertexKey {
public:
    const Address addr = 0;
    const size_t inlineId = 0;
    const std::string name;

    DfVertexKey() = delete;

    DfVertexKey(const DfVertex &vertex)
        : addr(vertex.address().orElse(0)), inlineId(vertex.inlineId()), name(vertex.fakedCall()) {}

    bool operator<(const DfVertexKey &other) const {
        if (addr != other.addr) {
            return addr < other.addr;
        } else if (name != other.name) {
            return name < other.name;
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
    enum class Method {                                 // inlining technique
        FAKE,                                           // inline only the first basic block and mark it as a fake function
        NORMAL                                          // inline all basic blocks if possible, otherwise fake it
    };
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
    const Debugging debugging_ = Debugging::OFF;
    const size_t maxSetSize_ = 1;

public:
    DfTransfer(const Settings &settings, const Partitioner::ConstPtr &partitioner, const BS::Dispatcher::Ptr &cpu,
               const Debugging debugging)
        : partitioner_(notnull(partitioner)), cpu_(notnull(cpu)), debugging_(debugging), maxSetSize_(settings.maxDataflowSetSize) {}

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
        if (debugging_ == Debugging::ON)
            debug.enable();

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

        auto merger = SymbolicSemantics::Merger::instance(maxSetSize_);
        retval->registerState()->merger(merger);
        retval->memoryState()->merger(merger);

        return retval;
    }

    BS::State::Ptr operator()(const DfGraph &dfGraph, size_t vertexId, const BS::State::Ptr &incomingState) const {
        ASSERT_not_null(incomingState);
        Sawyer::Message::Stream debug(mlog[DEBUG]);
        if (debugging_ == Debugging::ON)
            debug.enable();

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

        if (dfVertex->value().fakedCall().empty()) {
            // A normal basic block. The new state is created by symbolically executing the block's instructions.
            BasicBlock::Ptr bb = dfGraph.findVertex(vertexId)->value().basicBlock();
            ASSERT_not_null(bb);
            SAWYER_MESG(debug) <<"    dataflow transfer function for vertex #" <<vertexId <<" " <<bb->printableName() <<"\n";
            for (SgAsmInstruction *insn: bb->instructions()) {
                try {
                    SAWYER_MESG(debug) <<"      executing " <<insn->toString() <<"\n";
                    cpu_->processInstruction(insn);
                    if (debug) {
                        debug <<"      state after executing " <<insn->toString() <<"\n";
                        retval->print(debug, "        ");
                    }
                } catch (const BS::Exception &e) {
                    SAWYER_MESG(debug) <<"      execution failed (ignored): " <<e.what() <<"\n";
                }
            }

        } else {
            SAWYER_MESG(debug) <<"      faking a function call and return for function " <<dfVertex->value().fakedCall() <<"\n";
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
            if (debug) {
                debug <<"      state after executing " <<dfVertex->value().printableName() <<"\n";
                retval->print(debug, "        ");
            }
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
        const BS::RiscOperators::Ptr ops = notnull(operators());

        // Symbolic expression for the expected successor(s)
        const auto srcIp = SymbolicSemantics::SValue::promote(src->readRegister(IP, ops->undefined_(IP.nBits()), ops.get()))
                           ->get_expression();

        // Initial path constraints for the destination (before merging)
        SymbolicExpression::Ptr origConstraints;
        if (dst) {
            origConstraints = SymbolicSemantics::SValue::promote(dst->peekRegister(PATH, ops->boolean_(true), ops.get()))
                              ->get_expression();
        }

        // In order to get to the destination, we must have also satisfied the constraints to get to the source state.
        const auto srcConstraints = SymbolicSemantics::SValue::promote(src->peekRegister(PATH, ops->boolean_(true), ops.get()))
                                    ->get_expression();

        bool retval = Super::operator()(dstId, dst, srcId, src);
        ASSERT_not_null(dst);

        // New constraints for the destination
        if (const auto addr = dstVert->value().address()) {
            const SymbolicExpression::Ptr dstAddr = makeIntegerConstant(srcIp->nBits(), *addr);
            auto newConstraints = makeAnd(srcConstraints, makeEq(srcIp, dstAddr));
            if (origConstraints)
                newConstraints = makeOr(newConstraints, origConstraints);
            auto newConstraintsSVal = SymbolicSemantics::SValue::instance_symbolic(newConstraints);
            dst->writeRegister(PATH, newConstraintsSVal, ops.get());
        } else if (origConstraints) {
            auto newConstraintsSVal = SymbolicSemantics::SValue::instance_symbolic(origConstraints);
            dst->writeRegister(PATH, newConstraintsSVal, ops.get());
        }

        return retval;
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Base functor that tries to resolve indirect control flow from the final dataflow state.

class Resolver {
public:
    using Ptr = std::shared_ptr<Resolver>;
    using AddrAndPath = std::pair<SymbolicExpression::Ptr, SymbolicExpression::Ptr>;
    using AddrsAndPaths = std::vector<AddrAndPath>;

public:
    Partitioner::Ptr partitioner;
    BasicBlock::Ptr bb;
    BS::RiscOperators::Ptr ops;

public:
    virtual ~Resolver() {}
    Resolver(const Partitioner::Ptr &partitioner, const BasicBlock::Ptr &bb, const BS::RiscOperators::Ptr &ops)
        : partitioner(notnull(partitioner)), bb(notnull(bb)), ops(notnull(ops)) {}

    // If the instruction pointer expression is indirect and can be resolved (even if only partially to another simpler indirect
    // expression) then do so and return the expressions and their path constraints. Otherwise return an empty value.
    virtual AddrsAndPaths operator()(const SymbolicExpression::Ptr &ip, const SymbolicExpression::Ptr &path,
                                     Sawyer::Message::Stream &debug) = 0;

    // Sorts pairs by the first element and removes duplicates
    static void makeUnique(AddrsAndPaths &pairs) {
        std::sort(pairs.begin(), pairs.end(), [](const AddrAndPath &a, const AddrAndPath &b) {
            return a.first->hash() < b.first->hash();
        });
        pairs.erase(std::unique(pairs.begin(), pairs.end(), [](const AddrAndPath &a, const AddrAndPath &b) {
            return a.first->hash() == b.first->hash();
        }), pairs.end());
    }

    // Returns true if the two arguments are equal. The arguments should be sorted already by `makeUnique`.
    static bool areEqual(const AddrsAndPaths &a, const AddrsAndPaths &b) {
        if (a.size() != b.size())
            return false;
        return std::equal(a.begin(), a.end(), b.begin(), [](const AddrAndPath &a, const AddrAndPath &b) {
            return a.first->hash() == b.first->hash();
        });
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Hash the dataflow graph
static uint64_t
hashGraph(const DfGraph &graph) {
    // Hash each vertex individually into the `hashedVertices` list
    std::vector<std::pair<DfGraph::ConstVertexIterator, uint64_t>> hashedVertices;
    hashedVertices.reserve(graph.nVertices());

    for (const auto &vertex: graph.vertices()) {
        // Hash this vertex
        Combinatorics::HasherSha256Builtin hasher;
        if (const auto bb = vertex.value().basicBlock()) {
            hasher.insert(bb->address());
            hasher.insert(bb->nInstructions());
        } else {
            hasher.insert(vertex.value().fakedCall());
        }
        hasher.insert(vertex.value().inlineId());

        // Hash the outgoing edges in order of the target addresses
        std::vector<DfGraph::ConstVertexIterator> targets;
        targets.reserve(vertex.nOutEdges());
        for (const auto &edge: vertex.outEdges())
            targets.push_back(edge.target());
        std::sort(targets.begin(), targets.end(), [](const DfGraph::ConstVertexIterator &a, const DfGraph::ConstVertexIterator &b) {
            return DfVertexKey(a->value()) < DfVertexKey(b->value());
        });
        for (const auto &target: targets) {
            if (const auto bb = target->value().basicBlock()) {
                hasher.insert(bb->address());
                hasher.insert(bb->nInstructions());
            } else {
                hasher.insert(target->value().fakedCall());
            }
            hasher.insert(target->value().inlineId());
        }

        // Save the results for this vertex because we'll need to process them in order by their address
        hashedVertices.push_back(std::make_pair(graph.findVertex(vertex.id()), hasher.toU64()));
    }

    // Combine all the vertex hashes in order by the vertex address
    std::sort(hashedVertices.begin(), hashedVertices.end(),
              [](const std::pair<DfGraph::ConstVertexIterator, uint64_t> &a,
                 const std::pair<DfGraph::ConstVertexIterator, uint64_t> &b) {
                  return DfVertexKey(a.first->value()) < DfVertexKey(b.first->value());
              });
    Combinatorics::HasherSha256Builtin hasher;
    for (const auto &pair: hashedVertices)
        hasher.insert(pair.second);

    return hasher.toU64();
}

// Print a dataflow graph in Graphviz format for debugging.
static void
toGraphviz(std::ostream &out, const DfGraph &graph, const Partitioner::ConstPtr &partitioner, const size_t endVertexId) {
    ASSERT_not_null(partitioner);

    const Color::HSV entryColor(0.33, 1.0, 0.9);        // light green
    const Color::HSV endColor(0.67, 1.0, 0.9);       // light blue

    // One subgraph per inline ID, and give them names.
    std::map<size_t, std::string> subgraphs;
    for (const auto &vertex: graph.vertices()) {
        auto &name = subgraphs[vertex.value().inlineId()];
        if (name.empty()) {
            name = "inline #" + boost::lexical_cast<std::string>(vertex.value().inlineId());
            if (const auto addr = vertex.value().address()) {
                for (Function::Ptr &function: partitioner->functionsOverlapping(*addr)) {
                    name += " " + function->printableName();
                    break;
                }
            } else {
                ASSERT_forbid(vertex.value().fakedCall().empty());
                name += vertex.value().fakedCall();
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
            if (dfVertex.value().inlineId() == subgraph.first) {
                out <<dfVertex.id() <<" [";
                if (dfVertex.id() == endVertexId) {
                    out <<" shape=box style=filled fillcolor=\"" <<endColor.toHtml() <<"\"";
                } else if (dfVertex.nInEdges() == 0) {
                    out <<" shape=box style=filled fillcolor=\"" <<entryColor.toHtml() <<"\"";
                }
                out <<" label=<<b>Vertex " <<dfVertex.id() <<"</b>";

                if (dfVertex.value().fakedCall().empty()) {
                    ASSERT_require(dfVertex.value().address());
                    BasicBlock::Ptr bb = partitioner->basicBlockExists(*dfVertex.value().address());
                    ASSERT_not_null(bb);
                    for (SgAsmInstruction *insn: bb->instructions())
                        out <<"<br align=\"left\"/>" <<GraphViz::htmlEscape(insn->toStringNoColor());
                } else {
                    out <<"<br align=\"left\"/>faked call to " <<GraphViz::htmlEscape(dfVertex.value().fakedCall());
                }
                out <<"<br align=\"left\"/>> shape=box fontname=Courier";
                out <<" ];\n";
            }
        }
        out <<"}\n";
    }

    for (const auto &edge: graph.edges()) {
        out <<edge.source()->id() <<" -> " <<edge.target()->id();
        const auto cfgEdge = edge.value() ? partitioner->cfg().findEdge(*edge.value()) : partitioner->cfg().edges().end();
        out <<" [ label=<";
        if (partitioner->cfg().isValidEdge(cfgEdge)) {
            switch (cfgEdge->value().type()) {
                case E_NORMAL:
                    out <<"cfg-" <<*edge.value();
                    break;
                case E_FUNCTION_CALL:
                    out <<"call";
                    break;
                case E_FUNCTION_XFER:
                    out <<"xfer";
                    break;
                case E_FUNCTION_RETURN:
                    out <<"ret";
                    break;
                case E_CALL_RETURN:
                    out <<"cret";
                    break;
                case E_USER_DEFINED:
                    out <<"user";
                    break;
            }
        } else {
            out <<"df-" <<edge.id();
        }
        out <<"> ];\n";
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

// Find call return vertex in the dataflow graph
static DfGraph::ConstVertexIterator
findCallRet(const DfGraph &graph, const ControlFlowGraph &cfg, const DfGraph::Vertex &dfVertex) {
    for (const auto &dfEdge: dfVertex.outEdges()) {
        const auto cfgEdge = cfg.findEdge(dfEdge.value().orElse(UNLIMITED));
        if (cfg.isValidEdge(cfgEdge) && cfgEdge->value().type() == E_CALL_RETURN)
            return dfEdge.target();
    }
    return graph.vertices().end();
}

// Find call return basic block in the control flow graph
static BasicBlock::Ptr
findCallRet(const ControlFlowGraph::Vertex &caller) {
    for (const auto &edge: caller.outEdges()) {
        if (edge.value().type() == E_CALL_RETURN)
            return edge.target()->value().bblock();
    }
    return {};
}

// Inline into the dataflow graph a function called via CFG edge. Return a list of any new dataflow vertices that have subsequent
// function calls that were not inlined yet.
static std::vector<DfInline>
inlineFunctionCall(DfGraph &graph, const Partitioner::ConstPtr &partitioner, const DfInline &call, const size_t calleeInlineId,
                   const DfInline::Method method) {
    ASSERT_not_null(partitioner);
    const ControlFlowGraph &cfg = partitioner->cfg();
    std::vector<DfInline> retval;                       // next level for possible inlining

#ifndef NDEBUG
    {
        ASSERT_require(graph.isValidVertex(call.dfCaller));
        ASSERT_require(call.dfCaller->value().inlineId() == call.callerInlineId);
        ASSERT_not_null(call.dfCaller->value().basicBlock());

        ASSERT_require(graph.isValidVertex(call.dfCallRet));
        ASSERT_require(call.dfCallRet->value().inlineId() == call.callerInlineId);
        ASSERT_not_null(call.dfCallRet->value().basicBlock());

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

    // Valid until we erase anything from the graph
    std::set<size_t> insertedVertexIds;

    // Insert the first basic block of the callee and create an edge from the caller to the callee.
    DfGraph::VertexIterator dfCallee = graph.vertices().end();
    if (BasicBlock::Ptr entryBb = call.cfgCall->target()->value().bblock()) {
        dfCallee = graph.insertVertex(DfVertex(entryBb, calleeInlineId));
        insertedVertexIds.insert(dfCallee->id());
        graph.insertEdge(call.dfCaller, dfCallee, call.cfgCall->id());
    } else {
        return retval;
    }
    ASSERT_require(graph.isValidVertex(dfCallee));

    bool calledFunctionReturns = false;
    Function::Ptr function = partitioner->functionExists(*dfCallee->value().address());
    if (method == DfInline::Method::FAKE || (function && boost::ends_with(function->name(), "@plt"))) {
        // Fake the function body by inlining only its first basic block and giving it a non-emtpy name.
        dfCallee->value().fakedCall(function ? function->printableName() : addrToString(dfCallee->value().address()));
        graph.insertEdge(dfCallee, call.dfCallRet, Sawyer::Nothing());
        calledFunctionReturns = true;

    } else {
        // Insert all the basic blocks for the callee
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
                    const auto cretBb = findCallRet(*t.edge()->source());
                    ASSERT_not_null(cretBb);
                    const auto callret = graph.insertVertexMaybe(DfVertex(cretBb, calleeInlineId));
                    insertedVertexIds.insert(callret->id());
                    ASSERT_require(graph.isValidVertex(callret));
                    retval.push_back(DfInline(caller, callret, t.edge(), calleeInlineId));
                    t.skipChildren();

                } else if (src && t.edge()->value().type() == E_FUNCTION_RETURN) {
                    // The called function is returning. Create an edge from the returning vertex to the caller's callret vertex.
                    const auto dfReturnFrom = graph.findVertexValue(DfVertex(src, calleeInlineId));
                    ASSERT_require(graph.isValidVertex(dfReturnFrom));
                    graph.insertEdge(dfReturnFrom, call.dfCallRet, t.edge()->id());
                    calledFunctionReturns = true;
                    t.skipChildren();

                } else if (src && tgt && t.edge()->value().type() != E_FUNCTION_CALL) {
                    // Normal intra-function edge to be inlined into the dataflow graph
                    const auto srcVert = graph.insertVertexMaybe(DfVertex(src, calleeInlineId));
                    const auto tgtVert = graph.insertVertexMaybe(DfVertex(tgt, calleeInlineId));
                    insertedVertexIds.insert(srcVert->id());
                    insertedVertexIds.insert(tgtVert->id());
                    graph.insertEdge(srcVert, tgtVert, t.edge()->id());

                } else {
                    t.skipChildren();
                }
            }
        }
    }


    // Inlining failed if the inlined function doesn't return. In that case, replace what we just inlined with only the first block
    // marked as a "fake" function, and replace the original call-return edge with an edge from the first block to the call-return
    // vertex.
    if (!calledFunctionReturns) {
        std::vector<DfGraph::VertexIterator> toErase;
        toErase.reserve(insertedVertexIds.size());
        for (const size_t vid: insertedVertexIds) {
            if (vid != dfCallee->id()) {                // don't erase the entry block of the callee; we need it below
                const auto vertex = graph.findVertex(vid);
                if (vertex->value().inlineId() == calleeInlineId)
                    toErase.push_back(graph.findVertex(vid));
            }
        }
        for (const auto &vertex: toErase)
            graph.eraseVertex(vertex);
        dfCallee->value().fakedCall(function ? function->printableName() : addrToString(dfCallee->value().address()));
        graph.insertEdge(dfCallee, call.dfCallRet, Sawyer::Nothing());
        calledFunctionReturns = true;
    }

    // Erase the old call-return edge(s) that goes from the call vertex to the vertex to which the call returns.
    ASSERT_require(calledFunctionReturns);
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

// Replace each call-return edge from vertex A to B with a function-call edge from A to a faked function (new vertex C) and a
// function-return edge from C to B.
static void
replaceCallReturnEdges(DfGraph &graph, const ControlFlowGraph &cfg) {
    // Find the cret edges and create a faked call for each
    std::vector<DfGraph::EdgeIterator> cretEdges;
    std::vector<DfGraph::VertexIterator> fakedFunctions;
    for (auto dfEdge = graph.edges().begin(); dfEdge != graph.edges().end(); ++dfEdge) {
        if (dfEdge->value()) {
            const auto cfgEdge = cfg.findEdge(*dfEdge->value());
            ASSERT_require(cfg.isValidEdge(cfgEdge));
            if (cfgEdge->value().type() == E_CALL_RETURN) {
                cretEdges.push_back(dfEdge);
                const std::string uniqueVertexName = "indeterminate #" + boost::lexical_cast<std::string>(cfgEdge->id());
                fakedFunctions.push_back(graph.insertVertex(DfVertex(uniqueVertexName, dfEdge->source()->value().inlineId())));
            }
        }
    }
    ASSERT_require(cretEdges.size() == fakedFunctions.size());

    // Insert new edges fcall and fret edges that replace the cret edges
    for (size_t i = 0; i < cretEdges.size(); ++i) {
        graph.insertEdge(cretEdges[i]->source(), fakedFunctions[i], Sawyer::Nothing());
        graph.insertEdge(fakedFunctions[i], cretEdges[i]->target(), Sawyer::Nothing());
        graph.eraseEdge(cretEdges[i]);
    }
}

// Given a CFG vertex that has an indeterminate edge (i.e., a possible indirect branch), construct a dataflow graph that looks
// backward from that CFG vertex a certain distance. Return the dataflow graph.
static DfGraph
buildDfGraph(const Settings &settings, const Partitioner::ConstPtr &partitioner, const ControlFlowGraph::Vertex &cfgVertex,
             const size_t maxDepth, const Debugging debugging) {
    using namespace Sawyer::Container::Algorithm;

    ASSERT_not_null(partitioner);
    DfGraph graph;

    // Insert the ending vertex and capture it's location and basic block for later. We cannot rely on it having ID zero because
    // we might erase vertices from the graph and ID numbers are not stable over erasing (but iterators are).
    BasicBlock::Ptr endBb = cfgVertex.value().bblock();
    DfGraph::VertexIterator endVertex;
    if (endBb) {
        endVertex = graph.insertVertex(DfVertex(endBb, 0));
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
            if (++depth <= maxDepth && src && tgt /*&& src != endBb*/ && t.edge()->value().type() != E_FUNCTION_CALL) {
                graph.insertEdgeWithVertices(DfVertex(src, 0), DfVertex(tgt, 0), t.edge()->id());
            } else {
                t.skipChildren();
            }
        } else if (t.event() == LEAVE_EDGE) {
            ASSERT_require(depth > 0);
            --depth;
        }
    }
    if (debugging == Debugging::ON) {
        std::string name = "icf-dfgraph-" + addrToString(endBb->address()).substr(2) + "-1-reverse.dot";
        std::ofstream f(name.c_str());
        toGraphviz(f, graph, partitioner, endVertex->id());
    }

    // Find vertices in the dataflow graph that call other functions.
    std::vector<DfInline> callsToInline;
    for (const auto &dfVertex: graph.vertices()) {
        if (const auto addr = dfVertex.value().address()) {
            const auto cfgVertex = partitioner->findPlaceholder(*addr);
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
    }

    // Repeatedly inline function calls to the specified depth. So far, all vertices have an inlineId of zero. Each time we inline
    // another function we'll use a new inlineId. This ensures that even if the same function is inlined more than once we can
    // uniquely identify each vertex in the dataflow graph by a combination of address and inlineId.
    size_t inlineId = 0;
    for (size_t i = 0; i < settings.maxInliningDepth && !callsToInline.empty(); ++i) {
        std::vector<DfInline> nextLevelCalls;
        for (const auto &call: callsToInline) {
            const auto next = inlineFunctionCall(graph, partitioner, call, ++inlineId, DfInline::Method::NORMAL);
            nextLevelCalls.insert(nextLevelCalls.end(), next.begin(), next.end());
        }
        callsToInline = nextLevelCalls;
        if (debugging == Debugging::ON) {
            std::string s = (boost::format("icf-dfgraph-%s-2-inline%03d.dot") % addrToString(endBb->address()).substr(2) % i).str();
            std::ofstream f(s.c_str());
            toGraphviz(f, graph, partitioner, endVertex->id());
        }
    }

    // If there are any calls remaining in the graph, replace them with fake function placeholders.
    for (const auto &call: callsToInline) {
        const auto next = inlineFunctionCall(graph, partitioner, call, ++inlineId, DfInline::Method::FAKE);
        ASSERT_always_require(next.empty());            // a fake function has no calls to any other functions
    }
    if (debugging == Debugging::ON) {
        std::string name = "icf-dfgraph-" + addrToString(endBb->address()).substr(2) + "-3-caps.dot";
        std::ofstream f(name.c_str());
        toGraphviz(f, graph, partitioner, endVertex->id());
    }

    // If there are any call-return edges remaining in the graph, replace them with fake function placeholders. This can happen when
    // the corresponding function-call edge was pruned away early, leaving only the cret edge. We need to add a fake function here
    // in order to update the state (e.g., for x86 we need to pop the return address that was pushed by the `call` insn.
    replaceCallReturnEdges(graph, partitioner->cfg());
    if (debugging == Debugging::ON) {
        std::string name = "icf-dfgraph-" + addrToString(endBb->address()).substr(2) + "-4-cret.dot";
        std::ofstream f(name.c_str());
        toGraphviz(f, graph, partitioner, endVertex->id());
    }

    // Remove parts of the graph that can't reach the target vertex (vertex #0)
    removeUnreachableReverse(graph, 0);
    if (debugging == Debugging::ON) {
        std::string name = "icf-dfgraph-" + addrToString(endBb->address()).substr(2) + "-5-prune.dot";
        std::ofstream f(name.c_str());
        toGraphviz(f, graph, partitioner, endVertex->id());
    }

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

// This analysis attempts to resolve the indirect control flow by discovering and using statically created jump tables, such as what
// a C compiler typically creates for `switch` statements.
//
// For each such block having an indeterminate outgoing CFG edge, a dataflow graph is created from nearby basic blocks. The dataflow
// analysis uses instruction semantics to update the machine state associated with each dataflow graph vertex. A special merge
// operation merges states when control flow merges, such as at the end of an `if` statement. The analysis examines the final
// outgoing state for the basic block in question to ascertain whether a static jump table was used, what its starting address might
// be, and how entries in the table are used to compute target addresses for the jump instruction.
//
// Besides containing the typical symbolic registers and memory, the state also contains constraints imposed by the execution path.
// These constraints are used to limit the size of the jump table. Ideally, even if the compiler emits two consecutive tables for
// two different `switch` statements, the basic block for each `switch` statement will have only successors that are relevant to
// that statement.
class JumpTableResolver: public Resolver {
protected:
    JumpTableResolver(const Partitioner::Ptr &partitioner, const BasicBlock::Ptr &bb, const BS::RiscOperators::Ptr &ops)
        : Resolver(partitioner, bb, ops) {}

public:
    static Ptr instance(const Partitioner::Ptr &partitioner, const BasicBlock::Ptr &bb, const BS::RiscOperators::Ptr &ops) {
        return Ptr(new JumpTableResolver(partitioner, bb, ops));
    }

    AddrsAndPaths operator()(const SymbolicExpression::Ptr &ip, const SymbolicExpression::Ptr &path,
                             Sawyer::Message::Stream &debug) override {
        BS::State::Ptr state = ops->currentState();
        ASSERT_not_null(state);

        // We only handle x86 for now (although this might work for other instruction sets too).
        if (as<const Architecture::X86>(partitioner->architecture())) {
            SAWYER_MESG(debug) <<"  possible jump table for " <<bb->printableName() <<"\n";

            const auto matched = match(ip);
            const auto jumpTableEntry = matched.first;
            const Address perEntryOffset = matched.second;
            if (!jumpTableEntry) {
                SAWYER_MESG(debug) <<"    jump table IP expression not matched (no jump table)\n";
                return {};
            }
            SAWYER_MESG(debug) <<"    value read from jump table: " <<*jumpTableEntry <<"\n";

            const auto jumpTableAddrExpr = findAddressContaining(ops, jumpTableEntry);
            if (!jumpTableAddrExpr) {
                SAWYER_MESG(debug) <<"    cannot find jump table entry in memory state\n";
                return {};
            }
            SAWYER_MESG(debug) <<"    address from which it was read: " <<*jumpTableAddrExpr <<"\n";

            const std::set<Address> constants = findInterestingConstants(jumpTableAddrExpr);
            for (const Address tableAddr: constants) {
                SAWYER_MESG(debug) <<"    potential table address " <<addrToString(tableAddr) <<"\n";

                bool isWritable = false;
                if (!isMappedAccess(partitioner->memoryMap(), tableAddr, MemoryMap::NO_ACCESS, MemoryMap::WRITABLE)) {
                    SAWYER_MESG(debug) <<"      potential table entry is writable\n";
                    isWritable = true;
                }
                SAWYER_MESG(debug) <<"      possible jump table at " <<addrToString(tableAddr) <<"\n"
                                   <<"      per-entry offset is " <<addrToString(perEntryOffset) <<"\n";

                // Configure the table scanner
                const auto tableLimits = AddressInterval::whole(); // will be refined
                const size_t bytesPerEntry = (jumpTableEntry->nBits() + 7) / 8;
                auto table = JumpTable::instance(partitioner, tableLimits, bytesPerEntry, perEntryOffset,
                                                 JumpTable::EntryType::ABSOLUTE);
                if (debug)
                    table->showingDebug(true);
                table->maxPreEntries(0);
                table->refineLocationLimits(bb, tableAddr);
                SAWYER_MESG(debug) <<"      table limited to " <<addrToString(table->tableLimits()) <<"\n";
                SAWYER_MESG(debug) <<"      targets limited to " <<addrToString(table->targetLimits()) <<"\n";

                // Scan the table
                SAWYER_MESG(debug) <<"      scanning table at " <<addrToString(tableAddr)
                                   <<" within " <<addrToString(table->tableLimits()) <<"\n";
                table->scan(partitioner, ops, tableAddr);
                if (!table->location()) {
                    SAWYER_MESG(debug) <<"      table not found at " <<addrToString(tableAddr) <<"\n";
                    continue;
                }
                SAWYER_MESG(debug) <<"      parsed jump table at " <<addrToString(tableAddr)
                                   <<" with " <<plural(table->nEntries(), "entries") <<"\n";

                // Refine what we scanned
                std::set<Address> successors = satisfiableTargets(table, path, jumpTableAddrExpr, debug);
                if (debug) {
                    debug <<"      unique targets remaining: " <<successors.size() <<"\n";
                    for (const Address target: successors)
                        debug <<"        target " <<addrToString(target) <<"\n";
                }

                // Associate the jump table with the basic block
                partitioner->detachBasicBlock(bb);
                table->attachTableToBasicBlock(bb);
                partitioner->attachBasicBlock(bb);

                // Return the successor information
                AddrsAndPaths retval;
                for (const Address successor: successors)
                    retval.push_back(std::make_pair(SymbolicExpression::makeIntegerConstant(ip->nBits(), successor), path));
                if (isWritable) {
                    const auto indet = SymbolicExpression::makeIntegerVariable(ip->nBits());
                    SAWYER_MESG(debug) <<"        target " <<*indet <<" (because table is not read-only)\n";
                    retval.push_back(std::make_pair(indet, path));
                }
                return retval;
            }
        }
        return {};
    }

private:
    // matches "(add[u64] (sext 0x40, e1[u32]) c1)" and returns e1 and c1 if matched. If not matched, returns (nullptr, 0).
    static std::pair<SymbolicExpression::Ptr, Address> match1(const SymbolicExpression::Ptr &add) {
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
    static std::pair<SymbolicExpression::Ptr, Address> match2(const SymbolicExpression::Ptr &expr) {
        if (!expr || !expr->isOperator(SymbolicExpression::OP_ADD) || expr->nBits() != 32 || !expr->isIntegerExpr())
            return std::make_pair(SymbolicExpression::Ptr(), 0);

        const auto e1 = expr->child(0);
        if (!e1 || e1->nBits() != 32 || !e1->isIntegerExpr())
            return std::make_pair(SymbolicExpression::Ptr(), 0);

        const auto c1 = expr->child(1);
        if (!c1 || c1->nBits() != 32 || !c1->toUnsigned())
            return std::make_pair(SymbolicExpression::Ptr(), 0);

        return std::make_pair(e1, *c1->toUnsigned());
    }

    // matches e1[32] and if so, returns (e1, 0)
    static std::pair<SymbolicExpression::Ptr, Address> match3(const SymbolicExpression::Ptr &expr) {
        return std::make_pair(expr, 0);
    }

    // Looks at an instruction pointer and tries to figure out some information about the jump table. It returns the symbolic value
    // read from the jump table, and a constant that needs to be added to each entry in the jump table in order to obtain a target
    // address.
    static std::pair<SymbolicExpression::Ptr, Address> match(const SymbolicExpression::Ptr &expr) {
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

            std::vector<SymbolicExpression::Ptr> searchSetMembers;
            if (searchByte->isOperator(OP_SET)) {
                for (const auto &child: searchByte->children())
                    searchSetMembers.push_back(child);
            }

            struct Visitor: public BS::MemoryCell::Visitor {
                SymbolicExpression::Ptr searchValue, searchByte;
                SymbolicExpression::Ptr foundAddr;
                const std::vector<SymbolicExpression::Ptr> &searchSetMembers;

                Visitor(const SymbolicExpression::Ptr &searchValue, const SymbolicExpression::Ptr &searchByte,
                        const std::vector<SymbolicExpression::Ptr> &searchSetMembers)
                    : searchValue(notnull(searchValue)), searchByte(notnull(searchByte)), searchSetMembers(searchSetMembers) {}

                void operator()(BS::MemoryCell::Ptr &cell) {
                    auto cellValue = SymbolicSemantics::SValue::promote(cell->value())->get_expression();

                    // Look for the byte expression directly.
                    if (cellValue->isEquivalentTo(searchByte)) {
                        foundAddr = SymbolicSemantics::SValue::promote(cell->address())->get_expression();
                        return;
                    }

                    // Look through an "extract" expression if the byte matching above failed. For instance, the needle might be
                    // (extract[u8] 0[u32], 8[u32], expr[u64]) but the value in memory might be (extract[u8] 0[u64], 8[u64],
                    // expr[u64]) -- a difference only in the size of the types used to extract the byte. Since memory cell values
                    // are almost always extract expressions, we can also try looking "through" the extract to the value being
                    // extracted.
                    if (cellValue->isOperator(OP_EXTRACT) &&
                        cellValue->child(0)->toUnsigned().orElse(1) == 0 &&
                        cellValue->child(1)->toUnsigned().orElse(1) == 8 &&
                        cellValue->child(2)->isEquivalentTo(searchValue)) {
                        foundAddr = SymbolicSemantics::SValue::promote(cell->address())->get_expression();
                        return;
                    }

                    // If the memory read was ambiguous then the thing we're looking for might be a set of values. In that case the
                    // needle doesn't actually appear directly in memory due to the wildcard variables produced by the code that
                    // tries to resolve memory aliasing.
                    if (cellValue->isOperator(OP_SET)) {
                        for (const auto &needle: searchSetMembers) {
                            for (const auto &haystack: cellValue->children()) {
                                if (haystack->isEquivalentTo(needle)) {
                                    foundAddr = SymbolicSemantics::SValue::promote(cell->address())->get_expression();
                                    return;
                                }
                            }
                        }
                    }
                }
            } visitor(searchValue, searchByte, searchSetMembers);

            mem->traverse(visitor);
            return visitor.foundAddr;
        }

        return {};
    }

    // Return the set of interesting constants found in an expression. Constants are interesting if:
    //
    //   1. The expression itself is a constant
    //   2. A constant that is an operand of an `add` operation.
    //
    // The list may change in the future.
    static std::set<Address> findInterestingConstants(const SymbolicExpression::Ptr &expr) {
        using namespace SymbolicExpression;

        if (const auto addr = expr->toUnsigned())
            return {*addr};

        struct: Visitor {
            std::set<const Node*> seen;                 // avoid re-processing common subexpressions
            std::set<Address> found;

            VisitAction preVisit(const Node *expr) {
                if (seen.insert(expr).second) {
                    if (expr->isOperator(OP_ADD)) {
                        for (const SymbolicExpression::Ptr &child: expr->children()) {
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
    static bool isMappedAccess(const MemoryMap::Ptr &map, const Address addr, const unsigned required, const unsigned prohibited) {
        ASSERT_not_null(map);
        return !map->at(addr).require(required).prohibit(prohibited).segments().empty();
    }

    // Return jump table target addresses for table entries whose addresses are satisfiable given the path constraints. The
    // `entryAddrExpr` is the symbolic address that was read by the basic block and represents any table entry.
    std::set<Address> satisfiableTargets(const JumpTable::Ptr &table, const SymbolicExpression::Ptr &pathConstraint,
                                         const SymbolicExpression::Ptr &entryAddrExpr, Sawyer::Message::Stream &debug) {
        using namespace SymbolicExpression;
        ASSERT_not_null(table);
        ASSERT_not_null(pathConstraint);

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

            {
                SmtSolver::Transaction tx(solver);
                solver->insert(entryConstraint);
                if (debug) {
                    for (const auto &assertion: solver->assertions())
                        debug <<"      constraint: " <<*assertion <<"\n";
                }
                const auto isSatisfiable = solver->check();

                switch (isSatisfiable) {
                    case SmtSolver::SAT_UNKNOWN:
                        SAWYER_MESG(debug) <<"      SMT solver failed (assuming satisfiable)\n";
                        // fall through
                    case SmtSolver::SAT_YES: {
                        const Address target = table->targets()[i];
                        if (debug) {
                            debug <<"      satisfiable table entry address, when:\n";
                            const auto evidence = solver->evidence();
                            for (const auto &pair: evidence.nodes())
                                debug <<"        " <<*pair.key() <<" = " <<*pair.value() <<"\n";
                        }
                        retval.insert(target);
                        break;
                    }
                    case SmtSolver::SAT_NO:
                        SAWYER_MESG(debug) <<"      entry address is not satisfiable\n";
                        break;
                }
            }
        }
        return retval;
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
//// ICF Recovery Strategy: final state has an instruction pointer of the form (ite COND A1 A2) where A1 and/or A2 are concrete
//// addresses.
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class IteResolver: public Resolver {
protected:
    IteResolver(const Partitioner::Ptr &partitioner, const BasicBlock::Ptr &bb, const BS::RiscOperators::Ptr &ops)
        : Resolver(partitioner, bb, ops) {}

public:
    static Ptr instance(const Partitioner::Ptr &partitioner, const BasicBlock::Ptr &bb, const BS::RiscOperators::Ptr &ops) {
        return Ptr(new IteResolver(partitioner, bb, ops));
    }

    AddrsAndPaths operator()(const SymbolicExpression::Ptr &ip, const SymbolicExpression::Ptr &path,
                             Sawyer::Message::Stream &debug) override {
        if (!ip->isOperator(SymbolicExpression::OP_ITE))
            return {};

        SAWYER_MESG(debug) <<"  IP has two possible values with complementary path constraints:\n"
                           <<"    path constraint = " <<*path <<"\n"
                           <<"    ip = " <<*ip <<"\n"
                           <<"    expr[0] = " <<*ip->child(1) <<"\n"
                           <<"    expr[1] = " <<*ip->child(2) <<"\n";

        AddrsAndPaths retval;
        retval.push_back(std::make_pair(ip->child(1), path));
        retval.push_back(std::make_pair(ip->child(2), SymbolicExpression::makeInvert(path)));
        return retval;
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
//// ICF Recovery Strategy: final state has an instruction pointer that's the concatenation sets of bytes, some of which are
//// concrete.
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ValueSetResolver: public Resolver {
protected:
    ValueSetResolver(const Partitioner::Ptr &partitioner, const BasicBlock::Ptr &bb, const BS::RiscOperators::Ptr &ops)
        : Resolver(partitioner, bb, ops) {}

public:
    static Ptr instance(const Partitioner::Ptr &partitioner, const BasicBlock::Ptr &bb, const BS::RiscOperators::Ptr &ops) {
        return Ptr(new ValueSetResolver(partitioner, bb, ops));
    }

    AddrsAndPaths operator()(const SymbolicExpression::Ptr &ip, const SymbolicExpression::Ptr &path,
                             Sawyer::Message::Stream &debug) override {

        // A set of expressions expands to those expressions.
        if (ip->isOperator(SymbolicExpression::OP_SET)) {
            AddrsAndPaths retval;
            SAWYER_MESG(debug) <<"  IP is a set with " <<plural(ip->nChildren(), "member expressions") <<"\n"
                               <<"    path constraint = " <<*path <<"\n"
                               <<"    ip = " <<*ip <<"\n";
            for (const auto &expr: ip->children()) {
                SAWYER_MESG(debug) <<"    expr[" <<retval.size() <<"] = " <<*expr <<"\n";
                retval.push_back(std::make_pair(expr, path));
            }
            return retval;

        }

        // A concatenation of sets is treated in the inverse: a set of concatenations. This might not fully work since sets are
        // generally not ordered. I.e., an expression like (concat (set A B) (set C D)) has four possible values {AC, AD, BC, BD},
        // but we treat this as having only two values {AC, BD}.
        if (ip->isOperator(SymbolicExpression::OP_CONCAT) || ip->nChildren() == 0) {
            // Not all parts (extractions) of the IP need to be sets, but if there are sets they must all be the same size.
            size_t nMembers = 0;
            for (const auto &set: ip->children()) {
                if (set->isOperator(SymbolicExpression::OP_SET)) {
                    ASSERT_require(set->nChildren() > 0);
                    if (nMembers > 0) {
                        if (set->nChildren() != nMembers)
                            return {};
                    } else {
                        nMembers = set->nChildren();
                    }
                }
            }
            if (nMembers == 0)
                return {};
            SAWYER_MESG(debug) <<"  IP is a set with " <<plural(nMembers, "member expressions") <<"\n"
                               <<"    path constraint = " <<*path <<"\n"
                               <<"    ip = " <<*ip <<"\n";

            // Build addresses from the sets of values
            AddrsAndPaths retval;
            for (size_t i = 0; i < nMembers; ++i) {
                SymbolicExpression::Ptr addr;
                for (const auto &part: ip->children()) {
                    if (part->isOperator(SymbolicExpression::OP_SET)) {
                        addr = addr ? makeConcat(addr, part->child(i)) : part->child(i);
                    } else {
                        addr = addr ? makeConcat(addr, part) : part;
                    }
                }
                SAWYER_MESG(debug) <<"    expr[" <<i <<"] = " <<*addr <<"\n";
                retval.push_back(std::make_pair(addr, path));
            }
            return retval;
        }

        return {};
    }
};

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
    const Debugging debugging = enableDiagnostics(debug, settings, bb);
    SAWYER_MESG(debug) <<"analyzing " <<bb->printableName() <<"\n";

    // Build the dataflow graph
    const ControlFlowGraph::VertexIterator cfgVertex = partitioner->findPlaceholder(bb->address());
    ASSERT_require(partitioner->cfg().isValidVertex(cfgVertex));
    DfGraph dfGraph = buildDfGraph(settings, partitioner, *cfgVertex, settings.maxReversePathLength, debugging);
    if (dfGraph.isEmpty()) {
        SAWYER_MESG(debug) <<"  dataflow graph is empty\n";
        return false;
    }
    const uint64_t hash = hashGraph(dfGraph);
    if (!partitioner->icf().hashedGraphs.insert(hash).second) {
        SAWYER_MESG(debug) <<"  previously analyzed this dataflow graph\n";
        return false;
    }
    const auto endVertex = dfGraph.findVertexValue(DfVertex(bb, 0));
    ASSERT_require(dfGraph.isValidVertex(endVertex));
    if (debug) {
        printGraph(debug, dfGraph);
        if (!dfGraph.isEmpty()) {
            const std::string fname = "icf-dfgraph-" + addrToString(bb->address()).substr(2) +
                                      "-H" + addrToString(hash).substr(2) + ".dot";
            std::ofstream file(fname.c_str());
            toGraphviz(file, dfGraph, partitioner, endVertex->id());
            debug <<"  also written to " <<fname <<"\n";
        }
    }
    if (dfGraph.nVertices() > settings.maxDataflowVertices) {
        SAWYER_MESG(debug) <<"  dataflow graph is too big; "
                           <<plural(dfGraph.nVertices(), "vertices")
                           <<" exceeds the limit of " <<settings.maxDataflowVertices <<"\n";
        return false;
    }

    // Configure the dataflow engine
    BS::RiscOperators::Ptr ops = partitioner->newOperators(MAP_BASED_MEMORY);
    BS::Dispatcher::Ptr cpu = partitioner->newDispatcher(ops);
    if (!cpu)
        return false;
    SymbolicSemantics::RiscOperators::promote(ops)->trimThreshold(settings.maxSymbolicExprSize);

    DfTransfer xfer(settings, partitioner, cpu, debugging);
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
    SAWYER_MESG(debug) <<"  performing dataflow analysis...\n";
    try {
        // Temporarily turn on the debug stream for the dataflow analysis
        struct R {
            const bool wasEnabled;
            R(const Debugging debugging)
                : wasEnabled(BinaryAnalysis::DataFlow::mlog[DEBUG].enabled()) {
                if (debugging == Debugging::ON)
                    BinaryAnalysis::DataFlow::mlog[DEBUG].enable();
            }
            ~R() {
                BinaryAnalysis::DataFlow::mlog[DEBUG].enable(wasEnabled);
            }
        } r(debugging);

        // Probably won't reach a fixed point due to maxIterations set above, in which case it will throw `NotConverging`
        dfEngine.runToFixedPoint();
    } catch (const BinaryAnalysis::DataFlow::NotConverging&) {
        SAWYER_MESG(debug) <<"  dataflow did not converge\n";
    }
    if (const size_t nTrimmed = SymbolicSemantics::RiscOperators::promote(ops)->nTrimmed())
        SAWYER_MESG(debug) <<"  dataflow caused " <<plural(nTrimmed, "symbolic expressions") <<" to be trimmed\n";

    // Examine the outgoing state for the basic block in question.
    if (BS::State::Ptr state = dfEngine.getFinalState(endVertex->id())) {
        if (debug) {
            debug <<"  final dataflow state after " <<bb->printableName() <<":\n";
            state->print(debug, "    ");
        }

        // Get information about the final state
        ops->currentState(state);
        const RegisterDescriptor IP = partitioner->architecture()->registerDictionary()->instructionPointerRegister();
        const SymbolicExpression::Ptr ip = SymbolicSemantics::SValue::promote(ops->readRegister(IP))->get_expression();
        SAWYER_MESG(debug) <<"  IP = " <<*ip <<"\n";
        const RegisterDescriptor PATH = createPathRegister(partitioner);
        const SymbolicExpression::Ptr path = SymbolicSemantics::SValue::promote(ops->readRegister(PATH, ops->boolean_(true)))
                                             ->get_expression();
        SAWYER_MESG(debug) <<"  path constraint = " <<*path <<"\n";

        // Create the functions we'll use to recursively resolve the instruction pointer.
        std::vector<Resolver::Ptr> resolvers;
        resolvers.push_back(IteResolver::instance(partitioner, bb, ops));
        resolvers.push_back(ValueSetResolver::instance(partitioner, bb, ops));
        resolvers.push_back(JumpTableResolver::instance(partitioner, bb, ops));

        // Call each handler to give it a chance to examine the instruction pointer expression and refine it, returning zero or more
        // refined values. Do this repeatedly until nothing changes. For instance, if the IP is the expression '(ite COND EXPR1
        // EXPR2)' then this might return EXPR1 and EXPR2 and the process is repeated on both of those values. If a value is fully
        // resolved (i.e., concrete) then we're done with that value.
        Resolver::AddrsAndPaths successors;
        successors.push_back(std::make_pair(ip, path)); // start things off with the instruction pointer from the semantic state
        for (size_t i = 0; true; ++i) {
            SAWYER_MESG(debug) <<"  === resolver pass " <<i <<" ===\n";
            bool changed = false;
            for (const auto &resolver: resolvers) {
                Resolver::AddrsAndPaths next;           // the next set of IP expressions over which to iterate.
                for (const auto &addrAndPath: successors) {
                    if (addrAndPath.first->toUnsigned()) {
                        next.push_back(addrAndPath);    // already resolved
                    } else {
                        const Resolver::AddrsAndPaths result = (*resolver)(addrAndPath.first, addrAndPath.second, debug);
                        if (!result.empty()) {
                            next.insert(next.end(), result.begin(), result.end());
                        } else {
                            next.push_back(addrAndPath);
                        }
                    }
                }

                Resolver::makeUnique(next);
                if (!Resolver::areEqual(next, successors)) {
                    successors = next;
                    changed = true;
                    break;
                }
            }
            if (!changed)
                break;
            if (i > 5 /*arbitrary*/) {
                SAWYER_MESG(mlog[WARN]) <<"possible infinite loop avoided in ICF analyzeBasicBlock\n";
                break;
            }
        }

        // Update the basic block successors. The new successors replace the original indeterminate successor and have the same
        // edge type.
        ASSERT_forbid(successors.empty());              // we at least need the original IP
        if (successors.size() > 1 || !successors[0].first->isEquivalentTo(ip) || successors[0].first->toUnsigned()) {
            partitioner->detachBasicBlock(bb);

            // Remove the indeterminate edges
            EdgeType edgeType = E_NORMAL;
            if (bb->successors().isCached()) {
                for (const auto &successor: bb->successors().get()) {
                    if (!successor.expr()->get_expression()->toUnsigned()) {
                        edgeType = successor.type();
                        bb->eraseSuccessor(successor);
                        break;
                    }
                }
            }

            // Insert the new successors
            for (const auto &successor: successors) {
                auto addr = SymbolicSemantics::SValue::promote(ops->undefined_(successor.first->nBits()));
                addr->set_expression(successor.first);
                bb->insertSuccessor(addr, edgeType);
            }

            partitioner->attachBasicBlock(bb);
            return true;
        }
    } else {
        SAWYER_MESG(debug) <<"  dataflow failed to reach " <<bb->printableName() <<"\n";
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
    if (!settings.enabled)
        return false;

    bool madeChanges = false;
    for (const auto &cfgVertex: partitioner->cfg().vertices()) {
        if (BasicBlock::Ptr bb = shouldAnalyze(cfgVertex)) {
            bb->hasIndirectControlFlow(true);
            if (analyzeBasicBlock(settings, partitioner, bb))
                madeChanges = true;
        }
    }
    return madeChanges;
}

bool
reanalyzeSomeBlocks(const Settings &settings, const Partitioner::Ptr &partitioner) {
    ASSERT_not_null(partitioner);
    bool madeChanges = false;
    if (settings.enabled && settings.reanalyzeSomeBlocks) {
        for (const auto &cfgVertex: partitioner->cfg().vertices()) {
            if (BasicBlock::Ptr bb = cfgVertex.value().bblock()) {
                if (bb->hasIndirectControlFlow()) {
                    if (analyzeBasicBlock(settings, partitioner, bb))
                        madeChanges = true;
                }
            }
        }
    }
    return madeChanges;
}

} // namespace
} // namespace
} // namespace
} // namespace

#endif
