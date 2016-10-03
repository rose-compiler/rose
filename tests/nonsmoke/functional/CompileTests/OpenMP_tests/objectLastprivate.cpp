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
  int i;
  B b; 
#pragma omp parallel for lastprivate(b)
  for (i =0; i < 2; i++)
  {
    b.doit(); 
    printf("Iteration %2d is carried out by thread %2d\n",\
        i, omp_get_thread_num());
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
  // assignment operator: called once by lastprivate(b)'s implementation
  assert (B::aCounter == 1);
  return 0;
}
