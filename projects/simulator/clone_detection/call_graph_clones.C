////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Given a clone database that has a table indicating pairs of functions that are both syntactically and semantically similar,
// produce a callgraph_clone_pairs table using the following definition:
//     Functions A and B form a CG clone pair iff
//         (1)  A and B form a clone pair,        and
//         (2)  the number of unique functions called by A is the same as the number of unique functions called by B:
//                 |callees(A)| = |callees(B)|    and
//         (3)  there exists a permutation, P(callees(A)) such that
//                 P(callees(A))[i] = callees(B)[i] for all i in 0 .. |callees(A)|
//
// The following database tables must exist:
//     combined_clone_pairs with columns func_id_1 and func_id_2 which are both semantic function IDs. This table holds
//              pairs of functions that are both syntactically and semantically similar and where func_id_1 is less than
//              func_id_2.  The pairs (func_id_2,func_id_1), (func_id_1,func_id_1), and (func_id_2,func_id_2) are implicitly
//              similar due to the similarity relationship being both symmetric and reflexive.
//
//     semantic_cg with columns caller and callee which are both semantic function IDs.  This table holds the call graph
//              edges, which indicate that caller invokes callee.
//
// The following table is created:
//     cg_clone_pairs with columns func_id_1 and func_id_2.  The rows of this table form a subset of the rows of the
//              combined_clone_pairs table.
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "rose.h"
#include "sqlite3x.h"
#include "CloneDetectionProgress.h"

using namespace sqlite3x;

static std::string argv0;

typedef std::vector<int> Callees;
typedef std::map<int, Callees> CG;

static void
usage(int exit_status, const std::string &mesg="")
{
    if (!mesg.empty())
        std::cerr <<argv0 <<": " <<mesg <<"\n";
    std::cerr <<"usage: " <<argv0 <<" DATABASE_NAME PAIRS_TABLE RESULT_TABLE\n"
              <<"    DATABASE_NAME is the name of the SQLite3 database\n"
              <<"    PAIRS_TABLE is the name of the table containing clone pairs, such as \"combined_clone_pairs\"\n"
              <<"    RESULT_TABLE is the name of the table that is (re)created, such as \"combined_cgclone_pairs\"\n";
    exit(exit_status);
}

// Clone pair, sortable
struct ClonePair {
    int a, b; // function IDs
    ClonePair(int a, int b): a(std::min(a, b)), b(std::max(a, b)) {}
    bool operator<(const ClonePair &other) const {
        return a<other.a || (a==other.a && b<other.b);
    }
};

typedef std::set<ClonePair> ClonePairs;

// Matrix of relationships between values of set A and values of set B, both the same size.  In the comments below,
//     "a" is a member of set A
//     "b" is a member of set B
//     "a R b" means "a" is related to "b"
//     "x.i" means bit i of x
class Relationships {
protected:
    uint64_t av_, bv_;          // i is a member of A iff av_.i is set; i is a member of B iff bv_.i is set
    uint64_t r_[64];            // a R b iff r_[a].b
public:
    // creates new relationship where values of A and B are 0 .. size-1 and none of a are related to b
    Relationships(size_t size) {
        assert(size<=64);       // we use uint64_t as a fast std::set<bool>
        av_ = bv_ = IntegerOps::genMask<uint64_t>(size);
        memset(r_, 0, sizeof r_);
    }

    // returns true if bit i is set
    static bool setp(uint64_t x, size_t i) {
        assert(i>=0 && i<64);
        return 0 != (x & IntegerOps::shl1<uint64_t>(i));
    }
    
    // counts the number of bits set in x
    static size_t count(uint64_t x) {
        size_t n = 0;
        for (size_t i=0; i<64; ++i) {
            if (setp(x, i))
                ++n;
        }
        return n;
    }

    // returns the first value in a set
    static size_t first(uint64_t x) {
        for (size_t i=0; i<64; ++i) {
            if (setp(x, i))
                return i;
        }
        assert(!"set is empty");
        abort();
    }

    
    // returns the values in set A
    uint64_t A() const {
        return av_;
    }

    // returns the values in set B
    uint64_t B() const {
        return bv_;
    }
    
    // returns number of values in A (or B)
    size_t size() const {
        return count(av_);
    }

    // returns true if the A and B sets are empty
    bool empty() const {
        assert(av_!=0 || bv_==0);
        return av_==0;
    }

    // returns the number values b in B such that a R b
    size_t size(int a) const {
        return count(related(a));
    }

    // temporarily disable a vertex from A and B and all incident edges
    void disable(int a, int b) {
        assert(setp(av_, a) && setp(bv_, b));
        av_ &= ~IntegerOps::shl1<uint64_t>(a);
        bv_ &= ~IntegerOps::shl1<uint64_t>(b);
    }

    // re-enable a previously disable pair of vertices from A and B and all the incident edges
    void enable(int a, int b) {
        assert(!setp(av_, a) && !setp(bv_, b));
        av_ |= IntegerOps::shl1<uint64_t>(a);
        bv_ |= IntegerOps::shl1<uint64_t>(b);
    }
    
    // assert a R b
    void insert(int a, int b) {
        assert(setp(av_, a) && setp(bv_, b));
        r_[a] |= IntegerOps::shl1<uint64_t>(b);
    }

    // returns true iff a R b
    bool related(int a, int b) const {
        assert(setp(av_, a) && setp(bv_, b));
        return setp(r_[a], b);
    }

    // returns all b such that a R b
    uint64_t related(int a) const {
        return setp(av_, a) ? r_[a] & bv_ : 0;
    }

    // returns true iff every value in A is related to a unique value in B
    bool match() {
        if (empty())
            return true;
        int a = first(av_);
        if (size()==1)
            return related(a, first(bv_));
        uint64_t bv = related(a);
        for (size_t b=0; b<64; ++b) {
            if (setp(bv, b)) {
                disable(a, b);
                bool submatch = match();
                enable(a, b);
                if (submatch)
                    return true;
            }
        }
        return false;
    }

    void print(std::ostream &o, const Callees &A, const Callees &B) const {
        o <<"    A = {";
        for (int a=0; a<64; ++a) {
            if (setp(av_, a))
                o <<" " <<A[a];
        }
        o <<" }\n"
          <<"    B = {";
        for (int b=0; b<64; ++b) {
            if (setp(bv_, b))
                o <<" " <<B[b];
        }
        o <<" }\n"
          <<"    mapping from A to B:\n";
        
        for (int a=0; a<64; ++a) {
            if (setp(av_, a)) {
                uint64_t bv = related(a);
                if (bv) {
                    o <<"      " <<A[a] <<" => {";
                    for (int b=0; b<64; b++) {
                        if (setp(bv, b))
                            o <<" " <<B[b];
                    }
                    o <<" }\n";
                }
            }
        }
    }
};

int
main(int argc, char *argv[])
{
    std::ios::sync_with_stdio();
    argv0 = argv[0];
    size_t slash = argv0.rfind('/');
    if (slash!=std::string::npos)
        argv0 = argv0.substr(slash+1);
    if (0==argv0.substr(0, 3).compare("lt-"))
        argv0 = argv0.substr(3);

    int argno;
    for (argno=1; argno<argc && '-'==argv[argno][0]; ++argno) {
        if (!strcmp(argv[argno], "--help") || !strcmp(argv[argno], "-h") || !strcmp(argv[argno], "-?")) {
            usage(0);
        } else if (!strcmp(argv[argno], "--")) {
            ++argno;
            break;
        } else {
            usage(1, std::string("unknown switch: ")+argv[argno]);
        }
    }
    if (argno+3!=argc)
        usage(1);
    std::string dbname = argv[argno++];
    std::string src_tabname = argv[argno++];
    std::string dst_tabname = argv[argno++];

    sqlite3_connection db(dbname.c_str());
    db.executenonquery("drop table if exists " + dst_tabname);
    db.executenonquery("create table "+dst_tabname+" (\n"
                       "    func_id_1 integer references semantic_functions(id),\n"
                       "    func_id_2 integer references semantic_functions(id),\n"
                       "    ncallees -- number of unique analyzed functions called from either function (same for both)\n"
                       ")");

    // Read the call graph into memory
    CG cg;
    sqlite3_command cmd1(db, "select caller, callee from semantic_cg");
    sqlite3_reader c1 = cmd1.executereader();
    while (c1.read())
        cg[c1.getint(0)].push_back(c1.getint(1));

    // Read all the combined clone pairs into memory
    ClonePairs ccp; // combined clone pairs
    sqlite3_command cmd2(db, "select func_id_1, func_id_2 from " + src_tabname);
    sqlite3_reader c2 = cmd2.executereader();
    while (c2.read())
        ccp.insert(ClonePair(c2.getint(0), c2.getint(1)));
    CloneDetection::Progress progress(ccp.size());
        
    // Process the clone pairs
    sqlite3_transaction lock(db, sqlite3_transaction::LOCK_IMMEDIATE);
    sqlite3_command cmd3(db, "insert into "+dst_tabname+" (func_id_1, func_id_2, ncallees) values (?,?,?)");
    for (ClonePairs::const_iterator ccpi=ccp.begin(); ccpi!=ccp.end(); ++ccpi) {
        const Callees &callees_a = cg[ccpi->a];
        const Callees &callees_b = cg[ccpi->b];
        bool save = false;
        progress.show();

        if (callees_a.size()==callees_b.size()) {
            if (callees_a.empty()) {
                save = true;
            } else {
                // Number the callees from 0 to N-1 and build a relationship mapping callees(A) and callees(B)
                Relationships rel(callees_a.size());
                for (size_t i=callees_a.size(); i>0; --i) {
                    for (size_t j=callees_b.size(); j>0; --j) {
                        if (ccp.find(ClonePair(callees_a[i-1], callees_b[j-1]))!=ccp.end())
                            rel.insert(i-1, j-1);
                    }
                }
                // Is it possible for each callees(A) to match a unique callees(B)?
                save = rel.match();
            }
            if (save) {
                cmd3.bind(1, ccpi->a);
                cmd3.bind(2, ccpi->b);
                cmd3.bind(3, callees_a.size());
                cmd3.executenonquery();
            }
        }
    }
    lock.commit();
    return 0;
}
