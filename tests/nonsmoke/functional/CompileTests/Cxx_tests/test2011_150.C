// This code demonstrates 2 different bugs.

class A
   {
   };

int
main ()
   {
  // bug 1
  // A* a = new A();
     A* a = new A;

     return 0;
   }

