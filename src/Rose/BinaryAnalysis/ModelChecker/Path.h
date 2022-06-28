#ifndef ROSE_BinaryAnalysis_ModelChecker_Path_H
#define ROSE_BinaryAnalysis_ModelChecker_Path_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/ModelChecker/Types.h>

#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/Types.h>
#include <Rose/BinaryAnalysis/SmtSolver.h>
#include <Rose/BinaryAnalysis/SymbolicExpr.h>
#include <Combinatorics.h>

namespace Rose {
namespace BinaryAnalysis {
namespace ModelChecker {

/** Execution path.
 *
 *  An execution path points to a node in the execution tree which serves as the last node of this path. The
 *  functions defined in this interface operate on paths as a whole instead of single nodes. */
class Path {
public:
    using Ptr = PathPtr;

private:
    PathNodePtr end_;

protected:
    Path() = delete;
    explicit Path(const PathNodePtr &end);
public:
    ~Path();

public:
    /** Construct a path that is one vertex in length.
     *
     *  This constructs a root path that contains only one execution unit.
     *
     *  Thread safety: This constructor is thread safe. */
    static Ptr instance(const ExecutionUnitPtr&);

    /** Construct a path by extending some other path.
     *
     *  The new path will extend the @p prefix path by adding the specified execution unit. An assertion must
     *  be provided that is already known to be true for the extended path (but it will not be checked by this
     *  method).
     *
     *  Thread safety: This constructor is thread safe. */
    static Ptr instance(const PathPtr &prefix, const ExecutionUnitPtr&, const SymbolicExpr::Ptr &assertion,
                        const SmtSolver::Evidence&, const InstructionSemantics::BaseSemantics::StatePtr &parentOutgoingState);

    /** Test for empty path.
     *
     *  Returns true if the path is empty, false otherwise.
     *
     *  Thread safety: This method is thread safe. */
    bool isEmpty() const;

    /** Length of path in nodes.
     *
     *  Returns the length of the path in nodes.
     *
     *  Thread safety: This method is thread safe. */
    size_t nNodes() const;

    /** Length of path in steps.
     *
     *  Returns the length of the path in steps, .
     *
     *  Thread safety: This method is thread safe. */
    size_t nSteps() const;

    /** Total time to process path.
     *
     *  Returns the total elapsed time to process this path by summing the elapsed times for each of this path's nodes.  The
     *  returned time is measured in seconds. It excludes nodes for which no timing information is available, such as nodes
     *  that have not been executed yet.
     *
     *  Thread safety: This method is thread safe. */
    double processingTime() const;

    /** Path hash.
     *
     *  Hashes a path by hashing the address of each node. This results in a useful identification number for a path that's
     *  stable across different runs of a tool.
     *
     *  Thread safety: This method is thread safe.
     *
     * @{ */
    uint64_t hash() const;
    void hash(Combinatorics::Hasher&) const;
    /** @} */

    /** Property: Whether execution has failed.
     *
     *  This property is true if there has been a previous attempt to execute this path and that attempt has failed.
     *
     *  Thread safety: This method is thread safe. */
    bool executionFailed() const;

    /** All SMT assertions along the path.
     *
     *  Returns the list of all assertions that are required along the specified path.
     *
     *  Thread safety: This method is thread safe. */
    std::vector<SymbolicExpr::Ptr> assertions() const;

    /** Last node of the path.
     *
     *  Returns the last node of the path if the path is not empty. Returns null if the path is empty.
     *
     *  Thread safety: This method is thread safe. */
    PathNodePtr lastNode() const;

    /** Nodes composing path.
     *
     *  Returns a vector of the nodes that make up the path in the order they were executed.
     *
     *  Note: If you're able to process the path from its end toward its beginning, then it's more efficient to use each node's
     *  @p parent pointer instead of creating a vector of all the pointers.
     *
     *  Thread safety: This method is thread safe. */
    std::vector<PathNodePtr> nodes() const;

    /** Short name suitable for printing in diagnostics.
     *
     *  Thread safety: This method is thread safe. */
    std::string printableName() const;

    /** Multi-line output of path.
     *
     *  Prints details about a path in a multi-line text format. Up to @p maxSteps steps are printed.
     *
     *  Thread safety: This method is thread safe although the output from this thread may interleave with output from other
     *  threads. */
    void print(const SettingsPtr&, std::ostream&, const std::string &prefix, size_t maxSteps) const;

    /** Multi-line output of path.
     *
     *  Prints details about a path in YAML format. Up to @p maxSteps steps are printed.
     *
     *  Thread safety: This method is thread safe although the output from this thread may interleave with output from other
     *  threads. */
    void toYaml(const SettingsPtr&, std::ostream&, const std::string &prefix, size_t maxSteps) const;


};

} // namespace
} // namespace
} // namespace

#endif
#endif
