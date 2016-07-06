#ifndef NORMALIZATION_H
#define NORMALIZATION_H

#include "sage3basic.h"
#include "Normalization.h"

namespace SPRAY {

  void Normalization::normalizeAst(SgNode* root) {
    convertAllForsToWhiles(root);
#if 0
    changeBreakStatementsToGotos(root);
    ExtractFunctionArguments efa;
    if(!efa.IsNormalized(root)) {
      cout<<"STATUS: Normalizing function call arguments."<<endl;
      efa.NormalizeTree(root,true);
    }
    FunctionCallNormalization fn;
    cout<<"STATUS: Normalizing function calls in expressions."<<endl;
    fn.visit(root);
#endif
  }

  void Normalization::convertAllForsToWhiles (SgNode* top) {
    SageInterface::convertAllForsToWhiles (top);
  }
 
  void Normalization::changeBreakStatementsToGotos (SgStatement *loopOrSwitch) {
    SageInterface::changeBreakStatementsToGotos(loopOrSwitch);
  }

} // end of namespace SPRAY

#endif
