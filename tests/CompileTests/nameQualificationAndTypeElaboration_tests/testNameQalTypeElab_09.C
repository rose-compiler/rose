namespace X
   {
     class A{};
  // This type hides A (and forces type elaboration to resolve "class A"
     typedef A A;
     A a1;
     class A a2;
   }
