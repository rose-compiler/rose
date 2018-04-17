// t0403.cc
// resolve overloaded function name as argument to user-def operator


int foo();         // line 5
int foo(int);      // line 6

typedef int (*pf1)();
typedef int (*pf2)(int);

struct S {
  void operator+ (pf1);     // line 12
  void operator- (pf2);     // line 13
  void operator= (pf2);     // line 14
};
void operator+ (pf1, S&);   // line 16
void operator- (pf2, S&);   // line 17

// argument to user-defined operator
void zoo(S &s)              // line 20
{
  s + foo;         // refers to line 5
  s - foo;         // refers to line 6
  s = foo;         // refers to line 6

  foo + s;         // refers to line 5
  foo - s;         // refers to line 6
}

asm("collectLookupResults "
    "operator+=12 s=20 foo=5 "
    "operator-=13 s=20 foo=6 "
    "s=20 foo=6 "       // not entirely sure why operator= doesn't show up..
    "operator+=16 foo=5 s=20 "
    "operator-=17 foo=6 s=20");

// EOF
