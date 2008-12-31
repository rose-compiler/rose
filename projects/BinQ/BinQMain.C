/*
 *
 */

#include "rose.h"
#include <vector>
#include <qrose.h>
#include "BinQGui.h"
#include "BinQbatch.h"
#include "BinQinteractive.h"
#include <boost/program_options.hpp>
#include <iostream>
#include "boost/filesystem/operations.hpp" // includes boost/filesystem/path.hpp


using namespace qrs;
using namespace boost::program_options;
using namespace boost;
using namespace std;
using namespace boost::filesystem;

void printAssembly(string fileNameA, string fileNameB, SgNode* fileA, SgNode* fileB,
		   bool sourceFile) {
    // this part writes the file out to an assembly file -----------------------------------
    SgBinaryFile* binaryFileA = isSgBinaryFile(isSgProject(fileA)->get_fileList()[0]);
    SgAsmFile* file1 = binaryFileA != NULL ? binaryFileA->get_binaryFile() : NULL;
    SgAsmInterpretation* interpA = SageInterface::getMainInterpretation(file1);
    
    unparseAsmStatementToFile("unparsedA.s", interpA->get_global_block());
    
    if (fileNameB!="") 
      if(is_directory( fileNameB  ) == false && sourceFile==false) {
      SgBinaryFile* binaryFileB = isSgBinaryFile(isSgProject(fileB)->get_fileList()[0]);
      SgAsmFile* file2 = binaryFileB != NULL ? binaryFileB->get_binaryFile() : NULL;
      SgAsmInterpretation* interpB = SageInterface::getMainInterpretation(file2);
      unparseAsmStatementToFile("unparsedB.s", interpB->get_global_block());
    }
}


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
  cerr << "\nUSAGE : BinQ -a binaryFileA [.so|.dll]* [-b binaryFileB|IdaFile|SourceFile [.so|.dll]*] [--test] [--batch]\n\n " << endl;
  std::string fileA="";
  std::string fileB="";
  bool aActive=false;
  bool bActive=false;
  bool test=false;
  bool debug=false;
  bool batch=false;
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
    if (token=="--batch") {
      if (debug)
	cerr << " found batch" << endl;
      batch=true;
    }
#endif
    if (aActive && token!="-b" && token!="--test" && token!="--batch") {
      if (debug)
	cerr << " a active" << endl;
      if (fileA=="") 
	fileA=argv[i];
      else
	dllA.push_back(argv[i]);
    }
    if (bActive && token!="--test" && token!="--batch") {
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
  
  cerr << "FileA: " << fileA << "  FileB: " << fileB << "    test: " << test << "   batch : " << batch << endl;
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

  if (test && !batch) {
    BinQinteractive binGui(fileA,fileB,dllA,dllB,test);
    printAssembly(fileA,fileB, binGui.fileA, binGui.fileB, binGui.sourceFile);
  } else if (!test && !batch) {
    QROSE::init(argc,argv);
    BinQinteractive binGui(fileA,fileB,dllA,dllB,test);
    printAssembly(fileA,fileB, binGui.fileA, binGui.fileB, binGui.sourceFile);
    //binGui.run();
    return QROSE::exec();
  } else if (batch && !test) {
    QROSE::init(argc,argv);
    BinQbatch binGui(fileA,fileB,dllA,dllB,test);
    binGui.runAnalyses(binGui.preanalyses,true);
    binGui.runAnalyses(binGui.analyses,false);
    printAssembly(fileA,fileB, binGui.fileA, binGui.fileB, binGui.sourceFile);

    return QROSE::exec();
  } else {
    BinQbatch binGui(fileA,fileB,dllA,dllB,test);
    printAssembly(fileA,fileB, binGui.fileA, binGui.fileB, binGui.sourceFile);
  }
  return 0;
}
