// simple (trivial) example code used to demonstrate the call graph generation
class A
{
public:
	int f1() { return 0;}
	int f2() { pf = &A::f1; return (this->*pf)(); }
	int (A::*pf) ();
};

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
