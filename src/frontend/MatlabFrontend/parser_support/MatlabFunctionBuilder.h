#ifndef MATLAB_FUNCTION_BUILDER_H
#define MATLAB_FUNCTION_BUILDER_H

#include <string>

class MatlabFunctionBuilder
{
 private:
  SgFunctionParameterList* parameterList;
  SgStatement* statementList;
  SgScopeStatement* scope;
  std::string functionName;
  SgExprListExp* returnList;

 public:
  MatlabFunctionBuilder(SgFunctionParameterList* parameterList, SgStatement* statementList, SgScopeStatement* scope);

  MatlabFunctionBuilder(SgStatement* statementList, SgScopeStatement* scope);

  void set_function_name(std::string name);

  void set_return_list(SgExprListExp *returnList);

  SgFunctionDeclaration* build_function();
};
#endif
