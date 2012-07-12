/*********************************************************************
 *   Copyright 1993, UCAR/Unidata
 *   See netcdf/COPYRIGHT file for copying and redistribution conditions.
 *   $Header: /upc/share/CVS/netcdf-3/libncdap3/dapdispatch3.h,v 1.10 2009/12/01 00:14:43 dmh Exp $
 *********************************************************************/
#ifndef DAPDISPATCH_H
#define  DAPDISPATCH_H 1

/* Some of the following do not appear in the
   dispatch tables; rather they are convenince
   functions for accessing the libsrc renamed API.
*/

extern int
nc3d_redef(int ncid);

extern int
nc3d__enddef(int ncid, size_t h_minfree, size_t v_align, size_t v_minfree, size_t r_align);

extern int
nc3d_enddef(int ncid);

extern int
nc3d_sync(int ncid);

extern int
nc3d_abort(int ncid);

extern int
nc3d_close(int ncid);

extern int
nc3d_get_var1(int ncid, int varid, const size_t *indexp, void *value);

extern int
nc3d_get_var1_text(int ncid, int varid, const size_t *indexp, char *ip);

extern int
nc3d_get_var1_uchar(int ncid, int varid, const size_t *indexp, unsigned char *ip);

extern int
nc3d_get_var1_schar(int ncid, int varid, const size_t *indexp, signed char *ip);

extern int
nc3d_get_var1_short(int ncid, int varid, const size_t *indexp, short *ip);

extern int
nc3d_get_var1_int(int ncid, int varid, const size_t *indexp, int *ip);

extern int
nc3d_get_var1_long(int ncid, int varid, const size_t *indexp, long *ip);

extern int
nc3d_get_var1_float(int ncid, int varid, const size_t *indexp, float *ip);

extern int
nc3d_get_var1_double(int ncid, int varid, const size_t *indexp, double *ip);

extern int
nc3d_get_vara(int ncid, int varid, const size_t *start, const size_t *count, void *value);

extern int
nc3d_get_vara_text(int ncid, int varid, const size_t *startp, const size_t *countp, char *ip);

extern int
nc3d_get_vara_uchar(int ncid, int varid, const size_t *startp, const size_t *countp, unsigned char *ip);

extern int
nc3d_get_vara_schar(int ncid, int varid, const size_t *startp, const size_t *countp, signed char *ip);

extern int
nc3d_get_vara_short(int ncid, int varid, const size_t *startp, const size_t *countp, short *ip);

extern int
nc3d_get_vara_int(int ncid, int varid, const size_t *startp, const size_t *countp, int *ip);

extern int
nc3d_get_vara_long(int ncid, int varid, const size_t *startp, const size_t *countp, long *ip);

extern int
nc3d_get_vara_float(int ncid, int varid, const size_t *startp, const size_t *countp, float *ip);

extern int
nc3d_get_vara_double(int ncid, int varid, const size_t *startp, const size_t *countp, double *ip);

extern int
nc3d_get_vars(int ncid, int varid, const size_t *start, const size_t *count, const ptrdiff_t *stride,void * value);

extern int
nc3d_get_vars_text(int ncid, int varid, const size_t *startp, const size_t *countp, const ptrdiff_t *stridep,char *ip);

extern int
nc3d_get_vars_uchar(int ncid, int varid, const size_t *startp, const size_t *countp, const ptrdiff_t *stridep,unsigned char *ip);

extern int
nc3d_get_vars_schar(int ncid, int varid, const size_t *startp, const size_t *countp, const ptrdiff_t *stridep,signed char *ip);

extern int
nc3d_get_vars_short(int ncid, int varid, const size_t *startp, const size_t *countp, const ptrdiff_t *stridep,short *ip);

extern int
nc3d_get_vars_int(int ncid, int varid, const size_t *startp, const size_t *countp, const ptrdiff_t *stridep,int *ip);

extern int
nc3d_get_vars_long(int ncid, int varid, const size_t *startp, const size_t *countp, const ptrdiff_t *stridep,long *ip);

extern int
nc3d_get_vars_float(int ncid, int varid, const size_t *startp, const size_t *countp, const ptrdiff_t *stridep,float *ip);

extern int
nc3d_get_vars_double(int ncid, int varid, const size_t *startp, const size_t *countp, const ptrdiff_t *stridep,double *ip);

extern int
nc3d_get_varm(int ncid, int varid, const size_t *start, const size_t *count, const ptrdiff_t *stride, const ptrdiff_t *imapp, void *value);

extern int
nc3d_get_varm_text(int ncid, int varid, const size_t *startp, const size_t *countp, const ptrdiff_t *stridep,const ptrdiff_t *imapp, char *ip);

extern int
nc3d_get_varm_uchar(int ncid, int varid, const size_t *startp, const size_t *countp, const ptrdiff_t *stridep,const ptrdiff_t *imapp, unsigned char *ip);

extern int
nc3d_get_varm_schar(int ncid, int varid, const size_t *startp, const size_t *countp, const ptrdiff_t *stridep,const ptrdiff_t *imapp, signed char *ip);

extern int
nc3d_get_varm_short(int ncid, int varid, const size_t *startp, const size_t *countp, const ptrdiff_t *stridep,const ptrdiff_t *imapp, short *ip);

extern int
nc3d_get_varm_int(int ncid, int varid, const size_t *startp, const size_t *countp, const ptrdiff_t *stridep,const ptrdiff_t *imapp, int *ip);

extern int
nc3d_get_varm_long(int ncid, int varid, const size_t *startp, const size_t *countp, const ptrdiff_t *stridep,const ptrdiff_t *imapp, long *ip);

extern int
nc3d_get_varm_float(int ncid, int varid, const size_t *startp, const size_t *countp, const ptrdiff_t *stridep,const ptrdiff_t *imapp, float *ip);

extern int
nc3d_get_varm_double(int ncid, int varid, const size_t *startp, const size_t *countp, const ptrdiff_t *stridep,const ptrdiff_t * imapp, double *ip);

extern int
nc3d_get_var_text(int ncid, int varid, char *ip);

extern int
nc3d_get_var_uchar(int ncid, int varid, unsigned char *ip);

extern int
nc3d_get_var_schar(int ncid, int varid, signed char *ip);

extern int
nc3d_get_var_short(int ncid, int varid, short *ip);

extern int
nc3d_get_var_int(int ncid, int varid, int *ip);

extern int
nc3d_get_var_long(int ncid, int varid, long *ip);

extern int
nc3d_get_var_float(int ncid, int varid, float *ip);

extern int
nc3d_get_var_double(int ncid, int varid, double *ip);

extern int
nc3d_def_dim(int ncid, const char *name, size_t len, int *idp);

extern int
nc3d_rename_dim(int ncid, int dimid, const char *name);

extern int
nc3d__open_mp(const char *path, int mode, int basepe,
	      size_t *chunksizehintp, int *ncidp);

extern int
nc3d_def_dim(int ncid, const char *name, size_t len, int *idp);

extern int
nc3d_put_att(int ncid, int varid, const char *name, nc_type datatype,
	   size_t len, const void *value);

extern int
nc3d_put_att_text(int ncid, int varid, const char *name,
		size_t len, const char *op);

extern int
nc3d_def_var(int ncid, const char *name, nc_type xtype, int ndims, 
	   const int *dimidsp, int *varidp);

extern int
nc3d_getvar(int ncid, int varid, void* value, nc_type);

extern int
nc3d_getvara(int ncid, int varid,
                       const size_t* , const size_t*,
                       void*, nc_type);

extern int
nc3d_getvarmx(int ncid, int varid,
	    const size_t *start,
	    const size_t *edges,
	    const ptrdiff_t* stride,
 	    const ptrdiff_t* map,
	    void* data,
	    nc_type dsttype0);

#endif /*DAPDISPATCH_H*/
