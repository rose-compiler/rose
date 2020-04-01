#ifndef THREADIFICATION_H
#define THREADIFICATION_H

class SgNode;
class SgFunctionDefinition;
class SgExpression;

#include "VariableIdMapping.h"

namespace CodeThorn {

  /* class for transforming declarations and uses of variables such
   that they can be used in threads in parallel. The thread-ids are
   assumed to be in an array and indexed by variable thread_id.  This
   transformation is used for transforming programs such that we can
   link the binary to the codethorn analyzer and invoke the binary in
   parallel to improve performance of the model checker. This
   transformation is specific to RERS problems (as of now).
  */
  class Threadification {
  public:
    Threadification(CodeThorn::VariableIdMapping* vim);
    virtual ~Threadification();
    // this transformation introduces new code as strings. It cannot be used for rewrites.
    void transform(SgNode* root);
  private:
    /*
      computes the variables of interest. For RERS programs these are
      integer variables, integer pointer variables, and they reside in
      the same file (SgFile) (not in a std-header file).
     */
    virtual CodeThorn::VariableIdSet determineVariablesOfInterest(SgNode* root);

    /* transform variable declarations: 
       int  var = ... ; ==> int*  var;
       int* var = ... ; ==> int** var;

       param: varIdSet: the set of variables for which the
       declarations are to be transformed
    */
    void transformVariableDeclarations(SgNode* root, CodeThorn::VariableIdSet varIdSet);
    void transformVariableDeclaration(SgVariableDeclaration* varDecl);

    /* transform uses of variables:
       var ==> var[thread_id]

       var[Expr] ==> var[thread_id][Expr] // if var is a pointer
       variable (do NOT transform if var is an array)

       param: varIdSet: the set of variables for which the use of each
       variable is to be transformed. This set is supposed to only
       include variables and pointer variables (but no arrays).
     */
    void transformVariableAccesses(SgNode* root, CodeThorn::VariableIdSet varIdSet);
    void transformVariableAccess(SgVarRefExp* varRefExp);
    void transformVariableAccess(SgPntrArrRefExp* arrRefExp, CodeThorn::VariableIdSet varIdSet);
    CodeThorn::VariableIdMapping* _variableIdMapping;
  };
} // end of namespace CodeThorn

#endif
