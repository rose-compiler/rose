/*
 *
 */

#include "rose.h"
#include <vector>
#include <qrose.h>
#include "BinQGui.h"
#include <boost/program_options.hpp>
#include <iostream>

using namespace qrs;
using namespace boost::program_options;
using namespace boost;
using namespace std;



int main( int argc, char **argv )
{
#if 0
  RoseBin_Def::RoseAssemblyLanguage = RoseBin_Def::x86;
  fprintf(stderr, "Starting binCompass frontend...\n");
  SgProject* project = frontend(argc,argv);
  ROSE_ASSERT (project != NULL);
  fprintf(stderr, "End binCompass frontend...\n\n\n");
#endif
  vector<std::string> dllA;
  vector<std::string> dllB;
  cerr << "\nUSAGE : BinQ -a binaryFileA [.so|.dll]* [-b binaryFileB|IdaFile|SourceFile [.so|.dll]* ]\n\n" << endl;
  std::string fileA="";
  std::string fileB="";
  bool aActive=false;
  bool bActive=false;
  bool test=false;
  bool debug=false;
  for (int i=1; i<argc; ++i) {
    string token = argv[i];
    if (debug)
      cerr << "Recognized argument " << i << " : >" << token <<"<"<< endl;
#if 1
    if (token=="--test") {
      if (debug)
	cerr << " found test" << endl;
      test=true;
    }
#endif
    if (aActive && token!="-b" && token!="--test") {
      if (debug)
	cerr << " a active" << endl;
      if (fileA=="") 
	fileA=argv[i];
      else
	dllA.push_back(argv[i]);
    }
    if (bActive && token!="--test") {
      if (debug)
	cerr << " b active" << endl;
      if (fileB=="") 
	fileB=argv[i];
      else
	dllB.push_back(argv[i]);
    }
    if (token=="-a") {
      if (debug)
	cerr << " found a" << endl;
      aActive=true;
      bActive=false;
    }
    if (token=="-b") {
      if (debug)
	cerr << " found b" << endl;
      aActive=false;
      bActive=true;
    }

  }
  
  cerr << "FileA: " << fileA << "  FileB: " << fileB << "    test: " << test << endl;
  if (fileA=="") exit(1);
  vector<std::string>::const_iterator it= dllA.begin();
  for (;it!=dllA.end();++it) {
    cerr << "  File A dll : " << *it<<endl; 
  }
  if (fileB!="") {
    it= dllB.begin();
    for (;it!=dllB.end();++it) {
      cerr << "  File B dll : " << *it<<endl; 
    }
  }

  if (test) {
    BinQGUI binGui(fileA,fileB,dllA,dllB,test);
  } else {
    QROSE::init(argc,argv);
    BinQGUI binGui(fileA,fileB,dllA,dllB,test);
    binGui.run();
    return QROSE::exec();
  }
  return 0;
}
