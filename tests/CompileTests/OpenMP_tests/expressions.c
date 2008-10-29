/* some simple expressions */
void foo()
{
int i;
#pragma omp task if (99)
i=i++;

#pragma omp task if (i)
i=i++;

#pragma omp task if (i=0)
i=i++;

#pragma omp task if (i+=0)
i=i++;

#pragma omp task if (i==0)
i=i++;

#pragma omp task if (i!=0)
i=i++;

#pragma omp task if (i<0)
i=i++;

#pragma omp task if (i>0)
i=i++;
} 
