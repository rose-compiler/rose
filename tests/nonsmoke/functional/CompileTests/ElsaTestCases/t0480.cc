// t0480.cc
// expanding on t0478.cc and t0479.cc, trying to exercise various
// possibilities for rewriting -> dependind on templateness and
// presence of operator->


struct A {
  int x;
};

struct B {
  A* operator->();
};

struct C {
  B operator-> ();
};

template <class T>
struct D {
  T operator-> ();
};


// ------------------------
// not dependent, no operator ->
template <class T>
int f1(T t, A *a)
{
  return a->x;
}

// instantiation
template int f1(int, A*);


// ------------------------
// dependent
template <class T>
int f2(T t)
{
  return t->x;
}

// instantiate it such that operator -> is not used
template int f2(A *t);

// instantiate, and *do* use operator ->
template int f2(B t);

// instantiate and use operator-> twice
template int f2(C t);


// ------------------------
// more complicated
template <class T>
int f3(D<T> d)
{
  return d->x;
}

template int f3(D<A*> d);
template int f3(D<B> d);
template int f3(D<C> d);
