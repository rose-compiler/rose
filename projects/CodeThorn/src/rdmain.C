#include "VariableIdMapping.h"
#include "Labeler.h"
#include "CFAnalyzer.h"
#include <iostream>
#include "RDLattice.h"
#include "MFAnalyzer.h"

using namespace std;
using namespace CodeThorn;

class RDAnalyzer : public MFAnalyzer<RDLattice> {
public:
  RDAnalyzer(CFAnalyzer* cfanalyzer):MFAnalyzer<RDLattice>(cfanalyzer){}
  RDLattice transfer(RDLattice element) {
	cout << "RDAnalyzer: called transfer function."<<endl;
	return element;
  }
};


int main(int argc, char* argv[]) {
  cout << "INIT: Parsing and creating AST."<<endl;
  SgProject* sageProject = frontend(argc,argv);
  VariableIdMapping variableIdMapping;
  variableIdMapping.computeVariableSymbolMapping(sageProject);
  Labeler labeler(sageProject,&variableIdMapping);
  CFAnalyzer cfanalyzer(&labeler);
  RDAnalyzer rdAnalyzer(&cfanalyzer);
  rdAnalyzer.initialize();
  rdAnalyzer.run();
  return 0;
}
