#ifndef ROSE_MPIDETERMINISMANALYSIS_H
#define ROSE_MPIDETERMINISMANALYSIS_H

#include "rose.h"

enum DeterminismState { DETERMINISTIC = 0, QUESTIONABLE = 1, NONDETERMINISTIC = 2 };

struct MpiDeterminism
{
  DeterminismState source, tag, functions;
  MpiDeterminism() : source(DETERMINISTIC), tag(DETERMINISTIC), functions(DETERMINISTIC) { }
};

class MpiDeterminismAnalysis : public AstBottomUpProcessing<MpiDeterminism>
{
public:
  MpiDeterminismAnalysis() { }
  MpiDeterminism evaluateSynthesizedAttribute(SgNode *n, SynthesizedAttributesList childAttribs);

private:
  void bump(DeterminismState &target, DeterminismState value);
  MpiDeterminism checkCall(SgFunctionCallExp *fncall);
};


#endif
