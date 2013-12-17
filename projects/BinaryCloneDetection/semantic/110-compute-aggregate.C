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
              <<"    --min-insns=0|..|MAX_INT\n"
              <<"            Minimum number of instructions in candidate functions.\n"
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
compute_percent_similarity_statistics( double bucket_size, double increment,  SqlDatabase::TransactionPtr transaction)
{

  int num_pairs = transaction->statement("select count(*) from semantic_funcsim")->execute_int();


  transaction->execute("drop table IF EXISTS fr_percent_similar");
  transaction->execute("create table fr_percent_similar(similarity_low double precision, similarity_middle double precision," 
      " similarity_high double precision, percent double precision, num_matches integer);");

  SqlDatabase::StatementPtr pecent_similar_stmt = transaction->statement("insert into fr_percent_similar"
      // 0        1         2           3          4
      "(similarity_low, similarity_middle, similarity_high, percent, num_matches) "
      " values (?, ?, ?, ?, ?)"
      );

  for(double cur_bucket = 0.0; cur_bucket <= 1.0+bucket_size ; cur_bucket+=increment )
  {

    int num_matches = transaction->statement("select count(*) from semantic_funcsim where "
        "     similarity >= " + boost::lexical_cast<std::string>(cur_bucket - bucket_size) +
        " AND similarity <  " + boost::lexical_cast<std::string>(cur_bucket + bucket_size) )->execute_int();

    pecent_similar_stmt->bind(0, cur_bucket - bucket_size < 0 ? 0 : cur_bucket - bucket_size );
    pecent_similar_stmt->bind(1, cur_bucket);
    pecent_similar_stmt->bind(2, cur_bucket + bucket_size >= 1.0 ? 1.0 : cur_bucket + bucket_size );
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
compute_mean_similarity_statistics( double bucket_size, double increment, SqlDatabase::TransactionPtr transaction)
{
  int num_pairs = transaction->statement("select count(*) from semantic_funcsim")->execute_int();


  transaction->execute("drop table IF EXISTS fr_mean_similarity;");
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
    mean_similar_stmt->bind(3, num_pairs > 0 ? ((double) num_matches*100.0)/num_pairs : 0 );

    mean_similar_stmt->execute();

  }

};





int main(int argc, char *argv[])
{


  std::string prefix = "as_";


  double sem_threshold  = 0.7;
  double path_threshold = 0.0;
  double cg_threshold   = 0.0;

  double bucket_size = 0.0250;
  double increment   = 0.0500;

  int min_insns = 0;

  bool compute_semantic_distribution = false;

  int argno = 1;
  for (/*void*/; argno<argc && '-'==argv[argno][0]; ++argno) {
    if (!strcmp(argv[argno], "--")){
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
    } else {
      std::cerr <<argv0 <<": unknown switch: " <<argv[argno] <<"\n"
        <<argv0 <<": see --help for more info\n";
      exit(1);
    }
  };


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
    "   (select " + boost::lexical_cast<std::string>(min_insns)     +  " ) as min_insns,"
    "   (select " + boost::lexical_cast<std::string>(sem_threshold) +  " ) as similarity_threshold,"
    "   (select " + boost::lexical_cast<std::string>(path_threshold) + " ) as path_similarity_threshold,"
    "   (select " + boost::lexical_cast<std::string>(cg_threshold) +   " ) as cg_similarity_threshold;";

  transaction->execute(set_thresholds);
  transaction->execute(FailureEvaluation::failure_schema); // could take a long time if the database is large

  if(compute_semantic_distribution){
    compute_percent_similarity_statistics(bucket_size, increment, transaction);
    compute_mean_similarity_statistics(bucket_size,  increment, transaction);
  }
  transaction->commit();


  return 0;

}
