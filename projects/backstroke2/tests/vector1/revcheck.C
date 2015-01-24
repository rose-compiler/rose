#include <cassert>
#include <iostream>

using namespace std;

#include "runtime.h"

//Backstroke::RunTimeSystem rts;

#include "Timer.h"
#include "Timer.cpp"

namespace BackstrokeReversed {
  Backstroke::RunTimeSystem rts;
  #include "rev_rtest1.C"
}

int main() {
  
  bool verbose=true;
  int n=10000;
  int nr=n*0.25;
  Timer t;
  cout<<"Forward Events:"<<n<<endl;
  cout<<"Reversed Events:"<<nr<<endl;

  t.start();
  {
    for(int i=0;i<n;i++) {
      extern void func();
      func();
    }
  }
  t.stop();
  double origTime=t.getElapsedTimeInMilliSec();


  double fwdTime=0.0;
  double revTime=0.0;
  {
    //assert(rts.size()==0);
    int n=10;
    if(verbose)cout<<"Stack size: "<<BackstrokeReversed::rts.size()<<endl;
    t.start();
    for(int i=0;i<n;i++) {
      BackstrokeReversed::rts.init_stack_info();
      BackstrokeReversed::rts.initializeForwardEvent();
      BackstrokeReversed::func();
      BackstrokeReversed::rts.finalizeForwardEvent();
      if(verbose)cout<<"Stack size: "<<BackstrokeReversed::rts.size()<<endl;
    }
    t.stop();
    fwdTime=t.getElapsedTimeInMilliSec();

    t.start();
    // TODO: nr (less than n)
    for(int i=0;i<n;i++) {
      BackstrokeReversed::rts.reverseEvent();
      if(verbose)cout<<"Stack size: "<<BackstrokeReversed::rts.size()<<endl;
    }
    t.stop();
    revTime=t.getElapsedTimeInMilliSec();
  }
  double totalRevTime=fwdTime+revTime;
  double pureRevTime=origTime*((double)nr/(double)n);
  double totalPureRevTime=origTime+pureRevTime;
  cout<<"Comparison:"<<endl;
  cout<<"Orig: "<<origTime<<" Est-PureRev : "<<pureRevTime<<" Total: "<<totalPureRevTime<<endl;
  cout<<"Fwd : "<<fwdTime<< " Rev         : "<<revTime<<" Total: "<<totalRevTime<<endl;
  cout<<"Ratio Incremental vs Pure-Reverse(est): "<<totalRevTime/totalPureRevTime<<endl;
  return 0;
}
