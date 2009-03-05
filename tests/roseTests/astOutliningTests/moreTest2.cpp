/* an example to show user-defined data types used within an outlined function
 * It is used to test AST copy across different source files.
 * */

// If this is attached to the dependent declaration it will be included (because the AST copy mechanism will copy the CPP directives).
// #include "stdio.h"

// This is a non-dependent declaration (so it will cause the #include to be NOT included in the generated outlined code in the separate file).
// int x;

class A 
{
  float e1;
};

void foobar();

void foo()
{
  int i;
#pragma rose_outline
  {
 // Class reference
    A a;

    i=0;

 // Function reference
    foo();
    foobar();
  }
}
