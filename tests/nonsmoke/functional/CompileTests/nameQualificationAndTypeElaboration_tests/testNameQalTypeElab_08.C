class A{};
namespace X
   {
  // This type hides ::A
     typedef ::A A;
     A a1;
     ::A a2;
   }
