#ifndef ROSE_BinaryAnalysis_FeasiblePath_H
#define ROSE_BinaryAnalysis_FeasiblePath_H

#include <BaseSemantics2.h>
#include <Partitioner2/CfgPath.h>
#include <Sawyer/Message.h>
#include <SMTSolver.h>
#include <boost/filesystem/path.hpp>

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
    enum SearchMode { SEARCH_SINGLE_DFS, SEARCH_SINGLE_BFS, SEARCH_MULTI };

    /** Settings that control this analysis. */
    struct Settings {
        SearchMode searchMode;                          /**< Method to use when searching for feasible paths. */
        Sawyer::Optional<rose_addr_t> initialStackPtr;  /**< Concrete value to use for stack pointer register initial value. */
        size_t vertexVisitLimit;                        /**< Max times to visit a particular vertex in one path. */
        size_t maxPathLength;                           /**< Limit path length in terms of number of instructions. */
        size_t maxCallDepth;                            /**< Max length of path in terms of function calls. */
        size_t maxRecursionDepth;                       /**< Max path length in terms of recursive function calls. */
        std::vector<SymbolicExpr::Ptr> postConditions;  /**< Additional constraints to be satisifed at the end of a path. */
        std::vector<rose_addr_t> summarizeFunctions;    /**< Functions to always summarize. */
        bool nonAddressIsFeasible;                      /**< Indeterminate/undiscovered vertices are feasible? */

        /** Default settings. */
        Settings()
            : searchMode(SEARCH_SINGLE_DFS), vertexVisitLimit((size_t)-1), maxPathLength((size_t)-1), maxCallDepth((size_t)-1),
              maxRecursionDepth((size_t)-1), nonAddressIsFeasible(true) {}
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
        SymbolicExpr::Ptr memAddress;                   /**< Address where variable is located. */
        size_t memSize;                                 /**< Size of total memory access in bytes. */
        size_t memByteNumber;                           /**< Byte number for memory access. */
        Sawyer::Optional<rose_addr_t> returnFrom;       /**< This variable is the return value from the specified function. */

        VarDetail(): firstAccessInsn(NULL), memSize(0), memByteNumber(0) {}
        std::string toString() const;
    };

    /** Path searching functor.
     *
     *  This is the base class for user-defined functors called when searching for feasible paths. */
    class PathProcessor {
    public:
        enum Action {
            BREAK,                                      /**< Do not look for more paths. */
            CONTINUE                                    /**< Look for more paths. */
        };

        virtual ~PathProcessor() {}
        virtual Action found(const FeasiblePath &analyzer, const Partitioner2::CfgPath &path,
                             const std::vector<SymbolicExpr::Ptr> &pathConditions,
                             const InstructionSemantics2::BaseSemantics::DispatcherPtr&,
                             SMTSolver &solver) = 0;
    };

    /** Information stored per V_USER_DEFINED path vertex.
     *
     *  This is information for summarized functions. */
    struct FunctionSummary {
        rose_addr_t address;                            /**< Address of summarized function. */
        int64_t stackDelta;                             /**< Stack delta for summarized function. */
        std::string name;                               /**< Name of summarized function. */

        /** Construct empty function summary. */
        FunctionSummary(): stackDelta(SgAsmInstruction::INVALID_STACK_DELTA) {}

        /** Construct function summary with information. */
        FunctionSummary(const Partitioner2::ControlFlowGraph::ConstVertexIterator &cfgFuncVertex, uint64_t stackDelta);
    };

    /** Summaries for multiple functions. */
    typedef Sawyer::Container::Map<rose_addr_t, FunctionSummary> FunctionSummaries;


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Private data members
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
    const Partitioner2::Partitioner *partitioner_;      // binary analysis context
    RegisterDictionary *registers_;                     // registers augmented with "path" pseudo-register
    RegisterDescriptor REG_RETURN_;                     // FIXME[Robb P Matzke 2016-10-11]: see source
    Settings settings_;
    FunctionSummaries functionSummaries_;
    Partitioner2::CfgVertexMap vmap_;                   // relates CFG vertices to path vertices
    Partitioner2::ControlFlowGraph paths_;              // all possible paths, feasible or otherwise
    Partitioner2::CfgConstVertexSet pathsBeginVertices_;// vertices of paths_ where searching starts
    Partitioner2::CfgConstVertexSet pathsEndVertices_;  // vertices of paths_ where searching stops
    Partitioner2::CfgConstEdgeSet cfgAvoidEdges_;       // CFG edges to avoid
    Partitioner2::CfgConstVertexSet cfgEndAvoidVertices_;// CFG end-of-path and other avoidance vertices


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Construction, destruction
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Constructs a new feasible path analyzer. */
    FeasiblePath()
        : registers_(NULL) {}

    virtual ~FeasiblePath() {}
        
    /** Reset to initial state without changing settings. */
    void reset() {
        partitioner_ = NULL;
        registers_ = NULL;
        REG_PATH = REG_RETURN_ = RegisterDescriptor();
        functionSummaries_.clear();
        vmap_.clear();
        paths_.clear();
        pathsBeginVertices_.clear();
        pathsEndVertices_.clear();
        cfgAvoidEdges_.clear();
        cfgEndAvoidVertices_.clear();
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


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Overridable processing functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Create the virtual CPU.
     *
     *  Creates a new virtual CPU for each call.  The first call also makes a copy of the register dictionary from the
     *  specified partitioner and augments it with a "path" pseudo-register that holds a symbolic expressions on which the
     *  current CFG path depends. */
    virtual InstructionSemantics2::BaseSemantics::DispatcherPtr
    buildVirtualCpu(const Partitioner2::Partitioner&);

    /** Initialize state for first vertex of path.
     *
     *  Given the beginning of the path and the virtual CPU, initialize that state with whatever is suitable for the
     *  analysis. The default implementation sets the "path" pseudo-register to true (since the first vertex of the path is
     *  unconditionally feasible), sets the instruction pointer register to the first instruction, and initializes the stack
     *  pointer with the concrete stack pointer from @ref settings (if any).  On x86, the DF register is set. */
    virtual void
    setInitialState(const InstructionSemantics2::BaseSemantics::DispatcherPtr &cpu,
                    const Partitioner2::ControlFlowGraph::ConstVertexIterator &pathsBeginVertex);

    /** Process instructions for one basic block on the specified virtual CPU.
     *
     *  This is a state transfer function, updating the virtual machine state by processing the instructions of the specified
     *  basic block. */
    virtual void
    processBasicBlock(const Partitioner2::BasicBlock::Ptr &bblock,
                      const InstructionSemantics2::BaseSemantics::DispatcherPtr &cpu, size_t pathInsnIndex);

    /** Process an indeterminate block.
     *
     *  This is a state transfer function, representing flow of control through an unknown address. */
    virtual void
    processIndeterminateBlock(const Partitioner2::ControlFlowGraph::ConstVertexIterator &vertex,
                              const InstructionSemantics2::BaseSemantics::DispatcherPtr &cpu,
                              size_t pathInsnIndex);

    /** Process a function summary vertex.
     *
     *  This is a state transfer function, representing flow of control across a summarized function. */
    virtual void
    processFunctionSummary(const Partitioner2::ControlFlowGraph::ConstVertexIterator &pathsVertex,
                           const InstructionSemantics2::BaseSemantics::DispatcherPtr &cpu,
                           size_t pathInsnIndex);

    /** Process one vertex.
     *
     *  This is the general state transfer function, representing flow of control through any type of vertex. */
    virtual void
    processVertex(const InstructionSemantics2::BaseSemantics::DispatcherPtr &cpu,
                  const Partitioner2::ControlFlowGraph::ConstVertexIterator &pathsVertex,
                  size_t &pathInsnIndex /*in,out*/);

    /** Determines whether a function call should be summarized instead of inlined. */
    virtual bool
    shouldSummarizeCall(const Partitioner2::ControlFlowGraph::ConstVertexIterator &pathVertex,
                        const Partitioner2::ControlFlowGraph &cfg,
                        const Partitioner2::ControlFlowGraph::ConstVertexIterator &cfgCallTarget);

    /** Determines whether a function call should be inlined. */
    virtual bool
    shouldInline(const Partitioner2::CfgPath &path, const Partitioner2::ControlFlowGraph::ConstVertexIterator &cfgCallTarget);


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

    /** Determine whether a single path is feasible.
     *
     *  Returns true if the path is feasible, false if not feasible, or indeterminate if a conclusion cannot be reached.  The
     *  @p postConditions are additional optional conditions that must be satisified at the end of the path.  The entire set of
     *  conditions is returned via @p pathConditions argument, which can also initially contain preconditions. */
    virtual boost::tribool
    isPathFeasible(const Partitioner2::CfgPath &path, SMTSolver&, const std::vector<SymbolicExpr::Ptr> &postConditions,
                   std::vector<SymbolicExpr::Ptr> &pathConditions /*in,out*/,
                   InstructionSemantics2::BaseSemantics::DispatcherPtr &cpu /*out*/);


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Functions for describing the search space
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:

    /** Specify search boundary.
     *
     *  This function initializes the analysis by specifying starting and ending CFG vertices and the vertices and edges that
     *  should be avoided.
     *
     * @{ */
    void
    setSearchBoundary(const Partitioner2::Partitioner &partitioner,
                      const Partitioner2::CfgConstVertexSet &cfgBeginVertices,
                      const Partitioner2::CfgConstVertexSet &cfgEndVertices,
                      const Partitioner2::CfgConstVertexSet &cfgAvoidVertices = Partitioner2::CfgConstVertexSet(),
                      const Partitioner2::CfgConstEdgeSet &cfgAvoidEdges = Partitioner2::CfgConstEdgeSet());
    void
    setSearchBoundary(const Partitioner2::Partitioner &partitioner,
                      const Partitioner2::ControlFlowGraph::ConstVertexIterator &cfgBeginVertex,
                      const Partitioner2::ControlFlowGraph::ConstVertexIterator &cfgEndVertex,
                      const Partitioner2::CfgConstVertexSet &cfgAvoidVertices = Partitioner2::CfgConstVertexSet(),
                      const Partitioner2::CfgConstEdgeSet &cfgAvoidEdges = Partitioner2::CfgConstEdgeSet());
    /** @} */


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Functions for searching for paths
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Find all feasible paths.
     *
     *  Searches for paths and calls the @p pathProcessor each time a feasible path is found. The space explored using a depth
     *  first search, and the search can be limited with various @ref settings. */
    void depthFirstSearch(PathProcessor &pathProcessor);


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Functions for getting the results
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Property: Partitioner currently in use.
     *
     *  Returns a reference to the partitioner that is currently in use, set by @ref setSearchBoundary.  It is a fatal error to
     *  call this function if there is no partitioner. */
    const Partitioner2::Partitioner& partitioner() const;
    
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
    const VarDetail& varDetail(const InstructionSemantics2::BaseSemantics::StatePtr &state, const std::string &varName) const;


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Private supporting functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
    static rose_addr_t virtualAddress(const Partitioner2::ControlFlowGraph::ConstVertexIterator &vertex);

    void insertCallSummary(const Partitioner2::ControlFlowGraph::ConstVertexIterator &pathsCallSite,
                           const Partitioner2::ControlFlowGraph &cfg,
                           const Partitioner2::ControlFlowGraph::ConstEdgeIterator &cfgCallEdge);

    boost::filesystem::path emitPathGraph(size_t callId, size_t graphId);  // emit paths graph to "rose-debug" directory
};

} // namespace
} // namespace

#endif
