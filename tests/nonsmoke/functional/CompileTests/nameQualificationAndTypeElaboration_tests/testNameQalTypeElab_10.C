class A{};
namespace X
   {
  // This type enum A hides ::A and type elaboration is NOT enough to resolve class A.
     enum A{};
     A a1;
     ::A a2;
     class ::A a3;
   }
