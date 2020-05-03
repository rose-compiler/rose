// Author: Markus Schordan, 2013.
// Example AstMatcher : used for demonstrating and testing the matcher mechanism

#include <iostream>
#include <fstream>
#include "rose.h"
#include "AstTerm.h"
#include "AstMatching.h"
#include "SgNodeHelper.h"

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

  //std::string matchexpression;
  //std::cout<<"Enter match-expression: ";
  //std::getline(std::cin, matchexpression);

  bool measurementmode=false;

  // Build the AST used by ROSE
  SgProject* sageProject = frontend(argc,argv);
  
  // Run internal consistency tests on AST
  AstTests::runAllTests(sageProject);
  //AstDOTGeneration dotGen;
  //dotGen.generate(sageProject,"matcher",AstDOTGeneration::TOPDOWN);
  SgNode* root;
  //root=sageProject->get_traversalSuccessorByIndex(0)->get_traversalSuccessorByIndex(0)->get_traversalSuccessorByIndex(0)->get_traversalSuccessorByIndex(0);
  root=sageProject;
#if 0
  std::cout << "TERM INFO OUTPUT: START\n";
  std::cout << astTermToMultiLineString(root,0);
  std::cout << "TERM INFO OUTPUT: END\n";
#endif
  RoseAst ast(root);

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
  //$ARR=SgPntrArrRefExp($LHS,$RHS)";
  //std::string matchexpression="$Root=SgAssignOp($LHS,$RHS)";
  //std::string matchexpression="$LHS=SgPntrArrRefExp(SgPntrArrRefExp(SgArrowExp($E1,$E2),$E3),SgVarRefExp)"
  std::string matchexpression="$Root=SgAssignOp($LHS=SgPntrArrRefExp(SgPntrArrRefExp(SgArrowExp($WORK,$DS),$E1),$E2),$RHS)";
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
      cout<< "WORK:"<<(*i)["$WORK"]<<" : "<<(*i)["$WORK"]->unparseToString()<<endl;
      cout<< "DS:"<<(*i)["$DS"]<<" : "<<(*i)["$DS"]->unparseToString()<<endl;
      cout<< "E1:"<<(*i)["$E1"]<<" : "<<(*i)["$E1"]->unparseToString()<<endl;
      cout<< "E2:"<<(*i)["$E2"]<<" : "<<(*i)["$E2"]->unparseToString()<<endl;
      cout<< "RHS:"<<(*i)["$RHS"]<<" : "<<(*i)["$RHS"]->unparseToString()<<endl;

      // work -> dV[E1][E2] = RHS; ==> work -> dV.set(E1,E2,RHS);
      string work=(*i)["$WORK"]->unparseToString();
      string ds=(*i)["$DS"]->unparseToString();
      string e1=(*i)["$E1"]->unparseToString();
      string e2=(*i)["$E2"]->unparseToString();
      string rhs=(*i)["$RHS"]->unparseToString();
      string oldCode="/* OLD: "+(*i)["$Root"]->unparseToString()+"; */\n";
      string newCode="      /* NEW: */"+work+" -> "+ds+".set("+e1+","+e2+","+rhs+")"; // ';' is unparsed as part of the statement that contains the assignop
      SgNodeHelper::replaceAstWithString((*i)["$Root"], oldCode+newCode);

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
  backend(sageProject);
}

