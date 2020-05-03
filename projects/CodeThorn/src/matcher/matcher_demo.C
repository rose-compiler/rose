// Author: Markus Schordan, 2013.
// Example AstMatcher : used for demonstrating and testing the matcher mechanism

#include <iostream>
#include <fstream>
#include "rose.h"
#include "AstTerm.h"
#include "AstMatching.h"

#include "TimeMeasurement.h"

using namespace std;

class TestTraversal : public AstSimpleProcessing {
public:
  virtual void visit(SgNode* node) { /* do nothing */ };
};

void write_file(std::string filename, std::string data) {
  std::ofstream myfile;
  myfile.open(filename.c_str(),std::ios::out);
  myfile << data;
  myfile.close();
}

int main( int argc, char * argv[] ) {

  std::string matchexpression;
  std::cout<<"Enter match-expression: ";
  std::getline(std::cin, matchexpression);

  bool measurementmode=false;
  if(matchexpression[0]=='.') measurementmode=true;

  // Build the AST used by ROSE
  SgProject* sageProject = frontend(argc,argv);
  
  // Run internal consistency tests on AST
  AstTests::runAllTests(sageProject);
  AstDOTGeneration dotGen;
  dotGen.generate(sageProject,"matcher_demo",AstDOTGeneration::TOPDOWN);

  SgNode* root;
  //root=sageProject->get_traversalSuccessorByIndex(0)->get_traversalSuccessorByIndex(0)->get_traversalSuccessorByIndex(0)->get_traversalSuccessorByIndex(0);
  root=sageProject;
#if 0
  std::cout << "TERM INFO OUTPUT: START\n";
  std::cout << astTermToMultiLineString(root,0);
  std::cout << "TERM INFO OUTPUT: END\n";
#endif
  RoseAst ast(root);

#if 0
  int k=0;
  std::cout << "ITERATOR: Check1\n";
  RoseAst::iterator t1=ast.begin();  
  t1.print_top_element();
  RoseAst::iterator t0=ast.end();  
  t0.print_top_element();
  std::cout << "ITERATOR: Check2\n";
  *t1;
  std::cout << "ITERATOR: Check3\n";
  t1.print_top_element();
  std::cout << "ITERATOR: Check7.1\n";
  t1++;
  t1.print_top_element();
  std::cout << "ITERATOR: Check7.1\n";
  t1++;
  t1.print_top_element();
  std::cout << "ITERATOR: Check7.2\n";
  t1++;
  t1.print_top_element();
  std::cout << "ITERATOR: Check7.3\n";
  t1++;
  t1.print_top_element();
  std::cout << "ITERATOR: Check7.4\n";
  t1++;
  t1.print_top_element();
  std::cout << "ITERATOR: Check7.5\n";
  t1++;
  t1.print_top_element();
  std::cout << "ITERATOR: Check7.6\n";
  t1++;
  t1.print_top_element();
  std::cout << "ITERATOR: Check7.6\n";
  t1++;
  t1.print_top_element();
  std::cout << "ITERATOR: Check7.6\n";
  t1++;
  t1.print_top_element();
  std::cout << "ITERATOR: Check7.6\n";
  t1++;
  t1.print_top_element();
  std::cout << "ITERATOR: Check7.6\n";
  std::cout << "ITERATOR: START\n";
  for(RoseAst::iterator i=ast.begin().enableNullNodes();i!=ast.end();++i) {
    if(i.stack_size()==0) {
      std::cout << "\nDEBUG: Error found: empty stack, but we are still iterating:\n";
      std::cout << "i  :" << i.stack_size() << std::endl;
      std::cout << "end:" << ast.end().stack_size() << std::endl;
      std::cout << (i!=ast.end()) << std::endl;
    }
    //i.print_top_element(); std::cout << " :: ";
    std::cout << i.stack_size() << ":";
    if(*i)
      std::cout << k++ << ":"<< typeid(**i).name() << ";";
    else
      std::cout << k++ << ":null;";
    std::cout<<std::endl;
  }
  std::cout << "\nITERATOR: END"<<std::endl;
#endif


  TimeMeasurement timer;
  timer.start();
  long num1=0,num2=0;
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

  std::cout << "Iteration Length: with    null: " << num1 << std::endl;
  std::cout << "Iteration Length: without null: " << num2 << std::endl;
  
#if 1
  AstMatching m;
  if(!measurementmode) {
    timer.start();
    MatchResult r=m.performMatching(matchexpression,root);
    timer.stop();
    double matchingMeasurementTime=timer.getTimeDurationAndStop().milliSeconds();
    // print result in readable form for demo purposes
    std::cout << "Number of matched patterns with bound variables: " << r.size() << std::endl;
    for(MatchResult::iterator i=r.begin();i!=r.end();++i) {
      std::cout << "MATCH: \n"; 
      //SgNode* n=(*i)["X"];
      for(SingleMatchVarBindings::iterator vars_iter=(*i).begin();vars_iter!=(*i).end();++vars_iter) {
        SgNode* matchedTerm=(*vars_iter).second;
        std::cout << "  VAR: " << (*vars_iter).first << "=" << AstTerm::astTermWithNullValuesToString(matchedTerm) << " @" << matchedTerm << std::endl;
      }
      std::cout << std::endl;
      std::cout << "Matching time: "<<matchingMeasurementTime<<endl;
    }
    m.printMarkedLocations();
    m.printMatchOperationsSequence();
    write_file("astterm.txt",AstTerm::astTermToMultiLineString(root,2));
    write_file("astterm.dot",AstTerm::astTermWithNullValuesToDot(root));
  } else {
    std::string measurement_matchexpressions[]={"SgAssignOp","$X=SgAssignOp","_(_,_)","null","$X=SgAssignOp($Y,$Z=SgAddOp)","_($X,..)","_(#$X,..)"};
    int measurement_test_cases_num=7;
    double measurementTimes[7];

    for(int i=0;i<measurement_test_cases_num;i++) {
      timer.start();
      m.performMatching(measurement_matchexpressions[i],root);
      timer.stop();
      measurementTimes[i]=timer.getTimeDurationAndStop().milliSeconds();
    }

    TestTraversal tt;
    timer.start();
    tt.traverse(root, preorder);
    timer.stop();
    double ttm=timer.getTimeDurationAndStop().milliSeconds();
    std::cout << "Measurement:\n";
    std::cout << "Trav:"<<ttm << ";";
    std::cout << "iter:"<<iteratorMeasurementTime << ";";
    std::cout << "iter0:"<<iteratorMeasurementTimeWithoutNull << ";";
    for(int i=0;i<measurement_test_cases_num;i++) {
      //measurement_matchexpressions[i]
      std::cout << measurementTimes[i] << ";";
    }
    std::cout << std::endl;
  }
#endif
}

