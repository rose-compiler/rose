// #include <iostream>
// #include <sstream>
// #include <stdio.h>
// #include <stdlib.h>

class A
{
public:
	int f1() {}
	int f2() { pf = &A::f1; return (this->*pf)(); }
	int (A::*pf) ();
};

// simple (trivial) example code used to demonstrate the call graph generatation

// #include <sstream>

void foo1();
void foo2();
void foo3();
void foo4();


void foo1()
   {
     foo1();
     foo2();
     foo3();
     foo4();

     void (*pointerToFunction)();

   }

void foo2()
   {
     foo1();  
     foo2();
     foo3();
     foo4();
   }

void foo3()
   {
     foo1();
     foo2();
     foo3();
     foo4();
   }

void foo4()
   {
     foo1();
     foo2();
     foo3();
     foo4();
   }

int main()
   {
     foo1();
     foo2();
     foo3();
     foo4();

     return 0;
   }
