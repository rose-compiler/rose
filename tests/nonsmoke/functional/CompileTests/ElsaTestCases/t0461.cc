// t0461.cc
// in template class, use A<T> form in name of constructor
// derived from k0005a.cc

template <typename T>
struct A {
    A<T> (int) {}
    A<T> (float);
    A<T> (int*);
};

// external definition without A<T>
template <class T>
A<T>::A(float) {}

// external definition *with* A<T>
//
// this one is tricky b/c of scope delegation nonsense
template <class T>
A<T>::A<T>(int*) {}


