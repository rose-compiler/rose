// t0195.cc
// "default constructor" creation of a built-in type

typedef int myint;

void f()
{
  int i;
  i = myint();      // invoke it like a default constructor; indeterminate value
  
  // should also work w/o the typedef
  i = int();
  
  // this will be an error
  //ERROR(1): i = int(4,5);
}

