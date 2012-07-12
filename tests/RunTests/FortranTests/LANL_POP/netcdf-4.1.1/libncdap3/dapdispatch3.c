/*********************************************************************
 *   Copyright 1993, UCAR/Unidata
 *   See netcdf/COPYRIGHT file for copying and redistribution conditions.
 *   $Header: /upc/share/CVS/netcdf-3/libncdap3/dapdispatch3.c,v 1.19 2009/11/29 00:16:26 dmh Exp $
 *********************************************************************/
#include "config.h"
#include "netcdf3.h"
#include "ncdap3.h"
#include "daprename.h"
#include "dapdebug.h"
#include "dispatch3.h"
#include "dapdispatch3.h"

extern int
nc3d_getvarx(int ncid, int varid,
            const size_t *startp,
            const size_t *countp,
            const ptrdiff_t* stridep,
            void *value,
            nc_type dsttype);

/* Forward*/
static int nc3d_getvar1(int ncid, int varid,
                       const size_t*,
                       void*, nc_type);

static int nc3d_getvars(int ncid, int varid,
            const size_t* start, const size_t* count, const ptrdiff_t *stride,
	    void*  value,
	    nc_type dsttype);

/* Local*/
static size_t dapzerostart3[NC_MAX_DIMS];
static size_t dapsinglecount3[NC_MAX_DIMS];
static ptrdiff_t dapsinglestride3[NC_MAX_DIMS];

/* Invoked from ncdap3.c:nc3dinitialize() */
void
dapdispatch3init(void)
{
    int i;
    for(i=0;i<NC_MAX_DIMS;i++) {
        dapzerostart3[i] = 0;
	dapsinglecount3[i] = 1;
	dapsinglestride3[i] = 1;
    }
}


static void
setnc3dispatch(int ncid)
{
    NC* ncp;
    NC_check_id(ncid,&ncp); /* get the NC structure*/
    ncp->dispatch = &netcdf3lib;
}


/* Following are dispatch entries for dap.
   The entries that do the real work have
   been moved to ncdap3.c
*/

int
RENAME3(_open_mp)(const char *path, int mode, int basepe,
 	 size_t* chunksizehintp, int *ncidp)
{
    return THROW(nc3d_open(path, mode, ncidp));
}

#ifdef IGNORE
int
nc__open(const char *path, int mode, 
 	 size_t* chunksizehintp, int *ncidp)
{
    return THROW(nc__open_mp(path, mode, 0, chunksizehintp, ncidp));
}

int
nc_open(const char *path, int mode, int *ncidp)
{
    return THROW(nc__open(path, mode, NULL, ncidp));
}
#endif

#ifdef IGNORE
int
nc__create(const char *path, int cmode, size_t initialsz,
	    size_t* chunksizehintp, int *ncidp)
{
    int stat;
    stat = l3nc__create_mp(path,cmode,initialsz,0,chunksizehintp,ncidp);
    if(stat == NC_NOERR) setnc3dispatch(*ncidp);
    return THROW(stat);
}

int
nc_create(const char *path, int cmode, int *ncidp)
{
    int stat;
    stat = RENAMEDAP(create_mp)(path,cmode,0,0,NULL,ncidp);
    if(stat == NC_NOERR) setnc3dispatch(*ncidp);
    return THROW(stat);
}
#endif

/**************************************************/
/* Following are in the dispatch table */

int
nc3d_redef(int ncid)
{
    return THROW(NC_EPERM);
}

int
nc3d__enddef(int ncid, size_t h_minfree, size_t v_align, size_t v_minfree, size_t r_align)
{
    return THROW(NC_EPERM);
}

int
nc3d_enddef(int ncid)
{
    return THROW(NC_EPERM);
}

int
nc3d_sync(int ncid)
{
    return THROW(NC_EINVAL);
}

int
nc3d_abort(int ncid)
{
    return THROW(NC_NOERR);
}

/*
The DAP cdf info will have been integrated
into the existing netcdf data structures, so
that simple inquiries will work as expected
E.g all the nc_inq... entries (see dispatch.c)
*/

/*
The following entries are all ultimately mapped into a single
call that handles all cases.
*/

int
nc3d_get_var1(int ncid, int varid, const size_t* indexp, void* value)
{
    return THROW(nc3d_getvar1(ncid, varid, indexp, value, NC_NAT));
}

int
nc3d_get_vara(int ncid, int varid, const size_t* start, const size_t* count, void* value)
{
    return THROW(nc3d_getvara(ncid, varid, start, count, value, NC_NAT));
}

int
nc3d_get_vars(int ncid, int varid, const size_t* start, const size_t* count, const ptrdiff_t *stride,void*  value)
{
    return THROW(nc3d_getvars(ncid, varid, start, count, stride, value, NC_NAT));
}

int
nc3d_get_varm(int ncid, int varid, const size_t* start, const size_t* count, const ptrdiff_t *stride, const ptrdiff_t *imapp, void* value)
{
    return THROW(nc3d_getvarmx(ncid, varid, start, count, stride, imapp, value, NC_NAT));
}

#define DEFGETVAR1(T1,T2,T3) \
int nc3d_get_var1_##T1(int ncid, int varid, const size_t*  indexp, T2* ip)\
{\
    int stat;\
    stat = nc3d_getvar1(ncid, varid, indexp, ip, T3); \
    return THROW(stat); \
}

DEFGETVAR1(text,char,NC_CHAR)
DEFGETVAR1(uchar,unsigned char,NC_CHAR)
DEFGETVAR1(schar,signed char,NC_CHAR)
DEFGETVAR1(short,short,NC_SHORT)
DEFGETVAR1(int,int,NC_INT)
/* Special case
DEFGETVAR1(long,long,NC_INT64)
DEFGETVAR1(ulong,unsigned long,NC_UINT64)
*/
DEFGETVAR1(float,float,NC_FLOAT)
DEFGETVAR1(double,double,NC_DOUBLE)

#define DEFGETVARA(T1,T2,T3) \
int nc3d_get_vara_##T1(int ncid, int varid, const size_t*  start,const size_t*  count, T2* ip)\
{\
    return THROW(nc3d_getvara(ncid, varid, start, count, ip, T3));\
}

DEFGETVARA(text,char,NC_CHAR)
DEFGETVARA(uchar,unsigned char,NC_BYTE)
DEFGETVARA(schar,signed char,NC_BYTE)
DEFGETVARA(short,short,NC_SHORT)
DEFGETVARA(int,int,NC_INT)
/* Special case
DEFGETVARA(long,long,NC_INT64)
DEFGETVARA(ulong,unsigned long,NC_UINT64)
*/
DEFGETVARA(float,float,NC_FLOAT)
DEFGETVARA(double,double,NC_DOUBLE)

#define DEFGETVARS(T1,T2,T3) \
int nc3d_get_vars_##T1(int ncid, int varid, const size_t*  start,const size_t*  count, const ptrdiff_t* stridep, T2* ip)\
{\
    return THROW(nc3d_getvars(ncid, varid, start, count, stridep, ip, T3));\
}

DEFGETVARS(text,char,NC_CHAR)
DEFGETVARS(uchar,unsigned char,NC_CHAR)
DEFGETVARS(schar,signed char,NC_CHAR)
DEFGETVARS(short,short,NC_SHORT)
DEFGETVARS(int,int,NC_INT)
/* Special case
DEFGETVARS(long,long,NC_INT64)
DEFGETVARS(ulong,unsigned long,NC_UINT64)
*/
DEFGETVARS(float,float,NC_FLOAT)
DEFGETVARS(double,double,NC_DOUBLE)

/* Following is temporarily disabled */

#define DEFGETVARM(T1,T2,T3) \
int nc3d_get_varm_##T1(int ncid, int varid, const size_t* start, const size_t*  count, const ptrdiff_t* stridep, const ptrdiff_t* imapp, T2* ip)\
{\
    return THROW(nc3d_getvarmx(ncid, varid, start, count, stridep, imapp, ip, T3));\
}

DEFGETVARM(text,char,NC_CHAR)
DEFGETVARM(uchar,unsigned char,NC_CHAR)
DEFGETVARM(schar,signed char,NC_CHAR)
DEFGETVARM(short,short,NC_SHORT)
DEFGETVARM(int,int,NC_INT)
/* Special case
DEFGETVARM(long,long,NC_INT64)
DEFGETVARM(ulong,unsigned long,NC_UINT64)
*/
DEFGETVARM(float,float,NC_FLOAT)
DEFGETVARM(double,double,NC_DOUBLE)

#define DEFGETVAR(T1,T2,T3) \
int nc3d_get_var_##T1(int ncid, int varid, T2* ip)\
{\
    return THROW(nc3d_getvar(ncid,varid,ip,T3)); \
}

DEFGETVAR(text,char,NC_CHAR)
DEFGETVAR(uchar,unsigned char,NC_CHAR)
DEFGETVAR(schar,signed char,NC_CHAR)
DEFGETVAR(short,short,NC_SHORT)
DEFGETVAR(int,int,NC_INT)
/* Special case
DEFGETVAR(long,long,NC_INT64)
DEFGETVAR(ulong,unsigned long,NC_UINT64)
*/
DEFGETVAR(float,float,NC_FLOAT)
DEFGETVAR(double,double,NC_DOUBLE)

/**********************************************/

static int
nc3d_getvar1(int ncid, int varid,
            const size_t* indexp,
            void* value,
            nc_type dsttype)
{
    /* utilize the more general varx routine*/
    return THROW(nc3d_getvarx(ncid,varid,
		 indexp,dapsinglecount3,dapsinglestride3,
		 value,dsttype));
}

int
nc3d_getvara(int ncid, int varid,
            const size_t* start, const size_t* count,
            void* value,
            nc_type dsttype)
{
    /* utilize the more general varx routine*/
    return THROW(nc3d_getvarx(ncid,varid,
		 start,count,dapsinglestride3,
		 value,dsttype));
}

static int
nc3d_getvars(int ncid, int varid,
            const size_t* start, const size_t* count, const ptrdiff_t *stride,
	    void*  value,
	    nc_type dsttype)
{
    /* utilize the more general varx routine*/
    return THROW(nc3d_getvarx(ncid,varid,start,count,stride,value,dsttype));
}

int
nc3d_getvar(int ncid, int varid,
            void* value,
            nc_type dsttype)
{
    /* utilize the more general varx routine*/
    /* Use null count to force varx to compute total # of elements */
    return THROW(nc3d_getvarx(ncid,varid,
		 dapzerostart3,NULL,dapsinglestride3,
		 value,dsttype));
}

/* Following are not in dispatch table, but are here
   so that the RENAME macros only are used in the dispatch code
*/

int
RENAME3(_create_mp)(const char *path, int cmode, size_t initialsz, int basepe,
	      size_t* chunksizehintp, int *ncidp)
{
    int stat;
    stat = RENAMEDAP(_create_mp)(path,cmode,initialsz,basepe,chunksizehintp,ncidp);
    if(stat == NC_NOERR) setnc3dispatch(*ncidp);
    return THROW(stat);
}

int
nc3d__open_mp(const char *path, int mode, int basepe,
	      size_t *chunksizehintp, int *ncidp)
{
    NCerror ncstat = RENAMEDAP(_open_mp)(path,mode,0,NULL,ncidp);
    if(ncstat == NC_NOERR) setnc3dispatch(*ncidp);
    return ncstat;
}

int
nc3d_def_dim(int ncid, const char *name, size_t len, int *idp)
{
    return RENAME3(def_dim)(ncid,name,len,idp);
}

int
nc3d_put_att(int ncid, int varid, const char *name, nc_type datatype,
	   size_t len, const void *value)
{
    return RENAME3(put_att)(ncid,varid,name,datatype,len,value);
}

int
nc3d_put_att_text(int ncid, int varid, const char *name,
		size_t len, const char *op)
{
    return RENAME3(put_att_text)(ncid,varid,name,len,op);
}

int
nc3d_def_var(int ncid, const char *name, nc_type xtype, int ndims, 
	   const int *dimidsp, int *varidp)
{
    return RENAME3(def_var)(ncid,name,xtype,ndims,dimidsp,varidp);
}

/**************************************************/
/*
The nc_varx_(u)long procedures need special handling
because sizeof(long) varies depending on machine word size.
*/

#ifdef USE_NETCDF4
#define LONGCAST (sizeof(long)==sizeof(int)?NC_INT:NC_INT64)
#define ULONGCAST (sizeof(unsigned long)==sizeof(unsigned int)?NC_UINT:NC_UINT64)
#else
#define LONGCAST (sizeof(long)==sizeof(int)?NC_INT:NC_INT64)
#define ULONGCAST (sizeof(unsigned long)==sizeof(unsigned int)?NC_UINT:NC_UINT64)
#endif

int
nc3d_get_var1_long(int ncid, int varid, const size_t*  indexp, long* ip)
{
    return THROW(nc3d_getvar1(ncid, varid, indexp, ip, LONGCAST));
}

int
nc3d_get_vara_long(int ncid, int varid, const size_t*  start,const size_t*  count, long* ip)
{
    return THROW(nc3d_getvara(ncid, varid, start, count, ip, LONGCAST));
}

int
nc3d_get_vars_long(int ncid, int varid, const size_t*  start,const size_t*  count, const ptrdiff_t* stridep, long* ip)
{
    return THROW(nc3d_getvars(ncid, varid, start, count, stridep, ip, LONGCAST));
}

int
nc3d_get_varm_long(int ncid, int varid, const size_t* start, const size_t*  count, const ptrdiff_t* stridep, const ptrdiff_t* imapp, long* ip)
{
    return THROW(nc3d_getvarmx(ncid, varid, start, count, stridep, imapp, ip, LONGCAST));
}

int
nc3d_get_var_long(int ncid, int varid, long* ip)
{
    return THROW(nc3d_getvar(ncid,varid,ip,LONGCAST));
}

#ifdef NC_ULONG
int
nc3d_get_var1_long(int ncid, int varid, const size_t*  indexp, long* ip)
{
    return THROW(nc3d_getvar1(ncid, varid, indexp, ip, ULONGCAST));
}

int
nc3d_get_vara_long(int ncid, int varid, const size_t*  start,const size_t*  count, long* ip)
{
    return THROW(nc3d_getvara(ncid, varid, start, count, ip, ULONGCAST));
}

int
nc3d_get_vars_long(int ncid, int varid, const size_t*  start,const size_t*  count, const ptrdiff_t* stridep, long* ip)
{
    return THROW(nc3d_getvars(ncid, varid, start, count, stridep, ip, ULONGCAST));
}

int
nc3d_get_varm_long(int ncid, int varid, const size_t* start, const size_t*  count, const ptrdiff_t* stridep, const ptrdiff_t* imapp, long* ip)
{
    return THROW(nc3d_getvarmx(ncid, varid, start, count, stridep, imapp, ip, ULONGCAST));
}

int
nc3d_get_var_long(int ncid, int varid, long* ip)
{
    return THROW(nc3d_getvar(ncid,varid,ip,ULONGCAST));
}
#endif
