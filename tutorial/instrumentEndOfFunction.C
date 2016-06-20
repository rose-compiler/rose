/*! \brief  test instrumentation right before the end of a function
*/
#include "rose.h"
#include <iostream>
using namespace SageInterface;
using namespace SageBuilder;

int main (int argc, char *argv[])
{
  // Initialize and check compatibility. See rose::initialize
  ROSE_INITIALIZE;

  SgProject *project = frontend (argc, argv);

  // Find all function definitions we want to instrument
  std::vector<SgNode* > funcDefList = 
    NodeQuery::querySubTree (project, V_SgFunctionDefinition);

 std::vector<SgNode*>::iterator iter;
 for (iter = funcDefList.begin(); iter!= funcDefList.end(); iter++)
 { 
   SgFunctionDefinition* cur_def = isSgFunctionDefinition(*iter);
   ROSE_ASSERT(cur_def);
  SgBasicBlock* body = cur_def->get_body();
  // Build the call statement for each place
  SgExprStatement* callStmt1 = buildFunctionCallStmt("call1",
               buildIntType(),buildExprListExp() ,body);  

  // instrument the function
  int i= instrumentEndOfFunction(cur_def->get_declaration(), callStmt1); 
  std::cout<<"Instrumented "<<i<<" places. "<<std::endl;

 }  // end of instrumentation

  AstTests::runAllTests(project); 
  // translation only
   project->unparse();
}

