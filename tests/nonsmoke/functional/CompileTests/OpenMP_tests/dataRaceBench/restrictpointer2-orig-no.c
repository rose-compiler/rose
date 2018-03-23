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
The restrict type qualifier is an indication to the compiler that,
if the memory addressed by the restrict -qualified pointer is modified, no other pointer will access that same memory.
If a particular chunk of memory is not modified, it can be aliased through more than one restricted pointer.
A C99 restrict feature.
For gcc, you must use -std=c99 to compile this program.
*/

#include <stdlib.h>
#include <stdio.h>

void foo(int n, int * restrict  a, int * restrict b, int * restrict  c)
{
  int i;
#pragma omp parallel for 
  for (i = 0; i < n; i++)
    a[i] = b[i] + c[i];  
}

int main()
{
  int n = 1000;
  int * a , *b, *c;

  a = (int*) malloc (n* sizeof (int));
  if (a ==0)
  {
    fprintf (stderr, "skip the execution due to malloc failures.\n");
    return 1;
  }

  b = (int*) malloc (n* sizeof (int));
  if (b ==0)
  {
    fprintf (stderr, "skip the execution due to malloc failures.\n");
    return 1;
  }

  c = (int*) malloc (n* sizeof (int));
  if (c ==0)
  {
    fprintf (stderr, "skip the execution due to malloc failures.\n");
    return 1;
  }

  foo (n, a, b,c);

  free (a);
  free (b);
  free (c);
  return 0;
}  

