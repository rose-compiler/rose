// test normalization for label that is attached to construct which is
// normalized. Normalized code must be inserted between label and
// construct.
#include <cstdio>

static void f()
{
  char a[16];
  char* ap;
  printf("Text:",a+1);
}

int main(int argc, char * argv[])
{
  f();
}
