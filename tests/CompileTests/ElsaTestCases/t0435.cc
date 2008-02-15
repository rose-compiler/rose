// t0435.cc
// need for ArrayType with dependent size

template <int n>
struct A {
  int foo(int (*p)[n]);         // line 6
  int foo(int (*p)[n+1]);       // line 7
};

template <int n>
int A<n>::foo(int (*p)[n])
{
  return 1;
}

template <int n>
int A<n>::foo(int (*p)[n+1])
{
  return 2;
}

void f()
{
  A<3> a;
  int arr1[3];
  int arr2[4];

  a.foo(&arr1);                 // calls line 6
  a.foo(&arr2);                 // calls line 7
}


// EOF
