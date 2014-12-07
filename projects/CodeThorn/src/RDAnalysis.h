#ifndef RDANALYZER_H
#define RDANALYZER_H

// Author: Markus Schordan, 2013.

#include <iostream>
#include "VariableIdMapping.h"
#include "Labeler.h"
#include "CFAnalyzer.h"
#include "RDLattice.h"
#include "DFAnalyzer.h"
#include "WorkList.h"
#include "AnalysisAbstractionLayer.h"
#include "DFAstAttribute.h"

using namespace std;
using namespace CodeThorn;

#include "RDTransferFunctions.hpp"
/*! 
  * \author Markus Schordan
  * \date 2013.
 */
class RDAnalysis : public ProgramAnalysis {
 public:
  RDAnalysis();
  ~RDAnalysis();
  virtual void initializeSolver();
 protected:
  virtual DFAstAttribute* createDFAstAttribute(Lattice* elem);
};

#endif
