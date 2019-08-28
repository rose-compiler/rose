#include "FunctionCallTarget.h"

bool FunctionCallTarget::isDefinedFunction() {
  return _funDef!=nullptr;
}
SgDeclarationStatement* FunctionCallTarget::getDeclaration() {
  return _funDecl;
}
SgFunctionDefinition* FunctionCallTarget::getDefinition() {
  return _funDef;
}
void FunctionCallTarget::setDeclaration(SgDeclarationStatement* funDecl) {
  _funDecl=funDecl;
}
void FunctionCallTarget::setDefinition(SgFunctionDefinition* funDef) {
  _funDef=funDef;
}
