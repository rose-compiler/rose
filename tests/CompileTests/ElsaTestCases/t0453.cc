// t0453.cc
// coverage for some built-in functions

void f(int);

void g(int) {}

void f()
{
  //ERROR(1): __checkType();
  //ERROR(1): __getStandardConversion();
  //ERROR(1): __getImplicitConversion();
  //ERROR(1): __testOverload();
  //ERROR(1): __computeLUB();
  //ERROR(1): __checkCalleeDefnLine();

  //ERROR(2): __testOverload(f(3), 1);         // wrong line
  //ERROR(3): __testOverload(1+2, 1);          // not a function

  //ERROR(4): __checkCalleeDefnLine(f(3), 1);  // not a defined function
  //ERROR(5): __checkCalleeDefnLine(g(4), 1);  // wrong line
  //ERROR(6): __checkCalleeDefnLine(1+2, 1);   // not a function
}
