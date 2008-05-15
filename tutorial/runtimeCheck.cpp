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
using namespace SageInterface;

bool containsArgument(int argc, char** argv, char* pattern) {
  for (int i = 2; i < argc ; i++) {
    if (!strcmp(argv[i], pattern)) {
      return true;
    }
  }
  return false;
}


int main(int argc, char** argv) {

  if (!containsArgument(argc, argv, "-run") && 
      !containsArgument(argc, argv, "-pdf")
      ) {argc = 1;}

  if (argc < 2) {
    fprintf(stderr, "Usage: %s executableName [OPTIONS]\n", argv[0]);
    cout << "\nOPTIONS: " <<endl;
    cout << "-run             - plain run. " << endl; 
    cout << "-pdf             - print pdfAST. " << endl; 
    return 1;
  }

  string execName = argv[1];
  

  bool printPDF = false;

  if (containsArgument(argc, argv, "-pdf")) {
    fprintf(stderr, "Generating pdf file for input code only ...\n");
    printPDF = true;
  }


  SgProject* project  = frontend(argc,argv);
  ROSE_ASSERT (project != NULL);
  std::string filename = argv[1];

  if (printPDF) {
    SgGlobal* globalScope = getFirstGlobalScope(isSgProject(project));
    AstPDFGeneration pdf;
    getEnclosingFileNode(globalScope)->display("generating : ");
    //  cerr << " generating : " << getEnclosingFileNode(globalScope)->display("").str() << endl;
    //pdf.generateWithinFile(getEnclosingFileNode(globalScope));
    pdf.generateInputFiles(project);
  }

  fprintf(stderr, "Starting runtimeCheck ...\n");

  //generatePDF(*project);

  RuntimeInstrumentation* runtime = new RuntimeInstrumentation();
  runtime->run(project);

  fprintf(stderr, "Starting backend ...\n");
  //  project->unparse();
  return backend(project);

  //  return 0;
}


