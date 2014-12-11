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
#include <Wt/WBreak>
#include <Wt/WContainerWidget>
#include <Wt/WEnvironment>
#include <Wt/WGridLayout>
#include <Wt/WHBoxLayout>
#include <Wt/WImage>
#include <Wt/WLink>
#include <Wt/WRectArea>
#include <Wt/WScrollArea>
#include <Wt/WString>
#include <Wt/WTable>
#include <Wt/WTableView>
#include <Wt/WTabWidget>
#include <Wt/WText>
#include <Wt/WVBoxLayout>

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

// Context passed around to pretty much all the widgets.
class Context {
public:
    P2::Partitioner &partitioner;
    Wt::WApplication *application;                      // Wt probably has a method to get this, but I can't find it

    Context(P2::Partitioner &partitioner, Wt::WApplication *app): partitioner(partitioner), application(app) {
        if (ATTR_NBYTES == P2::Attribute::INVALID_ID) {
            ATTR_NBYTES         = P2::Attribute::registerName("Size in bytes");
            ATTR_NINSNS         = P2::Attribute::registerName("Number of instructions");
            ATTR_CFG_DOTFILE    = P2::Attribute::registerName("CFG GraphViz file name");
            ATTR_CFG_IMAGE      = P2::Attribute::registerName("CFG JPEG file name");
            ATTR_CFG_COORDS     = P2::Attribute::registerName("CFG vertex coordinates");
            ATTR_CG             = P2::Attribute::registerName("Function call graph");
            ATTR_NCALLERS       = P2::Attribute::registerName("Number of call sites from whence function is called");
            ATTR_NRETURNS       = P2::Attribute::registerName("Number of returning basic blocks");
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
        std::string dotCmd = "dot -Timap_np -o/proc/self/fd/1 " + sourcePath.string();
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

// Calculate the number of places from whence a function is called and cache it in the function.
size_t
functionNCallers(P2::Partitioner &partitioner, const P2::Function::Ptr &function) {
    size_t nCallers = 0;
    if (function && !function->attr<size_t>(ATTR_NCALLERS).assignTo(nCallers)) {
        P2::FunctionCallGraph *cg = functionCallGraph(partitioner);
        ASSERT_not_null(cg);
        nCallers = cg->nCallsIn(function);
        function->attr(ATTR_NCALLERS, nCallers);
    }
    return nCallers;
}

// Calculates the number of function return edges and caches it in the function
size_t
functionNReturns(P2::Partitioner &partitioner, const P2::Function::Ptr &function) {
    size_t nReturns = 0;
    if (function && !function->attr<size_t>(ATTR_NRETURNS).assignTo(nReturns)) {
        BOOST_FOREACH (rose_addr_t bblockVa, function->basicBlockAddresses()) {
            P2::ControlFlowGraph::ConstVertexNodeIterator vertex = partitioner.findPlaceholder(bblockVa);
            if (vertex != partitioner.cfg().vertices().end()) {
                BOOST_FOREACH (const P2::ControlFlowGraph::EdgeNode &edge, vertex->outEdges()) {
                    if (edge.value().type() == P2::E_FUNCTION_RETURN)
                        ++nReturns;
                }
            }
        }
        function->attr(ATTR_NRETURNS, nReturns);
    }
    return nReturns;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Model storing a list of functions.
class FunctionListModel: public Wt::WAbstractTableModel {
    Context &ctx_;
    std::vector<P2::Function::Ptr> functions_;
public:
    // To change the order of columns in the table, just change them in this enum.
    enum Column {
        C_ENTRY,                                        // function's primary entry address
        C_NAME,                                         // name if known, else empty string
        C_SIZE,                                         // size of function extent (code and data)
        C_NCALLERS,                                     // number of incoming calls
        C_NRETURNS,                                     // how many return edges, i.e., might the function return?
        C_IMPORT,                                       // is function an import
        C_EXPORT,                                       // is function an export
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

    virtual int rowCount(const Wt::WModelIndex &parent=Wt::WModelIndex()) const ROSE_OVERRIDE {
        return parent.isValid() ? 0 : functions_.size();
    }

    virtual int columnCount(const Wt::WModelIndex &parent=Wt::WModelIndex()) const ROSE_OVERRIDE {
        return parent.isValid() ? 0 : C_NCOLS;
    }

    // Name for each column
    virtual boost::any headerData(int column, Wt::Orientation orientation=Wt::Horizontal,
                                  int role=Wt::DisplayRole) const ROSE_OVERRIDE {
        if (Wt::Horizontal == orientation && Wt::DisplayRole == role) {
            switch (column) {
                case C_ENTRY:    return Wt::WString("Entry");
                case C_NAME:     return Wt::WString("Name");
                case C_SIZE:     return Wt::WString("Size");
                case C_IMPORT:   return Wt::WString("Import");
                case C_EXPORT:   return Wt::WString("Export");
                case C_NCALLERS: return Wt::WString("Calls");
                case C_NRETURNS: return Wt::WString("Returns");
                default:         return boost::any();
            }
        }
        return boost::any();
    }

    // Data to show in each table cell
    virtual boost::any data(const Wt::WModelIndex &index, int role=Wt::DisplayRole) const ROSE_OVERRIDE {
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
                case C_IMPORT:
                    return Wt::WString((function->reasons() & SgAsmFunction::FUNC_IMPORT)!=0 ? "yes" : "no");
                case C_EXPORT:
                    return Wt::WString((function->reasons() & SgAsmFunction::FUNC_EXPORT)!=0 ? "yes" : "no");
                case C_NCALLERS:
                    return functionNCallers(ctx_.partitioner, function);
                case C_NRETURNS:
                    return functionNReturns(ctx_.partitioner, function);
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
    static bool sortByAscendingImport(const P2::Function::Ptr &a, const P2::Function::Ptr &b) {
        unsigned aa = a->reasons() & SgAsmFunction::FUNC_IMPORT;
        unsigned bb = b->reasons() & SgAsmFunction::FUNC_IMPORT;
        return aa < bb;
    }
    static bool sortByDescendingImport(const P2::Function::Ptr &a, const P2::Function::Ptr &b) {
        unsigned aa = a->reasons() & SgAsmFunction::FUNC_IMPORT;
        unsigned bb = b->reasons() & SgAsmFunction::FUNC_IMPORT;
        return aa > bb;
    }
    static bool sortByAscendingExport(const P2::Function::Ptr &a, const P2::Function::Ptr &b) {
        unsigned aa = a->reasons() & SgAsmFunction::FUNC_EXPORT;
        unsigned bb = b->reasons() & SgAsmFunction::FUNC_EXPORT;
        return aa < bb;
    }
    static bool sortByDescendingExport(const P2::Function::Ptr &a, const P2::Function::Ptr &b) {
        unsigned aa = a->reasons() & SgAsmFunction::FUNC_EXPORT;
        unsigned bb = b->reasons() & SgAsmFunction::FUNC_EXPORT;
        return aa > bb;
    }
    static bool sortByAscendingCallers(const P2::Function::Ptr &a, const P2::Function::Ptr &b) {
        return a->attr<size_t>(ATTR_NCALLERS).orElse(0) < b->attr<size_t>(ATTR_NCALLERS).orElse(0);
    }
    static bool sortByDescendingCallers(const P2::Function::Ptr &a, const P2::Function::Ptr &b) {
        return a->attr<size_t>(ATTR_NCALLERS).orElse(0) > b->attr<size_t>(ATTR_NCALLERS).orElse(0);
    }
    static bool sortByAscendingReturns(const P2::Function::Ptr &a, const P2::Function::Ptr &b) {
        return a->attr<size_t>(ATTR_NRETURNS).orElse(0) < b->attr<size_t>(ATTR_NRETURNS).orElse(0);
    }
    static bool sortByDescendingReturns(const P2::Function::Ptr &a, const P2::Function::Ptr &b) {
        return a->attr<size_t>(ATTR_NRETURNS).orElse(0) > b->attr<size_t>(ATTR_NRETURNS).orElse(0);
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
            case C_IMPORT:
                sorter = Wt::AscendingOrder==order ? sortByAscendingImport : sortByDescendingImport;
                break;
            case C_EXPORT:
                sorter = Wt::AscendingOrder==order ? sortByAscendingExport : sortByDescendingExport;
                break;
            case C_NCALLERS:
                BOOST_FOREACH (const P2::Function::Ptr &function, functions_)
                    (void) functionNCallers(ctx_.partitioner, function); // make sure they're all cached before sorting
                sorter = Wt::AscendingOrder==order ? sortByAscendingCallers : sortByDescendingCallers;
                break;
            case C_NRETURNS:
                BOOST_FOREACH (const P2::Function::Ptr &function, functions_)
                    (void) functionNReturns(ctx_.partitioner, function); // make sure they're all cached
                sorter = Wt::AscendingOrder==order ? sortByAscendingReturns : sortByDescendingReturns;
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
    Wt::Signal<P2::Function::Ptr> doubleClicked_;
public:
    WFunctionList(FunctionListModel *model, Wt::WContainerWidget *parent=NULL)
        : Wt::WContainerWidget(parent), model_(model) {
        ASSERT_not_null(model);

        tableView_ = new Wt::WTableView;
        tableView_->setModel(model_);
        tableView_->setRowHeaderCount(1); // this must be first property set
        tableView_->setHeaderHeight(28);
        tableView_->setSortingEnabled(true);
        tableView_->setAlternatingRowColors(true);
        tableView_->setColumnWidth(FunctionListModel::C_ENTRY,          Wt::WLength( 6, Wt::WLength::FontEm));
        tableView_->setColumnWidth(FunctionListModel::C_NAME,           Wt::WLength(30, Wt::WLength::FontEm));
        tableView_->setColumnWidth(FunctionListModel::C_SIZE,           Wt::WLength( 5, Wt::WLength::FontEm));
        tableView_->setColumnWidth(FunctionListModel::C_NCALLERS,       Wt::WLength( 5, Wt::WLength::FontEm));
        tableView_->setColumnWidth(FunctionListModel::C_NRETURNS,       Wt::WLength( 5, Wt::WLength::FontEm));
        tableView_->setColumnWidth(FunctionListModel::C_IMPORT,         Wt::WLength( 5, Wt::WLength::FontEm));
        tableView_->setColumnWidth(FunctionListModel::C_EXPORT,         Wt::WLength( 5, Wt::WLength::FontEm));
        tableView_->setColumnResizeEnabled(true);
        tableView_->setSelectionMode(Wt::SingleSelection);
        tableView_->setEditTriggers(Wt::WAbstractItemView::NoEditTrigger);
        tableView_->clicked().connect(this, &WFunctionList::clickRow);
        tableView_->doubleClicked().connect(this, &WFunctionList::doubleClickRow);

        Wt::WHBoxLayout *hbox = new Wt::WHBoxLayout;
        setLayout(hbox);
        hbox->addWidget(tableView_);
    }

    // Emitted when a row of the table is clicked
    Wt::Signal<P2::Function::Ptr>& clicked() {
        return clicked_;
    }

    // Emitted when a row of the table is double clicked
    Wt::Signal<P2::Function::Ptr>& doubleClicked() {
        return doubleClicked_;
    }
    

private:
    void clickRow(const Wt::WModelIndex &idx) {
        if (idx.isValid()) {
            P2::Function::Ptr function = model_->functionAt(idx.row());
            clicked_.emit(function);
        }
    }

    void doubleClickRow(const Wt::WModelIndex &idx) {
        if (idx.isValid()) {
            P2::Function::Ptr function = model_->functionAt(idx.row());
            doubleClicked_.emit(function);
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
    static Wt::WText* field(std::vector<Wt::WText*> &fields, const std::string &toolTip) {
        Wt::WText *wValue = new Wt::WText();
        if (toolTip!="")
            wValue->setToolTip(toolTip);
        fields.push_back(wValue);
        return wValue;
    }

    WFunctionSummary(Context &ctx, Wt::WContainerWidget *parent=NULL)
        : Wt::WContainerWidget(parent), ctx_(ctx), table_(NULL), wName_(NULL), wEntry_(NULL), wBBlocks_(NULL), wDBlocks_(NULL),
          wInsns_(NULL), wBytes_(NULL), wDiscontig_(NULL), wCallers_(NULL), wCallsInto_(NULL), wCallees_(NULL),
          wCallsOut_(NULL), wRecursive_(NULL) {
        std::vector<Wt::WText*> fields;
        wName_      = field(fields, "Function name");
        wEntry_     = field(fields, "Primary entry virtual address");
        wBBlocks_   = field(fields, "Number of basic blocks");
        wDBlocks_   = field(fields, "Number of data blocks");
        wInsns_     = field(fields, "Number of instructions");
        wBytes_     = field(fields, "Number of distinct addresses with code and/or data");
        wDiscontig_ = field(fields, "Number of discontiguous regions in the address space");
        wCallers_   = field(fields, "Number of distinct functions that call this function");
        wCallsInto_ = field(fields, "Number of call sites calling this function");
        wCallees_   = field(fields, "Number of distinct functions this function calls.");
        wCallsOut_  = field(fields, "Number of function call sites in this function");
        wRecursive_ = field(fields, "Does this function call itself directly?");

        table_ = new Wt::WTable(this);
        table_->setWidth("100%");
        static const size_t ncols = 4;
        size_t nrows = (fields.size()+ncols-1) / ncols;
        for (size_t col=0, i=0; col<ncols; ++col) {
            for (size_t row=0; row<nrows && i<fields.size(); ++row)
                table_->elementAt(row, col)->addWidget(fields[i++]);
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
            wName_      ->setText(function->name()==""?std::string("(no name)"):function->name());
            wEntry_     ->setText("entry "+StringUtility::addrToString(function->address()));
            wBBlocks_   ->setText(StringUtility::plural(function->basicBlockAddresses().size(), "basic blocks"));
            wDBlocks_   ->setText(StringUtility::plural(function->dataBlocks().size(), "data blocks"));
            wInsns_     ->setText(StringUtility::plural(nInsns, "instructions"));
            wBytes_     ->setText(StringUtility::plural(nBytes, "bytes"));
            wDiscontig_ ->setText(StringUtility::plural(nIntervals, "contiguous intervals"));
            wCallers_   ->setText(StringUtility::plural(cg->nCallers(function), "distinct callers"));
            wCallsInto_ ->setText(StringUtility::plural(cg->nCallsIn(function), "calls")+" incoming");
            wCallees_   ->setText(StringUtility::plural(cg->nCallees(function), "distinct callees"));
            wCallsOut_  ->setText(StringUtility::plural(cg->nCallsOut(function), "calls")+" outgoing");
            wRecursive_ ->setText(cg->nCalls(function, function)?"recursive":"non-recursive");
            table_->show();
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
            table_->hide();
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
    typedef std::pair<Wt::WRectArea*, rose_addr_t> AreaAddr;
    std::vector<AreaAddr> areas_;
    Wt::Signal<P2::BasicBlock::Ptr> basicBlockClicked_;
    Wt::Signal<P2::Function::Ptr> functionClicked_;
public:
    WFunctionCfg(Context &ctx, Wt::WContainerWidget *parent=NULL)
        : Wt::WContainerWidget(parent), ctx_(ctx), wImage_(NULL) {
        wMessage_ = new Wt::WText("No function.", this);
    }

    // Returns currently displayed function
    P2::Function::Ptr function() const {
        return function_;
    }

    // Display information for the specified function
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
        wImage_->setImageLink(Wt::WLink(imagePath.string()));

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
                    // Gecko agents (e.g., firefox) don't like multi-line tooltips--they rewrap the tip as they see fit.
                    // Therefore, on such agents just indicate the number of instructions.  For other agents, create a tooltip
                    // with up to 10 lines listing the instructions.
                    if (ctx_.application->environment().agentIsGecko()) {
                        area->setToolTip(StringUtility::plural(bblock->nInstructions(), "instructions"));
                    } else {
                        std::string toolTip;
                        bool exitEarly = bblock->nInstructions()>10;
                        for (size_t i=0; (i<9 || (!exitEarly && i<10)) && i<bblock->nInstructions(); ++i)
                            toolTip += (i?"\n":"") + unparseInstruction(bblock->instructions()[i]);
                        if (bblock->nInstructions()>10)
                            toolTip += "\nand " + StringUtility::numberToString(bblock->nInstructions()-9) + " more...";
                        area->setToolTip(toolTip);
                    }
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

    // Emitted when a basic block vertex is clicked.
    Wt::Signal<P2::BasicBlock::Ptr>& basicBlockClicked() {
        return basicBlockClicked_;
    }

    // Emitted when a function vertex is clicked.
    Wt::Signal<P2::Function::Ptr>& functionClicked() {
        return functionClicked_;
    }
    
private:
    void selectBasicBlock(const Wt::WMouseEvent &event) {
        BOOST_FOREACH (const AreaAddr &pair, areas_) {
            Wt::WRectArea *area = pair.first;
            if (event.widget().x >= area->x() && event.widget().x <= area->x() + area->width() &&
                event.widget().y >= area->y() && event.widget().y <= area->y() + area->height()) {
                P2::BasicBlock::Ptr bblock = ctx_.partitioner.basicBlockExists(pair.second);
                if (bblock)
                    basicBlockClicked_.emit(bblock);
                return;
            }
        }
    }

    void selectFunction(const Wt::WMouseEvent &event) {
        BOOST_FOREACH (const AreaAddr &pair, areas_) {
            Wt::WRectArea *area = pair.first;
            if (event.widget().x >= area->x() && event.widget().x <= area->x() + area->width() &&
                event.widget().y >= area->y() && event.widget().y <= area->y() + area->height()) {
                P2::Function::Ptr function = ctx_.partitioner.functionExists(pair.second);
                if (function)
                    functionClicked_.emit(function);
                return;
            }
        }
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Model holding instructions

class InstructionListModel: public Wt::WAbstractTableModel {
    Context &ctx_;
    std::vector<SgAsmInstruction*> insns_;
public:
    enum Column {
        C_ADDR,                                         // instruction address
        C_BYTES,                                        // instruction bytes in hexadecimal
        C_CHARS,                                        // instruction printable characters
        C_NAME,                                         // instruction mnemonic
        C_ARGS,                                         // instruction operands
        C_COMMENT,                                      // arbitrary text
        C_NCOLS                                         // MUST BE LAST
    };

    explicit InstructionListModel(Context &ctx): ctx_(ctx) {}

    void changeInstructions(const std::vector<SgAsmInstruction*> &insns) {
        layoutAboutToBeChanged().emit();
        insns_ = insns;
        layoutChanged().emit();
    }

    void clear() {
        layoutAboutToBeChanged().emit();
        insns_.clear();
        layoutChanged().emit();
    }
    
    virtual int rowCount(const Wt::WModelIndex &parent=Wt::WModelIndex()) const ROSE_OVERRIDE {
        return parent.isValid() ? 0 : insns_.size();
    }

    virtual int columnCount(const Wt::WModelIndex &parent=Wt::WModelIndex()) const ROSE_OVERRIDE {
        return parent.isValid() ? 0 : C_NCOLS;
    }

    virtual boost::any headerData(int column, Wt::Orientation orientation=Wt::Horizontal,
                                  int role=Wt::DisplayRole) const ROSE_OVERRIDE {
        if (Wt::Horizontal == orientation && Wt::DisplayRole == role) {
            switch (column) {
                case C_ADDR:    return Wt::WString("Address");
                case C_BYTES:   return Wt::WString("Bytes");
                case C_CHARS:   return Wt::WString("ASCII");
                case C_NAME:    return Wt::WString("Mnemonic");
                case C_ARGS:    return Wt::WString("Operands");
                case C_COMMENT: return Wt::WString("Comments");
                default:        return boost::any();
            }
        }
        return boost::any();
    }

    virtual boost::any data(const Wt::WModelIndex &index, int role=Wt::DisplayRole) const ROSE_OVERRIDE {
        ASSERT_require(index.isValid());
        ASSERT_require(index.row()>=0 && (size_t)index.row() < insns_.size());
        SgAsmInstruction *insn = insns_[index.row()];
        if (Wt::DisplayRole == role) {
            switch (index.column()) {
                case C_ADDR: {
                    return Wt::WString(StringUtility::addrToString(insn->get_address()));
                }
                case C_BYTES: {
                    std::string s;
                    for (size_t i=0; i<insn->get_raw_bytes().size(); ++i) {
                        uint8_t byte = insn->get_raw_bytes()[i];
                        char buf[32];
                        sprintf(buf, "%02x", byte);
                        s += std::string(i?" ":"") + buf;
                    }
                    return Wt::WString(s);
                }
                case C_CHARS: {
                    std::string s;
                    for (size_t i=0; i<insn->get_raw_bytes().size(); ++i) {
                        char ch = insn->get_raw_bytes()[i];
                        s += std::string(i?" ":"") + (isgraph(ch) ? std::string(1, ch) : std::string(" "));
                    }
                    return Wt::WString(s);
                }
                case C_NAME: {
                    return Wt::WString(unparseMnemonic(insn));
                }
                case C_ARGS: {
                    std::string s;
                    const RegisterDictionary *regs = ctx_.partitioner.instructionProvider().registerDictionary();
                    const SgAsmExpressionPtrList &operands = insn->get_operandList()->get_operands();
                    for (size_t i=0; i<operands.size(); ++i)
                        s += (i?", ":"") + unparseExpression(operands[i], NULL, regs);
                    return Wt::WString(s);
                }
                case C_COMMENT: {
                    return Wt::WString(insn->get_comment());
                }
                default:        ASSERT_not_reachable("invalid column number");
            }
        }
        return boost::any();
    }
};

class WInstructionList: public Wt::WContainerWidget {
    InstructionListModel *model_;
    Wt::WTableView *tableView_;
public:
    WInstructionList(InstructionListModel *model, Wt::WContainerWidget *parent=NULL)
        : Wt::WContainerWidget(parent), model_(model) {
        ASSERT_not_null(model);
        tableView_ = new Wt::WTableView(this);
        tableView_->setModel(model_);
        tableView_->setRowHeaderCount(1);               // this must be first property set
        tableView_->setHeaderHeight(28);
        tableView_->setSortingEnabled(false);
        tableView_->setAlternatingRowColors(true);
        tableView_->setColumnWidth(InstructionListModel::C_ADDR,     Wt::WLength(6, Wt::WLength::FontEm));
        tableView_->setColumnWidth(InstructionListModel::C_BYTES,    Wt::WLength(12, Wt::WLength::FontEm));
        tableView_->setColumnWidth(InstructionListModel::C_CHARS,    Wt::WLength(4, Wt::WLength::FontEm));
        tableView_->setColumnWidth(InstructionListModel::C_NAME,     Wt::WLength(5, Wt::WLength::FontEm));
        tableView_->setColumnWidth(InstructionListModel::C_ARGS,     Wt::WLength(30, Wt::WLength::FontEm));
        tableView_->setColumnWidth(InstructionListModel::C_COMMENT,  Wt::WLength(50, Wt::WLength::FontEm));
        tableView_->setColumnResizeEnabled(true);
        tableView_->setSelectionMode(Wt::SingleSelection);
        tableView_->setEditTriggers(Wt::WAbstractItemView::NoEditTrigger);
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The instructions for a basic block
class WBasicBlockInsns: public Wt::WContainerWidget {
    P2::BasicBlock::Ptr bblock_;                        // currently displayed basic block
    InstructionListModel *model_;
    WInstructionList *insnList_;
public:
    WBasicBlockInsns(Context &ctx, Wt::WContainerWidget *parent=NULL)
        : Wt::WContainerWidget(parent), model_(NULL), insnList_(NULL) {
        model_ = new InstructionListModel(ctx);
        insnList_ = new WInstructionList(model_, this);
    }

    void changeBasicBlock(const P2::BasicBlock::Ptr &bblock) {
        if (bblock == bblock_)
            return;
        bblock_ = bblock;
        if (NULL==bblock) {
            model_->clear();
            return;
        }
        model_->changeInstructions(bblock->instructions());
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Summary information about a basic block
class WBasicBlockSummary: public Wt::WContainerWidget {
    Context &ctx_;
    P2::BasicBlock::Ptr bblock_;
    Wt::WTable *table_;
    Wt::WText *wHasOpaquePredicates_;                   // FIXME[Robb P. Matzke 2014-09-15]: should be a bb list
    Wt::WText *wDataBlocks_;                            // FIXME[Robb P. Matzke 2014-09-15]: should be a db list
    Wt::WText *wIsFunctionCall_;                        // does this block look like a function call?
    Wt::WText *wIsFunctionReturn_;                      // does this block appear to return from a function call?
    Wt::WText *wStackDelta_;                            // symbolic expression for the block's stack delta
public:
    static Wt::WText* field(std::vector<Wt::WText*> &fields, const std::string &toolTip) {
        Wt::WText *wValue = new Wt::WText();
        if (toolTip!="")
            wValue->setToolTip(toolTip);
        fields.push_back(wValue);
        return wValue;
    }

    WBasicBlockSummary(Context &ctx, Wt::WContainerWidget *parent=NULL)
        : Wt::WContainerWidget(parent), ctx_(ctx), table_(NULL),
          wHasOpaquePredicates_(NULL), wDataBlocks_(NULL), wIsFunctionCall_(NULL), wIsFunctionReturn_(NULL),
          wStackDelta_(NULL) {
        std::vector<Wt::WText*> fields;
        wHasOpaquePredicates_   = field(fields, "Block has conditional branches that are never taken?");
        wDataBlocks_            = field(fields, "Number of data blocks owned by this basic block.");
        wIsFunctionCall_        = field(fields, "Does this block appear to call a function?");
        wIsFunctionReturn_      = field(fields, "Does this block appear to return from a function call?");
        wStackDelta_            = field(fields, "Stack pointer delta within this one basic block.");

        table_ = new Wt::WTable(this);
        table_->setWidth("100%");
        static const size_t ncols = 1;
        size_t nrows = (fields.size()+ncols-1) / ncols;
        for (size_t col=0, i=0; col<ncols; ++col) {
            for (size_t row=0; row<nrows && i<fields.size(); ++row)
                table_->elementAt(row, col)->addWidget(fields[i++]);
        }
    }

    void changeBasicBlock(const P2::BasicBlock::Ptr &bblock) {
        if (bblock == bblock_)
            return;
        bblock_ = bblock;
        if (bblock) {
            std::set<rose_addr_t> ghostSuccessors = ctx_.partitioner.basicBlockGhostSuccessors(bblock);
            wHasOpaquePredicates_->setText(StringUtility::plural(ghostSuccessors.size(), "opaque predicates"));

            wDataBlocks_->setText(StringUtility::plural(bblock->dataBlocks().size(), "data blocks"));

            wIsFunctionCall_->setText(ctx_.partitioner.basicBlockIsFunctionCall(bblock) ?
                                      "appears to have function call semantics" : "lacks function call semantics");

            wIsFunctionReturn_->setText(ctx_.partitioner.basicBlockIsFunctionReturn(bblock) ?
                                        "appears to be a function return" : "lacks function return semantics");

            InstructionSemantics2::BaseSemantics::SValuePtr stackDelta = ctx_.partitioner.basicBlockStackDelta(bblock);
            if (stackDelta!=NULL) {
                std::ostringstream stackDeltaStream;
                stackDeltaStream <<*stackDelta;
                wStackDelta_->setText("stack delta: " + stackDeltaStream.str());
            } else {
                wStackDelta_->setText("stack delta not computed");
            }
            
            table_->show();
        } else {
            wHasOpaquePredicates_->setText("");
            wStackDelta_->setText("");
            table_->hide();
        }
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// All information about basic blocks
class WBasicBlock: public Wt::WContainerWidget {
    P2::BasicBlock::Ptr bblock_;                        // currently displayed basic block
    WBasicBlockInsns *wInsns_;                          // instruction list for the basic block
    WBasicBlockSummary *wSummary_;                      // summary information for the basic block
public:
    WBasicBlock(Context &ctx, Wt::WContainerWidget *parent=NULL)
        : Wt::WContainerWidget(parent), wInsns_(NULL), wSummary_(NULL) {
        wInsns_ = new WBasicBlockInsns(ctx, this);
        wSummary_ = new WBasicBlockSummary(ctx, this);
    }

    void changeBasicBlock(const P2::BasicBlock::Ptr &bblock) {
        if (bblock == bblock_)
            return;
        bblock_ = bblock;
        wSummary_->changeBasicBlock(bblock);
        wInsns_->changeBasicBlock(bblock);
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// All information about functions, CFGs, basic blocks, instructions...
class WFunctions: public Wt::WContainerWidget {
public:
    enum Tab { LIST_TAB, CFG_TAB, BB_TAB };
private:
    Context ctx_;
    Wt::WTabWidget *wTabs_;                             // function list; function details
    Wt::WContainerWidget *wListTab_, *wCfgTab_, *wBbTab_; // tabs in the tab widget
    WFunctionList *wFunctionList_;                      // list of all functions
    WFunctionSummary *wSummary1_, *wSummary2_;          // function summary info (in two different tabs)
    WFunctionCfg *wFunctionCfg_;                        // control flow graph
    WBasicBlock *wBasicBlock_;                          // basic block display
    P2::Function::Ptr function_;                        // current function
    P2::BasicBlock::Ptr bblock_;                        // current basic block
public:
    WFunctions(Context &ctx, Wt::WContainerWidget *parent=NULL)
        : Wt::WContainerWidget(parent), ctx_(ctx), wTabs_(NULL), wListTab_(NULL), wCfgTab_(NULL), wBbTab_(NULL),
          wFunctionList_(NULL), wSummary1_(NULL), wSummary2_(NULL), wFunctionCfg_(NULL), wBasicBlock_(NULL) {

        Wt::WVBoxLayout *vbox = new Wt::WVBoxLayout;
        setLayout(vbox);

        vbox->addWidget(wTabs_ = new Wt::WTabWidget());
        wTabs_->currentChanged().connect(this, &WFunctions::setCurrentTab);

        // Function list tab.
        ASSERT_require(LIST_TAB==0);
        wListTab_ = new Wt::WContainerWidget(this);
        wListTab_->hide();                              // working around bug with all tabs' widges visible on creation
        Wt::WVBoxLayout *wListTabLayout = new Wt::WVBoxLayout;
        wListTab_->setLayout(wListTabLayout);
        wListTabLayout->addWidget(wFunctionList_ = new WFunctionList(new FunctionListModel(ctx_)), 1);
        wFunctionList_->clicked().connect(this, &WFunctions::showFunctionSummary);
        wFunctionList_->doubleClicked().connect(this, &WFunctions::showFunctionCfg);
        wListTabLayout->addWidget(wSummary1_ = new WFunctionSummary(ctx_));
        wTabs_->addTab(wListTab_, "Functions");

        // CFG tab
        ASSERT_require(CFG_TAB==1);
        wCfgTab_ = new Wt::WContainerWidget(this);
        wCfgTab_->hide();
        Wt::WVBoxLayout *wCfgTabLayout = new Wt::WVBoxLayout;
        wCfgTab_->setLayout(wCfgTabLayout);
        wCfgTabLayout->addWidget(new Wt::WBreak);
        wCfgTabLayout->addWidget(wSummary2_ = new WFunctionSummary(ctx_));
        Wt::WScrollArea *sa = new Wt::WScrollArea;
        wCfgTabLayout->addWidget(sa, 1);
        sa->setWidget(wFunctionCfg_ = new WFunctionCfg(ctx_));
        wFunctionCfg_->functionClicked().connect(this, &WFunctions::showFunctionCfg);
        wFunctionCfg_->basicBlockClicked().connect(this, &WFunctions::showBasicBlock);
        wTabs_->addTab(wCfgTab_, "CFG");

        // Basic block tab
        ASSERT_require(BB_TAB==2);
        wBbTab_ = new Wt::WContainerWidget(this);
        wBbTab_->hide();
        Wt::WVBoxLayout *wBbTabLayout = new Wt::WVBoxLayout;
        wBbTab_->setLayout(wBbTabLayout);
        wBbTabLayout->addWidget(wBasicBlock_ = new WBasicBlock(ctx_));
        wBbTabLayout->addWidget(new Wt::WText, 1);
        wTabs_->addTab(wBbTab_, "Block");

        setCurrentTab(LIST_TAB);

    }

    void setCurrentTab(int idx) {
        switch (idx) {
            case LIST_TAB:
                wSummary1_->changeFunction(function_);
                wSummary1_->setHidden(function_==NULL);
                break;
            case CFG_TAB:
                wSummary2_->changeFunction(function_);
                wFunctionCfg_->changeFunction(function_);
                wFunctionCfg_->setHidden(function_==NULL);
                break;
            case BB_TAB:
                wBasicBlock_->changeBasicBlock(bblock_);
                break;
        }

        // When changing away from the CFG_TAB and our current function is something other than what that tab is displaying,
        // hide the CFG image so it doesn't flash an old one when we change back to the tab. The flashing would happen because
        // we don't generate and update the image, which is expensive, until the last possible moment.
        if (idx!=CFG_TAB)
            wFunctionCfg_->setHidden(function_ != wFunctionCfg_->function());
        wTabs_->setCurrentIndex(idx);
        wTabs_->widget(idx)->show();
    }
    
private:
    // Show the function list and summary of selected function
    void showFunctionSummary(const P2::Function::Ptr &function) {
        function_ = function;
        bblock_ = P2::BasicBlock::Ptr();
        setCurrentTab(LIST_TAB);
    }

    // display function details
    void showFunctionCfg(const P2::Function::Ptr &function) {
        function_ = function;
        bblock_ = P2::BasicBlock::Ptr();
        setCurrentTab(CFG_TAB);
    }

    // display basic block details
    void showBasicBlock(const P2::BasicBlock::Ptr &bblock) {
        bblock_ = bblock;
        setCurrentTab(BB_TAB);
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Application: public Wt::WApplication {
    Context ctx_;
    Wt::WGridLayout *wGrid_;
    WFunctions *wFunctions_;
public:
    Application(P2::Partitioner &partitioner, const Wt::WEnvironment &env)
        : Wt::WApplication(env), ctx_(partitioner, this),  wFunctions_(NULL) {
        setTitle("bROwSE");
        setCssTheme("polished");


        root()->setLayout(wGrid_ = new Wt::WGridLayout());
        wGrid_->setRowStretch(1, 1);
        wGrid_->setColumnStretch(1, 1);

        Wt::WImage *compassRose = new Wt::WImage("/images-nonfree/compassrose.jpg");
        wGrid_->addWidget(compassRose, 0, 0);

        wFunctions_ = new WFunctions(ctx_);
        wGrid_->addWidget(wFunctions_, 1, 1);

#if 1 // DEBUGGING [Robb P. Matzke 2014-09-12]
        wGrid_->addWidget(new Wt::WText("North"), 0, 1);
        wGrid_->addWidget(new Wt::WText("NE"),    0, 2);
        wGrid_->addWidget(new Wt::WText("West"),  1, 0);
        wGrid_->addWidget(new Wt::WText("East"),  1, 2);
        wGrid_->addWidget(new Wt::WText("SW"),    2, 0);
        wGrid_->addWidget(new Wt::WText("South"), 2, 1);
        wGrid_->addWidget(new Wt::WText("SE"),    2, 2);
#endif
        
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

    // Start the web server
    int wtArgc = 0;
    char *wtArgv[8];
    wtArgv[wtArgc++] = strdup(argv[0]);
    wtArgv[wtArgc++] = strdup("--docroot");
    wtArgv[wtArgc++] = strdup(settings.docRoot.c_str());
    wtArgv[wtArgc++] = strdup("--http-address");
    wtArgv[wtArgc++] = strdup(settings.httpAddress.c_str());
    wtArgv[wtArgc++] = strdup("--http-port");
    wtArgv[wtArgc++] = strdup(boost::lexical_cast<std::string>(settings.httpPort).c_str());
    wtArgv[wtArgc] = NULL;
    ApplicationCreator appCreator(partitioner);
    Wt::WRun(wtArgc, wtArgv, appCreator);

    exit(0);
}

