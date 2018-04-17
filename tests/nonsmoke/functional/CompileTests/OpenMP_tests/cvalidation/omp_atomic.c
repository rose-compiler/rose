/***** We checked the operations of 
****** ++, --, +, -, *, /, &, |, << and >> for atomic directives.
****** especially we checked both integer and double precision for
****** +, -, /.  It is revised by Zhenying Liu of University of Houston.
*****/
#include <stdio.h>
#include <math.h>
#include "omp_testsuite.h"

int
check_omp_atomic (FILE * logFile)
{
  int sum = 0;
  int known_sum;
  double dsum = 0;
  double dknown_sum;
  double dt = 0.5;		/* base of geometric row for + and - test */
  double rounding_error = 1.E-9;
#define DOUBLE_DIGITS 20	/* dt^DOUBLE_DIGITS */
  int diff;
  double ddiff;
  int product = 1;
  int known_product;
#define MAX_FACTOR 10
#define KNOWN_PRODUCT 3628800	/* 10! */
/*  int logic_and = 1; 
  int logic_or = 0; */
  int bit_and = 1;
  int bit_or = 0;
  int exclusiv_bit_or = 0;
  int logics[LOOPCOUNT];
  int i;
  double dpt, div;
  int x;
  int result = 0;

  dt = 1. / 3.;
  known_sum = (LOOPCOUNT * (LOOPCOUNT + 1)) / 2;

#pragma omp parallel
  {
#pragma omp for 
    for (i = 1; i <= LOOPCOUNT; i++)
      {
#pragma omp atomic
	sum += i;
      }
  }

  if (known_sum != sum)
    {
      result++;
      fprintf (logFile,
               "Error in sum with integers: Result was %d instead of %d.\n",
	       sum, known_sum);
    }

  diff = (LOOPCOUNT * (LOOPCOUNT + 1)) / 2;
#pragma omp parallel
  {
#pragma omp for 
    for (i = 1; i <= LOOPCOUNT; ++i)
      {
#pragma omp atomic
	diff -= i;
      }
  }

  if (diff != 0)
    {
      result++;
      fprintf (logFile,
               "Error in difference with integers: Result was %d instead of 0.\n",
	       diff);
    }

  /* Tests for doubles */
  dsum = 0;
  dpt = 1;

  for (i = 0; i < DOUBLE_DIGITS; ++i)
    {
      dpt *= dt;
    }
  dknown_sum = (1 - dpt) / (1 - dt);
#pragma omp parallel
  {
#pragma omp for 
    for (i = 0; i < DOUBLE_DIGITS; ++i)
      {
#pragma omp atomic
	dsum += pow (dt, i);
      }
  }

  if (dsum != dknown_sum && (fabs (dsum - dknown_sum) > rounding_error))
    {
      result++;
      fprintf (logFile,
               "\nError in sum with doubles: Result was %f instead of: %f (Difference: %E)\n",
	       dsum, dknown_sum, dsum - dknown_sum);
    }

  dpt = 1;

  for (i = 0; i < DOUBLE_DIGITS; ++i)
    {
      dpt *= dt;
    }
  ddiff = (1 - dpt) / (1 - dt);
#pragma omp parallel
  {
#pragma omp for 
    for (i = 0; i < DOUBLE_DIGITS; ++i)
      {
#pragma omp atomic
	ddiff -= pow (dt, i);
      }
  }
  if (fabs (ddiff) > rounding_error)
    {
      result++;
      fprintf (logFile,
               "Error in Difference with doubles: Result was %E instead of 0.0\n",
	       ddiff);
    }

#pragma omp parallel
  {
#pragma omp for 
    for (i = 1; i <= MAX_FACTOR; i++)
      {
#pragma omp atomic
	product *= i;
      }
  }

  known_product = KNOWN_PRODUCT;
  if (known_product != product)
    {
      result++;
      fprintf (logFile,
               "Error in Product with integers: Result was %d instead of %d\n",
	       product, known_product);
    }

   product = KNOWN_PRODUCT;
#pragma omp parallel
  {
#pragma omp for
    for (i = 1; i <= MAX_FACTOR; i++)
      {
#pragma omp atomic
        product /= i;
      }
  }

  if (product != 1)
    {
      result++;
      fprintf (logFile,
               "Error in division with integers: Result was %d instead of 1\n",
               product );
    }

  div = 5.0E+5;
#pragma omp parallel
  {
#pragma omp for
    for (i = 1; i <= MAX_FACTOR; i++)
      {
#pragma omp atomic
        div /= i;
      }
  }

  if ( fabs(div-0.137787) >= 1.0E-4 )
    {
      result++;
      fprintf (logFile,
               "Error in division with double: Result was %f instead of 0.137787\n", div);
    }


  x = 0;

#pragma omp parallel
  {
#pragma omp for 
    for (i = 0; i < LOOPCOUNT; ++i)
      {
#pragma omp atomic
	x++;
      }
  }

  if (x != LOOPCOUNT) 
    {
      result++;
      fprintf (logFile, "Error in ++\n");
    }

#pragma omp parallel
  {
#pragma omp for 
    for (i = 0; i < LOOPCOUNT; ++i)
      {
#pragma omp atomic
	x--;
      }
  }
  if ( x != 0)
    {
      result++;
      fprintf (logFile, "Error in --\n");
    }

  for (i = 0; i < LOOPCOUNT; ++i)
    {
      logics[i] = 1;
    }

#pragma omp parallel
  {
#pragma omp for 
    for (i = 0; i < LOOPCOUNT; ++i)
      {
#pragma omp atomic
	bit_and &=  logics[i];
      }
  }
  if (!bit_and)
    {
      result++;
      fprintf (logFile, "Error in BIT AND part 1\n");
    }

  bit_and = 1;
  logics[LOOPCOUNT / 2] = 0;

#pragma omp parallel
  {
#pragma omp for 
    for (i = 0; i < LOOPCOUNT; ++i)
      {
#pragma omp atomic
	bit_and &=  logics[i];
      }
  }
  if (bit_and)
    {
      result++;
      fprintf (logFile, "Error in BIT AND part 2\n");
    }

  for (i = 0; i < LOOPCOUNT; i++)
    {
      logics[i] = 0;
    }

#pragma omp parallel
  {
#pragma omp for 
    for (i = 0; i < LOOPCOUNT; ++i)
      {
#pragma omp atomic
	bit_or |=  logics[i];
      }
  }
  if (bit_or)
    {
      result++;
      fprintf (logFile, "Error in BIT OR part 1\n");
    }
  bit_or = 0;
  logics[LOOPCOUNT / 2] = 1;

#pragma omp parallel
  {
#pragma omp for 
    for (i = 0; i < LOOPCOUNT; ++i)
      {
#pragma omp atomic
	bit_or |=  logics[i];
      }
  }
  if (!bit_or)
    {
      result++;
      fprintf (logFile, "Error in BIT OR part 2\n");
    }

  for (i = 0; i < LOOPCOUNT; i++)
    {
      logics[i] = 0;
    }

#pragma omp parallel
  {
#pragma omp for 
    for (i = 0; i < LOOPCOUNT; ++i)
      {
#pragma omp atomic
	exclusiv_bit_or ^=  logics[i];
      }
  }
  if (exclusiv_bit_or)
    {
      result++;
      fprintf (logFile, "Error in EXCLUSIV BIT OR part 1\n");
    }

  exclusiv_bit_or = 0;
  logics[LOOPCOUNT / 2] = 1;

#pragma omp parallel
  {
#pragma omp for 
    for (i = 0; i < LOOPCOUNT; ++i)
      {
#pragma omp atomic
	exclusiv_bit_or ^=  logics[i];
      }
  }
  if (!exclusiv_bit_or)
    {
      result++;
      fprintf (logFile, "Error in EXCLUSIV BIT OR part 2\n");
    }

  x = 1;
  #pragma omp parallel
  {
#pragma omp for
    for (i = 0; i < 10; ++i)
      {
#pragma omp atomic
        x <<= 1;
      }
  }

  if ( x != 1024)
  { 
      result++;
      fprintf (logFile, "Error in <<\n");
      x = 1024;
  }

#pragma omp parallel
  {
#pragma omp for
    for (i = 0; i < 10; ++i)
      {
#pragma omp atomic
        x >>= 1;
      }
  }

  if ( x != 1 )
  {
      result++;
      fprintf (logFile, "Error in >>\n");
  }


  /*fprintf("\nResult:%d\n",result); */
  return (result == 0);
}

int
crosscheck_omp_atomic (FILE * logFile)
{
  int sum = 0;
  int known_sum;
  double dsum = 0;
  double dknown_sum;
  double dt = 0.5;		/* base of geometric row for + and - test */
  double rounding_error = 1.E-9;
#define DOUBLE_DIGITS 20	/* dt^DOUBLE_DIGITS */
  int diff;
  double ddiff;
  int product = 1;
  int known_product;
#define MAX_FACTOR 10
#define KNOWN_PRODUCT 3628800	/* 10! */
/*  int logic_and = 1; 
  int logic_or = 0; */
  int bit_and = 1;
  int bit_or = 0;
  int exclusiv_bit_or = 0;
  int logics[LOOPCOUNT];
  int i;
  double dpt, div;
  int x;
  int result = 0;

  dt = 1. / 3.;
  known_sum = (LOOPCOUNT * (LOOPCOUNT + 1)) / 2;

#pragma omp parallel
  {
#pragma omp for 
    for (i = 1; i <= LOOPCOUNT; i++)
      {
	sum += i;
      }
  }

  if (known_sum != sum)
    {
      result++;
      fprintf (logFile,
               "Error in sum with integers: Result was %d instead of %d.\n",
	       sum, known_sum);
    }

  diff = (LOOPCOUNT * (LOOPCOUNT + 1)) / 2;
#pragma omp parallel
  {
#pragma omp for 
    for (i = 1; i <= LOOPCOUNT; ++i)
      {
	diff -= i;
      }
  }

  if (diff != 0)
    {
      result++;
      fprintf (logFile,
               "Error in difference with integers: Result was %d instead of 0.\n",
	       diff);
    }

  /* Tests for doubles */
  dsum = 0;
  dpt = 1;

  for (i = 0; i < DOUBLE_DIGITS; ++i)
    {
      dpt *= dt;
    }
  dknown_sum = (1 - dpt) / (1 - dt);
#pragma omp parallel
  {
#pragma omp for 
    for (i = 0; i < DOUBLE_DIGITS; ++i)
      {
	dsum += pow (dt, i);
      }
  }

  if (dsum != dknown_sum && (fabs (dsum - dknown_sum) > rounding_error))
    {
      result++;
      fprintf (logFile,
               "\nError in sum with doubles: Result was %f instead of: %f (Difference: %E)\n",
	       dsum, dknown_sum, dsum - dknown_sum);
    }

  dpt = 1;

  for (i = 0; i < DOUBLE_DIGITS; ++i)
    {
      dpt *= dt;
    }
  ddiff = (1 - dpt) / (1 - dt);
#pragma omp parallel
  {
#pragma omp for 
    for (i = 0; i < DOUBLE_DIGITS; ++i)
      {
	ddiff -= pow (dt, i);
      }
  }
  if (fabs (ddiff) > rounding_error)
    {
      result++;
      fprintf (logFile,
               "Error in Difference with doubles: Result was %E instead of 0.0\n",
	       ddiff);
    }

#pragma omp parallel
  {
#pragma omp for 
    for (i = 1; i <= MAX_FACTOR; i++)
      {
	product *= i;
      }
  }

  known_product = KNOWN_PRODUCT;
  if (known_product != product)
    {
      result++;
      fprintf (logFile,
               "Error in Product with integers: Result was %d instead of %d\n",
	       product, known_product);
    }

   product = KNOWN_PRODUCT;
#pragma omp parallel
  {
#pragma omp for
    for (i = 1; i <= MAX_FACTOR; i++)
      {
        product /= i;
      }
  }

  if (product != 1)
    {
      result++;
/*      fprintf (logFile,
               "Error in division with integers: Result was %d instead of 1\n",
               product );
*/
    }

  div = 5.0E+5;
#pragma omp parallel
  {
#pragma omp for
    for (i = 1; i <= MAX_FACTOR; i++)
      {
        div /= i;
      }
  }

  if ( fabs(div-0.137787) >= 1.0E-4 )
    {
      result++;
/*      fprintf (logFile,
               "Error in division with double: Result was %f instead of 0.137787\n", div);
*/
    }


  x = 0;

#pragma omp parallel
  {
#pragma omp for 
    for (i = 0; i < LOOPCOUNT; ++i)
      {
	x++;
      }
  }

  if (x != LOOPCOUNT) 
    {
      result++;
      fprintf (logFile, "Error in ++\n");
    }

#pragma omp parallel
  {
#pragma omp for 
    for (i = 0; i < LOOPCOUNT; ++i)
      {
	x--;
      }
  }
  if ( x != 0)
    {
      result++;
      fprintf (logFile, "Error in --\n");
    }

  for (i = 0; i < LOOPCOUNT; ++i)
    {
      logics[i] = 1;
    }

#pragma omp parallel
  {
#pragma omp for 
    for (i = 0; i < LOOPCOUNT; ++i)
      {
	bit_and &=  logics[i];
      }
  }
  if (!bit_and)
    {
      result++;
      fprintf (logFile, "Error in BIT AND part 1\n");
    }

  bit_and = 1;
  logics[LOOPCOUNT / 2] = 0;

#pragma omp parallel
  {
#pragma omp for 
    for (i = 0; i < LOOPCOUNT; ++i)
      {
	bit_and &=  logics[i];
      }
  }
  if (bit_and)
    {
      result++;
      fprintf (logFile, "Error in BIT AND part 2\n");
    }

  for (i = 0; i < LOOPCOUNT; i++)
    {
      logics[i] = 0;
    }

#pragma omp parallel
  {
#pragma omp for 
    for (i = 0; i < LOOPCOUNT; ++i)
      {
	bit_or |=  logics[i];
      }
  }
  if (bit_or)
    {
      result++;
      fprintf (logFile, "Error in BIT OR part 1\n");
    }
  bit_or = 0;
  logics[LOOPCOUNT / 2] = 1;

#pragma omp parallel
  {
#pragma omp for 
    for (i = 0; i < LOOPCOUNT; ++i)
      {
	bit_or |=  logics[i];
      }
  }
  if (!bit_or)
    {
      result++;
      fprintf (logFile, "Error in BIT OR part 2\n");
    }

  for (i = 0; i < LOOPCOUNT; i++)
    {
      logics[i] = 0;
    }

#pragma omp parallel
  {
#pragma omp for 
    for (i = 0; i < LOOPCOUNT; ++i)
      {
	exclusiv_bit_or ^=  logics[i];
      }
  }
  if (exclusiv_bit_or)
    {
      result++;
      fprintf (logFile, "Error in EXCLUSIV BIT OR part 1\n");
    }

  exclusiv_bit_or = 0;
  logics[LOOPCOUNT / 2] = 1;

#pragma omp parallel
  {
#pragma omp for 
    for (i = 0; i < LOOPCOUNT; ++i)
      {
	exclusiv_bit_or ^=  logics[i];
      }
  }
  if (!exclusiv_bit_or)
    {
      result++;
      fprintf (logFile, "Error in EXCLUSIV BIT OR part 2\n");
    }

  x = 1;
  #pragma omp parallel
  {
#pragma omp for
    for (i = 0; i < 10; ++i)
      {
        x <<= 1;
      }
  }

  if ( x != 1024)
  { 
      result++;
      fprintf (logFile, "Error in <<\n");
      x = 1024;
  }

#pragma omp parallel
  {
#pragma omp for
    for (i = 0; i < 10; ++i)
      {
        x >>= 1;
      }
  }

  if ( x != 1 )
  {
      result++;
      fprintf (logFile, "Error in >>\n");
  }


  /*fprintf("\nResult:%d\n",result); */
  return (result == 0);
}
