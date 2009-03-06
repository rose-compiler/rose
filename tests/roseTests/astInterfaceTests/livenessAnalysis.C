/*
Test SageInterface's liveness analysis 
by Liao, 3/5/2009
*/
#include "rose.h"
#include <iostream>
#include <set>

using namespace std;

int main(int argc, char * argv[])
{
  SgProject *project = frontend (argc, argv);
  // call liveness analysis
  LivenessAnalysis* liv = SageInterface::call_liveness_analysis(project,true);

  SgFunctionDeclaration* func = SageInterface::findMain(project);
  ROSE_ASSERT(func != NULL);

  SgBasicBlock* body = func->get_definition()->get_body();

  SgStatement* stmt = SageInterface::getFirstStatement(body);
  ROSE_ASSERT(stmt != NULL);

  if (SageInterface::isCanonicalForLoop(stmt))
  {
    std::set<SgInitializedName*> liveIns, liveOuts;
    std::set<SgInitializedName*>::const_iterator iter; 
    // retrieve liveness analysis results for a loop
    SageInterface::getLiveVariables(liv, isSgForStatement(stmt),liveIns, liveOuts);
    cout<<"Live-in variables are:";
    for (iter=liveIns.begin(); iter!=liveIns.end(); iter++)
    {
      cout<<" "<<(*iter)->unparseToString()<<" ";
    }
    cout<<endl;

    cout<<"Live-out variables are:";
    for (iter=liveOuts.begin(); iter!=liveOuts.end(); iter++)
    {
      cout<<" "<<(*iter)->unparseToString()<<" ";
    }
    cout<<endl;
  }  
  else
    ROSE_ASSERT(false);
  return backend(project);
}

