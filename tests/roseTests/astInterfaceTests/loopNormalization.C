/*
test code
by Liao, 2/26/2009
*/
#include "rose.h"
#include "transformationTracking.h"
#include <iostream>
using namespace std;

int main(int argc, char * argv[])

{
  SgProject *project = frontend (argc, argv);
  AstDOTGeneration astdotgen;

  // register IDs and file info
  cout<<"Turning on transformation tracking ... "<<endl;
  TransformationTracking::registerAstSubtreeIds (project);
  astdotgen.generate(project,AstDOTGeneration::TOPDOWNBOTTOMUP, "v1");

  SgFunctionDeclaration* func = SageInterface::findMain(project);
  ROSE_ASSERT(func != NULL);
  SgBasicBlock* body = func->get_definition()->get_body();
  ROSE_ASSERT(body!= NULL);
  Rose_STL_Container<SgNode*> loops = NodeQuery::querySubTree(body,V_SgForStatement);

  for (size_t i=0; i< loops.size(); i++)
  {
    SgForStatement * cloop = isSgForStatement(loops[i]);
    ROSE_ASSERT(cloop != NULL);
    bool result=false;
    result = SageInterface::forLoopNormalization(cloop, false);
    if (i == loops.size() -1) // test File info retrieval: erase file info here first
      SageInterface::setSourcePositionForTransformation(cloop);
    ROSE_ASSERT(result != false);
  }
  // run all tests
  AstTests::runAllTests(project);

  TransformationTracking::registerAstSubtreeIds (project);
  astdotgen.generate(project,AstDOTGeneration::TOPDOWNBOTTOMUP, "v2");

#if 0
  // test File info retrieval 
  // iterate all node ids 
  for (int i =1; i<TransformationTracking::getNextId(); i++)
  {
    std::pair<Sg_File_Info*, Sg_File_Info*> info_pair = TransformationTracking::getFileInfo(i);
    Sg_File_Info* start_info = info_pair.first;
    Sg_File_Info* end_info = info_pair.second;
    cout<<"====>>Node :"<<i<<endl;
    if (start_info != NULL)
      {start_info->display();}
    if (end_info != NULL)
      {end_info->display();}
  }
#endif  
  // Generate source code from AST and call the vendor's compiler
  return backend(project);
}

