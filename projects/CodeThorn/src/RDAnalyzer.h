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

using namespace std;
using namespace CodeThorn;

#include "RDTransferFunctions.hpp"
/*! 
  * \author Markus Schordan
  * \date 2013.
 */
class RDAnalyzer : public DFAnalyzer<RDLattice> {
 public:
  RDAnalyzer();
  ~RDAnalyzer();
  void attachInInfoToAst(string attributeName);
  void attachOutInfoToAst(string attributeName);
  RDLattice transfer(Label lab, RDLattice element);

  typedef AnalyzerData::iterator iterator;
  iterator begin();
  iterator end();
  size_t size();

 protected:
  void attachInfoToAst(string attributeName,bool isInInfo);
};

#endif
