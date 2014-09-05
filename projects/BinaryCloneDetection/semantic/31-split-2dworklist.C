#include "rose.h"
#include "rose_getline.h"
#include "ParallelSort.h"
#include <boost/thread.hpp>
#include <cerrno>

using namespace rose;
using namespace StringUtility;

static std::string argv0;

static void
usage(int exit_status)
{
    std::cerr <<"usage: " <<argv0 <<" [--file=PAIRS] [--threads=N] [--] NPARTS < PAIRS\n"
              <<"  This command reads pairs of integers from either standard input or the file whose name is specified with\n"
              <<"  the --file switch and partitions them into NPARTS parts.  The algorithm treats the pairs as 2d points and\n"
              <<"  recursively subdivides the domain into smaller and smaller rectangular regions.  The goal is to keep the\n"
              <<"  size of the regions as closely balanced as possible, where \"size\" is defined as the number of unique\n"
              <<"  X and Y values represented by points in that region.\n"
              <<"\n"
              <<"  The quality of the result is limited by the fact that we use a fast, greedy algorithm.\n";
    exit(exit_status);
}

static struct Switches {
    std::string input_file_name;                        // non-empty means read coords from a file instead of stdin
    size_t nthreads;                                    // number of threads to use for sorting (counting self)
    Switches(): nthreads(1) {}
} opt;


enum Dimension { DIM_X=0, DIM_Y=1, DIM_NONE=2 };

Dimension opposite(Dimension dim) {
    assert(DIM_X==dim || DIM_Y==dim);
    return DIM_X==dim ? DIM_Y : DIM_X;
}

struct Point {
    int val[2];
    Point(int x, int y) {
        val[DIM_X] = x;
        val[DIM_Y] = y;
    }
};

static std::vector<Point> points;

struct PointCompare {
    const Dimension dim;
    PointCompare(Dimension dim): dim(dim) {}
    bool operator()(const Point &a, const Point &b) {
        return a.val[dim] < b.val[dim];
    }
};

typedef std::pair<size_t, size_t> IdxRange;


class WorkItem {
    IdxRange range_;
    mutable boost::optional<size_t> nUnique_[2];        // cached estimate of the number of unique values in each dimension
    Dimension sorted_;                                  // dimension by which points are sorted
public:
    // needed for std::priority_queue but not actually used
    WorkItem(): range_(0, 0), sorted_(DIM_NONE) {}

    // Construct a new work item for the entire vector of points before they are ever sorted
    explicit WorkItem(const IdxRange &range): range_(range), sorted_(DIM_NONE) {}

    // Construct a new work item when the points are already known to be sorted in one of the dimensions
    explicit WorkItem(const IdxRange &range, Dimension sortedDim): range_(range), sorted_(sortedDim) {
        assert(DIM_X==sortedDim || DIM_Y==sortedDim);
    }

    // Sort points by the specified dimension over a particular range.
    void sortPoints(Dimension dim, const IdxRange &range) {
        assert(DIM_X==dim || DIM_Y==dim);
        if (sorted_!=dim) {
            // Use pointers rather than iterators because the latter might have debugging enabled, which makes them
            // contend for locks in such a way that using more threads makes the sort slower, not faster.
            assert(!points.empty());                    // or else points[0] is invalid
            assert(range.first <= range.second);
            assert(range.second <= points.size());
            ParallelSort::quicksort(&points[0]+range.first, &points[0]+range.second, PointCompare(dim), opt.nthreads);
            sorted_ = dim;
        }
#ifndef NDEBUG
        for (size_t i=range.first+1; i<range.second; ++i)
            assert(points[i-1].val[sorted_] <= points[i].val[sorted_]);
#endif
    }

    // Estimate the number of unique values within a sorted range.  We do this in O(N) time by hashing the values to
    // a hash table and then measuring the size of the hash table.
    size_t nUnique(Dimension dim) const {
        if (!nUnique_[dim]) {
            static const int hashSize = 1123597;               // prime total number of buckets
            uint8_t hashTable[hashSize];
            memset(hashTable, 0, sizeof hashTable);
            size_t estimate = 0;
            for (size_t i=range_.first; i<range_.second; ++i) {
                int bucket = points[i].val[dim] % hashSize;
                if (0==hashTable[bucket])
                    ++estimate;
                hashTable[bucket] = 1;
            }
            nUnique_[dim] = estimate;
        }
        return *nUnique_[dim];
    }

    // Given a vector V of monotonically increasing integers of size N>0, choose an index i such that
    //    V[j] < V[i] for all j < i    (i is at a boundary between two values of the vector)
    //    |i - N/2| is minimized       (i is as close to the center of the vector as possible)
    // Example: given the vector
    //    V = (3, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 6)
    //    idx: 0  1  2  3  4  5  6  7  8  9 10 11       N=12
    //                 lo       mid            hi
    // Returns 3 since lo is closer to the midpoint than hi.  This allows the vector to be partitioned into two parts each
    // containing distinct values and being as close as possible in size, namely:
    //    V1 = (3, 4, 4)  V2 = (5, 5, 5, 5, 5, 5, 5, 5, 6)
    size_t medianIdx(Dimension dim, const IdxRange &range) const {
        assert(range.first < range.second);                 // range cannot be empty
        assert(sorted_==dim);
        size_t rangeSize = range.second - range.first;
        size_t midIdx = range.first + rangeSize / 2;        // middle:  range.first <= midIdx < range.second
        int midValue = points[midIdx].val[dim];
        size_t loIdx = midIdx;                              // lower boundary: range.first <= loIdx <= midIdx
        while (loIdx>range.first && points[loIdx-1].val[dim]==midValue)
            --loIdx;
        size_t hiIdx = midIdx+1;                            // upper boundary: midIdx < hi <= range.second
        while (hiIdx+1<range.second && points[hiIdx+1].val[dim]==midValue)
            ++hiIdx;
        size_t retval = midIdx-loIdx < hiIdx-midIdx ? loIdx : hiIdx; // the boundary closest to the center
        assert(retval >= range.first && retval < range.second);
        return retval;
    }

    // Comparison for queue based on number of unique items in either dimension
    bool operator<(const WorkItem &other) const {
        return std::max(nUnique(DIM_X), nUnique(DIM_Y)) < std::max(other.nUnique(DIM_X), other.nUnique(DIM_Y));
    }

    // Return true if this work item can be split in two
    bool canSplit() const {
        return nUnique(DIM_X) > 1 || nUnique(DIM_Y) > 1;
    }

    // Return the dimension that would give the best split.
    Dimension bestSplitDimension() const {
        assert(canSplit());
        return nUnique(DIM_X) > nUnique(DIM_Y) ? DIM_X : DIM_Y;
    }

    // Split this work item into two along the dimension that has the most unique values.  This item is modified in place to
    // become the left part and the right part is returned.  Both parts will have at least one point.
    WorkItem split() {
        Dimension splitDim = bestSplitDimension();
        size_t nelmts = range_.second - range_.first;
        std::cerr <<"split: range=("
                  <<std::setw(9) <<range_.first <<" + " <<std::setw(9) <<range_.second <<" = " <<std::setw(9) <<nelmts
                  <<");\tnUnique=("
                  <<std::setw(7) <<nUnique(DIM_X) <<", " <<std::setw(7) <<nUnique(DIM_Y) <<");"
                  <<" split on " <<(DIM_X==splitDim?"X":"Y")
                  <<(splitDim==sorted_ ? "\n" : " (sort required)\n");

        sortPoints(splitDim, range_);
        size_t idx = medianIdx(splitDim, range_);       // split point
        IdxRange right(idx, range_.second);             // range for the return value
        range_.second = idx;                            // our range is the left part, excluding idx
        WorkItem retval(right, splitDim);               // already sorted in one of the dimensions

        // recount our unique items since our range is smaller now
        nUnique_[DIM_X] = nUnique_[DIM_Y] = boost::optional<size_t>();
        return retval;
    }

    // Emit list of points for output
    void print(std::ostream &o) const {
        o <<"## CUT ##\n";
        for (size_t i=range_.first; i<range_.second; ++i) {
            const Point &point = points[i];
            o <<point.val[DIM_X] <<" " <<point.val[DIM_Y] <<"\n";
        }
    }
};

static std::ostream& operator<<(std::ostream &o, const WorkItem &item) {
    item.print(o);
    return o;
}

typedef std::priority_queue<WorkItem> Worklist;

// Read points from the fiel and append them to the global points arrays
static void loadPoints(const std::string &input_name, FILE *f) {
    char *line = NULL;
    size_t line_sz = 0, line_num = 0;
    while (rose_getline(&line, &line_sz, f)>0) {
        ++line_num;
        if (char *c = strchr(line, '#'))
            *c = '\0';
        char *s = line + strspn(line, " \t\r\n"), *rest;
        if (!*s)
            continue; // blank line

        errno = 0;
        int x = strtol(s, &rest, 0);
        if (errno!=0 || rest==s) {
            std::cerr <<argv0 <<": " <<input_name <<":" <<line_num <<": syntax error: x value expected\n";
            exit(1);
        }
        s = rest;

        errno = 0;
        int y = strtol(s, &rest, 0);
        if (errno!=0 || rest==s) {
            std::cerr <<argv0 <<": " <<input_name <<":" <<line_num <<": syntax error: y value expected\n";
            exit(1);
        }
        s = rest;

        while (isspace(*s)) ++s;
        if (*s) {
            std::cerr <<argv0 <<": " <<input_name <<":" <<line_num <<": syntax error: extra text after y value\n";
            exit(1);
        }

        points.push_back(Point(x, y));
    }
}

int
main(int argc, char *argv[])
{
    std::ios::sync_with_stdio();
    argv0 = argv[0];
    {
        size_t slash = argv0.rfind('/');
        argv0 = slash==std::string::npos ? argv0 : argv0.substr(slash+1);
        if (0==argv0.substr(0, 3).compare("lt-"))
            argv0 = argv0.substr(3);
    }

    // Parse switches
    size_t nparts = 1;
    int argno = 1;
    for (/*void*/; argno<argc && '-'==argv[argno][0]; ++argno) {
        if (!strcmp(argv[argno], "--")) {
            ++argno;
            break;
        } else if (!strcmp(argv[argno], "--help") || !strcmp(argv[argno], "-h")) {
            usage(0);
        } else if (!strncmp(argv[argno], "--file=", 7)) {
            opt.input_file_name = argv[argno]+7;
        } else if (!strncmp(argv[argno], "--threads=", 10)) {
            opt.nthreads = strtoul(argv[argno]+10, NULL, 0);
        } else {
            std::cerr <<argv0 <<": unrecognized command-line switch: " <<argv[argno] <<"\n";
            exit(1);
        }
    }
    if (argno+1==argc) {
        // number of parts
        char *rest;
        errno = 0;
        nparts = strtoul(argv[argno], &rest, 0);
        if (errno || rest==argv[argno] || *rest || 0==nparts) {
            std::cerr <<argv0 <<": needs a positive value for '--nparts=N' switch; got '" <<argv[argno] <<"'\n";
            exit(1);
        }
    } else {
        usage(1);
    }

    // Read function pairs (2d coordinates) from standard input or the file.
    if (opt.input_file_name.empty()) {
        std::cerr <<argv0 <<": reading points from stdin...\n";
        loadPoints("stdin", stdin);
    } else {
        FILE *in = fopen(opt.input_file_name.c_str(), "r");
        if (NULL==in) {
            std::cerr <<argv0 <<": " <<strerror(errno) <<": " <<opt.input_file_name <<"\n";
            exit(1);
        }
        loadPoints(opt.input_file_name, in);
        fclose(in);
    }
    std::cerr <<argv0 <<": read " <<StringUtility::plural(points.size(), "points") <<"\n";

    Worklist worklist;
    worklist.push(WorkItem(IdxRange(0, points.size())));
    size_t nemitted = 0;                                // number of work items already emitted to std::cout

    // Repeatedly split the list of points until we've split enough or we can't split more
    while (nemitted + worklist.size() < nparts && !worklist.empty()) {
#if 0 /*DEBUGGING [Robb P. Matzke 2014-01-29]*/
        std::cerr <<"progress: emitted " <<nemitted <<"; worklist " <<worklist.size() <<"\n";
#endif
#if 0 /*DEBUGGING [Robb P. Matzke 2014-01-29]*/
        dumpWorkList(worklist);
#endif
        WorkItem item1 = worklist.top();
        worklist.pop();
        if (item1.canSplit()) {
            WorkItem item2 = item1.split();
            worklist.push(item1);
            worklist.push(item2);
        } else {
            std::cout <<item1;
            ++nemitted;
        }
    }

    // Emit all remaining sets
    std::cerr <<"emitting worklist results...\n";
    while (!worklist.empty()) {
        std::cout <<worklist.top();
        worklist.pop();
        ++nemitted;
    }

    return 0;
}
