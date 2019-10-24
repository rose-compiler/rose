#include "FunctionCallTarget.h"
#include <sstream>

using namespace std;

bool CodeThorn::FunctionCallTarget::isDefinedFunction() const {
  return _funDef!=nullptr;
}
SgFunctionDeclaration* CodeThorn::FunctionCallTarget::getDeclaration() const {
  return _funDecl;
}
SgFunctionDefinition* CodeThorn::FunctionCallTarget::getDefinition() const {
  return _funDef;
}
void CodeThorn::FunctionCallTarget::setDeclaration(SgFunctionDeclaration* funDecl) {
  _funDecl=funDecl;
}
void CodeThorn::FunctionCallTarget::setDefinition(SgFunctionDefinition* funDef) {
  _funDef=funDef;
}
bool CodeThorn::FunctionCallTarget::isInitialized() const {
  return _funDef||_funDecl;
}

std::string CodeThorn::FunctionCallTarget::toString() const {
  stringstream ss;
  ss<<"FDECL:"<<_funDecl<<" FDEF:"<<_funDef;
  return ss.str();
}

bool CodeThorn::FunctionCallTarget::operator==(const CodeThorn::FunctionCallTarget& other) const {
  return _funDecl==other._funDecl && _funDef==other._funDef;
}

bool CodeThorn::FunctionCallTarget::operator!=(const CodeThorn::FunctionCallTarget& other) const {
  return !(*this==other);
}

std::size_t CodeThorn::FunctionCallTargetHashFunction::operator()(const CodeThorn::FunctionCallTarget& t) const
{
  std::size_t h1=((std::size_t)t.getDeclaration())>>2;
  std::size_t h2=((std::size_t)t.getDefinition())>>2;
  return h1 ^ h2;
}
