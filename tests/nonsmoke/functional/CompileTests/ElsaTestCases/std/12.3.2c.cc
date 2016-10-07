// 12.3.2c.cc
// potential ambiguity between conversion-type-id and expressions

struct A {
  operator int * ();
};

void f()
{
  A ac;
  int i;
  
  // example is this line:
  //ERROR(1): &ac.operator int*i;    // parse error
  
}
