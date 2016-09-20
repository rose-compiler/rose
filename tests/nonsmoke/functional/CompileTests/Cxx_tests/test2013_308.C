// Partial specialization of template functions in not allowed in C++,
// but we can build specializations of template functions.

// Declaration of template class
template<class T> class X { };

// Declaration of template function f.
template<class T> void f(X<T>);

// Specialization of template for function f.
template<> void f(X<int>);

// Equivalent specialization of template for function f.
template<> void f<int>(X<int>);


