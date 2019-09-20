#ifndef FUNCTION_CALL_MAPPING_H
#define FUNCTION_CALL_MAPPING_H

#include <string>
#include <unordered_set>
#include <unordered_map>
#include "Labeler.h"
#include "FunctionCallTarget.h"

namespace CodeThorn {
  /*!
   * \author Markus Schordan
   * \date 2019.
   */
  class FunctionCallMapping {
  public:
    void computeFunctionCallMapping(SgNode* root);
    FunctionCallTargetSet resolveFunctionCall(SgFunctionCallExp* funCall);
    std::string toString();
    static void initDiagnostics();
  protected:
    static Sawyer::Message::Facility logger;
    std::unordered_map<SgFunctionCallExp*,FunctionCallTargetSet> mapping;
  };

} // end of namespace CodeThorn

#endif
