#include "vectorization.h"
#include "normalization.h"
#include "CommandOptions.h"
#include "AstInterface.h"
#include "AstInterface_ROSE.h"

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
  We perform the node query on a forStatement.  If the result has only one forStatement from the query,
  this imply it's a innermost loop.
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

/******************************************************************************************************************************/
/*
  VF, passed into this function, is the vector factor, and usually represents the SIMD width.
  In single-precision SSE, VF will be 4.

  Perform loop strip mining transformations on loops. Strip mining splits a loop into two nested loops. 
  
  for (i=0; i<Num; i++) {
    ...
  }
    
  loop strip mining will transform the loop as if the user had written:
  for (i=0; i < Num; i+=VF) { 
    for (j=i; j < min(Num, i+VF); j++) 
      ...
    }
  }

*/
/******************************************************************************************************************************/
void vectorization::stripmineLoop(SgForStatement* forStatement, int VF)
{
  // Fetch all information from original forStatement
  SgInitializedName* indexVariable = getLoopIndexVariable(forStatement);
  ROSE_ASSERT(indexVariable);

  SgBinaryOp* testExpression = isSgBinaryOp(forStatement->get_test_expr());
  ROSE_ASSERT(testExpression);

  SgExpression* increment = forStatement->get_increment();
  ROSE_ASSERT(increment);

  SgBasicBlock* loopBody = isSgBasicBlock(forStatement->get_loop_body());
  ROSE_ASSERT(loopBody);

  SgScopeStatement* scope = forStatement->get_scope();
  ROSE_ASSERT(scope);

  /* 
    Create the min function for strip mining.
    NOTE: CreateFunction is defined in AstInterface. 
        SgNode*  AstInterfaceImpl::CreateFunction( string name, int numOfParameters)

  */

  AstInterfaceImpl faImpl = AstInterfaceImpl(forStatement);
  SgNode* minFunction = faImpl.CreateFunction("min",2);

  /* 
    Create outerloop for the stripmined loop.  
    If the index name is i, the outerput loop index is i_stripminedLoop_#, with the line number for surfix.
  */
  int forStatementLineNumber = forStatement->get_file_info()->get_line();
  ostringstream convert;
  convert << forStatementLineNumber;
  SgName innerLoopIndex =  SgName(indexVariable->get_name().getString() +"_stripminedLoop_"+ convert.str());


  // Change the loop stride to be VF for the original loop, which will be the outer loop after strip-mining.
  SgExpression* newIncrementExpression = buildAssignOp(buildVarRefExp(indexVariable), 
                                                    buildAddOp(buildVarRefExp(indexVariable),
                                                    buildIntVal(VF)));
  replaceExpression(increment,newIncrementExpression);

  // Create the index initialization for the inner loop index.
  SgAssignInitializer* assignInitializer = buildAssignInitializer(buildVarRefExp(indexVariable,scope),buildIntType()); 
  SgVariableDeclaration*   innerLoopInit = buildVariableDeclaration(innerLoopIndex,buildIntType(),assignInitializer, scope);
  SgVariableSymbol* innerLoopIndexSymbol = isSgVariableSymbol(innerLoopInit->get_decl_item(innerLoopIndex)->get_symbol_from_symbol_table());

  // The inner loop should have stride distance 1.
  SgExpression* innerLoopIncrement = buildPlusPlusOp(buildVarRefExp(innerLoopIndex,scope),SgUnaryOp::postfix); 

  /*
    Create the test expression for inner loop.
    The upper bound of the inner loop becomes result of a comparison of min(inner_index+VF, outer_upper_bound)
    
  */
  SgExprListExp* argumentList = buildExprListExp(buildAddOp(buildVarRefExp(indexVariable,scope),buildIntVal(VF)),
                                                 testExpression->get_rhs_operand());
  SgFunctionCallExp* innerLoopUpperbound = buildFunctionCallExp("min",
                                                                buildIntType(),
                                                                argumentList,
                                                                scope);
  SgExprStatement* innerLoopTestStmt = buildExprStatement(buildLessThanOp(buildVarRefExp(innerLoopIndex,scope),
                                                          innerLoopUpperbound));

  /*
    The original loop body becomes the inner loop body.  However, the index name has to be replaced by the inner
    loop index.
  */
  Rose_STL_Container<SgNode*> varRefs = NodeQuery::querySubTree(loopBody,V_SgVarRefExp);
  for (Rose_STL_Container<SgNode *>::iterator i = varRefs.begin(); i != varRefs.end(); i++)
  {
    SgVarRefExp *vRef = isSgVarRefExp((*i));
    if (vRef->get_symbol()==indexVariable->get_symbol_from_symbol_table())
      vRef->set_symbol(innerLoopIndexSymbol);
  }

  /*
    Moving original loop body into inner loop.
    Create new loop body that contains the inner loop, and then append it into the original loop(outer loop).
  */
  loopBody->set_parent(NULL);
  SgForStatement* innerLoop = buildForStatement(innerLoopInit,innerLoopTestStmt,innerLoopIncrement,loopBody, NULL);
  SgBasicBlock* outerBasicBlock = buildBasicBlock(innerLoop);
  forStatement->set_loop_body(outerBasicBlock);
}
