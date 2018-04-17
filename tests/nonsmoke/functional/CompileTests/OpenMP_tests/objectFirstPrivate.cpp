// A test to verify that a copy constructor instead of constructor is called for firstprivate class objects
// { dg-do run }
#include <stdio.h>
#include <omp.h>
#include <assert.h>

struct B
{
  static B *expected;
  static int ccCounter;

  B(){};
  B(int);
  B(const B &);
  ~B();
//  B& operator=(const B &);
  void doit();
};

int B::ccCounter =0;

B * B::expected;
//! Constructor will change the static variable
B::B(int)
{
  expected = this;
}

//! Copy constructor will not change the static variable
// so the source object == the static variable
B::B(const B &b)
{
#pragma omp critical
  ccCounter ++;

  printf("copy constructor ..\n");
  assert (&b == expected); // this insures that the parameter is passed by address, not by value copying during outlining
}

B::~B()
{
}

void B::doit()
{
  assert (this != expected); // the static variable should be different from this, which is created from copy constructor
}

void foo()
{
  B b(0); // This should set the static variable
#pragma omp parallel firstprivate(b)
  {
    b.doit(); // copy constructor should be called , the static variable should be intact
  }
}

int main()
{
  omp_set_dynamic (0);
  omp_set_num_threads (4);
  foo();
  assert (B::ccCounter == 4);
  return 0;
}
