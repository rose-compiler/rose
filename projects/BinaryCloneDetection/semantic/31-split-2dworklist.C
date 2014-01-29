#include "rose.h"
#include "rose_getline.h"

#include <cerrno>

using namespace StringUtility;

static std::string argv0;

static void
usage(int exit_status)
{
    std::cerr <<"usage: " <<argv0 <<" [--file=PAIRS] [--] NPARTS < PAIRS\n"
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

typedef std::pair<size_t, size_t> IdxRange;

// Stores a list of unsorted points, and also stores two sets of indirections to those points: one for sorting by X values
// and another for sorting by Y values.  This allows us to simultaneously sort in two orders.  The order of the points themselves
// are never adjusted after inserting them.
class Points {
    std::vector<Point> points_;
    std::vector<size_t> indirect_[2];                   // indirection for sorted X and Y values
public:
    size_t size() const {
        assert(points_.size()==indirect_[DIM_X].size());
        assert(points_.size()==indirect_[DIM_Y].size());
        return points_.size();
    }

    // Insert another point into this container
    void insert(const Point &point) {
        assert(points_.size()==indirect_[DIM_X].size());
        assert(points_.size()==indirect_[DIM_Y].size());
        indirect_[DIM_X].push_back(points_.size());
        indirect_[DIM_Y].push_back(points_.size());
        points_.push_back(point);
    }

    // Get a point by an indirect index
    const Point& get(Dimension sortDimension, size_t idx) const {
        assert(DIM_X==sortDimension || DIM_Y==sortDimension);
        assert(idx<indirect_[sortDimension].size());
        idx = indirect_[sortDimension][idx];
        assert(idx<points_.size());
        return points_[idx];
    }

    // Get an X or Y value by indirect index
    int get(Dimension sortDimension, size_t idx, Dimension selectDimension) const {
        return get(sortDimension, idx).val[selectDimension];
    }

    // Swap two points by indirection.  This moves the indirection but not the points.
    void swap(Dimension sortDimension, size_t idx1, size_t idx2) {
        assert(DIM_X==sortDimension || DIM_Y==sortDimension);
        assert(idx1<indirect_[sortDimension].size() && idx2<indirect_[sortDimension].size());
        std::swap(indirect_[sortDimension][idx1], indirect_[sortDimension][idx2]);
    }

    // Make the opposite dimension's indirection point to the same points as the specified dimension for the given range.
    void regroup(Dimension keepDimension, const IdxRange &range) {
        Dimension moveDimension = opposite(keepDimension);
        for (size_t i=range.first; i<range.second; ++i)
            indirect_[moveDimension][i] = indirect_[keepDimension][i];
    }

    // Return field widths for dump(). The first value is the field width for indices, the second for values.
    std::pair<size_t, size_t> fieldWidth(const IdxRange &range) const {
        if (range.first >= range.second)
            return std::pair<size_t, size_t>(0, 0);
        int maxval = std::max(points_[range.first].val[DIM_X], points_[range.first].val[DIM_Y]);
        for (size_t i=range.first+1; i<range.second; ++i)
            maxval = std::max(maxval, std::max(points_[i].val[DIM_X], points_[i].val[DIM_Y]));
        std::ostringstream maxval_ss;
        maxval_ss <<maxval;
        size_t maxval_w = maxval_ss.str().size();
        std::ostringstream maxidx_ss;
        maxidx_ss <<(points_.size()-1);
        size_t maxidx_w = maxidx_ss.str().size();
        return std::make_pair(maxidx_w, maxval_w);
    }

    // Dump data for debugging.
    void dump(std::ostream &o, const IdxRange &range, const std::string &prefix) {
        dump(o, range, fieldWidth(range), prefix);
    }
    void dump(std::ostream &o, const IdxRange &range, const std::pair<size_t, size_t> width, const std::string &prefix) {
        size_t idxwidth = width.first;
        size_t valwidth = width.second;
        for (size_t i=range.first; i<range.second; ++i) {
            o <<prefix
              <<std::setw(idxwidth) <<i <<": ("
              <<std::setw(valwidth) <<points_[i].val[DIM_X] <<", "
              <<std::setw(valwidth) <<points_[i].val[DIM_Y]
              <<")    "
              <<std::setw(idxwidth) <<indirect_[DIM_X][i] <<": ("
              <<std::setw(valwidth) <<points_[indirect_[DIM_X][i]].val[DIM_X] <<", "
              <<std::setw(valwidth) <<points_[indirect_[DIM_X][i]].val[DIM_Y]
              <<")    "
              <<std::setw(idxwidth) <<indirect_[DIM_Y][i] <<": ("
              <<std::setw(valwidth) <<points_[indirect_[DIM_Y][i]].val[DIM_X] <<", "
              <<std::setw(valwidth) <<points_[indirect_[DIM_Y][i]].val[DIM_Y]
              <<")\n";
        }
    }
};

// Partition a range of points around a pivot value specified by index and return the new index of the pivot value. All values
// to the left of the pivot index will be less than the pivot value.
static size_t partition(Points &points, Dimension dim, const IdxRange &range, size_t pivotIdx) {
    assert(range.first<range.second);                   // cannot be empty
    assert(pivotIdx>=range.first && pivotIdx<range.second);
    int pivotValue = points.get(dim, pivotIdx, dim);
    points.swap(dim, pivotIdx, range.second-1);         // move pivot element to the end
    size_t storeIdx = range.first;
    for (size_t i=range.first; i<range.second-1; ++i) {
        if (points.get(dim, i, dim) < pivotValue)
            points.swap(dim, i, storeIdx++);
    }
    points.swap(dim, range.second-1, storeIdx);         // move pivot to its final place
    return storeIdx;
}

#if 0 /* [Robb P. Matzke 2014-01-28]: currently unused */
// Return the Nth smallest value within the specified range of values
static const Point& select(Points &points, Dimension dim, IdxRange range, size_t nth) {
    assert(range.first < range.second);                 // range cannot be empty
    size_t rangeSize = range.second - range.first;
    assert(nth < rangeSize);                            // nth is zero-origin
    while (rangeSize > 1) {
        size_t pivotIdx = range.first + random() % rangeSize;
        pivotIdx = partition(points, dim, range, pivotIdx); // the pivot value is now in its final position
        if (nth == pivotIdx) {
            return points.get(dim, nth);
        } else if (nth < pivotIdx) {
            range.second = pivotIdx;
        } else {
            range.first = pivotIdx + 1;
        }
        rangeSize = range.second - range.first;
    }
    return points.get(dim, nth);
}
#endif

// Sort the points in the specified range according to one dimension
static void quicksort(Points &points, Dimension dim, const IdxRange &range) {
    if (range.first + 1 >= range.second)
        return;                                         // nothing to sort
    size_t rangeSize = range.second - range.first;
    size_t pivotIdx = range.first + random() % rangeSize;
    pivotIdx = partition(points, dim, range, pivotIdx);
    IdxRange left(range.first, pivotIdx);               // excludes pivot point
    IdxRange right(pivotIdx+1, range.second);           // excludes pivot point
    if (left.second-left.first > right.second-right.first)
        std::swap(left, right);                         // make left range the smaller one
    quicksort(points, dim, left);
    quicksort(points, dim, right);                      // so tail recursion is the larger one
}

// Count the number of unique values within a sorted range
static size_t nUnique(const Points &points, Dimension dim, const IdxRange &range) {
    if (range.first == range.second)
        return 0;
    size_t retval = 1;
    int prevValue = points.get(dim, range.first, dim);
    for (size_t i=range.first+1; i<range.second; ++i) {
        int nextValue = points.get(dim, i, dim);
        if (nextValue != prevValue)
            ++retval;
        prevValue = nextValue;
    }
    return retval;
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
static size_t medianIdx(const Points &points, Dimension dim, const IdxRange &range) {
    assert(range.first < range.second);                 // range cannot be empty
    size_t rangeSize = range.second - range.first;
    size_t midIdx = range.first + rangeSize / 2;        // middle:  range.first <= midIdx < range.second
    int midValue = points.get(dim, midIdx, dim);
    size_t loIdx = midIdx;                              // lower boundary: range.first <= loIdx <= midIdx
    while (loIdx>range.first && points.get(dim, loIdx-1, dim)==midValue)
        --loIdx;
    size_t hiIdx = midIdx+1;                            // upper boundary: midIdx < hi <= range.second
    while (hiIdx+1<range.second && points.get(dim, hiIdx+1, dim)==midValue)
        ++hiIdx;
    size_t retval = midIdx-loIdx < hiIdx-midIdx ? loIdx : hiIdx; // the boundary closest to the center
    assert(retval >= range.first && retval < range.second);
    return retval;
}

class WorkItem {
    Points *points_;
    IdxRange range_;
    size_t nUnique_[2];                                 // number of unique values in each dimension
public:
    // needed for std::priority_queue but not actually used
    WorkItem(): points_(NULL), range_(-1, -1) {
        nUnique_[DIM_X] = nUnique_[DIM_Y] = -1;
    }
    
    // Construct a new work item for the entire vector of points
    explicit WorkItem(Points &points): points_(&points) {
        range_ = IdxRange(0, points_->size());
        quicksort(*points_, DIM_X, range_);
        quicksort(*points_, DIM_Y, range_);
        nUnique_[DIM_X] = nUnique(*points_, DIM_X, range_);
        nUnique_[DIM_Y] = nUnique(*points_, DIM_Y, range_);
    }

    // Construct a  new work item when one of the dimensions is already sorted
    WorkItem(Points &points, const IdxRange &range, Dimension sortedDim): points_(&points), range_(range) {
        assert(range_.first <= range_.second);
        Dimension unsortedDim = opposite(sortedDim);
        quicksort(*points_, unsortedDim, range_);
#ifndef NDEBUG
        for (size_t i=range_.first+1; i<range_.second; ++i)
            assert(points_->get(sortedDim, i-1, sortedDim) <= points_->get(sortedDim, i, sortedDim));
#endif
        nUnique_[DIM_X] = nUnique(*points_, DIM_X, range_);
        nUnique_[DIM_Y] = nUnique(*points_, DIM_Y, range_);
    }

    // Comparison for queue based on number of unique items in either dimension
    bool operator<(const WorkItem &other) const {
        assert(points_!=NULL);
        return std::max(nUnique_[DIM_X], nUnique_[DIM_Y]) < std::max(other.nUnique_[DIM_X], other.nUnique_[DIM_Y]);
    }

    // Return true if this work item can be split in two
    bool canSplit() const {
        assert(points_!=NULL);
        return nUnique_[DIM_X] > 1 || nUnique_[DIM_Y] > 1;
    }

    // Return the dimension that would give the best split.
    Dimension bestSplitDimension() const {
        assert(canSplit());
        return nUnique_[DIM_X] > nUnique_[DIM_Y] ? DIM_X : DIM_Y;
    }

    // Split this work item into two along the dimension that has the most unique values.  This item is modified in place to
    // become the left part and the right part is returned.  Both parts will have at least one point.
    WorkItem split() {
        assert(points_!=NULL);
        Dimension splitDim = bestSplitDimension();
        Dimension otherDim = opposite(splitDim);
#if 0 /*DEBUGGING [Robb P. Matzke 2014-01-29]*/
        std::cerr <<"splitting [" <<range_.first <<"," <<range_.second <<"] on the " <<(splitDim==DIM_X?"X":"Y") <<" axis\n";
#endif
        points_->regroup(splitDim, range_);                 // regroup the other dimension to correspond to splitDim
        size_t idx = medianIdx(*points_, splitDim, range_); // split point
#if 0 /*DEBUGGING [Robb P. Matzke 2014-01-29]*/
        std::cerr <<"  at index " <<idx <<"\n";
#endif
        IdxRange right(idx, range_.second);                 // range for the return value
        range_.second = idx;                                // our range is the left part, excluding idx
        quicksort(*points_, otherDim, range_);              // other dimension is unsorted because of the regroup() above
        nUnique_[DIM_X] = nUnique(*points_, DIM_X, range_); // the range changed, so we need to recompute the
        nUnique_[DIM_Y] = nUnique(*points_, DIM_Y, range_); // ...number of unique values in each dimension
        return WorkItem(*points_, right, splitDim);
    }

    // Emit list for debugging
    void dump(std::ostream &o) const { dump(o, points_->fieldWidth(range_)); }
    void dump(std::ostream &o, const std::pair<size_t, size_t> &width) const {
        o <<"  has " <<StringUtility::plural(nUnique_[DIM_X], "unique X values")
          <<" and " <<StringUtility::plural(nUnique_[DIM_Y], "unique Y values") <<"\n";
        points_->dump(o, range_, width, "  ");
    }

    // Emit list of points for output
    void print(std::ostream &o) const {
        assert(points_!=NULL);
        o <<"## CUT ##\n";
        for (size_t i=range_.first; i<range_.second; ++i) {
            const Point &point = points_->get(DIM_X, i);
            o <<point.val[DIM_X] <<" " <<point.val[DIM_Y] <<"\n";
        }
    }
};

static std::ostream& operator<<(std::ostream &o, const WorkItem &item) {
    item.print(o);
    return o;
}

typedef std::priority_queue<WorkItem> Worklist;

static void dumpWorkList(Worklist workList) {
    std::cerr <<"\n\n";
    size_t i = 0;
    while (!workList.empty()) {
        std::cerr <<"Worklist #" <<i <<"\n";
        const WorkItem &work = workList.top();
        work.dump(std::cerr);
        workList.pop();
        ++i;
    }
}

// Read points from the fiel and append them to POINTS
static void loadPoints(const std::string &input_name, FILE *f, Points &points /*out*/) {
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

        points.insert(Point(x, y));
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
    Points points;
    if (opt.input_file_name.empty()) {
        std::cerr <<argv0 <<": reading points from stdin...\n";
        loadPoints("stdin", stdin, points);
    } else {
        FILE *in = fopen(opt.input_file_name.c_str(), "r");
        if (NULL==in) {
            std::cerr <<argv0 <<": " <<strerror(errno) <<": " <<opt.input_file_name <<"\n";
            exit(1);
        }
        loadPoints(opt.input_file_name, in, points);
        fclose(in);
    }
    std::cerr <<argv0 <<": read " <<StringUtility::plural(points.size(), "points") <<"\n";

    Worklist worklist;
    worklist.push(WorkItem(points));
    size_t nemitted = 0;                                // number of work items already emitted to std::cout

    // Repeatedly split the list of points until we've split enough or we can't split more
    while (nemitted + worklist.size() < nparts && !worklist.empty()) {
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
    while (!worklist.empty()) {
        std::cout <<worklist.top();
        worklist.pop();
        ++nemitted;
    }

    return 0;
}
