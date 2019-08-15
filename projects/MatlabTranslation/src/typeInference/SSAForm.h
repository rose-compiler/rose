#ifndef SSA_FORM_H
#define SSA_FORM_H

class SgFunctionDefinition;
class SgProject;

namespace MatlabAnalysis
{
  /// converts a function to SSA form
  void convertToSSA  (SgFunctionDefinition* def);
  void convertToSSA  (SgProject* proj);

  /// converts a function from SSA to normal form
  void convertFromSSA(SgFunctionDefinition* def);
  void convertFromSSA(SgProject* proj);
}

#endif /* SSA_FORM_H */
