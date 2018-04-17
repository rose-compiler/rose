// t0562.cc
// DQT in operator==

template <class T>
struct A {
  A(char const *);
  
  typedef char const *PTR;
};

template <class T>
bool operator==(A<T> const &a, typename A<T>::PTR b);

void foo()
{
  A<int> a("bar");
  
  char const *ptr;
  a == ptr;
  
  a == "foo";
}
