#ifndef LVANALYSIS_H
#define LVANALYSIS_H

// Author: Markus Schordan, 2013.

#include <iostream>
#include "VariableIdMapping.h"
#include "Labeler.h"
#include "CFAnalysis.h"
#include "LVLattice.h"
#include "DFAnalysisBase.h"
#include "WorkList.h"
#include "AnalysisAbstractionLayer.h"
#include "DFAstAttribute.h"
#include "LVTransferFunctions.h"

namespace CodeThorn {

class LVPropertyStateFactory : public PropertyStateFactory {
 public:
  LVPropertyStateFactory();
  virtual PropertyState* create();
  virtual ~LVPropertyStateFactory();
};

/*! 
  * \author Markus Schordan
  * \date 2013.
 */
class LVAnalysis : public DFAnalysisBase {
 public:
  LVAnalysis();
  ~LVAnalysis();
  //virtual void initializeExtremalValue(Lattice* element); 
 protected:
  virtual DFAstAttribute* createDFAstAttribute(Lattice* elem);
};

} // end namespace CodeThorn

#endif
