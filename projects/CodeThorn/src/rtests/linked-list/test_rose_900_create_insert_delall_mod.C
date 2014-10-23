#include <cstdio>


#include "runtime/runtime.h"
#include "runtime/runtime.C"

#include "Timer.h"
#include "Timer.cpp"

#include <iostream>
using namespace std;
using namespace Backstroke;

RunTimeSystem rts;

#if 0
#ifndef REVERSE
#include "900_create_insert_delall.C"
#else
#include "rose_900_create_insert_delall.C"
#endif
#endif

namespace ORIGINAL {
#include "900_create_insert_delall.C"
}
namespace MYREVERSE {
  #include "rose_900_create_insert_delall.C"
}

int main() {
  int num=1000000;
  double originalTime=0;
  double forwardTime=0;
  double reverseTime=0;
  double commitTime=0;
  Timer timer;

#ifndef COMMIT_ONLY
#if 1
  {
    ORIGINAL::State* state=new ORIGINAL::State();
    timer.start();
    for(int i=0;i<num;i++) {
      ORIGINAL::event(state);
    }
    timer.stop();
    originalTime=timer.getElapsedTimeInMilliSec();
  }
  cout<<"Original: "<<originalTime<<" ms"<<endl;
#endif
  {
    MYREVERSE::State* state=new MYREVERSE::State();
    rts.init_stack_info();
    timer.start();
    for(int i=0;i<num;i++) {
      rts.initializeForwardEvent();
      MYREVERSE::event(state);
      rts.finalizeForwardEvent();
    }
    timer.stop();
    forwardTime=timer.getElapsedTimeInMilliSec();
    cout<<"Forward : "<<forwardTime<<" ms"<<endl;
    timer.start();
    for(int i=0;i<num;i++) {
      rts.reverseEvent();
    }
    timer.stop();
    reverseTime=timer.getElapsedTimeInMilliSec();
  }
  cout<<"Reverse : "<<reverseTime<<" ms"<<endl;
#else

  {
    MYREVERSE::State* state=new MYREVERSE::State();
    rts.init_stack_info();
    timer.start();
    for(int i=0;i<num;i++) {
      rts.initializeForwardEvent();
      MYREVERSE::event(state);
      rts.finalizeForwardEvent();
    }
    timer.stop();
    forwardTime=timer.getElapsedTimeInMilliSec();
    //cout<<"Forward : "<<forwardTime<<" ms"<<endl;
    timer.start();
    for(int i=0;i<num;i++) {
      rts.commitEvent();
    }
    timer.stop();
    commitTime=timer.getElapsedTimeInMilliSec();
  }
  cout<<"Commit : "<<commitTime<<" ms"<<endl;
#endif

  return 0;
}
