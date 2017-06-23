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

  int statementTransformations=0;

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
  RoseAst ast(root);
  std::string matchexpression="$Assign=SgAddOp|$C=SgBinaryOp(_,_)|$AddPlusAssign=SgPlusAssignOp(..)";
  AstMatching m;
  MatchResult r=m.performMatching(matchexpression,root);
  // print result in readable form for demo purposes
  std::cout << "Number of matched patterns with bound variables: " << r.size() << std::endl;
  for(MatchResult::iterator i=r.begin();i!=r.end();++i) {
    statementTransformations++;
    std::cout << "---------------------------------------------------------"<<endl;
    std::cout << "MATCH-Assign: \n"; 
    //SgNode* n=(*i)["X"];
    for(SingleMatchVarBindings::iterator vars_iter=(*i).begin();vars_iter!=(*i).end();++vars_iter) {
      SgNode* matchedTerm=(*vars_iter).second;
      std::cout << "  Assign: " << (*vars_iter).first << "=" << AstTerm::astTermWithNullValuesToString(matchedTerm) << " @" << matchedTerm << std::endl;
    }
  }
  m.printMarkedLocations();
  m.printMatchOperationsSequence();
  
  backend(sageProject);
}
