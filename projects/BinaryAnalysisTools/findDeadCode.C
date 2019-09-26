#include <rose.h>
#include <rosePublicConfig.h>

#include <Partitioner2/DataFlow.h>
#include <Partitioner2/Engine.h>
#include <Sawyer/Assert.h>
#include <Sawyer/CommandLine.h>
#include <Sawyer/DistinctList.h>
#include <Sawyer/GraphTraversal.h>
#include <Sawyer/ProgressBar.h>
#include <Sawyer/Stopwatch.h>

using namespace Rose;
using namespace Rose::BinaryAnalysis;
using namespace Rose::Diagnostics;

namespace P2 = Partitioner2;

static Sawyer::Message::Facility mlog;

typedef Sawyer::Container::GraphIteratorSet<P2::ControlFlowGraph::ConstVertexIterator> VertexWorkList;
typedef Sawyer::Container::DistinctList<P2::Function::Ptr> FunctionWorkList;

// Convenient struct to hold settings specific to this tool. Settings related to disassembling are in the engine.
struct Settings {
    std::vector<AddressInterval> reachableVas;          // addresses declared to be reachable
    bool useContainer;                                  // assume entry points, exports, etc. are reachable?
    bool lookForImmediates;                             // find reachable code using instruction immediates?
    bool useDataFlow;                                   // use data flow analysis to find concrete values?
    std::vector<AddressInterval> readableVas;           // where to search for instruction addresses in data
    size_t addressAlignment;                            // alignment for reading addresses from data areas
    size_t addressSize;                                 // size of an address in bytes, or zero to auto-detect
    bool showMap;                                       // show the memory map?
    bool showAddresses;                                 // show unreached areas as address intervals?
    bool showReachableAddresses;                        // show reachable areas as address intervals?
    bool showInstructions;                              // show unreached disassembled instructions?
    bool showDisassembly;                               // disassemble unreached addresses linearly?
    bool showHexDump;                                   // show unreached areas as data hexdumps?
    bool useCompression;                                // compress sequences of identical instructions?
    bool showStats;                                     // show some final statistics
    Settings()
        : useContainer(true), lookForImmediates(true), useDataFlow(true), addressAlignment(1), addressSize(0),
          showMap(false), showAddresses(false), showReachableAddresses(false), showInstructions(false),
          showDisassembly(false), showHexDump(false), useCompression(true), showStats(true) {}
};

// Describe and parse the command-line
static std::vector<std::string>
parseCommandLine(int argc, char *argv[], P2::Engine &engine, Settings &settings)
{
    using namespace Sawyer::CommandLine;

    std::string purpose = "searches for unexecuted code areas";
    std::string description =
        "Parses and disassembles a binary specimen, then traverses the control flow graph to identify regions "
        "of code that cannot be reached according to a static analysis. Optionally augments the CFG traversal "
        "with other heuristics that identify reachable vs. unreachable code.";
    Parser parser = engine.commandLineParser(purpose, description);
    parser.errorStream(::mlog[FATAL]);

    SwitchGroup analysis("Dead code analysis switches");
    analysis.name("dc");

    analysis.insert(Switch("reachable-address")
                    .argument("addresses", P2::addressIntervalParser(settings.reachableVas))
                    .whichValue(SAVE_ALL)
                    .doc("Addresses that are declared to be reachable. This switch can occur more than once in order to "
                         "specify non-contiguous addresses, each time specifying one contiguous interval. " +
                         P2::AddressIntervalParser::docString()));

    analysis.insert(Switch("use-container")
                    .intrinsicValue(true, settings.useContainer)
                    .doc("When this switch is present, the analysis assumes that entry pointers and exported functions "
                         "are always reachable.  The @s{no-use-container} does not make this assumption.  The default is to " +
                         std::string(settings.useContainer?"":"not ") + "assume such functions are reachable."));
    analysis.insert(Switch("no-use-container")
                    .key("use-container")
                    .intrinsicValue(false, settings.useContainer)
                    .hidden(true));

    analysis.insert(Switch("immediates")
                    .intrinsicValue(true, settings.lookForImmediates)
                    .doc("This switch causes the analysis to scan reachable instructions and look for immediate constants "
                         "stored in those instructions.  If the immediate constant seems to be a valid instruction address "
                         "then that vertex is added to the list of reachable blocks, along with all vertices recurisvely "
                         "reachable from that vertex. Newly added vertices are also searched for immediate values.  The "
                         "@s{no-immediates} switch disables this feature. The default is to " +
                         std::string(settings.lookForImmediates?"":"not ") + "perform this analysis."));
    analysis.insert(Switch("no-immediates")
                    .key("immediates")
                    .intrinsicValue(false, settings.lookForImmediates)
                    .hidden(true));

    analysis.insert(Switch("data-flow")
                    .intrinsicValue(true, settings.useDataFlow)
                    .doc("Use a data-flow analysis to find concrete values. The analysis is performed one function at a time "
                         "and the entire data-flow steady state is searched to find concrete values that might be code "
                         "addresses. The data flow is computed for a function if the function's entry point is reachable. "
                         "The @s{no-data-flow} switch disables this. The default is to " +
                         std::string(settings.useDataFlow?"":"not ") + "perform this analysis."));
    analysis.insert(Switch("no-data-flow")
                    .key("data-flow")
                    .intrinsicValue(false, settings.useDataFlow)
                    .hidden(true));

    analysis.insert(Switch("read-from")
                    .argument("addresses", P2::addressIntervalParser(settings.readableVas))
                    .whichValue(SAVE_ALL)
                    .doc("Addresses that are read in order to find constants that are the addresses of basic blocks. "
                         "Such basic blocks are assumed to be reachable. This switch can occur more than once in order to "
                         "specify non-contiguous addresses, each time specifying one contiguous interval. " +
                         P2::AddressIntervalParser::docString()));

    analysis.insert(Switch("alignment")
                    .argument("bytes", nonNegativeIntegerParser(settings.addressAlignment))
                    .doc("Alignment to use when reading addresses.  If the alignment is smaller than the address size "
                         "then addresses are read from overlapping areas of memory.  The default alignment is " +
                         StringUtility::numberToString(settings.addressAlignment) + "."));

    analysis.insert(Switch("address-size")
                    .argument("nbytes", nonNegativeIntegerParser(settings.addressSize))
                    .doc("Size of an address in bytes.  This is used when searching for addresses in memory.  A value of "
                         "zero (the default) will assume addresses are the same as the architecture's word size."));

    SwitchGroup output("Output switches");
    output.name("out");

    output.insert(Switch("show-map")
                  .intrinsicValue(true, settings.showMap)
                  .doc("Output the memory map that is used for the dead code analysis. Sections that are marked as having "
                       "execute permission are those sections that will be reported in results, although the non-executable "
                       "sections may participate in the actual analysis. The @s{no-show-map} disables this output. The default "
                       " is to " + std::string(settings.showMap?"":"not ") + " show the memory map."));
    output.insert(Switch("no-show-map")
                  .key("show-map")
                  .intrinsicValue(false, settings.showMap)
                  .hidden(true));

    output.insert(Switch("show-addresses")
                  .intrinsicValue(true, settings.showAddresses)
                  .doc("Causes a list of reached executable addresses to be displayed. Each line of output contains three "
                       "values, \"@v{begin} + @v{size} = @v{end}\", that describe a largest contiguous interval of "
                       "unreached addresses.  The @s{no-show-addresses} switch turns this off. The default is to " +
                       std::string(settings.showAddresses?"":"not ") + "show this output."));
    output.insert(Switch("no-show-addresses")
                  .key("show-addresses")
                  .intrinsicValue(false, settings.showAddresses)
                  .hidden(true));

    output.insert(Switch("show-reachable-addresses")
                  .intrinsicValue(true, settings.showReachableAddresses)
                  .doc("List those instruction addresses that are determined to be reachable.  These addresses are "
                       "used to compute the unreachable lists (i.e., the analysis finds reachable addresses and "
                       "complements that set, intersecting with the set of executable addresses, to obtain unreachable "
                       "addresses).  The @s{no-show-reachable-addresses} switch disables showing reachable addresses. The "
                       "default is to " + std::string(settings.showReachableAddresses?"":"not ") + "show these addresses."));
    output.insert(Switch("no-show-reachable-addresses")
                  .key("show-reachable-addresses")
                  .intrinsicValue(false, settings.showReachableAddresses)
                  .hidden(true));
    
    output.insert(Switch("show-instructions")
                  .intrinsicValue(true, settings.showInstructions)
                  .doc("Causes a list of unreached instructions to be displayed. These are instructions that have been "
                       "disassembled by ROSE and which appear in the control flow graph but are unreachable according to "
                       "the dead code analysis run by this tool.  Since ROSE's disassembler uses a reachability analysis, "
                       "many areas of dead code detected by this tool might correspond to areas detected by the disassembler "
                       "as not reachable and thus no instructions will exist.  See also @s{show-disassembly}. The "
                       "default is to " + std::string(settings.showInstructions?"":"not ") + "show this output."));
    output.insert(Switch("no-show-instructions")
                  .key("show-instructions")
                  .intrinsicValue(false, settings.showInstructions)
                  .hidden(true));

    output.insert(Switch("show-disassembly")
                  .intrinsicValue(true, settings.showDisassembly)
                  .doc("Perform a linear disassembly of all unreached areas.  This is similar to the @s{show-instructions} "
                       "switch in that it shows instructions, but unlike that other switch, this one disassembles each "
                       "interval of unreachable addresses using a linear method: starting at the beginning of the interval, "
                       "disassemble adjacent instructions with no regard for the flow of control. This is how most simple "
                       "disassembler tools operate (e.g., objdump).  The @s{no-show-disassembly} switch turns this off. The "
                       "default is to " + std::string(settings.showDisassembly?"":"not ") + "show this output."));
    output.insert(Switch("no-show-disassembly")
                  .key("show-disassembly")
                  .intrinsicValue(false, settings.showDisassembly)
                  .hidden(true));

    output.insert(Switch("compress")
                  .intrinsicValue(true, settings.useCompression)
                  .doc("When performing a linear disassembly, such as with the @s{show-disassembly} switch, suppress repeated "
                       "instructions by replacing them with an indication of how many were repeated.  This is useful for long "
                       "sequences of padding.  The @s{no-compress} switch disables compression. The default is to " +
                       std::string(settings.useCompression?"":"not ") + "use compression."));
    output.insert(Switch("no-compress")
                  .key("compress")
                  .intrinsicValue(false, settings.useCompression)
                  .hidden(true));

    output.insert(Switch("show-hexdump")
                  .intrinsicValue(true, settings.showHexDump)
                  .doc("Show unused addresses in @man{hexdump}{1} style. The @s{no-show-hexdump} disables this feature. The "
                       "default is to " + std::string(settings.showHexDump?"":"not ") + "show this output."));
    output.insert(Switch("no-show-hexdump")
                  .key("show-hexdump")
                  .intrinsicValue(false, settings.showHexDump)
                  .hidden(true));

    output.insert(Switch("show-stats")
                  .intrinsicValue(true, settings.showStats)
                  .doc("Show some general statistics about the detection.  The @s{no-show-stats} disables this feature. The "
                       "default is to " + std::string(settings.showStats?"":"not ") + "show this output."));
    output.insert(Switch("no-show-stats")
                  .key("show-stats")
                  .intrinsicValue(false, settings.showStats)
                  .hidden(true));

    return parser.with(analysis).with(output).parse(argc, argv).apply().unreachedArgs();
}

// Returns true if the specified CFG vertex is reachable. Specifically, a vertex is reachable if its starting address or the
// starting address of any of its instructions appears in the specified reachable address set.  Special vertices are generally
// considered unreachable since they don't have addresses (e.g., an UNDISCOVERED vertex is the target for all vertices whose
// basic blocks have not been discovered yet, and doesn't have its own address).
static bool
isReachable(const AddressIntervalSet &reachable, const P2::Partitioner &partitioner,
            const P2::ControlFlowGraph::Vertex &vertex) {
    switch (vertex.value().type()) {
        case P2::V_INDETERMINATE:
        case P2::V_UNDISCOVERED:
        case P2::V_NONEXISTING:
            return false;

        case P2::V_USER_DEFINED:
            return reachable.contains(vertex.value().address());

        case P2::V_BASIC_BLOCK:
            BOOST_FOREACH (SgAsmInstruction *insn, vertex.value().bblock()->instructions()) {
                if (reachable.contains(insn->get_address()))
                    return true;
            }
            break;
    }
    return false;
}

// Return a list of vertices that are marked as reachable according to isReachable.
static VertexWorkList
reachableVertices(const AddressIntervalSet &reachable, const P2::Partitioner &partitioner) {
    VertexWorkList retval;
    BOOST_FOREACH (const P2::ControlFlowGraph::Vertex &vertex, partitioner.cfg().vertices()) {
        if (isReachable(reachable, partitioner, vertex))
            retval.insert(partitioner.cfg().findVertex(vertex.id()));
    }
    return retval;
}

// Starting at the specified vertex, mark that vertex readable and recursively mark all CFG successor vertices reachable. The
// traversal will stop when it reaches a vertex that's already fully marked as reachable (i.e., it assumes it's being called
// incrementally).
static void
insertReachableRecursively(AddressIntervalSet &reachable /*in,out*/, const P2::Partitioner &partitioner,
                           const P2::ControlFlowGraph::ConstVertexIterator &start) {
    typedef Sawyer::Container::Algorithm::DepthFirstForwardVertexTraversal<const P2::ControlFlowGraph> Traversal;
    for (Traversal t(partitioner.cfg(), start); t; ++t) {
        AddressIntervalSet vertexAddresses = t->value().addresses();
        if (reachable.contains(vertexAddresses)) {
            t.skipChildren();                           // includes vertexAddresses.isEmpty case
        } else {
            reachable |= vertexAddresses;
        }
    }
}

// Looks for CFG vertices that are reachable and recursively causes their successors to also be reachable.
static void
insertReachableRecursively(AddressIntervalSet &reachable /*in,out*/, const P2::Partitioner &partitioner) {
    const P2::ControlFlowGraph &cfg = partitioner.cfg();
    std::vector<bool> inserted(cfg.nVertices(), false);
    for (size_t i=0; i<cfg.nVertices(); ++i) {
        if (!inserted[i] && isReachable(reachable, partitioner, *cfg.findVertex(i))) {
            typedef Sawyer::Container::Algorithm::DepthFirstForwardVertexTraversal<const P2::ControlFlowGraph> Traversal;
            for (Traversal t(cfg, cfg.findVertex(i)); t; ++t) {
                if (inserted[t->id()]) {
                    t.skipChildren();                   // already added in a previous traversal
                } else {
                    reachable |= t->value().addresses();
                    inserted[t->id()] = true;
                }
            }
        }
    }
}

// Returns all immediate values found in instructions.
static std::set<rose_addr_t>
findImmediateValues(const P2::ControlFlowGraph::ConstVertexIterator &vertex) {
    struct T1: AstSimpleProcessing {
        std::set<rose_addr_t> values;
        void visit(SgNode *node) {
            if (SgAsmIntegerValueExpression *ive = isSgAsmIntegerValueExpression(node))
                values.insert(ive->get_absoluteValue());
        }
    } t1;

    if (vertex->value().type() == P2::V_BASIC_BLOCK) {
        BOOST_FOREACH (SgAsmInstruction *insn, vertex->value().bblock()->instructions())
            t1.traverse(insn, preorder);
    }
    return t1.values;
}

// Scan memory for reachable addresses. This is a little more complex than just reading one address at a time because:
//   1. We want it to be efficient, so we read a whole buffer at once and look for addresses in the buffer.
//   2. We need to handle alignment, byte order, and various sizes of addresses.
//   3. Depending on alignment and size, addresses can overlap each other in memory.
//   4. We need to watch out for addresses that would span two of our buffer-sized windows read from memory
//   5. We need to be careful of addresses that are 2^32 (or 2^64) to avoid overflow when incrementing
static void
insertAddressesFromMemory(AddressIntervalSet &reachable /*in,out*/, const P2::Partitioner &partitioner,
                          const MemoryMap::Ptr &map, size_t bytesPerWord, size_t alignment, ByteOrder::Endianness sex) {
    ASSERT_require(bytesPerWord <= sizeof(rose_addr_t));
    ASSERT_not_null(map);
    alignment = std::max(alignment, (size_t)1);
    uint8_t buf[4096];                                  // arbitrary size
    rose_addr_t bufVa = map->hull().least();
    while (map->atOrAfter(bufVa).next().assignTo(bufVa)) {

        {
            rose_addr_t tmp = alignUp(bufVa, alignment);
            if (tmp < bufVa)
                break;                                  // overflow
            bufVa = tmp;
        }
        size_t bufSize = map->at(bufVa).limit(sizeof buf).read(buf).size();
        if (bufSize < bytesPerWord) {
            bufVa += bytesPerWord;
            if (bufVa <= map->hull().least())
                break;                                  // overflow
            continue;
        }

        size_t bufOffset = 0;
        while (1) {
            bufOffset = alignUp(bufOffset, alignment);
            if (bufOffset + bytesPerWord > bufSize)
                break;

            rose_addr_t targetVa = 0;
            switch (sex) {
                case ByteOrder::ORDER_LSB:
                    for (size_t i=0; i<bytesPerWord; i++)
                        targetVa |= buf[bufOffset+i] << (8*i);
                    break;
                case ByteOrder::ORDER_MSB:
                    for (size_t i=0; i<bytesPerWord; i++)
                        targetVa = (targetVa << 8) | buf[bufOffset+i];
                    break;
                default:
                    ASSERT_not_reachable("invalid byte order");
            }
            P2::ControlFlowGraph::ConstVertexIterator targetVertex = partitioner.findPlaceholder(targetVa);
            if (targetVertex != partitioner.cfg().vertices().end())
                insertReachableRecursively(reachable, partitioner, targetVertex);
            ++bufOffset;
        }
        bufVa += bufOffset;
    }
}

// Scan reachable code to find immediate values mentioned in instructions. If an immediate value happens to be a basic block
// that's not reachable then add it to the reachable list. Do this recursively.
static void
insertReachableByImmediates(AddressIntervalSet &reachable /*in,out*/, const P2::Partitioner &partitioner) {
    VertexWorkList vertices = reachableVertices(reachable, partitioner);
    while (!vertices.isEmpty()) {
        P2::ControlFlowGraph::ConstVertexIterator vertex = vertices.popFront();
        BOOST_FOREACH (rose_addr_t va, findImmediateValues(vertex)) {
            if (P2::BasicBlock::Ptr targetBlock = partitioner.basicBlockContainingInstruction(va)) {
                P2::ControlFlowGraph::ConstVertexIterator targetVertex = partitioner.findPlaceholder(targetBlock->address());
                ASSERT_require2(partitioner.cfg().isValidVertex(targetVertex), "we already found its basic block");
                AddressIntervalSet targetVas = targetVertex->value().addresses();
                if (!reachable.contains(targetVas)) {
                    reachable |= targetVas;
                    vertices.insert(targetVertex);
                }
            }
        }
    }
}

// Run a data-flow analysis on a whole function to generate a list of concrete values that are stored in registers or memory.
static std::set<rose_addr_t>
findDataFlowValues(const P2::Partitioner &partitioner, const P2::Function::Ptr &function) {
    using namespace Rose::BinaryAnalysis::InstructionSemantics2;

    std::set<rose_addr_t> retval;
    BaseSemantics::RiscOperatorsPtr ops = partitioner.newOperators();
    BaseSemantics::DispatcherPtr cpu = partitioner.newDispatcher(ops);
    if (!cpu)
        return retval;

    // Build the data flow engine. We're using parts from a variety of locations.
    typedef P2::DataFlow::DfCfg                 DfCfg;
    typedef BaseSemantics::StatePtr             StatePtr;
    typedef P2::DataFlow::TransferFunction      TransferFunction;
    typedef DataFlow::SemanticsMerge            MergeFunction;
    typedef DataFlow::Engine<DfCfg, StatePtr, TransferFunction, MergeFunction> Engine;

    P2::ControlFlowGraph::ConstVertexIterator startVertex = partitioner.findPlaceholder(function->address());
    ASSERT_require2(partitioner.cfg().isValidVertex(startVertex), "function does not exist in partitioner");
    DfCfg dfCfg = P2::DataFlow::buildDfCfg(partitioner, partitioner.cfg(), startVertex); // not interprocedural
    size_t dfCfgStartVertexId = 0; // dfCfg vertex corresponding to function's entry ponit.
    TransferFunction xfer(cpu);
    MergeFunction mergeFunction(cpu);
    Engine engine(dfCfg, xfer, mergeFunction);
    engine.maxIterations(2 * dfCfg.nVertices());        // arbitrary limit for non-convergent flow

    StatePtr initialState = xfer.initialState();
    const RegisterDescriptor SP = cpu->stackPointerRegister();
    const RegisterDescriptor memSegReg;
    BaseSemantics::SValuePtr initialStackPointer = ops->readRegister(SP);
    size_t wordSize = SP.nBits() >> 3;              // word size in bytes

    // Run the data flow
    try {
        engine.runToFixedPoint(dfCfgStartVertexId, initialState);
    } catch (const BaseSemantics::Exception &e) {
        ::mlog[ERROR] <<function->printableName() <<": " <<e <<"\n"; // probably missing semantics capability for an instruction
        return retval;
    } catch (const DataFlow::NotConverging &e) {
        ::mlog[WARN] <<function->printableName() <<": " <<e.what() <<"\n";
    } catch (const DataFlow::Exception &e) {
        ::mlog[ERROR] <<function->printableName() <<": " <<e.what() <<"\n";
        return retval;
    }


    // Scan all outgoing states and accumulate any concrete values we find.
    BOOST_FOREACH (StatePtr state, engine.getFinalStates()) {
        if (state) {
            ops->currentState(state);
            BaseSemantics::RegisterStateGenericPtr regs =
                BaseSemantics::RegisterStateGeneric::promote(state->registerState());
            BOOST_FOREACH (const BaseSemantics::RegisterStateGeneric::RegPair &kv, regs->get_stored_registers()) {
                if (kv.value->is_number() && kv.value->get_width() <= SP.nBits())
                    retval.insert(kv.value->get_number());
            }

            BOOST_FOREACH (const StackVariable &var, P2::DataFlow::findStackVariables(ops, initialStackPointer)) {
                BaseSemantics::SValuePtr value = ops->readMemory(memSegReg, var.location.address,
                                                                 ops->undefined_(8*var.location.nBytes), ops->boolean_(true));
                if (value->is_number() && value->get_width() <= SP.nBits())
                    retval.insert(value->get_number());
            }

            BOOST_FOREACH (const AbstractLocation &var, P2::DataFlow::findGlobalVariables(ops, wordSize)) {
                if (var.isAddress()) {
                    BaseSemantics::SValuePtr value = ops->readMemory(memSegReg, var.getAddress(),
                                                                     ops->undefined_(8*var.nBytes()), ops->boolean_(true));
                    if (value->is_number() && value->get_width() <= SP.nBits())
                        retval.insert(value->get_number());
                }
            }
        }
    }
    return retval;
}

// List of all functions whose entry address is reachable.
static FunctionWorkList
reachableFunctions(const AddressIntervalSet &reachable, const P2::Partitioner &partitioner) {
    FunctionWorkList retval;
    BOOST_FOREACH (const P2::Function::Ptr &function, partitioner.functions()) {
        if (reachable.contains(function->address()))
            retval.pushBack(function);
    }
    return retval;
}

// Scan reachable functions to find concrete values by data-flow analysis. If an immediate value happens to be a basic block
// that's not reachable then add it to the reachable list. Do this recursively.
static void
insertReachableByDataFlow(AddressIntervalSet &reachable /*in,out*/, const P2::Partitioner &partitioner) {
    Sawyer::Container::Set<P2::Function::Ptr> processedFunctions;
    FunctionWorkList functions = reachableFunctions(reachable, partitioner);
    while (!functions.isEmpty()) {
        P2::Function::Ptr function = functions.popFront();
        processedFunctions.insert(function);
        BOOST_FOREACH (rose_addr_t va, findDataFlowValues(partitioner, function)) {
            if (P2::BasicBlock::Ptr targetBlock = partitioner.basicBlockContainingInstruction(va)) {
                P2::ControlFlowGraph::ConstVertexIterator targetVertex = partitioner.findPlaceholder(targetBlock->address());
                ASSERT_require2(partitioner.cfg().isValidVertex(targetVertex), "we already found its basic block");
                AddressIntervalSet targetVas = targetVertex->value().addresses();
                if (!reachable.contains(targetVas)) {
                    reachable |= targetVas;
                    insertReachableRecursively(reachable, partitioner, targetVertex);
                    P2::Function::Ptr targetFunction = targetVertex->value().isEntryBlock();
                    if (targetFunction &&
                        reachable.contains(targetFunction->address()) &&
                        !processedFunctions.exists(targetFunction)) {
                        functions.pushBack(targetFunction);
                    }
                }
            }
        }
    }
}

// True if a and b are both null or they both have the same encoding
static bool
sameInstruction(SgAsmInstruction *a, SgAsmInstruction *b) {
    if (NULL == a || NULL == b)
        return NULL == a && NULL == b;
    const SgUnsignedCharList &aBytes = a->get_raw_bytes();
    const SgUnsignedCharList &bBytes = b->get_raw_bytes();
    if (aBytes.size() != bBytes.size())
        return false;
    return std::equal(aBytes.begin(), aBytes.end(), bBytes.begin());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[]) {
    //---------------------------------------------
    // Initialization, parsing, disassembly, etc.
    //---------------------------------------------
    ROSE_INITIALIZE;
    Diagnostics::initAndRegister(&::mlog, "tool");
    Diagnostics::Stream info(::mlog[INFO]);

    // Use a partitioning engine since this makes this tool much easier to write.
    P2::Engine engine;

    // Parse the command-line
    Settings settings;
    std::vector<std::string> specimenNames = parseCommandLine(argc, argv, engine, settings);
    if (specimenNames.empty()) {
        ::mlog[FATAL] <<"no specimen specified; see --help\n";
        exit(1);
    }

    // Load the specimen as raw data or an ELF or PE container.
    info <<"loading specimen";
    Sawyer::Stopwatch timer;
    engine.loadSpecimens(specimenNames);
    if (settings.showMap)
        engine.memoryMap()->dump(std::cout);
    info <<"; took " <<timer <<" seconds\n";

    // Run the partitioner
    if (!P2::mlog[INFO])
        info <<"disassembling";
    timer.restart();
    P2::Partitioner partitioner = engine.createPartitioner();
    engine.runPartitioner(partitioner);
    if (!P2::mlog[INFO])
        info <<"; took " <<timer <<" seconds\n";
    if (partitioner.functions().empty() && engine.startingVas().empty())
        ::mlog[WARN] <<"no starting points for recursive disassembly; perhaps you need --start?\n";

    // Keep track of where instructions _might_ exist.
    ASSERT_not_null(engine.memoryMap());
    AddressIntervalSet executableSpace;
    BOOST_FOREACH (const MemoryMap::Node &node, engine.memoryMap()->nodes()) {
        if ((node.value().accessibility() & MemoryMap::EXECUTABLE)!=0)
            executableSpace.insert(node.key());
    }
    info <<"analyzing " <<StringUtility::plural(executableSpace.size(), "bytes") <<" of address space\n";

    if (0 == settings.addressSize)
        settings.addressSize = partitioner.newDispatcher(partitioner.newOperators())->stackPointerRegister().nBits() >> 3;
    if (0 == settings.addressAlignment)
        settings.addressAlignment = 1;

    //------------------------------------------ 
    // Mark things as reachable in various ways
    //------------------------------------------

    // Addresses declared reachable from the command-line
    AddressIntervalSet reachable;
    if (!settings.reachableVas.empty()) {
        info <<"marking declared reachable addresses";
        timer.restart();
        BOOST_FOREACH (const AddressInterval &interval, settings.reachableVas)
            reachable.insert(interval);
        info <<"; took " <<timer <<" seconds\n";
    }

    // Addresses declared reachable from the container (entry points, exports, etc).
    if (settings.useContainer) {
        info <<"marking entry points and exports";
        timer.restart();
        BOOST_FOREACH (const P2::Function::Ptr &function, partitioner.functions()) {
            if (0 != (function->reasons() & (SgAsmFunction::FUNC_ENTRY_POINT | SgAsmFunction::FUNC_EXPORT)))
                reachable.insert(function->address());
        }
        info <<"; took " <<timer <<" seconds\n";
    }

    // Addresses found by scanning memory
    if (!settings.readableVas.empty()) {
        using InstructionSemantics2::BaseSemantics::DispatcherPtr;
        info <<"scanning memory for addresses";
        timer.restart();
        MemoryMap::Ptr readable = engine.memoryMap()->shallowCopy();
        readable->any().changeAccess(0, MemoryMap::READABLE);
        BOOST_FOREACH (const AddressInterval &interval, settings.readableVas)
            readable->within(interval).changeAccess(MemoryMap::READABLE, 0);
        readable->require(MemoryMap::READABLE).keep();
        ByteOrder::Endianness sex = ByteOrder::ORDER_LSB;// FIXME[Robb P Matzke 2015-08-29]
        insertAddressesFromMemory(reachable /*in,out*/, partitioner, readable,
                                  settings.addressSize, settings.addressAlignment, sex);
        info <<"; took " <<timer <<" seconds\n";
    }
    
    // Recursively mark everything that reachable by CFG edges from other reachable things.
    info <<"recursive marking using the CFG";
    timer.restart();
    insertReachableRecursively(reachable, partitioner);
    info <<"; took " <<timer <<" seconds\n";

    // Addresses reachable because they're mentioned by immediates in other reachable code
    if (settings.lookForImmediates) {
        info <<"finding instruction immediate addresses";
        timer.restart();
        insertReachableByImmediates(reachable, partitioner);
        info <<"; took " <<timer <<" seconds\n";
    }

    // Addresses reachable because data-flow concrete values point to them
    if (settings.useDataFlow) {
        info <<"finding reachable code with pointers detected by data-flow";
        timer.restart();
        insertReachableByDataFlow(reachable, partitioner);
        info <<"; took " <<timer <<" seconds\n";
    }

    
    //------------------------------------------ 
    // Compute and show unreachable stuff
    //------------------------------------------
    timer.restart();
    info <<"generating results...\n";
    std::string sectionSeparator = "";

    // Show reachable addresses
    if (settings.showReachableAddresses) {
        std::cout <<sectionSeparator
                  <<"Found " <<StringUtility::plural(reachable.size(), "bytes") <<" of reachable, executable addresses:\n";
        BOOST_FOREACH (const AddressInterval &interval, reachable.intervals()) {
            std::cout <<"  " <<StringUtility::addrToString(interval.least())
                      <<" + " <<StringUtility::addrToString(interval.size())
                      <<" = " <<StringUtility::addrToString(interval.greatest()+1) <<"\n";
        }
        sectionSeparator = "\n";
    }

    // Everything that's non-reachable is dead code if it also appears in executable memory.
    AddressIntervalSet unreachable = ~reachable & executableSpace;
    if (unreachable.isEmpty()) {
        std::cout <<"no unreachable addresses found\n";
        return 0;
    }

    // Output unreachable addresses
    if (settings.showAddresses) {
        std::cout <<sectionSeparator
                  <<"Found " <<StringUtility::plural(unreachable.size(), "bytes") <<" of unreachable, executable addresses:\n";
        BOOST_FOREACH (const AddressInterval &interval, unreachable.intervals()) {
            std::cout <<"  " <<StringUtility::addrToString(interval.least())
                      <<" + " <<StringUtility::addrToString(interval.size())
                      <<" = " <<StringUtility::addrToString(interval.greatest()+1) <<"\n";
        }
        sectionSeparator = "\n";
    }

    // Output hexdump-style data
    if (settings.showHexDump) {
        std::cout <<sectionSeparator <<"Unreached addresses shown as data\n";
        HexdumpFormat fmt;
        fmt.prefix = "  ";
        uint8_t buf[4096];                              // arbitrary size, but a multiple of 16
        BOOST_FOREACH (const AddressInterval &interval, unreachable.intervals()) {
            std::cout <<"  " <<std::string(25, ':')
                      <<" " <<StringUtility::addrToString(interval.least())
                      <<" + " <<StringUtility::addrToString(interval.size())
                      <<" = " <<StringUtility::addrToString(interval.greatest()+1)
                      <<" " <<std::string(25, ':') <<"\n";
            rose_addr_t va = interval.least();
            rose_addr_t nRemain = interval.size();
            while (nRemain > 0) {
                size_t bufsz = std::min((rose_addr_t)sizeof(buf), nRemain);
                if (va % 16)                            // partial first line in order to align the rest
                    bufsz = std::min((rose_addr_t)bufsz, 16 - va%16);
                size_t nRead = engine.memoryMap()->at(va).limit(bufsz).read(buf).size();
                std::cout <<fmt.prefix;
                SgAsmExecutableFileFormat::hexdump(std::cout, va, buf, nRead, fmt);
                std::cout <<"\n";
                if (nRead < bufsz) {
                    ::mlog[ERROR] <<"short read: got " <<StringUtility::plural(nRead, "bytes") <<", expected " <<bufsz <<"\n";
                    break;
                }
                va += bufsz;
                nRemain -= bufsz;
            }
        }
        sectionSeparator = "\n";
    }

    // Output unreachable instructions
    if (settings.showInstructions) {
        std::cout <<sectionSeparator <<"Unreachable CFG instructions:\n";
        size_t nShown = 0;
        BOOST_FOREACH (const AddressInterval &interval, unreachable.intervals()) {
            std::cout <<"  " <<std::string(25, ':')
                      <<" " <<StringUtility::addrToString(interval.least())
                      <<" + " <<StringUtility::addrToString(interval.size())
                      <<" = " <<StringUtility::addrToString(interval.greatest()+1)
                      <<" " <<std::string(25, ':') <<"\n";
            std::vector<SgAsmInstruction*> insns = partitioner.instructionsOverlapping(interval);
            if (nShown > 0 && !insns.empty())
                std::cout <<"\n";
            BOOST_FOREACH (SgAsmInstruction *insn, insns) {
                std::cout <<"  " <<unparseInstructionWithAddress(insn) <<"\n";
                ++nShown;
            }
        }
        if (0 == nShown)
            std::cout <<"  none\n";
        sectionSeparator = "\n";
    }

    // Linear disassembly of each region
    if (settings.showDisassembly) {
        std::cout <<sectionSeparator <<"Disassembled unreachable addresses:\n";
        AsmUnparser unparser;
        unparser.set_prefix_format("  " + unparser.get_prefix_format());
        BOOST_FOREACH (const AddressInterval &interval, unreachable.intervals()) {
            std::cout <<"  " <<std::string(25, ':')
                      <<" " <<StringUtility::addrToString(interval.least())
                      <<" + " <<StringUtility::addrToString(interval.size())
                      <<" = " <<StringUtility::addrToString(interval.greatest()+1)
                      <<" " <<std::string(25, ':') <<"\n";
            SgAsmInstruction *prevInsn = NULL;
            size_t nRepeated = 0;
            rose_addr_t va = interval.least();
            while (va <= interval.greatest()) {
                SgAsmInstruction *insn = partitioner.instructionProvider()[va];
                size_t nBytes = insn ? insn->get_size() : 1;
                if (settings.useCompression && va != interval.least() && sameInstruction(prevInsn, insn)) {
                    ++nRepeated;
                } else {
                    if (nRepeated > 1)
                        std::cout <<"  repeated " <<StringUtility::plural(nRepeated, "more times") <<"\n";
                    nRepeated = 0;
                    if (insn) {
                        unparser.unparse(std::cout, insn);
                    } else {
                        std::cout <<"  no instruction\n";
                    }
                }
                prevInsn = insn;
                va += nBytes;
                if (va <= interval.least())
                    break;                              // increment overflowed
            }
            if (0 == nRepeated) {
                // no residual
            } else if (nRepeated > 1) {
                std::cout <<"  repeated " <<StringUtility::plural(nRepeated, "more times") <<"\n";
            } else if (prevInsn) {
                unparser.unparse(std::cout, prevInsn);
            } else {
                std::cout <<"  no instruction\n";
            }
        }
        sectionSeparator = "\n";
    }

    if (settings.showStats) {
        std::cout <<sectionSeparator <<"Statistics:\n";
        std::cout <<"  number of executable addresses:  " <<executableSpace.size() <<"\n";
        std::cout <<"  number of bytes in CFG:          " <<partitioner.aum().size()
                  <<" (" <<(100.0*partitioner.aum().size() / executableSpace.size()) <<"%)\n";
        std::cout <<"  number of reachable addresses:   " <<reachable.size()
                  <<" (" <<(100.0*reachable.size() / executableSpace.size()) <<"%)\n";
        std::cout <<"  number of unreachable addresses: " <<unreachable.size()
                  <<" (" <<(100.0*unreachable.size() / executableSpace.size()) <<"%)\n";
    }
    
    info <<"generating results; took " <<timer <<" seconds\n";
}
