#include "rose.h"
#include "BinQbatch.h"

#include "boost/filesystem/operations.hpp" 
#include <boost/lexical_cast.hpp>
#include <iostream>

#include "folder.xpm"

#include "BinQSupport.h"

using namespace qrs;
using namespace boost::filesystem;
using namespace std;
using namespace boost;
using namespace __gnu_cxx;


// Constructor
BinQbatch::BinQbatch(std::string fA, std::string fB, std::vector<std::string> dllAA, 
                     std::vector<std::string> dllBB, bool t, 
		     std::string save) { 
  window=0;
  saveAST=save;
  fileNameA=fA;
  fileNameB=fB;
  test =t;
  if (test==false) {
    cerr << "Running in batch mode." << endl;
    window = new QRWindow( "mainWindow", QROSE::TopDown );
    binqsupport= new BinQSupport();
  } else {
    cerr << "Running in batch test mode." << endl;
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
  if (test==false)
    createGUI();
  //cerr << "Initialization done." <<endl;
  if (test) {
    // only run for single tests
    testAnalyses(preanalyses);
    testAnalyses(analyses);
  }
}

void BinQbatch::initAnalyses() {
  //cerr << "Checking for analyses ... " << endl;
  analyses.clear();
  preanalyses.clear();
  preanalyses.push_back(new DynamicInfo());
  //preanalyses.push_back(new BinDataFlowAnalysis());

  analyses.push_back(new ForbiddenFunctionCall());
  analyses.push_back(new MallocAndFree());
  analyses.push_back(new NullAfterFree());
  analyses.push_back(new InitPointerToNull());
  analyses.push_back(new ComplexityMetric());

// DQ (3/21/2009): Need to have Thomas look at this, I have commented it back out for now!
// DQ (3/19/2009): Uncommented this test.
// analyses.push_back(new BufferOverflow());
// analyses.push_back(new BufferOverflow());

}

int BinQbatch::addRemainingAnalyses() {
  int before = analyses.size();
  std::vector<BinAnalyses*>::const_iterator it =preanalyses.begin();
  for (;it!=preanalyses.end();++it) {
    analyses.push_back(*it);    
  }

  if (isSgProject(fileA)) {
  analyses.push_back(new DwarfFileInfo(algo));
  analyses.push_back(new BinCallGraph());
  analyses.push_back(new DiffAlgo());
  analyses.push_back(new FunctionDiffAlgo());
  analyses.push_back(new AlignFunction());
  analyses.push_back(new BinControlFlowAnalysis());
  analyses.push_back(new InterruptAnalysis(algo));
  }
  return before;
}

// This is for testing purposes only
void
BinQbatch::runAnalyses( std::vector<BinAnalyses*>& analysesVec, bool init) {
  string fileName = "analysisResult.txt";
  double startTotal = RoseBin_support::getTime();
  std::ofstream myfile;
  myfile.open(fileName.c_str());
  int problems=0;

  for (unsigned int i=0;i<analysesVec.size();++i) {
    bool twoFiles = analysesVec[i]->twoFiles();
    if (twoFiles && fileB!=NULL || twoFiles==false) {
      currentAnalysis=analysesVec[i];
      if (currentAnalysis) {
	//cerr << "Running analysis : " << analysesVec[i]->name().c_str() << endl;
	double start = RoseBin_support::getTime();
	currentAnalysis->test(fileA,fileB);
	double end = RoseBin_support::getTime();
	double time = (double) (end - start);
	map<SgNode*,string> resu = currentAnalysis->getResult();
	problems+=resu.size();
	myfile << "Running analysis : " << analysesVec[i]->name().c_str() <<
	  "   time : " << time << "   Problems : " << RoseBin_support::ToString(resu.size()) << endl;
	//cerr << "Running analysis : " << analysesVec[i]->name().c_str() <<
	//  "   time : " << time << "   Problems : " << RoseBin_support::ToString(resu.size()) << endl;
	QString res = QString("Running ... %1  time : %2   Problems: %3")
	  .arg(currentAnalysis->name().c_str())
	  .arg(time)
	  .arg(resu.size());
	analysisResult->append(res);  
	map<SgNode*,string>::const_iterator it = resu.begin();
	for (;it!=resu.end();++it) {
	  string str = "   "+it->second;
	  analysisResult->append(QString(str.c_str()));
	  myfile << "   " << str << endl;
	  std::cout << str << endl;
	}
	analysisTab->setCurrentIndex(1);
	analysisResult->moveCursor(QTextCursor::Start);
      }
    }
  } 
 
  double endTotal = RoseBin_support::getTime();
  double timeTotal = (double) (endTotal - startTotal);
  myfile << "\nTotal time : " << timeTotal << "  total problems: " <<  RoseBin_support::ToString(problems) << endl;
  myfile.close();  

  QString res = QString("Total time ... %1   Total problems %2")
    .arg(timeTotal)
    .arg( RoseBin_support::ToString(problems).c_str());
  analysisResult->append(res);  

  if (init) {} 
  else {
    int start = addRemainingAnalyses();
    
    for (unsigned int i=start; i < analysesVec.size(); ++i){
      new QListWidgetItem((analysesVec[i]->name().c_str()), listWidget);
    }
    
    updateByteItemList();
  }
}



