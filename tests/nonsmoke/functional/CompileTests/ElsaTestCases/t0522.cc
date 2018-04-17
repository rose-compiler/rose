// t0522.cc
// use an explicit specialization before it is declared

template <class T>
struct A;

// does not require instantiation yet, so it is ok that the definition
// of A<T> has not been provided, and it is also ok that the
// declaration of A<int> has not been provided
typedef A<int> A_int;

template <class T>
struct A {};

// makes the program ill-formed, because A<int> has not
// been declared
//ERROR(1): A<int> bad;

template <>
struct A<int> {
  int x;
};

int foo()
{
  A_int a;
  return a.x;    //ERRORIFMISSING(1): error 1 is still bad even without this
}
