/* -----------------------------------------------------------
 * tps : 6th March 2009: RTED
 * -----------------------------------------------------------*/
#include <rose.h>
#include <string>
#include "DataStructures.h"
#include "RtedTransformation.h"

#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <vector>
#include <iostream>

#include <sys/stat.h>
#include "boost/filesystem/operations.hpp"

#include <unistd.h>

using namespace boost;
using namespace std;
using namespace boost::filesystem;

static string rtedpath;
static vector<string> cdirs;

/* -----------------------------------------------------------
 * Traversal used to clean-up AST
 * -----------------------------------------------------------*/
class DeleteAST2: public SgSimpleProcessing {
public:
  void visit(SgNode* node) {
    if (node) {
      delete node;
    }
  }
};

/* -----------------------------------------------------------
 * Check arguments to main
 * -----------------------------------------------------------*/
std::string containsArgument(int argc, char** argv, char* pattern) {
  for (int i = 1; i < argc; i++) {
    if (!strcmp(argv[i], pattern)) {
      return argv[i + 1];
    }
  }
  return "";
}

/* -----------------------------------------------------------
 * Cleanup Project
 * -----------------------------------------------------------*/
void cleanUp(SgProject* project) {
  // tps (24 Mar 2009) : can't clean up yet -- will cause trouble!
#if 0
  VariantVector vv1 = V_SgNode;
  vector<SgNode*> vec = NodeQuery::queryMemoryPool(vv1);
  std::cout << "Number of nodes before AST deleting: " <<
    vec.size() << std::endl;

  DeleteAST2 deleteTree;
  deleteTree.traverse(project,postorder);

  vec = NodeQuery::queryMemoryPool(vv1);
  std::cout << "Number of nodes after AST deletion: " <<
    vec.size() << std::endl;

  map<std::string, int> nodes;
  vector<SgNode*>::const_iterator it = vec.begin();
  for (;it!=vec.end();++it) {
    SgNode* node = *it;
    ROSE_ASSERT(node);
    std::string name = node->class_name();
    nodes[name]++;
    delete node;
  }

  map<std::string, int>::const_iterator it2 = nodes.begin();
  for (;it2!=nodes.end();++it2) {
    string name = it2->first;
    int amount = it2->second;
    cout << "  Could not delete : " << name << "  -  " << amount << endl;
  }

  std::cout << "Number of nodes after deleting in Memory pool: " <<
    NodeQuery::queryMemoryPool(vv1).size() << std::endl;
#endif
}

/* -----------------------------------------------------------
 * Read in all directories of RTED project.
 * Needed for automation
 * -----------------------------------------------------------*/
int getdir(string dir, vector<string> &files, vector<string> &filesExtra) {
  DIR *dp;
  struct dirent *dirp;
  if ((dp = opendir(dir.c_str())) == NULL) {
    cout << "Error(" << errno << ") opening " << dir << endl;
    return errno;
  }

  while ((dirp = readdir(dp)) != NULL) {
    if (dirp->d_type == 8) { //8 for file, 4 directory
      string name = string(dirp->d_name);
      string subdir = dir + "/" + name;
      if (name.find("_s.c") != std::string::npos) {
	unsigned int pos = name.find("_s.c");
	unsigned int length = pos + 4;
	if (name.length() == length)
	  filesExtra.push_back(name);
      } else if (name.find(".c") != std::string::npos) {
	unsigned int pos = name.find(".c");
	unsigned int length = pos + 2;
	if (name.length() == length)
	  files.push_back(name);
      }
    }
  }
  closedir(dp);
  return 0;
}

void
runtimeCheck(int argc, char** argv,vector <string>& rtedfiles) {
  // PARSE AND TRANSFORM - 1rst round--------------------------------
  // Init Transformation object
  RtedTransformation rted;
  // Start parsing the project and insert header files
  cerr << "Parsing project (1st time)..." << endl;
  SgProject* project = rted.parse(argc, argv);
  cerr << "Adding headers to project and writing out the project." << endl;
  rted.insertProlog(project);
  // The backend results in a file called rose_filename.c
  backend(project);
  // PARSE AND TRANSFORM - 1rst round--------------------------------


  // PARSE AND TRANSFORM - 2rst round--------------------------------
  // clean up the project and delete all nodes
  cerr << "Cleaning up old project..." << endl;
  cleanUp(project);

  // read in the generated project
  cerr << "Calling backend (1st time)..." << endl;
  vector<string>::const_iterator it = rtedfiles.begin();
  int i=1;
  for (; it !=rtedfiles.end() ; ++it) {
    string file = *it;
    string* nfile = new string ("rose_"+file);
    argv[i] = (char*)(*nfile).c_str();
    cerr << "argv["<<i << "] CHANGING : >>>>> " << file << " to " << argv[i] << endl;
    i++;
  }
  cerr << "argv[1] : " << argv[1] << endl;

  cerr << "Parsing rose_project... (2nd time)" << endl;
  for (int i=0;i<argc;++i)
    cout << argv[i] << " " ;
  cout << endl;
  project = rted.parse(argc, argv);
  ROSE_ASSERT(project);
  // perform all necessary transformations (calls)
  cerr << "Transforming ... " << endl;
  rted.transform(project);
  cerr << "Calling backend... (2nd time)" << endl;
  // call backend and create a new rose_rose_filename.c source file
  backend(project);
  // PARSE AND TRANSFORM - 2rst round--------------------------------
}

/* -----------------------------------------------------------
 * Main Function for RTED
 * -----------------------------------------------------------*/
int main(int argc, char** argv) {
  // INIT -----------------------------------------------------
  // call RTED like this:
  if (argc < 2) { //7
    cerr
      << "./runtimeCheck NRFILES FILES [-I...]"
      << endl;
    exit(0);
  }
  int nrfiles = static_cast<int>(strtol(argv[1], NULL, 10));

  vector <string> rtedfiles;
  string abs_path="";
  for (int i=0; i< argc; ++i) {
    if (i>1 && i<=(nrfiles+1)) {
      string filename = argv[i];
      int pos=filename.rfind("/");
      if (pos>0 && pos!=(int)string::npos) {
	abs_path = filename.substr(0,pos+1);
	filename = filename.substr(pos+1,filename.length());
      }
      rtedfiles.push_back(filename);
      cerr << i << ": >>>>> Found filename : " << filename << endl;
    }
  }
  // files are pushed on through bash script
  //sort(rtedfiles.begin(), rtedfiles.end(),greater<string>());
  cerr << " >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> NR OF FILES :: " << rtedfiles.size() << endl;

  // move arguments one to left
  for (int i=2;i<argc;++i) {
    argv[i-1]=argv[i];
  }
  argc=argc-1;

  for (int i=0; i< argc; ++i) {
    cout << i << " : " << argv[i] << endl;
  }

  cerr << "Running RTED in :" << abs_path << endl;
  // INIT -----------------------------------------------------


  runtimeCheck(argc, argv, rtedfiles);

  return 0;
}

