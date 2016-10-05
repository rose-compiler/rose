// Test default initialization of function parameters using functions
int f(int n);
int n = 42;
void g1(int);
void g2(int m);
void g3(int m = 42);
void g4(int m = n);
void g5(int = f(n));
void g6(int m = f(n));


void foo()
   {
  // Now test the functions to make sure that they are declared properly
     f(1);
     g1(1);
     g2(1);
     g3();
     g4();
     g5();
     g6();
   }
