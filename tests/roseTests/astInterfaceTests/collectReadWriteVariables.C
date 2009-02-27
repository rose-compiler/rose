/*
 *  * Call Qing's side effect analysis
 *  by Liao, 2/26/2009
 *  */
#include "rose.h"
#include <iostream>
#include <vector>
using namespace std;
int main(int argc, char * argv[])
{
  SgProject *project = frontend (argc, argv);
  SgFunctionDeclaration* func = SageInterface::findMain(project);
  ROSE_ASSERT(func != NULL);

  vector<SgInitializedName*> readNames, writeNames;
  if (!SageInterface::collectReadWriteVariables(func,readNames,writeNames))
  {
    ROSE_ASSERT(false);
  }
  // i, j, b , k, argc, argv
  ROSE_ASSERT(readNames.size() ==6);
  vector<SgInitializedName*>::iterator iter;
  for (iter=readNames.begin();iter!=readNames.end();iter++)
    cout<<"read variable:"<<(*iter)->unparseToString()<<endl;
  /* i, j, a, b
   */
  ROSE_ASSERT(writeNames.size() ==4);
  for (iter=writeNames.begin();iter!=writeNames.end();iter++)
    cout<<"written variable:"<<(*iter)->unparseToString()<<endl;
  // run all tests
  AstTests::runAllTests(project);

  // Generate source code from AST and call the vendor's compiler
  return backend(project);
}

