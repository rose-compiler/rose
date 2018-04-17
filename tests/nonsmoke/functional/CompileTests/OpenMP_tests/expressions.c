/* some simple expressions */
void foo()
{
int i;
#pragma omp task if (99)
i++;

#pragma omp task if (i)
i++;

#pragma omp task if (i==0)
i++;

#pragma omp task if (i+=0)
i++;

#pragma omp task if (i==0)
i++;

#pragma omp task if (i!=0)
i++;

#pragma omp task if (i<0)
i++;

#pragma omp task if (i>0)
i++;

} 
