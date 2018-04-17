// This test code is the C language verison of a strange test code for C++ (test2013_291.C)
// which demonstrates that there van be seperate scopes for the true and false statements 
// even when there are no explicit "{}" to define blocks (and thus scopes).

void foo (int x)
   {
  // Note that "{}" are required to support a declaration in the true and false blocks (for C).
     if (1)
        {
          struct A { int x; } x3; 
        }
#if 0
       else
        {
          struct A { int x; } x3;
        }
#endif
   }
