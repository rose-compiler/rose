#ifndef CODE_GENERATOR_H
#define CODE_GENERATOR_H

class SgProject;
class SgFunctionDefinition;

namespace Backstroke {
  class CodeGenerator {
  public:
    void generateCode(SgProject* root);
  private:
    void forwardCodeTransformation(SgFunctionDefinition* funDef);
  };
};

#endif
