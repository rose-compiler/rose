#ifndef LVANALYSIS_H
#define LVANALYSIS_H

// Author: Markus Schordan, 2013.

#include <iostream>
#include "VariableIdMapping.h"
#include "Labeler.h"
#include "CFAnalyzer.h"
#include "LVLattice.h"
#include "DFAnalysis2.h"
#include "WorkList.h"
#include "AnalysisAbstractionLayer.h"
#include "DFAstAttribute.h"

using namespace std;
using namespace CodeThorn;

#include "LVTransferFunctions.h"

namespace SPRAY {

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
class LVAnalysis : public DFAnalysis2 {
 public:
  LVAnalysis();
  ~LVAnalysis();
  virtual void initializeExtremalValue(Lattice* element); 
 protected:
  virtual DFAstAttribute* createDFAstAttribute(Lattice* elem);
};

} // end namespace SPRAY

#endif
