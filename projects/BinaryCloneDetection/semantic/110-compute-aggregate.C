#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/median.hpp>
#include <boost/accumulators/statistics/min.hpp>
#include <boost/accumulators/statistics/max.hpp>
#include <boost/accumulators/statistics/moment.hpp>
#include <boost/accumulators/statistics/variance.hpp>
#include <boost/lexical_cast.hpp>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/incremental_components.hpp>
#include <boost/pending/disjoint_sets.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/visitors.hpp>

#include <boost/foreach.hpp>

#include "sage3basic.h"
#include "CloneDetectionLib.h"

using namespace boost;

typedef boost::adjacency_list< boost::vecS,
                               boost::vecS,
                               boost::directedS > DirectedGraph;

namespace FailureEvaluation {
    extern const char *failure_schema;
};

using namespace CloneDetection;
using namespace FailureEvaluation;
using namespace boost::accumulators;

std::string argv0;


static void
usage(int exit_status)
{
    std::cerr <<"usage: " <<argv0 <<" [SWITCHES] \n"
              <<"This command computes the API Call similarity.\n"
              <<"\n"
              <<"  These switches control how api call traces are compared:\n"
              <<"    --sem-threshold=0.0|..|1.0\n"
              <<"            Similarity measure for semantic similarity between 0 and 1.\n"
              <<"    --path-threshold=0.0|..|1.0\n"
              <<"            Path sensitive similarity threshold between 0 and 1.\n"
              <<"    --hamming-threshold=0|..|MAX_INT\n"
              <<"            Function syntactic hamming threshold.\n"
              <<"    --euclidean-threshold=0|..|MAX_INT\n"
              <<"            Function syntactic euclidean threshold.\n"
              <<"    --cg-threshold=0.0|..|1.0\n"
              <<"            Call graph similarity threshold between 0 and 1.\n"
              <<"    --min-insns=0|..|MAX_INT\n"
              <<"            Minimum number of instructions in candidate functions.\n"
              <<"    --max-cluster-size=0|..|MAX_INT\n"
              <<"            Maximum number of functions in a cluster. All functions that \n"
              <<"            are part of a larger cluster will be ignored.   \n"
              <<"\n"
              <<"    DATABASE\n"
              <<"            The name of the database to which we are connecting.  For SQLite3 databases this is just a local\n"
              <<"            file name that will be created if it doesn't exist; for other database drivers this is a URL\n"
              <<"            containing the driver type and all necessary connection parameters.\n";
    exit(exit_status);
}



/****************************************************************************************
 *
 * Compute how many functions pairs are x%- bucket_size < x% <= x%+bucket size similar to each other.
 *
 * The goal is to show that the semantic clone detection is a binary function.
 *
 * The result is inserted into fr_percent_similar on the test db, and fr_percent_similar on
 * the global db.
 *
 */

void
compute_percent_similarity_statistics(double bucket_size, double increment,  SqlDatabase::TransactionPtr transaction)
{
    int num_pairs = transaction->statement("select count(*) from semantic_funcsim")->execute_int();
    transaction->execute("drop table IF EXISTS fr_percent_similar");
    transaction->execute("create table fr_percent_similar(similarity_low double precision, similarity_middle double precision,"
                         " similarity_high double precision, percent double precision, num_matches integer);");

    SqlDatabase::StatementPtr pecent_similar_stmt = transaction->statement("insert into fr_percent_similar"
                                                                           // 0              1                  2
                                                                           "(similarity_low, similarity_middle, similarity_high,"
                                                                           // 3       4
                                                                           " percent, num_matches) "
                                                                           " values (?, ?, ?, ?, ?)");
    for (double cur_bucket = 0.0; cur_bucket <= 1.0+bucket_size; cur_bucket+=increment) {
        int num_matches = transaction->statement("select count(*) from semantic_funcsim where "
                                                 " similarity >= " +
                                                 boost::lexical_cast<std::string>(cur_bucket - bucket_size) +
                                                 " and similarity < " +
                                                 boost::lexical_cast<std::string>(cur_bucket + bucket_size))->execute_int();
        pecent_similar_stmt->bind(0, cur_bucket - bucket_size < 0 ? 0 : cur_bucket - bucket_size);
        pecent_similar_stmt->bind(1, cur_bucket);
        pecent_similar_stmt->bind(2, cur_bucket + bucket_size >= 1.0 ? 1.0 : cur_bucket + bucket_size);
        pecent_similar_stmt->bind(3, num_pairs > 0 ? ((double) num_matches*100.0)/num_pairs : 0);
        pecent_similar_stmt->bind(4, num_matches);
        pecent_similar_stmt->execute();
    }
}

/****************************************************************************************
 *
 *
 * Compute how mean similar functions are to all other functions.
 *
 * The result is inserted into fr_mean_similarity on the test db, and fr_mean_similar on
 * the global db.
 *
 */
void
compute_mean_similarity_statistics(double bucket_size, double increment, SqlDatabase::TransactionPtr transaction)
{
    int num_pairs = transaction->statement("select count(*) from semantic_funcsim")->execute_int();


    transaction->execute("drop table IF EXISTS fr_mean_similarity;");
    transaction->execute("create table fr_mean_similarity as  select coalesce(sum(sf.similarity)/"+
                         boost::lexical_cast<std::string>(num_pairs)+
                         " ,0) as similarity,  ttf.id as func_id from semantic_funcsim as sf"+
                         " join semantic_functions as ttf on ttf.id = sf.func1_id  OR ttf.id = sf.func2_id GROUP BY ttf.id");

    transaction->execute("drop table IF EXISTS fr_mean_similar");
    transaction->execute("create table fr_mean_similar(similarity_low double precision, similarity_middle double precision,"
                                                       " similarity_high double precision, percent double precision);");

    SqlDatabase::StatementPtr mean_similar_stmt = transaction->statement("insert into fr_mean_similar"
                                                                         // 0              1                  2
                                                                         "(similarity_low, similarity_middle, similarity_high,"
                                                                         // 3
                                                                         " percent) "
                                                                         " values (?, ?, ?, ?)");

    for (double cur_bucket = 0.0; cur_bucket <= 1.0+bucket_size; cur_bucket+=increment) {
        int num_matches = transaction->statement("select count(*) from fr_mean_similarity where "
                                                 " similarity >= " +
                                                 boost::lexical_cast<std::string>(cur_bucket - bucket_size) +
                                                 " and similarity < " +
                                                 boost::lexical_cast<std::string>(cur_bucket + bucket_size))->execute_int();
        mean_similar_stmt->bind(0, cur_bucket - bucket_size < 0 ? 0 : cur_bucket - bucket_size);
        mean_similar_stmt->bind(1, cur_bucket);
        mean_similar_stmt->bind(2, cur_bucket + bucket_size >= 1.0 ? 1.0 : cur_bucket + bucket_size);
        mean_similar_stmt->bind(3, num_pairs > 0 ? ((double) num_matches*100.0)/num_pairs : 0);
        mean_similar_stmt->execute();
    }
}

void
find_clusters(int max_cluster_size_signed, SqlDatabase::TransactionPtr transaction)
{
    assert(max_cluster_size_signed >= 0);
    size_t max_cluster_size = max_cluster_size_signed;

    SqlDatabase::StatementPtr insert_stmt = transaction->statement("insert into fr_ignored_function_pairs"
                                                                   // 0        1         2
                                                                   "(func1_id, func2_id, from_cluster_of_size)"
                                                                   " values (?, ?, ?)");

    //Get all vetexes and find the union
    std::string _query_condition = "select func1_id, func2_id from fr_clone_pairs";
    SqlDatabase::StatementPtr stmt = transaction->statement(_query_condition);

    if (stmt->begin() == stmt->end())
        return;

    //Count how many vertices we have for boost graph
    int VERTEX_COUNT = transaction->statement("select count(*) from semantic_functions")->execute_int();

    typedef adjacency_list <vecS, vecS, undirectedS> Graph;
    typedef graph_traits<Graph>::vertex_descriptor Vertex;
    typedef graph_traits<Graph>::vertices_size_type VertexIndex;
    Graph graph(VERTEX_COUNT);

    std::vector<VertexIndex> rank(num_vertices(graph));
    std::vector<Vertex> parent(num_vertices(graph));

    typedef VertexIndex* Rank;
    typedef Vertex* Parent;
    disjoint_sets<Rank, Parent> ds(&rank[0], &parent[0]);
    initialize_incremental_components(graph, ds);
    incremental_components(graph, ds);

    graph_traits<Graph>::edge_descriptor edge;
    bool flag;

    for (SqlDatabase::Statement::iterator row=stmt->begin(); row!=stmt->end(); ++row) {
        int func1 = row.get<int>(0);
        int func2 = row.get<int>(1);
        boost::tie(edge, flag) = add_edge(func1, func2, graph);
        ds.union_set(func1,func2);
    }

    typedef component_index<VertexIndex> Components;
    Components components(parent.begin(), parent.end());
    std::map<int,int> size_distribution;

    // Iterate through the component indices
    BOOST_FOREACH(VertexIndex current_index, components) {
        std::vector<int> cluster_functions;

        // Iterate through the child vertex indices for [current_index]
        BOOST_FOREACH(VertexIndex child_index, components[current_index]) {
            cluster_functions.push_back(child_index);
        }

        std::sort(cluster_functions.begin(), cluster_functions.end());
        std::map<int,int>::iterator it = size_distribution.find(cluster_functions.size());

        if (it == size_distribution.end()) {
            size_distribution[cluster_functions.size()] = 1;
        } else {
            ++it->second;
        }

        if (max_cluster_size < cluster_functions.size()) {
            for (std::vector<int>::iterator it = cluster_functions.begin(); it != cluster_functions.end(); ++it) {
                for (std::vector<int>::iterator it2 = it; it2 != cluster_functions.end(); ++it2) {
                    insert_stmt->bind(0, *it);
                    insert_stmt->bind(1, *it2);
                    insert_stmt->bind(2, cluster_functions.size());
                    insert_stmt->execute();
                }
	    }
        }
    }

    transaction->execute("drop table IF EXISTS size_distribution");
    transaction->execute("create table size_distribution(cluster_size integer, num_clusters integer);");

    insert_stmt = transaction->statement("insert into size_distribution"
                                         // 0        1         2
                                         "(cluster_size, num_clusters)"
                                         " values (?, ?)");

    for (std::map<int, int>::iterator it = size_distribution.begin(); it != size_distribution.end(); ++it) {
        insert_stmt->bind(0, it->first);
        insert_stmt->bind(1, it->second);
        insert_stmt->execute();
    }
}

int main(int argc, char *argv[])
{
    std::string prefix = "as_";
    double sem_threshold  = 0.7;
    double path_threshold = 0.0;
    double cg_threshold   = 0.0;
    int hamming_threshold   = -1;
    int euclidean_threshold = -1;
    double bucket_size = 0.0250;
    double increment   = 0.0500;
    int min_insns = 100;
    int max_cluster_size = -1;
    bool compute_semantic_distribution = false;

    int argno = 1;
    for (/*void*/; argno<argc && '-'==argv[argno][0]; ++argno) {
        if (!strcmp(argv[argno], "--")) {
            ++argno;
            break;
        }else if (!strcmp(argv[argno], "--help") || !strcmp(argv[argno], "-h")) {
            usage(0);
        } else if (!strncmp(argv[argno], "--sem-threshold=",16)) {
            sem_threshold = boost::lexical_cast<double>(argv[argno]+16);
        } else if (!strncmp(argv[argno], "--path-threshold=",17)) {
            path_threshold = boost::lexical_cast<double>(argv[argno]+17);
        } else if (!strncmp(argv[argno], "--cg-threshold=",15)) {
            cg_threshold = boost::lexical_cast<double>(argv[argno]+15);
        } else if (!strncmp(argv[argno], "--semantic-distribution",23)) {
            compute_semantic_distribution = true;
        } else if (!strncmp(argv[argno], "--min-insns=",12)) {
            min_insns= boost::lexical_cast<int>(argv[argno]+12);
        } else if (!strncmp(argv[argno], "--hamming-threshold=",20)) {
            hamming_threshold= boost::lexical_cast<int>(argv[argno]+20);
        } else if (!strncmp(argv[argno], "--euclidean-threshold=",22)) {
            euclidean_threshold= boost::lexical_cast<int>(argv[argno]+22);
        } else if (!strncmp(argv[argno], "--max-cluster-size=",19)) {
            max_cluster_size = boost::lexical_cast<int>(argv[argno]+19);
        } else {
            std::cerr <<argv0 <<": unknown switch: " <<argv[argno] <<"\n"
                      <<argv0 <<": see --help for more info\n";
            exit(1);
        }
    }

    if (argno+1!=argc)
        usage(1);

    SqlDatabase::ConnectionPtr conn = SqlDatabase::Connection::create(argv[argno++]);
    SqlDatabase::TransactionPtr transaction = conn->transaction();

    transaction->execute("SET client_min_messages TO WARNING;");

    //create the schema
    std::string set_thresholds =
        "drop table IF EXISTS fr_settings; "
        "create table fr_settings as"
        " select "
        "   (select " + boost::lexical_cast<std::string>(max_cluster_size)     +  ") as max_cluster_size,"
        "   (select " + boost::lexical_cast<std::string>(min_insns)     +  ") as min_insns,"
        "   (select " + boost::lexical_cast<std::string>(hamming_threshold) +    ") as hamming_threshold,"
        "   (select " + boost::lexical_cast<std::string>(euclidean_threshold) +  ") as euclidean_threshold,"
        "   (select " + boost::lexical_cast<std::string>(sem_threshold) +  ") as similarity_threshold,"
        "   (select " + boost::lexical_cast<std::string>(path_threshold) + ") as path_similarity_threshold,"
        "   (select " + boost::lexical_cast<std::string>(cg_threshold) +   ") as cg_similarity_threshold;";
    transaction->execute(set_thresholds);

    //Functions that should be ignored when computing clone pairs
    transaction->execute("drop table IF EXISTS fr_ignored_function_pairs;");
    transaction->execute("create table fr_ignored_function_pairs(func1_id integer, func2_id integer,"
                         " from_cluster_of_size integer)");
    transaction->execute(FailureEvaluation::failure_schema); // could take a long time if the database is large

    if (max_cluster_size > 0) {
        find_clusters(max_cluster_size, transaction);
        transaction->execute(FailureEvaluation::failure_schema); // could take a long time if the database is large
    }

    if (compute_semantic_distribution) {
        compute_percent_similarity_statistics(bucket_size, increment, transaction);
        compute_mean_similarity_statistics(bucket_size,  increment, transaction);
    }
    transaction->commit();

    return 0;
}
