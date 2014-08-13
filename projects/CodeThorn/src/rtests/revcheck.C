#include <cassert>
#include <iostream>
#include "rev_rtest2.C"

using namespace std;
using namespace Backstroke;

extern RunTimeSystem rts;

int main() {
  //assert(rts.size()==0);
  int n=10;
  cout<<"Stack size: "<<rts.size()<<endl;
  for(int i=0;i<n;i++) {
    func_forward();
    cout<<"Stack size: "<<rts.size()<<endl;
  }
  for(int i=0;i<n;i++) {
    func_reverse();
    cout<<"Stack size: "<<rts.size()<<endl;
  }
  return 0;
}
