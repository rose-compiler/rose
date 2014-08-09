#include "runtime.h"

void func_forward();
void func_reverse();

using namespace Backstroke;

RunTimeSystem rts;

#include "rev_rtest1.C"

#include <iostream>
using namespace std;
#include "../runtime/runtime.C"

int main() {
  rts.init_stack_info();
  rts.initializeForwardEvent();
  __forward_func();
  cout<<"Event length: "<<rts.currentEventLength()<<endl;
  rts.finalizeForwardEvent();
  rts.initializeForwardEvent();
  __forward_func();
  cout<<"Event length: "<<rts.currentEventLength()<<endl;
  rts.finalizeForwardEvent();
  cout<<"X1"<<endl;
  __reverse_func();
  cout<<"X2"<<endl;
  __reverse_func();
  cout<<"X3"<<endl;
  return 0;
}
