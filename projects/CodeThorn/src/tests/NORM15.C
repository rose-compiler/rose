// test normalization for label that is attached to construct which is
// normalized. Normalized code must be inserted between label and
// construct.
static char* g(char* x) {
  return x-1;
}
static bool f()
{
  char a[16];
  char* ap;
  ap=g(a+1);
  return ap==a;
}

int main(int argc, char * argv[])
{
  f();
}
