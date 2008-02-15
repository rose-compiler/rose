#include "rose.h"
#include "rewrite.h"
#include <iostream>
#include <iomanip>

// DQ (1/1/2006): This is OK if not declared in a header file
using namespace std;

void outputCall(string from, string to) {
  static FILE* file = 0;
  if (!file) {
    file = fopen("/tmp/callgraph.out", "a");
    assert (file);
  }
  fprintf(file, "edge %s %s\n", from.c_str(), to.c_str());
}

class WriteCallgraphVisitor: public AstTopDownProcessing<string> {
  public:
  virtual string evaluateInheritedAttribute(SgNode* n, string parentFunc) {
    if (isSgFunctionDefinition(n)) {
      SgFunctionDefinition* n2 = isSgFunctionDefinition(n);
      return n2->get_declaration()->get_name().str();
    } else if (isSgFunctionRefExp(n)) { 
      // Includes taking function pointers, etc.
      outputCall(parentFunc, 
		 isSgFunctionRefExp(n)->get_symbol()->get_name().str());
      return parentFunc;
    } else {
      return parentFunc;
    }
  }
};

int main (int argc, char* argv[]) {
  // Main Function for default example ROSE Preprocessor
  // This is an example of a preprocessor that can be built with ROSE

  // Build the project object (AST) which we will fill up with multiple files and use as a
  // handle for all processing of the AST(s) associated with one or more source files.
  SgProject sageProject (argc,argv);

  WriteCallgraphVisitor().traverseInputFiles(&sageProject, "");

  return 0;
}
