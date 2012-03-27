template <typename T>
class X
   {
     public:
//       X ();
   };


X<int> 
foo()
   {
  // This should unparse to: X<int> object1;
     X<int> object1;

  // This should unparse to: X<int> object2 = X<int>();
     X<int> object2 = X<int>();

  // This should unparse to: X<int> object3 = object1;
     X<int> object3 = object1;
   }

