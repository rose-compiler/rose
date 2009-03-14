// Test reduction recognition 
#include "rose.h"
#include <iostream>
#include <set>

using namespace std;

int main(int argc, char * argv[])
{
  SgProject *project = frontend (argc, argv);
  //Find main() function
  SgFunctionDeclaration* func = SageInterface::findMain(project);
  ROSE_ASSERT(func != NULL);
  SgBasicBlock* body = func->get_definition()->get_body();

  //Find the first loop
  Rose_STL_Container<SgNode*> node_list = NodeQuery::querySubTree(body,V_SgForStatement);
  SgForStatement* loop = isSgForStatement(*(node_list.begin()));
  ROSE_ASSERT(loop != NULL);

  //Collect reduction variables and opertions
  std::set<  std::pair <SgInitializedName*, VariantT> > reductions;
  std::set<  std::pair <SgInitializedName*, VariantT> >::const_iterator iter;
  SageInterface::ReductionRecognition(loop, reductions);

  // Show the results
  cout<<"Reduction recognition results:"<<endl;
  for (iter=reductions.begin(); iter!=reductions.end(); iter++)
  {
    std::pair <SgInitializedName*, VariantT> item = *iter;
    cout<<"\t variable: "<<item.first->unparseToString()<<"\t operation:"<<getVariantName(item.second)<<endl;;
  }

  return backend(project);
}

