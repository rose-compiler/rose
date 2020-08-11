#ifndef EStateANALYSIS_H
#define EStateANALYSIS_H

// Author: Markus Schordan, 2013.

#include <iostream>
#include "VariableIdMapping.h"
#include "Labeler.h"
#include "CFAnalysis.h"
#include "EState.h"
#include "DFAnalysisBase.h"
#include "AnalysisAbstractionLayer.h"
#include "DFAstAttribute.h"
#include "EStateTransferFunctions.h"

namespace CodeThorn {

class EStateFactory : public PropertyStateFactory {
 public:
  EStateFactory();
  virtual PropertyState* create();
  virtual ~EStateFactory();
};

/*!
  * \author Markus Schordan
  * \date 2013.
 */
class EStateAnalysis : public DFAnalysisBase {
 public:
  EStateAnalysis();
  ~EStateAnalysis();
  virtual void initializeExtremalValue(Lattice* element);
  void initialize(SgProject* root);

 protected:
};

} // end of namespace CodeThorn

#endif
