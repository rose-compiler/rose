/*
test code
by Liao, 9/3/2014
*/
#include "rose.h"
#include "transformationTracking.h"
#include <iostream>
using namespace std;
bool debug = false;
class visitorTraversal : public AstSimpleProcessing
{
  protected:
    void virtual visit (SgNode* n)
    {
      if (isSgFunctionDeclaration(n)!=NULL){
        SgFunctionDeclaration* func = isSgFunctionDeclaration(n);  
        ROSE_ASSERT(func != NULL);
        if (func->get_definition() == NULL) return;

        // TODO: skip things from headers. 
        // skip compiler generated codes, mostly from template headers
        if (func->get_file_info()->isCompilerGenerated())
        {
          return;
        }

        SgBasicBlock* body = func->get_definition()->get_body();
        if (body == NULL) return; 
        // Prepare the function body: ensure body basic block for while, for, if, etc.
         SageInterface::changeAllBodiesToBlocks (body, false);

        Rose_STL_Container<SgNode*> var_decls= NodeQuery::querySubTree(body,V_SgVariableDeclaration);
        if (debug )
          cout<<"Number of declarations to be considered = "<<var_decls.size()<<endl;
        for (size_t i=0; i< var_decls.size(); i++)
        {
          SgVariableDeclaration* decl = isSgVariableDeclaration(var_decls[i]);
          ROSE_ASSERT(decl!= NULL);
          bool result=false;
          if (SageInterface::isStatic(decl))
          {
            if (debug)
              cout<<"skipping a static variable declaration .."<<endl;
          }
          else
            result = SageInterface::moveDeclarationToInnermostScope(decl, debug);
        }
      } // end if
    } // end visit()
};

int main(int argc, char * argv[])

{
  vector <string> argvList (argv, argv + argc);
  // pass -rose:debug to turn on debugging mode
  if (CommandlineProcessing::isOption (argvList,"-rose:debug","",true))
  {
    debug = true;
    cout<<"Turing on debugging model..."<<endl;
  }
  SgProject *project = frontend (argvList);

  SgFilePtrList file_ptr_list = project->get_fileList();
  visitorTraversal exampleTraversal;
  for (size_t i = 0; i<file_ptr_list.size(); i++)
  {
    SgFile* cur_file = file_ptr_list[i];
    SgSourceFile* s_file = isSgSourceFile(cur_file);
    if (s_file != NULL)
    {
      //exampleTraversal.traverseInputFiles(project,preorder);
       exampleTraversal.traverseWithinFile(s_file, preorder);
    }
  }
 // run all tests
  AstTests::runAllTests(project);
  return backend(project);
}

