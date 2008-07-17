#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <sstream>

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

  std::stringstream ss;

  ss << std::endl;
  std::cout << ss << std::endl;

  exit(999999999);

  return bar();
}
