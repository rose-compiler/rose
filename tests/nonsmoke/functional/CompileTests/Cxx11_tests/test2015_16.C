// DQ (10/29/2015): New test code to test C++11 and CUDA support added to EDG 4.9.
// This test code is designed to test the CUDA and C++11 support in ROSE (from Tristan).
// It requires Tristans's patch to fix the CUDA support in EDG 4.9 which was a previously
// a problem in C++11 mode.

template <class T>
struct A {};

///A<A<A<A<int>>>> a;

template<class = A<A<A<int>>>>
struct B {};
