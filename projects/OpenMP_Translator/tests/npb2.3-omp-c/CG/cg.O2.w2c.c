/*******************************************************
 * C file translated from WHIRL Sun Nov 20 21:06:07 2005
 *******************************************************/

/* Include file-level type and variable decls */
#include "cg.O2.w2c.h"


extern _INT32 main(
  _INT32 argc,
  _INT8 ** argv)
{
  register _IEEE64 __comma;
  register _INT32 __ompv_ok_to_fork;
  register _UINT32 reg7;
  register _INT32 preg13;
  register _INT32 preg19;
  register _INT32 preg7;
  register _INT32 preg20;
  register _UINT32 preg;
  register _INT32 preg3;
  register _INT32 preg8;
  register _UINT32 preg15;
  register _UINT32 preg0;
  register _UINT32 preg6;
  register _UINT32 preg21;
  register _INT32 preg14;
  register _INT32 preg22;
  register _INT32 preg9;
  register _INT32 preg23;
  register _UINT32 preg24;
  register _UINT32 preg25;
  register _IEEE64 preg2;
  register _IEEE64 __comma0;
  register _INT32 preg10;
  register _IEEE64 preg4;
  register _INT32 __ompv_ok_to_fork0;
  register _UINT32 preg5;
  register _INT32 preg11;
  register _IEEE64 __comma1;
  register _IEEE64 preg16;
  register _IEEE64 preg1;
  register _INT32 preg12;
  register _IEEE64 preg17;
  register _INT32 __comma2;
  register _IEEE64 __comma3;
  register _IEEE64 preg18;
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
  _IEEE64 mflops;
  _INT32 verified;
  
  /*Begin_of_nested_PU(s)*/
static void __ompregion_main2(__ompv_gtid_a, __ompv_slink_a)
  _INT32 __ompv_gtid_a;
  _UINT32 __ompv_slink_a;
{
  register _INT32 mpsp_status;
  register _INT32 preg;
  register _INT32 preg7;
  register _INT32 preg8;
  register _UINT32 preg0;
  register _INT32 preg2;
  register _UINT32 preg3;
  register _UINT32 preg4;
  register _IEEE64 preg5;
  register _INT32 mpsp_status0;
  register _IEEE64 __comma;
  register _INT32 mp_is_master;
  register _INT32 preg6;
  register _INT32 preg9;
  register _INT32 preg1;
  register _INT32 mp_is_master0;
  register _INT32 __comma0;
  _INT32 __mplocal_it;
  _INT32 __mplocal_j;
  _IEEE64 __mplocal_norm_temp11;
  _IEEE64 __mplocal_norm_temp12;
  _INT32 __ompv_temp_do_upper;
  _INT32 __ompv_temp_do_lower;
  _INT32 __ompv_temp_do_stride;
  _INT32 __ompv_temp_do_upper0;
  _INT32 __ompv_temp_do_lower0;
  _INT32 __ompv_temp_do_stride0;
  
  __mplocal_it = 1;
  _6 :;
  conj_grad((_INT32 *) colidx, (_INT32 *) rowstr, (_IEEE64 *) x, (_IEEE64 *) z, (_IEEE64 *) a, (_IEEE64 *) p, (_IEEE64 *) q, (_IEEE64 *) r, (_IEEE64 *) w, &rnorm);
  mpsp_status = __ompc_single(__ompv_gtid_a);
  if(!(mpsp_status == 1))
    goto _8;
  norm_temp11 = 0.0;
  norm_temp12 = 0.0;
  goto _7;
  _8 :;
  _7 :;
  __ompc_end_single(__ompv_gtid_a);
  __ompc_barrier();
  __mplocal_norm_temp11 = 0.0;
  __mplocal_norm_temp12 = 0.0;
  preg = lastcol - firstcol;
  preg7 = preg + 1;
  __ompv_temp_do_upper = preg7;
  __ompv_temp_do_lower = 1;
  __ompc_static_init_4(__ompv_gtid_a, 2, &__ompv_temp_do_lower, &__ompv_temp_do_upper, &__ompv_temp_do_stride, 1, 1);
  __ompv_temp_do_upper = (__ompv_temp_do_upper > preg7 ? preg7 : __ompv_temp_do_upper);
  __mplocal_j = __ompv_temp_do_lower;
  if(!(__ompv_temp_do_upper >= __ompv_temp_do_lower))
    goto _26;
  preg8 = (__ompv_temp_do_upper - __ompv_temp_do_lower) + 1;
  preg0 = (_UINT32)(__ompv_temp_do_lower) * (_UINT32)(8);
  preg2 = __ompv_temp_do_upper + 1;
  preg3 = (_UINT32)((_IEEE64 *)(preg0 + (_UINT8 *)(z)));
  preg4 = (_UINT32)((_IEEE64 *)(preg0 + (_UINT8 *)(x)));
  __mplocal_j = __mplocal_j;
  _11 :;
  preg5 = *((_IEEE64 *) preg3);
  __mplocal_norm_temp11 = __mplocal_norm_temp11 + (*((_IEEE64 *) preg4) * preg5);
  __mplocal_norm_temp12 = __mplocal_norm_temp12 + (preg5 * preg5);
  __mplocal_j = __mplocal_j + 1;
  preg4 = preg4 + (_UINT32)(8);
  preg3 = preg3 + (_UINT32)(8);
  if(__mplocal_j != preg2)
    goto _11;
  goto _9;
  _26 :;
  _9 :;
  __ompc_barrier();
  __ompc_critical(__ompv_gtid_a, (_UINT32) & __mplock_2);
  norm_temp11 = __mplocal_norm_temp11 + norm_temp11;
  norm_temp12 = __mplocal_norm_temp12 + norm_temp12;
  __ompc_end_critical(__ompv_gtid_a, (_UINT32) & __mplock_2);
  __ompc_barrier();
  mpsp_status0 = __ompc_single(__ompv_gtid_a);
  if(!(mpsp_status0 == 1))
    goto _14;
  __comma = sqrt(norm_temp12);
  norm_temp12 = _F8RECIP(__comma);
  zeta = _F8RECIP(norm_temp11) + 2.0e+01;
  goto _13;
  _14 :;
  _13 :;
  __ompc_end_single(__ompv_gtid_a);
  __ompc_barrier();
  mp_is_master = __ompc_master(__ompv_gtid_a);
  if(!(mp_is_master == 1))
    goto _16;
  if(!(__mplocal_it == 1))
    goto _18;
  printf((_INT8 *) "   iteration           ||r||                 zeta\n");
  goto _17;
  _18 :;
  _17 :;
  printf((_INT8 *) "    %5d       %20.14e%20.13e\n", __mplocal_it, rnorm, zeta);
  goto _15;
  _16 :;
  _15 :;
  preg = lastcol - firstcol;
  preg6 = preg + 1;
  __ompv_temp_do_upper0 = preg6;
  __ompv_temp_do_lower0 = 1;
  __ompc_static_init_4(__ompv_gtid_a, 2, &__ompv_temp_do_lower0, &__ompv_temp_do_upper0, &__ompv_temp_do_stride0, 1, 1);
  __ompv_temp_do_upper0 = (__ompv_temp_do_upper0 > preg6 ? preg6 : __ompv_temp_do_upper0);
  __mplocal_j = __ompv_temp_do_lower0;
  if(!(__ompv_temp_do_upper0 >= __ompv_temp_do_lower0))
    goto _27;
  preg9 = (__ompv_temp_do_upper0 - __ompv_temp_do_lower0) + 1;
  preg0 = (_UINT32)(__ompv_temp_do_lower0) * (_UINT32)(8);
  preg1 = __ompv_temp_do_upper0 + 1;
  preg3 = (_UINT32)((_IEEE64 *)(preg0 + (_UINT8 *)(z)));
  preg4 = (_UINT32)((_IEEE64 *)(preg0 + (_UINT8 *)(x)));
  __mplocal_j = __mplocal_j;
  _21 :;
  * ((_IEEE64 *) preg4) = *((_IEEE64 *) preg3) * norm_temp12;
  __mplocal_j = __mplocal_j + 1;
  preg4 = preg4 + (_UINT32)(8);
  preg3 = preg3 + (_UINT32)(8);
  if(__mplocal_j != preg1)
    goto _21;
  goto _19;
  _27 :;
  _19 :;
  __ompc_barrier();
  _3 :;
  __mplocal_it = __mplocal_it + 1;
  if(__mplocal_it != 16)
    goto _6;
  _23 :;
  mp_is_master0 = __ompc_master(__ompv_gtid_a);
  if(!(mp_is_master0 == 1))
    goto _25;
  __comma0 = omp_get_num_threads();
  nthreads = __comma0;
  goto _24;
  _25 :;
  _24 :;
  return;
} /* __ompregion_main2 */
static void __ompregion_main1(__ompv_gtid_a, __ompv_slink_a)
  _INT32 __ompv_gtid_a;
  _UINT32 __ompv_slink_a;
{
  register _INT32 preg16;
  register _INT32 preg9;
  register _INT32 preg10;
  register _UINT32 preg24;
  register _INT32 preg25;
  register _UINT32 preg2;
  register _INT32 preg8;
  register _INT32 preg5;
  register _INT32 preg11;
  register _UINT32 preg26;
  register _UINT32 preg3;
  register _INT32 preg12;
  register _UINT32 preg18;
  register _INT32 preg23;
  register _UINT32 preg;
  register _INT32 mpsp_status;
  register _INT32 mpsp_status0;
  register _INT32 preg17;
  register _INT32 preg7;
  register _INT32 preg13;
  register _UINT32 preg20;
  register _INT32 preg22;
  register _UINT32 preg0;
  register _UINT32 preg1;
  register _IEEE64 preg4;
  register _INT32 mpsp_status1;
  register _IEEE64 __comma;
  register _INT32 preg6;
  register _INT32 preg14;
  register _INT32 preg21;
  register _INT32 preg15;
  register _INT32 preg19;
  register _INT32 mpsp_status2;
  _INT32 __mplocal_i;
  _INT32 __mplocal_j;
  _INT32 __mplocal_k;
  _INT32 __ompv_temp_do_upper;
  _INT32 __ompv_temp_do_lower;
  _INT32 __ompv_temp_do_stride;
  _INT32 __ompv_temp_do_upper0;
  _INT32 __ompv_temp_do_lower0;
  _INT32 __ompv_temp_do_stride0;
  _IEEE64 __mplocal_norm_temp11;
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
  
  preg16 = lastrow - firstrow;
  preg9 = preg16 + 1;
  __ompv_temp_do_upper = preg9;
  __ompv_temp_do_lower = 1;
  __ompc_static_init_4(__ompv_gtid_a, 2, &__ompv_temp_do_lower, &__ompv_temp_do_upper, &__ompv_temp_do_stride, 1, 1);
  __ompv_temp_do_upper = (__ompv_temp_do_upper > preg9 ? preg9 : __ompv_temp_do_upper);
  __mplocal_j = __ompv_temp_do_lower;
  if(!(__ompv_temp_do_upper >= __ompv_temp_do_lower))
    goto _40;
  preg10 = (__ompv_temp_do_upper - __ompv_temp_do_lower) + 1;
  preg24 = (_UINT32)(__ompv_temp_do_lower) * (_UINT32)(4);
  preg25 = __ompv_temp_do_upper + 1;
  preg2 = (_UINT32)((_INT32 *)(preg24 + (_UINT8 *)(rowstr)));
  __mplocal_j = __mplocal_j;
  _10 :;
  preg8 = *((_INT32 *) preg2);
  __mplocal_k = preg8;
  preg5 = *(((_INT32 *) preg2) + 1LL);
  if(!(preg5 > preg8))
    goto _41;
  preg11 = _I4MAX(preg5 - preg8, 1);
  preg26 = (_UINT32)(preg8) * (_UINT32)(4);
  preg3 = (_UINT32)((_INT32 *)(preg26 + (_UINT8 *)(colidx)));
  _13 :;
  * ((_INT32 *) preg3) = (*((_INT32 *) preg3) - firstcol) + 1;
  _1 :;
  __mplocal_k = __mplocal_k + 1;
  preg3 = preg3 + (_UINT32)(4);
  if(__mplocal_k < preg5)
    goto _13;
  _14 :;
  goto _11;
  _41 :;
  _11 :;
  __mplocal_j = __mplocal_j + 1;
  preg2 = preg2 + (_UINT32)(4);
  if(__mplocal_j != preg25)
    goto _10;
  goto _8;
  _40 :;
  _8 :;
  __ompv_temp_do_upper0 = 14001;
  __ompv_temp_do_lower0 = 1;
  __ompc_static_init_4(__ompv_gtid_a, 2, &__ompv_temp_do_lower0, &__ompv_temp_do_upper0, &__ompv_temp_do_stride0, 1, 1);
  __ompv_temp_do_upper0 = (__ompv_temp_do_upper0 > 14001 ? 14001 : __ompv_temp_do_upper0);
  __mplocal_i = __ompv_temp_do_lower0;
  if(!(__ompv_temp_do_upper0 >= __ompv_temp_do_lower0))
    goto _42;
  preg12 = (__ompv_temp_do_upper0 - __ompv_temp_do_lower0) + 1;
  preg18 = (_UINT32)(__ompv_temp_do_lower0) * (_UINT32)(8);
  preg23 = __ompv_temp_do_upper0 + 1;
  preg = (_UINT32)((_IEEE64 *)(preg18 + (_UINT8 *)(x)));
  __mplocal_i = __mplocal_i;
  _18 :;
  * ((_IEEE64 *) preg) = 1.0;
  __mplocal_i = __mplocal_i + 1;
  preg = preg + (_UINT32)(8);
  if(__mplocal_i != preg23)
    goto _18;
  goto _16;
  _42 :;
  _16 :;
  mpsp_status = __ompc_single(__ompv_gtid_a);
  zeta = (mpsp_status == 1 ? 0.0 : zeta);
  __ompc_end_single(__ompv_gtid_a);
  __ompc_barrier();
  conj_grad((_INT32 *) colidx, (_INT32 *) rowstr, (_IEEE64 *) x, (_IEEE64 *) z, (_IEEE64 *) a, (_IEEE64 *) p, (_IEEE64 *) q, (_IEEE64 *) r, (_IEEE64 *) w, &rnorm);
  mpsp_status0 = __ompc_single(__ompv_gtid_a);
  if(!(mpsp_status0 == 1))
    goto _24;
  norm_temp11 = 0.0;
  norm_temp12 = 0.0;
  goto _23;
  _24 :;
  _23 :;
  __ompc_end_single(__ompv_gtid_a);
  __ompc_barrier();
  __mplocal_norm_temp11 = 0.0;
  __mplocal_norm_temp12 = 0.0;
  preg17 = lastcol - firstcol;
  preg7 = preg17 + 1;
  __ompv_temp_do_upper1 = preg7;
  __ompv_temp_do_lower1 = 1;
  __ompc_static_init_4(__ompv_gtid_a, 2, &__ompv_temp_do_lower1, &__ompv_temp_do_upper1, &__ompv_temp_do_stride1, 1, 1);
  __ompv_temp_do_upper1 = (__ompv_temp_do_upper1 > preg7 ? preg7 : __ompv_temp_do_upper1);
  __mplocal_j = __ompv_temp_do_lower1;
  if(!(__ompv_temp_do_upper1 >= __ompv_temp_do_lower1))
    goto _43;
  preg13 = (__ompv_temp_do_upper1 - __ompv_temp_do_lower1) + 1;
  preg20 = (_UINT32)(__ompv_temp_do_lower1) * (_UINT32)(8);
  preg22 = __ompv_temp_do_upper1 + 1;
  preg0 = (_UINT32)((_IEEE64 *)(preg20 + (_UINT8 *)(z)));
  preg1 = (_UINT32)((_IEEE64 *)(preg20 + (_UINT8 *)(x)));
  __mplocal_j = __mplocal_j;
  _27 :;
  preg4 = *((_IEEE64 *) preg0);
  __mplocal_norm_temp11 = __mplocal_norm_temp11 + (*((_IEEE64 *) preg1) * preg4);
  __mplocal_norm_temp12 = __mplocal_norm_temp12 + (preg4 * preg4);
  __mplocal_j = __mplocal_j + 1;
  preg1 = preg1 + (_UINT32)(8);
  preg0 = preg0 + (_UINT32)(8);
  if(__mplocal_j != preg22)
    goto _27;
  goto _25;
  _43 :;
  _25 :;
  __ompc_barrier();
  __ompc_critical(__ompv_gtid_a, (_UINT32) & __mplock_1);
  norm_temp11 = norm_temp11 + __mplocal_norm_temp11;
  norm_temp12 = norm_temp12 + __mplocal_norm_temp12;
  __ompc_end_critical(__ompv_gtid_a, (_UINT32) & __mplock_1);
  __ompc_barrier();
  mpsp_status1 = __ompc_single(__ompv_gtid_a);
  if(!(mpsp_status1 == 1))
    goto _30;
  __comma = sqrt(norm_temp12);
  norm_temp12 = _F8RECIP(__comma);
  goto _29;
  _30 :;
  _29 :;
  __ompc_end_single(__ompv_gtid_a);
  __ompc_barrier();
  preg17 = lastcol - firstcol;
  preg6 = preg17 + 1;
  __ompv_temp_do_upper2 = preg6;
  __ompv_temp_do_lower2 = 1;
  __ompc_static_init_4(__ompv_gtid_a, 2, &__ompv_temp_do_lower2, &__ompv_temp_do_upper2, &__ompv_temp_do_stride2, 1, 1);
  __ompv_temp_do_upper2 = (__ompv_temp_do_upper2 > preg6 ? preg6 : __ompv_temp_do_upper2);
  __mplocal_j = __ompv_temp_do_lower2;
  if(!(__ompv_temp_do_upper2 >= __ompv_temp_do_lower2))
    goto _44;
  preg14 = (__ompv_temp_do_upper2 - __ompv_temp_do_lower2) + 1;
  preg20 = (_UINT32)(__ompv_temp_do_lower2) * (_UINT32)(8);
  preg21 = __ompv_temp_do_upper2 + 1;
  preg0 = (_UINT32)((_IEEE64 *)(preg20 + (_UINT8 *)(z)));
  preg1 = (_UINT32)((_IEEE64 *)(preg20 + (_UINT8 *)(x)));
  __mplocal_j = __mplocal_j;
  _33 :;
  * ((_IEEE64 *) preg1) = *((_IEEE64 *) preg0) * norm_temp12;
  __mplocal_j = __mplocal_j + 1;
  preg1 = preg1 + (_UINT32)(8);
  preg0 = preg0 + (_UINT32)(8);
  if(__mplocal_j != preg21)
    goto _33;
  goto _31;
  _44 :;
  _31 :;
  __ompc_barrier();
  __ompv_temp_do_upper3 = 14001;
  __ompv_temp_do_lower3 = 1;
  __ompc_static_init_4(__ompv_gtid_a, 2, &__ompv_temp_do_lower3, &__ompv_temp_do_upper3, &__ompv_temp_do_stride3, 1, 1);
  __ompv_temp_do_upper3 = (__ompv_temp_do_upper3 > 14001 ? 14001 : __ompv_temp_do_upper3);
  __mplocal_i = __ompv_temp_do_lower3;
  if(!(__ompv_temp_do_upper3 >= __ompv_temp_do_lower3))
    goto _45;
  preg15 = (__ompv_temp_do_upper3 - __ompv_temp_do_lower3) + 1;
  preg18 = (_UINT32)(__ompv_temp_do_lower3) * (_UINT32)(8);
  preg19 = __ompv_temp_do_upper3 + 1;
  preg = (_UINT32)((_IEEE64 *)(preg18 + (_UINT8 *)(x)));
  __mplocal_i = __mplocal_i;
  _38 :;
  * ((_IEEE64 *) preg) = 1.0;
  __mplocal_i = __mplocal_i + 1;
  preg = preg + (_UINT32)(8);
  if(__mplocal_i != preg19)
    goto _38;
  goto _36;
  _45 :;
  _36 :;
  mpsp_status2 = __ompc_single(__ompv_gtid_a);
  zeta = (mpsp_status2 == 1 ? 0.0 : zeta);
  __ompc_end_single(__ompv_gtid_a);
  __ompc_barrier();
  return;
} /* __ompregion_main1 */
  
  nthreads = 1;
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
  __comma = randlc(&tran, 1.220703125e+09);
  zeta = __comma;
  makea(naa, nzz, (_IEEE64 *) a, (_INT32 *) colidx, (_INT32 *) rowstr, 11, firstrow, lastrow, firstcol, lastcol, 1.0000000000000001e-01, (_INT32 *) arow, (_INT32 *) acol, (_IEEE64 *) aelt, (_IEEE64 *) v, (_INT32 *) iv, 2.0e+01);
  __ompv_ok_to_fork = __ompc_can_fork();
  if(!(__ompv_ok_to_fork == 1))
    goto _70;
  __ompc_fork(0, &__ompregion_main1, reg7);
  goto _69;
  _70 :;
  __ompc_get_local_thread_num();
  __ompc_serialized_parallel();
  j = 1;
  preg13 = lastrow - firstrow;
  preg19 = preg13 + 1;
  if(!(preg19 >= 1))
    goto _122;
  preg7 = preg19;
  preg20 = preg13 + 2;
  preg = (_UINT32)((_INT32 *)(((_INT8 *) & rowstr) + 4LL));
  j = j;
  _73 :;
  k = *((_INT32 *) preg);
  preg3 = *(((_INT32 *) preg) + 1LL);
  if(!(k < preg3))
    goto _123;
  preg8 = _I4MAX(preg3 - k, 1);
  preg15 = (_UINT32)(k) * (_UINT32)(4);
  preg0 = (_UINT32)((_INT32 *)(preg15 + (_UINT8 *)(colidx)));
  _76 :;
  * ((_INT32 *) preg0) = (*((_INT32 *) preg0) - firstcol) + 1;
  _2 :;
  k = k + 1;
  preg0 = preg0 + (_UINT32)(4);
  if(k < preg3)
    goto _76;
  _77 :;
  goto _74;
  _123 :;
  _74 :;
  j = j + 1;
  preg = preg + (_UINT32)(4);
  if(j != preg20)
    goto _73;
  goto _71;
  _122 :;
  _71 :;
  i = 1;
  preg6 = (_UINT32)((_IEEE64 *)(((_INT8 *) & x) + 8LL));
  preg21 = preg6;
  i = i;
  _81 :;
  * ((_IEEE64 *) preg21) = 1.0;
  i = i + 1;
  preg21 = preg21 + (_UINT32)(8);
  if(i != 14002)
    goto _81;
  conj_grad((_INT32 *) colidx, (_INT32 *) rowstr, (_IEEE64 *) x, (_IEEE64 *) z, (_IEEE64 *) a, (_IEEE64 *) p, (_IEEE64 *) q, (_IEEE64 *) r, (_IEEE64 *) w, &rnorm);
  norm_temp11 = 0.0;
  norm_temp12 = 0.0;
  j = 1;
  preg14 = lastcol - firstcol;
  preg22 = preg14 + 1;
  if(!(preg22 >= 1))
    goto _124;
  preg9 = preg22;
  preg23 = preg14 + 2;
  preg24 = (_UINT32)((_IEEE64 *)(((_INT8 *) & z) + 8LL));
  preg25 = preg6;
  j = j;
  _88 :;
  preg2 = *((_IEEE64 *) preg24);
  norm_temp11 = norm_temp11 + (*((_IEEE64 *) preg25) * preg2);
  norm_temp12 = norm_temp12 + (preg2 * preg2);
  j = j + 1;
  preg25 = preg25 + (_UINT32)(8);
  preg24 = preg24 + (_UINT32)(8);
  if(j != preg23)
    goto _88;
  goto _86;
  _124 :;
  _86 :;
  __comma0 = sqrt(norm_temp12);
  j = 1;
  preg14 = lastcol - firstcol;
  preg22 = preg14 + 1;
  if(!(preg22 >= 1))
    goto _125;
  preg10 = preg22;
  preg4 = _F8RECIP(__comma0);
  preg23 = preg14 + 2;
  preg24 = (_UINT32)((_IEEE64 *)(((_INT8 *) & z) + 8LL));
  preg25 = preg6;
  j = j;
  _92 :;
  * ((_IEEE64 *) preg25) = *((_IEEE64 *) preg24) * preg4;
  j = j + 1;
  preg25 = preg25 + (_UINT32)(8);
  preg24 = preg24 + (_UINT32)(8);
  if(j != preg23)
    goto _92;
  goto _90;
  _125 :;
  preg4 = _F8RECIP(__comma0);
  _90 :;
  norm_temp12 = preg4;
  i = 1;
  preg21 = preg6;
  i = i;
  _97 :;
  * ((_IEEE64 *) preg21) = 1.0;
  i = i + 1;
  preg21 = preg21 + (_UINT32)(8);
  if(i != 14002)
    goto _97;
  zeta = 0.0;
  __ompc_end_serialized_parallel();
  _69 :;
  timer_clear(1);
  timer_start(1);
  __ompv_ok_to_fork0 = __ompc_can_fork();
  if(!(__ompv_ok_to_fork0 == 1))
    goto _100;
  __ompc_fork(0, &__ompregion_main2, reg7);
  goto _99;
  _100 :;
  __ompc_get_local_thread_num();
  __ompc_serialized_parallel();
  it = 1;
  preg5 = (_UINT32)((_IEEE64 *)(((_INT8 *) & z) + 8LL));
  preg6 = (_UINT32)((_IEEE64 *)(((_INT8 *) & x) + 8LL));
  _103 :;
  conj_grad((_INT32 *) colidx, (_INT32 *) rowstr, (_IEEE64 *) x, (_IEEE64 *) z, (_IEEE64 *) a, (_IEEE64 *) p, (_IEEE64 *) q, (_IEEE64 *) r, (_IEEE64 *) w, &rnorm);
  norm_temp11 = 0.0;
  norm_temp12 = 0.0;
  j = 1;
  preg14 = lastcol - firstcol;
  preg22 = preg14 + 1;
  if(!(preg22 >= 1))
    goto _126;
  preg11 = preg22;
  preg23 = preg14 + 2;
  preg24 = preg5;
  preg25 = preg6;
  j = j;
  _106 :;
  preg2 = *((_IEEE64 *) preg24);
  norm_temp11 = norm_temp11 + (*((_IEEE64 *) preg25) * preg2);
  norm_temp12 = norm_temp12 + (preg2 * preg2);
  j = j + 1;
  preg25 = preg25 + (_UINT32)(8);
  preg24 = preg24 + (_UINT32)(8);
  if(j != preg23)
    goto _106;
  goto _104;
  _126 :;
  _104 :;
  __comma1 = sqrt(norm_temp12);
  if(!(it == 1))
    goto _109;
  printf((_INT8 *) "   iteration           ||r||                 zeta\n");
  goto _108;
  _109 :;
  _108 :;
  preg16 = _F8RECIP(norm_temp11);
  preg1 = preg16 + 2.0e+01;
  printf((_INT8 *) "    %5d       %20.14e%20.13e\n", it, rnorm, preg1);
  j = 1;
  preg14 = lastcol - firstcol;
  preg22 = preg14 + 1;
  if(!(preg22 >= 1))
    goto _127;
  preg12 = preg22;
  preg17 = _F8RECIP(__comma1);
  preg23 = preg14 + 2;
  preg24 = preg5;
  preg25 = preg6;
  j = j;
  _112 :;
  * ((_IEEE64 *) preg25) = *((_IEEE64 *) preg24) * preg17;
  j = j + 1;
  preg25 = preg25 + (_UINT32)(8);
  preg24 = preg24 + (_UINT32)(8);
  if(j != preg23)
    goto _112;
  goto _110;
  _127 :;
  _110 :;
  _8 :;
  it = it + 1;
  if(it != 16)
    goto _103;
  _114 :;
  zeta = preg1;
  __comma2 = omp_get_num_threads();
  nthreads = __comma2;
  __ompc_end_serialized_parallel();
  _99 :;
  timer_stop(1);
  __comma3 = timer_read(1);
  printf((_INT8 *) " Benchmark completed\n");
  preg18 = zeta + -1.7130235054029001e+01;
  if(!(_F8ABS(preg18) <= 1.0e-10))
    goto _118;
  verified = 1;
  printf((_INT8 *) " VERIFICATION SUCCESSFUL\n");
  printf((_INT8 *) " Zeta is    %20.12e\n", zeta);
  printf((_INT8 *) " Error is   %20.12e\n", preg18);
  goto _117;
  _118 :;
  verified = 0;
  printf((_INT8 *) " VERIFICATION FAILED\n");
  printf((_INT8 *) " Zeta                %20.12e\n", zeta);
  printf((_INT8 *) " The correct zeta is %20.12e\n", 1.7130235054029001e+01);
  _117 :;
  goto _115;
  _115 :;
  if(!(__comma3 != 0.0))
    goto _120;
  mflops = (1.49646e+09 / __comma3) / 1.0e+06;
  goto _119;
  _120 :;
  mflops = 0.0;
  _119 :;
  c_print_results((_INT8 *) "CG", (_INT8) 65, 14000, 0, 0, 15, nthreads, __comma3, mflops, (_INT8 *) "          floating point", verified, (_INT8 *) "2.3", (_INT8 *) "18 Nov 2005", (_INT8 *) "gcc", (_INT8 *) "gcc", (_INT8 *) "(none)", (_INT8 *) "-I../common -I $(HOME)/orc2.1.uh/src/osprey...", (_INT8 *) "-O2", (_INT8 *) "-O2  -L $(HOME)/orc2.1.uh/src/osprey1.0/tar...", (_INT8 *) "randdp");
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
  register _INT32 preg72;
  register _INT32 preg74;
  register _INT64 preg44;
  register _INT32 preg46;
  register _UINT64 preg84;
  register _UINT32 preg110;
  register _UINT32 preg28;
  register _UINT32 preg30;
  register _UINT32 preg32;
  register _UINT32 preg34;
  register _UINT32 preg36;
  register _UINT32 preg38;
  register _IEEE64 preg47;
  register _INT32 preg119;
  register _INT32 preg70;
  register _INT32 preg76;
  register _INT64 preg40;
  register _INT32 preg42;
  register _UINT64 preg82;
  register _UINT32 preg109;
  register _UINT32 preg26;
  register _IEEE64 preg45;
  register _INT32 mpsp_status0;
  register _INT32 preg121;
  register _INT32 preg68;
  register _INT32 preg78;
  register _INT64 preg33;
  register _INT32 preg35;
  register _UINT64 preg73;
  register _UINT64 preg75;
  register _UINT32 preg105;
  register _UINT32 preg106;
  register _UINT32 preg18;
  register _UINT32 preg20;
  register _INT32 preg43;
  register _INT32 preg80;
  register _INT64 preg37;
  register _UINT64 preg77;
  register _UINT64 preg79;
  register _UINT32 preg107;
  register _UINT32 preg108;
  register _UINT32 preg22;
  register _UINT32 preg24;
  register _INT32 preg66;
  register _INT32 preg81;
  register _INT64 preg29;
  register _INT32 preg31;
  register _UINT64 preg71;
  register _UINT32 preg104;
  register _UINT32 preg14;
  register _UINT32 preg16;
  register _INT32 preg64;
  register _INT32 preg83;
  register _INT64 preg25;
  register _INT32 preg27;
  register _UINT64 preg69;
  register _UINT32 preg103;
  register _UINT32 preg12;
  register _INT32 preg62;
  register _INT32 preg86;
  register _INT64 preg21;
  register _INT32 preg23;
  register _UINT64 preg67;
  register _UINT32 preg102;
  register _UINT32 preg8;
  register _UINT32 preg10;
  register _INT32 mpsp_status1;
  register _INT32 preg60;
  register _INT32 preg88;
  register _INT64 preg17;
  register _INT32 preg19;
  register _UINT64 preg65;
  register _UINT32 preg101;
  register _UINT32 preg0;
  register _UINT32 preg2;
  register _UINT32 preg4;
  register _UINT32 preg6;
  register _INT32 preg58;
  register _INT32 preg90;
  register _INT64 preg13;
  register _INT32 preg15;
  register _UINT64 preg63;
  register _UINT32 preg100;
  register _UINT32 preg125;
  register _IEEE64 preg41;
  register _INT32 mpsp_status2;
  register _INT32 preg56;
  register _INT32 preg92;
  register _INT64 preg9;
  register _INT32 preg11;
  register _UINT64 preg61;
  register _UINT32 preg98;
  register _UINT32 preg120;
  register _UINT32 preg123;
  register _INT32 mpsp_status3;
  register _INT32 preg54;
  register _INT32 preg94;
  register _INT64 preg3;
  register _INT32 preg5;
  register _UINT64 preg53;
  register _UINT64 preg55;
  register _UINT32 preg89;
  register _UINT32 preg91;
  register _UINT32 preg115;
  register _UINT32 preg116;
  register _INT32 preg39;
  register _INT32 preg48;
  register _INT32 preg96;
  register _INT64 preg7;
  register _UINT64 preg57;
  register _UINT64 preg59;
  register _UINT32 preg93;
  register _UINT32 preg95;
  register _UINT32 preg117;
  register _UINT32 preg118;
  register _INT32 preg52;
  register _INT32 preg97;
  register _INT64 preg;
  register _INT32 preg1;
  register _UINT64 preg51;
  register _UINT32 preg87;
  register _UINT32 preg113;
  register _UINT32 preg114;
  register _INT32 preg50;
  register _INT32 preg99;
  register _INT64 preg122;
  register _INT32 preg124;
  register _UINT64 preg49;
  register _UINT32 preg85;
  register _UINT32 preg111;
  register _UINT32 preg112;
  register _INT32 mpsp_status4;
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
  _INT32 __mplocal_j0;
  _INT32 __ompv_temp_do_upper0;
  _INT32 __ompv_temp_do_lower0;
  _INT32 __ompv_temp_do_stride0;
  _INT32 __mplocal_j1;
  _IEEE64 __mplocal_sum;
  _INT32 __mplocal_k;
  _INT32 __ompv_temp_do_upper1;
  _INT32 __ompv_temp_do_lower1;
  _INT32 __ompv_temp_do_stride1;
  _INT32 __mplocal_j2;
  _INT32 __ompv_temp_do_upper2;
  _INT32 __ompv_temp_do_lower2;
  _INT32 __ompv_temp_do_stride2;
  _INT32 __mplocal_j3;
  _INT32 __ompv_temp_do_upper3;
  _INT32 __ompv_temp_do_lower3;
  _INT32 __ompv_temp_do_stride3;
  _IEEE64 __mplocal_d;
  _INT32 __mplocal_j4;
  _INT32 __ompv_temp_do_upper4;
  _INT32 __ompv_temp_do_lower4;
  _INT32 __ompv_temp_do_stride4;
  _INT32 __mplocal_j5;
  _INT32 __ompv_temp_do_upper5;
  _INT32 __ompv_temp_do_lower5;
  _INT32 __ompv_temp_do_stride5;
  _IEEE64 __mplocal_rho0;
  _INT32 __mplocal_j6;
  _INT32 __ompv_temp_do_upper6;
  _INT32 __ompv_temp_do_lower6;
  _INT32 __ompv_temp_do_stride6;
  _INT32 __mplocal_j7;
  _INT32 __ompv_temp_do_upper7;
  _INT32 __ompv_temp_do_lower7;
  _INT32 __ompv_temp_do_stride7;
  _INT32 __mplocal_j8;
  _IEEE64 __mplocal_d0;
  _INT32 __mplocal_k0;
  _INT32 __ompv_temp_do_upper8;
  _INT32 __ompv_temp_do_lower8;
  _INT32 __ompv_temp_do_stride8;
  _INT32 __mplocal_j9;
  _INT32 __ompv_temp_do_upper9;
  _INT32 __ompv_temp_do_lower9;
  _INT32 __ompv_temp_do_stride9;
  _IEEE64 __mplocal_sum0;
  _INT32 __mplocal_j10;
  _IEEE64 __mplocal_d1;
  _INT32 __ompv_temp_do_upper10;
  _INT32 __ompv_temp_do_lower10;
  _INT32 __ompv_temp_do_stride10;
  
  /*Begin_of_nested_PU(s)*/
  
  __ompv_gtid_s1 = __ompc_get_local_thread_num();
  mpsp_status = __ompc_single(__ompv_gtid_s1);
  rho = (mpsp_status == 1 ? 0.0 : rho);
  __ompc_end_single(__ompv_gtid_s1);
  __ompv_gtid_s1 = __ompc_get_local_thread_num();
  preg72 = naa + 1;
  __ompv_temp_do_upper = preg72;
  __ompv_temp_do_lower = 1;
  __ompc_static_init_4(__ompv_gtid_s1, 2, &__ompv_temp_do_lower, &__ompv_temp_do_upper, &__ompv_temp_do_stride, 1, 1);
  __ompv_temp_do_upper = (__ompv_temp_do_upper > preg72 ? preg72 : __ompv_temp_do_upper);
  __mplocal_j = __ompv_temp_do_lower;
  if(!(__ompv_temp_do_upper >= __ompv_temp_do_lower))
    goto _96;
  preg74 = (__ompv_temp_do_upper - __ompv_temp_do_lower) + 1;
  preg44 = __ompv_temp_do_lower;
  preg46 = __ompv_temp_do_upper + 1;
  preg84 = (_UINT64)(preg44) * (_UINT64)(8);
  preg110 = (_UINT32)(preg84);
  preg28 = (_UINT32)((_IEEE64 *)((_UINT8 *)(q0) + preg110));
  preg30 = (_UINT32)((_IEEE64 *)((_UINT8 *)(z0) + preg110));
  preg32 = (_UINT32)((_IEEE64 *)((_UINT8 *)(x0) + preg110));
  preg34 = (_UINT32)((_IEEE64 *)((_UINT8 *)(r0) + preg110));
  preg36 = (_UINT32)((_IEEE64 *)((_UINT8 *)(p0) + preg110));
  preg38 = (_UINT32)((_IEEE64 *)((_UINT8 *)(w0) + preg110));
  __mplocal_j = __mplocal_j;
  _30 :;
  * ((_IEEE64 *) preg28) = 0.0;
  * ((_IEEE64 *) preg30) = 0.0;
  preg47 = *((_IEEE64 *) preg32);
  * ((_IEEE64 *) preg34) = preg47;
  * ((_IEEE64 *) preg36) = preg47;
  * ((_IEEE64 *) preg38) = 0.0;
  __mplocal_j = (_INT32)((_UINT64) __mplocal_j) + 1;
  preg38 = preg38 + (_UINT32)(8);
  preg36 = preg36 + (_UINT32)(8);
  preg34 = preg34 + (_UINT32)(8);
  preg32 = preg32 + (_UINT32)(8);
  preg30 = preg30 + (_UINT32)(8);
  preg28 = preg28 + (_UINT32)(8);
  if(__mplocal_j != preg46)
    goto _30;
  goto _28;
  _96 :;
  _28 :;
  __ompv_gtid_s1 = __ompc_get_local_thread_num();
  __mplocal_rho = 0.0;
  preg119 = lastcol - firstcol;
  preg70 = preg119 + 1;
  __ompv_temp_do_upper0 = preg70;
  __ompv_temp_do_lower0 = 1;
  __ompc_static_init_4(__ompv_gtid_s1, 2, &__ompv_temp_do_lower0, &__ompv_temp_do_upper0, &__ompv_temp_do_stride0, 1, 1);
  __ompv_temp_do_upper0 = (__ompv_temp_do_upper0 > preg70 ? preg70 : __ompv_temp_do_upper0);
  __mplocal_j0 = __ompv_temp_do_lower0;
  if(!(__ompv_temp_do_upper0 >= __ompv_temp_do_lower0))
    goto _97;
  preg76 = (__ompv_temp_do_upper0 - __ompv_temp_do_lower0) + 1;
  preg40 = __ompv_temp_do_lower0;
  preg42 = __ompv_temp_do_upper0 + 1;
  preg82 = (_UINT64)(preg40) * (_UINT64)(8);
  preg109 = (_UINT32)(preg82);
  preg26 = (_UINT32)((_IEEE64 *)((_UINT8 *)(x0) + preg109));
  __mplocal_j0 = __mplocal_j0;
  _34 :;
  preg45 = *((_IEEE64 *) preg26);
  __mplocal_rho = __mplocal_rho + (preg45 * preg45);
  __mplocal_j0 = (_INT32)((_UINT64) __mplocal_j0) + 1;
  preg26 = preg26 + (_UINT32)(8);
  if(__mplocal_j0 != preg42)
    goto _34;
  goto _32;
  _97 :;
  _32 :;
  __ompc_barrier();
  __ompc_critical(__ompv_gtid_s1, (_UINT32) & __mplock_1);
  rho = rho + __mplocal_rho;
  __ompc_end_critical(__ompv_gtid_s1, (_UINT32) & __mplock_1);
  __ompc_barrier();
  cgit = 1;
  _38 :;
  __ompv_gtid_s1 = __ompc_get_local_thread_num();
  mpsp_status0 = __ompc_single(__ompv_gtid_s1);
  if(!(mpsp_status0 == 1))
    goto _40;
  rho0 = rho;
  d = 0.0;
  rho = 0.0;
  goto _39;
  _40 :;
  _39 :;
  __ompc_end_single(__ompv_gtid_s1);
  __ompv_gtid_s1 = __ompc_get_local_thread_num();
  preg121 = lastrow - firstrow;
  preg68 = preg121 + 1;
  __ompv_temp_do_upper1 = preg68;
  __ompv_temp_do_lower1 = 1;
  __ompc_static_init_4(__ompv_gtid_s1, 2, &__ompv_temp_do_lower1, &__ompv_temp_do_upper1, &__ompv_temp_do_stride1, 1, 1);
  __ompv_temp_do_upper1 = (__ompv_temp_do_upper1 > preg68 ? preg68 : __ompv_temp_do_upper1);
  __mplocal_j1 = __ompv_temp_do_lower1;
  if(!(__ompv_temp_do_upper1 >= __ompv_temp_do_lower1))
    goto _98;
  preg78 = (__ompv_temp_do_upper1 - __ompv_temp_do_lower1) + 1;
  preg33 = __ompv_temp_do_lower1;
  preg35 = __ompv_temp_do_upper1 + 1;
  preg73 = (_UINT64)(preg33) * (_UINT64)(4);
  preg75 = (_UINT64)(preg33) * (_UINT64)(8);
  preg105 = (_UINT32)(preg73);
  preg106 = (_UINT32)(preg75);
  preg18 = (_UINT32)((_INT32 *)((_UINT8 *)(rowstr0) + preg105));
  preg20 = (_UINT32)((_IEEE64 *)((_UINT8 *)(w0) + preg106));
  __mplocal_j1 = __mplocal_j1;
  _43 :;
  __mplocal_sum = 0.0;
  __mplocal_k = *((_INT32 *) preg18);
  preg43 = *(((_INT32 *) preg18) + 1LL);
  if(!(__mplocal_k < preg43))
    goto _99;
  preg80 = _I4MAX(preg43 - __mplocal_k, 1);
  preg37 = __mplocal_k;
  preg77 = (_UINT64)(preg37) * (_UINT64)(8);
  preg79 = (_UINT64)(preg37) * (_UINT64)(4);
  preg107 = (_UINT32)(preg77);
  preg108 = (_UINT32)(preg79);
  preg22 = (_UINT32)((_IEEE64 *)((_UINT8 *)(a0) + preg107));
  preg24 = (_UINT32)((_INT32 *)((_UINT8 *)(colidx0) + preg108));
  _46 :;
  __mplocal_sum = __mplocal_sum + (*((_IEEE64 *) preg22) ** (_IEEE64 *)((_UINT8 *)(p0) + (_UINT32)(((_UINT64)(*((_INT32 *) preg24)) * (_UINT64)(8)))));
  _5 :;
  __mplocal_k = (_INT32)((_UINT64) __mplocal_k) + 1;
  preg24 = preg24 + (_UINT32)(4);
  preg22 = preg22 + (_UINT32)(8);
  if(__mplocal_k < preg43)
    goto _46;
  _47 :;
  goto _44;
  _99 :;
  _44 :;
  * ((_IEEE64 *) preg20) = __mplocal_sum;
  __mplocal_j1 = (_INT32)((_UINT64) __mplocal_j1) + 1;
  preg20 = preg20 + (_UINT32)(8);
  preg18 = preg18 + (_UINT32)(4);
  if(__mplocal_j1 != preg35)
    goto _43;
  goto _41;
  _98 :;
  _41 :;
  __ompc_barrier();
  __ompv_gtid_s1 = __ompc_get_local_thread_num();
  preg119 = lastcol - firstcol;
  preg66 = preg119 + 1;
  __ompv_temp_do_upper2 = preg66;
  __ompv_temp_do_lower2 = 1;
  __ompc_static_init_4(__ompv_gtid_s1, 2, &__ompv_temp_do_lower2, &__ompv_temp_do_upper2, &__ompv_temp_do_stride2, 1, 1);
  __ompv_temp_do_upper2 = (__ompv_temp_do_upper2 > preg66 ? preg66 : __ompv_temp_do_upper2);
  __mplocal_j2 = __ompv_temp_do_lower2;
  if(!(__ompv_temp_do_upper2 >= __ompv_temp_do_lower2))
    goto _100;
  preg81 = (__ompv_temp_do_upper2 - __ompv_temp_do_lower2) + 1;
  preg29 = __ompv_temp_do_lower2;
  preg31 = __ompv_temp_do_upper2 + 1;
  preg71 = (_UINT64)(preg29) * (_UINT64)(8);
  preg104 = (_UINT32)(preg71);
  preg14 = (_UINT32)((_IEEE64 *)((_UINT8 *)(w0) + preg104));
  preg16 = (_UINT32)((_IEEE64 *)((_UINT8 *)(q0) + preg104));
  __mplocal_j2 = __mplocal_j2;
  _51 :;
  * ((_IEEE64 *) preg16) = *((_IEEE64 *) preg14);
  __mplocal_j2 = (_INT32)((_UINT64) __mplocal_j2) + 1;
  preg16 = preg16 + (_UINT32)(8);
  preg14 = preg14 + (_UINT32)(8);
  if(__mplocal_j2 != preg31)
    goto _51;
  goto _49;
  _100 :;
  _49 :;
  __ompc_barrier();
  __ompv_gtid_s1 = __ompc_get_local_thread_num();
  preg119 = lastcol - firstcol;
  preg64 = preg119 + 1;
  __ompv_temp_do_upper3 = preg64;
  __ompv_temp_do_lower3 = 1;
  __ompc_static_init_4(__ompv_gtid_s1, 2, &__ompv_temp_do_lower3, &__ompv_temp_do_upper3, &__ompv_temp_do_stride3, 1, 1);
  __ompv_temp_do_upper3 = (__ompv_temp_do_upper3 > preg64 ? preg64 : __ompv_temp_do_upper3);
  __mplocal_j3 = __ompv_temp_do_lower3;
  if(!(__ompv_temp_do_upper3 >= __ompv_temp_do_lower3))
    goto _101;
  preg83 = (__ompv_temp_do_upper3 - __ompv_temp_do_lower3) + 1;
  preg25 = __ompv_temp_do_lower3;
  preg27 = __ompv_temp_do_upper3 + 1;
  preg69 = (_UINT64)(preg25) * (_UINT64)(8);
  preg103 = (_UINT32)(preg69);
  preg12 = (_UINT32)((_IEEE64 *)((_UINT8 *)(w0) + preg103));
  __mplocal_j3 = __mplocal_j3;
  _55 :;
  * ((_IEEE64 *) preg12) = 0.0;
  __mplocal_j3 = (_INT32)((_UINT64) __mplocal_j3) + 1;
  preg12 = preg12 + (_UINT32)(8);
  if(__mplocal_j3 != preg27)
    goto _55;
  goto _53;
  _101 :;
  _53 :;
  __ompv_gtid_s1 = __ompc_get_local_thread_num();
  __mplocal_d = 0.0;
  preg119 = lastcol - firstcol;
  preg62 = preg119 + 1;
  __ompv_temp_do_upper4 = preg62;
  __ompv_temp_do_lower4 = 1;
  __ompc_static_init_4(__ompv_gtid_s1, 2, &__ompv_temp_do_lower4, &__ompv_temp_do_upper4, &__ompv_temp_do_stride4, 1, 1);
  __ompv_temp_do_upper4 = (__ompv_temp_do_upper4 > preg62 ? preg62 : __ompv_temp_do_upper4);
  __mplocal_j4 = __ompv_temp_do_lower4;
  if(!(__ompv_temp_do_upper4 >= __ompv_temp_do_lower4))
    goto _102;
  preg86 = (__ompv_temp_do_upper4 - __ompv_temp_do_lower4) + 1;
  preg21 = __ompv_temp_do_lower4;
  preg23 = __ompv_temp_do_upper4 + 1;
  preg67 = (_UINT64)(preg21) * (_UINT64)(8);
  preg102 = (_UINT32)(preg67);
  preg8 = (_UINT32)((_IEEE64 *)((_UINT8 *)(p0) + preg102));
  preg10 = (_UINT32)((_IEEE64 *)((_UINT8 *)(q0) + preg102));
  __mplocal_j4 = __mplocal_j4;
  _59 :;
  __mplocal_d = __mplocal_d + (*((_IEEE64 *) preg8) ** ((_IEEE64 *) preg10));
  __mplocal_j4 = (_INT32)((_UINT64) __mplocal_j4) + 1;
  preg10 = preg10 + (_UINT32)(8);
  preg8 = preg8 + (_UINT32)(8);
  if(__mplocal_j4 != preg23)
    goto _59;
  goto _57;
  _102 :;
  _57 :;
  __ompc_barrier();
  __ompc_critical(__ompv_gtid_s1, (_UINT32) & __mplock_2);
  d = d + __mplocal_d;
  __ompc_end_critical(__ompv_gtid_s1, (_UINT32) & __mplock_2);
  __ompc_barrier();
  __ompv_gtid_s1 = __ompc_get_local_thread_num();
  mpsp_status1 = __ompc_single(__ompv_gtid_s1);
  if(!(mpsp_status1 == 1))
    goto _62;
  alpha = rho0 / d;
  goto _61;
  _62 :;
  _61 :;
  __ompc_end_single(__ompv_gtid_s1);
  __ompc_barrier();
  __ompv_gtid_s1 = __ompc_get_local_thread_num();
  preg119 = lastcol - firstcol;
  preg60 = preg119 + 1;
  __ompv_temp_do_upper5 = preg60;
  __ompv_temp_do_lower5 = 1;
  __ompc_static_init_4(__ompv_gtid_s1, 2, &__ompv_temp_do_lower5, &__ompv_temp_do_upper5, &__ompv_temp_do_stride5, 1, 1);
  __ompv_temp_do_upper5 = (__ompv_temp_do_upper5 > preg60 ? preg60 : __ompv_temp_do_upper5);
  __mplocal_j5 = __ompv_temp_do_lower5;
  if(!(__ompv_temp_do_upper5 >= __ompv_temp_do_lower5))
    goto _103;
  preg88 = (__ompv_temp_do_upper5 - __ompv_temp_do_lower5) + 1;
  preg17 = __ompv_temp_do_lower5;
  preg19 = __ompv_temp_do_upper5 + 1;
  preg65 = (_UINT64)(preg17) * (_UINT64)(8);
  preg101 = (_UINT32)(preg65);
  preg0 = (_UINT32)((_IEEE64 *)((_UINT8 *)(z0) + preg101));
  preg2 = (_UINT32)((_IEEE64 *)((_UINT8 *)(p0) + preg101));
  preg4 = (_UINT32)((_IEEE64 *)((_UINT8 *)(r0) + preg101));
  preg6 = (_UINT32)((_IEEE64 *)((_UINT8 *)(q0) + preg101));
  __mplocal_j5 = __mplocal_j5;
  _65 :;
  * ((_IEEE64 *) preg0) = *((_IEEE64 *) preg0) + (*((_IEEE64 *) preg2) * alpha);
  * ((_IEEE64 *) preg4) = *((_IEEE64 *) preg4) - (*((_IEEE64 *) preg6) * alpha);
  __mplocal_j5 = (_INT32)((_UINT64) __mplocal_j5) + 1;
  preg6 = preg6 + (_UINT32)(8);
  preg4 = preg4 + (_UINT32)(8);
  preg2 = preg2 + (_UINT32)(8);
  preg0 = preg0 + (_UINT32)(8);
  if(__mplocal_j5 != preg19)
    goto _65;
  goto _63;
  _103 :;
  _63 :;
  __ompc_barrier();
  __ompv_gtid_s1 = __ompc_get_local_thread_num();
  __mplocal_rho0 = 0.0;
  preg119 = lastcol - firstcol;
  preg58 = preg119 + 1;
  __ompv_temp_do_upper6 = preg58;
  __ompv_temp_do_lower6 = 1;
  __ompc_static_init_4(__ompv_gtid_s1, 2, &__ompv_temp_do_lower6, &__ompv_temp_do_upper6, &__ompv_temp_do_stride6, 1, 1);
  __ompv_temp_do_upper6 = (__ompv_temp_do_upper6 > preg58 ? preg58 : __ompv_temp_do_upper6);
  __mplocal_j6 = __ompv_temp_do_lower6;
  if(!(__ompv_temp_do_upper6 >= __ompv_temp_do_lower6))
    goto _104;
  preg90 = (__ompv_temp_do_upper6 - __ompv_temp_do_lower6) + 1;
  preg13 = __ompv_temp_do_lower6;
  preg15 = __ompv_temp_do_upper6 + 1;
  preg63 = (_UINT64)(preg13) * (_UINT64)(8);
  preg100 = (_UINT32)(preg63);
  preg125 = (_UINT32)((_IEEE64 *)((_UINT8 *)(r0) + preg100));
  __mplocal_j6 = __mplocal_j6;
  _69 :;
  preg41 = *((_IEEE64 *) preg125);
  __mplocal_rho0 = __mplocal_rho0 + (preg41 * preg41);
  __mplocal_j6 = (_INT32)((_UINT64) __mplocal_j6) + 1;
  preg125 = preg125 + (_UINT32)(8);
  if(__mplocal_j6 != preg15)
    goto _69;
  goto _67;
  _104 :;
  _67 :;
  __ompc_barrier();
  __ompc_critical(__ompv_gtid_s1, (_UINT32) & __mplock_3);
  rho = rho + __mplocal_rho0;
  __ompc_end_critical(__ompv_gtid_s1, (_UINT32) & __mplock_3);
  __ompc_barrier();
  __ompv_gtid_s1 = __ompc_get_local_thread_num();
  mpsp_status2 = __ompc_single(__ompv_gtid_s1);
  if(!(mpsp_status2 == 1))
    goto _72;
  beta = rho / rho0;
  goto _71;
  _72 :;
  _71 :;
  __ompc_end_single(__ompv_gtid_s1);
  __ompc_barrier();
  __ompv_gtid_s1 = __ompc_get_local_thread_num();
  preg119 = lastcol - firstcol;
  preg56 = preg119 + 1;
  __ompv_temp_do_upper7 = preg56;
  __ompv_temp_do_lower7 = 1;
  __ompc_static_init_4(__ompv_gtid_s1, 2, &__ompv_temp_do_lower7, &__ompv_temp_do_upper7, &__ompv_temp_do_stride7, 1, 1);
  __ompv_temp_do_upper7 = (__ompv_temp_do_upper7 > preg56 ? preg56 : __ompv_temp_do_upper7);
  __mplocal_j7 = __ompv_temp_do_lower7;
  if(!(__ompv_temp_do_upper7 >= __ompv_temp_do_lower7))
    goto _105;
  preg92 = (__ompv_temp_do_upper7 - __ompv_temp_do_lower7) + 1;
  preg9 = __ompv_temp_do_lower7;
  preg11 = __ompv_temp_do_upper7 + 1;
  preg61 = (_UINT64)(preg9) * (_UINT64)(8);
  preg98 = (_UINT32)(preg61);
  preg120 = (_UINT32)((_IEEE64 *)((_UINT8 *)(r0) + preg98));
  preg123 = (_UINT32)((_IEEE64 *)((_UINT8 *)(p0) + preg98));
  __mplocal_j7 = __mplocal_j7;
  _75 :;
  * ((_IEEE64 *) preg123) = *((_IEEE64 *) preg120) + (*((_IEEE64 *) preg123) * beta);
  __mplocal_j7 = (_INT32)((_UINT64) __mplocal_j7) + 1;
  preg123 = preg123 + (_UINT32)(8);
  preg120 = preg120 + (_UINT32)(8);
  if(__mplocal_j7 != preg11)
    goto _75;
  goto _73;
  _105 :;
  _73 :;
  __ompc_barrier();
  _3 :;
  cgit = cgit + 1;
  if(cgit != 26)
    goto _38;
  _77 :;
  __ompv_gtid_s1 = __ompc_get_local_thread_num();
  mpsp_status3 = __ompc_single(__ompv_gtid_s1);
  sum = (mpsp_status3 == 1 ? 0.0 : sum);
  __ompc_end_single(__ompv_gtid_s1);
  __ompv_gtid_s1 = __ompc_get_local_thread_num();
  preg121 = lastrow - firstrow;
  preg54 = preg121 + 1;
  __ompv_temp_do_upper8 = preg54;
  __ompv_temp_do_lower8 = 1;
  __ompc_static_init_4(__ompv_gtid_s1, 2, &__ompv_temp_do_lower8, &__ompv_temp_do_upper8, &__ompv_temp_do_stride8, 1, 1);
  __ompv_temp_do_upper8 = (__ompv_temp_do_upper8 > preg54 ? preg54 : __ompv_temp_do_upper8);
  __mplocal_j8 = __ompv_temp_do_lower8;
  if(!(__ompv_temp_do_upper8 >= __ompv_temp_do_lower8))
    goto _106;
  preg94 = (__ompv_temp_do_upper8 - __ompv_temp_do_lower8) + 1;
  preg3 = __ompv_temp_do_lower8;
  preg5 = __ompv_temp_do_upper8 + 1;
  preg53 = (_UINT64)(preg3) * (_UINT64)(4);
  preg55 = (_UINT64)(preg3) * (_UINT64)(8);
  preg89 = (_UINT32)(preg53);
  preg91 = (_UINT32)(preg55);
  preg115 = (_UINT32)((_INT32 *)((_UINT8 *)(rowstr0) + preg89));
  preg116 = (_UINT32)((_IEEE64 *)((_UINT8 *)(w0) + preg91));
  __mplocal_j8 = __mplocal_j8;
  _80 :;
  __mplocal_d0 = 0.0;
  __mplocal_k0 = *((_INT32 *) preg115);
  preg39 = *(((_INT32 *) preg115) + 1LL);
  preg48 = preg39 + -1;
  if(!(__mplocal_k0 <= preg48))
    goto _107;
  preg96 = _I4MAX(preg39 - __mplocal_k0, 1);
  preg7 = __mplocal_k0;
  preg57 = (_UINT64)(preg7) * (_UINT64)(4);
  preg59 = (_UINT64)(preg7) * (_UINT64)(8);
  preg93 = (_UINT32)(preg57);
  preg95 = (_UINT32)(preg59);
  preg117 = (_UINT32)((_INT32 *)((_UINT8 *)(colidx0) + preg93));
  preg118 = (_UINT32)((_IEEE64 *)((_UINT8 *)(a0) + preg95));
  _83 :;
  __mplocal_d0 = __mplocal_d0 + (*((_IEEE64 *) preg118) ** (_IEEE64 *)((_UINT8 *)(z0) + (_UINT32)(((_UINT64)(*((_INT32 *) preg117)) * (_UINT64)(8)))));
  _13 :;
  __mplocal_k0 = (_INT32)((_UINT64) __mplocal_k0) + 1;
  preg118 = preg118 + (_UINT32)(8);
  preg117 = preg117 + (_UINT32)(4);
  if(__mplocal_k0 <= preg48)
    goto _83;
  _84 :;
  goto _81;
  _107 :;
  _81 :;
  * ((_IEEE64 *) preg116) = __mplocal_d0;
  __mplocal_j8 = (_INT32)((_UINT64) __mplocal_j8) + 1;
  preg116 = preg116 + (_UINT32)(8);
  preg115 = preg115 + (_UINT32)(4);
  if(__mplocal_j8 != preg5)
    goto _80;
  goto _78;
  _106 :;
  _78 :;
  __ompc_barrier();
  __ompv_gtid_s1 = __ompc_get_local_thread_num();
  preg119 = lastcol - firstcol;
  preg52 = preg119 + 1;
  __ompv_temp_do_upper9 = preg52;
  __ompv_temp_do_lower9 = 1;
  __ompc_static_init_4(__ompv_gtid_s1, 2, &__ompv_temp_do_lower9, &__ompv_temp_do_upper9, &__ompv_temp_do_stride9, 1, 1);
  __ompv_temp_do_upper9 = (__ompv_temp_do_upper9 > preg52 ? preg52 : __ompv_temp_do_upper9);
  __mplocal_j9 = __ompv_temp_do_lower9;
  if(!(__ompv_temp_do_upper9 >= __ompv_temp_do_lower9))
    goto _108;
  preg97 = (__ompv_temp_do_upper9 - __ompv_temp_do_lower9) + 1;
  preg = __ompv_temp_do_lower9;
  preg1 = __ompv_temp_do_upper9 + 1;
  preg51 = (_UINT64)(preg) * (_UINT64)(8);
  preg87 = (_UINT32)(preg51);
  preg113 = (_UINT32)((_IEEE64 *)((_UINT8 *)(w0) + preg87));
  preg114 = (_UINT32)((_IEEE64 *)((_UINT8 *)(r0) + preg87));
  __mplocal_j9 = __mplocal_j9;
  _88 :;
  * ((_IEEE64 *) preg114) = *((_IEEE64 *) preg113);
  __mplocal_j9 = (_INT32)((_UINT64) __mplocal_j9) + 1;
  preg114 = preg114 + (_UINT32)(8);
  preg113 = preg113 + (_UINT32)(8);
  if(__mplocal_j9 != preg1)
    goto _88;
  goto _86;
  _108 :;
  _86 :;
  __ompc_barrier();
  __ompv_gtid_s1 = __ompc_get_local_thread_num();
  __mplocal_sum0 = 0.0;
  preg119 = lastcol - firstcol;
  preg50 = preg119 + 1;
  __ompv_temp_do_upper10 = preg50;
  __ompv_temp_do_lower10 = 1;
  __ompc_static_init_4(__ompv_gtid_s1, 2, &__ompv_temp_do_lower10, &__ompv_temp_do_upper10, &__ompv_temp_do_stride10, 1, 1);
  __ompv_temp_do_upper10 = (__ompv_temp_do_upper10 > preg50 ? preg50 : __ompv_temp_do_upper10);
  __mplocal_j10 = __ompv_temp_do_lower10;
  if(!(__ompv_temp_do_upper10 >= __ompv_temp_do_lower10))
    goto _109;
  preg99 = (__ompv_temp_do_upper10 - __ompv_temp_do_lower10) + 1;
  preg122 = __ompv_temp_do_lower10;
  preg124 = __ompv_temp_do_upper10 + 1;
  preg49 = (_UINT64)(preg122) * (_UINT64)(8);
  preg85 = (_UINT32)(preg49);
  preg111 = (_UINT32)((_IEEE64 *)((_UINT8 *)(x0) + preg85));
  preg112 = (_UINT32)((_IEEE64 *)((_UINT8 *)(r0) + preg85));
  __mplocal_j10 = __mplocal_j10;
  _92 :;
  __mplocal_d1 = *((_IEEE64 *) preg111) - *((_IEEE64 *) preg112);
  __mplocal_sum0 = __mplocal_sum0 + (__mplocal_d1 * __mplocal_d1);
  __mplocal_j10 = (_INT32)((_UINT64) __mplocal_j10) + 1;
  preg112 = preg112 + (_UINT32)(8);
  preg111 = preg111 + (_UINT32)(8);
  if(__mplocal_j10 != preg124)
    goto _92;
  goto _90;
  _109 :;
  _90 :;
  __ompc_barrier();
  __ompc_critical(__ompv_gtid_s1, (_UINT32) & __mplock_4);
  sum = sum + __mplocal_sum0;
  __ompc_end_critical(__ompv_gtid_s1, (_UINT32) & __mplock_4);
  __ompc_barrier();
  __ompv_gtid_s1 = __ompc_get_local_thread_num();
  mpsp_status4 = __ompc_single(__ompv_gtid_s1);
  if(!(mpsp_status4 == 1))
    goto _95;
  __comma = sqrt(sum);
  * rnorm = __comma;
  goto _94;
  _95 :;
  _94 :;
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
  register _UINT32 reg7;
  register _BOOLEAN preg17;
  register _INT32 preg14;
  register _INT32 preg19;
  register _INT64 preg22;
  register _UINT64 preg25;
  register _UINT32 preg28;
  register _UINT32 preg7;
  register _INT32 preg15;
  register _INT64 preg13;
  register _UINT64 preg12;
  register _UINT32 preg11;
  register _UINT32 preg;
  register _UINT32 preg10;
  register _UINT32 preg9;
  register _UINT32 preg3;
  register _UINT32 preg4;
  register _INT64 preg21;
  register _UINT64 preg23;
  register _UINT64 preg24;
  register _UINT32 preg26;
  register _UINT32 preg27;
  register _UINT32 preg0;
  register _UINT32 preg1;
  register _UINT32 preg2;
  register _UINT32 preg5;
  register _UINT32 preg6;
  register _INT32 preg8;
  register _INT32 preg16;
  register _INT32 preg18;
  register _INT32 preg20;
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
  
  /*Begin_of_nested_PU(s)*/
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
    goto _6;
  preg = n + __ompv_temp_do_lower;
  preg0 = (_INT64)(preg);
  preg1 = (_UINT64)(preg0) * (_UINT64)(4);
  preg2 = (_UINT32)(preg1);
  preg3 = (_UINT32)((_INT32 *)((_UINT8 *)(colidx0) + preg2));
  __mplocal_i = __mplocal_i;
  _4 :;
  * ((_INT32 *) preg3) = 0;
  __mplocal_i = __mplocal_i + 1;
  preg3 = preg3 + (_UINT32)(4);
  if(__mplocal_i <= __ompv_temp_do_upper)
    goto _4;
  goto _2;
  _6 :;
  _2 :;
  return;
} /* __ompdo_makea1 */
  
  size = 1.0;
  ratio = pow(rcond, _F8RECIP((_IEEE64)(n)));
  nnza = 0;
  __ompv_ok_to_fork = __ompc_can_fork();
  if(!(n > 1))
    goto _43;
  if(!(__ompv_ok_to_fork == 1))
    goto _44;
  __ompc_fork(0, &__ompdo_makea1, reg7);
  preg17 = n >= 1;
  goto _6;
  _43 :;
  goto _7;
  _44 :;
  _7 :;
  __ompc_get_local_thread_num();
  __ompc_serialized_parallel();
  i = 1;
  preg17 = n >= 1;
  if(!(preg17))
    goto _45;
  preg14 = n;
  preg19 = n + 1;
  preg22 = (_INT64)(preg19);
  preg25 = (_UINT64)(preg22) * (_UINT64)(4);
  preg28 = (_UINT32)(preg25);
  preg7 = (_UINT32)((_INT32 *)((_UINT8 *)(colidx0) + preg28));
  i = i;
  _17 :;
  * ((_INT32 *) preg7) = 0;
  i = i + 1;
  preg7 = preg7 + (_UINT32)(4);
  if(i != preg19)
    goto _17;
  goto _15;
  _45 :;
  _15 :;
  __ompc_end_serialized_parallel();
  _6 :;
  iouter = 1;
  if(!(preg17))
    goto _46;
  preg15 = n;
  preg19 = n + 1;
  preg13 = n;
  preg12 = (_UINT64)(preg13) * (_UINT64)(4);
  preg11 = (_UINT32)(preg12);
  preg = (_UINT32)((_INT32 *)((_UINT8 *)(colidx0) + preg11));
  preg10 = (_UINT32)(iv0 + 1);
  preg9 = (_UINT32)(v0 + 1);
  _21 :;
  nzv = nonzer;
  sprnvc(n, nonzer, v0, iv0, colidx0, (_INT32 *) preg);
  vecset(n, v0, iv0, &nzv, iouter, 5.0e-01);
  ivelt = 1;
  if(!(nzv >= 1))
    goto _47;
  preg3 = preg10;
  preg4 = preg9;
  _24 :;
  jcol = *((_INT32 *) preg3);
  if(!(firstcol0 <= jcol))
    goto _48;
  if(!(lastcol0 >= jcol))
    goto _49;
  scale = *((_IEEE64 *) preg4) * size;
  ivelt1 = 1;
  if(!(nzv >= 1))
    goto _50;
  preg21 = nnza;
  preg23 = (_UINT64)(preg21) * (_UINT64)(4);
  preg24 = (_UINT64)(preg21) * (_UINT64)(8);
  preg26 = (_UINT32)(preg23);
  preg27 = (_UINT32)(preg24);
  preg0 = (_UINT32)((_INT32 *)((_UINT8 *)(acol0) + preg26));
  preg1 = (_UINT32)((_INT32 *)((_UINT8 *)(arow0) + preg26));
  preg2 = (_UINT32)((_IEEE64 *)((_UINT8 *)(aelt0) + preg27));
  preg5 = preg10;
  preg6 = preg9;
  _41 :;
  _42 :;
  irow = *((_INT32 *) preg5);
  if(!(firstrow0 <= irow))
    goto _51;
  if(!(lastrow0 >= irow))
    goto _52;
  nnza = nnza + 1;
  preg2 = preg2 + (_UINT32)(8);
  preg1 = preg1 + (_UINT32)(4);
  preg0 = preg0 + (_UINT32)(4);
  if(!(nz < nnza))
    goto _40;
  printf((_INT8 *) "Space for matrix elements exceeded in makea\n");
  printf((_INT8 *) "nnza, nzmax = %d, %d\n", nnza, nz);
  printf((_INT8 *) "iouter = %d\n", iouter);
  exit(1);
  preg21 = nnza;
  preg23 = (_UINT64)(preg21) * (_UINT64)(4);
  preg26 = (_UINT32)(preg23);
  preg0 = (_UINT32)((_INT32 *)((_UINT8 *)(acol0) + preg26));
  * ((_INT32 *) preg0) = jcol;
  preg1 = (_UINT32)((_INT32 *)((_UINT8 *)(arow0) + preg26));
  * ((_INT32 *) preg1) = irow;
  preg24 = (_UINT64)(preg21) * (_UINT64)(8);
  preg27 = (_UINT32)(preg24);
  preg2 = (_UINT32)((_IEEE64 *)((_UINT8 *)(aelt0) + preg27));
  * ((_IEEE64 *) preg2) = *((_IEEE64 *) preg6) * scale;
  preg8 = (_INT32)((_UINT64) ivelt1) + 1;
  ivelt1 = preg8;
  preg6 = preg6 + (_UINT32)(8);
  preg5 = preg5 + (_UINT32)(4);
  if(nzv >= preg8)
    goto _41;
  _30 :;
  goto _25;
  _40 :;
  * ((_INT32 *) preg0) = jcol;
  * ((_INT32 *) preg1) = irow;
  * ((_IEEE64 *) preg2) = *((_IEEE64 *) preg6) * scale;
  goto _39;
  _51 :;
  goto _39;
  _52 :;
  _39 :;
  preg8 = (_INT32)((_UINT64) ivelt1) + 1;
  ivelt1 = preg8;
  preg6 = preg6 + (_UINT32)(8);
  preg5 = preg5 + (_UINT32)(4);
  if(nzv >= preg8)
    goto _42;
  goto _30;
  _50 :;
  _25 :;
  goto _9;
  _48 :;
  goto _9;
  _49 :;
  _9 :;
  _3 :;
  ivelt = (_INT32)((_UINT64) ivelt) + 1;
  preg4 = preg4 + (_UINT32)(8);
  preg3 = preg3 + (_UINT32)(4);
  if(ivelt <= nzv)
    goto _24;
  _31 :;
  goto _22;
  _47 :;
  _22 :;
  size = size * ratio;
  _2 :;
  iouter = iouter + 1;
  if(iouter != preg19)
    goto _21;
  _32 :;
  goto _19;
  _46 :;
  preg13 = n;
  preg12 = (_UINT64)(preg13) * (_UINT64)(4);
  preg11 = (_UINT32)(preg12);
  _19 :;
  i = firstrow0;
  if(!(firstrow0 <= lastrow0))
    goto _53;
  preg16 = (lastrow0 - firstrow0) + 1;
  preg18 = n + firstrow0;
  preg20 = lastrow0 + 1;
  preg21 = nnza;
  preg23 = (_UINT64)(preg21) * (_UINT64)(4);
  preg24 = (_UINT64)(preg21) * (_UINT64)(8);
  preg26 = (_UINT32)(preg23);
  preg27 = (_UINT32)(preg24);
  preg0 = (_UINT32)((_INT32 *)((_UINT8 *)(acol0) + preg26));
  preg1 = (_UINT32)((_INT32 *)((_UINT8 *)(arow0) + preg26));
  preg2 = (_UINT32)((_IEEE64 *)((_UINT8 *)(aelt0) + preg27));
  _35 :;
  if(!(firstcol0 <= i))
    goto _54;
  if(!(lastcol0 >= i))
    goto _55;
  nnza = nnza + 1;
  preg2 = preg2 + (_UINT32)(8);
  preg1 = preg1 + (_UINT32)(4);
  preg0 = preg0 + (_UINT32)(4);
  if(!(nz < nnza))
    goto _37;
  printf((_INT8 *) "Space for matrix elements exceeded in makea\n");
  printf((_INT8 *) "nnza, nzmax = %d, %d\n", nnza, nz);
  printf((_INT8 *) "iouter = %d\n", preg18);
  exit(1);
  goto _36;
  _37 :;
  _36 :;
  * ((_INT32 *) preg0) = i;
  * ((_INT32 *) preg1) = i;
  * ((_IEEE64 *) preg2) = rcond - shift;
  goto _13;
  _54 :;
  goto _13;
  _55 :;
  _13 :;
  _5 :;
  i = i + 1;
  preg18 = preg18 + 1;
  if(i != preg20)
    goto _35;
  _38 :;
  goto _33;
  _53 :;
  _33 :;
  sparse(a0, colidx0, rowstr0, n, arow0, acol0, aelt0, firstrow0, lastrow0, v0, iv0, (_INT32 *)((_UINT8 *)(iv0) + preg11), nnza);
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
  register _INT32 __ompv_ok_to_fork;
  register _BOOLEAN preg31;
  register _UINT32 reg7;
  register _INT32 preg23;
  register _INT32 preg33;
  register _UINT32 preg4;
  register _UINT32 preg5;
  register _BOOLEAN preg34;
  register _INT32 preg24;
  register _INT32 preg22;
  register _UINT32 preg15;
  register _INT64 preg32;
  register _UINT64 preg39;
  register _UINT32 preg;
  register _INT32 preg35;
  register _INT32 preg40;
  register _INT32 preg21;
  register _INT32 preg25;
  register _INT32 preg41;
  register _INT32 preg26;
  register _UINT32 preg13;
  register _UINT32 preg16;
  register _INT64 preg37;
  register _INT32 preg27;
  register _INT32 __ompv_ok_to_fork0;
  register _INT32 preg28;
  register _UINT32 preg6;
  register _UINT32 preg7;
  register _INT32 preg29;
  register _UINT32 preg10;
  register _UINT32 preg12;
  register _UINT32 preg14;
  register _INT32 preg18;
  register _UINT64 preg44;
  register _UINT64 preg45;
  register _UINT32 preg2;
  register _UINT32 preg3;
  register _UINT32 preg8;
  register _UINT32 preg11;
  register _UINT32 preg17;
  register _INT64 preg36;
  register _UINT64 preg42;
  register _UINT32 preg0;
  register _IEEE64 preg19;
  register _UINT64 preg43;
  register _UINT32 preg1;
  register _INT32 preg20;
  register _INT32 preg30;
  register _INT32 preg38;
  register _UINT32 preg9;
  _INT32 i;
  _INT32 j;
  _INT32 jajp1;
  _INT32 nza;
  _INT32 k;
  _INT32 nzrow;
  _IEEE64 xi;
  
  /*Begin_of_nested_PU(s)*/
static void __ompdo_sparse2(__ompv_gtid_a, __ompv_slink_a)
  _INT32 __ompv_gtid_a;
  _UINT32 __ompv_slink_a;
{
  register _INT64 preg;
  register _UINT64 preg0;
  register _UINT64 preg1;
  register _UINT32 preg2;
  register _UINT32 preg3;
  register _UINT32 preg4;
  register _UINT32 preg5;
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
    goto _6;
  preg = __ompv_temp_do_lower;
  preg0 = (_UINT64)(preg) * (_UINT64)(8);
  preg1 = (_UINT64)(preg) * (_UINT64)(4);
  preg2 = (_UINT32)(preg0);
  preg3 = (_UINT32)(preg1);
  preg4 = (_UINT32)((_IEEE64 *)((_UINT8 *)(x0) + preg2));
  preg5 = (_UINT32)((_INT32 *)((_UINT8 *)(mark) + preg3));
  __mplocal_i = __mplocal_i;
  _4 :;
  * ((_IEEE64 *) preg4) = 0.0;
  * ((_INT32 *) preg5) = 0;
  __mplocal_i = (_INT32)((_UINT64) __mplocal_i) + 1;
  preg5 = preg5 + (_UINT32)(4);
  preg4 = preg4 + (_UINT32)(8);
  if(__mplocal_i <= __ompv_temp_do_upper)
    goto _4;
  goto _2;
  _6 :;
  _2 :;
  return;
} /* __ompdo_sparse2 */
static void __ompdo_sparse1(__ompv_gtid_a, __ompv_slink_a)
  _INT32 __ompv_gtid_a;
  _UINT32 __ompv_slink_a;
{
  register _INT64 preg;
  register _UINT64 preg0;
  register _UINT32 preg1;
  register _UINT32 preg2;
  register _UINT32 preg3;
  _INT32 __mplocal_j;
  _INT32 __ompv_temp_do_upper;
  _INT32 __ompv_temp_do_lower;
  _INT32 __ompv_temp_do_stride;
  
  __ompv_temp_do_upper = n;
  __ompv_temp_do_lower = 1;
  __ompc_static_init_4(__ompv_gtid_a, 2, &__ompv_temp_do_lower, &__ompv_temp_do_upper, &__ompv_temp_do_stride, 1, 1);
  __ompv_temp_do_upper = (n < __ompv_temp_do_upper ? n : __ompv_temp_do_upper);
  __mplocal_j = __ompv_temp_do_lower;
  if(!(__ompv_temp_do_upper >= __ompv_temp_do_lower))
    goto _6;
  preg = __ompv_temp_do_lower;
  preg0 = (_UINT64)(preg) * (_UINT64)(4);
  preg1 = (_UINT32)(preg0);
  preg2 = (_UINT32)((_INT32 *)((_UINT8 *)(rowstr0) + preg1));
  preg3 = (_UINT32)((_INT32 *)((_UINT8 *)(mark) + preg1));
  __mplocal_j = __mplocal_j;
  _4 :;
  * ((_INT32 *) preg2) = 0;
  * ((_INT32 *) preg3) = 0;
  __mplocal_j = (_INT32)((_UINT64) __mplocal_j) + 1;
  preg3 = preg3 + (_UINT32)(4);
  preg2 = preg2 + (_UINT32)(4);
  if(__mplocal_j <= __ompv_temp_do_upper)
    goto _4;
  goto _2;
  _6 :;
  _2 :;
  return;
} /* __ompdo_sparse1 */
  
  __ompv_ok_to_fork = __ompc_can_fork();
  preg31 = n > 1;
  if(!(preg31))
    goto _57;
  if(!(__ompv_ok_to_fork == 1))
    goto _58;
  __ompc_fork(0, &__ompdo_sparse1, reg7);
  preg31 = (_INT32)((_UINT64) n) > 1;
  goto _10;
  _57 :;
  goto _11;
  _58 :;
  _11 :;
  __ompc_get_local_thread_num();
  __ompc_serialized_parallel();
  j = 1;
  if(!(n >= 1))
    goto _59;
  preg23 = n;
  preg33 = n + 1;
  preg4 = (_UINT32)(rowstr0 + 1);
  preg5 = (_UINT32)(mark + 1);
  j = j;
  _20 :;
  * ((_INT32 *) preg4) = 0;
  * ((_INT32 *) preg5) = 0;
  j = (_INT32)((_UINT64) j) + 1;
  preg5 = preg5 + (_UINT32)(4);
  preg4 = preg4 + (_UINT32)(4);
  if(j != preg33)
    goto _20;
  goto _18;
  _59 :;
  _18 :;
  __ompc_end_serialized_parallel();
  _10 :;
  * ((_INT32 *)((_UINT8 *)(rowstr0) + (_UINT32)(((_UINT64) n * (_UINT64)(4)))) + 1LL) = 0;
  nza = 1;
  preg34 = nnza >= 1;
  if(!(preg34))
    goto _60;
  preg24 = nnza;
  preg22 = nnza + 1;
  preg15 = (_UINT32)(arow0 + 1);
  _24 :;
  j = (*((_INT32 *) preg15) - firstrow0) + 2;
  preg32 = j;
  preg39 = (_UINT64)(preg32) * (_UINT64)(4);
  preg = (_UINT32)(preg39);
  preg4 = (_UINT32)((_INT32 *)((_UINT8 *)(rowstr0) + preg));
  * ((_INT32 *) preg4) = *((_INT32 *) preg4) + 1;
  _2 :;
  nza = (_INT32)((_UINT64) nza) + 1;
  preg15 = preg15 + (_UINT32)(4);
  if(nza != preg22)
    goto _24;
  _25 :;
  goto _22;
  _60 :;
  _22 :;
  * (rowstr0 + 1LL) = 1;
  j = 2;
  preg35 = lastrow0 - firstrow0;
  preg40 = preg35 + 2;
  preg21 = preg35 + 1;
  if(!(preg40 >= 2))
    goto _61;
  preg25 = preg21;
  preg41 = preg35 + 3;
  preg4 = (_UINT32)(rowstr0 + 2);
  _28 :;
  * ((_INT32 *) preg4) = *(((_INT32 *) preg4) + -1LL) + *((_INT32 *) preg4);
  _3 :;
  j = (_INT32)((_UINT64) j) + 1;
  preg4 = preg4 + (_UINT32)(4);
  if(j != preg41)
    goto _28;
  _29 :;
  goto _26;
  _61 :;
  _26 :;
  nza = 1;
  if(!(preg34))
    goto _62;
  preg26 = nnza;
  preg22 = nnza + 1;
  preg13 = (_UINT32)(aelt0 + 1);
  preg15 = (_UINT32)(arow0 + 1);
  preg16 = (_UINT32)(acol0 + 1);
  _32 :;
  j = (*((_INT32 *) preg15) - firstrow0) + 1;
  preg32 = j;
  preg39 = (_UINT64)(preg32) * (_UINT64)(4);
  preg = (_UINT32)(preg39);
  preg4 = (_UINT32)((_INT32 *)((_UINT8 *)(rowstr0) + preg));
  k = *((_INT32 *) preg4);
  preg37 = k;
  * (_IEEE64 *)((_UINT8 *)(a0) + (_UINT32)(((_UINT64)(preg37) * (_UINT64)(8)))) = *((_IEEE64 *) preg13);
  * (_INT32 *)((_UINT8 *)(colidx0) + (_UINT32)(((_UINT64)(preg37) * (_UINT64)(4)))) = *((_INT32 *) preg16);
  * ((_INT32 *) preg4) = *((_INT32 *) preg4) + 1;
  _4 :;
  nza = (_INT32)((_UINT64) nza) + 1;
  preg16 = preg16 + (_UINT32)(4);
  preg15 = preg15 + (_UINT32)(4);
  preg13 = preg13 + (_UINT32)(8);
  if(nza != preg22)
    goto _32;
  _33 :;
  goto _30;
  _62 :;
  _30 :;
  j = preg21;
  if(!(preg21 > 0))
    goto _63;
  preg27 = preg21;
  preg32 = (_INT64)(preg21);
  preg39 = (_UINT64)(preg32) * (_UINT64)(4);
  preg = (_UINT32)(preg39);
  preg4 = (_UINT32)((_INT32 *)((_UINT8 *)(rowstr0) + preg));
  _36 :;
  * (((_INT32 *) preg4) + 1LL) = *((_INT32 *) preg4);
  _5 :;
  j = (_INT32)((_UINT64) j) + -1;
  preg4 = preg4 + (_UINT32)(-4);
  if(j != 0)
    goto _36;
  _37 :;
  goto _34;
  _63 :;
  _34 :;
  * (rowstr0 + 1LL) = 1;
  nza = 0;
  __ompv_ok_to_fork0 = __ompc_can_fork();
  if(!(preg31))
    goto _64;
  if(!(__ompv_ok_to_fork0 == 1))
    goto _65;
  __ompc_fork(0, &__ompdo_sparse2, reg7);
  goto _13;
  _64 :;
  goto _14;
  _65 :;
  _14 :;
  __ompc_get_local_thread_num();
  __ompc_serialized_parallel();
  i = 1;
  if(!((_INT32)((_UINT64) n) >= 1))
    goto _66;
  preg28 = (_UINT64) n;
  preg33 = (_INT32)((_UINT64) n) + 1;
  preg6 = (_UINT32)(x0 + 1);
  preg7 = (_UINT32)(mark + 1);
  i = i;
  _40 :;
  * ((_IEEE64 *) preg6) = 0.0;
  * ((_INT32 *) preg7) = 0;
  i = (_INT32)((_UINT64) i) + 1;
  preg7 = preg7 + (_UINT32)(4);
  preg6 = preg6 + (_UINT32)(8);
  if(i != preg33)
    goto _40;
  goto _38;
  _66 :;
  _38 :;
  __ompc_end_serialized_parallel();
  _13 :;
  jajp1 = *(rowstr0 + 1LL);
  j = 1;
  if(!(preg21 >= 1))
    goto _67;
  preg29 = preg21;
  preg4 = (_UINT32)(rowstr0 + 1);
  preg10 = (_UINT32)(nzloc + 1);
  preg12 = (_UINT32)(a0);
  preg14 = (_UINT32)(colidx0);
  _44 :;
  nzrow = 0;
  k = jajp1;
  preg18 = *(((_INT32 *) preg4) + 1LL);
  if(!(jajp1 < preg18))
    goto _56;
  preg37 = jajp1;
  preg44 = (_UINT64)(preg37) * (_UINT64)(4);
  preg45 = (_UINT64)(preg37) * (_UINT64)(8);
  preg2 = (_UINT32)(preg44);
  preg3 = (_UINT32)(preg45);
  preg8 = (_UINT32)((_INT32 *)((_UINT8 *)(colidx0) + preg2));
  preg11 = (_UINT32)((_IEEE64 *)((_UINT8 *)(a0) + preg3));
  preg17 = (_UINT32)(nzloc);
  _47 :;
  i = *((_INT32 *) preg8);
  preg36 = i;
  preg42 = (_UINT64)(preg36) * (_UINT64)(8);
  preg0 = (_UINT32)(preg42);
  preg6 = (_UINT32)((_IEEE64 *)((_UINT8 *)(x0) + preg0));
  preg19 = *((_IEEE64 *) preg6) + *((_IEEE64 *) preg11);
  * ((_IEEE64 *) preg6) = preg19;
  preg43 = (_UINT64)(preg36) * (_UINT64)(4);
  preg1 = (_UINT32)(preg43);
  preg7 = (_UINT32)((_INT32 *)((_UINT8 *)(mark) + preg1));
  if(!(*((_INT32 *) preg7) == 0))
    goto _68;
  if(!(preg19 != 0.0))
    goto _69;
  * ((_INT32 *) preg7) = 1;
  nzrow = nzrow + 1;
  preg17 = preg17 + (_UINT32)(4);
  * ((_INT32 *) preg17) = i;
  preg18 = *(((_INT32 *) preg4) + 1LL);
  goto _16;
  _68 :;
  goto _16;
  _69 :;
  _16 :;
  _8 :;
  k = (_INT32)((_UINT64) k) + 1;
  preg11 = preg11 + (_UINT32)(8);
  preg8 = preg8 + (_UINT32)(4);
  if(k < preg18)
    goto _47;
  _48 :;
  preg20 = nzrow;
  if(!(preg20 >= 1))
    goto _70;
  _50 :;
  k = 1;
  preg30 = preg20;
  preg38 = preg20 + 1;
  preg9 = preg10;
  goto _51;
  _56 :;
  goto _49;
  _51 :;
  i = *((_INT32 *) preg9);
  preg36 = i;
  * (_INT32 *)((_UINT8 *)(mark) + (_UINT32)(((_UINT64)(preg36) * (_UINT64)(4)))) = 0;
  preg42 = (_UINT64)(preg36) * (_UINT64)(8);
  preg0 = (_UINT32)(preg42);
  preg6 = (_UINT32)((_IEEE64 *)((_UINT8 *)(x0) + preg0));
  xi = *((_IEEE64 *) preg6);
  * ((_IEEE64 *) preg6) = 0.0;
  if(!(xi != 0.0))
    goto _53;
  nza = nza + 1;
  preg14 = preg14 + (_UINT32)(4);
  preg12 = preg12 + (_UINT32)(8);
  * ((_IEEE64 *) preg12) = xi;
  * ((_INT32 *) preg14) = i;
  goto _52;
  _53 :;
  _52 :;
  _9 :;
  k = (_INT32)((_UINT64) k) + 1;
  preg9 = preg9 + (_UINT32)(4);
  if(k != preg38)
    goto _51;
  _54 :;
  preg18 = *(((_INT32 *) preg4) + 1LL);
  goto _49;
  _70 :;
  _49 :;
  jajp1 = preg18;
  * (((_INT32 *) preg4) + 1LL) = *(rowstr0 + 1LL) + nza;
  _7 :;
  j = (_INT32)((_UINT64) j) + 1;
  preg4 = preg4 + (_UINT32)(4);
  if(j != preg40)
    goto _44;
  _55 :;
  goto _42;
  _67 :;
  _42 :;
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
  register _UINT32 preg7;
  register _UINT32 preg8;
  register _UINT32 preg9;
  register _IEEE64 __comma;
  register _IEEE64 __comma0;
  register _INT32 __comma1;
  register _INT32 preg1;
  register _UINT64 preg2;
  register _UINT64 preg4;
  register _UINT32 preg5;
  register _UINT32 preg6;
  register _INT32 preg11;
  register _INT32 preg;
  register _INT32 preg0;
  register _INT32 preg3;
  register _UINT32 preg10;
  _INT32 nn1;
  _INT32 nzrow;
  _INT32 nzv;
  _INT32 ii;
  
  nzv = 0;
  nzrow = 0;
  nn1 = 1;
  _25 :;
  nn1 = nn1 * 2;
  if(n > nn1)
    goto _25;
  if(!(nz > 0))
    goto _39;
  preg7 = (_UINT32)(nzloc);
  preg8 = (_UINT32)(v0);
  preg9 = (_UINT32)(iv0);
  _29 :;
  __comma = randlc(&tran, amult);
  __comma0 = randlc(&tran, amult);
  __comma1 = icnvrt(__comma0, nn1);
  preg1 = __comma1 + 1;
  if(!(n < preg1))
    goto _31;
  goto _2;
  _31 :;
  preg2 = (_UINT64)(__comma1) * (_UINT64)(4);
  preg4 = preg2 + (_UINT64)(4);
  preg5 = (_UINT32)(preg4);
  preg6 = (_UINT32)((_INT32 *)((_UINT8 *)(mark) + preg5));
  if(!(*((_INT32 *) preg6) == 0))
    goto _33;
  * ((_INT32 *) preg6) = 1;
  preg11 = nzrow + 1;
  nzrow = preg11;
  preg7 = preg7 + (_UINT32)(4);
  * ((_INT32 *) preg7) = preg1;
  nzv = preg11;
  preg9 = preg9 + (_UINT32)(4);
  preg8 = preg8 + (_UINT32)(8);
  * ((_IEEE64 *) preg8) = __comma;
  * ((_INT32 *) preg9) = preg1;
  goto _32;
  _33 :;
  _32 :;
  _2 :;
  preg = nzv;
  if(nz > preg)
    goto _29;
  if(!(preg >= 1))
    goto _40;
  _36 :;
  ii = 1;
  preg0 = preg;
  preg3 = preg + 1;
  preg10 = (_UINT32)(nzloc + 1);
  goto _37;
  _39 :;
  goto _35;
  _37 :;
  * (_INT32 *)((_UINT8 *)(mark) + (_UINT32)(((_UINT64)(*((_INT32 *) preg10)) * (_UINT64)(4)))) = 0;
  _3 :;
  ii = (_INT32)((_UINT64) ii) + 1;
  preg10 = preg10 + (_UINT32)(4);
  if(ii != preg3)
    goto _37;
  _38 :;
  goto _35;
  _40 :;
  _35 :;
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
  register _INT32 preg2;
  register _INT32 preg3;
  register _UINT32 preg0;
  register _UINT32 preg1;
  register _INT32 preg;
  _INT32 k;
  _INT32 set;
  
  set = 0;
  k = 1;
  preg2 = *nzv;
  if(!(preg2 >= 1))
    goto _10;
  preg3 = _I4MAX(preg2, 1);
  preg0 = (_UINT32)(iv0 + 1);
  preg1 = (_UINT32)(v0 + 1);
  _4 :;
  if(!(*((_INT32 *) preg0) == i))
    goto _6;
  * ((_IEEE64 *) preg1) = val;
  set = 1;
  goto _5;
  _6 :;
  _5 :;
  _1 :;
  k = (_INT32)((_UINT64) k) + 1;
  preg1 = preg1 + (_UINT32)(8);
  preg0 = preg0 + (_UINT32)(4);
  if(k <= preg2)
    goto _4;
  _7 :;
  if(!(set == 0))
    goto _9;
  _11 :;
  preg = preg2 + 1;
  * nzv = preg;
  * (_IEEE64 *)((_UINT8 *)(v0) + (_UINT32)(((_UINT64)((_INT64)(preg)) * (_UINT64)(8)))) = val;
  * (_INT32 *)((_UINT8 *)(iv0) + (_UINT32)(((_UINT64)(preg) * (_UINT64)(4)))) = i;
  goto _8;
  _10 :;
  goto _11;
  _9 :;
  _8 :;
  return;
} /* vecset */

