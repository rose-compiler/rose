#include <iostream>
#include "runtime.C"

using namespace Backstroke;
using namespace std;

int main() {
  RunTimeSystem rts;
  try {
    cout<<"Number of uncommitted events before forward: "<<rts.numberOfUncommittedEvents()<<endl;
    rts.initializeForwardEvent();
    int a=-1;
    rts.assign(&(a),5);
    int* ap=&a;
    short int b=-1;
    cout<<"a:"<<a<<" b:"<<b<<endl;
    rts.assign(&(b),50);
    short int barray[5];
    short int* bp=&barray[1];
    rts.assign(&(a),a+1);
    rts.assign(&(b),b+10);
    char c='A';
    cout<<"a:"<<a<<" b:"<<b<<" bp:"<<bp<<" c:"<<c<<endl;
    rts.assign(&(c),'B');
    rts.assignptr(((void**)&(bp)),&(barray[2])); // bp=&b;
    cout<<"a:"<<a<<" b:"<<b<<" bp:"<<bp<<" c:"<<c<<endl;
    rts.finalizeForwardEvent();
    cout<<"Number of uncommitted events before reverse: "<<rts.numberOfUncommittedEvents()<<endl;
    rts.reverseEvent();
    cout<<"Number of uncommitted events before commit: "<<rts.numberOfUncommittedEvents()<<endl;
    //rts.commitEvent();
    cout<<"a:"<<a<<" b:"<<b<<" bp:"<<bp<<" c:"<<c<<endl;
    cout<< "ALL TESTS PASSED."<<endl;
  } catch(char const* s) {
    cerr<<s<<endl;
    return 1;
  }
  return 0;
}
