// Generates input groups.  Each input group contains a sequence of input values.  The input values are consumed by functions
// as they run.

#include "sage3basic.h"
#include "CloneDetectionLib.h"
#include "Combinatorics.h"

using namespace CloneDetection;
std::string argv0;

static void
usage(int exit_status)
{
    std::cerr <<"usage: " <<argv0 <<" [SWITCHES] [--] DATABASE\n"
              <<"  This command populates the specified database with input groups. Each input group contains the values\n"
              <<"  that will be consumed by a tested function whenever it needs an input value.\n"
              <<"\n"
              <<"    --ngroups=N\n"
              <<"            Total number of input groups that should be present in the database.  If the database does not\n"
              <<"            have this many input groups, then additional groups are created; if the database already has at\n"
              <<"            least this many input groups then nothing happens.  Existing input groups are never modified.\n"
              <<"            Input groups are numbered consecutively starting at zero.  Even if not new input groups are\n"
              <<"            created, the input group tables are created in the database.\n"
              <<"    --nintegers=N\n"
              <<"            Each newly created input group will be initialized to have N integer values. The default is to\n"
              <<"            create 100 integer values per input group.  If a function consumes more than N non-pointer\n"
              <<"            values, the values after N are all zero.\n"
              <<"    --integer-modulus=N\n"
              <<"            Integer values are constrained to be in the range zero (inclusive) to N (exclusive).  The default\n"
              <<"            for N is 256.\n"
              <<"    --permute-integers=N\n"
              <<"            Normally, each input group contains random integer values.  However, if this switch is given with\n"
              <<"            N greater than 1 then the first N integer values of an input group is a permutation of the first N\n"
              <<"            integer values of an earlier input group.  For instance, if N is three then input groups will be\n"
              <<"            generated in groups of six (6=3!) with groups where ID = 0 mod 6 serving as the basis of the\n"
              <<"            following five permutations. N may be larger than the number of integer input values, in which\n"
              <<"            case some of the basis values are zero, and zeros will therefore get mixed into the permutation.\n"
              <<"    --npointers=N\n"
              <<"            Each newly created input group will be initialized to have N pointer values.  Pointer values are\n"
              <<"            consumed when a function reads a memory location that has been determined to hold a pointer;\n"
              <<"            pointer values are not consumed if pointer detection analysis is not performed.  The default\n"
              <<"            is to create 20 pointer values per input group.  If a function consumes more than N pointer\n"
              <<"            values, the values after N are all null.\n"
              <<"    --null-probability=RATIO\n"
              <<"            Pointer values are either null or non-null according to the specified RATIO, a value between zero\n"
              <<"            and one, inclusive.  A ratio of zero results in no null pointers, and a value of one results\n"
              <<"            in only null pointers.\n"
              <<"    --seed=N\n"
              <<"            Input group values are created by calling a linear congruential generator (LCG) which is seeded\n"
              <<"            with the integer N.  The default (when N is -1) is that each input group's LCG is seeded with the\n"
              <<"            ID number of that input group.\n"
              <<"    DATABASE\n"
              <<"            The name of the database to which we are connecting.  For SQLite3 databases this is just a local\n"
              <<"            file name that will be created if it doesn't exist; for other database drivers this is a URL\n"
              <<"            containing the driver type and all necessary connection parameters.\n";
    exit(exit_status);
}

struct Switches {
    Switches()
        : ngroups(0), nintegers(100), integer_modulus(256), permute_integers(0), npointers(20), null_probability(0.5),
          seed(-1) {}
    unsigned ngroups;
    unsigned nintegers;
    unsigned integer_modulus;
    unsigned permute_integers;
    unsigned npointers;
    double null_probability;
    int seed;
};

class PointerGenerator {
public:
    PointerGenerator(size_t seed): ncalls(seed), base(0x40000000), npages(512), page_size(4096) {}
    rose_addr_t operator()() {
        return base + (ncalls++ % npages)*page_size;
    }
private:
    size_t ncalls, base, npages, page_size;
};
    
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

    Switches opt;
    int argno = 1;
    for (/*void*/; argno<argc && '-'==argv[argno][0]; ++argno) {
        if (!strcmp(argv[argno], "--")) {
            ++argno;
            break;
        } else if (!strcmp(argv[argno], "--help") || !strcmp(argv[argno], "-h")) {
            usage(0);
        } else if (!strncmp(argv[argno], "--ngroups=", 10)) {
            opt.ngroups = strtoul(argv[argno]+10, NULL, 0);
        } else if (!strncmp(argv[argno], "--nintegers=", 12)) {
            opt.nintegers = strtoul(argv[argno]+12, NULL, 0);
        } else if (!strncmp(argv[argno], "--integer-modulus=", 18)) {
            opt.integer_modulus = strtoul(argv[argno]+18, NULL, 0);
            if (0==opt.integer_modulus) {
                std::cerr <<argv0 <<": --integer-modulus must be positive\n";
                exit(1);
            }
        } else if (!strncmp(argv[argno], "--permute-integers=", 19)) {
            opt.permute_integers = strtoul(argv[argno]+19, NULL, 0);
        } else if (!strncmp(argv[argno], "--npointers=", 12)) {
            opt.npointers = strtoul(argv[argno]+12, NULL, 0);
        } else if (!strncmp(argv[argno], "--null-probability=", 19)) {
            opt.null_probability = strtod(argv[argno]+19, NULL);
            if (opt.null_probability<0 || opt.null_probability>1) {
                std::cerr <<argv0 <<": --null-probability must be between zero and one, inclusive\n";
                exit(1);
            }
        } else {
            std::cerr <<argv0 <<": unrecognized switch: " <<argv[argno] <<"\n"
                      <<"see \"" <<argv0 <<" --help\" for usage info.\n";
            exit(1);
        }
    }
    if (argno+1!=argc)
        usage(1);
    SqlDatabase::TransactionPtr tx = SqlDatabase::Connection::create(argv[argno])->transaction();
    std::cerr <<argv0 <<": connected to database\n";
    int64_t cmd_id = start_command(tx, argc, argv, "generating input groups");
    LinearCongruentialGenerator lcg(0);

    unsigned first_id = tx->statement("select coalesce(max(id),-1)+1 from semantic_inputvalues")->execute_int();
    std::vector<uint64_t> permutation_base;
    size_t np = Combinatorics::factorial(opt.permute_integers); // number of possible permutations
    Progress progress(opt.ngroups);
    for (unsigned id=first_id; id<opt.ngroups; ++id) {
        ++progress;
        if (opt.seed!=-1)
            lcg.reseed(id);

        // Generate integer input values
        size_t pn = id % np; // integer permutation number; zero means no permutation, but random values
        std::vector<uint64_t> integer_inputs;
        if (0==pn) {
            for (size_t i=0; i<opt.nintegers; ++i)
                integer_inputs.push_back(lcg() % opt.integer_modulus);
            if (np>1) {
                permutation_base = integer_inputs;
                if (opt.permute_integers>permutation_base.size())
                    permutation_base.resize(opt.permute_integers, 0);
            }
        } else {
            if (permutation_base.empty()) {
                size_t base_group_id = (id / np) * np; // input group that serves as the base
                SqlDatabase::StatementPtr stmt = tx->statement("select val"
                                                               " from semantic_inputvalues"
                                                               " where id=? and vtype='I'"
                                                               " order by pos");
                stmt->bind(0, base_group_id);
                for (SqlDatabase::Statement::iterator result=stmt->begin(); result!=stmt->end(); ++result)
                    permutation_base.push_back(result.get<int>(0));
                if (opt.permute_integers>permutation_base.size())
                    permutation_base.resize(opt.permute_integers, 0);
            }
            integer_inputs = permutation_base;
            Combinatorics::permute(integer_inputs, pn, opt.permute_integers);
        }

        // Save generated integer inputs
        SqlDatabase::StatementPtr stmt = tx->statement("insert into semantic_inputvalues"
                                                       " (id, vtype, pos, val, cmd) values (?, ?, ?, ?, ?)");
        for (size_t i=0; i<integer_inputs.size(); ++i) {
            stmt->bind(0, id);
            stmt->bind(1, "I");
            stmt->bind(2, i);
            stmt->bind(3, integer_inputs[i]);
            stmt->bind(4, cmd_id);
            stmt->execute();
        }
                
        // Generate and save pointer input values
        PointerGenerator pgen(rand());
        for (size_t i=0; i<opt.npointers; ++i) {
            uint64_t val = ((double)lcg() / lcg.max()) < opt.null_probability ? 0 : pgen();
            stmt->bind(0, id);
            stmt->bind(1, "P");
            stmt->bind(2, i);
            stmt->bind(3, val);
            stmt->bind(4, cmd_id);
            stmt->execute();
        }
    }
    progress.clear();

    size_t ngenerated = opt.ngroups>first_id ? opt.ngroups-first_id : 0;
    std::string desc = "generated "+StringUtility::numberToString(ngenerated)+
                       " input group"+(1==ngenerated?"":"s");
    finish_command(tx, cmd_id, desc);
    tx->commit();
    std::cerr <<argv0 <<": " <<desc <<"\n";
    return 0;
}
