class B
   {
  // Must be a public member function to be able to be visible using "using" directive.
     public:
          void f(char);
   };

// The default for a class should be private (or for a struct it is public).
class D : B 
   {
  // using B::f;
     void foo () { f('c'); }
   };
