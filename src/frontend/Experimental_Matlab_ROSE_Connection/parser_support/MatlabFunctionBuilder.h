#ifndef MATLAB_FUNCTION_BUILDER_H
#define MATLAB_FUNCTION_BUILDER_H

#include <string>

// DQ (11/13/2017): The direct use of rose.h within ROSE infrastructure is a violation of policy.
// #include "rose.h"
#include <rosePublicConfig.h>
// #include <sage3basic.h>

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
