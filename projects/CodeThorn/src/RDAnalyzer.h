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

class RDAnalyzer : public DFAnalyzer<RDLattice> {
 public:
  RDAnalyzer();
  void attachResultsToAst(string attributeName);
  RDLattice transfer(Label lab, RDLattice element);
  // this function assumes that a pointer to an AST subtree representing a LHS of an assignment has been passed
  VariableIdSet determineLValueVariableIdSet(SgNode* node);
  typedef AnalyzerData::iterator iterator;
  iterator begin();
  iterator end();
  size_t size();
 private:
  void transfer_assignment(SgAssignOp* assignOp, Label& label, RDLattice& element);
};

#endif
