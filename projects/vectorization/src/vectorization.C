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
  Add the SIMD header files to the output file.  
  Inside the ROSE_SIMD.h, the header file includes the required header files for different architecture. 
  This mapping process depends on the translator options provided by user.
*/
/******************************************************************************************************************************/

void vectorization::addHeaderFile(SgProject* project,vector<string>&argvList)
{
  Rose_STL_Container<SgNode*> globalList = NodeQuery::querySubTree (project,V_SgGlobal);
  for(Rose_STL_Container<SgNode*>::iterator i = globalList.begin(); i != globalList.end(); i++)
  {
    SgGlobal* global = isSgGlobal(*i);
    ROSE_ASSERT(global);

    //SgFile* file = getEnclosingFileNode(global);
    //ROSE_ASSERT(file);
    //string filename = file->get_sourceFileNameWithoutPath();

    //SgScopeStatement* scopeStatement = global->get_scope();

    // Insert this SIMD header file before all other headers
    PreprocessingInfo* headerInfo = insertHeader("ROSE_SIMD.h",PreprocessingInfo::after,false,global);
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
  SgFunctionCallExp* SIMDExp = buildFunctionCallExp(name,binaryOp->get_type(), vectorAddArgs, scope);
  binaryOp->set_lhs_operand(NULL);
  binaryOp->set_rhs_operand(NULL);
  replaceExpression(binaryOp, SIMDExp, false);

  // convert the scalar operand to SIMD data type
  translateOperand(lhs);  
  translateOperand(rhs);  
}



/******************************************************************************************************************************/
/*
  get the mapped SIMD data type from the scalar data type:
  float => __SIMD ()  

  TODO: Do we really need this scope as argument?
        Now we rely on this scope to look up the declared SIMD data type, which should be in global scope.
*/
/******************************************************************************************************************************/
SgType* vectorization::getSIMDType(SgType* variableType, SgScopeStatement* scope)
{
  SgType* returnSIMDType;
  switch(variableType->variantT())
  {
    case V_SgTypeDouble:
      {
        returnSIMDType = lookupTypedefSymbolInParentScopes("__SIMDd",scope)->get_type();
      }
      break;
    case V_SgTypeInt:
      {
        returnSIMDType = lookupTypedefSymbolInParentScopes("__SIMDi",scope)->get_type();
      }
      break;
    default:
      {
        // TODO: This default case might cause problem.  Need more consideration!!
        returnSIMDType = lookupTypedefSymbolInParentScopes("__SIMD",scope)->get_type();
      }
      break;
  }
  return returnSIMDType;
}



/******************************************************************************************************************************/
/*
  If operand is SgVarRefExp or SgPntrArrRefExp, then we translate it into SIMD data type.
  If the operand is scalar variable, we promote it to the SIMD data type.
  If the operand is pointer reference, it implies an array reference,
  we substitute the operand to a SIMD data type pointer.
*/
/******************************************************************************************************************************/
void vectorization::translateOperand(SgExpression* operand)
{
  SgType* SIMDType = NULL;
  switch(operand->variantT())
  {
    case V_SgVarRefExp:
      {
        SgVarRefExp* varRefExp = isSgVarRefExp(operand);
        ROSE_ASSERT(varRefExp);
        SgVariableSymbol* variableSymbol = varRefExp->get_symbol();
        ROSE_ASSERT(variableSymbol);
        SgDeclarationStatement* scalarDeclarationStmt = variableSymbol->get_declaration()->get_declaration();
        ROSE_ASSERT(scalarDeclarationStmt);

        string scalarName = varRefExp->get_symbol()->get_name().getString();
        if (SgProject::get_verbose() > 2)
          std::cout << "scalar:" << scalarName << std::endl;
        // Create new SIMD variable that stores the same scalar value in all its data elements
        string SIMDName = scalarName + "_SIMD";

        // If the new SIMD variable isn't declared, then we create it right after the scalar varialbe declaration
        if(lookupSymbolInParentScopes(SIMDName,getEnclosingFunctionDefinition(operand)) == NULL)
        {
          SIMDType = getSIMDType(variableSymbol->get_type(),getEnclosingFunctionDefinition(operand)); 
          SgVariableDeclaration* SIMDDeclarationStmt = buildVariableDeclaration(SIMDName,SIMDType,NULL,getEnclosingFunctionDefinition(operand));        
          insertStatement(scalarDeclarationStmt,SIMDDeclarationStmt,false,true);
        }
        SgVarRefExp* newOperand = buildVarRefExp(SIMDName, getEnclosingFunctionDefinition(operand));
        replaceExpression(operand, newOperand, false);
      }
      break;
    case V_SgPntrArrRefExp:
      {
        SgPntrArrRefExp* pntrArrRefExp = isSgPntrArrRefExp(operand);
        SgVarRefExp* arrayRefExp  = isSgVarRefExp(pntrArrRefExp->get_lhs_operand());
        SgVariableSymbol* arraySymbol = arrayRefExp->get_symbol();
        SgDeclarationStatement* arrayDeclarationStmt = arraySymbol->get_declaration()->get_declaration();
        string arrayName = arrayRefExp->get_symbol()->get_name().getString();
        if (SgProject::get_verbose() > 2)
          std::cout << "array:" << arrayName << std::endl;
        string SIMDName = arrayName + "_SIMD";

        if(lookupSymbolInParentScopes(SIMDName,getEnclosingFunctionDefinition(operand)) == NULL)
        {
          SgArrayType* arrayType = isSgArrayType(arraySymbol->get_type());
          ROSE_ASSERT(arrayType);
          SIMDType = getSIMDType(arrayType->get_base_type(),getEnclosingFunctionDefinition(operand)); 
          SgPointerType* SIMDPointerType = buildPointerType(SIMDType);
          SgVariableDeclaration* SIMDDeclarationStmt = buildVariableDeclaration(SIMDName,SIMDPointerType,NULL,getEnclosingFunctionDefinition(operand));        
          insertStatement(arrayDeclarationStmt,SIMDDeclarationStmt,false,true);

          SgExprStatement* pointerAssignment = buildAssignStatement(buildVarRefExp(SIMDDeclarationStmt),
                                                                    buildCastExp(buildVarRefExp(arraySymbol), SIMDPointerType, SgCastExp::e_C_style_cast ));
          insertStatementAfterLastDeclaration(pointerAssignment,getEnclosingFunctionDefinition(operand));
        }
        SgVarRefExp* newOperand = buildVarRefExp(SIMDName, getEnclosingFunctionDefinition(operand));
        replaceExpression(pntrArrRefExp->get_lhs_operand(), newOperand, false);

      }
      break;
    case V_SgIntVal:
    case V_SgFloatVal:
    case V_SgCastExp:
      {
        SgExprListExp* SIMDAddArgs = buildExprListExp(deepCopy(operand));
        SgFunctionCallExp* SIMDSplats = buildFunctionCallExp("_SIMD_splats",operand->get_type(), SIMDAddArgs, getEnclosingFunctionDefinition(operand));
        replaceExpression(operand, SIMDSplats); 
      }
      break;
    default:
      {
        // If operand isn't varRefExp or pntrArrRefExp, then we do nothing with it.
      }
      break;
  }
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
    ROSE_ASSERT(binaryOp);

    string suffix = getSIMDOpSuffix(binaryOp->get_type());

    switch(binaryOp->variantT())
    {
      case V_SgAddOp:
        {
          translateBinaryOp(binaryOp, scope, "_SIMD_add"+suffix);
        }
        break;
      case V_SgSubtractOp:
        {
          translateBinaryOp(binaryOp, scope, "_SIMD_sub"+suffix);
        }
        break;
      case V_SgMultiplyOp:
        {
          translateBinaryOp(binaryOp, scope, "_SIMD_mul"+suffix);
        }
        break;
      case V_SgDivideOp:
        {
          translateBinaryOp(binaryOp, scope, "_SIMD_div"+suffix);
        }
        break;
      case V_SgAssignOp:
        {
          translateOperand(binaryOp->get_lhs_operand()); 
          translateOperand(binaryOp->get_rhs_operand()); 
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
  faImpl.CreateFunction("min",2);

  /* 
    Create outerloop for the stripmined loop.  
    If the index name is i, the outerput loop index is i_stripminedLoop_#, with the line number for surfix.
  */
  int forStatementLineNumber = forStatement->get_file_info()->get_line();
  ostringstream convert;
  convert << forStatementLineNumber;
  SgName innerLoopIndex =  indexVariable->get_name().getString() +"_strip_"+ convert.str();


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
  SgFunctionCallExp* innerLoopUpperbound = buildFunctionCallExp("min2",
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
  outerBasicBlock->set_parent(forStatement);
  SgPragmaDeclaration *pragmadecl = buildPragmaDeclaration("SIMD");
  prependStatement(pragmadecl,outerBasicBlock);
}

/******************************************************************************************************************************/
/*
  Insert these data types into AST.  
  This step has to be done in the beginning of translation.  
  The later SIMD translation can recognize and use these SIMD data type.
  These data types will map to the typedef in the header file.
*/
/******************************************************************************************************************************/

void vectorization::insertSIMDDataType(SgGlobal* globalScope)
{
  buildOpaqueType("__SIMD",globalScope);
  buildOpaqueType("__SIMDi",globalScope);
  buildOpaqueType("__SIMDd",globalScope);
}



/******************************************************************************************************************************/
/*
  Translation for multiply-accumulate operations.
  d = a * b + c   ==>  d = madd(a,b,c)

  The multiply-accumulate operations are not supported by every architecture.  
*/
/******************************************************************************************************************************/
class maddTraversal : public AstSimpleProcessing
{
  public:
    void visit(SgNode* n)
    {
      SgAddOp* addOp = isSgAddOp(n);
      SgSubtractOp* subOp = isSgSubtractOp(n);

      string suffix = "";
      if(addOp != NULL && isSgMultiplyOp(addOp->get_lhs_operand()) != NULL)
      {
        suffix = getSIMDOpSuffix(addOp->get_type());
        SgName functionName = SgName("_SIMD_madd"+suffix);
        generateMultiplyAccumulateFunctionCall(addOp,functionName);
      }
      else if(subOp != NULL && isSgMultiplyOp(subOp->get_lhs_operand()) != NULL)
      {
        suffix = getSIMDOpSuffix(subOp->get_type());
        SgName functionName = SgName("_SIMD_msub"+suffix);
        generateMultiplyAccumulateFunctionCall(subOp,functionName);
      }
    }
};

void vectorization::translateMultiplyAccumulateOperation(SgForStatement* forStatement)
{
  maddTraversal maddTranslation;
  maddTranslation.traverse(forStatement,postorder);
}



/******************************************************************************************************************************/
/*
  result = a * b + c
  the root will be the addOp.  The name will be _SIMD_add.

  This function creates a function call that invokes the accumulated function.
  The 3 operands become the arguments of this function call.
  Then we replace the root with this new functionCallExp.
*/
/******************************************************************************************************************************/
void vectorization::generateMultiplyAccumulateFunctionCall(SgBinaryOp* root, SgName functionName)
{
  SgBinaryOp* lhs = isSgBinaryOp(root->get_lhs_operand());
  ROSE_ASSERT(lhs);
  SgFunctionDefinition* functionDefinition = getEnclosingFunctionDefinition(root);
  ROSE_ASSERT(functionDefinition);
  /*
    result = exp1 * exp2 +/- exp3

    exp1 is the lhs of the multiply
    exp2 is the rhs or the multiply
    exp3 is the rhs or the add/sub 
    
  */
  SgExpression* exp1 = lhs->get_lhs_operand();
  SgExpression* exp2 = lhs->get_rhs_operand();
  SgExpression* exp3 = root->get_rhs_operand();


  SgExprListExp* functionExprList = buildExprListExp(exp1, exp2, exp3);
  SgFunctionCallExp* functionCallExp = buildFunctionCallExp(functionName, getSIMDType(root->get_type(),functionDefinition),functionExprList,functionDefinition);
  functionExprList->set_parent(functionCallExp);
  functionCallExp->set_parent(root->get_parent());
  lhs->set_lhs_operand(NULL);
  lhs->set_rhs_operand(NULL);
  root->set_lhs_operand(NULL);
  root->set_rhs_operand(NULL);
  replaceExpression(root, functionCallExp, false);
  deepDelete(lhs);

  translateOperand(exp1);
  translateOperand(exp2);
  translateOperand(exp3);
}




/******************************************************************************************************************************/
/*
*/
/******************************************************************************************************************************/
void vectorization::updateLoopIteration(SgForStatement* forStatement, int VF)
{
  // Fetch all information from original forStatement
  SgInitializedName* indexVariable = getLoopIndexVariable(forStatement);
  ROSE_ASSERT(indexVariable);

  SgBinaryOp* testExpression = isSgBinaryOp(forStatement->get_test_expr());
  ROSE_ASSERT(testExpression);

  SgBasicBlock* loopBody = isSgBasicBlock(forStatement->get_loop_body());
  ROSE_ASSERT(loopBody);

  SgScopeStatement* scope = forStatement->get_scope();
  ROSE_ASSERT(scope);

  SgStatementPtrList forInitStatementList = forStatement->get_init_stmt();

  SgFunctionDefinition* funcDef =  getEnclosingFunctionDefinition(forStatement);
  ROSE_ASSERT(funcDef!=NULL);
  SgBasicBlock* funcBody = funcDef->get_body();
  ROSE_ASSERT(funcBody!=NULL);

  /* 
    Create outerloop for the stripmined loop.  
    If the index name is i, the outerput loop index is i_stripminedLoop_#, with the line number for surfix.
  */
  int forStatementLineNumber = forStatement->get_file_info()->get_line();
  ostringstream convert;
  convert << forStatementLineNumber;
  SgName innerLoopIndex =  indexVariable->get_name().getString() +"_strip_"+ convert.str();

  // Create the index initialization for the inner loop index.
  SgVariableDeclaration* innerLoopIndexDecl = buildVariableDeclaration(innerLoopIndex,buildIntType(), NULL, funcBody);
  prependStatement(innerLoopIndexDecl, funcBody);
  SgVariableSymbol* innerLoopIndexSymbol = getFirstVarSym(innerLoopIndexDecl);

  
  SgExprStatement* innerLoopInit = buildAssignStatement(buildVarRefExp(innerLoopIndex,scope), buildVarRefExp(indexVariable,scope));

//  SgAssignInitializer* assignInitializer = buildAssignInitializer(buildVarRefExp(indexVariable,scope),buildIntType()); 
//  SgVariableDeclaration*   innerLoopInit = buildVariableDeclaration(innerLoopIndex,buildIntType(),assignInitializer, scope);

  forStatement->append_init_stmt(innerLoopInit);
  // Change the loop stride to be VF for the original loop, which will be the outer loop after strip-mining.
  setLoopStride(forStatement, buildIntVal(VF));  

  SgPlusAssignOp* newIncrementOp = buildPlusAssignOp(buildVarRefExp(innerLoopIndex,scope), buildIntVal(1));
  //prependStatement(newIncrementStmt,loopBody);
  SgExpression* increment = forStatement->get_increment();
  ROSE_ASSERT(increment);
  SgCommaOpExp* commaOpExp = buildCommaOpExp(increment, newIncrementOp);
  forStatement->set_increment(commaOpExp);
  
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

}



/******************************************************************************************************************************/
/*
  Check if the loop has stride distance 1.  
  We only test this after the loop normalization.  Therefore, the increment expression has only two cases:
  1. i += 1;
  2. i = i +/- k;
*/
/******************************************************************************************************************************/

bool vectorization::isStrideOneLoop(SgNode* loop)
{
  // Get the increment expression
  SgExpression* increment = NULL;
  if (SgFortranDo* doLoop = isSgFortranDo(loop))
  {
    increment = doLoop->get_increment();
  }
  else if  (SgForStatement* forLoop = isSgForStatement(loop))
  {
    increment = forLoop->get_increment();
  }
  else
  {
    ROSE_ASSERT(false);
  }

  switch (increment->variantT()) 
  {
    case V_SgAssignOp:
      {
        SgAssignOp* assignOp = isSgAssignOp(increment);
        SgVarRefExp* lhs = isSgVarRefExp(assignOp->get_rhs_operand());
        SgAddOp* addOp = isSgAddOp(assignOp->get_rhs_operand());
        if((lhs != NULL) && (addOp != NULL))
        {
          SgExpression* rhs = addOp->get_rhs_operand();
          SgVarRefExp* leftOperand = isSgVarRefExp(addOp->get_lhs_operand());
          SgIntVal* strideDistance = isSgIntVal(rhs);
          return ((leftOperand != NULL) && 
                  (lhs->get_symbol() == leftOperand->get_symbol()) &&
                  (strideDistance != NULL) && 
                  (strideDistance->get_value() == 1));
        }
        else
        {
          return false;
        }
      }
      break;
    case V_SgPlusAssignOp:
      {
        SgPlusAssignOp* plusAssignOp = isSgPlusAssignOp(increment);
        SgExpression* rhs = plusAssignOp->get_rhs_operand();
        SgIntVal* strideDistance = isSgIntVal(rhs);
        return ((strideDistance != NULL) && (strideDistance->get_value() == 1));
      }
      break;
    default:
      return false;
  }
}


string vectorization::getSIMDOpSuffix(SgType* opType)
{
  string suffix = "";
  SgType* type;
  SgArrayType* arrayType;
  arrayType = isSgArrayType(opType);
  if( arrayType != NULL)
    type = arrayType->get_base_type();
  else
    type = opType;

  switch(type->variantT())
  {
    case V_SgTypeDouble:
      {
        suffix = "_pd";
      }
      break;
    case V_SgTypeInt:
      {
        suffix = "_epi32";
      }
      break;
    default:
      {
        suffix = "_ps";
      }
      break;
  }

  return suffix;
}
