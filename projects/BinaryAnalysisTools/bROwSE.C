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

#include <Wt/WAbstractTableModel>
#include <Wt/WApplication>
#include <Wt/WContainerWidget>
#include <Wt/WImage>
#include <Wt/WLink>
#include <Wt/WRectArea>
#include <Wt/WString>
#include <Wt/WTable>
#include <Wt/WTableView>
#include <Wt/WText>

using namespace rose;
using namespace rose::BinaryAnalysis;
using namespace rose::Diagnostics;

namespace P2 = Partitioner2;

static Disassembler *
getDisassembler(const std::string &name)
{
    if (0==name.compare("list")) {
        std::cout <<"The following ISAs are supported:\n"
                  <<"  amd64\n"
                  <<"  arm\n"
                  <<"  coldfire\n"
                  <<"  i386\n"
                  <<"  m68040\n"
                  <<"  mips\n"
                  <<"  ppc\n";
        exit(0);
    } else if (0==name.compare("arm")) {
        return new DisassemblerArm();
    } else if (0==name.compare("ppc")) {
        return new DisassemblerPowerpc();
    } else if (0==name.compare("mips")) {
        return new DisassemblerMips();
    } else if (0==name.compare("i386")) {
        return new DisassemblerX86(4);
    } else if (0==name.compare("amd64")) {
        return new DisassemblerX86(8);
    } else if (0==name.compare("m68040")) {
        return new DisassemblerM68k(m68k_68040);
    } else if (0==name.compare("coldfire")) {
        return new DisassemblerM68k(m68k_freescale_emacb);
    } else {
        throw std::runtime_error("invalid ISA name \""+name+"\"; use --isa=list");
    }
}

static const rose_addr_t NO_ADDRESS(-1);

// Convenient struct to hold settings from the command-line all in one place.
struct Settings {
    std::string isaName;                                // instruction set architecture name
    rose_addr_t mapVa;                                  // where to map the specimen in virtual memory
    size_t deExecuteZeros;                              // threshold for removing execute permissions of zeros (zero disables)
    bool useSemantics;                                  // should we use symbolic semantics?
    bool followGhostEdges;                              // do we ignore opaque predicates?
    bool allowDiscontiguousBlocks;                      // can basic blocks be discontiguous in memory?
    bool findFunctionPadding;                           // look for pre-entry-point padding?
    bool findSwitchCases;                               // search for C-like "switch" statement cases
    bool findDeadCode;                                  // do we look for unreachable basic blocks?
    bool intraFunctionData;                             // suck up unused addresses as intra-function data
    std::string httpAddress;                            // IP address at which to listen for HTTP connections
    unsigned short httpPort;                            // TCP port at which to listen for HTTP connections
    Settings()
        : mapVa(NO_ADDRESS), deExecuteZeros(0), useSemantics(true), followGhostEdges(false), allowDiscontiguousBlocks(true),
          findFunctionPadding(true), findSwitchCases(true), findDeadCode(true), intraFunctionData(true),
          httpAddress("0.0.0.0"), httpPort(9090) {}
};

// Describe and parse the command-line
static Sawyer::CommandLine::ParserResult
parseCommandLine(int argc, char *argv[], Settings &settings)
{
    using namespace Sawyer::CommandLine;

    // Generic switches
    SwitchGroup gen;
    gen.doc("General switches:");
    gen.insert(Switch("help", 'h')
               .doc("Show this documentation.")
               .action(showHelpAndExit(0)));
    gen.insert(Switch("log", 'L')
               .action(configureDiagnostics("log", Sawyer::Message::mfacilities))
               .argument("config")
               .whichValue(SAVE_ALL)
               .doc("Configures diagnostics.  Use \"@s{log}=help\" and \"@s{log}=list\" to get started."));
    gen.insert(Switch("version", 'V')
               .action(showVersionAndExit(version_message(), 0))
               .doc("Shows version information for various ROSE components and then exits."));
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
    SwitchGroup dis;
    dis.doc("Switches for disassembly:");
    dis.insert(Switch("isa")
               .argument("architecture", anyParser(settings.isaName))
               .doc("Instruction set architecture. Specify \"list\" to see a list of possible ISAs."));
    dis.insert(Switch("map")
               .argument("virtual-address", nonNegativeIntegerParser(settings.mapVa))
               .doc("If this switch is present, then the specimen is treated as raw data and mapped in its entirety "
                    "into the address space beginning at the address specified for this switch. Otherwise the file "
                    "is interpreted as an ELF or PE container."));
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
    dis.insert(Switch("find-switch-cases")
               .intrinsicValue(true, settings.findSwitchCases)
               .doc("Scan for common encodings of C-like \"switch\" statements so that the cases can be disassembled. The "
                    "@s{no-find-switch-cases} switch turns this off.  The default is " +
                    std::string(settings.findSwitchCases?"true":"false") + "."));
    dis.insert(Switch("no-find-switch-cases")
               .key("find-switch-cases")
               .intrinsicValue(false, settings.findSwitchCases)
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
    SwitchGroup server;
    server.doc("Switches for the HTTP server:");
    server.insert(Switch("http-address")
                  .argument("IP-address", anyParser(settings.httpAddress))
                  .doc("IP address to bind to server listening socket. The default is " + settings.httpAddress));
    server.insert(Switch("http-port")
                  .argument("TCP-port", nonNegativeIntegerParser(settings.httpPort))
                  .doc("TCP port at which the HTTP server will listen. The default is " +
                       boost::lexical_cast<std::string>(settings.httpPort) + "."));
    
    Parser parser;
    parser
        .purpose("binary ROSE on-line workbench for specimen exploration")
        .doc("synopsis",
             "@prop{programName} [@v{switches}] @v{specimen_name}")
        .doc("description",
             "This is a web server for viewing the contents of a binary specimen.");
    
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

// Context passed around to pretty much all the widgets.
class Context {
public:
    P2::Partitioner &partitioner;
    P2::Function::Ptr function;                         // current function

    Context(P2::Partitioner &partitioner): partitioner(partitioner) {
        if (ATTR_NBYTES == P2::Attribute::INVALID_ID) {
            ATTR_NBYTES         = P2::Attribute::registerName("Size in bytes");
            ATTR_NINSNS         = P2::Attribute::registerName("Number of instructions");
            ATTR_CFG_DOTFILE    = P2::Attribute::registerName("CFG GraphViz file name");
            ATTR_CFG_IMAGE      = P2::Attribute::registerName("CFG JPEG file name");
            ATTR_CFG_COORDS     = P2::Attribute::registerName("CFG vertex coordinates");
            ATTR_CG             = P2::Attribute::registerName("Function call graph");
        }
    }
};

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

static boost::filesystem::path
uniquePath(const std::string &extension) {
#if 0 // [Robb P. Matzke 2014-09-10]
    return boost::filesystem::temp_directory_path() / boost::filesystem::unique_path("ROSE-%%%%%%%%%%%%%%%%"+extension);
#else
    return boost::filesystem::path("tmp") /  boost::filesystem::unique_path("ROSE-%%%%%%%%%%%%%%%%"+extension);
#endif
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
        std::string dotCmd = "dot /proc/self/fd/0 >" + fileName.native();
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
                fprintf(dot, "%zu->%zu;\n", placeholder->id(), target->id());
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
        std::string dotCmd = "dot -Tjpg -o" + imageName.native() + " " + srcName.native();
        if (0!=system(dotCmd.c_str())) {
            mlog[ERROR] <<"command failed: " <<dotCmd <<"\n";
            return boost::filesystem::path();
        }
        function->attr(ATTR_CFG_IMAGE, imageName);
    }
    return imageName;
}

// Obtain coordinates for all the vertices in a CFG GraphViz file and cache them as the ATTR_CFG_COORDS attribute of the
// function.
struct Box { int x, y, dx, dy; };
typedef Sawyer::Container::Map<rose_addr_t, Box> CfgVertexCoords;
static CfgVertexCoords
functionCfgVertexCoords(const P2::Partitioner &partitioner, const P2::Function::Ptr &function) {
    // Things that need to be cleaned up on error
    struct Locals {
        FILE *coordFile;
        char *line;
        size_t linesz;
        Locals(): coordFile(NULL), line(NULL), linesz(0) {}
        ~Locals() {
            if (coordFile)
                fclose(coordFile);
            if (line)
                free(line);
        }
    } my;

    CfgVertexCoords coords;
    if (function && !function->attr<CfgVertexCoords>(ATTR_CFG_COORDS).assignTo(coords)) {
        boost::filesystem::path sourcePath = functionCfgGraphvizFile(partitioner, function);
        if (sourcePath.empty())
            throw std::runtime_error("CFG not available");
        std::string dotCmd = "dot -Timap_np -o/proc/self/fd/1 " + sourcePath.native();
        my.coordFile = popen(dotCmd.c_str(), "r");
        size_t linenum = 0;
        while (rose_getline(&my.line, &my.linesz, my.coordFile)>0) {
            ++linenum;
            if (!strncmp(my.line, "base referer", 12)) {
                // file always starts with this line; ignore it
            } else if (!strncmp(my.line, "rect ", 5)) {
                // example: "rect 0x401959 73,5 215,53"
                char *s=my.line+5, *t=NULL;
                Box box;

                errno = 0;
                rose_addr_t va = rose_strtoull(s, &t, 0);
                if (t==s || errno!=0) {
                    throw std::runtime_error("problem parsing line "+StringUtility::numberToString(linenum)+": "+
                                             StringUtility::trim(my.line));
                }

                s = t;
                errno = 0;
                box.x = strtol(s, &t, 0);
                if (t==s || errno!=0 || *t!=',') {
                    throw std::runtime_error("problem parsing line "+StringUtility::numberToString(linenum)+": "+
                                             StringUtility::trim(my.line));
                }

                s = ++t;
                errno = 0;
                box.y = strtol(s, &t, 0);
                if (t==s || errno!=0 || *t!=' ') {
                    throw std::runtime_error("problem parsing line "+StringUtility::numberToString(linenum)+": "+
                                             StringUtility::trim(my.line));
                }
                
                s = t;
                errno = 0;
                int x2 = strtol(s, &t, 0);
                if (t==s || errno!=0 || *t!=',') {
                    throw std::runtime_error("problem parsing line "+StringUtility::numberToString(linenum)+": "+
                                             StringUtility::trim(my.line));
                }

                s = ++t;
                errno = 0;
                int y2 = strtol(s, &t, 0);
                if (t==s || errno!=0) {
                    throw std::runtime_error("problem parsing line "+StringUtility::numberToString(linenum)+": "+
                                             StringUtility::trim(my.line));
                }

                if (box.x > x2 || box.y > y2) {
                    throw std::runtime_error("invalid box coords at line "+StringUtility::numberToString(linenum)+": "+
                                             StringUtility::trim(my.line));
                }
                box.dx = x2 - box.x + 1;
                box.dy = y2 - box.y + 1;
                coords.insert(va, box);
            }
        }
        function->attr(ATTR_CFG_COORDS, coords);
    }
    return coords;
}

// Generate a function call graph and cache it on the partitioner.
P2::FunctionCallGraph*
functionCallGraph(P2::Partitioner &partitioner) {
    P2::FunctionCallGraph *cg = NULL;
    if (!partitioner.attr<P2::FunctionCallGraph*>(ATTR_CG).assignTo(cg)) {
        cg = new P2::FunctionCallGraph(partitioner.functionCallGraph());
        partitioner.attr(ATTR_CG, cg);
    }
    return cg;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Model storing a list of functions.
class FunctionListModel: public Wt::WAbstractTableModel {
    Context &ctx_;
    std::vector<P2::Function::Ptr> functions_;
public:
    enum Column {
        C_ENTRY,                                        // function's primary entry address
        C_NAME,                                         // name if known, else empty string
        C_SIZE,                                         // size of function extent (code and data)
        C_NCOLS                                         // must be last
    };

    // Construct a model having the list of all functions in the partitioner
    FunctionListModel(Context &ctx): ctx_(ctx) {
        BOOST_FOREACH (const P2::Function::Ptr &function, ctx.partitioner.functions())
            functions_.push_back(function);
    }

    // Returns the function at the specified index, or null
    P2::Function::Ptr functionAt(size_t idx) {
        return idx<functions_.size() ? functions_[idx] : P2::Function::Ptr();
    }

    virtual int rowCount(const Wt::WModelIndex &parent=Wt::WModelIndex()) const /*override*/ {
        return parent.isValid() ? 0 : functions_.size();
    }

    virtual int columnCount(const Wt::WModelIndex &parent=Wt::WModelIndex()) const /*override*/ {
        return parent.isValid() ? 0 : C_NCOLS;
    }

    // Name for each column
    virtual boost::any headerData(int column, Wt::Orientation orientation=Wt::Horizontal,
                                  int role=Wt::DisplayRole) const /*override*/ {
        if (Wt::Horizontal == orientation && Wt::DisplayRole == role) {
            switch (column) {
                case C_ENTRY:   return Wt::WString("Entry");
                case C_NAME:    return Wt::WString("Name");
                case C_SIZE:    return Wt::WString("Size (bytes)");
                default:        return boost::any();
            }
        }
        return boost::any();
    }

    // Data to show in each table cell
    virtual boost::any data(const Wt::WModelIndex &index, int role=Wt::DisplayRole) const /*override*/ {
        ASSERT_require(index.isValid());
        ASSERT_require(index.row()>=0 && (size_t)index.row() < functions_.size());
        P2::Function::Ptr function = functions_[index.row()];
        if (Wt::DisplayRole == role) {
            switch (index.column()) {
                case C_ENTRY:
                    return Wt::WString(StringUtility::addrToString(function->address()));
                case C_NAME:
                    return Wt::WString(StringUtility::cEscape(function->name()));
                case C_SIZE:
                    return functionNBytes(ctx_.partitioner, function);
                default:
                    ASSERT_not_reachable("invalid column number");
            }
        }
        return boost::any();
    }

    static bool sortByAscendingAddress(const P2::Function::Ptr &a, const P2::Function::Ptr &b) {
        return a->address() < b->address();
    }
    static bool sortByDescendingAddress(const P2::Function::Ptr &a, const P2::Function::Ptr &b) {
        return a->address() > b->address();
    }
    static bool sortByAscendingName(const P2::Function::Ptr &a, const P2::Function::Ptr &b) {
        return a->name() < b->name();
    }
    static bool sortByDescendingName(const P2::Function::Ptr &a, const P2::Function::Ptr &b) {
        return a->name() > b->name();
    }
    static bool sortByAscendingSize(const P2::Function::Ptr &a, const P2::Function::Ptr &b) {
        return a->attr<size_t>(ATTR_NBYTES).orElse(0) < b->attr<size_t>(ATTR_NBYTES).orElse(0);
    }
    static bool sortByDescendingSize(const P2::Function::Ptr &a, const P2::Function::Ptr &b) {
        return a->attr<size_t>(ATTR_NBYTES).orElse(0) > b->attr<size_t>(ATTR_NBYTES).orElse(0);
    }
    
    void sort(int column, Wt::SortOrder order) {
        bool(*sorter)(const P2::Function::Ptr&, const P2::Function::Ptr&) = NULL;
        switch (column) {
            case C_ENTRY:
                sorter = Wt::AscendingOrder==order ? sortByAscendingAddress : sortByDescendingAddress;
                break;
            case C_NAME:
                sorter = Wt::AscendingOrder==order ? sortByAscendingName : sortByDescendingName;
                break;
            case C_SIZE:
                BOOST_FOREACH (const P2::Function::Ptr &function, functions_)
                    (void) functionNBytes(ctx_.partitioner, function); // make sure sizes are cached for all functions
                sorter = Wt::AscendingOrder==order ? sortByAscendingSize : sortByDescendingSize;
                break;
            default:
                ASSERT_not_reachable("invalid column number");
        }
        if (sorter) {
            layoutAboutToBeChanged().emit();
            std::sort(functions_.begin(), functions_.end(), sorter);
            layoutChanged().emit();
        }
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Presents a list of functions
class WFunctionList: public Wt::WContainerWidget {
    FunctionListModel *model_;
    Wt::WTableView *tableView_;
    Wt::Signal<P2::Function::Ptr> clicked_;
public:
    WFunctionList(FunctionListModel *model, Wt::WContainerWidget *parent=NULL)
        : Wt::WContainerWidget(parent), model_(model) {
        ASSERT_not_null(model);
        tableView_ = new Wt::WTableView(this);
        tableView_->setModel(model_);
        tableView_->setRowHeaderCount(1); // this must be first property set
        tableView_->setHeaderHeight(28);
        tableView_->setSortingEnabled(true);
        tableView_->setAlternatingRowColors(true);
        tableView_->setRowHeight(28);
        tableView_->setHeight(500);
        tableView_->setColumnWidth(1, 50);
        tableView_->setColumnWidth(1, 420);
        tableView_->setColumnResizeEnabled(true);
        tableView_->setSelectionMode(Wt::SingleSelection);
        tableView_->setEditTriggers(Wt::WAbstractItemView::NoEditTrigger);
        tableView_->clicked().connect(this, &WFunctionList::selectRow);
    }

    // Emitted when a row of the table is clicked
    Wt::Signal<P2::Function::Ptr>& clicked() {
        return clicked_;
    }

private:
    void selectRow(const Wt::WModelIndex &idx) {
        if (idx.isValid()) {
            P2::Function::Ptr function = model_->functionAt(idx.row());
            clicked_.emit(function);
        }
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Presents function summary information
class WFunctionSummary: public Wt::WContainerWidget {
    Context &ctx_;
    P2::Function::Ptr function_;
    Wt::WTable *table_;
    Wt::WText *wName_, *wEntry_, *wBBlocks_, *wDBlocks_, *wInsns_, *wBytes_, *wDiscontig_, *wCallers_, *wCallsInto_;
    Wt::WText *wCallees_, *wCallsOut_, *wRecursive_;
public:
    typedef std::pair<Wt::WText*, Wt::WText*> FieldPair;
    Wt::WText* field(std::vector<FieldPair> &fields, const std::string &label, const std::string &toolTip) {
        Wt::WText *wLabel = new Wt::WText(label+":");
        if (toolTip!="")
            wLabel->setToolTip(toolTip);
        Wt::WText *wValue = new Wt::WText();
        fields.push_back(std::make_pair(wLabel, wValue));
        return wValue;
    }

    WFunctionSummary(Context &ctx, Wt::WContainerWidget *parent=NULL)
        : Wt::WContainerWidget(parent), ctx_(ctx), table_(NULL), wName_(NULL), wEntry_(NULL), wBBlocks_(NULL), wDBlocks_(NULL),
          wInsns_(NULL), wBytes_(NULL), wDiscontig_(NULL), wCallers_(NULL), wCallsInto_(NULL), wCallees_(NULL),
          wCallsOut_(NULL), wRecursive_(NULL) {
        std::vector<FieldPair> fields;
        wName_          = field(fields, "Name",           "Function name");
        wEntry_         = field(fields, "Entry VA",       "Primary entry virtual address");
        wBBlocks_       = field(fields, "Basic blocks",   "Number of basic blocks");
        wDBlocks_       = field(fields, "Data blocks",    "Number of data blocks");
        wInsns_         = field(fields, "Instructions",   "Number of instructions");
        wBytes_         = field(fields, "Bytes",          "Number of distinct addresses with code and/or data");
        wDiscontig_     = field(fields, "Address regions","Number of discontiguous regions in the address space");
        wCallers_       = field(fields, "Callers",        "Number of distinct functions that call this function");
        wCallsInto_     = field(fields, "Calls into",     "Number of call sites calling this function");
        wCallees_       = field(fields, "Callees",        "Number of distinct functions this function calls.");
        wCallsOut_      = field(fields, "Calls out",      "Number of function call sites in this function");
        wRecursive_     = field(fields, "Recursive",      "Does this function call itself directly?");

        table_ = new Wt::WTable(this);
        table_->setWidth("100%");
        size_t nrows = (fields.size()+1) / 2;
        for (size_t col=0, i=0; col<2; ++col) {
            for (size_t row=0; row<nrows && i<fields.size(); ++row) {
                table_->elementAt(row, 2*col+0)->addWidget(fields[i].first);// label
                table_->elementAt(row, 2*col+1)->addWidget(fields[i].second);// value
                ++i;
            }
        }
    }

    void changeFunction(const P2::Function::Ptr &function) {
        if (function == function_)
            return;
        function_ = function;
        if (function) {
            size_t nInsns = functionNInsns(ctx_.partitioner, function);
            size_t nBytes = functionNBytes(ctx_.partitioner, function);
            size_t nIntervals = ctx_.partitioner.functionExtent(function).nIntervals();
            const P2::FunctionCallGraph *cg = functionCallGraph(ctx_.partitioner);
            ASSERT_not_null(cg);
            wName_      ->setText(function->name());
            wEntry_     ->setText(StringUtility::addrToString(function->address()));
            wBBlocks_   ->setText(StringUtility::plural(function->basicBlockAddresses().size(), "basic blocks"));
            wDBlocks_   ->setText(StringUtility::plural(function->dataBlocks().size(), "data blocks"));
            wInsns_     ->setText(StringUtility::plural(nInsns, "instructions"));
            wBytes_     ->setText(StringUtility::plural(nBytes, "bytes"));
            wDiscontig_ ->setText(nIntervals<=1 ? "Contiguous" : StringUtility::plural(nIntervals, "intervals"));
            wCallers_   ->setText(StringUtility::plural(cg->nCallers(function), "distinct functions"));
            wCallsInto_ ->setText(StringUtility::plural(cg->nCallsIn(function), "calls")+" incoming");
            wCallees_   ->setText(StringUtility::plural(cg->nCallees(function), "distinct functions"));
            wCallsOut_  ->setText(StringUtility::plural(cg->nCallsOut(function), "calls")+" outgoing");
            wRecursive_ ->setText(cg->nCalls(function, function)?"yes":"no");
        } else {
            wName_      ->setText("");
            wEntry_     ->setText("");
            wBBlocks_   ->setText("");
            wDBlocks_   ->setText("");
            wInsns_     ->setText("");
            wBytes_     ->setText("");
            wDiscontig_ ->setText("");
            wCallers_   ->setText("");
            wCallsInto_ ->setText("");
            wCallees_   ->setText("");
            wCallsOut_  ->setText("");
            wRecursive_ ->setText("");
        }
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Control flow graph for a function

class WFunctionCfg: public Wt::WContainerWidget {
    Context &ctx_;
    P2::Function::Ptr function_;                        // currently-displayed function
    Wt::WImage *wImage_;                                // image for the CFG
    Wt::WText *wMessage_;
    std::vector<std::pair<Wt::WRectArea*, rose_addr_t> > areas_;
public:
    WFunctionCfg(Context &ctx, Wt::WContainerWidget *parent=NULL)
        : Wt::WContainerWidget(parent), ctx_(ctx), wImage_(NULL) {
        wMessage_ = new Wt::WText("No function.", this);
    }

    void changeFunction(const P2::Function::Ptr &function) {
        if (function_ == function)
            return;
        function_ = function;

        // We need to create a new wImage each time because we're adding WRectArea.  See limitation documented for the
        // WImage::addArea method in Wt-3.3.3 [Robb P. Matzke 2014-09-10]
        areas_.clear();
        delete wImage_;
        wImage_ = new Wt::WImage(this);

        // Get the CFG image
        if (NULL==function_) {
            wMessage_->setText("No function.");
            wMessage_->show();
            wImage_->hide();
            return;
        }
        boost::filesystem::path imagePath = functionCfgImage(ctx_.partitioner, function);
        if (imagePath.empty()) {
            wMessage_->setText("CFG not available.");
            wMessage_->show();
            wImage_->hide();
            return;
        }
        wImage_->setImageLink(Wt::WLink(imagePath.native()));

        // Add sensitive areas to the image.
        try {
            if (!function)
                throw std::runtime_error("No function");
            CfgVertexCoords coords = functionCfgVertexCoords(ctx_.partitioner, function);
            BOOST_FOREACH (const CfgVertexCoords::Node &node, coords.nodes()) {
                rose_addr_t va = node.key();
                Wt::WRectArea *area = new Wt::WRectArea(node.value().x, node.value().y, node.value().dx, node.value().dy);
                P2::Function::Ptr callee = ctx_.partitioner.functionExists(va);
                if (callee && callee!=function) {
                    if (!callee->name().empty())
                        area->setToolTip(callee->name());
                    area->clicked().connect(this, &WFunctionCfg::selectFunction);
                } else if (P2::BasicBlock::Ptr bblock = ctx_.partitioner.basicBlockExists(va)) {
                    // Tooltip is the instructions, but be careful not to abuse the size
                    std::string toolTip;
                    bool exitEarly = bblock->nInstructions()>10;
                    for (size_t i=0; (i<9 || (!exitEarly && i<10)) && i<bblock->nInstructions(); ++i)
                        toolTip += (i?"\n":"") + unparseInstruction(bblock->instructions()[i]);
                    if (bblock->nInstructions()>10)
                        toolTip += "\nand " + StringUtility::numberToString(bblock->nInstructions()-9) + " more...";
                    area->setToolTip(toolTip);
                    area->clicked().connect(this, &WFunctionCfg::selectBasicBlock);
                }
                wImage_->addArea(area);
                areas_.push_back(std::make_pair(area, va));
            }
        } catch (const std::runtime_error &e) {
            wMessage_->setText(e.what());
            wMessage_->show();
            wImage_->hide();
            return;
        }

        wMessage_->hide();
        wImage_->show();
        return;
    }

private:
    void selectBasicBlock(const Wt::WMouseEvent &event) {
        mlog[INFO] <<"ROBB: clicked basic block at (" <<event.widget().x <<"," <<event.widget().y <<")\n";
    }
    void selectFunction(const Wt::WMouseEvent &event) {
        mlog[INFO] <<"ROBB: clicked function at (" <<event.widget().x <<"," <<event.widget().y <<")\n";
    }
};
        
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Application: public Wt::WApplication {
    Context ctx_;
    WFunctionList *wFunctionList_;                      // list of all functions
    WFunctionSummary *wFunctionSummary_;                // summary of current function
    WFunctionCfg *wFunctionCfg_;                        // control flow graph
public:
    Application(P2::Partitioner &partitioner, const Wt::WEnvironment &env): Wt::WApplication(env), ctx_(partitioner) {
        setTitle("bROwSE");
        setCssTheme("polished");

        wFunctionList_ = new WFunctionList(new FunctionListModel(ctx_), root());
        wFunctionList_->clicked().connect(this, &Application::setCurrentFunction);
        wFunctionSummary_ = new WFunctionSummary(ctx_, root());
        wFunctionCfg_ = new WFunctionCfg(ctx_, root());
    }

    void setCurrentFunction(const P2::Function::Ptr &function) {
        ctx_.function = function;
        wFunctionSummary_->changeFunction(function);
        wFunctionCfg_->changeFunction(function);
    }
};

class ApplicationCreator {
    P2::Partitioner &partitioner_;
public:
    ApplicationCreator(P2::Partitioner &partitioner): partitioner_(partitioner) {}
    Wt::WApplication* operator()(const Wt::WEnvironment &env) {
        return new Application(partitioner_, env);
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[]) {
    // Do this explicitly since librose doesn't do this automatically yet
    Diagnostics::initialize();

    // Parse the command-line
    Settings settings;
    Sawyer::CommandLine::ParserResult cmdline = parseCommandLine(argc, argv, settings);
    std::vector<std::string> positionalArgs = cmdline.unreachedArgs();
    Disassembler *disassembler = NULL;
    if (!settings.isaName.empty())
        disassembler = getDisassembler(settings.isaName);// do this before we check for positional arguments (for --isa=list)
    if (positionalArgs.empty())
        throw std::runtime_error("no specimen specified; see --help");
    if (positionalArgs.size()>1)
        throw std::runtime_error("too many specimens specified; see --help");
    std::string specimenName = positionalArgs[0];

    // Load the specimen as raw data or an ELF or PE container
    P2::Engine engine;
    SgAsmInterpretation *interp = NULL;
    MemoryMap map;
    if (settings.mapVa!=NO_ADDRESS) {
        if (!disassembler)
            throw std::runtime_error("an instruction set architecture must be specified with the \"--isa\" switch");
        size_t nBytesMapped = map.insertFile(specimenName, settings.mapVa);
        if (0==nBytesMapped)
            throw std::runtime_error("problem reading file: " + specimenName);
        map.at(settings.mapVa).limit(nBytesMapped).changeAccess(MemoryMap::EXECUTABLE, 0);
    } else {
        std::vector<std::string> args;
        args.push_back(argv[0]);
        args.push_back("-rose:binary");
        args.push_back("-rose:read_executable_file_format_only");
        args.push_back(specimenName);
        SgProject *project = frontend(args);
        std::vector<SgAsmInterpretation*> interps = SageInterface::querySubTree<SgAsmInterpretation>(project);
        if (interps.empty())
            throw std::runtime_error("a binary specimen container must have at least one SgAsmInterpretation");
        interp = interps.back();    // windows PE is always after DOS
        disassembler = engine.loadSpecimen(interp, disassembler);
        ASSERT_not_null(interp->get_map());
        map = *interp->get_map();
    }
    disassembler->set_progress_reporting(-1.0);         // turn it off
    P2::Modules::deExecuteZeros(map /*in,out*/, settings.deExecuteZeros);

    // Create the partitioner
    P2::Partitioner partitioner = engine.createTunedPartitioner(disassembler, map);
    partitioner.enableSymbolicSemantics(settings.useSemantics);
    if (settings.findSwitchCases)
        partitioner.basicBlockCallbacks().append(P2::ModulesM68k::SwitchSuccessors::instance());
    if (settings.followGhostEdges)
        partitioner.basicBlockCallbacks().append(P2::Modules::AddGhostSuccessors::instance());
    if (!settings.allowDiscontiguousBlocks)
        partitioner.basicBlockCallbacks().append(P2::Modules::PreventDiscontiguousBlocks::instance());
    partitioner.memoryMap().dump(std::cout);            // show what we'll be working on

    // Disassemble and partition into functions
    engine.partition(partitioner, interp);
    std::cout <<"CFG contains " <<StringUtility::plural(partitioner.nFunctions(), "functions") <<"\n";
    std::cout <<"CFG contains " <<StringUtility::plural(partitioner.nBasicBlocks(), "basic blocks") <<"\n";
    std::cout <<"CFG contains " <<StringUtility::plural(partitioner.nDataBlocks(), "data blocks") <<"\n";
    std::cout <<"CFG contains " <<StringUtility::plural(partitioner.nInstructions(), "instructions") <<"\n";
    std::cout <<"CFG contains " <<StringUtility::plural(partitioner.nBytes(), "bytes") <<"\n";
    std::cout <<"Instruction cache contains "
              <<StringUtility::plural(partitioner.instructionProvider().nCached(), "instructions") <<"\n";

    // Start the web server
    int wtArgc = 0;
    char *wtArgv[8];
    wtArgv[wtArgc++] = strdup(argv[0]);
    wtArgv[wtArgc++] = strdup("--docroot");
    wtArgv[wtArgc++] = strdup(".");
    wtArgv[wtArgc++] = strdup("--http-address");
    wtArgv[wtArgc++] = strdup(settings.httpAddress.c_str());
    wtArgv[wtArgc++] = strdup("--http-port");
    wtArgv[wtArgc++] = strdup(boost::lexical_cast<std::string>(settings.httpPort).c_str());
    wtArgv[wtArgc] = NULL;
    ApplicationCreator appCreator(partitioner);
    Wt::WRun(wtArgc, wtArgv, appCreator);

    exit(0);
}

