/*******************************************************
 * C file translated from WHIRL Sun Nov 20 21:07:17 2005
 *******************************************************/

/* Include file-level type and variable decls */
#include "cg.O3.w2c.h"


extern _INT32 main(
  _INT32 argc,
  _INT8 ** argv)
{
  register _INT32 __ompv_ok_to_fork;
  register _IEEE64 norm_temp;
  register _UINT32 reg7;
  register _INT32 preg31;
  register _INT32 preg25;
  register _INT32 preg2;
  register _UINT32 preg10;
  register _INT32 preg24;
  register _INT32 preg18;
  register _INT32 preg23;
  register _INT32 k0;
  register _INT32 preg26;
  register _UINT32 preg11;
  register _UINT32 preg19;
  register _UINT32 preg22;
  register _UINT32 preg3;
  register _INT32 j0;
  register _INT32 preg32;
  register _INT32 preg4;
  register _INT32 preg27;
  register _INT32 preg5;
  register _UINT32 preg9;
  register _IEEE64 preg17;
  register _IEEE64 __comma;
  register _INT32 j1;
  register _INT32 preg28;
  register _IEEE64 preg33;
  register _UINT32 preg7;
  register _UINT32 preg8;
  register _INT32 i0;
  register _UINT32 preg6;
  register _INT32 __ompv_ok_to_fork0;
  register _IEEE64 norm_temp0;
  register _INT32 it0;
  register _UINT32 preg21;
  register _INT32 j2;
  register _INT32 preg29;
  register _UINT32 preg15;
  register _UINT32 preg16;
  register _IEEE64 preg20;
  register _IEEE64 __comma0;
  register _IEEE64 preg;
  register _IEEE64 preg12;
  register _INT32 j3;
  register _INT32 preg30;
  register _IEEE64 preg0;
  register _UINT32 preg13;
  register _UINT32 preg14;
  register _INT32 __comma1;
  register _IEEE64 __comma2;
  register _IEEE64 preg1;
  register _INT32 reg8;
  _INT32 i;
  _INT32 j;
  _INT32 nthreads;
  _IEEE64 zeta;
  _IEEE64 rnorm;
  _IEEE64 norm_temp11;
  _IEEE64 norm_temp12;
  _IEEE64 mflops;
  _INT32 verified;
  _IEEE64 __mptemp_preg278;
  _IEEE64 __mptemp_preg279;
  
  /*Begin_of_nested_PU(s)*/
static void __ompregion_main2(__ompv_gtid_a, __ompv_slink_a)
  _INT32 __ompv_gtid_a;
  _UINT32 __ompv_slink_a;
{
  register _INT32 it;
  register _INT32 mpsp_status;
  register _INT32 preg9;
  register _INT32 preg6;
  register _INT32 j;
  register _INT32 preg7;
  register _UINT32 preg12;
  register _INT32 preg;
  register _UINT32 preg2;
  register _UINT32 preg3;
  register _IEEE64 preg4;
  register _INT32 mpsp_status0;
  register _IEEE64 __comma;
  register _INT32 mp_is_master;
  register _INT32 preg5;
  register _INT32 j0;
  register _INT32 preg8;
  register _UINT32 preg10;
  register _INT32 preg11;
  register _UINT32 preg0;
  register _UINT32 preg1;
  register _INT32 mp_is_master0;
  register _INT32 __comma0;
  _IEEE64 __mplocal_norm_temp11;
  _IEEE64 __mplocal_norm_temp12;
  _INT32 __ompv_temp_do_upper;
  _INT32 __ompv_temp_do_lower;
  _INT32 __ompv_temp_do_stride;
  _INT32 __ompv_temp_do_upper0;
  _INT32 __ompv_temp_do_lower0;
  _INT32 __ompv_temp_do_stride0;
  
  it = 1;
  it = it;
  _3 :;
  conj_grad((_INT32 *) colidx, (_INT32 *) rowstr, (_IEEE64 *) x, (_IEEE64 *) z, (_IEEE64 *) a, (_IEEE64 *) p, (_IEEE64 *) q, (_IEEE64 *) r, (_IEEE64 *) w, &rnorm);
  mpsp_status = __ompc_single(__ompv_gtid_a);
  if(!(mpsp_status == 1))
    goto _5;
  norm_temp11 = 0.0;
  norm_temp12 = 0.0;
  goto _4;
  _5 :;
  _4 :;
  __ompc_end_single(__ompv_gtid_a);
  __ompc_barrier();
  __mplocal_norm_temp11 = 0.0;
  __mplocal_norm_temp12 = 0.0;
  preg9 = lastcol - firstcol;
  preg6 = preg9 + 1;
  __ompv_temp_do_upper = preg6;
  __ompv_temp_do_lower = 1;
  __ompc_static_init_4(__ompv_gtid_a, 2, &__ompv_temp_do_lower, &__ompv_temp_do_upper, &__ompv_temp_do_stride, 1, 1);
  __ompv_temp_do_upper = (__ompv_temp_do_upper > preg6 ? preg6 : __ompv_temp_do_upper);
  j = __ompv_temp_do_lower;
  if(!(__ompv_temp_do_upper >= __ompv_temp_do_lower))
    goto _23;
  preg7 = (__ompv_temp_do_upper - __ompv_temp_do_lower) + 1;
  preg12 = (_UINT32)(__ompv_temp_do_lower) * (_UINT32)(8);
  preg = __ompv_temp_do_upper + 1;
  preg2 = (_UINT32)((_IEEE64 *)(preg12 + (_UINT8 *)(z)));
  preg3 = (_UINT32)((_IEEE64 *)(preg12 + (_UINT8 *)(x)));
  j = j;
  _8 :;
  preg4 = *((_IEEE64 *) preg2);
  __mplocal_norm_temp11 = __mplocal_norm_temp11 + (*((_IEEE64 *) preg3) * preg4);
  __mplocal_norm_temp12 = __mplocal_norm_temp12 + (preg4 * preg4);
  j = j + 1;
  preg3 = preg3 + (_UINT32)(8);
  preg2 = preg2 + (_UINT32)(8);
  if(j != preg)
    goto _8;
  goto _6;
  _23 :;
  _6 :;
  __ompc_barrier();
  __ompc_critical(__ompv_gtid_a, (_UINT32) & __mplock_2);
  norm_temp11 = __mplocal_norm_temp11 + norm_temp11;
  norm_temp12 = __mplocal_norm_temp12 + norm_temp12;
  __ompc_end_critical(__ompv_gtid_a, (_UINT32) & __mplock_2);
  __ompc_barrier();
  mpsp_status0 = __ompc_single(__ompv_gtid_a);
  if(!(mpsp_status0 == 1))
    goto _11;
  __comma = sqrt(norm_temp12);
  __mptemp_preg279 = _F8RECIP(__comma);
  zeta = _F8RECIP(norm_temp11) + 2.0e+01;
  goto _10;
  _11 :;
  _10 :;
  __ompc_end_single(__ompv_gtid_a);
  __ompc_barrier();
  mp_is_master = __ompc_master(__ompv_gtid_a);
  if(!(mp_is_master == 1))
    goto _13;
  if(!(it == 1))
    goto _15;
  printf((_INT8 *) "   iteration           ||r||                 zeta\n");
  goto _14;
  _15 :;
  _14 :;
  printf((_INT8 *) "    %5d       %20.14e%20.13e\n", it, rnorm, zeta);
  goto _12;
  _13 :;
  _12 :;
  preg9 = lastcol - firstcol;
  preg5 = preg9 + 1;
  __ompv_temp_do_upper0 = preg5;
  __ompv_temp_do_lower0 = 1;
  __ompc_static_init_4(__ompv_gtid_a, 2, &__ompv_temp_do_lower0, &__ompv_temp_do_upper0, &__ompv_temp_do_stride0, 1, 1);
  __ompv_temp_do_upper0 = (__ompv_temp_do_upper0 > preg5 ? preg5 : __ompv_temp_do_upper0);
  j0 = __ompv_temp_do_lower0;
  if(!(__ompv_temp_do_upper0 >= __ompv_temp_do_lower0))
    goto _24;
  preg8 = (__ompv_temp_do_upper0 - __ompv_temp_do_lower0) + 1;
  preg10 = (_UINT32)(__ompv_temp_do_lower0) * (_UINT32)(8);
  preg11 = __ompv_temp_do_upper0 + 1;
  preg0 = (_UINT32)((_IEEE64 *)(preg10 + (_UINT8 *)(z)));
  preg1 = (_UINT32)((_IEEE64 *)(preg10 + (_UINT8 *)(x)));
  j0 = j0;
  _18 :;
  * ((_IEEE64 *) preg1) = *((_IEEE64 *) preg0) * __mptemp_preg279;
  j0 = j0 + 1;
  preg1 = preg1 + (_UINT32)(8);
  preg0 = preg0 + (_UINT32)(8);
  if(j0 != preg11)
    goto _18;
  goto _16;
  _24 :;
  _16 :;
  __ompc_barrier();
  it = it + 1;
  if(it != 16)
    goto _3;
  mp_is_master0 = __ompc_master(__ompv_gtid_a);
  if(!(mp_is_master0 == 1))
    goto _22;
  __comma0 = omp_get_num_threads();
  nthreads = __comma0;
  goto _21;
  _22 :;
  _21 :;
  return;
} /* __ompregion_main2 */
static void __ompregion_main1(__ompv_gtid_a, __ompv_slink_a)
  _INT32 __ompv_gtid_a;
  _UINT32 __ompv_slink_a;
{
  register _INT32 preg30;
  register _INT32 preg23;
  register _INT32 preg24;
  register _UINT32 preg7;
  register _INT32 preg8;
  register _UINT32 preg14;
  register _INT32 preg16;
  register _INT32 preg17;
  register _INT32 preg18;
  register _INT32 k;
  register _INT32 preg25;
  register _UINT32 preg19;
  register _UINT32 preg20;
  register _INT32 preg26;
  register _UINT32 preg5;
  register _INT32 preg6;
  register _UINT32 preg13;
  register _INT32 mpsp_status;
  register _INT32 preg31;
  register _INT32 preg22;
  register _INT32 j;
  register _INT32 preg27;
  register _UINT32 preg3;
  register _INT32 preg4;
  register _UINT32 preg12;
  register _IEEE64 preg15;
  register _INT32 mpsp_status0;
  register _IEEE64 __comma;
  register _INT32 preg21;
  register _INT32 j0;
  register _INT32 preg28;
  register _UINT32 preg1;
  register _INT32 preg2;
  register _UINT32 preg10;
  register _UINT32 preg11;
  register _INT32 i;
  register _INT32 preg29;
  register _UINT32 preg;
  register _INT32 preg0;
  register _UINT32 preg9;
  register _INT32 mpsp_status1;
  _INT32 __mplocal_i;
  _INT32 __mplocal_j;
  _INT32 __ompv_temp_do_upper;
  _INT32 __ompv_temp_do_lower;
  _INT32 __ompv_temp_do_stride;
  _INT32 __ompv_temp_do_upper0;
  _INT32 __ompv_temp_do_lower0;
  _INT32 __ompv_temp_do_stride0;
  _IEEE64 __mplocal_norm_temp12;
  _INT32 __ompv_temp_do_upper1;
  _INT32 __ompv_temp_do_lower1;
  _INT32 __ompv_temp_do_stride1;
  _INT32 __ompv_temp_do_upper2;
  _INT32 __ompv_temp_do_lower2;
  _INT32 __ompv_temp_do_stride2;
  _INT32 __ompv_temp_do_upper3;
  _INT32 __ompv_temp_do_lower3;
  _INT32 __ompv_temp_do_stride3;
  
  preg30 = lastrow - firstrow;
  preg23 = preg30 + 1;
  __ompv_temp_do_upper = preg23;
  __ompv_temp_do_lower = 1;
  __ompc_static_init_4(__ompv_gtid_a, 2, &__ompv_temp_do_lower, &__ompv_temp_do_upper, &__ompv_temp_do_stride, 1, 1);
  __ompv_temp_do_upper = (__ompv_temp_do_upper > preg23 ? preg23 : __ompv_temp_do_upper);
  __mplocal_j = __ompv_temp_do_lower;
  if(!(__ompv_temp_do_upper >= __ompv_temp_do_lower))
    goto _33;
  preg24 = (__ompv_temp_do_upper - __ompv_temp_do_lower) + 1;
  preg7 = (_UINT32)(__ompv_temp_do_lower) * (_UINT32)(4);
  preg8 = __ompv_temp_do_upper + 1;
  preg14 = (_UINT32)((_INT32 *)(preg7 + (_UINT8 *)(rowstr)));
  __mplocal_j = __mplocal_j;
  _3 :;
  preg16 = *(((_INT32 *) preg14) + 1LL);
  preg17 = *((_INT32 *) preg14);
  preg18 = preg16 - preg17;
  if(!(preg18 >= 1))
    goto _5;
  k = 0;
  preg25 = preg18;
  preg19 = (_UINT32)(preg17) * (_UINT32)(4);
  preg20 = (_UINT32)((_INT32 *)(preg19 + (_UINT8 *)(colidx)));
  k = k;
  _8 :;
  * ((_INT32 *) preg20) = (*((_INT32 *) preg20) - firstcol) + 1;
  k = k + 1;
  preg20 = preg20 + (_UINT32)(4);
  if(k != preg18)
    goto _8;
  goto _4;
  _5 :;
  _4 :;
  __mplocal_j = __mplocal_j + 1;
  preg14 = preg14 + (_UINT32)(4);
  if(__mplocal_j != preg8)
    goto _3;
  goto _1;
  _33 :;
  _1 :;
  __ompv_temp_do_upper0 = 14001;
  __ompv_temp_do_lower0 = 1;
  __ompc_static_init_4(__ompv_gtid_a, 2, &__ompv_temp_do_lower0, &__ompv_temp_do_upper0, &__ompv_temp_do_stride0, 1, 1);
  __ompv_temp_do_upper0 = (__ompv_temp_do_upper0 > 14001 ? 14001 : __ompv_temp_do_upper0);
  __mplocal_i = __ompv_temp_do_lower0;
  if(!(__ompv_temp_do_upper0 >= __ompv_temp_do_lower0))
    goto _34;
  preg26 = (__ompv_temp_do_upper0 - __ompv_temp_do_lower0) + 1;
  preg5 = (_UINT32)(__ompv_temp_do_lower0) * (_UINT32)(8);
  preg6 = __ompv_temp_do_upper0 + 1;
  preg13 = (_UINT32)((_IEEE64 *)(preg5 + (_UINT8 *)(x)));
  __mplocal_i = __mplocal_i;
  _13 :;
  * ((_IEEE64 *) preg13) = 1.0;
  __mplocal_i = __mplocal_i + 1;
  preg13 = preg13 + (_UINT32)(8);
  if(__mplocal_i != preg6)
    goto _13;
  goto _11;
  _34 :;
  _11 :;
  conj_grad((_INT32 *) colidx, (_INT32 *) rowstr, (_IEEE64 *) x, (_IEEE64 *) z, (_IEEE64 *) a, (_IEEE64 *) p, (_IEEE64 *) q, (_IEEE64 *) r, (_IEEE64 *) w, &rnorm);
  mpsp_status = __ompc_single(__ompv_gtid_a);
  norm_temp12 = (mpsp_status == 1 ? 0.0 : norm_temp12);
  __ompc_end_single(__ompv_gtid_a);
  __ompc_barrier();
  __mplocal_norm_temp12 = 0.0;
  preg31 = lastcol - firstcol;
  preg22 = preg31 + 1;
  __ompv_temp_do_upper1 = preg22;
  __ompv_temp_do_lower1 = 1;
  __ompc_static_init_4(__ompv_gtid_a, 2, &__ompv_temp_do_lower1, &__ompv_temp_do_upper1, &__ompv_temp_do_stride1, 1, 1);
  __ompv_temp_do_upper1 = (__ompv_temp_do_upper1 > preg22 ? preg22 : __ompv_temp_do_upper1);
  j = __ompv_temp_do_lower1;
  if(!(__ompv_temp_do_upper1 >= __ompv_temp_do_lower1))
    goto _35;
  preg27 = (__ompv_temp_do_upper1 - __ompv_temp_do_lower1) + 1;
  preg3 = (_UINT32)(__ompv_temp_do_lower1) * (_UINT32)(8);
  preg4 = __ompv_temp_do_upper1 + 1;
  preg12 = (_UINT32)((_IEEE64 *)(preg3 + (_UINT8 *)(z)));
  j = j;
  _20 :;
  preg15 = *((_IEEE64 *) preg12);
  __mplocal_norm_temp12 = __mplocal_norm_temp12 + (preg15 * preg15);
  j = j + 1;
  preg12 = preg12 + (_UINT32)(8);
  if(j != preg4)
    goto _20;
  goto _18;
  _35 :;
  _18 :;
  __ompc_barrier();
  __ompc_critical(__ompv_gtid_a, (_UINT32) & __mplock_1);
  norm_temp12 = norm_temp12 + __mplocal_norm_temp12;
  __ompc_end_critical(__ompv_gtid_a, (_UINT32) & __mplock_1);
  __ompc_barrier();
  mpsp_status0 = __ompc_single(__ompv_gtid_a);
  if(!(mpsp_status0 == 1))
    goto _23;
  __comma = sqrt(norm_temp12);
  __mptemp_preg278 = _F8RECIP(__comma);
  goto _22;
  _23 :;
  _22 :;
  __ompc_end_single(__ompv_gtid_a);
  __ompc_barrier();
  preg31 = lastcol - firstcol;
  preg21 = preg31 + 1;
  __ompv_temp_do_upper2 = preg21;
  __ompv_temp_do_lower2 = 1;
  __ompc_static_init_4(__ompv_gtid_a, 2, &__ompv_temp_do_lower2, &__ompv_temp_do_upper2, &__ompv_temp_do_stride2, 1, 1);
  __ompv_temp_do_upper2 = (__ompv_temp_do_upper2 > preg21 ? preg21 : __ompv_temp_do_upper2);
  j0 = __ompv_temp_do_lower2;
  if(!(__ompv_temp_do_upper2 >= __ompv_temp_do_lower2))
    goto _36;
  preg28 = (__ompv_temp_do_upper2 - __ompv_temp_do_lower2) + 1;
  preg1 = (_UINT32)(__ompv_temp_do_lower2) * (_UINT32)(8);
  preg2 = __ompv_temp_do_upper2 + 1;
  preg10 = (_UINT32)((_IEEE64 *)(preg1 + (_UINT8 *)(z)));
  preg11 = (_UINT32)((_IEEE64 *)(preg1 + (_UINT8 *)(x)));
  j0 = j0;
  _26 :;
  * ((_IEEE64 *) preg11) = *((_IEEE64 *) preg10) * __mptemp_preg278;
  j0 = j0 + 1;
  preg11 = preg11 + (_UINT32)(8);
  preg10 = preg10 + (_UINT32)(8);
  if(j0 != preg2)
    goto _26;
  goto _24;
  _36 :;
  _24 :;
  __ompc_barrier();
  __ompv_temp_do_upper3 = 14001;
  __ompv_temp_do_lower3 = 1;
  __ompc_static_init_4(__ompv_gtid_a, 2, &__ompv_temp_do_lower3, &__ompv_temp_do_upper3, &__ompv_temp_do_stride3, 1, 1);
  __ompv_temp_do_upper3 = (__ompv_temp_do_upper3 > 14001 ? 14001 : __ompv_temp_do_upper3);
  i = __ompv_temp_do_lower3;
  if(!(__ompv_temp_do_upper3 >= __ompv_temp_do_lower3))
    goto _37;
  preg29 = (__ompv_temp_do_upper3 - __ompv_temp_do_lower3) + 1;
  preg = (_UINT32)(__ompv_temp_do_lower3) * (_UINT32)(8);
  preg0 = __ompv_temp_do_upper3 + 1;
  preg9 = (_UINT32)((_IEEE64 *)(preg + (_UINT8 *)(x)));
  i = i;
  _31 :;
  * ((_IEEE64 *) preg9) = 1.0;
  i = i + 1;
  preg9 = preg9 + (_UINT32)(8);
  if(i != preg0)
    goto _31;
  goto _29;
  _37 :;
  _29 :;
  mpsp_status1 = __ompc_single(__ompv_gtid_a);
  zeta = (mpsp_status1 == 1 ? 0.0 : zeta);
  __ompc_end_single(__ompv_gtid_a);
  __ompc_barrier();
  return;
} /* __ompregion_main1 */
  
  firstrow = 1;
  lastrow = 14000;
  firstcol = 1;
  lastcol = 14000;
  printf((_INT8 *) "\n\n NAS Parallel Benchmarks 2.3 OpenMP C version - CG Benchmark\n");
  printf((_INT8 *) " Size: %10d\n", 14000);
  printf((_INT8 *) " Iterations: %5d\n", 15);
  naa = 14000;
  nzz = 2198000;
  tran = 3.14159265e+08;
  amult = 1.220703125e+09;
  randlc(&tran, 1.220703125e+09);
  makea(naa, nzz, (_IEEE64 *) a, (_INT32 *) colidx, (_INT32 *) rowstr, 11, firstrow, lastrow, firstcol, lastcol, 1.0000000000000001e-01, (_INT32 *) arow, (_INT32 *) acol, (_IEEE64 *) aelt, (_IEEE64 *) v, (_INT32 *) iv, 2.0e+01);
  __ompv_ok_to_fork = __ompc_can_fork();
  if(!(__ompv_ok_to_fork == 1))
    goto _85;
  __mptemp_preg278 = norm_temp;
  __ompc_fork(0, &__ompregion_main1, reg7);
  goto _84;
  _85 :;
  __ompc_get_local_thread_num();
  __ompc_serialized_parallel();
  j = 1;
  preg31 = lastrow - firstrow;
  if(!(preg31 >= 0))
    goto _136;
  preg25 = preg31 + 1;
  preg2 = preg31 + 2;
  preg10 = (_UINT32)((_INT32 *)(((_INT8 *) & rowstr) + 4LL));
  j = j;
  _88 :;
  preg24 = *((_INT32 *) preg10);
  preg18 = *(((_INT32 *) preg10) + 1LL);
  preg23 = preg18 - preg24;
  if(!(preg23 >= 1))
    goto _90;
  k0 = 0;
  preg26 = preg23;
  preg11 = (_UINT32)(preg24) * (_UINT32)(4);
  preg19 = (_UINT32)((_INT32 *)(preg11 + (_UINT8 *)(colidx)));
  k0 = k0;
  _93 :;
  * ((_INT32 *) preg19) = (*((_INT32 *) preg19) - firstcol) + 1;
  k0 = k0 + 1;
  preg19 = preg19 + (_UINT32)(4);
  if(k0 != preg23)
    goto _93;
  goto _89;
  _90 :;
  _89 :;
  j = j + 1;
  preg10 = preg10 + (_UINT32)(4);
  if(j != preg2)
    goto _88;
  goto _86;
  _136 :;
  _86 :;
  i = 1;
  preg22 = (_UINT32)((_IEEE64 *)(((_INT8 *) & x) + 8LL));
  preg3 = preg22;
  i = i;
  _98 :;
  * ((_IEEE64 *) preg3) = 1.0;
  i = i + 1;
  preg3 = preg3 + (_UINT32)(8);
  if(i != 14002)
    goto _98;
  conj_grad((_INT32 *) colidx, (_INT32 *) rowstr, (_IEEE64 *) x, (_IEEE64 *) z, (_IEEE64 *) a, (_IEEE64 *) p, (_IEEE64 *) q, (_IEEE64 *) r, (_IEEE64 *) w, &rnorm);
  norm_temp12 = 0.0;
  j0 = 1;
  preg32 = lastcol - firstcol;
  preg4 = preg32 + 1;
  if(!(preg4 >= 1))
    goto _137;
  preg27 = preg4;
  preg5 = preg32 + 2;
  preg9 = (_UINT32)((_IEEE64 *)(((_INT8 *) & z) + 8LL));
  j0 = j0;
  _105 :;
  preg17 = *((_IEEE64 *) preg9);
  norm_temp12 = norm_temp12 + (preg17 * preg17);
  j0 = j0 + 1;
  preg9 = preg9 + (_UINT32)(8);
  if(j0 != preg5)
    goto _105;
  goto _103;
  _137 :;
  _103 :;
  __comma = sqrt(norm_temp12);
  j1 = 1;
  preg32 = lastcol - firstcol;
  preg4 = preg32 + 1;
  if(!(preg4 >= 1))
    goto _138;
  preg28 = preg4;
  preg33 = _F8RECIP(__comma);
  preg5 = preg32 + 2;
  preg7 = (_UINT32)((_IEEE64 *)(((_INT8 *) & z) + 8LL));
  preg8 = preg22;
  j1 = j1;
  _109 :;
  * ((_IEEE64 *) preg8) = *((_IEEE64 *) preg7) * preg33;
  j1 = j1 + 1;
  preg8 = preg8 + (_UINT32)(8);
  preg7 = preg7 + (_UINT32)(8);
  if(j1 != preg5)
    goto _109;
  goto _107;
  _138 :;
  _107 :;
  i0 = 1;
  preg6 = preg22;
  i0 = i0;
  _114 :;
  * ((_IEEE64 *) preg6) = 1.0;
  i0 = i0 + 1;
  preg6 = preg6 + (_UINT32)(8);
  if(i0 != 14002)
    goto _114;
  zeta = 0.0;
  __ompc_end_serialized_parallel();
  _84 :;
  timer_clear(1);
  timer_start(1);
  __ompv_ok_to_fork0 = __ompc_can_fork();
  if(!(__ompv_ok_to_fork0 == 1))
    goto _117;
  __mptemp_preg279 = norm_temp0;
  __ompc_fork(0, &__ompregion_main2, reg7);
  goto _116;
  _117 :;
  __ompc_get_local_thread_num();
  __ompc_serialized_parallel();
  it0 = 1;
  preg21 = (_UINT32)((_IEEE64 *)(((_INT8 *) & z) + 8LL));
  preg22 = (_UINT32)((_IEEE64 *)(((_INT8 *) & x) + 8LL));
  it0 = it0;
  _120 :;
  conj_grad((_INT32 *) colidx, (_INT32 *) rowstr, (_IEEE64 *) x, (_IEEE64 *) z, (_IEEE64 *) a, (_IEEE64 *) p, (_IEEE64 *) q, (_IEEE64 *) r, (_IEEE64 *) w, &rnorm);
  norm_temp11 = 0.0;
  norm_temp12 = 0.0;
  j2 = 1;
  preg32 = lastcol - firstcol;
  preg4 = preg32 + 1;
  if(!(preg4 >= 1))
    goto _139;
  preg29 = preg4;
  preg5 = preg32 + 2;
  preg15 = preg21;
  preg16 = preg22;
  j2 = j2;
  _123 :;
  preg20 = *((_IEEE64 *) preg15);
  norm_temp11 = norm_temp11 + (*((_IEEE64 *) preg16) * preg20);
  norm_temp12 = norm_temp12 + (preg20 * preg20);
  j2 = j2 + 1;
  preg16 = preg16 + (_UINT32)(8);
  preg15 = preg15 + (_UINT32)(8);
  if(j2 != preg5)
    goto _123;
  goto _121;
  _139 :;
  _121 :;
  __comma0 = sqrt(norm_temp12);
  if(!(it0 == 1))
    goto _126;
  printf((_INT8 *) "   iteration           ||r||                 zeta\n");
  goto _125;
  _126 :;
  _125 :;
  preg = _F8RECIP(norm_temp11);
  preg12 = preg + 2.0e+01;
  printf((_INT8 *) "    %5d       %20.14e%20.13e\n", it0, rnorm, preg12);
  j3 = 1;
  preg32 = lastcol - firstcol;
  preg4 = preg32 + 1;
  if(!(preg4 >= 1))
    goto _140;
  preg30 = preg4;
  preg0 = _F8RECIP(__comma0);
  preg5 = preg32 + 2;
  preg13 = preg21;
  preg14 = preg22;
  j3 = j3;
  _129 :;
  * ((_IEEE64 *) preg14) = *((_IEEE64 *) preg13) * preg0;
  j3 = j3 + 1;
  preg14 = preg14 + (_UINT32)(8);
  preg13 = preg13 + (_UINT32)(8);
  if(j3 != preg5)
    goto _129;
  goto _127;
  _140 :;
  _127 :;
  it0 = it0 + 1;
  if(it0 != 16)
    goto _120;
  zeta = preg12;
  __comma1 = omp_get_num_threads();
  nthreads = __comma1;
  __ompc_end_serialized_parallel();
  _116 :;
  timer_stop(1);
  __comma2 = timer_read(1);
  printf((_INT8 *) " Benchmark completed\n");
  preg1 = zeta + -1.7130235054029001e+01;
  if(!(_F8ABS(preg1) <= 1.0e-10))
    goto _133;
  verified = 1;
  printf((_INT8 *) " VERIFICATION SUCCESSFUL\n");
  printf((_INT8 *) " Zeta is    %20.12e\n", zeta);
  printf((_INT8 *) " Error is   %20.12e\n", preg1);
  goto _132;
  _133 :;
  verified = 0;
  printf((_INT8 *) " VERIFICATION FAILED\n");
  printf((_INT8 *) " Zeta                %20.12e\n", zeta);
  printf((_INT8 *) " The correct zeta is %20.12e\n", 1.7130235054029001e+01);
  _132 :;
  if(!(__comma2 != 0.0))
    goto _135;
  mflops = (1.49646e+09 / __comma2) / 1.0e+06;
  goto _134;
  _135 :;
  mflops = 0.0;
  _134 :;
  c_print_results((_INT8 *) "CG", (_INT8) 65, 14000, 0, 0, 15, nthreads, __comma2, mflops, (_INT8 *) "          floating point", verified, (_INT8 *) "2.3", (_INT8 *) "18 Nov 2005", (_INT8 *) "gcc", (_INT8 *) "gcc", (_INT8 *) "(none)", (_INT8 *) "-I../common -I $(HOME)/orc2.1.uh/src/osprey...", (_INT8 *) "-O2", (_INT8 *) "-O2  -L $(HOME)/orc2.1.uh/src/osprey1.0/tar...", (_INT8 *) "randdp");
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
  register _INT32 mpsp_status3;
  register _INT32 preg106;
  register _INT32 preg108;
  register _INT64 preg65;
  register _INT32 preg66;
  register _UINT64 preg100;
  register _UINT32 preg5;
  register _UINT32 preg47;
  register _UINT32 preg49;
  register _UINT32 preg51;
  register _UINT32 preg53;
  register _UINT32 preg55;
  register _UINT32 preg57;
  register _IEEE64 preg74;
  register _INT32 preg16;
  register _INT32 preg104;
  register _INT32 j0;
  register _INT32 preg110;
  register _INT64 preg61;
  register _INT32 preg63;
  register _UINT64 preg98;
  register _UINT32 preg4;
  register _UINT32 preg45;
  register _IEEE64 preg72;
  register _INT32 mpsp_status4;
  register _INT32 preg18;
  register _INT32 preg102;
  register _INT32 j1;
  register _INT32 preg112;
  register _INT64 preg56;
  register _INT32 preg58;
  register _UINT64 preg92;
  register _UINT64 preg94;
  register _UINT32 preg126;
  register _UINT32 preg128;
  register _UINT32 preg37;
  register _UINT32 preg39;
  register _IEEE64 sum0;
  register _INT32 preg99;
  register _INT32 preg67;
  register _INT32 k0;
  register _INT32 preg78;
  register _INT32 preg114;
  register _INT64 preg96;
  register _UINT64 preg0;
  register _UINT64 preg2;
  register _UINT32 preg41;
  register _UINT32 preg43;
  register _UINT32 preg68;
  register _UINT32 preg70;
  register _INT32 preg97;
  register _INT32 j2;
  register _INT32 preg116;
  register _INT64 preg52;
  register _INT32 preg54;
  register _UINT64 preg90;
  register _UINT32 preg124;
  register _UINT32 preg33;
  register _UINT32 preg35;
  register _INT32 preg95;
  register _INT32 j3;
  register _INT32 preg118;
  register _INT64 preg48;
  register _INT32 preg50;
  register _UINT64 preg88;
  register _UINT32 preg122;
  register _UINT32 preg31;
  register _INT32 preg93;
  register _INT32 j4;
  register _INT32 preg120;
  register _INT64 preg44;
  register _INT32 preg46;
  register _UINT64 preg86;
  register _UINT32 preg119;
  register _UINT32 preg28;
  register _UINT32 preg30;
  register _INT32 mpsp_status;
  register _INT32 preg91;
  register _INT32 j5;
  register _INT32 preg121;
  register _INT64 preg40;
  register _INT32 preg42;
  register _UINT64 preg84;
  register _UINT32 preg117;
  register _UINT32 preg20;
  register _UINT32 preg22;
  register _UINT32 preg24;
  register _UINT32 preg26;
  register _INT32 preg89;
  register _INT32 j6;
  register _INT32 preg123;
  register _INT64 preg36;
  register _INT32 preg38;
  register _UINT64 preg82;
  register _UINT32 preg115;
  register _UINT32 preg17;
  register _IEEE64 preg64;
  register _INT32 mpsp_status0;
  register _INT32 preg87;
  register _INT32 j7;
  register _INT32 preg125;
  register _INT64 preg32;
  register _INT32 preg34;
  register _UINT64 preg80;
  register _UINT32 preg113;
  register _UINT32 preg14;
  register _UINT32 preg15;
  register _INT32 mpsp_status1;
  register _INT32 preg85;
  register _INT32 j8;
  register _INT32 preg127;
  register _INT64 preg27;
  register _INT32 preg29;
  register _UINT64 preg73;
  register _UINT64 preg76;
  register _UINT32 preg105;
  register _UINT32 preg107;
  register _UINT32 preg10;
  register _UINT32 preg11;
  register _INT32 preg83;
  register _INT32 preg59;
  register _INT32 k1;
  register _INT32 preg75;
  register _INT32 preg;
  register _INT64 preg77;
  register _UINT64 preg109;
  register _UINT64 preg111;
  register _UINT32 preg12;
  register _UINT32 preg13;
  register _UINT32 preg60;
  register _UINT32 preg62;
  register _INT32 preg81;
  register _INT32 j9;
  register _INT32 preg1;
  register _INT64 preg23;
  register _INT32 preg25;
  register _UINT64 preg71;
  register _UINT32 preg103;
  register _UINT32 preg8;
  register _UINT32 preg9;
  register _INT32 preg79;
  register _INT32 j10;
  register _INT32 preg3;
  register _INT64 preg19;
  register _INT32 preg21;
  register _UINT64 preg69;
  register _UINT32 preg101;
  register _UINT32 preg6;
  register _UINT32 preg7;
  register _INT32 mpsp_status2;
  register _IEEE64 __comma;
  static _IEEE64 d;
  static _IEEE64 sum;
  static _IEEE64 rho;
  static _IEEE64 rho0;
  static _IEEE64 alpha;
  static _IEEE64 beta;
  _INT32 cgit;
  _INT32 __ompv_gtid_s1;
  _INT32 __mplocal_j;
  _INT32 __ompv_temp_do_upper;
  _INT32 __ompv_temp_do_lower;
  _INT32 __ompv_temp_do_stride;
  _IEEE64 __mplocal_rho;
  _INT32 __ompv_temp_do_upper0;
  _INT32 __ompv_temp_do_lower0;
  _INT32 __ompv_temp_do_stride0;
  _INT32 __ompv_temp_do_upper1;
  _INT32 __ompv_temp_do_lower1;
  _INT32 __ompv_temp_do_stride1;
  _INT32 __ompv_temp_do_upper2;
  _INT32 __ompv_temp_do_lower2;
  _INT32 __ompv_temp_do_stride2;
  _INT32 __ompv_temp_do_upper3;
  _INT32 __ompv_temp_do_lower3;
  _INT32 __ompv_temp_do_stride3;
  _IEEE64 __mplocal_d;
  _INT32 __ompv_temp_do_upper4;
  _INT32 __ompv_temp_do_lower4;
  _INT32 __ompv_temp_do_stride4;
  _INT32 __ompv_temp_do_upper5;
  _INT32 __ompv_temp_do_lower5;
  _INT32 __ompv_temp_do_stride5;
  _IEEE64 __mplocal_rho0;
  _INT32 __ompv_temp_do_upper6;
  _INT32 __ompv_temp_do_lower6;
  _INT32 __ompv_temp_do_stride6;
  _INT32 __ompv_temp_do_upper7;
  _INT32 __ompv_temp_do_lower7;
  _INT32 __ompv_temp_do_stride7;
  _IEEE64 __mplocal_d0;
  _INT32 __ompv_temp_do_upper8;
  _INT32 __ompv_temp_do_lower8;
  _INT32 __ompv_temp_do_stride8;
  _INT32 __ompv_temp_do_upper9;
  _INT32 __ompv_temp_do_lower9;
  _INT32 __ompv_temp_do_stride9;
  _IEEE64 __mplocal_sum0;
  _IEEE64 __mplocal_d1;
  _INT32 __ompv_temp_do_upper10;
  _INT32 __ompv_temp_do_lower10;
  _INT32 __ompv_temp_do_stride10;
  
  /*Begin_of_nested_PU(s)*/
  
  __ompv_gtid_s1 = __ompc_get_local_thread_num();
  mpsp_status3 = __ompc_single(__ompv_gtid_s1);
  rho = (mpsp_status3 == 1 ? 0.0 : rho);
  __ompc_end_single(__ompv_gtid_s1);
  __ompv_gtid_s1 = __ompc_get_local_thread_num();
  preg106 = naa + 1;
  __ompv_temp_do_upper = preg106;
  __ompv_temp_do_lower = 1;
  __ompc_static_init_4(__ompv_gtid_s1, 2, &__ompv_temp_do_lower, &__ompv_temp_do_upper, &__ompv_temp_do_stride, 1, 1);
  __ompv_temp_do_upper = (__ompv_temp_do_upper > preg106 ? preg106 : __ompv_temp_do_upper);
  __mplocal_j = __ompv_temp_do_lower;
  if(!(__ompv_temp_do_upper >= __ompv_temp_do_lower))
    goto _142;
  preg108 = (__ompv_temp_do_upper - __ompv_temp_do_lower) + 1;
  preg65 = __ompv_temp_do_lower;
  preg66 = __ompv_temp_do_upper + 1;
  preg100 = (_UINT64)(preg65) * (_UINT64)(8);
  preg5 = (_UINT32)(preg100);
  preg47 = (_UINT32)((_IEEE64 *)((_UINT8 *)(q0) + preg5));
  preg49 = (_UINT32)((_IEEE64 *)((_UINT8 *)(z0) + preg5));
  preg51 = (_UINT32)((_IEEE64 *)((_UINT8 *)(x0) + preg5));
  preg53 = (_UINT32)((_IEEE64 *)((_UINT8 *)(r0) + preg5));
  preg55 = (_UINT32)((_IEEE64 *)((_UINT8 *)(p0) + preg5));
  preg57 = (_UINT32)((_IEEE64 *)((_UINT8 *)(w0) + preg5));
  __mplocal_j = __mplocal_j;
  _72 :;
  * ((_IEEE64 *) preg47) = 0.0;
  * ((_IEEE64 *) preg49) = 0.0;
  preg74 = *((_IEEE64 *) preg51);
  * ((_IEEE64 *) preg53) = preg74;
  * ((_IEEE64 *) preg55) = preg74;
  * ((_IEEE64 *) preg57) = 0.0;
  __mplocal_j = (_INT32)((_UINT64) __mplocal_j) + 1;
  preg57 = preg57 + (_UINT32)(8);
  preg55 = preg55 + (_UINT32)(8);
  preg53 = preg53 + (_UINT32)(8);
  preg51 = preg51 + (_UINT32)(8);
  preg49 = preg49 + (_UINT32)(8);
  preg47 = preg47 + (_UINT32)(8);
  if(__mplocal_j != preg66)
    goto _72;
  goto _70;
  _142 :;
  _70 :;
  __ompv_gtid_s1 = __ompc_get_local_thread_num();
  __mplocal_rho = 0.0;
  preg16 = lastcol - firstcol;
  preg104 = preg16 + 1;
  __ompv_temp_do_upper0 = preg104;
  __ompv_temp_do_lower0 = 1;
  __ompc_static_init_4(__ompv_gtid_s1, 2, &__ompv_temp_do_lower0, &__ompv_temp_do_upper0, &__ompv_temp_do_stride0, 1, 1);
  __ompv_temp_do_upper0 = (__ompv_temp_do_upper0 > preg104 ? preg104 : __ompv_temp_do_upper0);
  j0 = __ompv_temp_do_lower0;
  if(!(__ompv_temp_do_upper0 >= __ompv_temp_do_lower0))
    goto _143;
  preg110 = (__ompv_temp_do_upper0 - __ompv_temp_do_lower0) + 1;
  preg61 = __ompv_temp_do_lower0;
  preg63 = __ompv_temp_do_upper0 + 1;
  preg98 = (_UINT64)(preg61) * (_UINT64)(8);
  preg4 = (_UINT32)(preg98);
  preg45 = (_UINT32)((_IEEE64 *)((_UINT8 *)(x0) + preg4));
  j0 = j0;
  _76 :;
  preg72 = *((_IEEE64 *) preg45);
  __mplocal_rho = __mplocal_rho + (preg72 * preg72);
  j0 = j0 + 1;
  preg45 = preg45 + (_UINT32)(8);
  if(j0 != preg63)
    goto _76;
  goto _74;
  _143 :;
  _74 :;
  __ompc_barrier();
  __ompc_critical(__ompv_gtid_s1, (_UINT32) & __mplock_1);
  rho = rho + __mplocal_rho;
  __ompc_end_critical(__ompv_gtid_s1, (_UINT32) & __mplock_1);
  __ompc_barrier();
  cgit = 1;
  cgit = cgit;
  _80 :;
  __ompv_gtid_s1 = __ompc_get_local_thread_num();
  mpsp_status4 = __ompc_single(__ompv_gtid_s1);
  if(!(mpsp_status4 == 1))
    goto _82;
  rho0 = rho;
  d = 0.0;
  rho = 0.0;
  goto _81;
  _82 :;
  _81 :;
  __ompc_end_single(__ompv_gtid_s1);
  __ompv_gtid_s1 = __ompc_get_local_thread_num();
  preg18 = lastrow - firstrow;
  preg102 = preg18 + 1;
  __ompv_temp_do_upper1 = preg102;
  __ompv_temp_do_lower1 = 1;
  __ompc_static_init_4(__ompv_gtid_s1, 2, &__ompv_temp_do_lower1, &__ompv_temp_do_upper1, &__ompv_temp_do_stride1, 1, 1);
  __ompv_temp_do_upper1 = (__ompv_temp_do_upper1 > preg102 ? preg102 : __ompv_temp_do_upper1);
  j1 = __ompv_temp_do_lower1;
  if(!(__ompv_temp_do_upper1 >= __ompv_temp_do_lower1))
    goto _144;
  preg112 = (__ompv_temp_do_upper1 - __ompv_temp_do_lower1) + 1;
  preg56 = __ompv_temp_do_lower1;
  preg58 = __ompv_temp_do_upper1 + 1;
  preg92 = (_UINT64)(preg56) * (_UINT64)(4);
  preg94 = (_UINT64)(preg56) * (_UINT64)(8);
  preg126 = (_UINT32)(preg92);
  preg128 = (_UINT32)(preg94);
  preg37 = (_UINT32)((_INT32 *)((_UINT8 *)(rowstr0) + preg126));
  preg39 = (_UINT32)((_IEEE64 *)((_UINT8 *)(w0) + preg128));
  j1 = j1;
  _85 :;
  sum0 = 0.0;
  preg99 = *((_INT32 *) preg37);
  preg67 = *(((_INT32 *) preg37) + 1LL);
  if(!(preg67 > preg99))
    goto _87;
  k0 = 0;
  preg78 = preg67 - preg99;
  preg114 = preg78;
  preg96 = (_INT64)(preg99);
  preg0 = (_UINT64)(preg96) * (_UINT64)(8);
  preg2 = (_UINT64)(preg96) * (_UINT64)(4);
  preg41 = (_UINT32)(preg0);
  preg43 = (_UINT32)(preg2);
  preg68 = (_UINT32)((_IEEE64 *)((_UINT8 *)(a0) + preg41));
  preg70 = (_UINT32)((_INT32 *)((_UINT8 *)(colidx0) + preg43));
  k0 = k0;
  _90 :;
  sum0 = sum0 + (*((_IEEE64 *) preg68) ** (_IEEE64 *)((_UINT8 *)(p0) + (_UINT32)(((_UINT64)(*((_INT32 *) preg70)) * (_UINT64)(8)))));
  k0 = k0 + 1;
  preg70 = preg70 + (_UINT32)(4);
  preg68 = preg68 + (_UINT32)(8);
  if(k0 != preg78)
    goto _90;
  goto _86;
  _87 :;
  _86 :;
  * ((_IEEE64 *) preg39) = sum0;
  j1 = j1 + 1;
  preg39 = preg39 + (_UINT32)(8);
  preg37 = preg37 + (_UINT32)(4);
  if(j1 != preg58)
    goto _85;
  goto _83;
  _144 :;
  _83 :;
  __ompc_barrier();
  __ompv_gtid_s1 = __ompc_get_local_thread_num();
  preg16 = lastcol - firstcol;
  preg97 = preg16 + 1;
  __ompv_temp_do_upper2 = preg97;
  __ompv_temp_do_lower2 = 1;
  __ompc_static_init_4(__ompv_gtid_s1, 2, &__ompv_temp_do_lower2, &__ompv_temp_do_upper2, &__ompv_temp_do_stride2, 1, 1);
  __ompv_temp_do_upper2 = (__ompv_temp_do_upper2 > preg97 ? preg97 : __ompv_temp_do_upper2);
  j2 = __ompv_temp_do_lower2;
  if(!(__ompv_temp_do_upper2 >= __ompv_temp_do_lower2))
    goto _145;
  preg116 = (__ompv_temp_do_upper2 - __ompv_temp_do_lower2) + 1;
  preg52 = __ompv_temp_do_lower2;
  preg54 = __ompv_temp_do_upper2 + 1;
  preg90 = (_UINT64)(preg52) * (_UINT64)(8);
  preg124 = (_UINT32)(preg90);
  preg33 = (_UINT32)((_IEEE64 *)((_UINT8 *)(w0) + preg124));
  preg35 = (_UINT32)((_IEEE64 *)((_UINT8 *)(q0) + preg124));
  j2 = j2;
  _95 :;
  * ((_IEEE64 *) preg35) = *((_IEEE64 *) preg33);
  j2 = j2 + 1;
  preg35 = preg35 + (_UINT32)(8);
  preg33 = preg33 + (_UINT32)(8);
  if(j2 != preg54)
    goto _95;
  goto _93;
  _145 :;
  _93 :;
  __ompc_barrier();
  __ompv_gtid_s1 = __ompc_get_local_thread_num();
  preg16 = lastcol - firstcol;
  preg95 = preg16 + 1;
  __ompv_temp_do_upper3 = preg95;
  __ompv_temp_do_lower3 = 1;
  __ompc_static_init_4(__ompv_gtid_s1, 2, &__ompv_temp_do_lower3, &__ompv_temp_do_upper3, &__ompv_temp_do_stride3, 1, 1);
  __ompv_temp_do_upper3 = (__ompv_temp_do_upper3 > preg95 ? preg95 : __ompv_temp_do_upper3);
  j3 = __ompv_temp_do_lower3;
  if(!(__ompv_temp_do_upper3 >= __ompv_temp_do_lower3))
    goto _146;
  preg118 = (__ompv_temp_do_upper3 - __ompv_temp_do_lower3) + 1;
  preg48 = __ompv_temp_do_lower3;
  preg50 = __ompv_temp_do_upper3 + 1;
  preg88 = (_UINT64)(preg48) * (_UINT64)(8);
  preg122 = (_UINT32)(preg88);
  preg31 = (_UINT32)((_IEEE64 *)((_UINT8 *)(w0) + preg122));
  j3 = j3;
  _99 :;
  * ((_IEEE64 *) preg31) = 0.0;
  j3 = j3 + 1;
  preg31 = preg31 + (_UINT32)(8);
  if(j3 != preg50)
    goto _99;
  goto _97;
  _146 :;
  _97 :;
  __ompv_gtid_s1 = __ompc_get_local_thread_num();
  __mplocal_d = 0.0;
  preg16 = lastcol - firstcol;
  preg93 = preg16 + 1;
  __ompv_temp_do_upper4 = preg93;
  __ompv_temp_do_lower4 = 1;
  __ompc_static_init_4(__ompv_gtid_s1, 2, &__ompv_temp_do_lower4, &__ompv_temp_do_upper4, &__ompv_temp_do_stride4, 1, 1);
  __ompv_temp_do_upper4 = (__ompv_temp_do_upper4 > preg93 ? preg93 : __ompv_temp_do_upper4);
  j4 = __ompv_temp_do_lower4;
  if(!(__ompv_temp_do_upper4 >= __ompv_temp_do_lower4))
    goto _147;
  preg120 = (__ompv_temp_do_upper4 - __ompv_temp_do_lower4) + 1;
  preg44 = __ompv_temp_do_lower4;
  preg46 = __ompv_temp_do_upper4 + 1;
  preg86 = (_UINT64)(preg44) * (_UINT64)(8);
  preg119 = (_UINT32)(preg86);
  preg28 = (_UINT32)((_IEEE64 *)((_UINT8 *)(p0) + preg119));
  preg30 = (_UINT32)((_IEEE64 *)((_UINT8 *)(q0) + preg119));
  j4 = j4;
  _103 :;
  __mplocal_d = __mplocal_d + (*((_IEEE64 *) preg28) ** ((_IEEE64 *) preg30));
  j4 = j4 + 1;
  preg30 = preg30 + (_UINT32)(8);
  preg28 = preg28 + (_UINT32)(8);
  if(j4 != preg46)
    goto _103;
  goto _101;
  _147 :;
  _101 :;
  __ompc_barrier();
  __ompc_critical(__ompv_gtid_s1, (_UINT32) & __mplock_2);
  d = d + __mplocal_d;
  __ompc_end_critical(__ompv_gtid_s1, (_UINT32) & __mplock_2);
  __ompc_barrier();
  __ompv_gtid_s1 = __ompc_get_local_thread_num();
  mpsp_status = __ompc_single(__ompv_gtid_s1);
  if(!(mpsp_status == 1))
    goto _106;
  alpha = rho0 / d;
  goto _105;
  _106 :;
  _105 :;
  __ompc_end_single(__ompv_gtid_s1);
  __ompc_barrier();
  __ompv_gtid_s1 = __ompc_get_local_thread_num();
  preg16 = lastcol - firstcol;
  preg91 = preg16 + 1;
  __ompv_temp_do_upper5 = preg91;
  __ompv_temp_do_lower5 = 1;
  __ompc_static_init_4(__ompv_gtid_s1, 2, &__ompv_temp_do_lower5, &__ompv_temp_do_upper5, &__ompv_temp_do_stride5, 1, 1);
  __ompv_temp_do_upper5 = (__ompv_temp_do_upper5 > preg91 ? preg91 : __ompv_temp_do_upper5);
  j5 = __ompv_temp_do_lower5;
  if(!(__ompv_temp_do_upper5 >= __ompv_temp_do_lower5))
    goto _148;
  preg121 = (__ompv_temp_do_upper5 - __ompv_temp_do_lower5) + 1;
  preg40 = __ompv_temp_do_lower5;
  preg42 = __ompv_temp_do_upper5 + 1;
  preg84 = (_UINT64)(preg40) * (_UINT64)(8);
  preg117 = (_UINT32)(preg84);
  preg20 = (_UINT32)((_IEEE64 *)((_UINT8 *)(z0) + preg117));
  preg22 = (_UINT32)((_IEEE64 *)((_UINT8 *)(p0) + preg117));
  preg24 = (_UINT32)((_IEEE64 *)((_UINT8 *)(r0) + preg117));
  preg26 = (_UINT32)((_IEEE64 *)((_UINT8 *)(q0) + preg117));
  j5 = j5;
  _109 :;
  * ((_IEEE64 *) preg20) = *((_IEEE64 *) preg20) + (*((_IEEE64 *) preg22) * alpha);
  * ((_IEEE64 *) preg24) = *((_IEEE64 *) preg24) - (*((_IEEE64 *) preg26) * alpha);
  j5 = j5 + 1;
  preg26 = preg26 + (_UINT32)(8);
  preg24 = preg24 + (_UINT32)(8);
  preg22 = preg22 + (_UINT32)(8);
  preg20 = preg20 + (_UINT32)(8);
  if(j5 != preg42)
    goto _109;
  goto _107;
  _148 :;
  _107 :;
  __ompc_barrier();
  __ompv_gtid_s1 = __ompc_get_local_thread_num();
  __mplocal_rho0 = 0.0;
  preg16 = lastcol - firstcol;
  preg89 = preg16 + 1;
  __ompv_temp_do_upper6 = preg89;
  __ompv_temp_do_lower6 = 1;
  __ompc_static_init_4(__ompv_gtid_s1, 2, &__ompv_temp_do_lower6, &__ompv_temp_do_upper6, &__ompv_temp_do_stride6, 1, 1);
  __ompv_temp_do_upper6 = (__ompv_temp_do_upper6 > preg89 ? preg89 : __ompv_temp_do_upper6);
  j6 = __ompv_temp_do_lower6;
  if(!(__ompv_temp_do_upper6 >= __ompv_temp_do_lower6))
    goto _149;
  preg123 = (__ompv_temp_do_upper6 - __ompv_temp_do_lower6) + 1;
  preg36 = __ompv_temp_do_lower6;
  preg38 = __ompv_temp_do_upper6 + 1;
  preg82 = (_UINT64)(preg36) * (_UINT64)(8);
  preg115 = (_UINT32)(preg82);
  preg17 = (_UINT32)((_IEEE64 *)((_UINT8 *)(r0) + preg115));
  j6 = j6;
  _113 :;
  preg64 = *((_IEEE64 *) preg17);
  __mplocal_rho0 = __mplocal_rho0 + (preg64 * preg64);
  j6 = j6 + 1;
  preg17 = preg17 + (_UINT32)(8);
  if(j6 != preg38)
    goto _113;
  goto _111;
  _149 :;
  _111 :;
  __ompc_barrier();
  __ompc_critical(__ompv_gtid_s1, (_UINT32) & __mplock_3);
  rho = rho + __mplocal_rho0;
  __ompc_end_critical(__ompv_gtid_s1, (_UINT32) & __mplock_3);
  __ompc_barrier();
  __ompv_gtid_s1 = __ompc_get_local_thread_num();
  mpsp_status0 = __ompc_single(__ompv_gtid_s1);
  if(!(mpsp_status0 == 1))
    goto _116;
  beta = rho / rho0;
  goto _115;
  _116 :;
  _115 :;
  __ompc_end_single(__ompv_gtid_s1);
  __ompc_barrier();
  __ompv_gtid_s1 = __ompc_get_local_thread_num();
  preg16 = lastcol - firstcol;
  preg87 = preg16 + 1;
  __ompv_temp_do_upper7 = preg87;
  __ompv_temp_do_lower7 = 1;
  __ompc_static_init_4(__ompv_gtid_s1, 2, &__ompv_temp_do_lower7, &__ompv_temp_do_upper7, &__ompv_temp_do_stride7, 1, 1);
  __ompv_temp_do_upper7 = (__ompv_temp_do_upper7 > preg87 ? preg87 : __ompv_temp_do_upper7);
  j7 = __ompv_temp_do_lower7;
  if(!(__ompv_temp_do_upper7 >= __ompv_temp_do_lower7))
    goto _150;
  preg125 = (__ompv_temp_do_upper7 - __ompv_temp_do_lower7) + 1;
  preg32 = __ompv_temp_do_lower7;
  preg34 = __ompv_temp_do_upper7 + 1;
  preg80 = (_UINT64)(preg32) * (_UINT64)(8);
  preg113 = (_UINT32)(preg80);
  preg14 = (_UINT32)((_IEEE64 *)((_UINT8 *)(r0) + preg113));
  preg15 = (_UINT32)((_IEEE64 *)((_UINT8 *)(p0) + preg113));
  j7 = j7;
  _119 :;
  * ((_IEEE64 *) preg15) = *((_IEEE64 *) preg14) + (*((_IEEE64 *) preg15) * beta);
  j7 = j7 + 1;
  preg15 = preg15 + (_UINT32)(8);
  preg14 = preg14 + (_UINT32)(8);
  if(j7 != preg34)
    goto _119;
  goto _117;
  _150 :;
  _117 :;
  __ompc_barrier();
  cgit = cgit + 1;
  if(cgit != 26)
    goto _80;
  __ompv_gtid_s1 = __ompc_get_local_thread_num();
  mpsp_status1 = __ompc_single(__ompv_gtid_s1);
  sum = (mpsp_status1 == 1 ? 0.0 : sum);
  __ompc_end_single(__ompv_gtid_s1);
  __ompv_gtid_s1 = __ompc_get_local_thread_num();
  preg18 = lastrow - firstrow;
  preg85 = preg18 + 1;
  __ompv_temp_do_upper8 = preg85;
  __ompv_temp_do_lower8 = 1;
  __ompc_static_init_4(__ompv_gtid_s1, 2, &__ompv_temp_do_lower8, &__ompv_temp_do_upper8, &__ompv_temp_do_stride8, 1, 1);
  __ompv_temp_do_upper8 = (__ompv_temp_do_upper8 > preg85 ? preg85 : __ompv_temp_do_upper8);
  j8 = __ompv_temp_do_lower8;
  if(!(__ompv_temp_do_upper8 >= __ompv_temp_do_lower8))
    goto _151;
  preg127 = (__ompv_temp_do_upper8 - __ompv_temp_do_lower8) + 1;
  preg27 = __ompv_temp_do_lower8;
  preg29 = __ompv_temp_do_upper8 + 1;
  preg73 = (_UINT64)(preg27) * (_UINT64)(4);
  preg76 = (_UINT64)(preg27) * (_UINT64)(8);
  preg105 = (_UINT32)(preg73);
  preg107 = (_UINT32)(preg76);
  preg10 = (_UINT32)((_INT32 *)((_UINT8 *)(rowstr0) + preg105));
  preg11 = (_UINT32)((_IEEE64 *)((_UINT8 *)(w0) + preg107));
  j8 = j8;
  _124 :;
  __mplocal_d0 = 0.0;
  preg83 = *((_INT32 *) preg10);
  preg59 = *(((_INT32 *) preg10) + 1LL);
  if(!(preg59 > preg83))
    goto _126;
  k1 = 0;
  preg75 = preg59 - preg83;
  preg = preg75;
  preg77 = (_INT64)(preg83);
  preg109 = (_UINT64)(preg77) * (_UINT64)(4);
  preg111 = (_UINT64)(preg77) * (_UINT64)(8);
  preg12 = (_UINT32)(preg109);
  preg13 = (_UINT32)(preg111);
  preg60 = (_UINT32)((_INT32 *)((_UINT8 *)(colidx0) + preg12));
  preg62 = (_UINT32)((_IEEE64 *)((_UINT8 *)(a0) + preg13));
  k1 = k1;
  _129 :;
  __mplocal_d0 = __mplocal_d0 + (*((_IEEE64 *) preg62) ** (_IEEE64 *)((_UINT8 *)(z0) + (_UINT32)(((_UINT64)(*((_INT32 *) preg60)) * (_UINT64)(8)))));
  k1 = k1 + 1;
  preg62 = preg62 + (_UINT32)(8);
  preg60 = preg60 + (_UINT32)(4);
  if(k1 != preg75)
    goto _129;
  goto _125;
  _126 :;
  _125 :;
  * ((_IEEE64 *) preg11) = __mplocal_d0;
  j8 = j8 + 1;
  preg11 = preg11 + (_UINT32)(8);
  preg10 = preg10 + (_UINT32)(4);
  if(j8 != preg29)
    goto _124;
  goto _122;
  _151 :;
  _122 :;
  __ompc_barrier();
  __ompv_gtid_s1 = __ompc_get_local_thread_num();
  preg16 = lastcol - firstcol;
  preg81 = preg16 + 1;
  __ompv_temp_do_upper9 = preg81;
  __ompv_temp_do_lower9 = 1;
  __ompc_static_init_4(__ompv_gtid_s1, 2, &__ompv_temp_do_lower9, &__ompv_temp_do_upper9, &__ompv_temp_do_stride9, 1, 1);
  __ompv_temp_do_upper9 = (__ompv_temp_do_upper9 > preg81 ? preg81 : __ompv_temp_do_upper9);
  j9 = __ompv_temp_do_lower9;
  if(!(__ompv_temp_do_upper9 >= __ompv_temp_do_lower9))
    goto _152;
  preg1 = (__ompv_temp_do_upper9 - __ompv_temp_do_lower9) + 1;
  preg23 = __ompv_temp_do_lower9;
  preg25 = __ompv_temp_do_upper9 + 1;
  preg71 = (_UINT64)(preg23) * (_UINT64)(8);
  preg103 = (_UINT32)(preg71);
  preg8 = (_UINT32)((_IEEE64 *)((_UINT8 *)(w0) + preg103));
  preg9 = (_UINT32)((_IEEE64 *)((_UINT8 *)(r0) + preg103));
  j9 = j9;
  _134 :;
  * ((_IEEE64 *) preg9) = *((_IEEE64 *) preg8);
  j9 = j9 + 1;
  preg9 = preg9 + (_UINT32)(8);
  preg8 = preg8 + (_UINT32)(8);
  if(j9 != preg25)
    goto _134;
  goto _132;
  _152 :;
  _132 :;
  __ompc_barrier();
  __ompv_gtid_s1 = __ompc_get_local_thread_num();
  __mplocal_sum0 = 0.0;
  preg16 = lastcol - firstcol;
  preg79 = preg16 + 1;
  __ompv_temp_do_upper10 = preg79;
  __ompv_temp_do_lower10 = 1;
  __ompc_static_init_4(__ompv_gtid_s1, 2, &__ompv_temp_do_lower10, &__ompv_temp_do_upper10, &__ompv_temp_do_stride10, 1, 1);
  __ompv_temp_do_upper10 = (__ompv_temp_do_upper10 > preg79 ? preg79 : __ompv_temp_do_upper10);
  j10 = __ompv_temp_do_lower10;
  if(!(__ompv_temp_do_upper10 >= __ompv_temp_do_lower10))
    goto _153;
  preg3 = (__ompv_temp_do_upper10 - __ompv_temp_do_lower10) + 1;
  preg19 = __ompv_temp_do_lower10;
  preg21 = __ompv_temp_do_upper10 + 1;
  preg69 = (_UINT64)(preg19) * (_UINT64)(8);
  preg101 = (_UINT32)(preg69);
  preg6 = (_UINT32)((_IEEE64 *)((_UINT8 *)(x0) + preg101));
  preg7 = (_UINT32)((_IEEE64 *)((_UINT8 *)(r0) + preg101));
  j10 = j10;
  _138 :;
  __mplocal_d1 = *((_IEEE64 *) preg6) - *((_IEEE64 *) preg7);
  __mplocal_sum0 = __mplocal_sum0 + (__mplocal_d1 * __mplocal_d1);
  j10 = j10 + 1;
  preg7 = preg7 + (_UINT32)(8);
  preg6 = preg6 + (_UINT32)(8);
  if(j10 != preg21)
    goto _138;
  goto _136;
  _153 :;
  _136 :;
  __ompc_barrier();
  __ompc_critical(__ompv_gtid_s1, (_UINT32) & __mplock_4);
  sum = sum + __mplocal_sum0;
  __ompc_end_critical(__ompv_gtid_s1, (_UINT32) & __mplock_4);
  __ompc_barrier();
  __ompv_gtid_s1 = __ompc_get_local_thread_num();
  mpsp_status2 = __ompc_single(__ompv_gtid_s1);
  if(!(mpsp_status2 == 1))
    goto _141;
  __comma = sqrt(sum);
  * rnorm = __comma;
  goto _140;
  _141 :;
  _140 :;
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
  register _INT32 __ompv_ok_to_fork;
  register _BOOLEAN preg96;
  register _UINT32 reg7;
  register _INT32 preg20;
  register _INT32 preg100;
  register _INT64 preg54;
  register _UINT64 preg84;
  register _UINT32 preg25;
  register _UINT32 preg69;
  register _INT32 preg15;
  register _INT32 iouter0;
  register _INT32 preg22;
  register _INT64 preg14;
  register _INT32 preg19;
  register _BOOLEAN preg41;
  register _INT32 preg48;
  register _UINT64 preg11;
  register _UINT32 preg8;
  register _UINT32 preg35;
  register _UINT32 preg62;
  register _UINT32 preg64;
  register _UINT32 preg66;
  register _UINT32 preg18;
  register _UINT32 preg6;
  register _UINT32 preg5;
  register _INT32 k1;
  register _INT32 preg24;
  register _UINT32 preg97;
  register _UINT32 preg99;
  register _INT64 preg45;
  register _INT32 ivelt2;
  register _INT32 preg27;
  register _UINT32 preg88;
  register _UINT32 preg89;
  register _INT32 ivelt0;
  register _INT32 preg29;
  register _UINT32 preg91;
  register _UINT32 preg94;
  register _INT32 preg17;
  register _INT32 preg4;
  register _INT32 i3;
  register _INT32 preg3;
  register _INT32 preg32;
  register _INT32 preg34;
  register _INT64 preg38;
  register _INT32 preg75;
  register _UINT64 preg77;
  register _UINT64 preg79;
  register _UINT32 preg10;
  register _UINT32 preg13;
  register _INT32 __ompv_ok_to_fork0;
  register _INT32 preg33;
  register _INT32 preg102;
  register _UINT32 preg26;
  register _UINT32 preg28;
  register _BOOLEAN preg1;
  register _INT32 nza0;
  register _INT32 preg37;
  register _UINT32 preg87;
  register _INT32 j3;
  register _INT64 preg31;
  register _UINT64 preg73;
  register _UINT32 preg7;
  register _UINT32 preg60;
  register _BOOLEAN preg58;
  register _INT32 j0;
  register _INT32 preg40;
  register _UINT32 preg86;
  register _INT32 nza1;
  register _INT32 preg43;
  register _UINT32 preg82;
  register _UINT32 preg83;
  register _UINT32 preg85;
  register _INT32 j4;
  register _INT64 preg21;
  register _UINT64 preg68;
  register _UINT32 preg2;
  register _UINT32 preg59;
  register _INT64 preg23;
  register _INT32 j1;
  register _INT32 preg44;
  register _INT64 preg36;
  register _UINT64 preg71;
  register _UINT32 preg101;
  register _UINT32 preg0;
  register _INT32 __ompv_ok_to_fork1;
  register _INT32 preg47;
  register _UINT32 preg39;
  register _UINT32 preg42;
  register _INT32 j2;
  register _INT32 preg50;
  register _UINT32 preg30;
  register _UINT32 preg56;
  register _UINT32 preg57;
  register _UINT32 preg72;
  register _UINT32 preg81;
  register _INT32 preg;
  register _INT32 preg9;
  register _INT64 preg61;
  register _UINT64 preg90;
  register _UINT64 preg92;
  register _UINT32 preg46;
  register _UINT32 preg49;
  register _UINT32 preg70;
  register _UINT32 preg74;
  register _UINT32 preg76;
  register _INT32 i4;
  register _INT64 preg12;
  register _UINT64 preg63;
  register _UINT32 preg93;
  register _UINT32 preg52;
  register _IEEE64 preg78;
  register _UINT64 preg65;
  register _UINT32 preg95;
  register _UINT32 preg53;
  register _INT32 k2;
  register _INT32 preg51;
  register _UINT32 preg80;
  register _INT32 i5;
  register _INT64 preg16;
  register _UINT64 preg67;
  register _UINT32 preg98;
  register _UINT32 preg55;
  _INT32 i;
  _INT32 nnza;
  _INT32 irow;
  _INT32 nzv;
  _IEEE64 size;
  _IEEE64 ratio;
  _IEEE64 scale;
  _INT32 jcol;
  _INT32 i0;
  _INT32 j;
  _INT32 jajp1;
  _INT32 nza;
  _INT32 k;
  _INT32 nzrow;
  _IEEE64 xi;
  _INT32 * rowstr2;
  _INT32 n1;
  _IEEE64 * x1;
  _INT32 * mark0;
  _INT32 set;
  
  /*Begin_of_nested_PU(s)*/
static void __ompdo_makea3(__ompv_gtid_a, __ompv_slink_a)
  _INT32 __ompv_gtid_a;
  _UINT32 __ompv_slink_a;
{
  register _INT32 preg0;
  register _INT64 preg1;
  register _UINT64 preg2;
  register _UINT64 preg3;
  register _UINT32 preg4;
  register _UINT32 preg5;
  register _UINT32 preg6;
  register _UINT32 preg;
  _INT32 __mplocal_i;
  _INT32 __ompv_temp_do_upper;
  _INT32 __ompv_temp_do_lower;
  _INT32 __ompv_temp_do_stride;
  
  preg0 = n;
  __ompv_temp_do_upper = preg0;
  __ompv_temp_do_lower = 1;
  __ompc_static_init_4(__ompv_gtid_a, 2, &__ompv_temp_do_lower, &__ompv_temp_do_upper, &__ompv_temp_do_stride, 1, 1);
  __ompv_temp_do_upper = (__ompv_temp_do_upper > preg0 ? preg0 : __ompv_temp_do_upper);
  __mplocal_i = __ompv_temp_do_lower;
  if(!(__ompv_temp_do_upper >= __ompv_temp_do_lower))
    goto _5;
  preg1 = __ompv_temp_do_lower;
  preg2 = (_UINT64)(preg1) * (_UINT64)(8);
  preg3 = (_UINT64)(preg1) * (_UINT64)(4);
  preg4 = (_UINT32)(preg2);
  preg5 = (_UINT32)(preg3);
  preg6 = (_UINT32)((_IEEE64 *)((_UINT8 *)(x1) + preg4));
  //Liao, bug,preg6 = (_UINT32)((_IEEE64 *)((_UINT8 *)(x0) + preg4));
  preg = (_UINT32)((_INT32 *)((_UINT8 *)(mark0) + preg5));
  //preg = (_UINT32)((_INT32 *)((_UINT8 *)(mark) + preg5));
  __mplocal_i = __mplocal_i;
  _3 :;
  * ((_IEEE64 *) preg6) = 0.0;
  * ((_INT32 *) preg) = 0;
  __mplocal_i = (_INT32)((_UINT64) __mplocal_i) + 1;
  preg = preg + (_UINT32)(4);
  preg6 = preg6 + (_UINT32)(8);
  if(__mplocal_i <= __ompv_temp_do_upper)
    goto _3;
  goto _1;
  _5 :;
  _1 :;
  return;
} /* __ompdo_makea3 */
static void __ompdo_makea2(__ompv_gtid_a, __ompv_slink_a)
  _INT32 __ompv_gtid_a;
  _UINT32 __ompv_slink_a;
{
  register _INT32 preg4;
  register _INT64 preg;
  register _UINT64 preg0;
  register _UINT32 preg1;
  register _UINT32 preg2;
  register _UINT32 preg3;
  _INT32 __mplocal_j;
  _INT32 __ompv_temp_do_upper;
  _INT32 __ompv_temp_do_lower;
  _INT32 __ompv_temp_do_stride;
  
  preg4 = n;
  __ompv_temp_do_upper = preg4;
  __ompv_temp_do_lower = 1;
  __ompc_static_init_4(__ompv_gtid_a, 2, &__ompv_temp_do_lower, &__ompv_temp_do_upper, &__ompv_temp_do_stride, 1, 1);
  __ompv_temp_do_upper = (__ompv_temp_do_upper > preg4 ? preg4 : __ompv_temp_do_upper);
  __mplocal_j = __ompv_temp_do_lower;
  if(!(__ompv_temp_do_upper >= __ompv_temp_do_lower))
    goto _5;
  preg = __ompv_temp_do_lower;
  preg0 = (_UINT64)(preg) * (_UINT64)(4);
  preg1 = (_UINT32)(preg0);
  preg2 = (_UINT32)((_INT32 *)((_UINT8 *)(rowstr0) + preg1));
  preg3 = (_UINT32)((_INT32 *)((_UINT8 *)(mark0) + preg1));
  //preg3 = (_UINT32)((_INT32 *)((_UINT8 *)(mark) + preg1));
  __mplocal_j = __mplocal_j;
  _3 :;
  * ((_INT32 *) preg2) = 0;
  * ((_INT32 *) preg3) = 0;
  __mplocal_j = (_INT32)((_UINT64) __mplocal_j) + 1;
  preg3 = preg3 + (_UINT32)(4);
  preg2 = preg2 + (_UINT32)(4);
  if(__mplocal_j <= __ompv_temp_do_upper)
    goto _3;
  goto _1;
  _5 :;
  _1 :;
  return;
} /* __ompdo_makea2 */
static void __ompdo_makea1(__ompv_gtid_a, __ompv_slink_a)
  _INT32 __ompv_gtid_a;
  _UINT32 __ompv_slink_a;
{
  register _INT32 preg;
  register _INT64 preg0;
  register _UINT64 preg1;
  register _UINT32 preg2;
  register _UINT32 preg3;
  _INT32 __mplocal_i;
  _INT32 __ompv_temp_do_upper;
  _INT32 __ompv_temp_do_lower;
  _INT32 __ompv_temp_do_stride;
  
  __ompv_temp_do_upper = n;
  __ompv_temp_do_lower = 1;
  __ompc_static_init_4(__ompv_gtid_a, 2, &__ompv_temp_do_lower, &__ompv_temp_do_upper, &__ompv_temp_do_stride, 1, 1);
  __ompv_temp_do_upper = (n < __ompv_temp_do_upper ? n : __ompv_temp_do_upper);
  __mplocal_i = __ompv_temp_do_lower;
  if(!(__ompv_temp_do_upper >= __ompv_temp_do_lower))
    goto _5;
  preg = n + __ompv_temp_do_lower;
  preg0 = (_INT64)(preg);
  preg1 = (_UINT64)(preg0) * (_UINT64)(4);
  preg2 = (_UINT32)(preg1);
  preg3 = (_UINT32)((_INT32 *)((_UINT8 *)(colidx0) + preg2));
  __mplocal_i = __mplocal_i;
  _3 :;
  * ((_INT32 *) preg3) = 0;
  __mplocal_i = __mplocal_i + 1;
  preg3 = preg3 + (_UINT32)(4);
  if(__mplocal_i <= __ompv_temp_do_upper)
    goto _3;
  goto _1;
  _5 :;
  _1 :;
  return;
} /* __ompdo_makea1 */
  
  size = 1.0;
  ratio = pow(rcond, _F8RECIP((_IEEE64)(n)));
  nnza = 0;
  __ompv_ok_to_fork = __ompc_can_fork();
  preg96 = n > 1;
  if(!(preg96))
    goto _174;
  if(!(__ompv_ok_to_fork == 1))
    goto _175;
  __ompc_fork(0, &__ompdo_makea1, reg7);
  preg96 = n > 1;
  goto _64;
  _174 :;
  goto _65;
  _175 :;
  _65 :;
  __ompc_get_local_thread_num();
  __ompc_serialized_parallel();
  i = 1;
  if(!(n >= 1))
    goto _176;
  preg20 = n;
  preg100 = n + 1;
  preg54 = (_INT64)(preg100);
  preg84 = (_UINT64)(preg54) * (_UINT64)(4);
  preg25 = (_UINT32)(preg84);
  preg69 = (_UINT32)((_INT32 *)((_UINT8 *)(colidx0) + preg25));
  i = i;
  _83 :;
  * ((_INT32 *) preg69) = 0;
  i = i + 1;
  preg69 = preg69 + (_UINT32)(4);
  if(i != preg100)
    goto _83;
  goto _81;
  _176 :;
  _81 :;
  __ompc_end_serialized_parallel();
  _64 :;
  preg15 = n;
  if(!(preg15 > 0))
    goto _86;
  iouter0 = 0;
  preg22 = preg15;
  preg14 = (_INT64)(preg15);
  preg19 = nonzer;
  preg41 = preg19 > 0;
  preg48 = 1;
  preg11 = (_UINT64)(preg14) * (_UINT64)(4);
  preg8 = (_UINT32)(preg11);
  preg35 = (_UINT32)((_INT32 *)((_UINT8 *)(colidx0) + preg8));
  preg62 = (_UINT32)(acol0);
  preg64 = (_UINT32)(arow0);
  preg66 = (_UINT32)(aelt0);
  preg18 = (_UINT32)(iv0);
  preg6 = preg18 + (_UINT32)(4);
  preg5 = (_UINT32)(v0 + 1);
  iouter0 = iouter0;
  _89 :;
  nzv = preg19;
  sprnvc(preg15, preg19, v0, (_INT32 *) preg18, colidx0, (_INT32 *) preg35);
  set = 0;
  if(!(preg41))
    goto _91;
  k1 = 0;
  preg24 = preg19;
  preg97 = preg6;
  preg99 = preg5;
  k1 = k1;
  _94 :;
  if(!(*((_INT32 *) preg97) == preg48))
    goto _96;
  * ((_IEEE64 *) preg99) = 5.0e-01;
  set = 1;
  goto _95;
  _96 :;
  _95 :;
  k1 = k1 + 1;
  preg99 = preg99 + (_UINT32)(8);
  preg97 = preg97 + (_UINT32)(4);
  if(k1 != preg19)
    goto _94;
  if(!(set == 0))
    goto _99;
  _171 :;
  nzv = preg19 + 1;
  preg45 = nzv;
  * (_IEEE64 *)((_UINT8 *)(v0) + (_UINT32)(((_UINT64)(preg45) * (_UINT64)(8)))) = 5.0e-01;
  * ((_INT32 *)(preg18 + (_UINT32)(((_UINT64)(preg45) * (_UINT64)(4))))) = preg48;
  goto _98;
  _99 :;
  goto _98;
  _91 :;
  goto _171;
  _98 :;
  if(!(nzv > 0))
    goto _101;
  ivelt2 = 0;
  preg27 = nzv;
  preg88 = preg6;
  preg89 = preg5;
  ivelt2 = ivelt2;
  _104 :;
  jcol = *((_INT32 *) preg88);
  if(!(firstcol0 <= jcol))
    goto _177;
  if(!(lastcol0 >= jcol))
    goto _178;
  scale = *((_IEEE64 *) preg89) * size;
  ivelt0 = 0;
  preg29 = nzv;
  preg91 = preg6;
  preg94 = preg5;
  ivelt0 = ivelt0;
  _107 :;
  irow = *((_INT32 *) preg91);
  if(!(firstrow0 <= irow))
    goto _179;
  if(!(lastrow0 >= irow))
    goto _180;
  nnza = nnza + 1;
  preg66 = preg66 + (_UINT32)(8);
  preg64 = preg64 + (_UINT32)(4);
  preg62 = preg62 + (_UINT32)(4);
  if(!(nz < nnza))
    goto _109;
  printf((_INT8 *) "Space for matrix elements exceeded in makea\n");
  printf((_INT8 *) "nnza, nzmax = %d, %d\n", nnza, nz);
  printf((_INT8 *) "iouter = %d\n", preg48);
  exit(1);
  goto _108;
  _109 :;
  _108 :;
  * ((_INT32 *) preg62) = jcol;
  * ((_INT32 *) preg64) = irow;
  * ((_IEEE64 *) preg66) = *((_IEEE64 *) preg94) * scale;
  goto _69;
  _179 :;
  goto _69;
  _180 :;
  _69 :;
  ivelt0 = ivelt0 + 1;
  preg94 = preg94 + (_UINT32)(8);
  preg91 = preg91 + (_UINT32)(4);
  if(nzv != ivelt0)
    goto _107;
  goto _67;
  _177 :;
  goto _67;
  _178 :;
  _67 :;
  ivelt2 = ivelt2 + 1;
  preg89 = preg89 + (_UINT32)(8);
  preg88 = preg88 + (_UINT32)(4);
  if(nzv != ivelt2)
    goto _104;
  goto _100;
  _101 :;
  _100 :;
  size = size * ratio;
  iouter0 = iouter0 + 1;
  preg48 = preg48 + 1;
  if(iouter0 != preg15)
    goto _89;
  goto _85;
  _86 :;
  preg14 = (_INT64)(preg15);
  preg11 = (_UINT64)(preg14) * (_UINT64)(4);
  preg8 = (_UINT32)(preg11);
  _85 :;
  preg17 = firstrow0;
  preg4 = lastrow0 - preg17;
  if(!(lastrow0 >= preg17))
    goto _114;
  i3 = 0;
  preg3 = preg4 + 1;
  preg32 = preg3;
  preg34 = preg17;
  preg38 = nnza;
  preg75 = preg15 + preg17;
  preg77 = (_UINT64)(preg38) * (_UINT64)(4);
  preg79 = (_UINT64)(preg38) * (_UINT64)(8);
  preg10 = (_UINT32)(preg77);
  preg13 = (_UINT32)(preg79);
  preg62 = (_UINT32)((_INT32 *)((_UINT8 *)(acol0) + preg10));
  preg64 = (_UINT32)((_INT32 *)((_UINT8 *)(arow0) + preg10));
  preg66 = (_UINT32)((_IEEE64 *)((_UINT8 *)(aelt0) + preg13));
  i3 = i3;
  _117 :;
  if(!(firstcol0 <= preg34))
    goto _181;
  if(!(lastcol0 >= preg34))
    goto _182;
  nnza = nnza + 1;
  preg66 = preg66 + (_UINT32)(8);
  preg64 = preg64 + (_UINT32)(4);
  preg62 = preg62 + (_UINT32)(4);
  if(!(nz < nnza))
    goto _119;
  printf((_INT8 *) "Space for matrix elements exceeded in makea\n");
  printf((_INT8 *) "nnza, nzmax = %d, %d\n", nnza, nz);
  printf((_INT8 *) "iouter = %d\n", preg75);
  exit(1);
  goto _118;
  _119 :;
  _118 :;
  * ((_INT32 *) preg62) = preg34;
  * ((_INT32 *) preg64) = preg34;
  * ((_IEEE64 *) preg66) = rcond - shift;
  goto _71;
  _181 :;
  goto _71;
  _182 :;
  _71 :;
  i3 = i3 + 1;
  preg34 = preg34 + 1;
  preg75 = preg75 + 1;
  if(i3 != preg3)
    goto _117;
  goto _113;
  _114 :;
  _113 :;
  rowstr2 = rowstr0;
  n1 = preg15;
  x1 = v0;
  preg18 = (_UINT32)(iv0);
  mark0 = (_INT32 *)(preg18);
  __ompv_ok_to_fork0 = __ompc_can_fork();
  if(!(preg96))
    goto _183;
  if(!(__ompv_ok_to_fork0 == 1))
    goto _184;
  __ompc_fork(0, &__ompdo_makea2, reg7);
  goto _73;
  _183 :;
  goto _74;
  _184 :;
  _74 :;
  __ompc_get_local_thread_num();
  __ompc_serialized_parallel();
  j = 1;
  if(!(preg15 >= 1))
    goto _185;
  preg33 = preg15;
  preg102 = preg15 + 1;
  preg26 = (_UINT32)(rowstr0 + 1);
  preg28 = preg18 + (_UINT32)(4);
  j = j;
  _123 :;
  * ((_INT32 *) preg26) = 0;
  * ((_INT32 *) preg28) = 0;
  j = (_INT32)((_UINT64) j) + 1;
  preg28 = preg28 + (_UINT32)(4);
  preg26 = preg26 + (_UINT32)(4);
  if(j != preg102)
    goto _123;
  goto _121;
  _185 :;
  _121 :;
  __ompc_end_serialized_parallel();
  _73 :;
  * ((_INT32 *)((_UINT8 *)(rowstr0) + preg8) + 1LL) = 0;
  preg1 = nnza > 0;
  if(!(preg1))
    goto _126;
  nza0 = 0;
  preg37 = nnza;
  preg87 = (_UINT32)(arow0 + 1);
  nza0 = nza0;
  _129 :;
  j3 = (*((_INT32 *) preg87) - preg17) + 2;
  preg31 = (_INT64)(j3);
  preg73 = (_UINT64)(preg31) * (_UINT64)(4);
  preg7 = (_UINT32)(preg73);
  preg60 = (_UINT32)((_INT32 *)((_UINT8 *)(rowstr0) + preg7));
  * ((_INT32 *) preg60) = *((_INT32 *) preg60) + 1;
  nza0 = nza0 + 1;
  preg87 = preg87 + (_UINT32)(4);
  if(nnza != nza0)
    goto _129;
  goto _125;
  _126 :;
  _125 :;
  * (rowstr0 + 1LL) = 1;
  preg58 = preg4 >= 0;
  if(!(preg58))
    goto _132;
  j0 = 0;
  preg3 = preg4 + 1;
  preg40 = preg3;
  preg86 = (_UINT32)(rowstr0 + 2);
  j0 = j0;
  _135 :;
  * ((_INT32 *) preg86) = *(((_INT32 *) preg86) + -1LL) + *((_INT32 *) preg86);
  j0 = j0 + 1;
  preg86 = preg86 + (_UINT32)(4);
  if(j0 != preg3)
    goto _135;
  goto _131;
  _132 :;
  _131 :;
  if(!(preg1))
    goto _138;
  nza1 = 0;
  preg43 = nnza;
  preg82 = (_UINT32)(arow0 + 1);
  preg83 = (_UINT32)(acol0 + 1);
  preg85 = (_UINT32)(aelt0 + 1);
  nza1 = nza1;
  _141 :;
  j4 = (*((_INT32 *) preg82) - preg17) + 1;
  preg21 = (_INT64)(j4);
  preg68 = (_UINT64)(preg21) * (_UINT64)(4);
  preg2 = (_UINT32)(preg68);
  preg59 = (_UINT32)((_INT32 *)((_UINT8 *)(rowstr0) + preg2));
  k = *((_INT32 *) preg59);
  preg23 = k;
  * (_IEEE64 *)((_UINT8 *)(a0) + (_UINT32)(((_UINT64)(preg23) * (_UINT64)(8)))) = *((_IEEE64 *) preg85);
  * (_INT32 *)((_UINT8 *)(colidx0) + (_UINT32)(((_UINT64)(preg23) * (_UINT64)(4)))) = *((_INT32 *) preg83);
  * ((_INT32 *) preg59) = *((_INT32 *) preg59) + 1;
  nza1 = nza1 + 1;
  preg85 = preg85 + (_UINT32)(8);
  preg83 = preg83 + (_UINT32)(4);
  preg82 = preg82 + (_UINT32)(4);
  if(nnza != nza1)
    goto _141;
  goto _137;
  _138 :;
  _137 :;
  if(!(preg4 > -1))
    goto _144;
  j1 = 0;
  preg3 = preg4 + 1;
  preg44 = preg3;
  preg36 = (_INT64)(preg3);
  preg71 = (_UINT64)(preg36) * (_UINT64)(4);
  preg101 = (_UINT32)(preg71);
  preg0 = (_UINT32)((_INT32 *)((_UINT8 *)(rowstr0) + preg101));
  j1 = j1;
  _147 :;
  * (((_INT32 *) preg0) + 1LL) = *((_INT32 *) preg0);
  j1 = j1 + 1;
  preg0 = preg0 + (_UINT32)(-4);
  if(j1 != preg3)
    goto _147;
  goto _143;
  _144 :;
  _143 :;
  * (rowstr0 + 1LL) = 1;
  nza = 0;
  __ompv_ok_to_fork1 = __ompc_can_fork();
  if(!(n1 > 1))
    goto _186;
  if(!(__ompv_ok_to_fork1 == 1))
    goto _187;
  __ompc_fork(0, &__ompdo_makea3, reg7);
  goto _76;
  _186 :;
  goto _77;
  _187 :;
  _77 :;
  __ompc_get_local_thread_num();
  __ompc_serialized_parallel();
  i0 = 1;
  if(!(n1 >= 1))
    goto _188;
  preg47 = n1;
  preg102 = n1 + 1;
  preg39 = (_UINT32)(v0 + 1);
  preg42 = (_UINT32)(mark0 + 1);
  i0 = i0;
  _151 :;
  * ((_IEEE64 *) preg39) = 0.0;
  * ((_INT32 *) preg42) = 0;
  i0 = (_INT32)((_UINT64) i0) + 1;
  preg42 = preg42 + (_UINT32)(4);
  preg39 = preg39 + (_UINT32)(8);
  if(i0 != preg102)
    goto _151;
  goto _149;
  _188 :;
  _149 :;
  __ompc_end_serialized_parallel();
  _76 :;
  jajp1 = *(rowstr0 + 1LL);
  if(!(preg58))
    goto _154;
  j2 = 0;
  preg3 = preg4 + 1;
  preg50 = preg3;
  preg30 = preg8 + preg18;
  preg56 = (_UINT32)(a0);
  preg57 = (_UINT32)(colidx0);
  preg72 = (_UINT32)(rowstr0 + 1);
  preg81 = preg30 + (_UINT32)(4);
  j2 = j2;
  _157 :;
  nzrow = 0;
  preg = *(((_INT32 *) preg72) + 1LL);
  if(!(jajp1 < preg))
    goto _172;
  preg9 = jajp1;
  preg61 = jajp1;
  preg90 = (_UINT64)(preg61) * (_UINT64)(4);
  preg92 = (_UINT64)(preg61) * (_UINT64)(8);
  preg46 = (_UINT32)(preg90);
  preg49 = (_UINT32)(preg92);
  preg70 = preg30;
  preg74 = (_UINT32)((_INT32 *)((_UINT8 *)(colidx0) + preg46));
  preg76 = (_UINT32)((_IEEE64 *)((_UINT8 *)(a0) + preg49));
  _160 :;
  i4 = *((_INT32 *) preg74);
  preg12 = (_INT64)(i4);
  preg63 = (_UINT64)(preg12) * (_UINT64)(8);
  preg93 = (_UINT32)(preg63);
  preg52 = (_UINT32)((_IEEE64 *)((_UINT8 *)(v0) + preg93));
  preg78 = *((_IEEE64 *) preg52) + *((_IEEE64 *) preg76);
  * ((_IEEE64 *) preg52) = preg78;
  preg65 = (_UINT64)(preg12) * (_UINT64)(4);
  preg95 = (_UINT32)(preg65);
  preg53 = preg95 + preg18;
  if(!(*((_INT32 *) preg53) == 0))
    goto _189;
  if(!(preg78 != 0.0))
    goto _190;
  * ((_INT32 *) preg53) = 1;
  nzrow = nzrow + 1;
  preg70 = preg70 + (_UINT32)(4);
  * ((_INT32 *) preg70) = i4;
  preg = *(((_INT32 *) preg72) + 1LL);
  goto _79;
  _189 :;
  goto _79;
  _190 :;
  _79 :;
  preg9 = preg9 + 1;
  preg76 = preg76 + (_UINT32)(8);
  preg74 = preg74 + (_UINT32)(4);
  if(preg9 < preg)
    goto _160;
  _161 :;
  if(!(nzrow > 0))
    goto _191;
  k2 = 0;
  preg51 = nzrow;
  preg80 = preg81;
  k2 = k2;
  _166 :;
  i5 = *((_INT32 *) preg80);
  preg16 = (_INT64)(i5);
  * ((_INT32 *)(preg18 + (_UINT32)(((_UINT64)(preg16) * (_UINT64)(4))))) = 0;
  preg67 = (_UINT64)(preg16) * (_UINT64)(8);
  preg98 = (_UINT32)(preg67);
  preg55 = (_UINT32)((_IEEE64 *)((_UINT8 *)(v0) + preg98));
  xi = *((_IEEE64 *) preg55);
  * ((_IEEE64 *) preg55) = 0.0;
  if(!(xi != 0.0))
    goto _168;
  nza = nza + 1;
  preg57 = preg57 + (_UINT32)(4);
  preg56 = preg56 + (_UINT32)(8);
  * ((_IEEE64 *) preg56) = xi;
  * ((_INT32 *) preg57) = i5;
  goto _167;
  _168 :;
  _167 :;
  k2 = k2 + 1;
  preg80 = preg80 + (_UINT32)(4);
  if(nzrow != k2)
    goto _166;
  preg = *(((_INT32 *) preg72) + 1LL);
  goto _162;
  _172 :;
  goto _163;
  _191 :;
  _163 :;
  _162 :;
  jajp1 = preg;
  * (((_INT32 *) preg72) + 1LL) = *(rowstr0 + 1LL) + nza;
  j2 = j2 + 1;
  preg72 = preg72 + (_UINT32)(4);
  if(j2 != preg3)
    goto _157;
  goto _153;
  _154 :;
  _153 :;
  return;
} /* makea */








static void sprnvc(
  _INT32 n,
  _INT32 nz,
  _IEEE64 * v0,
  _INT32 * iv0,
  _INT32 * nzloc,
  _INT32 * mark)
{
  register _IEEE64 preg4;
  register _UINT32 preg9;
  register _UINT32 preg10;
  register _UINT32 preg11;
  register _IEEE64 __comma;
  register _IEEE64 __comma0;
  register _IEEE64 preg5;
  register _INT32 preg6;
  register _UINT64 preg7;
  register _UINT64 preg12;
  register _UINT32 preg;
  register _UINT32 preg0;
  register _INT32 preg1;
  register _INT32 preg8;
  register _INT32 preg2;
  register _INT32 ii0;
  register _INT32 preg3;
  register _UINT32 preg13;
  _INT32 nn1;
  _INT32 nzrow;
  _INT32 nzv;
  
  nzv = 0;
  nzrow = 0;
  nn1 = 1;
  _34 :;
  nn1 = nn1 * 2;
  if(n > nn1)
    goto _34;
  if(!(nz > 0))
    goto _50;
  preg4 = (_IEEE64)(nn1);
  preg9 = (_UINT32)(nzloc);
  preg10 = (_UINT32)(v0);
  preg11 = (_UINT32)(iv0);
  _38 :;
  __comma = randlc(&tran, amult);
  __comma0 = randlc(&tran, amult);
  preg5 = __comma0 * preg4;
  preg6 = _I4F8TRUNC(preg5);
  if(!(n <= preg6))
    goto _40;
  goto _39;
  _40 :;
  preg7 = (_UINT64)(preg6) * (_UINT64)(4);
  preg12 = preg7 + (_UINT64)(4);
  preg = (_UINT32)(preg12);
  preg0 = (_UINT32)((_INT32 *)((_UINT8 *)(mark) + preg));
  if(!(*((_INT32 *) preg0) == 0))
    goto _42;
  * ((_INT32 *) preg0) = 1;
  preg1 = nzrow + 1;
  nzrow = preg1;
  preg9 = preg9 + (_UINT32)(4);
  preg8 = preg6 + 1;
  * ((_INT32 *) preg9) = preg8;
  nzv = preg1;
  preg11 = preg11 + (_UINT32)(4);
  preg10 = preg10 + (_UINT32)(8);
  * ((_IEEE64 *) preg10) = __comma;
  * ((_INT32 *) preg11) = preg8;
  goto _41;
  _42 :;
  _41 :;
  _39 :;
  preg2 = nzv;
  if(nz > preg2)
    goto _38;
  if(!(preg2 > 0))
    goto _52;
  _51 :;
  ii0 = 0;
  goto _47;
  _50 :;
  goto _45;
  _52 :;
  _45 :;
  goto _44;
  _47 :;
  preg3 = preg2;
  preg13 = (_UINT32)(nzloc + 1);
  ii0 = ii0;
  _48 :;
  * (_INT32 *)((_UINT8 *)(mark) + (_UINT32)(((_UINT64)(*((_INT32 *) preg13)) * (_UINT64)(4)))) = 0;
  ii0 = ii0 + 1;
  preg13 = preg13 + (_UINT32)(4);
  if(ii0 != preg2)
    goto _48;
  _44 :;
  return;
} /* sprnvc */

