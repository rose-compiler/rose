// Creates a clusters table from a pairs table.

#include "sage3basic.h"
#include "CloneDetectionLib.h"

std::string argv0;

static void
usage(int exit_status)
{
    std::cerr <<"usage: " <<argv0 <<" [SWITCHES] [--] DATABASE PAIRS_TABLE CLUSTERS_TABLE\n"
              <<"  This command reads the list of function similarity pairs and builds clusters, storing the result in\n"
              <<"  the specified CLUSTERS_TABLE.\n"
              <<"\n"
              <<"    --threshold=T\n"
              <<"            Only function pairs whose similarity is at least T are considered to be similar enough\n"
              <<"            to be in the same cluster.  All pairs of functions in a cluster thus have a similarity\n"
              <<"            of at least T. The default is 0.75.\n"
              <<"\n"
              <<"    DATABASE\n"
              <<"            The name of the database to which we are connecting.  For SQLite3 databases this is just a local\n"
              <<"            file name that will be created if it doesn't exist; for other database drivers this is a URL\n"
              <<"            containing the driver type and all necessary connection parameters.\n"
              <<"    PAIRS_TABLE\n"
              <<"            The name of the table containing function similarity pairs.  This table should have at least\n"
              <<"            these three columns: \"func1_id\" and \"func2_id\" are function ID numbers, and \"similarity\"\n"
              <<"            is a floating point value between zero and one, inclusive, that indicates how similar the\n"
              <<"            two functions are.  Since similarity is reflexive and symmetric, the table only needs to\n"
              <<"            contain rows where \"func1_id\" is less than \"func2_id\".\n"
              <<"    CLUSTERS_TABLE\n"
              <<"            The name of the table that will contain cluster information.  This table is droped and\n"
              <<"            recreated to contain two columns: \"id\", the cluster identification number chosen\n"
              <<"            sequentially starting at zero; and \"func_id\", the ID number of a function belonging to this\n"
              <<"            cluster.\n";
    exit(exit_status);
}

struct Switches {
    Switches(): threshold(0.75) {}
    double threshold;
};

typedef std::set<int/*func_id*/> Functions;

struct FuncPair {
    int func1_id, func2_id;
    FuncPair()
        : func1_id(-1), func2_id(-1) {}
    FuncPair(int func1_id, int func2_id)
        : func1_id(std::min(func1_id, func2_id)),
          func2_id(std::max(func1_id, func2_id)) {}
    bool operator<(const FuncPair &other) const {
        if (func1_id!=other.func1_id)
            return func1_id < other.func1_id;
        return func2_id < other.func2_id;
    }
};

typedef std::map<FuncPair, double> Similarities;

// Complete subgraph
struct Clique {
    Functions functions;
    Clique(const Functions &functions): functions(functions) {}
    void insert(int func_id) { functions.insert(func_id); }
    bool operator<(const Clique &other) const {
        if (functions.size()!=other.functions.size())
            return functions.size() < other.functions.size();
        std::pair<Functions::const_iterator, Functions::const_iterator> ip = std::mismatch(functions.begin(), functions.end(),
                                                                                           other.functions.begin());
        return ip.first!=functions.end() && *ip.first < *ip.second;
    }
};

typedef std::set<Clique> Cliques;

static bool
is_valid_table_name(const std::string &name)
{
    static const char *valid_chars = "abcdefghijklmnopqrstuvwxyz"
                                     "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                     "0123456789"
                                     "_";
    return std::string::npos == name.find_first_not_of(valid_chars);
}

static double
similarity(const Similarities &similarities, int func1_id, int func2_id)
{
    if (func1_id==func2_id)
        return 1.0;
    Similarities::const_iterator found = similarities.find(FuncPair(func1_id, func2_id));
    return found == similarities.end() ? 0.0 : found->second;
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

    // Parse command line
    Switches opt;
    int argno = 1;
    for (/*void*/; argno<argc && '-'==argv[argno][0]; ++argno) {
        if (!strcmp(argv[argno], "--")) {
            ++argno;
            break;
        } else if (!strcmp(argv[argno], "--help") || !strcmp(argv[argno], "-h")) {
            usage(0);
        } else if (!strncmp(argv[argno], "--threshold=", 12)) {
            opt.threshold = strtod(argv[argno]+12, NULL);
        } else {
            std::cerr <<argv0 <<": unknown switch: " <<argv[argno] <<"\n"
                      <<argv0 <<": see --help for more info\n";
            exit(1);
        }
    };
    if (argno+3!=argc)
        usage(1);
    SqlDatabase::ConnectionPtr conn = SqlDatabase::Connection::create(argv[argno++]);
    SqlDatabase::TransactionPtr tx = conn->transaction();
    std::string pairs_table = argv[argno++];
    std::string clusters_table = argv[argno++];
    if (!is_valid_table_name(pairs_table)) {
        std::cerr <<argv0 <<": not a valid table name: " <<pairs_table <<"\n";
        exit(1);
    }
    if (!is_valid_table_name(clusters_table)) {
        std::cerr <<argv0 <<": not a valid table name: " <<clusters_table <<"\n";
        exit(1);
    }
    int64_t cmd_id = CloneDetection::start_command(tx, argc, argv, "generating clusters from pairs");

    // Count how much work to do
    size_t npairs = tx->statement("select count(*) from "+pairs_table+" where func1_id<func2_id and similarity>=?")
                    ->bind(0, opt.threshold)->execute_int();
    CloneDetection::Progress progress(npairs);

    // Recreate the output table.
    progress.message("recreating cluster table");
    tx->execute("drop table if exists "+clusters_table);
    tx->execute("create table "+clusters_table+" as select * from semantic_clusters_tpl limit 0");
    progress.message("");

    // Load all function similarities so we don't have to make lots of queries
    Similarities similarities;
    Functions all_functions;
    SqlDatabase::StatementPtr stmt1 = tx->statement("select func1_id, func2_id, similarity from "+pairs_table+
                                                    " where func1_id < func2_id and similarity >= ?")->bind(0, opt.threshold);
    progress.clear();
    std::cerr <<argv0 <<": loading function similarity pairs\n";
    for (SqlDatabase::Statement::iterator row=stmt1->begin(); row!=stmt1->end(); ++row) {
        ++progress;
        FuncPair fpair(row.get<int>(0), row.get<int>(1));
        double similarity = row.get<double>(2);
        similarities[fpair] = similarity;
        all_functions.insert(fpair.func1_id);
        all_functions.insert(fpair.func2_id);
    }


    // Find maximal graph cliques (aka, clusters)
    progress.clear();
    std::cerr <<argv0 <<": computing clusters\n";
    progress.reset(0, all_functions.size()*all_functions.size());
    Cliques cliques;
    for (Functions::iterator fi1=all_functions.begin(); fi1!=all_functions.end(); ++fi1) {

        // Find maximal clique starting with vertex *fi1
        Functions clique;
        clique.insert(*fi1);
        for (Functions::iterator fi2=all_functions.begin(); fi2!=all_functions.end(); ++fi2) {
            ++progress;
            if (fi1!=fi2) {
                bool for_all = true;
                for (Functions::iterator ci=clique.begin(); ci!=clique.end() && for_all; ++ci)
                    for_all = similarity(similarities, *fi1, *fi2) >= opt.threshold;
                if (for_all)
                    clique.insert(*fi2);
            }
        }

        cliques.insert(clique);
    }

    // Write cliques to database
    progress.clear();
    std::cerr <<argv0 <<": writing clusters to database\n";
    progress.reset(0, cliques.size());
    SqlDatabase::StatementPtr stmt2 = tx->statement("insert into "+clusters_table+" (id, func_id) values (?, ?)");
    size_t cluster_id=0, largest_clique=0;
    for (Cliques::iterator ci=cliques.begin(); ci!=cliques.end(); ++ci, ++cluster_id) {
        ++progress;
        largest_clique = std::max(largest_clique, ci->functions.size());
        for (Functions::iterator fi=ci->functions.begin(); fi!=ci->functions.end(); ++fi) {
            stmt2->bind(0, cluster_id);
            stmt2->bind(1, *fi);
            stmt2->execute();
        }
    }

    // Cleanup
    std::string mesg = "created "+StringUtility::numberToString(cliques.size())+" cluster"+(1==cliques.size()?"":"s");
    if (!cliques.empty())
        mesg += ", largest having "+StringUtility::numberToString(largest_clique)+" function"+(1==largest_clique?"":"s");
    progress.clear();
    std::cerr <<argv0 <<": " <<mesg <<"\n";
    progress.message("committing changes");
    CloneDetection::finish_command(tx, cmd_id, mesg);
    tx->commit();
    progress.clear();
    return 0;
}
