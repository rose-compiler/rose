//gcc-4.2-20060603/libgomp/testsuite/libgomp.c++
//
//Modified based on the test code from gcc
// by C. Liao, July 11, 2006

#include <omp.h> 
#include <assert.h>

struct B
{
  static int icount;
  static int dcount;
  static int xcount;

  B();
  B(const B &);
  ~B();
  B& operator=(const B &);
  void doit();
};

int B::icount;
int B::dcount;
int B::xcount;

B::B()
{
  #pragma omp atomic 
    icount++;
}

B::~B()
{
  #pragma omp atomic
    dcount++;
}

void B::doit()
{
  #pragma omp atomic
    xcount++;
}

static int nthreads;

void foo()
{
  B b;
  B a;
  #pragma omp parallel private(b)
    {
      #pragma omp master
	nthreads = omp_get_num_threads ();
      b.doit();
      a.doit();
    }
}

int main()
{
  omp_set_dynamic (0);
  omp_set_num_threads (2);
  foo();

  assert (B::xcount == nthreads*2); // two objects
  assert (B::icount == nthreads+1+1); // n private copies + 2 original ones
  assert (B::dcount == nthreads+1+1);

  return 0;
}
