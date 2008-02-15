int f2(int *x, int y, int z) { return 2;}

int main()
{
  int z = 1;
  int res = f2(&z, 5, z);
} 
