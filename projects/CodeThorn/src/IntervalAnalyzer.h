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

using namespace std;
using namespace CodeThorn;

#include "RDTransferFunctions.hpp"
/*! 
  * \author Markus Schordan
  * \date 2013.
 */
class IntervalAnalyzer : public DFAnalyzer<RDLattice> {
 public:
  IntervalAnalyzer();
  ~IntervalAnalyzer();
 protected:
  virtual DFAstAttribute* createDFAstAttribute(RDLattice* elem);
};

#endif
