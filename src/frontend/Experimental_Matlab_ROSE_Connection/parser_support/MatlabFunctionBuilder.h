#ifndef MATLAB_FUNCTION_BUILDER_H
#define MATLAB_FUNCTION_BUILDER_H

#include <string>

#include "rose.h"

class StatementList;

class MatlabFunctionBuilder
{
 private:
  SgFunctionParameterList* parameterList;
  StatementList*           statementList;
  SgScopeStatement* scope;
  std::string functionName;
  SgExprListExp* returnList;

 public:
  MatlabFunctionBuilder(SgFunctionParameterList* parameterList, StatementList* statementList, SgScopeStatement* scope);

  MatlabFunctionBuilder(StatementList* statementList, SgScopeStatement* scope);

  void set_function_name(std::string name);

  void set_return_list(SgExprListExp *returnList);

  SgFunctionDeclaration* build_function();
};
#endif
