#ifndef INTERVALANALYZER_H
#define INTERVALANALYZER_H

// Author: Markus Schordan, 2013.

#include <iostream>
#include "VariableIdMapping.h"
#include "Labeler.h"
#include "CFAnalyzer.h"
#include "RDLattice.h"
#include "DFAnalysis2.h"
#include "WorkList.h"
#include "AnalysisAbstractionLayer.h"
#include "DFAstAttribute.h"
#include "IntervalPropertyState.h"

using namespace std;
using namespace CodeThorn;

#include "DFAnalysis2.h"
#include "IntervalPropertyState.h"
/*! 
  * \author Markus Schordan
  * \date 2013.
 */
class IntervalAnalysis : public DFAnalysis2 {
 public:
  IntervalAnalysis();
  ~IntervalAnalysis();
  void initializeExtremalValue(Lattice* element);
  void initializeTransferFunctions();
 protected:
  virtual DFAstAttribute* createDFAstAttribute(IntervalPropertyState* elem);
  NumberIntervalLattice* _numberIntervalLattice;
};

#endif
