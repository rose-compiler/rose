
#include "rose.h"

#include <vector>
#include <qrose.h>
#include "BinQGui.h"
#include "BinQbatch.h"
#include "BinQinteractive.h"
#include <boost/program_options.hpp>
#include <iostream>
#include "boost/filesystem/operations.hpp" // includes boost/filesystem/path.hpp

#include <fstream>

using namespace qrs;
using namespace boost::program_options;
using namespace boost;
using namespace std;
using namespace boost::filesystem;


// unparser for testing purposes
void 
printAssembly(string fileNameA, string fileNameB, SgNode* fileA, SgNode* fileB,
	      bool sourceFile) {
  // this part writes the file out to an assembly file -----------------------------------

  if(is_directory( fileNameA ) == false ) {
    SgBinaryComposite* binaryA = isSgBinaryComposite(isSgProject(fileA)->get_fileList()[0]);
    SgAsmGenericFile* file1 = binaryA != NULL ? binaryA->get_binaryFile() : NULL;
    SgAsmInterpretation* interpA = SageInterface::getMainInterpretation(file1);

    unparseAsmStatementToFile(fileNameA+".dump2", interpA->get_global_block());
  } else if (is_directory( fileNameA ) == true ) {
    //cerr << " Node : " << fileA->class_name() << endl;
    SgAsmBlock* block = isSgAsmBlock(fileA);
    if (block) {
      //      SgProject* proj = new SgProject();
      //      proj->append_statement(block);
      //proj->insertStatementInScope(block,false);
      //      SageInterface::appendStatement(
      //     isSgStatement(block),isSgScopeStatement(proj));
      //block->set_parent(proj);


      //fileA=proj;
    }
  }

  if (fileNameB!="") 
    if(is_directory( fileNameB  ) == false && sourceFile==false) {
      SgBinaryComposite* binaryB = isSgBinaryComposite(isSgProject(fileB)->get_fileList()[0]);
      SgAsmGenericFile* file2 = binaryB != NULL ? binaryB->get_binaryFile() : NULL;
      SgAsmInterpretation* interpB = SageInterface::getMainInterpretation(file2);
      unparseAsmStatementToFile(fileNameB+".dump2", interpB->get_global_block());
    }
}

double
createAndDeleteBinGUI(std::string fileName, std::string empty, 
		      std::vector<std::string> emptyVec, 
		      std::string saveFile, map<std::string,int>& analysisResults) {
  BinQbatch binGui(fileName, empty, emptyVec, emptyVec, true, saveFile);
  printAssembly(fileName,empty, binGui.fileA, binGui.fileB, binGui.sourceFile);
  // run Analyses here and collect info
  double timeForFile = binGui.getTestAnalysisTime();
  // iterate through all results and add to current file
  analysisResults = binGui.getTestAnalysisResults();
  return timeForFile;
}

// runs a list of files and returns the results
void
runListMode(string saveFile, 
	    string listFile) {
  //cerr << " Running in List Mode. Loading File: " << listFile << endl;
  ifstream inFile;
  vector<string> files;
  vector<string> filesSkip;

  // skip current files in result.xls (dont recalculate) if 
  // results.xls exits
  ifstream inFile2;
  string fileName2="results.xls";
  string fileNameSkip="";
  inFile2.open(fileName2.c_str());
  if (!inFile2) {
    cout << "Unable to open listFile2" << endl;
  } else {
    int i=0;
    bool removedPreheader=false;
    while (inFile2 >> fileNameSkip) {
      if (!removedPreheader && i>18) {i=0; removedPreheader=true;}
      if ( removedPreheader && (i%8)==0) {
	filesSkip.push_back(fileNameSkip);
	//cerr << i << " skipping file : " << fileNameSkip << endl;
      } //else 
	//cerr << i << " junk "  << fileNameSkip << endl;
      i++;

    }
    inFile2.close();
  }

  string fileName;
  inFile.open(listFile.c_str());
  if (!inFile) {
    cout << "Unable to open listFile" << endl;
    exit(1); // terminate with error
  }
  //cerr << "Found the following filenames : " << endl;
  while (inFile >> fileName) {
    //    if (!is_directory(fileName))
    bool found=false;
    vector<string>::const_iterator its = filesSkip.begin();
    for (;its!=filesSkip.end();++its) {
      if ((*its)==fileName)
	found=true;
    }
    if (!found) {
      files.push_back(fileName);
      //cerr << " Adding file : " << fileName << endl;
    }
  }

  inFile.close();
  //cerr << "Reading done. Files found: " << files.size() << endl;


  // store the final results per file
  int columns=0;
  int i=0;
  string timeStr="Time";
  string empty="";
  vector<string> emptyVec;
  ofstream fp_out;
  bool firstIt=true;
  int* totalBugs = NULL;
  double totalTime=0;
  fp_out.open("results.xls", ios::out);
  // iterate through all files
  vector<string>::const_iterator it = files.begin();
  map<string,int> analysisRes;
  for (;it!=files.end();++it) {
    string fileName = *it;
    //cerr << "Analyzing : " << fileName ;
    //if (i<10) {
    analysisRes.clear();
    try {
      double timeForFile = createAndDeleteBinGUI(fileName, empty, emptyVec, 
                                                 saveFile, analysisRes);

      if (i==0) {
	columns=analysisRes.size();
	totalBugs = new int[columns];
	for (int i=0;i<columns;++i) {
	  totalBugs[i]=0;
	}
      }
      // remove -tsv
      if (fileName.find("-tsv")!=std::string::npos)
	if (fileName.size()>4)
	  fileName = fileName.substr(0,fileName.size()-4);
      //cerr << "Writing results for : " << fileName << endl;
      if (firstIt) {
	firstIt=false;
	fp_out << "FILENAME " ;
	map<string,int>::const_iterator it4 = analysisRes.begin();
	for (;it4!=analysisRes.end();++it4) {
	  string analysisName = it4->first;
	  fp_out << "\t" << analysisName ;
	}
	fp_out << "\tTotal Bugs \tTime" << endl;
      }
      map<string,int>::const_iterator it3 = analysisRes.begin();
      fp_out << fileName ;
      int totalbugsRow=0;
      int j=0;
      for (;it3!=analysisRes.end();++it3) {
	int value = it3->second;
	fp_out << "\t" << value;
	totalbugsRow +=value;
	totalBugs[j]+=value;
	j++;
      }
      totalTime += timeForFile;
      fp_out << "\t" <<totalbugsRow << "\t" <<timeForFile << endl;
      //      }
      i++;
    } catch (int i) {
      //cerr << "Stop -- Aborted : " << fileName << endl;
    }
  }
  for (int i=0;i<columns;++i) {
    fp_out << "\t" <<totalBugs[i];
  }
  fp_out << "\t" << totalTime << endl;
  fp_out.close(); 
}




// main function for BinQ
int 
main( int argc, char **argv ) {
#if 0
  // testing of frontend
  RoseBin_Def::RoseAssemblyLanguage = RoseBin_Def::x86;
  fprintf(stderr, "Starting binCompass frontend...\n");
  SgProject* project = frontend(argc,argv);
  ROSE_ASSERT (project != NULL);
  fprintf(stderr, "End binCompass frontend...\n\n\n");
#endif

  string listName="";
  vector<std::string> dllA;
  vector<std::string> dllB;
  if (argc<=2)
  cerr << "\nUSAGE : BinQ -a binaryFileA [.so|.dll]* [-b binaryFileB|IdaFile|SourceFile [.so|.dll]*] [--test] [--batch] [--list listFile]\n\n " << endl;
  std::string fileA="";
  std::string fileB="";
  bool aActive=false;
  bool bActive=false;
  bool test=false;
  bool debug=false;
  bool batch=false;

  std::string saveFile="";
  for (int i=1; i<argc; ++i) {
    string token = argv[i];
    if (debug)
      cerr << "Recognized argument " << i << " : >" << token <<"<"<< endl;

    if (token=="-save") {
      if (debug)
	cerr << " saving ..." << endl;
      ROSE_ASSERT(argc>(i+1));
      saveFile=argv[i+1];
      ++i;
      continue;
    }
    if (token=="--test") {
      if (debug)
	cerr << " found test" << endl;
      test=true;
    }
    if (token=="--list") {
      if (debug)
	cerr << " found list" << endl;
      ROSE_ASSERT(argc>(i+1));
      listName=argv[i+1];
      ++i;
    }
    if (token=="--batch") {
      if (debug)
	cerr << " found batch" << endl;
      batch=true;
    }
    if (aActive && token!="-b" && token!="--test" && token!="--batch" && token!="--list") {
      if (debug)
	cerr << " a active" << endl;
      if (fileA=="") 
	fileA=argv[i];
      else
	dllA.push_back(argv[i]);
    }
    if (bActive && token!="--test" && token!="--batch" && token!="--list") {
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
  
  if (debug)
    cerr << "FileA: " << fileA << "  FileB: " << fileB << "    test: " << test << "   batch : " 
         << batch << " List : " << listName << endl;
  if (fileA=="" && listName=="") exit(1);
  vector<std::string>::const_iterator it= dllA.begin();
  for (;it!=dllA.end();++it) {
    if (debug)
      cerr << "  File A dll : " << *it<<endl; 
  }
  if (fileB!="") {
    it= dllB.begin();
    for (;it!=dllB.end();++it) {
      if (debug)
	cerr << "  File B dll : " << *it<<endl; 
    }
  }

  if (listName!="" ) {
    runListMode(saveFile, listName);
    return 0;
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
    BinQbatch binGui(fileA,fileB,dllA,dllB,test,saveFile);
    binGui.runAnalyses(binGui.preanalyses,true);
    binGui.runAnalyses(binGui.analyses,false);
    printAssembly(fileA,fileB, binGui.fileA, binGui.fileB, binGui.sourceFile);

    return QROSE::exec();
  } else {
    BinQbatch binGui(fileA,fileB,dllA,dllB,test,saveFile);
    printAssembly(fileA,fileB, binGui.fileA, binGui.fileB, binGui.sourceFile);
  }
  return 0;
}
