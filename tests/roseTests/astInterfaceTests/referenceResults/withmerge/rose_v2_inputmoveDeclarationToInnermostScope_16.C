// Test from Dan: demonstrate preprocessing info. attached

void foobar(int e)
{
#if 1
  int x;
#endif
  if (e) {
    int d = 0;
  }
  else {
    int d = 1;
  }
}
