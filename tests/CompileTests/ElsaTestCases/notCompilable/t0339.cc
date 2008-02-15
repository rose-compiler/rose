// t0339.cc
// overload resolution when passing a function

void foo( int (*f)(int) );     // line 4
void foo( int (*f)(int, int, int, int) );

int g(int, int, int);          // line 7
int g(int, int);               // line 8
int g(int);                    // line 9

void foo2( int (*f)(int,int) );      // line 11
void foo3( int (*f)(int,int,int) );  // line 12

void bar()
{
  foo(g);        // pass third 'g'
  foo(&g);       // pass third 'g'
  foo2(g);       // pass second 'g'
  foo3(&g);      // pass first 'g'
  
  foo( ((((&((((g)))))))) );     // third 'g'
}

asm("collectLookupResults foo=4 g=9 foo=4 g=9 foo2=11 g=8 foo3=12 g=7 foo=4 g=9");
