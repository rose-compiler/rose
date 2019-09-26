class C 
   {
  // These are not trivially defaulted, though they are marked explicitly as using the "= default" syntax.
     C(const C&) = default;
     C(C&&) = default;
   };
