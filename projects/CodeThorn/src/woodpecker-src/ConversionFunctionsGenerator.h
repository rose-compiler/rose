#ifndef CONVERSION_FUNCTIONS_GENERATOR_H
#define CONVERSION_FUNCTIONS_GENERATOR_H

#include <string>
#include <set>
#include <iostream>

class ConversionFunctionsGenerator {
 public:
  std::string generateCodeForGlobalVarAdressMaps(std::set<std::string> vars);
  void generateFile(SgProject* root, std::string conversionFunctionsFileName);
};

#endif
