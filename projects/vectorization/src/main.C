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
std::map<SgExprStatement*, vector<SgStatement*> > insertList;
std::map<std::string, std::string> constantValMap;
/* 
  VF is the vector factor, usually is the SIMD width.  
  We set it up here for the ealy stage development.
*/
int VF = 4;

bool isSIMDDirectiveAttached(SgStatement* stmt)
{
  SgPragmaDeclaration* pragmaStmt = isSgPragmaDeclaration(SageInterface::getPreviousStatement(stmt));
  if(pragmaStmt != NULL)
  {
    SgPragma* pragma = isSgPragma(pragmaStmt->get_pragma());
    ROSE_ASSERT(pragma); 
    if (SgProject::get_verbose() > 2)
      cout << "pragma: " << pragma->get_pragma() << endl;

    if(pragma->get_pragma().find("SIMD") != string::npos)
      return true;
  }
  return false;
}

// memory pool traversal for variable declaration
class loopTraversal : public ROSE_VisitorPattern
{
  public:
    void visit(SgForStatement* loop);
};

void loopTraversal::visit(SgForStatement* loop)
{
  if(isSIMDDirectiveAttached(loop))
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

class vectorizeLoopTraversal : public AstSimpleProcessing
{
  public:
    virtual void visit(SgNode* n);
};

void vectorizeLoopTraversal::visit(SgNode* n)
{
  switch(n->variantT())
  {
      case V_SgAddOp:
      case V_SgSubtractOp:
      case V_SgMultiplyOp:
      case V_SgDivideOp:
      case V_SgAssignOp:
      case V_SgBitAndOp:
      case V_SgBitOrOp:
      case V_SgBitXorOp:
      case V_SgEqualityOp:
      case V_SgGreaterOrEqualOp:
      case V_SgGreaterThanOp:
      case V_SgLessOrEqualOp:
      case V_SgLessThanOp:
      case V_SgNotEqualOp:
      case V_SgCompoundAssignOp:
      case V_SgPntrArrRefExp:
      case V_SgAndOp:
      case V_SgExponentiationOp:
      {
        SgBinaryOp* binaryOp = isSgBinaryOp(n);
        vectorizeBinaryOp(binaryOp);
        break;
      }
      case V_SgMinusOp:
      case V_SgUnaryAddOp:
      case V_SgMinusMinusOp:
      case V_SgPlusPlusOp:
      case V_SgNotOp:
      case V_SgAddressOfOp:
      case V_SgBitComplementOp:
      case V_SgCastExp:
      case V_SgConjugateOp:
      case V_SgExpressionRoot:
      case V_SgPointerDerefExp:
      case V_SgRealPartOp:
      case V_SgThrowOp:
      case V_SgUserDefinedUnaryOp:
      {
        SgUnaryOp* unaryOp = isSgUnaryOp(n);
        vectorizeUnaryOp(unaryOp);
        break;
      }
    case V_SgIfStmt:
      {
        SgIfStmt* ifStmt = isSgIfStmt(n);
        vectorizeConditionalStmt(ifStmt);
        break;
      }
    case V_SgFunctionCallExp:
      {
        SgFunctionCallExp* functionCallExp = isSgFunctionCallExp(n);
        vectorizeFunctionCall(functionCallExp);
        break;
      }
    default:
      {}
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
      updateLoopIteration(forStatement,VF);
      normalizeCompoundAssignOp(forStatement);
    }
  }
//  defuse = new DefUseAnalysis(project);
//  defuse->run(false);

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
  
      vectorizeLoopTraversal innerLoopTransformation;
      innerLoopTransformation.traverse(loopBody,postorder);
    }
  }

  /*
    This map contains the translated true and false statements for the if statement inside vector loop.  
    We insert these statements here to avoid the issue cause by post-order transeversal.
  */
  for(map<SgExprStatement*, vector<SgStatement*> >::iterator i=insertList.begin(); i != insertList.end(); ++i)
  {
    SageInterface::insertStatementListAfter((*i).first, (*i).second);
  }

  if (SgProject::get_verbose() > 2)
    generateAstGraph(project,80000);

  //generateDOT(*project);
 

  // Output preprocessed source file.
  unparseProject(project);
  //return backend(project); 
  return 0;
}

