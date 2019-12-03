#ifndef FUNCTION_CALL_TARGET_H
#define FUNCTION_CALL_TARGET_H

#include <string>
#include <algorithm>
#include <unordered_set>

class SgFunctionDeclaration;
class SgFunctionDefinition;
class SgFunctionType;

namespace CodeThorn {

class FunctionCallTarget {
 public:
  FunctionCallTarget();
  FunctionCallTarget(SgFunctionDefinition* funDef);
  bool isDefinedFunction() const;
  SgFunctionDeclaration* getDeclaration() const;
  SgFunctionDefinition* getDefinition() const;
  void setDeclaration(SgFunctionDeclaration* funDecl);
  void setDefinition(SgFunctionDefinition* funDef);
  bool isInitialized() const;
  std::string toString() const;
  bool operator==(const FunctionCallTarget& other) const;
  bool operator!=(const FunctionCallTarget& other) const;
  std::string getFunctionName();
  SgFunctionType* getFunctionType();
  SgName getMangledFunctionTypeName();
  void print();
 private:
  SgFunctionDeclaration* _funDecl=nullptr;
  SgFunctionDefinition* _funDef=nullptr;
  SgFunctionType* funCallType=nullptr; // type of function to be called
  SgName mangledFunCallTypeName;
  std::string funCallName; // name of function to be called.
};

 struct FunctionCallTargetHashFunction {
   std::size_t operator()(const FunctionCallTarget& t) const;
 };

 typedef std::unordered_set<FunctionCallTarget,FunctionCallTargetHashFunction> FunctionCallTargetSet;

} // end of namespace CodeThorn

#endif
