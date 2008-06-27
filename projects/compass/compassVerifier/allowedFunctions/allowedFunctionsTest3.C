#include <stdio.h>
#include <stdlib.h>

class Class
{
  public:
    int foobar(){ int ret = 1; fprintf( stdout, "..." ); return ret; }
    int keroberos(){ int ret = 0; printf( "Goodbye Cruel World...\n" ); return ret; }
};

void foo()
{
  return;
}

int add( int a, int b, ... )
{
  return 0;
}

int bar()
{
  return 0;
}

int main()
{
  char buf[256] = "\0";

  foo();

  sprintf( buf, "Hello World!\n" );

  printf( "%s\n", buf );

  Class c;

  c.foobar();
  c.keroberos();

  exit(999999999);

  return bar();
}
