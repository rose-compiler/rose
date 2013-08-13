#include "rose.h"
#include "SqlDatabase.h"

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
compute_function_sizes(const SqlDatabase::TransactionPtr &tx, FunctionSizes &function_sizes/*out*/)
{
    std::cerr <<argv0 <<": pre-computing function sizes...\n";
    SqlDatabase::StatementPtr cmd1 = tx->statement("select id, isize from semantic_functions");
    for (SqlDatabase::Statement::iterator c1=cmd1->begin(); c1!=cmd1->end(); ++c1) {
        int func_id = c1.get<int>(0);
        size_t sz = c1.get<int>(1);
        function_sizes[func_id] = sz;
    }
}

// Compute an ExtentMap for each vector.
static void
compute_vector_extents(const SqlDatabase::TransactionPtr &tx, int window_size, VectorExtents &vector_extents/*out*/)
{
    std::cerr <<argv0 <<": counting vectors... ";
    SqlDatabase::StatementPtr cmd0 = tx->statement("select count(*)"
                                                   " from vectors as vector"
                                                   " join semantic_instructions as insn"
                                                   "   on vector.function_id = insn.func_id and"
                                                   " insn.position >= vector.index_within_function and"
                                                   " insn.position < vector.index_within_function + ?");
    cmd0->bind(0, window_size);
    int nrows = cmd0->execute_int();
    if (verbose)
        std::cerr <<nrows <<"\n"
                  <<argv0 <<": scanning vectors...\n";

    SqlDatabase::StatementPtr cmd1 = tx->statement(//      0          1             2
                                                   "select vector.id, insn.address, insn.size"
                                                   " from vectors as vector"
                                                   " join semantic_instructions as insn"
                                                   "   on vector.function_id = insn.func_id and"
                                                   "      insn.position >= vector.index_within_function and"
                                                   "      insn.position <  vector.index_within_function + ?");
    cmd1->bind(0, window_size);
    size_t row = 0;
    for (SqlDatabase::Statement::iterator c1=cmd1->begin(); c1!=cmd1->end(); ++c1, ++row) {
        if (verbose && 0==row % 1000 && isatty(2)) {
            int nchars = round((double)row/nrows * progress_ncols);
            fprintf(stderr, "  vector %-10zu %3d%% |%-*s|\r",
                    row, (int)round(100.0*row/nrows), progress_ncols, std::string(nchars, '=').c_str());
            fflush(stderr);
        }
        int vector_id = c1.get<int>(0);
        rose_addr_t va = c1.get<int64_t>(1);
        rose_addr_t sz = c1.get<int64_t>(2);
        vector_extents[vector_id].insert(Extent(va, sz));
    }
    if (verbose && isatty(2))
        fputc('\n', stderr);
}

// Compute pair-wise coverage and store the results in the database.
static void
coverage(const SqlDatabase::TransactionPtr &tx, const Key &key, std::set<int> vector_ids[2],
         const VectorExtents &vector_extents, const FunctionSizes &func_sizes)
{
    ExtentMap extents[2];
    for (size_t fi=0; fi<2; ++fi) { // iterate over both functions in the similarity pair
        for (std::set<int>::iterator vi=vector_ids[fi].begin(); vi!=vector_ids[fi].end(); ++vi)
            extents[fi].insert_ranges(vector_extents.at(*vi));
    }

    SqlDatabase::StatementPtr cmd = tx->statement("insert into cluster_pairs"
                                                  // 1           2              3              4         5
                                                  " (cluster_id, function_id_1, function_id_2, nbytes_1, nbytes_2,"
                                                  // 6       7
                                                  " ratio_1, ratio_2)"
                                                  " values (?,?,?,?,?,?,?)");
    size_t coverage_0 = extents[0].size();
    size_t coverage_1 = extents[1].size();
    size_t isize_0 = func_sizes.at(key.k[1]);
    size_t isize_1 = func_sizes.at(key.k[2]);

    cmd->bind(0, key.k[0]);
    cmd->bind(1, key.k[1]);
    cmd->bind(2, key.k[2]);
    cmd->bind(3, coverage_0);
    cmd->bind(4, coverage_1);
    cmd->bind(5, isize_0 ? (double)coverage_0 / isize_0 : 0.0);
    cmd->bind(6, isize_1 ? (double)coverage_1 / isize_1 : 0.0);
    cmd->execute();
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
    SqlDatabase::TransactionPtr tx = SqlDatabase::Connection::create(dbname)->transaction();

    int window_size = tx->statement("select window_size from run_parameters")->execute_int(); // number of instructions

    // Pre-compute the stuff that we use repeatedly
    FunctionSizes func_sizes;
    compute_function_sizes(tx, func_sizes/*out*/);
    VectorExtents vector_extents;
    compute_vector_extents(tx, window_size, vector_extents/*out*/);

    // Look at pairs of similar functions.  The pair will consist of certain similar vectors from each function, and
    // these are the two sets vector extents that need to be unioned to get the similarity coverage between these two pairs.
    std::cerr <<argv0 <<": counting cluster pairs... ";
    int nrows = tx->statement("select count(*)"
                              " from clusters as a"
                              " join clusters as b on a.cluster = b.cluster and a.function_id < b.function_id")->execute_int();
    std::cerr <<nrows <<"\n"
              <<argv0 <<": scanning cluster pairs...\n";
    if (verbose && isatty(2)) {
        fprintf(stderr, "  waiting for sqlite...\r");
        fflush(stderr);
    }
    SqlDatabase::StatementPtr cmd1 = tx->statement(
                                                   //      0          1              2              3              4
                                                   "select a.cluster, a.function_id, b.function_id, a.vectors_row, b.vectors_row"
                                                   " from clusters as a"
                                                   " join clusters as b"
                                                   "   on a.cluster = b.cluster and a.function_id < b.function_id"
                                                   " order by a.cluster, a.function_id, b.function_id");
    std::set<int> vector_ids[2]; // IDs for vectors that are similar between the two functions
    Key prev_key;
    size_t row = 0;
    for (SqlDatabase::Statement::iterator c1=cmd1->begin(); c1!=cmd1->end(); ++c1, ++row) {
        if (verbose && 0 == row % 20 && isatty(2)) {
            int nchars = round((double)row/nrows * progress_ncols);
            fprintf(stderr, "  pair %-10zu   %3d%% |%-*s|\r",
                    row, (int)round(100.0*row/nrows), progress_ncols, std::string(nchars, '=').c_str());
            fflush(stderr);
        }
        Key key(c1.get<int>(0), c1.get<int>(1), c1.get<int>(2));
        if (prev_key!=key) {
            if (prev_key.is_valid())
                coverage(tx, prev_key, vector_ids, vector_extents, func_sizes);
            vector_ids[0].clear();
            vector_ids[1].clear();
            prev_key = key;
        }
        vector_ids[0].insert(c1.get<int>(3));
        vector_ids[1].insert(c1.get<int>(4));
    }
    if (prev_key.is_valid())
        coverage(tx, prev_key, vector_ids, vector_extents, func_sizes);
    if (verbose && isatty(2))
        fputc('\n', stderr);

    tx->commit();
    return 0;
}
