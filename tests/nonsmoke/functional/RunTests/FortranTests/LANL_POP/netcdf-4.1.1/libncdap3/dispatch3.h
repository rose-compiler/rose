/*********************************************************************
 *   Copyright 1993, UCAR/Unidata
 *   See netcdf/COPYRIGHT file for copying and redistribution conditions.
 *   $Header: /upc/share/CVS/netcdf-3/libncdap3/dispatch3.h,v 1.9 2009/11/29 00:16:26 dmh Exp $
 *********************************************************************/
#ifndef DISPATCH_H
#define DISPATCH_H 1

/**************************************************/
/* Eventually add support for e.g nc_get_var_ulong */
#undef NCULONG


/**************************************************/
/* Dispatch Macros */

#define CONST(t) const t

#define SENTRY0(f)    const char* (*f)(void)
#define SENTRY1(f,a1) const char* (*f)(at)

#define ENTRY0(f)             int (*f)(void)
#define ENTRY1(f,a1)          int (*f)(a1)
#define ENTRY2(f,a1,a2)       int (*f)(a1,a2)
#define ENTRY3(f,a1,a2,a3)    int (*f)(a1,a2,a3)
#define ENTRY4(f,a1,a2,a3,a4) int (*f)(a1,a2,a3,a4)
#define ENTRY5(f,a1,a2,a3,a4,a5) int (*f)(a1,a2,a3,a4,a5)
#define ENTRY6(f,a1,a2,a3,a4,a5,a6) int (*f)(a1,a2,a3,a4,a5,a6)
#define ENTRY7(f,a1,a2,a3,a4,a5,a6,a7) int (*f)(a1,a2,a3,a4,a5,a6,a7)


#define GETATT(f,arg)  ENTRY4(f,int,int,CONST(char*),arg)
#define GETVAR1(f,arg) ENTRY4(f,int,int,CONST(size_t*),arg)
#define GETVARA(f,arg) ENTRY5(f,int,int,CONST(size_t*),CONST(size_t*),arg)
#define GETVARS(f,arg) ENTRY6(f,int,int,CONST(size_t*),CONST(size_t*),CONST(ptrdiff_t*),arg)
#define GETVARM(f,arg) ENTRY7(f,int,int,CONST(size_t*),CONST(size_t*),CONST(ptrdiff_t*),CONST(ptrdiff_t*),arg)
#define GETVAR(f,arg)  ENTRY3(f,int,int,arg)

/**************************************************/

typedef struct NC_Dispatch3 {
ENTRY1(close,int);
ENTRY1(sync,int);
ENTRY1(abort,int);

ENTRY1(redef,int);
ENTRY5(_enddef,int,size_t,size_t,size_t,size_t);
ENTRY1(enddef,int);

GETVAR1(get_var1,void*);
GETVAR1(get_var1_text,char*);
GETVAR1(get_var1_uchar,unsigned char*);
GETVAR1(get_var1_schar,signed char*);
GETVAR1(get_var1_short,short*);
GETVAR1(get_var1_int,int*);
GETVAR1(get_var1_long,long*);
#ifdef NCULONG
GETVAR1(get_var1_ulong,unsigned long*);
#endif
GETVAR1(get_var1_float,float*);
GETVAR1(get_var1_double,double*);

GETVARA(get_vara,void*);
GETVARA(get_vara_text,char*);
GETVARA(get_vara_uchar,unsigned char*);
GETVARA(get_vara_schar,signed char*);
GETVARA(get_vara_short,short*);
GETVARA(get_vara_int,int*);
GETVARA(get_vara_long,long*);
#ifdef NCULONG
GETVARA(get_vara_ulong,unsigned long*);
#endif
GETVARA(get_vara_float,float*);
GETVARA(get_vara_double,double*);

GETVARS(get_vars,void*);
GETVARS(get_vars_text,char*);
GETVARS(get_vars_uchar,unsigned char*);
GETVARS(get_vars_schar,signed char*);
GETVARS(get_vars_short,short*);
GETVARS(get_vars_int,int*);
GETVARS(get_vars_long,long*);
#ifdef NCULONG
GETVARS(get_vars_ulong,unsigned long*);
#endif
GETVARS(get_vars_float,float*);
GETVARS(get_vars_double,double*);

GETVARM(get_varm,void*);
GETVARM(get_varm_text,char*);
GETVARM(get_varm_uchar,unsigned char*);
GETVARM(get_varm_schar,signed char*);
GETVARM(get_varm_short,short*);
GETVARM(get_varm_int,int*);
GETVARM(get_varm_long,long*);
#ifdef NCULONG
GETVARM(get_varm_ulong,unsigned long*);
#endif
GETVARM(get_varm_float,float*);
GETVARM(get_varm_double,double*);

/* For some reason, this is apparently not defined*/
/*GETVAR(get_var,void*);*/
GETVAR(get_var_text,char*);
GETVAR(get_var_uchar,unsigned char*);
GETVAR(get_var_schar,signed char*);
GETVAR(get_var_short,short*);
GETVAR(get_var_int,int*);
GETVAR(get_var_long,long*);
#ifdef NCULONG
GETVAR(get_var_ulong,unsigned long*);
#endif
GETVAR(get_var_float,float*);
GETVAR(get_var_double,double*);

} NC_Dispatch3;

/* The two defined dispatch tables*/
extern NC_Dispatch3 netcdf3lib;
extern NC_Dispatch3 ncdap3lib;

#endif /*DISPATCH_H*/
