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
#include <boost/smart_ptr.hpp>

#include <algorithm>
#include <string>
#include <cstdio> // for std::remove
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

template <typename T>
class scoped_array_with_size {
  boost::scoped_array<T> sa;
  size_t theSize;

  public:
  scoped_array_with_size(): sa(), theSize(0) {}
  scoped_array_with_size(size_t s): sa(new T[s]), theSize(s) {}

  void allocate(size_t s) {
    sa.reset(new T[s]);
    theSize = s;
  }
  size_t size() const {return theSize;}
  T* get() const {return sa.get();}

  T& operator[](size_t i) {return sa[i];}
  const T& operator[](size_t i) const {return sa[i];}

  private:
  scoped_array_with_size(const scoped_array_with_size<T>&); // Not copyable
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



  sqlite3_connection con;
  con.open(database.c_str());


  double similarity =0;
  
  int windowSize=0;
  int stride=0;
  
  try {
    similarity = sqlite3x::sqlite3_command(con, "select similarity_threshold from detection_parameters limit 1").executedouble();
  } catch (std::exception& ex) {std::cerr << "Exception Occurred: " << ex.what() << std::endl;}
  try {
    windowSize = sqlite3x::sqlite3_command(con, "select window_size from run_parameters limit 1").executeint();
  } catch (std::exception& ex) {std::cerr << "Exception Occurred: " << ex.what() << std::endl;}
  try {
    stride = sqlite3x::sqlite3_command(con, "select stride from run_parameters limit 1").executeint();
  } catch (std::exception& ex) {std::cerr << "Exception Occurred: " << ex.what() << std::endl;}


  std::cout << "Dataset generated with similarity " << similarity << " stride " << stride << " windowSize " << windowSize 
            << std::endl;




  scoped_array_with_size<vector<Element> > vectorOfClusters;

  int eltCount =0;
  
  try {
    if( similarity == 1.0)
    eltCount = boost::lexical_cast<size_t>(con.executestring("select count(distinct cluster) from postprocessed_clusters"));

    else
    eltCount = boost::lexical_cast<size_t>(con.executestring("select count(distinct cluster) from clusters"));
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
    std::string selectSeparateDatasets ="select c.cluster, vec.row_number, fid.row_number, fid.file, fid.function_name, vec.line, vec.offset  from vectors vec join "+ databaseTable +  " c  on c.vectors_row = vec.row_number join function_ids fid  on c.function_id = fid.row_number ORDER BY c.cluster;";

    sqlite3_command cmd(con, selectSeparateDatasets.c_str());
    sqlite3_reader datasets=cmd.executereader();

    
   
    std::vector<Element> thisCluster;
    
    //for all integers
//    int set_number = ds.find_set(0);

    
    int64_t last_clone=0;
    int64_t clone_cluster=0;
      
    while(datasets.read())
    {
    
      Element cur_elem;
      cur_elem.cluster       = boost::lexical_cast<int64_t>(datasets.getstring(0));

      cur_elem.vectors_row   = boost::lexical_cast<int64_t>(datasets.getstring(1));
      cur_elem.function_id   = boost::lexical_cast<int64_t>(datasets.getstring(2));
      cur_elem.file          = datasets.getstring(3);
      cur_elem.function_name = datasets.getstring(4);
      cur_elem.line          = boost::lexical_cast<int64_t>(datasets.getstring(5));
      cur_elem.offset        = boost::lexical_cast<int64_t>(datasets.getstring(6));

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

    for(int j=0; j < vectorOfClusters[i].size(); j++)
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



