#ifndef FUNCTION_CALL_MAPPING_H
#define FUNCTION_CALL_MAPPING_H

#include <string>
#include <unordered_set>
#include <unordered_map>
#include "Labeler.h"
#include "FunctionCallTarget.h"
#include "ClassHierarchyGraph.h"

#include "FunctionCallMapping2.h"

struct ClassHierarchyWrapper;

namespace CodeThorn {
  /*!
   * \author Markus Schordan
   * \date 2019.
   */
  struct FunctionCallInfo {
    std::string getFunctionName() {
      return funCallName;
    }
    bool isFunctionPointerCall();
    void print();
    SgFunctionType* funCallType=nullptr; // type of function to be called
    std::string funCallName; // name of function to be called.
    SgName mangledFunCallName;
    SgFunctionSymbol* functionSymbol=0;
    bool infoAvailable=false;
  };

  class FunctionCallMapping {
  public:
    FunctionCallMapping();
    void virtual computeFunctionCallMapping(SgNode* root);
    FunctionCallTargetSet virtual resolveFunctionCall(SgFunctionCallExp* funCall);
    std::string virtual toString();
    static void initDiagnostics();
    FunctionCallInfo virtual determineFunctionCallInfo(SgFunctionCallExp* fc);
    static bool isFunctionPointerCall(SgFunctionCallExp* fc);
    
    /** access the class hierarchy for analyzing member function calls.
     *  @{
     */
    virtual void setClassHierarchy(ClassHierarchyWrapper* ch) { classHierarchy = ch; }
    virtual ClassHierarchyWrapper* getClassHierarchy() const { return classHierarchy; }
    /** @} */
    virtual void dumpFunctionCallInfo();
    virtual void dumpFunctionCallTargetInfo();

    typedef std::unordered_set< std::string> ExternalFunctionNameContainerType;
    virtual ExternalFunctionNameContainerType getExternalFunctionNames();

  protected:
    virtual void collectRelevantNodes(SgNode* root);
    static Sawyer::Message::Facility logger;
    std::vector<SgFunctionCallExp*> funCallList;
    std::vector<SgFunctionDefinition*> funDefList;
    std::unordered_map<SgFunctionCallExp*,FunctionCallTargetSet> mapping;

  private:
    unsigned int _matchMode=5;
    ClassHierarchyWrapper* classHierarchy=nullptr;
  };

} // end of namespace CodeThorn

#endif
