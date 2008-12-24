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
		     std::vector<std::string> dllBB, bool t) { 
  window=0;
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
  
  initAnalyses();
  init();
  currentSelectedFile=fileA;
  if (test==false)
    createGUI();
  cerr << "Initialization done." <<endl;
  if (test)
    testAnalyses();
}

void BinQbatch::initAnalyses() {
  cerr << "Checking for analyses ... " << endl;
  analyses.clear();
  analyses.push_back(new BinCallGraph());
  analyses.push_back(new DynamicInfo());
  analyses.push_back(new ForbiddenFunctionCall());
  analyses.push_back(new MallocAndFree());
  analyses.push_back(new NullAfterFree());
  analyses.push_back(new InitPointerToNull());
  analyses.push_back(new ComplexityMetric());
  analyses.push_back(new BufferOverflow());

}

int BinQbatch::addRemainingAnalyses() {
  int before = analyses.size();
  analyses.push_back(new DiffAlgo());
  analyses.push_back(new FunctionDiffAlgo());
  analyses.push_back(new AlignFunction());
  analyses.push_back(new BinControlFlowAnalysis());
  analyses.push_back(new BinDataFlowAnalysis());
  analyses.push_back(new InterruptAnalysis());
  return before;
}

// This is for testing purposes only
void
BinQbatch::runAnalyses() {
  string fileName = "analysisResult.txt";
  double startTotal = RoseBin_support::getTime();
  std::ofstream myfile;
  myfile.open(fileName.c_str());
  int problems=0;

  for (unsigned int i=0;i<analyses.size();++i) {
    bool twoFiles = analyses[i]->twoFiles();
    if (twoFiles && fileB!=NULL || twoFiles==false) {
      currentAnalysis=analyses[i];
      if (currentAnalysis) {
	cerr << "Running analysis : " << analyses[i]->name().c_str() << endl;
	double start = RoseBin_support::getTime();
	currentAnalysis->test(fileA,fileB);
	double end = RoseBin_support::getTime();
	double time = (double) (end - start);
	map<SgNode*,string> resu = currentAnalysis->getResult();
	problems+=resu.size();
	myfile << "Running analysis : " << analyses[i]->name().c_str() <<
	  "   time : " << time << "   Problems : " << RoseBin_support::ToString(resu.size()) << endl;
	cerr << "Running analysis : " << analyses[i]->name().c_str() <<
	  "   time : " << time << "   Problems : " << RoseBin_support::ToString(resu.size()) << endl;
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
	}
	analysisTab->setCurrentIndex(1);
	analysisResult->moveCursor(QTextCursor::Start);
      }
    }
  } 
 
  double endTotal = RoseBin_support::getTime();
  double timeTotal = (double) (endTotal - startTotal);
  myfile << "\nTotal time : " << timeTotal << "  total problems: " << problems << endl;
  myfile.close();  

  QString res = QString("Total time ... %1   Total problems %2")
    .arg(timeTotal)
    .arg(problems);
  analysisResult->append(res);  
  
  int start = addRemainingAnalyses();

  for (unsigned int i=start; i < analyses.size(); ++i){
    new QListWidgetItem((analyses[i]->name().c_str()), listWidget);
  }

  updateByteItemList();
}



BinQbatch::~BinQbatch(){

} //BinQGUI::~BinQGUI()
