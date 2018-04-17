template <typename T>
class X
   {
     public:
         X ();
   };


void
foo()
   {
  // This should unparse to: X<int> object2 = X<int>::X();
     X<int> object2 = X<int>::X();
   }
