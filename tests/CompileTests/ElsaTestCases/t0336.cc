// t0336.cc
// default argument union

void f(int, int);
void f(int x, int y = 2);

void f();


void g(int x, int y, int z);
void g(int x, int y, int z=3);
void g(int x, int y=2, int z);
void g(int x=1, int y, int z);

// would provide a redundant value
//ERROR(2): void g(int x=1, int y, int z);

void g(char*);


void h(int x, int y);

// would make a param w/o default after one with default
//ERROR(1): void h(int x=1, int y);


void foo()
{
  f(1);     // needs default arg for 'y' from 2nd decl

  g();      // needs all defaults
}
