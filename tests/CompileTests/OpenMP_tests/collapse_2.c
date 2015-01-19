#include<stdio.h>

#ifdef _OPENMP
#include <omp.h>
#endif




int a[11][11];
int main(void)
{
int i, j;
int m = 10;
int n = 10;  

  for(i = 0; i < 11; i ++)
  {
     for(j = 0; j < 11; j ++)
     {
         a[i][j] = 0;
     }
  }
 

#pragma omp target map(to: m, n) map(tofrom:a[0:11][0:11])
#pragma omp parallel for collapse(2) private(j,i)// nowait    
  
  for (i=1;i<m;i+=1)
  {
     for (j=1;j<n;j+=1)
        {
            int k=3;
            int l=3;
            int z=3;
		    a[i][j]=i+j+l+k+z;	
	 }
  }
/*
  for(i = 0; i < 11; i ++)
  {
     for(j = 0; j < 11; j ++)
     {
        fprintf(stderr, "%d\n", a[i][j]);

     }
  }
 */
    return 0;
}

