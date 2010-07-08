#include <rose.h>
using namespace SageInterface;

int main(int argc, char** argv)
{
  SgProject* project = frontend(argc, argv);
  AstTests::runAllTests(project);

  //  generateDOT(*project);
  SgProject* copy = deepCopy<SgProject>(project);
  AstTests::runAllTests(copy);

  deepDelete(copy);
  AstTests::runAllTests(project);
//  backend(project);   
  return 0;
}
