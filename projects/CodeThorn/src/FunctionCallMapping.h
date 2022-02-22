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
  class FunctionCallInfo {
  public:
    std::string getFunctionName() {
      return funCallName;
    }
    bool isFunctionPointerCall();
    void setFunctionPointerCallFlag(bool);
    SgFunctionDeclaration* getFunctionDeclaration();
    std::string toString();
    SgFunctionType* funCallType=nullptr; // type of function to be called
    std::string funCallName; // non-qualified name of function 
    SgName mangledFunCallName;
    SgFunctionSymbol* functionSymbol=0;
    bool infoAvailable=false;
  private:
    bool _functionPointerCallFlag=false;
  };

  class FunctionCallMapping {
  public:
    FunctionCallMapping();
    void virtual computeFunctionCallMapping(SgNode* root);
    FunctionCallTargetSet virtual resolveFunctionCall(SgFunctionCallExp* funCall);
    std::string virtual toString();
    static void initDiagnostics();
    FunctionCallInfo virtual determineFunctionCallInfo(SgFunctionCallExp* fc);
    static bool isAstFunctionPointerCall(SgFunctionCallExp* fc);
    bool isFunctionPointerCall(SgFunctionCallExp* fc);
    
    /** access the class hierarchy for analyzing member function calls.
     *  @{
     */
    virtual void setClassHierarchy(ClassHierarchyWrapper* ch) { classHierarchy = ch; }
    virtual ClassHierarchyWrapper* getClassHierarchy() const { return classHierarchy; }
    /** @} */
    virtual void dumpFunctionCallInfo();
    virtual void dumpFunctionCallTargetInfo();
    virtual void dumpFunctionCallMapping();

    typedef std::unordered_set< std::string> ExternalFunctionNameContainerType;
    // generates info about external functions: header file;function name;complete function declaration (3 entries separated by ';')
    virtual ExternalFunctionNameContainerType getExternalFunctionNames();

    // returns nullptr if fcall is not in mapping
    FunctionCallInfo* getFunctionCallInfoPtr(SgFunctionCallExp*);
    // returns nullptr if fcall is nullptr or not in mapping
    FunctionCallTargetSet* getFunctionCallTargetSetPtr(SgFunctionCallExp*);
    // returns empty set if fcall is nullptr or not in mapping
    FunctionCallTargetSet getFunctionCallTargetSet(SgFunctionCallExp*);
    
    static std::string funDefTypeToString(SgFunctionDefinition* fn);
    bool isValidMapping();

  protected:
    FunctionCallInfo* getFunctionCallInfoPtrUnsafe(SgFunctionCallExp*);
    FunctionCallTargetSet* getFunctionCallTargetSetPtrUnsafe(SgFunctionCallExp*);
    virtual void collectRelevantNodes(SgNode* root);
    static Sawyer::Message::Facility logger;
    std::vector<SgFunctionCallExp*> funCallList;
    std::vector<SgFunctionDefinition*> funDefList;
    std::unordered_map<SgFunctionCallExp*,std::pair<FunctionCallInfo,FunctionCallTargetSet> > mapping;
    
  private:
    unsigned int _matchMode; // init in constructor
    ClassHierarchyWrapper* classHierarchy=nullptr;
    bool _mappingValidFlag=false;
  };

} // end of namespace CodeThorn

#endif
