#ifndef RDANALYSIS_H
#define RDANALYSIS_H

// Author: Markus Schordan, 2013.

#include <iostream>
#include "VariableIdMapping.h"
#include "Labeler.h"
#include "CFAnalyzer.h"
#include "RDLattice.h"
#include "ProgramAnalysis.h"
#include "WorkList.h"
#include "AnalysisAbstractionLayer.h"
#include "DFAstAttribute.h"

using namespace std;
using namespace CodeThorn;

#include "RDTransferFunctions2.h"

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
class RDAnalysis : public ProgramAnalysis {
 public:
  RDAnalysis();
  ~RDAnalysis();
  virtual void initializeExtremalValue(Lattice* element); 
 protected:
  virtual DFAstAttribute* createDFAstAttribute(Lattice* elem);
};

#endif
