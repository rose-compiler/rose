#ifndef UTILS_H
#define UTILS_H

#include <string>

#include "rose.h"
#include "sageInterface.h"

namespace RoseUtils
{
  template <class T>
  static inline
  T* deepCopy(T* orig)
  {
    T* res = SageInterface::deepCopy(orig);

    //~ std::cerr << "clone " << orig << " -> " << res;
    return res;
  }

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

  /// returns the argument list of a function call
  SgExpressionPtrList& arglist(SgCallExpression* call);

  /// returns the argument list of a function call
  static inline
  SgExpressionPtrList& arglist(SgCallExpression& call)
  {
    return arglist(&call);
  }

  /// returns the n-th argument of a function call
  SgExpression* argN(SgCallExpression*, size_t);

  /// returns the n-th argument of a function call
  static inline
  SgExpression& argN(SgCallExpression& call, size_t n)
  {
    SgExpression* arg = argN(&call, n);

    ROSE_ASSERT(arg);
    return *arg;
  }

  static inline
  std::string str(const SgNode* n)
  {
    return (n? n->unparseToString() : std::string("<null>"));
  }
}
#endif
