// t0576.cc
// throw specs are not part of function pointer type


int f1(int);
int f2(int) throw();
int f3(int) throw(int);
int f4(int) throw(int,float);


typedef int ft1(int);
typedef int ft2(int) throw();


void foo()
{
  int (*f)(int);
  
  f = f1;
  f = f2;
  f = f3;
  f = f4;
}
