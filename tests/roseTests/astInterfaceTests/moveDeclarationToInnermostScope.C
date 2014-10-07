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
        SgBasicBlock* body = func->get_definition()->get_body();
        if (body == NULL) return; 
        Rose_STL_Container<SgNode*> var_decls= NodeQuery::querySubTree(body,V_SgVariableDeclaration);
        if (debug )
          cout<<"Number of declarations to be considered = "<<var_decls.size()<<endl;
        for (size_t i=0; i< var_decls.size(); i++)
        {
          SgVariableDeclaration* decl = isSgVariableDeclaration(var_decls[i]);
          ROSE_ASSERT(decl!= NULL);
          bool result=false;
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
  visitorTraversal exampleTraversal;
  exampleTraversal.traverseInputFiles(project,preorder);
#if 0
  SgFunctionDeclaration* func = SageInterface::findMain(project);
  ROSE_ASSERT(func != NULL);
  SgBasicBlock* body = func->get_definition()->get_body();
  ROSE_ASSERT(body!= NULL);
  Rose_STL_Container<SgNode*> var_decls= NodeQuery::querySubTree(body,V_SgVariableDeclaration);
  cout<<"Number of declarations to be considered = "<<var_decls.size()<<endl;
  for (size_t i=0; i< var_decls.size(); i++)
  {
    SgVariableDeclaration* decl = isSgVariableDeclaration(var_decls[i]);
    ROSE_ASSERT(decl!= NULL);
    bool result=false;
    result = SageInterface::moveDeclarationToInnermostScope(decl);
  }
#endif
  // run all tests
  AstTests::runAllTests(project);
  return backend(project);
}

