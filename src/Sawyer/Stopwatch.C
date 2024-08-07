// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          https://gitlab.com/charger7534/sawyer.git.




#include <Sawyer/Sawyer.h>
#include <Sawyer/Stopwatch.h>
#include <boost/format.hpp>
#include <iostream>

#if defined(SAWYER_HAVE_BOOST_CHRONO)
#   include <boost/chrono/duration.hpp>
#   include <boost/chrono/system_clocks.hpp>
#elif defined(BOOST_WINDOWS)
#   include <time.h>
#   include <windows.h>
#   undef ERROR                                         // not sure where this pollution comes from
#   undef max                                           // more pollution
#else // POSIX
#   include <sys/time.h>                                // gettimeofday() and struct timeval
#endif

namespace Sawyer {

static Stopwatch::TimePoint getCurrentTime() {
#if defined(SAWYER_HAVE_BOOST_CHRONO)
    return boost::chrono::high_resolution_clock::now();
#elif defined(BOOST_WINDOWS)
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);
    unsigned __int64 t = ft.dwHighDateTime;
    t <<= 32;
    t |= ft.dwLowDateTime;
    t /= 10;                                            // convert into microseconds
    //t -= 11644473600000000Ui64;                       // convert file time to microseconds since Unix epoch
    return t / 1e6;
#else // POSIX
    struct timeval t;
    if (-1==gettimeofday(&t, NULL))
        return 0.0;
    return t.tv_sec + 1e-6 * t.tv_usec;
#endif
}

SAWYER_EXPORT double
Stopwatch::report(bool clear) const {
    if (running_) {
        TimePoint now = getCurrentTime();
        elapsed_ += now - begin_;
        begin_ = now;
    }
#ifdef SAWYER_HAVE_BOOST_CHRONO
    double retval = elapsed_.count();
#else
    double retval = elapsed_;
#endif
    if (clear)
        elapsed_ = Duration();
    return retval;
}

SAWYER_EXPORT double
Stopwatch::start() {
    double retval = report(false);
    if (!running_) {
        begin_ = getCurrentTime();
        running_ = true;
    }
    return retval;
}

SAWYER_EXPORT double
Stopwatch::start(double value) {
    double retval = report(false);
    elapsed_ = Duration(value);
    if (!running_) {
        begin_ = getCurrentTime();
        running_ = true;
    }
    return retval;
}

SAWYER_EXPORT double
Stopwatch::restart() {
    return start(0.0);
}

SAWYER_EXPORT double
Stopwatch::stop(bool clear) {
    double retval = report(clear);
    running_ = false;
    return retval;
}

SAWYER_EXPORT double
Stopwatch::clear(double value) {
    double retval = stop();
    elapsed_ = Duration(value);
    return retval;
}

SAWYER_EXPORT std::string
Stopwatch::toString() const {
    return toString(report());
}

SAWYER_EXPORT std::string
Stopwatch::toString(double seconds) {
    if (seconds < 0.0) {
        return "negative-duration";
    } else if (seconds < 59.9999995) {
        return (boost::format("%1.6f seconds") % seconds).str();
    } else {
        unsigned ts = ::round(seconds);
        unsigned long days = ts / 86400;
        unsigned long hours = (ts % 86400) / 3600;
        unsigned long minutes = (ts % 3600) / 60;
        unsigned long s = ts % 60;

        if (days > 0) {
            return (boost::format("%dd%02dh%02dm%02ds") % days % hours % minutes % s).str();
        } else if (hours > 0) {
            return (boost::format("%dh%02dm%02ds") % hours % minutes % s).str();
        } else {
            return (boost::format("%dm%02ds") % minutes % s).str();
        }
    }
}

SAWYER_EXPORT std::ostream&
operator<<(std::ostream &out, const Stopwatch &x) {
    out <<x.toString();
    return out;
}

} // namespace
