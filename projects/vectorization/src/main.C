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
using namespace SIMDVectorization;
using namespace SIMDNormalization;
using namespace SIMDAnalysis;
vector<SgForStatement*> loopList;
/* 
  VF is the vector factor, usually is the SIMD width.  
  We set it up here for the ealy stage development.
*/
int VF = 4;

// memory pool traversal for variable declaration
class loopTraversal : public ROSE_VisitorPattern
{
  public:
    void visit(SgForStatement* loop);
};

void loopTraversal::visit(SgForStatement* loop)
{
  loopList.push_back(loop);
}

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
        break;
      }
    default:
      {}
  }
}


void parseSIMDOption(vector<string> & inputCommandLine, vector<string> & argv)
{
  // *******************************************************************
  // phlin (6/25/2012)  Handle SSE/AVX option
  // *******************************************************************
     if ( CommandlineProcessing::isOption(argv,"-m","sse4.2",false) == true )
        {
       // printf ("In build_EDG_CommandLine(): Option -msse4.2 found (compile only)! \n");
          inputCommandLine.push_back("-D__SSE3__");
          inputCommandLine.push_back("-D__SSSE3__");
          inputCommandLine.push_back("-D__SSE4__");
          inputCommandLine.push_back("-D__SSE4_1__");
          inputCommandLine.push_back("-D__SSE4_2__");
        }

     if ( CommandlineProcessing::isOption(argv,"-m","sse4.1",false) == true )
        {
       // printf ("In build_EDG_CommandLine(): Option -msse4.1 found (compile only)! \n");
          inputCommandLine.push_back("-D__SSE3__");
          inputCommandLine.push_back("-D__SSSE3__");
          inputCommandLine.push_back("-D__SSE4__");
          inputCommandLine.push_back("-D__SSE4_1__");
        }

     if ( CommandlineProcessing::isOption(argv,"-m","sse4",false) == true )
        {
       // printf ("In build_EDG_CommandLine(): Option -msse4 found (compile only)! \n");
          inputCommandLine.push_back("-D__SSE3__");
          inputCommandLine.push_back("-D__SSSE3__");
          inputCommandLine.push_back("-D__SSE4__");
        }

     if ( CommandlineProcessing::isOption(argv,"-m","sse3",false) == true )
        {
       // printf ("In build_EDG_CommandLine(): Option -msse3 found (compile only)! \n");
          inputCommandLine.push_back("-D__SSE3__");
          inputCommandLine.push_back("-D__SSSE3__");
        }

     if ( CommandlineProcessing::isOption(argv,"-m","avx",false) == true )
        {
       // AVX doesn't need any special option here.
       // printf ("In build_EDG_CommandLine(): Option -mavx found (compile only)! \n");
        }

}


void build_SIMD_CommandLine(vector<string> & inputCommandLine, vector<string> & argv)
{
  // *******************************************************************
  // phlin (6/25/2012)  Handle SSE/AVX option
  // *******************************************************************
     if ( CommandlineProcessing::isOption(argv,"-m","sse4.2",false) == true )
        {
       // printf ("In build_EDG_CommandLine(): Option -msse4.2 found (compile only)! \n");
          inputCommandLine.push_back("-D__SSE3__");
          inputCommandLine.push_back("-D__SSSE3__");
          inputCommandLine.push_back("-D__SSE4__");
          inputCommandLine.push_back("-D__SSE4_1__");
          inputCommandLine.push_back("-D__SSE4_2__");
        }

     if ( CommandlineProcessing::isOption(argv,"-m","sse4.1",false) == true )
        {
       // printf ("In build_EDG_CommandLine(): Option -msse4.1 found (compile only)! \n");
          inputCommandLine.push_back("-D__SSE3__");
          inputCommandLine.push_back("-D__SSSE3__");
          inputCommandLine.push_back("-D__SSE4__");
          inputCommandLine.push_back("-D__SSE4_1__");
        }

     if ( CommandlineProcessing::isOption(argv,"-m","sse4",false) == true )
        {
       // printf ("In build_EDG_CommandLine(): Option -msse4 found (compile only)! \n");
          inputCommandLine.push_back("-D__SSE3__");
          inputCommandLine.push_back("-D__SSSE3__");
          inputCommandLine.push_back("-D__SSE4__");
        }

     if ( CommandlineProcessing::isOption(argv,"-m","sse3",false) == true )
        {
       // printf ("In build_EDG_CommandLine(): Option -msse3 found (compile only)! \n");
          inputCommandLine.push_back("-D__SSE3__");
          inputCommandLine.push_back("-D__SSSE3__");
        }

     if ( CommandlineProcessing::isOption(argv,"-m","avx",false) == true )
        {
       // AVX doesn't need any special option here.
       // printf ("In build_EDG_CommandLine(): Option -mavx found (compile only)! \n");
        }

}


int main( int argc, char * argv[] )
{
  Rose_STL_Container<std::string> localCopy_argv = CommandlineProcessing::generateArgListFromArgcArgv(argc, argv);
  vector<string> argList = localCopy_argv;
  build_SIMD_CommandLine(argList,localCopy_argv);
  int newArgc;
  char** newArgv = NULL;
  CommandlineProcessing::generateArgcArgvFromList(argList,newArgc, newArgv);
// Build the AST used by ROSE
  SgProject* project = frontend(newArgc,newArgv);
  AstTests::runAllTests(project);   
  if (SgProject::get_verbose() > 2)
    generateAstGraph(project,8000,"_orig");

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
  normalizeExpression(project);
// Add required header files for ROSE vectorization framework.
  addHeaderFile(project);

/*
  This stage includes loop normalization (implemented in mid-end), and loop strip-mining.
*/ 
  transformTraversal loopTransformation;
  loopTransformation.traverseInputFiles(project,postorder);
  
  loopTraversal translateLoop;
  traverseMemoryPoolVisitorPattern(translateLoop);
  for(vector<SgForStatement*>::iterator i=loopList.begin(); i!=loopList.end(); ++i)
  {
    SgForStatement* forStatement = isSgForStatement(*i);
    SageInterface::forLoopNormalization(forStatement);
    if(isInnermostLoop(forStatement) && isStrideOneLoop(forStatement)){
      //stripmineLoop(forStatement,4);
      updateLoopIteration(forStatement,VF);
      normalizeCompoundAssignOp(forStatement);
    }
  }
//  defuse = new DefUseAnalysis(project);
//  defuse->run(false);
    generateAstGraph(project,8000,"_tmp");

// clear the vector, and redo the memory traversal to collect new loop list
  loopList.clear();
/*
  This stage translates the operators to the intrinsic function calls. 
*/ 
  traverseMemoryPoolVisitorPattern(translateLoop);
  for(vector<SgForStatement*>::iterator i=loopList.begin(); i!=loopList.end(); ++i)
  {
    SgForStatement* forStatement = isSgForStatement(*i);
    if(isInnermostLoop(forStatement)){
      SgStatement* loopBody = forStatement->get_loop_body();
      ROSE_ASSERT(loopBody);

      std::set< SgInitializedName *> liveIns, liveOuts;
      LivenessAnalysis * liv = SageInterface::call_liveness_analysis (SageInterface::getProject());
      SageInterface::getLiveVariables(liv, forStatement, liveIns, liveOuts);

      scalarVariableConversion(forStatement, liveIns, liveOuts);

      translateMultiplyAccumulateOperation(loopBody);
      vectorizeBinaryOp(loopBody);
      vectorizeUnaryOp(loopBody);
      vectorizeConditionalStmt(loopBody);
    }
  }

  if (SgProject::get_verbose() > 2)
    generateAstGraph(project,80000);

  //generateDOT(*project);
 

  // Output preprocessed source file.
  unparseProject(project);
  //return backend(project); 
  return 0;
}

