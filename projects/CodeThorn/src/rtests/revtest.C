#include "runtime.h"

using namespace Backstroke;

RunTimeSystem rts;

//#include "rev_rtest1.C"

#include <iostream>
using namespace std;
#include "../runtime/runtime.C"

extern void func();

int main() {
  rts.init_stack_info();
  rts.initializeForwardEvent();
  func();
  cout<<"Event length: "<<rts.currentEventLength()<<endl;
  rts.finalizeForwardEvent();
  rts.initializeForwardEvent();
  func();
  cout<<"Event length: "<<rts.currentEventLength()<<endl;
  rts.finalizeForwardEvent();
  cout<<"X1"<<endl;
  rts.reverseEvent();
  cout<<"X2"<<endl;
  rts.reverseEvent();
  cout<<"X3"<<endl;
  return 0;
}
