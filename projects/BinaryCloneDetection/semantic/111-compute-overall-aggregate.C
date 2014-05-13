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


#include "sage3basic.h"
#include "CloneDetectionLib.h"


using namespace CloneDetection;
using namespace boost::accumulators;

std::string argv0;

double
compute_fscore(double precision, double recall)
{
    if (precision+recall > 0) {
        return 2*((precision*recall) / (precision+recall));
    } else {
        return 0;
    }
}

static void
usage(int exit_status)
{
    std::cerr <<"usage: " <<argv0 <<" [SWITCHES] \n"
              <<"This command computes the API Call similarity.\n"
              <<"\n"
              <<"  These switches control how api call traces are compared:\n"
              <<"    --semantic-distribution\n"
              <<"            Compute mean and percent distribution of semantic\n"
              <<"            similarity over all function pairs\n"
              <<"    --prefix=STRING\n"
              <<"            Prefix for databases.\n"
              <<"\n";
    exit(exit_status);
}

std::vector<std::string>
get_database_names(std::string prefix)
{
    std::vector<std::string> db_names;
    SqlDatabase::ConnectionPtr conn = SqlDatabase::Connection::create("postgresql:///template1");
    SqlDatabase::TransactionPtr transaction = conn->transaction();
    SqlDatabase::StatementPtr stmt = transaction->statement("select datname from pg_database where datname like"
                                                            " '" + prefix + "%'");
    for (SqlDatabase::Statement::iterator row=stmt->begin(); row!=stmt->end(); ++row)
        db_names.push_back(row.get<std::string>(0));
    return db_names;
}

/** Get all database groups based on the prefix. The prefix is everything but the last 5 characters. */
std::map<std::string, std::set<std::string> >
get_database_groups(std::string prefix)
{
    std::vector<std::string> dbs = get_database_names(prefix);

    //Group databases based upon prefix
    std::map<std::string, std::set<std::string> > db_groups;
    for (unsigned int i = 0; i < dbs.size(); ++i) {
        std::string cur_db = dbs[i];
        std::string cur_db_prefix = cur_db.substr(0,cur_db.size()-5);
        std::map<std::string, std::set<std::string> >::iterator it=db_groups.find(cur_db_prefix);

        if (it == db_groups.end()) {
            std::set<std::string> new_set;
            new_set.insert(cur_db);
            db_groups[cur_db_prefix] = new_set;
        } else {
            it->second.insert(cur_db);
        }
    }
    return db_groups;
}

/** Compute how many functions pairs are x%- bucket_size < x% <= x%+bucket size similar to each other.
 *
 * The goal is to show that the semantic clone detection is a binary function.
 *
 * The result is inserted into fr_percent_similar on the test db, and fr_percent_similar on
 * the global db. */
void
compute_percent_similarity_statistics(double bucket_size, double increment, std::string group, std::string name,
                                      SqlDatabase::TransactionPtr transaction, SqlDatabase::TransactionPtr r_transaction)
{
    SqlDatabase::StatementPtr global_percent_similar_stmt = r_transaction->statement("insert into fr_percent_similar"
                                                                                     "(group_name, name, similarity_low,"
                                                                                     " similarity_middle, similarity_high,"
                                                                                     " percent, num_matches)"
                                                                                     " values (?, ?, ?, ?, ?, ?, ?)");
    SqlDatabase::StatementPtr stmt = transaction->statement("select similarity_low, similarity_middle, similarity_high,"
                                                            " percent, num_matches from fr_percent_similar");
    for (SqlDatabase::Statement::iterator row=stmt->begin(); row!=stmt->end(); ++row) {
        double similarity_low    = row.get<double>(0);
        double similarity_middle = row.get<double>(1);
        double similarity_high   = row.get<double>(2);
        double percent           = row.get<double>(3);
        int    num_matches       = row.get<int>(4);

        //insert into global db
        global_percent_similar_stmt->bind(0, group);
        global_percent_similar_stmt->bind(1, name);
        global_percent_similar_stmt->bind(2, similarity_low);
        global_percent_similar_stmt->bind(3, similarity_middle);
        global_percent_similar_stmt->bind(4, similarity_high);
        global_percent_similar_stmt->bind(5, percent);
        global_percent_similar_stmt->bind(6, num_matches);
        global_percent_similar_stmt->execute();
    }
}

/** Compute how mean similar functions are to all other functions.
 *
 * The result is inserted into fr_mean_similarity on the test db, and fr_mean_similar on
 * the global db. */
void
compute_mean_similarity_statistics(double bucket_size, double increment, std::string group, std::string name,
                                   SqlDatabase::TransactionPtr transaction, SqlDatabase::TransactionPtr r_transaction)
{
    SqlDatabase::StatementPtr global_mean_similar_stmt = r_transaction->statement("insert into fr_mean_similar"
                                                                                  "(group_name, name, similarity_low,"
                                                                                  " similarity_middle, similarity_high, percent)"
                                                                                  " values (?, ?, ?, ?, ?, ?)");
    SqlDatabase::StatementPtr stmt = transaction->statement("select similarity_low, similarity_middle, similarity_high,"
                                                            "percent from fr_percent_similar");
    for (SqlDatabase::Statement::iterator row=stmt->begin(); row!=stmt->end(); ++row) {
        double similarity_low    = row.get<double>(0);
        double similarity_middle = row.get<double>(1);
        double similarity_high   = row.get<double>(2);
        double percent           = row.get<double>(3);

        //insert into global db
        global_mean_similar_stmt->bind(0, group);
        global_mean_similar_stmt->bind(1, name);
        global_mean_similar_stmt->bind(2, similarity_low);
        global_mean_similar_stmt->bind(3, similarity_middle);
        global_mean_similar_stmt->bind(4, similarity_high);
        global_mean_similar_stmt->bind(5, percent);
        global_mean_similar_stmt->execute();
    }
}

/** Compute two metrics
 *  @li how many functions pairs are X%  similar
 *  @li how many functions are mean X% similar to each other */
void
compute_aggregate_statistics(double bucket_size, double increment, SqlDatabase::TransactionPtr r_transaction)
{
    //compute percent similar
    r_transaction->execute("drop table IF EXISTS aggregate_percent;");
    r_transaction->execute("create table aggregate_percent( similarity_low double precision,"
                           " similarity_middle double precision, similarity_high double precision,"
                           " percent double precision, num_matches integer);");

    SqlDatabase::StatementPtr percent_similar_stmt = r_transaction->statement("insert into aggregate_percent"
                                                                              "(similarity_low, similarity_middle,"
                                                                              " similarity_high, percent, num_matches)"
                                                                              " values (?, ?, ?, ?, ?)");
    int num_pairs = r_transaction->statement("select sum(num_matches) from fr_percent_similar")->execute_int();

    for (double cur_bucket = 0.0; cur_bucket <= 1.0+bucket_size; cur_bucket+=increment) {
        int num_matches = r_transaction->statement("select sum(num_matches) from fr_percent_similar where"
                                                   " similarity_middle < " +
                                                   boost::lexical_cast<std::string>(cur_bucket + bucket_size) +
                                                   " AND similarity_middle >= " +
                                                   boost::lexical_cast<std::string>(cur_bucket - bucket_size))->execute_int();

        percent_similar_stmt->bind(0, cur_bucket - bucket_size < 0 ? 0 : cur_bucket - bucket_size);
        percent_similar_stmt->bind(1, cur_bucket);
        percent_similar_stmt->bind(2, cur_bucket + bucket_size >= 1.0 ? 1.0 : cur_bucket + bucket_size);
        percent_similar_stmt->bind(3, num_pairs > 0 ? ((double) num_matches * 100.0)/num_pairs : 0);
        percent_similar_stmt->bind(4, num_matches);

        percent_similar_stmt->execute();
    }

    //compute mean similar
    r_transaction->execute("drop table IF EXISTS aggregate_mean;");
    r_transaction->execute("create table aggregate_mean( similarity_low double precision,"
                           " similarity_middle double precision, similarity_high double precision,"
                           " mean double precision, median double precision, minimum double precision, maximum double precision,"
                           " variance double precision);");
    SqlDatabase::StatementPtr mean_similar_stmt = r_transaction->statement("insert into aggregate_mean"
                                                                           "(similarity_low, similarity_middle, similarity_high,"
                                                                           " mean, median, minimum, maximum, variance)"
                                                                           " values (?, ?, ?, ?, ?, ?, ?, ?)");
    for (double cur_bucket = 0.0; cur_bucket <= 1.0+bucket_size; cur_bucket+=increment) {
        SqlDatabase::StatementPtr stmt = r_transaction->statement("select percent from fr_mean_similar where"
                                                                  " similarity_middle < " +
                                                                  boost::lexical_cast<std::string>(cur_bucket + bucket_size) +
                                                                  " and similarity_middle >= " +
                                                                  boost::lexical_cast<std::string>(cur_bucket - bucket_size));
        accumulator_set<double, stats< tag::min, tag::max, tag::mean, tag::variance > > percents;
        for (SqlDatabase::Statement::iterator row=stmt->begin(); row!=stmt->end(); ++row)
            percents(row.get<double>(0));

        mean_similar_stmt->bind(0, cur_bucket - bucket_size < 0 ? 0 : cur_bucket - bucket_size);
        mean_similar_stmt->bind(1, cur_bucket);
        mean_similar_stmt->bind(2, cur_bucket + bucket_size >= 1.0 ? 1.0 : cur_bucket + bucket_size);
        mean_similar_stmt->bind(3, mean(percents));
        mean_similar_stmt->bind(4, 0);
        mean_similar_stmt->bind(5, min(percents));
        mean_similar_stmt->bind(6, max(percents));
        mean_similar_stmt->bind(7, sqrt(boost::accumulators::variance(percents)));

        mean_similar_stmt->execute();
    }
}

/** Get the failure rate of a database for a specific group (e.g splash O0 vs O1) */
void
get_failure_rates_insert_into_global(std::string db_name, SqlDatabase::TransactionPtr transaction,
                                     SqlDatabase::TransactionPtr r_transaction)
{
    std::string db_group = db_name.substr(db_name.size()-6, db_name.size());

    SqlDatabase::StatementPtr insert_stmt = r_transaction->statement("insert into analysis_results"
                                                                     " (name, db_group, true_positives, true_negatives,"
                                                                     " false_positives, false_negatives)"
                                                                     " values (?,?,?,?,?,?) ");
    SqlDatabase::StatementPtr stmt = transaction->statement("select true_positives, true_negatives, false_positives,"
                                                            " false_negatives from fr_results limit 1");

    //this loop will only execute once
    for (SqlDatabase::Statement::iterator row=stmt->begin(); row!=stmt->end(); ++row) {
        int true_positives  = row.get<int>(0);
        int true_negatives  = row.get<int>(1);
        int false_positives = row.get<int>(2);
        int false_negatives = row.get<int>(3);

        insert_stmt->bind(0, db_name);
        insert_stmt->bind(1, db_group);
        insert_stmt->bind(2, true_positives);
        insert_stmt->bind(3, true_negatives);
        insert_stmt->bind(4, false_positives);
        insert_stmt->bind(5, false_negatives);
        insert_stmt->execute();
    }
}

/** Classifies a database using the last 5 characters of the name, and computes the difference in precision, recall and
 *  specificity actross the group.
 *
 * The result is output in resilience_to_optimization per group (e.g 'O0_O1' which is O0 vs O1).
 *
 * And for mean, min, max and standard deviation of precision, recall, and specificity across
 * all row in resilience_to_optimization into resilience_to_optimization_rate. */
void
compute_resilience_to_optimization(SqlDatabase::TransactionPtr r_transaction)
{
    r_transaction->execute("drop table IF EXISTS resilience_to_optimization");
    r_transaction->execute("create table resilience_to_optimization(db_group text, recall double precision,"
                           " specificity double precision, precision double precision, fscore double precision)");

    SqlDatabase::StatementPtr insert_stmt = r_transaction->statement("insert into resilience_to_optimization"
                                                                     " (db_group, recall, specificity, precision, fscore)"
                                                                     " values (?, ?, ?, ?, ?) ");

    accumulator_set<double, stats< tag::min, tag::max, tag::mean, tag::variance > > recalls;
    accumulator_set<double, stats< tag::min, tag::max, tag::mean, tag::variance > > specificity;
    accumulator_set<double, stats< tag::min, tag::max, tag::mean, tag::variance > > precision;
    accumulator_set<double, stats< tag::min, tag::max, tag::mean, tag::variance > > fscore;

    SqlDatabase::StatementPtr db_group_stmt = r_transaction->statement("select distinct db_group from analysis_results");

    for (SqlDatabase::Statement::iterator group_row=db_group_stmt->begin(); group_row!=db_group_stmt->end(); ++group_row) {
        std::string db_group = group_row.get<std::string>(0);
        SqlDatabase::StatementPtr stmt = r_transaction->statement("select sum(true_positives) as true_positives,"
                                                                  " sum(true_negatives) as true_negative,"
                                                                  " sum(false_positives) as false_positives,"
                                                                  " sum(false_negatives) as false_negatives"
                                                                  " from analysis_results where db_group=?");
        stmt->bind(0, db_group);

        //this loop will only execute once
        for (SqlDatabase::Statement::iterator row=stmt->begin(); row!=stmt->end(); ++row) {
            int true_positives  = row.get<int>(0);
            int true_negatives  = row.get<int>(1);
            int false_positives = row.get<int>(2);
            int false_negatives = row.get<int>(3);

            double cur_recall      = (100.0*true_positives) / (0.001 + true_positives + false_negatives);
            double cur_specificity = (100.0*true_negatives) / (0.001 + true_negatives + false_positives);
            double cur_precision   = (100.0*true_positives) / (0.001 + true_positives + false_positives);
            double cur_fscore      = compute_fscore(cur_recall, cur_precision);

            recalls(cur_recall);
            specificity(cur_specificity);
            precision(cur_precision);
            fscore(cur_fscore);

            insert_stmt->bind(0, db_group);
            insert_stmt->bind(1, cur_recall);
            insert_stmt->bind(2, cur_specificity);
            insert_stmt->bind(3, cur_precision);
            insert_stmt->bind(4, compute_fscore(cur_recall, cur_precision));

            insert_stmt->execute();
        }
    }

    r_transaction->execute("drop table IF EXISTS resilience_to_optimization_rate");
    r_transaction->execute("create table resilience_to_optimization_rate("
                           " recall_min double precision, recall_max double precision,"
                           " recall_mean double precision, recall_standard_deviation double precision,"
                           " specificity_min double precision, specificity_max double precision,"
                           " specificity_mean double precision, specificity_standard_deviation double precision,"
                           " precision_min double precision, precision_max double precision,"
                           " precision_mean double precision, precision_standard_deviation double precision,"
                           " fscore_mean double precision, fscore_min double precision, fscore_max double precision,"
                           " fscore_standard_deviation double precision)");

    SqlDatabase::StatementPtr insert_overall_rates_stmt = r_transaction->statement("insert into resilience_to_optimization_rate"
                                                                                   "(recall_min, recall_max, recall_mean,"
                                                                                   " recall_standard_deviation,"
                                                                                   " specificity_min, specificity_max,"
                                                                                   " specificity_mean,"
                                                                                   " specificity_standard_deviation,"
                                                                                   " precision_min, precision_max,"
                                                                                   " precision_mean,"
                                                                                   " precision_standard_deviation,"
                                                                                   " fscore_mean, fscore_min, fscore_max,"
                                                                                   " fscore_standard_deviation)"
                                                                                   " values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?,"
                                                                                   " ?, ?, ?, ?, ?)");
    insert_overall_rates_stmt->bind(0, min(recalls));
    insert_overall_rates_stmt->bind(1, max(recalls));
    insert_overall_rates_stmt->bind(2, mean(recalls));
    insert_overall_rates_stmt->bind(3, sqrt(boost::accumulators::variance(recalls)));
    insert_overall_rates_stmt->bind(4, min(specificity));
    insert_overall_rates_stmt->bind(5, max(specificity));
    insert_overall_rates_stmt->bind(6, mean(specificity));
    insert_overall_rates_stmt->bind(7, sqrt(boost::accumulators::variance(specificity)));
    insert_overall_rates_stmt->bind(8, min(precision));
    insert_overall_rates_stmt->bind(9, max(precision));
    insert_overall_rates_stmt->bind(10, mean(precision));
    insert_overall_rates_stmt->bind(11, sqrt(boost::accumulators::variance(precision)));
    insert_overall_rates_stmt->bind(12, mean(fscore));
    insert_overall_rates_stmt->bind(13, min(fscore));
    insert_overall_rates_stmt->bind(14, max(fscore));
    insert_overall_rates_stmt->bind(15, sqrt(boost::accumulators::variance(fscore)));

    insert_overall_rates_stmt->execute();

    std::cout << "\n\n Overall for all optimization levels for this db group: " << std::endl;
    std::cout << "\n\n    recall is      " << mean(recalls)
              << "+-" << sqrt(boost::accumulators::variance(recalls))
              << " min " << min(recalls)
              << " max " << max(recalls);
    std::cout << "\n\n    specificity is " << mean(specificity)
              << "+-" << sqrt(boost::accumulators::variance(specificity))
              << " min " << min(specificity)
              << " max " << max(specificity);
    std::cout << "\n\n    precision is   " << mean(precision)
              << "+-" << sqrt(boost::accumulators::variance(precision))
              << " min " << min(precision)
              << " max " << max(precision);
    std::cout << "\n\n    fscore is   " << mean(fscore)
              << "+-" << sqrt(boost::accumulators::variance(fscore))
              << " min " << min(fscore)
              << " max " << max(fscore);
    std::cout << "\n\n";
}

/** Compute precision and recall for all tests run.
 *
 *  Output is in overall_rates. */
void
compute_overall_rates(SqlDatabase::TransactionPtr r_transaction)
{
    r_transaction->execute("drop table IF EXISTS overall_rates");
    r_transaction->execute("create table overall_rates(recall double precision, specificity double precision,"
                           " precision double precision, fscore double precision)");
    SqlDatabase::StatementPtr insert_stmt = r_transaction->statement("insert into overall_rates"
                                                                     " (recall, specificity, precision, fscore)"
                                                                     " values (?,?,?,?) ");
    SqlDatabase::StatementPtr stmt = r_transaction->statement("select sum(true_positives) as true_positives,"
                                                              " sum(true_negatives) as true_negative,"
                                                              " sum(false_positives) as false_positives,"
                                                              " sum(false_negatives) as false_negatives"
                                                              " from analysis_results");

    //this loop will only execute once
    for (SqlDatabase::Statement::iterator row=stmt->begin(); row!=stmt->end(); ++row) {
        int true_positives  = row.get<int>(0);
        int true_negatives  = row.get<int>(1);
        int false_positives = row.get<int>(2);
        int false_negatives = row.get<int>(3);

        double recall      = (100.0*true_positives) / (0.001 + true_positives + false_negatives);
        double specificity = (100.0*true_negatives) / (0.001 + true_negatives + false_positives);
        double precision   = (100.0*true_positives) / (0.001 + true_positives + false_positives);

        insert_stmt->bind(0, recall);
        insert_stmt->bind(1, specificity);
        insert_stmt->bind(2, precision);
        insert_stmt->bind(3, compute_fscore(precision, recall));
        insert_stmt->execute();
    }
}


int main(int argc, char *argv[])
{
    std::string prefix = "as_";
    double sem_threshold  = 0.7;
    double path_threshold = 0.0;
    double cg_threshold   = 0.0;
    double bucket_size = 0.0250;
    double increment   = 0.0500;
    bool compute_semantic_distribution = false;

    int argno = 1;
    for (/*void*/; argno<argc && '-'==argv[argno][0]; ++argno) {
        if (!strcmp(argv[argno], "--help") || !strcmp(argv[argno], "-h")) {
            usage(0);
        } else if (!strncmp(argv[argno], "--sem-threshold=",16)) {
            sem_threshold = boost::lexical_cast<double>(argv[argno]+16);
        } else if (!strncmp(argv[argno], "--path-threshold=",17)) {
            path_threshold = boost::lexical_cast<double>(argv[argno]+17);
        } else if (!strncmp(argv[argno], "--cg-threshold=",15)) {
            cg_threshold = boost::lexical_cast<double>(argv[argno]+15);
        } else if (!strncmp(argv[argno], "--prefix=",9)) {
            prefix = boost::lexical_cast<std::string>(argv[argno]+9);
        } else if (!strncmp(argv[argno], "--semantic-distribution",23)) {
            compute_semantic_distribution = true;
        } else {
            std::cerr <<argv0 <<": unknown switch: " <<argv[argno] <<"\n"
                      <<argv0 <<": see --help for more info\n";
            exit(1);
        }
    }

    std::cout << argno << " " << argc << std::endl;
    if (argno!=argc)
        usage(1);

    std::string results_db_name = "results_db_" + prefix;
    SqlDatabase::ConnectionPtr  results_db = SqlDatabase::Connection::create("postgresql:///" + results_db_name);
    SqlDatabase::TransactionPtr r_transaction = results_db->transaction();

    r_transaction->execute("drop table IF EXISTS analysis_results");
    r_transaction->execute("create table analysis_results( name text, db_group text, "
                           " true_positives integer, true_negatives integer, false_positives integer, false_negatives integer)");

    r_transaction->execute("drop table IF EXISTS specimen_results");
    r_transaction->execute("create table specimen_results( name text, "
                           " recall_min double precision, recall_max double precision, "
                           " recall_mean double precision, recall_standard_deviation double precision, "
                           " specificity_min double precision, specificity_max double precision, "
                           " specificity_mean double precision, specificity_standard_deviation double precision, "
                           " precision_min double precision, precision_max double precision, "
                           " precision_mean double precision, precision_standard_deviation double precision, "
                           " fscore double precision)");

    SqlDatabase::StatementPtr insert_stmt = r_transaction->statement("insert into specimen_results"
                                                                     "(name, recall_min, recall_max, recall_mean,"
                                                                     " recall_standard_deviation, specificity_min,"
                                                                     " specificity_max, specificity_mean,"
                                                                     " specificity_standard_deviation, precision_min,"
                                                                     " precision_max, precision_mean,"
                                                                     " precision_standard_deviation, fscore)"
                                                                     " values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
    r_transaction->execute("drop table IF EXISTS per_specimen_results");
    r_transaction->execute("create table per_specimen_results( db_group text, name text, "
                           " precision double precision, specificity double precision, "
                           " recall double precision, fscore double precision)");

    SqlDatabase::StatementPtr per_insert_stmt = r_transaction->statement("insert into per_specimen_results"
                                                                         "(db_group, name,  precision, specificity, recall,"
                                                                         " fscore)"
                                                                         " values (?, ?, ?, ?, ?, ?)");

    r_transaction->execute("drop table IF EXISTS fr_percent_similar; drop table IF EXISTS fr_mean_similar;");
    r_transaction->execute("create table fr_percent_similar(group_name text, name text, similarity_low double precision,"
                           " num_matches integer, similarity_middle double precision,"
                           " similarity_high double precision, percent double precision)");
    r_transaction->execute("create table fr_mean_similar(group_name text, name text, similarity_low double precision,"
                           " similarity_middle double precision, similarity_high double precision, percent double precision)");

    //Aggregate of how many calls are made by a function at a specific level
    r_transaction->execute("drop table IF EXISTS dpath_ncalls;");
    r_transaction->execute("create table dpath_ncalls(db_name text, program_name text, ncalls integer,"
                           " num_output_call_vecs integer);");
    SqlDatabase::StatementPtr ncalls_stmt = r_transaction->statement("insert into dpath_ncalls"
                                                                     "(db_name, program_name, ncalls, num_output_call_vecs) "
                                                                     " values (?, ?, ?, ?)");

    r_transaction->execute("drop table IF EXISTS ougroup_nelem;");
    r_transaction->execute("create table ogroup_nelem(db_name text, program_name text, noutputs  integer, num_ogroups integer)");
    SqlDatabase::StatementPtr noutputs_stmt = r_transaction->statement("insert into ogroup_nelem"
                                                                       "(db_name, program_name, noutputs, num_ogroups)"
                                                                       " values (?, ?, ?, ?)");

    std::map<std::string, std::set<std::string> >  db_groups = get_database_groups(prefix);

    //Iterate over each group and compute mean and variance of recall, specificity and precision
    for (std::map<std::string, std::set<std::string> >::iterator it = db_groups.begin(); it != db_groups.end(); ++it) {
        accumulator_set<double, stats< tag::min, tag::max, tag::mean, tag::variance > > recalls;
        accumulator_set<double, stats< tag::min, tag::max, tag::mean, tag::variance > > specificity;
        accumulator_set<double, stats< tag::min, tag::max, tag::mean, tag::variance > > precision;

        for (std::set<std::string >::iterator m_it = it->second.begin(); m_it != it->second.end(); ++m_it) {
            SqlDatabase::ConnectionPtr conn = SqlDatabase::Connection::create("postgresql:///"+*m_it);
            SqlDatabase::TransactionPtr transaction = conn->transaction();

            //create the schema
            get_failure_rates_insert_into_global(*m_it, transaction, r_transaction);

            SqlDatabase::StatementPtr stmt = transaction->statement("select recall, specificity, precision"
                                                                    " from fr_results_precision_recall");
            for (SqlDatabase::Statement::iterator row=stmt->begin(); row!=stmt->end(); ++row) {
                double cur_recall   = row.get<double>(0);
                double cur_specificity = row.get<double>(1);
                double cur_precision   = row.get<double>(2);
                recalls(cur_recall);
                specificity(cur_specificity);
                precision(cur_precision);

                std::string full_db_name   = *m_it;
                std::string cur_db_prefix  = it->first;
                std::string cur_db_group = full_db_name.substr(full_db_name.size()-5, 5);

                per_insert_stmt->bind(0, cur_db_group);
                per_insert_stmt->bind(1, it->first);
                per_insert_stmt->bind(2, cur_precision);
                per_insert_stmt->bind(3, cur_specificity);
                per_insert_stmt->bind(4, cur_recall);
                per_insert_stmt->bind(5, compute_fscore(cur_precision, cur_recall));

                per_insert_stmt->execute();
            }

            if (compute_semantic_distribution == true) {
                compute_percent_similarity_statistics(bucket_size, increment, it->first, *m_it, transaction, r_transaction);
                compute_mean_similarity_statistics(bucket_size,  increment, it->first, *m_it, transaction, r_transaction);
            }

            SqlDatabase::StatementPtr count_dyncalls_stmt = transaction->statement("select ncalls, count(*) as"
                                                                                   " num_output_call_vecs from ("
                                                                                   " select sum(ncalls) as ncalls"
                                                                                   " from semantic_fio_calls "
                                                                                   " group by func_id, igroup_id"
                                                                                   " order by func_id)"
                                                                                   " as calls_by_function"
                                                                                   " group by ncalls order by ncalls");
            for (SqlDatabase::Statement::iterator row=count_dyncalls_stmt->begin(); row!=count_dyncalls_stmt->end(); ++row) {
                int ncalls = row.get<int>(0);
                int num_output_call_vecs = row.get<int>(1);

                std::string full_db_name     = *m_it;
                std::string cur_program_name = full_db_name.substr(0, full_db_name.size() - 5);

                ncalls_stmt->bind(0, full_db_name);
                ncalls_stmt->bind(1, cur_program_name);
                ncalls_stmt->bind(2,ncalls);
                ncalls_stmt->bind(3,num_output_call_vecs);
                ncalls_stmt->execute();
            }

            SqlDatabase::StatementPtr count_outputvalues_stmt = transaction->statement("select noutputs,"
                                                                                       " count(*) as num_ogroups"
                                                                                       " from (select count(*) as noutputs"
                                                                                       " from semantic_fio as fio"
                                                                                       " join semantic_outputvalues as out"
                                                                                       " on out.hashkey = fio.ogroup_id"
                                                                                       " group by fio.func_id, fio.igroup_id)"
                                                                                       " as aggregate_outputvalues"
                                                                                       " group by noutputs");
            for (SqlDatabase::Statement::iterator row=count_outputvalues_stmt->begin();
                 row!=count_outputvalues_stmt->end();
                 ++row) {
                int noutputs = row.get<int>(0);
                int num_ogroups = row.get<int>(1);

                std::string full_db_name     = *m_it;
                std::string cur_program_name = full_db_name.substr(0, full_db_name.size() - 5);

                noutputs_stmt->bind(0, full_db_name);
                noutputs_stmt->bind(1, cur_program_name);
                noutputs_stmt->bind(2, noutputs);
                noutputs_stmt->bind(3, num_ogroups);
                noutputs_stmt->execute();
            }
            transaction->commit();
        }

        insert_stmt->bind(0, it->first);
        insert_stmt->bind(1, min(recalls));
        insert_stmt->bind(2, max(recalls));
        insert_stmt->bind(3, mean(recalls));
        insert_stmt->bind(4, sqrt(boost::accumulators::variance(recalls)));
        insert_stmt->bind(5, min(specificity));
        insert_stmt->bind(6, max(specificity));
        insert_stmt->bind(7, mean(specificity));
        insert_stmt->bind(8, sqrt(boost::accumulators::variance(specificity)));
        insert_stmt->bind(9, min(precision));
        insert_stmt->bind(10, max(precision));
        insert_stmt->bind(11, mean(precision));
        insert_stmt->bind(12, sqrt(boost::accumulators::variance(precision)));
        insert_stmt->bind(13, compute_fscore(mean(precision), mean(recalls)));

        insert_stmt->execute();

        std::cout << "\n\n Current db group: " << it->first << std::endl;
        std::cout << "\n\n    recall is      " << mean(recalls)
                  << "+-" << sqrt(boost::accumulators::variance(recalls))
                  << " min " << min(recalls)
                  << " max " << max(recalls);
        std::cout << "\n\n    specificity is " << mean(specificity)
                  << "+-" << sqrt(boost::accumulators::variance(specificity))
                  << " min " << min(specificity)
                  << " max " << max(specificity);
        std::cout << "\n\n    precision is   " << mean(precision)
                  << "+-" << sqrt(boost::accumulators::variance(precision))
                  << " min " << min(precision)
                  << " max " << max(precision);
        std::cout << "\n\n";

        if (compute_semantic_distribution == true)
            compute_aggregate_statistics(bucket_size, increment, r_transaction);
    }

    compute_resilience_to_optimization(r_transaction);
    compute_overall_rates(r_transaction);

    r_transaction->commit();

    return 0;
}
