#ifndef bROwSE_H
#define bROwSE_H

#include <rose.h>
#include <Color.h>                                      // ROSE
#include <Diagnostics.h>                                // ROSE
#include <Partitioner2/Partitioner.h>                   // ROSE
#include <Wt/WApplication>
#include <Wt/WColor>

namespace bROwSE {

namespace P2 = rose::BinaryAnalysis::Partitioner2;

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
    rose_addr_t peScramblerDispatcherVa;                // run the PeDescrambler module if non-zero
    bool intraFunctionCode;                             // suck up unused addresses as intra-function code
    bool intraFunctionData;                             // suck up unused addresses as intra-function data
    std::string httpAddress;                            // IP address at which to listen for HTTP connections
    unsigned short httpPort;                            // TCP port at which to listen for HTTP connections
    std::string docRoot;                                // document root directory for HTTP server
    std::string configurationName;                      // name of config file or directory containing such
    Settings()
        : deExecuteZeros(0), useSemantics(false), followGhostEdges(false), allowDiscontiguousBlocks(true),
          findFunctionPadding(true), findDeadCode(true), peScramblerDispatcherVa(0), intraFunctionCode(true),
          intraFunctionData(true), httpAddress("0.0.0.0"), httpPort(80), docRoot(".") {}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Attributes initialized by the first construction of a Context object (where they are documented)
extern P2::Attribute::Id ATTR_NBytes;
extern P2::Attribute::Id ATTR_NInsns;
extern P2::Attribute::Id ATTR_NIntervals;
extern P2::Attribute::Id ATTR_NDiscontiguousBlocks;
extern P2::Attribute::Id ATTR_CfgGraphVizFile;
extern P2::Attribute::Id ATTR_CfgImage;
extern P2::Attribute::Id ATTR_CfgVertexCoords;
extern P2::Attribute::Id ATTR_CallGraph;
extern P2::Attribute::Id ATTR_NCallers;
extern P2::Attribute::Id ATTR_NReturns;
extern P2::Attribute::Id ATTR_MayReturn;
extern P2::Attribute::Id ATTR_StackDelta;
extern P2::Attribute::Id ATTR_Ast;
extern P2::Attribute::Id ATTR_Heat;

// Context passed around to pretty much all the widgets.
class Context {
public:
    P2::Partitioner &partitioner;
    Wt::WApplication *application;                      // Wt probably has a method to get this, but I can't find it

    Context(P2::Partitioner &partitioner, Wt::WApplication *app): partitioner(partitioner), application(app) {
        init();
    }

private:
    void init();

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Convert a color from ROSE's representation to Wt's representation. */
Wt::WColor toWt(const rose::Color::RGB&);


} // namespace

#endif
