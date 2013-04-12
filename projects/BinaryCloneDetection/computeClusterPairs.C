#include "rose.h"
#include "sqlite3x.h"
using namespace sqlite3x; // all classes are prefixed with "sqlite3_"

static std::string argv0;

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

static void
save(sqlite3_connection &db, const Key &key, ExtentMap extents[2], size_t isize[2])
{
    sqlite3_command cmd(db, "insert into cluster_pairs"
                        // 1           2              3              4         5         6        7
                        " (cluster_id, function_id_1, function_id_2, nbytes_1, nbytes_2, ratio_1, ratio_2)"
                        " values (?,?,?,?,?,?,?)");
    for (size_t i=0; i<3; ++i)
        cmd.bind(i+1, key.k[i]);        // bind indices are one-origin (even though sqlite3_reader are zero-origin)
    cmd.bind(4, extents[0].size());
    cmd.bind(5, extents[1].size());
    cmd.bind(6, isize[0] ? (double)extents[0].size() / isize[0] : 0.0);
    cmd.bind(7, isize[1] ? (double)extents[1].size() / isize[1] : 0.0);
    cmd.executenonquery();
}

static void
coverage(sqlite3_connection &db, const Key &key, std::set<int> windows[2], int window_size)
{
    sqlite3_command cmd1(db, //  0        1
                         "select address, size from instructions"
                         //                  1                            2                           3
                         " where function_id=? and index_within_function>=? and index_within_function<?"
                         " order by index_within_function");

    sqlite3_command cmd2(db, "select isize from function_ids where row_number = ?");

    ExtentMap extents[2];
    size_t isize[2];
    for (size_t w=0; w<2; ++w) {
        for (std::set<int>::iterator wi=windows[w].begin(); wi!=windows[w].end(); ++wi) {
            // iterate over the instructions within this window
            cmd1.bind(1, key.k[w+1]);
            cmd1.bind(2, *wi);
            cmd1.bind(3, *wi + window_size);
            sqlite3_reader c1 = cmd1.executereader();
            while (c1.read()) {
                Extent e(c1.getint(0), c1.getint(1));
                extents[w].insert(e);
            }
        }
        cmd2.bind(1, key.k[w+1]);
        isize[w] = cmd2.executeint();
    }
    save(db, key, extents, isize);
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
    db.executenonquery("create table cluster_pairs ("
                       " cluster_id integer,"
                       " function_id_1 integer references function_ids(row_number),"
                       " function_id_2 integer references function_ids(row_number),"
                       " nbytes_1 integer,"     // number of function_id_1 bytes that are similar to function_id_2
                       " nbytes_2 integer,"     // number of function_id_2 bytes that are similar to function_id_1
                       " ratio_1 real,"         // ratio nbytes_1 to total size of function_id_1
                       " ratio_2 real)");       // ratio nbytes_2 to total size of function_id_2

    int window_size = db.executeint("select window_size from run_parameters"); // number of instructions

    // Traverse the join of "clusters" with "vectors", compute the overlap, and write to cluster_pairs
    sqlite3_command cmd1(db,
                         //      0          1              2              3                        4
                         "select a.cluster, a.function_id, b.function_id, a.index_within_function, b.index_within_function"
                         " from clusters as a"
                         " join clusters as b on a.cluster = b.cluster and a.function_id < b.function_id"
                         " order by a.cluster, a.function_id, b.function_id");
    sqlite3_reader c1 = cmd1.executereader();
    Key prev_key;
    std::set<int> windows[2];
    while (c1.read()) {
        Key key(c1.getint(0), c1.getint(1), c1.getint(2));
        if (prev_key!=key) {
            if (prev_key.is_valid())
                coverage(db, prev_key, windows, window_size);
            windows[0].clear();
            windows[1].clear();
            prev_key = key;
        }
        windows[0].insert(c1.getint(3));
        windows[1].insert(c1.getint(4));
    }
    if (prev_key.is_valid())
        coverage(db, prev_key, windows, window_size);
    return 0;
}
