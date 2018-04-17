/* test two omp for loops in two subteams
 and a single thread in the 3rd subteam
*/ 
#include <stdio.h>
#include <stdlib.h>

#if defined(_OPENMP)
#include <omp.h>
#endif /* _OPENMP */

/*by Liao, new data types and functions to support thread subteams*/
/*compiler generated new data type to store thread ids in a subteam*/

typedef struct{
   int iCount;
   int *iThreadIds;
} omp_id_set_t;
omp_id_set_t idSet1,idSet2,idSet3;

extern int __ompc_is_in_idset();
extern void __ompc_subteam_create();
void *subteam1, *subteam2, *subteam3; /*use it as &threadsubteam*/

#define NUMELEMENT 100

int main(void)
{
  int a[NUMELEMENT];
  int i,j=0,sum=0,sum2=0;

  /* assume 5 threads */
#ifdef _OPENMP
  omp_set_num_threads(5);
#endif
  /* manual code to generate the thread subteams' ID sets currently */
  /*stuff code to get ids from the thread ids in the subteam*/
  idSet1.iCount=2;
  idSet1.iThreadIds=(int *)malloc(2*sizeof(int));
  idSet1.iThreadIds[0]=1;
  idSet1.iThreadIds[1]=3;

  idSet2.iCount=2;
  idSet2.iThreadIds=(int *)malloc(2*sizeof(int));
  idSet2.iThreadIds[0]=0;
  idSet2.iThreadIds[1]=2;

  idSet3.iCount=1;
  idSet3.iThreadIds=(int *)malloc(1*sizeof(int));
  idSet3.iThreadIds[0]=1;


#pragma omp parallel
  {
    /* onthreads(0,2) */
#pragma omp for reduction(+:sum)
    for (i=1;i<=NUMELEMENT;i++)
    {
      sum = sum +i;
    }

    /* onthreads(1,3) */
#pragma omp for schedule(dynamic,5)
    for (i=0;i<NUMELEMENT;i++)
    {
      a[i]=9;
    }

    /* onthread 4 */
#pragma omp single 
    {
#ifdef _OPENMP
      j=omp_get_thread_num();
#endif
      printf("I am the single one: %d\n",j );
    }
  }/*end of parallel */

  /*------verify results---------------*/
  for (i=0;i<NUMELEMENT;i++)
  {
    sum2=sum2+a[i];
  }
  printf("sum=%d\n",sum);
  printf("sum2=%d\n",sum2);
  return 0;
}

