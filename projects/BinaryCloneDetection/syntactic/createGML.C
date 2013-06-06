
#include <stdio.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <iostream>
#include <fstream>
#include "sqlite3x.h"

#include <boost/program_options.hpp>

#include <string>
#include <cstdio> // for std::remove
using namespace std;
using namespace sqlite3x;
using namespace boost::program_options;

#define test_gml 1

template <class T>
inline std::string tostring (const T& t)
{
  std::stringstream ss;
  ss << t;
  return ss.str();
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
  std::string filename = filen+".gml";
  std::ofstream myfile;
  myfile.open(filename.c_str());
  myfile << "graph CloneDetect[" <<endl;
  myfile << " directed" <<endl;
  int nr=0;
  int connect=5;
  int max=200000;

  typedef std::map < std::string,std::string> clusterType;
  clusterType clusterMap;


  try{
#if test_gml
	std::string selectSeparateDatasets ="SELECT distinct cluster from clusters where cluster<";
	//std::string selectSeparateDatasets ="SELECT distinct cluster from clusters where cluster<";
	string maxs =tostring(max);
	selectSeparateDatasets.append(maxs) ;
	selectSeparateDatasets.append(" group by cluster having count(distinct file)>");
	selectSeparateDatasets.append(tostring(connect)) ;

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
	  myfile << "  node [ id " << cluster << endl 
		<< "         label \"" << cluster <<"\"" << endl 
		<< "         Node_Color_ \"00FF00\"" << endl
		<< "         Node_Shape_ \"1\"" << endl
		<< "         Width_ \"0.1\"" << endl
		<< "         Height_ \"0.1\"" << endl
		<< "         Depth_ \"0.1\"" << endl
		<< "         Type_ \"[ 67108864 FUNCTION_NODE ]\"" << endl;
	  myfile << "         graphics [ type \"ellipse\" fill \"#00FF00\" ]" << endl
		<< "  ] " << endl;
	  clusterMap[cluster] = cluster;
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

	//std::string selectSeparateDatasets ="SELECT distinct file from clusters where cluster<";
	string maxs =tostring(max);
	selectSeparateDatasets.append(maxs) ;
	selectSeparateDatasets.append(" group by file having count(distinct cluster)>");
	selectSeparateDatasets.append(tostring(connect)) ;
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
	  myfile << "  node [ id " << nr << endl 
		<< "         label \"" << file <<"\"" << endl 
		<< "         Node_Color_ \"FF0000\"" << endl
		<< "         Node_Shape_ \"1\"" << endl
		<< "         Width_ \"0.2\"" << endl
		<< "         Height_ \"0.2\"" << endl
		<< "         Depth_ \"0.2\"" << endl
		<< "         type Type_ \"[ 67108864 FILE_NODE ]\"" << endl;
	  myfile << "         graphics [ type \"rectangle\" fill \"#FF0000\" ]" << endl
		<< "  ] " << endl;
	  fileMap[file] = nr;
	}
  }catch(exception &ex) {
	cerr << "Exception Occured: " << ex.what() << endl;
  }

  //Find all unique datasets
  try{

#if test_gml
	//    std::string selectSeparateDatasets ="SELECT cluster from clusters group by cluster having count(distinct file)>1";
	std::string selectSeparateDatasets ="select cluster, file, count(row_number) from clusters where cluster<";
	string maxs = tostring(max);
	selectSeparateDatasets.append(maxs+"  GROUP BY cluster, file;") ;
	cerr << selectSeparateDatasets << endl;
	cerr << selectSeparateDatasets << endl;
#else
	std::string selectSeparateDatasets ="select cluster, file, count(row_number) from clusters GROUP BY cluster, file;" ;
#endif
	sqlite3_command cmd(con, selectSeparateDatasets.c_str());
	sqlite3_reader datasets=cmd.executereader();

	while(datasets.read())
	{
	  std::string cluster = datasets.getstring(0);
	  std::string file   = datasets.getstring(1);
	  std::string count   = datasets.getstring(2);
	  fileType::iterator it = fileMap.find(file);
	  clusterType::iterator it2 = clusterMap.find(tostring(cluster));
	  if (it!=fileMap.end() && it2!=clusterMap.end()) {
		int nr = fileMap[file];
#if test_gml
		std::cout << "cluster " << cluster << " file " << file << " count "<< count << "  nr-file: " << nr << std::endl;
#endif	  
		myfile << "  edge [ "
		  << "         label \"" << cluster << "->" << nr <<"\"" << endl 
		  << "         source " << cluster << " target " << nr << endl
		  << "         Multiplicity_ \""<<count<<"\"" << endl
		  << "         Edge_Radius_ \""<<count<<"\"" << endl
		  << "  ] " << endl;
	  }
	}
  }catch(exception &ex) {
	cerr << "Exception Occured: " << ex.what() << endl;
  }

  myfile << "]" << endl;
  myfile.close();

  return 0;
};



