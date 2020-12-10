/*************************************************************
 * Author   : Markus Schordan                                *
 *************************************************************/

#include <iostream>
#include <fstream>
#include "rose.h"
#include "AstMatching.h"
#include "AstTerm.h"
#include "TimeMeasurement.h"
#include "SgNodeHelper.h"

using namespace std;
using namespace CodeThorn;

class TestTraversal : public AstSimpleProcessing {
public:
  virtual void visit(SgNode* node) { /* do nothing */ };
};

// our helper function for writing info to a file
void write_file(string filename, string data) {
  ofstream myfile;
  myfile.open(filename.c_str(),ios::out);
  myfile << data;
  myfile.close();
}

int main( int argc, char * argv[] ) {

  TimeMeasurement timer;

  // Build the AST used by ROSE
  timer.start();
  SgProject* sageProject = frontend(argc,argv);
  double measurementFrontend=timer.getTimeDurationAndStop().milliSeconds();
  timer.stop();

  // Run internal consistency tests on AST
  timer.start();
  AstTests::runAllTests(sageProject);
  timer.stop();
  double measurementAstChecks=timer.getTimeDurationAndStop().milliSeconds();

  // default ROSE dot-file generation
  AstDOTGeneration dotGen;
  dotGen.generate(sageProject,"ast_demo",AstDOTGeneration::TOPDOWN);

  // create RoseAst object necessary for iterator
  RoseAst ast(sageProject);
  
  // obtain pointer to main function (if no main function is found, the pointer is 0)
  SgNode* functionRoot=ast.findFunctionByName("main");
  RoseAst functionAst(functionRoot);

  // create AST term as string and write on stdout
  string astAsString;
  #if 1
  astAsString=astTermToMultiLineString(functionRoot,4);
  #else
  astAsString=astTermWithNullToString(functionRoot);
  #endif
  cout<<endl;
  cout << "AST of main function:\n"<<astAsString<<endl;
  cout<<endl;

  // create dot-file as string using the RoseAst::iterator
  string dotFile=astTermWithNullValuesToDot(functionRoot);
  write_file("astmain.dot", dotFile);

  // measure iterator with null values (default)
  timer.start();
  long num1=0,num2=0;
  for(RoseAst::iterator i=ast.begin().withNullValues();i!=ast.end();++i) {
    num1++;
  }
  timer.stop();
  double iteratorMeasurementTimeWithNull=timer.getTimeDurationAndStop().milliSeconds();

  // measure iterator without null value
  timer.start();
  for(RoseAst::iterator i=ast.begin().withoutNullValues();i!=ast.end();++i) {
    num2++;
  }
  timer.stop();
  double iteratorMeasurementTimeWithoutNull=timer.getTimeDurationAndStop().milliSeconds();

  cout << "Iteration Length: with null   : " << num1 << " nodes."<<endl;
  cout << "Iteration Length: without null: " << num2 << " nodes."<<endl;
  cout<<endl;
  //double ttm=timer.getTimeDurationAndStop().milliSeconds();
  cout << "Measurement:"<<endl;
  //cout << "Trav:"<<ttm << ";";
  cout << "ROSE Frontend    : " << measurementFrontend << " ms"<<endl;
  cout << "ROSE AST checks  : " << measurementAstChecks << " ms"<<endl;
  cout << "iter-with-null   : "<<iteratorMeasurementTimeWithNull << " ms"<<endl;
  cout << "iter-without-null: "<<iteratorMeasurementTimeWithoutNull << " ms"<<endl;
  return 0;
}
