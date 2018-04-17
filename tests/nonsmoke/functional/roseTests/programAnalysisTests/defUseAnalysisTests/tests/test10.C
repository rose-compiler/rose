int f2(int *x, int y, int z) { return 2;}

int foo()
{
  int z = 1;
  int res = f2(&z, 5, z);
  return 0;
} 
