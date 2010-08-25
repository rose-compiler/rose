/****************************************************
 * RoseBin :: Binary Analysis for ROSE
 * Author : tps
 * Date : 3Apr07
 * Decription : Example code on how to use RoseBin
 ****************************************************/
#include "rose.h"
#include <stdio.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <iostream>

#include "sqlite3x.h"

#include "createSignatureVectors.h"

#include <boost/program_options.hpp>

#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"
#include "boost/progress.hpp"

using namespace boost::filesystem;


using namespace std;
using namespace sqlite3x;
using namespace boost::program_options;

inline double tvToDouble(const timeval& tv) {
  return tv.tv_sec + tv.tv_usec * 1.e-6;
}


class MyTraversal : public SgSimpleProcessing {
  public:
   SgAsmGenericFile* file;
   int counter;
    void visit(SgNode *astNode);
};

void
MyTraversal::visit(SgNode* astNode)
{
  SgAsmGenericFile *asmFile = isSgAsmGenericFile(astNode);
  if(isSgAsmInstruction(astNode) != NULL)
    counter++;
  if(asmFile) {
    file = asmFile;
  }
}


int main(int argc, char* argv[]) {
  struct timeval start;
  gettimeofday(&start, NULL);

  std::string database;
  std::string tsv_directory;
  //Default stride and windowSize
  size_t stride=1;
  size_t windowSize=80; 
/* 
{
    SgProject* project = frontend(argc,argv);
    MyTraversal myTraversal;
    myTraversal.traverseInputFiles(project , postorder);
}

exit(1);
*/
  bool respectFunctionBoundaries = true;

  try {
	options_description desc("Allowed options");
	desc.add_options()
          ("help", "produce a help message")
          ("ignoreBoundaries,i", "ignore function boundaries")
          ("database", value< string >()->composing(), 
           "the sqlite database that we are to use")
          ("tsv-directory", value< string >()->composing(), 
           "the input tsv directory or binary file")
          ("stride", value< size_t>()->composing(), "stride to use" )
          ("windowSize", value< size_t >()->composing(), "sliding window size" )
          ;

	variables_map vm;
	store(command_line_parser(argc, argv).options(desc)
		.run(), vm);


	if (vm.count("help")) {
	  cout << desc;           
	  exit(0);
	}

        if (vm.count("ignoreBoundaries")) {
          respectFunctionBoundaries = false;
	}

	if (vm.count("database")!=1 || vm.count("tsv-directory")!=1 
		|| vm.count("stride")!=1 ||  vm.count("windowSize")!=1  ) {
	  std::cerr << "Missing options. Call as: createVectorsBinary --stride <stride> --windowSize <window-size> --database <database-name>"
		        << " --tsv-directory <tsv-directory>" << std::endl;
	  exit(1);

	}

	tsv_directory = vm["tsv-directory"].as< string >();
	cout << "tsv-directory: " << tsv_directory << std::endl;

	database = vm["database"].as<string >();
	cout << "database: " << database << std::endl;

	stride = vm["stride"].as< int >();
	cout << "stride: " << stride << std::endl;

	windowSize = vm["windowSize"].as< int >();
	cout << "windowSize: " << windowSize << std::endl;
  }
  catch(exception& e) {
	cout << e.what() << "\n";
  }

  

  // binary code analysis *******************************************************
  cerr << " Starting binary analysis ... " << endl;

  std::cout << "Done reading options" << std::endl;

  SgNode* globalBlock;

  if(is_directory( tsv_directory  ) == true )
  {
    RoseBin_Def::RoseAssemblyLanguage=RoseBin_Def::x86;
    RoseBin_Arch::arch=RoseBin_Arch::bit32;
    RoseBin_OS::os_sys=RoseBin_OS::linux_op;
    RoseBin_OS_VER::os_ver=RoseBin_OS_VER::linux_26;


    RoseFile* roseBin = new RoseFile( (char*)tsv_directory.c_str() );

    cerr << " ASSEMBLY LANGUAGE :: " << RoseBin_Def::RoseAssemblyLanguage << endl;
    // query the DB to retrieve all data

    globalBlock = roseBin->retrieve_DB();

    // traverse the AST and test it
    roseBin->test();
  }else{
    vector<char*> args;
    args.push_back(strdup(""));
    args.push_back(strdup(tsv_directory.c_str()));

    ostringstream outStr; 

    for(vector<char*>::iterator iItr = args.begin(); iItr != args.end();
        ++iItr )
    {
      outStr << *iItr << " ";
    }     
    ;
    std::cout << "Calling " << outStr.str() << std::endl;


    globalBlock =  frontend(args.size(),&args[0]);

    MyTraversal myTraversal;
    myTraversal.counter=0;
    myTraversal.traverseInputFiles((SgProject*)globalBlock, postorder);
  //  globalBlock = myTraversal.file;
    std::cout << "The number of instructions is: " << myTraversal.counter << std::endl;

  }

  sqlite3_connection con;
  con.open(database.c_str());
  con.setbusytimeout(1800 * 1000); // 30 minutes
  createDatabases(con);

  try {
	if (con.executeint("select count(*) from run_parameters") >= 1) {
	  size_t oldWindowSize = boost::lexical_cast<size_t>(con.executestring("select window_size from run_parameters"));
	  size_t oldStride = boost::lexical_cast<size_t>(con.executestring("select stride from run_parameters"));
	  if (oldWindowSize != windowSize || oldStride != stride) {
		cerr << "Window size and stride do not match those already in database -- please remove the database and try again" << endl;
		cerr << "Old window size is " << oldWindowSize << endl;
		cerr << "Old stride is " << oldStride << endl;
		exit (1);
	  }
	} else {
	  sqlite3_command cmd(con, "insert into run_parameters(window_size, stride) values (?, ?)");
	  cmd.bind(1, boost::lexical_cast<string>(windowSize));
	  cmd.bind(2, boost::lexical_cast<string>(stride));
	  cmd.executenonquery();
	}
  } catch (exception& e) {cerr << "Exception checking run parameters " << e.what() << endl;}

  try {
	struct timeval before, after;
	struct rusage ru_before, ru_after;
	gettimeofday(&before, NULL);
	getrusage(RUSAGE_SELF, &ru_before);
	{
	  sqlite3_transaction trans2(con);
          if( respectFunctionBoundaries == true )
            createVectorsRespectingFunctionBoundaries(globalBlock, tsv_directory, windowSize, stride, con);
          else
            createVectorsNotRespectingFunctionBoundaries(globalBlock, tsv_directory, windowSize, stride, con);
    
	  trans2.commit();
	}
	gettimeofday(&after, NULL);
	getrusage(RUSAGE_SELF, &ru_after);
	cerr << "Time: " << (tvToDouble(after) - tvToDouble(before)) << " wall, " << (tvToDouble(ru_after.ru_utime) - tvToDouble(ru_before.ru_utime)) << " user, " << (tvToDouble(ru_after.ru_stime) - tvToDouble(ru_before.ru_stime)) << " sys" << endl;
	cerr << "Total time: " << (tvToDouble(after) - tvToDouble(start)) << " wall, " << tvToDouble(ru_after.ru_utime) << " user, " << tvToDouble(ru_after.ru_stime) << " sys" << endl;
	cerr << "Pre-vecgen time: " << (tvToDouble(before) - tvToDouble(start)) << " wall, " << tvToDouble(ru_before.ru_utime) << " user, " << tvToDouble(ru_before.ru_stime) << " sys" << endl;
	try {
	  string timing_insert = "INSERT INTO vector_generator_timing(file, total_wallclock, total_usertime, total_systime, vecgen_wallclock, vecgen_usertime, vecgen_systime) VALUES(?,?,?,?,?,?,?)";
	  sqlite3_command cmd(con, timing_insert.c_str());
	  cmd.bind(1, tsv_directory);
	  cmd.bind(2, (tvToDouble(after) - tvToDouble(start)));
	  cmd.bind(3, tvToDouble(ru_after.ru_utime));
	  cmd.bind(4, tvToDouble(ru_after.ru_stime));
	  cmd.bind(5, (tvToDouble(after) - tvToDouble(before)));
	  cmd.bind(6, (tvToDouble(ru_after.ru_utime) - tvToDouble(ru_before.ru_utime)));
	  cmd.bind(7, (tvToDouble(ru_after.ru_stime) - tvToDouble(ru_before.ru_stime)));
	  cmd.executenonquery();
	} catch (exception& ex) {
	  cerr << "Exception on timing write: " << ex.what() << endl;
	}
  } catch(exception &ex) {
    cerr << "Exception Occured: " << ex.what() << endl;
  }

  return 0;

} 
