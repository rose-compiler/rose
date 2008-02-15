// t0349.cc
// problem with operator[] inside template bodies

// class with operator[]
template <class T>
struct AS {
  T const& operator[] (int i) const;
  T      & operator[] (int i)      ;

  void foo(int i)
    { operator[](i); }
};

// no problem
void foo(AS<int> const &src)
{
  int i;
  src[i];
}

// problem
template <class T>
void copyIndexPtrArray(T *base, AS<int> const &src)
{
  int i;
  src[i];
}

// instantiate the template
void foo()
{
  int *p;
  AS<int> a;

  copyIndexPtrArray(p, a);
}
