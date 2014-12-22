// Tests the algorithms in util/ParallelSort.h
#include "ParallelSort.h"
#include "LinearCongruentialGenerator.h"
#include <cstring>
#include <iostream>
#include <sawyer/Stopwatch.h>

using namespace rose;

// Things we're sorting
struct Thing {
    int x, y;
    Thing(int x, int y): x(x), y(y) {}
};

// When are two things in sorted order?
struct Comparer {
    bool operator()(const Thing &a, const Thing &b) {
        return a.x < b.x || (a.x==b.x && a.y<b.y);
    }
} compare;

std::ostream& operator<<(std::ostream &o, const Thing &thing) {
    o <<"(" <<thing.x <<", " <<thing.y <<")";
    return o;
}

// usage: testSort NTHINGS NTHREADS
int main(int argc, char *argv[]) {
    size_t nvalues = 16;
    size_t nthreads = 1;
    if (argc>1)
        nvalues = strtoul(argv[1], 0, NULL);
    if (argc>2)
        nthreads = strtoul(argv[2], 0, NULL);

    std::cerr <<"Generating " <<nvalues <<" values... ";
    Sawyer::Stopwatch generation;
    LinearCongruentialGenerator random;
    std::vector<Thing> values;
    values.reserve(nvalues);
    for (size_t i=0; i<nvalues; ++i) {
        static const int maxval = 1000000;
        values.push_back(Thing(random() % maxval, random() % maxval));
    }
    std::cerr <<"done (" <<generation.stop() <<" seconds)\n";
                         
    std::cerr <<"Sorting with " <<nthreads <<" threads... ";
    Sawyer::Stopwatch sorting;
    rose::ParallelSort::quicksort(&values[0], &values[0]+values.size(), compare, nthreads);
    std::cerr <<"done (" <<sorting.stop() <<" seconds)\n";

    std::cerr <<"Checking results...\n";
    size_t nfailures = 0;
    static const size_t failureLimit = 100;
    for (size_t i=1; i<values.size() && nfailures<failureLimit; ++i) {
        if (!compare(values[i-1], values[i]) && compare(values[i], values[i-1])) {
            std::cerr <<"sort failed: values[" <<i-1 <<", " <<i <<"] = (" <<values[i-1] <<", " <<values[i] <<")\n";
            ++nfailures;
        }
    }
    if (nfailures>=failureLimit)
        std::cerr <<"additional failures suppressed.\n";

    return nfailures ? 1 : 0;
}
