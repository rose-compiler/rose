// t0141.cc
// test operator~

// turn on operator overloading
int dummy();             // line 5
void ddummy() { __testOverload(dummy(), 5); }

enum E1 {};
enum E2 {};

void operator~ (E1);     // line 11

void f1()
{
  E1 e1;
  E2 e2;
  
  __testOverload(~e1, 11);
  __testOverload(~e2, 0);
}


struct A {
  void operator~ ();     // line 24
};
  
struct B {
  operator int ();       // line 28
};

void f2()
{
  A a;
  B b;
  
  __testOverload(~a, 24);
  __testOverload(~b, 0);
}



struct C {
  void operator~ ();     // line 43
  operator char ();      // line 44
};

struct D {
  void operator~ ();     // line 48
  operator int ();       // line 49
};

void f3()
{
  C c;
  D d;
                         
  // operator~ wins, because the conversion to char and then
  // to int requires a promotion
  __testOverload(~c, 43);       
  
  // operator~ still wins; operator int() is considered a conversion,
  // whereas use of operator~ doesn't require any conversions
  __testOverload(~d, 48);
}



// finish up some unary operators
void operator+(C&);    // line 69
void operator-(C&);    // line 70
void operator!(C&);    // line 71

void f4()
{
  C c;
  E1 e1;

  __testOverload(+c, 69);
  __testOverload(-c, 70);
  __testOverload(!c, 71);

  __testOverload(+e1, 0);
  __testOverload(-e1, 0);
  __testOverload(!e1, 0);
}



// wrong # of arguments
//ERROR(1): void operator!(C&,C&);





