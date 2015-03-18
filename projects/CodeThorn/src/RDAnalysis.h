#ifndef RDANALYSIS_H
#define RDANALYSIS_H

// Author: Markus Schordan, 2013.

#include <iostream>
#include "VariableIdMapping.h"
#include "Labeler.h"
#include "CFAnalyzer.h"
#include "RDLattice.h"
#include "DFAnalysisBase.h"
#include "WorkList.h"
#include "AnalysisAbstractionLayer.h"
#include "DFAstAttribute.h"
#include "RDTransferFunctions.h"

using namespace std;
using namespace CodeThorn;

namespace SPRAY {

class RDPropertyStateFactory : public PropertyStateFactory {
 public:
  RDPropertyStateFactory();
  virtual PropertyState* create();
  virtual ~RDPropertyStateFactory();
};

/*! 
  * \author Markus Schordan
  * \date 2013.
 */
class RDAnalysis : public DFAnalysisBase {
 public:
  RDAnalysis();
  ~RDAnalysis();
  virtual void initializeExtremalValue(Lattice* element);
  // allows to define a pointer analysis to be used in resolving pointer-expressions
 protected:
  virtual DFAstAttribute* createDFAstAttribute(Lattice* elem);
};

} // end of namespace SPRAY

#endif
