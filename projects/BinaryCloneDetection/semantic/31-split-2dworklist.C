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

struct Switches {
    std::string input_file_name;                // non-empty means read coords from a file instead of stdin
};
static Switches opt;

enum Dimension { DIM_X=0, DIM_Y=1, DIM_NONE=2 };

typedef std::pair<size_t, size_t> SizePair;

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
static size_t
median_idx(std::vector<int> v)
{
    assert(!v.empty());
    size_t mid = v.size() / 2;                          // middle:  0 <= mid < N
    size_t lo = mid;                                    // lower boundary: 0 <= lo <= mid
    while (lo>0 && v[lo-1]==v[mid]) --lo;
    size_t hi = mid+1;                                  // upper boundary: mid < hi <= N
    while (hi+1<v.size() && v[hi+1]==v[mid]) ++hi;
    return mid-lo < hi-mid ? lo : hi;                   // the boundary closest to the center
}

// Used to sort 'data' without actually reordering its members.  Instead, an index-to-data is sorted.
struct IndexedSort {
    const std::vector<int> &data;
    IndexedSort(const std::vector<int> &data): data(data) {}
    bool operator()(size_t idx_a, size_t idx_b) {
        assert(idx_a<data.size() && idx_b<data.size());
        return data[idx_a] < data[idx_b];
    }
};

// Coordinates as parallel arrays
class CoordSet {
private:
    std::vector<int> coords_x_, coords_y_;      // parallel arrays for 2d points
    Dimension is_sorted_;                       // is one of the dimensions sorted?
private: // cached items
    mutable bool ndistinct_valid_;
    mutable SizePair ndistinct_;                // valid only when ndistinct_valid_ is true
public:
    CoordSet(): is_sorted_(DIM_NONE), ndistinct_valid_(false) {}

    size_t size() const {
        return coords_x_.size();
    }

    bool empty() const {
        assert(coords_x_.size()==coords_y_.size());
        return coords_x_.empty();
    }
    
    void insert(int x, int y) {
        assert(coords_x_.size()==coords_y_.size());
        coords_x_.push_back(x);
        coords_y_.push_back(y);
        ndistinct_valid_ = false;
        is_sorted_ = DIM_NONE;
    }

    const SizePair& ndistinct() const {
        if (!ndistinct_valid_) {
            ndistinct_ = SizePair(ndistinct(coords_x_), ndistinct(coords_y_));
            ndistinct_valid_ = true;
        }
        return ndistinct_;
    }

    bool multiple() const {     // true if this CoordSet has more than one distinct point
        const SizePair &sp = ndistinct();
        return sp.first>1 || sp.second>1;
    }

    const std::vector<int>& operator[](size_t dimension) const {
        assert(dimension<2);
        return DIM_X==dimension ? coords_x_ : coords_y_;
    }

    bool operator<(const CoordSet &other) const {
        const SizePair &sp1 = ndistinct();
        const SizePair &sp2 = other.ndistinct();
        return std::max(sp1.first, sp1.second) < std::max(sp2.first, sp2.second);
    }

    void sort(Dimension dimension) {
        if (is_sorted_!=dimension) {
            // Perform the sort on one dimension, but do not yet change the order of the coordinate vector
            std::vector<size_t> index; 
            for (size_t i=0; i<coords_x_.size(); ++i)
                index.push_back(i);
            IndexedSort index_cmp(DIM_X==dimension ? coords_x_ : coords_y_);
            std::sort(index.begin(), index.end(), index_cmp);

            // Now change the order of both coordinate vectors
            std::vector<int> x=coords_x_, y=coords_y_;
            for (size_t i=0; i<index.size(); ++i) {
                coords_x_[i] = x[index[i]];
                coords_y_[i] = y[index[i]];
            }
            is_sorted_ = dimension;
        }
    }
    
    void split(size_t split_point, CoordSet &other/*out*/) {
        assert(other.empty());
        split(split_point, coords_x_, other.coords_x_);
        split(split_point, coords_y_, other.coords_y_);
        other.is_sorted_ = is_sorted_;
        ndistinct_valid_ = false;
    }

    // Prints some header comments followed by all the coordinates.  Additionally, notation is printed after a coordinate
    // pair to indicate the last time a value appears (X and Y are treated as a single stream of values for this purpose).
    void print(std::ostream &o) const {
        o <<"# coord set contains " <<coords_x_.size() <<" point" <<(1==coords_x_.size()?"":"s") <<"\n"
          <<"# " <<plural(ndistinct().first, "distinct X values") <<"\n"
          <<"# " <<plural(ndistinct().second, "distinct Y values") <<"\n";

        std::map<int, size_t> last_time;
        for (size_t i=coords_x_.size(); i>0; --i) {
            last_time.insert(std::make_pair(coords_x_[i-1], i-1));
            last_time.insert(std::make_pair(coords_y_[i-1], i-1));
        }

        for (size_t i=0; i<coords_x_.size(); ++i) {
            o <<coords_x_[i] <<" " <<coords_y_[i] <<"\n";
            if (last_time[coords_x_[i]]==i)
                o <<"##LAST " <<coords_x_[i] <<"\n";
            if (coords_y_[i]!=coords_x_[i] && last_time[coords_y_[i]]==i) {
                o <<"##LAST " <<coords_y_[i] <<"\n";
            }
        }
    }

private:
    static size_t ndistinct(const std::vector<int> &v) {
        std::set<int> s;
        for (std::vector<int>::const_iterator vi=v.begin(); vi!=v.end(); ++vi)
            s.insert(*vi);
        return s.size();
    }

    static void split(size_t split_point, std::vector<int> &lo/*in,out*/, std::vector<int> &hi/*out*/) {
        assert(split_point<=lo.size());
        assert(hi.empty());
        hi.insert(hi.end(), &lo[0]+split_point, &lo[0]+lo.size());
        lo.resize(split_point);
    }
};

typedef std::priority_queue<CoordSet> CoordSets;

std::ostream& operator<<(std::ostream &o, const CoordSet &x) 
{
    x.print(o);
    return o;
}





static void
load_worklist(const std::string &input_name, FILE *f, CoordSet &coords)
{
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
        int func1_id = strtol(s, &rest, 0);
        if (errno!=0 || rest==s) {
            std::cerr <<argv0 <<": " <<input_name <<":" <<line_num <<": syntax error: func1_id expected\n";
            exit(1);
        }
        s = rest;

        errno = 0;
        int func2_id = strtol(s, &rest, 0);
        if (errno!=0 || rest==s) {
            std::cerr <<argv0 <<": " <<input_name <<":" <<line_num <<": syntax error: func2_id expected\n";
            exit(1);
        }
        s = rest;

        while (isspace(*s)) ++s;
        if (*s) {
            std::cerr <<argv0 <<": " <<input_name <<":" <<line_num <<": syntax error: extra text after func2_id\n";
            exit(1);
        }

        coords.insert(func1_id, func2_id);
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
    CoordSet all_coords;
    if (opt.input_file_name.empty()) {
        std::cerr <<argv0 <<": reading function pairs worklist from stdin...\n";
        load_worklist("stdin", stdin, all_coords);
    } else {
        FILE *in = fopen(opt.input_file_name.c_str(), "r");
        if (NULL==in) {
            std::cerr <<argv0 <<": " <<strerror(errno) <<": " <<opt.input_file_name <<"\n";
            exit(1);
        }
        load_worklist(opt.input_file_name, in, all_coords);
        fclose(in);
    }
    size_t npairs = all_coords.size();
    std::cerr <<argv0 <<": work list has " <<npairs <<" function pair" <<(1==npairs?"":"s") <<"\n";

    CoordSets coordsets;
    if (all_coords.multiple()) {
        coordsets.push(all_coords);
    } else {
        std::cout <<"## CUT ##\n" <<all_coords;
    }

    // Repeatedly divide the coordinate set with the most distinct X or Y values.  We use this metric rather than pure size of
    // the coordinate set because we're trying to minimize the amount of information that 32-func-similarity (and similar
    // tools) need to download from the database, and we're assuming that they're able to cache what they download.
    for (size_t i=1; i<nparts && !coordsets.empty(); ++i) {
        CoordSet coords = coordsets.top();
        coordsets.pop();
        SizePair sizepair = coords.ndistinct();

        // Decide where to split either the X coordinates or the Y coordinates, depending on which has more values.
        Dimension dimension = sizepair.first > sizepair.second ? DIM_X : DIM_Y;
        coords.sort(dimension);
        size_t split_point = median_idx(coords[dimension]);
        assert(split_point>0 && split_point<coords.size()); // otherwise it wouldn't split

        // Split the coordinate arrays at the desired position.
        CoordSet coords2;
        coords.split(split_point, coords2);

        // Insert the two smaller sets if they can be split more, otherwise print them
        if (coords.multiple()) {
            coordsets.push(coords);
        } else {
            std::cout <<"## CUT ##\n" <<coords <<"\n";
        }
        if (coords2.multiple()) {
            coordsets.push(coords2);
        } else {
            std::cout <<"## CUT ##\n" <<coords2 <<"\n";
        }
    }

    // Emit all the other coordinate sets
    while (!coordsets.empty()) {
        std::cout <<"## CUT ##\n" <<coordsets.top();
        coordsets.pop();
    }
    
    return 0;
}
