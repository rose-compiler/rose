void *_pp_globalobject;
void *__ompc_lock_critical;
static void **_thdprv_x;
/*--------------------------------------------------------------------
  
  NAS Parallel Benchmarks 2.3 OpenMP C versions - EP
  This benchmark is an OpenMP C version of the NPB EP code.
  
  The OpenMP C versions are developed by RWCP and derived from the serial
  Fortran versions in "NPB 2.3-serial" developed by NAS.
  Permission to use, copy, distribute and modify this software for any
  purpose with or without fee is hereby granted.
  This software is provided "as is" without express or implied warranty.
  
  Send comments on the OpenMP C versions to pdp-openmp@rwcp.or.jp
  Information on OpenMP activities at RWCP is available at:
           http://pdplab.trc.rwcp.or.jp/pdperf/Omni/
  
  Information on NAS Parallel Benchmarks 2.3 is available at:
  
           http://www.nas.nasa.gov/NAS/NPB/
--------------------------------------------------------------------*/
/*--------------------------------------------------------------------
  Author: P. O. Frederickson 
          D. H. Bailey
          A. C. Woo
  OpenMP C version: S. Satoh
  
--------------------------------------------------------------------*/
#include "npb-C.h"
#include "npbparams.h"
/* parameters */
#define	MK		16
#define	MM		(M - MK)
#define	NN		(1 << MM)
#define	NK		(1 << MK)
#define	NQ		10
#define EPSILON		1.0e-8
#define	A		1220703125.0
#define	S		271828183.0
#define	TIMERS_ENABLED	FALSE
/* global variables */
/* common /storage/ */
#include "ompcLib.h" 
static double x[131072];
static double q[10];

static void __ompc_func_1(void **__ompc_args)
{
  double (*_ppthd_x)[131072];
  double *_pp_sx;
  _pp_sx = ((double *)(__ompc_args[0]));
  double *_pp_sy;
  _pp_sy = ((double *)(__ompc_args[1]));
  double *_pp_an;
  _pp_an = ((double *)(__ompc_args[2]));
  int *_pp_np;
  _pp_np = ((int *)(__ompc_args[3]));
  int *_pp_k;
  _pp_k = ((int *)(__ompc_args[4]));
  int *_pp_k_offset;
  _pp_k_offset = ((int *)(__ompc_args[5]));
  int *_pp_nthreads;
  _pp_nthreads = ((int *)(__ompc_args[6]));
  _ppthd_x = ((double (*)[131072])(_ompc_get_thdprv(&_thdprv_x,sizeof(x),&x)));
  _ompc_copyin_thdprv(_ppthd_x,&x,sizeof(x));
{
    double t1;
    double t2;
    double t3;
    double t4;
    double x1;
    double x2;
    int kk;
    int i;
    int ik;
    int l;
/* private copy of q[0:NQ-1] */
    double qq[10];
    for (i = 0; i < 10; i++) {
      qq[i] = 0.0000000000000000;
    }
{
      double _p_sx;
      _p_sx = 0;
      double _p_sy;
      _p_sy = 0;
      int _p_loop_index;
      int _p_loop_lower;
      int _p_loop_upper;
      int _p_loop_stride;
      _p_loop_lower = 1;
      _p_loop_upper =  *_pp_np + 1;
      _p_loop_stride = 1;
      _ompc_static_bsched(&_p_loop_lower,&_p_loop_upper,&_p_loop_stride);
      for (_p_loop_index = _p_loop_lower; _p_loop_index < _p_loop_upper; _p_loop_index += _p_loop_stride) {
        kk = ( *_pp_k_offset + _p_loop_index);
        t1 = 271828183.00000000;
        t2 =  *_pp_an;
/*      Find starting seed t1 for this kk. */
        for (i = 1; i <= 100; i++) {
          ik = (kk / 2);
          if ((2 * ik) != kk) {
            t3 = randlc(&t1,t2);
          }
          else {
          }
          if (ik == 0) {
            break; 
          }
          else {
          }
          t3 = randlc(&t2,t2);
          kk = ik;
        }
/*      Compute uniform pseudorandom numbers. */
        if ((0 == 1)) {
          timer_start(3);
        }
        else {
        }
        vranlc((2 * (1 << 16)),&t1,1220703125.0000000, *_ppthd_x -1);
        if ((0 == 1)) {
          timer_stop(3);
        }
        else {
        }
/*
c       Compute Gaussian deviates by acceptance-rejection method and 
c       tally counts in concentric square annuli.  This loop is not 
c       vectorizable.
*/
        if ((0 == 1)) {
          timer_start(2);
        }
        else {
        }
        for (i = 0; i < (1 << 16); i++) {
          x1 = ((2.0000000000000000 * (( *_ppthd_x)[2 * i])) - 1.0000000000000000);
          x2 = ((2.0000000000000000 * (( *_ppthd_x)[(2 * i) + 1])) - 1.0000000000000000);
          t1 = ((x1 * x1) + (x2 * x2));
          if (t1 <= 1.0000000000000000) {
            t2 = sqrt((((-2.0000000000000000) * log(t1)) / t1));
/* Xi */
            t3 = (x1 * t2);
/* Yi */
            t4 = (x2 * t2);
            l = ((((fabs(t3) > fabs(t4))?fabs(t3):fabs(t4))));
/* counts */
            qq[l] += 1.0000000000000000;
/* sum of Xi */
            _p_sx = (_p_sx + t3);
/* sum of Yi */
            _p_sy = (_p_sy + t4);
          }
          else {
          }
        }
        if ((0 == 1)) {
          timer_stop(2);
        }
        else {
        }
      }
      _ompc_reduction(&_p_sx,_pp_sx,14,6);
      _ompc_reduction(&_p_sy,_pp_sy,14,6);
      _ompc_barrier();
    }
    _ompc_enter_critical(&__ompc_lock_critical);
{
      for (i = 0; i <= (10 - 1); i++) {
        q[i] += (qq[i]);
      }
    }
    _ompc_exit_critical(&__ompc_lock_critical);
    if (_ompc_is_master()) {
       *_pp_nthreads = omp_get_num_threads();
    }
    else {
    }
  }
}

/*--------------------------------------------------------------------
      program EMBAR
c-------------------------------------------------------------------*/
/*
c   This is the serial version of the APP Benchmark 1,
c   the "embarassingly parallel" benchmark.
c
c   M is the Log_2 of the number of complex pairs of uniform (0, 1) random
c   numbers.  MK is the Log_2 of the size of each batch of uniform random
c   numbers.  MK can be set for convenience on a given system, since it does
c   not affect the results.
*/

int main(int argc,char **argv)
{
  int status = 0;
  _ompc_init(argc,argv);
  double Mops;
  double t1;
  double t2;
  double t3;
  double t4;
  double x1;
  double x2;
  double sx;
  double sy;
  double tm;
  double an;
  double tt;
  double gc;
  double dum[3] = {(1.0000000000000000), (1.0000000000000000), (1.0000000000000000)};
  int np;
  int ierr;
  int node;
  int no_nodes;
  int i;
  int ik;
  int kk;
  int l;
  int k;
  int nit;
  int ierrcode;
  int no_large_nodes;
  int np_add;
  int k_offset;
  int j;
  int nthreads = 1;
  boolean verified;
/* character*13 */
  char size[14];
/*
c   Because the size of the problem is too large to store in a 32-bit
c   integer for some classes, we put it into a string (for printing).
c   Have to strip off the decimal point put in there by the floating
c   point print statement (internal file)
*/
  printf("\n\n NAS Parallel Benchmarks 2.3 OpenMP C version - EP Benchmark\n");
  sprintf(size,"%12.0f",pow(2.0000000000000000,((28 + 1))));
  for (j = 13; j >= 1; j--) {
    if (((size[j])) == ('.')) {
      size[j] = ' ';
    }
    else {
    }
  }
  printf(" Number of random numbers generated: %13s\n",size);
  verified = (0);
/*
c   Compute the number of "batches" of random number pairs generated 
c   per processor. Adjust if the number of processors does not evenly 
c   divide the total number
*/
  np = (1 << (28 - 16));
/*
c   Call the random number generator functions and initialize
c   the x-array to reduce the effects of paging on the timings.
c   Also, call all mathematical functions that are used. Make
c   sure these initializations cannot be eliminated as dead code.
*/
  vranlc(0,(dum + 0),(dum[1]),(dum + 2));
  dum[0] = randlc((dum + 1),(dum[2]));
  for (i = 0; i < (2 * (1 << 16)); i++) {
    x[i] = (-9.9999999999999997e+98);
  }
  Mops = log(sqrt(fabs((((1.0000000000000000 > 1.0000000000000000)?1.0000000000000000:1.0000000000000000)))));
  timer_clear(1);
  timer_clear(2);
  timer_clear(3);
  timer_start(1);
  vranlc(0,&t1,1220703125.0000000,x);
/*   Compute AN = A ^ (2 * NK) (mod 2^46). */
  t1 = 1220703125.0000000;
  for (i = 1; i <= (16 + 1); i++) {
    t2 = randlc(&t1,t1);
  }
  an = t1;
  tt = 271828183.00000000;
  gc = 0.0000000000000000;
  sx = 0.0000000000000000;
  sy = 0.0000000000000000;
  for (i = 0; i <= (10 - 1); i++) {
    q[i] = 0.0000000000000000;
  }
/*
c   Each instance of this loop may be performed independently. We compute
c   the k offsets separately to take into account the fact that some nodes
c   have more numbers to generate than others
*/
  k_offset = (-1);
{
    void *__ompc_argv[7];
     *(__ompc_argv + 0) = ((void *)(&sx));
     *(__ompc_argv + 1) = ((void *)(&sy));
     *(__ompc_argv + 2) = ((void *)(&an));
     *(__ompc_argv + 3) = ((void *)(&np));
     *(__ompc_argv + 4) = ((void *)(&k));
     *(__ompc_argv + 5) = ((void *)(&k_offset));
     *(__ompc_argv + 6) = ((void *)(&nthreads));
    _ompc_do_parallel(__ompc_func_1,__ompc_argv);
  }
  for (i = 0; i <= (10 - 1); i++) {
    gc = (gc + (q[i]));
  }
  timer_stop(1);
  tm = timer_read(1);
  nit = 0;
  if ((28 == 24)) {
    if ((fabs(((sx - (-3247.8346520347400)) / sx)) <= 1.0000000000000000e-08) && (fabs(((sy - (-6958.4070783822972)) / sy)) <= 1.0000000000000000e-08)) {
      verified = (1);
    }
    else {
    }
  }
  else {
    if ((28 == 25)) {
      if ((fabs(((sx - (-2863.3197316457531)) / sx)) <= 1.0000000000000000e-08) && (fabs(((sy - (-6320.0536791094992)) / sy)) <= 1.0000000000000000e-08)) {
        verified = (1);
      }
      else {
      }
    }
    else {
      if ((28 == 28)) {{
          if ((fabs(((sx - (-4295.8751656298919)) / sx)) <= 1.0000000000000000e-08) && (fabs(((sy - (-15807.325736784311)) / sy)) <= 1.0000000000000000e-08)) {
            verified = (1);
          }
          else {
          }
          printf("Debug: 231, sx is:%f, sy is:%f\n",sx,sy);
        }
      }
      else {
        if ((28 == 30)) {
          if ((fabs(((sx - 40338.155424414981) / sx)) <= 1.0000000000000000e-08) && (fabs(((sy - (-26606.691928092350)) / sy)) <= 1.0000000000000000e-08)) {
            verified = (1);
          }
          else {
          }
        }
        else {
          if ((28 == 32)) {
            if ((fabs(((sx - 47643.679279953743) / sx)) <= 1.0000000000000000e-08) && (fabs(((sy - (-80840.729880437313)) / sy)) <= 1.0000000000000000e-08)) {
              verified = (1);
            }
            else {
            }
          }
          else {
          }
        }
      }
    }
  }
  Mops = ((pow(2.0000000000000000,((28 + 1))) / tm) / 1000000.0000000000);
  printf("EP Benchmark Results: \nCPU Time = %10.4f\nN = 2^%5d\nNo. Gaussian Pairs = %15.0f\nSums = %25.15e %25.15e\nCounts:\n",tm,28,gc,sx,sy);
  for (i = 0; i <= (10 - 1); i++) {
    printf("%3d %15.0f\n",i,(q[i]));
  }
  c_print_results(("EP"),'A',(28 + 1),0,0,nit,nthreads,tm,Mops,("Random numbers generated"),verified,("2.3"),("07 Aug 2006"),("rose"),("$(CC)"),("-L$(omniInstallPath)/lib/openmp/lib -lompc ..."),("-I../common -I/home/liao6/OpenMPtranslation"),("-D_OPENMP"),("-lm"),("randdp"));
  if ((0 == 1)) {
    printf("Total time:     %f",timer_read(1));
    printf("Gaussian pairs: %f",timer_read(2));
    printf("Random numbers: %f",timer_read(3));
  }
  else {
  }
  _ompc_terminate(status);
}

