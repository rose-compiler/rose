#ifndef FUNCTION_CALL_TARGET_H
#define FUNCTION_CALL_TARGET_H

#include <string>
#include <algorithm>
#include <unordered_set>

class SgFunctionDeclaration;
class SgFunctionDefinition;

namespace CodeThorn {

class FunctionCallTarget {
 public:
  bool isDefinedFunction() const;
  SgFunctionDeclaration* getDeclaration() const;
  SgFunctionDefinition* getDefinition() const;
  void setDeclaration(SgFunctionDeclaration* funDecl);
  void setDefinition(SgFunctionDefinition* funDef);
  bool isInitialized() const;
  std::string toString() const;
  bool operator==(const FunctionCallTarget& other) const;
  bool operator!=(const FunctionCallTarget& other) const;
 private:
  SgFunctionDeclaration* _funDecl=nullptr;;
  SgFunctionDefinition* _funDef=nullptr;
};

 struct FunctionCallTargetHashFunction {
   std::size_t operator()(const FunctionCallTarget& t) const;
 };

 typedef std::unordered_set<FunctionCallTarget,FunctionCallTargetHashFunction> FunctionCallTargetSet;

} // end of namespace CodeThorn

#endif
