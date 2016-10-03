// t0004.c
// non-inner struct

struct Outer {
  struct Inner {     // not really!
    int x;
  } is;
  enum InnerEnum { InnerEnumerator } ie;
  int y;
  
  // not legal C, but I wanted to test the mechanism that pushes
  // type definitions to the outer scope, so I'll leave it..
  typedef int InnerTypedef;
};

int foo(struct Inner *i)
{
  enum InnerEnum gotcha;
  InnerTypedef z;
  return i->x + InnerEnumerator;
}
