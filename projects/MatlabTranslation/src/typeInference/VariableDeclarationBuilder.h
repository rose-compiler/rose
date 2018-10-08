#ifndef VARIABLE_DECLARATION_BUILDER_H
#define VARIABLE_DECLARATION_BUILDER_H

class SgFunctionDeclaration;

namespace MatlabAnalysis
{
  void buildVariableDeclarations(SgFunctionDeclaration* functionDeclaration);
}

#endif
