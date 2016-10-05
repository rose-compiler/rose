// attempt at examples of all of the basic situations in which type
// inference is used; redundant with other tests

// class template
template<class T> struct A {
};

// specialization of class template
template<class T2> struct A<T2*> {
};

// class template that contains another class template instantiation
template <class T> class B {
  A<T> a3;
};

// standard function template
template<class T> T f(T x) {
  return x;
}

// recursive function template
template<class T> T frec(T x) {
  return frec(x);
}

// recursive function template with explicit arguments
template<class T> T frec2(T x) {
  return frec2<int>(x);
}

// function template that uses template inference
template<class T> A<T> f2(A<T> x) {
  return x;
}

// forwarded function template and mutual recursion
template<class T> T gfwd(T x);
template<class T> T ffwd(T x) {
  return gfwd(x);
}
template<class T> T gfwd(T x) {
  return ffwd(x);
}

int main() {
  A<int> a;                     // primary A
  A<int*> a2;                   // specialization of A
  B<int> a3;                    // primary B containing primary A
  B<int*> a4;                   // primary B containing specialization of A

  int x1;
  // implicit
  int y1;
  y1 = f(x1);                   // function template
  y1 = frec(x1);                // recursive function template
  y1 = frec2(x1);               // recursive function template
  f2(a);                        // function template argument inference
  f2(a2);                       // function template argument inference
  ffwd(a);                      // function template argument inference

  // explicit
  y1 = f<int>(x1);              // function template
  y1 = frec<int>(x1);           // recursive function template
  y1 = frec2<int>(x1);          // recursive function template
  f2<int>(a);                   // function template argument inference
  f2<int*>(a2);                 // function template argument inference
  ffwd<A<int> >(a);             // function template argument inference
}
