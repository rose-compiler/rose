#include <stdio.h>

struct {
  int struct_x;
} my_struct;

class A {
 public:
    A();
  int class_x;

};

int main()
{
  int i=0;
  A* a = new A();
  a->class_x = 5;
  my_struct.struct_x = 2;

  printf("Print with only text\n");
}

