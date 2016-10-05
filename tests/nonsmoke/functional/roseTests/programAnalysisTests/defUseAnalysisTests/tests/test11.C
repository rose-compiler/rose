int f1() {return 1;}
int f2(int *x, int y, int z) { return 2;}

int foo()
{
  int z = f1();
  int* p = &z;
  for (int i=0;i<10;i++) {
    for (int j=i; j<10; j++) {
       z = f2(&z, 5, z);
       *p = f2(&z, 5, z);
    }
  }
  int res = *p; 
  return 0;
} 
