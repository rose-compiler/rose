#include <boost/thread.hpp>
#include <boost/cstdint.hpp>
#include <iostream>

#define NWORKERS 12ul
#define WORKER_MAJOR 10000ul
#define WORKER_MINOR 1000ul

boost::mutex globalAccumulatorMutex;
static boost::uint64_t globalAccumulator = 0;

void
worker() {
    for (boost::uint64_t i = 0; i < WORKER_MAJOR; ++i) {
        boost::uint64_t accum = 0;
        for (boost::uint64_t j = 0; j < WORKER_MINOR; ++j) {
            accum = (accum << 13) ^ (accum >> 7);
            accum += j;
        }

        boost::lock_guard<boost::mutex> lock(globalAccumulatorMutex);
        globalAccumulator += accum;
    }
}

int
main() {
    boost::thread *workers = new boost::thread[NWORKERS];
    for (size_t i = 0; i < NWORKERS; ++i)
        workers[i] = boost::thread(worker);
    for (size_t i = 0; i < NWORKERS; ++i)
        workers[i].join();
    delete[] workers;
    std::cout <<globalAccumulator <<"\n";

    // Done this way for 32-bit compilers
    return
        (( globalAccumulator        & 0xfffffffful) == 0xe257aec0ul &&
         ((globalAccumulator >> 32) & 0xfffffffful) == 0x0a31b218ul)
        ? 0 : 1;
}
