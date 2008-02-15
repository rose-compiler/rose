// 13.3.3.2a.cc

int f(const int *);                       // line 3
int f(int *);                             // line 4
int i;
int j = __testOverload(f(&i), 4);         // Calls f(int *)
