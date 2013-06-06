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
using namespace std;
using namespace sqlite3x;
using namespace boost::program_options;
//using namespace boost;

struct Element {
      int function_id_A;
      int index_within_function_A;
      int end_index_within_function_A;
      int function_id_B;
      int index_within_function_B;
      int end_index_within_function_B;


      int vectors_row;
};




uint64_t cantor_pairing(const uint64_t& x, const uint64_t& y)
{
  return (x+y)*(x+y+1)/2+y;
};

std::pair<uint64_t,uint64_t> invert_cantor_pairing(const uint64_t& z)
{
  uint64_t w = (uint64_t) floorl( (sqrtl((long double)8*z+1)-1)/2 );
  uint64_t t = (w*w+w)/2;
  uint64_t y = z-t;
  uint64_t x = w-y;
  return pair<uint64_t,uint64_t>(x,y);
};

void insert_into_clusters(
    sqlite3x::sqlite3_connection& con,
    int cluster,  
    int function_id, 
    int begin_index_within_function,
    int end_index_within_function 
    ) {
  std::string db_select_n = "INSERT INTO largest_clusters(cluster, function_id, begin_index_within_function, end_index_within_function) VALUES(?,?,?,?)";
  try{
    //sqlite3_transaction trans2(con);
    {
      sqlite3x::sqlite3_command cmd(con, db_select_n.c_str());
      cmd.bind(1,cluster);
      cmd.bind(2,function_id);
      cmd.bind(3,begin_index_within_function);
      cmd.bind(4,end_index_within_function);
      cmd.executenonquery();
    }
    //trans2.commit();
  }
  catch(std::exception &ex) {
    std::cerr << "Exception Occurred: " << ex.what() << std::endl;
  }
}

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
  catch(exception& e) {
	cout << e.what() << "\n";
  }



  sqlite3_connection con;
  con.open(database.c_str());

  try {
	  con.executenonquery("create table IF NOT EXISTS largest_clusters(row_number INTEGER PRIMARY KEY, cluster INTEGER, function_id INTEGER, begin_index_within_function INTEGER, end_index_within_function INTEGER)");


  }
  catch(exception &ex) {
	cerr << "Exception Occurred: " << ex.what() << endl;
  }
 
  try {
	  con.executenonquery("delete from largest_clusters");


  }
  catch(exception &ex) {
	cerr << "Exception Occurred: " << ex.what() << endl;
  }

  string filen = database;


  std::vector<Element > listOfClonePairs;

  //Create set of clone pairs
  try{
    std::string selectSeparateDatasets ="SELECT * from  largest_clones ORDER BY end_index_within_function_B-begin_index_within_function_B";

    sqlite3_command cmd(con, selectSeparateDatasets.c_str());
    sqlite3_reader datasets=cmd.executereader();

    int thisClusterName;
    
    size_t eltCount = 0;

    try {
      eltCount = boost::lexical_cast<size_t>(con.executestring("select count(row_number) from largest_clones"));
    } catch (exception& e) {cerr << "Exception: " << e.what() << endl;}

    if (eltCount == 0) {
      cerr << "No largest_clones table found -- invalid database?" << endl;
      exit (1);
    }

    cerr << "Found " << eltCount << " pairs " << endl;

    std::vector<Element> thisCluster;
    
    //for all integers
//    int set_number = ds.find_set(0);

    
    while(datasets.read())
    {
      Element cur_elem;
      cur_elem.vectors_row   = boost::lexical_cast<int>(datasets.getstring(0));
      cur_elem.function_id_A = boost::lexical_cast<int>(datasets.getstring(1));
      cur_elem.index_within_function_A = boost::lexical_cast<int>(datasets.getstring(2));
      cur_elem.end_index_within_function_A = boost::lexical_cast<int>(datasets.getstring(3));
      cur_elem.function_id_B           = boost::lexical_cast<int>(datasets.getstring(4));
      cur_elem.index_within_function_B = boost::lexical_cast<int>(datasets.getstring(5));
      cur_elem.end_index_within_function_B = boost::lexical_cast<int>(datasets.getstring(6));

      cur_elem.vectors_row      = boost::lexical_cast<int>(datasets.getstring(3));
      listOfClonePairs.push_back(cur_elem);
/*
      std::cout << 
        ds.find_set(cantor_pairing(cur_elem.function_id_A, cur_elem.index_within_function_A) )
        << " "  << 
      ds.find_set(cantor_pairing(cur_elem.function_id_B, cur_elem.index_within_function_B) )
      << std::endl;
*/
      
    }
  }catch(exception &ex) {
	cerr << "Exception Occured: " << ex.what() << endl;
  }


  //The pairs are sorted according to the size of the code sequence they correspond to. We want to
  //iterate over each size separately so that a (function_id, begin_index, end_index) can be represented
  //completeley by (function_id,begin_index) which makes it possible to use canto_pairing.
  std::vector<int> indexForChange;
  int size = listOfClonePairs[0].end_index_within_function_B-listOfClonePairs[0].index_within_function_B;
  for( int i =0; i < listOfClonePairs.size(); i++   )
  {
    if( listOfClonePairs[i].end_index_within_function_B -  listOfClonePairs[i].index_within_function_B != size )
    {
      size = listOfClonePairs[i].end_index_within_function_B -  listOfClonePairs[i].index_within_function_B;
      indexForChange.push_back(i);
    }
  }


  int cluster =0;
  //Find clusters as dsjoint sets
  for( int i=0; i < indexForChange.size(); i++ )
  {
    int begin_index = 0;
    
    if(i>0)
      begin_index =  indexForChange[i-1];

    int end_index = indexForChange[i];
 
    uint64_t size = listOfClonePairs[begin_index].end_index_within_function_B-listOfClonePairs[begin_index].index_within_function_B;

    std::cout << "The size of this index " << end_index-begin_index <<
      " with window size " << size << std::endl;
//    continue;

    
    //Create a list of all the unique numbering for each A and B in a clone
    //pair (A,B). This is done because disjoint_sets_with_storage needs to know
    //how many sets there are.
    std::vector<std::pair<int,int> > uniqueElements;
    for(int j=begin_index; j < end_index; j++ )
    {
      Element& cur_elem = listOfClonePairs[j];
      assert(size == cur_elem.end_index_within_function_B-cur_elem.index_within_function_B);

      uniqueElements.push_back(std::pair<int,int>(cur_elem.function_id_A, cur_elem.index_within_function_A) );
      uniqueElements.push_back(std::pair<int,int>(cur_elem.function_id_B, cur_elem.index_within_function_B) );
      
    }
    unique(uniqueElements.begin(), uniqueElements.end());


    //disjoint_sets_with_storage can't handle negative numbers. Cantor pairing produces negative
    //numbers as well so we need a workaround. Therefore the map and the variable num.
    boost::disjoint_sets_with_storage<
      boost::identity_property_map, boost::identity_property_map,
            boost::find_with_full_path_compression
      
      > ds(uniqueElements.size());

    std::cout << "Size:" << end_index-begin_index << std::endl;
     
    //Insert each A and B in a clone pair (A,B) into set
    for(int j =0; j < uniqueElements.size() ; j++) ds.make_set(j);
     
    std::map<std::pair<int,int>,int> hack;
    //Pair A and B in a clone pair (A,B)
    for(int j=begin_index; j < end_index; j++ )
    {
      Element& cur_elem = listOfClonePairs[j];

      int curClusters = -1;
      std::map< std::pair<int,int>,int >::iterator itA = hack.find(std::pair<int,int> (cur_elem.function_id_A, cur_elem.index_within_function_A));
      if( itA !=  hack.end()  )
      {
        curClusters = itA->second;
        hack[  std::pair<int,int>(cur_elem.function_id_B, cur_elem.index_within_function_B) ]= curClusters;  

      }else{
        std::map< std::pair<int,int>,int >::iterator itB = hack.find(std::pair<int,int>(cur_elem.function_id_B, cur_elem.index_within_function_B));
        if( itB !=  hack.end()  )
        {
          curClusters = itB->second;

          hack[ std::pair<int,int> (cur_elem.function_id_A, cur_elem.index_within_function_A)] = curClusters;

              }
        else
        {
          cluster++;
          curClusters=cluster;
          hack[ std::pair<int,int> (cur_elem.function_id_A, cur_elem.index_within_function_A)] = curClusters;
          hack[  std::pair<int,int>(cur_elem.function_id_B, cur_elem.index_within_function_B) ]= curClusters;  

        }




      }

    }


    for( std::map<std::pair<int,int>,int>::iterator mapItr = hack.begin();
        mapItr != hack.end(); ++mapItr)
    {
      insert_into_clusters(con,mapItr->second, mapItr->first.first, mapItr->first.second,
          mapItr->first.second+size);


    }
   
    
# if 0
    //Pair A and B in a clone pair (A,B)
    for(int j=begin_index; j < end_index; j++ )
    {
      Element& cur_elem = listOfClonePairs[j];
      std::vector< std::pair<int,int> >::iterator itA = std::find(uniqueElements.begin(),uniqueElements.end(), std::pair<int,int> (cur_elem.function_id_A, cur_elem.index_within_function_A));
      assert(itA !=uniqueElements.end());

      std::vector< std::pair<int,int> >::iterator itB = std::find(uniqueElements.begin(), uniqueElements.end(), std::pair<int,int>(cur_elem.function_id_B, cur_elem.index_within_function_B));
      if(itB == uniqueElements.end())
      {

        std::cout << cur_elem.function_id_B << " : " << cur_elem.index_within_function_B << std::endl; 
      }
      assert( itA != itB );
      assert(*itA != *itB);
      assert(itB !=uniqueElements.end());
      int elemA = std::distance(uniqueElements.begin(), itA);
      int elemB =std::distance(uniqueElements.begin(),itB);
      ds.union_set( elemA, elemB );
      assert(ds.find_set(elemA) == 
          ds.find_set(elemB));

      std::cout << "elemA : " << elemA << " elemB: "<< elemB << " set " << ds.find_set(elemA) << std::endl;
    }


    map<int,int> clusterMap;
    for(int j = 0; j < uniqueElements.size() ; j++ )
    {
      int setNumber = ds.find_set(j);

      map<int,int>::iterator setNumItr = clusterMap.find(setNumber);

      int curCluster;
      if(setNumItr == clusterMap.end()){
        cluster++;
        curCluster = cluster;
        clusterMap[setNumber] = cluster;
      }else
        curCluster = setNumItr->second;
      //       std::cout << "The set:" << setNumber << std::endl;
      std::cout << "Set Number " <<  setNumber << " : " << curCluster <<std::endl;


      std::pair<int,int> element = uniqueElements[j];
      std::cout << "Pair " << element.first << " : " << element.second << std::endl;

      insert_into_clusters(con,curCluster, element.first, element.second,
          element.second+size);

    }
#endif
    //DO MAGIC ITERATION OVER RESULTS HERE

     
     
   }


  //Sort set of clone pairs

  //Perform clone merging

  bool first = true;

  try{
    sqlite3_transaction trans2(con);

    trans2.commit();

  }
  catch(exception &ex) {
	cerr << "Exception Occurred: " << ex.what() << endl;
  }

  std::cout << "Before size: " << listOfClonePairs.size() << std::endl;

  return 0;
};



