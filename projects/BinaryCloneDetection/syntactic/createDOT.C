
#include <stdio.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <iostream>

#include "sqlite3x.h"
#include <fstream>

#include <boost/program_options.hpp>

#include <string>
#include <cstdio> // for std::remove
#include <istream>
using namespace std;
using namespace sqlite3x;
using namespace boost::program_options;

#define test_gml 0

namespace toDOT {
template <class T>
inline std::string tostring (const T& t)
{
  std::stringstream ss;
  ss << t;
  return ss.str();
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

  string filen = database;
  std::string filename = filen+".dot";
  std::ofstream myfile;
  myfile.open(filename.c_str());
  myfile << "digraph CloneDetect {" <<endl;
  int nr=0;
  try{
#if test_gml
    std::string selectSeparateDatasets ="SELECT distinct cluster from clusters where cluster<";
    string maxs =toDOT::tostring(max);
    selectSeparateDatasets.append(maxs) ;
    cerr << selectSeparateDatasets << endl;
#else
	std::string selectSeparateDatasets ="SELECT distinct cluster from clusters" ;
#endif
	sqlite3_command cmd(con, selectSeparateDatasets.c_str());
	sqlite3_reader datasets=cmd.executereader();


	std::cout << "Unique clusters:" << std::endl;

	while(datasets.read())
	{
	  std::string cluster = datasets.getstring(0);
#if test_gml
	  std::cout << cluster << std::endl;
#endif	
	  myfile << "\""<< cluster << "\"" 
		 << "         [shape=ellipse,regular=0,color=\"Blue\",fillcolor=white,fontname=\"7x13bold\",fontcolor=black,style=filled];"
		 << endl;
	  nr++;
	}
  }catch(exception &ex) {
	cerr << "Exception Occured: " << ex.what() << endl;
  }


  typedef std::map < std::string,int> fileType;
  fileType fileMap;

  try{
#if test_gml
	std::string selectSeparateDatasets ="SELECT distinct file from clusters where cluster<";
	string maxs =toDOT::tostring(max);
	selectSeparateDatasets.append(maxs) ;
#else
	std::string selectSeparateDatasets ="SELECT distinct file from clusters" ;
#endif
 	sqlite3_command cmd(con, selectSeparateDatasets.c_str());
	sqlite3_reader datasets=cmd.executereader();
 
	std::cout << "Unique files:" << std::endl;

	while(datasets.read())
	{
	  nr++;
	  std::string file = datasets.getstring(0);
	  file = file.substr(0, file.find_last_not_of(" ") +1);
	  file = file.substr(0, file.find_last_not_of('\n') +1);
#if test_gml
	  std::cout << file << "  nr-file : " << nr << std::endl;
#endif
	  myfile << "\""<< nr << "\"" 
		 << "         [label=\"" << file <<"\"" << endl 
	         << "         ,shape=diamond,regular=0,color=\"Green\",fillcolor=green,fontname=\"7x13bold\",fontcolor=black,style=filled];"
		 << endl;
	  fileMap[file] = nr;
	}
  }catch(exception &ex) {
	cerr << "Exception Occured: " << ex.what() << endl;
  }

  //Find all unique datasets
  try{

#if test_gml
    std::string selectSeparateDatasets ="select cluster, file, count(row_number) from clusters where cluster<";
    string maxs = toDOT::tostring(max);
    selectSeparateDatasets.append(maxs+"  GROUP BY cluster, file;") ;
    cerr << selectSeparateDatasets << endl;
    cerr << selectSeparateDatasets << endl;
#else
	// std::string selectSeparateDatasets ="select cluster, file, count(row_number) from clusters where cluster < 1000 GROUP BY cluster, file;" ;
	std::string selectSeparateDatasets ="select cluster, file, count(row_number) from clusters GROUP BY cluster, file;" ;
#endif
 	sqlite3_command cmd(con, selectSeparateDatasets.c_str());
	sqlite3_reader datasets=cmd.executereader();

	while(datasets.read())
	{
	  std::string cluster = datasets.getstring(0);
	  std::string file   = datasets.getstring(1);
	  std::string count   = datasets.getstring(2);
	  int nr = fileMap[file];
#if test_gml
	  std::cout << "cluster " << cluster << " file " << file << " count "<< count << "  nr-file: " << nr << std::endl;
#endif	  
	  myfile << "\"" << cluster << "\" -> \"" << nr << "\";" << endl;

	}
  }catch(exception &ex) {
	cerr << "Exception Occured: " << ex.what() << endl;
  }

  myfile << "}" << endl;
  myfile.close();

  return 0;
};



