// Test from Dan: demonstrate preprocessing info. attached

void foobar(int e)
{
#if 1
  int x;
#endif
  if (e) {
    int d;
    d = 0;
  }
  else {
    int d;
    d = 1;
  }
}
