#include "ReachabilityResults.h"

// basic file operations
#include <iostream>
#include <fstream>
using namespace std;
#include <cassert>

using CodeThorn::color;

ReachabilityResults::ReachabilityResults() {
  init();
}

void ReachabilityResults::init() {
  for(int i=0;i<NUM_ASSERT_LOCATIONS;i++) {
    _reachable.push_back(REACH_UNKNOWN);
  }
}

void ReachabilityResults::reachable(int num) {
  //std::cout<< "REACHABLE: "<<num<<std::endl;
  _reachable[num]=REACH_YES;
}
void ReachabilityResults::nonReachable(int num) {
  //std::cout<< "REACHABLE: "<<num<<std::endl;
  _reachable[num]=REACH_NO;
}
string ReachabilityResults::reachToString(Reachability num) {
  switch(num) {
  case REACH_UNKNOWN: return "unknown,0";
  case REACH_YES: return "yes,9";
  case REACH_NO: return "no,9";
  default: {
    cerr<<"Error: unkown reachability information.";
    assert(0);
  }
  }
  assert(0);
}
// we were able to compute the entire state space. All unknown become non-reachable (=no)
void ReachabilityResults::finished() {
  for(int i=0;i<NUM_ASSERT_LOCATIONS;++i) {
    if(_reachable[i]==REACH_UNKNOWN) {
      _reachable[i]=REACH_NO;
    }
  }
}
void ReachabilityResults::write2013File(const char* filename, bool onlyyesno) {
  ofstream myfile;
  myfile.open(filename);
  for(int i=0;i<60;++i) {
    if(onlyyesno && (_reachable[i]!=REACH_YES && _reachable[i]!=REACH_NO))
      continue;
    myfile<<i+100<<","<<reachToString(_reachable[i])<<endl;
  }
  myfile.close();
}
void ReachabilityResults::write2012File(const char* filename, bool onlyyesno) {
  ofstream myfile;
  myfile.open(filename);
  // 2012: difference by 1 to 2013
  for(int i=0;i<61;++i) {
    if(onlyyesno && (_reachable[i]!=REACH_YES && _reachable[i]!=REACH_NO))
      continue;
    myfile<<i<<","<<reachToString(_reachable[i])<<endl;
  }
  myfile.close();
}
void ReachabilityResults::printResults() {
  cout<<"Reachability Results:"<<endl;
  int maxCode=59;
  for(int i=0;i<=maxCode;++i) {
    cout<<color("white")<<"error_"<<i<<": ";
    switch(_reachable[i]) {
    case REACH_UNKNOWN: cout <<color("magenta")<<"UNKNOWN";break;
    case REACH_YES: cout <<color("green")<<"YES (REACHABLE)";break;
    case REACH_NO: cout  <<color("cyan")<<"NO (UNREACHABLE)";break;
    default:cerr<<"Error: unknown reachability type."<<endl;assert(0);
    }
    cout<<color("normal")<<endl;
  }
  cout<<color("default-text-color");
}

void ReachabilityResults::printResultsStatistics() {
  int maxCode=59;
  int numReach=0, numNonReach=0;
  int numUnknown=0;
  for(int i=0;i<=maxCode;++i) {
    switch(_reachable[i]) {
    case REACH_UNKNOWN: numUnknown++;break;
    case REACH_YES: numReach++;break;
    case REACH_NO: numNonReach++;break;
    default:cerr<<"Error: unknown reachability type."<<endl;assert(0);
    }
  }
  cout<<"Assert reachability statistics: "
      <<color("white")<<"YES: "<<color("green")<<numReach
      <<color("white")<<" NO: "<<color("cyan")<<numNonReach
      <<color("white")<<" UNKNOWN: "<<color("magenta")<<numUnknown
      <<color("default-text-color")<<" Total: "<<maxCode+1
      <<endl;
}
