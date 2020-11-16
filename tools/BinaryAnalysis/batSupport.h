#ifndef ROSE_BatSupport_H
#define ROSE_BatSupport_H

#include <rose.h>
#include <BinaryFeasiblePath.h>                         // rose
#include <BinarySerialIo.h>                             // rose
#include <BinarySymbolicExpr.h>                         // rose
#include <Partitioner2/Function.h>                      // rose
#include <Partitioner2/Partitioner.h>                   // rose

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <fstream>
#include <map>
#include <Sawyer/CommandLine.h>
#include <Sawyer/Message.h>
#include <set>
#include <string>

// Minimum ROSE versions required by this tool
#define MINIMUM_ROSE_HEADER_VERSION 11007006uL
#define MINIMUM_ROSE_LIBRARY_VERSION "0.11.7.6"

#if !defined(ROSE_VERSION)
    #warning "unknown ROSE version"
#elif ROSE_VERSION < MINIMUM_ROSE_HEADER_VERSION
    #error "ROSE headers version is too old"
#endif

/** Support for ROSE binary analysis tools. */
namespace Bat {

/** Whether to print state information. */
namespace ShowStates {                                  // using a namespace since we don't have C++11 enum classes
    /** Whether to print state information. */
    enum Flag {
        NO,                                             /**< Do not print state. */
        YES                                             /**< Print state. */
    };
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Output formatters
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Output formats for results.
 *
 *  This is the base class for output formatters for tools. */
class OutputFormatter: public Sawyer::SharedObject {
public:
    typedef Sawyer::SharedPointer<OutputFormatter> Ptr;

protected:
    OutputFormatter() {}

public:
    // Path as a whole
    virtual void title(std::ostream&, const std::string&) = 0;/**< Path title. */
    virtual void pathNumber(std::ostream&, size_t) = 0; /**< Path identification number. */
    virtual void pathHash(std::ostream&, const std::string&) = 0;/**< Path identification hash. */
    virtual void ioMode(std::ostream&, Rose::BinaryAnalysis::FeasiblePath::IoMode,
                        const std::string &what = std::string()) = 0;/**< Input or output. */
    virtual void mayMust(std::ostream&, Rose::BinaryAnalysis::FeasiblePath::MayOrMust,
                         const std::string &what = std::string()) = 0;/**< Solver mode. */
    virtual void objectAddress(std::ostream&, const Rose::BinaryAnalysis::SymbolicExpr::Ptr&) = 0;/**< Address of sentence object. */
    virtual void finalInsn(std::ostream&, const Rose::BinaryAnalysis::Partitioner2::Partitioner&,
                           SgAsmInstruction*) = 0;/**< Final instruction of path. */
    virtual void variable(std::ostream&, const Rose::BinaryAnalysis::Variables::StackVariable&) = 0;/**< Location of weakness. */
    virtual void variable(std::ostream&, const Rose::BinaryAnalysis::Variables::GlobalVariable&) = 0;/**< Location of weakness. */
    virtual void frameOffset(std::ostream&, const Rose::BinaryAnalysis::Variables::OffsetInterval&) = 0;/**< Location on stack. */
    virtual void frameRelative(std::ostream&, const Rose::BinaryAnalysis::Variables::StackVariable&,
                               const Rose::BinaryAnalysis::Variables::OffsetInterval&,
                               Rose::BinaryAnalysis::FeasiblePath::IoMode) = 0;/**< Loc of var w.r.t. accessed stack frame. */
    virtual void localVarsFound(std::ostream&, const Rose::BinaryAnalysis::Variables::StackVariables&,
                                const Rose::BinaryAnalysis::Partitioner2::Partitioner&) = 0;/**< Local variables. */
    virtual void pathLength(std::ostream&, size_t nVerts, size_t nSteps) = 0;     /**< Say number of steps in path. */
    virtual void startFunction(std::ostream&, const std::string&) = 0; /**< Name of first function. */
    virtual void endFunction(std::ostream&, const std::string&) = 0; /**< Name of last function. */
    virtual void solverEvidence(std::ostream&,
                                const Rose::BinaryAnalysis::SmtSolverPtr &solver) = 0; /**< Solver evidence of satisfiability. */
    virtual void pathIntro(std::ostream&) = 0;/**< Line introducing the path elements. */

    // Parts of the path
    virtual void bbVertex(std::ostream&, size_t id, const Rose::BinaryAnalysis::Partitioner2::BasicBlockPtr&,
                          const std::string &funcName) = 0; /**< Path vertex name. */
    virtual void bbSrcLoc(std::ostream&, const Rose::SourceLocation&) = 0; /**< Location of basic block. */
    virtual void insnListIntro(std::ostream&) = 0;      /**< Introduce a list of instructions. */
    virtual void insnStep(std::ostream&, size_t idx, const Rose::BinaryAnalysis::Partitioner2::Partitioner&,
                          SgAsmInstruction *insn) = 0; /**< Instruction within vertex. */
    virtual void semanticFailure(std::ostream&) = 0;    /**< semantic failure message. */
    virtual void indetVertex(std::ostream&, size_t idx) = 0;  /**< path vertex that's indeterminate. */
    virtual void indetStep(std::ostream&, size_t idx) = 0;  /**< Indeterminate location within vertex. */
    virtual void summaryVertex(std::ostream&, size_t idx, rose_addr_t) = 0; /**< Function summary vertex. */
    virtual void summaryStep(std::ostream&, size_t idx, const std::string &name) = 0; /**< Function summary within vertex. */
    virtual void edge(std::ostream&, const std::string&) = 0; /**< Name of edge along path. */
    virtual void state(std::ostream&, size_t vertexIdx, const std::string &title,
                       const Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::StatePtr&,
                       const Rose::BinaryAnalysis::RegisterDictionary*) = 0; /**< Semantic state within path. */
};

/** Produce no output.
 *
 *  This is mainly for testing that a command produces no unformatted output. */
class NullFormatter: public OutputFormatter {
protected:
    NullFormatter() {}

public:
    static Ptr instance();
    void title(std::ostream&, const std::string&) ROSE_OVERRIDE {}
    void pathNumber(std::ostream&, size_t) ROSE_OVERRIDE {}
    void pathHash(std::ostream&, const std::string&) ROSE_OVERRIDE {}
    void ioMode(std::ostream&, Rose::BinaryAnalysis::FeasiblePath::IoMode, const std::string &what) ROSE_OVERRIDE {}
    void mayMust(std::ostream&, Rose::BinaryAnalysis::FeasiblePath::MayOrMust, const std::string &what) ROSE_OVERRIDE {}
    void objectAddress(std::ostream&, const Rose::BinaryAnalysis::SymbolicExpr::Ptr&) ROSE_OVERRIDE {}
    void finalInsn(std::ostream&, const Rose::BinaryAnalysis::Partitioner2::Partitioner&, SgAsmInstruction*) ROSE_OVERRIDE {}
    void variable(std::ostream&, const Rose::BinaryAnalysis::Variables::StackVariable&) ROSE_OVERRIDE {}
    void variable(std::ostream&, const Rose::BinaryAnalysis::Variables::GlobalVariable&) ROSE_OVERRIDE {}
    void frameOffset(std::ostream&, const Rose::BinaryAnalysis::Variables::OffsetInterval&) ROSE_OVERRIDE {}
    void frameRelative(std::ostream&, const Rose::BinaryAnalysis::Variables::StackVariable&,
                       const Rose::BinaryAnalysis::Variables::OffsetInterval&,
                       Rose::BinaryAnalysis::FeasiblePath::IoMode) ROSE_OVERRIDE {}
    void localVarsFound(std::ostream&, const Rose::BinaryAnalysis::Variables::StackVariables&,
                        const Rose::BinaryAnalysis::Partitioner2::Partitioner&) ROSE_OVERRIDE {}
    void pathLength(std::ostream&, size_t nVerts, size_t nSteps) ROSE_OVERRIDE {}
    void startFunction(std::ostream&, const std::string &name) ROSE_OVERRIDE {}
    void endFunction(std::ostream&, const std::string &name) ROSE_OVERRIDE {}
    void pathIntro(std::ostream&) ROSE_OVERRIDE {}
    void bbVertex(std::ostream&, size_t id, const Rose::BinaryAnalysis::Partitioner2::BasicBlockPtr&,
                  const std::string &funcName) ROSE_OVERRIDE {}
    void bbSrcLoc(std::ostream&, const Rose::SourceLocation &loc) ROSE_OVERRIDE {}
    void insnListIntro(std::ostream&) ROSE_OVERRIDE {}
    void insnStep(std::ostream&, size_t idx, const Rose::BinaryAnalysis::Partitioner2::Partitioner&,
                  SgAsmInstruction *insn) ROSE_OVERRIDE {}
    void semanticFailure(std::ostream&) ROSE_OVERRIDE {}
    void indetVertex(std::ostream&, size_t idx) ROSE_OVERRIDE {}
    void indetStep(std::ostream&, size_t idx) ROSE_OVERRIDE {}
    void summaryVertex(std::ostream&, size_t idx, rose_addr_t) ROSE_OVERRIDE {}
    void summaryStep(std::ostream&, size_t idx, const std::string &name) ROSE_OVERRIDE {}
    void edge(std::ostream&, const std::string &name) ROSE_OVERRIDE {}
    void state(std::ostream&, size_t vertexIdx, const std::string &title,
               const Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::StatePtr&,
               const Rose::BinaryAnalysis::RegisterDictionary*) ROSE_OVERRIDE {}
    void solverEvidence(std::ostream&, const Rose::BinaryAnalysis::SmtSolverPtr&) ROSE_OVERRIDE {}
};

/** Format results using plain text.
 *
 *  This produces unformatted, ad hoc, output intended for human consumption. */
class PlainTextFormatter: public OutputFormatter {
protected:
    PlainTextFormatter() {}

public:
    static Ptr instance();
    void title(std::ostream&, const std::string&) ROSE_OVERRIDE;
    void pathNumber(std::ostream&, size_t) ROSE_OVERRIDE;
    void pathHash(std::ostream&, const std::string&) ROSE_OVERRIDE;
    void ioMode(std::ostream&, Rose::BinaryAnalysis::FeasiblePath::IoMode,
                const std::string &what = std::string()) ROSE_OVERRIDE;
    void mayMust(std::ostream&, Rose::BinaryAnalysis::FeasiblePath::MayOrMust,
                 const std::string &what = std::string()) ROSE_OVERRIDE;
    void objectAddress(std::ostream&, const Rose::BinaryAnalysis::SymbolicExpr::Ptr&) ROSE_OVERRIDE;
    void finalInsn(std::ostream&, const Rose::BinaryAnalysis::Partitioner2::Partitioner&, SgAsmInstruction*) ROSE_OVERRIDE;
    void variable(std::ostream&, const Rose::BinaryAnalysis::Variables::StackVariable&) ROSE_OVERRIDE;
    void variable(std::ostream&, const Rose::BinaryAnalysis::Variables::GlobalVariable&) ROSE_OVERRIDE;
    void frameOffset(std::ostream&, const Rose::BinaryAnalysis::Variables::OffsetInterval&) ROSE_OVERRIDE;
    void frameRelative(std::ostream&, const Rose::BinaryAnalysis::Variables::StackVariable&,
                       const Rose::BinaryAnalysis::Variables::OffsetInterval&,
                       Rose::BinaryAnalysis::FeasiblePath::IoMode) ROSE_OVERRIDE;
    void localVarsFound(std::ostream&, const Rose::BinaryAnalysis::Variables::StackVariables&,
                        const Rose::BinaryAnalysis::Partitioner2::Partitioner&) ROSE_OVERRIDE;
    void pathLength(std::ostream&, size_t nVerts, size_t nSteps) ROSE_OVERRIDE;
    void startFunction(std::ostream&, const std::string &name) ROSE_OVERRIDE;
    void endFunction(std::ostream&, const std::string &name) ROSE_OVERRIDE;
    void pathIntro(std::ostream&) ROSE_OVERRIDE;
    void bbVertex(std::ostream&, size_t id, const Rose::BinaryAnalysis::Partitioner2::BasicBlockPtr&,
                  const std::string &funcName) ROSE_OVERRIDE;
    void bbSrcLoc(std::ostream&, const Rose::SourceLocation &loc) ROSE_OVERRIDE;
    void insnListIntro(std::ostream&) ROSE_OVERRIDE;
    void insnStep(std::ostream&, size_t idx, const Rose::BinaryAnalysis::Partitioner2::Partitioner&,
                  SgAsmInstruction *insn) ROSE_OVERRIDE;
    void semanticFailure(std::ostream&) ROSE_OVERRIDE;
    void indetVertex(std::ostream&, size_t idx) ROSE_OVERRIDE;
    void indetStep(std::ostream&, size_t idx) ROSE_OVERRIDE;
    void summaryVertex(std::ostream&, size_t idx, rose_addr_t) ROSE_OVERRIDE;
    void summaryStep(std::ostream&, size_t idx, const std::string &name) ROSE_OVERRIDE;
    void edge(std::ostream&, const std::string &name) ROSE_OVERRIDE;
    void state(std::ostream&, size_t vertexIdx, const std::string &title,
               const Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::StatePtr&,
               const Rose::BinaryAnalysis::RegisterDictionary*) ROSE_OVERRIDE;
    void solverEvidence(std::ostream&, const Rose::BinaryAnalysis::SmtSolverPtr&) ROSE_OVERRIDE;
};

/**  Format output as YAML.
 *
 *   This output formatter produces YAML. */
class YamlFormatter: public OutputFormatter {
protected:
    YamlFormatter() {}
    std::string formatTag(const std::string &tag);

    template<class T>
    void writeln(std::ostream &out, const std::string &tag, const T &value) {
        out <<formatTag(tag);
        std::string s = boost::lexical_cast<std::string>(value);
        if (!s.empty())
            out <<" " <<s;
        out <<"\n";
    }

    void writeln(std::ostream &out, const std::string &tag) {
        writeln(out, tag, "");
    }

public:
    static Ptr instance();
    void title(std::ostream&, const std::string&) ROSE_OVERRIDE;
    void pathNumber(std::ostream&, size_t) ROSE_OVERRIDE;
    void pathHash(std::ostream&, const std::string&) ROSE_OVERRIDE;
    void ioMode(std::ostream&, Rose::BinaryAnalysis::FeasiblePath::IoMode,
                const std::string &what = std::string()) ROSE_OVERRIDE;
    void mayMust(std::ostream&, Rose::BinaryAnalysis::FeasiblePath::MayOrMust,
                 const std::string &what = std::string()) ROSE_OVERRIDE;
    void objectAddress(std::ostream&, const Rose::BinaryAnalysis::SymbolicExpr::Ptr&) ROSE_OVERRIDE;
    void finalInsn(std::ostream&, const Rose::BinaryAnalysis::Partitioner2::Partitioner&, SgAsmInstruction*) ROSE_OVERRIDE;
    void variable(std::ostream&, const Rose::BinaryAnalysis::Variables::StackVariable&) ROSE_OVERRIDE;
    void variable(std::ostream&, const Rose::BinaryAnalysis::Variables::GlobalVariable&) ROSE_OVERRIDE;
    void frameOffset(std::ostream&, const Rose::BinaryAnalysis::Variables::OffsetInterval&) ROSE_OVERRIDE;
    void frameRelative(std::ostream&, const Rose::BinaryAnalysis::Variables::StackVariable&,
                       const Rose::BinaryAnalysis::Variables::OffsetInterval&,
                       Rose::BinaryAnalysis::FeasiblePath::IoMode) ROSE_OVERRIDE;
    void localVarsFound(std::ostream&, const Rose::BinaryAnalysis::Variables::StackVariables&,
                        const Rose::BinaryAnalysis::Partitioner2::Partitioner&) ROSE_OVERRIDE;
    void pathLength(std::ostream&, size_t nVerts, size_t nSteps) ROSE_OVERRIDE;
    void pathIntro(std::ostream&) ROSE_OVERRIDE;
    void startFunction(std::ostream&, const std::string &name) ROSE_OVERRIDE;
    void endFunction(std::ostream&, const std::string &name) ROSE_OVERRIDE;
    void bbVertex(std::ostream&, size_t id, const Rose::BinaryAnalysis::Partitioner2::BasicBlockPtr&,
                  const std::string &funcName) ROSE_OVERRIDE;
    void bbSrcLoc(std::ostream&, const Rose::SourceLocation &loc) ROSE_OVERRIDE;
    void insnListIntro(std::ostream&) ROSE_OVERRIDE;
    void insnStep(std::ostream&, size_t idx, const Rose::BinaryAnalysis::Partitioner2::Partitioner&,
                  SgAsmInstruction *insn) ROSE_OVERRIDE;
    void semanticFailure(std::ostream&) ROSE_OVERRIDE;
    void indetVertex(std::ostream&, size_t idx) ROSE_OVERRIDE;
    void indetStep(std::ostream&, size_t idx) ROSE_OVERRIDE;
    void summaryVertex(std::ostream&, size_t idx, rose_addr_t) ROSE_OVERRIDE;
    void summaryStep(std::ostream&, size_t idx, const std::string &name) ROSE_OVERRIDE;
    void edge(std::ostream&, const std::string &name) ROSE_OVERRIDE;
    void state(std::ostream&, size_t vertexIdx, const std::string &title,
               const Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::StatePtr&,
               const Rose::BinaryAnalysis::RegisterDictionary*) ROSE_OVERRIDE;
    void solverEvidence(std::ostream&, const Rose::BinaryAnalysis::SmtSolverPtr&) ROSE_OVERRIDE;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Initialization functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Checks whether the ROSE version number is sufficient.
 *
 *  Given a version number string, like "0.9.7.128", return true if the ROSE library is that version or later, and false
 *  otherwise.  The versions that takes a stream argument will print an error message and exit.
 *
 * @{ */
bool checkRoseVersionNumber(const std::string &minimumVersion);
void checkRoseVersionNumber(const std::string &minimumVersion, Sawyer::Message::Stream &fatalStream);
/** @} */

/** Register all the Bat self tests for the --self-test switch. */
void registerSelfTests();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Stuff related to ROSE binary analysis files
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Create a command-line switch for the state file format. */
Sawyer::CommandLine::Switch stateFileFormatSwitch(Rose::BinaryAnalysis::SerialIo::Format &fmt/*in,out*/);

/** Check that output file name is valid or exit. */
void checkRbaOutput(const boost::filesystem::path &name, Sawyer::Message::Facility &mlog);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Functions for selecting things
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Select functions by name or address.
 *
 *  Given a list of functions, select those that have a name or address that matches any in @p namesOrAddresses.  Names are
 *  matched exactly, and addresses are matched numerically. The returned vector's functions will be in the same order as the
 *  input vector.
 *
 *  Any elements of @p namesOrAddresses that didn't match any of the @p functions is added to the @p unmatched set, or if an
 *  error stream is provided instead will be reported to that stream.
 *
 * @{ */
std::vector<Rose::BinaryAnalysis::Partitioner2::Function::Ptr>
selectFunctionsByNameOrAddress(const std::vector<Rose::BinaryAnalysis::Partitioner2::Function::Ptr> &functions,
                               const std::set<std::string> &namesOrAddresses, std::set<std::string> &unmatched /*in,out*/);
std::vector<Rose::BinaryAnalysis::Partitioner2::Function::Ptr>
selectFunctionsByNameOrAddress(const std::vector<Rose::BinaryAnalysis::Partitioner2::Function::Ptr> &functions,
                               const std::set<std::string> &namesOrAddresses, Sawyer::Message::Stream&);
/** @} */

/** Select functions that contain an address.
 *
 *  Returns a list of functions sorted by primary entry address such that each function spans the specified address. In other
 *  words, the address can be the function entry address, a basic block address, an instruction address, or any address within
 *  an instruction. */
std::vector<Rose::BinaryAnalysis::Partitioner2::Function::Ptr>
selectFunctionsContainingInstruction(const Rose::BinaryAnalysis::Partitioner2::Partitioner &partitioner,
                                     const std::set<rose_addr_t> &insnVas);

/** Select CFG vertex by name or address. */
Rose::BinaryAnalysis::Partitioner2::ControlFlowGraph::ConstVertexIterator
vertexForInstruction(const Rose::BinaryAnalysis::Partitioner2::Partitioner &partitioner, const std::string &nameOrVa);

/** Select CFG edge by endpoint vertices. */
Rose::BinaryAnalysis::Partitioner2::ControlFlowGraph::ConstEdgeIterator
edgeForInstructions(const Rose::BinaryAnalysis::Partitioner2::Partitioner &partitioner,
                    const Rose::BinaryAnalysis::Partitioner2::ControlFlowGraph::ConstVertexIterator &source,
                    const Rose::BinaryAnalysis::Partitioner2::ControlFlowGraph::ConstVertexIterator &target);

/** Select CFG edge by endpoint names. */
Rose::BinaryAnalysis::Partitioner2::ControlFlowGraph::ConstEdgeIterator
edgeForInstructions(const Rose::BinaryAnalysis::Partitioner2::Partitioner &partitioner, const std::string &sourceNameOrVa,
                    const std::string &targetNameOrVa);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Stuff for execution paths
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Return the first and last function names from the path if known. */
std::pair<std::string, std::string>
pathEndpointFunctionNames(const Rose::BinaryAnalysis::FeasiblePath&, const Rose::BinaryAnalysis::Partitioner2::CfgPath&);

/** Print information about a path from the model checker. */
void
printPath(std::ostream&, const Rose::BinaryAnalysis::FeasiblePath&,
          const Rose::BinaryAnalysis::Partitioner2::CfgPath&, const Rose::BinaryAnalysis::SmtSolverPtr&,
          const Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::RiscOperatorsPtr&,
          SgAsmInstruction *lastInsn, ShowStates::Flag, const OutputFormatter::Ptr&);

/** Compute calling conventions. */
void assignCallingConventions(const Rose::BinaryAnalysis::Partitioner2::Partitioner&);

/** Selects whether a path is to be printed. */
class PathSelector {
public:
    // Settings
    bool suppressAll;                                   /**< Show no output. Useful when generating only summaries. */
    bool suppressUninteresting;                         /**< Suppress paths that are "uninteresting". */
    bool suppressDuplicatePaths;                        /**< Suppress paths that have the same CFG execution sequence. */
    bool suppressDuplicateEndpoints;                    /**< Suppress paths that end at the same address. */
    bool showShorterPaths;                              /**< When suppressing endpoints, show shorter paths anyway. */
    size_t maxPaths;                                    /**< Maximum number of paths to show. */
    std::set<uint64_t> requiredHashes;                  /**< If non-empty, show only paths matching these hashes. */

    typedef Sawyer::Container::Map<rose_addr_t, size_t> EndpointLengths; /**< Path length per endpoint. */

private:
    SAWYER_THREAD_TRAITS::Mutex mutex_;                 // protects the following data members
    std::set<uint64_t> seenPaths_;                      // hashes of paths we've seen
    EndpointLengths seenEndpoints_;                     // path lengths for endpoints we've emitted
    size_t nSuppressed_;                                // number of paths suppressed for any reason
    size_t nUninteresting_;                             // number of uninteresting paths suppressed
    size_t nDuplicatePaths_;                            // number of duplicate paths suppressed
    size_t nDuplicateEndpoints_;                        // number of duplicate endpoints suppressed
    size_t nWrongHashes_;                               // number of suppressions due to not being a specified hash
    size_t nLimitExceeded_;                             // number of suppressions due to maxPaths being exceeded
    size_t nSelected_;                                  // number of times operator() returned true

public:
    PathSelector()
        : suppressAll(false), suppressUninteresting(false), suppressDuplicatePaths(false), suppressDuplicateEndpoints(false),
          showShorterPaths(false), maxPaths(Rose::UNLIMITED), nSuppressed_(0), nUninteresting_(0), nDuplicatePaths_(0),
          nDuplicateEndpoints_(0), nWrongHashes_(0), nLimitExceeded_(0), nSelected_(0) {}

public:
    /** Reset statistics. */
    void resetStats() {
        nSuppressed_ = nUninteresting_ = nDuplicatePaths_ = nDuplicateEndpoints_ = nLimitExceeded_ = nSelected_ = 0;
    }

    /** Return non-zero if the path should be shown, zero if suppressed.
     *
     *  The specified path is compared against previously checked paths to see whether it should be presented to the user
     *  based on settings and indexes contained in this object. Checking whether a path should be shown also updates various
     *  indexes.
     *
     *  If the path is suitable for being shown to the user, then this function returns a non-zero path number that's
     *  incremented each time this method is called.
     *
     *  Thread safety: This method is thread safe if the user is not concurrently adjusting this object's settings. */
    size_t operator()(const Rose::BinaryAnalysis::FeasiblePath &fpAnalysis,
                      const Rose::BinaryAnalysis::Partitioner2::CfgPath &path,
                      SgAsmInstruction *offendingInstruction);

    /** Property: Total number of paths suppressed.
     *
     *  This is the number of paths suppressed for any reason. */
    size_t nSuppressed() const { return nSuppressed_; }

    /** Property: Number of uninteresting paths suppressed.
     *
     *  This does not include those paths suppressed due to all paths being suppressed. */
    size_t nUninteresting() const { return nUninteresting_; }

    /** Property: Number of paths suppressed due to non-matching hashes.
     *
     *  This does not include those paths suppressed due to all paths being suppressed, or those paths suppressed due to being
     *  deemed uninteresting paths. */
    size_t nWrongHashes() const { return nWrongHashes_; }

    /** Property: Number of duplicate paths suppressed.
     *
     *  This does not include paths suppressed due to all paths being suppressed, paths suppressed due to being deemed uninteresting,
     *  or paths suppressed because they didn't match the set of required path hashes. */
    size_t nDuplicatePaths() const { return nDuplicatePaths_; }

    /** Property: Number of duplicate endpoints suppressed.
     *
     *  This does not include paths suppressed due to all paths being suppressed, paths suppressed due to being deemed
     *  uninteresting, paths suppressed because they didn't match the set of required path hashes, or paths suppressed because
     *  the entire path was a duplicate. This also does not include paths that would have been suppressed due to being a
     *  duplicate endpoint when the path is shown anyway because its shorter than any previous path at that endpoint. */
    size_t nDuplicateEndpoints() const { return nDuplicateEndpoints_; }

    /** Property: Number of paths suppressed due to limit being exceeded.
     *
     *  This is the number of paths that were suppressed because we've already shown the maximum permitted number of
     *  paths. This count does not include paths suppressed due to all paths being suppressed, paths suppressed due to being deemed
     *  uninteresting, paths suppressed because they didn't match the set of required path hashes, paths suppressed because
     *  the entire path was a duplicate, or paths suppressed because their end points were duplicates. */
    size_t nLimitExceeded() const { return nLimitExceeded_; }

    /** Property: Number of paths selected.
     *
     *  The number of times that the function operator returned a non-zero value. */
    size_t nSelected() const { return nSelected_; }

    /** Terminate entire program if we've selected the maximum number of paths.
     *
     *  It's not safe to call "exit" in a multi-threaded application when other threads are still executing (expect failed
     *  assertions in boost and other weirdness), so we use "_exit" instead, which is an immediate termination of the process
     *  without any in-process cleanup. */
    void maybeTerminate() const;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Instruction histograms
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if __cplusplus >= 201402L

/** Instruction histogram.
 *
 *  Maps instruction mnemonics to number of occurrences. */
using InsnHistogram = std::map<std::string, size_t>;

/** Compute histogram from memory map.
 *
 *  Scans the executable parts of the memory map like a linear sweeep disassembler would do in order to obtain a histogram
 *  describing how frequently each instruction mnemonic appears. */
InsnHistogram
computeInsnHistogram(const Rose::BinaryAnalysis::InstructionProvider&, const Rose::BinaryAnalysis::MemoryMap::Ptr&);

/** Save an instruction histogram in a file. */
void
saveInsnHistogram(const InsnHistogram&, const boost::filesystem::path&);

/** Load an instruction histogram from a file. */
InsnHistogram
loadInsnHistogram(const boost::filesystem::path&);

/** Merge one histogram into another.
 *
 *  The contents of the @p other histogram are merged into the first argument. */
void
mergeInsnHistogram(InsnHistogram &histogram, const InsnHistogram &other);

/** Split a histogram into parts.
 *
 *  The return value is a vector of sub-histograms with the first histogram being the most frequent instructions, second is the
 *  next frequent instructions, etc. All the histograms have the same number of symbols, except the last histograms may have
 *  one fewer instruction than the first histogram. */
std::vector<InsnHistogram>
splitInsnHistogram(const InsnHistogram&, size_t nParts);

/** Compare two histograms.
 *
 *  Both histograms are split into parts by calling @ref splitInsnHistogram. Then an amount of difference is calculated by
 *  comparing the locations of the mnemonics in the first set of histograms with which part they appear in among the second set
 *  of histograms. If a mnemonic appears only in the first histogram then it is assumed to appear in the last part of the second
 *  histogram.  The total difference is the sum of absolute differences between the part numbers. This function returns a relative
 *  amount of difference between 0.0 and 1.0 as the ratio of total difference to the total possible difference.
 *
 *  For the sake of performance when comparing one histogram with many others, the first histogram can be split prior to the
 *  comparison loop, in which case the @p nParts argument is omitted.
 *
 * @{ */
double compareInsnHistograms(const InsnHistogram&, const InsnHistogram&, size_t nParts);
double compareInsnHistograms(const std::vector<InsnHistogram>&, const InsnHistogram&);
/** @} */

/** Pretty print a histogram. */
void
printInsnHistogram(const InsnHistogram&, std::ostream&);

#endif

} // namespace

#endif
