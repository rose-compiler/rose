
void foo ()
   {
  // Test without declaration of variable (see test2014_124.C).
     class A
        {
          public:
               A (int) {}
               operator bool () { return false; }
        };

     if (A x = 0)
        {
        }
   }
