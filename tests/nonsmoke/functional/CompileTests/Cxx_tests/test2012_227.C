void foo (int x)
   {
     class A
        {
          public:
               A () {}
               int foo (int x) { return x; }

               A (int) {}
               operator bool () { return false; }
        } x1;

  // BUG: the TRUE statment is output in the function scope and the true branch is empty.
     if (A x2 = 0)
          class A { public: int foo (int x) { return x; } } x3;
       else
          class A { public: int foo (int x) { return x; } } x4;
   }

