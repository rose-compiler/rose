#include "rose.h"
#include "MatlabParser.h"
#include "MatlabUnparser.h"

int main(int argc, char *argv[])
{
  SgProject *project = MatlabParser::frontend(argc, argv);

  AstTests::runAllTests(project);

  MatlabUnparser::backend(project);

  generateAstGraph(project, 2000);

  generateDOT(*project);
  
  return 0;
}
