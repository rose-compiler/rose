#include "BinQbatch.h"

#include "boost/filesystem/operations.hpp" 
#include <boost/lexical_cast.hpp>
#include <iostream>

#include "folder.xpm"


//#include "LCS.h"
//#include "Clone.h"
//#include "FunctionDiff.h"
//#include "AlignFunctions.h"
//#include "BinCallGraph.h"
//#include "BinControlFlowAnalysis.h"
//#include "BinDataFlowAnalysis.h"
//#include "BufferOverflow.h"
//#include "InterruptAnalysis.h"
//#include "BinDynamicInfo.h"

using namespace boost::filesystem;
using namespace std;
using namespace boost;
using namespace __gnu_cxx;


// Constructor
BinQbatch::BinQbatch(std::string fA, std::string fB, std::vector<std::string> dllAA, 
		     std::vector<std::string> dllBB) { 
  fileNameA=fA;
  fileNameB=fB;
  cerr << "Running in batch mode." << endl;

  dllA=dllAA;
  dllB=dllBB;
  currentAnalysis=NULL;
  sourceFile=false;

  init();
  currentSelectedFile=fileA;

  cerr << "Initialization done." <<endl;
  testAnalyses();
}


// This is for testing purposes only
void
BinQbatch::testAnalyses() {
  for (unsigned int i=0;i<analyses.size();++i) {
    cerr << " testing analysis : " << analyses[i]->getDescription() << endl;
    bool twoFiles = analyses[i]->twoFiles();
    if (twoFiles && fileB!=NULL || twoFiles==false) {
      currentAnalysis=analyses[i];
      if (currentAnalysis) 
	currentAnalysis->test(fileA,fileB);
    }
  } 
 
}



BinQbatch::~BinQbatch(){

} //BinQGUI::~BinQGUI()
