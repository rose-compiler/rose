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

  // If the input file is not present in the current directory then the input file is ignored.
  // It is a bit strange that a filename is required (empty string is not allowed), it also
  // appears that the language specification for the file is taken from the suffix of the 
  // input file. This can be fixed later.
     SgFile* file = buildFile("my_header_in.c","my_header_out.C",project);
     ROSE_ASSERT(file != NULL);

     SgSourceFile* sourceFile = isSgSourceFile(file);
     ROSE_ASSERT(sourceFile != NULL);

     printf ("DONE: Calling buildFile function() \n");

     SgGlobal* globalScope = sourceFile->get_globalScope();
     ROSE_ASSERT(globalScope != NULL);

#if 0
  // SgFunctionDeclaration* buildDefiningFunctionDeclaration(const SgName& name, SgType* return_type, SgFunctionParameterList* parameter_list, SgScopeStatement* scope = NULL);
     string functionName = "foobar";
     SgType* return_type = buildVoidType();
     SgFunctionParameterList* parameter_list = NULL;
     SgFunctionDeclaration* functionDeclaration = buildDefiningFunctionDeclaration(functionName,return_type,parameter_list,globalScope);
     ROSE_ASSERT(functionDeclaration != NULL);
#endif

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

