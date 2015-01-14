#include "CodeGenerator.h"
#include "TransformationSequence.h"

#include "SgNodeHelper.h"

using namespace std;

void Backstroke::CodeGenerator::forwardCodeTransformation(SgFunctionDefinition* funDef) {
  TransformationSequence ts;
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
