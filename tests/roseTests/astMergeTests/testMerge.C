
#include "rose.h"

// #include "AstMerge.h"
// #include "MergeUtils.h"
// #include <map>

// DQ (1/1/2006): This is OK if not declared in a header file
using namespace std;

void createDOT(SgProject * root, string postfix = "") {
  AstDOTGeneration astdotgen;
  astdotgen.generate(root);

  if (postfix != "") {
    postfix = "." + postfix;
  }

// list<string> files = root->getFileNames();
// list<string> files = root->getFileNamesWithoutPath();
// for (list<string>::iterator i = files.begin(); i != files.end(); i++) {
   for (int i = 0; i < root->numberOfFiles(); i++) {
 // string filename = *i;
    string workingDirectory = rose::getWorkingDirectory();
    string filename =  workingDirectory + string("/") + rose::utility_stripPathFromFileName(root->get_file(i).getFileName());

    printf ("filename = %s \n",filename.c_str());

    string dotCommand = "dot -Tps -o " + filename + postfix + 
      ".ps " + filename + ".dot -Gsize=\"10.5,8\" -Grotate=90 -Gcenter=1";
    system(dotCommand.c_str());
  }
}


int main(int argc, char * argv[]) {
  
  SgProject * project = frontend(argc, argv);

  AstTests::runAllTests(project);

#if 0
  // DQ (8/1/2005): Commented out because AstMerge is now called within frontend processing
  createDOT(project, "pre");
  AstMerge am;
  am.addAST(project);
  project = am.getAST();
#endif

  AstTests::runAllTests(project);

  vector<string> files = project->getAbsolutePathFileNames();
  printf("Number of files: %zu \n", files.size());

  createDOT(project);

  return 0;

}
