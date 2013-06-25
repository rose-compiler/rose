// Updates a table that describes similarity between pairs of functions.

#include "sage3basic.h"
#include "CloneDetectionLib.h"
#include "rose_getline.h"
#include "Combinatorics.h"

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
              <<"  The similarity of two functions is calculated by first computing the similarity of the output groups\n"
              <<"  of those functions.  Output groups are compared only when the are both generated from input groups\n"
              <<"  belonging to the same input group collection.  Usually each input group belongs to its own singleton\n"
              <<"  collection (see the --collection switch for 10-generate-inputs).  Once output group similarities are\n"
              <<"  computed they are aggregated to form a function pair similarity."
              <<"\n"
              <<"  These switches control how output groups are compared:\n"
              <<"    --[no-]ignore-faults\n"
              <<"            If --ignore-faults is specified, then any test that failed is ignored as if it never even ran.\n"
              <<"            Ignoring failures can speed up the function similarity calculations, especially when output\n"
              <<"            groups are large, because it avoids having to download failed output groups from the database\n"
              <<"            server.  When failed tests aren't ignored (--no-ignore-faults) then the --ogroup algorithm\n"
              <<"            decides how to compare those outputs, and those output group similarities are eventually\n"
              <<"            folded into the similarity measure for the function pair whose outputs are being considered.\n"
              <<"            The default is to not ignore faults.\n"
              <<"    --collection-ratio=RATIO1,RATIO2\n"
              <<"    --collection-limit=LIMIT1,LIMIT2\n"
              <<"            The number of output comparisons for a pair of input group collections is the product of the\n"
              <<"            number of input groups in each collection--comparisons are computed for the cross product of\n"
              <<"            the first function's input group collection with the second function's input group collection.\n"
              <<"            This switch limits the number of comparisons by selecting a random sample from the two\n"
              <<"            collections, with RATIO1 and LIMIT1 applying to the first function's collection and RATIO2 and\n"
              <<"            LIMIT2 applying to the second function's collection.  A RATIO is a floating point value between\n"
              <<"            zero and one which is multiplied by the collection size to determine the number of groups. A\n"
              <<"            LIMIT is an integer that limits the number of groups. At least one group is always selected.\n"
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
              <<"\n"
              <<"  These switches control  how output group similarities are combined to form function similarities:\n"
              <<"    --aggregate=ALGORITHM\n"
              <<"            Indicates how the similarities for individual output groups are combined to form a single\n"
              <<"            similarity value for two functions.  The following choices are available, where \"average\"\n"
              <<"            is the default:\n"
              <<"              * \"average\" takes the average of the individual output group similarities.\n"
              <<"              * \"maximum\" takes the maximum of the individual output group similarities.\n"
              <<"\n"
              <<"  Other switches and arguments:\n"
              <<"    --relation=ID\n"
              <<"            An integer that identifies which similarity values are being selected.  All similarity values\n"
              <<"            that have the same relation ID form a single similarity relationship.  This allows the database\n"
              <<"            to store multiple relationships. For example, relationship #1 could be similarity values that\n"
              <<"            are calculated from the maximum output group Jaccard index, while relationship #2 could be\n"
              <<"            similarity values that are calculated using a threshold of output group equality count. The\n"
              <<"            default relation ID is zero.\n"
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
        : recreate(false), show_progress(false), ignore_faults(false),
          collection_ratio(1.0, 1.0), collection_limit((size_t)-1, (size_t)-1),
          output_cmp(OC_VALUESET_JACCARD), aggregation(AG_AVERAGE), relation_id(0) {}
    bool recreate;                      // recreate the database
    bool show_progress;                 // the --show-progress switch
    bool ignore_faults;                 // ignore tests that failed
    std::pair<double, double> collection_ratio; // ratio of lhs and rhs collection groups to compare, but at least one group
    std::pair<size_t, size_t> collection_limit; // maximum number of groups to compare from collections
    OutputComparison output_cmp;
    Aggregation aggregation;
    int relation_id;
} opt;

static SqlDatabase::TransactionPtr transaction;

typedef WorkList< std::pair<int/*func1_id*/, int/*func2_id*/> > FunctionPairs;

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

typedef std::map<int/*igroup_id*/, CloneDetection::InputGroup*> InputGroups;
static InputGroups igroup_cache;

// Return the input group with the specified ID, loading it from the database if necessary.
static CloneDetection::InputGroup *
igroup(int igroup_id)
{
    InputGroups::iterator found = igroup_cache.find(igroup_id);
    if (found!=igroup_cache.end())
        return found->second;

    CloneDetection::InputGroup *igroup = new CloneDetection::InputGroup;
    if (!igroup->load(transaction, igroup_id)) {
        delete igroup;
        igroup_cache.insert(std::make_pair(igroup_id, (CloneDetection::InputGroup*)0));
        return NULL;
    }

    igroup_cache.insert(std::make_pair(igroup_id, igroup));
    return igroup;
}


// Load output group if it isn't loaded already. Return its pointer in any case.
static CachedOutput *
load_output(CachedOutputs &all_outputs, int64_t ogroup_id)
{
    CachedOutputs::iterator found = all_outputs.find(ogroup_id);
    if (found!=all_outputs.end())
        return found->second;
    CloneDetection::OutputGroups ogs;
    ogs.load(transaction, ogroup_id);
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
load_function_outputs(CachedOutputs &all_outputs, FunctionOutputs &function_outputs, int func_id)
{
    FunctionOutputs::iterator found = function_outputs.find(func_id);
    if (found==function_outputs.end()) {
        SqlDatabase::StatementPtr stmt = transaction->statement("select igroup_id, ogroup_id"
                                                                " from semantic_fio"
                                                                " where func_id = ?"+
                                                                std::string(opt.ignore_faults?" and status = 0":""));
        stmt->bind(0, func_id);
        CachedOutputs outputs;
        for (SqlDatabase::Statement::iterator row=stmt->begin(); row!=stmt->end(); ++row) {
            int igroup_id = row.get<int>(0);
            int64_t ogroup_id = row.get<int64_t>(1);
            CachedOutput *output = load_output(all_outputs, ogroup_id);
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
similarity(const CachedOutputs &f1_outs, const CachedOutputs &f2_outs,
           size_t &ncompares/*out*/, size_t &maxcompares/*out*/)
{
    // Create buckets of output. Each bucket contains the output groups that are all part of the same collection.
    typedef std::vector<int/*igroup_id*/> Bucket;
    typedef std::map<int/*collection_id*/, Bucket> Buckets;
    Buckets f1_buckets, f2_buckets;
    for (CachedOutputs::const_iterator oi=f1_outs.begin(); oi!=f1_outs.end(); ++oi)
        f1_buckets[igroup(oi->first)->get_collection_id()].push_back(oi->first);
    for (CachedOutputs::const_iterator oi=f2_outs.begin(); oi!=f2_outs.end(); ++oi)
        f2_buckets[igroup(oi->first)->get_collection_id()].push_back(oi->first);

    // Statistics accumulators
    ncompares = maxcompares = 0;
    double total_sim = 0, max_sim = 0.0;

    // Compare buckets with each other
    for (Buckets::iterator b1=f1_buckets.begin(); b1!=f1_buckets.end(); ++b1) {
        Bucket &f1_bucket = b1->second;
        Buckets::iterator b2 = f2_buckets.find(b1->first);
        if (b2!=f2_buckets.end()) {
            Bucket &f2_bucket = b2->second;
            maxcompares += f1_bucket.size() * f2_bucket.size();

            // How many outputs should be compared? Shuffle them so they're at the beginning of the bucket.
            size_t nsel1 = f1_bucket.size();
            size_t nsel2 = f2_bucket.size();
            if (f1_bucket.size()>1) {
                nsel1 = std::max((size_t)1, (size_t)round(opt.collection_ratio.first * nsel1));
                nsel1 = std::min(nsel1, opt.collection_limit.first);
                if (nsel1!=f1_bucket.size())
                    Combinatorics::shuffle(f1_bucket, f1_bucket.size(), nsel1);
            }
            if (f2_bucket.size()>1) {
                nsel2 = std::max((size_t)1, (size_t)round(opt.collection_ratio.second * nsel2));
                nsel2 = std::min(nsel2, opt.collection_limit.second);
                if (nsel2!=f2_bucket.size())
                    Combinatorics::shuffle(f2_bucket, f2_bucket.size(), nsel2);
            }

            // Pairwise compare the selected output groups from the f1_bucket with those selected from the f2_bucket
            for (size_t i=0; i<nsel1; ++i) {
                const CachedOutput *f1_ogroup = f1_outs.find(f1_bucket[i])->second;
                for (size_t j=0; j<nsel2; ++j) {
                    const CachedOutput *f2_ogroup = f2_outs.find(f2_bucket[j])->second;
                    double sim = f1_ogroup->similarity(f2_ogroup);
                    total_sim += sim;
                    max_sim = std::max(max_sim, sim);
                    ++ncompares;
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

static const unsigned long BAD_ULONG = (unsigned long)(-2);
static const double BAD_DOUBLE = -911911.0;

static unsigned long
parse_ulong(const std::string &str, unsigned long bad_value=BAD_ULONG)
{
    const char *s = str.c_str();
    char *rest;
    errno = 0;
    unsigned long v = strtoul(s, &rest, 0);
    if (errno || rest==s)
        return bad_value;
    while (isspace(*rest)) ++rest;
    if (*rest)
        return bad_value;
    return v;
}

static double
parse_double(const std::string &str, double bad_value=BAD_DOUBLE)
{
    const char *s = str.c_str();
    char *rest;
    errno = 0;
    double v = strtod(s, &rest);
    if (errno || rest==s)
        return bad_value;
    while (isspace(*rest)) ++rest;
    if (*rest)
        return bad_value;
    return v;
}

static FunctionPairs
load_worklist()
{
    FunctionPairs worklist;
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
    return worklist;
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
        } else if (!strncmp(argv[argno], "--collection-limit=", 19)) {
            std::vector<std::string> args = StringUtility::split(",", argv[argno]+19, 2, true);
            if (args.empty()) {
                std::cerr <<argv0 <<": --collection-limit requires one or two positive values\n";
                exit(1);
            }
            opt.collection_limit.first = parse_ulong(args[0], 0);
            opt.collection_limit.second = args.size()>=2 ? parse_ulong(args[1], 0) : (size_t)(-1);
            if (opt.collection_limit.first<1 || opt.collection_limit.second<1) {
                std::cerr <<argv0 <<": --collection-limit requires one or two positive values\n";
                exit(1);
            }
        } else if (!strncmp(argv[argno], "--collection-ratio=", 19)) {
            std::vector<std::string> args = StringUtility::split(",", argv[argno]+19, 2, true);
            if (args.empty()) {
                std::cerr <<argv0 <<": --collection-ratio requires one or two floating point values\n";
                exit(1);
            }
            opt.collection_ratio.first = parse_double(args[0], -1.0);
            opt.collection_ratio.second = args.size()>=2 ? parse_double(args[1], -1.0) : 1.0;
            if (opt.collection_ratio.first<0 || opt.collection_ratio.second<0 ||
                opt.collection_ratio.first>1 || opt.collection_ratio.second>1) {
                std::cerr <<argv0 <<": --collection-ratio requires one or two floating point values in [0..1]\n";
                exit(1);
            }
        } else if (!strncmp(argv[argno], "--relation=", 11)) {
            opt.relation_id = strtol(argv[argno]+11, NULL, 0);
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
    transaction = conn->transaction();
    int64_t cmd_id = CloneDetection::start_command(transaction, argc, argv, "calculating function similarity");

    // Read function pairs from standard input
    std::cerr <<argv0 <<": reading function pairs worklist from stdin...\n";
    FunctionPairs worklist = load_worklist();
    size_t npairs = worklist.size();
    std::cerr <<argv0 <<": work list has " <<npairs <<" function pair" <<(1==npairs?"":"s") <<"\n";

    // Process each function pair
    CloneDetection::Progress progress(npairs);
    progress.force_output(opt.show_progress);
    SqlDatabase::StatementPtr stmt = transaction->statement("insert into semantic_funcsim"
                                                            // 0        1         2           3          4
                                                            "(func1_id, func2_id, similarity, ncompares, maxcompares,"
                                                            // 5           6
                                                            " relation_id, cmd)"
                                                            " values (?, ?, ?, ?, ?, ?, ?)");
    CachedOutputs all_outputs;
    FunctionOutputs func_outputs;
    while (!worklist.empty()) {
        ++progress;
        int func1_id, func2_id;
        boost::tie(func1_id, func2_id) = worklist.shift();
        assert(func1_id < func2_id);
        load_function_outputs(all_outputs, func_outputs, func1_id);
        load_function_outputs(all_outputs, func_outputs, func2_id);
        const CachedOutputs &f1_outs = find_outputs(func_outputs, func1_id);
        const CachedOutputs &f2_outs = find_outputs(func_outputs, func2_id);
        size_t ncompares, maxcompares;
        double sim = similarity(f1_outs, f2_outs, ncompares/*out*/, maxcompares/*out*/);
        stmt->bind(0, func1_id);
        stmt->bind(1, func2_id);
        stmt->bind(2, sim);
        stmt->bind(3, ncompares);
        stmt->bind(4, maxcompares);
        stmt->bind(5, opt.relation_id);
        stmt->bind(6, cmd_id);
        stmt->execute();
    }
    
    progress.message("committing changes");
    std::string mesg = "calculated similarity relationship #"+StringUtility::numberToString(opt.relation_id)+
                       " for "+StringUtility::numberToString(npairs)+" function pair"+(1==npairs?"":"s");
    CloneDetection::finish_command(transaction, cmd_id, mesg);
    transaction->commit();
    progress.clear();
    return 0;
}
