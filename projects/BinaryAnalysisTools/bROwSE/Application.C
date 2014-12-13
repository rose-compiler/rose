#include <bROwSE/Application.h>
#include <bROwSE/WFunctions.h>
#include <Disassembler.h>                               // ROSE
#include <Partitioner2/Engine.h>                        // ROSE
#include <Partitioner2/Modules.h>                       // ROSE
#include <Partitioner2/ModulesPe.h>                     // ROSE
#include <Partitioner2/Partitioner.h>                   // ROSE
#include <Wt/WApplication>
#include <Wt/WContainerWidget>
#include <Wt/WGridLayout>
#include <Wt/WImage>
#include <Wt/WText>

int main(int argc, char *argv[]) {
    bROwSE::Application::main(argc, argv);
}





using namespace ::rose;
using namespace ::rose::BinaryAnalysis;
using namespace ::rose::Diagnostics;

namespace bROwSE {

void
Application::init() {
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

Sawyer::CommandLine::ParserResult
Application::parseCommandLine(int argc, char *argv[], Settings &settings)
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

    gen.insert(Switch("config")
               .argument("name", anyParser(settings.configurationName))
               .doc("Directory containing configuration files, or a configuration file itself.  A directory is searched "
                    "recursively searched for files whose names end with \".json\" or and each file is parsed and used to "
                    "to configure the partitioner.  The JSON file contents is defined by the Carnegie Mellon University "
                    "Software Engineering Institute. It should have a top-level \"config.exports\" table whose keys are "
                    "function names and whose values are have a \"function.delta\" integer. The delta does not include "
                    "popping the return address from the stack in the final RET instruction.  Function names of the form "
                    "\"lib:func\" are translated to the ROSE format \"func@lib\"."));

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

    dis.insert(Switch("pe-scrambler")
               .argument("dispatcher_address", nonNegativeIntegerParser(settings.peScramblerDispatcherVa))
               .doc("Simulate the action of the PEScrambler dispatch function in order to rewrite CFG edges.  Any edges "
                    "that go into the specified @v{dispatcher_address} are immediately rewritten so they appear to go "
                    "instead to the function contained in the dispatcher table which normally immediately follows the "
                    "dispatcher function.  The dispatcher function is quite easy to find in a call graph because nearly "
                    "everything calls it -- it will likely have far and away more callers than anything else.  Setting the "
                    "address to zero disables this module (which is the default)."));

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

class AppCreator {
    P2::Partitioner &partitioner_;
public:
    AppCreator(P2::Partitioner &partitioner): partitioner_(partitioner) {}
    Wt::WApplication* operator()(const Wt::WEnvironment &env) {
        return new Application(partitioner_, env);
    }
};


void
Application::main(int argc, char *argv[]) {
    // Do this explicitly since librose doesn't do this automatically yet
    Diagnostics::initialize();

    // Parse the command-line
    P2::Engine engine;
    Settings settings;
    std::vector<std::string> specimenNames = Application::parseCommandLine(argc, argv, settings).unreachedArgs();
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
    if (settings.peScramblerDispatcherVa) {
        P2::ModulesPe::PeDescrambler::Ptr cb = P2::ModulesPe::PeDescrambler::instance(settings.peScramblerDispatcherVa);
        cb->nameKeyAddresses(partitioner);              // give names to certain PEScrambler things
        partitioner.basicBlockCallbacks().append(cb);
        partitioner.attachFunction(P2::Function::instance(settings.peScramblerDispatcherVa,
                                                          partitioner.addressName(settings.peScramblerDispatcherVa),
                                                          SgAsmFunction::FUNC_USERDEF)); 
    }
    if (!settings.configurationName.empty()) {
        Sawyer::Message::Stream info(mlog[INFO]);
        info <<"loading configuration files";
        size_t nItems = engine.configureFromFile(partitioner, settings.configurationName);
        info <<"; configured " <<StringUtility::plural(nItems, "items") <<"\n";
    }
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
    AppCreator ac(partitioner);
    Wt::WRun(wtArgc, wtArgv, ac);

    exit(0);
}

} // namespace

