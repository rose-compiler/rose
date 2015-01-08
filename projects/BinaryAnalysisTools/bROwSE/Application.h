#ifndef bROwSE_Application_H
#define bROwSE_Application_H

#include <bROwSE/bROwSE.h>

#include <Partitioner2/Partitioner.h>
#include <sawyer/CommandLine.h>
#include <Wt/WApplication>
#include <Wt/WEnvironment>

namespace bROwSE {

class Application: public Wt::WApplication {
public:
    enum Phase {
        SplashPhase,                                    /**< GUI is saying hello. */
        BusyPhase,                                      /**< GUI is waiting for some long-running task like partitioning. */
        InteractivePhase                                /**< GUI is in normal phase of interactive operation. */
    };

    /** The tabs in the central part of the main window. */
    enum MainTab {
        PartitionerTab,                                 /**< Partitioner configuration and actions. */
        MemoryMapTab,                                   /**< Description of specimen memory and its contents. */
        FunctionListTab,                                /**< List of all known functions. */
        FunctionSummaryTab,                             /**< Summary information for one function. */
        FunctionCfgTab,                                 /**< Control flow graph for one function. */
        AssemblyTab,                                    /**< More traditional assembly listing. */
        HexDumpTab,                                     /**< Dump raw data bytes from anywhere in memory. */
        StatusTab,                                      /**< Complete list of all messages from ROSE, system status, etc. */
        NMainTabs
    };

private:
    Context ctx_;                                       // many other objects have a reference to this
    Wt::WStackedWidget *wStacked_;                      // the main phases of the application

    // Interact phase
    Wt::WContainerWidget *wInteractivePhase_;
    Wt::WGridLayout *wGrid_;                            // layout for the application as a whole
    Wt::WTabWidget *wMainTabs_;                         // main tabs in the central region of the app
    WPartitioner *wPartitioner_;                        // configuration
    WMemoryMap *wMemoryMap_;                            // widget for editing the specimen's memory map
    WFunctionList *wFunctionList_;                      // widget listing all functions
    WFunctionSummary *wFunctionSummary_;                // summary info for one function at a time
    WFunctionCfg *wFunctionCfg_;                        // control flow graph for one function at a time
    WHexDump *wHexDump_;                                // raw data from anywhere in memory
    WAssemblyListing *wAssembly_;                       // traditional assembly listing
    WStatus *wStatus_;                                  // info for the "Status" tab
    WStatusBar *wStatusBar_;                            // info across the bottom of the screen
    P2::Function::Ptr currentFunction_;                 // current function or null

public:
    explicit Application(const Settings &settings, const std::vector<std::string> &specimenNames, const Wt::WEnvironment &env)
        : Wt::WApplication(env), ctx_(settings, specimenNames, this), wStacked_(NULL), wInteractivePhase_(NULL),
          wGrid_(NULL), wMainTabs_(NULL), wPartitioner_(NULL), wMemoryMap_(NULL), wFunctionList_(NULL),
          wFunctionSummary_(NULL), wFunctionCfg_(NULL), wHexDump_(NULL), wAssembly_(NULL), wStatus_(NULL), wStatusBar_(NULL) {
        init();
    }

    static Sawyer::CommandLine::ParserResult parseCommandLine(int argc, char *argv[], Settings&);

    static void main(int argc, char *argv[]);

private:
    void init();

    Wt::WContainerWidget* instantiateMainTabs();

    bool isTabAvailable(MainTab);

    void showHideTabs();

    void handleSpecimenParsed(bool done);
    void handleSpecimenLoaded(bool done);
    void handleSpecimenPartitioned(bool done);

    void changeTab2(int x) { changeTab((MainTab)x); }
    void changeTab(MainTab);

    void changeFunction(const P2::Function::Ptr &function);

    void changeFunctionDoubleClick(const P2::Function::Ptr &function);

    void showHexDumpAtAddress(rose_addr_t);

    void memoryMapChanged();
};

} // namespace
#endif
