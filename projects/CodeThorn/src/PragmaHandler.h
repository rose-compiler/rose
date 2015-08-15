#ifndef PRAGMA_HANDLER_H
#define PRAGMA_HANDLER_H

#include <string>
#include <vector>

#include "Analyzer.h"

class PragmaHandler {
public:
  size_t handlePragmas(SgProject* sageProject, CodeThorn::Analyzer* analyzer);
  std::string option_specialize_fun_name;
private:
  size_t extractAndParsePragmas(SgNode* root, CodeThorn::Analyzer* analyzer);
  void performSpecialization(SgProject* sageProject, CodeThorn::Analyzer* analyzer);
  std::vector<int> option_specialize_fun_param_list;
  std::vector<int> option_specialize_fun_const_list;
};

#endif

