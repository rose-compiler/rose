#include "vectorization.h"
#include "normalization.h"
#include "CommandOptions.h"

using namespace std;
using namespace SageInterface;
using namespace SageBuilder;
using namespace vectorization;
using namespace normalization;

/******************************************************************************************************************************/
/*
  Add the SIMD header files to the output file.  The header file is chosen based on the translation command. 
*/
/******************************************************************************************************************************/

void vectorization::addHeaderFile(SgProject* project,vector<string>&argvList)
{
  Rose_STL_Container<SgNode*> globalList = NodeQuery::querySubTree (project,V_SgGlobal);
  for(Rose_STL_Container<SgNode*>::iterator i = globalList.begin(); i != globalList.end(); i++)
  {
    SgGlobal* global = isSgGlobal(*i);
    ROSE_ASSERT(global);

    SgFile* file = getEnclosingFileNode(global);
    ROSE_ASSERT(file);

    string filename = file->get_sourceFileNameWithoutPath();
    std::cout << "global in filename: " << filename << std::endl;

    SgScopeStatement* scopeStatement = global->get_scope();

    PreprocessingInfo* headerInfo = insertHeader("ROSE_SIMD.h",PreprocessingInfo::before,true,scopeStatement);
    headerInfo->set_file_info(global->get_file_info());
  }
}
/*
void vectorization::addHeaderFile(SgProject* project,vector<string>&argvList)
{
  Rose_STL_Container<SgNode*> globalList = NodeQuery::querySubTree (project,V_SgGlobal);
  for(Rose_STL_Container<SgNode*>::iterator i = globalList.begin(); i != globalList.end(); i++)
  {
    SgGlobal* global = isSgGlobal(*i);
    ROSE_ASSERT(global);

    SgFile* file = getEnclosingFileNode(global);
    ROSE_ASSERT(file);

    string filename = file->get_sourceFileNameWithoutPath();
    std::cout << "global in filename: " << filename << std::endl;

    SgScopeStatement* scopeStatement = global->get_scope();
    string headerString;    

    if (CommandlineProcessing::isOption (argvList,"-m","avx",false))
    {
      std::cout << "insert avx header" << std::endl;
      headerString = "avxintrin.h";
    }
    else if (CommandlineProcessing::isOption (argvList,"-m","sse4.2",false))
    {
      std::cout << "insert sse4.2 header" << std::endl;
      headerString = "nmmintrin.h";
    }
    else if (CommandlineProcessing::isOption (argvList,"-m","sse4.1",false))
    {
      std::cout << "insert sse4.1 header" << std::endl;
      headerString = "smmintrin.h";
    }
    else if (CommandlineProcessing::isOption (argvList,"-m","sse4",false))
    {
      std::cout << "insert sse4 header" << std::endl;
      headerString = "tmmintrin.h";
    }
    else
    {
      std::cout << "insert sse3 header" << std::endl;
      headerString = "xmmintrin.h";
    }
    PreprocessingInfo* headerInfo = insertHeader(headerString,PreprocessingInfo::before,true,scopeStatement);
    headerInfo->set_file_info(global->get_file_info());

  }
}
*/


/******************************************************************************************************************************/
/*
  translate the binary operator to the mapped SIMD intrisic functions
*/
/******************************************************************************************************************************/
void vectorization::translateBinaryOp(SgBinaryOp* binaryOp, SgScopeStatement* scope, SgName name)
{
  SgExpression* lhs = binaryOp->get_lhs_operand();
  SgExpression* rhs = binaryOp->get_rhs_operand();
  lhs->set_parent(NULL);
  rhs->set_parent(NULL);
  SgExprListExp* vectorAddArgs = buildExprListExp(lhs, rhs);
  SgFunctionCallExp* addExp = buildFunctionCallExp(name,binaryOp->get_type(), vectorAddArgs, scope);
  binaryOp->set_lhs_operand(NULL);
  binaryOp->set_rhs_operand(NULL);
  replaceExpression(binaryOp, addExp, false);
}


/******************************************************************************************************************************/
/*
  Search for all the binary operations in the loop body and translate them into SIMD function calls.
*/
/******************************************************************************************************************************/
void vectorization::vectorizeBinaryOp(SgForStatement* forStatement)
{
  SgStatement* loopBody = forStatement->get_loop_body();
  ROSE_ASSERT(loopBody);
  SgScopeStatement* scope = loopBody->get_scope();
  Rose_STL_Container<SgNode*> binaryOpList = NodeQuery::querySubTree (loopBody,V_SgBinaryOp);
  for (Rose_STL_Container<SgNode*>::iterator i = binaryOpList.begin(); i != binaryOpList.end(); i++)
  {
    SgBinaryOp* binaryOp = isSgBinaryOp(*i);
    //std::cout << "replacing SIMD operator in loop:" << binaryOp->variantT() << std::endl;

    SgName functionName; 
    switch(binaryOp->variantT())
    {
      case V_SgAddOp:
        {
          functionName = SgName("SIMD_add");
          translateBinaryOp(binaryOp, scope, functionName);
        }
        break;
      case V_SgSubtractOp:
        {
          functionName = SgName("SIMD_sub");
          translateBinaryOp(binaryOp, scope, functionName);
        }
        break;
      case V_SgMultiplyOp:
        {
          functionName = SgName("SIMD_mul");
          translateBinaryOp(binaryOp, scope, functionName);
        }
        break;
      case V_SgDivideOp:
        {
          functionName = SgName("SIMD_div");
          translateBinaryOp(binaryOp, scope, functionName);
        }
        break;
      default:
        break;
    }
  }
}


/******************************************************************************************************************************/
/*
  Check if the loop is innermostloop.  If yes, it is candidate for SIMD translation.
  Otherwise, it needs more transformation to perform SIMD. 
*/
/******************************************************************************************************************************/
bool vectorization::isInnermostLoop(SgForStatement* forStatement)
{
  Rose_STL_Container<SgNode*> innerLoopList = NodeQuery::querySubTree (forStatement,V_SgForStatement);
  if(innerLoopList.size() == 1){
    return true;
  }
  else{
    return false;
  }
}

