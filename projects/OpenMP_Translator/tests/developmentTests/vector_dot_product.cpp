#include <iostream>
#include <vector>
#include <math.h>
#define SIZE 50000000
int main()
{
  std::vector <double> v1 (SIZE), v2 (SIZE);
  double result=0.0;
  int i;

#pragma omp parallel for private(i)
  for (i=0;i<SIZE;i++)
  {  
    v1[i]=(double)i;
    v2[i]=(double)(i/2.0);
  }

#pragma omp parallel for private(i) reduction(+:result)
  for (i=0;i<SIZE;i++)
    result=result + sqrt(v1[i]*v1[i]+v2[i]*v2[i]);

  std::cout<<result<<std::endl;
  
}
