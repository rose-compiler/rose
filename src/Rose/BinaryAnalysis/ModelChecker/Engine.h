#ifndef ROSE_BinaryAnalysis_ModelChecker_Engine_H
#define ROSE_BinaryAnalysis_ModelChecker_Engine_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_MODEL_CHECKER

#include <Rose/BinaryAnalysis/ModelChecker/PathQueue.h>

#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/BasicTypes.h>
#include <Rose/BinaryAnalysis/SmtSolver.h>
#include <Rose/BinaryAnalysis/Partitioner2/BasicTypes.h>
#include <Sawyer/Stopwatch.h>
#include <boost/filesystem.hpp>
#include <boost/thread/thread.hpp>
#include <thread>

namespace Rose {
namespace BinaryAnalysis {
namespace ModelChecker {

/** Main class for model checking.
 *
 *  This class holds or references everything needed for model checking and runs the model checker. Most interaction with
 *  the model checker framework is through this object.
 *
 *  The engine maintains a priority queue of paths on which to work, and the work is farmed out to some combination of
 *  managed worker threads and/or unmanaged user threads. A unit of work is generally one node (the last node) of a path,
 *  which consists of executing the node symbolically, finding a list of control flow successor nodes, creating a new path
 *  for each of those successors, testing whether any of the new paths are feasible (using an SMT solver), and adding the
 *  feasible paths to the priority queue. The new paths are thus one node longer than the original path.
 *
 *  Part of the execution of a node involves checking execution state against a model specification. For instance, checking
 *  that no memory reads or writes occur to addresses within the first reserved page of memory. If a violation of the
 *  specification is detected, then a tag describing the violation can be attached to the path and the path can be added to a
 *  second priority queue that holds the "interesting" paths.
 *
 *  The engine is mainly responsible containing the prioritiy queues, managing worker threads and knowing about user threads,
 *  and handing work out to the threads.  Many of the components of model checking are user-defined specializations of model
 *  checker base classes, and the @ref Engine is reponsible for pointing to all of them so their virtual functions can be
 *  called at the appropriate times. */
class Engine final {
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Types
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Reference counting pointer. */
    using Ptr = EnginePtr;

    /** Information about a path that is in progress. */
    struct InProgress {
        InProgress();
        explicit InProgress(const PathPtr&);
        ~InProgress();

        PathPtr path;                                   /**< The path being worked on. */
        Sawyer::Stopwatch elapsed;                      /**< Wall clock time elapsed for this work. */
        boost::thread::id threadId;                     /**< Identifier for thread doing the work. */
        int tid = 0;                                    /**< Linux thread ID. */
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Data members
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
    // Unsynchronized data members
    PathQueue frontier_;                                // paths with work remaining
    PathQueue interesting_;                             // paths that are interesting, placed here by workers
    SemanticCallbacksPtr semantics_;                    // various configurable semantic operations

private:
    // Synchronized data members
    mutable SAWYER_THREAD_TRAITS::Mutex mutex_;         // protects all following data members
    Sawyer::Container::Map<boost::thread::id, InProgress> inProgress_;// work that is in progress
    SAWYER_THREAD_TRAITS::ConditionVariable newWork_;   // signaled when work arrives or thread finishes
    SAWYER_THREAD_TRAITS::ConditionVariable newInteresting_; // signaled when interesting paths arrive or threads finish
    PathPredicatePtr frontierPredicate_;                // predicate for inserting paths into the "frontier" queue
    PathPredicatePtr interestingPredicate_;             // predicate for inserting paths into the "interesting" queue
    SettingsPtr settings_;                              // overall settings
    std::vector<std::thread> workers_;                  // managed worker threads
    size_t workCapacity_ = 0;                           // managed workers plus user threads
    size_t nWorking_ = 0;                               // number of threads currently doing work
    size_t nPathsExplored_ = 0;                         // number of path nodes executed, i.e., number of paths explored
    size_t nStepsExplored_ = 0;                         // number of steps executed
    bool stopping_ = false;                             // when true, workers stop even if there is still work remaining
    std::vector<std::pair<double, size_t>> fanout_;     // total fanout and number of nodes for each level of the forest
    size_t nFanoutRoots_ = 0;                           // number of execution trees for calculating total forest size
    Sawyer::Stopwatch elapsedTime_;                     // time since model checking started
    mutable Sawyer::Stopwatch timeSinceStats_;          // time since last statistics were reported
    mutable size_t nPathsStats_ = 0;                    // number of paths reported in last statistics output
    size_t nExpressionsTrimmed_ = 0;                    // number of symbolic expressions trimmed down to a new variable
    WorkerStatusPtr workerStatus_;                      // mostly for debugging

protected:
    Engine() = delete;
    explicit Engine(const SettingsPtr&);

public:
    /** Allocating constructor.
     *
     *  Creates a model checker engine with default settings. */
    static Ptr instance();

    /** Allocating constructor.
     *
     *  Creates a model checker engine with specified settings. */
    static Ptr instance(const SettingsPtr&);

    /** Destructor.
     *
     *  The destructor will wait for all worker threads to finish before this object is destroyed. */
    ~Engine();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Configuration functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Property: Settings.
     *
     *  Various settings used throughout the model checking framework.
     *
     *  Thread safety: This property accessor is thread safe. The settings themselves are not thread safe and should generally
     *  not be modified once worker threads are created.
     *
     *  @{ */
    SettingsPtr settings() const;
    void settings(const SettingsPtr&);
    /** @} */

    /** Property: Adjustments to semantic operations.
     *
     *  All configurable semantic operations are encapsulated in this property. This should be set before running the engine,
     *  and not changed once the engine is running.
     *
     *  Thread safety: This property accessor is thread safe.
     *
     *  @{ */
    SemanticCallbacksPtr semantics() const;
    void semantics(const SemanticCallbacksPtr&);
    /** @} */

    /** Property: Path exploration prioritizer.
     *
     *  The specified prioritizer will be used to specify the order by which outstanding work is performed.  The default is
     *  to explore the virtual execution tree with a depth-first search.
     *
     *  The prioritizer can be changed while the model checker is running, which has the effect of immediately re-sorting the
     *  outstanding work. Any worker threads that are already working on a particular path will continue to do so even if other
     *  paths now have higher priority.
     *
     *  The path prioritizer cannot be a null pointer.
     *
     *  Thread safety: This property accesor is thread safe.
     *
     *  @{ */
    PathPrioritizerPtr explorationPrioritizer() const;
    void explorationPrioritizer(const PathPrioritizerPtr&);
    /** @} */

    /** Property: Work insertion predicate.
     *
     *  This predicate is the gatekeeper for inserting paths into the work queue. Any path other than those inserted as part
     *  of initialization gets checked by this predicate and inserted only if the predicate returns true.  The predicate will
     *  be called exactly once for each attempt to insert a path.
     *
     *  The predicate cannot be null. The default predicate always returns true.
     *
     *  Thread safety: This property accessor is thread safe.
     *
     *  @{ */
    PathPredicatePtr explorationPredicate() const;
    void explorationPredicate(const PathPredicatePtr&);
    /** @} */

    /** Property: Interesting insertion predicate.
     *
     *  This predicate is the gatekeeper for inserting paths into the "interesting" queue. Each attempt to insert an interesting
     *  path through the model checker's API will result in exactly one call to this predicate. If the predicate returns true, then
     *  the path is inserted.  Inserting a path using the @ref interesting @ref PathQueue directly bypasses this predicate.
     *
     *  The predicate cannot be null. The default predicate returns true if the path's final node has one or more associated
     *  tags.
     *
     *  Thread safety: This property accessor is thread safe.
     *
     *  @{ */
    PathPredicatePtr interestingPredicate() const;
    void interestingPredicate(const PathPredicatePtr&);
    /** @} */


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Initialization functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Reset engine to initial state.
     *
     *  Resets this engine to its initial state without changing settings. Counters, queues, etc. are cleared.
     *
     *  Thread safety: This method is not thread safe. */
    void reset();

    /** Insert a path starting point.
     *
     *  Each starting point represents the root of a virtual execution tree that will be explored in parallel with all other
     *  work according to a user-defined path prioritizer.  As many starting points as desired can be inserted, but no starting
     *  point should be inserted more than once.
     *
     *  Insertion of starting points is not limited by the @ref explorationPredicate unless @p prune is `YES` and the @ref
     *  explorationPredicate is not null.
     *
     *  Thread safety: This method is thread safe. New starting points can be inserted before the model checker starts
     *  executing, or during execution.
     *
     * @{ */
    void insertStartingPoint(const ExecutionUnitPtr&, Prune = Prune::NO);
    void insertStartingPoint(const PathPtr&, Prune = Prune::NO);
    /** @} */


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Model checker execution
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Start multiple worker threads.
     *
     *  Model checker can be done with any combination of worker threads managed by the model checking framework, and user
     *  threads that call model checker functions to do their own work.
     *
     *  This function starts @p n worker threads that perform model checking, and then immediately returns. When there is no
     *  work left to be done, all worker threads exit. Thus it's important to create work first (by inserting some starting
     *  points) before calling this function, otherwise all the workers will just immediately exit without doing anything.
     *
     *  Although it's usually called once per round of work, this function can be called more than once to increase the
     *  number of workers by @p n each time. If @p n is zero, then workers will be increased only up to the hardware
     *  parallelism. No workers are ever killed by this function.
     *
     *  See also, @ref workCapacity.
     *
     *  Thread safety: This method is thread safe. In fact, it can even be called from within another worker. */
    void startWorkers(size_t n = 0);

    /** Complete all work.
     *
     *  Blocks until all work is completed.
     *
     *  If there are managed workers, then the calling thread blocks without doing any work until all the workers are
     *  finished. On the other hand, if there are no managed workers then the calling thread will participate in the work even
     *  if other user threads are also working. This method can be called by as many user threads as desired, each of which
     *  will either block waiting for workers to exit, or will itself participate in the work.
     *
     *  Thread safety: This method is thread safe. However, it must not be called recursively or from within a managed
     *  worker. */
    void run();

    /** Complete all current work and then stop.
     *
     *  Blocks until the current work in progress finishes, joins the worker threads, and then returns. Work pending
     *  on the work queue is not removed.
     *
     *  Thread safety: This method is thread safe. */
    void stop();

    /** Run one step of checking in the calling thread.
     *
     *  When work is immediately available, this function performs that work and returns true.
     *
     *  When work is not immediately available (even if another thread might create work later), this function does nothing
     *  and returns false.
     *
     *  Thread safety: This method is thread safe. */
    bool step();

    /** Test whether work remains to be done.
     *
     *  Returns true if there is work left to be done according to the work queue, or any threads are working and might still
     *  add more items to the work queue. If this method is called from a thread that is working (in the WORKING state) then
     *  it must necessarily return true since this thread could still create more work.
     *
     *  Thread safety: This method is thread safe. However, if it's pointless to call it from a thread that's doing work
     *  since it will always return true in that situation. */
    bool workRemains() const;

    /** Work capacity.
     *
     *  This is the total number of workers working or waiting for work, plus the number of user threads currently doing work
     *  in this model checker. The simple act of calling this function does not necessarily mean the caller is doing work. In
     *  order to be "doing work", a thread needs to actually be working on a path (see @ref step and @ref run).
     *
     *  Thread safety: This method is thread safe. */
    size_t workCapacity() const;

    /** Number of threads that are working.
     *
     *  Returns the number of managed workers and user threads that are doing work. This does not count managed workers that
     *  are blocked waiting for work to become available.
     *
     *  Thread safety: This method is thread safe. */
    size_t nWorking() const;

    /** Number of paths to explore.
     *
     *  Returns the number of paths waiting to be explored.
     *
     *  Thread safety: This method is thread safe. */
    size_t nPathsPending() const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Model checker status
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Property: Elapsed time for model checking.
     *
     *  Returns the time in seconds since model checking started.
     *
     *  Thread safety: This method is thread safe. */
    Sawyer::Stopwatch elapsedTime() const;

    /** Property: Number of paths explored.
     *
     *  This read-only property contains the number of paths explored.
     *
     *  Thread safety: This method is thread safe. */
    size_t nPathsExplored() const;

    /** Property: Number of steps explored.
     *
     *  This is similar to the number of paths explored (@ref nPathsExplored) except it measures individual steps. Some
     *  path components (such as basic blocks) have multiple steps (instructions).
     *
     *  Thread safety: This method is thread safe. */
    size_t nStepsExplored() const;

    /** Property: Paths waiting to be explored.
     *
     *  Returns the (read-only) queue of paths that are waiting to be explored. From this, one can count the number of pending
     *  paths, and measure various properties of those paths.
     *
     *  Thread safety: This method is thread safe. The returned object is a reference valid while this model checker @ref
     *  Engine object exists. The referenced returned object may be changing by other threads, so only thread-safe methods
     *  should be called. */
    const PathQueue& pendingPaths() const;

    /** Paths that are currently in progress.
     *
     *  Returns a copy of the information about paths that are in progress at the time this function is called.
     *
     *  Thread safety: This method is thread safe. */
    std::vector<InProgress> inProgress() const;

    /** Number of symbolic expressions trimmed to a new variable.
     *
     *  This is the number of times a large symbolic expression was replaced by a new variable.
     *
     *  Thread safety: This method is thread safe. */
    size_t nExpressionsTrimmed() const;

    /** Print some statistics.
     *
     *  Thread safety: This method is thread safe. */
    void showStatistics(std::ostream&, const std::string &prefix = "") const;

    /** Write worker status to a text file.
     *
     *  The specified file is created or truncated, and then updated continuously until this object is destroyed or this method
     *  is called with a different file name.  An empty file name turns this feature off.
     *
     *  Thread safety: This method is thread safe.
     *
     * @{ */
    boost::filesystem::path workerStatusFile() const;
    void workerStatusFile(const boost::filesystem::path&);
    /** @} */

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Model checker results
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Property: The interesting results queue.
     *
     *  As workers discover interesting things, they will insert those paths into the "interesting" queue. The queue can be
     *  accessed directly using this method.
     *
     *  The interesting results can be re-sorted at any time by changing the prioritizer property of the returned path queue.
     *
     *  Thread safety: This method is thread safe, but the returned queue is valid only as long as this model checker is valid.
     *
     * @{ */
    const PathQueue& interesting() const;
    PathQueue& interesting();
    /** @} */

    /** Take next interesting result.
     *
     *  This takes the next path from the interesting queue if the queue is not empty. If the queue is empty, then this call
     *  blocks until something is placed on the queue or until all work has completed. If all work has completed without
     *  anything being placed on the queue, this function returns a null pointer.
     *
     *  If a non-null path is returned, then that path is also atomically inserted into the @ref inProgress set.
     *
     *  Do not call this in a single-threaded application or it may deadlock since there are no other threads creating
     *  interesting results. Instead, use @c interesting()->takeNext() which does not block.
     *
     *  Thread safety: This method is thread safe. */
    PathPtr takeNextInteresting();

    /** Potentially insert a path into the interesting queue.
     *
     *  The specified path is checked by the @ref interestingPredicate and if the predicate returns true then the path is
     *  inserted into the @ref interesting queue.  Returns true if inserted, false if rejected.  To unconditionally insert
     *  a path, insert it using the @ref interesting @ref PathQueue directly.
     *
     *  Thread safety: This method is thread safe. */
    bool insertInteresting(const PathPtr&);

    /** Estimated execution forest size to K levels.
     *
     *  Returns the estimated number of nodes in the execution trees to K levels (execution steps) deep based on the
     *  average fanout per step.
     *
     *  Thread safety: This method is thread safe. */
    double estimatedForestSize(size_t k) const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Worker functions. It is useful to call these from user code.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Insert a path into the execution tree.
     *
     *  The insertion is subject to the @ref explorationPredicate. If the predicate returns false then the path is not inserted.
     *
     *  Returns true if inserted, false if not inserted.
     *
     *  Thread safety: This method is thread safe. */
    bool insertWork(const PathPtr&);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Internal stuff.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
    // Update the fanout parameters used to estimate execution tree size.  The nChildren are the number of new paths created by
    // extending the current path, totalSteps is the number of steps in the whole path, and lastSteps is the number of steps in
    // the last node of the path.
    void updateFanout(size_t nChildren, size_t totalSteps, size_t lastSteps);

    // Called by each worker thread when they start. Users should not call this even though it's public. The ID is the index
    // of the managed worker in the workers_ vector.
    void worker(size_t id, const Progress::Ptr&);

    // Change state. The thread should hold its current state in a local variable. The state transition edges affect how
    // various counters are updated. All threads must start by initializing their current state to STARTING and then calling
    // changeState to also specify the new state is STARTING. Only certain transitions are allowed--read the source. The
    // pathHash is optional and should be the hash for the path on which the worker is actively working.
    void changeState(size_t workerId, WorkerState &currentState, WorkerState newState, uint64_t pathHash);

    // Non-synchronized version of changeState
    void changeStateNS(size_t workerId, WorkerState &currentState, WorkerState newState, uint64_t pathHash);

    // Do things that should be done when work is started.
    void startWorkingNS();

    // Called by changeState when a worker has finished.
    void finishWorkingNS();

    // Get the next path on which to work.  This function blocks until either more work is available, or no more work can
    // possibly be available. In the former case, it removes the highest priority path from the work queue, changes the work
    // state to WORKING, and returns the path. In the latter case, it returns a null pointer.
    //
    // Thread safety: This method is thread safe.
    PathPtr takeNextWorkItem(size_t workerId, WorkerState&);

    // Non-blocking version of takeNextWorkItem.
    PathPtr takeNextWorkItemNow(WorkerState&);

    // Execute a path. This goes back as far as necessary to get a known state, and then executes from there to the end
    // of the path.
    //
    // After a path is executed, it is conditionally added to the "interesting" queue according to the @ref
    // interestingPredicate.
    //
    // The solver argument is the solver used by model checker, which might be different than the optional solver already
    // attached to the RiscOperators object.
    //
    // Thread safety: The RISC operators must be thread-local, otherwise thread safe. All paths will have an identical
    // set of states for this path after this returns.
    void execute(const PathPtr&, const InstructionSemantics::BaseSemantics::RiscOperatorsPtr&, const SmtSolver::Ptr&);

    // Produce more work, typically by looking at the instruction pointer register for the outgoing state of a previous
    // path (which is not currently in the work queue) and creating new paths.
    void extend(const PathPtr&, const InstructionSemantics::BaseSemantics::RiscOperatorsPtr&, const SmtSolver::Ptr&);

    // Perform one step of model checking.
    void doOneStep(const PathPtr&, const InstructionSemantics::BaseSemantics::RiscOperatorsPtr&, const SmtSolver::Ptr&);

    // Indicates that a path has been finished.
    //
    // This should be called by a worker thread whenever its work on a path finishes, regardless of whether the work was
    // successful or not.  It removes the path from the @ref inProgress list.
    //
    // Thread safety: This method is thread safe.
    void finishPath(const InstructionSemantics::BaseSemantics::RiscOperatorsPtr&);

    // Display diagnostics about where variables appear in path constraints.
    void displaySmtAssertions(const PathPtr&);
};


} // namespace
} // namespace
} // namespace

#endif
#endif
