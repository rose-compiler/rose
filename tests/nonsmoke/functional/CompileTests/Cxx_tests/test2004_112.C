// This test code demonstrates the handling of template parameters
// and previously demonstrated several bugs in the setting of parent
// pointers in declarations hidden in types buried in typedefs which
// where hidden in different namespaces. This is getting hard!

namespace Y
   {
     typedef int Integer;
     typedef struct type_A typedefType_A;
     typedef struct type_B typedefType_B;
//   struct type_B *Bptr;
   }

namespace X
   {
     typedef int Integer;
     template < typename T = int > class A;
     template < typename T = A < Integer > > class B;
     template < typename T = A < X::Integer > > class C;
     template < typename T = A < Y::Integer > > class D;
     template < typename T = B <> > class E;
     template < typename T = A < Y::typedefType_A > > class F;
     template < typename T = A < Y::type_A > > class G;
     template < typename T = A < Y::type_B > > class H;
   }

typedef struct localtype_A typedefType_A;
struct localtype_B *Bptr;

template < typename T > class A {};

template < typename T = A < typedefType_A > > class F;
template < typename T = A < localtype_A > > class G;
template < typename T = A < Y::type_A > > class H;
template < typename T = A < Y::type_B > > class I;

struct type_A {} J;

// If we use "typedef struct localtype_A typedefType_A;" then this
// will currently unparse as: "A<typedefType_A> I;" but that is OK for now!
// Otherwise it is unparsed correctly.  However, I think it is a bug so it
// really should not compile with the typedef as:
// "typedef struct localtype_A typedefType_A;" since type_A is not defined 
// anywhere but in namespace Y.
A<type_A> K;


A<Y::type_A> K2;

A<typedefType_A> L;

// We can do similar tricks with variable declarations as with typedefs (using class declarations hidden in variable declarations)
A<localtype_B> M;

// This does not unparse properly ("A<type_B> N;", lacks qualified name for type "type_B")
A<Y::type_B> N;
