// t0433.cc
// matching out-of-line defn when arg type is dependent qualified

template <class T>
struct A {
  typedef T some_type;
  
  int foo1(some_type s);
  int foo2(some_type s);
};

template <class T>
int A<T>::foo1(typename A<T>::some_type s)
{
  return 2;
}

template <class T2>
int A<T2>::foo2(typename A<T2>::some_type s)
{
  return 3;
}

void f()
{
  A<int> a;
  a.foo1(1);
  a.foo2(2);
}


template <class T>
struct A<T*> {
  typedef T some_type;

  int foo(some_type** s);
};

template <class T>
int A<T*>::foo(typename A<T*>::some_type** s)
{
  return 2;
}

void g()
{
  A<int*> a;
  int **q = 0;
  a.foo(q);
}
