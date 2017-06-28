// Author: Markus Schordan, 2013.
// Example AstMatcher : used for demonstrating and testing the matcher mechanism

#include <iostream>
#include <fstream>
#include "rose.h"
#include "AstTerm.h"
#include "AstMatching.h"
#include "SgNodeHelper.h"
#include <list>
#include <vector>
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
  SgProject* sageProject=frontend (argvList); 
  // Run internal consistency tests on AST
  //AstTests::runAllTests(sageProject);
  //AstDOTGeneration dotGen;
  //dotGen.generate(sageProject,"matcher",AstDOTGeneration::TOPDOWN);
  SgNode* root;
  root=sageProject;
  RoseAst ast(root);

  std::string matchexpression="$CastNode=SgCastExp($CastOpChild)";
  AstMatching m;
  MatchResult r=m.performMatching(matchexpression,root);
  //std::cout << "Number of matched patterns with bound variables: " << r.size() << std::endl;
  list<string> report;
  int statementTransformations=0;
  for(MatchResult::reverse_iterator i=r.rbegin();i!=r.rend();++i) {
    statementTransformations++;
    SgCastExp* castExp=isSgCastExp((*i)["$CastNode"]);
    ROSE_ASSERT(castExp);
    SgExpression* childNode=isSgExpression((*i)["$CastOpChild"]);
    ROSE_ASSERT(childNode);
    if(castExp->isCompilerGenerated()) {
      SgType* castType=castExp->get_type();
      string castTypeString=castType->unparseToString();
      SgType* castedType=childNode->get_type();
      string castedTypeString=castedType->unparseToString();
      string reportLine="compiler generated cast: "
        +SgNodeHelper::sourceLineColumnToString(castExp->get_parent())
        +": "+castTypeString+" <== "+castedTypeString;
      if(castType==castedType) {
        reportLine+=" [ no change in type. ]";
      }
      // line are created in reverse order
      report.push_front(reportLine); 

      string newSourceCode;
      newSourceCode="/* CAST ("+castTypeString+") */";
      newSourceCode+=castExp->unparseToString();
      castExp->unsetCompilerGenerated(); // otherwise it is not replaced
      SgNodeHelper::replaceAstWithString(castExp,newSourceCode);
    }
  }
  for(list<string>::iterator i=report.begin();i!=report.end();++i) {
    cout<<*i<<endl;
  }
  //m.printMarkedLocations();
  //m.printMatchOperationsSequence();
  cout<<"Number of compiler generated casts: "<<statementTransformations<<endl;
  backend(sageProject);
}
