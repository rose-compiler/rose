#ifndef ROSE_BinaryAnalysis_ModelChecker_Periodic_H
#define ROSE_BinaryAnalysis_ModelChecker_Periodic_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/ModelChecker/Types.h>
#include <condition_variable>
#include <mutex>
#include <thread>

namespace Rose {
namespace BinaryAnalysis {
namespace ModelChecker {

/** Do something periodically in another thread.
 *
 *  The pure virtual @p step method is called periodically and asynchronously until it's told to stop.
 *
 *  The calling thread invokes @ref start, which creates a new thread that will call the @ref step method. The @ref step method
 *  is implemented in the subclasses. */
class Periodic {
public:
    using Ptr = PeriodicPtr;
private:
    std::thread worker_;
    std::mutex mutex_;
    bool stopping_ = false;
    std::condition_variable cv_;

public:
    virtual ~Periodic();

    /** Creates a new thread to run periodically. */
    void start(const EnginePtr&, std::chrono::duration<double> period);

    /** Stop running periodically. */
    void stop();

    /** The work to be done.
     *
     *  This function is called periodically from the thread created by @ref start. */
    virtual void step(const EnginePtr&) = 0;

private:
    void run(const EnginePtr&, std::chrono::duration<double> period);
};

} // namespace
} // namespace
} // namespace

#endif
#endif
