#include<assert.h> 

int main(void)
{
  int i =100;
  int j = 77;
#pragma omp parallel firstprivate(i)
  {
    assert(i == 100);
    assert(j == 77);
  }
#pragma omp parallel private(i)
  {
    assert(i != 100);
  }

  return 0;
}
