
int array[100][100];
extern int foo(int i, int j);
extern int bar();
int a, b;
int place1, place2;
#pragma CUDA kernel
int foo();
#pragma hc   entry
int foo();
#pragma hc suspendable entry
int foo();
#pragma CUDA 1 autodim ( ((1<<2 < (a==b) & place2)?(a||b|2):1^1&&0) , array[8][77])
int foo();
#pragma CUDA place1 autodim (~1, foo(place1, place2), !3, -a)
int foo();
#pragma CUDA (a+(b-a)*b/sizeof(double)+5) dim ((place1)*sizeof(array), (int)2)
int foo();
#pragma CUDA (place2++)++ dim ((3,2,place1!=place2), 4/bar(), place1+5*place2)
int foo();

