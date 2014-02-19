#include "Stopwatch.h"
#include <cstdlib>

#ifdef _MSC_VER
#   include <time.h>
#   include <windows.h>
#else // POSIX
#   include <sys/time.h>                                // gettimeofday() and struct timeval
#endif

namespace rose {

// Portable version of gettimeofday()
int Stopwatch::getCurrentTime(Timeval *retval) {
#ifdef _MSC_VER
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);
    unsigned __int64 t = ft.dwHighDateTime;
    t <<= 32;
    t |= ft.dwLowDateTime;
    t /= 10;                                            // convert into microseconds
    //t -= 11644473600000000Ui64;                       // convert file time to microseconds since Unix epoch
    retval->seconds = t / 1000000ul;
    retval->useconds = t % 1000000ul;
#else // POSIX
    struct timeval t;
    if (-1==gettimeofday(&t, NULL))
        return -1;
    retval->seconds = t.tv_sec;
    retval->useconds = t.tv_usec;
#endif
    return 0;
}

// Accumulate run time and report it (and optionally reset to zero) without starting or stopping. This method is conceptually
// const, but it caches the last result gettimeofday() to avoid having to make extra system calls.
double Stopwatch::report(bool clear) const {
    if (running_) {
        Timeval now;
        if (0==getCurrentTime(&now))
            elapsed_ += (now.seconds - begin_.seconds) + (1e-6*now.useconds - 1e-6*begin_.useconds);
    }
    double retval = elapsed_;
    if (clear)
        elapsed_ = 0.0;
    return retval;
}

// Optionally clear, then start if necessary.  Does not start if gettimeofday returned false.
double Stopwatch::start(bool clear) {
    double retval = report(clear);
    if (!running_) {
        if (-1!=getCurrentTime(&begin_))
            running_ = true;
    }
    return retval;
}

// Stop if necessary and accumulate and report run time, optionally resetting.
double Stopwatch::stop(bool clear) {
    double retval = report(clear);
    running_ = false;
    return retval;
}

// Stop if necessary and reset to specified value after returning accumulated run time
double Stopwatch::clear(double value) {
    double retval = stop();
    elapsed_ = value;
    return retval;
}

} // namespace
