// t0127.cc
// some operator overload resolution tests

class A {};

class B : public A {
public:
  void operator+(A &a);             // 8
};

void operator+(A &a, A &b);         // 11
void operator+(B &a, B &b);         // 12

void some_random_crap();            // 14

// the built-in operators are all non-functions, and 0 is my code for that
enum { BUILTIN=0 };

class C {
public:
  operator int ();                  // 21
};

class D {
public:
  operator int* ();                 // 26
};

class E {
public:
  operator int ();                  // 31
  operator int* ();                 // 32
  void operator-(A &a);             // 33
};

class F {
public:
  void operator* (F const &);       // 38
  void operator/ (F const &);       // 39
  void operator+ (F const &);       // 40
  void operator- (F const &);       // 41
  void operator< (F const &);       // 42
  void operator> (F const &);       // 43
  void operator<=(F const &);       // 44
  void operator>=(F const &);       // 45
  void operator==(F const &);       // 46
  void operator!=(F const &);       // 47
};

void f()
{
  A a1,a2;
  B b1,b2;
  C c;
  D d;
  E e;
  F f1,f2;


  // turn on overload resolution
  __testOverload(some_random_crap(), 14);

  __testOverload(a1+a2, 11);

  __testOverload(b1+b2, 12);

  __testOverload(b1+a1, 8);

  __testOverload(c+1, BUILTIN);
  __testOverload(c+(char)1, BUILTIN);

  __testOverload(d+1, BUILTIN);
  __testOverload(1+d, BUILTIN);

  //ERROR(1): __testOverload(e+1, BUILTIN);    // ambiguous

  // BIN_MINUS
  __testOverload(e-a1, 33);
  __testOverload(c-1, BUILTIN);
  __testOverload(d-1, BUILTIN);
  __testOverload(d-d, BUILTIN);
  //ERROR(2): __testOverload(1-d, BUILTIN);    // no viable

  // these resolve to member functions
  __testOverload(f1* f2, 38);
  __testOverload(f1/ f2, 39);
  __testOverload(f1+ f2, 40);
  __testOverload(f1- f2, 41);
  __testOverload(f1< f2, 42);
  __testOverload(f1> f2, 43);
  __testOverload(f1<=f2, 44);
  __testOverload(f1>=f2, 45);
  __testOverload(f1==f2, 46);
  __testOverload(f1!=f2, 47);

  // these resolve to the built-ins of 13.6 para 12
  __testOverload(c* c, BUILTIN);
  __testOverload(c/ c, BUILTIN);
  __testOverload(c+ c, BUILTIN);
  __testOverload(c- c, BUILTIN);
  __testOverload(c< c, BUILTIN);
  __testOverload(c> c, BUILTIN);
  __testOverload(c<=c, BUILTIN);
  __testOverload(c>=c, BUILTIN);
  __testOverload(c==c, BUILTIN);
  __testOverload(c!=c, BUILTIN);
}
