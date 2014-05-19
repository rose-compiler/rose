// Simple timer for real elapsed time.  Operates similar to a stopwatch.
#ifndef ROSE_Stopwatch_H
#define ROSE_Stopwatch_H

namespace rose {

/** Simple elapsed time.
 *
 * @code
 *  Stopwatch stopwatch; // starts immediately unless false argument is given
 *  do_something();
 *  std::cerr <<"that took " <<stopwatch.stop() <<" seconds.\n";
 * @endcode
 *
 *  All times are returned as floating point number of seconds.  The underlying data structure has microsecond resolution
 *  although the system call might not be capable of reporting at that resolution.
 *
 *  A stopwatch can be started and stopped as often as desired and it will accumulate the total amount of time it has been in
 *  the running state (the clear() method resets the stopwatch by stopping it and zeroing the total).  Starting an already
 *  running stopwatch does nothing, and neither does stopping an already stopped stopwatch.
 *
 *  When a stopwatch is copied the destination stopwatch inherits the accumulated time and running state of the source. Both
 *  instances are then completely independent of one another.
 *
 *  Thread safety: These functions are not thread-safe, although multiple threads can invoke the methods concurrently on
 *  different objects.  It is permissible for different threads to invoke methods on the same object provided that
 *  synchronization occurs above the method calls.
 *
 *  See also, AstPerformance
 */
class Stopwatch {
    // Cached value from last call to gettimeofday(). We store the values as integer seconds and microseconds rather than
    // floating point to avoid the loss of precision when subtracting to very similar floating point values.  We use our
    // own struct rather than struct timeval to avoid having OS-specific conditional compilation in this header.
    struct Timeval {
        unsigned long seconds, useconds;
    };
    mutable Timeval begin_;

    // Sum of elapsed run time time in seconds
    mutable double elapsed_;

    // State of the stopwatch: running or not.
    bool running_;

    // Similar to gettimeofday, but portable
    static int getCurrentTime(Timeval*);

public:
    /** Construct and optionally start a timer.
     *
     *  The timer is started immediately unless the constructor is invoked with a false argument. */
    explicit Stopwatch(bool start=true)
        : elapsed_(0.0), running_(false) {
        start && this->start();
    }

    /** Stop and reset the timer to the specified value.
     *
     *  The timer is stopped if it is running.  Returns the accumulated time before resetting it to the specified value. */
    double clear(double value=0.0);

    /** Start the timer and report accumulated time.
     *
     *  Reports the time accumulated as of this call, resets the accumulated time to zero if @p clear is true, then restarts
     *  the stopwatch if not already running. */
    double start(bool clear=false);

    /** Stop the timer and report accumulated time.
     *
     *  If the timer is running then it is stopped and the elapsed time is accumulated, otherwise the timer
     *  remains in the stopped state an no additional time is accumulated.  If @p clear is set, then the accumlated time is
     *  reset to zero.  In any case, the return value is the accumulated time before being optionally reset to zero. */
    double stop(bool clear=false);

    /** Report current accumulated time without stopping or starting.
     *
     *  If @p clear is set then the accumulated time is reset to zero without affecting the return value or the running state,
     *  and the stopwatch starts reaccumulating time as of this call. */
    double report(bool clear=false) const;

    /** Query state of stopwatch.
     *
     *  Returns true if and only if the stopwatch is running. */
    bool isRunning() const { return running_; }
};

} // namespace
#endif
