/*
test input for C++ code
	parallel region within a non-static member function

By C. Liao
*/
#include <stdio.h>
#include <iostream>
#ifdef _OPENMP
#include "omp.h"
#endif
using namespace std;

class Hello {
private:
  int i;
public:
  Hello(int m=0):i(m) {cout<<"Hello::constructor"<<endl; }
  ~Hello() {cout<<"Hello::destructor"<<endl;}
  Hello(const Hello&rhs) {cout<<"Hello::copy constructor"<<endl;}
  Hello& operator=(const Hello &rhs) 
    {
      cout<<"Hello::assignment operator"<<endl;
      return *this;
    }
  void run() {cout<<"Hello::run, Hello, world!"<<endl;}
  void pararun() 
  {
    int j;
    int m;
    m=9;
    #pragma omp parallel private(j) num_threads(4)
    {
      j=11;
      cout<<"Hello::pararun, Hello, world "<<i<<" !"<<endl;
      cout<<"variables j="<<j<<endl;
      cout<<"variables m="<<m<<endl;
    }
  }
private:
  int j;
};

int main(void)
{
Hello hello(99);
Hello hello88(88);

hello88.pararun();
hello.pararun();

#ifdef _OPENMP
  omp_set_nested(1);
#endif

#pragma omp parallel num_threads(4)
{
 printf("Hello,world!\n");
 hello.run();
 hello.pararun();
 hello88.pararun();
}
#pragma omp parallel num_threads(4)
{
 printf("1Hello,world!\n");
#pragma omp parallel num_threads(4)
 printf("2Hello,world!\n");
}

return 0;
}

