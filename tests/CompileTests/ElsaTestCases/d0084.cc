//  ./qt2-2.3.1-13/qstring-ht7v.i.cpp_out:/home/ballAruns/tmpfiles/./qt2-2.3.1-13/qstring-ht7v.i:5628:8: error: no viable candidate for function call

// this one involves operator overloading

struct A {
  A (int s);
};

A operator+ (char c, A const &a);

struct B {
  operator char () const;
};

void f() {
  B b;
  b - 1 + 1;
}
