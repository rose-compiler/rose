#ifndef UTILS_H
#define UTILS_H

#include <string>

#include "rose.h"

/// copied from Matlab project
namespace SageUtil
{
#if 0
  /*
    Build Empty parameters
   */
  SgExprListExp* buildEmptyParams();

  /*
Returns a string of types separated by commas from a list of types
*/
  std::string getFunctionCallTypeString(Rose_STL_Container<SgType*> typeList);

  /*
    Replace all variables in the given SgStatement that have the same name as given variable by the new expression
  */
  void replaceVariable(SgStatement *statement, SgVarRefExp *variable, SgExpression *expression);

/*
 * Get the rows of a SgMatrixExp as a vector of ExprListExp
*/
  Rose_STL_Container<SgExprListExp*> getMatrixRows(SgMatrixExp *matrix);


  /*
  *Create a template variable declaration with your provided types
  *Eg. You may want to create Matrix<int> x;
  *And you may not have an existing symbol for Matrix<int>
  */
  SgVariableDeclaration *createOpaqueTemplateObject(std::string varName, std::string className, std::string type, SgScopeStatement *scope);

  /*
   *Create a member function call
   *This function looks for the function symbol in the given className
   *The function should exist in the class
   *The class should be #included or present in the source file parsed by frontend
   */
  SgFunctionCallExp *createMemberFunctionCall(std::string className, SgExpression *objectExpression, std::string functionName, SgExprListExp *params, SgScopeStatement *scope);


  /*
   * Returns start:stride:end as a SgExprListExp
   */
  SgExprListExp *getExprListExpFromRangeExp(SgRangeExp *rangeExp);

  SgFunctionCallExp *createFunctionCall(std::string functionName, SgScopeStatement *scope, SgExprListExp *parameters);


  /// returns name of symbol
  SgName nameOf(const SgSymbol& varsy);

  /// returns name of symbol
  SgName nameOf(const SgVarRefExp& var_ref);

  /// returns name of symbol
  SgName nameOf(const SgVarRefExp* var_ref);
#endif

namespace {

  /// returns the argument list of a function call.
  /// @{
  static inline
  SgExpressionPtrList& arglist(const SgCallExpression& call)
  {
    SgExprListExp& args = sg::deref(call.get_args());

    return args.get_expressions();
  }

  static inline
  SgExpressionPtrList& arglist(const SgCallExpression* call)
  {
    return arglist(sg::deref(call));
  }
  /// @}

  /// returns the n-th argument of a function call.
  /// @{
  static inline
  SgExpression& argN(const SgCallExpression& call, size_t n)
  {
    return sg::deref(arglist(call).at(n));
  }

  static inline
  SgExpression& argN(const SgCallExpression* call, size_t n)
  {
    return argN(sg::deref(call), n);
  }
  /// @}

  static inline
  std::string name(const SgFunctionDeclaration& n)
  {
    return n.get_name().getString();
  }

  /// A key declaration is a declaration that uniquely identifies
  /// a set of declarations (forward, internal, definition, ..).
  /// The key is the defining declaration (if available),
  /// or the first non-defining declaration otherwise.
  template <class SageDecl>
  static inline
  SageDecl& keyDecl(const SageDecl& dcl)
  {
    SgDeclarationStatement* keydcl = dcl.get_definingDeclaration();
    if (keydcl) return SG_ASSERT_TYPE(SageDecl, *keydcl);

    keydcl = dcl.get_firstNondefiningDeclaration();
    return SG_ASSERT_TYPE(SageDecl, sg::deref(keydcl));
  }

  /// unparses a node (possibly null)
  static inline
  std::string str(const SgNode* n)
  {
    return (n? n->unparseToString() : std::string("<null>"));
  }

  /// Throws an exception if !success.
  template <class ErrorClass = std::runtime_error>
  inline
  void chk(bool success, std::string a, std::string b = "", std::string c = "")
  {
    if (!success) throw ErrorClass(a + b + c);
  }

  static inline
  SgType& skipTypeModifier(SgType& t)
  {
    SgModifierType* modty = isSgModifierType(&t);
    if (!modty) return t;

    return skipTypeModifier(SG_DEREF(modty->get_base_type()));
  }
} // anonymous namespace
} // namespace SageUtil
#endif
