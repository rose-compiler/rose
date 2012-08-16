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
#include "normalization.h"
#include "SIMDAnalysis.h"
#include "CommandOptions.h"

using namespace std;
using namespace vectorization;
using namespace normalization;
using namespace SIMDAnalysis;

DFAnalysis* defuse;
/* 
  VF is the vector factor, usually is the SIMD width.  
  We set it up here for the ealy stage development.
*/
int VF = 4;

class transformTraversal : public AstSimpleProcessing
{
  public:
    virtual void visit(SgNode* n);
};

void transformTraversal::visit(SgNode* n)
{
  switch(n->variantT())
  {
    case V_SgGlobal:
      {
        SgGlobal* global = isSgGlobal(n);
        insertSIMDDataType(global);
      }
      break;
    case V_SgForStatement:
      {
        SgForStatement* forStatement = isSgForStatement(n);
        SageInterface::forLoopNormalization(forStatement);
        if(isInnermostLoop(forStatement) && isStrideOneLoop(forStatement)){
          //stripmineLoop(forStatement,4);
          updateLoopIteration(forStatement,VF);
        }
      }
      break;
    default:
      break;
  }
}

class vectorizeTraversal : public AstSimpleProcessing
{
  public:
    virtual void visit(SgNode* n);
};

void vectorizeTraversal::visit(SgNode* n)
{
  switch(n->variantT())
  {
    case V_SgForStatement:
      {
        SgForStatement* forStatement = isSgForStatement(n);
        if(isInnermostLoop(forStatement)){
//          getDefList(defuse, forStatement->get_loop_body());
//          getUseList(defuse, forStatement->get_loop_body());
          translateMultiplyAccumulateOperation(forStatement);
          vectorizeBinaryOp(forStatement);
        }
      }
      break;
    default:
      break;
  }
}



int main( int argc, char * argv[] )
{
  vector<string> argvList(argv, argv+argc);
// Build the AST used by ROSE
  SgProject* project = frontend(argc,argv);
  AstTests::runAllTests(project);   

  //generateAstGraph(project,8000,"_orig");
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
  
// Normalize the loop to identify FMA operation.
  normalizeExperission(project);
// Add required header files for ROSE vectorization framework.
  addHeaderFile(project,argvList);

/*
  This stage includes loop normalization (implemented in mid-end), and loop strip-mining.
*/ 
  transformTraversal loopTransformation;
  loopTransformation.traverseInputFiles(project,postorder);
  

//  defuse = new DefUseAnalysis(project);
//  defuse->run(false);

/*
  This stage translates the operators to the intrinsic function calls. 
*/ 
  vectorizeTraversal doVectorization;
  doVectorization.traverseInputFiles(project,postorder);

  //generateAstGraph(project,80000);

  //generateDOT(*project);
 

  // Output preprocessed source file.
  //unparseProject(project);
  return backend(project); 
  //return 0;
}

