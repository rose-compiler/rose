#include "npb-C.h"
#include "npbparams.h"
/* parameters */
#define MK              16
#define MM              (M - MK)
#define NN              (1 << MM)
#define NK              (1 << MK)
#define NQ              10
#define EPSILON         1.0e-8
#define A               1220703125.0
#define S               271828183.0
#define TIMERS_ENABLED  FALSE
/* global variables */
/* common /storage/ */
#include "ompcLib.h"
/* decompile from source='ep.c' input=<stdin>' */
# 1 "ep.c"
static void **_thdprv_x;
//static void *_thdprv_x;
void *__ompc_lock_critical;
static void __ompc_func_3 (void **__ompc_args);
static double x[131072];
static double q[10];
static void
__ompc_func_3 (void **__ompc_args)
{
  auto double *_pp_sx;
  auto double *_pp_sy;
  auto int *_pp_np;
  auto int *_pp_k_offset;
  auto double *_pp_an;
  auto int *_pp_nthreads;
  auto double *_ppthd_x;
  (_ppthd_x) = (((double *) (_ompc_get_thdprv (&_thdprv_x, 1048576, x))));
  (_pp_sx) = (((double *) (*__ompc_args)));
  (_pp_sy) = (((double *) (*((__ompc_args) + (1)))));
  (_pp_np) = (((int *) (*((__ompc_args) + (2)))));
  (_pp_k_offset) = (((int *) (*((__ompc_args) + (3)))));
  (_pp_an) = (((double *) (*((__ompc_args) + (4)))));
  (_pp_nthreads) = (((int *) (*((__ompc_args) + (5)))));
  _ompc_copyin_thdprv (_ppthd_x, x, 1048576);
  {
    auto double t1;
    auto double t2;
    auto double t3;
    auto double t4;
    auto double x1;
    auto double x2;
    auto int kk;
    auto int i;
    auto int ik;
    auto int l;
    auto double qq[10];

# 150 "ep.c"
    for ((i) = (0); (i) < (10); (i)++)
      {
	(*((qq) + (i))) = (0.0);
      }
    {
      auto double _p_sx;
      auto double _p_sy;
      auto int _p_k;
      auto int _p_k_0;
      auto int _p_k_1;
      auto int _p_k_2;
      (_p_sy) = (0.0);
      (_p_sx) = (0.0);
      (_p_k_0) = (1);
      (_p_k_1) = ((*_pp_np) + (1));
      (_p_k_2) = (1);
      _ompc_static_bsched (&_p_k_0, &_p_k_1, &_p_k_2);
# 153 "ep.c"
      for ((_p_k) = (_p_k_0); (_p_k) < (_p_k_1); (_p_k) += (_p_k_2))
	{

# 154 "ep.c"
	  (kk) = ((*_pp_k_offset) + (_p_k));
# 155 "ep.c"
	  (t1) = (2.71828183E8);
# 156 "ep.c"
	  (t2) = (*_pp_an);
# 160 "ep.c"
	  for ((i) = (1); (i) <= (100); (i)++)
	    {

# 161 "ep.c"
	      (ik) = ((kk) / (2));
# 162 "ep.c"
	      if (((2) * (ik)) != (kk))
		{
		  (t3) = (randlc (&(t1), t2));
		}
# 163 "ep.c"
	      if ((ik) == (0))
# 163 "ep.c"
		break;
# 164 "ep.c"
	      (t3) = (randlc (&(t2), t2));
# 165 "ep.c"
	      (kk) = (ik);
	    }
# 170 "ep.c"
	  if ((0) == (1))
	    {
	      timer_start (3);
	    }
# 171 "ep.c"
	  vranlc ((2) * ((1) << (16)), &(t1), 1.220703125E9,
		  (_ppthd_x) - (1));
# 172 "ep.c"
	  if ((0) == (1))
	    {
	      timer_stop (3);
	    }
# 179 "ep.c"
	  if ((0) == (1))
	    {
	      timer_start (2);
	    }
# 181 "ep.c"
	  for ((i) = (0); (i) < ((1) << (16)); (i)++)
	    {

# 182 "ep.c"
	      (x1) = (((2.0) * (*((_ppthd_x) + ((2) * (i))))) - (1.0));
# 183 "ep.c"
	      (x2) =
		(((2.0) * (*((_ppthd_x) + (((2) * (i)) + (1))))) - (1.0));
# 184 "ep.c"
	      (t1) = (((x1) * (x1)) + ((x2) * (x2)));
# 185 "ep.c"
	      if ((t1) <= (1.0))
		{

# 186 "ep.c"
		  (t2) = (sqrt (((-(2.0)) * (log (t1))) / (t1)));
# 187 "ep.c"
		  (t3) = ((x1) * (t2));
# 188 "ep.c"
		  (t4) = ((x2) * (t2));
# 189 "ep.c"
		  (l) =
		    (((int)
		      (((fabs (t3)) >
			(fabs (t4))) ? (fabs (t3)) : (fabs (t4)))));
# 190 "ep.c"
		  (*((qq) + (l))) += (1.0);
# 191 "ep.c"
		  (_p_sx) = ((_p_sx) + (t3));
# 192 "ep.c"
		  (_p_sy) = ((_p_sy) + (t4));
		}
	    }
# 195 "ep.c"
	  if ((0) == (1))
	    {
	      timer_stop (2);
	    }
	}
      _ompc_reduction (&_p_sy, _pp_sy, 14, 6);
      _ompc_reduction (&_p_sx, _pp_sx, 14, 6);
      _ompc_barrier ();
    }
    {
      _ompc_enter_critical (&__ompc_lock_critical);
# 199 "ep.c"
      for ((i) = (0); (i) <= ((10) - (1)); (i)++)
	{
	  (*((q) + (i))) += (*((qq) + (i)));
	}
      _ompc_exit_critical (&__ompc_lock_critical);
    }
    if (_ompc_is_master ())
      {
	(*_pp_nthreads) = (omp_get_num_threads ());
      }
  }
}
int
main (int argc, char **argv)
{
  //auto double *_ppthd_x;
  auto double Mops;
  auto double t1;
  auto double t2;
  auto double t3;
  auto double t4;
  auto double x1;
  auto double x2;
  auto double sx;
  auto double sy;
  auto double tm;
  auto double an;
  auto double tt;
  auto double gc;
  auto double dum[3];
  auto int np;
  auto int ierr;
  auto int node;
  auto int no_nodes;
  auto int i;
  auto int ik;
  auto int kk;
  auto int l;
  auto int k;
  auto int nit;
  auto int ierrcode;
  auto int no_large_nodes;
  auto int np_add;
  auto int k_offset;
  auto int j;
  auto int nthreads;
  auto int verified;
  auto char size[14];
 int status = 0;
  _ompc_init(argc,argv);

  //(_ppthd_x) = (((double *) (_ompc_get_thdprv (&_thdprv_x, 1048576, x))));
  (*(dum)) = (1.0);
  (*((dum) + (1))) = (1.0);
  (*((dum) + (2))) = (1.0);
  (nthreads) = (1);
# 84 "ep.c"
  printf
    ("\012\012 NAS Parallel Benchmarks 2.3 OpenMP C version - EP Benchmark\012");
# 86 "ep.c"
  sprintf (size, "%12.0f", pow (2.0, (28) + (1)));
# 87 "ep.c"
  for ((j) = (13); (j) >= (1); (j)--)
    {

# 88 "ep.c"
      if ((((int) (*((size) + (j))))) == (46))
	{
	  (*((size) + (j))) = (((char) (32)));
	}
    }
# 90 "ep.c"
  printf (" Number of random numbers generated: %13s\012", size);
# 92 "ep.c"
  (verified) = (0);
# 99 "ep.c"
  (np) = ((1) << ((28) - (16)));
# 107 "ep.c"
  vranlc (0, (dum) + (0), *((dum) + (1)), (dum) + (2));
# 108 "ep.c"
  (*((dum) + (0))) = (randlc ((dum) + (1), *((dum) + (2))));
# 109 "ep.c"
  for ((i) = (0); (i) < ((2) * ((1) << (16))); (i)++)
    {
      x[i] = (-(1.0E99));
      //(*((_ppthd_x) + (i))) = (-(1.0E99));
    }
# 110 "ep.c"
  (Mops) = (log (sqrt (fabs (((1.0) > (1.0)) ? (1.0) : (1.0)))));
# 112 "ep.c"
  timer_clear (1);
# 113 "ep.c"
  timer_clear (2);
# 114 "ep.c"
  timer_clear (3);
# 115 "ep.c"
  timer_start (1);
# 117 "ep.c"
  vranlc (0, &(t1), 1.220703125E9, x);
  //vranlc (0, &(t1), 1.220703125E9, _ppthd_x);
# 121 "ep.c"
  (t1) = (1.220703125E9);
# 123 "ep.c"
  for ((i) = (1); (i) <= ((16) + (1)); (i)++)
    {

# 124 "ep.c"
      (t2) = (randlc (&(t1), t1));
    }
# 127 "ep.c"
  (an) = (t1);
# 128 "ep.c"
  (tt) = (2.71828183E8);
# 129 "ep.c"
  (gc) = (0.0);
# 130 "ep.c"
  (sx) = (0.0);
# 131 "ep.c"
  (sy) = (0.0);
# 133 "ep.c"
  for ((i) = (0); (i) <= ((10) - (1)); (i)++)
    {

# 134 "ep.c"
      (*((q) + (i))) = (0.0);
    }
# 142 "ep.c"
  (k_offset) = (-(1));
  {
    auto void *__ompc_argv[6];
    (*(__ompc_argv)) = (((void *) (&sx)));
    (*((__ompc_argv) + (1))) = (((void *) (&sy)));
    (*((__ompc_argv) + (2))) = (((void *) (&np)));
    (*((__ompc_argv) + (3))) = (((void *) (&k_offset)));
    (*((__ompc_argv) + (4))) = (((void *) (&an)));
    (*((__ompc_argv) + (5))) = (((void *) (&nthreads)));
    _ompc_do_parallel (__ompc_func_3, __ompc_argv);
  }
# 207 "ep.c"
  for ((i) = (0); (i) <= ((10) - (1)); (i)++)
    {

# 208 "ep.c"
      (gc) = ((gc) + (*((q) + (i))));
    }
# 211 "ep.c"
  timer_stop (1);
# 212 "ep.c"
  (tm) = (timer_read (1));
# 214 "ep.c"
  (nit) = (0);
# 215 "ep.c"
  if ((28) == (24))
    {

# 216 "ep.c"
      if (((fabs (((sx) - (-(3247.83465203474))) / (sx))) <= (1.0E-8))
	  && ((fabs (((sy) - (-(6958.407078382297))) / (sy))) <= (1.0E-8)))
	{

# 218 "ep.c"
	  (verified) = (1);
	}
    }
  else
# 220 "ep.c"
  if ((28) == (25))
    {

# 221 "ep.c"
      if (((fabs (((sx) - (-(2863.319731645753))) / (sx))) <= (1.0E-8))
	  && ((fabs (((sy) - (-(6320.053679109499))) / (sy))) <= (1.0E-8)))
	{

# 223 "ep.c"
	  (verified) = (1);
	}
    }
  else
# 225 "ep.c"
  if ((28) == (28))
    {

# 226 "ep.c"
      if (((fabs (((sx) - (-(4295.875165629892))) / (sx))) <= (1.0E-8))
	  && ((fabs (((sy) - (-(15807.32573678431))) / (sy))) <= (1.0E-8)))
	{

# 228 "ep.c"
	  (verified) = (1);
          printf("Debug:ompc_manual. 359, sx is:%f, sy is:%f\n",sx,sy);
       }
     }

  else
# 230 "ep.c"
  if ((28) == (30))
    {

# 231 "ep.c"
      if (((fabs (((sx) - (40338.15542441498)) / (sx))) <= (1.0E-8))
	  && ((fabs (((sy) - (-(26606.69192809235))) / (sy))) <= (1.0E-8)))
	{

# 233 "ep.c"
	  (verified) = (1);
	}
    }
  else
# 235 "ep.c"
  if ((28) == (32))
    {

# 236 "ep.c"
      if (((fabs (((sx) - (47643.67927995374)) / (sx))) <= (1.0E-8))
	  && ((fabs (((sy) - (-(80840.72988043731))) / (sy))) <= (1.0E-8)))
	{

# 238 "ep.c"
	  (verified) = (1);
	}
    }
# 242 "ep.c"
  (Mops) = (((pow (2.0, (28) + (1))) / (tm)) / (1000000.0));
# 244 "ep.c"
  printf
    ("EP Benchmark Results: \012CPU Time = %10.4f\012N = 2^%5d\012No. Gaussian Pairs = %15.0f\012Sums = %25.15e %25.15e\012Counts:\012",
     tm, 28, gc, sx, sy);
# 251 "ep.c"
  for ((i) = (0); (i) <= ((10) - (1)); (i)++)
    {

# 252 "ep.c"
      printf ("%3d %15.0f\012", i, *((q) + (i)));
    }
# 255 "ep.c"
  c_print_results ("EP", 65, (28) + (1), 0, 0, nit, nthreads, tm, Mops,
		   "Random numbers generated", verified, "2.3", "07 Aug 2006",
		   "omcc", "$(CC)", "(none)", "-I../common", "-t", "-lm",
		   "randdp");
# 261 "ep.c"
  if ((0) == (1))
    {

# 262 "ep.c"
      printf ("Total time:     %f", timer_read (1));
# 263 "ep.c"
      printf ("Gaussian pairs: %f", timer_read (2));
# 264 "ep.c"
      printf ("Random numbers: %f", timer_read (3));
    }
}
