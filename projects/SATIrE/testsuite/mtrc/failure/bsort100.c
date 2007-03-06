/* bsort100.c */

/* All output disabled for wcsim */
#define WCSIM 1

/* A read from this address will result in an known value of 1 */
#define KNOWN_VALUE (int)(*((char *)0x80200001))

/* A read from this address will result in an unknown value */
#define UNKNOWN_VALUE (int)(*((char *)0x80200003))


#include <sys/types.h>
#include <sys/times.h>
#include <stdio.h>

#define WORSTCASE 1
#define FALSE 0
#define TRUE 1
#define NUMELEMS 100
#define MAXDIM   (NUMELEMS+1)

/* BUBBLESORT BENCHMARK PROGRAM:
 * This program tests the basic loop constructs, integer comparisons,
 * and simple array handling of compilers by sorting 10 arrays of
 * randomly generated integers.
 */

int Array[MAXDIM], Seed;
int factor;

main()
{
   long  StartTime, StopTime;
   float TotalTime;

#ifndef WCSIM
   printf("\n *** BUBBLE SORT BENCHMARK TEST ***\n\n");
   printf("RESULTS OF TEST:\n\n");
#endif
   Initialize(Array);
   /*   StartTime = ttime (); */
   BubbleSort(Array);
   /*   StopTime = ttime(); */
   /*   TotalTime = (StopTime - StartTime) / 1000.0; */
#ifndef WCSIM
   printf("     - Number of elements sorted is %d\n", NUMELEMS);
   printf("     - Total time sorting is %3.3f seconds\n\n", TotalTime);
#endif
}


int ttime()
/*
 * This function returns in milliseconds the amount of compiler time
 * used prior to it being called.
 */
{
   struct tms buffer;
   int utime;

   /*   times(&buffer);  not implemented */
   utime = (buffer.tms_utime / 60.0) * 1000.0;
   return(utime);
}


Initialize(Array)
int Array[];
/*
 * Initializes given array with randomly generated integers.
 */
{
   int  Index, fact;

#ifdef WORSTCASE
   factor = -1;
#else
   factor = 1;
#endif

fact = factor;
for (Index = 1; Index <= NUMELEMS; Index ++)
    Array[Index] = Index*fact * KNOWN_VALUE;
}



BubbleSort(Array)
int Array[];
/*
 * Sorts an array of integers of size NUMELEMS in ascending order.
 */
{
   int Sorted = FALSE;
   int Temp, LastIndex, Index, i;

   for (i = 1;
	i <= NUMELEMS-1;           /* apsim_loop 1 0 */
	i++)
   {
      Sorted = TRUE;
      for (Index = 1;
	   Index <= NUMELEMS-1;      /* apsim_loop 10 1 */
	   Index ++) {
         if (Index > NUMELEMS-i)
            break;
         if (Array[Index] > Array[Index + 1])
         {
            Temp = Array[Index];
            Array[Index] = Array[Index+1];
            Array[Index+1] = Temp;
            Sorted = FALSE;
         }
      }

      if (Sorted)
         break;
   }

#ifndef WCSIM
   if (Sorted || i == 1)
      fprintf(stderr, "array was successfully sorted in %d passes\n", i-1);
   else
      fprintf(stderr, "array was unsuccessfully sorted in %d passes\n", i-1);
#endif
}
