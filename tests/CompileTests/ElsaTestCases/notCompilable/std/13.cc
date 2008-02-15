// 13.cc

double abs(double);        // line 3
int abs(int);              // line 4

void f()
{
  __testOverload(abs(1), 4);       // calls abs(int);
  __testOverload(abs(1.0), 3);     // calls abs(double);
}

