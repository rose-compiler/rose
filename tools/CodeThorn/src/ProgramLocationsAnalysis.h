#ifndef PROGRAM_LOCATIONS_ANALYSIS_H
#define PROGRAM_LOCATIONS_ANALYSIS_H

#include <string>
#include "Labeler.h"
#include <iostream>
#include <cstdint>

namespace CodeThorn {
  class ProgramLocationsAnalysis {
  public:
    // compute all locations at which a null pointer dereference can potentially happen
    // expressions: exp->exp, *exp
    LabelSet pointerDereferenceLocations(Labeler& labeler);
    // stack allocated arrays
    LabelSet arrayAccessLocations(Labeler& labeler);
    // initializers and RHS of assignments (+LHS with r-value expressions)
    LabelSet readAccessLocations(Labeler& labeler);

    // searches subtree for SgArrowOp and SgDerefOp
    std::int32_t numPointerDerefOps(SgNode* node);
    std::int32_t numLocalArrayAccessOps(SgNode* node);

  };
}

#endif
