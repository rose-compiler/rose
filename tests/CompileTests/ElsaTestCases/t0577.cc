// t0577.cc
// demonstrate flaw in abstract enumerator value processing
  

template <int n>
struct A {
  // Elsa will fail to const-eval 'n', then suppress the
  // error, but go on thinking that n is always 0.
  enum { x = n };
  
  // when x equal to 0, this will fail
  int arr[x - 5];
  
  // on the other hand, it recognizes that this is not necessarily bad
  int arr2[n - 5];
};

// valid specialization *can* be generated
A<10> a;


