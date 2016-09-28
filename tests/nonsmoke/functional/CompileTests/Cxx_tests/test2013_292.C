void foo (int x)
   {
     class A { public: A (int) {} operator bool () { return false; } };

  // Predicate in "if" statement can not contain a class declaration (only simple declaration).
  // Also "x3" can not be redefined in the "true" of "false" cases if it is declared in the conditional
  // (done not matter if "{}" are used to define new scope for the true or false branches.
  // if (class A { public: A (int) {} operator bool () { return false; } } x3 = 1)
     if (A x1 = 1)
        {
          class A { public: int foo (int x) { return x; } } x3;
        }
// #if 1
       else
        {
          class A { public: int foo (int x) { return x; } } x3;
        }
// #endif
   }
