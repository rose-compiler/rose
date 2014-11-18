#include "createCloneDetectionVectorsBinary.h"


#include <boost/pending/disjoint_sets.hpp>
#include <stdio.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <iostream>
#include <fstream>
#include "sqlite3x.h"
#include <list>
#include <math.h>
#include <boost/program_options.hpp>
#include <algorithm>
#include <string>
#include <cstdio> // for std::remove
#include "lsh.h"
using namespace std;
using namespace sqlite3x;
using namespace boost::program_options;

struct Element {
      int cluster;
      int64_t vectors_row;
      int64_t function_id;
      std::string file;
      std::string function_name;
      int64_t line;
      int64_t offset;
};



int main(int argc, char* argv[])
{
  std::string database;

  try {
	options_description desc("Allowed options");
	desc.add_options()
	  ("help", "produce a help message")
	  ("database,q", value< string >()->composing(), 
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
	  std::cerr << "Missing options. Call as: findClones --database <database-name>" 
		<< std::endl;
	  exit(1);

	}

	database = vm["database"].as<string >();
	cout << "database: " << database << std::endl;


  }
  catch(std::exception& e) {
	cout << e.what() << "\n";
  }



  SqlDatabase::TransactionPtr tx = SqlDatabase::Connection::create(database)->transaction();
 
  double similarity =0;
  
  int windowSize=0;
  int stride=0;
  
  try {
    similarity = tx->statement("select similarity_threshold from run_parameters limit 1")->execute_double();
  } catch (std::exception& ex) {std::cerr << "Exception Occurred: " << ex.what() << std::endl;}
  try {
    windowSize = tx->statement("select window_size from run_parameters limit 1")->execute_int();
  } catch (std::exception& ex) {std::cerr << "Exception Occurred: " << ex.what() << std::endl;}
  try {
    stride = tx->statement("select stride from run_parameters limit 1")->execute_int();
  } catch (std::exception& ex) {std::cerr << "Exception Occurred: " << ex.what() << std::endl;}


  std::cout << "Dataset generated with similarity " << similarity << " stride " << stride << " windowSize " << windowSize 
            << std::endl;




  scoped_array_with_size<vector<Element> > vectorOfClusters;

  int eltCount =0;
  
  try {
    if( similarity == 1.0)
    eltCount = boost::lexical_cast<size_t>(tx->statement("select count(distinct cluster) from postprocessed_clusters")->execute_string());

    else
    eltCount = boost::lexical_cast<size_t>(tx->statement("select count(distinct cluster) from clusters")->execute_int());
  } catch (exception& e) {cerr << "Exception: " << e.what() << endl;}

  if (eltCount == 0) {
    cerr << "No largest_clones table found -- invalid database?" << endl;
    exit (1);
  }

  std::cout << "Allocating " << eltCount << " elements " << std::endl;
  vectorOfClusters.allocate(eltCount);
  //Create set of clone pairs
  try{
    std::string databaseTable =(similarity == 1.0 ) ?" postprocessed_clusters " : " clusters " ;
    std::string selectSeparateDatasets ="select c.cluster, vec.id, function.id, file.name, function.name, vec.line, vec.index_within_function from vectors as vec join "+ databaseTable +" as c  on c.vectors_row = vec.id join semantic_functions as function on c.function_id = function.id join semantic_files as file on function.file_id = file.id order by c.cluster;";

    SqlDatabase::StatementPtr cmd = tx->statement(selectSeparateDatasets);

    
   
    std::vector<Element> thisCluster;
    
    //for all integers
//    int set_number = ds.find_set(0);

    
    int64_t last_clone=0;
    int64_t clone_cluster=0;
    
    for (SqlDatabase::Statement::iterator r=cmd->begin(); r!=cmd->end(); ++r) {
      Element cur_elem;
      cur_elem.cluster       = r.get<int64_t>(0);

      cur_elem.vectors_row   = r.get<int64_t>(1);
      cur_elem.function_id   = r.get<int64_t>(2);
      cur_elem.file          = r.get<std::string>(3);
      cur_elem.function_name = r.get<std::string>(4);
      cur_elem.line          = r.get<int64_t>(5);
      cur_elem.offset        = r.get<int64_t>(6);

      if( cur_elem.cluster != last_clone )
      {
         last_clone = cur_elem.cluster;
         clone_cluster++;
      }
      
      
      if(clone_cluster == eltCount)
        std::cout << "Bad idea: eltCount exceeded " << std::endl;
      vectorOfClusters[clone_cluster].push_back(cur_elem);
      
    }
  }catch(exception &ex) {
	cerr << "Exception Occured: " << ex.what() << endl;
  }


  for(int i = 0 ; i < eltCount; i++)
  {
    std::cout << "Cluster " << vectorOfClusters[i][0].cluster <<
      " has " << vectorOfClusters[i].size() << " elements. " << std::endl;

    for(size_t j=0; j < vectorOfClusters[i].size(); j++)
    {
      std::cout << "  elem " << j << ": row in vectors " << vectorOfClusters[i][j].vectors_row
                << " function id " << vectorOfClusters[i][j].function_id 
                << " file " << vectorOfClusters[i][j].file
                << " function name " << vectorOfClusters[i][j].function_name
                << " begin address " << hex << vectorOfClusters[i][j].line 
                << " end address " << hex << vectorOfClusters[i][j].offset 
                << dec << std::endl;
           

    }

  };

  
  std::cout << "\n\nDataset generated with similarity " << similarity << " stride " << stride << " windowSize " << windowSize 
            << std::endl;

  if( similarity == 1.0)
  std::cout << "\n\nClones are from the postprocessed dataset since the data was generated using exact clone detection " << std::endl;
  else
  std::cout << "\n\nClones are from the non-postprocessed dataset since the data was generated using inexact clone detection " << std::endl;

  return 0;
};



