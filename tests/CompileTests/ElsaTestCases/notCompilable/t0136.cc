// t0136.cc
// test op overloading with some enums

enum E1 {};
enum E2 {};
enum E3 {};
enum E4 {};
enum E5 {};
enum E6 {};

void dummy();                 // line 11

void operator+(E1,E1);        // line 13
void operator+(E2,int);       // line 14
void operator+(E2,E3);        // line 15

void operator<(E1,E1);        // line 17

void f()
{
  // turn on operator overloading
  __testOverload(dummy(), 11);

  E1 e1;
  E2 e2;
  E3 e3;
  E4 e4;
  E5 e5;
  E6 e6;

  __testOverload(e1+e1, 13);
  __testOverload(e2+1, 14);
  __testOverload(e2+e2, 14);
  __testOverload(e2+e3, 15);
  __testOverload(e3+e3, 0);

  __testOverload(e1<e1, 17);
  __testOverload(e1<e2, 0);
}



