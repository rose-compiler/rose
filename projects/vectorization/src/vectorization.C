#include "vectorization.h"
#include "CommandOptions.h"
#include "AstInterface.h"
#include "AstInterface_ROSE.h"

using namespace std;
using namespace SageInterface;
using namespace SageBuilder;
using namespace SIMDVectorization;

/******************************************************************************************************************************/
/*
  Add the SIMD header files to the output file.  
  Inside the rose_simd.h, the header file includes the required header files for different architecture. 
  This mapping process depends on the translator options provided by user.
*/
/******************************************************************************************************************************/
extern int VF;

void SIMDVectorization::addHeaderFile(SgProject* project)
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
    PreprocessingInfo* headerInfo = insertHeader("rose_simd.h",PreprocessingInfo::after,false,global);
    headerInfo->set_file_info(global->get_file_info());
  }
}

/******************************************************************************************************************************/
/*
  Insert these data types into AST.  
  This step has to be done in the beginning of translation.  
  The later SIMD translation can recognize and use these SIMD data type.
  These data types will map to the typedef in the header file.
*/
/******************************************************************************************************************************/

void SIMDVectorization::insertSIMDDataType(SgGlobal* globalScope)
{
  buildOpaqueType("__SIMD",globalScope);
  buildOpaqueType("__SIMDi",globalScope);
  buildOpaqueType("__SIMDd",globalScope);
}

/******************************************************************************************************************************/
/*
  translate the binary operator to the mapped SIMD intrisic functions
  Example:  
  a[i] = b[i] + c[i]  ==>  a_SIMD[i_strip_] = _SIMD_add_ps( b_SIMD[i_strip_] , c_SIMD[i_strip_]);
  a = b - c           ==> __SIMD a_SIMD = _SIMD_sub_ps(_SIMD_splats_ps(b), _SIMD_spalts_ps(c));
*/
/******************************************************************************************************************************/
void SIMDVectorization::translateBinaryOp(SgBinaryOp* binaryOp, SgScopeStatement* scope, SgName name)
{
  SgExpression* lhs = binaryOp->get_lhs_operand();
  ROSE_ASSERT(lhs);
  SgExpression* rhs = binaryOp->get_rhs_operand();
  ROSE_ASSERT(rhs);
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
SgType* SIMDVectorization::getSIMDType(SgType* variableType, SgScopeStatement* scope)
{
  ROSE_ASSERT(variableType);
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
    case V_SgTypeFloat:
      {
        returnSIMDType = lookupTypedefSymbolInParentScopes("__SIMD",scope)->get_type();
      }
      break;
    case V_SgArrayType:
      {
        /*
          If the variableSymbol returns its type as SgArrayType, then we need to query the base_type to get the real data type.
        */
        SgArrayType* arrayType = isSgArrayType(variableType);
        returnSIMDType = getSIMDType(arrayType->get_base_type(),scope); 
      }
      break;
    default:
      {
        cerr<<"warning, unhandled node type: "<< variableType->class_name()<<endl;
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
void SIMDVectorization::translateOperand(SgExpression* operand)
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

        //  If the operand is multi-dimensional array, then we have to do a recursive search to get the SgVarRefExp
        SgNode* tmpNode = pntrArrRefExp->get_lhs_operand();
        SgVarRefExp* arrayRefExp  = NULL;
        while((arrayRefExp = isSgVarRefExp(tmpNode)) == NULL)
        {
          pntrArrRefExp = isSgPntrArrRefExp(tmpNode);
          tmpNode = (isSgPntrArrRefExp(tmpNode))->get_lhs_operand();
        }
        ROSE_ASSERT(arrayRefExp);
        SgVariableSymbol* arraySymbol = arrayRefExp->get_symbol();

        SgDeclarationStatement* arrayDeclarationStmt = arraySymbol->get_declaration()->get_declaration();
        string arrayName = arrayRefExp->get_symbol()->get_name().getString();
        string SIMDName = arrayName + "_SIMD";

        if(lookupSymbolInParentScopes(SIMDName,getEnclosingFunctionDefinition(operand)) == NULL)
        {
          /* 
            To create pointers that can point to multi-dimension array, 
            we need to retrieve the dimension information for each array.
            Multi-dimensional array in C is declared as array of arrays.  Therefore, we have to fetch all the information recursively.
          */
          vector<SgExpression*> arrayInfo;
          SgType* originalType = arraySymbol->get_type();
          SgArrayType* tmpArrayType = isSgArrayType(originalType);
          arrayInfo.insert(arrayInfo.begin(),tmpArrayType->get_index());
          while((tmpArrayType = isSgArrayType(tmpArrayType->get_base_type())) != NULL)
          {
            arrayInfo.insert(arrayInfo.begin(),tmpArrayType->get_index());
          }
          // Get the original data type and find the mapped SIMD data type.
          SgArrayType* arrayType = isSgArrayType(arraySymbol->get_type());
          ROSE_ASSERT(arrayType);
          SIMDType = getSIMDType(arrayType->get_base_type(),getEnclosingFunctionDefinition(operand)); 

          /*
            This following section creates pointer that points to a multi-dimension array, and applied the type casting on that.
            The following table should expalin the details:

            dimension       original array        SIMD pointer that points to the original array
              1             float a[i];           __SIMD *a_SIMD;       
              2             float b[j][i];        __SIMD (*b_SIMD)[i / 4];
              3             float c[k][j][i];     __SIMD (*c_SIMD)[j][i / 4];
          */
          SgType* newType = NULL;
          SgType* baseType = isSgType(SIMDType);
          for(vector<SgExpression*>::iterator i=arrayInfo.begin(); i <arrayInfo.end()-1;++i)
          {
            SgArrayType* tmpArrayType;
            if(i == arrayInfo.begin())
            {
              tmpArrayType = buildArrayType(baseType,buildDivideOp(*i,buildIntVal(VF)));
            }
            else
            {
              tmpArrayType = buildArrayType(baseType,*i);
            }
            baseType = isSgType(tmpArrayType);
          }
          newType = buildPointerType(baseType);

          // VariableDeclaration for this pointer, that points to the array space
          SgVariableDeclaration* SIMDDeclarationStmt = buildVariableDeclaration(SIMDName,newType,NULL,getEnclosingFunctionDefinition(operand));        
          insertStatement(arrayDeclarationStmt,SIMDDeclarationStmt,false,true);
          // define the pointer and make sure it points to the beginning of the array
          SgExprStatement* pointerAssignment = buildAssignStatement(buildVarRefExp(SIMDDeclarationStmt),
                                                                    buildCastExp(buildVarRefExp(arraySymbol), 
                                                                    newType, 
                                                                    SgCastExp::e_C_style_cast ));
          insertStatementAfterLastDeclaration(pointerAssignment,getEnclosingFunctionDefinition(operand));
        }
        /* 
          we have the SIMD pointer, we can use this as the operand in the SIMD intrinsic functions.
        */
        SgVarRefExp* newOperand = buildVarRefExp(SIMDName, getEnclosingFunctionDefinition(arrayRefExp));
        replaceExpression(arrayRefExp, newOperand, false);

      }
      break;
    case V_SgIntVal:
    case V_SgFloatVal:
    case V_SgCastExp:
      {
        string suffix = "";
        suffix = getSIMDOpSuffix(operand->get_type());
        SgName functionName = SgName("_SIMD_splats"+suffix);
        SgExprListExp* SIMDAddArgs = buildExprListExp(deepCopy(operand));
        SgFunctionCallExp* SIMDSplats = buildFunctionCallExp(functionName,operand->get_type(), SIMDAddArgs, getEnclosingFunctionDefinition(operand));
        replaceExpression(operand, SIMDSplats); 
      }
      break;
    default:
      {
        // By default, we don't change anything for the operands. 
      }
      break;
  }
}

/******************************************************************************************************************************/
/*
  Search for all the binary operations in the loop body and translate them into SIMD function calls.
  We implement translations for the basic arithmetic operator first.
*/
/******************************************************************************************************************************/
void SIMDVectorization::vectorizeBinaryOp(SgForStatement* forStatement)
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
      case V_SgAndOp:
      case V_SgBitAndOp:
      case V_SgBitOrOp:
      case V_SgBitXorOp:
      case V_SgEqualityOp:
      case V_SgGreaterOrEqualOp:
      case V_SgGreaterThanOp:
      case V_SgExponentiationOp:
      case V_SgNotOp:
      case V_SgLessOrEqualOp:
      case V_SgLessThanOp:
      case V_SgNotEqualOp:
        {
          cout << "support for " << binaryOp->class_name() << " is under construction" << endl;
        }
        break;
      case V_SgPntrArrRefExp:
        break;
      default:
        {
          cerr<<"warning, unhandled binaryOp: "<< binaryOp->class_name()<<endl;
        }
    }
    
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
void SIMDVectorization::stripmineLoop(SgForStatement* forStatement, int VF)
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

void SIMDVectorization::translateMultiplyAccumulateOperation(SgForStatement* forStatement)
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
void SIMDVectorization::generateMultiplyAccumulateFunctionCall(SgBinaryOp* root, SgName functionName)
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
  THis is a similar function to the stripmineLoop. Difference is in the loopStatement.
  StripmineLoop can be applied on non-vectorizable loop, but this one has to be applied to the vectorizable loop.

  for (i=0; i<Num; i++) {
    ...
  }
    
  transform the loop to the following format:

  for (i=0, j = i; i < Num; i+=VF, j ++) { 
  }


*/
/******************************************************************************************************************************/
void SIMDVectorization::updateLoopIteration(SgForStatement* forStatement, int VF)
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
  commaOpExp->set_parent(forStatement);
  
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
  SIMD intrinsic function has different surffix name for different type of operands.
  If it is a integer operand, then it has "_epi32" at the end.
  If it is a float operand, then it has "_ps" at the end.
  If it is a double operand, then it has "_pd" at the end.
*/
/******************************************************************************************************************************/

string SIMDVectorization::getSIMDOpSuffix(SgType* opType)
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
