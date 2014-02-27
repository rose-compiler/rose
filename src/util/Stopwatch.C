#include "Stopwatch.h"
#include <cstdlib>

namespace rose {

#ifdef _MSC_VER
// If Windows doesn't have struct timeval then it probably doesn't have gettimeofday() either.  Our definition will always
// fail, preventing a Stopwatch object from ever entering a "running" state.
static int gettimeofday(struct timeval *tv, void *) {
    return -1;
}
#endif

double Stopwatch::start(bool clear) {
    double retval = elapsed_;
    if (clear) stop(true);
    if (!running_) {
        if (-1!=gettimeofday(&begin_, NULL))
            running_ = true;
    }
    return retval;
}

double Stopwatch::stop(bool clear) {
    if (running_) {
        struct timeval end;
        gettimeofday(&end, NULL);
        elapsed_ += (end.tv_sec - begin_.tv_sec) + (1e-6*end.tv_usec - 1e-6*begin_.tv_usec);
        running_ = false;
    }
    double retval = elapsed_;
    if (clear)
        elapsed_ = 0.0;
    return retval;
}

} // namespace
