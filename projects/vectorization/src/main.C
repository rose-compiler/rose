/*
  Author: Pei-Hung Lin
  Contact: phlin@cs.umn.edu

  Date Created       : July 16th, 2012
 
  Vectorization
  * input  : C input
  * output : C output with Vector intrinsic functions

*/  

#include "rose.h"
//Array Annotation headers
#include <CPPAstInterface.h>
#include <ArrayAnnot.h>
#include <ArrayRewrite.h>
//Dependence graph headers
#include <AstInterface_ROSE.h>
#include <LoopTransformInterface.h>
#include <AnnotCollect.h>
#include <OperatorAnnotation.h>
#include <LoopTreeDepComp.h>

#include "vectorization.h"
#include "CommandOptions.h"

using namespace std;
using namespace vectorization;

int main( int argc, char * argv[] )
{
  vector<string> argvList(argv, argv+argc);
// Build the AST used by ROSE
  SgProject* project = frontend(argc,argv);
  AstTests::runAllTests(project);   

/* Generate data dependence graph
  ArrayAnnotation* annot = ArrayAnnotation::get_inst(); 
  ArrayInterface array_interface(*annot);
  
  Rose_STL_Container<SgNode*> forLoopList = NodeQuery::querySubTree (project,V_SgForStatement);
  for (Rose_STL_Container<SgNode*>::iterator i = forLoopList.begin(); i != forLoopList.end(); i++)
  {
    SageInterface::forLoopNormalization(isSgForStatement(*i));
    // Prepare AstInterface: implementation and head pointer
    AstInterfaceImpl faImpl_2 = AstInterfaceImpl(*i);
    CPPAstInterface fa(&faImpl_2);
    AstNodePtr head = AstNodePtrImpl(*i);
    //AstNodePtr head = AstNodePtrImpl(body);
    fa.SetRoot(head);

    LoopTransformInterface::set_astInterface(fa);
    LoopTransformInterface::set_arrayInfo(&array_interface);
    LoopTransformInterface::set_aliasInfo(&array_interface);
    LoopTransformInterface::set_sideEffectInfo(annot);
    LoopTreeDepCompCreate* comp = new LoopTreeDepCompCreate(head);// TODO when to release this?
    comp->DumpDep();
  }
*/
  addHeaderFile(project,argvList);

  Rose_STL_Container<SgNode*> forLoopList = NodeQuery::querySubTree (project,V_SgForStatement);
  for (Rose_STL_Container<SgNode*>::iterator i = forLoopList.begin(); i != forLoopList.end(); i++)
  {
    SageInterface::forLoopNormalization(isSgForStatement(*i));
  }
  

  //generateDOT(*project);
  generateAstGraph(project,80000);
 

  // Output preprocessed source file.
  unparseProject(project);
  return 0;
}

