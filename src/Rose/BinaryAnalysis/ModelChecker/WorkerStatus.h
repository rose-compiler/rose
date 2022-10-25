#ifndef ROSE_BinaryAnalysis_ModelChecker_WorkerStatus_H
#define ROSE_BinaryAnalysis_ModelChecker_WorkerStatus_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_MODEL_CHECKER

#include <Rose/BinaryAnalysis/ModelChecker/Types.h>

#include <Rose/Progress.h>
#include <boost/filesystem.hpp>

namespace Rose {
namespace BinaryAnalysis {
namespace ModelChecker {

/** Writes information about workers to a text file for debugging. */
class WorkerStatus {
public:
    /** Shared ownership pointer. */
    using Ptr = WorkerStatusPtr;

private:
    boost::filesystem::path fileName_;
    int fd_ = -1;

    struct Status {
        WorkerState state = WorkerState::STARTING;      // current state
        time_t stateChange = 0;                         // time of last state change
        Progress::Ptr progress;                         // progress reports
    };

    std::vector<Status> workers_;
    time_t lastFileUpdate_ = 0;                         // last time that file was updated

    static constexpr time_t fileUpdatePeriod_ = 1;      // update the file every so many seconds
    static constexpr size_t workersPerLine_ = 4;        // number of worker information per line
    static constexpr size_t barWidth_ = 20;             // width of the bar that says how long since a state change

protected:
    WorkerStatus(const boost::filesystem::path &fileName);

public:
    ~WorkerStatus();

public:
    /** Allocating constructor. */
    static Ptr instance(const boost::filesystem::path &fileName);

    /** Create a new worker.
     *
     *  The @p workerIdx should be a small integer. Workers are numbered consecutively starting at zero. */
    void insert(size_t workerIdx, const Progress::Ptr&);

    /** Change the state of a worker. */
    void setState(size_t workerIdx, WorkerState);

    /** Name of file to which status is written. */
    const boost::filesystem::path& fileName() const;

    /** Update the file if enough time has elapsed since the prior update. */
    void updateFile();

    /** Update file now regardless of how much time has elapsed since prior update. */
    void updateFileNow();
};

} // namespace
} // namespace
} // namespace

#endif
#endif
