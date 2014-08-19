
void foo (int x)
   {
  // DQ: The declaration of "x1" causes the use of A to detect an unset parent for the class declaration.
  // This is likely caused by new support for conditional expressions that detects the call the the bool() 
  // conversion operator.
     class A
        {
          public:
               A () {}
               int foo (int x) { return x; }

               A (int) {}
               operator bool () { return false; }

        } x1;

     for (; A x2 = 0;)
        {
        }
   }

