#include <rose.h>
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
#include <boost/foreach.hpp>
#include <boost/graph/graphviz.hpp>

#include <Wt/WAbstractTableModel>
#include <Wt/WApplication>
#include <Wt/WContainerWidget>
#include <Wt/WString>
#include <Wt/WTableView>


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
        .purpose("tests new partitioner architecture")
        .doc("synopsis",
             "@prop{programName} [@v{switches}] @v{specimen_name}")
        .doc("description",
             "This program tests the new partitioner architecture by disassembling the specified file.");
    
    return parser.with(gen).with(dis).with(server).parse(argc, argv).apply();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

P2::Attribute::Id ATTR_FUNCTION_SIZE(-1);


class Context {
public:
    P2::Partitioner &partitioner;

    Context(P2::Partitioner &partitioner): partitioner(partitioner) {
        if (ATTR_FUNCTION_SIZE == P2::Attribute::INVALID_ID) {
            ATTR_FUNCTION_SIZE = P2::Attribute::registerName("Function size in bytes");
        }
    }
};

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
        BOOST_FOREACH (const P2::Function::Ptr &function, ctx.partitioner.functions()) {
            size_t nBytes;                              // cached size of function in bytes (code + data)
            if (!function->attr<size_t>(ATTR_FUNCTION_SIZE).assignTo(nBytes)) {
                nBytes = ctx.partitioner.functionExtent(function).size();
                function->attr(ATTR_FUNCTION_SIZE, nBytes);
            }
            functions_.push_back(function);
        }
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
                    return function->attr<size_t>(ATTR_FUNCTION_SIZE).orElse(0);
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
        return a->attr<size_t>(ATTR_FUNCTION_SIZE).orElse(0) < b->attr<size_t>(ATTR_FUNCTION_SIZE).orElse(0);
    }
    static bool sortByDescendingSize(const P2::Function::Ptr &a, const P2::Function::Ptr &b) {
        return a->attr<size_t>(ATTR_FUNCTION_SIZE).orElse(0) > b->attr<size_t>(ATTR_FUNCTION_SIZE).orElse(0);
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

class WFunctionList: public Wt::WContainerWidget {
    FunctionListModel *model_;
    Wt::WTableView *tableView_;
public:
    WFunctionList(Context &ctx, Wt::WContainerWidget *parent=NULL)
        : Wt::WContainerWidget(parent), model_(NULL) {
        model_ = new FunctionListModel(ctx);
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
        tableView_->setSelectionMode(Wt::ExtendedSelection);
        tableView_->setEditTriggers(Wt::WAbstractItemView::NoEditTrigger);
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Application: public Wt::WApplication {
public:
    Application(Context &ctx, const Wt::WEnvironment &env): Wt::WApplication(env) {
        setTitle("bROwSE");
        setCssTheme("polished");
        new WFunctionList(ctx, root());
    }
};

class ApplicationCreator {
    Context ctx_;
public:
    ApplicationCreator(P2::Partitioner &partitioner): ctx_(partitioner) {}
    Wt::WApplication* operator()(const Wt::WEnvironment &env) {
        return new Application(ctx_, env);
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

