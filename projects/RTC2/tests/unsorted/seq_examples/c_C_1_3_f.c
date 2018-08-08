/*
* Copyright (c) 2005 Iowa State University, Glenn Luecke, James Coyle,
* James Hoekstra, Marina Kraeva, Olga Taborskaia, Andre Wehe, Ying Xu,
* and Ziyu Zhang, All rights reserved.
* Licensed under the Educational Community License version 1.0.
* See the full agreement at http://rted.public.iastate.edu/ .
*/
/*
*    Test name:      c_C_1_3_f.c
*
*    Test description:  Call C library function that has a prototype.
*                       The function returns a value of type double,
*                       but the prototype shows an integer return value.
*                       The library links at the compile time.
*
*    Error line:        46
*
*    Support files:     libm, index_array.txt, TEST_CONST.H, TEST_PARAM.H
*
*    Env. requirements: Not needed
*
*    Keywords:    incorrect argument data types
*        sin
*                       return double value
*
*    Last modified:  June 28, 2005
*
*    Programmer:        Olga Taborskaia, Iowa State University
*/

#include "TEST_PARAM.H"

#ifndef EXITCODE_OK
#define EXITCODE_OK 1
#endif

int sin(double);

int main(void)
{
   double varA ;
   double varB;

   varA = 1.1;

   varB = sin(varA);

   ret();
   if(ret2!=1) printf("%f\n", varB);     /* the value of ret2 = 1 */

   return EXITCODE_OK;
}
