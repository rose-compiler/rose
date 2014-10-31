#include <iostream>
#include <cassert>
#include "runtime.h"

using namespace Backstroke;
using namespace std;

void check(bool check, string msg) {
  cout<<msg<<": ";
  if(check) cout<<"PASS";
  else cout<<"FAIL";
  cout<<endl;
}

template<typename T>
void printarray(string s, T* a, int size) {
  cout<<s;
  for(int i=0;i<size;i++) {
    cout<<a[i]<<" ";
  }
  cout<<endl;
}

class MemObject {
public:
  MemObject():val(0){ num++;}
  int val;
  static int num;
  ~MemObject() {num--;}
};

int MemObject::num=0;

//#define VECTOR_TEST

class Event {
public:
#ifdef VECTOR_TEST
  Event(int n):x(0),memObjectContainer(new vector<MemObject*>(n)){}
  vector<MemObject*>* memObjectContainer;
#else
  Event(int n):x(0){}
  MemObject* memObjectContainer[1000];
#endif
  int x;
};

int main() {
  RunTimeSystem rts;
  rts.init_stack_info();
  bool correct=true;
  try {
    cout<<"Number of uncommitted events before forward: "<<rts.numberOfUncommittedEvents()<<endl;
    int n=10;
    int nr=n-2;
    int nc=n-nr;
    const int asize=10;
    short int* a=new short int[asize];
    Event* event=new Event(n);
    for(int i=0;i<asize;i++) {
      a[i]=0;
    }
    cout<<"STATUS: forward computation of "<<n<<" events."<<endl;
    for(int i=0;i<n;i++) {
      rts.initializeForwardEvent();
      for(int j=0;j<asize;j++) {
#if 0
        rts.assign(&a[j],a[j]+1);
        rts.assign(&a[j],a[j]+2);
#else
        (*rts.avpush(&(a[j])))=a[j]+1;
        (*rts.avpush(&(a[j])))=a[j]+2;
#endif
      }
      (*rts.avpush(&event->x))++;
      ++(*rts.avpush(&event->x));
      (*rts.avpush(&event->x))+=1;
      // test new operation (allocation in heap)
      *(rts.avpushptr((void**)(&(event->memObjectContainer[i]))))=new MemObject();
      // test delete operation (register for commit)
      rts.registerForCommit((void*)(event->memObjectContainer[i]));
      event->memObjectContainer[i]->~MemObject(); // call destructor explicitely (delete is called by commit)
      cout<<"Current event length: "<<rts.currentEventLength()<<endl;
      rts.finalizeForwardEvent();
    }
    // check forward results
    for(int j=0;j<asize;j++) {
      correct=correct&&(a[j]==3*n);
      correct=correct&&(event->x==3*n);
      correct=correct&&(MemObject::num==0);
    }
    if(!correct) {
      printarray("Array after forward: ",a,asize);
      cout<<"event->x after forward: "<<event->x<<endl;
    }

    check(correct,"FORWARD");

    cout<<"STATUS: reverse computation of "<<nr<<" events."<<endl;
    cout<<"Number of uncommitted events before reverse: "<<rts.numberOfUncommittedEvents()<<endl;
    for(int i=0;i<nr;i++) {
      rts.reverseEvent();
      //      printarray("Array after reverse: ",a,asize);
      //cout<<"event->x after forward: "<<event->x<<endl;
    }
    // check reverse results
    for(int j=0;j<asize;j++) {
      correct=correct&&(a[j]==3*n-3*nr);
      correct=correct&&(event->x==3*n-3*nr);
    }
    check(correct, "REVERSE");
    cout<<"Number of uncommitted events before commit : "<<rts.numberOfUncommittedEvents()<<endl;
    if(!correct) {
      printarray("Array after reverse: ",a,asize);
      cout<<"event->x after forward: "<<event->x<<endl;
      rts.print_stack_info();
      cout<<"Array address:"<<a<<endl;
    }

    // commit test
    cout<<"STATUS: committing "<<nc<<" events."<<endl;
    for(int i=0;i<nc;i++) {
      rts.commitEvent();
    }
    // check results after commit (should not be effected by commit after reverse)
    for(int j=0;j<asize;j++) {
      correct=correct&&(a[j]==3*n-3*nr);
      correct=correct&&(event->x==3*n-3*nr);
    }
    cout<<"Number of uncommitted events after commit  : "<<rts.numberOfUncommittedEvents()<<endl;
    //correct=correct&&(rts.numberOfUncommittedEvents()==0);
    check(correct,"COMMIT");
    delete[] a;
    delete event;
    if(correct) {
      cout<< "ALL TESTS PASSED."<<endl;
    } else {
      cout<<"SOME TESTS FAILED."<<endl;
      return 1;
    }
  } catch(char const* s) {
    cerr<<s<<endl;
    return 1;
  }
  return 0;
}
