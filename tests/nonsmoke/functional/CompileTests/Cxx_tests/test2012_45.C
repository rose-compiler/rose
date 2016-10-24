namespace A
{
template <typename T>
class X
   {
     public:
         X ();
   };
}


A::X<int> 
foo()
   {
  // This should unparse to: X<int> object1;
      A::X<int> object1;

  // This should unparse to: X<int> object2 = X<int>();
     A::X<int> object2 = A::X<int>();

  // This should unparse to: X<int> object3 = object1;
     A::X<int> object3 = object1;
   }

