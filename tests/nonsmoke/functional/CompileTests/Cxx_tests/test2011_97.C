// This is taken from hiddenTypeAndDeclarationListTests/test57.C (and simplified).
// test for (not) hiding a function from a class' function:

int foo(int a) { return a; }

class A 
   {
     public:
          void foo()
             {
            // See C++ standard 13.2 example #2
            // int r = foo(1); // not possible because void foo hides int foo
               int r = ::foo(1); // not possible because void foo hides int foo
             }
   };

