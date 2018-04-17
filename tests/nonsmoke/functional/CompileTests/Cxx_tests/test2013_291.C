
void foo (int x)
   {
     class A { public: A (int) {} operator bool () { return false; } };

  // Predicate in "if" statement can not contain a class declaration (only simple declaration).
  // if (class A { public: A (int) {} operator bool () { return false; } } x3 = 1)
     if (A x1 = 1)
          class A { public: int foo (int x) { return x; } } x3; 
// #if 1
       else
          class A { public: int foo (int x) { return x; } } x3;
// #endif
   }
