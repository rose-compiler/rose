/* -----------------------------------------------------------
 * tps : 6th March 2009: RTED
 * -----------------------------------------------------------*/
#include <rose.h>
#include <string>

#include "RtedTransformation.h"

#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <vector>
#include <iostream>

#include <sys/stat.h>
#include "boost/filesystem/operations.hpp"

using namespace boost;
using namespace std;
using namespace boost::filesystem;


static string rtedpath;
static vector<string> cdirs;

/* -----------------------------------------------------------
 * Check arguments to main
 * -----------------------------------------------------------*/
std::string
containsArgument(int argc, char** argv, char* pattern) {
  for (int i = 1; i < argc ; i++) {
    if (!strcmp(argv[i], pattern)) {
      return argv[i+1];
    }
  }
  return "";
}

/* -----------------------------------------------------------
 * Cleanup Project
 * -----------------------------------------------------------*/
void
cleanUp(SgProject* proj) {
#if 0
  // make sure all nodes in the project are deleted.
	  VariantVector vv1 = V_SgNode;
	  vector<SgNode*> vec = NodeQuery::queryMemoryPool(vv1);
	  std::cout << "Number of nodes before deleting: " <<
	    vec.size() << std::endl;

	  vector<SgNode*>::const_iterator it = vec.begin();
	  for (;it!=vec.end();++it) {
	    SgNode* node = *it;
	    // tps :: the following nodes are not deleted with the
	    // AST traversal. We can only delete some of them -
	    // but not all within the memory pool traversal
//	    cerr << "  Not deleted : " << node->class_name() << endl;
        delete node;
	  }

	    std::cout << "Number of nodes after deleting in Memory pool: " <<
	  NodeQuery::queryMemoryPool(vv1).size() << std::endl;
#endif
}

/* -----------------------------------------------------------
 * Read in all directories of RTED project.
 * Needed for automation
 * -----------------------------------------------------------*/
int
getdir (string dir, map<string, vector<string> > &files) {
  DIR *dp;
  struct dirent *dirp;
  if((dp  = opendir(dir.c_str())) == NULL) {
    cout << "Error(" << errno << ") opening " << dir << endl;
    return errno;
  }

  while ((dirp = readdir(dp)) != NULL) {
    if (dirp->d_type==4)  //8 for file, 4 directory
      if (string(dirp->d_name)!="." &&
	  string(dirp->d_name)!="..") {
	// for each language (C, C++ and Fortran,
	// find every project within RTED
	string language = dir+"/"+string(dirp->d_name);
	string plain_language = string(dirp->d_name);

	DIR *dp2;
	struct dirent *dirp2;
	if((dp2  = opendir(language.c_str())) == NULL) {
	  cout << "Error(" << errno << ") opening " << language << endl;
	  return errno;
	}
	while ((dirp2 = readdir(dp2)) != NULL) {
	  if (dirp2->d_type==4)  //8 for file, 4 directory
	    if (string(dirp2->d_name)!="." &&
		string(dirp2->d_name)!="..") {
	      string subdir = language + "/"+string(dirp2->d_name);
	      files[plain_language].push_back(subdir);
	    }
	}
	closedir(dp2);
      }
  }
  closedir(dp);
  return 0;
}

/* -----------------------------------------------------------
 * Main Function for RTED
 * -----------------------------------------------------------*/
int main(int argc, char** argv) {
  // INIT -----------------------------------------------------
  // call RTED like this:
  if (argc<7) {
    cerr << "./runtimeCheck -rtedpath RTED-PATH -path PATH -file FILE [-I...]" << endl;
    exit(0);
  }
  // make sure the arguments are valid and perpare the output files
  rtedpath=containsArgument(argc, argv, (char*)"-rtedpath");
  string path=containsArgument(argc, argv, (char*)"-path");
  string rtedfile=containsArgument(argc, argv, (char*)"-file");
  string includePath ="-I"+rtedpath+path;
  string abs_path=rtedpath+path+rtedfile;
  string abs_path_new="rose_"+rtedfile;
  cerr <<"Running RTED in :" <<abs_path << "  New file : " << abs_path_new << endl;

  // if the arguments are invalid, we assume that this is a test of a regular file
  // we check if it compiles and exit
  if (rtedpath=="") {
    // run only frontend for testing
    frontend(argc,argv);
    exit(0);
  }
  ROSE_ASSERT(rtedpath!="");

  // adjust all input parameters
  int moved=0;
  argv[1]=(char*)(abs_path).c_str();
  argv[2]=(char*)(includePath).c_str();
  for (int i=3;i<7;++i)
    if (i<argc)
      argv[i]=(char*)"";
  for (int i=7;i<argc;++i) {
    argv[i-4]=argv[i];
    moved++;
  }
  argc=moved+3;
  // INIT -----------------------------------------------------


  // AUTOMATION -----------------------------------------------------
  // Collect information about RTED directories
  // for automation of this work
  map<string,vector<string> > files;
  getdir(rtedpath,files);
  cout <<"RTED contains " << files.size() << " Language Folders." << endl;
  // AUTOMATION -----------------------------------------------------

  // PARSE AND TRANSFORM - 1rst round--------------------------------
  // Init Transformation object
  RtedTransformation rted(files);
  // Start parsing the project and insert header files
  cerr <<"Parsing project (1st time)..." << endl;
  SgProject* project = rted.parse(argc, argv);
  cerr <<"Creating pdf..." << endl;
  AstPDFGeneration pdf;
  pdf.generateInputFiles(project);
  cerr << "Adding headers to project and writing out the project." << endl;
  rted.insertProlog(project);
  cerr << "Calling backend (1st time)..." << endl;
  for (int i=0; i<argc;++i) {
    cout << argv[i] << " ";
  }
  cout << endl;
  // The backend results in a file called rose_filename.c
  backend(project);
  // PARSE AND TRANSFORM - 1rst round--------------------------------


  // PARSE AND TRANSFORM - 2rst round--------------------------------
  // clean up the project and delete all nodes
  cerr << "Cleaning up old project..." << endl;
  cleanUp(project);

  // read in the generated project
  argv[1]=(char*)(abs_path_new).c_str();
  cerr << "Running new project... (2st time)" << endl;
  project = rted.parse(argc, argv);
  ROSE_ASSERT(project);
  // perform all necessary transformations (calls)
  cerr << "Transforming ... " << endl;
  rted.transform(project);
  cerr << "Calling backend... (2st time)" << endl;
  for (int i=0; i<argc;++i) {
    cout << argv[i] << " ";
  }
  cout << endl;
  // call backend and create a new rose_rose_filename.c source file
  backend(project);
  // PARSE AND TRANSFORM - 2rst round--------------------------------
  return 0;
}


