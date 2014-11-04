#include <rose.h>

#include <Diagnostics.h>
#include <Disassembler.h>
#include <Partitioner2/Engine.h>

#include <sawyer/Stopwatch.h>
#include <string>
#include <vector>

using namespace rose;
using namespace rose::Diagnostics;
using namespace rose::BinaryAnalysis;
namespace P2 = rose::BinaryAnalysis::Partitioner2;

static const rose_addr_t NO_FUNCTION_VA = 0xffffffff;

// Convenient struct to hold settings from the command-line all in one place.
struct Settings {
    std::string isaName;                                // instruction set architecture name
    size_t deExecuteZeros;                              // threshold for removing execute permissions of zeros (zero disables)
    bool useSemantics;                                  // should we use symbolic semantics?
    bool followGhostEdges;                              // do we ignore opaque predicates?
    bool allowDiscontiguousBlocks;                      // can basic blocks be discontiguous in memory?
    int partitionerVersion;                             // 1 or 2 for old or new implementation
    Settings()
        : deExecuteZeros(0), useSemantics(false), followGhostEdges(false), allowDiscontiguousBlocks(true),
          partitionerVersion(2) {}
};

// Describe and parse the command-line
static Sawyer::CommandLine::ParserResult
parseCommandLine(int argc, char *argv[], Settings &settings)
{
    using namespace Sawyer::CommandLine;

    // Generic switches
    SwitchGroup gen = CommandlineProcessing::genericSwitches();
    gen.insert(Switch("use-semantics")
               .intrinsicValue(true, settings.useSemantics)
               .doc("The partitioner can either use quick and naive methods of determining instruction characteristics, or "
                    "it can use slower but more accurate methods, such as symbolic semantics.  This switch enables use of "
                    "the slower symbolic semantics, or the feature can be disabled with @s{no-use-semantics}. The default is " +
                    std::string(settings.useSemantics?"true":"false") + ".  This switch has no effect for partitioner "
                    "version 1."));
    gen.insert(Switch("no-use-semantics")
               .key("use-semantics")
               .intrinsicValue(false, settings.useSemantics)
               .hidden(true));

    // Switches for disassembly
    SwitchGroup dis("Disassembly switches");
    dis.insert(Switch("isa")
               .argument("architecture", anyParser(settings.isaName))
               .doc("Instruction set architecture. Specify \"list\" to see a list of possible ISAs."));

    dis.insert(Switch("allow-discontiguous-blocks")
               .intrinsicValue(true, settings.allowDiscontiguousBlocks)
               .doc("This setting allows basic blocks to contain instructions that are discontiguous in memory as long as "
                    "the other requirements for a basic block are still met. Discontiguous blocks can be formed when a "
                    "compiler fails to optimize away an opaque predicate for a conditional branch, or when basic blocks "
                    "are scattered in memory by the introduction of unconditional jumps.  The @s{no-allow-discontiguous-blocks} "
                    "switch disables this feature and can slightly improve partitioner performance by avoiding cases where "
                    "an unconditional branch initially creates a larger basic block which is later discovered to be "
                    "multiple blocks.  The default is to " + std::string(settings.allowDiscontiguousBlocks?"":"not ") +
                    "allow discontiguous basic blocks.  This switch has no effect for partitioner version 1."));
    dis.insert(Switch("no-allow-discontiguous-blocks")
               .key("allow-discontiguous-blocks")
               .intrinsicValue(false, settings.allowDiscontiguousBlocks)
               .hidden(true));

    dis.insert(Switch("follow-ghost-edges")
               .intrinsicValue(true, settings.followGhostEdges)
               .doc("When discovering the instructions for a basic block, treat instructions individually rather than "
                    "looking for opaque predicates.  The @s{no-follow-ghost-edges} switch turns this off.  The default "
                    "is " + std::string(settings.followGhostEdges?"true":"false") + ".  This switch has no effect for"
                    "partitioner version 1."));
    dis.insert(Switch("no-follow-ghost-edges")
               .key("follow-ghost-edges")
               .intrinsicValue(false, settings.followGhostEdges)
               .hidden(true));

    dis.insert(Switch("partitioner")
               .argument("version", enumParser(settings.partitionerVersion)->with("1", 1)->with("2", 2))
               .doc("Specifies which version of the partitioner should be used.  Version 1" +
                    std::string(1==settings.partitionerVersion?", the default,":"") +
                    " is the original partitioner in rose::BinaryAnalysis::Partitioner and version 2" +
                    std::string(2==settings.partitionerVersion?", the default,":"") +
                    " is the newer version in rose::BinaryAnalysis::Partitioner2.  At this time (late 2014) version 2 "
                    " is still under active development."));

    dis.insert(Switch("remove-zeros")
               .argument("size", nonNegativeIntegerParser(settings.deExecuteZeros), "128")
               .doc("This switch causes execute permission to be removed from sequences of contiguous zero bytes. The "
                    "switch argument is the minimum number of consecutive zeros that will trigger the removal, and "
                    "defaults to 128.  An argument of zero disables the removal.  When this switch is not specified at "
                    "all, this tool assumes a value of " + StringUtility::plural(settings.deExecuteZeros, "bytes") + ". "
                    "This switch has no effect for partitioner version 1."));

    Parser parser;
    parser
        .purpose("locate and dump information about functions")
        .version(std::string(ROSE_SCM_VERSION_ID).substr(0, 8), ROSE_CONFIGURE_DATE)
        .chapter(1, "ROSE Command-line Tools")
        .doc("synopsis",
             "@prop{programName} [@v{switches}] @v{specimen_names}")
        .doc("description",
             "Parses, disassembles and partitions the specimens given as positional arguments on the command-line, then "
             "produces a CSV file on standard output for comparing with other disassembly tools.")
        .doc("Specimens",
             "The binary specimen can be constructed from files in two ways."

             "@bullet{If the specimen name is a simple file name then the specimen is passed to ROSE's \"frontend\" "
             "so its container format (ELF, PE, etc) can be parsed and its segments loaded into virtual memory. All "
             "such specimens are passed to \"frontend\" at once in a single call.}"

             "@bullet{If the specimen name begins with the string \"map:\" then it is treated as a memory map resource "
             "string. " + MemoryMap::insertFileDocumentation() + "}"

             "Multiple memory map resources can be specified. If both types of files are specified, ROSE's \"frontend\" "
             "and \"BinaryLoader\" run first on the regular files and then the map resources are applied.");
    
    return parser.with(gen).with(dis).parse(argc, argv).apply();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Emit CSV records

static void
emitBasicBlockCsv(const P2::Partitioner &partitioner, rose_addr_t bblockVa, rose_addr_t functionVa) {
    if (P2::BasicBlock::Ptr bblock = partitioner.basicBlockExists(bblockVa)) {
        // Instructions belonging to this basic block
        BOOST_FOREACH (SgAsmInstruction *insn, bblock->instructions()) {
            std::cout <<std::hex <<insn->get_address() <<","
                      <<std::dec <<insn->get_size() <<","
                      <<"CODE,"
                      <<std::hex <<functionVa <<"\n";
        }
        // Data blocks associated with a basic block
        BOOST_FOREACH (const P2::DataBlock::Ptr &dblock, bblock->dataBlocks()) {
            std::cout <<std::hex <<dblock->address() <<","
                      <<std::dec <<dblock->size() <<","
                      <<"DATA,"
                      <<std::hex <<functionVa <<"\n";
        }
    } else {
        // Basic block placeholder (perhaps the address is not mapped or doesn't have execute permission?)
        std::cout <<std::hex <<bblockVa <<","
                  <<"0,"
                  <<"CODE,"
                  <<std::hex <<functionVa <<"\n";
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Old partitioner results

static void
partitioner1(const Settings &settings, SgAsmInterpretation *interp, Disassembler *disassembler, MemoryMap map) {
    Partitioner *partitioner = new Partitioner();
    map.dump(mlog[INFO]);
    Stream info(mlog[INFO] <<"Disassembling and partitioning (version 1)");
    Sawyer::Stopwatch partitionTime;
    SgAsmBlock *block = partitioner->partition(interp, disassembler, &map);
    info <<"; completed in " <<partitionTime <<" seconds.\n";

    // Show some stats
    struct T1: AstSimpleProcessing {
        size_t nFunctions, nDataBlocks, nInstructions;
        std::set<rose_addr_t> basicBlocks;
        AddressIntervalSet bytes;
        T1(): nFunctions(0), nDataBlocks(0), nInstructions(0) {}
        void visit(SgNode *node) {
            if (isSgAsmFunction(node)) {
                ++nFunctions;
            } else if (SgAsmInstruction *insn = isSgAsmInstruction(node)) {
                ++nInstructions;
                // All insns belong to a basic block, but not all SgAsmBlock nodes are basic blocks
                if (SgAsmBlock *bb = SageInterface::getEnclosingNode<SgAsmBlock>(insn))
                    basicBlocks.insert(bb->get_address());
                // Count each insn address only once regardless of how many instructions overlap at that address
                bytes.insert(AddressInterval::baseSize(insn->get_address(), insn->get_size()));
            } else if (isSgAsmStaticData(node)) {
                ++nDataBlocks;
            }
        }
    } t1;
    t1.traverse(block, preorder);
    info <<"Partitioned "
         <<StringUtility::plural(t1.nFunctions, "functions") <<", "
         <<StringUtility::plural(t1.basicBlocks.size(), "basic blocks") <<", "
         <<StringUtility::plural(t1.nDataBlocks, "data blocks") <<", "
         <<StringUtility::plural(t1.nInstructions, "instructions") <<", "
         <<StringUtility::plural(t1.bytes.size(), "bytes") <<"\n";

    // Dump CSV records to standard output
    struct: AstSimpleProcessing {
        void visit(SgNode *node) {
            if (SgAsmInstruction *insn = isSgAsmInstruction(node)) {
                if (SgAsmFunction *function = SageInterface::getEnclosingNode<SgAsmFunction>(insn)) {
                    std::cout <<std::hex <<insn->get_address() <<","
                              <<std::dec <<insn->get_size() <<","
                              <<"CODE,"
                              <<std::hex <<function->get_entry_va() <<"\n";
                } else {
                    std::cout <<std::hex <<insn->get_address() <<","
                              <<std::dec <<insn->get_size() <<","
                              <<"CODE,"
                              <<std::hex <<NO_FUNCTION_VA <<"\n";
                }
            } else if (SgAsmStaticData *data = isSgAsmStaticData(node)) {
                if (SgAsmFunction *function = SageInterface::getEnclosingNode<SgAsmFunction>(insn)) {
                    std::cout <<std::hex <<data->get_address() <<","
                              <<std::dec <<data->get_size() <<","
                              <<"DATA,"
                              <<std::hex <<function->get_entry_va() <<"\n";
                } else {
                    std::cout <<std::hex <<data->get_address() <<","
                              <<std::dec <<data->get_size() <<","
                              <<"DATA,"
                              <<std::hex <<NO_FUNCTION_VA <<"\n";
                }
            }
        }
    } t2;
    t2.traverse(block, preorder);
    delete partitioner;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// New partitioner results

static void
partitioner2(const Settings &settings, SgAsmInterpretation *interp, Disassembler *disassembler, MemoryMap map) {
    // Remove execute permission from regions of memory that contain only zero.  This will prevent them from being disassembled
    // since they're almost always padding at the end of sections (e.g., MVC pads all sections to 256 bytes by appending zeros).
    P2::Modules::deExecuteZeros(map /*in,out*/, settings.deExecuteZeros);

    // Create a partitioner that's tuned for a certain architecture, and then tune it even more depending on our command-line.
    map.dump(mlog[INFO]);
    Stream info(mlog[INFO] <<"Disassembling and partitioning (version 2)");
    Sawyer::Stopwatch partitionTime;
    P2::Engine engine;
    engine.interpretation(interp);
    engine.memoryMap(map);
    engine.disassembler(disassembler);
    P2::Partitioner partitioner = engine.createTunedPartitioner();
    partitioner.enableSymbolicSemantics(settings.useSemantics);
    if (settings.followGhostEdges)
        partitioner.basicBlockCallbacks().append(P2::Modules::AddGhostSuccessors::instance());
    if (!settings.allowDiscontiguousBlocks)
        partitioner.basicBlockCallbacks().append(P2::Modules::PreventDiscontiguousBlocks::instance());
    
    engine.runPartitioner(partitioner, interp);
    info <<"; completed in " <<partitionTime <<" seconds.\n";

    // Show some stats
    info <<"Partitioned "
         <<StringUtility::plural(partitioner.nFunctions(), "functions") <<", "
         <<StringUtility::plural(partitioner.nBasicBlocks(), "basic blocks") <<", "
         <<StringUtility::plural(partitioner.nDataBlocks(), "data blocks") <<", "
         <<StringUtility::plural(partitioner.nInstructions(), "instructions") <<", "
         <<StringUtility::plural(partitioner.nBytes(), "bytes") <<"\n";

    // Dump records for each function.
    BOOST_FOREACH (const P2::Function::Ptr &function, partitioner.functions()) {
        BOOST_FOREACH (rose_addr_t bblockVa, function->basicBlockAddresses()) {
            emitBasicBlockCsv(partitioner, bblockVa, function->address());
        }
        BOOST_FOREACH (const P2::DataBlock::Ptr &dblock, function->dataBlocks()) {
            std::cout <<std::hex <<dblock->address() <<","
                      <<std::dec <<dblock->size() <<","
                      <<"DATA,"
                      <<std::hex <<function->address() <<"\n";
        }
    }

    // Dump records for each basic block that isn't attached to a function.
    BOOST_FOREACH (const P2::ControlFlowGraph::VertexValue &vertex, partitioner.cfg().vertexValues()) {
        if (vertex.type() == P2::V_BASIC_BLOCK && !vertex.function())
            emitBasicBlockCsv(partitioner, vertex.address(), NO_FUNCTION_VA);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[]) {
    Diagnostics::initialize();

    // Parse the command-line
    Settings settings;
    std::vector<std::string> specimenNames = parseCommandLine(argc, argv, settings).unreachedArgs();
    Disassembler *disassembler = NULL;
    if (!settings.isaName.empty())
        disassembler = Disassembler::lookup(settings.isaName);
    if (specimenNames.empty())
        throw std::runtime_error("no specimen specified; see --help");
    Stream info(mlog[INFO]);

    // Load the specimen as raw data or an ELF/PE container.
    P2::Engine engine;
    MemoryMap map = engine.load(specimenNames);
    SgAsmInterpretation *interp = SageInterface::getProject() ?
                                  SageInterface::querySubTree<SgAsmInterpretation>(SageInterface::getProject()).back() :
                                  NULL;

    // Obtain a suitable disassembler if none was specified on the command-line
    if (!disassembler) {
        if (!interp)
            throw std::runtime_error("an instruction set architecture must be specified with the \"--isa\" switch");
        disassembler = Disassembler::lookup(interp);
        if (!disassembler)
            throw std::runtime_error("unable to find an appropriate disassembler");
        disassembler = disassembler->clone();
    }
    disassembler->set_progress_reporting(-1.0);         // turn it off

    // Disassemble, partition, and show results
    if (1==settings.partitionerVersion) {
        partitioner1(settings, interp, disassembler, map);
    } else {
        partitioner2(settings, interp, disassembler, map);
    }
}
