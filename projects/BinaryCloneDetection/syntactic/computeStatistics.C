#include <sys/time.h>
#include <sys/resource.h>

#include "sqlite3x.h"
#include <string>
#include <cstdio> // for std::remove
#include <iostream>
#include <cassert>
#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>
using namespace sqlite3x;
using namespace std;
using namespace boost::program_options;


struct Element
{

  std::string function_id;
std::string num_instructions_covered_clusters;
std::string fraction_instructions_covered_clusters;
std::string num_instructions_covered_postprocessed;
std::string fraction_instructions_covered_postprocessed;

};


void 
calculate_result_statistics(sqlite3_connection& con)
{

  std::cout << "Calculate false positive rate" << std::endl;

  int windowSize = 0;
  int stride = 0;
  try {
	windowSize = sqlite3_command(con, "select window_size from run_parameters limit 1").executeint();
  } catch (exception& ex) {cerr << "Exception Occurred: " << ex.what() << endl;}
  try {
	stride = sqlite3_command(con, "select stride from run_parameters limit 1").executeint();
  } catch (exception& ex) {cerr << "Exception Occurred: " << ex.what() << endl;}
  assert (windowSize != 0);
  assert (stride != 0);

  // Compute coverage statistics for each function
  cerr << "About to compute function coverage" << endl;

  try {con.executenonquery("delete from function_coverage");}
  catch(exception &ex) {cerr << "Exception Occurred: " << ex.what() << endl;}

  try {con.executenonquery("delete from total_coverage");}
  catch(exception &ex) {cerr << "Exception Occurred: " << ex.what() << endl;}


  int similarity =0;

  try {
    similarity = sqlite3x::sqlite3_command(con, "select similarity_threshold from detection_parameters limit 1").executeint();
  } catch (std::exception& ex) {std::cerr << "Exception Occurred: " << ex.what() << std::endl;}
  try {
    windowSize = sqlite3x::sqlite3_command(con, "select window_size from run_parameters limit 1").executeint();
  } catch (std::exception& ex) {std::cerr << "Exception Occurred: " << ex.what() << std::endl;}


  std::vector<Element> coverage;
  
  
  try {
    // FIXME: there seem to be several tuples with a single function_id -- this might change with the new vector generation run
    sqlite3_command select_function_stats(con, "select function_id, max(num_instructions) from function_statistics where num_instructions <> 0 group by function_id");
    sqlite3_reader r = select_function_stats.executereader();
    sqlite3_transaction trans(con);
    size_t count = 0;
    while (r.read()) {
      string function_id = r.getstring(0);
      ++count;
      if (count % 1000 == 0) {
        cerr << "function_id = " << function_id << endl;
      }
      double num_instructions_total = boost::lexical_cast<double>(r.getstring(1));
      size_t num_instructions_covered[2] = {0, 0};
      double fraction_instructions_covered[2] = {0, 0};
      string table_names[2] = {"clusters", "postprocessed_clusters"};
      for (size_t table_index = 0; table_index < 2; ++table_index) {
        try {
          sqlite3_command selectCmd(con, "select index_within_function from " + table_names[table_index] + " where function_id = ? order by index_within_function");
          selectCmd.bind(1, function_id);
          sqlite3_reader r2 = selectCmd.executereader();
          bool first = true;
          size_t last_index_within_function = 0;
          while (r2.read()) {
            size_t index_within_function = boost::lexical_cast<size_t>(r2.getstring(0));
            size_t non_overlapping_instructions =
              (first || index_within_function * stride >= last_index_within_function * stride + windowSize) ?
              windowSize :
              (index_within_function - last_index_within_function) * stride;
            first = false;
            last_index_within_function = index_within_function;
            num_instructions_covered[table_index] += non_overlapping_instructions;
          }
          fraction_instructions_covered[table_index] = (double)num_instructions_covered[table_index] / num_instructions_total;
        } catch (exception& ex) {cerr << "Exception: " << ex.what() << endl;}
      }

      Element el;
      el.function_id = function_id;
      el.num_instructions_covered_clusters = boost::lexical_cast<string>(num_instructions_covered[0]);
      el.fraction_instructions_covered_clusters = boost::lexical_cast<string>(fraction_instructions_covered[0]);
      el.num_instructions_covered_postprocessed = boost::lexical_cast<string>(num_instructions_covered[1]);
      el.fraction_instructions_covered_postprocessed = boost::lexical_cast<string>(fraction_instructions_covered[1]);
     
      coverage.push_back(el);
    }
  } catch (exception& ex) {cerr << "Exception outer: " << ex.what() << endl;}

  for(unsigned int i = 0 ; i < coverage.size(); i++ )
  {
    Element& el = coverage[i];
    if(el.num_instructions_covered_clusters == "0") continue;
    
      try {
        sqlite3_command insertCmd(con, "insert into function_coverage(function_id, num_instructions_covered_not_postprocessed, fraction_instructions_covered_not_postprocessed, num_instructions_covered_postprocessed, fraction_instructions_covered_postprocessed) values (?, ?, ?, ?, ?)");
        insertCmd.bind(1, el.function_id);
        insertCmd.bind(2, el.num_instructions_covered_clusters);
        insertCmd.bind(3, el.fraction_instructions_covered_clusters);
        insertCmd.bind(4, el.num_instructions_covered_postprocessed);
        insertCmd.bind(5, el.fraction_instructions_covered_postprocessed);
        insertCmd.executenonquery();
      } catch (exception& ex) {cerr << "Exception: " << ex.what() << endl;}


  }
 
  int num_instructions_covered_not_postprocessed =0; 

  try {
      num_instructions_covered_not_postprocessed = sqlite3x::sqlite3_command(con, 
          "select sum(num_instructions_covered_not_postprocessed) from function_coverage").executeint();
  }catch(exception &ex) {cerr << "Exception Occurred: " << ex.what() << endl;}

  int num_instructions_covered_postprocessed = 0;
   try {
      num_instructions_covered_postprocessed = sqlite3x::sqlite3_command(con, 
          "select sum(num_instructions_covered_postprocessed) from function_coverage").executeint();
  }catch(exception &ex) {cerr << "Exception Occurred: " << ex.what() << endl;}

   int num_instructions = 0;
   try {
      num_instructions = sqlite3x::sqlite3_command(con, 
          "select sum(num_instructions) from function_statistics").executeint();
  }catch(exception &ex) {cerr << "Exception Occurred: " << ex.what() << endl;}
   try {

     std::string db_insert_n =  "insert into total_coverage(num_instructions_covered_not_postprocessed, fraction_instructions_covered_not_postprocessed, num_instructions_covered_postprocessed, fraction_instructions_covered_postprocessed) VALUES(?,?,?,?)";
     sqlite3x::sqlite3_command cmd(con, db_insert_n.c_str());
     cmd.bind(1,num_instructions_covered_not_postprocessed);
     cmd.bind(2,boost::lexical_cast<string>(num_instructions_covered_not_postprocessed*1.0/num_instructions) );
     cmd.bind(3,num_instructions_covered_postprocessed);
     cmd.bind(4,boost::lexical_cast<string>(num_instructions_covered_postprocessed*1.0/num_instructions));
     cmd.executenonquery();
   }
   catch(exception &ex) {cerr << "Exception Occurred: " << ex.what() << endl;}


/*
   try {

     std::string db_insert_n =  "insert into total_coverage(num_instructions_covered_not_postprocessed, fraction_instructions_covered_not_postprocessed, num_instructions_covered_postprocessed, fraction_instructions_covered_postprocessed) VALUES(?,?,?,?)";
     sqlite3x::sqlite3_command cmd(con, db_insert_n.c_str());
     cmd.bind(1,num_instructions_covered_not_postprocessed);
     cmd.bind(2,boost::lexical_cast<string>(num_instructions_covered_not_postprocessed*1.0/num_instructions) );
     cmd.bind(3,num_instructions_covered_postprocessed);
     cmd.bind(4,boost::lexical_cast<string>(num_instructions_covered_postprocessed*1.0/num_instructions));
     cmd.executenonquery();
   }
   catch(exception &ex) {cerr << "Exception Occurred: " << ex.what() << endl;}
*/

/*  try {con.executenonquery("insert into total_coverage(num_instructions_covered_not_postprocessed, fraction_instructions_covered_not_postprocessed, num_instructions_covered_postprocessed, fraction_instructions_covered_postprocessed) select sum(num_instructions_covered_not_postprocessed), sum(num_instructions_covered_not_postprocessed) * 1.0 / (select sum(num_instructions) from function_statistics), sum(num_instructions_covered_postprocessed), sum(num_instructions_covered_postprocessed) * 1.0 / (select sum(num_instructions) from function_statistics) from function_coverage");}
  catch(exception &ex) {cerr << "Exception Occurred: " << ex.what() << endl;}
*/

  // End of coverage gathering
  cerr << "... done" << endl;

  try{
	con.executenonquery("delete from results;");
  }
  catch(exception &ex) {
	cerr << "Exception Occurred: " << ex.what() << endl;
  }

  try{

        // For any cluster that has more than one distinct instruction
        // sequence, insert the cluster ID and count of number of clones with
        // each instruction sequence into the_rate
        try {
          sqlite3_command(con, "create temporary table cluster_instr_seqs(cluster integer, instr_seq blob, instr_seq_count integer)").executenonquery();
        } catch (exception& ex) {cerr << "Exception Occurred 1 : " << ex.what() << endl;}
        try {
          sqlite3_command(con, "insert into cluster_instr_seqs select distinct cluster, instr_seq, count(postprocessed_clusters.row_number) as instr_seq_count from postprocessed_clusters, vectors where postprocessed_clusters.vectors_row = vectors.row_number group by cluster, instr_seq").executenonquery();
        } catch (exception& ex) {cerr << "Exception Occurred 2 : " << ex.what() << endl;}
        try {
          sqlite3_command(con, "create temporary table clusters_with_false_positives(cluster integer, num_false_positive_elements integer)").executenonquery();
        } catch (exception& ex) {cerr << "Exception Occurred 3 : " << ex.what() << endl;}
        try {
          sqlite3_command(con, "insert into clusters_with_false_positives select cluster, sum(instr_seq_count) - max(instr_seq_count) from cluster_instr_seqs group by cluster").executenonquery();
        } catch (exception& ex) {cerr << "Exception Occurred 4 : " << ex.what() << endl;}
        cerr << "clusters_with_false_positives made" << endl;
  }catch(exception &ex) {
	cerr << "Exception Occurred outer: " << ex.what() << endl;
  }

  int num_false_positive_elts = sqlite3_command(con, "select sum(num_false_positive_elements) from clusters_with_false_positives").executeint();
  int total = sqlite3_command(con, "select count(row_number) from postprocessed_clusters").executeint();
  double false_positive_rate = (double)num_false_positive_elts / total;
  std::cout << "Total:" <<  total<< std::endl;

  // std::cout << "Total distinct:" <<  total_false << std::endl;
  std::cout << "False positive rate: " << false_positive_rate << std::endl;

  std::string db_select_n = "INSERT INTO results(edit_distance,false_positive_rate) VALUES(?,?)";

  try{
	//sqlite3_transaction trans2(con);

	{

	  sqlite3_command cmd(con, db_select_n.c_str());
	  cmd.bind(1,0);
	  cmd.bind(2,false_positive_rate);

	  cmd.executenonquery();
	}
	//trans2.commit();
  }
  catch(exception &ex) {
	cerr << "Exception Occurred: " << ex.what() << endl;
  }



};

inline double tvToDouble(const timeval& tv) {
  return tv.tv_sec + tv.tv_usec * 1.e-6;
}

void insert_timing(sqlite3_connection& con, 
	std::string property_name,
	const timeval& before, const timeval& after,
	const rusage& ru_before, const rusage& ru_after)
{
  try {
	string timing_insert = "INSERT INTO timing(property_name, total_wallclock, total_usertime, total_systime, wallclock, usertime, systime) VALUES(?,?,?,?,?,?,?)";
	sqlite3_command cmd(con, timing_insert.c_str());
	cmd.bind(1, property_name);
	cmd.bind(2, 0);
	cmd.bind(3, tvToDouble(ru_after.ru_utime));
	cmd.bind(4, tvToDouble(ru_after.ru_stime));
	cmd.bind(5, (tvToDouble(after) - tvToDouble(before)));
	cmd.bind(6, (tvToDouble(ru_after.ru_utime) - tvToDouble(ru_before.ru_utime)));
	cmd.bind(7, (tvToDouble(ru_after.ru_stime) - tvToDouble(ru_before.ru_stime)));
	cmd.executenonquery();
  } catch (exception& ex) {
	cerr << "Exception on timing write: " << ex.what() << endl;
  }


}


int main(int argc, char* argv[])
{
  std::string database;

  try {
	options_description desc("Allowed options");
	desc.add_options()
	  ("help", "produce a help message")
	  ("database", value< string >()->composing(), 
	   "the sqlite database that we are to use")
	  ;

	variables_map vm;
	store(command_line_parser(argc, argv).options(desc)
		.run(), vm);


	if (vm.count("help")) {
	  cout << desc;            
	  exit(0);
	}

	if (vm.count("database")!=1  ) {
	  std::cerr << "Missing options. Call as: postProcessing --database <database-name>" 
		<< std::endl;
	  exit(1);

	}

	database = vm["database"].as<string >();
	cout << "database: " << database << std::endl;


  }
  catch(exception& e) {
	cout << e.what() << "\n";
  }

  sqlite3_connection con(database.c_str());

  {
	struct timeval before, after;
	struct rusage ru_before, ru_after;
	gettimeofday(&before, NULL);
	getrusage(RUSAGE_SELF, &ru_before);

	{ 
	  calculate_result_statistics(con);
	}


	gettimeofday(&after, NULL);
	getrusage(RUSAGE_SELF, &ru_after);

	insert_timing(con, "compute-statistics",before,after, ru_before, ru_after );

  }
  return 0;
};



