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
  Rose::global_options.set_frontend_notes(false);
  Rose::global_options.set_frontend_warnings(false);
  Rose::global_options.set_backend_warnings(false);

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
  
  write_file("astterm.txt",AstTerm::astTermToMultiLineString(root,2));
  write_file("astterm.dot",AstTerm::astTermWithNullValuesToDot(root));
  backend(sageProject);
}
