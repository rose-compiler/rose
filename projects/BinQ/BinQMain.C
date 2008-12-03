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
  cerr << "USAGE : BinQ -a binaryFileA [.so|.dll]* [-b binaryFileB|IdaFile|SourceFile [.so|.dll]* ]" << endl;
  std::string fileA="";
  std::string fileB="";
  bool aActive=false;
  bool bActive=false;
  for (int i=1; i<argc; ++i) {
    string token = argv[i];
    //    cerr << "Recognized argument " << i << " : >" << token <<"<"<< endl;
    if (aActive) {
      if (fileA=="") 
	fileA=argv[i];
      else
      dllA.push_back(argv[i]);
    }
    if (bActive) {
      if (fileB=="") 
	fileB=argv[i];
      else
      dllB.push_back(argv[i]);
    }
    if (token=="-a") {
      aActive=true;
      bActive=false;
    }
    if (token=="-b") {
      aActive=false;
      bActive=true;
    }

  }
  
  cerr << "FileA: " << fileA << "  FileB: " << fileB << endl;
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


  QROSE::init(argc,argv);
  BinQGUI binGui(fileA,fileB,dllA,dllB);
  binGui.run();
  return QROSE::exec();
}
