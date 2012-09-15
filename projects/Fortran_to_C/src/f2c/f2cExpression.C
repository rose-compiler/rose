#include "f2c.h"
#include "AstInterface.h"
#include "AstInterface_ROSE.h"

using namespace std;
using namespace SageInterface;
using namespace SageBuilder;
using namespace Fortran_to_C;


void Fortran_to_C::translateImplicitFunctionCallExp(SgFunctionCallExp* funcCallExp)
{
  SgScopeStatement* scope = getScope(funcCallExp);
  SgFunctionRefExp* functionRefExp = isSgFunctionRefExp(funcCallExp->get_function());
  ROSE_ASSERT(functionRefExp); 
  SgExprListExp* exprListExp = funcCallExp->get_args();
  SgName functionName = functionRefExp->get_symbol()->get_name();

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
    SgExpressionPtrList operandList = exprListExp->get_expressions();
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
  else
  {
    return false;
  }
  return true;
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
