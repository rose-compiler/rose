#ifndef AST_UTIL_INTERFACE
#define AST_UTIL_INTERFACE

#include <functional>

class SgNode;
class SgFunctionDeclaration;
class AstUtilInterface{
  public:
    // Returns the collection of memory references modified, read, and invoked (as function
    // calls) as potential side effects of running the given ast, inside the given scope.
    // If any of the function pointers are null, the respective information is not collected.
    bool ComputeAstSideEffects(SgNode* ast, SgNode* scope,
                   std::function<bool(SgNode*, SgNode*)>* collect_mod_set,
                   std::function<bool(SgNode*, SgNode*)>* collect_read_set,
                   std::function<bool(SgNode*, SgNode*)>* collect_call_set);
};
#endif
