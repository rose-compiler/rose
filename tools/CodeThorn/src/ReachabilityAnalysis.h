#ifndef REACHABILITYANALYSIS_H
#define REACHABILITYANALYSIS_H

#include "Labeler.h"
#include "FIConstAnalysis.h"
#include "PropertyValueTable.h"

class ReachabilityAnalysis {
 public:
  PropertyValueTable fiReachabilityAnalysis(Labeler& labeler, FIConstAnalysis& fiConstAnalysis);
 public:
  static int isIfWithLabeledAssert(SgNode* node);
  static int isConditionOfIfWithLabeledAssert(SgNode* node);
};

#endif // REACHABILITYANALYSIS_H
