// Test reduction recognition 
#include "rose.h"
#include <iostream>
#include <set>

using namespace std;

int main(int argc, char * argv[])
{
#ifdef ROSE_BUILD_CPP_LANGUAGE_SUPPORT
  // Initialize and check compatibility. See Rose::initialize
  ROSE_INITIALIZE;

  SgProject *project = frontend (argc, argv);
  //Find main() function
  SgFunctionDeclaration* func = SageInterface::findMain(project);
  ROSE_ASSERT(func != NULL);
  SgBasicBlock* body = func->get_definition()->get_body();

  //Find the first loop
  Rose_STL_Container<SgNode*> node_list = NodeQuery::querySubTree(body,V_SgForStatement);
  SgForStatement* loop = isSgForStatement(*(node_list.begin()));
  ROSE_ASSERT(loop != NULL);

  //Collect reduction variables and operations
//  std::set<  std::pair <SgInitializedName*, VariantT> > reductions;
//  std::set<  std::pair <SgInitializedName*, VariantT> >::const_iterator iter;
  std::set< std::pair <SgInitializedName*, OmpSupport::omp_construct_enum> > reductions; 
  std::set< std::pair <SgInitializedName*, OmpSupport::omp_construct_enum> >::const_iterator iter;
  SageInterface::ReductionRecognition(loop, reductions);

  // Show the results
  cout<<"Reduction recognition results:"<<endl;
  for (iter=reductions.begin(); iter!=reductions.end(); iter++)
  {
    std::pair <SgInitializedName*, OmpSupport::omp_construct_enum> item = *iter;
    cout<<"\t variable: "<<item.first->unparseToString()<<"\t operation:"<<OmpSupport::toString(item.second)<<endl;;
  }

  return backend(project);
#else
  std::cerr <<"C preprocessor is not supported in this version of ROSE\n";
#endif
}

