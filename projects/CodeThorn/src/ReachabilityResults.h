#ifndef ASSERTRESULTS_H
#define ASSERTRESULTS_H

#include <vector>
#include <string>

#include "Miscellaneous.h"

using namespace std;

enum Reachability { REACH_UNKNOWN,REACH_YES,REACH_NO };

// will be eliminated once we have converted 2012 property files
#define NUM_ASSERT_LOCATIONS 100

class ReachabilityResults {
 public:
  ReachabilityResults();
  void reachable(int num);
  void nonReachable(int num);
  void finished();
  void write2013File(const char* filename, bool onlyyesno=false);
  void write2012File(const char* filename, bool onlyyesno=false);
  void printResults();
  void printResultsStatistics();
  void init();
 private:
  string reachToString(Reachability num);
  vector<Reachability> _reachable;
};

#endif
