/* an example to show user-defined data types used within an outlined function
 * It is used to test AST copy across different source files.
 * */

class A 
{
  float e1;
};

void foo()
{
  int i;
#pragma rose_outline
  {
    A a;
    i=0;
  }
}
