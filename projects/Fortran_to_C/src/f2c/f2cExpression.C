#include "f2c.h"
#include "AstInterface.h"
#include "AstInterface_ROSE.h"

using namespace std;
using namespace SageInterface;
using namespace SageBuilder;
using namespace Fortran_to_C;

extern stack<SgStatement*> insertList;

void Fortran_to_C::translateDoubleVal(SgFloatVal* floatVal)
{
  string valString = floatVal->get_valueString();
  if(valString.find('d',0) != string::npos)
  {
    replace(valString.begin(),valString.end(),'d','e');
    SgDoubleVal* doubleVal = buildDoubleVal(atof(valString.c_str()));
    doubleVal->set_valueString(valString);
    replaceExpression(floatVal, doubleVal, false);
  }
  else if(valString.find('D',0) != string::npos)
  {
    replace(valString.begin(),valString.end(),'D','E');
    SgDoubleVal* doubleVal = buildDoubleVal(atof(valString.c_str()));
    doubleVal->set_valueString(valString);
    replaceExpression(floatVal, doubleVal, false);
  }
  else
  {
    valString = valString.append("f");
    floatVal->set_valueString(valString);
  }
}

void Fortran_to_C::translateExponentiationOp(SgExponentiationOp* expOp)
{
  SgScopeStatement* scope = getScope(expOp);
  SgExpression* operand1 = expOp->get_lhs_operand();
  ROSE_ASSERT(operand1); 
  SgExpression* operand2 = expOp->get_rhs_operand();
  ROSE_ASSERT(operand2); 

  insertSystemHeader("math.h",scope);
  SgExprListExp* exprListExp = buildExprListExp(deepCopy(operand1), deepCopy(operand2));
  SgFunctionCallExp* powerFunctionCall = buildFunctionCallExp("pow", expOp->get_type(), exprListExp, scope);
  replaceExpression(expOp,powerFunctionCall,false);
}

void Fortran_to_C::translateImplicitFunctionCallExp(SgFunctionCallExp* funcCallExp)
{
  SgScopeStatement* scope = getScope(funcCallExp);
  SgFunctionRefExp* functionRefExp = isSgFunctionRefExp(funcCallExp->get_function());
  ROSE_ASSERT(functionRefExp); 
  SgExprListExp* exprListExp = funcCallExp->get_args();
  SgExpressionPtrList operandList = exprListExp->get_expressions();
  SgName functionName = functionRefExp->get_symbol()->get_name();
// cout << "function name=" << functionName << endl;
  SgFunctionCallExp* newFunctionCallExp = NULL;
  // case 1: same argument for both C and Fortran
  SgName newFunctionName;
  if(convertMathFunctionName(functionName, &newFunctionName, funcCallExp->get_type()))
  {
    newFunctionCallExp = buildFunctionCallExp(newFunctionName, deepCopy(funcCallExp->get_type()), deepCopy(exprListExp), scope);
    ROSE_ASSERT(newFunctionCallExp);
    insertSystemHeader("math.h",scope);
    replaceExpression(funcCallExp, newFunctionCallExp,false);
  }
  else if(isMaxMinFunctionName(functionName, funcCallExp->get_type()))
  {
    ROSE_ASSERT(operandList.size() == 2);
    SgExpression* operand1 = operandList[0];
    SgExpression* operand2 = operandList[1];
    SgConditionalExp* conditionalExp = NULL;
    if(functionName.getString().find("max") != string::npos)
      conditionalExp = buildConditionalExp(buildGreaterThanOp(deepCopy(operand1), deepCopy(operand2)),deepCopy(operand1), deepCopy(operand2));
    else
      conditionalExp = buildConditionalExp(buildLessThanOp(deepCopy(operand1), deepCopy(operand2)),deepCopy(operand1), deepCopy(operand2));
    replaceExpression(funcCallExp, conditionalExp,false);
  }
  else if(convertConvertFunctionName(functionName, &newFunctionName, funcCallExp->get_type()))
  {
    const char* charPtr = functionName.str();
    ROSE_ASSERT(operandList.size() == 1);
    SgExpression* expr = operandList[0]; 
    SgCastExp* castExp;
    if((strncmp(charPtr, "achar",4) == 0))
      castExp  = buildCastExp(deepCopy(expr),buildCharType(),SgCastExp::e_C_style_cast);
    else if((strncmp(charPtr, "ichar",4) == 0))
      castExp  = buildCastExp(deepCopy(expr),buildIntType(),SgCastExp::e_C_style_cast);
    ROSE_ASSERT(castExp);
    replaceExpression(funcCallExp, castExp,false);
    
  }
  else
  {
    SgFunctionSymbol* calledFuncSymbol = lookupFunctionSymbolInParentScopes(functionName,scope);
    SgFunctionDeclaration* funcDecl = calledFuncSymbol->get_declaration();
    SgFunctionParameterList* calledArgsList = funcDecl->get_parameterList ();
    SgInitializedNamePtrList argList = calledArgsList->get_args();
    SgInitializedNamePtrList::iterator j = argList.begin();
    for(SgExpressionPtrList::iterator i=operandList.begin(); i != operandList.end(); ++i)
    {
      // Argument is scalar, or base address of array
      if(isSgVarRefExp(*i) != NULL)
      {
        SgVarRefExp* varRefExp = isSgVarRefExp(*i);
        if (isScalarType(varRefExp->get_symbol()->get_type()))
        {
//          cout << " this is a scalar type" << endl;
          // Passing the address of scalar variable
          SgAddressOfOp* addrOfExp = buildAddressOfOp(deepCopy(varRefExp));
          replaceExpression(varRefExp, addrOfExp, false);
          
        }
        else if (isSgArrayType(varRefExp->get_symbol()->get_type()))
        {

// This works only when the called function is translated.
//           SgType* calledType = (*j)->get_type();
// cout << "in ArrayType type:" << calledType->class_name() << endl;
//           if(isSgPointerType(calledType))
//           {
//             SgCastExp* castExp  = buildCastExp(deepCopy(varRefExp),calledType,SgCastExp::e_C_style_cast);
//             replaceExpression(varRefExp, castExp, false);
//           }

//           cout << " this is an array type" << varRefExp->get_symbol()->get_name() <<  endl;
          // doing nothing for base address of array 
        }
      }
      // Argument is an array element
      else if(isSgPntrArrRefExp(*i) != NULL)
      {
//           cout << " this is a array element" << endl;
          // Passing the address of array element
          SgPntrArrRefExp* pntrArrRefExp = isSgPntrArrRefExp(*i);
          SgAddressOfOp* addrOfExp = buildAddressOfOp(deepCopy(pntrArrRefExp));

// This works only when the called function is translated.
//          SgType* calledType = (*j)->get_type();
// cout << "in PntrArr type:" << calledType->class_name() << endl;
//          if(isSgPointerType(calledType))
//          {
//            SgCastExp* castExp  = buildCastExp(deepCopy(addrOfExp),calledType,SgCastExp::e_C_style_cast);
//            replaceExpression(pntrArrRefExp, castExp, false);
//          }
//          else

            replaceExpression(pntrArrRefExp, addrOfExp, false);
      }
      else if(isSgValueExp(*i) != NULL || isSgBinaryOp(*i) != NULL)
      {
          // Passing the constant arugment
          // A temp variable is required to store the constant value.
           SgExpression* valExp = isSgExpression(*i);
           SgScopeStatement* scope = getScope(valExp);
           string tmpName = generateUniqueVariableName(scope);
           SgType* type = (*i)->get_type();
           SgAssignInitializer* initializer = buildAssignInitializer(deepCopy(valExp),(valExp)->get_type());
           SgInitializedName* initializedName = buildInitializedName(tmpName, type, initializer);
           SgVariableDeclaration* declaration = buildVariableDeclaration(tmpName,type, initializer,getScope(valExp));
           insertList.push(declaration);
           fixVariableDeclaration(declaration,scope);
           SgAddressOfOp* addrOfExp = buildAddressOfOp(buildVarRefExp(tmpName));
           replaceExpression(valExp, addrOfExp, false);           
      }
      ++j;
    }
  }
}

void Fortran_to_C::insertSystemHeader(const string &headerName, SgScopeStatement* scope)
{
  SgGlobal* global = getGlobalScope(scope);
  ROSE_ASSERT(global);

  AttachedPreprocessingInfoType* info = global->get_attachedPreprocessingInfoPtr();
  if(info != NULL)
  {
    for(AttachedPreprocessingInfoType::iterator i=info->begin();i != info->end();++i)
    {
      if((*i)->getTypeOfDirective() == PreprocessingInfo::CpreprocessorIncludeDeclaration &&
         (*i)->getString().find(headerName) != string::npos)
        return;  // header file found
    }
  }
  else
  {
    // header not found.  insert it into AST.
    PreprocessingInfo* headerInfo = new PreprocessingInfo(PreprocessingInfo::CpreprocessorIncludeDeclaration,
                                            "#include <math.h>", "Transformation generated",0, 0, 0, PreprocessingInfo::before);
/*
TODO: insertHeader might have bug.
      global->get_attachedPreprocessingInfoPtr() won't be able to find the inserted header.
*/
//    PreprocessingInfo* headerInfo = insertHeader(headerName, PreprocessingInfo::before, true, global);
    headerInfo->set_file_info(global->get_file_info());
    global->addToAttachedPreprocessingInfo(headerInfo,PreprocessingInfo::before);
  }
}

bool Fortran_to_C::convertMathFunctionName(SgName inputName, SgName* output, SgType* inputType)
{
  const char* charPtr = inputName.str();
  if((strncmp(charPtr, "sin",3) == 0) || (strncmp(charPtr, "dsin",4) == 0))
    *output = "sin";
  else if((strncmp(charPtr, "cos",3) == 0) || (strncmp(charPtr, "dcos",4) == 0))
    *output = "cos";
  else if((strncmp(charPtr, "tan",3) == 0) || (strncmp(charPtr, "dtan",4) == 0))
    *output = "tan";
  else if((strncmp(charPtr, "acos",4) == 0) || (strncmp(charPtr, "dacos",5) == 0))
    *output = "acos";
  else if((strncmp(charPtr, "asin",4) == 0) || (strncmp(charPtr, "dasin",5) == 0))
    *output = "asin";
  else if((strncmp(charPtr, "atan",4) == 0) || (strncmp(charPtr, "datan",5) == 0))
    *output = "atan";
  else if((strncmp(charPtr, "atan2",5) == 0) || (strncmp(charPtr, "datan2",6) == 0))
    *output = "atan2";
  else if((strncmp(charPtr, "cosh",4) == 0) || (strncmp(charPtr, "dcosh",5) == 0))
    *output = "cosh";
  else if((strncmp(charPtr, "sinh",4) == 0) || (strncmp(charPtr, "dsinh",5) == 0))
    *output = "sinh";
  else if((strncmp(charPtr, "tanh",4) == 0) || (strncmp(charPtr, "dtanh",5) == 0))
    *output = "tanh";
  else if((strncmp(charPtr, "log",3) == 0) || (strncmp(charPtr, "dlog",4) == 0) || (strncmp(charPtr, "alog",4) == 0))
    *output = "log";
  else if((strncmp(charPtr, "log10",5) == 0) || (strncmp(charPtr, "dlog10",6) == 0) || (strncmp(charPtr, "alog10",6) == 0))
    *output = "log10";
  else if((strncmp(charPtr, "exp",3) == 0) || (strncmp(charPtr, "dexp",4) == 0))
    *output = "exp";
  else if((strncmp(charPtr, "sqrt",4) == 0) || (strncmp(charPtr, "dsqrt",5) == 0))
    *output = "sqrt";
  else if((strncmp(charPtr, "abs",3) == 0))
    {
      if(isSgTypeFloat(inputType) || isSgTypeDouble(inputType))
        *output = "fabs";
      else 
        *output = "abs";
    }
  else
  {
    return false;
  }
  return true;
}

bool Fortran_to_C::convertConvertFunctionName(SgName inputName, SgName* output, SgType* inputType)
{
  const char* charPtr = inputName.str();
  if((strncmp(charPtr, "achar",4) == 0) || (strncmp(charPtr, "ichar",4) == 0))
    return true;
  else 
    return false;
}

bool Fortran_to_C::isMaxMinFunctionName(SgName inputName, SgType* inputType)
{
  const char* charPtr = inputName.str();
  if((strncmp(charPtr, "min",3) == 0) || (strncmp(charPtr, "dmin",4) == 0) ||
     (strncmp(charPtr, "max",3) == 0) || (strncmp(charPtr, "dmax",4) == 0))
    return true;
  else
    return false;
}
