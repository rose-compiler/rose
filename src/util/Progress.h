#ifndef Rose_Progress_H
#define Rose_Progress_H

#include <rosePublicConfig.h>                           // for ROSE_USE_CMAKE
#include <boost/chrono.hpp>
#include <boost/thread.hpp>
#include <rose_constants.h>
#include <rose_isnan.h>
#include <Sawyer/SharedPointer.h>
#include <Sawyer/Stopwatch.h>
#include <Sawyer/Synchronization.h>
#include <string>
#include <vector>

namespace Rose {

/** A general, thread-safe way to report progress made on some task.
 *
 *  Consider some long-running task that needs to be performed by some worker thread or collection of worker threads. There may
 *  be other unrelated listener threads that need to monitor the progress of the task, such as a GUI thread that updates a
 *  progress bar widget.  This can be accomplished in a general way by having an intermediatey object to which workers can
 *  write progress reports and other threads can query those reports.
 *
 *  The intention is that each long-running task will have its own @ref Progress object into which reports are written, and the
 *  report will be a floating point number between zero (work just started) and one (work is almost complete). When work on the
 *  task completes (either because the task was finished or it had an error), one of the workers calls @ref finished to let all
 *  the listeners know there will be nothing new to report.
 *
 *  Sometimes a task has multiple phases and it's hard to predict the total amount of work across all phases before earlier
 *  phases have completed. Therefore, progress reports have two parts: not only do they have a completion amount, they
 *  also have a phase name.  When a @ref Progress object is first created, its phase name is empty and the completion
 *  amount is zero.  There is no intrinsic @ref Progress requirement that phases occur in any particular order, or that the
 *  completion amount is monotonically increasing, or that the completion amount is in the interval [0..1] although listeners
 *  might be expecting certain things.
 *
 *  In order to support a hierarchy of reports, such as when one long running analysis calls other long-running analyses and
 *  they in turn also have multiple phases, the progress object has a stack of reports that is adjusted with @ref push and @ref
 *  pop methods.  When progress is reported to a listener, the listener has a choice between getting the name of the top-most
 *  report or getting a name that is constructed by joining all the non-empty names in the stack.  There's also a @ref
 *  ProgressTask class that does the push and pop using RAII.
 *
 *  Each @ref Progress object is shared between worker threads and querying threads--it cannot be deleted when the last worker
 *  is finished because other threads might still be listening, and it cannot be deleted when nothing is listening because
 *  workers might still need to write progress reports to it. Therefore each @ref Progress object is allocated on the heap and
 *  referenced through thread-safe, shared-ownership pointers. Allocation is done by the @ref instance factory and the object
 *  should not be explicitly deleted by the user.
 *
 *  Example:
 * @code
 *  // function that does lots of work and optionally reports its progress.
 *  void worker(Progress::Ptr progress) {
 *      for (size_t i = 0; i < 1000000; ++i) {
 *          do_some_work();
 *          if (progress)
 *              progress->update((double)i/1000000);
 *      }
 *      if (progress)
 *          progress->finished(1.0);
 *  }
 *
 *  // Start a worker and provide a way for it to report its progress
 *  Progress::Ptr progress = Progress::instance();
 *  boost::thread_t worker(worker, progress);
 *
 *  // Report progress on standard error each time it changes, but not more than once every 10 seconds
 *  progress->reportChanges(boost::chrono::seconds(10), [](const Progress::Report &rpt) -> bool {
 *      std::cout <<rpt.phase <<": " <<(100*rpt.completion) <<" percent completed\n";
 *      return true; // keep listening until task is finished
 *  });
 *
 *  // Or report progress every 10 seconds whether it changed or not
 *  progress->reportRegularly(boost::chrono::seconds(10), [](const Progress::Report &rpt, double age) -> bool {
 *      std::cout <<rpt.phase <<": " <<(100*rpt.completion) <<" percent completed\n";
 *      return true; // keep listening until task is finished
 *  });
 *
 *  // Or query the progress in an event loop
 *  while (!progress->isFinished()) {
 *      Progress::Report rpt = progress->reportLatest().first;
 *      std::cout <<rpt.phase <<": " <<(100*rpt.completion) <<" percent completed\n";
 *      do_other_stuff();
 *  }
 * @endcode
 *
 *  The following guidelines should be used when writing a long-running task, such as a ROSE analysis or transformation, that
 *  supports progress reporting. These guidelines assume that the task is encapsulated in a class (as most analyses and
 *  transformations should be) so that an application is able to have more than one instance of the task. A task that's
 *  implemented as a single function should take an argument of type <code>const Rose::Progress::Ptr&</code> (which may be a
 *  null progress object), and a task that's implemented in a namespace should try to provide an API similar to a class (the
 *  main difference will be that there's only once "instance" of the analysis).
 *
 *  @li The task class should have a private data member to hold a @ref Progress::Ptr.
 *
 *  @li The class should initialize its progress reporter variable to a non-null value by calling @ref Progress::instance
 *  during construction. The user can reset this to null if he really doesn't want the task to spend any time updating
 *  progress reports.
 *
 *  @li The class should provide an API for users to query or change the progress report for any particular task. The
 *  preferred protyptes are "Rose::Progress::Ptr progress() const" and "void progress(const Rose::Progress::Ptr&)" although you
 *  should use whatever scheme is consistent with the rest of your API.
 *
 *  @li The task should expect that the progress object could be null, in which case the user is saying that there's no need
 *  for the analyzer to spend time updating any progress reports because nobody will be listening for them.
 *
 *  @li The task should update the progress with a completion ratio between 0 and 1 without specifying a phase name (unless
 *  the analysis has multiple phases).  I.e., use <code>update(double)</code> instead of <code>update(Report)</code>.
 *
 *  @li If the task creates an inner task object that's able to report progress, then the same progress object should be
 *  assigned to the inner task.
 *
 *  @li If the task calls an inner task that uses the same progress object as the outer task (see previous bullet) then the
 *  outer task should use the @ref ProgressTask RAII aproach by declaring a variable like this: <code>ProgressTask pt(progress,
 *  "name_of_task", completion)</code>, where @c completion is the progress of the outer task after the inner task returns
 *  (either normally or by exception).
 *
 *  @li The task should call @ref finished when the analysis has completed if the progress object is non-null, even if the
 *  task has some other mechanism to notify listeners that it has completed.
 *
 *  Here's an example analysis that uses these guidelines:
 *
 * @code
 *  class OuterAnalyzer {
 *      Rose::Progress::Ptr progress_;
 *
 *  public:
 *      OuterAnalyzer(): progress_(Rose::Progress::instance()) {}
 *
 *      Rose::Progress::Ptr progress() const {
 *          return progress_;
 *      }
 *
 *      void progress(const Rose::Progress::Ptr &p) {
 *          progress_ = p;
 *      }
 *
 *      void run() {
 *          size_t totalExpectedWork = 3 * nSteps;
 *          size_t workCompleted = 0;
 *
 *          for (size_t step = 0; step < nSteps; ++step, workCompleted += 3) {
 *              if (progress_)
 *                  progress_->update((double)workCompleted / totalExpectedWork);
 *
 *              do_something();
 *              if(progress_)
 *                  progress_->update((double)(workCompleted+1) / totalExpectedWork);
 *
 *              // Run an inner analysis. Assume InnerAnalysis has the same API as OuterAnalysis
 *              {
 *                  ProgressTask pt(progress_, "inner", (double)(workCompleted+2) / totalExpectedWork);
 *                  InnerAnalysis inner;
 *                  inner.progress(progress_);
 *                  inner.run();
 *              }
 *
 *              // progress was already emitted by ProgressTask destructor
 *              do_something_more();
 *          }
 *
 *          if (progress_)
 *              progress_->finished(1.0);
 *      }
 *  };
 * @endcode */
class Progress: public Sawyer::SharedObject {
public:
    /** Progress objects are reference counted. */
    typedef Sawyer::SharedPointer<Progress> Ptr;

    /** A single progress report.
     *
     *  Progress is reported and monitored as a stream of progress reports, and this object represents one such report.
     *
     *  A progress report has a name, an amount completed, and a maximum value. The name is optional. The amount
     *  completed is normally a non-negative number and can be NAN if the completion amount is unknown.  The maximum is
     *  the expected upper range of the completion (defaulting to 1.0) and is used to calculate percents. If the maximum
     *  is NAN, then the percent completed cannot be computed and perhaps a busy indicator or spinner would be used instead
     *  of a progress bar to represent the state. */
    struct Report {
        std::string name;                               /**< What is being reported. Defaults to empty string. */
        double completion;                              /**< Estimated degree of completion. In [0..maximum] or NAN. */
        double maximum;                                 /**< Maximum value for completion. Defaults to 1.0. NAN => spinner. */

        /** Initial progress report. */
        Report()
            : completion(0.0), maximum(1.0) {}

        /** Report completion with default name. */
        explicit Report(double completion, double maximum = 1.0)
            : completion(completion), maximum(maximum) {}

        /** Report with name and completion. */
        Report(const std::string &name, double completion, double maximum = 1.0)
            : name(name), completion(completion), maximum(maximum) {}
    };

private:
    static const size_t TERMINATING = INVALID_INDEX;

    // Synchronized data members
    mutable SAWYER_THREAD_TRAITS::Mutex mutex_;         // protects the following data members
#if SAWYER_MULTI_THREADED
    mutable SAWYER_THREAD_TRAITS::ConditionVariable cv_;// for signaling changes to the latestReport_
#endif
    std::vector<Report> reports_;                       // report stack, one elemet per nested phase
    size_t reportNumber_;                               // sequence number of latestReport_, or TERMINATING
    Sawyer::Stopwatch reportAge_;                       // time since last report arrived (or since construction)

protected:
    Progress(): reportNumber_(0) {}

public:
    /** Factory to create a new instance of this class. */
    static Ptr instance();

public:
    /** Make a progress report.
     *
     *  This method is called by the threads that are doing the work in order to update the record of the amount of work they
     *  have completed.
     *
     *  @code
     *  void worker(Progress::Ptr progress) {
     *      for (i = 0; i < totalWork; ++i) {
     *          doSomeWork();
     *          if (progress)
     *              progress->update((double)i / totalWork);
     *      }
     *      if (progress)
     *          progress->finished();
     *  }
     *  @endcode
     *
     *  This updates the progress report for the innermost (top-of-stack) phase when there are nested phases.
     *
     *  Thread safety: This method is thread safe.
     *
     * @{ */
    void update(double completion, double maximum = 1.0);
    void update(const Report&);
    /** @} */

    /** Push a new progress phase onto the stack.
     *
     *  If some analysis needs to call another analysis and both want to report progress, the outer analysis may push a new
     *  phase onto the progress stack before calling the inner analysis.  Once the inner analysis returns, the outer analysis
     *  should pop the inner phase from the stack.
     *
     *  Passing a completion ratio or report argument is the same as calling @ref update first, except the two operations are
     *  atomic.  Pushing a new context (regardless of whether an argument was given) notifies listeners that the progress has
     *  changed.
     *
     *  Returns the previous report. The name for the previous report is always just the base name, not joined with any other
     *  levels in the stack.
     *
     * @{ */
    Report push();
    Report push(double completion, double maximum = 1.0);
    Report push(const Report&);
    /** @} */

    /** Pop the top progress phase from the stack.
     *
     *  This is intended to be called after one analysis calls another and the other has returned. Before the outer analysis
     *  calls the inner analysis, it should push a new record onto the progress stack, and after the inner analysis returns the
     *  outer analysis should pop that record.
     *
     *  Attempting to pop the final item from the stack is the same as calling @ref finished (it doesn't actually pop the final
     *  item). Otherwise, popping the stack notifies listeners that the progress has changed.
     *
     *  Passing a completion ratio or report argument is the same as calling @ref update after popping, except the two
     *  operations are atomic. Popping a context (regardless of whether an argument was given) notifies listeners that the
     *  progress has changed.
     *
     * @{ */
    void pop();
    void pop(double completion, double maximum = 1.0);
    void pop(const Report&);
    /** @} */

    /** Indicate that the task is complete.
     *
     *  This method is called by one (or more) of the threads doing the work in order to indicate that the work has been
     *  terminated, either because it was completed or there was an error, and that no more progress updates will be
     *  forthcoming.  If this progress object is nested (i.e., the report stack has more than one element) then the @ref
     *  finished method doesn't actually do anything because outer phases can still potentially update their progress.
     *
     *  If no worker thread calls @ref finished then the listeners will not know that work has finished and they may continue
     *  listening for progress updates indefinitely.  It is permissible to call @ref finished more than once.
     *
     *  Passing completion ratio or report argument has the same effect as calling @ref update first, except the two operations
     *  are atomic.  Each call to this method (regardless of whether an argument was specified) notifies listeners that the
     *  progress changed.
     *
     *  The @ref Progress API is not responsible for reporting task status (whether the workers were collectively successful or
     *  encountered an error). Status should be reported by the usual mechanisms, such as futures.
     *
     *  Thread safety: This method is thread safe.
     *
     * @{ */
    void finished();
    void finished(double completion, double maximum = 1.0);
    void finished(const Report&);
    /** @} */

    /** Predicate indicating whether the task is finished.
     *
     *  Returns true if the task which was being monitored has been terminated either because it finished or it had an
     *  error. If an inner phase calls @ref finished, it doesn't actually cause this phase object to be marked as finished
     *  because outer phases may continue to update their progress.
     *
     *  Thread safety: This method is thread safe. */
    bool isFinished() const;

    /** Latest report and its age in seconds.
     *
     *  Returns the most recent report that has been received and how long it's been (in seconds) since the report was
     *  received.  If no report has ever been received, then this returns a default-constructed report and the time since this
     *  Progress object was created; such reports have completion of zero.
     *
     *  If @p nameSeparator is not empty, then the returned report name is formed by joining all phase names on the report
     *  stack with the specified separator.
     *
     *  Thread safety: This method is thread safe. */
    std::pair<Report, double /*seconds*/> reportLatest(const std::string &nameSeparator = ".") const;

    /** Invoke the specified function at regular intervals.
     *
     *  The specified functor is invoked as soon as this method is called, and then every @p interval milliseconds thereafter
     *  until some thread calls @ref finished on this object or the functor returns false.  The functor is invoked with
     *  two arguments: the last known progress report and the time in seconds since it was reported.
     *
     *  If @p nameSeparator is not empty, then the returned report name is formed by joining all phase names on the report
     *  stack with the specified separator.
     *
     *  Return value: Returns false if the functor returned false, true if the reporting ended because the task is finished.
     *
     *  Example:
     * @code
     *  void reportPerSecond(std::ostream &output, Progress *progress) {
     *      progress->reportRegularly(boost::chrono::seconds(1), [&output](const Report &report, double age) {
     *          output <<(boost::format("%s %.0f%%\n") % report.name % (100.0 * report.completion));
     *      });
     *  }
     * @endcode
     *
     *  Thread safety: This method is thread safe. */
    template<class Functor>
    bool reportRegularly(boost::chrono::milliseconds interval, Functor f, const std::string &nameSeparator = ".") const {
        // Makes no sense for single threaded. Does not compile when ROSE is built with CMake
#if SAWYER_MULTI_THREADED && !defined(ROSE_USE_CMAKE)
        while (1) {
            std::pair<Report, double /*seconds*/> rpt = reportLatest(nameSeparator);
            if (!f(rpt.first, rpt.second))
                return false;
            if (isFinished())
                return true;
            boost::this_thread::sleep_for(interval);
        }
#else
        return false;
#endif
    }
    
    /** Invoke the specified function each time the progress changes.
     *
     *  The specified functor is invoked each time a worker thread updates the progress, but not more than the specified
     *  period.  The functor is called with one argument, the most recent progress report, and returns a Boolean. If the
     *  functor returns false then this function also immediately returns false, otherwise the functor is called until a worker
     *  thread indicates that the task is finished (either complete or had an error) by calling @ref finished.
     *
     *  If @p nameSeparator is not empty, then the returned report name is formed by joining all phase names on the report
     *  stack with the specified separator.
     *
     *  Example:
     * @code
     *  void reportChanges(std::ostream &output, Progress *progress) {
     *      progress->reportChanges(boost::chrono::seconds(1), [&output](const Report &report) {
     *          output <<(boost::format("%s %.0f%%\n") % report.name % (100.0 * report.completion));
     *      });
     *  }
     * @endcode
     *
     *  Thread safety: This method is thread safe. */
    template<class Functor>
    bool reportChanges(boost::chrono::milliseconds limit, Functor f, const std::string &nameSeparator = ".") const {
#if SAWYER_MULTI_THREADED && !defined(ROSE_USE_CMAKE)   // makes no sense for single threaded. Does not compile with cmake
        Sawyer::Stopwatch timer;
        size_t seen = TERMINATING - 1;
        while (1) {
            SAWYER_THREAD_TRAITS::UniqueLock lock(mutex_);
            while (reportNumber_ == seen)
                cv_.wait(lock);
            ASSERT_forbid(reports_.empty());
            Report report = reports_.back();
            report.name = reportNameNS(nameSeparator);
            seen = reportNumber_;
            lock.unlock();

            if (!f(report))
                return false;
            if (TERMINATING == seen)
                return true;
            boost::this_thread::sleep_for(limit);
        }
#else
        return false;
#endif
    }

private:
    std::string reportNameNS(const std::string &nameSeparator) const;
};

/** RAII sub-task progress.
 *
 *  Pushes a new context onto the specified progress reporting object in preparation for calling an analysis that does its own
 *  progress reporting.
 *
 *  Use this class like this. The completion ratios are arbitrary and we've assumed that do_something, inner both take roughly
 *  the same (large) amount of time.
 *
 * @code
 *  void outerTask(const Progress::Report &progress) {
 *      progress->update(Progress::Report(0.0));
 *      do_something(); // after this, we're 1/3 done
 *
 *      {
 *          ProgressTask t(progress, "inner", 0.67);
 *          inner(progress); // progress reports named "outer.inner" going from [0..1]
 *      }
 *
 *      progress->update(Progress::Report(0.67)); // optional since 0.67 was reported by destructor
 *      do_something();
 *  }
 * @endcode */
class ProgressTask {
    Progress::Ptr progress_;
    Progress::Report after_;

public:
    /** Prepare existing progress object for subtask. */
    ProgressTask(const Progress::Ptr &progress, const std::string &name, double afterCompletion = NAN)
        : progress_(progress) {
        if (progress_) {
            after_ = progress_->push(Progress::Report(name, 0.0));
            if (!rose_isnan(afterCompletion))
                after_.completion = afterCompletion;
        }
    }

    /** Create progress object for subtask. */
    ProgressTask(const std::string &name, double afterCompletion = NAN)
        : progress_(Progress::instance()) {
        after_ = progress_->push(Progress::Report(name, 0.0));
        if (!rose_isnan(afterCompletion))
            after_.completion = afterCompletion;
    }

    /** Clean up subtask progress. */
    ~ProgressTask() {
        if (progress_)
            progress_->pop(after_);
    }

public:
    /** Progress object being used.
     *
     *  This returns one of the following values:
     *  @li If @ref cancel has been called then return the null pointer.
     *  @li If constructed with a progress object, return that object (possibly null).
     *  @li Return the progress created by the constructor. */
    Progress::Ptr progress() const {
        return progress_;
    }

    /**  Cancel all cleanup operations.
     *
     *   If called, then the destructor will do nothing other than possibly releasing a reference to the progress object
     *   supplied to or created by the constructor. */
    void cancel() {
        progress_ = Progress::Ptr();
    }
};

} // namespace

#endif
