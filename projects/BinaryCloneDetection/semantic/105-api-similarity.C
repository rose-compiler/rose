#include <cerrno>
#include "sage3basic.h"
#include "CloneDetectionLib.h"
#include "rose_getline.h"
#include <EditDistance/DamerauLevenshtein.h>

#include <cerrno>

#include <algorithm>
#include <set>
#include <iterator>

using namespace rose;

std::string argv0;

static SqlDatabase::TransactionPtr transaction;

static void
usage(int exit_status)
{
    std::cerr <<"usage: " <<argv0 <<" [SWITCHES] [--] DATABASE \n"
              <<"This command computes the API Call similarity.\n"
              <<"\n"
              <<"  These switches control how api call traces are compared:\n"
              <<"    --call-depth=-1|0|..|MAXINT\n"
              <<"            Controls which API Calls of the trace is considered part of the similarity computation.\n"
              <<"            Options are: -1 is all calls, 0 is only direct calls, or a custom depth from 1\n"
              <<"            to some arbitrary depth.\n"
              <<"    --ignore-inline-candidates\n"
              <<"            Ignore functions present in only one of the traces that has a semantic equivalent in both\n"
              <<"            functions compilation unit.\n"
              <<"    --ignore-no-compares\n"
              <<"            Ignore functions that in the semantic clone detection never succeeded for any test.\n"
              <<"\n"
              <<"  Other switches and arguments:\n"
              <<"    --file=NAME\n"
              <<"            Read pairs from this file instead of standard input.\n"
              <<"    --verbose\n"
              <<"            Show lots of diagnostics.\n"
              <<"\n"
              <<"    DATABASE\n"
              <<"            The name of the database to which we are connecting.  For SQLite3 databases this is just a local\n"
              <<"            file name that will be created if it doesn't exist; for other database drivers this is a URL\n"
              <<"            containing the driver type and all necessary connection parameters.\n";
    exit(exit_status);
}

typedef WorkList< std::pair<int/*func1_id*/, int/*func2_id*/> > FunctionPairs;

static FunctionPairs
load_worklist(const std::string &input_name, FILE *f)
{
    FunctionPairs worklist;
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

        worklist.push(std::make_pair(std::min(func1_id, func2_id), std::max(func1_id, func2_id)));
    }
    return worklist;
}


void
computational_equivalent_classes(std::map<int,int>& norm_map)
{
    SqlDatabase::StatementPtr stmt = transaction->statement("select func_id, equivalent_func_id from equivalent_classes");
    for (SqlDatabase::Statement::iterator row=stmt->begin(); row!= stmt->end(); ++row)
        norm_map[row.get<int>(0)] = row.get<int>(1);
}

typedef std::vector<int> CallVec;
typedef std::multiset<int> MSet;

CallVec*
load_api_calls_for(int func_id, int igroup_id, bool ignore_no_compares, int call_depth, bool expand_ncalls)
{
    SqlDatabase::StatementPtr stmt = transaction->statement("select distinct fio.pos, fio.callee_id, fio.ncalls"
                                                            " from semantic_fio_calls as fio"
                                                            " join tmp_interesting_funcs as f1"
                                                            // filter out functions with no compares
                                                            " on f1.func_id = fio.callee_id"
                                                            // filter on current parameters
                                                            " where fio.func_id = ? and fio.igroup_id = ?"
                                                            // filter out function not called directly
                                                            + std::string(call_depth >= 0 ? " and fio.caller_id = ?" : "")
                                                            +" order by fio.pos");
    stmt->bind(0, func_id);
    stmt->bind(1, igroup_id);

    if (call_depth >= 0)
        stmt->bind(2, func_id);

    CallVec* call_vec = new CallVec;
    for (SqlDatabase::Statement::iterator row=stmt->begin(); row!=stmt->end(); ++row) {
        int callee_id = row.get<int>(1);
        int ncalls    = row.get<int>(2);

        if (expand_ncalls) {
            for (int i = 0; i < ncalls; i++)
                call_vec->push_back(callee_id);
        } else {
            call_vec->push_back(callee_id);
        }
    }
    return call_vec;
}

CallVec*
load_function_api_calls_for(int func_id, bool reachability_graph)
{
    SqlDatabase::StatementPtr stmt = transaction->statement("select distinct scg.callee from "
                                                            + std::string(reachability_graph ? "semantic_rg" : "semantic_cg ") +
                                                            " as scg "
                                                            //" join tmp_interesting_funcs as tif on tif.func_id = scg.callee "
                                                            " where scg.caller=? ORDER BY scg.callee");
    stmt->bind(0, func_id);

    CallVec* call_vec = new CallVec;
    for (SqlDatabase::Statement::iterator row=stmt->begin(); row!=stmt->end(); ++row) {
        int callee_id = row.get<int>(0);
        call_vec->push_back(callee_id);
    }
    return call_vec;
}

/* Remove the functions from the compilation unit that is only available in one of the traces.
 *   - criteria complement of the functions from the files of the caller functions in the call trace is removed. */
std::pair<CallVec*, CallVec*>
remove_compilation_unit_complement(int func1_id, int func2_id, int igroup_id, int similarity, CallVec* func1_vec,
                                   CallVec* func2_vec)
{
    CallVec* new_func1_vec = new CallVec;
    CallVec* new_func2_vec = new CallVec;

    if (func1_vec->size() > 0 || func2_vec->size() > 0) {
        // Find the set complement of functions called by the two functions
        // - we are not interested in functions called by both
        std::set<int> func1_vec_set;
        std::set<int> func2_vec_set;

        for (CallVec::iterator it = func1_vec->begin(); it != func1_vec->end(); ++it)
            func1_vec_set.insert(*it);
        for (CallVec::iterator it = func2_vec->begin(); it != func2_vec->end(); ++it)
            func2_vec_set.insert(*it);

        std::set<int> func1_func2_complement;
        std::set_difference(func1_vec_set.begin(), func1_vec_set.end(), func2_vec_set.begin(), func2_vec_set.end(),
                            std::inserter(func1_func2_complement, func1_func2_complement.end()));

        // Find the compilation units in question. A compilation unit is in our case a file.
        SqlDatabase::StatementPtr func1_file_stmt = transaction->statement("select file_id from semantic_functions"
                                                                           " where id = ?");
        func1_file_stmt->bind(0, func1_id);
        int func1_file_id = func1_file_stmt->execute_int();

        SqlDatabase::StatementPtr func2_file_stmt = transaction->statement("select file_id from semantic_functions"
                                                                           " where id = ?");
        func2_file_stmt->bind(0, func2_id);
        int func2_file_id = func2_file_stmt->execute_int();

        // Find the functions that needs to be removed
        //  - all functions that has a clone in between the files
        SqlDatabase::StatementPtr stmt = transaction->statement("select sem.func1_id, sem.func2_id from semantic_funcsim as sem"
                                                                " join semantic_functions as sf1 on sem.func1_id = sf1.id"
                                                                " join semantic_functions as sf2 on sem.func2_id = sf2.id"
                                                                " where similarity >= ? and sf1.file_id in (?,?)"
                                                                "   and sf2.file_id in (?, ?) and sf1.file_id != sf2.file_id");
        stmt->bind(0, similarity);
        stmt->bind(1, func1_file_id);
        stmt->bind(2, func2_file_id);
        stmt->bind(3, func1_file_id);
        stmt->bind(4, func2_file_id);

        std::set<int> complement_functions;
        for (SqlDatabase::Statement::iterator row=stmt->begin(); row!=stmt->end(); ++row) {
            int clone_func1 = row.get<int>(0);
            int clone_func2 = row.get<int>(1);

            complement_functions.insert(clone_func1);
            complement_functions.insert(clone_func2);
        }

        // Find the functions we want to remove
        //  - functions present with clones in between the files that is not part of both traces
        std::set<int> remove_these;
        std::set_intersection(complement_functions.begin(), complement_functions.end(), func1_func2_complement.begin(),
                              func1_func2_complement.end(), std::inserter(remove_these, remove_these.end()));

        //prune functions to remove away from the call trace into new vectors
        for (CallVec::iterator it = func1_vec->begin(); it != func1_vec->end(); ++it) {
            if (remove_these.find(*it) == remove_these.end())
                new_func1_vec->push_back(*it);
        }

        for (CallVec::iterator it = func2_vec->begin(); it != func2_vec->end(); ++it) {
            if (remove_these.find(*it) == remove_these.end())
                new_func2_vec->push_back(*it);
        }
    }
    return std::pair<CallVec*, CallVec*>(new_func1_vec, new_func2_vec);
}

double
similarity(int func1_id, int func2_id, int igroup_id, double similarity, bool ignore_inline_candidates, bool ignore_no_compares,
           int call_depth, bool expand_ncalls, std::map<int,int>& norm_map)
{
    CallVec* func1_vec = load_api_calls_for(func1_id, igroup_id, ignore_no_compares, call_depth, expand_ncalls);
    CallVec* func2_vec = load_api_calls_for(func2_id, igroup_id, ignore_no_compares, call_depth, expand_ncalls);

    // Detect and normalize similar function calls
    if ((func1_vec->size() == 0) & (func2_vec->size() == 0))
        return -1;

    for (CallVec::iterator it = func1_vec->begin(); it != func1_vec->end(); ++it) {
        std::map<int,int>::iterator located_it = norm_map.find(*it);
        if (located_it != norm_map.end())
            *it = located_it->second;
    }

    for (CallVec::iterator it = func2_vec->begin(); it != func2_vec->end(); ++it) {
        std::map<int,int>::iterator located_it = norm_map.find(*it);
        if (located_it != norm_map.end())
            *it = located_it->second;
    }

    // Remove possible inlined functions from the traces
    if (ignore_inline_candidates) {
        std::pair<CallVec*, CallVec*> removed_complement = remove_compilation_unit_complement(func1_id, func2_id, igroup_id,
                                                                                              similarity, func1_vec, func2_vec);
        delete func1_vec;
        delete func2_vec;
        func1_vec = removed_complement.first;
        func2_vec = removed_complement.second;
    }

    if (func1_vec->size() == 0 && func2_vec->size() == 0)
        return -1;

    size_t dl_max = std::max(func1_vec->size(), func2_vec->size());
    double dl_similarity = 1.0;
    if (dl_max > 0) {
        size_t dl = EditDistance::damerauLevenshteinDistance(*func1_vec, *func2_vec);
        dl_similarity = 1.0 - (double)dl / dl_max;
    }

    delete func1_vec;
    delete func2_vec;

    return dl_similarity;
}

double
multiset_jaccard(CallVec* vec1, CallVec* vec2)
{
    MSet ms1, ms2;

    ms1.insert(vec1->begin(), vec1->end());
    ms2.insert(vec2->begin(), vec2->end());

    typedef std::vector<CloneDetection::OutputGroup::value_type> Vector;

    std::vector<int> sunion(ms1.size()+ms2.size(), 0);
    std::vector<int> sinter(std::max(ms1.size(), ms2.size()));
    std::vector<int>::iterator ui = std::set_union(ms1.begin(), ms1.end(), ms2.begin(), ms2.end(), sunion.begin());
    std::vector<int>::iterator ii = std::set_intersection(ms1.begin(), ms1.end(), ms2.begin(), ms2.end(), sinter.begin());
    size_t usize = ui-sunion.begin();
    size_t isize = ii-sinter.begin();

    return usize ? (double)isize/usize : 1.0;
}

double
whole_function_similarity(int func1_id, int func2_id, std::map<int,int>& norm_map, bool reachability_graph)
{
    CallVec* func1_vec = load_function_api_calls_for(func1_id, reachability_graph);
    CallVec* func2_vec = load_function_api_calls_for(func2_id, reachability_graph);

    // Normalize functions
    for (CallVec::iterator it = func1_vec->begin(); it != func1_vec->end(); ++it) {
        std::map<int,int>::iterator located_it = norm_map.find(*it);
        if (located_it != norm_map.end())
            *it = located_it->second;
    }

    for (CallVec::iterator it = func2_vec->begin(); it != func2_vec->end(); ++it) {
        std::map<int,int>::iterator located_it = norm_map.find(*it);
        if (located_it != norm_map.end())
            *it = located_it->second;
    }

    // Compute similarity
    double dl_similarity = multiset_jaccard(func1_vec, func2_vec);
    delete func1_vec;
    delete func2_vec;
    return dl_similarity;
}

class FunctionPair {
public:
    int func1_id;
    int func2_id;
    FunctionPair(int _func1_id, int _func2_id) :  func1_id(_func1_id), func2_id(_func2_id) {};
};

typedef std::vector<FunctionPair*> FunctionPairVec;

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

    bool ignore_inline_candidates = false;
    bool ignore_no_compares = false;
    int  call_depth = -1;
    bool ignore_faults = true;
    double semantic_similarity_threshold = 0.70;
    bool expand_ncalls = false;
    bool reachability_graph = true;
    bool show_progress = false;
    bool verbose = false;
    std::string input_file_name;

    int argno = 1;
    for (/*void*/; argno<argc && '-'==argv[argno][0]; ++argno) {
        if (!strcmp(argv[argno], "--")) {
            ++argno;
            break;
        } else if (!strcmp(argv[argno], "--help") || !strcmp(argv[argno], "-h")) {
            ::usage(0);
        } else if (!strcmp(argv[argno], "--ignore-inline-candidates")) {
            ignore_inline_candidates = true;
        } else if (!strcmp(argv[argno], "--ignore-no-compares")) {
            ignore_no_compares = false;
        } else if (!strcmp(argv[argno], "--progress")) {
            show_progress = true;
        } else if (!strcmp(argv[argno], "--no-expand-ncalls")) {
            expand_ncalls = false;
        } else if (!strncmp(argv[argno], "--file=", 7)) {
            input_file_name = argv[argno]+7;
        } else if (!strcmp(argv[argno], "--verbose")) {
            verbose = true;
        } else if (!strncmp(argv[argno], "--call-depth=",13)) {
            call_depth = strtol(argv[argno]+13, NULL, 0);
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
    int64_t cmd_id = CloneDetection::start_command(transaction, argc, argv, "calculating api similarity");

    // Read function pairs from standard input or the file
    FunctionPairs worklist;
    if (input_file_name.empty()) {
        std::cerr <<argv0 <<": reading function pairs worklist from stdin...\n";
        worklist = load_worklist("stdin", stdin);
    } else {
        FILE *in = fopen(input_file_name.c_str(), "r");
        if (NULL==in) {
            std::cerr <<argv0 <<": " <<strerror(errno) <<": " << input_file_name <<"\n";
            exit(1);
        }
        worklist = load_worklist(input_file_name, in);
        fclose(in);
    }
    size_t npairs = worklist.size();
    std::cerr <<argv0 <<": work list has " <<npairs <<" function pair" <<(1==npairs?"":"s") <<"\n";

    // Process each function pair
    CloneDetection::Progress progress(npairs);
    progress.force_output(show_progress);

    // Load the computational equivalence classes
    std::map<int,int> norm_map;
    computational_equivalent_classes(norm_map);

    // Create list of functions and igroups to analyze
    SqlDatabase::StatementPtr insert_stmt = transaction->statement("insert into api_call_similarity"
                                                                   "(func1_id, func2_id, max_similarity, min_similarity,"
                                                                   " ave_similarity, cg_similarity)"
                                                                   " values (?, ?, ?, ?, ?, ?)");
    while (!worklist.empty()) {
        ++progress;
        int func1_id, func2_id;
        boost::tie(func1_id, func2_id) = worklist.shift();
        if (verbose)
            std::cerr <<argv0 <<": func1_id=" <<func1_id <<" func2_id=" <<func2_id <<"\n";

        SqlDatabase::StatementPtr igroup_stmt = transaction->statement("select distinct sem1.igroup_id"
                                                                       " from semantic_fio as sem1 "
                                                                       " join semantic_fio as sem2"
                                                                       "   on sem2.igroup_id = sem1.igroup_id"
                                                                       "   and sem2.func_id = ?"
                                                                       " where sem1.func_id = ? " +
                                                                       std::string(ignore_faults ?
                                                                                   " and sem1.status = 0 and sem2.status = 0" :
                                                                                   "") +
                                                                       " order by sem1.igroup_id");
        igroup_stmt->bind(0, func2_id);
        igroup_stmt->bind(1, func1_id);

        int ncompares = 0;
        double max_api_similarity = 0;
        double min_api_similarity = INT_MAX;
        double ave_api_similarity = 0;
        for (SqlDatabase::Statement::iterator row=igroup_stmt->begin(); row!=igroup_stmt->end(); ++row) {
            int igroup_id = row.get<int>(0);
            double api_similarity = similarity(func1_id, func2_id, igroup_id, semantic_similarity_threshold,
                                               ignore_inline_candidates, ignore_no_compares, call_depth, expand_ncalls, norm_map);

            if (api_similarity < 0)
                continue;

            max_api_similarity = std::max(api_similarity, max_api_similarity);
            min_api_similarity = std::min(api_similarity, min_api_similarity);
            ave_api_similarity += api_similarity;
            ncompares++;
        }

        if (ncompares == 0) {
            ave_api_similarity = 1.0;
            max_api_similarity = 1.0;
            min_api_similarity = 1.0;
        } else {
            ave_api_similarity = ave_api_similarity/ncompares;
        }

        // Find call similarity between functions
        double cg_similarity = whole_function_similarity(func1_id, func2_id, norm_map, reachability_graph);

        insert_stmt->bind(0, func1_id);
        insert_stmt->bind(1, func2_id);
        insert_stmt->bind(2, max_api_similarity);
        insert_stmt->bind(3, min_api_similarity);
        insert_stmt->bind(4, ave_api_similarity);
        insert_stmt->bind(5, cg_similarity);

        insert_stmt->execute();
    }

    progress.message("committing changes");
    std::string mesg = "calculated api similarity for "+
                       StringUtility::numberToString(npairs)+" function pair"+(1==npairs?"":"s");
    CloneDetection::finish_command(transaction, cmd_id, mesg);
    transaction->commit();
    progress.clear();
    return 0;
}
