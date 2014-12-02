#define BOOST_FILESYSTEM_VERSION 3

#include <rose.h>
#include <rose_getline.h>
#include <rose_strtoull.h>
#include <rosePublicConfig.h>
#include <AsmFunctionIndex.h>
#include <AsmUnparser.h>
#include <BinaryControlFlow.h>
#include <BinaryFunctionCall.h>
#include <BinaryLoader.h>
#include <DisassemblerArm.h>
#include <DisassemblerPowerpc.h>
#include <DisassemblerMips.h>
#include <DisassemblerX86.h>
#include <DisassemblerM68k.h>
#include <Partitioner2/Attribute.h>
#include <Partitioner2/Engine.h>
#include <Partitioner2/ModulesM68k.h>
#include <Partitioner2/ModulesPe.h>

#include <sawyer/Assert.h>
#include <sawyer/CommandLine.h>
#include <sawyer/ProgressBar.h>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/foreach.hpp>
#include <boost/graph/graphviz.hpp>

using namespace rose;
using namespace rose::BinaryAnalysis;
using namespace rose::Diagnostics;

namespace P2 = Partitioner2;

static const rose_addr_t NO_ADDRESS(-1);

// Convenient struct to hold settings from the command-line all in one place.
struct Settings {
    std::string isaName;                                // instruction set architecture name
    size_t deExecuteZeros;                              // threshold for removing execute permissions of zeros (zero disables)
    bool useSemantics;                                  // should we use symbolic semantics?
    bool followGhostEdges;                              // do we ignore opaque predicates?
    bool allowDiscontiguousBlocks;                      // can basic blocks be discontiguous in memory?
    bool findFunctionPadding;                           // look for pre-entry-point padding?
    bool findDeadCode;                                  // do we look for unreachable basic blocks?
    bool intraFunctionData;                             // suck up unused addresses as intra-function data
    std::string httpAddress;                            // IP address at which to listen for HTTP connections
    unsigned short httpPort;                            // TCP port at which to listen for HTTP connections
    std::string docRoot;                                // document root directory for HTTP server
    Settings()
        : deExecuteZeros(0), useSemantics(false), followGhostEdges(false), allowDiscontiguousBlocks(true),
          findFunctionPadding(true), findDeadCode(true), intraFunctionData(true),
          httpAddress("0.0.0.0"), httpPort(80), docRoot(".") {}
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
                    std::string(settings.useSemantics?"true":"false") + "."));
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
                    "allow discontiguous basic blocks."));
    dis.insert(Switch("no-allow-discontiguous-blocks")
               .key("allow-discontiguous-blocks")
               .intrinsicValue(false, settings.allowDiscontiguousBlocks)
               .hidden(true));
    dis.insert(Switch("find-function-padding")
               .intrinsicValue(true, settings.findFunctionPadding)
               .doc("Look for padding such as zero bytes and certain instructions like no-ops that occur prior to the "
                    "lowest address of a function and attach them to the function as static data.  The "
                    "@s{no-find-function-padding} switch turns this off.  The default is to " +
                    std::string(settings.findFunctionPadding?"":"not ") + "search for padding."));
    dis.insert(Switch("no-find-function-padding")
               .key("find-function-padding")
               .intrinsicValue(false, settings.findFunctionPadding)
               .hidden(true));
    dis.insert(Switch("follow-ghost-edges")
               .intrinsicValue(true, settings.followGhostEdges)
               .doc("When discovering the instructions for a basic block, treat instructions individually rather than "
                    "looking for opaque predicates.  The @s{no-follow-ghost-edges} switch turns this off.  The default "
                    "is " + std::string(settings.followGhostEdges?"true":"false") + "."));
    dis.insert(Switch("no-follow-ghost-edges")
               .key("follow-ghost-edges")
               .intrinsicValue(false, settings.followGhostEdges)
               .hidden(true));
    dis.insert(Switch("find-dead-code")
               .intrinsicValue(true, settings.findDeadCode)
               .doc("Use ghost edges (non-followed control flow from branches with opaque predicates) to locate addresses "
                    "for unreachable code, then recursively discover basic blocks at those addresses and add them to the "
                    "same function.  The @s{no-find-dead-code} switch turns this off.  The default is " +
                    std::string(settings.findDeadCode?"true":"false") + "."));
    dis.insert(Switch("no-find-dead-code")
               .key("find-dead-code")
               .intrinsicValue(false, settings.findDeadCode)
               .hidden(true));
    dis.insert(Switch("intra-function-data")
               .intrinsicValue(true, settings.intraFunctionData)
               .doc("Near the end of processing, if there are regions of unused memory that are immediately preceded and "
                    "followed by the same function then add that region of memory to that function as a static data block."
                    "The @s{no-intra-function-data} switch turns this feature off.  The default is " +
                    std::string(settings.intraFunctionData?"true":"false") + "."));
    dis.insert(Switch("no-intra-function-data")
               .key("intra-function-data")
               .intrinsicValue(false, settings.intraFunctionData)
               .hidden(true));
    dis.insert(Switch("remove-zeros")
               .argument("size", nonNegativeIntegerParser(settings.deExecuteZeros), "128")
               .doc("This switch causes execute permission to be removed from sequences of contiguous zero bytes. The "
                    "switch argument is the minimum number of consecutive zeros that will trigger the removal, and "
                    "defaults to 128.  An argument of zero disables the removal.  When this switch is not specified at "
                    "all, this tool assumes a value of " + StringUtility::plural(settings.deExecuteZeros, "bytes") + "."));

    // Switches for HTTP server
    SwitchGroup server("Server switches");
    server.insert(Switch("http-address")
                  .argument("IP-address", anyParser(settings.httpAddress))
                  .doc("IP address to bind to server listening socket. The default is " + settings.httpAddress));
    server.insert(Switch("http-port")
                  .argument("TCP-port", nonNegativeIntegerParser(settings.httpPort))
                  .doc("TCP port at which the HTTP server will listen. The default is " +
                       boost::lexical_cast<std::string>(settings.httpPort) + "."));
    server.insert(Switch("docroot")
                  .argument("directory", anyParser(settings.docRoot))
                  .doc("Name of root directory for serving HTTP documents.  The default is \"" + settings.docRoot + "\"."));

    Parser parser;
    parser
        .purpose("binary ROSE on-line workbench for specimen exploration")
        .doc("synopsis",
             "@prop{programName} [@v{switches}] @v{specimen_names}")
        .doc("description",
             "This is a web server for viewing the contents of a binary specimen.")
        .doc("Specimens", P2::Engine::specimenNameDocumentation());
    
    return parser.with(gen).with(dis).with(server).parse(argc, argv).apply();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[]) {
    // Do this explicitly since librose doesn't do this automatically yet
    Diagnostics::initialize();

    // Parse the command-line
    P2::Engine engine;
    Settings settings;
    std::vector<std::string> specimenNames = parseCommandLine(argc, argv, settings).unreachedArgs();
    if (!settings.isaName.empty())
        engine.disassembler(Disassembler::lookup(settings.isaName));
    if (specimenNames.empty())
        throw std::runtime_error("no specimen specified; see --help");

    // Load the specimen as raw data or an ELF or PE container
    MemoryMap map = engine.load(specimenNames);
    P2::Modules::deExecuteZeros(map /*in,out*/, settings.deExecuteZeros);
    SgAsmInterpretation *interp = engine.interpretation();

    // Obtain a suitable disassembler if none was specified on the command-line
    Disassembler *disassembler = engine.obtainDisassembler();
    if (NULL==disassembler)
        throw std::runtime_error("an instruction set architecture must be specified with the \"--isa\" switch");

    // Create the partitioner
    P2::Partitioner partitioner = engine.createTunedPartitioner();
    partitioner.enableSymbolicSemantics(settings.useSemantics);
    if (settings.followGhostEdges)
        partitioner.basicBlockCallbacks().append(P2::Modules::AddGhostSuccessors::instance());
    if (!settings.allowDiscontiguousBlocks)
        partitioner.basicBlockCallbacks().append(P2::Modules::PreventDiscontiguousBlocks::instance());
    partitioner.memoryMap().dump(std::cout);            // show what we'll be working on

    // Disassemble and partition into functions
    engine.runPartitioner(partitioner, interp);
    std::cout <<"CFG contains " <<StringUtility::plural(partitioner.nFunctions(), "functions") <<"\n";
    std::cout <<"CFG contains " <<StringUtility::plural(partitioner.nBasicBlocks(), "basic blocks") <<"\n";
    std::cout <<"CFG contains " <<StringUtility::plural(partitioner.nDataBlocks(), "data blocks") <<"\n";
    std::cout <<"CFG contains " <<StringUtility::plural(partitioner.nInstructions(), "instructions") <<"\n";
    std::cout <<"CFG contains " <<StringUtility::plural(partitioner.nBytes(), "bytes") <<"\n";
    std::cout <<"Instruction cache contains "
              <<StringUtility::plural(partitioner.instructionProvider().nCached(), "instructions") <<"\n";

    std::filebuf fb;
    fb.open("test.dot", std::ios::out);
    std::ostream os(&fb); 
    partitioner.cfgGraphViz(os);
    fb.close();
    

    exit(0);
}

