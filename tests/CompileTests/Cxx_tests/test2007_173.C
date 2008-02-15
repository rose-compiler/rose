
// This test fails for g++ version 3.4.6, but should work for a later versions.
// But it does not work in g++ 4.2.2, so maybe it will be fixed later.

// It is considered legal code and it does pass with EDG (and it is properly
// handled by ROSE.)
class A
   {
  // This will default to private, but should be available to class B
  // Note that the "use of the "private:" keyword is not required.
     private: void f();
   };

class B
   {
  // I expected that this would work with later versions of GNU, but 4.2 also fails.
  // This is a bug in GNU.

// #if ( (__GNUC__ >= 4) && (__GNUC_MINOR__ >=0) )
#ifndef __GNUC__
  // This is a private member of class A, but is available for use as a friend
  // Note that the "use of the "private:" keyword is not required.
     private: friend void A::f();
#endif
   };

