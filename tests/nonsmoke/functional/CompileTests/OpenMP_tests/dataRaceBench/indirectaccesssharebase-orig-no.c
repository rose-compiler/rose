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
This example is to mimic a memory access pattern extracted from an LLNL proxy app.
Two pointers have distance of 12.
They are used as base addresses of two arrays, indexed through an index set.
The index set has no two indices with distance of 12.
So there is no loop carried dependence.
*/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define N 180
int indexSet[N] = {
521, 523, 525, 527, 529, 531,
547, 549, 551, 553, 555, 557,
573, 575, 577, 579, 581, 583,
599, 601, 603, 605, 607, 609,
625, 627, 629, 631, 633, 635,

651, 653, 655, 657, 659, 661,
859, 861, 863, 865, 867, 869,
885, 887, 889, 891, 893, 895,
911, 913, 915, 917, 919, 921, 
937, 939, 941, 943, 945, 947,

963, 965, 967, 969, 971, 973,
989, 991, 993, 995, 997, 999,
1197, 1199, 1201, 1203, 1205, 1207,
1223, 1225, 1227, 1229, 1231, 1233,
1249, 1251, 1253, 1255, 1257, 1259,

1275, 1277, 1279, 1281, 1283, 1285,
1301, 1303, 1305, 1307, 1309, 1311,
1327, 1329, 1331, 1333, 1335, 1337,
1535, 1537, 1539, 1541, 1543, 1545,
1561, 1563, 1565, 1567, 1569, 1571,

1587, 1589, 1591, 1593, 1595, 1597,
1613, 1615, 1617, 1619, 1621, 1623,
1639, 1641, 1643, 1645, 1647, 1649,
1665, 1667, 1669, 1671, 1673, 1675,
1873, 1875, 1877, 1879, 1881, 1883,

1899, 1901, 1903, 1905, 1907, 1909,
1925, 1927, 1929, 1931, 1933, 1935,
1951, 1953, 1955, 1957, 1959, 1961,
1977, 1979, 1981, 1983, 1985, 1987,
2003, 2005, 2007, 2009, 2011, 2013};

int main (int argc, char* argv[])
{
  double * base = (double*) malloc(sizeof(double)* (2013+12+1));
  if (base == 0)
  {
    printf("Error, malloc() returns NULL. End execution. \n");
    return 1;  
  }

  double * xa1 = base;
  double * xa2 = base + 12;
  int i;

  for (i =521; i<= 2025; ++i)
  {
    base[i]=0.0;
  }

#pragma omp parallel for
  for (i =0; i< N; ++i) // this level of loop has no loop carried dependence
  {
    int idx = indexSet[i];
    xa1[idx]+= 1.0;
    xa2[idx]+= 3.0;
  }

  // verify the results, no overlapping of xa1 vs. xa2, no addition happens to the same element twice
  for (i =521; i<= 2025; ++i)
  {
    //printf ("%f  ", base[i]);
    assert (base[i]!=4.0);
  }

  free (base);
  return  0;
}

