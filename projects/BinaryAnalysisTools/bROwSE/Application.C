#include <rose.h>
#include <bROwSE/Application.h>

#include <bROwSE/WAssemblyListing.h>
#include <bROwSE/WBusy.h>
#include <bROwSE/WCrossReferences.h>
#include <bROwSE/WFunctionCfg.h>
#include <bROwSE/WFunctionList.h>
#include <bROwSE/WFunctionSummary.h>
#include <bROwSE/WHexDump.h>
#include <bROwSE/WMagic.h>
#include <bROwSE/WMemoryMap.h>
#include <bROwSE/WPartitioner.h>
#include <bROwSE/WSemantics.h>
#include <bROwSE/WSplash.h>
#include <bROwSE/WStatus.h>
#include <bROwSE/WStrings.h>
#include <CommandLine.h>                                // ROSE
#include <Disassembler.h>                               // ROSE
#include <Partitioner2/Engine.h>                        // ROSE
#include <Partitioner2/Modules.h>                       // ROSE
#include <Partitioner2/ModulesPe.h>                     // ROSE
#include <Partitioner2/Partitioner.h>                   // ROSE
#include <signal.h>
#include <Wt/WApplication>
#include <Wt/WBootstrapTheme>
#include <Wt/WContainerWidget>
#include <Wt/WGridLayout>
#include <Wt/WImage>
#include <Wt/WStackedWidget>
#include <Wt/WTabWidget>
#include <Wt/WText>
#include <Wt/WVBoxLayout>

int main(int argc, char *argv[]) {
    bROwSE::Application::main(argc, argv);
}





using namespace ::Rose;
using namespace ::Rose::BinaryAnalysis;
using namespace ::Rose::Diagnostics;

namespace bROwSE {

// Class method
Sawyer::CommandLine::ParserResult
Application::parseCommandLine(int argc, char *argv[], Settings &settings)
{
    using namespace Sawyer::CommandLine;

    // Generic switches
    SwitchGroup gen = Rose::CommandLine::genericSwitches();
    gen.insert(Switch("config")
               .argument("names", listParser(anyParser(settings.configurationNames), ":"))
               .whichValue(SAVE_ALL)
               .explosiveLists(true)
               .doc("Directories containing configuration files, or configuration files themselves.  A directory is searched "
                    "recursively searched for files whose names end with \".json\" or and each file is parsed and used to "
                    "to configure the partitioner.  The JSON file contents is defined by the Carnegie Mellon University "
                    "Software Engineering Institute. It should have a top-level \"config.exports\" table whose keys are "
                    "function names and whose values are have a \"function.delta\" integer. The delta does not include "
                    "popping the return address from the stack in the final RET instruction.  Function names of the form "
                    "\"lib:func\" are translated to the ROSE format \"func@@lib\"."));

    // Switches for disassembly
    SwitchGroup dis("Disassembly switches");
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

    server.insert(Switch("allow-downloads")
                  .intrinsicValue(true, settings.allowDownloads)
                  .doc("Allow parts of the binary specimen to be downloaded. The @s{no-allow-downloads} disables this "
                       "features. The default is to " + std::string(settings.allowDownloads?"":"not ") + "allow downloads."));
    server.insert(Switch("no-allow-downloads")
                  .key("allow-downloads")
                  .intrinsicValue(false, settings.allowDownloads)
                  .hidden(true));
                  

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

// Functor to create a new application on first connection to server
class AppCreator {
    Settings settings;                                  // settings from the command-line
    std::vector<std::string> specimenNames_;
public:
    explicit AppCreator(const Settings &settings, const std::vector<std::string> &specimenNames)
        : settings(settings), specimenNames_(specimenNames) {}
    Wt::WApplication* operator()(const Wt::WEnvironment &env) {
        return new Application(settings, specimenNames_, env);
    }
};

// Class method
void
Application::main(int argc, char *argv[]) {
    // Initialize diagnostics. The destination is usually just stderr, but we turn it into a multiplexer so that we can add our
    // own message sinks to the list later.
    Diagnostics::destination = Sawyer::Message::Multiplexer::instance()
                               ->to(Sawyer::Message::FileSink::instance(stderr));
    ROSE_INITIALIZE;
    Diagnostics::initAndRegister(&mlog, "bROwSE");

    // Parse the command-line
    Settings settings;
    std::vector<std::string> specimenNames = Application::parseCommandLine(argc, argv, settings /*out*/).unreachedArgs();
    if (specimenNames.empty())
        throw std::runtime_error("no specimen specified; see --help");

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
    AppCreator ac(settings, specimenNames);
    Wt::WRun(wtArgc, wtArgv, ac);

    exit(0);
}

// This probably isn't the right way to do this, but we need some method for killing the built-in server. Calling exit doesn't
// always work.
static void
killServer() {
    raise(SIGKILL);
    exit(0);
}

void
Application::init() {
    setTitle("bROwSE");

    // This looks nicer than setCssTheme("polished"), but requires some minor CSS adjustments:
    //  + IMG should not be restricted to 100% available width, otherwise CFGs will be scaled down by the browser to a point
    //     which might be unreadable.
    setTheme(new Wt::WBootstrapTheme);

    // Hexdump style sheet rules. The colors here are the same as those used by WAddressSpace.
    styleSheet().addRule(".hexdump_evenrow", "font-family:monospace;");
    styleSheet().addRule(".hexdump_oddrow", "font-family:monospace; background-color:#f9f9f9;");
    styleSheet().addRule(".hexdump_unmapped", "background-color:black;");
    styleSheet().addRule(".hexdump_nochar", "background-color:lightgray;");
    styleSheet().addRule(".hexdump_addr_none", "font-family:monospace;"
                         " background-color:" + toHtml(darken(Color::HSV_RED, 0.25)) + ";");
    styleSheet().addRule(".hexdump_addr_r", "font-family:monospace;"
                         "background-color:" + toHtml(Color::RGB(0.9, 0.8, 0)) + ";");
    styleSheet().addRule(".hexdump_addr_w", "font-family:monospace;"
                         " background-color:" + toHtml(fade(darken(Color::HSV_RED, 0.25), 0.75)) + ";");
    styleSheet().addRule(".hexdump_addr_x", "font-family:monospace;"
                         " background-color:" + toHtml(darken(Color::HSV_GREEN, 0.15)) + ";");
    styleSheet().addRule(".hexdump_addr_rw", "font-family:monospace;"
                         " background-color:" + toHtml(fade(Color::RGB(0.9, 0.8, 0), 0.75)) + ";");
    styleSheet().addRule(".hexdump_addr_rx", "font-family:monospace;"
                         " background-color:" + toHtml(darken(Color::HSV_GREEN, 0.15)) + ";"); // same as execute-only
    styleSheet().addRule(".hexdump_addr_wx", "font-family:monospace;"
                         " background-color:" + toHtml(fade(darken(Color::HSV_GREEN, 0.15), 0.75)) + ";");
    styleSheet().addRule(".hexdump_addr_rwx", "font-family:monospace;"
                         " background-color:" + toHtml(fade(darken(Color::HSV_GREEN, 0.15), 0.75)) + ";");

    // Status message style sheet rules.
    styleSheet().addRule(".status_oddrow", "background-color:#f9f9f9;");
    styleSheet().addRule(".status_info", "background-color:#b5ffb3;");// light green
    styleSheet().addRule(".status_warn", "background-color:#f8ff81;");// light yellow
    styleSheet().addRule(".status_error", "background-color:#ffd781;");// light orange
    styleSheet().addRule(".status_fatal", "background-color:#ff8181;");// light red

    // Strings table style sheet rules
    styleSheet().addRule(".strings_oddrow", "background-color:#f9f9f9;");
    styleSheet().addRule(".strings_matched", "background-color:#fff195;");// light yellow

    // So other threads can obtain a lock, modify the DOM, and then call triggerUpdate.
    enableUpdates(true);

    // The application has mutually exclusive phases that are children of a stacked widget
    Wt::WVBoxLayout *vbox = new Wt::WVBoxLayout;
    root()->setLayout(vbox);
    wStacked_ = new Wt::WStackedWidget;
    vbox->addWidget(wStacked_);

    //--------------
    // Splash phase
    //--------------
    WSplash *wSplash = new WSplash;
    wSplash->clicked().connect(boost::bind(&Wt::WStackedWidget::setCurrentIndex, wStacked_, InteractivePhase));
    ASSERT_require(wStacked_->count() == SplashPhase);
    wStacked_->addWidget(wSplash);
    wStacked_->setCurrentIndex(SplashPhase);

    //------------ 
    // Busy phase
    //------------
    ctx_.busy = new WBusy(ctx_);
    ctx_.busy->workStarted().connect(boost::bind(&Wt::WStackedWidget::setCurrentIndex, wStacked_, BusyPhase));
    ctx_.busy->workFinished().connect(boost::bind(&Wt::WStackedWidget::setCurrentIndex, wStacked_, InteractivePhase));
    ASSERT_require(wStacked_->count() == BusyPhase);
    wStacked_->addWidget(ctx_.busy);

    //-------------------
    // Interactive phase
    //-------------------
    wInteractivePhase_ = new Wt::WContainerWidget;
    ASSERT_require(wStacked_->count() == InteractivePhase);
    wStacked_->addWidget(wInteractivePhase_);

    // Grid layout
    wInteractivePhase_->setLayout(wGrid_ = new Wt::WGridLayout);
    wGrid_->setRowStretch(1, 1);
    wGrid_->setColumnStretch(1, 1);

    // Logo (reset button)
    // FIXME[Robb P. Matzke 2014-12-27]: eventually this should be a reset button
    Wt::WImage *wLogo = new Wt::WImage("/images/logo.png");
    wLogo->setToolTip("Click here to reset the server");
    wLogo->clicked().connect(boost::bind(killServer));
    wLogo->resize(32, 32);
    wGrid_->addWidget(wLogo, 0, 0);
    wGrid_->addWidget(new Wt::WText("bROwSE: Binary ROSE On-line Workbench for Specimen Exploration"), 0, 1);

    // The central region the page is a set of tabs that are visible or not depending on the context
    wGrid_->addWidget(instantiateMainTabs(), 1, 1);

    // East side is a tool pane
    Wt::WContainerWidget *rightPane = new Wt::WContainerWidget;
    wGrid_->addWidget(rightPane, 1, 2);

    wSemantics_ = new WSemantics(ctx_, rightPane);

    wCrossRefs_ = new WCrossReferences(rightPane);
    wCrossRefs_->referenceClicked().connect(boost::bind(&Application::gotoReference, this, _1));

    // The bottom center is the status area
    wStatusBar_ = new WStatusBar;
    wStatus_->messageArrived().connect(boost::bind(&WStatusBar::appendMessage, wStatusBar_, _1));
    wGrid_->addWidget(wStatusBar_, 2, 1);

    //---------
    // Startup
    //---------
    
    wPartitioner_->memoryMapProvider(wMemoryMap_);
    showHideTabs();
}

Wt::WContainerWidget*
Application::instantiateMainTabs() {
    Wt::WContainerWidget *mainTabContainer = new Wt::WContainerWidget;
    Wt::WVBoxLayout *vbox = new Wt::WVBoxLayout;
    mainTabContainer->setLayout(vbox);

    wMainTabs_ = new Wt::WTabWidget;
    wMainTabs_->currentChanged().connect(boost::bind(&Application::changeTab2, this, _1));
    vbox->addWidget(wMainTabs_, 1 /*stretch*/);

    for (size_t i=0; i<NMainTabs; ++i) {
        Wt::WString tabName;
        Wt::WContainerWidget *tabContent = NULL;
        switch ((MainTab)i) {
            case PartitionerTab: {
                tabName = "Partitioner";
                tabContent = wPartitioner_ = new WPartitioner(ctx_);
                wPartitioner_->specimenParsed().connect(boost::bind(&Application::handleSpecimenParsed, this, _1));
                wPartitioner_->specimenLoaded().connect(boost::bind(&Application::handleSpecimenLoaded, this, _1));
                wPartitioner_->specimenPartitioned().connect(boost::bind(&Application::handleSpecimenPartitioned, this, _1));
                break;
            }
            case MemoryMapTab: {
                tabName = "Memory Map";
                tabContent = wMemoryMap_ = new WMemoryMap;
                wMemoryMap_->mapChanged().connect(boost::bind(&Application::memoryMapChanged, this));
                wMemoryMap_->allowDownloads(ctx_.settings.allowDownloads);
                break;
            }
            case FunctionListTab: {
                tabName = "Functions";
                tabContent = wFunctionList_ = new WFunctionList(ctx_);
                wFunctionList_->functionChanged().connect(boost::bind(&Application::changeFunction, this, _1));
                wFunctionList_->functionRowDoubleClicked()
                    .connect(boost::bind(&Application::changeFunctionDoubleClick, this, _1));
                break;
            }
            case FunctionSummaryTab: {
                tabName = "Summary";
                tabContent = wFunctionSummary_ = new WFunctionSummary(ctx_);
                break;
            }
            case FunctionCfgTab: {
                tabName = "CFG";
                tabContent = wFunctionCfg_ = new WFunctionCfg(ctx_);
                wFunctionCfg_->functionChanged().connect(boost::bind(&Application::changeFunction, this, _1));
                wFunctionCfg_->functionClicked().connect(boost::bind(&Application::changeFunction, this, _1));
                wFunctionCfg_->addressClicked().connect(boost::bind(&Application::showHexDumpAtAddress, this, _1));
                wFunctionCfg_->basicBlockClicked().connect(boost::bind(&Application::changeBasicBlock, this, _1));
                break;
            }
            case AssemblyTab: {
                tabName = "Assembly";
                tabContent = wAssembly_ = new WAssemblyListing(ctx_);
                break;
            }
            case HexDumpTab: {
                tabName = "Hexdump";
                tabContent = wHexDump_ = new WHexDump;
                wHexDump_->byteClicked().connect(boost::bind(&Application::updateAddressCrossReferences, this, _1));
                break;
            }
            case MagicTab: {
                tabName = "Magic";
                tabContent = wMagic_ = new WMagic;
                break;
            }
            case StringsTab: {
                tabName = "Strings";
                tabContent = wStrings_ = new WStrings;
                wStrings_->stringClicked().connect(boost::bind(&Application::updateStringCrossReferences, this, _1));
                break;
            }
            case StatusTab: {
                tabName = "Status";
                tabContent = wStatus_ = new WStatus(ctx_);
                break;
            }
            default:
                ASSERT_not_reachable("invalid main tab");
        }
        ASSERT_not_null(tabContent);
        ASSERT_forbid(tabName.empty());
        wMainTabs_->addTab(tabContent, tabName);
    }
    return mainTabContainer;
}

bool
Application::isTabAvailable(MainTab idx) {
    ASSERT_require(idx < NMainTabs);
    switch (idx) {
        case PartitionerTab:
            return true;
        case MemoryMapTab:
            return !wMemoryMap_->memoryMap()->isEmpty();
        case FunctionListTab:
            return (!ctx_.partitioner.isDefaultConstructed() && wFunctionList_->functions().size());
        case FunctionSummaryTab:
        case FunctionCfgTab:
        case AssemblyTab:
            return currentFunction_ != NULL;
        case HexDumpTab:
            return !wHexDump_->memoryMap()->isEmpty();
        case MagicTab:
            return !wMagic_->memoryMap()->isEmpty();
        case StringsTab:
            return !wStrings_->memoryMap()->isEmpty();
        case StatusTab:
            return true;
        default:
            ASSERT_not_reachable("invalid main tab");
    }
}

void
Application::showHideTabs() {
    Sawyer::Optional<size_t> someAvail;
    for (size_t i=0; i<NMainTabs; ++i) {
        bool avail = isTabAvailable((MainTab)i);
        wMainTabs_->setTabHidden(i, !avail);
        if (avail && (!someAvail || i<=(size_t)wMainTabs_->count()))
            someAvail = i;
    }

    // If current tab is now hidden we need to select some other
    if (wMainTabs_->isTabHidden(wMainTabs_->currentIndex())) {
        ASSERT_require2(someAvail, "all main tabs are hidden");
        wMainTabs_->setCurrentIndex(*someAvail);
    }

    showHideTools();
}

void
Application::showHideTools() {
    MainTab curTab = (MainTab)wMainTabs_->currentIndex();
    wSemantics_->setHidden(curTab != FunctionCfgTab && curTab != AssemblyTab);
    wCrossRefs_->setHidden(curTab != StringsTab && curTab != HexDumpTab);
}

void
Application::handleSpecimenParsed(bool done) {
    showHideTabs();
}

void
Application::handleSpecimenLoaded(bool done) {
    // Changing the WMemoryMap will cause it to emit a mapChanged signal, which is wired to our memoryMapChanged method.
    if (done) {
        wMemoryMap_->memoryMap(ctx_.engine.memoryMap());
    } else {
        wMemoryMap_->memoryMap(MemoryMap::instance());
    }
    showHideTabs();
}

void
Application::handleSpecimenPartitioned(bool done) {
    wMemoryMap_->isEditable(!done);                     // disallow memory map editing once the partitioner has run
    wFunctionList_->reload();
    wStrings_->partitioner(ctx_.partitioner);           // updates string-code cross references
    wHexDump_->partitioner(ctx_.partitioner);           // updates address cross references
    if (!done) 
        currentFunction_ = P2::Function::Ptr();
    showHideTabs();
}

// Called either when the current function is changed or when the user clicks on a tab.
void
Application::changeTab(MainTab tab) {
    // Update the child before we switch to it.  Some of these operations are expensive, which is why we've delayed them until
    // we're about to make the child visible.
    switch (tab) {
        case MemoryMapTab:
        case PartitionerTab:
        case HexDumpTab:
        case MagicTab:
            break;
        case FunctionListTab:
            wFunctionList_->changeFunction(currentFunction_);
            break;
        case FunctionSummaryTab:
            wFunctionSummary_->changeFunction(currentFunction_);
            wFunctionSummary_->show();
            break;
        case FunctionCfgTab:
            wFunctionCfg_->changeFunction(currentFunction_);
            wFunctionCfg_->show();
            break;
        case AssemblyTab:
            wAssembly_->changeFunction(currentFunction_);
            wAssembly_->show();
            break;
        case StringsTab:
            wStrings_->updateModelIfNecessary();
            wStrings_->show();
            break;
        case StatusTab:
            wStatus_->redraw();
            break;
        default:
            ASSERT_not_reachable("invalid main tab");
    }

    // When moving away from certain expensive tabs, hide the tab so stale info isn't shown when we return.
    wFunctionSummary_->setHidden(FunctionSummaryTab!=tab && wFunctionSummary_->function()!=currentFunction_);
    wFunctionCfg_->setHidden(FunctionCfgTab!=tab && wFunctionCfg_->function()!=currentFunction_);
    wAssembly_->setHidden(AssemblyTab!=tab && wAssembly_->function()!=currentFunction_);

    // redundant when user clicked on a tab, but not otherwise
    wMainTabs_->setCurrentIndex(tab);
    showHideTools();
}

void
Application::changeFunction(const P2::Function::Ptr &function) {
    // In order to keep this fast, we want to delay changing the current function in the children until that child is actually
    // displayed. See changeTab.
    if (function != currentFunction_) {
        currentFunction_ = function;
        showHideTabs();
        changeTab((MainTab)(wMainTabs_->currentIndex()));
    }
}

// called when a table row is double clicked in the WFunctionList widget.
void
Application::changeFunctionDoubleClick(const P2::Function::Ptr &function) {
    changeFunction(function);
    changeTab(FunctionCfgTab);
}

// This is called when the user edits the memory map or a new map is given to the WMemoryMap widget.
void
Application::memoryMapChanged() {
    wHexDump_->memoryMap(wMemoryMap_->memoryMap());
    wStrings_->memoryMap(wMemoryMap_->memoryMap());
    wMagic_->memoryMap(wMemoryMap_->memoryMap());
}

void
Application::showHexDumpAtAddress(rose_addr_t va) {
    wHexDump_->makeVisible(va);
    changeTab(HexDumpTab);
}

void
Application::changeBasicBlock(const P2::BasicBlock::Ptr &bb) {
    wSemantics_->changeBasicBlock(bb);
}

void
Application::updateStringCrossReferences(size_t stringIdx) {
    const P2::ReferenceSet &xrefs = wStrings_->crossReferences(stringIdx);
    wCrossRefs_->refs(xrefs);
    wCrossRefs_->name("String " + StringUtility::addrToString(wStrings_->string(stringIdx).address()));
}

void
Application::updateAddressCrossReferences(rose_addr_t va) {
    const P2::ReferenceSet &xrefs = wHexDump_->crossReferences(va);
    wCrossRefs_->refs(xrefs);
    wCrossRefs_->name("Address " + StringUtility::addrToString(va));
}

void
Application::gotoReference(const P2::Reference &ref) {
    if (P2::BasicBlock::Ptr bblock = ref.basicBlock()) {
        P2::Function::Ptr function = ref.function();
        if (function == NULL) {
            std::vector<P2::Function::Ptr> functions = ctx_.partitioner.functionsOwningBasicBlock(bblock);
            if (!functions.empty())
                function = functions[0];
        }
        if (function) {
            changeFunction(function);
            changeTab(FunctionSummaryTab);
        }
    }
}

} // namespace

