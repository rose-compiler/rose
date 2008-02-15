/*******************************************************
 * C file translated from WHIRL Sun Nov 20 21:04:40 2005
 *******************************************************/

/* Include file-level type and variable decls */
#include "cg.O0.w2c.h"


extern _INT32 main(
  _INT32 argc,
  _INT8 ** argv)
{
  register _IEEE64 __comma;
  register _INT32 __ompv_ok_to_fork;
  register _UINT32 reg7;
  register _IEEE64 __comma0;
  register _INT32 __ompv_ok_to_fork0;
  register _IEEE64 __comma1;
  register _INT32 __comma2;
  register _IEEE64 __comma3;
  register _INT32 reg8;
  _INT32 i;
  _INT32 j;
  _INT32 k;
  _INT32 it;
  _INT32 nthreads;
  _IEEE64 zeta;
  _IEEE64 rnorm;
  _IEEE64 norm_temp11;
  _IEEE64 norm_temp12;
  _IEEE64 t;
  _IEEE64 mflops;
  _INT8 class;
  _INT32 verified;
  _IEEE64 zeta_verify_value;
  _IEEE64 epsilon;
  _INT32 __ompv_gtid_s1;
  
  /*Begin_of_nested_PU(s)*/
static void __ompregion_main2(__ompv_gtid_a, __ompv_slink_a)
  _INT32 __ompv_gtid_a;
  _UINT32 __ompv_slink_a;
{
  register _INT32 mpsp_status;
  register _INT32 temp_limit;
  register _INT32 mpsp_status0;
  register _IEEE64 __comma;
  register _INT32 mp_is_master;
  register _INT32 temp_limit0;
  register _INT32 mp_is_master0;
  register _INT32 __comma0;
  _UINT32 __slink_sym_temp_1;
  _INT32 __ompv_temp_gtid;
  _INT32 __mplocal_it;
  _INT32 __mplocal_j;
  _IEEE64 __mplocal_norm_temp11;
  _IEEE64 __mplocal_norm_temp12;
  _INT32 __ompv_temp_do_upper;
  _INT32 __ompv_temp_do_lower;
  _INT32 __ompv_temp_do_stride;
  _INT32 __ompv_temp_last_iter;
  _INT32 __ompv_temp_do_upper0;
  _INT32 __ompv_temp_do_lower0;
  _INT32 __ompv_temp_do_stride0;
  _INT32 __ompv_temp_last_iter0;
  
  __slink_sym_temp_1 = __ompv_slink_a;
  __ompv_temp_gtid = __ompv_gtid_a;
  __mplocal_it = 1;
  while(__mplocal_it <= 15)
  {
    conj_grad((_INT32 *) colidx, (_INT32 *) rowstr, (_IEEE64 *) x, (_IEEE64 *) z, (_IEEE64 *) a, (_IEEE64 *) p, (_IEEE64 *) q, (_IEEE64 *) r, (_IEEE64 *) w, &rnorm);
    mpsp_status = __ompc_single(__ompv_temp_gtid);
    if(mpsp_status == 1)
    {
      norm_temp11 = 0.0;
      norm_temp12 = 0.0;
    }
    __ompc_end_single(__ompv_temp_gtid);
    __ompc_barrier();
    __mplocal_norm_temp11 = 0.0;
    __mplocal_norm_temp12 = 0.0;
    temp_limit = (lastcol - firstcol) + 1;
    __ompv_temp_do_upper = temp_limit;
    __ompv_temp_do_lower = 1;
    __ompv_temp_last_iter = 0;
    __ompc_static_init_4(__ompv_temp_gtid, 2, &__ompv_temp_do_lower, &__ompv_temp_do_upper, &__ompv_temp_do_stride, 1, 1);
    if(__ompv_temp_do_upper > temp_limit)
    {
      __ompv_temp_do_upper = temp_limit;
    }
    for(__mplocal_j = __ompv_temp_do_lower; __mplocal_j <= __ompv_temp_do_upper; __mplocal_j = __mplocal_j + 1)
    {
      __mplocal_norm_temp11 = __mplocal_norm_temp11 + (x[__mplocal_j] * z[__mplocal_j]);
      __mplocal_norm_temp12 = __mplocal_norm_temp12 + (z[__mplocal_j] * z[__mplocal_j]);
      _1 :;
    }
    __ompc_barrier();
    __ompc_critical(__ompv_temp_gtid, (_UINT32) & __mplock_2);
    norm_temp11 = __mplocal_norm_temp11 + norm_temp11;
    norm_temp12 = __mplocal_norm_temp12 + norm_temp12;
    __ompc_end_critical(__ompv_temp_gtid, (_UINT32) & __mplock_2);
    __ompc_barrier();
    mpsp_status0 = __ompc_single(__ompv_temp_gtid);
    if(mpsp_status0 == 1)
    {
      __comma = sqrt(norm_temp12);
      norm_temp12 = _F8RECIP(__comma);
      zeta = _F8RECIP(norm_temp11) + 2.0e+01;
    }
    __ompc_end_single(__ompv_temp_gtid);
    __ompc_barrier();
    mp_is_master = __ompc_master(__ompv_temp_gtid);
    if(mp_is_master == 1)
    {
      if(__mplocal_it == 1)
      {
        printf((_INT8 *) "   iteration           ||r||                 zeta\n");
      }
      printf((_INT8 *) "    %5d       %20.14e%20.13e\n", __mplocal_it, rnorm, zeta);
    }
    temp_limit0 = (lastcol - firstcol) + 1;
    __ompv_temp_do_upper0 = temp_limit0;
    __ompv_temp_do_lower0 = 1;
    __ompv_temp_last_iter0 = 0;
    __ompc_static_init_4(__ompv_temp_gtid, 2, &__ompv_temp_do_lower0, &__ompv_temp_do_upper0, &__ompv_temp_do_stride0, 1, 1);
    if(__ompv_temp_do_upper0 > temp_limit0)
    {
      __ompv_temp_do_upper0 = temp_limit0;
    }
    for(__mplocal_j = __ompv_temp_do_lower0; __mplocal_j <= __ompv_temp_do_upper0; __mplocal_j = __mplocal_j + 1)
    {
      x[__mplocal_j] = z[__mplocal_j] * norm_temp12;
      _2 :;
    }
    __ompc_barrier();
    _3 :;
    __mplocal_it = __mplocal_it + 1;
  }
  mp_is_master0 = __ompc_master(__ompv_temp_gtid);
  if(mp_is_master0 == 1)
  {
    __comma0 = omp_get_num_threads();
    nthreads = __comma0;
  }
  return;
} /* __ompregion_main2 */
static void __ompregion_main1(__ompv_gtid_a, __ompv_slink_a)
  _INT32 __ompv_gtid_a;
  _UINT32 __ompv_slink_a;
{
  register _INT32 temp_limit;
  register _INT32 temp_limit0;
  register _INT32 mpsp_status;
  register _INT32 mpsp_status0;
  register _INT32 temp_limit1;
  register _INT32 mpsp_status1;
  register _IEEE64 __comma;
  register _INT32 temp_limit2;
  register _INT32 temp_limit3;
  register _INT32 mpsp_status2;
  _UINT32 __slink_sym_temp_0;
  _INT32 __ompv_temp_gtid;
  _INT32 __mplocal_it;
  _INT32 __mplocal_i;
  _INT32 __mplocal_j;
  _INT32 __mplocal_k;
  _INT32 __ompv_temp_do_upper;
  _INT32 __ompv_temp_do_lower;
  _INT32 __ompv_temp_do_stride;
  _INT32 __ompv_temp_last_iter;
  _INT32 __ompv_temp_do_upper0;
  _INT32 __ompv_temp_do_lower0;
  _INT32 __ompv_temp_do_stride0;
  _INT32 __ompv_temp_last_iter0;
  _IEEE64 __mplocal_norm_temp11;
  _IEEE64 __mplocal_norm_temp12;
  _INT32 __ompv_temp_do_upper1;
  _INT32 __ompv_temp_do_lower1;
  _INT32 __ompv_temp_do_stride1;
  _INT32 __ompv_temp_last_iter1;
  _INT32 __ompv_temp_do_upper2;
  _INT32 __ompv_temp_do_lower2;
  _INT32 __ompv_temp_do_stride2;
  _INT32 __ompv_temp_last_iter2;
  _INT32 __ompv_temp_do_upper3;
  _INT32 __ompv_temp_do_lower3;
  _INT32 __ompv_temp_do_stride3;
  _INT32 __ompv_temp_last_iter3;
  
  __slink_sym_temp_0 = __ompv_slink_a;
  __ompv_temp_gtid = __ompv_gtid_a;
  temp_limit = (lastrow - firstrow) + 1;
  __ompv_temp_do_upper = temp_limit;
  __ompv_temp_do_lower = 1;
  __ompv_temp_last_iter = 0;
  __ompc_static_init_4(__ompv_temp_gtid, 2, &__ompv_temp_do_lower, &__ompv_temp_do_upper, &__ompv_temp_do_stride, 1, 1);
  if(__ompv_temp_do_upper > temp_limit)
  {
    __ompv_temp_do_upper = temp_limit;
  }
  for(__mplocal_j = __ompv_temp_do_lower; __mplocal_j <= __ompv_temp_do_upper; __mplocal_j = __mplocal_j + 1)
  {
    __mplocal_k = rowstr[__mplocal_j];
    while(rowstr[__mplocal_j + 1] > __mplocal_k)
    {
      colidx[__mplocal_k] = (colidx[__mplocal_k] - firstcol) + 1;
      _1 :;
      __mplocal_k = __mplocal_k + 1;
    }
    _2 :;
  }
  temp_limit0 = 14001;
  __ompv_temp_do_upper0 = temp_limit0;
  __ompv_temp_do_lower0 = 1;
  __ompv_temp_last_iter0 = 0;
  __ompc_static_init_4(__ompv_temp_gtid, 2, &__ompv_temp_do_lower0, &__ompv_temp_do_upper0, &__ompv_temp_do_stride0, 1, 1);
  if(__ompv_temp_do_upper0 > temp_limit0)
  {
    __ompv_temp_do_upper0 = temp_limit0;
  }
  for(__mplocal_i = __ompv_temp_do_lower0; __mplocal_i <= __ompv_temp_do_upper0; __mplocal_i = __mplocal_i + 1)
  {
    x[__mplocal_i] = 1.0;
    _3 :;
  }
  mpsp_status = __ompc_single(__ompv_temp_gtid);
  if(mpsp_status == 1)
  {
    zeta = 0.0;
  }
  __ompc_end_single(__ompv_temp_gtid);
  __ompc_barrier();
  __mplocal_it = 1;
  while(__mplocal_it <= 1)
  {
    conj_grad((_INT32 *) colidx, (_INT32 *) rowstr, (_IEEE64 *) x, (_IEEE64 *) z, (_IEEE64 *) a, (_IEEE64 *) p, (_IEEE64 *) q, (_IEEE64 *) r, (_IEEE64 *) w, &rnorm);
    mpsp_status0 = __ompc_single(__ompv_temp_gtid);
    if(mpsp_status0 == 1)
    {
      norm_temp11 = 0.0;
      norm_temp12 = 0.0;
    }
    __ompc_end_single(__ompv_temp_gtid);
    __ompc_barrier();
    __mplocal_norm_temp11 = 0.0;
    __mplocal_norm_temp12 = 0.0;
    temp_limit1 = (lastcol - firstcol) + 1;
    __ompv_temp_do_upper1 = temp_limit1;
    __ompv_temp_do_lower1 = 1;
    __ompv_temp_last_iter1 = 0;
    __ompc_static_init_4(__ompv_temp_gtid, 2, &__ompv_temp_do_lower1, &__ompv_temp_do_upper1, &__ompv_temp_do_stride1, 1, 1);
    if(__ompv_temp_do_upper1 > temp_limit1)
    {
      __ompv_temp_do_upper1 = temp_limit1;
    }
    for(__mplocal_j = __ompv_temp_do_lower1; __mplocal_j <= __ompv_temp_do_upper1; __mplocal_j = __mplocal_j + 1)
    {
      __mplocal_norm_temp11 = __mplocal_norm_temp11 + (x[__mplocal_j] * z[__mplocal_j]);
      __mplocal_norm_temp12 = __mplocal_norm_temp12 + (z[__mplocal_j] * z[__mplocal_j]);
      _4 :;
    }
    __ompc_barrier();
    __ompc_critical(__ompv_temp_gtid, (_UINT32) & __mplock_1);
    norm_temp11 = norm_temp11 + __mplocal_norm_temp11;
    norm_temp12 = norm_temp12 + __mplocal_norm_temp12;
    __ompc_end_critical(__ompv_temp_gtid, (_UINT32) & __mplock_1);
    __ompc_barrier();
    mpsp_status1 = __ompc_single(__ompv_temp_gtid);
    if(mpsp_status1 == 1)
    {
      __comma = sqrt(norm_temp12);
      norm_temp12 = _F8RECIP(__comma);
    }
    __ompc_end_single(__ompv_temp_gtid);
    __ompc_barrier();
    temp_limit2 = (lastcol - firstcol) + 1;
    __ompv_temp_do_upper2 = temp_limit2;
    __ompv_temp_do_lower2 = 1;
    __ompv_temp_last_iter2 = 0;
    __ompc_static_init_4(__ompv_temp_gtid, 2, &__ompv_temp_do_lower2, &__ompv_temp_do_upper2, &__ompv_temp_do_stride2, 1, 1);
    if(__ompv_temp_do_upper2 > temp_limit2)
    {
      __ompv_temp_do_upper2 = temp_limit2;
    }
    for(__mplocal_j = __ompv_temp_do_lower2; __mplocal_j <= __ompv_temp_do_upper2; __mplocal_j = __mplocal_j + 1)
    {
      x[__mplocal_j] = z[__mplocal_j] * norm_temp12;
      _5 :;
    }
    __ompc_barrier();
    _6 :;
    __mplocal_it = __mplocal_it + 1;
  }
  temp_limit3 = 14001;
  __ompv_temp_do_upper3 = temp_limit3;
  __ompv_temp_do_lower3 = 1;
  __ompv_temp_last_iter3 = 0;
  __ompc_static_init_4(__ompv_temp_gtid, 2, &__ompv_temp_do_lower3, &__ompv_temp_do_upper3, &__ompv_temp_do_stride3, 1, 1);
  if(__ompv_temp_do_upper3 > temp_limit3)
  {
    __ompv_temp_do_upper3 = temp_limit3;
  }
  for(__mplocal_i = __ompv_temp_do_lower3; __mplocal_i <= __ompv_temp_do_upper3; __mplocal_i = __mplocal_i + 1)
  {
    x[__mplocal_i] = 1.0;
    _7 :;
  }
  mpsp_status2 = __ompc_single(__ompv_temp_gtid);
  if(mpsp_status2 == 1)
  {
    zeta = 0.0;
  }
  __ompc_end_single(__ompv_temp_gtid);
  __ompc_barrier();
  return;
} /* __ompregion_main1 */
  
  nthreads = 1;
  firstrow = 1;
  lastrow = 14000;
  firstcol = 1;
  lastcol = 14000;
  if(0)
  {
    class = 83;
    zeta_verify_value = 8.5971775078647994;
  }
  else
  {
    if(0)
    {
      class = 87;
      zeta_verify_value = 1.0362595087123999e+01;
    }
    else
    {
      if(1)
      {
        class = 65;
        zeta_verify_value = 1.7130235054029001e+01;
      }
      else
      {
        if(0)
        {
          class = 66;
          zeta_verify_value = 2.2712745482631e+01;
        }
        else
        {
          if(0)
          {
            class = 67;
            zeta_verify_value = 2.8973605592845001e+01;
          }
          else
          {
            class = 85;
          }
        }
      }
    }
  }
  printf((_INT8 *) "\n\n NAS Parallel Benchmarks 2.3 OpenMP C version - CG Benchmark\n");
  printf((_INT8 *) " Size: %10d\n", 14000);
  printf((_INT8 *) " Iterations: %5d\n", 15);
  naa = 14000;
  nzz = 2198000;
  tran = 3.14159265e+08;
  amult = 1.220703125e+09;
  __comma = randlc(&tran, amult);
  zeta = __comma;
  makea(naa, nzz, (_IEEE64 *) a, (_INT32 *) colidx, (_INT32 *) rowstr, 11, firstrow, lastrow, firstcol, lastcol, 1.0000000000000001e-01, (_INT32 *) arow, (_INT32 *) acol, (_IEEE64 *) aelt, (_IEEE64 *) v, (_INT32 *) iv, 2.0e+01);
  __ompv_ok_to_fork = __ompc_can_fork();
  if(__ompv_ok_to_fork == 1)
  {
    __ompc_fork(0, &__ompregion_main1, reg7);
  }
  else
  {
    __ompv_gtid_s1 = __ompc_get_local_thread_num();
    __ompc_serialized_parallel();
    for(j = 1; j <= ((lastrow - firstrow) + 1); j = j + 1)
    {
      k = rowstr[j];
      while(rowstr[j + 1] > k)
      {
        colidx[k] = (colidx[k] - firstcol) + 1;
        _2 :;
        k = k + 1;
      }
      _1 :;
    }
    for(i = 1; i <= 14001; i = i + 1)
    {
      x[i] = 1.0;
      _3 :;
    }
    zeta = 0.0;
    it = 1;
    while(it <= 1)
    {
      conj_grad((_INT32 *) colidx, (_INT32 *) rowstr, (_IEEE64 *) x, (_IEEE64 *) z, (_IEEE64 *) a, (_IEEE64 *) p, (_IEEE64 *) q, (_IEEE64 *) r, (_IEEE64 *) w, &rnorm);
      norm_temp11 = 0.0;
      norm_temp12 = 0.0;
      for(j = 1; j <= ((lastcol - firstcol) + 1); j = j + 1)
      {
        norm_temp11 = norm_temp11 + (x[j] * z[j]);
        norm_temp12 = norm_temp12 + (z[j] * z[j]);
        _5 :;
      }
      __comma0 = sqrt(norm_temp12);
      norm_temp12 = _F8RECIP(__comma0);
      for(j = 1; j <= ((lastcol - firstcol) + 1); j = j + 1)
      {
        x[j] = z[j] * norm_temp12;
        _6 :;
      }
      _4 :;
      it = it + 1;
    }
    for(i = 1; i <= 14001; i = i + 1)
    {
      x[i] = 1.0;
      _7 :;
    }
    zeta = 0.0;
    __ompc_end_serialized_parallel();
  }
  timer_clear(1);
  timer_start(1);
  __ompv_ok_to_fork0 = __ompc_can_fork();
  if(__ompv_ok_to_fork0 == 1)
  {
    __ompc_fork(0, &__ompregion_main2, reg7);
  }
  else
  {
    __ompv_gtid_s1 = __ompc_get_local_thread_num();
    __ompc_serialized_parallel();
    it = 1;
    while(it <= 15)
    {
      conj_grad((_INT32 *) colidx, (_INT32 *) rowstr, (_IEEE64 *) x, (_IEEE64 *) z, (_IEEE64 *) a, (_IEEE64 *) p, (_IEEE64 *) q, (_IEEE64 *) r, (_IEEE64 *) w, &rnorm);
      norm_temp11 = 0.0;
      norm_temp12 = 0.0;
      for(j = 1; j <= ((lastcol - firstcol) + 1); j = j + 1)
      {
        norm_temp11 = norm_temp11 + (x[j] * z[j]);
        norm_temp12 = norm_temp12 + (z[j] * z[j]);
        _9 :;
      }
      __comma1 = sqrt(norm_temp12);
      norm_temp12 = _F8RECIP(__comma1);
      zeta = _F8RECIP(norm_temp11) + 2.0e+01;
      if(it == 1)
      {
        printf((_INT8 *) "   iteration           ||r||                 zeta\n");
      }
      printf((_INT8 *) "    %5d       %20.14e%20.13e\n", it, rnorm, zeta);
      for(j = 1; j <= ((lastcol - firstcol) + 1); j = j + 1)
      {
        x[j] = z[j] * norm_temp12;
        _10 :;
      }
      _8 :;
      it = it + 1;
    }
    __comma2 = omp_get_num_threads();
    nthreads = __comma2;
    __ompc_end_serialized_parallel();
  }
  timer_stop(1);
  __comma3 = timer_read(1);
  t = __comma3;
  printf((_INT8 *) " Benchmark completed\n");
  epsilon = 1.0e-10;
  if((_INT32)(class) != 85)
  {
    if(epsilon >= _F8ABS(zeta - zeta_verify_value))
    {
      verified = 1;
      printf((_INT8 *) " VERIFICATION SUCCESSFUL\n");
      printf((_INT8 *) " Zeta is    %20.12e\n", zeta);
      printf((_INT8 *) " Error is   %20.12e\n", zeta - zeta_verify_value);
    }
    else
    {
      verified = 0;
      printf((_INT8 *) " VERIFICATION FAILED\n");
      printf((_INT8 *) " Zeta                %20.12e\n", zeta);
      printf((_INT8 *) " The correct zeta is %20.12e\n", zeta_verify_value);
    }
  }
  else
  {
    verified = 0;
    printf((_INT8 *) " Problem size unknown\n");
    printf((_INT8 *) " NO VERIFICATION PERFORMED\n");
  }
  if(t != 0.0)
  {
    mflops = (1.49646e+09 / t) / 1.0e+06;
  }
  else
  {
    mflops = 0.0;
  }
  c_print_results((_INT8 *) "CG", (_INT8)(_INT32) class, 14000, 0, 0, 15, nthreads, t, mflops, (_INT8 *) "          floating point", verified, (_INT8 *) "2.3", (_INT8 *) "18 Nov 2005", (_INT8 *) "gcc", (_INT8 *) "gcc", (_INT8 *) "(none)", (_INT8 *) "-I../common -I $(HOME)/orc2.1.uh/src/osprey...", (_INT8 *) "-O2", (_INT8 *) "-O2  -L $(HOME)/orc2.1.uh/src/osprey1.0/tar...", (_INT8 *) "randdp");
  return reg8;
} /* main */






static void conj_grad(
  _INT32 * colidx0,
  _INT32 * rowstr0,
  _IEEE64 * x0,
  _IEEE64 * z0,
  _IEEE64 * a0,
  _IEEE64 * p0,
  _IEEE64 * q0,
  _IEEE64 * r0,
  _IEEE64 * w0,
  _IEEE64 * rnorm)
{
  register _INT32 mpsp_status;
  register _INT32 temp_limit;
  register _INT32 temp_limit0;
  register _INT32 mpsp_status0;
  register _INT32 temp_limit1;
  register _INT32 temp_limit2;
  register _INT32 temp_limit3;
  register _INT32 temp_limit4;
  register _INT32 mpsp_status1;
  register _INT32 temp_limit5;
  register _INT32 temp_limit6;
  register _INT32 mpsp_status2;
  register _INT32 temp_limit7;
  register _INT32 mpsp_status3;
  register _INT32 temp_limit8;
  register _INT32 temp_limit9;
  register _INT32 temp_limit10;
  register _INT32 mpsp_status4;
  register _IEEE64 __comma;
  static _IEEE64 d;
  static _IEEE64 sum;
  static _IEEE64 rho;
  static _IEEE64 rho0;
  static _IEEE64 alpha;
  static _IEEE64 beta;
  _INT32 cgit;
  _INT32 cgitmax;
  _INT32 __ompv_gtid_s1;
  _INT32 __mplocal_j;
  _INT32 __ompv_temp_do_upper;
  _INT32 __ompv_temp_do_lower;
  _INT32 __ompv_temp_do_stride;
  _INT32 __ompv_temp_last_iter;
  _IEEE64 __mplocal_rho;
  _INT32 __mplocal_j0;
  _INT32 __ompv_temp_do_upper0;
  _INT32 __ompv_temp_do_lower0;
  _INT32 __ompv_temp_do_stride0;
  _INT32 __ompv_temp_last_iter0;
  _INT32 __mplocal_j1;
  _IEEE64 __mplocal_sum;
  _INT32 __mplocal_k;
  _INT32 __ompv_temp_do_upper1;
  _INT32 __ompv_temp_do_lower1;
  _INT32 __ompv_temp_do_stride1;
  _INT32 __ompv_temp_last_iter1;
  _INT32 __mplocal_j2;
  _INT32 __ompv_temp_do_upper2;
  _INT32 __ompv_temp_do_lower2;
  _INT32 __ompv_temp_do_stride2;
  _INT32 __ompv_temp_last_iter2;
  _INT32 __mplocal_j3;
  _INT32 __ompv_temp_do_upper3;
  _INT32 __ompv_temp_do_lower3;
  _INT32 __ompv_temp_do_stride3;
  _INT32 __ompv_temp_last_iter3;
  _IEEE64 __mplocal_d;
  _INT32 __mplocal_j4;
  _INT32 __ompv_temp_do_upper4;
  _INT32 __ompv_temp_do_lower4;
  _INT32 __ompv_temp_do_stride4;
  _INT32 __ompv_temp_last_iter4;
  _INT32 __mplocal_j5;
  _INT32 __ompv_temp_do_upper5;
  _INT32 __ompv_temp_do_lower5;
  _INT32 __ompv_temp_do_stride5;
  _INT32 __ompv_temp_last_iter5;
  _IEEE64 __mplocal_rho0;
  _INT32 __mplocal_j6;
  _INT32 __ompv_temp_do_upper6;
  _INT32 __ompv_temp_do_lower6;
  _INT32 __ompv_temp_do_stride6;
  _INT32 __ompv_temp_last_iter6;
  _INT32 __mplocal_j7;
  _INT32 __ompv_temp_do_upper7;
  _INT32 __ompv_temp_do_lower7;
  _INT32 __ompv_temp_do_stride7;
  _INT32 __ompv_temp_last_iter7;
  _INT32 __mplocal_j8;
  _IEEE64 __mplocal_d0;
  _INT32 __mplocal_k0;
  _INT32 __ompv_temp_do_upper8;
  _INT32 __ompv_temp_do_lower8;
  _INT32 __ompv_temp_do_stride8;
  _INT32 __ompv_temp_last_iter8;
  _INT32 __mplocal_j9;
  _INT32 __ompv_temp_do_upper9;
  _INT32 __ompv_temp_do_lower9;
  _INT32 __ompv_temp_do_stride9;
  _INT32 __ompv_temp_last_iter9;
  _IEEE64 __mplocal_sum0;
  _INT32 __mplocal_j10;
  _IEEE64 __mplocal_d1;
  _INT32 __ompv_temp_do_upper10;
  _INT32 __ompv_temp_do_lower10;
  _INT32 __ompv_temp_do_stride10;
  _INT32 __ompv_temp_last_iter10;
  
  /*Begin_of_nested_PU(s)*/
  
  cgitmax = 25;
  __ompv_gtid_s1 = __ompc_get_local_thread_num();
  mpsp_status = __ompc_single(__ompv_gtid_s1);
  if(mpsp_status == 1)
  {
    rho = 0.0;
  }
  __ompc_end_single(__ompv_gtid_s1);
  __ompv_gtid_s1 = __ompc_get_local_thread_num();
  temp_limit = naa + 1;
  __ompv_temp_do_upper = temp_limit;
  __ompv_temp_do_lower = 1;
  __ompv_temp_last_iter = 0;
  __ompc_static_init_4(__ompv_gtid_s1, 2, &__ompv_temp_do_lower, &__ompv_temp_do_upper, &__ompv_temp_do_stride, 1, 1);
  if(__ompv_temp_do_upper > temp_limit)
  {
    __ompv_temp_do_upper = temp_limit;
  }
  for(__mplocal_j = __ompv_temp_do_lower; __mplocal_j <= __ompv_temp_do_upper; __mplocal_j = __mplocal_j + 1)
  {
    * (_IEEE64 *)((_UINT8 *)(q0) + (_UINT32)(((_UINT64)((_UINT64) __mplocal_j) * 8ULL))) = 0.0;
    * (_IEEE64 *)((_UINT8 *)(z0) + (_UINT32)(((_UINT64)((_UINT64) __mplocal_j) * 8ULL))) = 0.0;
    * (_IEEE64 *)((_UINT8 *)(r0) + (_UINT32)(((_UINT64)((_UINT64) __mplocal_j) * 8ULL))) = *(_IEEE64 *)((_UINT8 *)(x0) + (_UINT32)(((_UINT64)((_UINT64) __mplocal_j) * 8ULL)));
    * (_IEEE64 *)((_UINT8 *)(p0) + (_UINT32)(((_UINT64)((_UINT64) __mplocal_j) * 8ULL))) = *(_IEEE64 *)((_UINT8 *)(r0) + (_UINT32)(((_UINT64)((_UINT64) __mplocal_j) * 8ULL)));
    * (_IEEE64 *)((_UINT8 *)(w0) + (_UINT32)(((_UINT64)((_UINT64) __mplocal_j) * 8ULL))) = 0.0;
    _1 :;
  }
  __ompv_gtid_s1 = __ompc_get_local_thread_num();
  __mplocal_rho = 0.0;
  temp_limit0 = (lastcol - firstcol) + 1;
  __ompv_temp_do_upper0 = temp_limit0;
  __ompv_temp_do_lower0 = 1;
  __ompv_temp_last_iter0 = 0;
  __ompc_static_init_4(__ompv_gtid_s1, 2, &__ompv_temp_do_lower0, &__ompv_temp_do_upper0, &__ompv_temp_do_stride0, 1, 1);
  if(__ompv_temp_do_upper0 > temp_limit0)
  {
    __ompv_temp_do_upper0 = temp_limit0;
  }
  for(__mplocal_j0 = __ompv_temp_do_lower0; __mplocal_j0 <= __ompv_temp_do_upper0; __mplocal_j0 = __mplocal_j0 + 1)
  {
    __mplocal_rho = __mplocal_rho + (*(_IEEE64 *)((_UINT8 *)(x0) + (_UINT32)(((_UINT64)((_UINT64) __mplocal_j0) * 8ULL))) ** (_IEEE64 *)((_UINT8 *)(x0) + (_UINT32)(((_UINT64)((_UINT64) __mplocal_j0) * 8ULL))));
    _2 :;
  }
  __ompc_barrier();
  __ompc_critical(__ompv_gtid_s1, (_UINT32) & __mplock_1);
  rho = rho + __mplocal_rho;
  __ompc_end_critical(__ompv_gtid_s1, (_UINT32) & __mplock_1);
  __ompc_barrier();
  cgit = 1;
  while(cgit <= cgitmax)
  {
    __ompv_gtid_s1 = __ompc_get_local_thread_num();
    mpsp_status0 = __ompc_single(__ompv_gtid_s1);
    if(mpsp_status0 == 1)
    {
      rho0 = rho;
      d = 0.0;
      rho = 0.0;
    }
    __ompc_end_single(__ompv_gtid_s1);
    __ompv_gtid_s1 = __ompc_get_local_thread_num();
    temp_limit1 = (lastrow - firstrow) + 1;
    __ompv_temp_do_upper1 = temp_limit1;
    __ompv_temp_do_lower1 = 1;
    __ompv_temp_last_iter1 = 0;
    __ompc_static_init_4(__ompv_gtid_s1, 2, &__ompv_temp_do_lower1, &__ompv_temp_do_upper1, &__ompv_temp_do_stride1, 1, 1);
    if(__ompv_temp_do_upper1 > temp_limit1)
    {
      __ompv_temp_do_upper1 = temp_limit1;
    }
    for(__mplocal_j1 = __ompv_temp_do_lower1; __mplocal_j1 <= __ompv_temp_do_upper1; __mplocal_j1 = __mplocal_j1 + 1)
    {
      __mplocal_sum = 0.0;
      __mplocal_k = *(_INT32 *)((_UINT8 *)(rowstr0) + (_UINT32)(((_UINT64)((_UINT64) __mplocal_j1) * 4ULL)));
      while(*((_INT32 *)((_UINT8 *)(rowstr0) + (_UINT32)(((_UINT64)((_UINT64) __mplocal_j1) * 4ULL))) + 1LL) > __mplocal_k)
      {
        __mplocal_sum = __mplocal_sum + (*(_IEEE64 *)((_UINT8 *)(a0) + (_UINT32)(((_UINT64)((_UINT64) __mplocal_k) * 8ULL))) ** (_IEEE64 *)((_UINT8 *)(p0) + (_UINT32)(((_UINT64)(*(_INT32 *)((_UINT8 *)(colidx0) + (_UINT32)(((_UINT64)((_UINT64) __mplocal_k) * 4ULL)))) * 8ULL))));
        _5 :;
        __mplocal_k = __mplocal_k + 1;
      }
      * (_IEEE64 *)((_UINT8 *)(w0) + (_UINT32)(((_UINT64)((_UINT64) __mplocal_j1) * 8ULL))) = __mplocal_sum;
      _4 :;
    }
    __ompc_barrier();
    __ompv_gtid_s1 = __ompc_get_local_thread_num();
    temp_limit2 = (lastcol - firstcol) + 1;
    __ompv_temp_do_upper2 = temp_limit2;
    __ompv_temp_do_lower2 = 1;
    __ompv_temp_last_iter2 = 0;
    __ompc_static_init_4(__ompv_gtid_s1, 2, &__ompv_temp_do_lower2, &__ompv_temp_do_upper2, &__ompv_temp_do_stride2, 1, 1);
    if(__ompv_temp_do_upper2 > temp_limit2)
    {
      __ompv_temp_do_upper2 = temp_limit2;
    }
    for(__mplocal_j2 = __ompv_temp_do_lower2; __mplocal_j2 <= __ompv_temp_do_upper2; __mplocal_j2 = __mplocal_j2 + 1)
    {
      * (_IEEE64 *)((_UINT8 *)(q0) + (_UINT32)(((_UINT64)((_UINT64) __mplocal_j2) * 8ULL))) = *(_IEEE64 *)((_UINT8 *)(w0) + (_UINT32)(((_UINT64)((_UINT64) __mplocal_j2) * 8ULL)));
      _6 :;
    }
    __ompc_barrier();
    __ompv_gtid_s1 = __ompc_get_local_thread_num();
    temp_limit3 = (lastcol - firstcol) + 1;
    __ompv_temp_do_upper3 = temp_limit3;
    __ompv_temp_do_lower3 = 1;
    __ompv_temp_last_iter3 = 0;
    __ompc_static_init_4(__ompv_gtid_s1, 2, &__ompv_temp_do_lower3, &__ompv_temp_do_upper3, &__ompv_temp_do_stride3, 1, 1);
    if(__ompv_temp_do_upper3 > temp_limit3)
    {
      __ompv_temp_do_upper3 = temp_limit3;
    }
    for(__mplocal_j3 = __ompv_temp_do_lower3; __mplocal_j3 <= __ompv_temp_do_upper3; __mplocal_j3 = __mplocal_j3 + 1)
    {
      * (_IEEE64 *)((_UINT8 *)(w0) + (_UINT32)(((_UINT64)((_UINT64) __mplocal_j3) * 8ULL))) = 0.0;
      _7 :;
    }
    __ompv_gtid_s1 = __ompc_get_local_thread_num();
    __mplocal_d = 0.0;
    temp_limit4 = (lastcol - firstcol) + 1;
    __ompv_temp_do_upper4 = temp_limit4;
    __ompv_temp_do_lower4 = 1;
    __ompv_temp_last_iter4 = 0;
    __ompc_static_init_4(__ompv_gtid_s1, 2, &__ompv_temp_do_lower4, &__ompv_temp_do_upper4, &__ompv_temp_do_stride4, 1, 1);
    if(__ompv_temp_do_upper4 > temp_limit4)
    {
      __ompv_temp_do_upper4 = temp_limit4;
    }
    for(__mplocal_j4 = __ompv_temp_do_lower4; __mplocal_j4 <= __ompv_temp_do_upper4; __mplocal_j4 = __mplocal_j4 + 1)
    {
      __mplocal_d = __mplocal_d + (*(_IEEE64 *)((_UINT8 *)(p0) + (_UINT32)(((_UINT64)((_UINT64) __mplocal_j4) * 8ULL))) ** (_IEEE64 *)((_UINT8 *)(q0) + (_UINT32)(((_UINT64)((_UINT64) __mplocal_j4) * 8ULL))));
      _8 :;
    }
    __ompc_barrier();
    __ompc_critical(__ompv_gtid_s1, (_UINT32) & __mplock_2);
    d = d + __mplocal_d;
    __ompc_end_critical(__ompv_gtid_s1, (_UINT32) & __mplock_2);
    __ompc_barrier();
    __ompv_gtid_s1 = __ompc_get_local_thread_num();
    mpsp_status1 = __ompc_single(__ompv_gtid_s1);
    if(mpsp_status1 == 1)
    {
      alpha = rho0 / d;
    }
    __ompc_end_single(__ompv_gtid_s1);
    __ompc_barrier();
    __ompv_gtid_s1 = __ompc_get_local_thread_num();
    temp_limit5 = (lastcol - firstcol) + 1;
    __ompv_temp_do_upper5 = temp_limit5;
    __ompv_temp_do_lower5 = 1;
    __ompv_temp_last_iter5 = 0;
    __ompc_static_init_4(__ompv_gtid_s1, 2, &__ompv_temp_do_lower5, &__ompv_temp_do_upper5, &__ompv_temp_do_stride5, 1, 1);
    if(__ompv_temp_do_upper5 > temp_limit5)
    {
      __ompv_temp_do_upper5 = temp_limit5;
    }
    for(__mplocal_j5 = __ompv_temp_do_lower5; __mplocal_j5 <= __ompv_temp_do_upper5; __mplocal_j5 = __mplocal_j5 + 1)
    {
      * (_IEEE64 *)((_UINT8 *)(z0) + (_UINT32)(((_UINT64)((_UINT64) __mplocal_j5) * 8ULL))) = *(_IEEE64 *)((_UINT8 *)(z0) + (_UINT32)(((_UINT64)((_UINT64) __mplocal_j5) * 8ULL))) + (*(_IEEE64 *)((_UINT8 *)(p0) + (_UINT32)(((_UINT64)((_UINT64) __mplocal_j5) * 8ULL))) * alpha);
      * (_IEEE64 *)((_UINT8 *)(r0) + (_UINT32)(((_UINT64)((_UINT64) __mplocal_j5) * 8ULL))) = *(_IEEE64 *)((_UINT8 *)(r0) + (_UINT32)(((_UINT64)((_UINT64) __mplocal_j5) * 8ULL))) - (*(_IEEE64 *)((_UINT8 *)(q0) + (_UINT32)(((_UINT64)((_UINT64) __mplocal_j5) * 8ULL))) * alpha);
      _9 :;
    }
    __ompc_barrier();
    __ompv_gtid_s1 = __ompc_get_local_thread_num();
    __mplocal_rho0 = 0.0;
    temp_limit6 = (lastcol - firstcol) + 1;
    __ompv_temp_do_upper6 = temp_limit6;
    __ompv_temp_do_lower6 = 1;
    __ompv_temp_last_iter6 = 0;
    __ompc_static_init_4(__ompv_gtid_s1, 2, &__ompv_temp_do_lower6, &__ompv_temp_do_upper6, &__ompv_temp_do_stride6, 1, 1);
    if(__ompv_temp_do_upper6 > temp_limit6)
    {
      __ompv_temp_do_upper6 = temp_limit6;
    }
    for(__mplocal_j6 = __ompv_temp_do_lower6; __mplocal_j6 <= __ompv_temp_do_upper6; __mplocal_j6 = __mplocal_j6 + 1)
    {
      __mplocal_rho0 = __mplocal_rho0 + (*(_IEEE64 *)((_UINT8 *)(r0) + (_UINT32)(((_UINT64)((_UINT64) __mplocal_j6) * 8ULL))) ** (_IEEE64 *)((_UINT8 *)(r0) + (_UINT32)(((_UINT64)((_UINT64) __mplocal_j6) * 8ULL))));
      _10 :;
    }
    __ompc_barrier();
    __ompc_critical(__ompv_gtid_s1, (_UINT32) & __mplock_3);
    rho = rho + __mplocal_rho0;
    __ompc_end_critical(__ompv_gtid_s1, (_UINT32) & __mplock_3);
    __ompc_barrier();
    __ompv_gtid_s1 = __ompc_get_local_thread_num();
    mpsp_status2 = __ompc_single(__ompv_gtid_s1);
    if(mpsp_status2 == 1)
    {
      beta = rho / rho0;
    }
    __ompc_end_single(__ompv_gtid_s1);
    __ompc_barrier();
    __ompv_gtid_s1 = __ompc_get_local_thread_num();
    temp_limit7 = (lastcol - firstcol) + 1;
    __ompv_temp_do_upper7 = temp_limit7;
    __ompv_temp_do_lower7 = 1;
    __ompv_temp_last_iter7 = 0;
    __ompc_static_init_4(__ompv_gtid_s1, 2, &__ompv_temp_do_lower7, &__ompv_temp_do_upper7, &__ompv_temp_do_stride7, 1, 1);
    if(__ompv_temp_do_upper7 > temp_limit7)
    {
      __ompv_temp_do_upper7 = temp_limit7;
    }
    for(__mplocal_j7 = __ompv_temp_do_lower7; __mplocal_j7 <= __ompv_temp_do_upper7; __mplocal_j7 = __mplocal_j7 + 1)
    {
      * (_IEEE64 *)((_UINT8 *)(p0) + (_UINT32)(((_UINT64)((_UINT64) __mplocal_j7) * 8ULL))) = *(_IEEE64 *)((_UINT8 *)(r0) + (_UINT32)(((_UINT64)((_UINT64) __mplocal_j7) * 8ULL))) + (*(_IEEE64 *)((_UINT8 *)(p0) + (_UINT32)(((_UINT64)((_UINT64) __mplocal_j7) * 8ULL))) * beta);
      _11 :;
    }
    __ompc_barrier();
    _3 :;
    cgit = cgit + 1;
  }
  __ompv_gtid_s1 = __ompc_get_local_thread_num();
  mpsp_status3 = __ompc_single(__ompv_gtid_s1);
  if(mpsp_status3 == 1)
  {
    sum = 0.0;
  }
  __ompc_end_single(__ompv_gtid_s1);
  __ompv_gtid_s1 = __ompc_get_local_thread_num();
  temp_limit8 = (lastrow - firstrow) + 1;
  __ompv_temp_do_upper8 = temp_limit8;
  __ompv_temp_do_lower8 = 1;
  __ompv_temp_last_iter8 = 0;
  __ompc_static_init_4(__ompv_gtid_s1, 2, &__ompv_temp_do_lower8, &__ompv_temp_do_upper8, &__ompv_temp_do_stride8, 1, 1);
  if(__ompv_temp_do_upper8 > temp_limit8)
  {
    __ompv_temp_do_upper8 = temp_limit8;
  }
  for(__mplocal_j8 = __ompv_temp_do_lower8; __mplocal_j8 <= __ompv_temp_do_upper8; __mplocal_j8 = __mplocal_j8 + 1)
  {
    __mplocal_d0 = 0.0;
    __mplocal_k0 = *(_INT32 *)((_UINT8 *)(rowstr0) + (_UINT32)(((_UINT64)((_UINT64) __mplocal_j8) * 4ULL)));
    while(__mplocal_k0 <= (*((_INT32 *)((_UINT8 *)(rowstr0) + (_UINT32)(((_UINT64)((_UINT64) __mplocal_j8) * 4ULL))) + 1LL) + -1))
    {
      __mplocal_d0 = __mplocal_d0 + (*(_IEEE64 *)((_UINT8 *)(z0) + (_UINT32)(((_UINT64)(*(_INT32 *)((_UINT8 *)(colidx0) + (_UINT32)(((_UINT64)((_UINT64) __mplocal_k0) * 4ULL)))) * 8ULL))) ** (_IEEE64 *)((_UINT8 *)(a0) + (_UINT32)(((_UINT64)((_UINT64) __mplocal_k0) * 8ULL))));
      _13 :;
      __mplocal_k0 = __mplocal_k0 + 1;
    }
    * (_IEEE64 *)((_UINT8 *)(w0) + (_UINT32)(((_UINT64)((_UINT64) __mplocal_j8) * 8ULL))) = __mplocal_d0;
    _12 :;
  }
  __ompc_barrier();
  __ompv_gtid_s1 = __ompc_get_local_thread_num();
  temp_limit9 = (lastcol - firstcol) + 1;
  __ompv_temp_do_upper9 = temp_limit9;
  __ompv_temp_do_lower9 = 1;
  __ompv_temp_last_iter9 = 0;
  __ompc_static_init_4(__ompv_gtid_s1, 2, &__ompv_temp_do_lower9, &__ompv_temp_do_upper9, &__ompv_temp_do_stride9, 1, 1);
  if(__ompv_temp_do_upper9 > temp_limit9)
  {
    __ompv_temp_do_upper9 = temp_limit9;
  }
  for(__mplocal_j9 = __ompv_temp_do_lower9; __mplocal_j9 <= __ompv_temp_do_upper9; __mplocal_j9 = __mplocal_j9 + 1)
  {
    * (_IEEE64 *)((_UINT8 *)(r0) + (_UINT32)(((_UINT64)((_UINT64) __mplocal_j9) * 8ULL))) = *(_IEEE64 *)((_UINT8 *)(w0) + (_UINT32)(((_UINT64)((_UINT64) __mplocal_j9) * 8ULL)));
    _14 :;
  }
  __ompc_barrier();
  __ompv_gtid_s1 = __ompc_get_local_thread_num();
  __mplocal_sum0 = 0.0;
  temp_limit10 = (lastcol - firstcol) + 1;
  __ompv_temp_do_upper10 = temp_limit10;
  __ompv_temp_do_lower10 = 1;
  __ompv_temp_last_iter10 = 0;
  __ompc_static_init_4(__ompv_gtid_s1, 2, &__ompv_temp_do_lower10, &__ompv_temp_do_upper10, &__ompv_temp_do_stride10, 1, 1);
  if(__ompv_temp_do_upper10 > temp_limit10)
  {
    __ompv_temp_do_upper10 = temp_limit10;
  }
  for(__mplocal_j10 = __ompv_temp_do_lower10; __mplocal_j10 <= __ompv_temp_do_upper10; __mplocal_j10 = __mplocal_j10 + 1)
  {
    __mplocal_d1 = *(_IEEE64 *)((_UINT8 *)(x0) + (_UINT32)(((_UINT64)((_UINT64) __mplocal_j10) * 8ULL))) - *(_IEEE64 *)((_UINT8 *)(r0) + (_UINT32)(((_UINT64)((_UINT64) __mplocal_j10) * 8ULL)));
    __mplocal_sum0 = __mplocal_sum0 + (__mplocal_d1 * __mplocal_d1);
    _15 :;
  }
  __ompc_barrier();
  __ompc_critical(__ompv_gtid_s1, (_UINT32) & __mplock_4);
  sum = sum + __mplocal_sum0;
  __ompc_end_critical(__ompv_gtid_s1, (_UINT32) & __mplock_4);
  __ompc_barrier();
  __ompv_gtid_s1 = __ompc_get_local_thread_num();
  mpsp_status4 = __ompc_single(__ompv_gtid_s1);
  if(mpsp_status4 == 1)
  {
    __comma = sqrt(sum);
    * rnorm = __comma;
  }
  __ompc_end_single(__ompv_gtid_s1);
  __ompc_barrier();
  return;
} /* conj_grad */


static void makea(
  _INT32 n,
  _INT32 nz,
  _IEEE64 * a0,
  _INT32 * colidx0,
  _INT32 * rowstr0,
  _INT32 nonzer,
  _INT32 firstrow0,
  _INT32 lastrow0,
  _INT32 firstcol0,
  _INT32 lastcol0,
  _IEEE64 rcond,
  _INT32 * arow0,
  _INT32 * acol0,
  _IEEE64 * aelt0,
  _IEEE64 * v0,
  _INT32 * iv0,
  _IEEE64 shift)
{
  register _INT32 trip_count;
  register _INT32 __ompv_ok_to_fork;
  register _UINT32 reg7;
  _INT32 i;
  _INT32 nnza;
  _INT32 iouter;
  _INT32 ivelt;
  _INT32 ivelt1;
  _INT32 irow;
  _INT32 nzv;
  _IEEE64 size;
  _IEEE64 ratio;
  _IEEE64 scale;
  _INT32 jcol;
  _INT32 __ompv_gtid_s1;
  
  /*Begin_of_nested_PU(s)*/
static void __ompdo_makea1(__ompv_gtid_a, __ompv_slink_a)
  _INT32 __ompv_gtid_a;
  _UINT32 __ompv_slink_a;
{
  register _INT32 temp_limit;
  _UINT32 __slink_sym_temp_4;
  _INT32 __ompv_temp_gtid;
  _INT32 __mplocal_i;
  _INT32 __ompv_temp_do_upper;
  _INT32 __ompv_temp_do_lower;
  _INT32 __ompv_temp_do_stride;
  _INT32 __ompv_temp_last_iter;
  
  __slink_sym_temp_4 = __ompv_slink_a;
  __ompv_temp_gtid = __ompv_gtid_a;
  temp_limit = n;
  __ompv_temp_do_upper = temp_limit;
  __ompv_temp_do_lower = 1;
  __ompv_temp_last_iter = 0;
  __ompc_static_init_4(__ompv_temp_gtid, 2, &__ompv_temp_do_lower, &__ompv_temp_do_upper, &__ompv_temp_do_stride, 1, 1);
  if(__ompv_temp_do_upper > temp_limit)
  {
    __ompv_temp_do_upper = temp_limit;
  }
  for(__mplocal_i = __ompv_temp_do_lower; __mplocal_i <= __ompv_temp_do_upper; __mplocal_i = __mplocal_i + 1)
  {
    * (_INT32 *)((_UINT8 *)(colidx0) + (_UINT32)(((_UINT64)((_INT64)(n + __mplocal_i)) * 4ULL))) = 0;
    _1 :;
  }
  return;
} /* __ompdo_makea1 */
  
  size = 1.0;
  ratio = pow(rcond, _F8RECIP((_IEEE64)(n)));
  nnza = 0;
  trip_count = n;
  __ompv_ok_to_fork = __ompc_can_fork();
  if((trip_count > 1) && (__ompv_ok_to_fork == 1))
  {
    __ompc_fork(0, &__ompdo_makea1, reg7);
  }
  else
  {
    __ompv_gtid_s1 = __ompc_get_local_thread_num();
    __ompc_serialized_parallel();
    for(i = 1; n >= i; i = i + 1)
    {
      * (_INT32 *)((_UINT8 *)(colidx0) + (_UINT32)(((_UINT64)((_INT64)(n + i)) * 4ULL))) = 0;
      _1 :;
    }
    __ompc_end_serialized_parallel();
  }
  iouter = 1;
  while(n >= iouter)
  {
    nzv = nonzer;
    sprnvc(n, nzv, v0, iv0, colidx0, (_INT32 *)((_UINT8 *)(colidx0) + (_UINT32)(((_UINT64)((_UINT64) n) * 4ULL))));
    vecset(n, v0, iv0, &nzv, iouter, 5.0e-01);
    ivelt = 1;
    while(ivelt <= nzv)
    {
      jcol = *(_INT32 *)((_UINT8 *)(iv0) + (_UINT32)(((_UINT64)((_UINT64) ivelt) * 4ULL)));
      if((firstcol0 <= jcol) && (lastcol0 >= jcol))
      {
        scale = *(_IEEE64 *)((_UINT8 *)(v0) + (_UINT32)(((_UINT64)((_UINT64) ivelt) * 8ULL))) * size;
        ivelt1 = 1;
        while(ivelt1 <= nzv)
        {
          irow = *(_INT32 *)((_UINT8 *)(iv0) + (_UINT32)(((_UINT64)((_UINT64) ivelt1) * 4ULL)));
          if((firstrow0 <= irow) && (lastrow0 >= irow))
          {
            nnza = nnza + 1;
            if(nz < nnza)
            {
              printf((_INT8 *) "Space for matrix elements exceeded in makea\n");
              printf((_INT8 *) "nnza, nzmax = %d, %d\n", nnza, nz);
              printf((_INT8 *) "iouter = %d\n", iouter);
              exit(1);
            }
            * (_INT32 *)((_UINT8 *)(acol0) + (_UINT32)(((_UINT64)((_UINT64) nnza) * 4ULL))) = jcol;
            * (_INT32 *)((_UINT8 *)(arow0) + (_UINT32)(((_UINT64)((_UINT64) nnza) * 4ULL))) = irow;
            * (_IEEE64 *)((_UINT8 *)(aelt0) + (_UINT32)(((_UINT64)((_UINT64) nnza) * 8ULL))) = *(_IEEE64 *)((_UINT8 *)(v0) + (_UINT32)(((_UINT64)((_UINT64) ivelt1) * 8ULL))) * scale;
          }
          _4 :;
          ivelt1 = ivelt1 + 1;
        }
      }
      _3 :;
      ivelt = ivelt + 1;
    }
    size = size * ratio;
    _2 :;
    iouter = iouter + 1;
  }
  i = firstrow0;
  while(lastrow0 >= i)
  {
    if((firstcol0 <= i) && (lastcol0 >= i))
    {
      iouter = n + i;
      nnza = nnza + 1;
      if(nz < nnza)
      {
        printf((_INT8 *) "Space for matrix elements exceeded in makea\n");
        printf((_INT8 *) "nnza, nzmax = %d, %d\n", nnza, nz);
        printf((_INT8 *) "iouter = %d\n", iouter);
        exit(1);
      }
      * (_INT32 *)((_UINT8 *)(acol0) + (_UINT32)(((_UINT64)((_UINT64) nnza) * 4ULL))) = i;
      * (_INT32 *)((_UINT8 *)(arow0) + (_UINT32)(((_UINT64)((_UINT64) nnza) * 4ULL))) = i;
      * (_IEEE64 *)((_UINT8 *)(aelt0) + (_UINT32)(((_UINT64)((_UINT64) nnza) * 8ULL))) = rcond - shift;
    }
    _5 :;
    i = i + 1;
  }
  sparse(a0, colidx0, rowstr0, n, arow0, acol0, aelt0, firstrow0, lastrow0, v0, iv0, (_INT32 *)((_UINT8 *)(iv0) + (_UINT32)(((_UINT64)((_UINT64) n) * 4ULL))), nnza);
  return;
} /* makea */




static void sparse(
  _IEEE64 * a0,
  _INT32 * colidx0,
  _INT32 * rowstr0,
  _INT32 n,
  _INT32 * arow0,
  _INT32 * acol0,
  _IEEE64 * aelt0,
  _INT32 firstrow0,
  _INT32 lastrow0,
  _IEEE64 * x0,
  _INT32 * mark,
  _INT32 * nzloc,
  _INT32 nnza)
{
  register _INT32 trip_count;
  register _INT32 __ompv_ok_to_fork;
  register _UINT32 reg7;
  register _INT32 trip_count0;
  register _INT32 __ompv_ok_to_fork0;
  _INT32 nrows;
  _INT32 i;
  _INT32 j;
  _INT32 jajp1;
  _INT32 nza;
  _INT32 k;
  _INT32 nzrow;
  _IEEE64 xi;
  _INT32 __ompv_gtid_s1;
  
  /*Begin_of_nested_PU(s)*/
static void __ompdo_sparse2(__ompv_gtid_a, __ompv_slink_a)
  _INT32 __ompv_gtid_a;
  _UINT32 __ompv_slink_a;
{
  register _INT32 temp_limit;
  _UINT32 __slink_sym_temp_7;
  _INT32 __ompv_temp_gtid;
  _INT32 __mplocal_i;
  _INT32 __ompv_temp_do_upper;
  _INT32 __ompv_temp_do_lower;
  _INT32 __ompv_temp_do_stride;
  _INT32 __ompv_temp_last_iter;
  
  __slink_sym_temp_7 = __ompv_slink_a;
  __ompv_temp_gtid = __ompv_gtid_a;
  temp_limit = n;
  __ompv_temp_do_upper = temp_limit;
  __ompv_temp_do_lower = 1;
  __ompv_temp_last_iter = 0;
  __ompc_static_init_4(__ompv_temp_gtid, 2, &__ompv_temp_do_lower, &__ompv_temp_do_upper, &__ompv_temp_do_stride, 1, 1);
  if(__ompv_temp_do_upper > temp_limit)
  {
    __ompv_temp_do_upper = temp_limit;
  }
  for(__mplocal_i = __ompv_temp_do_lower; __mplocal_i <= __ompv_temp_do_upper; __mplocal_i = __mplocal_i + 1)
  {
    * (_IEEE64 *)((_UINT8 *)(x0) + (_UINT32)(((_UINT64)((_UINT64) __mplocal_i) * 8ULL))) = 0.0;
    * (_INT32 *)((_UINT8 *)(mark) + (_UINT32)(((_UINT64)((_UINT64) __mplocal_i) * 4ULL))) = 0;
    _1 :;
  }
  return;
} /* __ompdo_sparse2 */
static void __ompdo_sparse1(__ompv_gtid_a, __ompv_slink_a)
  _INT32 __ompv_gtid_a;
  _UINT32 __ompv_slink_a;
{
  register _INT32 temp_limit;
  _UINT32 __slink_sym_temp_6;
  _INT32 __ompv_temp_gtid;
  _INT32 __mplocal_j;
  _INT32 __ompv_temp_do_upper;
  _INT32 __ompv_temp_do_lower;
  _INT32 __ompv_temp_do_stride;
  _INT32 __ompv_temp_last_iter;
  
  __slink_sym_temp_6 = __ompv_slink_a;
  __ompv_temp_gtid = __ompv_gtid_a;
  temp_limit = n;
  __ompv_temp_do_upper = temp_limit;
  __ompv_temp_do_lower = 1;
  __ompv_temp_last_iter = 0;
  __ompc_static_init_4(__ompv_temp_gtid, 2, &__ompv_temp_do_lower, &__ompv_temp_do_upper, &__ompv_temp_do_stride, 1, 1);
  if(__ompv_temp_do_upper > temp_limit)
  {
    __ompv_temp_do_upper = temp_limit;
  }
  for(__mplocal_j = __ompv_temp_do_lower; __mplocal_j <= __ompv_temp_do_upper; __mplocal_j = __mplocal_j + 1)
  {
    * (_INT32 *)((_UINT8 *)(rowstr0) + (_UINT32)(((_UINT64)((_UINT64) __mplocal_j) * 4ULL))) = 0;
    * (_INT32 *)((_UINT8 *)(mark) + (_UINT32)(((_UINT64)((_UINT64) __mplocal_j) * 4ULL))) = 0;
    _1 :;
  }
  return;
} /* __ompdo_sparse1 */
  
  nrows = (lastrow0 - firstrow0) + 1;
  trip_count = n;
  __ompv_ok_to_fork = __ompc_can_fork();
  if((trip_count > 1) && (__ompv_ok_to_fork == 1))
  {
    __ompc_fork(0, &__ompdo_sparse1, reg7);
  }
  else
  {
    __ompv_gtid_s1 = __ompc_get_local_thread_num();
    __ompc_serialized_parallel();
    for(j = 1; n >= j; j = j + 1)
    {
      * (_INT32 *)((_UINT8 *)(rowstr0) + (_UINT32)(((_UINT64)((_UINT64) j) * 4ULL))) = 0;
      * (_INT32 *)((_UINT8 *)(mark) + (_UINT32)(((_UINT64)((_UINT64) j) * 4ULL))) = 0;
      _1 :;
    }
    __ompc_end_serialized_parallel();
  }
  * ((_INT32 *)((_UINT8 *)(rowstr0) + (_UINT32)(((_UINT64)((_UINT64) n) * 4ULL))) + 1LL) = 0;
  nza = 1;
  while(nnza >= nza)
  {
    j = (*(_INT32 *)((_UINT8 *)(arow0) + (_UINT32)(((_UINT64)((_UINT64) nza) * 4ULL))) - firstrow0) + 2;
    * (_INT32 *)((_UINT8 *)(rowstr0) + (_UINT32)(((_UINT64)((_UINT64) j) * 4ULL))) = *(_INT32 *)((_UINT8 *)(rowstr0) + (_UINT32)(((_UINT64)((_UINT64) j) * 4ULL))) + 1;
    _2 :;
    nza = nza + 1;
  }
  * (rowstr0 + 1LL) = 1;
  j = 2;
  while(j <= (nrows + 1))
  {
    * (_INT32 *)((_UINT8 *)(rowstr0) + (_UINT32)(((_UINT64)((_UINT64) j) * 4ULL))) = *((_INT32 *)((_UINT8 *)(rowstr0) + (_UINT32)(((_UINT64)((_UINT64) j) * 4ULL))) + -1LL) + *(_INT32 *)((_UINT8 *)(rowstr0) + (_UINT32)(((_UINT64)((_UINT64) j) * 4ULL)));
    _3 :;
    j = j + 1;
  }
  nza = 1;
  while(nnza >= nza)
  {
    j = (*(_INT32 *)((_UINT8 *)(arow0) + (_UINT32)(((_UINT64)((_UINT64) nza) * 4ULL))) - firstrow0) + 1;
    k = *(_INT32 *)((_UINT8 *)(rowstr0) + (_UINT32)(((_UINT64)((_UINT64) j) * 4ULL)));
    * (_IEEE64 *)((_UINT8 *)(a0) + (_UINT32)(((_UINT64)((_UINT64) k) * 8ULL))) = *(_IEEE64 *)((_UINT8 *)(aelt0) + (_UINT32)(((_UINT64)((_UINT64) nza) * 8ULL)));
    * (_INT32 *)((_UINT8 *)(colidx0) + (_UINT32)(((_UINT64)((_UINT64) k) * 4ULL))) = *(_INT32 *)((_UINT8 *)(acol0) + (_UINT32)(((_UINT64)((_UINT64) nza) * 4ULL)));
    * (_INT32 *)((_UINT8 *)(rowstr0) + (_UINT32)(((_UINT64)((_UINT64) j) * 4ULL))) = *(_INT32 *)((_UINT8 *)(rowstr0) + (_UINT32)(((_UINT64)((_UINT64) j) * 4ULL))) + 1;
    _4 :;
    nza = nza + 1;
  }
  j = nrows;
  while(j > 0)
  {
    * ((_INT32 *)((_UINT8 *)(rowstr0) + (_UINT32)(((_UINT64)((_UINT64) j) * 4ULL))) + 1LL) = *(_INT32 *)((_UINT8 *)(rowstr0) + (_UINT32)(((_UINT64)((_UINT64) j) * 4ULL)));
    _5 :;
    j = j + -1;
  }
  * (rowstr0 + 1LL) = 1;
  nza = 0;
  trip_count0 = n;
  __ompv_ok_to_fork0 = __ompc_can_fork();
  if((trip_count0 > 1) && (__ompv_ok_to_fork0 == 1))
  {
    __ompc_fork(0, &__ompdo_sparse2, reg7);
  }
  else
  {
    __ompv_gtid_s1 = __ompc_get_local_thread_num();
    __ompc_serialized_parallel();
    for(i = 1; n >= i; i = i + 1)
    {
      * (_IEEE64 *)((_UINT8 *)(x0) + (_UINT32)(((_UINT64)((_UINT64) i) * 8ULL))) = 0.0;
      * (_INT32 *)((_UINT8 *)(mark) + (_UINT32)(((_UINT64)((_UINT64) i) * 4ULL))) = 0;
      _6 :;
    }
    __ompc_end_serialized_parallel();
  }
  jajp1 = *(rowstr0 + 1LL);
  j = 1;
  while(nrows >= j)
  {
    nzrow = 0;
    k = jajp1;
    while(*((_INT32 *)((_UINT8 *)(rowstr0) + (_UINT32)(((_UINT64)((_UINT64) j) * 4ULL))) + 1LL) > k)
    {
      i = *(_INT32 *)((_UINT8 *)(colidx0) + (_UINT32)(((_UINT64)((_UINT64) k) * 4ULL)));
      * (_IEEE64 *)((_UINT8 *)(x0) + (_UINT32)(((_UINT64)((_UINT64) i) * 8ULL))) = *(_IEEE64 *)((_UINT8 *)(a0) + (_UINT32)(((_UINT64)((_UINT64) k) * 8ULL))) + *(_IEEE64 *)((_UINT8 *)(x0) + (_UINT32)(((_UINT64)((_UINT64) i) * 8ULL)));
      if((*(_INT32 *)((_UINT8 *)(mark) + (_UINT32)(((_UINT64)((_UINT64) i) * 4ULL))) == 0) && (*(_IEEE64 *)((_UINT8 *)(x0) + (_UINT32)(((_UINT64)((_UINT64) i) * 8ULL))) != 0.0))
      {
        * (_INT32 *)((_UINT8 *)(mark) + (_UINT32)(((_UINT64)((_UINT64) i) * 4ULL))) = 1;
        nzrow = nzrow + 1;
        * (_INT32 *)((_UINT8 *)(nzloc) + (_UINT32)(((_UINT64)((_UINT64) nzrow) * 4ULL))) = i;
      }
      _8 :;
      k = k + 1;
    }
    k = 1;
    while(k <= nzrow)
    {
      i = *(_INT32 *)((_UINT8 *)(nzloc) + (_UINT32)(((_UINT64)((_UINT64) k) * 4ULL)));
      * (_INT32 *)((_UINT8 *)(mark) + (_UINT32)(((_UINT64)((_UINT64) i) * 4ULL))) = 0;
      xi = *(_IEEE64 *)((_UINT8 *)(x0) + (_UINT32)(((_UINT64)((_UINT64) i) * 8ULL)));
      * (_IEEE64 *)((_UINT8 *)(x0) + (_UINT32)(((_UINT64)((_UINT64) i) * 8ULL))) = 0.0;
      if(xi != 0.0)
      {
        nza = nza + 1;
        * (_IEEE64 *)((_UINT8 *)(a0) + (_UINT32)(((_UINT64)((_UINT64) nza) * 8ULL))) = xi;
        * (_INT32 *)((_UINT8 *)(colidx0) + (_UINT32)(((_UINT64)((_UINT64) nza) * 4ULL))) = i;
      }
      _9 :;
      k = k + 1;
    }
    jajp1 = *((_INT32 *)((_UINT8 *)(rowstr0) + (_UINT32)(((_UINT64)((_UINT64) j) * 4ULL))) + 1LL);
    * ((_INT32 *)((_UINT8 *)(rowstr0) + (_UINT32)(((_UINT64)((_UINT64) j) * 4ULL))) + 1LL) = *(rowstr0 + 1LL) + nza;
    _7 :;
    j = j + 1;
  }
  return;
} /* sparse */






static void sprnvc(
  _INT32 n,
  _INT32 nz,
  _IEEE64 * v0,
  _INT32 * iv0,
  _INT32 * nzloc,
  _INT32 * mark)
{
  register _IEEE64 __comma;
  register _IEEE64 __comma0;
  register _INT32 __comma1;
  _INT32 nn1;
  _INT32 nzrow;
  _INT32 nzv;
  _INT32 ii;
  _INT32 i;
  _IEEE64 vecelt;
  _IEEE64 vecloc;
  
  nzv = 0;
  nzrow = 0;
  nn1 = 1;
  do
  {
    nn1 = nn1 * 2;
    _1 :;
  }
  while(n > nn1);
  while(nz > nzv)
  {
    __comma = randlc(&tran, amult);
    vecelt = __comma;
    __comma0 = randlc(&tran, amult);
    vecloc = __comma0;
    __comma1 = icnvrt(vecloc, nn1);
    i = __comma1 + 1;
    if(n < i)
    {
      goto _2;
    }
    if(*(_INT32 *)((_UINT8 *)(mark) + (_UINT32)(((_UINT64)((_UINT64) i) * 4ULL))) == 0)
    {
      * (_INT32 *)((_UINT8 *)(mark) + (_UINT32)(((_UINT64)((_UINT64) i) * 4ULL))) = 1;
      nzrow = nzrow + 1;
      * (_INT32 *)((_UINT8 *)(nzloc) + (_UINT32)(((_UINT64)((_UINT64) nzrow) * 4ULL))) = i;
      nzv = nzv + 1;
      * (_IEEE64 *)((_UINT8 *)(v0) + (_UINT32)(((_UINT64)((_UINT64) nzv) * 8ULL))) = vecelt;
      * (_INT32 *)((_UINT8 *)(iv0) + (_UINT32)(((_UINT64)((_UINT64) nzv) * 4ULL))) = i;
    }
    _2 :;
  }
  ii = 1;
  while(nzrow >= ii)
  {
    i = *(_INT32 *)((_UINT8 *)(nzloc) + (_UINT32)(((_UINT64)((_UINT64) ii) * 4ULL)));
    * (_INT32 *)((_UINT8 *)(mark) + (_UINT32)(((_UINT64)((_UINT64) i) * 4ULL))) = 0;
    _3 :;
    ii = ii + 1;
  }
  return;
} /* sprnvc */


static _INT32 icnvrt(
  _IEEE64 x0,
  _INT32 ipwr2)
{
  
  return _I4F8TRUNC((_IEEE64)(ipwr2) * x0);
} /* icnvrt */


static void vecset(
  _INT32 n,
  _IEEE64 * v0,
  _INT32 * iv0,
  _INT32 * nzv,
  _INT32 i,
  _IEEE64 val)
{
  _INT32 k;
  _INT32 set;
  
  set = 0;
  k = 1;
  while(*nzv >= k)
  {
    if(*(_INT32 *)((_UINT8 *)(iv0) + (_UINT32)(((_UINT64)((_UINT64) k) * 4ULL))) == i)
    {
      * (_IEEE64 *)((_UINT8 *)(v0) + (_UINT32)(((_UINT64)((_UINT64) k) * 8ULL))) = val;
      set = 1;
    }
    _1 :;
    k = k + 1;
  }
  if(set == 0)
  {
    * nzv = *nzv + 1;
    * (_IEEE64 *)((_UINT8 *)(v0) + (_UINT32)(((_UINT64)(*nzv) * 8ULL))) = val;
    * (_INT32 *)((_UINT8 *)(iv0) + (_UINT32)(((_UINT64)(*nzv) * 4ULL))) = i;
  }
  return;
} /* vecset */

