#ifndef bROwSE_H
#define bROwSE_H

#include <Color.h>                                      // ROSE
#include <Diagnostics.h>                                // ROSE
#include <Partitioner2/Engine.h>                        // ROSE
#include <Partitioner2/Partitioner.h>                   // ROSE
#include <Wt/WApplication>
#include <Wt/WColor>

namespace bROwSE {

// Forward declarations for all bROwSE widgets
class WAddressComboBox;
class WAssemblyListing;
class WAddressSpace;
class WBusy;
class WCrossReferences;
class WFunctionCfg;
class WFunctionList;
class WFunctionNavigation;
class WFunctionSummary;
class WHexDump;
class WHexValueEdit;
class WMagic;
class WMemoryMap;
class WPartitioner;
class WSemantics;
class WStatus;
class WStatusBar;
class WStringDetail;
class WStrings;
class WToggleButton;

namespace P2 = rose::BinaryAnalysis::Partitioner2;

static const rose_addr_t NO_ADDRESS(-1);

// Convenient struct to hold settings from the command-line all in one place.
struct Settings {
    size_t deExecuteZeros;                              // threshold for removing execute permissions of zeros (zero disables)
    bool allowDownloads;                                // should the server allow file downloads?
    std::string httpAddress;                            // IP address at which to listen for HTTP connections
    unsigned short httpPort;                            // TCP port at which to listen for HTTP connections
    std::string docRoot;                                // document root directory for HTTP server
    std::vector<std::string> configurationNames;        // names of config files or directories containing such
    Settings()
        : deExecuteZeros(0), allowDownloads(true), httpAddress("0.0.0.0"), httpPort(80), docRoot(".") {}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Attributes initialized by the first construction of a Context object (where they are documented)
extern rose::BinaryAnalysis::Attribute::Id ATTR_NBytes;
extern rose::BinaryAnalysis::Attribute::Id ATTR_NInsns;
extern rose::BinaryAnalysis::Attribute::Id ATTR_NIntervals;
extern rose::BinaryAnalysis::Attribute::Id ATTR_NDiscontiguousBlocks;
extern rose::BinaryAnalysis::Attribute::Id ATTR_CfgGraphVizFile;
extern rose::BinaryAnalysis::Attribute::Id ATTR_CfgImage;
extern rose::BinaryAnalysis::Attribute::Id ATTR_CfgVertexCoords;
extern rose::BinaryAnalysis::Attribute::Id ATTR_CallGraph;
extern rose::BinaryAnalysis::Attribute::Id ATTR_NCallers;
extern rose::BinaryAnalysis::Attribute::Id ATTR_NCallees;
extern rose::BinaryAnalysis::Attribute::Id ATTR_NReturns;
extern rose::BinaryAnalysis::Attribute::Id ATTR_MayReturn;
extern rose::BinaryAnalysis::Attribute::Id ATTR_StackDelta;
extern rose::BinaryAnalysis::Attribute::Id ATTR_Ast;
extern rose::BinaryAnalysis::Attribute::Id ATTR_Heat;
extern rose::BinaryAnalysis::Attribute::Id ATTR_DataFlow;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Context passed around to pretty much all the widgets.
class Context {
public:
    Settings settings;                                  // settings mostly from the command-line
    std::vector<std::string> specimenNames;             // names of specimen files
    P2::Engine engine;                                  // partitioning engine
    P2::Partitioner partitioner;                        // partitioner used by the engine
    Wt::WApplication *application;                      // Wt probably has a method to get this, but I can't find it
    WBusy *busy;                                        // the main busy widget
    
    explicit Context(const Settings &settings, const std::vector<std::string> &specimenNames, Wt::WApplication *app)
        : settings(settings), specimenNames(specimenNames), application(app) {
        init();
    }

private:
    void init();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Error handling
extern Sawyer::Message::Facility mlog;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Convert a color from ROSE's representation to Wt's representation. */
Wt::WColor toWt(const rose::Color::RGB&);


} // namespace

#endif
