void foo (int x)
   {
  // Predicate in "if" statement can not contain a class declaration (only simple declaration).
  // if (class A { public: A (int) {} operator bool () { return false; } } x3 = 1)
     if (true)
          class A {int x; } x3; 
// #if 1
       else
          class A { int x; } x3;
// #endif
   }
