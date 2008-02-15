// t0268a.cc
// slightly simpler than t0268.cc, so should be tested first

template <class T>
struct A {
  // out-of-line defn goes with this decl
  void append(T* t);

  int append(int, int, int)  { }
};

template <class T>
void A<T>::append(T* t)
{}
