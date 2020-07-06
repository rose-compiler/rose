#ifndef MemANALYSIS_H
#define MemANALYSIS_H

// Author: Markus Schordan, 2013.

#include <iostream>
#include "VariableIdMapping.h"
#include "Labeler.h"
#include "CFAnalysis.h"
#include "MemPropertyState.h"
#include "DFAnalysisBase.h"
#include "AnalysisAbstractionLayer.h"
#include "DFAstAttribute.h"
#include "MemTransferFunctions.h"

namespace CodeThorn {

class MemPropertyStateFactory : public PropertyStateFactory {
 public:
  MemPropertyStateFactory();
  virtual PropertyState* create();
  virtual ~MemPropertyStateFactory();
};

/*!
  * \author Markus Schordan
  * \date 2013.
 */
class MemAnalysis : public DFAnalysisBase {
 public:
  MemAnalysis();
  ~MemAnalysis();
  virtual void initializeExtremalValue(Lattice* element);
  void initialize(SgProject* root);

 protected:
};

} // end of namespace CodeThorn

#endif
