#ifndef FUNCTION_CALL_TARGET_H
#define FUNCTION_CALL_TARGET_H

class SgDeclarationStatement;
class SgFunctionDefinition;

class FunctionCallTarget {
 public:
  bool isDefinedFunction();
  SgDeclarationStatement* getDeclaration();
  SgFunctionDefinition* getDefinition();
  void setDeclaration(SgDeclarationStatement* funDecl);
  void setDefinition(SgFunctionDefinition* funDef);
 private:
  SgDeclarationStatement* _funDecl=nullptr;;
  SgFunctionDefinition* _funDef=nullptr;
};

#endif
