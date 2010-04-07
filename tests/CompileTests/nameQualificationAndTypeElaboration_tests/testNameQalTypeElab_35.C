// number #35

// This example shows how integer declarations can hide enum fields.

enum enum1 { zeroA };
void foo(enum1);
// int zeroA;

void foo(int);

void foo()
   {
     int zeroA;

     foo();
     foo(::zeroA);
   }
