#include "rose.h"
#include "sqlite3x.h"
using namespace sqlite3x; // all classes are prefixed with "sqlite3_"

static std::string argv0;
static const bool verbose = true;
static const int progress_ncols = 100;

static void usage(int exit_status)
{
    std::cerr <<"usage: " <<argv0 <<" DATABASE_NAME\n"
              <<"  Consults the \"clusters\" and \"vectors\" tables and builds a new\n"
              <<"  cluster_pairs table.  The new table lists all pairs of functions\n"
              <<"  (per cluster) and the number of bytes of overlap among the similar windows\n"
              <<"  of the two functions.  Since the overlap relationship is reflexive and\n"
              <<"  symmetric, we only store pairs where the first function ID is less than\n"
              <<"  the second.\n";
    exit(exit_status);
}





typedef std::map<int/*function_id*/, ExtentMap> FunctionExtents;
typedef std::map<int/*function_id*/, size_t/*bytes*/> FunctionSizes;
typedef std::map<int/*vector_id*/, ExtentMap> VectorExtents;

// Key that stores the ID triplet (cluster, func1, func2).
struct Key {
    int k[3];
    Key() {
        k[0] = k[1] = k[2] = -1;
    }
    Key(int cluster_id, int function_id_1, int function_id_2) {
        k[0] = cluster_id;
        k[1] = function_id_1;
        k[2] = function_id_2;
    }
    bool operator==(const Key &other) const {
        return k[0]==other.k[0] && k[1]==other.k[1] && k[2]==other.k[2];
    }
    bool operator!=(const Key &other) const {
        return !(*this==other);
    }
    bool is_valid() const {
        return *this != Key(-1, -1, -1);
    }
};

// Compute the total size of each function.
static void
compute_function_sizes(sqlite3_connection &db, FunctionSizes &function_sizes/*out*/)
{
    std::cerr <<argv0 <<": pre-computing function sizes...\n";
    FunctionExtents extents;
    sqlite3_command cmd1(db, "select function_id, address, size from instructions");
    sqlite3_reader c1 = cmd1.executereader();
    while (c1.read()) {
        int func_id = c1.getint(0);
        rose_addr_t va = c1.getint(1);
        rose_addr_t sz = c1.getint(2);
        extents[func_id].insert(Extent(va, sz));
    }
    for (FunctionExtents::iterator ei=extents.begin(); ei!=extents.end(); ++ei)
        function_sizes[ei->first] = ei->second.size();
}

// Compute an ExtentMap for each vector.
static void
compute_vector_extents(sqlite3_connection &db, int window_size, VectorExtents &vector_extents/*out*/)
{
    std::cerr <<argv0 <<": counting vectors... ";
    sqlite3_command cmd0(db,
                         "select count(*) from vectors as vector join instructions as insn"
                         " on vector.function_id = insn.function_id and"
                         " insn.index_within_function >= vector.index_within_function and"
                         " insn.index_within_function < vector.index_within_function + ?");
    cmd0.bind(1, window_size);
    int nrows = cmd0.executeint();
    if (verbose)
        std::cerr <<nrows <<"\n"
                  <<argv0 <<": scanning vectors...\n";

    sqlite3_command cmd1(db,//   0                  1             2
                         "select vector.row_number, insn.address, insn.size"
                         " from vectors as vector"
                         " join instructions as insn"
                         "   on vector.function_id = insn.function_id and"
                         "      insn.index_within_function >= vector.index_within_function and"
                         "      insn.index_within_function <  vector.index_within_function + ?");
    cmd1.bind(1, window_size);
    sqlite3_reader c1 = cmd1.executereader();
    for (size_t row=0; c1.read(); ++row) {
        if (verbose && 0==row % 1000 && isatty(2)) {
            int nchars = round((double)row/nrows * progress_ncols);
            fprintf(stderr, "  vector %-10zu %3d%% |%-*s|\r",
                    row, (int)round(100.0*row/nrows), progress_ncols, std::string(nchars, '=').c_str());
            fflush(stderr);
        }
        int vector_id = c1.getint(0);
        rose_addr_t va = c1.getint64(1);
        rose_addr_t sz = c1.getint64(2);
        vector_extents[vector_id].insert(Extent(va, sz));
    }
    if (verbose && isatty(2))
        fputc('\n', stderr);
}

// Compute pair-wise coverage and store the results in the database.
static void
coverage(sqlite3_connection &db, const Key &key, std::set<int> vector_ids[2],
         const VectorExtents &vector_extents, const FunctionSizes &func_sizes)
{
    ExtentMap extents[2];
    for (size_t fi=0; fi<2; ++fi) { // iterate over both functions in the similarity pair
        for (std::set<int>::iterator vi=vector_ids[fi].begin(); vi!=vector_ids[fi].end(); ++vi)
            extents[fi].insert_ranges(vector_extents.at(*vi));
    }

    sqlite3_command cmd(db, "insert into cluster_pairs"
                        // 1           2              3              4         5         6        7
                        " (cluster_id, function_id_1, function_id_2, nbytes_1, nbytes_2, ratio_1, ratio_2)"
                        " values (?,?,?,?,?,?,?)");
    size_t coverage_0 = extents[0].size();
    size_t coverage_1 = extents[1].size();
    size_t isize_0 = func_sizes.at(key.k[1]);
    size_t isize_1 = func_sizes.at(key.k[2]);

    cmd.bind(1, key.k[0]);
    cmd.bind(2, key.k[1]);
    cmd.bind(3, key.k[2]);
    cmd.bind(4, coverage_0);
    cmd.bind(5, coverage_1);
    cmd.bind(6, isize_0 ? (double)coverage_0 / isize_0 : 0.0);
    cmd.bind(7, isize_1 ? (double)coverage_1 / isize_1 : 0.0);
    cmd.executenonquery();
}

int
main(int argc, char *argv[])
{
    argv0 = argv[0];
    if (std::string::npos!=argv0.rfind('/'))
        argv0 = argv0.substr(argv0.rfind('/')+1);
    if (0==argv0.substr(0, 3).compare("lt-"))
        argv0 = argv0.substr(3);

    // Parse command-line
    int argno;
    for (argno=1; argno<argc && '-'==argv[argno][0]; ++argno) {
        if (!strcmp(argv[argno], "--help") || !strcmp(argv[argno], "-h") || !strcmp(argv[argno], "-?")) {
            usage(0);
        } else if (!strcmp(argv[argno], "--")) {
            ++argno;
            break;
        } else {
            std::cerr <<argv0 <<": unknown switch: " <<argv[argno] <<"\n";
            usage(1);
        }
    }
    if (1!=argc-argno)
        usage(1);
    std::string dbname = argv[argno];

    // Prepare the database
    sqlite3_connection db(dbname.c_str());
    db.executenonquery("drop table if exists cluster_pairs;");
    db.executenonquery("create table cluster_pairs (\n"
                       " cluster_id integer,\n"
                       " function_id_1 integer references function_ids(row_number),\n"
                       " function_id_2 integer references function_ids(row_number),\n"
                       " nbytes_1 integer,     -- number of function_id_1 bytes that are similar to function_id_2\n"
                       " nbytes_2 integer,     -- number of function_id_2 bytes that are similar to function_id_1\n"
                       " ratio_1 real,         -- ratio nbytes_1 to total size of function_id_1\n"
                       " ratio_2 real          -- ratio nbytes_2 to total size of function_id_2\n"
                       ")");

    int window_size = db.executeint("select window_size from run_parameters"); // number of instructions

    // Pre-compute the stuff that we use repeatedly
    FunctionSizes func_sizes;
    compute_function_sizes(db, func_sizes/*out*/);
    VectorExtents vector_extents;
    compute_vector_extents(db, window_size, vector_extents/*out*/);

    // Look at pairs of similar functions.  The pair will consist of certain similar vectors from each function, and
    // these are the two sets vector extents that need to be unioned to get the similarity coverage between these two pairs.
    std::cerr <<argv0 <<": counting cluster pairs... ";
    int nrows = db.executeint("select count(*)"
                              " from clusters as a"
                              " join clusters as b on a.cluster = b.cluster and a.function_id < b.function_id");
    std::cerr <<nrows <<"\n"
              <<argv0 <<": scanning cluster pairs...\n";
    sqlite3_command cmd1(db,//   0          1              2              3              4
                         "select a.cluster, a.function_id, b.function_id, a.vectors_row, b.vectors_row"
                         " from clusters as a"
                         " join clusters as b on a.cluster = b.cluster and a.function_id < b.function_id"
                         " order by a.cluster, a.function_id, b.function_id");
    sqlite3_reader c1 = cmd1.executereader();
    std::set<int> vector_ids[2]; // IDs for vectors that are similar between the two functions
    Key prev_key;
    sqlite3_transaction lock(db, sqlite3_transaction::LOCK_IMMEDIATE); // MUCH, MUCH faster as a transaction
    for (size_t row=0; c1.read(); ++row) {
        if (verbose && 0 == row % 20 && isatty(2)) {
            int nchars = round((double)row/nrows * progress_ncols);
            fprintf(stderr, "  pair %-10zu   %3d%% |%-*s|\r",
                    row, (int)round(100.0*row/nrows), progress_ncols, std::string(nchars, '=').c_str());
            fflush(stderr);
        }
        Key key(c1.getint(0), c1.getint(1), c1.getint(2));
        if (prev_key!=key) {
            if (prev_key.is_valid())
                coverage(db, prev_key, vector_ids, vector_extents, func_sizes);
            vector_ids[0].clear();
            vector_ids[1].clear();
            prev_key = key;
        }
        vector_ids[0].insert(c1.getint(3));
        vector_ids[1].insert(c1.getint(4));
    }
    if (prev_key.is_valid())
        coverage(db, prev_key, vector_ids, vector_extents, func_sizes);
    if (verbose && isatty(2))
        fputc('\n', stderr);
    lock.commit();
    return 0;
}
