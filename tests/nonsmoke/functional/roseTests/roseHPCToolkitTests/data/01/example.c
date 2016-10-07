#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef double val_t;

static val_t *
gen (size_t n)
{
  val_t* x = (val_t *) malloc (sizeof(val_t) * n);
  if (x != NULL)
    {
      size_t i;
      for (i = 0; i < n; i++)
	  x[i] = (val_t)1.0 / n;
    }
  return x;
}

static val_t
dot (size_t n, const val_t* x, const val_t* y)
{
  size_t i;
  val_t sum;
  for (i = 0, sum = 0.0; i < n; i++)
    sum += x[i] * y[i];
  return sum;
}

static size_t
max (size_t n, const val_t* x)
{
  size_t i;
  size_t i_max;
  val_t v_max;

  if (n <= 0)
    return 0;

  v_max = x[0];
  i_max = 0;
  for (i = 1; i < n; i++)
    if (x[i] > v_max)
      {
	v_max = x[i];
	i_max = i;
      }
  return i_max;
}

static void
mv (size_t n, const val_t* A, const val_t* x, val_t* y)
{
  size_t j;
  memset (y, 0, sizeof(val_t) * n);
  for (j = 0; j < n; j++)
    {
      const val_t* Ap;
      register val_t xj = x[j];
      size_t i;
      for (i = 0, Ap = A + j*n; i < n; i++, Ap++)
	y[i] += Ap[0] * xj;
    }
}

int
main (int argc, char* argv[])
{
  size_t n;
  val_t* x;
  val_t* y;
  val_t* A;
  size_t i;

  /* outputs */
  val_t sum;
  size_t i_max;
  val_t y_max;

  if (argc != 2)
    {
      fprintf (stderr, "usage: %s <n>\n", argv[0]);
      return 1;
    }
  n = atoi (argv[1]);
  if (n <= 0)
    return 1;

  A = gen (n * n);
  x = gen (n);
  y = gen (n);

  if (A == NULL || x == NULL || y == NULL)
    {
      fprintf (stderr, "*** Out of memory ***\n");
      return 1;
    }

  sum = 0;
  for (i = 0; i < n; i++)
    sum += dot (n, x, y);
  mv (n, A, x, y);
  i_max = max (n, y);
  y_max = y[i_max];

  printf ("%g %lu %g\n", sum, (unsigned long)i_max, y_max);

  return 0;
}

/* eof */
