#ifdef PROFILING_ON
#include <Profile/Profiler.h>
#endif

#include <sys/time.h>
#include <math.h>
#include <stdlib.h>

int main(int argc,char **argv)
{ 
  int theResult, theNumberOfRepetitions;
  int theArraySize,theIndexArraySize,i,j,rep;
  double *x,*u,dx,dt,pi;
  long *I,*indexList;
  long theIndex,theBaseSeed,theSeed,theRandomNumber;

#ifdef PROFILING_ON
  TAU_PROFILE("main()", "int (int, char **)", TAU_DEFAULT);
  TAU_PROFILE_INIT(argc,argv);
#endif
  
  theBaseSeed = 100;
  theSeed = theBaseSeed;
  pi=4.0*atan(1.0);
  
  if ( argc > 1 ){
    theArraySize = atoi(argv[1]);
  }
  else
  {  
    theArraySize = 100000;
  }

  theIndexArraySize = theArraySize-2;
  
  x = (double*) malloc(sizeof(double)*theArraySize);
  u = (double*) malloc(sizeof(double)*theArraySize);
  I = (long*) malloc(sizeof(long)*theIndexArraySize);
  indexList = (long*) malloc(sizeof(long)*theIndexArraySize);
  
  dx = 1.0/((double)theArraySize);
    
  dt = .5*dx;
  
  for(i=0;i<theArraySize; i++)
  {
    x[i] = 0.0 + i * dx;
    u[i] = sin(pi*x[i]);
  }

  for(i=0;i<theIndexArraySize;i++)
  {
    I[i] = -1;
    indexList[i] = i+1;
  }
  
  printf("filling indirection vector.\n");
  
  srand48( theSeed );  

  for(i=0;i<theIndexArraySize;i++)
  {
    theRandomNumber = lrand48( );
    
    theIndex = (long)( (theIndexArraySize-i) * (((double)(theRandomNumber))/(pow(2.0,31.0)+1.0)));
    
    I[i] = indexList[theIndex];

    /* shift indexList data by one */
    for(j=theIndex;j<theIndexArraySize-i-1;j++)
      indexList[j]=indexList[j+1];
    
  }
  printf("completed filling indirection vector.\n");
  
  theNumberOfRepetitions = 100;
    
#ifdef PROFILING_ON
  TAU_PROFILE_TIMER(loopTimer,"work loop","",TAU_DEFAULT);
  TAU_PROFILE_START( loopTimer );
#endif

  /*
   do the work we want to profile
  */
  for(rep = 0 ;rep < theNumberOfRepetitions; rep++)
  {
      
    for(i=0;i<theIndexArraySize; i++)
    {
      u[I[i]] += - dt * (u[I[i]] - u[I[i]-1])/dx;
    }
    u[0] = u[theArraySize-2];
    u[theArraySize-1] = u[1];
      
  }/*end of loop over repetitions */
    
  /*
     turn off the instrumentation
  */
#ifdef PROFILING_ON
  TAU_PROFILE_STOP( loopTimer );
#endif

  double sum=0.0;
      
  for(i=1;i<theArraySize-1; i++)
  {
    sum += dx *u[i];
  }
  printf("sum  = %f\n",sum);
    
    
#ifdef PROFILING_ON
  TAU_PROFILE_EXIT("Finished TAU profiling.");
#endif
    
  free(I);
  free(u);
  free(x);
  free(indexList);
  
  return 0;
}
 
