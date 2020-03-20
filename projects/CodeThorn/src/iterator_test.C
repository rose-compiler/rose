#include <iostream>
#include <fstream>
#include "rose.h"
#include "AstMatching.h"
#include "AstTerm.h"

#include "RoseAst.C"
#include "MatchOperation.C"
#include "AstMatching.C"
#include "AstTerm.C"

// for measurements only
#include "TimeMeasurement.cpp"
// to make ROSE policy check succeed only
#include "ShowSeq.h"

class TestTraversal : public AstSimpleProcessing {
public:
  TestTraversal():_counter(0) {}
  virtual void visit(SgNode* node) { _counter++; }
private:
  long _counter;
};

void write_file(std::string filename, std::string data) {
  std::ofstream myfile;
  myfile.open(filename.c_str(),std::ios::out);
  myfile << data;
  myfile.close();
}

int main( int argc, char * argv[] ) {
  // Build the AST used by ROSE
  SgProject* sageProject = frontend(argc,argv);
  
  // Run internal consistency tests on AST
  AstTests::runAllTests(sageProject);

  SgNode* root=sageProject;
  RoseAst completeast(root);
  std::string funtofind="my_sqrt";
  SgNode* mainroot=completeast.findFunctionByName(funtofind);
  if(!mainroot) { std::cerr << "No function '"<<funtofind<<"' found."; exit(1); }
  RoseAst ast(mainroot);
  //  SgNode* ast=completeast.findMethodInClass("myclass","mymethod");

  TimeMeasurement timer;
  timer.start();
  long num1=0,num2=0,num3=0;
  for(RoseAst::iterator i=ast.begin().withNullValues();i!=ast.end();++i) {
    num1++;
  }
  timer.stop();
  double iteratorMeasurementTime=timer.getTimeDurationAndStop().milliSeconds();

  timer.start();
  for(RoseAst::iterator i=ast.begin().withoutNullValues();i!=ast.end();++i) {
    num2++;
  }
  timer.stop();
  double iteratorMeasurementTimeWithoutNull=timer.getTimeDurationAndStop().milliSeconds();

#if 0
  timer.start();
  for(RoseAst::iterator_without_null i=ast.begin_without_null();i!=ast.end_without_null();++i) {
    num3++;
  }
  timer.stop();
  double fastiteratorMeasurementTimeWithoutNull=timer.getTimeDurationAndStop().milliSeconds();
#endif

  std::cout << "Iteration Length: with    null: " << num1 << std::endl;
  std::cout << "Iteration Length: without null: " << num2 << std::endl;
#if 0
  std::cout << "Iteration Length: fast without null: " << num3 << std::endl;
#endif
  
  TestTraversal tt;
  timer.start();
  tt.traverse(root, preorder);
  timer.stop();
  double ttm=timer.getTimeDurationAndStop().milliSeconds();

  write_file("iterator_test.dot", astTermToDot(ast.begin().withNullValues(),ast.end()));

  std::cout << "Measurement:\n";
  std::cout << "Trav:"<<ttm << ";";
#if 0
  std::cout << "fastiter-nonnull:"<<fastiteratorMeasurementTimeWithoutNull << ";";
#endif
  std::cout << "iter-nonnull:"<<iteratorMeasurementTimeWithoutNull << ";";
  std::cout << "iter:"<<iteratorMeasurementTime << ";";
  std::cout << std::endl;

  return 0;
}
