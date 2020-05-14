#ifndef FUNCTION_CALL_MAPPING2_H
#define FUNCTION_CALL_MAPPING2_H

#include <string>
#include <unordered_set>
#include <unordered_map>
#include "Labeler.h"
#include "FunctionCallTarget.h"
#include "ClassHierarchyGraph.h"

// struct ClassHierarchyWrapper;

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
    void computeFunctionCallMapping(SgProject*);

    FunctionCallTargetSet resolveFunctionCall(Label callLabel);
    std::string toString();

    /** access the class hierarchy for analyzing member function calls.
     *  @{
     */
    void setClassHierarchy(ClassHierarchyWrapper* ch) { classHierarchy = ch; }
    ClassHierarchyWrapper* getClassHierarchy() const { return classHierarchy; }
    /** @} */

    /** access the labeler.
     *  @{
     */
    void setLabeler(Labeler* lbler) { labeler = lbler; }
    Labeler* getLabeler() const { return labeler; }
    /** @} */

    static void initDiagnostics();
    
  protected:
    std::unordered_map<Label, FunctionCallTargetSet, HashLabel> mapping;
    Labeler* labeler;
  private:
    unsigned int _matchMode=3; // workaround mode
    ClassHierarchyWrapper* classHierarchy = nullptr;
  };
} // end of namespace CodeThorn

#endif /* FUNCTION_CALL_MAPPING2_H */
