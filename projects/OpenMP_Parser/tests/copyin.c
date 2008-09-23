/*
test for copyin 
*/
#include <stdio.h>
#ifdef _OPENMP
#include <omp.h>
#endif 

float x;
int y;
struct s_zi
{
  int id;
  char name[20];
  float height;
};

struct s_zi z={123,"hello",1.75};

#pragma omp threadprivate(x,y,z)

int main (int argc, char * argv[])
{
    x=1555.0;
    y=765;
#pragma omp parallel copyin(x,y,z)
  {
 printf("x=%f, y=%d,z.name=%s\n",x,y,z.name);
  }

  return 0;
}

