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
  struct FunctionCallInfo {
    SgFunctionType* funCallType=nullptr; // type of function to be called
    SgName mangledFunCallTypeName;
    std::string funCallName; // name of function to be called.
    bool isFunctionPointerCall();
    void print();
  };

  class FunctionCallMapping {
  public:
    void computeFunctionCallMapping(SgNode* root);
    FunctionCallTargetSet resolveFunctionCall(SgFunctionCallExp* funCall);
    std::string toString();
    static void initDiagnostics();
    FunctionCallInfo determineFunctionCallInfo(SgFunctionCallExp* fc);

  protected:
    static Sawyer::Message::Facility logger;
    std::unordered_map<SgFunctionCallExp*,FunctionCallTargetSet> mapping;
  private:
    unsigned int _matchMode=3; // workaround mode
  };

} // end of namespace CodeThorn

#endif
