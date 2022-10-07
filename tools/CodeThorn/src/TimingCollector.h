#ifndef TIMING_COLLECTOR_H
#define TIMING_COLLECTOR_H

#include "TimeMeasurement.h"

namespace CodeThorn {

  // Author: Markus Schordan
  class TimingCollector {
  public:
    enum TimeDurationName {
    frontEnd,
    init,
    normalization,
    extractAssertionTraces,
    classHierarchyAnalysis,
    virtualFunctionAnalysis,
    variableIdMapping,
    labeler,
    //functionCallMapping,
    icfgConstruction,
    reverseIcfgConstruction,
    transitionSystemAnalysis,
    reportGeneration,
    callGraphDotFile,
    visualization,
    NUM};
    void startTimer();
    void stopTimer();
    void stopTimer(TimeDurationName n);
    TimeDuration getTotalRunTime();
    std::string toString();
    std::string runtime(TimeDuration t);
  private:
    TimeMeasurement timer;
    TimeDuration t[TimeDurationName::NUM]; // TimeDuration objects are default initialized with 0
  };

}

#endif
