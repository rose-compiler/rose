#include "rose.h"
#include "BinQinteractive.h"

#include "boost/filesystem/operations.hpp" 
#include <boost/lexical_cast.hpp>
#include <iostream>

#include "BinQSupport.h"
#include "folder.xpm"

using namespace qrs;
using namespace boost::filesystem;
using namespace std;
using namespace boost;
using namespace __gnu_cxx;


// Constructor
BinQinteractive::BinQinteractive(std::string fA, std::string fB, std::vector<std::string> dllAA, 
                                 std::vector<std::string> dllBB, bool t) {
  window=0;
  fileNameA=fA;
  fileNameB=fB;
  test =t;
  if (test==false) {
    cerr << "Running in interactive mode." << endl;
    window = new QRWindow( "mainWindow", QROSE::TopDown );
    binqsupport= new BinQSupport();
  } else {
    cerr << "Running in interactive test mode." << endl;
  }

  maxrows=5;
  dllA=dllAA;
  dllB=dllBB;
  currentAnalysis=NULL;
  sourceFile=false;
  
  // switsched order of theses two!
  init();   // create fileA and AuxInfo
  initAnalyses();

  currentSelectedFile=fileA;
  if (test==false) {
    createGUI();
    updateByteItemList();
  }
  //cerr << "Initialization done." <<endl;
  if (test)
    testAnalyses(analyses);
}

void BinQinteractive::initAnalyses() {
  //cerr << "Checking for analyses ... " << endl;
  analyses.clear();
  analyses.push_back(new DwarfFileInfo(algo));
  analyses.push_back(new DynamicInfo());
  analyses.push_back(new ForbiddenFunctionCall());
  analyses.push_back(new NullAfterFree());
  analyses.push_back(new MallocAndFree());
  analyses.push_back(new InitPointerToNull());
  analyses.push_back(new ComplexityMetric());
  analyses.push_back(new DiffAlgo());
  analyses.push_back(new FunctionDiffAlgo());
  analyses.push_back(new AlignFunction());
  analyses.push_back(new BinCallGraph());
  analyses.push_back(new BinControlFlowAnalysis());
  analyses.push_back(new BinDataFlowAnalysis());
  analyses.push_back(new BufferOverflow(algo));
  analyses.push_back(new InterruptAnalysis(algo));
}




BinQinteractive::~BinQinteractive(){

} //BinQGUI::~BinQGUI()
