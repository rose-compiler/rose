#include <stack>
#include <iostream>
using namespace std;

union Data {
  int intvalue;
  void* pointervalue;
};


class RevStack {
 public:
  int* pvpush_int(int* pval) { 
    d.intvalue=*pval;
    istack.push(d);
    d.pointervalue=pval;
    istack.push(d);
    return pval;
  }
  void push(int val) { d.intvalue=val; istack.push(d);}
  int* ppop() { d=istack.top();istack.pop();return (int*)d.pointervalue; }
  int tpop() { d=istack.top();istack.pop();return d.intvalue; }
  int int_tpop() { d=istack.top();istack.pop();return d.intvalue; }
  size_t size() { return istack.size(); }
private:
  stack<Data> istack;
  Data d;
};



