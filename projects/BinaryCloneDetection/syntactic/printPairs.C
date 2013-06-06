
#include <sys/time.h>
#include <sys/resource.h>


#include <stdio.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <iostream>
#include <fstream>
#include "sqlite3x.h"
#include <list>

#include <boost/program_options.hpp>

#include <string>
#include <cstdio> // for std::remove
using namespace std;
using namespace sqlite3x;
using namespace boost::program_options;
using namespace std;
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
  } catch (std::exception& ex) {
	cerr << "Exception on timing write: " << ex.what() << endl;
  }


}

struct Element {
      int function_id;
      int index_within_function;
      int vectors_row;
      int last_index_within_function;
};


bool
operator<(const Element& left, const Element& right)
{
  if( left.function_id < right.function_id )
    return true;

  if( left.function_id == right.function_id)
  {
    if( left.index_within_function  <  right.index_within_function  )
      return true;
      
  };

  return false;
      
}

bool
operator==(const Element& left, const Element& right)
{

  if( left.function_id == right.function_id )
  {
    if( left.index_within_function  == right.index_within_function )
      return true;
  }

  return false;
      
}


bool overlaps(const std::pair<Element,Element>& left, const std::pair<Element, Element>& right, int windowSize, int stride)
{

  if (left.first.function_id != right.first.function_id  ) return false;
  if (left.second.function_id != right.second.function_id  ) return false;

  if (left.first.index_within_function >= right.first.last_index_within_function + windowSize / stride) return false;
  if (right.first.index_within_function >= left.first.last_index_within_function + windowSize / stride) return false;
  if (left.second.index_within_function >= right.second.last_index_within_function + windowSize / stride) return false;
  if (right.second.index_within_function >= left.second.last_index_within_function + windowSize / stride) return false;

  int strideFirst = left.first.last_index_within_function - right.first.last_index_within_function;
  int strideSecond = left.second.last_index_within_function - right.second.last_index_within_function;
  const int strideFuzz = 0;
  if (strideFirst > strideSecond + strideFuzz || strideSecond > strideFirst + strideFuzz) {
    return false;
  }

  return true;
};



class sort_list : public binary_function< bool, const std::pair<Element,Element>& , const std::pair<Element,Element>& > 
{
  public:
      bool operator()(const std::pair<Element,Element>& left, const std::pair<Element,Element>& right);
};

bool
sort_list::operator()(const std::pair<Element,Element>& left, const std::pair<Element,Element>& right)
{
   if( left.first.function_id < right.first.function_id  ) return true;
   if( left.first.function_id > right.first.function_id  ) return false;
   if( left.second.function_id < right.second.function_id  ) return true;
   if( left.second.function_id > right.second.function_id  ) return false;
   if (left.first.index_within_function - left.second.index_within_function < right.first.index_within_function - right.second.index_within_function) return true;
   if (left.first.index_within_function - left.second.index_within_function > right.first.index_within_function - right.second.index_within_function) return false;
#if 0
   if( left.first.index_within_function < right.first.index_within_function  ) return true;
   if( left.first.index_within_function > right.first.index_within_function  ) return false;
   return( left.second.index_within_function < right.second.index_within_function  );
#endif
   return( left.first.index_within_function < right.first.index_within_function  );
     

#if 0
  if( *left.first < *right.first  )
    return true;
  if( *left.first == *right.first )
  {
    if( *left.second < *right.second )
      return true;
  }
#endif
  return false;
}

void addCloneToDatabase(sqlite3_connection& con, std::pair<Element,Element>& clonePair) {

  string db_select_n = "INSERT INTO largest_clones( function_id_A,  begin_index_within_function_A, end_index_within_function_A, function_id_B, begin_index_within_function_B, end_index_within_function_B) VALUES(?,?,?,?,?,?)";

  sqlite3_command cmd(con, db_select_n.c_str());
  cmd.bind(1, clonePair.first.function_id );
  cmd.bind(2, clonePair.first.index_within_function);
  cmd.bind(3, clonePair.first.last_index_within_function );
  cmd.bind(4, clonePair.second.function_id);
  cmd.bind(5, clonePair.second.index_within_function);
  cmd.bind(6, clonePair.second.last_index_within_function);

  cmd.executenonquery();
};

int main(int argc, char* argv[])
{
  std::string database;
  //Timing
  struct timeval before;
  struct rusage ru_before;
  gettimeofday(&before, NULL);
  getrusage(RUSAGE_SELF, &ru_before);


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
	  con.executenonquery("create table IF NOT EXISTS largest_clones(row_number INTEGER PRIMARY KEY, function_id_A INTEGER, begin_index_within_function_A INTEGER, end_index_within_function_A INTEGER,"
            "function_id_B INTEGER , begin_index_within_function_B INTEGER, end_index_within_function_B INTEGER )");
  }
  catch(exception &ex) {
	cerr << "Exception Occurred: " << ex.what() << endl;
  }

   try {
	  con.executenonquery("delete from largest_clones");
  }
  catch(exception &ex) {
	cerr << "Exception Occurred: " << ex.what() << endl;
  }
 
  string filen = database;

  std::vector<std::pair <Element, Element> > listOfClonePairs;
  std::list<std::pair<int,int> > listOfFunctionClonePairs;

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

  //Create set of clone pairs
  try{
    std::string selectSeparateDatasets ="SELECT cluster, function_id, index_within_function, vectors_row from  postprocessed_clusters ORDER BY cluster, function_id, index_within_function";

    sqlite3_command cmd(con, selectSeparateDatasets.c_str());
    sqlite3_reader datasets=cmd.executereader();

    int64_t thisClusterName=-1;
    
    std::vector<Element> thisCluster;
    
    while(datasets.read())
    {
      Element cur_elem;
      int64_t cluster     = boost::lexical_cast<int64_t>(datasets.getstring(0));
      cur_elem.function_id = boost::lexical_cast<int>(datasets.getstring(1));
      cur_elem.index_within_function = boost::lexical_cast<int>(datasets.getstring(2));
      cur_elem.last_index_within_function = cur_elem.index_within_function;

      cur_elem.vectors_row      = boost::lexical_cast<int>(datasets.getstring(3));
//      cur_elem.line        = boost::lexical_cast<int> ( datasets.getstring(5) );
//      cur_elem.offset      = boost::lexical_cast<int> ( datasets.getstring(6) );


      if( cluster == thisClusterName )
      {
      
        for( std::vector<Element>::iterator iItr = thisCluster.begin();
            iItr != thisCluster.end(); ++iItr )
        {


          if( cur_elem.function_id < iItr->function_id)
          listOfFunctionClonePairs.push_back(std::pair<int,int>(cur_elem.function_id, iItr->function_id));
          else
           listOfFunctionClonePairs.push_back(std::pair<int,int>( iItr->function_id, cur_elem.function_id  ));

          if( cur_elem < *iItr )
          {

             listOfClonePairs.push_back(std::pair<Element,Element>( cur_elem,*iItr ) );
          }
          else            
          {
 
             listOfClonePairs.push_back(std::pair<Element,Element>(*iItr,cur_elem ) );
          }
        };
        

      }else{
        thisCluster.clear();
        thisClusterName = cluster;

      }
      
      thisCluster.push_back( cur_elem );

      
    }
  }catch(exception &ex) {
	cerr << "Exception Occured: " << ex.what() << endl;
  }

  listOfFunctionClonePairs.sort();

  listOfFunctionClonePairs.unique();



  for(std::list<std::pair<int,int> >::iterator iItr = listOfFunctionClonePairs.begin();
      iItr != listOfFunctionClonePairs.end(); iItr++ )
  {
    if(iItr->first != iItr->second)
        std::cout << iItr->second << " " << iItr->first << std::endl;
  }

  
  return 0;
};



