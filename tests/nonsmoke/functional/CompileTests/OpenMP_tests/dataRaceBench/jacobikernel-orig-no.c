/*
Copyright (c) 2017, Lawrence Livermore National Security, LLC.
Produced at the Lawrence Livermore National Laboratory
Written by Chunhua Liao, Pei-Hung Lin, Joshua Asplund,
Markus Schordan, and Ian Karlin
(email: liao6@llnl.gov, lin32@llnl.gov, asplund1@llnl.gov,
schordan1@llnl.gov, karlin1@llnl.gov)
LLNL-CODE-732144
All rights reserved.

This file is part of DataRaceBench. For details, see
https://github.com/LLNL/dataracebench. Please also see the LICENSE file
for our additional BSD notice.

Redistribution and use in source and binary forms, with
or without modification, are permitted provided that the following
conditions are met:

* Redistributions of source code must retain the above copyright
  notice, this list of conditions and the disclaimer below.

* Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the disclaimer (as noted below)
  in the documentation and/or other materials provided with the
  distribution.

* Neither the name of the LLNS/LLNL nor the names of its contributors
  may be used to endorse or promote products derived from this
  software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL LAWRENCE LIVERMORE NATIONAL
SECURITY, LLC, THE U.S. DEPARTMENT OF ENERGY OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
THE POSSIBILITY OF SUCH DAMAGE.
*/

/*
Two parallel for loops within one single parallel region,
combined with private() and reduction().
*/
#include <stdio.h>
#include <math.h>

#define MSIZE 200
int n=MSIZE, m=MSIZE, mits=1000;
double tol=0.0000000001, relax = 1.0, alpha = 0.0543;
double u[MSIZE][MSIZE], f[MSIZE][MSIZE], uold[MSIZE][MSIZE];
double dx, dy;

void
initialize ()
{
  int i, j, xx, yy;

  dx = 2.0 / (n - 1);
  dy = 2.0 / (m - 1);

/* Initialize initial condition and RHS */
//#pragma omp parallel for private(i,j,xx,yy)
  for (i = 0; i < n; i++)
    for (j = 0; j < m; j++)
    {
      xx = (int) (-1.0 + dx * (i - 1));       /* -1 < x < 1 */
      yy = (int) (-1.0 + dy * (j - 1));       /* -1 < y < 1 */
      u[i][j] = 0.0;
      f[i][j] = -1.0 * alpha * (1.0 - xx * xx) * (1.0 - yy * yy)
        - 2.0 * (1.0 - xx * xx) - 2.0 * (1.0 - yy * yy);

    }
}

void
jacobi ()
{
  double omega;
  int i, j, k;
  double error, resid,  ax, ay, b;

  omega = relax;
/* Initialize coefficients */

  dx = 2.0 / (n - 1); 
  dy = 2.0 / (m - 1);

  ax = 1.0 / (dx * dx);         /* X-direction coef */
  ay = 1.0 / (dy * dy);         /* Y-direction coef */
  b = -2.0 / (dx * dx) - 2.0 / (dy * dy) - alpha;       /* Central coeff */

  error = 10.0 * tol;
  k = 1;

  while (k <= mits)
    {
      error = 0.0;

/* Copy new solution into old */
#pragma omp parallel
      {
#pragma omp for private(i,j)
        for (i = 0; i < n; i++)
          for (j = 0; j < m; j++)
            uold[i][j] = u[i][j];
#pragma omp for private(i,j,resid) reduction(+:error) nowait
        for (i = 1; i < (n - 1); i++)
          for (j = 1; j < (m - 1); j++)
            {
              resid = (ax * (uold[i - 1][j] + uold[i + 1][j])
                       + ay * (uold[i][j - 1] + uold[i][j + 1]) +
                       b * uold[i][j] - f[i][j]) / b;

              u[i][j] = uold[i][j] - omega * resid;
              error = error + resid * resid;
            }
      }
/*  omp end parallel */

/* Error check */

     k = k + 1;
      error = sqrt (error) / (n * m);
    }                           /*  End iteration loop */

  printf ("Total Number of Iterations:%d\n", k);
  printf ("Residual:%E\n", error);
}

int main()
{
  initialize();
  jacobi();
  return 0;
}
