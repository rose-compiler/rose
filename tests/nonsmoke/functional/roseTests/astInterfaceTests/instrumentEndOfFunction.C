/*! \brief  test instrumentation right before the end of a function

*/
#include "rose.h"
#include <iostream>
using namespace SageInterface;
using namespace SageBuilder;

int main (int argc, char *argv[])
{
  SgProject *project = frontend (argc, argv);

  // go to the function body
  SgFunctionDeclaration* mainFunc= findMain(project);
  std::cout<<mainFunc->unparseToString()<<std::endl;
  
  // prepare the function call statement we want to use
  SgBasicBlock* body = mainFunc->get_definition()->get_body();
  SgExprStatement* callStmt1 = buildFunctionCallStmt("call1",
               buildIntType(),buildExprListExp() ,body);  

  // instrument the main function
  int i= instrumentEndOfFunction(mainFunc, callStmt1); 

  std::cout<<"Instrumented "<<i<<" places. "<<std::endl;
  AstTests::runAllTests(project); 
  // translation only
   project->unparse();

  //invoke backend compiler to generate object/binary files
  // return backend (project);
}

