// t0179.cc
// template, partially specialized to a template argument

// this one uncovered all kinds of subtle errors in the
// handling of template argument lists ...

template <class S, class T>
struct A {};
        
template <class T>
struct B {};

typedef int myint;

template <>
struct A<int, B<myint> >
{};
