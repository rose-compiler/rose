namespace X
   {
     class A{};
  // This type hides A (and forces type elaboration to resolve "class A")
  // enum A{};

  // This variable hides A (and forces type elaboration to resolve "class A")
  // Type elaboration is optional
     A A;

  // A a1;

  // Type elaboration is required
     class A a2;
   }
