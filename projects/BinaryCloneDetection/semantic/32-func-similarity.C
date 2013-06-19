// Updates a table that describes similarity between pairs of functions.

#include "sage3basic.h"
#include "CloneDetectionLib.h"
#include "rose_getline.h"

#include <cerrno>

std::string argv0;

static void
usage(int exit_status)
{
    std::cerr <<"usage: " <<argv0 <<" [SWITCHES] [--] DATABASE < FUNC_PAIRS\n"
              <<"  This command updates the semantic_funcsim table with information about the similarity between pairs of\n"
              <<"  functions.  The table contains three primary columns: two function IDs and one floating point similarity\n"
              <<"  value between zero and one (one implies that functions are identical according to the chosen similarity\n"
              <<"  algorithm).  Since similarity is reflexive and symmetric, the table will store function IDs such that\n"
              <<"  the first function ID is less than the second function ID.  The pairs of function IDs are read from the\n"
              <<"  standard input stream.\n"
              <<"\n"
              <<"    --[no-]ignore-faults\n"
              <<"            If --ignore-faults is specified, then any test that failed is ignored as if it never even ran.\n"
              <<"            Ignoring failures can speed up the function similarity calculations, especially when output\n"
              <<"            groups are large, because it avoids having to download failed output groups from the database\n"
              <<"            server.  When failed tests aren't ignored (--no-ignore-faults) then the --ogroup algorithm\n"
              <<"            decides how to compare those outputs, and those output group similarities are eventually\n"
              <<"            folded into the similarity measure for the function pair whose outputs are being considered.\n"
              <<"            The default is to not ignore faults.\n"
              <<"    --ogroup=ALGORITHM\n"
              <<"            Indicates the algorithm that should be used to compare output groups. These are the choices,\n"
              <<"            where \"valueset-jaccard\" is the default:\n"
              <<"              * \"full-equality\" returns 1.0 if two functions produced identical output groups for each\n"
              <<"                input group on which they both ran, and zero in all other cases.  The comparison is made\n"
              <<"                by the CloneDetection::OutputGroup::operator==(), and is transitive.\n"
              <<"              * \"valueset-equality\" compares only the output values, not the call graph, system calls, or\n"
              <<"                other members of an output group.  The values are compared using set equality so that\n"
              <<"                neither the order that the outputs were produced nor their cardinality is considered. Also,\n"
              <<"                if either output group indicates an analysis fault, then the value sets are considered to\n"
              <<"                be unequal. This algorithm produces similarities of zero or one and is transitive.\n"
              <<"              * \"valueset-jaccard\" compares values (but not the other members of an output group) as sets\n"
              <<"                using the Jaccard index to measure similarity of the sets.  It returns a continuum of values\n"
              <<"                between zero and one and is not transitive.  The Jaccard index is the ratio of the size of\n"
              <<"                intersection of two sets to the size of the union.  Also, if either output group indicates\n"
              <<"                an analysis fault, then the Jaccard index is multiplied by 0.25.\n"
              <<"    --permutations=NPERM[,NSEL]\n"
              <<"            Normally, the similarity of a pair of output groups is computed only if both output groups were\n"
              <<"            produced from the same input, however, this switch changes that restriction and computes the\n"
              <<"            similarity for other pairs of output groups.  When this switch is specified, input groups 0\n"
              <<"            through NPERM-1 are assumed to be permutations of one another, input groups NPERM through\n"
              <<"            2 NPERM - 1 are assumed to be permutations of one another, etc.  Similarity is calculated across\n"
              <<"            the entire set of NPERM outputs from one function crossed with the NPERM outputs from another\n"
              <<"            function (with some optimizations due to similarity being reflexive and symmetric).  Since this\n"
              <<"            can be a very large number (e.g., 8!8! = 1,625,702,400), the NSEL parameter can be used to pare\n"
              <<"            it down:  at most NSEL output groups from one function are compared to all output groups from\n"
              <<"            the other function for a single set of permutations of inputs.  NSEL defaults to the same value\n"
              <<"            as NPERM and must be positive.\n"
              <<"    --aggregate=ALGORITHM\n"
              <<"            Indicates how the similarities for individual output groups are combined to form a single\n"
              <<"            similarity value for two functions.  The following choices are available, where \"average\"\n"
              <<"            is the default:\n"
              <<"              * \"average\" takes the average of the individual output group similarities.\n"
              <<"              * \"maximum\" takes the maximum of the individual output group similarities.\n"
              <<"    --progress\n"
              <<"            Show progress reports even if stderr is not a tty.\n"
              <<"\n"
              <<"    DATABASE\n"
              <<"            The name of the database to which we are connecting.  For SQLite3 databases this is just a local\n"
              <<"            file name that will be created if it doesn't exist; for other database drivers this is a URL\n"
              <<"            containing the driver type and all necessary connection parameters.\n"
              <<"    FUNC_PAIRS\n"
              <<"            Standard input should contain one pair of function IDs per line with the IDs separated from one\n"
              <<"            another by spaces and/or tabs.  Blank lines and lines beginning with '#' are ignored. The input\n"
              <<"            is read in its entirety before computation starts.\n";
    exit(exit_status);
}

enum OutputComparison {
    OC_FULL_EQUALITY,
    OC_VALUESET_EQUALITY,
    OC_VALUESET_JACCARD,
};

enum Aggregation {
    AG_AVERAGE,
    AG_MAXIMUM, 
};

static struct Switches {
    Switches()
        : ignore_faults(false), show_progress(false), output_cmp(OC_VALUESET_JACCARD), aggregation(AG_AVERAGE),
          nperm(1), nsel(1) {}
    bool recreate, ignore_faults, show_progress;
    OutputComparison output_cmp;
    Aggregation aggregation;
    size_t nperm, nsel;
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
        SqlDatabase::StatementPtr stmt = tx->statement("select igroup_id, ogroup_id"
                                                       " from semantic_fio"
                                                       " where func_id = ?"+
                                                       std::string(opt.ignore_faults?" and status = 0":""));
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

template<typename T>
static size_t
shuffle(std::vector<T> &vector, size_t nitems)
{
    static LinearCongruentialGenerator lcg;
    nitems = std::min(nitems, vector.size());
    for (size_t i=0; i<nitems; ++i)
        std::swap(vector[i], vector[lcg()%vector.size()]);
    return nitems;
}

static double
similarity(const CachedOutputs &f1_outs, const CachedOutputs &f2_outs,
           size_t &ncompares/*out*/, size_t &maxcompares/*out*/)
{
    // Create buckets of output. Each bucket contains the output groups that were generated from inputs that were
    // (presumably) permutations of one another. The opt.nperms is nominally 1, but is always positive
    typedef std::vector<int/*igroup_id*/> Bucket;
    typedef std::map<int/*bucket*/, Bucket> Buckets;
    Buckets f1_buckets, f2_buckets;
    for (CachedOutputs::const_iterator oi=f1_outs.begin(); oi!=f1_outs.end(); ++oi)
        f1_buckets[oi->first/opt.nperm].push_back(oi->first);
    for (CachedOutputs::const_iterator oi=f2_outs.begin(); oi!=f2_outs.end(); ++oi)
        f2_buckets[oi->first/opt.nperm].push_back(oi->first);

    // Statistics accumulators
    ncompares = maxcompares = 0;
    double total_sim = 0, max_sim = 0.0;

    // Compare buckets with each other
    for (Buckets::iterator b1=f1_buckets.begin(); b1!=f1_buckets.end(); ++b1) {
        Bucket &f1_bucket = b1->second;
        Buckets::iterator b2 = f2_buckets.find(b1->first);
        if (b2!=f2_buckets.end()) {
            Bucket &f2_bucket = b2->second;

            // Both functions produced at least one output group from some permutation of the input.  Select some random output
            // groups from the first function and compare them with all the output groups of the second function.
            maxcompares += f1_bucket.size() * f2_bucket.size();
            size_t n = shuffle(f1_bucket, opt.nsel);
            for (size_t i=0; i<n; ++i) {
                const CachedOutput *f1_ogroup = f1_outs.find(f1_bucket[i])->second;
                for (size_t j=0; j<f2_bucket.size(); ++j) {
                    const CachedOutput *f2_ogroup = f2_outs.find(f2_bucket[j])->second;

                    // Compare output groups f1_ogroup and f2_ogroup
                    ++ncompares;
                    double sim = f1_ogroup->similarity(f2_ogroup);
                    total_sim += sim;
                    max_sim = std::max(max_sim, sim);
                }
            }
        }
    }
    
    // Compute aggreged value for these two functions
    double ave_sim = total_sim / (ncompares?ncompares:1);
    switch (opt.aggregation) {
        case AG_AVERAGE:
            return ave_sim;
        case AG_MAXIMUM:
            return max_sim;
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

    int argno = 1;
    for (/*void*/; argno<argc && '-'==argv[argno][0]; ++argno) {
        if (!strcmp(argv[argno], "--")) {
            ++argno;
            break;
        } else if (!strcmp(argv[argno], "--help") || !strcmp(argv[argno], "-h")) {
            usage(0);
        } else if (!strncmp(argv[argno], "--aggregate=", 12)) {
            if (!strcmp(argv[argno]+12, "average")) {
                opt.aggregation = AG_AVERAGE;
            } else if (!strcmp(argv[argno]+12, "maximum")) {
                opt.aggregation = AG_MAXIMUM;
            } else {
                std::cerr <<argv0 <<": unknown value for --aggregate switch: " <<argv[argno]+12 <<"\n"
                          <<argv0 <<": see --help for more info\n";
                exit(1);
            }
        } else if (!strcmp(argv[argno], "--ignore-faults")) {
            opt.ignore_faults = true;
        } else if (!strcmp(argv[argno], "--no-ignore-faults")) {
            opt.ignore_faults = false;
        } else if (!strncmp(argv[argno], "--ogroup=", 9)) {
            if (!strcmp(argv[argno]+9, "full-equality")) {
                opt.output_cmp = OC_FULL_EQUALITY;
            } else if (!strcmp(argv[argno]+9, "valueset-equality")) {
                opt.output_cmp = OC_VALUESET_EQUALITY;
            } else if (!strcmp(argv[argno]+9, "valueset-jaccard")) {
                opt.output_cmp = OC_VALUESET_JACCARD;
            } else {
                std::cerr <<argv0 <<": unknown value for --ogroup switch: " <<argv[argno]+9 <<"\n"
                          <<argv0 <<": see --help for more info\n";
                exit(1);
            }
        } else if (!strncmp(argv[argno], "--permutations=", 15) || !strncmp(argv[argno], "--permute=", 10)) {
            char *s = strchr(argv[argno], '=')+1, *rest;
            errno = 0;
            opt.nperm = strtoul(s, &rest, 0);
            if (errno || rest==s || opt.nperm<1) {
                std::cerr <<argv0 <<": NPERM value is invalid: " <<argv[argno] <<"\n";
                exit(1);
            }
            s = rest;
            while (isspace(*s)) ++s;
            if (','==*s) ++s;
            while (isspace(*s)) ++s;
            if (*s) {
                errno = 0;
                opt.nsel = strtoul(s, &rest, 0);
                if (errno || rest==s || *rest) {
                    std::cerr <<argv0 <<": NSEL value is invalid: " <<argv[argno] <<"\n";
                    exit(1);
                }
            } else {
                opt.nsel = opt.nperm;
            }
        } else if (!strcmp(argv[argno], "--progress")) {
            opt.show_progress = true;
        } else {
            std::cerr <<argv0 <<": unknown switch: " <<argv[argno] <<"\n"
                      <<argv0 <<": see --help for more info\n";
            exit(1);
        }
    };
    if (argno+1!=argc)
        usage(1);
    SqlDatabase::ConnectionPtr conn = SqlDatabase::Connection::create(argv[argno++]);
    SqlDatabase::TransactionPtr tx = conn->transaction();
    int64_t cmd_id = CloneDetection::start_command(tx, argc, argv, "calculating function similarity");

    // Read function pairs from standard input
    std::cerr <<argv0 <<": reading function pairs worklist from stdin...\n";
    WorkList< std::pair<int, int> > worklist;
    char *line = NULL;
    size_t line_sz = 0, line_num = 0;
    while (rose_getline(&line, &line_sz, stdin)>0) {
        ++line_num;
        if (char *c = strchr(line, '#'))
            *c = '\0';
        char *s = line + strspn(line, " \t\r\n"), *rest;
        if (!*s)
            continue; // blank line

        errno = 0;
        int func1_id = strtol(s, &rest, 0);
        if (errno!=0 || rest==s) {
            std::cerr <<argv0 <<": stdin:" <<line_num <<": syntax error: func1_id expected\n";
            exit(1);
        }
        s = rest;

        errno = 0;
        int func2_id = strtol(s, &rest, 0);
        if (errno!=0 || rest==s) {
            std::cerr <<argv0 <<": stdin:" <<line_num <<": syntax error: func2_id expected\n";
            exit(1);
        }
        s = rest;

        while (isspace(*s)) ++s;
        if (*s) {
            std::cerr <<argv0 <<": stdin:" <<line_num <<": syntax error: extra text after func2_id\n";
            exit(1);
        }

        worklist.push(std::make_pair(std::min(func1_id, func2_id), std::max(func1_id, func2_id)));
    }
    size_t npairs = worklist.size();
    std::cerr <<argv0 <<": work list has " <<npairs <<" function pair" <<(1==npairs?"":"s") <<"\n";

    // Process each function pair
    CloneDetection::Progress progress(npairs);
    progress.force_output(opt.show_progress);
    SqlDatabase::StatementPtr stmt = tx->statement("insert into semantic_funcsim"
                                                   // 0        1         2           3          4            5
                                                   "(func1_id, func2_id, similarity, ncompares, maxcompares, cmd)"
                                                   " values (?, ?, ?, ?, ?, ?)");
    CachedOutputs all_outputs;
    FunctionOutputs func_outputs;
    while (!worklist.empty()) {
        ++progress;
        int func1_id, func2_id;
        boost::tie(func1_id, func2_id) = worklist.shift();
        assert(func1_id < func2_id);
        load_function_outputs(tx, all_outputs, func_outputs, func1_id);
        load_function_outputs(tx, all_outputs, func_outputs, func2_id);
        const CachedOutputs &f1_outs = find_outputs(func_outputs, func1_id);
        const CachedOutputs &f2_outs = find_outputs(func_outputs, func2_id);
        size_t ncompares, maxcompares;
        double sim = similarity(f1_outs, f2_outs, ncompares/*out*/, maxcompares/*out*/);
        stmt->bind(0, func1_id);
        stmt->bind(1, func2_id);
        stmt->bind(2, sim);
        stmt->bind(3, ncompares);
        stmt->bind(4, maxcompares);
        stmt->bind(5, cmd_id);
        stmt->execute();
    }
    
    progress.message("committing changes");
    std::string mesg = "calculated similarity for "+StringUtility::numberToString(npairs)+" function pair"+(1==npairs?"":"s");
    CloneDetection::finish_command(tx, cmd_id, mesg);
    tx->commit();
    progress.clear();
    return 0;
}
