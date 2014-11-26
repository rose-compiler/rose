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

// Attributes initialized by the first construction of a Context object (where they are documented)
P2::Attribute::Id ATTR_NBYTES(-1);
P2::Attribute::Id ATTR_NINSNS(-1);
P2::Attribute::Id ATTR_CFG_DOTFILE(-1);
P2::Attribute::Id ATTR_CFG_IMAGE(-1);
P2::Attribute::Id ATTR_CFG_COORDS(-1);
P2::Attribute::Id ATTR_CG(-1);
P2::Attribute::Id ATTR_NCALLERS(-1);
P2::Attribute::Id ATTR_NRETURNS(-1);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Count function size in bytes and cache as the function's ATTR_NBYTES attribute
static size_t
functionNBytes(const P2::Partitioner &partitioner, const P2::Function::Ptr &function) {
    size_t nBytes = 0;
    if (function && !function->attr<size_t>(ATTR_NBYTES).assignTo(nBytes)) {
        nBytes = partitioner.functionExtent(function).size();
        function->attr(ATTR_NBYTES, nBytes);
    }
    return nBytes;
}

// Count number of instructions in function and cache as the function's ATTR_NINSNS attribute
static size_t
functionNInsns(const P2::Partitioner &partitioner, const P2::Function::Ptr &function) {
    size_t nInsns = 0;
    if (function && !function->attr<size_t>(ATTR_NINSNS).assignTo(nInsns)) {
        BOOST_FOREACH (rose_addr_t bblockVa, function->basicBlockAddresses()) {
            if (P2::BasicBlock::Ptr bblock = partitioner.basicBlockExists(bblockVa))
                nInsns += bblock->nInstructions();
        }
        function->attr(ATTR_NINSNS, nInsns);
    }
    return nInsns;
}

// Generates a unique path relative to the doc root (--docroot switch).
static boost::filesystem::path
uniquePath(const std::string &extension) {
    return boost::filesystem::path("tmp") /  boost::filesystem::unique_path("ROSE-%%%%%%%%%%%%%%%%"+extension);
}

// Generate a GraphViz file describing a function's control flow graph and store the name of the file as the function's
// ATTR_CFG_DOTFILE attribute.
static boost::filesystem::path
functionCfgGraphvizFile(const P2::Partitioner &partitioner, const P2::Function::Ptr &function) {
    boost::filesystem::path fileName;
    if (function && !function->attr<boost::filesystem::path>(ATTR_CFG_DOTFILE).assignTo(fileName)) {

        // Write to the "dot" command, which will add layout information to our graph.
        // FIXME[Robb P. Matzke 2014-09-10]: how to do this on Windows?
        fileName = uniquePath(".dot");
        std::string dotCmd = "dot /proc/self/fd/0 >" + fileName.string();
        FILE *dot = popen(dotCmd.c_str(), "w");
        if (NULL==dot) {
            mlog[ERROR] <<"command failed: " <<dotCmd <<"\n";
            return boost::filesystem::path();
        }

        // Vertices
        std::set<size_t> vertices;
        fprintf(dot, "digraph G {\n");
        BOOST_FOREACH (rose_addr_t bblockVa, function->basicBlockAddresses()) {
            P2::ControlFlowGraph::ConstVertexNodeIterator placeholder = partitioner.findPlaceholder(bblockVa);
            ASSERT_require(placeholder != partitioner.cfg().vertices().end());
            if (bblockVa == function->address()) {
                // Entry vertex: light green
                fprintf(dot, "%zu [ shape=plaintext, label=\"0x%08"PRIx64"\", href=\"0x%"PRIx64"\""
                        ", style=filled, fillcolor=\"0.455,0.1,1\" ];\n",
                        placeholder->id(), bblockVa, bblockVa);
            } else if (0==placeholder->nInEdges()) {
                // Non-reachable vertex: light red
                fprintf(dot, "%zu [ shape=plaintext, label=\"0x%08"PRIx64"\", href=\"0x%"PRIx64"\""
                        ", style=filled, fillcolor=\"0,0.051,1\" ];\n",
                        placeholder->id(), bblockVa, bblockVa);
            } else {
                // Normal function vertex: no fill color
                fprintf(dot, "%zu [ shape=plaintext, label=\"0x%08"PRIx64"\", href=\"0x%"PRIx64"\" ];\n",
                        placeholder->id(), bblockVa, bblockVa);
            }
            vertices.insert(placeholder->id());
        }

        // Edges and special vertices
        BOOST_FOREACH (rose_addr_t bblockVa, function->basicBlockAddresses()) {
            P2::ControlFlowGraph::ConstVertexNodeIterator placeholder = partitioner.findPlaceholder(bblockVa);
            BOOST_FOREACH (const P2::ControlFlowGraph::EdgeNode &edge, placeholder->outEdges()) {
                P2::ControlFlowGraph::ConstVertexNodeIterator target = edge.target();
                // Vertices that weren't emitted above
                if (vertices.find(target->id())==vertices.end()) {
                    if (target==partitioner.undiscoveredVertex()) {
                        fprintf(dot, "%zu [ label=\"undiscovered\" ];\n", target->id());
                    } else if (target==partitioner.indeterminateVertex()) {
                        fprintf(dot, "%zu [ label=\"indeterminate\" ];\n", target->id());
                    } else if (target==partitioner.nonexistingVertex()) {
                        fprintf(dot, "%zu [ label=\"non-existing\" ];\n", target->id());
                    } else if (P2::Function::Ptr targetFunction = target->value().function()) {
                        // Function call. Use the address and first few characters of the function name.
                        std::string label = StringUtility::addrToString(targetFunction->address());
                        if (!targetFunction->name().empty()) {
                            if (targetFunction->name().size() > 12) {
                                label += "\\n" + StringUtility::cEscape(targetFunction->name()).substr(0, 12) + "...";
                            } else {
                                label += "\\n" + StringUtility::cEscape(targetFunction->name());
                            }
                        }
                        fprintf(dot, "%zu [ shape=box, label=\"%s\", href=\"0x%"PRIx64"\" ];\n",
                                target->id(), label.c_str(), targetFunction->address());
                    } else {
                        // non-call, inter-function edge
                        fprintf(dot, "%zu [ label=\"0x%08"PRIx64"\", style=filled, fillcolor=yellow ];\n",
                                target->id(), target->value().address());
                    }
                    vertices.insert(target->id());
                }

                // Emit the edge
                fprintf(dot, "%zu->%zu", placeholder->id(), target->id());
                if (edge.value().type() == P2::E_FUNCTION_RETURN)
                    fprintf(dot, " [ label=\"return\" ]");
                fprintf(dot, ";\n");
            }
        }
        fprintf(dot, "}\n");
        fclose(dot); dot=NULL;
        function->attr(ATTR_CFG_DOTFILE, fileName);
    }
    return fileName;
}

// Generate a JPEG image of the function's CFG
static boost::filesystem::path
functionCfgImage(const P2::Partitioner &partitioner, const P2::Function::Ptr &function) {
    boost::filesystem::path imageName;
    if (function && !function->attr<boost::filesystem::path>(ATTR_CFG_IMAGE).assignTo(imageName)) {
        boost::filesystem::path srcName = functionCfgGraphvizFile(partitioner, function);
        if (srcName.empty())
            return boost::filesystem::path();
        imageName = uniquePath(".jpg");
        std::string dotCmd = "dot -Tjpg -o" + imageName.string() + " " + srcName.native();
        if (0!=system(dotCmd.c_str())) {
            mlog[ERROR] <<"command failed: " <<dotCmd <<"\n";
            return boost::filesystem::path();
        }
        function->attr(ATTR_CFG_IMAGE, imageName);
    }
    return imageName;
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

