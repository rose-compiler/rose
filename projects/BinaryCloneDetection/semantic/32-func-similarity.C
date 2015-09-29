// Updates a table that describes similarity between pairs of functions.

#include "sage3basic.h"
#include "CloneDetectionLib.h"
#include "rose_getline.h"
#include <EditDistance/DamerauLevenshtein.h>

#include <cerrno>

#include "lsh.h"

using namespace rose;

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
              <<"  of those functions.  Output groups are compared only when they are both generated from input groups\n"
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
              <<"              * \"dl-edit-distance\" compares value vectors by calculating the Damerau-Levenshtein edit\n"
              <<"                distance. The similarity value is (1-DL/DL_max) where DL_max is the maximum possible edit\n"
              <<"                distance for vectors of that length.\n"
              <<"\n"
              <<"  These switches control  how output group similarities are combined to form function similarities:\n"
              <<"    --aggregate=ALGORITHM\n"
              <<"            Indicates how the similarities for individual output groups are combined to form a single\n"
              <<"            similarity value for two functions.  The following choices are available, where \"average\"\n"
              <<"            is the default:\n"
              <<"              * \"average\" takes the average of the individual output group similarities.\n"
              <<"              * \"maximum\" takes the maximum of the individual output group similarities.\n"
              <<"              * \"minimum\" takes the minimum of the individual output group similarities.\n"
              <<"\n"
              <<"  Other switches and arguments:\n"
              <<"    --dry-run\n"
              <<"            Don't modify the database, but do everything else.\n"
              <<"    --file=NAME\n"
              <<"            Read pairs from this file instead of standard input.\n"
              <<"    --relation=ID\n"
              <<"            An integer that identifies which similarity values are being selected.  All similarity values\n"
              <<"            that have the same relation ID form a single similarity relationship.  This allows the database\n"
              <<"            to store multiple relationships. For example, relationship #1 could be similarity values that\n"
              <<"            are calculated from the maximum output group Jaccard index, while relationship #2 could be\n"
              <<"            similarity values that are calculated using a threshold of output group equality count. The\n"
              <<"            default relation ID is zero.\n"
              <<"    --return-threshold=R\n"
              <<"            Threshold for determining when a function returns a value.  The clone detection heuristically\n"
              <<"            computes the likelihood that a function returns a value; a value between zero and one. However,\n"
              <<"            the algorithm that computes similarity needs a Boolean and obtains it by comparing the likelihood\n"
              <<"            with a threshold, R.  If the likelihood of returning a value is less than R then we assume that\n"
              <<"            the function does not return a value; otherwise we assume it does. The default for R is 0.25.\n"
              <<"    --progress\n"
              <<"            Show progress reports even if stderr is not a tty.\n"
              <<"    --verbose\n"
              <<"            Show lots of diagnostics.\n"
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
    OC_DAMERAU_LEVENSHTEIN,
};

enum Aggregation {
    AG_AVERAGE,
    AG_MINIMUM,
    AG_MAXIMUM,
};

static struct Switches {
    Switches()
        : recreate(false), show_progress(false), ignore_faults(false),
          collection_ratio(1.0, 1.0), collection_limit((size_t)-1, (size_t)-1), output_cmp(OC_VALUESET_JACCARD),
          aggregation(AG_AVERAGE), relation_id(0), verbose(false), dry_run(false), return_threshold(0.25) {}
    bool recreate;                      // recreate the database
    bool show_progress;                 // the --show-progress switch
    bool ignore_faults;                 // ignore tests that failed
    std::pair<double, double> collection_ratio; // ratio of lhs and rhs collection groups to compare, but at least one group
    std::pair<size_t, size_t> collection_limit; // maximum number of groups to compare from collections
    OutputComparison output_cmp;
    Aggregation aggregation;
    int relation_id;
    bool verbose;
    std::string input_file_name;
    bool dry_run;
    double return_threshold;
} opt;

static SqlDatabase::TransactionPtr transaction;

static const int FUNC_EVICT = -2; // func_id to indicate that the we're done with this function
typedef WorkList< std::pair<int/*func1_id*/, int/*func2_id*/> > FunctionPairs;

typedef std::map<int64_t/*id*/, size_t/*use-count*/> ObjUsage;
typedef std::set<int64_t/*id*/> IdSet;

// Smallish information about functions
struct FuncInfo {
    bool returns_value;         // true if the function returns a value; false if the function is void
    ObjUsage ogroup_usage;      // output groups resulting from testing this function
};

typedef std::map<int/*func_id*/, FuncInfo> FuncInfos;
static FuncInfos func_infos;

// Decompress vectors from database
boost::scoped_array<uint16_t>*
decompress_string_to_array(const std::string& array_from_db)
{
    std::vector<uint8_t> counts = StringUtility::decode_base64(array_from_db);
    int vec_length = x86_last_instruction* 4 + 300 + 9 + 3;
    boost::scoped_array<uint16_t>* array_uncompressed = new boost::scoped_array<uint16_t>(new uint16_t[vec_length]);
    decompressVector(counts.data(), counts.size(), array_uncompressed->get());
    return array_uncompressed;
}


// Abstract base class for various methods of computing similarity between two output groups.  The class not only provides
// the similarity() operator, but also can cache any other information that makes computing the similarity faster. The cached
// info can sometimes be substantially smaller than the output group stored in the database.  Because we need to be able to
// evict these from the cache on demand, we also reference count them.
typedef boost::shared_ptr<class CachedOutput> CachedOutputPtr;
class CachedOutput { // abstract
protected:
    int64_t ogroup_id; // from the database

public:
    const boost::scoped_array<uint16_t>* signature_vector;
    int syntactic_ninsns;

protected:
    CachedOutput(int64_t ogroup_id): ogroup_id(ogroup_id), signature_vector(NULL), syntactic_ninsns(0) {}

public:
    virtual ~CachedOutput() {
        delete signature_vector;
        if (opt.verbose)
            std::cerr <<argv0 <<": deleted output group " <<ogroup_id <<"\n";
    }
    int64_t get_id() const { return ogroup_id; }
    // Similarity of two objects as a value between zero and one (one being identical).
    virtual double similarity(const CachedOutput *other, const FuncInfo &finfo1, const FuncInfo &finfo2) const = 0;
    virtual void print(std::ostream&, const std::string &prefix) const = 0;
};

typedef std::map<int64_t/*igroup_id or ogroup_id*/, CachedOutputPtr> CachedOutputs;
typedef std::map<int/*func_id*/, CachedOutputs> FunctionOutputs;

// Similarity using equality of the CloneDetection::OutputGroup objects.  If the objects are equal return 1.0, otherwise 0.0
typedef boost::shared_ptr<class FullEquality> FullEqualityPtr;
class FullEquality: public CachedOutput {
private:
    const CloneDetection::OutputGroup *ogroup;
protected: // use create() instead
    FullEquality(int64_t ogroup_id, const CloneDetection::OutputGroup *ogroup,
                 const std::string& array_from_db, int tmp_syntactic_ninsns)
        : CachedOutput(ogroup_id) {
        ogroup = new CloneDetection::OutputGroup(*ogroup); // because caller is about to delete it
        signature_vector = decompress_string_to_array(array_from_db);
        syntactic_ninsns = tmp_syntactic_ninsns;
    }
public:
    static FullEqualityPtr create(int64_t ogroup_id, const CloneDetection::OutputGroup *ogroup,
                                  const std::string& array_from_db, int tmp_syntactic_ninsns) {
        return FullEqualityPtr(new FullEquality(ogroup_id, ogroup, array_from_db, tmp_syntactic_ninsns));
    }
    virtual double similarity(const CachedOutput *other_, const FuncInfo &finfo1, const FuncInfo &finfo2) const ROSE_OVERRIDE {
        const FullEquality *other = dynamic_cast<const FullEquality*>(other_);
        return *ogroup == *other->ogroup ? 1.0 : 0.0;
    }
    virtual void print(std::ostream &o, const std::string &prefix="") const ROSE_OVERRIDE {
        ogroup->print(o, prefix);
    }
};

// Treat output values and return value as a set and compare them using set equality.
typedef boost::shared_ptr<class ValuesetEquality> ValuesetEqualityPtr;
class ValuesetEquality: public CachedOutput {
protected:
    typedef std::set<CloneDetection::OutputGroup::value_type> VSet;
    typedef std::pair<VSet::const_iterator, VSet::const_iterator> IterPair;
    VSet values;
    std::pair<bool, CloneDetection::OutputGroup::value_type> retval;
    CloneDetection::AnalysisFault::Fault fault;

protected: // use create() instead
    ValuesetEquality(int64_t ogroup_id, const CloneDetection::OutputGroup *ogroup,
                     const std::string& array_from_db, int tmp_syntactic_ninsns)
        : CachedOutput(ogroup_id) {
        std::vector<VSet::value_type> vvec = ogroup->get_values();
        for (size_t i=0; i<vvec.size(); ++i)
            values.insert(vvec[i]);
        fault = ogroup->get_fault();
        retval = ogroup->get_retval();
        signature_vector = decompress_string_to_array(array_from_db);
        syntactic_ninsns = tmp_syntactic_ninsns;

    }

public:
    static ValuesetEqualityPtr create(int64_t ogroup_id, const CloneDetection::OutputGroup *ogroup,
                                      const std::string& array_from_db, int tmp_syntactic_ninsns) {
        return ValuesetEqualityPtr(new ValuesetEquality(ogroup_id, ogroup, array_from_db, tmp_syntactic_ninsns));
    }

    virtual double similarity(const CachedOutput *other_, const FuncInfo &finfo1, const FuncInfo &finfo2) const ROSE_OVERRIDE {
        const ValuesetEquality *other = dynamic_cast<const ValuesetEquality*>(other_);
        bool has_retval1 = finfo1.returns_value && retval.first;
        bool has_retval2 = finfo2.returns_value && other->retval.first;
        if (fault==CloneDetection::AnalysisFault::NONE && other->fault==CloneDetection::AnalysisFault::NONE &&
            values.size()==other->values.size() &&
            std::equal(values.begin(), values.end(), other->values.begin()) &&
            has_retval1==has_retval2 &&
            (!has_retval1 || retval.second==other->retval.second))
            return 1.0;
        return 0.0;
    }

    virtual void print(std::ostream &o, const std::string &prefix="") const ROSE_OVERRIDE {
        o <<prefix <<"values:";
        for (VSet::const_iterator vi=values.begin(); vi!=values.end(); ++vi)
            o <<" " <<*vi;
        if (retval.first)
            o <<"\n" <<prefix <<"retval: " <<retval.second;
        o <<"\n" <<prefix <<"fault: " <<CloneDetection::AnalysisFault::fault_name(fault) <<"\n";
    }
};

// Treat return value and output values as vectors and use the Damerau-Levenshtein edit distance to calculate similarity.
typedef boost::shared_ptr<class ValuesDamerauLevenshtein> ValuesDamerauLevenshteinPtr;
class ValuesDamerauLevenshtein: public CachedOutput {
private:
    typedef CloneDetection::OutputGroup::value_type VType;
    typedef std::vector<VType> ValVector;
    ValVector values;
    std::pair<bool, CloneDetection::OutputGroup::value_type> retval;

protected: // use create() instead
    ValuesDamerauLevenshtein(int64_t ogroup_id, const CloneDetection::OutputGroup *ogroup,
                             const std::string& array_from_db, int tmp_syntactic_ninsns)
        : CachedOutput(ogroup_id) {
        values = ogroup->get_values();
        retval = ogroup->get_retval();
        signature_vector = decompress_string_to_array(array_from_db);
        syntactic_ninsns = tmp_syntactic_ninsns;

    }

public:
    static ValuesDamerauLevenshteinPtr create(int64_t ogroup_id, const CloneDetection::OutputGroup *ogroup,
                                              const std::string& array_from_db, int tmp_syntactic_ninsns) {
        return ValuesDamerauLevenshteinPtr(new ValuesDamerauLevenshtein(ogroup_id, ogroup, array_from_db, tmp_syntactic_ninsns));
    }

    virtual double similarity(const CachedOutput *other_, const FuncInfo &finfo1, const FuncInfo &finfo2) const ROSE_OVERRIDE {
        const ValuesDamerauLevenshtein *other = dynamic_cast<const ValuesDamerauLevenshtein*>(other_);
        ValVector vv1, vv2;
        if (finfo1.returns_value && retval.first)
            vv1.push_back(retval.second);
        if (finfo2.returns_value && other->retval.first)
            vv2.push_back(other->retval.second);
        vv1.insert(vv1.end(), values.begin(), values.end());
        vv2.insert(vv2.end(), other->values.begin(), other->values.end());

        size_t dl = EditDistance::damerauLevenshteinDistance(vv1, vv2);
        size_t dl_max = std::max(vv1.size(), vv2.size());
        if (0==dl && 0==dl_max) {
            assert(vv1.empty() && vv2.empty());
            return 1;
        }
        return 1.0 - (double)dl / dl_max;
    }

    virtual void print(std::ostream &o, const std::string &prefix="") const ROSE_OVERRIDE {
        o <<prefix <<"values:";
        for (ValVector::const_iterator vi=values.begin(); vi!=values.end(); ++vi)
            o <<" " <<*vi;
        if (retval.first)
            o <<"\n" <<prefix <<"retval: " <<retval.second;
        o <<"\n";
    }
};

// Treat output values and return value as a and use the Jaccard index to measure similarity. Use a penalty for failed
// tests.  The Jaccard index of two empty sets is 1.
typedef boost::shared_ptr<class ValuesetJaccard> ValuesetJaccardPtr;
class ValuesetJaccard: public ValuesetEquality {
protected: // use create() instead
    ValuesetJaccard(int64_t ogroup_id, const CloneDetection::OutputGroup *ogroup,
                    const std::string& array_from_db, int tmp_syntactic_ninsns)
        : ValuesetEquality(ogroup_id, ogroup, array_from_db, tmp_syntactic_ninsns) {}

public:
    static ValuesetJaccardPtr create(int64_t ogroup_id, const CloneDetection::OutputGroup *ogroup,
                                     const std::string& array_from_db, int tmp_syntactic_ninsns) {
        return ValuesetJaccardPtr(new ValuesetJaccard(ogroup_id, ogroup, array_from_db, tmp_syntactic_ninsns));
    }

    virtual double similarity(const CachedOutput *other_, const FuncInfo &finfo1, const FuncInfo &finfo2) const ROSE_OVERRIDE {
        const ValuesetJaccard *other = dynamic_cast<const ValuesetJaccard*>(other_);
        VSet vs1, vs2;
        if (finfo1.returns_value && retval.first)
            vs1.insert(retval.second);
        if (finfo2.returns_value && other->retval.first)
            vs2.insert(other->retval.second);
        vs1.insert(values.begin(), values.end());
        vs2.insert(other->values.begin(), other->values.end());

        double multiplier = 1.0;
        if (fault!=CloneDetection::AnalysisFault::NONE || other->fault!=CloneDetection::AnalysisFault::NONE)
            multiplier = 0.25; // penalty for having failed
        typedef std::vector<CloneDetection::OutputGroup::value_type> Vector;
        Vector sunion(vs1.size()+vs2.size(), 0);
        Vector sinter(std::max(vs1.size(), vs2.size()));
        Vector::iterator ui = std::set_union(vs1.begin(), vs1.end(), vs2.begin(), vs2.end(), sunion.begin());
        Vector::iterator ii = std::set_intersection(vs1.begin(), vs1.end(), vs2.begin(), vs2.end(), sinter.begin());
        size_t usize = ui-sunion.begin();
        size_t isize = ii-sinter.begin();
        double jaccard = usize ? (double)isize/usize : 1.0;
        return multiplier*jaccard;
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
static CachedOutputPtr
load_output(CachedOutputs &all_outputs, int64_t ogroup_id, const std::string& array_from_db, int syntactic_ninsns)
{
    CachedOutputs::iterator found = all_outputs.find(ogroup_id);
    if (found!=all_outputs.end())
        return found->second;
    CloneDetection::OutputGroups ogs;
    ogs.load(transaction, ogroup_id);
    CachedOutputPtr output;
    switch (opt.output_cmp) {
        case OC_FULL_EQUALITY:
            output = FullEquality::create(ogroup_id, ogs.lookup(ogroup_id), array_from_db, syntactic_ninsns);
            break;
        case OC_VALUESET_EQUALITY:
            output = ValuesetEquality::create(ogroup_id, ogs.lookup(ogroup_id), array_from_db, syntactic_ninsns);
            break;
        case OC_VALUESET_JACCARD:
            output = ValuesetJaccard::create(ogroup_id, ogs.lookup(ogroup_id), array_from_db, syntactic_ninsns);
            break;
        case OC_DAMERAU_LEVENSHTEIN:
            output = ValuesDamerauLevenshtein::create(ogroup_id, ogs.lookup(ogroup_id), array_from_db, syntactic_ninsns);
            break;
    }
    all_outputs.insert(std::make_pair(ogroup_id, output));
    if (opt.verbose)
        std::cerr <<argv0 <<": loaded output group " <<ogroup_id <<" (cache size=" <<all_outputs.size() <<")\n";
    return output;
}

// Load all the function info from the database.  This data is small compared to output groups.
static void
load_function_infos(const IdSet &function_ids, ObjUsage &ogroup_usage)
{
    std::map<int, double> returns_value = CloneDetection::function_returns_value(transaction);
    for (std::map<int, double>::iterator ri=returns_value.begin(); ri!=returns_value.end(); ++ri)
        func_infos[ri->first].returns_value = ri->second >= opt.return_threshold;

    // Figure out how many times each function uses each output group, and the total number of times each output
    // group is used.  This information will be necessary during cache eviction.
    transaction->execute("create temporary table load_function_infos (func_id integer)");
    SqlDatabase::StatementPtr stmt = transaction->statement("insert into load_function_infos (func_id) values (?)");
    for (IdSet::const_iterator fi=function_ids.begin(); fi!=function_ids.end(); ++fi)
        stmt->bind(0, *fi)->execute();
    stmt = transaction->statement("select fio.func_id, fio.ogroup_id"
                                  " from load_function_infos as need"
                                  " join semantic_fio as fio on need.func_id=fio.func_id");
    for (SqlDatabase::Statement::iterator row=stmt->begin(); row!=stmt->end(); ++row) {
        int func_id = row.get<int>(0);
        int64_t ogroup_id = row.get<int64_t>(1);
        func_infos[func_id].ogroup_usage.insert(std::make_pair(ogroup_id, 0));
        ++func_infos[func_id].ogroup_usage[ogroup_id];
        ogroup_usage.insert(std::make_pair(ogroup_id, 0));
        ++ogroup_usage[ogroup_id];
    }
    transaction->execute("drop table load_function_infos");
}

// Load all outputs for the specified function if they're not in memory already
static void
load_function_outputs(CachedOutputs &all_outputs, FunctionOutputs &function_outputs, int func_id)
{
    FunctionOutputs::iterator found = function_outputs.find(func_id);
    if (found==function_outputs.end()) {
        SqlDatabase::StatementPtr stmt = transaction->statement("select igroup_id, ogroup_id, counts_b64, syntactic_ninsns "
                                                                " from semantic_fio"
                                                                " where func_id = ? "+
                                                                std::string(opt.ignore_faults?" and status = 0":""));
        stmt->bind(0, func_id);
        CachedOutputs outputs;
        for (SqlDatabase::Statement::iterator row=stmt->begin(); row!=stmt->end(); ++row) {
            int igroup_id = row.get<int>(0);
            int64_t ogroup_id = row.get<int64_t>(1);
            std::string array_from_db = row.get<std::string>(2);
            int syntactic_ninsns = row.get<int>(3);
            CachedOutputPtr output = load_output(all_outputs, ogroup_id, array_from_db, syntactic_ninsns);
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


class OutputSimilarity{
public:
    double ave_semantic_sim;
    double min_semantic_sim;
    double max_semantic_sim;

    double ave_hamming_d;
    int min_hamming_d;
    int max_hamming_d;

    double ave_euclidean_d;
    double min_euclidean_d;
    double max_euclidean_d;

    double ave_euclidean_d_ratio;
    double min_euclidean_d_ratio;
    double max_euclidean_d_ratio;

    OutputSimilarity() {
        ave_semantic_sim = 0;
        min_semantic_sim = 0;
        max_semantic_sim = 0;

        ave_hamming_d = 0;
        min_hamming_d = 0;
        max_hamming_d = 0;

        ave_euclidean_d = 0;
        min_euclidean_d = 0;
        max_euclidean_d = 0;

        ave_euclidean_d_ratio = 0;
        min_euclidean_d_ratio = 0;
        max_euclidean_d_ratio = 0;
    }
};

static OutputSimilarity
similarity(const FuncInfo &func1_info, const FuncInfo &func2_info,
           const CachedOutputs &f1_outs, const CachedOutputs &f2_outs,
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
    double total_sim = 0, max_sim = 0.0, min_sim=1.0;

    int hamming_d            = 0;
    int min_hamming_d        = INT_MAX;
    int max_hamming_d        = 0;

    double euclidean_d       = 0.0;
    double min_euclidean_d   = INFINITY;
    double max_euclidean_d   = 0.0;

    double euclidean_d_ratio = 0.0;
    double min_euclidean_d_ratio = INFINITY;
    double max_euclidean_d_ratio = 0.0;

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
                const CachedOutputPtr f1_ogroup = f1_outs.find(f1_bucket[i])->second;

                const boost::scoped_array<uint16_t>& f1_signature_vector = *f1_ogroup->signature_vector;
                const int f1_syntactic_ninsns = f1_ogroup->syntactic_ninsns;

                for (size_t j=0; j<nsel2; ++j) {
                    // Output group similarity
                    const CachedOutputPtr f2_ogroup = f2_outs.find(f2_bucket[j])->second;
                    double sim = f1_ogroup->similarity(f2_ogroup.get(), func1_info, func2_info);
                    total_sim += sim;
                    max_sim = std::max(max_sim, sim);
                    min_sim = std::min(min_sim, sim);


                    // Syntactic similarity
                    const int f2_syntactic_ninsns = f2_ogroup->syntactic_ninsns;
                    const boost::scoped_array<uint16_t>& f2_signature_vector = *f2_ogroup->signature_vector;
                    int cur_hamming_d            = 0;
                    double cur_euclidean_d       = 0.0;
                    double cur_euclidean_d_ratio = 0.0;
                    int f1_v=0;
                    int f2_v=0;
                    int vec_length = x86_last_instruction*4 + 300 + 9 + 3;
                    for (int k = 0; k < vec_length; k++) {
                        f1_v = f1_signature_vector[k];
                        f2_v = f2_signature_vector[k];

                        if (f1_v != f2_v)
                            cur_hamming_d++;
                        cur_euclidean_d += (f1_v - f2_v)*(f1_v - f2_v);
                    }

                    hamming_d     += cur_hamming_d;
                    max_hamming_d = std::max(max_hamming_d, cur_hamming_d);
                    min_hamming_d = std::min(min_hamming_d, cur_hamming_d);

                    cur_euclidean_d = sqrt(cur_euclidean_d);
                    euclidean_d    += cur_euclidean_d;
                    max_euclidean_d = std::max(max_euclidean_d, cur_euclidean_d);
                    min_euclidean_d = std::min(min_euclidean_d, cur_euclidean_d);

                    int difference = std::max(f1_syntactic_ninsns, f2_syntactic_ninsns);
                    if (difference != 0) {
                        cur_euclidean_d_ratio = 100.0*cur_euclidean_d/difference;
                        euclidean_d_ratio    += cur_euclidean_d_ratio;
                        max_euclidean_d_ratio = std::max(cur_euclidean_d_ratio, max_euclidean_d_ratio);
                        min_euclidean_d_ratio = std::min(cur_euclidean_d_ratio, min_euclidean_d_ratio);
                    } else {
                        min_euclidean_d_ratio = 0.0;
                    }

                    // Diagnostics
                    ++ncompares;
                    if (opt.verbose) {
                        std::cerr <<argv0 <<":  ogroup1 = " <<f1_ogroup->get_id() <<" (bucket idx=" <<f1_bucket[i] <<"):\n";
                        f1_ogroup->print(std::cerr, "    ");
                        std::cerr <<argv0 <<":  ogroup2 = " <<f1_ogroup->get_id() <<" (bucket idx=" <<f2_bucket[j] <<"):\n";
                        f2_ogroup->print(std::cerr, "    ");
                        std::cerr <<argv0 <<":  similarity(" <<f1_bucket[i] <<", " <<f2_bucket[j] <<") = " <<sim <<"\n";
                    }
                }
            }
        }
    }

    OutputSimilarity output_sim;
    if (0<ncompares) {
      output_sim.ave_hamming_d = (1.0*hamming_d) / ncompares;
      output_sim.min_hamming_d = min_hamming_d;
      output_sim.max_hamming_d = max_hamming_d;
      output_sim.ave_euclidean_d   = euclidean_d / ncompares;
      output_sim.min_euclidean_d = min_euclidean_d;
      output_sim.max_euclidean_d = max_euclidean_d;
      output_sim.ave_euclidean_d_ratio = euclidean_d_ratio / ncompares;
      output_sim.min_euclidean_d_ratio = min_euclidean_d_ratio;
      output_sim.max_euclidean_d_ratio = max_euclidean_d_ratio;

      output_sim.ave_semantic_sim = total_sim / ncompares;
      output_sim.min_semantic_sim = min_sim;
      output_sim.max_semantic_sim = max_sim;
    }
    return output_sim;
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
load_worklist(const std::string &input_name, FILE *f, IdSet &function_ids)
{
    FunctionPairs worklist;
    char *line = NULL;
    size_t line_sz = 0, line_num = 0;
    while (rose_getline(&line, &line_sz, f)>0) {
        ++line_num;
        if (!strncmp(line, "##LAST", 6)) {
            char *rest;
            errno = 0;
            int func_id = strtol(line+6, &rest, 0);
            if (errno!=0 || rest==line+6) {
                std::cerr <<argv0 <<": " <<input_name <<":" <<line_num
                          <<": syntax error in '##LAST' directive: " <<line+6 <<"\n";
                exit(1);
            }
            worklist.push(std::make_pair(FUNC_EVICT, func_id));

        } else {
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

            worklist.push(std::make_pair(std::min(func1_id, func2_id), std::max(func1_id, func2_id)));
            function_ids.insert(func1_id);
            function_ids.insert(func2_id);
        }
    }
    return worklist;
}

void
read_vector_data(const SqlDatabase::TransactionPtr &tx, scoped_array_with_size<VectorEntry>& vectors,
                 std::map<int, VectorEntry*>& id_to_vec)
{
    size_t eltCount = 0;
    SqlDatabase::StatementPtr cmd1 = tx->statement("select count(*) from semantic_functions");
    eltCount = cmd1->execute_int();

    vectors.allocate(eltCount);

    size_t indexInVectors=0;
    SqlDatabase::StatementPtr cmd3 = tx->statement("select id, ninsns, counts_b64  from semantic_functions");

    for (SqlDatabase::Statement::iterator r=cmd3->begin(); r!=cmd3->end(); ++r) {
      //Add feature vectors
      std::vector<uint8_t> counts = StringUtility::decode_base64(r.get<std::string>(2));
      std::string compressedCounts(&counts[0], &counts[0]+counts.size());

      int functionId = r.get<int64_t>(0);
      int ninsns     = r.get<int>(1);

      VectorEntry& ve = vectors[indexInVectors];
      ve.functionId = functionId;
      ve.indexWithinFunction = 0;
      ve.line = 0;
      ve.offset = 0;
      ve.rowNumber = 0;
      ve.ninsns = ninsns;

      ve.compressedCounts.allocate(compressedCounts.size());
      memcpy(ve.compressedCounts.get(), compressedCounts.data(), compressedCounts.size());

      id_to_vec.insert(std::pair<int, VectorEntry*>(functionId,&ve));
      indexInVectors++;
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
            ::usage(0);
        } else if (!strncmp(argv[argno], "--aggregate=", 12)) {
            if (!strcmp(argv[argno]+12, "average")) {
                opt.aggregation = AG_AVERAGE;
            } else if (!strcmp(argv[argno]+12, "maximum")) {
                opt.aggregation = AG_MAXIMUM;
            } else if (!strcmp(argv[argno]+12, "minimum")) {
                opt.aggregation = AG_MINIMUM;
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
            if (opt.collection_ratio.first < 0 || opt.collection_ratio.second < 0 ||
                opt.collection_ratio.first > 1 || opt.collection_ratio.second > 1) {
                std::cerr <<argv0 <<": --collection-ratio requires one or two floating point values in [0..1]\n";
                exit(1);
            }
        } else if (!strncmp(argv[argno], "--file=", 7)) {
            opt.input_file_name = argv[argno]+7;
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
            } else if (!strcmp(argv[argno]+9, "dl-edit-distance")) {
                opt.output_cmp = OC_DAMERAU_LEVENSHTEIN;
            } else {
                std::cerr <<argv0 <<": unknown value for --ogroup switch: " <<argv[argno]+9 <<"\n"
                          <<argv0 <<": see --help for more info\n";
                exit(1);
            }
        } else if (!strcmp(argv[argno], "--progress")) {
            opt.show_progress = true;
        } else if (!strncmp(argv[argno], "--return-threshold=", 19)) {
            char *rest;
            opt.return_threshold = strtod(argv[argno]+19, &rest);
            if (rest==argv[argno]+19 || *rest || opt.return_threshold<0 || opt.return_threshold>1) {
                std::cerr <<argv0 <<": invalid value for --return-threshold: " <<argv[argno]+19 <<"\n";
                exit(1);
            }
        } else if (!strcmp(argv[argno], "--verbose")) {
            opt.verbose = true;
        } else if (!strcmp(argv[argno], "--dry-run")) {
            opt.dry_run = true;
        } else {
            std::cerr <<argv0 <<": unknown switch: " <<argv[argno] <<"\n"
                      <<argv0 <<": see --help for more info\n";
            exit(1);
        }
    }
    if (argno+1!=argc)
        ::usage(1);
    SqlDatabase::ConnectionPtr conn = SqlDatabase::Connection::create(argv[argno++]);
    transaction = conn->transaction();
    int64_t cmd_id = CloneDetection::start_command(transaction, argc, argv, "calculating function similarity");

    // Read function pairs from standard input or the file
    IdSet all_func_ids;
    FunctionPairs worklist;
    if (opt.input_file_name.empty()) {
        std::cerr <<argv0 <<": reading function pairs worklist from stdin...\n";
        worklist = load_worklist("stdin", stdin, all_func_ids/*out*/);
    } else {
        FILE *in = fopen(opt.input_file_name.c_str(), "r");
        if (NULL==in) {
            std::cerr <<argv0 <<": " <<strerror(errno) <<": " <<opt.input_file_name <<"\n";
            exit(1);
        }
        worklist = load_worklist(opt.input_file_name, in, all_func_ids/*out*/);
        fclose(in);
    }
    size_t npairs = worklist.size();
    std::cerr <<argv0 <<": work list has " <<npairs <<" function pair" <<(1==npairs?"":"s") <<"\n";


    // Process each function pair
    CloneDetection::Progress progress(npairs);
    progress.force_output(opt.show_progress);


    scoped_array_with_size<VectorEntry> allVectors;
    std::map<int, VectorEntry* > id_to_vec;

    read_vector_data(transaction, allVectors, id_to_vec);


    SqlDatabase::StatementPtr stmt = transaction->statement("insert into semantic_funcsim"
                                                            "(func1_id, func2_id, similarity, ncompares, maxcompares,"
                                                            " relation_id, cmd, hamming_d, euclidean_d, euclidean_d_ratio,"
                                                            "path_ave_hamming_d, path_min_hamming_d, path_max_hamming_d,"
                                                            "path_ave_euclidean_d, path_min_euclidean_d, path_max_euclidean_d,"
                                                            "path_ave_euclidean_d_ratio, path_min_euclidean_d_ratio,"
                                                            "path_max_euclidean_d_ratio)"
                                                            " values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?,?,?,?,?,?,?,?,?,?)");
    CachedOutputs all_outputs;
    FunctionOutputs func_outputs;
    ObjUsage ogroup_usage; // output group usage by function_outputs

    load_function_infos(all_func_ids, ogroup_usage/*out*/);
    while (!worklist.empty()) {
        ++progress;
        int func1_id, func2_id;
        boost::tie(func1_id, func2_id) = worklist.shift();

        if (FUNC_EVICT==func1_id) {
            // This function will never be used again, so we can delete all the stuff it uses. However, we need to be careful
            // about deleting output groups because they can be used by multiple functions.  Therefore, we decrement the output
            // group use count to account for this function going away, and delete the output groups that have a resulting zero
            // use count.  Actually, we only remove the output groups from the cache, and boost::smart_ptr will take care of
            // deleting them eventually.
            if (opt.verbose)
                std::cerr <<argv0 <<": evicting function " <<func2_id <<" from cache\n";

            FuncInfos::iterator fii = func_infos.find(func2_id);
            assert(fii!=func_infos.end());
            const FuncInfo &finfo = fii->second;
            for (ObjUsage::const_iterator oui=finfo.ogroup_usage.begin(); oui!=finfo.ogroup_usage.end(); ++oui) {
                int64_t ogroup_id = oui->first;
                size_t nuses = oui->second;
                assert(ogroup_usage[ogroup_id] >= nuses);
                if (0 == (ogroup_usage[ogroup_id] -= nuses)) {
                    if (opt.verbose)
                        std::cerr <<argv0 <<": evicting output group " <<ogroup_id <<" from cache\n";
                    all_outputs.erase(ogroup_id);
                }
            }
            func_outputs.erase(func2_id);
            func_infos.erase(func2_id);

        } else {
            if (opt.verbose)
                std::cerr <<argv0 <<": func1_id=" <<func1_id <<" func2_id=" <<func2_id <<"\n";
            assert(func1_id < func2_id);
            load_function_outputs(all_outputs, func_outputs, func1_id);
            load_function_outputs(all_outputs, func_outputs, func2_id);
            const CachedOutputs &f1_outs = find_outputs(func_outputs, func1_id);
            const CachedOutputs &f2_outs = find_outputs(func_outputs, func2_id);
            size_t ncompares, maxcompares;
            OutputSimilarity output_sim = similarity(func_infos[func1_id], func_infos[func2_id], f1_outs, f2_outs,
                                                     ncompares/*out*/, maxcompares/*out*/);

            double sim;
            // Compute aggreged value for these two functions
            switch (opt.aggregation) {
                case AG_AVERAGE:
                    sim = output_sim.ave_semantic_sim; break;
                case AG_MAXIMUM:
                    sim = output_sim.max_semantic_sim; break;
                case AG_MINIMUM:
                    sim = output_sim.min_semantic_sim; break;
                default:
                    assert(!"not handled");
                    abort();
            }

            std::map<int, VectorEntry*>::iterator it;
            it = id_to_vec.find(func1_id);
            if (it == id_to_vec.end()) {
                assert(!"func 1 not found");
                exit(1);
            }

            VectorEntry* f1_compressed = it->second;
            it = id_to_vec.find(func2_id);
            if (it == id_to_vec.end()) {
                assert(!"func 2 not found");
                exit(1);
            }

            VectorEntry* f2_compressed = it->second;
            int vec_length = SignatureVector::Size;
            boost::scoped_array<uint16_t> f1_uncompressed(new uint16_t[vec_length]);
            decompressVector(f1_compressed->compressedCounts.get(), f1_compressed->compressedCounts.size(),
                             f1_uncompressed.get());

            boost::scoped_array<uint16_t> f2_uncompressed(new uint16_t[vec_length]);
            decompressVector(f2_compressed->compressedCounts.get(), f2_compressed->compressedCounts.size(),
                             f2_uncompressed.get());

            int hamming_d            = 0;
            double euclidean_d       = 0;
            double euclidean_d_ratio = 0;
            int f1_v=0;
            int f2_v=0;
            for (int i = 0; i < vec_length; i++) {
                f1_v = f1_uncompressed[i];
                f2_v = f2_uncompressed[i];

                if (f1_v != f2_v)
                    hamming_d++;
                euclidean_d += (f1_v - f2_v)*(f1_v - f2_v);

            }

            hamming_d = hamming_d;
            euclidean_d = sqrt(euclidean_d);
            int difference = abs(f1_compressed->ninsns-f2_compressed->ninsns);
            if (difference != 0) {
                euclidean_d_ratio = 100.0*euclidean_d/(abs(f1_compressed->ninsns+f2_compressed->ninsns));
            }

            if (opt.verbose)
                std::cerr <<argv0 <<": similarity(func1=" <<func1_id <<", func2=" <<func2_id <<") = " <<sim <<"\n";
            stmt->bind(0, func1_id);
            stmt->bind(1, func2_id);
            stmt->bind(2, sim);
            stmt->bind(3, ncompares);
            stmt->bind(4, maxcompares);
            stmt->bind(5, opt.relation_id);
            stmt->bind(6, cmd_id);
            stmt->bind(7, hamming_d);
            stmt->bind(8, euclidean_d);
            stmt->bind(9, euclidean_d_ratio);
            stmt->bind(10, output_sim.ave_hamming_d);
            stmt->bind(11, output_sim.min_hamming_d);
            stmt->bind(12, output_sim.max_hamming_d);
            stmt->bind(13, output_sim.ave_euclidean_d);
            stmt->bind(14, output_sim.min_euclidean_d);
            stmt->bind(15, output_sim.max_euclidean_d);
            stmt->bind(16, output_sim.ave_euclidean_d_ratio);
            stmt->bind(17, output_sim.min_euclidean_d_ratio);
            stmt->bind(18, output_sim.max_euclidean_d_ratio);

            stmt->execute();
        }
    }

    progress.message("committing changes");
    std::string mesg = "calculated similarity relationship #"+StringUtility::numberToString(opt.relation_id)+
                       " for "+StringUtility::numberToString(npairs)+" function pair"+(1==npairs?"":"s");
    CloneDetection::finish_command(transaction, cmd_id, mesg);

    if (opt.dry_run) {
        transaction->rollback();
    } else {
        transaction->commit();
    }
    progress.clear();
    return 0;
}
