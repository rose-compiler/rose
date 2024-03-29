#ifndef REACHABILITYANALYSIS_H
#define REACHABILITYANALYSIS_H

#include "Labeler.h"
#include "FIConstAnalysis.h"
#include "PropertyValueTable.h"
#include "FIConstAnalysis.h"

namespace CodeThorn {
  class ReachabilityAnalysis {
  public:
    PropertyValueTable fiReachabilityAnalysis(CodeThorn::Labeler& labeler, CodeThorn::FIConstAnalysis& fiConstAnalysis);
  public:
    static int isIfWithLabeledAssert(SgNode* node);
    static int isConditionOfIfWithLabeledAssert(SgNode* node);
  };

}

#endif // REACHABILITYANALYSIS_H
