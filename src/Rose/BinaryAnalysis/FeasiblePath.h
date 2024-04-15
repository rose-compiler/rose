#ifndef ROSE_BinaryAnalysis_FeasiblePath_H
#define ROSE_BinaryAnalysis_FeasiblePath_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/BasicTypes.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/SymbolicSemantics.h>
#include <Rose/BinaryAnalysis/SmtSolver.h>
#include <Rose/BinaryAnalysis/SymbolicExpressionParser.h>
#include <Rose/BinaryAnalysis/Partitioner2/CfgPath.h>
#include <Rose/Exception.h>
#include <Sawyer/CommandLine.h>
#include <Sawyer/Message.h>
#include <boost/filesystem/path.hpp>
#include <boost/logic/tribool.hpp>

namespace Rose {
namespace BinaryAnalysis {

/** Feasible path analysis.
 *
 *  Determines whether CFG paths are feasible paths. */
class FeasiblePath {
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Types and public data members
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Exception for errors specific to feasible path analysis. */
    class Exception: public Rose::Exception {
    public:
        Exception(const std::string &what)
            : Rose::Exception(what) {}
        ~Exception() throw () {}
    };

    /** How to search for paths. */
    enum SearchMode {
        SEARCH_SINGLE_DFS,                              /**< Perform a depth first search. */
        SEARCH_SINGLE_BFS,                              /**< Perform a breadth first search. */
        SEARCH_MULTI                                    /**< Blast everything at once to the SMT solver. */
    };

    /** Organization of semantic memory. */
    enum SemanticMemoryParadigm {
        LIST_BASED_MEMORY,                              /**< Precise but slow. */
        MAP_BASED_MEMORY                                /**< Fast but not precise. */
    };

    /** Edge visitation order. */
    enum EdgeVisitOrder {
        VISIT_NATURAL,                                  /**< Visit edges in their natural, forward order. */
        VISIT_REVERSE,                                  /**< Visit edges in reverse of the natural order. */
        VISIT_RANDOM,                                   /**< Visit edges in random order. */
    };

    /** Read or write operation. */
    enum IoMode { READ, WRITE };

    /** Types of comparisons. */
    enum MayOrMust { MAY, MUST };

    /** Set of basic block addresses. */
    typedef std::set<rose_addr_t> AddressSet;

    /** Expression to be evaluated.
     *
     *  If the expression is a string, then the string is parsed to create a symbolic expression, substituting registers
     *  and memory from a supplied semantic state.
     *
     *  If the expression is an expression tree, then the expression is used directly. */
    struct Expression {
        AddressIntervalSet location;                    /**< Location where constraint applies. Empty implies end-of-path. */
        std::string parsable;                           /**< String to be parsed as an expression. */
        SymbolicExpressionPtr expr;                     /**< Symbolic expression. */

        Expression() {}
        /*implicit*/ Expression(const std::string &parsable): parsable(parsable) {}
        /*implicit*/ Expression(const SymbolicExpressionPtr &expr): expr(expr) {}

        void print(std::ostream&) const;
    };

    /** Settings that control this analysis. */
    struct Settings {
        // Path feasibility
        SearchMode searchMode;                          /**< Method to use when searching for feasible paths. */
        Sawyer::Optional<rose_addr_t> initialStackPtr;  /**< Concrete value to use for stack pointer register initial value. */
        size_t maxVertexVisit;                          /**< Max times to visit a particular vertex in one path. */
        size_t maxPathLength;                           /**< Limit path length in terms of number of instructions. */
        size_t maxCallDepth;                            /**< Max length of path in terms of function calls. */
        size_t maxRecursionDepth;                       /**< Max path length in terms of recursive function calls. */
        std::vector<Expression> assertions;             /**< Constraints to be satisfied at some point along the path. */
        std::vector<std::string> assertionLocations;    /**< Locations at which "constraints" are checked. */
        std::vector<rose_addr_t> summarizeFunctions;    /**< Functions to always summarize. */
        bool nonAddressIsFeasible;                      /**< Indeterminate/undiscovered vertices are feasible? */
        std::string solverName;                         /**< Type of SMT solver. */
        SemanticMemoryParadigm memoryParadigm;          /**< Type of memory state when there's a choice to be made. */
        bool processFinalVertex;                        /**< Whether to process the last vertex of the path. */
        bool ignoreSemanticFailure;                     /**< Whether to ignore instructions with no semantic info. */
        double kCycleCoefficient;                       /**< Coefficient for adjusting maxPathLengh during CFG cycles. */
        EdgeVisitOrder edgeVisitOrder;                  /**< Order in which to visit edges. */
        bool trackingCodeCoverage;                      /**< If set, track which block addresses are reached. */
        std::vector<rose_addr_t> ipRewrite;             /**< An even number of from,to pairs for rewriting the insn ptr reg. */
        Sawyer::Optional<boost::chrono::duration<double> > smtTimeout; /**< Max seconds allowed per SMT solve call. */
        size_t maxExprSize;                             /**< Maximum symbolic expression size before replacement. */
        bool traceSemantics;                            /**< Trace all instruction semantics operations. */

        // Null dereferences
        struct NullDeref {
            bool check;                                 /**< If true, look for null dereferences along the paths. */
            MayOrMust mode;                             /**< Check for addrs that may or must be null. */
            bool constOnly;                             /**< If true, check only constants or sets of constants. */
            rose_addr_t minValid;                       /**< Minnimum address that is not treated as a null dereference */

            NullDeref()
                : check(false), mode(MUST), constOnly(false), minValid(1024) {}
        } nullDeref;                                    /**< Settings for null-dereference analysis. */

        std::string exprParserDoc;                      /**< String documenting how expressions are parsed, empty for default. */

        /** Default settings. */
        Settings()
            : searchMode(SEARCH_SINGLE_DFS), maxVertexVisit((size_t)-1), maxPathLength(200), maxCallDepth((size_t)-1),
              maxRecursionDepth((size_t)-1), nonAddressIsFeasible(true), solverName("best"),
              memoryParadigm(LIST_BASED_MEMORY), processFinalVertex(false), ignoreSemanticFailure(false),
              kCycleCoefficient(0.0), edgeVisitOrder(VISIT_NATURAL), trackingCodeCoverage(true), maxExprSize(UNLIMITED),
              traceSemantics(false) {}
    };

    /** Statistics from path searching. */
    struct Statistics {
        size_t nPathsExplored;                          /**< Number of paths explored. */
        size_t maxVertexVisitHits;                      /**< Number of times settings.maxVertexVisit was hit. */
        size_t maxPathLengthHits;                       /**< Number of times settings.maxPathLength was hit (effective K). */
        size_t maxCallDepthHits;                        /**< Number of times settings.maxCallDepth was hit. */
        size_t maxRecursionDepthHits;                   /**< Number of times settings.maxRecursionDepth was hit. */
        Sawyer::Container::Map<rose_addr_t, size_t> reachedBlockVas; /**< Number of times each basic block was reached. */

        Statistics()
            : nPathsExplored(0), maxVertexVisitHits(0), maxPathLengthHits(0), maxCallDepthHits(0), maxRecursionDepthHits(0) {}

        Statistics& operator+=(const Statistics&);
    };

    /** Diagnostic output. */
    static Sawyer::Message::Facility mlog;

    /** Descriptor of path pseudo-registers.
     *
     *  This analysis adds a special register named "path" to the register dictionary. This register holds the expression that
     *  determines how to reach the end of the path from the beginning. The major and minor numbers are arbitrary, but chosen
     *  so that they hopefully don't conflict with any real registers, which tend to start counting at zero.  Since we're using
     *  BaseSemantics::RegisterStateGeneric, we can use its flexibility to store extra "registers" without making any other
     *  changes to the architecture. */
     RegisterDescriptor REG_PATH;

    /** Information about a variable seen on a path. */
    struct VarDetail {
        std::string registerName;
        std::string firstAccessMode;                    /**< How was variable first accessed ("read" or "write"). */
        SgAsmInstruction *firstAccessInsn;              /**< Instruction address where this var was first read. */
        Sawyer::Optional<size_t> firstAccessIdx;        /**< Instruction position in path where this var was first read. */
        SymbolicExpressionPtr memAddress;               /**< Address where variable is located. */
        size_t memSize;                                 /**< Size of total memory access in bytes. */
        size_t memByteNumber;                           /**< Byte number for memory access. */
        Sawyer::Optional<rose_addr_t> returnFrom;       /**< This variable is the return value from the specified function. */

        VarDetail(): firstAccessInsn(NULL), memSize(0), memByteNumber(0) {}
        std::string toString() const;
    };

    /** Variable detail by name. */
    typedef Sawyer::Container::Map<std::string /*name*/, FeasiblePath::VarDetail> VarDetails;

    /** Path searching functor.
     *
     *  This is the base class for user-defined functors called when searching for feasible paths. */
    class PathProcessor {
    public:
        enum Action {
            BREAK,                                      /**< Do not continue along this path. */
            CONTINUE                                    /**< Continue along this path. */
        };

        virtual ~PathProcessor() {}

        /** Function invoked whenever a complete path is found.
         *
         *  The @p analyzer is a reference to the analyzer that's invoking this callback.
         *
         *  The @p path enumerates the CFG vertices and edges that compose the path.
         *
         *  The @p cpu represents the machine state at the start of the final vertex of the path. Modifications to the state
         *  have undefined behavior; the state may be re-used by the analysis when testing subsequent paths.
         *
         *  The @p solver contains the assertions that are satisfied to prove that this path is feasible. The solver contains
         *  multiple levels: an initial level that's probably empty (trivially satisfiable), followed by an additional level
         *  pushed for each edge of the path.
         *
         *  The return value from this callback determines whether the analysis will search for additional paths. A return
         *  value of @c Action::CONTINUE means the model checker will try other paths, and a return value of @c
         *  Action::BREAK means no more paths will be tested. */
        virtual Action found(const FeasiblePath &analyzer, const Partitioner2::CfgPath &path,
                             const InstructionSemantics::BaseSemantics::DispatcherPtr &cpu,
                             const SmtSolverPtr &solver);

        /** Function invoked whenever a null pointer dereference is detected.
         *
         *  The following parameters are passed to this callback:
         *
         *  The @p analyzer is the state of the analysis at the time that the null dereference is detected. Additional
         *  information such as the @ref Partitioner2::Partitioner "partitioner" is available through this object.
         *
         *  The @p path is the execution path from a starting vertex to the vertex in which the null dereference occurs.  Each
         *  vertex of the path is a basic block or function summary. All but the last vertex will have a corresponding symbolic
         *  state of the model checker as it existed at the end of processing the vertex. These states should not be modified
         *  by this callback.
         *
         *  The @p insn is the instruction during which the null dereference occurred and may be a null pointer in some
         *  situations. For instance, the instruction will be null if the dereference occurs when popping the return address
         *  from the stack for a function that was called but whose implementation is not present (such as when the
         *  inter-procedural depth was too great, the function is a non-linked import, etc.)
         *
         *  The @p cpu is the model checker's state immediately prior to the null dereference. This callback must not modify
         *  the state.
         *
         *  The @p solver is the optional SMT solver used to conclude that the execution path is feasible and that a null
         *  dereference occurs. This callback can query the SMT solver to obtain information about the evidence of
         *  satisfiability.  This callback may use the solver for additional work either in its current transaction or by
         *  pushing additional transactions; this callback should not pop the current transaction.
         *
         *  The @p ioMode indicates whether the null address was read or written.
         *
         *  The @p addr is the address that was accessed.  Depending on the model checker's settings, this is either a constant
         *  or a symbolic expression. In the latter case, the @p solver will have evidence that the expression can be zero.
         *
         *  The return value indicates whether the model checker should continue along the same path (@c Action::CONTINUE) or
         *  backtrack (@c Action::BREAK). If this callback requests backtracking then the model checker may continue evaluating
         *  the current path vertex but will not call any more more path processing functions until the backtrack occurs. */
        virtual Action nullDeref(const FeasiblePath &analyzer, const Partitioner2::CfgPath &path, SgAsmInstruction *insn,
                                 const InstructionSemantics::BaseSemantics::RiscOperatorsPtr &cpu, const SmtSolverPtr &solver,
                                 IoMode ioMode, const InstructionSemantics::BaseSemantics::SValuePtr &addr);

        /** Function invoked every time a memory reference occurs.
         *
         *  The following parameters are passed to this callback:
         *
         *  The @p analyzer is the state of the analysis at the time that the memory I/O is detected. Additional information
         *  such as the @ref Partitioner2::Partitioner "partitioner" is available through this object.
         *
         *  The @p path is the execution path from a starting vertex to the vertex in which the memory I/O occurs.  Each vertex
         *  of the path is a basic block or function summary. All but the last vertex will have a corresponding symbolic state
         *  of the model checker as it existed at the end of processing the vertex. These states should not be modified by this
         *  callback.
         *
         *  The @p insn is the instruction during which the memoryIo occurred and may be a null pointer in some
         *  situations. For instance, the instruction will be null if the I/O occurs when popping the return address
         *  from the stack for a function that was called but whose implementation is not present (such as when the
         *  inter-procedural depth was too great, the function is a non-linked import, etc.)
         *
         *  The @p cpu is the model checker's state immediately prior to the memory I/O. This callback must not modify the
         *  state.
         *
         *  The @p solver is the optional SMT solver used to conclude that the execution path is feasible.  This callback can
         *  query the SMT solver to obtain information about the evidence of satisfiability.  This callback may use the solver
         *  for additional work either in its current transaction or by pushing additional transactions; this callback should
         *  not pop the current transaction.
         *
         *  The @p ioMode indicates whether the memory address was read or written.
         *
         *  The @p addr is the address that was accessed.
         *
         *  The @p value is the value read or written.
         *
         *  The @p insn is the instruction during which the null dereference occurred and may be a null pointer in some
         *  situations. For instance, the instruction will be null if the dereference occurs when popping the return address
         *  from the stack for a function that was called but whose implementation is not present (such as when the
         *  inter-procedural depth was too great, the function is a non-linked import, etc.).
         *
         *  The return value indicates whether the model checker should continue along the same path (@c Action::CONTINUE) or
         *  backtrack (@c Action::BREAK). If this callback requests backtracking then the model checker may continue evaluating
         *  the current path vertex but will not call any more more path processing functions until the backtrack occurs. */
        virtual Action memoryIo(const FeasiblePath &analyzer, const Partitioner2::CfgPath &path, SgAsmInstruction *insn,
                                const InstructionSemantics::BaseSemantics::RiscOperatorsPtr &cpu, const SmtSolverPtr &solver,
                                IoMode ioMode, const InstructionSemantics::BaseSemantics::SValuePtr &addr,
                                const InstructionSemantics::BaseSemantics::SValuePtr &value);
    };

    /** Information stored per V_USER_DEFINED path vertex.
     *
     *  This is information for summarized functions. */
    struct FunctionSummary {
        rose_addr_t address = 0;                            /**< Address of summarized function. */
        int64_t stackDelta;                                 /**< Stack delta for summarized function. */
        std::string name;                                   /**< Name of summarized function. */

        /** Construct empty function summary. */
        FunctionSummary();

        /** Construct function summary with information. */
        FunctionSummary(const Partitioner2::ControlFlowGraph::ConstVertexIterator &cfgFuncVertex, uint64_t stackDelta);
    };

    /** Summaries for multiple functions. */
    typedef Sawyer::Container::Map<rose_addr_t, FunctionSummary> FunctionSummaries;

    /** Base class for callbacks for function summaries.
     *
     *  See the @ref functionSummarizer property. These objects are reference counted and allocated on the heap. */
    class FunctionSummarizer: public Sawyer::SharedObject {
    public:
        /** Reference counting pointer. */
        typedef Sawyer::SharedPointer<FunctionSummarizer> Ptr;
    protected:
        FunctionSummarizer() {}
    public:
        /** Invoked when a new summary is created. */
        virtual void init(const FeasiblePath &analysis, FunctionSummary &summary /*in,out*/,
                          const Partitioner2::FunctionPtr &function,
                          Partitioner2::ControlFlowGraph::ConstVertexIterator cfgCallTarget) = 0;

        /** Invoked when the analysis traverses the summary.
         *
         *  Returns true if the function was processed, false if we decline to process the function. If returning false, then
         *  the caller will do some basic processing based on the calling convention. */
        virtual bool process(const FeasiblePath &analysis, const FunctionSummary &summary,
                             const InstructionSemantics::SymbolicSemantics::RiscOperatorsPtr &ops) = 0;

        /** Return value for function.
         *
         *  This is called after @ref process in order to obtain the primary return value for the function. If the function
         *  doesn't return anything, then this method returns a null pointer. */
        virtual InstructionSemantics::SymbolicSemantics::SValuePtr
        returnValue(const FeasiblePath &analysis, const FunctionSummary &summary,
                    const InstructionSemantics::SymbolicSemantics::RiscOperatorsPtr &ops) = 0;
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Private data members
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
    Partitioner2::PartitionerConstPtr partitioner_;     // binary analysis context, might be null
    RegisterDictionaryPtr registers_;                   // registers augmented with "path" pseudo-register
    RegisterDescriptor REG_RETURN_;                     // FIXME[Robb P Matzke 2016-10-11]: see source
    Settings settings_;
    FunctionSummaries functionSummaries_;
    Partitioner2::CfgVertexMap vmap_;                   // relates CFG vertices to path vertices
    Partitioner2::ControlFlowGraph paths_;              // all possible paths, feasible or otherwise
    Partitioner2::CfgConstVertexSet pathsBeginVertices_;// vertices of paths_ where searching starts
    Partitioner2::CfgConstVertexSet pathsEndVertices_;  // vertices of paths_ where searching stops
    bool isDirectedSearch_ = true;                      // use pathsEndVertices_?
    Partitioner2::CfgConstEdgeSet cfgAvoidEdges_;       // CFG edges to avoid
    Partitioner2::CfgConstVertexSet cfgEndAvoidVertices_;// CFG end-of-path and other avoidance vertices
    FunctionSummarizer::Ptr functionSummarizer_;        // user-defined function for handling function summaries
    InstructionSemantics::BaseSemantics::StatePtr initialState_; // set by setInitialState.
    static Sawyer::Attribute::Id POST_STATE;            // stores semantic state after executing the insns for a vertex
    static Sawyer::Attribute::Id POST_INSN_LENGTH;      // path length in instructions at end of vertex
    static Sawyer::Attribute::Id EFFECTIVE_K;           // (double) effective maximimum path length

    mutable SAWYER_THREAD_TRAITS::Mutex statsMutex_;    // protects the following data member
    Statistics stats_;                                  // statistical results of the analysis

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Construction, destruction
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Constructs a new feasible path analyzer. */
    FeasiblePath();
    virtual ~FeasiblePath();

    /** Reset to initial state without changing settings. */
    void reset();

    /** Reset only statistics.
     *
     *  Thread safety: This method is thread safe. */
    void resetStatistics() {
        SAWYER_THREAD_TRAITS::LockGuard lock(statsMutex_);
        stats_ = Statistics();
    }

    /** Initialize diagnostic output. This is called automatically when ROSE is initialized. */
    static void initDiagnostics();


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Settings affecting behavior
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Property: Settings used by this analysis.
     *
     * @{ */
    const Settings& settings() const { return settings_; }
    Settings& settings() { return settings_; }
    void settings(const Settings &s) { settings_ = s; }
    /** @} */

    /** Describe command-line switches.
     *
     *  The @p settings provide default values. A reference to @p settings is saved and when the command-line is parsed and
     *  applied, the settings are adjusted. */
    static Sawyer::CommandLine::SwitchGroup commandLineSwitches(Settings &settings);

    /** Documentation for the symbolic expression parser. */
    static std::string expressionDocumentation();


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Overridable processing functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Create the virtual CPU.
     *
     *  Creates a new virtual CPU for each call.  The first call also makes a copy of the register dictionary from the
     *  specified partitioner and augments it with a "path" pseudo-register that holds a symbolic expressions on which the
     *  current CFG path depends. */
    virtual InstructionSemantics::BaseSemantics::DispatcherPtr
    buildVirtualCpu(const Partitioner2::PartitionerConstPtr&, const Partitioner2::CfgPath*, PathProcessor*, const SmtSolverPtr&);

    /** Initialize state for first vertex of path.
     *
     *  Given the beginning of the path and the virtual CPU, initialize that state with whatever is suitable for the
     *  analysis. The default implementation sets the "path" pseudo-register to true (since the first vertex of the path is
     *  unconditionally feasible), sets the instruction pointer register to the first instruction, and initializes the stack
     *  pointer with the concrete stack pointer from @ref settings (if any).  On x86, the DF register is set. */
    virtual void
    setInitialState(const InstructionSemantics::BaseSemantics::DispatcherPtr &cpu,
                    const Partitioner2::ControlFlowGraph::ConstVertexIterator &pathsBeginVertex);

    /** Process instructions for one basic block on the specified virtual CPU.
     *
     *  This is a state transfer function, updating the virtual machine state by processing the instructions of the specified
     *  basic block. */
    virtual void
    processBasicBlock(const Partitioner2::BasicBlockPtr &bblock,
                      const InstructionSemantics::BaseSemantics::DispatcherPtr &cpu, size_t pathInsnIndex);

    /** Process an indeterminate block.
     *
     *  This is a state transfer function, representing flow of control through an unknown address. */
    virtual void
    processIndeterminateBlock(const Partitioner2::ControlFlowGraph::ConstVertexIterator &vertex,
                              const InstructionSemantics::BaseSemantics::DispatcherPtr &cpu,
                              size_t pathInsnIndex);

    /** Process a function summary vertex.
     *
     *  This is a state transfer function, representing flow of control across a summarized function. */
    virtual void
    processFunctionSummary(const Partitioner2::ControlFlowGraph::ConstVertexIterator &pathsVertex,
                           const InstructionSemantics::BaseSemantics::DispatcherPtr &cpu,
                           size_t pathInsnIndex);

    /** Process one vertex.
     *
     *  This is the general state transfer function, representing flow of control through any type of vertex. */
    virtual void
    processVertex(const InstructionSemantics::BaseSemantics::DispatcherPtr &cpu,
                  const Partitioner2::ControlFlowGraph::ConstVertexIterator &pathsVertex,
                  size_t pathInsnIndex);

    /** Determines whether a function call should be summarized instead of inlined. */
    virtual bool
    shouldSummarizeCall(const Partitioner2::ControlFlowGraph::ConstVertexIterator &pathVertex,
                        const Partitioner2::ControlFlowGraph &cfg,
                        const Partitioner2::ControlFlowGraph::ConstVertexIterator &cfgCallTarget);

    /** Determines whether a function call should be inlined. */
    virtual bool
    shouldInline(const Partitioner2::CfgPath &path, const Partitioner2::ControlFlowGraph::ConstVertexIterator &cfgCallTarget);

    /** Property: Function summary handling.
     *
     *  As an alternative to creating a subclass to override the @ref processFunctionSummary, this property can contain an
     *  object that will be called in various ways whenever a function summary is processed.  If non-null, then whenever a
     *  function summary is created, the object's @c init method is called, and whenever a function summary is traversed its @c
     *  process method is called.
     *
     *  @{ */
    FunctionSummarizer::Ptr functionSummarizer() const { return functionSummarizer_; }
    void functionSummarizer(const FunctionSummarizer::Ptr &f) { functionSummarizer_ = f; }
    /** @} */

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Utilities
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Convert path vertex to a CFG vertex. */
    Partitioner2::ControlFlowGraph::ConstVertexIterator
    pathToCfg(const Partitioner2::ControlFlowGraph::ConstVertexIterator &pathVertex) const;

    /** Convert CFG vertices to path vertices. */
    Partitioner2::CfgConstVertexSet
    cfgToPaths(const Partitioner2::CfgConstVertexSet&) const;

    /** Convert CFG edges to path edges.
     *
     *  Any edges that don't exist in the paths graph are ignored and not returned. */
    Partitioner2::CfgConstEdgeSet
    cfgToPaths(const Partitioner2::CfgConstEdgeSet&) const;

    /** True if path ends with a function call. */
    bool pathEndsWithFunctionCall(const Partitioner2::CfgPath&) const;

    /** True if vertex is a function call. */
    bool isFunctionCall(const Partitioner2::ControlFlowGraph::ConstVertexIterator&) const;

    /** Print one vertex of a path for debugging. */
    void printPathVertex(std::ostream &out, const Partitioner2::ControlFlowGraph::Vertex &pathVertex,
                         size_t &insnIdx /*in,out*/) const;

    /** Print the path to the specified output stream.
     *
     *  This is intended mainly for debugging. */
    void printPath(std::ostream &out, const Partitioner2::CfgPath&) const;

    /** Determine whether any ending vertex is reachable.
     *
     *  Returns true if any of the @p endVertices can be reached from the @p beginVertex by following the edges of the graph.
     *  However, if @ref isDirectedSearch is false, then the end vertices are ignored and this function always returns true. */
    bool isAnyEndpointReachable(const Partitioner2::ControlFlowGraph &cfg,
                                const Partitioner2::ControlFlowGraph::ConstVertexIterator &beginVertex,
                                const Partitioner2::CfgConstVertexSet &endVertices);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Functions for describing the search space
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:

    /** Specify search boundary.
     *
     *  This function initializes the analysis by specifying starting and ending CFG vertices and the vertices and edges that
     *  should be avoided.  If the @p cfgEndVertices is supplied (even if empty) then the search is directed. A directed search
     *  considers only the subset of the CFG that consists of vertices and edges that appear on some path from any of the @p
     *  cfgBeginVertices to any of the @p cfgEndVertices.
     *
     * @{ */
    void
    setSearchBoundary(const Partitioner2::PartitionerConstPtr &partitioner,
                      const Partitioner2::CfgConstVertexSet &cfgBeginVertices,
                      const Partitioner2::CfgConstVertexSet &cfgEndVertices,
                      const Partitioner2::CfgConstVertexSet &cfgAvoidVertices = Partitioner2::CfgConstVertexSet(),
                      const Partitioner2::CfgConstEdgeSet &cfgAvoidEdges = Partitioner2::CfgConstEdgeSet());
    void
    setSearchBoundary(const Partitioner2::PartitionerConstPtr &partitioner,
                      const Partitioner2::ControlFlowGraph::ConstVertexIterator &cfgBeginVertex,
                      const Partitioner2::ControlFlowGraph::ConstVertexIterator &cfgEndVertex,
                      const Partitioner2::CfgConstVertexSet &cfgAvoidVertices = Partitioner2::CfgConstVertexSet(),
                      const Partitioner2::CfgConstEdgeSet &cfgAvoidEdges = Partitioner2::CfgConstEdgeSet());
    void
    setSearchBoundary(const Partitioner2::PartitionerConstPtr &partitioner,
                      const Partitioner2::CfgConstVertexSet &cfgBeginVertices,
                      const Partitioner2::CfgConstVertexSet &cfgAvoidVertices = Partitioner2::CfgConstVertexSet(),
                      const Partitioner2::CfgConstEdgeSet &cfgAvoidEdges = Partitioner2::CfgConstEdgeSet());
    void
    setSearchBoundary(const Partitioner2::PartitionerConstPtr &partitioner,
                      const Partitioner2::ControlFlowGraph::ConstVertexIterator &cfgBeginVertex,
                      const Partitioner2::CfgConstVertexSet &cfgAvoidVertices = Partitioner2::CfgConstVertexSet(),
                      const Partitioner2::CfgConstEdgeSet &cfgAvoidEdges = Partitioner2::CfgConstEdgeSet());
    /** @} */

    /** Property: Whether search is directed or not.
     *
     *  A directed search attempts to find a path that reaches one of a set of goal vertices, set by the @p setSearchBoundary
     *  functions that take a @c cfgEndVertex or @c cfgEndVertices argument.  On the other hand, an undirected search just
     *  keeps following paths to explore the entire execution space. */
    bool isDirectedSearch() const {
        return isDirectedSearch_;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Functions for searching for paths
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Find all feasible paths.
     *
     *  Searches for paths and calls the @p pathProcessor each time a feasible path is found. The space is explored using a
     *  depth first search, and the search can be limited with various @ref settings. */
    void depthFirstSearch(PathProcessor &pathProcessor);


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Functions for getting the results
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Property: Partitioner currently in use.
     *
     *  Returns a reference to the partitioner that is currently in use, set by @ref setSearchBoundary.  It is a fatal error to
     *  call this function if there is no partitioner. */
    Partitioner2::PartitionerConstPtr partitioner() const;

    /** Function summary information.
     *
     *  This is a map of functions that have been summarized, indexed by function entry address. */
    const FunctionSummaries& functionSummaries() const {
        return functionSummaries_;
    }

    /** Function summary information.
     *
     *  This is the summary information for a single function. If the specified function is not summarized then a
     *  default-constructed summary information object is returned. */
    const FunctionSummary& functionSummary(rose_addr_t entryVa) const;

    /** Details about a variable. */
    const VarDetail& varDetail(const InstructionSemantics::BaseSemantics::StatePtr&, const std::string &varName) const;

    /** Details about all variables by name. */
    const VarDetails& varDetails(const InstructionSemantics::BaseSemantics::StatePtr&) const;

    /** Get the initial state before the first path vertex. */
    InstructionSemantics::BaseSemantics::StatePtr initialState() const;

    /** Get the state at the end of the specified vertex. */
    static InstructionSemantics::BaseSemantics::StatePtr pathPostState(const Partitioner2::CfgPath&, size_t vertexIdx);

    /** Effective maximum path length.
     *
     *  Returns the effective maximum path length, k, for the specified path. The maximum is based on the @ref
     *  Settings::maxPathLength "maxPathLength" property, but adjusted up or down as vertices are added to the path. The
     *  adjusted values are stored as attributes of the path, and this function returns the current value. */
    double pathEffectiveK(const Partitioner2::CfgPath&) const;

    /** Total length of path up to and including the specified vertex.
     *
     *  The path length is different than the number of vertices (@ref Partitioner2::CfgPath::nVertices). Path length is
     *  measured by summing the sizes of all the vertices. The size of a vertex that represents a basic block is the number
     *  of instructions in that basic block. The path length is what's used to limit the depth of the search in k-bounded
     *  model checking. */
    static size_t pathLength(const Partitioner2::CfgPath&, int position = -1);

    /** Cumulative statistics about prior analyses.
     *
     *  These statistics accumulate across all analysis calls and can be reset by either @ref reset or @ref resetStatistics.
     *
     *  Thread safety: This method is thread safe. */
    Statistics statistics() const {
        SAWYER_THREAD_TRAITS::LockGuard lock(statsMutex_);
        return stats_;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Private supporting functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
    // Check that analysis settings are valid, or throw an exception.
    void checkSettings() const;

    static rose_addr_t virtualAddress(const Partitioner2::ControlFlowGraph::ConstVertexIterator &vertex);

    void insertCallSummary(const Partitioner2::ControlFlowGraph::ConstVertexIterator &pathsCallSite,
                           const Partitioner2::ControlFlowGraph &cfg,
                           const Partitioner2::ControlFlowGraph::ConstEdgeIterator &cfgCallEdge);

    boost::filesystem::path emitPathGraph(size_t callId, size_t graphId);  // emit paths graph to "rose-debug" directory

    // Pop an edge (or more) from the path and follow some other edge.  Also, adjust the SMT solver's stack in a similar
    // way. The SMT solver will have an initial state, plus one pushed state per edge of the path.
    void backtrack(Partitioner2::CfgPath &path /*in,out*/, const SmtSolverPtr&);

    // Process one edge of a path to find any path constraints. When called, the cpu's current state should be the virtual
    // machine state at it exists just prior to executing the target vertex of the specified edge.
    //
    // Returns a null pointer if the edge's assertion is trivially unsatisfiable, such as when the edge points to a basic block
    // whose address doesn't match the contents of the instruction pointer register after executing the edge's source
    // block. Otherwise, returns a symbolic expression which must be tree if the edge is feasible. For trivially feasible
    // edges, the return value is the constant 1 (one bit wide; i.e., true).
    SymbolicExpressionPtr pathEdgeConstraint(const Partitioner2::ControlFlowGraph::ConstEdgeIterator &pathEdge,
                                             const InstructionSemantics::BaseSemantics::DispatcherPtr &cpu);

    // Parse the expression if it's a parsable string, otherwise return the expression as is. */
    Expression parseExpression(Expression, const std::string &where, SymbolicExpressionParser&) const;

    SymbolicExpressionPtr expandExpression(const Expression&, const SymbolicExpressionParser&);

    // Based on the last vertex of the path, insert user-specified assertions into the SMT solver.
    void insertAssertions(const SmtSolverPtr&, const Partitioner2::CfgPath&,
                          const std::vector<Expression> &assertions, bool atEndOfPath, const SymbolicExpressionParser&);

    // Size of vertex. How much of "k" does this vertex consume?
    static size_t vertexSize(const Partitioner2::ControlFlowGraph::ConstVertexIterator&);

    // Information needed for adding user-supplied assertions to the solver.
    struct Substitutions {
        SymbolicExpressionParser exprParser;
        std::vector<Expression> assertions;
        SymbolicExpressionParser::RegisterSubstituter::Ptr regSubber;
        SymbolicExpressionParser::MemorySubstituter::Ptr memSubber;
    };

    // Insert the edge assertion and any applicable user assertions (after delayed expansion of the expressions' register
    // and memory references), and run the solver, returning its result.
    SmtSolver::Satisfiable
    solvePathConstraints(const SmtSolverPtr&, const Partitioner2::CfgPath&, const SymbolicExpressionPtr &edgeAssertion,
                         const Substitutions&, bool atEndOfPath);

    // Mark vertex as being reached
    void markAsReached(const Partitioner2::ControlFlowGraph::ConstVertexIterator&);

    // Top-level info for debugging
    void dfsDebugHeader(Sawyer::Message::Stream &trace, Sawyer::Message::Stream &debug, size_t callId, size_t graphId);

    // Top-level info for debugging a path.
    void dfsDebugCurrentPath(Sawyer::Message::Stream&, const Partitioner2::CfgPath&, const SmtSolverPtr&, size_t effectiveK);

    // Prepare substitutions for registers and memory based on user-supplied symbolic expressions.
    Substitutions parseSubstitutions();

    // Substitute registers and memory values into user-supplied symbolic expressions.
    void makeSubstitutions(const Substitutions&, const InstructionSemantics::BaseSemantics::RiscOperatorsPtr&);

    // Create an SMT solver. It will have an initial state plus, eventually, a transaction for each path edge.
    SmtSolverPtr createSmtSolver();

    // The parts of the instruction semantics framework
    struct Semantics {
        InstructionSemantics::BaseSemantics::DispatcherPtr cpu;
        InstructionSemantics::BaseSemantics::RiscOperatorsPtr ops;
        InstructionSemantics::BaseSemantics::StatePtr originalState;
    };

    // Create the parts of the instruction semantics framework.
    Semantics createSemantics(const Partitioner2::CfgPath&, PathProcessor&, const SmtSolverPtr&);

    // Convert a position to an index. Negative positions measure from the end of the sequence so that -1 refers to the last
    // element, -2 to the second-to-last element, etc. Non-negative positions are the same thing as an index.  Returns nothing
    // if the position is out of range.
    static Sawyer::Optional<size_t> positionToIndex(int position, size_t nElmts);

    // Obtain the incoming state for the specified path vertex. This is a pointer to the state, so copy it if you make changes.
    // The incoming state for the first vertex is the specified initial state.
    InstructionSemantics::BaseSemantics::StatePtr
    incomingState(const Partitioner2::CfgPath&, int position, const InstructionSemantics::BaseSemantics::StatePtr &initialState);

    // Number of steps (e.g., instructions) up to but not including the specified path vertex.
    size_t incomingStepCount(const Partitioner2::CfgPath&, int position);

    // Number of steps (e.g., instructions) up to and including the specified path vertex.
    size_t outgoingStepCount(const Partitioner2::CfgPath&, int position);

    // Evaluate semantics up to and including the specified path vertex, returning the outgoing state for that vertex. If
    // semantics fails, then returns a null state pointer.
    InstructionSemantics::BaseSemantics::StatePtr evaluate(Partitioner2::CfgPath&, int position, const Semantics&);

    // Check whether the last vertex of the path is feasible. Returns true if provably feasible, false if provably infeasible,
    // or indeterminate if not provable one way or the other.
    boost::logic::tribool isFeasible(Partitioner2::CfgPath&, const Substitutions&, const Semantics&, const SmtSolverPtr&);

    // Safely call the path processor's "found" action for the path's final vertex and return its value.
    PathProcessor::Action callPathProcessorFound(PathProcessor&, Partitioner2::CfgPath&, const Semantics&, const SmtSolverPtr&);

    // Given an effective K value, adjust it based on how often the last vertex of the path has been visited.  Returns a new
    // effective K. As a special case, the new K is zero if the last vertex has been visited too often.
    double adjustEffectiveK(Partitioner2::CfgPath&, double oldK);

    // Given a path that ends with a function call, inline the function or a summary of the function, adjusting the paths_
    // control flow graph.
    void summarizeOrInline(Partitioner2::CfgPath&, const Semantics&);
};

} // namespace
} // namespace

std::ostream& operator<<(std::ostream&, const Rose::BinaryAnalysis::FeasiblePath::Expression&);

// Convert string to feasible path expression during command-line parsing
namespace Sawyer {
    namespace CommandLine {
        template<>
        struct LexicalCast<Rose::BinaryAnalysis::FeasiblePath::Expression> {
            static Rose::BinaryAnalysis::FeasiblePath::Expression convert(const std::string &src) {
                return Rose::BinaryAnalysis::FeasiblePath::Expression(src);
            }
        };
    }
}

#endif
#endif
