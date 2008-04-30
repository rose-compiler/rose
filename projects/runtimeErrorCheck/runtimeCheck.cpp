#include <rose.h>
#include <string>

#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <vector>
#include <string>
#include <iostream>
#include <dlfcn.h>

#include <sys/stat.h>
#include <sys/types.h>

#include "RuntimeInstrumentation.h"

using namespace std;

bool containsArgument(int argc, char** argv, char* pattern) {
  for (int i = 2; i < argc ; i++) {
    if (!strcmp(argv[i], pattern)) {
      return true;
    }
  }
  return false;
}


int main(int argc, char** argv) {
#if 0
  if (!containsArgument(argc, argv, "-checkAST") && 
      !containsArgument(argc, argv, "-checkGraph") &&
      !containsArgument(argc, argv, "-printTree") &&
      !containsArgument(argc, argv, "-callgraph") &&
      !containsArgument(argc, argv, "-cfa") &&
      !containsArgument(argc, argv, "-dfa") 
      ) {argc = 1;}

  if (argc < 2) {
    fprintf(stderr, "Usage: %s executableName [OPTIONS]\n", argv[0]);
    cout << "\nOPTIONS: " <<endl;
    cout << "-checkAST             - run all checkers on binary AST. " << endl; 
    cout << "-checkGraph           - run all checkers on dataflow graph. " << endl; 
    cout << "-printTree            - create dot file of AST. " << endl; 
    cout << "-callgraph            - perform callgraph analysis and print callgraph.dot file. " << endl; 
    cout << "-cfa                  - perform control flow analysis and print cfg.dot file. " << endl; 
    cout << "-dfa                  - perform dataflow flow analysis and print dfg.dot file. " << endl; 
    cout << "-inter                - perform dataflow analysis interprocedurally (default intraprocedural). " << endl; 
    cout << "-backward             - perform backward analysis (default forward). " << endl; 
    cout << "-gml                  - all graphs (except AST) are saved as gml files (default dot). " << endl; 
    cout << "-mergeedges           - aggregate edges between same nodes. " << endl; 
    cout << "-noedges              - do not print edges into dot or gml file (only nodes). " << endl; 
    return 1;
  }
#endif
  string execName = argv[1];
  
#if 0
  bool interprocedural = false;
  if (containsArgument(argc, argv, "-inter")) {
    interprocedural = true;
  }
#endif

  fprintf(stderr, "Starting runtimeCheck ...\n");
  SgProject* project = frontend(argc,argv);
  ROSE_ASSERT (project != NULL);

  generatePDF(*project);

  RuntimeInstrumentation* runtime = new RuntimeInstrumentation();
  runtime->run(project);

  project->unparse();

  return 0;
}


