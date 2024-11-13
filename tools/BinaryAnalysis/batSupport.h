#ifndef ROSE_BatSupport_H
#define ROSE_BatSupport_H
#include <Rose/BinaryAnalysis/BasicTypes.h>

#include <Rose/As.h>
#include <Rose/BinaryAnalysis/FeasiblePath.h>
#include <Rose/BinaryAnalysis/InstructionProvider.h>
#include <Rose/BinaryAnalysis/SerialIo.h>
#include <Rose/BinaryAnalysis/SymbolicExpression.h>

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
    virtual void objectAddress(std::ostream&, const Rose::BinaryAnalysis::SymbolicExpression::Ptr&) = 0;/**< Address of sentence object. */
    virtual void finalInsn(std::ostream&, const Rose::BinaryAnalysis::Partitioner2::PartitionerConstPtr&,
                           SgAsmInstruction*) = 0;/**< Final instruction of path. */
    virtual void variable(std::ostream&, const Rose::BinaryAnalysis::Variables::StackVariable&) = 0;/**< Location of weakness. */
    virtual void variable(std::ostream&, const Rose::BinaryAnalysis::Variables::GlobalVariable&) = 0;/**< Location of weakness. */
    virtual void frameOffset(std::ostream&, const Rose::BinaryAnalysis::Variables::OffsetInterval&) = 0;/**< Location on stack. */
    virtual void frameRelative(std::ostream&, const Rose::BinaryAnalysis::Variables::StackVariable&,
                               const Rose::BinaryAnalysis::Variables::OffsetInterval&,
                               Rose::BinaryAnalysis::FeasiblePath::IoMode) = 0;/**< Loc of var w.r.t. accessed stack frame. */
    virtual void localVarsFound(std::ostream&, const Rose::BinaryAnalysis::Variables::StackVariables&,
                                const Rose::BinaryAnalysis::Partitioner2::PartitionerConstPtr&) = 0;/**< Local variables. */
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
    virtual void insnStep(std::ostream&, size_t idx, const Rose::BinaryAnalysis::Partitioner2::PartitionerConstPtr&,
                          SgAsmInstruction *insn) = 0; /**< Instruction within vertex. */
    virtual void semanticFailure(std::ostream&) = 0;    /**< semantic failure message. */
    virtual void indetVertex(std::ostream&, size_t idx) = 0;  /**< path vertex that's indeterminate. */
    virtual void indetStep(std::ostream&, size_t idx) = 0;  /**< Indeterminate location within vertex. */
    virtual void summaryVertex(std::ostream&, size_t idx, rose_addr_t) = 0; /**< Function summary vertex. */
    virtual void summaryStep(std::ostream&, size_t idx, const std::string &name) = 0; /**< Function summary within vertex. */
    virtual void edge(std::ostream&, const std::string&) = 0; /**< Name of edge along path. */
    virtual void state(std::ostream&, size_t vertexIdx, const std::string &title,
                       const Rose::BinaryAnalysis::InstructionSemantics::BaseSemantics::StatePtr&,
                       const Rose::BinaryAnalysis::RegisterDictionaryPtr&) = 0; /**< Semantic state within path. */
};

/** Produce no output.
 *
 *  This is mainly for testing that a command produces no unformatted output. */
class NullFormatter: public OutputFormatter {
protected:
    NullFormatter() {}

public:
    static Ptr instance();
    void title(std::ostream&, const std::string&) override {}
    void pathNumber(std::ostream&, size_t) override {}
    void pathHash(std::ostream&, const std::string&) override {}
    void ioMode(std::ostream&, Rose::BinaryAnalysis::FeasiblePath::IoMode, const std::string&/*what*/) override {}
    void mayMust(std::ostream&, Rose::BinaryAnalysis::FeasiblePath::MayOrMust, const std::string&/*what*/) override {}
    void objectAddress(std::ostream&, const Rose::BinaryAnalysis::SymbolicExpression::Ptr&) override {}
    void finalInsn(std::ostream&, const Rose::BinaryAnalysis::Partitioner2::PartitionerConstPtr&, SgAsmInstruction*) override {}
    void variable(std::ostream&, const Rose::BinaryAnalysis::Variables::StackVariable&) override {}
    void variable(std::ostream&, const Rose::BinaryAnalysis::Variables::GlobalVariable&) override {}
    void frameOffset(std::ostream&, const Rose::BinaryAnalysis::Variables::OffsetInterval&) override {}
    void frameRelative(std::ostream&, const Rose::BinaryAnalysis::Variables::StackVariable&,
                       const Rose::BinaryAnalysis::Variables::OffsetInterval&,
                       Rose::BinaryAnalysis::FeasiblePath::IoMode) override {}
    void localVarsFound(std::ostream&, const Rose::BinaryAnalysis::Variables::StackVariables&,
                        const Rose::BinaryAnalysis::Partitioner2::PartitionerConstPtr&) override {}
    void pathLength(std::ostream&, size_t /*nVerts*/, size_t /*nSteps*/) override {}
    void startFunction(std::ostream&, const std::string &/*name*/) override {}
    void endFunction(std::ostream&, const std::string &/*name*/) override {}
    void pathIntro(std::ostream&) override {}
    void bbVertex(std::ostream&, size_t /*id*/, const Rose::BinaryAnalysis::Partitioner2::BasicBlockPtr&,
                  const std::string &/*funcName*/) override {}
    void bbSrcLoc(std::ostream&, const Rose::SourceLocation&) override {}
    void insnListIntro(std::ostream&) override {}
    void insnStep(std::ostream&, size_t /*idx*/, const Rose::BinaryAnalysis::Partitioner2::PartitionerConstPtr&,
                  SgAsmInstruction*) override {}
    void semanticFailure(std::ostream&) override {}
    void indetVertex(std::ostream&, size_t /*idx*/) override {}
    void indetStep(std::ostream&, size_t /*idx*/) override {}
    void summaryVertex(std::ostream&, size_t /*idx*/, rose_addr_t) override {}
    void summaryStep(std::ostream&, size_t /*idx*/, const std::string &/*name*/) override {}
    void edge(std::ostream&, const std::string &/*name*/) override {}
    void state(std::ostream&, size_t /*vertexIdx*/, const std::string &/*title*/,
               const Rose::BinaryAnalysis::InstructionSemantics::BaseSemantics::StatePtr&,
               const Rose::BinaryAnalysis::RegisterDictionaryPtr&) override {}
    void solverEvidence(std::ostream&, const Rose::BinaryAnalysis::SmtSolverPtr&) override {}
};

/** Format results using plain text.
 *
 *  This produces unformatted, ad hoc, output intended for human consumption. */
class PlainTextFormatter: public OutputFormatter {
protected:
    PlainTextFormatter() {}

public:
    static Ptr instance();
    void title(std::ostream&, const std::string&) override;
    void pathNumber(std::ostream&, size_t) override;
    void pathHash(std::ostream&, const std::string&) override;
    void ioMode(std::ostream&, Rose::BinaryAnalysis::FeasiblePath::IoMode,
                const std::string &what = std::string()) override;
    void mayMust(std::ostream&, Rose::BinaryAnalysis::FeasiblePath::MayOrMust,
                 const std::string &what = std::string()) override;
    void objectAddress(std::ostream&, const Rose::BinaryAnalysis::SymbolicExpression::Ptr&) override;
    void finalInsn(std::ostream&, const Rose::BinaryAnalysis::Partitioner2::PartitionerConstPtr&, SgAsmInstruction*) override;
    void variable(std::ostream&, const Rose::BinaryAnalysis::Variables::StackVariable&) override;
    void variable(std::ostream&, const Rose::BinaryAnalysis::Variables::GlobalVariable&) override;
    void frameOffset(std::ostream&, const Rose::BinaryAnalysis::Variables::OffsetInterval&) override;
    void frameRelative(std::ostream&, const Rose::BinaryAnalysis::Variables::StackVariable&,
                       const Rose::BinaryAnalysis::Variables::OffsetInterval&,
                       Rose::BinaryAnalysis::FeasiblePath::IoMode) override;
    void localVarsFound(std::ostream&, const Rose::BinaryAnalysis::Variables::StackVariables&,
                        const Rose::BinaryAnalysis::Partitioner2::PartitionerConstPtr&) override;
    void pathLength(std::ostream&, size_t nVerts, size_t nSteps) override;
    void startFunction(std::ostream&, const std::string &name) override;
    void endFunction(std::ostream&, const std::string &name) override;
    void pathIntro(std::ostream&) override;
    void bbVertex(std::ostream&, size_t id, const Rose::BinaryAnalysis::Partitioner2::BasicBlockPtr&,
                  const std::string &funcName) override;
    void bbSrcLoc(std::ostream&, const Rose::SourceLocation &loc) override;
    void insnListIntro(std::ostream&) override;
    void insnStep(std::ostream&, size_t idx, const Rose::BinaryAnalysis::Partitioner2::PartitionerConstPtr&,
                  SgAsmInstruction *insn) override;
    void semanticFailure(std::ostream&) override;
    void indetVertex(std::ostream&, size_t idx) override;
    void indetStep(std::ostream&, size_t idx) override;
    void summaryVertex(std::ostream&, size_t idx, rose_addr_t) override;
    void summaryStep(std::ostream&, size_t idx, const std::string &name) override;
    void edge(std::ostream&, const std::string &name) override;
    void state(std::ostream&, size_t vertexIdx, const std::string &title,
               const Rose::BinaryAnalysis::InstructionSemantics::BaseSemantics::StatePtr&,
               const Rose::BinaryAnalysis::RegisterDictionaryPtr&) override;
    void solverEvidence(std::ostream&, const Rose::BinaryAnalysis::SmtSolverPtr&) override;
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
    void title(std::ostream&, const std::string&) override;
    void pathNumber(std::ostream&, size_t) override;
    void pathHash(std::ostream&, const std::string&) override;
    void ioMode(std::ostream&, Rose::BinaryAnalysis::FeasiblePath::IoMode,
                const std::string &what = std::string()) override;
    void mayMust(std::ostream&, Rose::BinaryAnalysis::FeasiblePath::MayOrMust,
                 const std::string &what = std::string()) override;
    void objectAddress(std::ostream&, const Rose::BinaryAnalysis::SymbolicExpression::Ptr&) override;
    void finalInsn(std::ostream&, const Rose::BinaryAnalysis::Partitioner2::PartitionerConstPtr&, SgAsmInstruction*) override;
    void variable(std::ostream&, const Rose::BinaryAnalysis::Variables::StackVariable&) override;
    void variable(std::ostream&, const Rose::BinaryAnalysis::Variables::GlobalVariable&) override;
    void frameOffset(std::ostream&, const Rose::BinaryAnalysis::Variables::OffsetInterval&) override;
    void frameRelative(std::ostream&, const Rose::BinaryAnalysis::Variables::StackVariable&,
                       const Rose::BinaryAnalysis::Variables::OffsetInterval&,
                       Rose::BinaryAnalysis::FeasiblePath::IoMode) override;
    void localVarsFound(std::ostream&, const Rose::BinaryAnalysis::Variables::StackVariables&,
                        const Rose::BinaryAnalysis::Partitioner2::PartitionerConstPtr&) override;
    void pathLength(std::ostream&, size_t nVerts, size_t nSteps) override;
    void pathIntro(std::ostream&) override;
    void startFunction(std::ostream&, const std::string &name) override;
    void endFunction(std::ostream&, const std::string &name) override;
    void bbVertex(std::ostream&, size_t id, const Rose::BinaryAnalysis::Partitioner2::BasicBlockPtr&,
                  const std::string &funcName) override;
    void bbSrcLoc(std::ostream&, const Rose::SourceLocation &loc) override;
    void insnListIntro(std::ostream&) override;
    void insnStep(std::ostream&, size_t idx, const Rose::BinaryAnalysis::Partitioner2::PartitionerConstPtr&,
                  SgAsmInstruction *insn) override;
    void semanticFailure(std::ostream&) override;
    void indetVertex(std::ostream&, size_t idx) override;
    void indetStep(std::ostream&, size_t idx) override;
    void summaryVertex(std::ostream&, size_t idx, rose_addr_t) override;
    void summaryStep(std::ostream&, size_t idx, const std::string &name) override;
    void edge(std::ostream&, const std::string &name) override;
    void state(std::ostream&, size_t vertexIdx, const std::string &title,
               const Rose::BinaryAnalysis::InstructionSemantics::BaseSemantics::StatePtr&,
               const Rose::BinaryAnalysis::RegisterDictionaryPtr&) override;
    void solverEvidence(std::ostream&, const Rose::BinaryAnalysis::SmtSolverPtr&) override;
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
std::vector<Rose::BinaryAnalysis::Partitioner2::FunctionPtr>
selectFunctionsByNameOrAddress(const std::vector<Rose::BinaryAnalysis::Partitioner2::FunctionPtr> &functions,
                               const std::set<std::string> &namesOrAddresses, std::set<std::string> &unmatched /*in,out*/);
std::vector<Rose::BinaryAnalysis::Partitioner2::FunctionPtr>
selectFunctionsByNameOrAddress(const std::vector<Rose::BinaryAnalysis::Partitioner2::FunctionPtr> &functions,
                               const std::set<std::string> &namesOrAddresses, Sawyer::Message::Stream&);
/** @} */

/** Select functions that contain an address.
 *
 *  Returns a list of functions sorted by primary entry address such that each function spans the specified address. In other
 *  words, the address can be the function entry address, a basic block address, an instruction address, or any address within
 *  an instruction. */
std::vector<Rose::BinaryAnalysis::Partitioner2::FunctionPtr>
selectFunctionsContainingInstruction(const Rose::BinaryAnalysis::Partitioner2::PartitionerConstPtr&,
                                     const std::set<rose_addr_t> &insnVas);

/** Select CFG vertex by name or address. */
Rose::BinaryAnalysis::Partitioner2::ControlFlowGraph::ConstVertexIterator
vertexForInstruction(const Rose::BinaryAnalysis::Partitioner2::PartitionerConstPtr&, const std::string &nameOrVa);

/** Select CFG edge by endpoint vertices. */
Rose::BinaryAnalysis::Partitioner2::ControlFlowGraph::ConstEdgeIterator
edgeForInstructions(const Rose::BinaryAnalysis::Partitioner2::PartitionerConstPtr&,
                    const Rose::BinaryAnalysis::Partitioner2::ControlFlowGraph::ConstVertexIterator &source,
                    const Rose::BinaryAnalysis::Partitioner2::ControlFlowGraph::ConstVertexIterator &target);

/** Select CFG edge by endpoint names. */
Rose::BinaryAnalysis::Partitioner2::ControlFlowGraph::ConstEdgeIterator
edgeForInstructions(const Rose::BinaryAnalysis::Partitioner2::PartitionerConstPtr&, const std::string &sourceNameOrVa,
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
          const Rose::BinaryAnalysis::InstructionSemantics::BaseSemantics::RiscOperatorsPtr&,
          SgAsmInstruction *lastInsn, ShowStates::Flag, const OutputFormatter::Ptr&);

/** Compute calling conventions. */
void assignCallingConventions(const Rose::BinaryAnalysis::Partitioner2::PartitionerConstPtr&);

/** Selects whether a path is to be printed.
 *
 *  A PathSelector applies an ordered list of predicates to the path in question until one of them indicates that the path
 *  should be rejected. If none indicate that the path should be rejected, then the path is accepted and the caller should
 *  produce some kind of output. The predicates are ordered so that final statistics can be displayed. However, the built-in
 *  predicates created by the constructor do not depend on each other, so the user is free to rearrange them or delete them as
 *  he sees fit. */
class PathSelector {
public:
    /** Predicate that tests whether a path should be rejected. */
    class Predicate {
    public:
        std::string name;                               /**< Name of this predicate displayed in diagnostics and as a lookup key. */
        std::string description;                        /**< Description @em x fitting the sentence "path was rejected because @em X." */
        size_t nCalls;                                  /**< Number of times this predicate was called, updated by the caller. */
        size_t nRejects;                                /**< Number of times the provided path was rejected, updated by the caller. */

        /** Base constructor.
         *
         *  The name should be unique and short so it can be used as a lookup key. The description should be worded to explain
         *  why a path was rejected, and should make sense in this sentence: "the path was rejected because @p desc." See
         *  built-in predicates for examples. */
        Predicate(const std::string &name, const std::string &desc)
            : name(name), description(desc), nCalls(0), nRejects(0) {}

        virtual ~Predicate() {}

        /** Tests whether a path should be rejected.
         *
         *  Returns true if a path is rejected, and false if not. The @p nCalls and @p nRejects data members will be incremented
         *  appropriately by the caller, so this function operator shouldn't mess with them. */
        virtual bool shouldReject(const Rose::BinaryAnalysis::FeasiblePath&, const Rose::BinaryAnalysis::Partitioner2::CfgPath&,
                                  SgAsmInstruction *offendingInstruction) = 0;

        /** Final disposition of the path.
         *
         *  This is called once we know whether the path was selected or rejected. The @p disposition argument is true if
         *  rejected, false if accepted. The @p nCalls and @p nRejects have already been updated by this point. */
        virtual void wasRejected(bool disposition, const Rose::BinaryAnalysis::FeasiblePath&,
                                 const Rose::BinaryAnalysis::Partitioner2::CfgPath&,
                                 SgAsmInstruction *offendingInstruction);
    };

    /** Always reject a path.
     *
     *  This is useful when all we want to do is report a summary of how many paths were found. */
    class AlwaysReject: public Predicate {
    public:
        AlwaysReject()
            : Predicate("always", "all paths are suppressed") {}

        virtual bool shouldReject(const Rose::BinaryAnalysis::FeasiblePath&, const Rose::BinaryAnalysis::Partitioner2::CfgPath&,
                                  SgAsmInstruction*) override {
            return true;
        }
    };

    /** Accept only unnamed to named paths.
     *
     *  Accept a path if it begins in an unnamed function but ends in a named function, otherwise reject it. With firmware,
     *  this usually means the path begins in user code and ends in a library function that was given a name by matching the
     *  function against a database. */
    class RejectEndNames: public Predicate {
    public:
        RejectEndNames()
            : Predicate("unamed-to-named", "it starts unnamed and ends named") {}
        virtual bool shouldReject(const Rose::BinaryAnalysis::FeasiblePath&, const Rose::BinaryAnalysis::Partitioner2::CfgPath&,
                                  SgAsmInstruction*) override;
    };

    /** Accept only specified paths. */
    class RejectUnlisted: public Predicate {
        std::set<uint64_t> listed_;                     // paths to accept (by hash), rejecting all others
    public:
        explicit RejectUnlisted(const std::set<uint64_t> &listed)
            : Predicate("unlisted", "it is not among those requested"), listed_(listed) {}
        virtual bool shouldReject(const Rose::BinaryAnalysis::FeasiblePath&,
                                  const Rose::BinaryAnalysis::Partitioner2::CfgPath &path,
                                  SgAsmInstruction *insn) override {
            return listed_.find(path.hash(insn)) == listed_.end();
        }
    };

    /** Reject duplicate paths. */
    class RejectDuplicatePaths: public Predicate {
        std::set<uint64_t> selected_;                   // paths that were selected (i.e., not rejected by any predicate)
    public:
        RejectDuplicatePaths()
            : Predicate("dup-paths", "path was seen already") {}
        virtual bool shouldReject(const Rose::BinaryAnalysis::FeasiblePath&,
                                  const Rose::BinaryAnalysis::Partitioner2::CfgPath& path,
                                  SgAsmInstruction *insn) override {
            return selected_.find(path.hash(insn)) != selected_.end(); // not previously selected
        }
        virtual void wasRejected(bool rejected, const Rose::BinaryAnalysis::FeasiblePath&,
                                 const Rose::BinaryAnalysis::Partitioner2::CfgPath &path,
                                 SgAsmInstruction *insn) override {
            if (!rejected)
                selected_.insert(path.hash(insn));
        }
    };

    /** Reject paths with duplicate endpoints. */
    class RejectDuplicateEndpoints: public Predicate {
        Sawyer::Container::Map<rose_addr_t /*insn*/, size_t /*path_length*/> seen_;
        bool showShorterPaths_;                         // if true, don't reject a seen path that's shorter than previously seen
    public:
        explicit RejectDuplicateEndpoints(bool showShorterPaths)
            : Predicate("dup-end", "its endpoint was already seen"), showShorterPaths_(showShorterPaths) {}
        virtual bool shouldReject(const Rose::BinaryAnalysis::FeasiblePath&, const Rose::BinaryAnalysis::Partitioner2::CfgPath&,
                                  SgAsmInstruction*) override;
        void wasRejected(bool rejected, const Rose::BinaryAnalysis::FeasiblePath&,
                         const Rose::BinaryAnalysis::Partitioner2::CfgPath &, SgAsmInstruction*) override;
    };

    /** Reject paths if we've accepted too many already. */
    class RejectTooMany: public Predicate {
        size_t limit_;                                  // number of paths to allow before we start rejecting them
        size_t nSelected_;                              // number of paths previously selected
    public:
        explicit RejectTooMany(size_t limit)
            : Predicate("too-many", "too many paths found"), limit_(limit) {}
        size_t limit() const {
            return limit_;
        }
        virtual bool shouldReject(const Rose::BinaryAnalysis::FeasiblePath&, const Rose::BinaryAnalysis::Partitioner2::CfgPath&,
                                  SgAsmInstruction*) override {
            return nSelected_ >= limit_;
        }
        virtual void wasRejected(bool rejected, const Rose::BinaryAnalysis::FeasiblePath&,
                                 const Rose::BinaryAnalysis::Partitioner2::CfgPath&, SgAsmInstruction*) override {
            if (!rejected)
                ++nSelected_;
        }
    };

    /** Ordered list of selectors.
     *
     *  Some of these are predefined by the constructor, but the user can add, remove, or reorder selectors as desired. */
    std::vector<std::shared_ptr<Predicate>> predicates; // Ordered list of

private:
    mutable SAWYER_THREAD_TRAITS::Mutex mutex_;         // protects the following data members
    size_t nSelected_ = 0;                              // total number of paths selected
    size_t nRejected_ = 0;                              // total number of paths rejected

public:
    /** Reset statistics. */
    void resetStats();

    /** Find a predicate by name.
     *
     *  Returns a null pointer if the name does not exist. If more than one predicate has the specified name, then the first
     *  match is returned. */
    std::shared_ptr<Predicate> findPredicate(const std::string &name) const;

    /** Find a predicate by type.
     *
     *  Returns the first predicate whose type inherits from T, or returns the null pointer if there is no such predicate. */
    template<class T>
    std::shared_ptr<T> findPredicate() const {
        for (auto predicate: predicates) {
            if (auto retval = Rose::as<T>(predicate))
                return retval;
        }
        return {};
    }

    /** Return non-zero path ID the path should be shown, zero if suppressed.
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

    /** Tests whether a path should be rejected.
     *
     *  If rejected, returns the non-null predicate that rejected the path and sets @p pathId to zero; otherwise returns null
     *  and sets @p pathId to the non-zero path identifier unique for each call that returns null.
     *
     *  Thread safety: This method is thread safe if the user is not currently adjusting the predicate list. */
    std::shared_ptr<PathSelector::Predicate>
    shouldReject(const Rose::BinaryAnalysis::FeasiblePath &fpAnalysis,
                 const Rose::BinaryAnalysis::Partitioner2::CfgPath &path,
                 SgAsmInstruction *offendingInstruction, size_t &pathId /*out*/);

    /** Property: Total number of paths rejected.
     *
     *  This is the number of paths rejected for any reason. */
    size_t nRejected() const;

    /** Property: Number of paths selected.
     *
     *  The number of paths selected (i.e., not rejected). */
    size_t nSelected() const;

    /** Terminate entire program if we've selected the maximum number of paths.
     *
     *  It's not safe to call "exit" in a multi-threaded application when other threads are still executing (expect failed
     *  assertions in boost and other weirdness), so we use "_exit" instead, which is an immediate termination of the process
     *  without any in-process cleanup. */
    void maybeTerminate() const;

    /** Print statistics about paths. */
    void printStatistics(std::ostream&, const std::string &prefix = "") const;
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
computeInsnHistogram(const Rose::BinaryAnalysis::InstructionProvider&, const Rose::BinaryAnalysis::MemoryMapPtr&);

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
