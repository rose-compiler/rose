/*
 * Liao, 10/29/2009
 * */
#include <stdio.h>
#include <omp.h>
#include <assert.h>

struct B
{
  static int cCounter; // constructor calling counter
  static int ccCounter; //copy constructor calling counter
  static int aCounter; //assignment operator calling counter
  static int dCounter; //destructor calling counter

  B();
  B(const B &);
  ~B();
  B& operator=(const B &);
  void doit();
};

int B::cCounter =0;
int B::ccCounter =0;
int B::aCounter =0;
int B::dCounter =0;

B::B()
{
#pragma omp critical
  cCounter ++;
  printf("constructor ..\n");
}

B::B(const B &b)
{
#pragma omp critical
  ccCounter ++;
  printf("copy constructor ..\n");
}

B::~B()
{
#pragma omp critical
  dCounter ++;
  printf("destructor ..\n");
}

B& B::operator=( const B& that)
{
 #pragma omp critical
  aCounter ++;
  printf("assign operator ..\n");
  if (this != &that)
  {
    // nothing to assign here
  }
  return *this;
}


void B::doit()
{
}

void foo()
{
  B b; 
#pragma omp parallel private(b)
  {
    b.doit(); 
  }
}

int main()
{
  omp_set_dynamic (0);
  omp_set_num_threads (4);
  foo();
  // constructor 1+ 1*threadNum
  assert (B::cCounter == (4 +1));
  // destructor 1+ 1*threadNum
  assert (B::dCounter == (4 +1));
  // copy constructor and assignment operator should not be used
  assert (B::ccCounter == 0);
  assert (B::aCounter == 0);
  return 0;
}
