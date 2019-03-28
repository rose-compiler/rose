
// https://stackoverflow.com/questions/19474374/what-is-the-purpose-of-ref-qualified-member-functions
struct S 
   {
  // Non-static member function can be ref-qualified.

  // Unparsed as: inline void f()
     void f() & { }

  // Unparsed as: inline void f()
     void f() &&{ }
   };
