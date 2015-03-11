#include "vectorization.h"
#include "SIMDAnalysis.h"
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
extern std::map<SgExprStatement*, vector<SgStatement*> > insertList;
extern std::map<std::string, std::string> constantValMap;
vector<SgPntrArrRefExp*> nonAlignedPntrArrList;

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

bool SIMDVectorization::isSubscriptExpression(SgExpression* exp)
{
  SgNode* tmp1 = exp;
  SgNode* tmp2 = exp;
  while(!isSgStatement(tmp1->get_parent()))
  {
     tmp2 = tmp1;
     tmp1 = tmp1->get_parent();
     if(isSgPntrArrRefExp(tmp1))
     {
       SgPntrArrRefExp* pntr = isSgPntrArrRefExp(tmp1);
       if(pntr->get_rhs_operand() == tmp2)
         return true;
       else return false;
     }
  } 
  return false;
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
//  if(isSgPntrArrRefExp(binaryOp->get_parent()) != NULL)
  if(isSubscriptExpression(binaryOp))
    return;
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
        cerr<<"warning, unhandled node type in getSIMDType: "<< variableType->class_name()<<endl;
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
  string constantValString;
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

        string scalarName = variableSymbol->get_name().getString();
        if (SgProject::get_verbose() > 2)
          std::cout << "translateOperand sees scalar:" << scalarName << std::endl;
        // Create new SIMD variable that stores the same scalar value in all its data elements
        string SIMDName = scalarName + "_SIMD";

        // If the new SIMD variable isn't declared, then we create it right after the scalar varialbe declaration
        if(lookupSymbolInParentScopes(SIMDName,getScope(operand)) == NULL)
        {
          SIMDType = getSIMDType(variableSymbol->get_type(),getScope(operand)); 
          /*
             This is the case that operand is one of the function parameter
          */
          if(isSgFunctionParameterList(scalarDeclarationStmt) != NULL)
          {
            SgVariableDeclaration* SIMDDeclarationStmt = buildVariableDeclaration(SIMDName,SIMDType,NULL,getEnclosingFunctionDefinition(operand));
            insertStatementAfterLastDeclaration(SIMDDeclarationStmt,getEnclosingFunctionDefinition(operand));
          }
          else
          {
            SgVariableDeclaration* SIMDDeclarationStmt = buildVariableDeclaration(SIMDName,SIMDType,NULL,variableSymbol->get_scope());        
            insertStatement(scalarDeclarationStmt,SIMDDeclarationStmt,false,true);
          }
        }
        SgVarRefExp* newOperand = buildVarRefExp(SIMDName, getScope(operand));
        newOperand->set_parent(operand->get_parent());
        replaceExpression(operand, newOperand, false);
        break;
      }
    case V_SgPntrArrRefExp:
      {
        SgPntrArrRefExp* pntrArrRefExp = isSgPntrArrRefExp(operand);

        if(find(nonAlignedPntrArrList.begin(), nonAlignedPntrArrList.end(), pntrArrRefExp) != nonAlignedPntrArrList.end())
        {
          SgNode* tmpNode = pntrArrRefExp;
          SgNode* tmpNode2 = tmpNode;
          while(!isSgAssignOp(tmpNode))
          {
            tmpNode2 = tmpNode;
            tmpNode = tmpNode->get_parent();
          }
          SgAssignOp* assignOp = isSgAssignOp(tmpNode);
          ROSE_ASSERT(assignOp);
          bool isLHS = false;
          if(assignOp->get_lhs_operand() == tmpNode2) 
           isLHS = true;


          tmpNode = pntrArrRefExp->get_lhs_operand();
          SgVarRefExp* arrayRefExp  = NULL;
          while((arrayRefExp = isSgVarRefExp(tmpNode)) == NULL)
          {
            pntrArrRefExp = isSgPntrArrRefExp(tmpNode);
            tmpNode = (isSgPntrArrRefExp(tmpNode))->get_lhs_operand();
          }
          ROSE_ASSERT(arrayRefExp);
          SgVariableSymbol* arraySymbol = arrayRefExp->get_symbol();

          SgDeclarationStatement* arrayDeclarationStmt = arraySymbol->get_declaration()->get_declaration();
          string arrayName = arraySymbol->get_name().getString();
          cout << "array " << arrayName << " is not aligned and isLHS=" << isLHS << endl;

          string SIMDName = generateUniqueVariableName(arraySymbol->get_scope(),"SIMDtmp");
          if(lookupSymbolInParentScopes(SIMDName,getScope(operand)) == NULL)
          {
//            SgArrayType* arrayType = isSgArrayType(arraySymbol->get_type());
//            ROSE_ASSERT(arrayType);
//            SIMDType = getSIMDType(arrayType->get_base_type(),getScope(operand)); 
            SIMDType = getSIMDType(arraySymbol->get_type()->findBaseType(),getScope(operand)); 
            SgVariableDeclaration* SIMDDeclarationStmt = buildVariableDeclaration(SIMDName,SIMDType,NULL,getGlobalScope(operand));        
            insertStatementAfterLastDeclaration(SIMDDeclarationStmt,arraySymbol->get_scope());
          }
          if(isLHS)
          {
            // Storing the tmp SIMD operand back to the array address
            SgName functionName = SgName("_SIMD_st");
            SgExprListExp* SIMDAddArgs = buildExprListExp(buildAddressOfOp(deepCopy(operand)),buildVarRefExp(SIMDName, getScope(operand)));
            SgFunctionCallExp* SIMDST = buildFunctionCallExp(functionName,SIMDType, 
                                                               SIMDAddArgs, 
                                                               getScope(operand));
            SgExprStatement* stmt = buildExprStatement(SIMDST);
            appendStatement(stmt, getScope(operand)); 
          }
          else
          {
            // Loading the tmp SIMD operand from array address, this is expected to be an unaligned load
            SgName functionName = SgName("_SIMD_load");
            SgExprListExp* SIMDAddArgs = buildExprListExp(buildAddressOfOp(deepCopy(operand)));
            SgFunctionCallExp* SIMDLD = buildFunctionCallExp(functionName,SIMDType, 
                                                               SIMDAddArgs, 
                                                               getScope(operand));
            SgAssignOp* assignTmp = buildAssignOp(buildVarRefExp(SIMDName, getScope(operand)),SIMDLD);
            SgExprStatement* stmt = buildExprStatement(assignTmp);
            prependStatement(stmt, getScope(operand)); 
          }
          SgVarRefExp* newOperand = buildVarRefExp(SIMDName, arraySymbol->get_scope());
          newOperand->set_parent(operand->get_parent());
          replaceExpression(operand, newOperand, false);
        }
        else
        {
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
          string arrayName = arraySymbol->get_name().getString();

          //  If the operand is multi-dimensional array, then we have to do a recursive search to get the SgVarRefExp
          string SIMDName = arrayName + "_SIMD";
          if(lookupSymbolInParentScopes(SIMDName,getScope(operand)) == NULL)
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
//            SgArrayType* arrayType = isSgArrayType(arraySymbol->get_type());
//            ROSE_ASSERT(arrayType);
//            SIMDType = getSIMDType(arrayType->get_base_type(),getScope(operand)); 
            SIMDType = getSIMDType(arraySymbol->get_type()->findBaseType(),getScope(operand)); 

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

            /* 
              VariableDeclaration for this pointer, that points to the array space
              The assignInitializer should take care of the assignment, and no further pointer assignment is needed.
            */
            SgVariableDeclaration* SIMDDeclarationStmt = buildVariableDeclaration(SIMDName,
                                                                                  newType,
                                                                                  buildAssignInitializer(buildCastExp(buildVarRefExp(arraySymbol),newType,SgCastExp::e_C_style_cast ),newType),
                                                                                  arraySymbol->get_scope());        
            insertStatement(arrayDeclarationStmt,SIMDDeclarationStmt,false,true);
          }
          /* 
            we have the SIMD pointer, we can use this as the operand in the SIMD intrinsic functions.
          */
          SgVarRefExp* newOperand = buildVarRefExp(SIMDName, getScope(arrayRefExp));
          replaceExpression(arrayRefExp, newOperand, false);
        }
        break;
      }
    case V_SgCastExp:
      {
        translateOperand(isSgCastExp(operand)->get_operand());
        replaceExpression(operand, deepCopy(isSgCastExp(operand)->get_operand()),false);
        break;
      }
    case V_SgIntVal:
        {
          if(isSgIntVal(operand) != NULL)
            constantValString = isSgIntVal(operand)->get_valueString();
        }
    case V_SgFloatVal:
        {
          if(isSgFloatVal(operand) != NULL)
            constantValString = isSgFloatVal(operand)->get_valueString();
        }
    case V_SgDoubleVal:
      {
        if(isSgDoubleVal(operand) != NULL)
          constantValString = isSgDoubleVal(operand)->get_valueString();
        SgType* valType = operand->get_type();
        if(isSgCastExp(operand->get_parent()) != NULL)
        {
          valType = isSgCastExp(operand->get_parent())->get_type();
        }
        if(constantValMap.find(constantValString) == constantValMap.end())
        {
          string constantName = generateUniqueVariableName(getEnclosingFunctionDefinition(operand),"constant");
          string suffix = "";
          suffix = getSIMDOpSuffix(valType);
          SgType* SIMDType = getSIMDType(valType, getScope(operand));
          SgName functionName = SgName("_SIMD_splats"+suffix);
          SgExprListExp* SIMDAddArgs = buildExprListExp(deepCopy(operand));
          SgFunctionCallExp* SIMDSplats = buildFunctionCallExp(functionName,SIMDType, 
                                                               SIMDAddArgs, 
                                                               getScope(operand));
          SgAssignInitializer* constantInitializer = buildAssignInitializer(SIMDSplats,SIMDType);
          SgVariableDeclaration* constantValDecl = buildVariableDeclaration(constantName,
                                                                            SIMDType,
                                                                            constantInitializer,
                                                                            getEnclosingFunctionDefinition(operand));
          insertStatementAfterLastDeclaration(constantValDecl,getEnclosingFunctionDefinition(operand));
          constantValMap.insert(pair<string,string>(constantValString,constantName)); 
        }
        SgVarRefExp* constantVarRefExp = buildVarRefExp(constantValMap.find(constantValString)->second,getScope(operand));
        replaceExpression(operand, constantVarRefExp); 
      }
      break;
    default:
      {
        // By default, we don't change anything for the operands. 
      }
  }
}

/******************************************************************************************************************************/
/*
  Search for all the binary operations in the loop body and translate them into SIMD function calls.
  We implement translations for the basic arithmetic operator first.
*/
/******************************************************************************************************************************/
void SIMDVectorization::vectorizeUnaryOp(SgUnaryOp* unaryOp)
{
  SgScopeStatement* scope = getScope(unaryOp);
    string suffix = getSIMDOpSuffix(unaryOp->get_type());

    switch(unaryOp->variantT())
    {
      case V_SgMinusOp:
        {
          SgExpression* operand = unaryOp->get_operand_i();
          SgExprListExp* vectorArgs = buildExprListExp(operand);
          SgFunctionCallExp* SIMDExp = buildFunctionCallExp("_SIMD_neg"+suffix, unaryOp->get_type(), vectorArgs, scope);
          unaryOp->set_operand(NULL);
          replaceExpression(unaryOp, SIMDExp, false);
          translateOperand(operand);  
          break;
        }
      case V_SgUnaryAddOp:
        // We should be able to remove the "+".
        {
          SgExpression* operand = unaryOp->get_operand_i();
          unaryOp->set_operand(NULL);
          replaceExpression(unaryOp, operand, false);
          translateOperand(operand);  
          break;
        }
      case V_SgMinusMinusOp:
      case V_SgPlusPlusOp:
      case V_SgCastExp:
        break;
      case V_SgNotOp:
      case V_SgAddressOfOp:
      case V_SgBitComplementOp:
      case V_SgConjugateOp:
      case V_SgExpressionRoot:
      case V_SgPointerDerefExp:
      case V_SgRealPartOp:
      case V_SgThrowOp:
      case V_SgUserDefinedUnaryOp:
      default:
        {
          cerr<<"warning, unhandled unaryOp in vectorizeUnaryOp: "<< unaryOp->class_name()<<endl;
        }
    }
}

/******************************************************************************************************************************/
/*
  Search for all the binary operations in the loop body and translate them into SIMD function calls.
  We implement translations for the basic arithmetic operator first.
*/
/******************************************************************************************************************************/
void SIMDVectorization::vectorizeBinaryOp(SgBinaryOp* binaryOp)
{
  SgScopeStatement* scope = getScope(binaryOp);
    string suffix = getSIMDOpSuffix(binaryOp->get_type());

    switch(binaryOp->variantT())
    {
      case V_SgAddOp:
        {
          translateBinaryOp(binaryOp, scope, "_SIMD_add"+suffix);
          break;
        }
      case V_SgSubtractOp:
        {
          translateBinaryOp(binaryOp, scope, "_SIMD_sub"+suffix);
          break;
        }
      case V_SgMultiplyOp:
        {
          translateBinaryOp(binaryOp, scope, "_SIMD_mul"+suffix);
          break;
        }
      case V_SgDivideOp:
        {
          translateBinaryOp(binaryOp, scope, "_SIMD_div"+suffix);
          break;
        }
      case V_SgAssignOp:
        {
          translateOperand(binaryOp->get_lhs_operand()); 
          translateOperand(binaryOp->get_rhs_operand()); 
          break;
        }
      case V_SgBitAndOp:
        {
          translateBinaryOp(binaryOp, scope, "_SIMD_and"+suffix);
          break;
        }
      case V_SgBitOrOp:
        {
          translateBinaryOp(binaryOp, scope, "_SIMD_or"+suffix);
          break;
        }
      case V_SgBitXorOp:
        {
          translateBinaryOp(binaryOp, scope, "_SIMD_xor"+suffix);
          break;
        }
      case V_SgEqualityOp:
      case V_SgGreaterOrEqualOp:
      case V_SgGreaterThanOp:
      case V_SgExponentiationOp:
      case V_SgNotOp:
      case V_SgLessOrEqualOp:
      case V_SgLessThanOp:
      case V_SgNotEqualOp:
        {
          // We simply change their operands, then leave it for vectorizeConditionalStmt to handle
          translateOperand(binaryOp->get_lhs_operand()); 
          translateOperand(binaryOp->get_rhs_operand()); 
          break;
        }
      case V_SgCompoundAssignOp:
        {
          cout << "support for " << binaryOp->class_name() << " is under construction" << endl;
          break;
        }
      case V_SgPntrArrRefExp:
        break;
      default:
        {
          cerr<<"warning, unhandled binaryOp in vectorizeBinaryOp: "<< binaryOp->class_name()<<endl;
        }
    }
}

/******************************************************************************************************************************/
/*
  This will handle the case of conditional vector merge functions (CVMGx)
  The input has to follow strict rules:
    1. Only assignmentStmt is allowed in true_body and false_body
    2. If there is false_body, then the assignment list in both true and false cases are identical.
        Ex.  if(true)
               {
                a = ...
                b = ...
               }
             else
               {
                a = ...
                b = ...
               }
    

  reference: http://publib.boulder.ibm.com/infocenter/comphelp/v8v101/index.jsp?topic=%2Fcom.ibm.xlf101a.doc%2Fxlflr%2Fcvmgx.htm
*/
/******************************************************************************************************************************/
void SIMDVectorization::vectorizeConditionalStmt(SgIfStmt* ifStmt)
{

    // Create a map table for each ifStmt.
    map<SgName, SIMDVectorization::conditionalStmts*> conditionStmtTable;
    // First handle the true_body
    if(isSgBasicBlock(ifStmt->get_true_body()))
    {
      SgBasicBlock* basicBlock = isSgBasicBlock(ifStmt->get_true_body());
      ROSE_ASSERT(basicBlock);
      SgStatementPtrList stmtList = basicBlock->get_statements();
      Rose_STL_Container<SgStatement*>::iterator stmtIter;
      int index;
      for(stmtIter = stmtList.begin(), index = 0; stmtIter != stmtList.end(); ++stmtIter, ++index)
      {
        insertConditionalStmtTable(*stmtIter,conditionStmtTable, index);
      }
    }
    else
    {
      insertConditionalStmtTable(ifStmt->get_true_body(),conditionStmtTable, 0);
    }
    // Now handle the false_body, if there is one
    if(ifStmt->get_false_body() != NULL)
    {
      if(isSgBasicBlock(ifStmt->get_false_body()))
      {
        SgBasicBlock* basicBlock = isSgBasicBlock(ifStmt->get_false_body());
        ROSE_ASSERT(basicBlock);
        SgStatementPtrList stmtList = basicBlock->get_statements();
        for(Rose_STL_Container<SgStatement*>::iterator stmtIter = stmtList.begin(); stmtIter != stmtList.end(); ++stmtIter)
        {
          updateConditionalStmtTable(*stmtIter,conditionStmtTable);
        }
      }
      else
      {
        updateConditionalStmtTable(ifStmt->get_false_body(),conditionStmtTable);
      }
    }

    // Now we have both true and false statements in the table.
    translateIfStmt(ifStmt, conditionStmtTable);
}

/******************************************************************************************************************************/
/*
*/
/******************************************************************************************************************************/
void SIMDVectorization::translateIfStmt(SgIfStmt* ifStmt, std::map<SgName,conditionalStmts*>& table)
{
  SgScopeStatement* scope = ifStmt->get_scope();
  ROSE_ASSERT(scope);
  SgBinaryOp* conditionOp = isSgBinaryOp(isSgExprStatement(ifStmt->get_conditional())->get_expression());
  ROSE_ASSERT(conditionOp);
  SgExpression* lhsExp = conditionOp->get_lhs_operand();
  SgExpression* rhsExp = conditionOp->get_rhs_operand();
  string conditionFunctionName = "_SIMD_cmp";
  switch(conditionOp->variantT())
  {
    case V_SgEqualityOp:
      {
        conditionFunctionName += "eq";
        break;
      }
    case V_SgGreaterOrEqualOp:
      {
        conditionFunctionName += "ge";
        break;
      }
    case V_SgGreaterThanOp:
      {
        conditionFunctionName += "gt";
        break;
      }
    case V_SgLessOrEqualOp:
      {
        conditionFunctionName += "le";
        break;
      }
    case V_SgLessThanOp:
      {
        conditionFunctionName += "lt";
        break;
      }
    case V_SgNotEqualOp:
      {
        conditionFunctionName += "ne";
        break;
      }
    default:
      {
        cerr << "unhandled conditional operation " << conditionOp->class_name() << endl;
        ROSE_ASSERT(false);
      }
  }

  /* 
    in C, get_type from the conditionOp is always int.
    We need to find the type from operand.
  */
  SgExpression* varRefExp = lhsExp;
  while(isSgVarRefExp(varRefExp) == NULL)
    varRefExp = isSgBinaryOp(varRefExp)->get_lhs_operand();
  conditionFunctionName += getSIMDOpSuffix(varRefExp->get_type());

  string cmpReturnName = "cmpReturn_";
  SgType* cmpReturnType = buildPointerType(buildVoidType());
/*
  cmpReturn will be treated as a reserved name to store the result of conditional comparison.
  We append the line number to the end and create a new name.
  TODO: It's better to find a way to reuse the same name.
*/
//  if(lookupSymbolInParentScopes(cmpReturnName,getEnclosingFunctionDefinition(conditionOp)) != NULL)
  {
    int lineNumber = conditionOp->get_file_info()->get_line();
    ostringstream convert;
    convert << lineNumber;
    cmpReturnName += convert.str(); 
  }
  SgVariableDeclaration* cmpReturnVarDecl = buildVariableDeclaration(cmpReturnName,cmpReturnType,NULL,getEnclosingFunctionDefinition(conditionOp));        
  insertStatementAfterLastDeclaration(cmpReturnVarDecl,getEnclosingFunctionDefinition(conditionOp));


  SgExprListExp* SIMDCmpArgs = buildExprListExp(lhsExp,rhsExp,buildAddressOfOp(buildVarRefExp(cmpReturnName,scope)));
  SgFunctionCallExp* SIMDCmpFunctionCall = buildFunctionCallExp(conditionFunctionName,buildVoidType(), 
                                                        SIMDCmpArgs, 
                                                        scope);
  SgExprStatement* cmpFunctionCallStmt = buildExprStatement(SIMDCmpFunctionCall);

  // Create a list of statement that will replace the original true and false basicBlcok. 
  vector<SgStatement*> ifConditionStmtList(table.size(), NULL);
  for(map<SgName,conditionalStmts*>::iterator i=table.begin(); i != table.end(); ++i)
  {
    SgType* lhsType = lookupSymbolInParentScopes((*i).first,scope)->get_type();
    string SIMDSelName = "_SIMD_sel" + getSIMDOpSuffix(lhsType);
    conditionalStmts* ifStmtData = (*i).second; 
    SgExprListExp* SIMDSelArgs = buildExprListExp(ifStmtData->getFalseExpr(),ifStmtData->getTrueExpr(),buildAddressOfOp(buildVarRefExp(cmpReturnName,scope)));
    SgFunctionCallExp* SIMDCmpFunctionCall = buildFunctionCallExp(SIMDSelName,lhsType, 
                                                          SIMDSelArgs, 
                                                          scope);
    ifConditionStmtList[ifStmtData->getIndex()] =buildAssignStatement(ifStmtData->getLhsExpr(),SIMDCmpFunctionCall);
  }
  /*
     Because of the postorder traversal, inserting statements after the current
     statement would cause problem. 
     The true, and false block in this if statement are already translated. Insert the new asignment
     statement after will make the vectorizeBinaryOp to translate it again.
  */
  //insertStatementListAfter(ifStmt, ifConditionStmtList); 
  replaceStatement(ifStmt, cmpFunctionCallStmt,true);
  insertList.insert(pair<SgExprStatement*, vector<SgStatement*> >(cmpFunctionCallStmt,ifConditionStmtList));
  buildComment(cmpFunctionCallStmt,
               "if statement is converted into vectorizaed conditional statement",
               PreprocessingInfo::before, PreprocessingInfo::C_StyleComment);
}

/******************************************************************************************************************************/
/*
  This function is only called for the ifStmt->get_true_body()
  Insert both true and false ecases into ConditionalStmtTable.
  Ex.
  if(test == 1)
  {
    a = c
    b = d
  }

  We insert c as the true case for a, and a itself as the false case for a.
  We insert d as the true case for b, and b itself as the false case for b.
*/
/******************************************************************************************************************************/
void SIMDVectorization::insertConditionalStmtTable(SgStatement* stmt, std::map<SgName,conditionalStmts*>& table, int index)
{
  SgName name = NULL;
  SgExpression* lhsExp = NULL;
  SgExpression* trueExp = NULL;
  SgExpression* falseExp = NULL;

  SgExprStatement* exprStmt = isSgExprStatement(stmt);
  ROSE_ASSERT(exprStmt);
  SgAssignOp* assignOp = isSgAssignOp(exprStmt->get_expression());
  if(assignOp == NULL)
  {
    cerr << "not an assignment Op in conditionalStmt" << endl;
    ROSE_ASSERT(assignOp);
  }

  switch (assignOp->get_lhs_operand()->variantT())
  {
    case V_SgVarRefExp:
      {
        name = isSgVarRefExp(assignOp->get_lhs_operand())->get_symbol()->get_name();
        lhsExp = assignOp->get_lhs_operand();
        trueExp = assignOp->get_rhs_operand();
        falseExp = assignOp->get_lhs_operand();
        break;
      }
    case V_SgPntrArrRefExp:
      {
        SgPntrArrRefExp* tmp = isSgPntrArrRefExp(assignOp->get_lhs_operand());
        while(isSgVarRefExp(tmp->get_lhs_operand()) == NULL)
          tmp = isSgPntrArrRefExp(tmp->get_lhs_operand());
        name = isSgVarRefExp(tmp->get_lhs_operand())->get_symbol()->get_name();
        lhsExp = assignOp->get_lhs_operand();
        trueExp = assignOp->get_rhs_operand();
        falseExp = assignOp->get_lhs_operand();
        break;
      }
    default:
      {
        cerr << "LHS is not SgVarRefExp or SgPntrArrRefExp" << endl;
        ROSE_ASSERT(false);
      }
  }
  conditionalStmts* condition = new conditionalStmts(lhsExp, trueExp, falseExp, index);
  table.insert(pair<SgName,conditionalStmts*>(name,condition)); 
}

/******************************************************************************************************************************/
/*
  This function is only called for the ifStmt->get_false_body()
  Search name inside conditionalStmtTable. If found, then update its false case in the table.
  If not found, then insert a new record into the table.

  Ex.
  if(test == 1)
  {
    a = c
  }
  else
  {
    a = e
    b = d
  }

  Data for a is already inside table. The existed false case for a is a itself.
  This function update the false case of a to be e.
  Data for b isn't in the table.  We have to insert a new one.
  The true case for b is b itself, and the false case for be will be d.
*/
/******************************************************************************************************************************/
void SIMDVectorization::updateConditionalStmtTable(SgStatement* stmt, std::map<SgName,conditionalStmts*>& table)
{
  SgName name = NULL;
  SgExpression* lhsExp = NULL;
  SgExpression* trueExp = NULL;
  SgExpression* falseExp = NULL;

  SgExprStatement* exprStmt = isSgExprStatement(stmt);
  ROSE_ASSERT(exprStmt);
  SgAssignOp* assignOp = isSgAssignOp(exprStmt->get_expression());
  if(assignOp == NULL)
  {
    cerr << "not an assignment Op in conditionalStmt" << endl;
    ROSE_ASSERT(assignOp);
  }

  switch (assignOp->get_lhs_operand()->variantT())
  {
    case V_SgVarRefExp:
      {
        name = isSgVarRefExp(assignOp->get_lhs_operand())->get_symbol()->get_name();
        trueExp = assignOp->get_lhs_operand();
        falseExp = assignOp->get_rhs_operand();
        break;
      }
    case V_SgPntrArrRefExp:
      {
        SgPntrArrRefExp* tmp = isSgPntrArrRefExp(assignOp->get_lhs_operand());
        while(isSgVarRefExp(tmp->get_lhs_operand()) == NULL)
          tmp = isSgPntrArrRefExp(tmp->get_lhs_operand());
        name = isSgVarRefExp(tmp->get_lhs_operand())->get_symbol()->get_name();
        lhsExp = assignOp->get_lhs_operand();
        trueExp = assignOp->get_lhs_operand();
        falseExp = assignOp->get_rhs_operand();
        break;
      }
    default:
      {
        cerr << "LHS is not SgVarRefExp or SgPntrArrRefExp" << endl;
        ROSE_ASSERT(false);
      }
  }
  conditionalStmts* condition = NULL;
  if(table.find(name) == table.end())
  {
    condition = new conditionalStmts(lhsExp, trueExp, falseExp, table.size());
    table.insert(pair<SgName,conditionalStmts*>(name,condition)); 
  }
  else
  {
    condition = table.find(name)->second;
    condition->updateFalseStmt(falseExp);
  }
}


SIMDVectorization::conditionalStmts::conditionalStmts(SgExpression* lhsName, SgExpression* rhsTrue, SgExpression* rhsFalse, int i)
{
  lhsExpr = lhsName;
  trueExpr = rhsTrue;
  falseExpr = rhsFalse;
  index = i;
}

void SIMDVectorization::conditionalStmts::updateFalseStmt(SgExpression* rhsFalse)
{
  falseExpr = rhsFalse;
}

SgExpression* SIMDVectorization::conditionalStmts::getLhsExpr()
{
  return lhsExpr;
}

SgExpression* SIMDVectorization::conditionalStmts::getTrueExpr()
{
  return trueExpr;
}

SgExpression* SIMDVectorization::conditionalStmts::getFalseExpr()
{
  return falseExpr;
}

int SIMDVectorization::conditionalStmts::getIndex()
{
  return index;
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
    If the index name is i, the outerput loop index is i_stripminedLoop_#, with the line number for suffix.
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
      bool isMatched = false;
      SgAddOp* addOp = isSgAddOp(n);
      SgSubtractOp* subOp = isSgSubtractOp(n);

      string suffix = "";
      if(addOp != NULL)
      {
        if(isSgMultiplyOp(addOp->get_lhs_operand()) != NULL)
        {
          isMatched = true;
        }
        else if(isSgCastExp(addOp->get_lhs_operand()) != NULL)
        {
          SgCastExp* castExp = isSgCastExp(addOp->get_lhs_operand());
          if(isSgMultiplyOp(castExp->get_operand()) != NULL)
          {
            isMatched = true;
          }
        }
        if(isSubscriptExpression(addOp))
          isMatched = false;
        if(isMatched)
        {
          suffix = getSIMDOpSuffix(addOp->get_type());
          SgName functionName = SgName("_SIMD_madd"+suffix);
          generateMultiplyAccumulateFunctionCall(addOp,functionName);
        }
      }
      else if(subOp != NULL)
      {
        if(isSgMultiplyOp(subOp->get_lhs_operand()) != NULL)
        {
          isMatched = true;
        }
        else if(isSgCastExp(subOp->get_lhs_operand()) != NULL)
        {
          SgCastExp* castExp = isSgCastExp(subOp->get_lhs_operand());
          if(isSgMultiplyOp(castExp->get_operand()) != NULL)
          {
            isMatched = true;
          }
        }
        if(isSubscriptExpression(subOp))
          isMatched = false;
        if(isMatched)
        {
          suffix = getSIMDOpSuffix(subOp->get_type());
          SgName functionName = SgName("_SIMD_msub"+suffix);
          generateMultiplyAccumulateFunctionCall(subOp,functionName);
        }
      }
    }
};

void SIMDVectorization::translateMultiplyAccumulateOperation(SgStatement* loopBody)
{
  maddTraversal maddTranslation;
  maddTranslation.traverse(loopBody,postorder);
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
  // This is the special case when CastExp appears in lhs.
  if(lhs == NULL)
  {
    SgCastExp* cast = isSgCastExp(root->get_lhs_operand());
    ROSE_ASSERT(cast);
    lhs = isSgBinaryOp(cast->get_operand());
  }
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

/*
  TODO:  There might be the need to add CastExp before the arguments.
         However, the backend compiler might be smart enough to handle implicit cast.
*/

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
    If the index name is i, the outerput loop index is i_stripminedLoop_#, with the line number for suffix.
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
    {
      if (isSgAddOp(vRef->get_parent()) || isSgSubtractOp(vRef->get_parent()) || isSgMultiplyOp(vRef->get_parent()) || isSgDivideOp(vRef->get_parent()))
      {
        SgNode* tmpNode = vRef;
        while(!isSgPntrArrRefExp(tmpNode))
        {
          tmpNode = tmpNode->get_parent();
        }
        SgPntrArrRefExp* nonAlignedPntr = isSgPntrArrRefExp(tmpNode);
        ROSE_ASSERT(nonAlignedPntr);
        nonAlignedPntrArrList.push_back(nonAlignedPntr); 
// Special code required to handle subscript computation
      }
      else
        vRef->set_symbol(innerLoopIndexSymbol);
    }
  }

}

/******************************************************************************************************************************/
/*
  This is to change the lowerbound of the remaining loop iterations.


  for (i=0; i<Num; i++) {
    ...
  }
    
  transform the loop to the following format:

  for (i=0, j = i; i < Num; i+=VF, j ++) { 
  }
  for (i=VF*(Num/VF); i < Num; i++) { 
  }


*/
/******************************************************************************************************************************/
void SIMDVectorization::changeRemainingLowerBound(SgForStatement* forStatement, int VF)
{
  // Fetch all information from original forStatement
  SgInitializedName* indexVariable = getLoopIndexVariable(forStatement);
  ROSE_ASSERT(indexVariable);

  SgBinaryOp* testExpression = isSgBinaryOp(forStatement->get_test_expr());
  ROSE_ASSERT(testExpression);

  SgScopeStatement* scope = forStatement->get_scope();
  ROSE_ASSERT(scope);

  SgMultiplyOp* newinit = buildMultiplyOp(buildIntVal(VF),buildDivideOp(deepCopy(testExpression->get_rhs_operand()),buildIntVal(VF)));
  setLoopLowerBound(forStatement,newinit);

}

void SIMDVectorization::scalarVariableConversion(SgForStatement* forStatement, std::set<SgInitializedName*> liveIns, std::set<SgInitializedName*> liveOuts)
{
//  for(std::set< SgInitializedName *>::iterator i=liveIns.begin(); i!=liveIns.end(); ++i)
//  {
//    cout << "in: " << (*i)->get_name() << endl;
//  }
//  for(std::set< SgInitializedName *>::iterator i=liveOuts.begin(); i!=liveOuts.end(); ++i)
//  {
//    cout << "out: " << (*i)->get_name() << endl;
//  }


// Insert the promotion statement for the scalars used in the vector loop.
  for(std::set< SgInitializedName *>::iterator i=liveIns.begin(), i_next = i ; i!=liveIns.end(); i = i_next)
  {
    ++i_next;
    if(SIMDAnalysis::isLoopIndexVariable((*i),forStatement))
    {
      liveIns.erase(i);
      if (SgProject::get_verbose() > 2)
      {
        SgVariableSymbol* symbol = isSgVariableSymbol((*i)->get_symbol_from_symbol_table());
        ROSE_ASSERT(symbol);
        SgDeclarationStatement* scalarDeclarationStmt = symbol->get_declaration()->get_declaration();
        ROSE_ASSERT(scalarDeclarationStmt);
        string scalarName = symbol->get_name().getString();
        std::cout << "remove from LiveIns:" << scalarName << std::endl;
      }
    }
    else if(isScalarType((*i)->get_typeptr()))
    {
      SgVariableSymbol* symbol = isSgVariableSymbol((*i)->get_symbol_from_symbol_table());
      ROSE_ASSERT(symbol);
      SgDeclarationStatement* scalarDeclarationStmt = symbol->get_declaration()->get_declaration();
      ROSE_ASSERT(scalarDeclarationStmt);
      string scalarName = symbol->get_name().getString();
      if (SgProject::get_verbose() > 2)
        std::cout << "scalarVariableConversion sees scalar:" << scalarName << std::endl;
      // Create new SIMD variable that stores the same scalar value in all its data elements
      string SIMDName = scalarName + "_SIMD";
      SgType* SIMDType = getSIMDType(symbol->get_type(),getScope(forStatement)); 
      if(lookupSymbolInParentScopes(SIMDName,getScope(forStatement)) == NULL)
      {
        /*
           This is the case that variable is one of the function parameter
        */
        if(isSgFunctionParameterList(scalarDeclarationStmt) != NULL)
        {
          SgVariableDeclaration* SIMDDeclarationStmt = buildVariableDeclaration(SIMDName,SIMDType,NULL,getEnclosingFunctionDefinition(forStatement));
          insertStatementAfterLastDeclaration(SIMDDeclarationStmt,getEnclosingFunctionDefinition(forStatement));
        }
        else
        {
          SgVariableDeclaration* SIMDDeclarationStmt = buildVariableDeclaration(SIMDName,SIMDType,NULL,symbol->get_scope());
          insertStatement(scalarDeclarationStmt,SIMDDeclarationStmt,false,true);
        }
      }
      string promoteFuncName = "_SIMD_splats" + getSIMDOpSuffix(symbol->get_type());
      SgFunctionCallExp* callScalarPromotion = buildFunctionCallExp(promoteFuncName, 
                                                                    SIMDType, 
                                                                    buildExprListExp(buildVarRefExp(symbol)), 
                                                                    getScope(forStatement));
      SgExprStatement* promotionStmt = buildAssignStatement(buildVarRefExp(SIMDName, getScope(forStatement)),
                                                            callScalarPromotion);
      insertStatementBefore(forStatement, promotionStmt);
    }
  }

 
// insert the extraction statement at the end of the vector loop
  for(std::set< SgInitializedName *>::iterator i=liveOuts.begin(), i_next = i; i!=liveOuts.end(); i = i_next)
  {
    ++i_next;
    if(SIMDAnalysis::isLoopIndexVariable((*i),forStatement))
    {
      liveOuts.erase(i);
      if (SgProject::get_verbose() > 2)
      {
        SgVariableSymbol* symbol = isSgVariableSymbol((*i)->get_symbol_from_symbol_table());
        ROSE_ASSERT(symbol);
        SgDeclarationStatement* scalarDeclarationStmt = symbol->get_declaration()->get_declaration();
        ROSE_ASSERT(scalarDeclarationStmt);
        string scalarName = symbol->get_name().getString();
        std::cout << "remove from LiveOuts:" << scalarName << std::endl;
      }
    }
    else if(isScalarType((*i)->get_typeptr()))
    {
      SgVariableSymbol* symbol = isSgVariableSymbol((*i)->get_symbol_from_symbol_table());
      ROSE_ASSERT(symbol);
      SgDeclarationStatement* scalarDeclarationStmt = symbol->get_declaration()->get_declaration();
      ROSE_ASSERT(scalarDeclarationStmt);
      string scalarName = symbol->get_name().getString();
      if (SgProject::get_verbose() > 2)
        std::cout << "scalar:" << scalarName << std::endl;
      // Create new SIMD variable that stores the same scalar value in all its data elements
      string SIMDName = scalarName + "_SIMD";
      SgType* SIMDType = getSIMDType(symbol->get_type(),getScope(forStatement)); 
      if(lookupSymbolInParentScopes(SIMDName,getScope(forStatement)) == NULL)
      {
        SgVariableDeclaration* SIMDDeclarationStmt = buildVariableDeclaration(SIMDName,SIMDType,NULL,symbol->get_scope());        
        insertStatement(scalarDeclarationStmt,SIMDDeclarationStmt,false,true);
      }
      string extractFuncName = "_SIMD_extract" + getSIMDOpSuffix(symbol->get_type());
      SgFunctionCallExp* callScalarExtraction = buildFunctionCallExp(extractFuncName, 
                                                                    symbol->get_type(), 
                                                                    buildExprListExp(buildVarRefExp(SIMDName, getScope(forStatement)), buildIntVal(VF-1)), 
                                                                    getScope(forStatement));
      SgExprStatement* extractStmt = buildAssignStatement(buildVarRefExp(symbol),
                                                            callScalarExtraction);
      insertStatementAfter(forStatement, extractStmt);
    }
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

bool convertMathFunctionName(SgName inputName, SgName* output, SgType* inputType)
{
  const char* charPtr = inputName.str();
  if((strncmp(charPtr, "max",3) == 0) )
    *output = "_SIMD_max";
  else if((strncmp(charPtr, "min",3) == 0) )
    *output = "_SIMD_min";
  else if((strncmp(charPtr, "abs",3) == 0) )
    *output = "_SIMD_abs";
  else if((strncmp(charPtr, "sqrt",4) == 0) )
    *output = "_SIMD_sqrt";
  else if((strncmp(charPtr, "log",3) == 0) )
    *output = "_SIMD_log";
  else if((strncmp(charPtr, "exp",3) == 0) )
    *output = "_SIMD_exp";
  else if((strncmp(charPtr, "sin",3) == 0) )
    *output = "_SIMD_sin";
  else if((strncmp(charPtr, "cos",3) == 0) )
    *output = "_SIMD_cos";
  else
  {
    return false;
  }
  return true;
}

void SIMDVectorization::vectorizeFunctionCall(SgFunctionCallExp* funcCallExp)
{
  SgScopeStatement* scope = getScope(funcCallExp);
  SgFunctionRefExp* functionRefExp = isSgFunctionRefExp(funcCallExp->get_function());
  ROSE_ASSERT(functionRefExp); 
  SgExprListExp* exprListExp = funcCallExp->get_args();
  
  SgName functionName = functionRefExp->get_symbol()->get_name();

  SgFunctionCallExp* newFunctionCallExp = NULL;
  SgType* returnType = funcCallExp->get_type();
  if(isSgCastExp(funcCallExp->get_parent()))
  {
    SgCastExp* castExp = isSgCastExp(funcCallExp->get_parent());
    returnType = castExp->get_type();
  }
  SgName newFunctionName;
  if(convertMathFunctionName(functionName, &newFunctionName, returnType))
  {
    newFunctionName += getSIMDOpSuffix(returnType); 
    SgExprListExp* newExprListExp = deepCopy(exprListExp);
    
    newFunctionCallExp = buildFunctionCallExp(newFunctionName, deepCopy(funcCallExp->get_type()), newExprListExp, scope);
    ROSE_ASSERT(newFunctionCallExp);
    //insertSystemHeader("math.h",scope);
    replaceExpression(funcCallExp, newFunctionCallExp,false);
    SgExpressionPtrList operandList = newExprListExp->get_expressions();
    for(SgExpressionPtrList::iterator i = operandList.begin(); i!=operandList.end(); ++i)
    {
      translateOperand(*i);
    }
  }
  else
  {
    if(functionName.getString().find("_SIMD_") == string::npos)
    { 
      cerr << "function " << functionName << " is not supported in vector loop !" << endl;
      ROSE_ASSERT(false);
    }
  }
}

