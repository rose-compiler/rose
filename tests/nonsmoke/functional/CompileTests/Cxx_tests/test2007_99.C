/*
I am having trouble with the following code:

template <typename T> struct A {
  template <typename U> friend void f(U*);
};
extern template class A<char>;
template<> void f(int*);

It is extracted from the <istream> header for GCC 4.1.1.  In particular, I am getting an assertion about a Sg_File_Info being compiler-generated, with the error message claiming that it shouldn't be.  Is this also true with the current snapshot of ROSE?  My build of the snapshot is broken because of buffer overflows in the (now obsolete) version of the hidden list computation.  Could you please check it instead, and see if you can fix it?  Thank you,

-- Jeremiah Willcock 
*/

template <typename T> struct A {
  template <typename U> friend void f(U*);
};
extern template class A<char>;
template<> void f(int*);
