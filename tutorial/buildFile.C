/*! \brief  Demonstrate how to build a new file
*/
#include "rose.h"
#include <iostream>

using namespace std;

using namespace SageInterface;
using namespace SageBuilder;

int main (int argc, char *argv[])
   {
     SgProject *project = frontend (argc, argv);

     printf ("Calling buildFile function() \n");

  // Calling: SgFile* buildFile(const std::string& inputFileName,const std::string& outputFileName, SgProject* project=NULL);
     SgSourceFile* sourceFile = buildSourceFile("my_header_out.h",project);

     printf ("DONE: Calling buildFile function() \n");

     SgGlobal* globalScope = sourceFile->get_globalScope();
     ROSE_ASSERT(globalScope != NULL);

  // SgFunctionDeclaration* buildDefiningFunctionDeclaration(const SgName& name, SgType* return_type, SgFunctionParameterList* parameter_list, SgScopeStatement* scope = NULL);
     string functionName = "foobar";
     SgType* return_type = buildVoidType();
     SgFunctionParameterList* parameter_list = NULL;
     SgFunctionDeclaration* functionDeclaration = buildDefiningFunctionDeclaration(functionName,return_type,parameter_list,globalScope);
     ROSE_ASSERT(functionDeclaration != NULL);


#if 0
  // Find all function definitions we want to instrument
     std::vector<SgNode* > funcDefList = NodeQuery::querySubTree (project, V_SgFunctionDefinition);

     std::vector<SgNode*>::iterator iter;
     for (iter = funcDefList.begin(); iter!= funcDefList.end(); iter++)
        {
          SgFunctionDefinition* cur_def = isSgFunctionDefinition(*iter);
          ROSE_ASSERT(cur_def);
          SgBasicBlock* body = cur_def->get_body();
       // Build the call statement for each place
          SgExprStatement* callStmt1 = buildFunctionCallStmt("call1",buildIntType(),buildExprListExp() ,body);  

       // instrument the function
          int i= instrumentEndOfFunction(cur_def->get_declaration(), callStmt1); 
          std::cout<<"Instrumented "<<i<<" places. "<<std::endl;
        }
#endif

     AstTests::runAllTests(project); 
  // translation only
     project->unparse();
   }

