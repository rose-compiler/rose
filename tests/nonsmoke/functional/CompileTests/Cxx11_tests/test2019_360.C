class C 
   {
  // These are not trivially deleted, though they are marked explicitly as using the "= delete" syntax.
     C(const C&) = delete;
     C(C&&) = delete;
   };
