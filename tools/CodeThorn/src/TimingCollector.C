
#include "TimingCollector.h"
#include <sstream>
#include <string>
#include <iostream>

using namespace std;

namespace CodeThorn {

  void TimingCollector::startTimer() { timer.start(); }
  void TimingCollector::stopTimer() { timer.stop(); }

  string TimingCollector::runtime(TimeDuration t) {
    stringstream ss;
    ss<<t.longTimeString();
    return ss.str();
  }

  TimeDuration  TimingCollector::getTotalRunTime() {
    TimeDuration sum;
    for(int i=0;i<TimingCollector::TimeDurationName::NUM;i++) {
      sum+=t[i];
    }
    return sum;
  }

  void TimingCollector::stopTimer(TimeDurationName n) {
    t[n]=timer.getTimeDurationAndStop();
  }

  string TimingCollector::toString() {
    stringstream ss;
    ss<<"Front end                      : "<<runtime(t[frontEnd])<<endl;
    ss<<"Normalization                  : "<<runtime(t[normalization])<<endl;
    ss<<"Labeler                        : "<<runtime(t[labeler])<<endl;
    ss<<"VariableIdMapping              : "<<runtime(t[variableIdMapping])<<endl;
    //ss<<"FunctionCallMapping            : "<<runtime(t[functionCallMapping])<<endl;
    ss<<"Class hierarchy analysis       : "<<runtime(t[classHierarchyAnalysis])<<endl;
    ss<<"Virtual function analysis      : "<<runtime(t[virtualFunctionAnalysis])<<endl;
    ss<<"ICFG construction              : "<<runtime(t[icfgConstruction])<<endl;
    ss<<"RevICFG construction           : "<<runtime(t[reverseIcfgConstruction])<<endl;
    ss<<"Solver initialization time     : "<<runtime(t[init])<<endl;
    ss<<"Transition system analysis     : "<<runtime(t[transitionSystemAnalysis])<<endl;
    ss<<"Report generation time         : "<<runtime(t[reportGeneration])<<endl;
    ss<<"Call Graph DOT file            : "<<runtime(t[callGraphDotFile])<<endl;
    ss<<"Visualization generation       : "<<runtime(t[visualization])<<endl;
    ss<<"Total time                     : "<<runtime(getTotalRunTime())<<endl;
    return ss.str();
  }

}
