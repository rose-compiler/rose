extern void *_pp_globalobject;
extern void *__ompc_lock_critical;
#include "wtime.h"
#include <stdlib.h>
/*  Prototype  */
#include "ompcLib.h" 
extern void wtime_(double *);
/*****************************************************************/
/******         E  L  A  P  S  E  D  _  T  I  M  E          ******/
/*****************************************************************/

double elapsed_time()
{
  double t;
  wtime_(&t);
  return t;
}

double start[64];
double elapsed[64];
/*****************************************************************/
/******            T  I  M  E  R  _  C  L  E  A  R          ******/
/*****************************************************************/

void timer_clear(int n)
{
  elapsed[n] = 0.0000000000000000;
}

/*****************************************************************/
/******            T  I  M  E  R  _  S  T  A  R  T          ******/
/*****************************************************************/

void timer_start(int n)
{
  start[n] = elapsed_time();
}

/*****************************************************************/
/******            T  I  M  E  R  _  S  T  O  P             ******/
/*****************************************************************/

void timer_stop(int n)
{
  double t;
  double now;
  now = elapsed_time();
  t = (now - (start[n]));
  elapsed[n] += t;
}

/*****************************************************************/
/******            T  I  M  E  R  _  R  E  A  D             ******/
/*****************************************************************/

double timer_read(int n)
{
  return elapsed[n];
}

