#include "f2c.h"

using namespace std;
using namespace SageInterface;
using namespace SageBuilder;
using namespace Fortran_to_C;


void Fortran_to_C::translateImplicitFunctionCallExp(SgFunctionCallExp* funcCallExp)
{
  SgFunctionRefExp* functionRefExp = isSgFunctionRefExp(funcCallExp->get_function());
  ROSE_ASSERT(functionRefExp); 
  SgExprListExp* exprListExp = funcCallExp->get_args();
  
  SgName functionName = functionRefExp->get_symbol()->get_name();
  SgName newFunctionName = convertImplicitFunctionName(functionName, funcCallExp->get_type());
  if(newFunctionName != NULL)
  {

  }
}

SgName Fortran_to_C::convertImplicitFunctionName(SgName inputName, SgType* inputType)
{
  SgName output = NULL;
  const char* charPtr = inputName.str();
  if((strncmp(charPtr, "sin",   3) == 0) ||
     (strncmp(charPtr, "cos",   3) == 0) ||
     (strncmp(charPtr, "tan",   3) == 0) ||
     (strncmp(charPtr, "acos",  4) == 0) ||
     (strncmp(charPtr, "asin",  4) == 0) ||
     (strncmp(charPtr, "atan",  4) == 0) ||
     (strncmp(charPtr, "atan2", 4) == 0) ||
     (strncmp(charPtr, "cosh",  4) == 0) ||
     (strncmp(charPtr, "sinh",  4) == 0) ||
     (strncmp(charPtr, "tanh",  4) == 0) ||
     (strncmp(charPtr, "exp",   4) == 0)}
  {
    return inputName;
  }
  else if(strncmp(charPtr, "frexp", 4) == 0)
  {
  }
  else if(strncmp(charPtr, "ldexp", 4) == 0)
  {
  }
  else if(strncmp(charPtr, "log", 4) == 0)
  {
  }
  else if(strncmp(charPtr, "log10", 4) == 0)
  {
  }
  else if(strncmp(charPtr, "modf", 4) == 0)
  {
  }
  else if(strncmp(charPtr, "pow", 4) == 0)
  {
  }
  else if(strncmp(charPtr, "sqrt", 4) == 0)
  {
  }
  else if(strncmp(charPtr, "ceil", 4) == 0)
  {
  }
  else if(strncmp(charPtr, "floor", 4) == 0)
  {
  }
  else if(strncmp(charPtr, "fmod", 4) == 0)
  {
  }
  else
  {
    return NULL;
  }
  return output;
}
