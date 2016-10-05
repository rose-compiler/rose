// t0268.cc
// problem associating a defn with a decl
// from ostream, basic_string::append

// this fwd decl appears to be important for my failing testcase
template <class T >
struct A;

template <class T>
struct A {
  typedef typename T::size_type size_type;

  // out-of-line defn goes with this decl
  A& append(T* t, size_type n);

  int append(int, int, int)  { }
};

template <class T>
A<T>& A<T>::append(T* t, size_type n)
{ }


// EOF
