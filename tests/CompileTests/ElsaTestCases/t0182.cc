// t0182.cc
// ambiguity in ExpressionList


// ------------------
// in this section, the correct interpretation has 1 arg
template <class S, class T>
int a1(int d) 
{ return d + sizeof(S) + sizeof(T); }

typedef int b1;
typedef int c1;

int f1(int);

int foo(int d1)
{
  return f1(a1<b1,c1>(d1));
}


// ------------------
// in this section, the correct interpretation has 2 args

int a2;
int b2;
int c2;

int f2(int, int);

int bar(int d2)
{
  return f2(a2<b2,c2>(d2));
}


// the parser thinks this looks like a global variable with
// its ctor being called...
//  template<typename _CharT, typename _Traits, typename _Alloc>
//  inline void swap(basic_string<_CharT, _Traits, _Alloc>& __lhs,
//                   basic_string<_CharT, _Traits, _Alloc>& __rhs)
//  {
//    __lhs.swap(__rhs);
//  }
