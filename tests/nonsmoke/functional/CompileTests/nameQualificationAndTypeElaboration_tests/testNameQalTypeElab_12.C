namespace X
   {
     class A{};
  // This type hides A (and forces type elaboration to resolve "class A"
     int A;
  // A a1;
     class A a2;
   }

