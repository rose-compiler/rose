
// this test code demonstrates what happens when a defining declaration does not exist.
// In this case the unparse will output the definition in the final declaration.
class X;

class Y
   {
     X* xptr;
  // typedef X* (*foo)(void);
   };
