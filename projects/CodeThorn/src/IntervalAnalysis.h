#ifndef INTERVALANALYZER_H
#define INTERVALANALYZER_H

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
#include "IntervalPropertyState.h"

using namespace std;
using namespace CodeThorn;

#include "ProgramAnalysis.h"
#include "IntervalPropertyState.h"
/*! 
  * \author Markus Schordan
  * \date 2013.
 */
class IntervalAnalysis : public ProgramAnalysis {
 public:
  IntervalAnalysis();
  ~IntervalAnalysis();
  void initializeExtremalValue(Lattice* element);
 protected:
  virtual DFAstAttribute* createDFAstAttribute(IntervalPropertyState* elem);
};

#endif
