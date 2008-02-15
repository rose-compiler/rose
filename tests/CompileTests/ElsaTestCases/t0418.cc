// t0418.cc
// duplicate definition of template member function

template <class T>
struct A {
  int foo(T *t);
};

template <class T>
int A<T>::foo(T *t)
{
  return 0;
}

//ERROR(1): template <class T>
//ERROR(1): int A<T>::foo(T *t)
//ERROR(1): {
//ERROR(1):   return 1;
//ERROR(1): }
          
void f()
{
  A<int> a;
}

// EOF
