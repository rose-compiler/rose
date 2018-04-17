#include "test2005_88.h"

int main()
   {
  // Note that if class X does not define a constructor then ther is no constructor initializer 
  // (SgConstructorInitializer) built in the AST.
     X<int> x1;

     Y<int> functionPrototype();
     Y<int> y1;
     Y<int> y2(1);

     return 0;
   }
