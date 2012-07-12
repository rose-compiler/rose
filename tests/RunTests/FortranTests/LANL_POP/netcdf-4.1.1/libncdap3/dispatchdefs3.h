/*********************************************************************
 *   Copyright 1993, UCAR/Unidata
 *   See netcdf/COPYRIGHT file for copying and redistribution conditions.
 *   $Header: /upc/share/CVS/netcdf-3/libncdap3/dispatchdefs3.h,v 1.4 2009/09/23 22:26:04 dmh Exp $
 *********************************************************************/
#ifndef DISPATCHDEFS_H
#define DISPATCHDEFS_H 1

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
#define PUTATT(f,arg)  ENTRY6(f,int,int,CONST(char*),nc_type,size_t,CONST(arg))
#define PUTATTX(f)     ENTRY5(f,int,int,CONST(char*),size_t,CONST(char*))
#define PUTVAR1(f,arg) ENTRY4(f,int,int,CONST(size_t*),const arg)
#define PUTVARA(f,arg) ENTRY5(f,int,int,CONST(size_t*),CONST(size_t*),CONST(arg))
#define PUTVARS(f,arg) ENTRY6(f,int,int,CONST(size_t*),CONST(size_t*),CONST(ptrdiff_t*),CONST(arg))
#define PUTVARM(f,arg) ENTRY7(f,int,int,CONST(size_t*),CONST(size_t*),CONST(ptrdiff_t*),CONST(ptrdiff_t*),CONST(arg))
#define PUTVAR(f,arg)  ENTRY3(f,int,int,CONST(arg))

/*
#define GETATT(f,arg)  int (*f)(int,int,const char*,arg)
#define GETVAR1(f,arg)  int (*f)(int,int,const size_t*,arg)
#define GETVARA(f,arg)  int (*f)(int,int,const size_t*,const size_t*,arg)
#define GETVARS(f,arg)  int (*f)(int,int,const size_t*,const size_t*,const ptrdiff_t*,arg)
#define GETVARM(f,arg) int (*f)(int,int,const size_t*,const size_t*,const ptrdiff_t*,const ptrdiff_t*,arg)
#define GETVAR(f,arg)  int (*f)(int,int,arg)
#define PUTATT(f,arg)  int (*f)(int,int,const char*,nc_type,size_t,const arg)
#define PUTATTX(f)  int (*f)(int,int,const char*,size_t,const char*)
#define PUTVAR1(f,arg)  int (*f)(int,int,const size_t,size_t,const arg)
#define PUTVARA(f,arg)  int (*f)(int,int,const size_t,const size_t*,const arg)
#define PUTVARS(f,arg)  int (*f)(int,int,const size_t,const size_t*,const ptrdiff_t*,const arg)
#define PUTVARM(f,arg)  int (*f)(int,int,const size_t,const size_t*,const ptrdiff_t*,const ptrdiff_t*,const arg)
#define PUTVAR(f,arg)  int (*f)(int,int,const arg)
*/

#endif /*DISPATCHDEFS_H*/

