/*
Test SageInterface::isCanonicalForLoop()
by Liao, 2/25/2009
*/
#include "rose.h"
#include <iostream>
using namespace std;

int main(int argc, char * argv[])
{
  SgProject *project = frontend (argc, argv);
  SgFunctionDeclaration* func = SageInterface::findMain(project);
  ROSE_ASSERT(func != NULL);

  SgBasicBlock* body = func->get_definition()->get_body();

  SgStatement* stmt = SageInterface::getFirstStatement(body);
  ROSE_ASSERT(stmt != NULL);

  SgInitializedName* ivar=NULL ;
  SgExpression *lb=NULL, *ub=NULL, *step=NULL;
  bool isIncremental; 
  SgStatement* lbody=NULL;
  if (SageInterface::isCanonicalForLoop(stmt,&ivar, &lb, &ub, &step, &lbody, &isIncremental))
  {
    cout<<"Found a canonical for loop: "<<endl;
    cout<<"loop index variable is:"<<ivar->unparseToString()
      <<"\n bound 1 is:"<<lb->unparseToString()
      <<"\n bound 2 is:"<<ub->unparseToString()
      <<"\n step is:"<<step->unparseToString()
      <<"\n body is:"<<lbody->unparseToString()
      <<"\n iteration is incremental:"<<isIncremental
      <<endl;
  }
  return backend(project);
}

