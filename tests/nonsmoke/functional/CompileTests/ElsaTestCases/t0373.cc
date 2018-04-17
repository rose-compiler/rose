// t0373.cc
// ambiguity with angle brackets, this time for function templates

// bears some relation to t0182.cc

template <int x>
void foo(int z);

int bar;

void f()
{
  foo<3>(4);         // template function call
  bar<3>(4);         // comparison expression
  
  //ERROR(1): foo<3>4;           // syntax error, arguments must be parenthesized
  //ERROR(2): bar<>(4);          // error, bar is not a template
}
