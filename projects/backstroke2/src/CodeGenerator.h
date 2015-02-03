#ifndef CODE_GENERATOR_H
#define CODE_GENERATOR_H

class SgProject;
class SgFunctionDefinition;

#include "CommandLineOptions.h"

namespace Backstroke {
  class CodeGenerator {
  public:
    CodeGenerator();
    CodeGenerator(CommandLineOptions* clo);
    void setCommandLineOptions(CommandLineOptions* clo);
    void generateCode(SgProject* root);
  private:
    void forwardCodeTransformation(SgFunctionDefinition* funDef);
    CommandLineOptions* _commandLineOptions;
  };
};

#endif
