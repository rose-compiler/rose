#include "CodeGenerator.h"
#include "TransformationSequence.h"

#include "SgNodeHelper.h"

using namespace std;

Backstroke::CodeGenerator::CodeGenerator():_commandLineOptions(0) {
}

Backstroke::CodeGenerator::CodeGenerator(CommandLineOptions* clo):_commandLineOptions(clo) {
}

void Backstroke::CodeGenerator::setCommandLineOptions(CommandLineOptions* clo) {
  _commandLineOptions=clo;
}

void Backstroke::CodeGenerator::forwardCodeTransformation(SgFunctionDefinition* funDef) {
  TransformationSequence ts;
  if(_commandLineOptions) {
    ts.setShowTransformationTrace(_commandLineOptions->optionShowTransformationTrace());
  }
  ts.create(funDef);
  ts.apply();
}

void Backstroke::CodeGenerator::generateCode(SgProject* root) {
  list<SgFunctionDefinition*> functionDefs=SgNodeHelper::listOfFunctionDefinitions(root);
  for(list<SgFunctionDefinition*>::iterator i=functionDefs.begin();i!=functionDefs.end();++i) {
    SgFunctionDefinition* forwardFunDef=*i;
    forwardCodeTransformation(forwardFunDef);
  }
  root->unparse(0,0);
}
