// test normalization for label that is attached to construct which is
// normalized. Normalized code must be inserted between label and
// construct.
#include <cstdio>

static void f()
{
  char a[16];
  char* ap;
  a[0]='0';
  a[1]='1';
  a[2]=0;
  printf("Text:%s",a+1);
}

int main(int argc, char * argv[])
{
  f();
}
