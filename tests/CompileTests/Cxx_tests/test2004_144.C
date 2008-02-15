// This test demonstrates an error in the unparsing when a poonter to a class is used 
// as a template parameter.  The bug is that the class is unparsed with a class specifier 
// and definition.

// Now it works with A defined as a class, but fails with A defined as a template!
class A {};

// This is a current bug (but now fixed)
template <typename T> class B {};

template <typename T> class X {};

X<A*> xa;

X<B<int>*> xb;

