// Updates a table that describes similarity between pairs of functions.

#include "sage3basic.h"
#include "CloneDetectionLib.h"

std::string argv0;

static void
usage(int exit_status)
{
    std::cerr <<"usage: " <<argv0 <<" DATABASE\n"
              <<"  This command updates the semantic_funcsim table with information about the similarity between pairs of\n"
              <<"  functions.  The table contains three primary columns: two function IDs and one floating point similarity\n"
              <<"  value between zero and one (one implies that functions are identical according to the chosen similarity\n"
              <<"  algorithm).  Since similarity is reflexive and symmetric, the pairs of functions are chosen so that\n"
              <<"  the first function ID is less than the second function ID.\n"
              <<"\n"
              <<"    --[no-]delete\n"
              <<"            The --delete switch causes all previous similarity information to be discarded and recalculated\n"
              <<"            from scratch. The default is to calculate similarity only for those pairs of functions for which\n"
              <<"            similarity has not been calculated already.\n"
              <<"    --ogroup=ALGORITHM\n"
              <<"            Indicates the algorithm that should be used to compare output groups.\n"
              <<"    DATABASE\n"
              <<"            The name of the database to which we are connecting.  For SQLite3 databases this is just a local\n"
              <<"            file name that will be created if it doesn't exist; for other database drivers this is a URL\n"
              <<"            containing the driver type and all necessary connection parameters.\n";
    exit(exit_status);
}

enum OutputComparison {
    OC_FULL_EQUALITY,
    OC_VALUESET_EQUALITY,
    OC_VALUESET_JACCARD,
};

static struct Switches {
    Switches(): recreate(false), output_cmp(OC_VALUESET_JACCARD) {}
    bool recreate;
    OutputComparison output_cmp;
} opt;

// Abstract base class for various methods of computing similarity between two output groups.  The class not only provides
// the similarity() operator, but also can cache any other information that makes computing the similarity faster. The cached
// info can sometimes be substantially smaller than the output group stored in the database.
class CachedOutput {
public:
    virtual ~CachedOutput() {}
    // Similarity of two objects as a value between zero and one (one being identical)
    virtual double similarity(const CachedOutput *other) const = 0;
};

typedef std::map<int64_t/*igroup_id or ogroup_id*/, CachedOutput*> CachedOutputs;
typedef std::map<int/*func_id*/, CachedOutputs> FunctionOutputs;

// Similarity using equality of the CloneDetection::OutputGroup objects.  If the objects are equal return 1.0, otherwise 0.0
class FullEquality: public CachedOutput {
public:
    const CloneDetection::OutputGroup *ogroup;
    FullEquality(const CloneDetection::OutputGroup *ogroup) {
        ogroup = new CloneDetection::OutputGroup(*ogroup); // because caller is about to delete it
    }
    virtual double similarity(const CachedOutput *other_) const /*override*/ {
        const FullEquality *other = dynamic_cast<const FullEquality*>(other_);
        return *ogroup == *other->ogroup ? 1.0 : 0.0;
    }
};

// Treat output values as a set and compare them using set equality.
class ValuesetEquality: public CachedOutput {
public:
    typedef std::set<CloneDetection::OutputGroup::value_type> VSet;
    typedef std::pair<VSet::const_iterator, VSet::const_iterator> IterPair;
    VSet values;
    CloneDetection::AnalysisFault::Fault fault;

    ValuesetEquality(const CloneDetection::OutputGroup *ogroup) {
        values.insert(ogroup->values.begin(), ogroup->values.end());
        fault = ogroup->fault;
    }

    virtual double similarity(const CachedOutput *other_) const /*override*/ {
        const ValuesetEquality *other = dynamic_cast<const ValuesetEquality*>(other_);
        if (fault==CloneDetection::AnalysisFault::NONE && other->fault==CloneDetection::AnalysisFault::NONE &&
            values.size()==other->values.size() &&
            std::equal(values.begin(), values.end(), other->values.begin()))
            return 1.0;
        return 0.0;
    }
};

// Treat output values as sets and use the Jaccard index to measure similarity. Use a penalty for failed tests
class ValuesetJaccard: public ValuesetEquality {
public:
    ValuesetJaccard(const CloneDetection::OutputGroup *ogroup): ValuesetEquality(ogroup) {}
    virtual double similarity(const CachedOutput *other_) const /*override*/ {
        const ValuesetJaccard *other = dynamic_cast<const ValuesetJaccard*>(other_);
        double multiplier = 1.0;
        if (fault!=CloneDetection::AnalysisFault::NONE || other->fault!=CloneDetection::AnalysisFault::NONE)
            multiplier = 0.25; // penalty for having failed
        typedef std::vector<CloneDetection::OutputGroup::value_type> Vector;
        Vector sunion(values.size()+other->values.size(), 0);
        Vector sinter(std::max(values.size(), other->values.size()));
        Vector::iterator ui = std::set_union(values.begin(), values.end(),
                                             other->values.begin(), other->values.end(), sunion.begin());
        Vector::iterator ii = std::set_intersection(values.begin(), values.end(),
                                                    other->values.begin(), other->values.end(), sinter.begin());
        size_t usize = ui-sunion.begin();
        size_t isize = ii-sinter.begin();
        return multiplier*isize/usize;
    }
};

// Load output group if it isn't loaded already. Return its pointer in any case.
static CachedOutput *
load_output(const SqlDatabase::TransactionPtr &tx, CachedOutputs &all_outputs, int64_t ogroup_id)
{
    CachedOutputs::iterator found = all_outputs.find(ogroup_id);
    if (found!=all_outputs.end())
        return found->second;
    CloneDetection::OutputGroups ogs;
    ogs.load(tx, ogroup_id);
    CachedOutput *output = NULL;
    switch (opt.output_cmp) {
        case OC_FULL_EQUALITY:
            output = new FullEquality(ogs.lookup(ogroup_id));
            break;
        case OC_VALUESET_EQUALITY:
            output = new ValuesetEquality(ogs.lookup(ogroup_id));
            break;
        case OC_VALUESET_JACCARD:
            output = new ValuesetJaccard(ogs.lookup(ogroup_id));
            break;
    }
    return output;
}

// Load all outputs for the specified function if they're not in memory already
static void
load_function_outputs(const SqlDatabase::TransactionPtr &tx, CachedOutputs &all_outputs, FunctionOutputs &function_outputs,
                      int func_id)
{
    FunctionOutputs::iterator found = function_outputs.find(func_id);
    if (found==function_outputs.end()) {
        SqlDatabase::StatementPtr stmt = tx->statement("select igroup_id, ogroup_id from semantic_fio where func_id = ?");
        stmt->bind(0, func_id);
        CachedOutputs outputs;
        for (SqlDatabase::Statement::iterator row=stmt->begin(); row!=stmt->end(); ++row) {
            int igroup_id = row.get<int>(0);
            int64_t ogroup_id = row.get<int64_t>(1);
            CachedOutput *output = load_output(tx, all_outputs, ogroup_id);
            outputs.insert(std::make_pair(igroup_id, output));
        }
        function_outputs.insert(std::make_pair(func_id, outputs));
    }
}

static const CachedOutputs &
find_outputs(const FunctionOutputs &func_outputs, int func_id)
{
    FunctionOutputs::const_iterator found = func_outputs.find(func_id);
    assert(found!=func_outputs.end());
    return found->second;
}

static double
similarity(const CachedOutputs &f1_outs, const CachedOutputs &f2_outs)
{
    size_t nintersect = 0;
    double total_sim = 0;
    for (CachedOutputs::const_iterator oi1=f1_outs.begin(); oi1!=f1_outs.end(); ++oi1) {
        int igroup_id = oi1->first;
        CachedOutput *o1 = oi1->second;
        CachedOutputs::const_iterator oi2 = f2_outs.find(igroup_id);
        if (oi2!=f2_outs.end()) {
            // We only compare f1's output with f2's output if both outputs were created with the same inputs.
            ++nintersect;
            CachedOutput *o2 = oi2->second;
            double sim = o1->similarity(o2);
            total_sim += sim;
        }
    }
    double ave_sim = total_sim / (nintersect?nintersect:1);
    return ave_sim;
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

    int argno = 1;
    for (/*void*/; argno<argc && '-'==argv[argno][0]; ++argv) {
        if (!strcmp(argv[argno], "--")) {
            ++argno;
            break;
        } else if (!strcmp(argv[argno], "--help") || !strcmp(argv[argno], "-h")) {
            usage(0);
        } else if (!strcmp(argv[argno], "--delete")) {
            opt.recreate = true;
        } else if (!strcmp(argv[argno], "--no-delete")) {
            opt.recreate = false;
        } else if (!strncmp(argv[argno], "--ogroup=", 9)) {
            if (!strcmp(argv[argno]+9, "full-equality")) {
                opt.output_cmp = OC_FULL_EQUALITY;
            } else if (!strcmp(argv[argno]+9, "valueset-equality")) {
                opt.output_cmp = OC_VALUESET_EQUALITY;
            } else if (!strcmp(argv[argno]+9, "valueset-jaccard")) {
                opt.output_cmp = OC_VALUESET_JACCARD;
            } else {
                std::cerr <<argv0 <<": unknown value for --ogroup switch: " <<argv[argno]+13 <<"\n"
                          <<argv0 <<": see --help for more info\n";
                exit(1);
            }
        } else {
            std::cerr <<argv0 <<": unknown switch: " <<argv[argno] <<"\n"
                      <<argv0 <<": see --help for more info\n";
            exit(1);
        }
    };
    if (argno>=argc)
        usage(1);
    SqlDatabase::ConnectionPtr conn = SqlDatabase::Connection::create(argv[argno++]);
    SqlDatabase::TransactionPtr tx = conn->transaction();
    int64_t cmd_id = CloneDetection::start_command(tx, argc, argv, "calculating function similarity");

    // Create the ogroup index. This table can be huge!  This might take a long time, so don't drop the index if it
    // alreay exists.  PostgreSQL doesn't have "create index if not exists...".
    std::cerr <<argv0 <<": creating output group index (could take a while)\n";
    try {
        tx->execute("create index idx_ogroups_hashkey on semantic_outputvalues(hashkey)");
    } catch (const SqlDatabase::Exception&) {
        std::cerr <<argv0 <<": idx_ogroups_hashkey index already exists; NOT dropping and recreating\n";
        // postgres seems to need a new transaction now, otherwise the next query fails with:
        // Error executing query .  Attempt to activate transaction<READ COMMITTED> 'Transaction_0' which is already closed
        tx = conn->transaction();
    }

    // Delete rather than recreate, otherwise we have to duplicate code from Schema.sql
    if (opt.recreate) {
        std::cerr <<argv0 <<": deleting rows from semantic_funcsim\n";
        tx->execute("delete from semantic_funcsim");
    }

    // Create pairs of function IDs for those functions which have been tested and for which no similarity measurement has been
    // computed.  (FIXME: We should probably recompute similarity that might have changed due to re-running functions)
    std::cerr <<argv0 <<": creating work list\n";
    tx->execute("create temporary table tmp_tested_funcs as select distinct func_id from semantic_fio");
    tx->execute("create temporary table tmp_fpairs as"
                "  select distinct f1.func_id as func1_id, f2.func_id as func2_id"
                "    from tmp_tested_funcs as f1"
                "    join tmp_tested_funcs as f2 on f1.func_id < f2.func_id"
                "  except"
                "    select func1_id, func2_id from semantic_funcsim");
    size_t npairs = tx->statement("select count(*) from tmp_fpairs")->execute_int();
    std::cerr <<argv0 <<": work list has " <<npairs <<" pair" <<(1==npairs?"":"s") <<" of functions\n";

    // Process each function pair
    CloneDetection::Progress progress(npairs);
    SqlDatabase::StatementPtr stmt1 = tx->statement("select func1_id, func2_id from tmp_fpairs");
    SqlDatabase::StatementPtr stmt2 = tx->statement("insert into semantic_funcsim"
                                                    // 0        1         2           3
                                                    "(func1_id, func2_id, similarity, cmd) values (?, ?, ?, ?)");
    CachedOutputs all_outputs;
    FunctionOutputs func_outputs;
    for (SqlDatabase::Statement::iterator pair=stmt1->begin(); pair!=stmt1->end(); ++pair) {
        ++progress;
        int func1_id = pair.get<int>(0);
        int func2_id = pair.get<int>(1);
        load_function_outputs(tx, all_outputs, func_outputs, func1_id);
        load_function_outputs(tx, all_outputs, func_outputs, func2_id);
        const CachedOutputs &f1_outs = find_outputs(func_outputs, func1_id);
        const CachedOutputs &f2_outs = find_outputs(func_outputs, func2_id);
        double sim = similarity(f1_outs, f2_outs);
        stmt2->bind(0, func1_id);
        stmt2->bind(1, func2_id);
        stmt2->bind(2, sim);
        stmt2->bind(3, cmd_id);
        stmt2->execute();
    }
    
        
    progress.message("committing changes");
    std::string mesg = "calculated similarity for "+StringUtility::numberToString(npairs)+" function pair"+(1==npairs?"":"s");
    CloneDetection::finish_command(tx, cmd_id, mesg);
    tx->commit();
    progress.clear();
    return 0;
}
        



//     SqlDatabase::StatementPtr stmt1 = tx->statement("select"
//                                                     "   fpair.func1_id,"
//                                                     "   fpair.func2_id,"
//                                                     "   fio1.ogroup_id as func1_ogroup,"
//                                                     "   fio2.ogroup_id as func2_ogroup"
//                                                     " from tmp_fpairs as fpair"
//                                                     " join semantic_fio as fio1 on fpair.func1_id=fio1.func_id"
//                                                     " join semantic_fio as fio2 on fpair.func2_id=fio2.func_id"
// 
// 
// 
//     // Process each hash key pair.
//     OutputGroups ogroups;
//     SimilarityMap simmap;
//     int npairs = tx->statement("select count(*) from tmp_hashkeypairs")->execute_int();
//     std::cerr <<argv0 <<": compairing " <<npairs <<" pair" <<(1==npairs?"":"s") <<" of output groups\n";
//     Progress progress(npairs);
//     SqlDatabase::StatementPtr stmt1 = tx->statement("select hashkey1, hashkey2 from tmp_hashkeypairs");
//     SqlDatabase::StatementPtr stmt2 = tx->statement("insert into semantic_ogprsim"
//                                                     // 0         1         2           3
//                                                     " (hashkey1, hashkey2, similarity, cmd) values (?, ?, ?, ?)");
//     for (SqlDatabase::Statement::iterator row=stmt1->begin(); row!=stmt1->end(); ++row) {
//         ++progress;
//         int64_t hashkey1 = row.get<int64_t>(0);
//         int64_t hashkey2 = row.get<int64_t>(1);
//         std::pair<SimilarityBase*, SimilarityBase*> pair(NULL, NULL);
//         switch (opt.algorithm) {
//             case SIM_EQUALITY:
//                 pair = load<FullEquality>(tx, ogroups, simmap, hashkey1, hashkey2);
//                 break;
//             case SIM_VALUE_SET_EQUALITY:
//                 pair = load<ValueSetEquality>(tx, ogroups, simmap, hashkey1, hashkey2);
//                 break;
//             case SIM_VALUE_JACCARD:
//                 pair = load<ValueSetJaccard>(tx, ogroups, simmap, hashkey1, hashkey2);
//                 break;
//         }
//         double similarity = pair.first->similarity(pair.second);
//         stmt2->bind(0, hashkey1)->bind(1, hashkey2)->bind(2, similarity)->bind(3, cmd_id)->execute();
//     }
// 
//     progress.message("committing changes");
//     finish_command(tx, cmd_id,
//                    "calculated output group similarity for "+StringUtility::numberToString(npairs)+" pair"+(1==npairs?"":"s"));
//     tx->commit();
//     progress.clear();
//     return 0;
// }
