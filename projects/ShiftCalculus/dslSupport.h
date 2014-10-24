

namespace DSL_Support
   {
     bool isMatchingClassType(SgType* type, const std::string & name, bool isTemplateClass);
     bool isMatchingMemberFunction(SgMemberFunctionRefExp* memberFunctionRefExp, const std::string & name, bool isTemplateInstantiation);

  // SgFunctionCallExp* buildMemberFunctionCall(SgExpression* expressionRoot, SgType* type, const std::string & memberFunctionName, SgExpression* expression, bool isOperator);
     SgFunctionCallExp* buildMemberFunctionCall(SgExpression* expressionRoot, const std::string & memberFunctionName, SgExpression* expression, bool isOperator);

     SgFunctionCallExp* buildMemberFunctionCall(SgVariableSymbol* variableSymbol, const std::string & memberFunctionName, SgExpression* expression, bool isOperator);

     SgVariableDeclaration* buildDataPointer(const std::string & pointerVariableName, SgVariableSymbol* variableSymbol, SgScopeStatement* outerScope);

   }
