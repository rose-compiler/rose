// This is trying to represent the use of covariant return types, but I'm not tetecting them.

class A
   {
   };
 
class B : A 
   {
   };
 
// "Class B is more narrow than class A"
// Classes demonstrating method overriding: 
class C
   {
     A getFoo()
        {
//         return new A();
        }
   };

class D : C
   {
     B getFoo()
        {
//       return new B();
        }
   };



