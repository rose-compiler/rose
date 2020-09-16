template <class T> void f(T t);
template <class X> void g(const X x);
template <class Z> void h(Z z, Z* zp);

// two different functions with the same type, but 
// within the function, t has different cv qualifications
f<int>(1);       // function type is void(int), t is int
f<const int>(1); // function type is void(int), t is const int
 
// two different functions with the same type and the same x
// (pointers to these two functions are not equal,
//  and function-local statics would have different addresses)
g<int>(1);       // function type is void(int), x is const int
g<const int>(1); // function type is void(int), x is const int
 
// only top-level cv-qualifiers are dropped:
h<const int>(1, NULL); // function type is void(int, const int*) 
                       // z is const int, zp is const int*


