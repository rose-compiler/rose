#include <rose.h>
#include <stdio.h>
using namespace SageInterface;

int main(int argc, char** argv)
{
  SgProject* project = frontend(argc, argv);
  AstTests::runAllTests(project);

  //  generateDOT(*project);
  SgNode* copy = deepCopy(project);
  deepDelete(copy);

  AstTests::runAllTests(project);
  printf("Test deepdelete finished successfully\n");
//  backend(project);   
  return 0;
}
