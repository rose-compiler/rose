// test normalization for label that is attached to construct which is
// normalized. Normalized code must be inserted between label and
// construct.
static void f()
{
  int * data;
  goto sink;
 sink:
  if (data != __null) {
    ;
  }
}

int main(int argc, char * argv[])
{
  f();
}
