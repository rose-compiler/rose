#ifndef bROwSE_Application_H
#define bROwSE_Application_H

#include <bROwSE/bROwSE.h>

#include <Partitioner2/Partitioner.h>
#include <sawyer/CommandLine.h>
#include <Wt/WApplication>
#include <Wt/WEnvironment>

namespace bROwSE {

class WPartitioner;
class WMemoryMap;
class WFunctionCfg;
class WFunctionList;
class WFunctionSummary;

class Application: public Wt::WApplication {
public:
    /** The tabs in the central part of the main window. */
    enum MainTab {
        PartitionerTab,                                 /**< Partitioner configuration and actions. */
        MemoryMapTab,                                   /**< Description of specimen memory and its contents. */
        FunctionListTab,                                /**< List of all known functions. */
        FunctionSummaryTab,                             /**< Summary information for one function. */
        FunctionCfgTab,                                 /**< Control flow graph for one function. */
        NMainTabs
    };

private:
    Context ctx_;                                       // many other objects have a reference to this
    Wt::WGridLayout *wGrid_;                            // layout for the application as a whole
    Wt::WTabWidget *wMainTabs_;                         // main tabs in the central region of the app
    WPartitioner *wPartitioner_;                        // configuration
    WMemoryMap *wMemoryMap_;                            // widget for editing the specimen's memory map
    WFunctionList *wFunctionList_;                      // widget listing all functions
    WFunctionSummary *wFunctionSummary_;                // summary info for one function at a time
    WFunctionCfg *wFunctionCfg_;                        // control flow graph for one function at a time
    P2::Function::Ptr currentFunction_;                 // current function or null

public:
    explicit Application(const std::vector<std::string> &specimenNames, const Wt::WEnvironment &env)
        : Wt::WApplication(env), ctx_(specimenNames, this),
          wGrid_(NULL), wMainTabs_(NULL), wPartitioner_(NULL), wMemoryMap_(NULL), wFunctionList_(NULL),
          wFunctionSummary_(NULL), wFunctionCfg_(NULL) {
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

    // Change current function and go to the FunctionSummary tab
    void changeFunctionSummary(const P2::Function::Ptr &function);

    void showHexDumpAtAddress(rose_addr_t);
};

} // namespace
#endif
