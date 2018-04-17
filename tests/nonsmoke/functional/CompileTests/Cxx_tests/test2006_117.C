/* pg. 853 in Stroustrup
   For testing SgDotStarOp and SgArrowStarOp
*/

// Skip version 4.x gnu compilers
// #if ( __GNUC__ == 3 )
#if ( defined(__clang__) == 0 && __GNUC__ == 3 )

#include <iostream>
using namespace std;

struct C {
    char * val;
    int i;
    void print(int x) { cout << val << x << "\n"; }
    void f1();
    void f2();
    C(char* v) { val = v; }
};

typedef void (C::*PMFI)(int);
typedef char* C::*PM;

void f(C& z1, C& z2) 
  {
    C* p = &z2;
    PMFI pf = &C::print;

 // Error: this unparses to PM pm = val;
    PM pm = &C::val;
    PM pmNull = NULL;

    z1.print(1);
    (z1.*pf)(2);
    z1.*pm = "nv1";
    p->*pm = "nv2";
    z2.print(3);
    (p->*pf)(4);
  }

#else
  #warning "Not tested on gnu 4.0 or greater versions"
#endif

