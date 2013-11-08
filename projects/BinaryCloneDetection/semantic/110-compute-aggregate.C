#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/min.hpp>
#include <boost/accumulators/statistics/max.hpp>
#include <boost/accumulators/statistics/moment.hpp>
#include <boost/accumulators/statistics/variance.hpp>
#include <boost/lexical_cast.hpp>


#include "sage3basic.h"
#include "CloneDetectionLib.h"

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
              <<"    --cg-threshold=0.0|..|1.0\n"
              <<"            Call graph similarity threshold between 0 and 1.\n"
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

  SqlDatabase::StatementPtr stmt = transaction->statement( "select datname from pg_database where datname LIKE '" + prefix + "%'" );

  for (SqlDatabase::Statement::iterator row=stmt->begin(); row!=stmt->end(); ++row) {
    db_names.push_back(row.get<std::string>(0));
  }

  return db_names;

};

std::map<std::string, std::set<std::string> >
get_database_groups(std::string prefix)
{
  std::vector<std::string> dbs = get_database_names(prefix);

  //Group databases based upon prefix
  std::map<std::string, std::set<std::string> > db_groups;
  for(unsigned int i = 0; i < dbs.size(); ++i){
    std::string cur_db = dbs[i];

    std::string cur_db_prefix = cur_db.substr(0,cur_db.size()-5);
    
    std::map<std::string, std::set<std::string> >::iterator it=db_groups.find(cur_db_prefix);

    if( it == db_groups.end() ){

      std::set<std::string> new_set;
      new_set.insert(cur_db);

      db_groups[cur_db_prefix] = new_set;

    }else{
      it->second.insert(cur_db);
    }
  }

  return db_groups;
}

void
compute_percent_similarity_statistics( double bucket_size, double increment, std::string group, std::string name, SqlDatabase::TransactionPtr transaction, SqlDatabase::TransactionPtr r_transaction)
{

  int num_pairs = transaction->statement("select count(*) from semantic_funcsim")->execute_int();


  transaction->execute("drop table IF EXISTS fr_percent_similar");
  transaction->execute("create table fr_percent_similar(similarity_low double precision, similarity_middle double precision," 
      " similarity_high double precision, percent double precision);");

  SqlDatabase::StatementPtr pecent_similar_stmt = transaction->statement("insert into fr_percent_similar"
      // 0        1         2           3          4
      "(similarity_low, similarity_middle, similarity_high, percent) "
      " values (?, ?, ?, ?)"
      );

  for(double cur_bucket = 0.0; cur_bucket <= 1.0+bucket_size ; cur_bucket+=increment )
  {

    int num_matches = transaction->statement("select count(*) from semantic_funcsim where "
        "     similarity >= " + boost::lexical_cast<std::string>(cur_bucket - bucket_size) +
        " AND similarity <  " + boost::lexical_cast<std::string>(cur_bucket + bucket_size) )->execute_int();

    pecent_similar_stmt->bind(0, cur_bucket - bucket_size < 0 ? 0 : cur_bucket - bucket_size );
    pecent_similar_stmt->bind(1, cur_bucket);
    pecent_similar_stmt->bind(2, cur_bucket + bucket_size >= 1.0 ? 1.0 : cur_bucket + bucket_size );
    pecent_similar_stmt->bind(3, ((double) num_matches*100.0)/num_pairs);

    pecent_similar_stmt->execute();

 
    //insert into global db
    SqlDatabase::StatementPtr global_percent_similar_stmt = r_transaction->statement("insert into fr_percent_similar"
        // 0        1         2           3          4
        "(group_name, name, similarity_low, similarity_middle, similarity_high, percent) "
        " values (?, ?, ?, ?, ?, ?)"
        );

    global_percent_similar_stmt->bind(0, group);
    global_percent_similar_stmt->bind(1, name);
    global_percent_similar_stmt->bind(2, cur_bucket - bucket_size < 0 ? 0 : cur_bucket - bucket_size );
    global_percent_similar_stmt->bind(3, cur_bucket);
    global_percent_similar_stmt->bind(4, cur_bucket + bucket_size >= 1.0 ? 1.0 : cur_bucket + bucket_size);
    global_percent_similar_stmt->bind(5, ((double) num_matches*100.0)/num_pairs);

    global_percent_similar_stmt->execute();

  }
}

void
compute_mean_similarity_statistics( double bucket_size, double increment, std::string group, std::string name, SqlDatabase::TransactionPtr transaction, SqlDatabase::TransactionPtr r_transaction)
{
  int num_pairs = transaction->statement("select count(*) from semantic_funcsim")->execute_int();


  transaction->execute("create table fr_mean_similarity as  select coalesce(sum(sf.similarity)/"+
      boost::lexical_cast<std::string>(num_pairs)+     
      " ,0) as similarity,  ttf.id as func_id from semantic_funcsim as sf "+ 
      " join semantic_functions as ttf on ttf.id = sf.func1_id  OR ttf.id = sf.func2_id GROUP BY ttf.id;"
      );


  transaction->execute("drop table IF EXISTS fr_mean_similar");
  transaction->execute("create table fr_mean_similar(similarity_low double precision, similarity_middle double precision," 
      " similarity_high double precision, percent double precision);");

  SqlDatabase::StatementPtr mean_similar_stmt = transaction->statement("insert into fr_mean_similar"
      // 0        1         2           3          4
      "(similarity_low, similarity_middle, similarity_high, percent) "
      " values (?, ?, ?, ?)"
      );

  for(double cur_bucket = 0.0; cur_bucket <= 1.0+bucket_size ; cur_bucket+=increment )
  {

    int num_matches = transaction->statement("select count(*) from fr_mean_similarity where "
        "     similarity >= " + boost::lexical_cast<std::string>(cur_bucket - bucket_size) +
        " AND similarity <  " + boost::lexical_cast<std::string>(cur_bucket + bucket_size) )->execute_int();

    mean_similar_stmt->bind(0, cur_bucket - bucket_size < 0 ? 0 : cur_bucket - bucket_size );
    mean_similar_stmt->bind(1, cur_bucket);
    mean_similar_stmt->bind(2, cur_bucket + bucket_size >= 1.0 ? 1.0 : cur_bucket + bucket_size );
    mean_similar_stmt->bind(3, ((double) num_matches*100.0)/num_pairs);

    mean_similar_stmt->execute();


    //insert into global db
    SqlDatabase::StatementPtr global_mean_similar_stmt = r_transaction->statement("insert into fr_mean_similar"
        // 0        1         2           3          4
        "(group_name, name, similarity_low, similarity_middle, similarity_high, percent) "
        " values (?, ?, ?, ?, ?, ?)"
        );

    global_mean_similar_stmt->bind(0, group);
    global_mean_similar_stmt->bind(1, name);
    global_mean_similar_stmt->bind(2, cur_bucket - bucket_size < 0 ? 0 : cur_bucket - bucket_size );
    global_mean_similar_stmt->bind(3, cur_bucket);
    global_mean_similar_stmt->bind(4, cur_bucket + bucket_size >= 1.0 ? 1.0 : cur_bucket + bucket_size);
    global_mean_similar_stmt->bind(5, ((double) num_matches*100.0)/num_pairs);

    global_mean_similar_stmt->execute();
  
  }

};

void
compute_aggregate_statistics(double bucket_size, double increment, SqlDatabase::TransactionPtr r_transaction)
{

  //compute percent similar
  r_transaction->execute("drop table IF EXISTS aggregate_percent;");
  r_transaction->execute("create table aggregate_percent( similarity_low double precision,"
      " similarity_middle double precision, similarity_high double precision, " 
      " mean double precision, minimum double precision, maximum double precision, variance double precision);");

  SqlDatabase::StatementPtr percent_similar_stmt = r_transaction->statement("insert into aggregate_percent"
      // 0        1         2           3          4
      "(similarity_low, similarity_middle, similarity_high, mean, minimum, maximum, variance) "
      " values (?, ?, ?, ?, ?, ?, ?)"
      );


  for(double cur_bucket = 0.0; cur_bucket <= 1.0+bucket_size ; cur_bucket+=increment )
  {
    SqlDatabase::StatementPtr stmt = r_transaction->statement( "select percent from fr_percent_similar where "
        " similarity_middle < " + boost::lexical_cast<std::string>(cur_bucket + bucket_size) +
        " AND similarity_middle > " + boost::lexical_cast<std::string>(cur_bucket - bucket_size) );

    accumulator_set<double, features< tag::min, tag::max, tag::mean, tag::variance > > percents;

    for (SqlDatabase::Statement::iterator row=stmt->begin(); row!=stmt->end(); ++row) {
      percents(row.get<double>(0));
    }

    percent_similar_stmt->bind(0, cur_bucket - bucket_size < 0 ? 0 : cur_bucket - bucket_size);
    percent_similar_stmt->bind(1, cur_bucket);
    percent_similar_stmt->bind(2, cur_bucket + bucket_size >= 1.0 ? 1.0 : cur_bucket + bucket_size );
    percent_similar_stmt->bind(3, mean(percents));
    percent_similar_stmt->bind(4, min(percents));
    percent_similar_stmt->bind(5, max(percents));
    percent_similar_stmt->bind(6, sqrt(boost::accumulators::variance(percents)));

    percent_similar_stmt->execute();
  }

  //compute mean similar
  r_transaction->execute("drop table IF EXISTS aggregate_mean;");
  r_transaction->execute("create table aggregate_mean( similarity_low double precision,"
      " similarity_middle double precision, similarity_high double precision, " 
      " mean double precision, minimum double precision, maximum double precision, variance double precision);");

  SqlDatabase::StatementPtr mean_similar_stmt = r_transaction->statement("insert into aggregate_mean"
      // 0        1         2           3          4
      "(similarity_low, similarity_middle, similarity_high, mean, minimum, maximum, variance) "
      " values (?, ?, ?, ?, ?, ?, ?)"
      );

  for(double cur_bucket = 0.0; cur_bucket <= 1.0+bucket_size ; cur_bucket+=increment )
  {
    SqlDatabase::StatementPtr stmt = r_transaction->statement( "select percent from fr_mean_similar where "
        " similarity_middle < " + boost::lexical_cast<std::string>(cur_bucket + bucket_size) +
        " AND similarity_middle > " + boost::lexical_cast<std::string>(cur_bucket - bucket_size) );

    accumulator_set<double, features< tag::min, tag::max, tag::mean, tag::variance > > percents;

    for (SqlDatabase::Statement::iterator row=stmt->begin(); row!=stmt->end(); ++row) {
      percents(row.get<double>(0));
    }

    mean_similar_stmt->bind(0, cur_bucket - bucket_size < 0 ? 0 : cur_bucket - bucket_size);
    mean_similar_stmt->bind(1, cur_bucket);
    mean_similar_stmt->bind(2, cur_bucket + bucket_size >= 1.0 ? 1.0 : cur_bucket + bucket_size );
    mean_similar_stmt->bind(3, mean(percents));
    mean_similar_stmt->bind(4, min(percents));
    mean_similar_stmt->bind(5, max(percents));
    mean_similar_stmt->bind(6, sqrt(boost::accumulators::variance(percents)));


    mean_similar_stmt->execute();
  }

}


int main(int argc, char *argv[])
{


  std::string prefix = "as_";


  double sem_threshold  = 0.7;
  double path_threshold = 0.5;
  double cg_threshold   = 0.5;

  double bucket_size = 0.0250;
  double increment   = 0.0500;

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
    } else {
      std::cerr <<argv0 <<": unknown switch: " <<argv[argno] <<"\n"
        <<argv0 <<": see --help for more info\n";
      exit(1);
    }
  };
 
  std::cout << argno << " " << argc << std::endl;
  if (  argno!=argc)
    usage(1);


  std::string results_db_name = "results_db_" + prefix;


  SqlDatabase::ConnectionPtr  results_db = SqlDatabase::Connection::create("postgresql:///" + results_db_name);
  SqlDatabase::TransactionPtr r_transaction = results_db->transaction();

  r_transaction->execute("drop table IF EXISTS specimen_results");
  r_transaction->execute("create table specimen_results( name text, " 
      " recall_min double precision, recall_max double precision, " 
      " recall_mean double precision, recall_variance double precision, " 
      " specificity_min double precision, specificity_max double precision, "
      " specificity_mean double precision, specificity_variance double precision, "
      " precision_min double precision, precision_max double precision, "
      " precision_mean double precision, precision_variance double precision "
      " );");


  SqlDatabase::StatementPtr insert_stmt = r_transaction->statement("insert into specimen_results"
      // 0        1         2           3          4
      "(name, recall_min, recall_max, recall_mean, recall_variance, "
      "specificity_min, specificity_max, specificity_mean, specificity_variance, "
      "precision_min, precision_max, precision_mean, precision_variance)"
      " values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");

  r_transaction->execute("drop table IF EXISTS per_specimen_results");
  r_transaction->execute("create table per_specimen_results( db_group text, name text, " 
      " precision double precision, specificity double precision, " 
      " recall double precision" 
      " );");


  SqlDatabase::StatementPtr per_insert_stmt = r_transaction->statement("insert into per_specimen_results"
      // 0        1         2           3          4
      "(db_group, name,  precision, specificity, recall ) "
      " values (?, ?, ?, ?, ?)");


  r_transaction->execute("drop table IF EXISTS fr_percent_similar; drop table IF EXISTS fr_mean_similar;");
  r_transaction->execute("create table fr_percent_similar(group_name text, name text, similarity_low double precision, similarity_middle double precision," 
      " similarity_high double precision, percent double precision);");
  r_transaction->execute("create table fr_mean_similar(group_name text, name text, similarity_low double precision, similarity_middle double precision," 
      " similarity_high double precision, percent double precision);");


  std::map<std::string, std::set<std::string> >  db_groups = get_database_groups(prefix);
 
  //Iterate over each group and compute mean and variance of recall, specificity and precision

  for(std::map<std::string, std::set<std::string> >::iterator it = db_groups.begin(); it != db_groups.end(); ++it)
  {

    accumulator_set<double, features< tag::min, tag::max, tag::mean, tag::variance > > recalls;
    accumulator_set<double, features< tag::min, tag::max, tag::mean, tag::variance > > specificity;
    accumulator_set<double, features< tag::min, tag::max, tag::mean, tag::variance > > precision;

    for(std::set<std::string >::iterator m_it = it->second.begin(); m_it != it->second.end(); ++m_it)
    {

      SqlDatabase::ConnectionPtr conn = SqlDatabase::Connection::create("postgresql:///"+*m_it);
      SqlDatabase::TransactionPtr transaction = conn->transaction();

      transaction->execute("SET client_min_messages TO WARNING;");
      //create the schema

      std::string set_thresholds =
        "drop table IF EXISTS fr_settings; "
        "create table fr_settings as"
        " select "
        "   (select " + boost::lexical_cast<std::string>(sem_threshold) +  " ) as similarity_threshold,"
        "   (select " + boost::lexical_cast<std::string>(path_threshold) + " ) as path_similarity_threshold,"
        "   (select " + boost::lexical_cast<std::string>(cg_threshold) +   " ) as cg_similarity_threshold;";

      transaction->execute(set_thresholds);
      transaction->execute(FailureEvaluation::failure_schema); // could take a long time if the database is large

      SqlDatabase::StatementPtr stmt = transaction->statement( "select recall, specificity, precision from fr_results_precision_recall" );

      for (SqlDatabase::Statement::iterator row=stmt->begin(); row!=stmt->end(); ++row) {
        double cur_recall   = row.get<double>(0);
        double cur_specificity = row.get<double>(1);
        double cur_precision   = row.get<double>(2);
        recalls(cur_recall);
        specificity(cur_specificity);
        precision(cur_precision);
  
	per_insert_stmt->bind(0, it->first);
	per_insert_stmt->bind(1, *m_it); 
	per_insert_stmt->bind(2, cur_recall);  
	per_insert_stmt->bind(3, cur_specificity);  
	per_insert_stmt->bind(4, cur_precision);  

        per_insert_stmt->execute();

      }

      compute_percent_similarity_statistics(bucket_size, increment, it->first, *m_it, transaction, r_transaction);
      compute_mean_similarity_statistics(bucket_size,  increment, it->first, *m_it, transaction, r_transaction);

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
 
    insert_stmt->execute();


    compute_aggregate_statistics(bucket_size, increment, r_transaction);


    std::cout << "\n\n Current db group: " << it->first << std::endl;
    std::cout << "\n\n    recall is      " << mean(recalls)     << "+-" << sqrt(boost::accumulators::variance(recalls))     << " min " << min(recalls)     << " max " << max(recalls) ;
    std::cout << "\n\n    specificity is " << mean(specificity) << "+-" << sqrt(boost::accumulators::variance(specificity)) << " min " << min(specificity) << " max " << max(specificity) ;
    std::cout << "\n\n    precision is   " << mean(precision)   << "+-" << sqrt(boost::accumulators::variance(precision))   << " min " << min(precision)   << " max " << max(precision) ;
    std::cout << "\n\n";
  }

  r_transaction->commit();

  return 0;
}
