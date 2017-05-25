// Author: Markus Schordan, 2013.
// Example AstMatcher : used for demonstrating and testing the matcher mechanism

#include <iostream>
#include <fstream>
#include "rose.h"
#include "AstTerm.h"
#include "AstMatching.h"
#include "SgNodeHelper.h"

#include "Timer.h"

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

int main (int argc, char* argv[])
{
  rose::global_options.set_frontend_notes(false);
  rose::global_options.set_frontend_warnings(false);
  rose::global_options.set_backend_warnings(false);

  vector<string> argvList(argv, argv+argc);
  argvList.push_back("-rose:skipfinalCompileStep");
  // Build the AST used by ROSE
  //SgProject* sageProject = frontend(argc,argv);
  SgProject* sageProject=frontend (argvList); 

  
  // Run internal consistency tests on AST
  //AstTests::runAllTests(sageProject);
  //AstDOTGeneration dotGen;
  //dotGen.generate(sageProject,"matcher",AstDOTGeneration::TOPDOWN);
  SgNode* root;
  root=sageProject;
#if 0
  std::cout << "TERM INFO OUTPUT: START\n";
  std::cout << astTermToMultiLineString(root,0);
  std::cout << "TERM INFO OUTPUT: END\n";
#endif
  RoseAst ast(root);
  std::string matchexpression="$Root=SgAssignOp($LHS=SgPntrArrRefExp(SgPntrArrRefExp(SgArrowExp($WORK,$DS),$E1),$E2),$RHS)";
  AstMatching m;
  MatchResult r=m.performMatching(matchexpression,root);
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
    string oldCode0=(*i)["$Root"]->unparseToString();
    string oldCode="/* OLD: "+oldCode0+"; */\n";
    string newCode0=work+" -> "+ds+".set("+e1+","+e2+","+rhs+")";
    string newCode="      /* NEW: */"+newCode0; // ';' is unparsed as part of the statement that contains the assignop
    SgNodeHelper::replaceAstWithString((*i)["$Root"], oldCode+newCode);
    std::cout << std::endl;
    std::string lineCol=SgNodeHelper::sourceLineColumnToString((*i)["$Root"]);
    cout <<"TRANSFORMATION: "<<lineCol<<" OLD:"<<oldCode0<<endl;
    cout <<"TRANSFORMATION: "<<lineCol<<" NEW:"<<newCode0<<endl;
  }
  m.printMarkedLocations();
  m.printMatchOperationsSequence();
  write_file("astterm.txt",AstTerm::astTermToMultiLineString(root,2));
  write_file("astterm.dot",AstTerm::astTermWithNullValuesToDot(root));
  backend(sageProject);
}
