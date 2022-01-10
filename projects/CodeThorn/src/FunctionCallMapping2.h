#ifndef FUNCTION_CALL_MAPPING2_H
#define FUNCTION_CALL_MAPPING2_H

#include <string>
#include <unordered_set>
#include <unordered_map>
#include "Labeler.h"
#include "FunctionCallTarget.h"
#include "ClassHierarchyAnalysis.h"


namespace CodeThorn {

  // \todo when ROSE allows C++11, move this to Labeler.h as std::hash<CodeThorn::Label> ...
  //       C++11 and newer allow template specializations for user defined
  //       types to be made within namespace std.
  struct HashLabel
  {
    std::size_t operator()(Label lbl) const noexcept
    {
      std::hash<decltype(std::declval<Label>().getId())> hashFun;

      return hashFun(lbl.getId());
    }
  };

  /// tests if n occurs as part of a template
  // \todo consider only labeling non-template code..
  bool insideTemplatedCode(const SgNode* n);

  /*!
   * \author Markus Schordan
   * \date 2019.
   */
  class FunctionCallMapping2 {
  public:
    FunctionCallMapping2(Labeler* labeler, ClassAnalysis* classAnalysis, VirtualFunctionAnalysis* virtualFunctions)
    : mapping(), _labeler(labeler), _classAnalysis(classAnalysis), _virtualFunctions(virtualFunctions)
    {
      ASSERT_not_null(_labeler);
      ASSERT_not_null(_classAnalysis);
      ASSERT_not_null(_virtualFunctions);
    }

    ~FunctionCallMapping2() = default;

    void computeFunctionCallMapping(SgProject*);

    FunctionCallTargetSet resolveFunctionCall(Label callLabel);
    std::string toString();

    /** access the class hierarchy for analyzing member function calls.
     *  @{
     */
    ClassAnalysis* getClassAnalysis() const { return _classAnalysis; }
    /** @} */

    /** access the class hierarchy for analyzing member function calls.
     *  @{
     */
    VirtualFunctionAnalysis* getVirtualFunctions() const { return _virtualFunctions; }
    /** @} */

    /** access the labeler.
     *  @{
     */
    Labeler* getLabeler() const { return _labeler; }
    /** @} */

    // static void initDiagnostics(); \use CodeThorn diagnostics

  protected:
    std::unordered_map<Label, FunctionCallTargetSet, HashLabel> mapping;
    Labeler* _labeler;
    ClassAnalysis* _classAnalysis;
    VirtualFunctionAnalysis* _virtualFunctions;
  private:
    unsigned int _matchMode=3; // workaround mode

    FunctionCallMapping2()                                       = delete;
    FunctionCallMapping2(const FunctionCallMapping2&)            = delete;
    FunctionCallMapping2(FunctionCallMapping2&&)                 = delete;
    FunctionCallMapping2& operator=(const FunctionCallMapping2&) = delete;
    FunctionCallMapping2& operator=(FunctionCallMapping2&&)      = delete;
  };
} // end of namespace CodeThorn

#endif /* FUNCTION_CALL_MAPPING2_H */
