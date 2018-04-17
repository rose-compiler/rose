// And that new syntax emerged from the effort to have a way to express "template typedef":
class MyAllocatorType;

// template<class T1, class T2, int I> class A {}; // primary template
// template<class T, int I> class A<T, T*, I> {}; // partial specialization where T2 is a pointer to T1
// template<class T, class T2, int I> class A<T*, T2, I> {}; // partial specialization where T1 is a pointer 
// template<class T> class A<int, T*, 5> {}; // partial specialization where T1 is int, I is 5, and T2 is a pointer

template< typename T, class U > class AnotherType {};

// Example of C++11 templated typedef (using the new C++11 typedef syntax).
template< class T > using MyType_C = AnotherType< T, MyAllocatorType >;

// This should generate an instantiation of the template typedef declaration.
MyType_C<int> xxx;

// int ABCDEFG;
