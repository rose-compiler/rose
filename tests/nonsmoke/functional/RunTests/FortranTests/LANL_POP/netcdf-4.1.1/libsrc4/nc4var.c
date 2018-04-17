/*
This file is part of netcdf-4, a netCDF-like interface for HDF5, or a
HDF5 backend for netCDF, depending on your point of view.

This file handles the nc4 variable functions.

Copyright 2003-2006, University Corporation for Atmospheric
Research. See COPYRIGHT file for copying and redistribution
conditions.

$Id: nc4var.c,v 1.138 2010/02/24 16:03:24 ed Exp $
*/

#include <nc4internal.h>
#include <math.h>

#ifdef USE_PNETCDF
#include <pnetcdf.h>
#endif

/* Min and max deflate levels tolerated by HDF5. */
#define MIN_DEFLATE_LEVEL 0
#define MAX_DEFLATE_LEVEL 9

/* This is to track opened HDF5 objects to make sure they are
 * closed. */
#ifdef EXTRA_TESTS
extern int num_plists;
#endif /* EXTRA_TESTS */

/* One meg is the minimum buffer size. */
#define ONE_MEG 1048576

/* Szip options. */
#define NC_SZIP_EC_OPTION_MASK 4
#define NC_SZIP_NN_OPTION_MASK 32
#define NC_SZIP_MAX_PIXELS_PER_BLOCK 32

int nc4_get_default_fill_value(NC_TYPE_INFO_T *type_info, void *fill_value);


/* If the HDF5 dataset for this variable is open, then close it and
 * reopen it, with the perhaps new settings for chunk caching. */
int
nc4_reopen_dataset(NC_GRP_INFO_T *grp, NC_VAR_INFO_T *var)
{
   hid_t access_pid;

   if (var->hdf_datasetid)
   {
      if ((access_pid = H5Pcreate(H5P_DATASET_ACCESS)) < 0)
	 return NC_EHDFERR;
#ifdef EXTRA_TESTS
      num_plists++;
#endif
      if (H5Pset_chunk_cache(access_pid, var->chunk_cache_nelems, 
			     var->chunk_cache_size, 
			     var->chunk_cache_preemption) < 0)
	 return NC_EHDFERR;
      if (H5Dclose(var->hdf_datasetid) < 0)
	 return NC_EHDFERR;
      if ((var->hdf_datasetid = H5Dopen2(grp->hdf_grpid, var->name, 
					 access_pid)) < 0)
	 return NC_EHDFERR;
      if (H5Pclose(access_pid) < 0)
	 return NC_EHDFERR;
#ifdef EXTRA_TESTS
      num_plists--;
#endif

      if (var->dimscale)
	 var->dim[0]->hdf_dimscaleid = var->hdf_datasetid;
   }
   
   return NC_NOERR;
}

/* Set chunk cache size for a variable. You can't touch this! */
int
nc_set_var_chunk_cache(int ncid, int varid, size_t size, size_t nelems, 
		       float preemption)
{
   NC_FILE_INFO_T *nc;
   NC_GRP_INFO_T *grp; 
   NC_HDF5_FILE_INFO_T *h5;
   NC_VAR_INFO_T *var;
   int retval;

   /* Check input for validity. */
   if (preemption < 0 || preemption > 1)
      return NC_EINVAL;

   /* Find info for this file and group, and set pointer to each. */
   if ((retval = nc4_find_nc_grp_h5(ncid, &nc, &grp, &h5)))
      return retval;

   /* Attempting to do any of these things on a netCDF-3 file produces
    * an error. */
   if (!h5)
      return NC_ENOTNC4;

   assert(nc && grp && h5);

   /* Find the var. */
   for (var = grp->var; var; var = var->next)
      if (var->varid == varid)
         break;
   if (!var)
      return NC_ENOTVAR;

   /* Set the values. */
   var->chunk_cache_size = size;
   var->chunk_cache_nelems = nelems;
   var->chunk_cache_preemption = preemption;

   if ((retval = nc4_reopen_dataset(grp, var)))
      return retval;

   return NC_NOERR;
}

/* Need this version for fortran. Accept negative numbers to leave
 * settings as they are. */
int
nc_set_var_chunk_cache_ints(int ncid, int varid, int size, int nelems, 
			    int preemption)
{
   size_t real_size = H5D_CHUNK_CACHE_NBYTES_DEFAULT;
   size_t real_nelems = H5D_CHUNK_CACHE_NSLOTS_DEFAULT;
   float real_preemption = H5D_CHUNK_CACHE_W0_DEFAULT;

   if (size >= 0)
      real_size = size * MEGABYTE;

   if (nelems >= 0)
      real_nelems = nelems;

   if (preemption >= 0)
      real_preemption = preemption / 100.;
	 
   return nc_set_var_chunk_cache(ncid, varid, real_size, real_nelems, 
				 real_preemption);
}

/* Get chunk cache size for a variable. Can't touch this. */
int
nc_get_var_chunk_cache(int ncid, int varid, size_t *sizep, size_t *nelemsp,
		       float *preemptionp)
{
   NC_FILE_INFO_T *nc;
   NC_GRP_INFO_T *grp; 
   NC_HDF5_FILE_INFO_T *h5;
   NC_VAR_INFO_T *var;
   int retval;

   /* Find info for this file and group, and set pointer to each. */
   if ((retval = nc4_find_nc_grp_h5(ncid, &nc, &grp, &h5)))
      return retval;

   /* Attempting to do any of these things on a netCDF-3 file produces
    * an error. */
   if (!h5)
      return NC_ENOTNC4;

   assert(nc && grp && h5);

   /* Find the var. */
   for (var = grp->var; var; var = var->next)
      if (var->varid == varid)
         break;
   if (!var)
      return NC_ENOTVAR;

   /* Give the user what they want. */
   if (sizep)
      *sizep = var->chunk_cache_size;
   if (nelemsp)
      *nelemsp = var->chunk_cache_nelems;
   if (preemptionp)
      *preemptionp = var->chunk_cache_preemption;

   return NC_NOERR;
}

/* Get chunk cache size for a variable. Can't touch this. */
int
nc_get_var_chunk_cache_ints(int ncid, int varid, int *sizep, 
			    int *nelemsp, int *preemptionp)
{
   size_t real_size, real_nelems;
   float real_preemption;
   int ret;

   if ((ret = nc_get_var_chunk_cache(ncid, varid, &real_size, 
				     &real_nelems, &real_preemption)))
      return ret;
   
   if (sizep)
      *sizep = real_size / MEGABYTE;
   if (nelemsp)
      *nelemsp = (int)real_nelems;
   if(preemptionp)
      *preemptionp = (int)(real_preemption * 100);

   return NC_NOERR;
}

/* Find the default chunk nelems (i.e. length of chunk along each
 * dimension). */
int 
nc4_find_default_chunksizes(NC_VAR_INFO_T *var)
{
   int d;
   size_t type_size, num_values = 1, num_unlim = 0;

   if (var->type_info->nc_typeid == NC_STRING)
      type_size = sizeof(char *);
   else
      type_size = var->type_info->size;

   /* No chunk sizes for contiguous variables, you dunce! */
   if (var->contiguous) 
      return NC_NOERR;

   /* How many values in the non-unlimited dimensions? */
   for (d = 0; d < var->ndims; d++)
   {
      assert(var->dim[d]);
      if (var->dim[d]->len) 
	 num_values *= var->dim[d]->len;
      else
	 num_unlim++;
   }

   /* Pick a chunk length for each dimension. */
   for (d = 0; d < var->ndims; d++)
      if (var->dim[d]->unlimited)
	 var->chunksizes[d] = 1;
      else
      {
	 var->chunksizes[d] = (pow((double)DEFAULT_CHUNK_SIZE/(num_values * type_size), 
				   1/(double)(var->ndims - num_unlim)) * var->dim[d]->len + .5);
	 if (var->chunksizes[d] > var->dim[d]->len)
	    var->chunksizes[d] = var->dim[d]->len;
      }

   return NC_NOERR;
}

/* This is called when a new netCDF-4 variable is defined. Break it
 * down! */
static int 
nc_def_var_nc4(int ncid, const char *name, nc_type xtype, 
               int ndims, const int *dimidsp, int *varidp)
{
   NC_GRP_INFO_T *grp;
   NC_VAR_INFO_T *var;
   NC_DIM_INFO_T *dim;
   NC_HDF5_FILE_INFO_T *h5;
   NC_TYPE_INFO_T *type_info;
   char norm_name[NC_MAX_NAME + 1];
   int new_varid = 0;
   int num_unlim = 0;
   int d;
   size_t num_values = 1;
   int retval;

   /* Find info for this file and group, and set pointer to each. */
   if ((retval = nc4_find_grp_h5(ncid, &grp, &h5)))
      return retval;
   assert(grp && h5);

   /* If it's not in define mode, strict nc3 files error out,
    * otherwise switch to define mode. */
   if (!(h5->flags & NC_INDEF))
   {
      if (h5->cmode & NC_CLASSIC_MODEL)
         return NC_ENOTINDEFINE;
      if ((retval = nc_redef(ncid)))
         return retval;
   }

   /* Check and normalize the name. */
   if ((retval = nc4_check_name(name, norm_name)))
      return retval;

   /* Not a Type is, well, not a type.*/
   if (xtype == NC_NAT)
      return NC_EBADTYPE;
   
   /* For classic files, only classic types are allowed. */
   if (h5->cmode & NC_CLASSIC_MODEL && xtype > NC_DOUBLE)
      return NC_ESTRICTNC3;

   /* If this is a user defined type, find it. */
   if (xtype > NC_STRING)
      if ((retval = nc4_find_type(grp->file->nc4_info, xtype, &type_info)))
         return NC_EBADTYPE;

   /* cast needed for braindead systems with signed size_t */
   if((unsigned long) ndims > X_INT_MAX) /* Backward compat */
      return NC_EINVAL;

   /* Classic model files have a limit on number of vars. */
   if(h5->cmode & NC_CLASSIC_MODEL && h5->nvars >= NC_MAX_VARS)
      return NC_EMAXVARS;

   /* Check that this name is not in use as a var, grp, or type. */
   if ((retval = nc4_check_dup_name(grp, norm_name)))
      return retval;

   /* If the file is read-only, return an error. */
   if (h5->no_write)
     return NC_EPERM;

   /* Get the new varid. */
   for (var = grp->var; var; var = var->next)
      new_varid++;

   /* Check all the dimids to make sure they exist. */
   for (d = 0; d < ndims; d++)
   {
      if ((retval = nc4_find_dim(grp, dimidsp[d], &dim, NULL)))
         return retval;
      if (dim->unlimited)
	 num_unlim++;
      else
	 num_values *= dim->len;
   }

   /* These degrubbing messages sure are handy! */
   LOG((3, "nc_def_var_nc4: name %s type %d ndims %d", norm_name, xtype, ndims));
#ifdef LOGGING
   {
      int dd;
      for (dd = 0; dd < ndims; dd++)
         LOG((4, "dimid[%d] %d", dd, dimidsp[dd]));
   }
#endif

   /* Add the var to the end of the list. */
   if ((retval = nc4_var_list_add(&grp->var, &var)))
      return retval;

   /* Now fill in the values in the var info structure. */
   strcpy(var->name, norm_name);
   var->varid = grp->nvars++;
   var->xtype = xtype;
   var->ndims = ndims;
   var->dirty++;
   
   /* If this is a user-defined type, there is a type_info stuct with
    * all the type information. For atomic types, fake up a type_info
    * struct. */
   if (xtype > NC_STRING)
      var->type_info = type_info;
   else
   {
      if (!(var->type_info = calloc(1, sizeof(NC_TYPE_INFO_T))))
	 return NC_ENOMEM;
      var->type_info->nc_typeid = xtype;
      if ((retval = nc4_get_hdf_typeid(h5, var->xtype, &var->type_info->hdf_typeid, 
				       var->type_info->endianness)))
	 return retval;
      if ((var->type_info->native_typeid = H5Tget_native_type(var->type_info->hdf_typeid, 
							      H5T_DIR_DEFAULT)) < 0)
         return NC_EHDFERR;
      if ((retval = nc4_get_typelen_mem(h5, var->type_info->nc_typeid, 0, 
					&var->type_info->size)))
	 return retval;
   }
   if (!num_unlim)
      var->contiguous = 1;

   /* At the same time, check to see if this is a coordinate
    * variable. If so, it will have the same name as one of its
    * dimensions. If it is a coordinate var, is it a coordinate var in
    * the same group as the dim? */
   if (ndims)
      if (!(var->dim = malloc(sizeof(NC_DIM_INFO_T *) * ndims)))
	 return NC_ENOMEM;
   for (d = 0; d < ndims; d++)
   {
      NC_GRP_INFO_T *dim_grp;
      if ((retval = nc4_find_dim(grp, dimidsp[d], &dim, &dim_grp)))
         return retval;
      if (strcmp(dim->name, norm_name) == 0 && dim_grp == grp)
      {
         var->dimscale++;
         dim->coord_var = var;
	 dim->coord_var_in_grp++;
      }
      var->dimids[d] = dimidsp[d];
      var->dim[d] = dim;
   }

   /* Determine default chunksizes for this variable. */
   if ((retval = nc4_find_default_chunksizes(var)))
      return retval;

   /* Is this a variable with a chunksize greater than the current
    * cache size? */
   if ((retval = nc4_adjust_var_cache(grp, var)))
      return retval;

   /* If the user names this variable the same as a dimension, but
    * doesn't use that dimension in its list of dimension ids, is not
    * a coordinate variable. I need to change its HDF5 name, because
    * the dimension will cause a HDF5 dataset to be created, and this
    * var has the same name. */
   for (dim = grp->dim; dim; dim = dim->next)
      if (!strcmp(dim->name, norm_name))
      {
	 int d2;
	 for (d2 = 0; d2 < ndims; d2++)
	    if (dimidsp[d2] == dim->dimid)
	       break;
	 if (d2 == ndims)
	 {
	    /* Set a different hdf5 name for this variable to avoid
	     * name clash. */
	    if (strlen(norm_name) + strlen(NON_COORD_PREPEND) > NC_MAX_NAME)
	       return NC_EMAXNAME;
	    sprintf(var->hdf5_name, "%s%s", NON_COORD_PREPEND, norm_name);
	 }
      }

   /* If this is a coordinate var, it is marked as a HDF5 dimension
    * scale. (We found dim above.) Otherwise, allocate space to
    * remember whether dimension scales have been attached to each
    * dimension. */
   if (!var->dimscale && ndims)
      if (ndims && !(var->dimscale_attached = calloc(ndims, sizeof(int))))
         return NC_ENOMEM;

   /* Return the varid. */
   if (varidp)
      *varidp = var->varid;
   LOG((4, "new varid %d", var->varid));

   return retval;
}

/* Create a new variable to hold user data. This is what it's all
 * about baby! */
int
nc_def_var(int ncid, const char *name, nc_type xtype, int ndims, 
           const int *dimidsp, int *varidp)
{
   NC_FILE_INFO_T *nc;

   LOG((2, "nc_def_var: ncid 0x%x name %s xtype %d ndims %d",
        ncid, name, xtype, ndims));

   /* If there are dimensions, I need their ids. */
   if (ndims && !dimidsp)
      return NC_EINVAL;

   /* Find metadata for this file. */
   if (!(nc = nc4_find_nc_file(ncid)))
      return NC_EBADID;

#ifdef USE_PNETCDF
   /* Take care of files created/opened with parallel-netcdf library. */
   if (nc->pnetcdf_file)
   {
      int ret;

      ret = ncmpi_def_var(nc->int_ncid, name, xtype, ndims, 
			  dimidsp, varidp);
      nc->pnetcdf_ndims[*varidp] = ndims;
      return ret;

   }
#endif /* USE_PNETCDF */

   /* Handle netcdf-3 cases. */
   if (!nc->nc4_info)
      return nc3_def_var(nc->int_ncid, name, xtype, ndims, 
                         dimidsp, varidp);

   /* Handle netcdf-4 cases. */
   return nc_def_var_nc4(ncid, name, xtype, ndims, dimidsp, varidp);
}

/* Get all the information about a variable. Pass NULL for whatever
 * you don't care about. This is an internal function, not exposed to
 * the user. */
static int 
nc_inq_var_all(int ncid, int varid, char *name, nc_type *xtypep, 
               int *ndimsp, int *dimidsp, int *nattsp, 
               int *shufflep, int *deflatep, int *deflate_levelp,
               int *fletcher32p, int *contiguousp, size_t *chunksizesp, 
               int *no_fill, void *fill_valuep, int *endiannessp, 
	       int *options_maskp, int *pixels_per_blockp)
{
   NC_FILE_INFO_T *nc;
   NC_GRP_INFO_T *grp; 
   NC_HDF5_FILE_INFO_T *h5;
   NC_VAR_INFO_T *var;
   NC_ATT_INFO_T *att;
   int natts=0;
   size_t type_size;
   int d;
   int retval;

   LOG((2, "nc_inq_var_all: ncid 0x%x varid %d", ncid, varid));

   /* Find info for this file and group, and set pointer to each. */
   if ((retval = nc4_find_nc_grp_h5(ncid, &nc, &grp, &h5)))
      return retval;

#ifdef USE_PNETCDF
   /* Take care of files created/opened with parallel-netcdf library. */
   if (nc->pnetcdf_file)
      return ncmpi_inq_var(nc->int_ncid, varid, name, xtypep, ndimsp, 
			   dimidsp, nattsp);
#endif /* USE_PNETCDF */

   /* Handle netcdf-3 cases. Better not ask for var options for a
    * netCDF-3 file! */
   if (!h5)
   {
      if (contiguousp)
	 *contiguousp = NC_CONTIGUOUS;
      return nc3_inq_var(nc->int_ncid, varid, name, xtypep, ndimsp, 
                         dimidsp, nattsp);
   }
   assert(nc && grp && h5);

   /* Walk through the list of vars, and return the info about the one
      with a matching varid. If the varid is -1, find the global
      atts and call it a day. */
   if (varid == NC_GLOBAL)
   {
      if (nattsp)
      {
         for (att = grp->att; att; att = att->next)
            natts++;
         *nattsp = natts;
      }
      return NC_NOERR;
   }

   /* Find the var. */
   for (var = grp->var; var; var = var->next)
      if (var->varid == varid)
         break;
   
   /* Oh no! Maybe we couldn't find it (*sob*)! */
   if (!var)
      return NC_ENOTVAR;
   
   /* Copy the data to the user's data buffers. */
   if (name)
      strcpy(name, var->name);
   if (xtypep)
      *xtypep = var->xtype;
   if (ndimsp)
      *ndimsp = var->ndims;
   if (dimidsp)
      for (d = 0; d < var->ndims; d++)
         dimidsp[d] = var->dimids[d];
   if (nattsp)
   {
      for (att = var->att; att; att = att->next)
         natts++;
      *nattsp = natts;
   }
   
   /* Chunking stuff. */
   if (chunksizesp)
      for (d = 0; d < var->ndims; d++)
      {
         chunksizesp[d] = var->chunksizes[d];
         LOG((4, "chunksizesp[%d]=%d", d, chunksizesp[d]));
      }

   if (contiguousp)
      *contiguousp = var->contiguous;

   /* Filter stuff. */
   if (deflatep)
      *deflatep = var->deflate;
   if (deflate_levelp)
      *deflate_levelp = var->deflate_level;
   if (shufflep)
      *shufflep = var->shuffle;
   if (fletcher32p)
      *fletcher32p = var->fletcher32;
   if (options_maskp)
      *options_maskp = var->options_mask;
   if (pixels_per_blockp)
      *pixels_per_blockp = var->pixels_per_block;

   /* Fill value stuff. */
   if (no_fill)
      *no_fill = var->no_fill;

   /* Don't do a thing with fill_valuep if no_fill mode is set for
    * this var, or if fill_valuep is NULL. */
   if (!var->no_fill && fill_valuep)
   {
      /* Do we have a fill value for this var? */
      if (var->fill_value)
      {
         if ((retval = nc4_get_typelen_mem(grp->file->nc4_info, var->xtype, 0, &type_size)))
            return retval;
         memcpy(fill_valuep, var->fill_value, type_size);
      }
      else
      {
         if ((retval = nc4_get_default_fill_value(var->type_info, fill_valuep)))
            return retval;
      }
   }

   /* Does the user want the endianness of this variable? */
   if (endiannessp)
      *endiannessp = var->type_info->endianness;

   return NC_NOERR;
}

/* Find out about a var. The function ignores any of the pointer
   parameters if a NULL is passed. */
int
nc_inq_var(int ncid, int varid, char *name, nc_type *xtypep, 
           int *ndimsp, int *dimidsp, int *nattsp)
{
   return nc_inq_var_all(ncid, varid, name, xtypep, ndimsp, 
                         dimidsp, nattsp, NULL, NULL, NULL, NULL, 
                         NULL, NULL, NULL, NULL, NULL, NULL, NULL);
}

/* This functions sets extra stuff about a netCDF-4 variable which
   must be set before the enddef but after the def_var. This is an
   internal function, deliberately hidden from the user so that we can
   change the prototype of this functions without changing the API. */
static int
nc_def_var_extra(int ncid, int varid, int *shuffle, int *deflate, 
		 int *deflate_level, int *fletcher32, int contiguous, 
		 const size_t *chunksizes, int *no_fill, 
                 const void *fill_value, int *endianness, 
		 int *options_mask, int *pixels_per_block)
{
   NC_FILE_INFO_T *nc;
   NC_GRP_INFO_T *grp; 
   NC_HDF5_FILE_INFO_T *h5;
   NC_VAR_INFO_T *var;
   NC_DIM_INFO_T *dim;
   size_t type_size;
   int d;
   int retval;

   LOG((2, "nc_def_var_extra: ncid 0x%x varid %d", ncid, varid));

   /* Find info for this file and group, and set pointer to each. */
   if ((retval = nc4_find_nc_grp_h5(ncid, &nc, &grp, &h5)))
      return retval;

   /* Attempting to do any of these things on a netCDF-3 file produces
    * an error. */
   if (!h5)
      return NC_ENOTNC4;

   assert(nc && grp && h5);

   /* Find the var. */
   for (var = grp->var; var; var = var->next)
      if (var->varid == varid)
         break;
   
   /* Oh no! Maybe we couldn't find it (*sob*)! */
   if (!var)
      return NC_ENOTVAR;

   /* Can't turn on contiguous and deflate/fletcher32/szip. */
   if ((contiguous != NC_CHUNKED && deflate) || 
       (contiguous != NC_CHUNKED && fletcher32) ||
       (contiguous != NC_CHUNKED && options_mask))
      return NC_EINVAL;

   /* If the HDF5 dataset has already been created, then it is too
    * late to set all the extra stuff. */
   if (var->created)
      return NC_ELATEDEF;

   /* Check compression options. */
   if ((deflate && options_mask) ||
       (deflate && !deflate_level) ||
       (options_mask && !pixels_per_block))
      return NC_EINVAL;      
       
   /* Valid deflate level? */
   if (deflate && deflate_level)
   {
      if (*deflate)
         if (*deflate_level < MIN_DEFLATE_LEVEL ||
             *deflate_level > MAX_DEFLATE_LEVEL)
            return NC_EINVAL;
      if (var->options_mask)
            return NC_EINVAL;

      /* Must not be a scalar. */
      if (!var->ndims)
            return NC_EINVAL;

      /* Well, if we couldn't find any errors, I guess we have to take
       * the users settings. Darn! */
      var->contiguous = 0;
      var->deflate = *deflate;
      if (*deflate)
         var->deflate_level = *deflate_level;
      LOG((3, "nc_def_var_extra: *deflate_level %d", *deflate_level));      
   }

   /* Szip in use? */
   if (options_mask)
   {
#ifndef USE_SZIP
      return NC_EINVAL;
#endif
      if (var->deflate)
	 return NC_EINVAL;
      if ((*options_mask != NC_SZIP_EC_OPTION_MASK) &&
	  (*options_mask != NC_SZIP_NN_OPTION_MASK))
	 return NC_EINVAL;
      if ((*pixels_per_block > NC_SZIP_MAX_PIXELS_PER_BLOCK) ||
	  (var->type_info->nc_typeid >= NC_STRING))
	 return NC_EINVAL;
      var->options_mask = *options_mask;
      var->pixels_per_block = *pixels_per_block;
      var->contiguous = 0;
   }

   /* Shuffle filter? */
   if (shuffle)
   {
      var->shuffle = *shuffle;
      var->contiguous = 0;
   }

   /* Fltcher32 checksum error protection? */
   if (fletcher32)
   {
      var->fletcher32 = *fletcher32;
      var->contiguous = 0;
   }
   
   /* Does the user want a contiguous dataset? Not so fast! Make sure
    * that there are no unlimited dimensions, and no filters in use
    * for this data. */
   if (contiguous)
   {
      if (var->deflate || var->fletcher32 || var->shuffle || var->options_mask)
	 return NC_EINVAL;
      
      for (d = 0; d < var->ndims; d++)
      {
	 if ((retval = nc4_find_dim(grp, var->dimids[d], &dim, NULL)))
	    return retval;
	 if (dim->unlimited)
	    return NC_EINVAL;
      }

      var->contiguous++;
   }

   /* Chunksizes anyone? */
   if (contiguous == NC_CHUNKED && chunksizes)
   {
      var->contiguous = 0;

      /* Check that they are not too big, and that their total size
       * of chunk is less than 4 GB. */
      {
            NC_TYPE_INFO_T *type_info;
            long long total;
            size_t type_len;

            if ((retval = nc4_get_typelen_mem(grp->file->nc4_info, var->xtype, 
                                              0, &type_len)))
               return retval;
            if ((retval = nc4_find_type(grp->file->nc4_info, var->xtype, &type_info)))
               return retval;
            if (type_info && type_info->class == NC_VLEN)
               total = sizeof(hvl_t);
            else
               total = type_len;
            for (d = 0; d < var->ndims; d++)
            {
               if (chunksizes[d] < 1)
                  return NC_EBADCHUNK;
               total *= chunksizes[d];
            }
            
            if (total > NC_MAX_UINT)
               return NC_EBADCHUNK;
      }

      /* Set the chunksizes for this variable. */
      for (d = 0; d < var->ndims; d++)
         var->chunksizes[d] = chunksizes[d];
   }

   /* Is this a variable with a chunksize greater than the current
    * cache size? */
   if (contiguous == NC_CHUNKED && (chunksizes || deflate))
   {
      /* Determine default chunksizes for this variable. */
      if (!var->chunksizes[0])
	 if ((retval = nc4_find_default_chunksizes(var)))
	    return retval;

      /* Adjust the cache. */
      if ((retval = nc4_adjust_var_cache(grp, var)))
	 return retval;
   }

   /* Are we setting a fill modes? */
   if (no_fill)
   {
      if (*no_fill)
         var->no_fill = 1;
      else
         var->no_fill = 0;
   }

   /* Are we setting a fill value? */
   if (fill_value && !var->no_fill)
   {
      /* If fill value hasn't been set, allocate space. */
      if ((retval = nc4_get_typelen_mem(h5, var->xtype, 0, &type_size)))
         return retval;
      if (!var->fill_value)
         if (!(var->fill_value = malloc(type_size)))
            return NC_ENOMEM;

      /* Copy the fill_value. */
      LOG((4, "Copying fill value into metadata for variable %s", 
           var->name));
      memcpy(var->fill_value, fill_value, type_size);

      /* If there's a _FillValue attribute, delete it. */
      retval = nc_del_att(ncid, varid, _FillValue);
      if (retval && retval != NC_ENOTATT)
         return retval;

      /* Create a _FillValue attribute. */
      if ((retval = nc_put_att(ncid, varid, _FillValue, var->xtype, 1, fill_value)))
         return retval;
   }

   /* Is the user setting the endianness? */
   if (endianness)
      var->type_info->endianness = *endianness;

   return NC_NOERR;
}

/* Set the deflate level for a var, lower is faster, higher is
 * better. Must be called after nc_def_var and before nc_enddef or any
 * functions which writes data to the file. */
int
nc_def_var_deflate(int ncid, int varid, int shuffle, int deflate, 
                   int deflate_level)
{
   return nc_def_var_extra(ncid, varid, &shuffle, &deflate, 
                           &deflate_level, NULL, 0, NULL, NULL, NULL, NULL, NULL, NULL);
}

/* Find out compression settings of a var. */
int
nc_inq_var_deflate(int ncid, int varid, int *shufflep, 
                   int *deflatep, int *deflate_levelp)
{
   return nc_inq_var_all(ncid, varid, NULL, NULL, NULL, NULL, NULL, 
                         shufflep, deflatep, deflate_levelp, 
                         NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
}

/* Set the szip settings for a var. Must be called after nc_def_var
 * and before nc_enddef or any functions which writes data to the
 * file. NOTE: THIS FUNCTION REMOVED BY THE COMITTEE FOR THE REMOVAL
 * OF STUPID FUNCTIONS. FOR A COMPLETE EXPLANATION, SEE THE COMMITTTEE
 * FOR THE PROVISION OF COMPLETE EXPLANATIONS. */
/* int */
/* nc_def_var_szip(int ncid, int varid, int options_mask, int pixels_per_block) */
/* { */
/*    return nc_def_var_extra(ncid, varid, NULL, NULL,  */
/*                            NULL, NULL, 0, NULL, NULL, NULL, NULL, &options_mask,  */
/* 			   &pixels_per_block); */
/* } */

/* Find out szip settings of a var. */
int
nc_inq_var_szip(int ncid, int varid, int *options_maskp, 
		int *pixels_per_blockp)
{
   return nc_inq_var_all(ncid, varid, NULL, NULL, NULL, NULL, NULL, 
                         NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 
			 NULL, options_maskp, pixels_per_blockp);
}

/* Set checksum for a var. This must be called after the nc_def_var
 * but before the nc_enddef. */
int
nc_def_var_fletcher32(int ncid, int varid, int fletcher32)
{
   return nc_def_var_extra(ncid, varid, NULL, NULL, NULL, &fletcher32, 
                           0, NULL, NULL, NULL, NULL, NULL, NULL);
}
   
/* Find out if checksum is in use for a var. */
int
nc_inq_var_fletcher32(int ncid, int varid, int *fletcher32p)
{
   return nc_inq_var_all(ncid, varid, NULL, NULL, NULL, NULL, NULL, 
                         NULL, NULL, NULL, fletcher32p, NULL, NULL, 
                         NULL, NULL, NULL, NULL, NULL);
}

/* Define chunking stuff for a var. This must be done after nc_def_var
   and before nc_enddef. 

   Chunking is required in any dataset with one or more unlimited
   dimension in HDF5, or any dataset using a filter.

   Where chunksize is a pointer to an array of size ndims, with the
   chunksize in each dimension. 
*/
int
nc_def_var_chunking(int ncid, int varid, int contiguous, const size_t *chunksizesp)
{
   return nc_def_var_extra(ncid, varid, NULL, NULL, NULL, NULL, 
                           contiguous, chunksizesp, NULL, NULL, NULL, NULL, NULL);
}

/* Inquire about chunking stuff for a var. This is a private,
 * undocumented function, used by the f77 API to avoid size_t
 * problems. */
int
nc_inq_var_chunking_ints(int ncid, int varid, int *contiguousp, int *chunksizesp)
{
   NC_FILE_INFO_T *nc;
   NC_GRP_INFO_T *grp; 
   NC_HDF5_FILE_INFO_T *h5;
   NC_VAR_INFO_T *var;
   size_t *cs = NULL;
   int i, retval;

   /* Find this ncid's file info. */
   if ((retval = nc4_find_nc_grp_h5(ncid, &nc, &grp, &h5)))
      return retval;
   assert(nc);

   /* Must be netcdf-4. */
   if (!h5)
      return NC_ENOTNC4;

   /* Find var cause I need the number of dims. */
   if ((retval = nc4_find_g_var_nc(nc, ncid, varid, &grp, &var)))
      return retval;
   
   /* Allocate space for the size_t copy of the chunksizes array. */
   if (var->ndims)
      if (!(cs = malloc(var->ndims * sizeof(size_t))))
	 return NC_ENOMEM;
   
   retval = nc_inq_var_all(ncid, varid, NULL, NULL, NULL, NULL, NULL, 
                           NULL, NULL, NULL, NULL, contiguousp, cs, NULL,
                           NULL, NULL, NULL, NULL);

   /* Copy to size_t array. */
   for (i = 0; i < var->ndims; i++)
   {
      chunksizesp[i] = cs[i];
      if (cs[i] > NC_MAX_INT)
	 retval = NC_ERANGE;
   }

   if (var->ndims)
      free(cs);
   return retval;
}

/* This function defines the chunking with ints, which works better
 * with F77 portability. It is a secret function, which has been
 * rendered unmappable, and it is impossible to apparate anywhere in
 * this function. */
int
nc_def_var_chunking_ints(int ncid, int varid, int contiguous, int *chunksizesp)
{
   NC_FILE_INFO_T *nc;
   NC_GRP_INFO_T *grp; 
   NC_HDF5_FILE_INFO_T *h5;
   NC_VAR_INFO_T *var;
   size_t *cs = NULL;
   int i, retval;

   /* Find this ncid's file info. */
   if ((retval = nc4_find_nc_grp_h5(ncid, &nc, &grp, &h5)))
      return retval;
   assert(nc);

   /* Must be netcdf-4. */
   if (!h5)
      return NC_ENOTNC4;

   /* Find var cause I need the number of dims. */
   if ((retval = nc4_find_g_var_nc(nc, ncid, varid, &grp, &var)))
      return retval;
   
   /* Allocate space for the size_t copy of the chunksizes array. */
   if (var->ndims)
      if (!(cs = malloc(var->ndims * sizeof(size_t))))
	 return NC_ENOMEM;
   
   /* Copy to size_t array. */
   for (i = 0; i < var->ndims; i++)
      cs[i] = chunksizesp[i];

   retval = nc_def_var_extra(ncid, varid, NULL, NULL, NULL, NULL, 
                             contiguous, cs, NULL, NULL, NULL, NULL, NULL);

   if (var->ndims)
      free(cs);
   return retval;
}

/* Inquire about chunking stuff for a var. */
int
nc_inq_var_chunking(int ncid, int varid, int *contiguousp, size_t *chunksizesp)
{
   return nc_inq_var_all(ncid, varid, NULL, NULL, NULL, NULL, NULL, 
                         NULL, NULL, NULL, NULL, contiguousp, chunksizesp, NULL,
                         NULL, NULL, NULL, NULL);
}

/* Define fill value behavior for a variable. This must be done after
   nc_def_var and before nc_enddef. */
int
nc_def_var_fill(int ncid, int varid, int no_fill, const void *fill_value)
{
   return nc_def_var_extra(ncid, varid, NULL, NULL, NULL, NULL, 0, 
                           NULL, &no_fill, fill_value, NULL, NULL, NULL);
}


/* Inq fill value setting for a var. */
int
nc_inq_var_fill(int ncid, int varid, int *no_fillp, void *fill_valuep)
{
   return nc_inq_var_all(ncid, varid, NULL, NULL, NULL, NULL, NULL, 
                         NULL, NULL, NULL, NULL, NULL, NULL, 
                         no_fillp, fill_valuep, NULL, NULL, NULL);
}

/* Define the endianness of a variable. */
int
nc_def_var_endian(int ncid, int varid, int endianness)
{
   return nc_def_var_extra(ncid, varid, NULL, NULL, NULL, NULL, 0,
                           NULL, NULL, NULL, &endianness, NULL, NULL);
}

/* Learn about the endianness of a variable. */
int
nc_inq_var_endian(int ncid, int varid, int *endiannessp)
{
   return nc_inq_var_all(ncid, varid, NULL, NULL, NULL, NULL, NULL, 
                         NULL, NULL, NULL, NULL, NULL, NULL,
                         NULL, NULL, endiannessp, NULL, NULL);
}

/* Get var id from name. */
int
nc_inq_varid(int ncid, const char *name, int *varidp)
{
   NC_FILE_INFO_T *nc;
   NC_GRP_INFO_T *grp; 
   NC_HDF5_FILE_INFO_T *h5;
   NC_VAR_INFO_T *var;
   char norm_name[NC_MAX_NAME + 1];
   int retval;
   
   if (!name)
      return NC_EINVAL;
   if (!varidp)
      return NC_NOERR;

   LOG((2, "nc_inq_varid: ncid 0x%x name %s", ncid, name));
   
   /* Find info for this file and group, and set pointer to each. */
   if ((retval = nc4_find_nc_grp_h5(ncid, &nc, &grp, &h5)))
      return retval;
   
   /* Handle netcdf-3. */
   if (!h5)
      return nc3_inq_varid(nc->int_ncid, name, varidp);
   
   /* Normalize name. */
   if ((retval = nc4_normalize_name(name, norm_name)))
      return retval;

   /* Find var of this name. */
   for (var = grp->var; var; var = var->next)
      if (!(strcmp(var->name, norm_name)))
      {
         *varidp = var->varid;
         return NC_NOERR;
      }

   return NC_ENOTVAR;
}

/* Get name from varid. */
int 
nc_inq_varname(int ncid, int varid, char *name)
{
   return nc_inq_var(ncid, varid, name, NULL, NULL, 
                     NULL, NULL);
}

/* Return a var's type. */
int 
nc_inq_vartype(int ncid, int varid, nc_type *xtypep)
{
   return nc_inq_var(ncid, varid, NULL, xtypep, NULL, 
                     NULL, NULL);
}

/* Return number of dims for a var. */
int 
nc_inq_varndims(int ncid, int varid, int *ndimsp)
{
   return nc_inq_var(ncid, varid, NULL, NULL, ndimsp, 
                     NULL, NULL);
}

/* Return the dimids of a var. */
int 
nc_inq_vardimid(int ncid, int varid, int *dimidsp)
{
   return nc_inq_var(ncid, varid, NULL, NULL, NULL, dimidsp, NULL);
}

/* Return the number of atts of a var. */
int 
nc_inq_varnatts(int ncid, int varid, int *nattsp)
{
   NC_FILE_INFO_T *nc;

   LOG((2, "nc_inq_varnatts: ncid 0x%x varid %d", ncid, varid));

   /* Find metadata for this file. */
   if (!(nc = nc4_find_nc_file(ncid)))
      return NC_EBADID;

   /* Handle netcdf-3 cases. */
   if (!nc->nc4_info)
      return nc3_inq_varnatts(nc->int_ncid, varid, nattsp);

   /* Handle netcdf-4 cases. */
   return nc_inq_var(ncid, varid, NULL, NULL, NULL, NULL, nattsp);
}

/* Rename a var to "bubba," for example.
   
   According to the netcdf-3.5 docs: If the new name is longer than
   the old name, the netCDF dataset must be in define mode.  */
int
nc_rename_var(int ncid, int varid, const char *name)
{
   NC_FILE_INFO_T *nc;
   NC_GRP_INFO_T *grp; 
   NC_HDF5_FILE_INFO_T *h5;
   NC_VAR_INFO_T *var;
   int retval = NC_NOERR;

   LOG((2, "nc_rename_var: ncid 0x%x varid %d name %s", 
        ncid, varid, name));

   /* Find info for this file and group, and set pointer to each. */
   if ((retval = nc4_find_nc_grp_h5(ncid, &nc, &grp, &h5)))
      return retval;
   
#ifdef USE_PNETCDF
   /* Take care of files created/opened with parallel-netcdf library. */
   if (nc->pnetcdf_file)
      return ncmpi_rename_var(nc->int_ncid, varid, name);
#endif /* USE_PNETCDF */

   /* Take care of netcdf-3 files. */
   if (!h5)
      return nc3_rename_var(nc->int_ncid, varid, name);

   /* Is the new name too long? */
   if (strlen(name) > NC_MAX_NAME)
      return NC_EMAXNAME;

   /* Trying to write to a read-only file? No way, Jose! */
   if (h5->no_write)
      return NC_EPERM;

   /* Check name validity, if strict nc3 rules are in effect for this
    * file. */
   if ((retval = NC_check_name(name)))
      return retval;

   /* Is name in use? */
   for (var = grp->var; var; var = var->next)
      if (!strncmp(var->name, name, NC_MAX_NAME))
         return NC_ENAMEINUSE;   

   /* Find the var. */
   for (var = grp->var; var; var = var->next)
      if (var->varid == varid)
         break;
   if (!var)
      return NC_ENOTVAR;

   /* If we're not in define mode, new name must be of equal or
      less size, if strict nc3 rules are in effect for this . */
   if (!(h5->flags & NC_INDEF) && strlen(name) > strlen(var->name) &&
       (h5->cmode & NC_CLASSIC_MODEL))
      return NC_ENOTINDEFINE;

   /* Change the HDF5 file, if this var has already been created
      there. */
   if (var->created)
   {
      if (H5Gmove(grp->hdf_grpid, var->name, name) < 0)
         BAIL(NC_EHDFERR);
   }

   /* Now change the name in our metadata. */
   strcpy(var->name, name);

  exit:
   return retval;
}

/* This will copy a variable from one file to another, assuming
   dimensions in output file are already defined and have same
   dimension ids.

   This function must work even if the files are different formats,
   (i.e. one old netcdf, the other hdf5-netcdf.)

   But if you're copying into a netcdf-3 file, from a netcdf-4 file,
   you must be copying a var of one of the six netcdf-3
   types. Similarly for the attributes.

*/
int
nc_copy_var(int ncid_in, int varid_in, int ncid_out)
{
   char name[NC_MAX_NAME + 1];
   char att_name[NC_MAX_NAME + 1];
   nc_type xtype;
   int ndims, dimids[NC_MAX_DIMS], natts, real_ndims;
   int varid_out;
   int a, d;
   void *data = NULL;
   size_t *count = NULL, *start = NULL;
   size_t reclen = 1;
   size_t *dimlen = NULL;
   int retval = NC_NOERR;
   size_t type_size;
   int src_format, dest_format;
   char type_name[NC_MAX_NAME+1];

   /* Learn about this var. */
   if ((retval = nc_inq_var(ncid_in, varid_in, name, &xtype, 
                            &ndims, dimids, &natts)))
      return retval;

   LOG((2, "nc_copy_var: ncid_in 0x%x varid_in %d ncid_out 0x%x", 
        ncid_in, varid_in, ncid_out));

   /* Make sure we are not trying to write into a netcdf-3 file
    * anything that won't fit in netcdf-3. */
   if ((retval = nc_inq_format(ncid_in, &src_format)))
      return retval;
   if ((retval = nc_inq_format(ncid_out, &dest_format)))
      return retval;
   if ((dest_format == NC_FORMAT_CLASSIC || dest_format == NC_FORMAT_64BIT) &&
       src_format == NC_FORMAT_NETCDF4 && xtype > NC_DOUBLE)
      return NC_ENOTNC4;

   /* Later on, we will need to know the size of this type. */
   if ((retval = nc_inq_type(ncid_in, xtype, type_name, &type_size)))
      return retval;
   LOG((3, "type %s has size %d", type_name, type_size));

   /* Switch back to define mode, and create the output var. */
   retval = nc_redef(ncid_out);
   if (retval && retval != NC_EINDEFINE)
      BAIL(retval);
   if ((retval = nc_def_var(ncid_out, name, xtype,
                            ndims, dimids, &varid_out)))
      BAIL(retval);

   /* Copy the attributes. */
   for (a=0; a<natts; a++)
   {
      if ((retval = nc_inq_attname(ncid_in, varid_in, a, att_name)))
         BAIL(retval);
      if ((retval = nc_copy_att(ncid_in, varid_in, att_name, 
                             ncid_out, varid_out)))
         BAIL(retval);
   }

   /* End define mode, to write metadata and create file. */
   nc_enddef(ncid_out);
   nc_sync(ncid_out);

   /* Allocate memory for our start and count arrays. If ndims = 0
      this is a scalar, which I will treat as a 1-D array with one
      element. */
   real_ndims = ndims ? ndims : 1;
   if (!(start = malloc(real_ndims * sizeof(size_t))))
      BAIL(NC_ENOMEM);
   if (!(count = malloc(real_ndims * sizeof(size_t))))
      BAIL(NC_ENOMEM);

   /* The start array will be all zeros, except the first element,
      which will be the record number. Count will be the dimension
      size, except for the first element, which will be one, because
      we will copy one record at a time. For this we need the var
      shape. */
   if (!(dimlen = malloc(real_ndims * sizeof(size_t))))
      BAIL(NC_ENOMEM);

   /* Find out how much data. */
   for (d=0; d<ndims; d++)
   {
      if ((retval = nc_inq_dimlen(ncid_in, dimids[d], &dimlen[d])))
         BAIL(retval);
      LOG((4, "nc_copy_var: there are %d data", dimlen[d]));
   }

   /* If this is really a scalar, then set the dimlen to 1. */
   if (ndims == 0)
      dimlen[0] = 1;

   for (d=0; d<real_ndims; d++)
   {
      start[d] = 0;
      count[d] = d ? dimlen[d] : 1;
      if (d) reclen *= dimlen[d];
   }

   /* If there are no records, we're done. */
   if (!dimlen[0])
      goto exit;

   /* Allocate memory for one record. */
   if (!(data = malloc(reclen * type_size)))
      return NC_ENOMEM;
   
   /* Copy the var data one record at a time. */
   for (start[0]=0; !retval && start[0]<(size_t)dimlen[0]; start[0]++)
   {
      switch (xtype)
      {
      case NC_BYTE:
         retval = nc_get_vara_schar(ncid_in, varid_in, start, count,
                                 (signed char *)data);
         if (!retval)
            retval = nc_put_vara_schar(ncid_out, varid_out, start, count, 
                                   (const signed char *)data);
         break;
      case NC_CHAR:
         retval = nc_get_vara_text(ncid_in, varid_in, start, count,
                               (char *)data);
         if (!retval)
            retval = nc_put_vara_text(ncid_out, varid_out, start, count, 
                                  (char *)data);
         break;
      case NC_SHORT:
         retval = nc_get_vara_short(ncid_in, varid_in, start, count,
                                (short *)data);
         if (!retval)
            retval = nc_put_vara_short(ncid_out, varid_out, start, count,
                                   (short *)data);
         break;
      case NC_INT:
         retval = nc_get_vara_int(ncid_in, varid_in, start, count,
                              (int *)data);
         if (!retval)
            retval = nc_put_vara_int(ncid_out, varid_out, start, count,
                                 (int *)data);
         break;
      case NC_FLOAT:
         retval = nc_get_vara_float(ncid_in, varid_in, start, count,
                                (float *)data);
         if (!retval)
            retval = nc_put_vara_float(ncid_out, varid_out, start, count,
                                   (float *)data);
         break;
      case NC_DOUBLE:
         retval = nc_get_vara_double(ncid_in, varid_in, start, count,
                                 (double *)data);
         if (!retval)
            retval = nc_put_vara_double(ncid_out, varid_out, start, count, 
                                    (double *)data);
         break;
      case NC_UBYTE:
         retval = nc_get_vara_ubyte(ncid_in, varid_in, start, count,
                                    (unsigned char *)data);
         if (!retval)
            retval = nc_put_vara_ubyte(ncid_out, varid_out, start, count, 
                                       (unsigned char *)data);
         break;
      case NC_USHORT:
         retval = nc_get_vara_ushort(ncid_in, varid_in, start, count,
                                     (unsigned short *)data);
         if (!retval)
            retval = nc_put_vara_ushort(ncid_out, varid_out, start, count, 
                                        (unsigned short *)data);
         break;
      case NC_UINT:
         retval = nc_get_vara_uint(ncid_in, varid_in, start, count,
                                   (unsigned int *)data);
         if (!retval)
            retval = nc_put_vara_uint(ncid_out, varid_out, start, count, 
                                      (unsigned int *)data);
         break;
      case NC_UINT64:
         retval = nc_get_vara_ulonglong(ncid_in, varid_in, start, count,
                                     (unsigned long long *)data);
         if (!retval)
            retval = nc_put_vara_ulonglong(ncid_out, varid_out, start, count, 
                                        (unsigned long long *)data);
         break;
      case NC_INT64:
         retval = nc_get_vara_longlong(ncid_in, varid_in, start, count,
                                       (long long *)data);
         if (!retval)
            retval = nc_put_vara_longlong(ncid_out, varid_out, start, count, 
                                       (long long *)data);
         break;
      default:
         retval = NC_EBADTYPE;
      }
   }
    
 exit:
   if (data) free(data);
   if (dimlen) free(dimlen);
   if (start) free(start);
   if (count) free(count);
   return retval;
}

/*#ifdef USE_PARALLEL*/
int
nc_var_par_access(int ncid, int varid, int par_access) 
{
   NC_FILE_INFO_T *nc; 
   NC_GRP_INFO_T *grp; 
   NC_HDF5_FILE_INFO_T *h5;
   NC_VAR_INFO_T *var;
   int retval;

   LOG((1, "nc_var_par_access: ncid 0x%x varid %d par_access %d", ncid, 
        varid, par_access));

   if (par_access != NC_INDEPENDENT && par_access != NC_COLLECTIVE)
      return NC_EINVAL;
   
   /* Find info for this file and group, and set pointer to each. */
   if ((retval = nc4_find_nc_grp_h5(ncid, &nc, &grp, &h5)))
      return retval;

#ifdef USE_PNETCDF
   /* Handle files opened/created with parallel-netcdf library. */
   if (nc->pnetcdf_file)
   {
      if (par_access == nc->pnetcdf_access_mode)
	 return NC_NOERR;

      nc->pnetcdf_access_mode = par_access;
      if (par_access == NC_INDEPENDENT)
	 return ncmpi_begin_indep_data(nc->int_ncid);
      else
	 return ncmpi_end_indep_data(nc->int_ncid);
   }
#endif /* USE_PNETCDF */   
   
   /* This function only for files opened with nc_open_par or nc_create_par. */
   if (!h5->parallel)
      return NC_ENOPAR;

   /* Find the var, and set its preference. */
   for (var = grp->var; var; var = var->next)
      if (var->varid == varid)
         break;
   if (!var)
      return NC_ENOTVAR;

   if (par_access) 
      var->parallel_access = NC_COLLECTIVE;
   else
      var->parallel_access = NC_INDEPENDENT;

   return NC_NOERR;
}
/*#endif*/ /* USE_PARALLEL */

static int
nc4_put_var1_tc(int ncid, int varid, nc_type file_type, nc_type mem_type, 
                int mem_type_is_long, const size_t *indexp, const void *op)
{
   NC_FILE_INFO_T *nc;

   LOG((2, "nc4_put_var1_tc: ncid 0x%x varid %d", ncid, varid));

   if (!(nc = nc4_find_nc_file(ncid)))
      return NC_EBADID;

#ifdef USE_PNETCDF
   /* var1 functions are not supported by the parallel-netcdf library. */
   if (nc->pnetcdf_file)
      return NC_EINVAL;
#endif /* USE_PNETCDF */   
   
   /* Handle netCDF-3 cases. */
   if (!nc->nc4_info)
   {
      if (mem_type == NC_UBYTE)
         mem_type = NC_BYTE;
      switch(mem_type)
      {
         case NC_NAT:
            return nc3_put_var1(nc->int_ncid, varid, indexp, op);
         case NC_BYTE:
            return nc3_put_var1_schar(nc->int_ncid, varid, indexp, op);
         case NC_CHAR:
            return nc3_put_var1_text(nc->int_ncid, varid, indexp, op);
         case NC_SHORT:
            return nc3_put_var1_short(nc->int_ncid, varid, indexp, op);
         case NC_INT:
            if (mem_type_is_long)
               return nc3_put_var1_long(nc->int_ncid, varid, indexp, op);
            else
               return nc3_put_var1_int(nc->int_ncid, varid, indexp, op);
         case NC_FLOAT:
            return nc3_put_var1_float(nc->int_ncid, varid, indexp, op);
         case NC_DOUBLE:
            return nc3_put_var1_double(nc->int_ncid, varid, indexp, op);
         default:
            return NC_EBADTYPE;
      }
   }

   /* Handle netCDF-4 case. */
   return (nc4_pg_var1(PUT, nc, ncid, varid, indexp, mem_type, 
                   mem_type_is_long, (void *)op));
}

/* Get one little itsy-bitsy datum, with type conversion. */
static int
nc4_get_var1_tc(int ncid, int varid, nc_type mem_type, int mem_type_is_long, 
                const size_t *indexp, void *ip)
{
   NC_FILE_INFO_T *nc;

   LOG((2, "nc4_get_var1_tc: ncid 0x%x varid %d mem_type %d mem_type_is_long %d", 
        ncid, varid, mem_type, mem_type_is_long));

   if (!(nc = nc4_find_nc_file(ncid)))
      return NC_EBADID;
   
#ifdef USE_PNETCDF
   /* var1 functions are not supported by the parallel-netcdf library. */
   if (nc->pnetcdf_file)
      return NC_EINVAL;
#endif /* USE_PNETCDF */   
   
   /* Handle netCDF-3 cases. */
   if (!nc->nc4_info)
   {
      switch(mem_type)
      {
         case NC_NAT:
            return nc3_get_var1(nc->int_ncid, varid, indexp, ip);
         case NC_BYTE:
            return nc3_get_var1_schar(nc->int_ncid, varid, indexp, ip);
         case NC_UBYTE:
            return nc3_get_var1_uchar(nc->int_ncid, varid, indexp, ip);
         case NC_CHAR:
            return nc3_get_var1_text(nc->int_ncid, varid, indexp, ip);
         case NC_SHORT:
            return nc3_get_var1_short(nc->int_ncid, varid, indexp, ip);
         case NC_INT:
            if (mem_type_is_long)
               return nc3_get_var1_long(nc->int_ncid, varid, indexp, ip);
            else
               return nc3_get_var1_int(nc->int_ncid, varid, indexp, ip);
         case NC_FLOAT:
            return nc3_get_var1_float(nc->int_ncid, varid, indexp, ip);
         case NC_DOUBLE:
            return nc3_get_var1_double(nc->int_ncid, varid, indexp, ip);
         default:
            return NC_EBADTYPE;
      }
   }
      
   /* Handle netcdf-4 cases. */
   return (nc4_pg_var1(GET, nc, ncid, varid, indexp, mem_type, 
                       mem_type_is_long, ip));
}

int
nc_put_var1(int ncid, int varid, const size_t *indexp, const void *op)
{
   return nc4_put_var1_tc(ncid, varid, NC_NAT, NC_NAT, 0, indexp, op);
}

/* Get one little itsy-bitsy text byte. */
int
nc_get_var1(int ncid, int varid, const size_t *indexp, void *ip)
{
   return nc4_get_var1_tc(ncid, varid, NC_NAT, 0, indexp, ip);
}

/* Put one little itsy-bitsy text byte. */
int
nc_put_var1_text(int ncid, int varid, const size_t *indexp, 
                  const char *op)
{
   return nc4_put_var1_tc(ncid, varid, NC_NAT, NC_CHAR, 0, indexp, op);
}

/* Get one little itsy-bitsy text byte. */
int
nc_get_var1_text(int ncid, int varid, const size_t *indexp, char *ip)
{
   return nc4_get_var1_tc(ncid, varid, NC_CHAR, 0, indexp, ip);
}

int
nc_put_var1_uchar(int ncid, int varid, const size_t *indexp,
                   const unsigned char *op)
{
   return nc4_put_var1_tc(ncid, varid, NC_NAT, NC_UBYTE, 0, indexp, op);
}

int
nc_get_var1_uchar(int ncid, int varid, const size_t *indexp,
                   unsigned char *ip)
{
   return nc4_get_var1_tc(ncid, varid, NC_UBYTE, 0, indexp, ip);
}

int
nc_put_var1_schar(int ncid, int varid, const size_t *indexp,
                   const signed char *op)
{
   return nc4_put_var1_tc(ncid, varid, NC_NAT, NC_BYTE, 0, indexp, op);
}

int
nc_get_var1_schar(int ncid, int varid, const size_t *indexp,
                   signed char *ip)
{
   return nc4_get_var1_tc(ncid, varid, NC_BYTE, 0, indexp, ip);
}

int
nc_put_var1_short(int ncid, int varid, const size_t *indexp,
                   const short *op)
{
   return nc4_put_var1_tc(ncid, varid, NC_NAT, NC_SHORT, 0, indexp, op);
}

int
nc_get_var1_short(int ncid, int varid, const size_t *indexp,
                   short *ip)
{
   return nc4_get_var1_tc(ncid, varid, NC_SHORT, 0, indexp, ip);
}

int
nc_put_var1_int(int ncid, int varid, const size_t *indexp, const int *op)
{
   return nc4_put_var1_tc(ncid, varid, NC_NAT, NC_INT, 0, indexp, op);
}

int
nc_get_var1_int(int ncid, int varid, const size_t *indexp, int *ip)
{
   return nc4_get_var1_tc(ncid, varid, NC_INT, 0, indexp, ip);
}
int
nc_put_var1_long(int ncid, int varid, const size_t *indexp, const long *op)
{
   return nc4_put_var1_tc(ncid, varid, NC_NAT, NC_INT, 1, indexp, op);
}

int
nc_get_var1_long(int ncid, int varid, const size_t *indexp, long *ip)
{
   return nc4_get_var1_tc(ncid, varid, NC_INT, 1, indexp, ip);
}

int
nc_put_var1_float(int ncid, int varid, const size_t *indexp, const float *op)
{
   return nc4_put_var1_tc(ncid, varid, NC_NAT, NC_FLOAT, 0, indexp, op);
}

int
nc_get_var1_float(int ncid, int varid, const size_t *indexp, float *ip)
{
   return nc4_get_var1_tc(ncid, varid, NC_FLOAT, 0, indexp, ip);
}

int
nc_put_var1_double(int ncid, int varid, const size_t *indexp, const double *op)
{
   return nc4_put_var1_tc(ncid, varid, NC_NAT, NC_DOUBLE, 0, indexp, op);
}

int
nc_get_var1_double(int ncid, int varid, const size_t *indexp, 
                    double *ip)
{
   return nc4_get_var1_tc(ncid, varid, NC_DOUBLE, 0, indexp, ip);
}

static int
nc4_put_vara_tc(int ncid, int varid, nc_type mem_type, int mem_type_is_long, 
                const size_t *startp, const size_t *countp, const void *op)
{
   NC_FILE_INFO_T *nc;

   LOG((2, "nc4_put_vara_tc: ncid 0x%x varid %d mem_type %d mem_type_is_long %d", 
        ncid, varid, mem_type, mem_type_is_long));

   if (!(nc = nc4_find_nc_file(ncid)))
      return NC_EBADID;
   
#ifdef USE_PNETCDF
   /* Handle files opened/created with the parallel-netcdf library. */
   if (nc->pnetcdf_file)
   {
      MPI_Offset mpi_start[NC_MAX_DIMS], mpi_count[NC_MAX_DIMS];
      int d;

      /* No NC_LONGs for parallel-netcdf library! */
      if (mem_type_is_long)
	 return NC_EINVAL;
      
      /* We must convert the start, count, and stride arrays to
       * MPI_Offset type. */
      for (d = 0; d < nc->pnetcdf_ndims[varid]; d++)
      {
	 mpi_start[d] = startp[d];
	 mpi_count[d] = countp[d];
      }

      if (nc->pnetcdf_access_mode == NC_INDEPENDENT)
      {
	 switch(mem_type)
	 {
	    case NC_BYTE:
	       return ncmpi_put_vara_schar(nc->int_ncid, varid, mpi_start, mpi_count, op);
	    case NC_UBYTE:
	       return ncmpi_put_vara_uchar(nc->int_ncid, varid, mpi_start, mpi_count, op);
	    case NC_CHAR:
	       return ncmpi_put_vara_text(nc->int_ncid, varid, mpi_start, mpi_count, op);
	    case NC_SHORT:
	       return ncmpi_put_vara_short(nc->int_ncid, varid, mpi_start, mpi_count, op);
	    case NC_INT:
	       return ncmpi_put_vara_int(nc->int_ncid, varid, mpi_start, mpi_count, op);
	    case NC_FLOAT:
	       return ncmpi_put_vara_float(nc->int_ncid, varid, mpi_start, mpi_count, op);
	    case NC_DOUBLE:
	       return ncmpi_put_vara_double(nc->int_ncid, varid, mpi_start, mpi_count, op);
	    case NC_NAT:
	    default:
	       return NC_EBADTYPE;
	 }
      } 
      else
      {
	 switch(mem_type)
	 {
	    case NC_BYTE:
	       return ncmpi_put_vara_schar_all(nc->int_ncid, varid, mpi_start, mpi_count, op);
	    case NC_UBYTE:
	       return ncmpi_put_vara_uchar_all(nc->int_ncid, varid, mpi_start, mpi_count, op);
	    case NC_CHAR:
	       return ncmpi_put_vara_text_all(nc->int_ncid, varid, mpi_start, mpi_count, op);
	    case NC_SHORT:
	       return ncmpi_put_vara_short_all(nc->int_ncid, varid, mpi_start, mpi_count, op);
	    case NC_INT:
	       return ncmpi_put_vara_int_all(nc->int_ncid, varid, mpi_start, mpi_count, op);
	    case NC_FLOAT:
	       return ncmpi_put_vara_float_all(nc->int_ncid, varid, mpi_start, mpi_count, op);
	    case NC_DOUBLE:
	       return ncmpi_put_vara_double_all(nc->int_ncid, varid, mpi_start, mpi_count, op);
	    case NC_NAT:
	    default:
	       return NC_EBADTYPE;
	 }
      }
   }
#endif /* USE_PNETCDF */   
   
   /* Handle netCDF-3 cases. */
   if (!nc->nc4_info)
   {
      if (mem_type == NC_UBYTE)
         mem_type = NC_BYTE;
      switch(mem_type)
      {
         case NC_NAT:
            return nc3_put_vara(nc->int_ncid, varid, startp, countp, op);
         case NC_BYTE:
            return nc3_put_vara_schar(nc->int_ncid, varid, startp, countp, op);
         case NC_CHAR:
            return nc3_put_vara_text(nc->int_ncid, varid, startp, countp, op);
         case NC_SHORT:
            return nc3_put_vara_short(nc->int_ncid, varid, startp, countp, op);
         case NC_INT:
            if (mem_type_is_long)
               return nc3_put_vara_long(nc->int_ncid, varid, startp, countp, op);
            else
               return nc3_put_vara_int(nc->int_ncid, varid, startp, countp, op);
         case NC_FLOAT:
            return nc3_put_vara_float(nc->int_ncid, varid, startp, countp, op);
         case NC_DOUBLE:
            return nc3_put_vara_double(nc->int_ncid, varid, startp, countp, op);
         default:
            return NC_EBADTYPE;
      }
   }

   return nc4_put_vara(nc, ncid, varid, startp, countp, mem_type, 
                       mem_type_is_long, (void *)op);
}

int 
nc4_get_hdf4_vara(NC_FILE_INFO_T *nc, int ncid, int varid, const size_t *startp, 
		  const size_t *countp, nc_type mem_nc_type, int is_long, void *data)
{
#ifdef USE_HDF4   
   NC_GRP_INFO_T *grp, *g;
   NC_HDF5_FILE_INFO_T *h5;
   NC_VAR_INFO_T *var;
   NC_DIM_INFO_T *dim;
   int32 start32[NC_MAX_DIMS], edge32[NC_MAX_DIMS];
   int retval, d;
   
   /* Find our metadata for this file, group, and var. */
   assert(nc);
   if ((retval = nc4_find_g_var_nc(nc, ncid, varid, &grp, &var)))
      return retval;
   h5 = nc->nc4_info;
   assert(grp && h5 && var && var->name);
   
   for (d = 0; d < var->ndims; d++)
   {
      start32[d] = startp[d];
      edge32[d] = countp[d];
   }
   
   if (SDreaddata(var->sdsid, start32, NULL, edge32, data))
      return NC_EHDFERR;

#endif /* USE_HDF4 */
   return NC_NOERR;
}

/* Get an array. */
static int
nc4_get_vara_tc(int ncid, int varid, nc_type mem_type, int mem_type_is_long,
                const size_t *startp, const size_t *countp, void *ip)
{
   NC_FILE_INFO_T *nc;

   LOG((2, "nc4_get_vara_tc: ncid 0x%x varid %d mem_type %d mem_type_is_long %d", 
        ncid, varid, mem_type, mem_type_is_long));

   if (!(nc = nc4_find_nc_file(ncid)))
      return NC_EBADID;
   
#ifdef USE_PNETCDF
   /* Handle files opened/created with the parallel-netcdf library. */
   if (nc->pnetcdf_file)
   {
      MPI_Offset mpi_start[NC_MAX_DIMS], mpi_count[NC_MAX_DIMS];
      int d;

      /* No NC_LONGs for parallel-netcdf library! */
      if (mem_type_is_long)
	 return NC_EINVAL;
      
      /* We must convert the start, count, and stride arrays to
       * MPI_Offset type. */
      for (d = 0; d < nc->pnetcdf_ndims[varid]; d++)
      {
	 mpi_start[d] = startp[d];
	 mpi_count[d] = countp[d];
      }

      if (nc->pnetcdf_access_mode == NC_INDEPENDENT)
      {
	 switch(mem_type)
	 {
	    case NC_BYTE:
	       return ncmpi_get_vara_schar(nc->int_ncid, varid, mpi_start, mpi_count, ip);
	    case NC_UBYTE:
	       return ncmpi_get_vara_uchar(nc->int_ncid, varid, mpi_start, mpi_count, ip);
	    case NC_CHAR:
	       return ncmpi_get_vara_text(nc->int_ncid, varid, mpi_start, mpi_count, ip);
	    case NC_SHORT:
	       return ncmpi_get_vara_short(nc->int_ncid, varid, mpi_start, mpi_count, ip);
	    case NC_INT:
	       return ncmpi_get_vara_int(nc->int_ncid, varid, mpi_start, mpi_count, ip);
	    case NC_FLOAT:
	       return ncmpi_get_vara_float(nc->int_ncid, varid, mpi_start, mpi_count, ip);
	    case NC_DOUBLE:
	       return ncmpi_get_vara_double(nc->int_ncid, varid, mpi_start, mpi_count, ip);
	    case NC_NAT:
	    default:
	       return NC_EBADTYPE;
	 }
      } 
      else
      {
	 switch(mem_type)
	 {
	    case NC_BYTE:
	       return ncmpi_get_vara_schar_all(nc->int_ncid, varid, mpi_start, mpi_count, ip);
	    case NC_UBYTE:
	       return ncmpi_get_vara_uchar_all(nc->int_ncid, varid, mpi_start, mpi_count, ip);
	    case NC_CHAR:
	       return ncmpi_get_vara_text_all(nc->int_ncid, varid, mpi_start, mpi_count, ip);
	    case NC_SHORT:
	       return ncmpi_get_vara_short_all(nc->int_ncid, varid, mpi_start, mpi_count, ip);
	    case NC_INT:
	       return ncmpi_get_vara_int_all(nc->int_ncid, varid, mpi_start, mpi_count, ip);
	    case NC_FLOAT:
	       return ncmpi_get_vara_float_all(nc->int_ncid, varid, mpi_start, mpi_count, ip);
	    case NC_DOUBLE:
	       return ncmpi_get_vara_double_all(nc->int_ncid, varid, mpi_start, mpi_count, ip);
	    case NC_NAT:
	    default:
	       return NC_EBADTYPE;
	 }
      }
   }
#endif /* USE_PNETCDF */   
   
   /* Handle netCDF-3 cases. */
   if (!nc->nc4_info)
   {
      switch(mem_type)
      {
         case NC_NAT:
            return nc3_get_vara(nc->int_ncid, varid, startp, countp, ip);
         case NC_BYTE:
            return nc3_get_vara_schar(nc->int_ncid, varid, startp, countp, ip);
         case NC_UBYTE:
            return nc3_get_vara_uchar(nc->int_ncid, varid, startp, countp, ip);
         case NC_CHAR:
            return nc3_get_vara_text(nc->int_ncid, varid, startp, countp, ip);
         case NC_SHORT:
            return nc3_get_vara_short(nc->int_ncid, varid, startp, countp, ip);
         case NC_INT:
            if (mem_type_is_long)
               return nc3_get_vara_long(nc->int_ncid, varid, startp, countp, ip);
            else
               return nc3_get_vara_int(nc->int_ncid, varid, startp, countp, ip);
         case NC_FLOAT:
            return nc3_get_vara_float(nc->int_ncid, varid, startp, countp, ip);
         case NC_DOUBLE:
            return nc3_get_vara_double(nc->int_ncid, varid, startp, countp, ip);
         default:
            return NC_EBADTYPE;
      }
   }

   /* Handle HDF4 cases. */
   if (nc->nc4_info->hdf4)
      return nc4_get_hdf4_vara(nc, ncid, varid, startp, countp, mem_type, 
			       mem_type_is_long, (void *)ip);
   
   /* Handle HDF5 cases. */
   return nc4_get_vara(nc, ncid, varid, startp, countp, mem_type, 
                       mem_type_is_long, (void *)ip);
}


int
nc_put_vara_text(int ncid, int varid, const size_t *startp, 
                  const size_t *countp, const char *op)
{
   return nc4_put_vara_tc(ncid, varid, NC_CHAR, 0, startp, countp, op);
}

/* Get an array of text. */
int
nc_get_vara_text(int ncid, int varid, const size_t *startp, 
                  const size_t *countp, char *ip)
{
   return nc4_get_vara_tc(ncid, varid, NC_CHAR, 0, startp, countp, ip);
}

/* Put an array of text. */
int
nc_put_vara_uchar(int ncid, int varid, const size_t *startp, 
                   const size_t *countp, const unsigned char *op)
{
   return nc4_put_vara_tc(ncid, varid, NC_UBYTE, 0, startp, countp, op);
}

int
nc_get_vara_uchar(int ncid, int varid, const size_t *startp, 
                  const size_t *countp, unsigned char *ip)
{
   return nc4_get_vara_tc(ncid, varid, NC_UBYTE, 0, startp, countp, ip);
}

int
nc_put_vara_schar(int ncid, int varid, const size_t *startp, 
                   const size_t *countp, const signed char *op)
{
   return nc4_put_vara_tc(ncid, varid, NC_BYTE, 0, startp, countp, op);
}

int
nc_get_vara_schar(int ncid, int varid, const size_t *startp, 
                   const size_t *countp, signed char *ip)
{
   return nc4_get_vara_tc(ncid, varid, NC_BYTE, 0, startp, countp, ip);
}

int
nc_put_vara_short(int ncid, int varid, const size_t *startp, 
                   const size_t *countp, const short *op)
{
   return nc4_put_vara_tc(ncid, varid, NC_SHORT, 0, startp, countp, op);
}

int
nc_get_vara_short(int ncid, int varid, const size_t *startp, 
                   const size_t *countp, short *ip)
{
   return nc4_get_vara_tc(ncid, varid, NC_SHORT, 0, startp, countp, ip);
}

int
nc_put_vara_int(int ncid, int varid, const size_t *startp, 
                 const size_t *countp, const int *op)
{
   return nc4_put_vara_tc(ncid, varid, NC_INT, 0, startp, countp, op);
}

int
nc_get_vara_int(int ncid, int varid, const size_t *startp, 
                 const size_t *countp, int *ip)
{
   return nc4_get_vara_tc(ncid, varid, NC_INT, 0, startp, countp, ip);
}

int
nc_put_vara_long(int ncid, int varid, const size_t *startp, 
                  const size_t *countp, const long *op)
{
   return nc4_put_vara_tc(ncid, varid, NC_INT, 1, startp, countp, op);
}

int
nc_get_vara_long(int ncid, int varid, const size_t *startp, 
                  const size_t *countp, long *ip)
{
   return nc4_get_vara_tc(ncid, varid, NC_INT, 1, startp, countp, ip);
}

int
nc_put_vara_float(int ncid, int varid, const size_t *startp, 
                   const size_t *countp, const float *op)
{
   return nc4_put_vara_tc(ncid, varid, NC_FLOAT, 0, startp, countp, op);
}

int
nc_get_vara_float(int ncid, int varid, const size_t *startp, 
                   const size_t *countp, float *ip)
{
   return nc4_get_vara_tc(ncid, varid, NC_FLOAT, 0, startp, countp, ip);
}

int
nc_put_vara_double(int ncid, int varid, const size_t *startp, 
                    const size_t *countp, const double *op)
{
   return nc4_put_vara_tc(ncid, varid, NC_DOUBLE, 0, startp, countp, op);
}

int
nc_get_vara_double(int ncid, int varid, const size_t *startp, 
                    const size_t *countp, double *ip)
{
   return nc4_get_vara_tc(ncid, varid, NC_DOUBLE, 0, startp, countp, ip);
}

static int
nc4_put_vars_tc(int ncid, int varid, nc_type mem_type, int mem_type_is_long,
                const size_t *startp, const size_t *countp, 
                const ptrdiff_t *stridep, const void *op)
{
   NC_FILE_INFO_T *nc;

   LOG((2, "nc4_put_vars_tc: ncid 0x%x varid %d", ncid, varid));

   if (!(nc = nc4_find_nc_file(ncid)))
      return NC_EBADID;
   
#ifdef USE_PNETCDF
   /* Handle files opened/created with the parallel-netcdf library. */
   if (nc->pnetcdf_file)
   {
      MPI_Offset mpi_start[NC_MAX_DIMS], mpi_count[NC_MAX_DIMS];
      MPI_Offset mpi_stride[NC_MAX_DIMS];
      int d;

      /* No NC_LONGs for parallel-netcdf library! */
      if (mem_type_is_long)
	 return NC_EINVAL;
      
      /* We must convert the start, count, and stride arrays to
       * MPI_Offset type. */
      for (d = 0; d < nc->pnetcdf_ndims[varid]; d++)
      {
	 mpi_start[d] = startp[d];
	 mpi_count[d] = countp[d];
	 mpi_stride[d] = stridep[d];
      }

      if (nc->pnetcdf_access_mode == NC_INDEPENDENT)
      {
	 switch(mem_type)
	 {
	    case NC_BYTE:
	       return ncmpi_put_vars_schar(nc->int_ncid, varid, mpi_start, mpi_count, mpi_stride, op);
	    case NC_UBYTE:
	       return ncmpi_put_vars_uchar(nc->int_ncid, varid, mpi_start, mpi_count, mpi_stride, op);
	    case NC_CHAR:
	       return ncmpi_put_vars_text(nc->int_ncid, varid, mpi_start, mpi_count, mpi_stride, op);
	    case NC_SHORT:
	       return ncmpi_put_vars_short(nc->int_ncid, varid, mpi_start, mpi_count, mpi_stride, op);
	    case NC_INT:
	       return ncmpi_put_vars_int(nc->int_ncid, varid, mpi_start, mpi_count, mpi_stride, op);
	    case NC_FLOAT:
	       return ncmpi_put_vars_float(nc->int_ncid, varid, mpi_start, mpi_count, mpi_stride, op);
	    case NC_DOUBLE:
	       return ncmpi_put_vars_double(nc->int_ncid, varid, mpi_start, mpi_count, mpi_stride, op);
	    case NC_NAT:
	    default:
	       return NC_EBADTYPE;
	 }
      } 
      else
      {
	 switch(mem_type)
	 {
	    case NC_BYTE:
	       return ncmpi_put_vars_schar_all(nc->int_ncid, varid, mpi_start, mpi_count, mpi_stride, op);
	    case NC_UBYTE:
	       return ncmpi_put_vars_uchar_all(nc->int_ncid, varid, mpi_start, mpi_count, mpi_stride, op);
	    case NC_CHAR:
	       return ncmpi_put_vars_text_all(nc->int_ncid, varid, mpi_start, mpi_count, mpi_stride, op);
	    case NC_SHORT:
	       return ncmpi_put_vars_short_all(nc->int_ncid, varid, mpi_start, mpi_count, mpi_stride, op);
	    case NC_INT:
	       return ncmpi_put_vars_int_all(nc->int_ncid, varid, mpi_start, mpi_count, mpi_stride, op);
	    case NC_FLOAT:
	       return ncmpi_put_vars_float_all(nc->int_ncid, varid, mpi_start, mpi_count, mpi_stride, op);
	    case NC_DOUBLE:
	       return ncmpi_put_vars_double_all(nc->int_ncid, varid, mpi_start, mpi_count, mpi_stride, op);
	    case NC_NAT:
	    default:
	       return NC_EBADTYPE;
	 }
      }
   }
#endif /* USE_PNETCDF */   
   
   /* Handle netCDF-3 cases. */
   if (!nc->nc4_info)
   {
      if (mem_type == NC_UBYTE)
         mem_type = NC_BYTE;
      switch(mem_type)
      {
         case NC_NAT:
            return nc3_put_vars(nc->int_ncid, varid, startp, countp, stridep, op);
         case NC_BYTE:
            return nc3_put_vars_schar(nc->int_ncid, varid, startp, countp, stridep, op);
         case NC_CHAR:
            return nc3_put_vars_text(nc->int_ncid, varid, startp, countp, stridep, op);
         case NC_SHORT:
            return nc3_put_vars_short(nc->int_ncid, varid, startp, countp, stridep, op);
         case NC_INT:
            if (mem_type_is_long)
               return nc3_put_vars_long(nc->int_ncid, varid, startp, countp, stridep, op);
            else
               return nc3_put_vars_int(nc->int_ncid, varid, startp, countp, stridep, op);
         case NC_FLOAT:
            return nc3_put_vars_float(nc->int_ncid, varid, startp, countp, stridep, op);
         case NC_DOUBLE:
            return nc3_put_vars_double(nc->int_ncid, varid, startp, countp, stridep, op);
         default:
            return NC_EBADTYPE;
      }
   }

   return nc4_pg_varm(PUT, nc, ncid, varid, startp, countp, stridep, NULL,
                  mem_type, mem_type_is_long, (void *)op);
}

static int
nc4_get_vars_tc(int ncid, int varid, nc_type mem_type, int mem_type_is_long, 
                const size_t *startp, const size_t *countp, const ptrdiff_t *stridep, 
                void *ip)
{
   NC_FILE_INFO_T *nc;

   LOG((2, "nc4_get_vars_tc: ncid 0x%x varid %d mem_type %d mem_type_is_long %d", 
        ncid, varid, mem_type, mem_type_is_long));

   if (!(nc = nc4_find_nc_file(ncid)))
      return NC_EBADID;
   
#ifdef USE_PNETCDF
   /* Handle files opened/created with the parallel-netcdf library. */
   if (nc->pnetcdf_file)
   {
      MPI_Offset mpi_start[NC_MAX_DIMS], mpi_count[NC_MAX_DIMS];
      MPI_Offset mpi_stride[NC_MAX_DIMS];
      int d;

      /* No NC_LONGs for parallel-netcdf library! */
      if (mem_type_is_long)
	 return NC_EINVAL;
      
      /* We must convert the start, count, and stride arrays to
       * MPI_Offset type. */
      for (d = 0; d < nc->pnetcdf_ndims[varid]; d++)
      {
	 mpi_start[d] = startp[d];
	 mpi_count[d] = countp[d];
	 mpi_stride[d] = stridep[d];
      }

      if (nc->pnetcdf_access_mode == NC_INDEPENDENT)
      {
	 switch(mem_type)
	 {
	    case NC_BYTE:
	       return ncmpi_get_vars_schar(nc->int_ncid, varid, mpi_start, 
					   mpi_count, mpi_stride, ip);
	    case NC_UBYTE:
	       return ncmpi_get_vars_uchar(nc->int_ncid, varid, mpi_start, 
					   mpi_count, mpi_stride, ip);
	    case NC_CHAR:
	       return ncmpi_get_vars_text(nc->int_ncid, varid, mpi_start, 
					  mpi_count, mpi_stride, ip);
	    case NC_SHORT:
	       return ncmpi_get_vars_short(nc->int_ncid, varid, mpi_start, 
					   mpi_count, mpi_stride, ip);
	    case NC_INT:
	       return ncmpi_get_vars_int(nc->int_ncid, varid, mpi_start, 
					 mpi_count, mpi_stride, ip);
	    case NC_FLOAT:
	       return ncmpi_get_vars_float(nc->int_ncid, varid, mpi_start, 
					   mpi_count, mpi_stride, ip);
	    case NC_DOUBLE:
	       return ncmpi_get_vars_double(nc->int_ncid, varid, mpi_start, 
					    mpi_count, mpi_stride, ip);
	    case NC_NAT:
	    default:
	       return NC_EBADTYPE;
	 }
      } 
      else
      {
	 switch(mem_type)
	 {
	    case NC_BYTE:
	       return ncmpi_get_vars_schar_all(nc->int_ncid, varid, mpi_start, 
					       mpi_count, mpi_stride, ip);
	    case NC_UBYTE:
	       return ncmpi_get_vars_uchar_all(nc->int_ncid, varid, mpi_start, 
					       mpi_count, mpi_stride, ip);
	    case NC_CHAR:
	       return ncmpi_get_vars_text_all(nc->int_ncid, varid, mpi_start, 
					      mpi_count, mpi_stride, ip);
	    case NC_SHORT:
	       return ncmpi_get_vars_short_all(nc->int_ncid, varid, mpi_start, 
					       mpi_count, mpi_stride, ip);
	    case NC_INT:
	       return ncmpi_get_vars_int_all(nc->int_ncid, varid, mpi_start, 
					     mpi_count, mpi_stride, ip);
	    case NC_FLOAT:
	       return ncmpi_get_vars_float_all(nc->int_ncid, varid, mpi_start, 
					       mpi_count, mpi_stride, ip);
	    case NC_DOUBLE:
	       return ncmpi_get_vars_double_all(nc->int_ncid, varid, mpi_start, 
						mpi_count, mpi_stride, ip);
	    case NC_NAT:
	    default:
	       return NC_EBADTYPE;
	 }
      }
   }
#endif /* USE_PNETCDF */   
   
   /* Handle netCDF-3 cases. */
   if (!nc->nc4_info)
   {
      switch(mem_type)
      {
         case NC_NAT:
            return nc3_get_vars(nc->int_ncid, varid, startp, countp, stridep, ip);
         case NC_BYTE:
            return nc3_get_vars_schar(nc->int_ncid, varid, startp, countp, stridep, ip);
         case NC_UBYTE:
            return nc3_get_vars_uchar(nc->int_ncid, varid, startp, countp, stridep, ip);
         case NC_CHAR:
            return nc3_get_vars_text(nc->int_ncid, varid, startp, countp, stridep, ip);
         case NC_SHORT:
            return nc3_get_vars_short(nc->int_ncid, varid, startp, countp, stridep, ip);
         case NC_INT:
            if (mem_type_is_long)
               return nc3_get_vars_long(nc->int_ncid, varid, startp, countp, stridep, ip);
            else
               return nc3_get_vars_int(nc->int_ncid, varid, startp, countp, stridep, ip);
         case NC_FLOAT:
            return nc3_get_vars_float(nc->int_ncid, varid, startp, countp, stridep, ip);
         case NC_DOUBLE:
            return nc3_get_vars_double(nc->int_ncid, varid, startp, countp, stridep, ip);
         default:
            return NC_EBADTYPE;
      }
   }

   return nc4_pg_varm(GET, nc, ncid, varid, startp, countp, stridep, NULL,
                  mem_type, mem_type_is_long, (void *)ip);
}


int
nc_put_vars(int ncid, int varid, const size_t *startp, 
            const size_t *countp, const ptrdiff_t *stridep,
            const void *op)
{
   return nc4_put_vars_tc(ncid, varid, NC_NAT, 0, startp, countp, 
                          stridep, op);
}

int
nc_get_vars(int ncid, int varid, const size_t *startp, 
            const size_t *countp, const ptrdiff_t *stridep,
            void *ip)
{
   return nc4_get_vars_tc(ncid, varid, NC_NAT, 0, startp, countp, 
                          stridep, ip);
}

int
nc_put_vars_text(int ncid, int varid, const size_t *startp, 
                  const size_t *countp, const ptrdiff_t *stridep,
                  const char *op)
{
   return nc4_put_vars_tc(ncid, varid, NC_CHAR, 0, startp, countp, 
                          stridep, op);
}

int
nc_get_vars_text(int ncid, int varid, const size_t *startp, 
                  const size_t *countp, const ptrdiff_t *stridep,
                  char *ip)
{
   return nc4_get_vars_tc(ncid, varid, NC_CHAR, 0, startp, countp, 
                          stridep, ip);
}

int
nc_put_vars_uchar(int ncid, int varid, const size_t *startp, 
                   const size_t *countp, const ptrdiff_t *stridep,
                   const unsigned char *op)
{
   return nc4_put_vars_tc(ncid, varid, NC_UBYTE, 0, startp, countp, 
                          stridep, op);
}

int
nc_get_vars_uchar(int ncid, int varid, const size_t *startp, 
                   const size_t *countp, const ptrdiff_t *stridep,
                   unsigned char *ip)
{
   return nc4_get_vars_tc(ncid, varid, NC_UBYTE, 0, startp, countp, 
                          stridep, ip);
}

int
nc_put_vars_schar(int ncid, int varid, const size_t *startp, 
                   const size_t *countp, const ptrdiff_t *stridep,
                   const signed char *op)
{
   return nc4_put_vars_tc(ncid, varid, NC_BYTE, 0, startp, countp, 
                          stridep, op);
}

int
nc_get_vars_schar(int ncid, int varid, const size_t *startp, 
                   const size_t *countp, const ptrdiff_t *stridep,
                   signed char *ip)
{
   return nc4_get_vars_tc(ncid, varid, NC_BYTE, 0, startp, countp, 
                          stridep, ip);
}

int
nc_put_vars_short(int ncid, int varid, const size_t *startp, 
                   const size_t *countp, const ptrdiff_t *stridep,
                   const short *op)
{
   return nc4_put_vars_tc(ncid, varid, NC_SHORT, 0, startp, countp, 
                          stridep, op);
}

int
nc_get_vars_short(int ncid, int varid, const size_t *startp, 
                   const size_t *countp, const ptrdiff_t *stridep,
                   short *ip)
{
   return nc4_get_vars_tc(ncid, varid, NC_SHORT, 0, startp, countp, 
                          stridep, ip);
}

int
nc_put_vars_int(int ncid, int varid, const size_t *startp, 
                 const size_t *countp, const ptrdiff_t *stridep,
                 const int *op)
{
   return nc4_put_vars_tc(ncid, varid, NC_INT, 0, startp, countp, 
                          stridep, op);
}

int
nc_get_vars_int(int ncid, int varid, const size_t *startp, 
                 const size_t *countp, const ptrdiff_t *stridep,
                 int *ip)
{
   return nc4_get_vars_tc(ncid, varid, NC_INT, 0, startp, countp, 
                          stridep, ip);
}

int
nc_put_vars_long(int ncid, int varid, const size_t *startp, 
                  const size_t *countp, const ptrdiff_t *stridep,
                  const long *op)
{
   return nc4_put_vars_tc(ncid, varid, NC_INT, 1, startp, countp, 
                          stridep, op);
}

int
nc_get_vars_long(int ncid, int varid, const size_t *startp, 
                  const size_t *countp, const ptrdiff_t *stridep,
                  long *ip)
{
   return nc4_get_vars_tc(ncid, varid, NC_INT, 1, startp, countp, 
                          stridep, ip);
}

int
nc_put_vars_float(int ncid, int varid, const size_t *startp, 
                   const size_t *countp, const ptrdiff_t *stridep,
                   const float *op)
{
   return nc4_put_vars_tc(ncid, varid, NC_FLOAT, 0, startp, countp, 
                          stridep, op);
}

int
nc_get_vars_float(int ncid, int varid, const size_t *startp, 
                   const size_t *countp, const ptrdiff_t *stridep,
                   float *ip)
{
   return nc4_get_vars_tc(ncid, varid, NC_FLOAT, 0, startp, countp, 
                          stridep, ip);
}

int
nc_put_vars_double(int ncid, int varid, const size_t *startp, 
                    const size_t *countp, const ptrdiff_t *stridep,
                    const double *op)
{
   return nc4_put_vars_tc(ncid, varid, NC_DOUBLE, 0, startp, countp, 
                          stridep, op);
}

int
nc_get_vars_double(int ncid, int varid, const size_t *startp, 
                    const size_t *countp, const ptrdiff_t *stridep,
                    double *ip)
{
   return nc4_get_vars_tc(ncid, varid, NC_DOUBLE, 0, startp, countp, 
                          stridep, ip);
}

static int
nc4_put_varm_tc(int ncid, int varid, nc_type mem_type, int mem_type_is_long,
                const size_t *startp, const size_t *countp, 
                const ptrdiff_t *stridep, const ptrdiff_t *imapp, 
                const void *op)
{
   NC_FILE_INFO_T *nc;

   LOG((2, "nc4_put_varm_tc: ncid 0x%x varid %d mem_type %d mem_type_is_long %d", 
        ncid, varid, mem_type, mem_type_is_long));

   if (!(nc = nc4_find_nc_file(ncid)))
      return NC_EBADID;

#ifdef USE_PNETCDF
   /* varm functions are not supported by the parallel-netcdf library. */
   if (nc->pnetcdf_file)
      return NC_EINVAL;
#endif /* USE_PNETCDF */   
   
   /* Handle netCDF-3 cases. */
   if (!nc->nc4_info)
   {
      if (mem_type == NC_UBYTE)
         mem_type = NC_BYTE;
      switch(mem_type)
      {
         case NC_NAT:
            return nc3_put_varm(nc->int_ncid, varid, startp, countp, stridep, imapp, op);
         case NC_BYTE:
            return nc3_put_varm_schar(nc->int_ncid, varid, startp, countp, stridep, imapp, op);
         case NC_CHAR:
            return nc3_put_varm_text(nc->int_ncid, varid, startp, countp, stridep, imapp, op);
         case NC_SHORT:
            return nc3_put_varm_short(nc->int_ncid, varid, startp, countp, stridep, imapp, op);
         case NC_INT:
            if (mem_type_is_long)
               return nc3_put_varm_long(nc->int_ncid, varid, startp, countp, stridep, imapp, op);
            else
               return nc3_put_varm_int(nc->int_ncid, varid, startp, countp, stridep, imapp, op);
         case NC_FLOAT:
            return nc3_put_varm_float(nc->int_ncid, varid, startp, countp, stridep, imapp, op);
         case NC_DOUBLE:
            return nc3_put_varm_double(nc->int_ncid, varid, startp, countp, stridep, imapp, op);
         default:
            return NC_EBADTYPE;
      }
   }

   return nc4_pg_varm(PUT, nc, ncid, varid, startp, countp, stridep, 
                  imapp, mem_type, mem_type_is_long, (void *)op);
}

static int
nc4_get_varm_tc(int ncid, int varid, nc_type mem_type, int mem_type_is_long, 
               const size_t *startp, const size_t *countp, const ptrdiff_t *stridep,
               const ptrdiff_t *imapp, void *ip)
{
   NC_FILE_INFO_T *nc;

   LOG((2, "nc4_get_varm_tc: ncid 0x%x varid %d mem_type %d mem_type_is_long %d", 
        ncid, varid, mem_type, mem_type_is_long));

   if (!(nc = nc4_find_nc_file(ncid)))
      return NC_EBADID;
   
#ifdef USE_PNETCDF
   /* varm functions are not supported by the parallel-netcdf library. */
   if (nc->pnetcdf_file)
      return NC_EINVAL;
#endif /* USE_PNETCDF */   
   
   /* Handle netCDF-3 cases. */
   if (!nc->nc4_info)
   {
      switch(mem_type)
      {
         case NC_NAT:
            return nc3_get_varm(nc->int_ncid, varid, startp, countp, stridep, imapp, ip);
         case NC_BYTE:
            return nc3_get_varm_schar(nc->int_ncid, varid, startp, countp, stridep, imapp, ip);
         case NC_UBYTE:
            return nc3_get_varm_uchar(nc->int_ncid, varid, startp, countp, stridep, imapp, ip);
         case NC_CHAR:
            return nc3_get_varm_text(nc->int_ncid, varid, startp, countp, stridep, imapp, ip);
         case NC_SHORT:
            return nc3_get_varm_short(nc->int_ncid, varid, startp, countp, stridep, imapp, ip);
         case NC_INT:
            if (mem_type_is_long)
               return nc3_get_varm_long(nc->int_ncid, varid, startp, countp, stridep, imapp, ip);
            else
               return nc3_get_varm_int(nc->int_ncid, varid, startp, countp, stridep, imapp, ip);
         case NC_FLOAT:
            return nc3_get_varm_float(nc->int_ncid, varid, startp, countp, stridep, imapp, ip);
         case NC_DOUBLE:
            return nc3_get_varm_double(nc->int_ncid, varid, startp, countp, stridep, imapp, ip);
         default:
            return NC_EBADTYPE;
      }
   }

   return nc4_pg_varm(GET, nc, ncid, varid, startp, countp, stridep, 
                      imapp, mem_type, mem_type_is_long, (void *)ip);
}

int
nc_put_varm(int ncid, int varid, const size_t *startp, 
            const size_t *countp, const ptrdiff_t *stridep,
            const ptrdiff_t *imapp, const void *op)
{
   return nc4_put_varm_tc(ncid, varid, NC_NAT, 0, startp, 
                          countp, stridep, imapp, op);
}

int
nc_get_varm(int ncid, int varid, const size_t *startp, 
            const size_t *countp, const ptrdiff_t *stridep,
            const ptrdiff_t *imapp, void *ip)
{
   return nc4_get_varm_tc(ncid, varid, NC_NAT, 0, startp, 
                          countp, stridep, imapp, ip);
}

int
nc_put_varm_text(int ncid, int varid, const size_t *startp, 
                  const size_t *countp, const ptrdiff_t *stridep,
                  const ptrdiff_t *imapp, const char *op)
{
   return nc4_put_varm_tc(ncid, varid, NC_CHAR, 0, startp, 
                          countp, stridep, imapp, op);
}

int
nc_get_varm_text(int ncid, int varid, const size_t *startp, 
                  const size_t *countp, const ptrdiff_t *stridep,
                  const ptrdiff_t *imapp, char *ip)
{
   return nc4_get_varm_tc(ncid, varid, NC_CHAR, 0, startp, 
                          countp, stridep, imapp, ip);
}

int
nc_put_varm_uchar(int ncid, int varid, const size_t *startp, 
                   const size_t *countp, const ptrdiff_t *stridep,
                   const ptrdiff_t *imapp, const unsigned char *op)
{
   return nc4_put_varm_tc(ncid, varid, NC_UBYTE, 0, startp, 
                          countp, stridep, imapp, op);
}

int
nc_get_varm_uchar(int ncid, int varid, const size_t *startp, 
                   const size_t *countp, const ptrdiff_t *stridep,
                   const ptrdiff_t *imapp, unsigned char *ip)
{
   return nc4_get_varm_tc(ncid, varid, NC_UBYTE, 0, startp, 
                          countp, stridep, imapp, ip);
}

int
nc_put_varm_schar(int ncid, int varid, const size_t *startp, 
                   const size_t *countp, const ptrdiff_t *stridep,
                   const ptrdiff_t *imapp, const signed char *op)
{
   return nc4_put_varm_tc(ncid, varid, NC_BYTE, 0, startp, 
                          countp, stridep, imapp, op);
}

int
nc_get_varm_schar(int ncid, int varid, const size_t *startp, 
                   const size_t *countp, const ptrdiff_t *stridep,
                   const ptrdiff_t *imapp, signed char *ip)
{
   return nc4_get_varm_tc(ncid, varid, NC_BYTE, 0, startp, 
                          countp, stridep, imapp, ip);
}

int
nc_put_varm_short(int ncid, int varid, const size_t *startp, 
                   const size_t *countp, const ptrdiff_t *stridep,
                   const ptrdiff_t *imapp, const short *op)
{
   return nc4_put_varm_tc(ncid, varid, NC_SHORT, 0, startp, 
                          countp, stridep, imapp, op);
}

int
nc_get_varm_short(int ncid, int varid, const size_t *startp, 
                   const size_t *countp, const ptrdiff_t *stridep,
                   const ptrdiff_t *imapp, short *ip)
{
   return nc4_get_varm_tc(ncid, varid, NC_SHORT, 0, startp, 
                          countp, stridep, imapp, ip);
}

int
nc_put_varm_int(int ncid, int varid, const size_t *startp, 
                 const size_t *countp, const ptrdiff_t *stridep,
                 const ptrdiff_t *imapp, const int *op)
{
   return nc4_put_varm_tc(ncid, varid, NC_INT, 0, startp, 
                          countp, stridep, imapp, op);
}

int
nc_get_varm_int(int ncid, int varid, const size_t *startp, 
                 const size_t *countp, const ptrdiff_t *stridep,
                 const ptrdiff_t *imapp, int *ip)
{
   return nc4_get_varm_tc(ncid, varid, NC_INT, 0, startp, 
                          countp, stridep, imapp, ip);
}

int
nc_put_varm_long(int ncid, int varid, const size_t *startp, 
                  const size_t *countp, const ptrdiff_t *stridep,
                  const ptrdiff_t *imapp, const long *op)
{
   return nc4_put_varm_tc(ncid, varid, NC_INT, 1, startp, 
                          countp, stridep, imapp, op);
}

int
nc_get_varm_long(int ncid, int varid, const size_t *startp, 
                  const size_t *countp, const ptrdiff_t *stridep,
                  const ptrdiff_t *imapp, long *ip)
{
   return nc4_get_varm_tc(ncid, varid, NC_INT, 1, startp, 
                          countp, stridep, imapp, ip);
}

int
nc_put_varm_float(int ncid, int varid, const size_t *startp, 
                   const size_t *countp, const ptrdiff_t *stridep,
                   const ptrdiff_t *imapp, const float *op)
{
   return nc4_put_varm_tc(ncid, varid, NC_FLOAT, 0, startp, 
                          countp, stridep, imapp, op);
}

int
nc_get_varm_float(int ncid, int varid, const size_t *startp, 
                   const size_t *countp, const ptrdiff_t *stridep,
                   const ptrdiff_t *imapp, float *ip)
{
   return nc4_get_varm_tc(ncid, varid, NC_FLOAT, 0, startp, 
                          countp, stridep, imapp, ip);
}

int
nc_put_varm_double(int ncid, int varid, const size_t *startp, 
                    const size_t *countp, const ptrdiff_t *stridep,
                    const ptrdiff_t *imapp, const double *op)
{
   return nc4_put_varm_tc(ncid, varid, NC_DOUBLE, 0, startp, 
                          countp, stridep, imapp, op);
}

int
nc_get_varm_double(int ncid, int varid, const size_t *startp, 
                    const size_t *countp, const ptrdiff_t *stridep,
                    const ptrdiff_t * imapp, double *ip)
{
   return nc4_get_varm_tc(ncid, varid, NC_DOUBLE, 0, startp, 
                          countp, stridep, imapp, ip);
}

static int
nc4_put_var_tc(int ncid, int varid, nc_type mem_type, int mem_type_is_long, 
               const void *op)
{
   NC_FILE_INFO_T *nc;
   int ret;

   LOG((2, "nc4_put_var_tc: ncid 0x%x varid %d mem_type %d mem_type_is_long %d", 
        ncid, varid, mem_type, mem_type_is_long));

   if (!(nc = nc4_find_nc_file(ncid)))
      return NC_EBADID;
   
#ifdef USE_PNETCDF
   /* whole var functions are not supported by the parallel-netcdf library. */
   if (nc->pnetcdf_file)
      return NC_EINVAL;
#endif /* USE_PNETCDF */   
   
   /* Handle netCDF-3 cases. */
   if (!nc->nc4_info)
   {
      /* Always do signed bytes, as per the netCDF users guide.*/
      if (mem_type == NC_UBYTE)
         mem_type = NC_BYTE;

      /* If caller did not provide type, then use the variable's
       * type. */
      if (mem_type == NC_NAT)
         if ((ret = nc_inq_vartype(ncid, varid, &mem_type)))
            return ret;

      /* Write the data. */
      switch(mem_type)
      {
         case NC_BYTE:
            return nc3_put_var_schar(nc->int_ncid, varid, op);
         case NC_CHAR:
            return nc3_put_var_text(nc->int_ncid, varid, op);
         case NC_SHORT:
            return nc3_put_var_short(nc->int_ncid, varid, op);
         case NC_INT:
            if (mem_type_is_long)
               return nc3_put_var_long(nc->int_ncid, varid, op);
            else
               return nc3_put_var_int(nc->int_ncid, varid, op);
         case NC_FLOAT:
            return nc3_put_var_float(nc->int_ncid, varid, op);
         case NC_DOUBLE:
            return nc3_put_var_double(nc->int_ncid, varid, op);
         default:
            return NC_EBADTYPE;
      }
   }

   /* Handle netCDF_4 case. */
   return pg_var(PUT, nc, ncid, varid, mem_type, mem_type_is_long, 
                 (void *)op);
}

static int
nc4_get_var_tc(int ncid, int varid, nc_type mem_type, 
               int mem_type_is_long, void *ip)
{
   NC_FILE_INFO_T *nc;

   LOG((2, "nc4_get_var_tc: ncid 0x%x varid %d mem_type %d mem_type_is_long %d", 
        ncid, varid, mem_type, mem_type_is_long));

   if (!(nc = nc4_find_nc_file(ncid)))
      return NC_EBADID;
   
#ifdef USE_PNETCDF
   /* whole var functions are not supported by the parallel-netcdf library. */
   if (nc->pnetcdf_file)
      return NC_EINVAL;
#endif /* USE_PNETCDF */   
   
   /* Handle netCDF-3 cases. */
   if (!nc->nc4_info)
   {
      switch(mem_type)
      {
         case NC_NAT:
            return NC_ENOTNC4;
         case NC_BYTE:
            return nc3_get_var_schar(nc->int_ncid, varid, ip);
         case NC_UBYTE:
            return nc3_get_var_uchar(nc->int_ncid, varid, ip);
         case NC_CHAR:
            return nc3_get_var_text(nc->int_ncid, varid, ip);
         case NC_SHORT:
            return nc3_get_var_short(nc->int_ncid, varid, ip);
         case NC_INT:
            if (mem_type_is_long)
               return nc3_get_var_long(nc->int_ncid, varid, ip);
            else
               return nc3_get_var_int(nc->int_ncid, varid, ip);
         case NC_FLOAT:
            return nc3_get_var_float(nc->int_ncid, varid, ip);
         case NC_DOUBLE:
            return nc3_get_var_double(nc->int_ncid, varid, ip);
         default:
            return NC_EBADTYPE;
      }
   }

   return pg_var(GET, nc, ncid, varid, mem_type, mem_type_is_long, ip);
}

int
nc_put_var_text(int ncid, int varid, const char *op)
{
   return nc4_put_var_tc(ncid, varid, NC_CHAR, 0, op);
}

int
nc_get_var_text(int ncid, int varid, char *ip)
{
   return nc4_get_var_tc(ncid, varid, NC_CHAR, 0, ip);
}

int
nc_put_var_uchar(int ncid, int varid, const unsigned char *op)
{
   return nc4_put_var_tc(ncid, varid, NC_UBYTE, 0, op);
}

int
nc_get_var_uchar(int ncid, int varid, unsigned char *ip)
{
   return nc4_get_var_tc(ncid, varid, NC_UBYTE, 0, ip);
}

int
nc_put_var_schar(int ncid, int varid, const signed char *op)
{
   return nc4_put_var_tc(ncid, varid, NC_BYTE, 0, op);
}

int
nc_get_var_schar(int ncid, int varid, signed char *ip)
{
   return nc4_get_var_tc(ncid, varid, NC_BYTE, 0, ip);
}

int
nc_put_var_short(int ncid, int varid, const short *op)
{
   return nc4_put_var_tc(ncid, varid, NC_SHORT, 0, op);
}

int
nc_get_var_short(int ncid, int varid, short *ip)
{
   return nc4_get_var_tc(ncid, varid, NC_SHORT, 0, ip);
}

int
nc_put_var_int(int ncid, int varid, const int *op)
{
   return nc4_put_var_tc(ncid, varid, NC_INT, 0, op);
}

int
nc_get_var_int(int ncid, int varid, int *ip)
{
   return nc4_get_var_tc(ncid, varid, NC_INT, 0, ip);
}

int
nc_put_var_long(int ncid, int varid, const long *op)
{
   return nc4_put_var_tc(ncid, varid, NC_INT, 1, op);
}

int
nc_get_var_long(int ncid, int varid, long *ip)
{
   return nc4_get_var_tc(ncid, varid, NC_INT, 1, ip);
}

int
nc_put_var_float(int ncid, int varid, const float *op)
{
   return nc4_put_var_tc(ncid, varid, NC_FLOAT, 0, op);
}


int
nc_get_var_float(int ncid, int varid, float *ip)
{
   return nc4_get_var_tc(ncid, varid, NC_FLOAT, 0, ip);
}
   
int
nc_put_var_double(int ncid, int varid, const double *op)
{
   return nc4_put_var_tc(ncid, varid, NC_DOUBLE, 0, op);
}

int
nc_get_var_double(int ncid, int varid, double *ip)
{
   return nc4_get_var_tc(ncid, varid, NC_DOUBLE, 0, ip);
}

int
nc_put_var1_ubyte(int ncid, int varid, const size_t *indexp, 
                  const unsigned char *op)
{
   return nc4_put_var1_tc(ncid, varid, NC_NAT, NC_UBYTE, 0, indexp, op);
}

int
nc_get_var1_ubyte(int ncid, int varid, const size_t *indexp, 
                  unsigned char *ip)
{
   return nc4_get_var1_tc(ncid, varid, NC_UBYTE, 0, indexp, ip);
}

int
nc_put_var1_ushort(int ncid, int varid, const size_t *indexp, 
                   const unsigned short *op)
{
   return nc4_put_var1_tc(ncid, varid, NC_NAT, NC_USHORT, 0, indexp, op);
}

int
nc_get_var1_ushort(int ncid, int varid, const size_t *indexp, 
                   unsigned short *ip)
{
   return nc4_get_var1_tc(ncid, varid, NC_USHORT, 0, indexp, ip);
}

int
nc_put_var1_uint(int ncid, int varid, const size_t *indexp, 
                 const unsigned int *op)
{
   return nc4_put_var1_tc(ncid, varid, NC_NAT, NC_UINT, 0, indexp, op);
}

int
nc_get_var1_uint(int ncid, int varid, const size_t *indexp, 
                 unsigned int *ip)
{
   return nc4_get_var1_tc(ncid, varid, NC_UINT, 0, indexp, ip);
}

int
nc_put_var1_longlong(int ncid, int varid, const size_t *indexp, 
                  const long long *op)
{
   return nc4_put_var1_tc(ncid, varid, NC_NAT, NC_INT64, 0, indexp, op);
}

int
nc_get_var1_longlong(int ncid, int varid, const size_t *indexp, 
                     long long *ip)
{
   return nc4_get_var1_tc(ncid, varid, NC_INT64, 0, indexp, ip);
}

int
nc_put_var1_ulonglong(int ncid, int varid, const size_t *indexp, 
                   const unsigned long long *op)
{
   return nc4_put_var1_tc(ncid, varid, NC_NAT, NC_UINT64, 0, indexp, op);
}

int
nc_get_var1_ulonglong(int ncid, int varid, const size_t *indexp, 
                   unsigned long long *ip)
{
   return nc4_get_var1_tc(ncid, varid, NC_UINT64, 0, indexp, ip);
}

int
nc_put_var1_string(int ncid, int varid, const size_t *indexp, 
                   const char **op)
{
   return nc4_put_var1_tc(ncid, varid, NC_NAT, NC_STRING, 0, indexp, op);
}

int
nc_get_var1_string(int ncid, int varid, const size_t *indexp, 
                   char **ip)
{
   return nc4_get_var1_tc(ncid, varid, NC_STRING, 0, indexp, ip);
}

int
nc_put_vara_ubyte(int ncid, int varid, const size_t *startp, 
                  const size_t *countp, const unsigned char *op)
{
   return nc4_put_vara_tc(ncid, varid, NC_UBYTE, 0, startp, 
                          countp, op);
}

int
nc_get_vara_ubyte(int ncid, int varid, const size_t *startp, 
                  const size_t *countp, unsigned char *ip)
{
   return nc4_get_vara_tc(ncid, varid, NC_UBYTE, 0, startp, 
                          countp, ip);
}

int
nc_put_vara_ushort(int ncid, int varid, const size_t *startp, 
                   const size_t *countp, const unsigned short *op)
{
   return nc4_put_vara_tc(ncid, varid, NC_USHORT, 0, startp, 
                          countp, op);
}

int
nc_get_vara_ushort(int ncid, int varid, const size_t *startp, 
                   const size_t *countp, unsigned short *ip)
{
   return nc4_get_vara_tc(ncid, varid, NC_USHORT, 0, startp, 
                          countp, ip);
}

int
nc_put_vara_uint(int ncid, int varid, const size_t *startp, 
                 const size_t *countp, const unsigned int *op)
{
   return nc4_put_vara_tc(ncid, varid, NC_UINT, 0, startp, 
                          countp, op);
}

int
nc_get_vara_uint(int ncid, int varid, const size_t *startp, 
                 const size_t *countp, unsigned int *ip)
{
   return nc4_get_vara_tc(ncid, varid, NC_UINT, 0, startp, 
                          countp, ip);
}

int
nc_put_vara_longlong(int ncid, int varid, const size_t *startp, 
                  const size_t *countp, const long long *op)
{
   return nc4_put_vara_tc(ncid, varid, NC_INT64, 0, startp, 
                          countp, op);
}

int
nc_get_vara_longlong(int ncid, int varid, const size_t *startp, 
                  const size_t *countp, long long *ip)
{
   return nc4_get_vara_tc(ncid, varid, NC_INT64, 0, startp, 
                          countp, ip);
}

int
nc_put_vara_ulonglong(int ncid, int varid, const size_t *startp, 
                   const size_t *countp, const unsigned long long *op)
{
   return nc4_put_vara_tc(ncid, varid, NC_UINT64, 0, startp, 
                          countp, op);
}

int
nc_get_vara_ulonglong(int ncid, int varid, const size_t *startp, 
                   const size_t *countp, unsigned long long *ip)
{
   return nc4_get_vara_tc(ncid, varid, NC_UINT64, 0, startp, 
                          countp, ip);
}

int
nc_put_vara_string(int ncid, int varid, const size_t *startp, 
                   const size_t *countp, const char **op)
{
   return nc4_put_vara_tc(ncid, varid, NC_STRING, 0, startp, 
                          countp, op);
}

int
nc_get_vara_string(int ncid, int varid, const size_t *startp, 
                   const size_t *countp, char **ip)
{
   return nc4_get_vara_tc(ncid, varid, NC_STRING, 0, startp, 
                          countp, ip);
}

int
nc_put_vars_ubyte(int ncid, int varid, const size_t *startp, 
                  const size_t *countp, const ptrdiff_t *stridep, 
                  const unsigned char *op)
{
   return nc4_put_vars_tc(ncid, varid, NC_UBYTE, 0, startp, countp, 
                          stridep, op);
}

int
nc_get_vars_ubyte(int ncid, int varid, const size_t *startp, 
                  const size_t *countp, const ptrdiff_t *stridep, 
                  unsigned char *ip)
{
   return nc4_get_vars_tc(ncid, varid, NC_UBYTE, 0, startp, countp, 
                          stridep, ip);
}

int
nc_put_vars_ushort(int ncid, int varid, const size_t *startp, 
                   const size_t *countp, const ptrdiff_t *stridep, 
                   const unsigned short *op)
{
   return nc4_put_vars_tc(ncid, varid, NC_USHORT, 0, startp, countp, 
                          stridep, op);
}

int
nc_get_vars_ushort(int ncid, int varid, const size_t *startp, 
                   const size_t *countp, const ptrdiff_t *stridep, 
                   unsigned short *ip)
{
   return nc4_get_vars_tc(ncid, varid, NC_USHORT, 0, startp, countp, 
                          stridep, ip);
}

int
nc_put_vars_uint(int ncid, int varid, const size_t *startp, 
                 const size_t *countp, const ptrdiff_t *stridep, 
                 const unsigned int *op)
{
   return nc4_put_vars_tc(ncid, varid, NC_UINT, 0, startp, countp, 
                          stridep, op);
}

int
nc_get_vars_uint(int ncid, int varid, const size_t *startp, 
                 const size_t *countp, const ptrdiff_t *stridep, 
                 unsigned int *ip)
{
   return nc4_get_vars_tc(ncid, varid, NC_UINT, 0, startp, countp, 
                          stridep, ip);
}

int
nc_put_vars_longlong(int ncid, int varid, const size_t *startp, 
                  const size_t *countp, const ptrdiff_t *stridep, 
                  const long long *op)
{
   return nc4_put_vars_tc(ncid, varid, NC_INT64, 0, startp, countp, 
                          stridep, op);
}

int
nc_get_vars_longlong(int ncid, int varid, const size_t *startp, 
                  const size_t *countp, const ptrdiff_t *stridep, 
                  long long *ip)
{
   return nc4_get_vars_tc(ncid, varid, NC_INT64, 0, startp, countp, 
                          stridep, ip);
}

int
nc_put_vars_ulonglong(int ncid, int varid, const size_t *startp, 
                   const size_t *countp, const ptrdiff_t *stridep, 
                   const unsigned long long *op)
{
   return nc4_put_vars_tc(ncid, varid, NC_UINT64, 0, startp, countp, 
                          stridep, op);
}

int
nc_get_vars_ulonglong(int ncid, int varid, const size_t *startp, 
                   const size_t *countp, const ptrdiff_t *stridep, 
                   unsigned long long *ip)
{
   return nc4_get_vars_tc(ncid, varid, NC_UINT64, 0, startp, countp, 
                          stridep, ip);
}

int
nc_put_vars_string(int ncid, int varid, const size_t *startp, 
                   const size_t *countp, const ptrdiff_t *stridep, 
                   const char **op)
{
   return nc4_put_vars_tc(ncid, varid, NC_STRING, 0, startp, countp, 
                          stridep, op);
}

int
nc_get_vars_string(int ncid, int varid, const size_t *startp, 
                   const size_t *countp, const ptrdiff_t *stridep, 
                   char **ip)
{
   return nc4_get_vars_tc(ncid, varid, NC_STRING, 0, startp, countp, 
                          stridep, ip);
}

int
nc_put_varm_ubyte(int ncid, int varid, const size_t *startp, 
                  const size_t *countp, const ptrdiff_t *stridep, 
                  const ptrdiff_t * imapp, const unsigned char *op)
{
   return nc4_put_varm_tc(ncid, varid, NC_UBYTE, 0, startp, 
                          countp, stridep, imapp, op);
}

int
nc_get_varm_ubyte(int ncid, int varid, const size_t *startp, 
                  const size_t *countp, const ptrdiff_t *stridep, 
                  const ptrdiff_t * imapp, unsigned char *ip)
{
   return nc4_get_varm_tc(ncid, varid, NC_UBYTE, 0, startp, 
                          countp, stridep, imapp, ip);
}

int
nc_put_varm_ushort(int ncid, int varid, const size_t *startp, 
                   const size_t *countp, const ptrdiff_t *stridep, 
                   const ptrdiff_t * imapp, const unsigned short *op)
{
   return nc4_put_varm_tc(ncid, varid, NC_USHORT, 0, startp, 
                          countp, stridep, imapp, op);
}

int
nc_get_varm_ushort(int ncid, int varid, const size_t *startp, 
                   const size_t *countp, const ptrdiff_t *stridep, 
                   const ptrdiff_t * imapp, unsigned short *ip)
{
   return nc4_get_varm_tc(ncid, varid, NC_USHORT, 0, startp, 
                          countp, stridep, imapp, ip);
}

int
nc_put_varm_uint(int ncid, int varid, const size_t *startp, 
                 const size_t *countp, const ptrdiff_t *stridep, 
                 const ptrdiff_t * imapp, const unsigned int *op)
{
   return nc4_put_varm_tc(ncid, varid, NC_UINT, 0, startp, 
                          countp, stridep, imapp, op);
}

int
nc_get_varm_uint(int ncid, int varid, const size_t *startp, 
                 const size_t *countp, const ptrdiff_t *stridep, 
                 const ptrdiff_t * imapp, unsigned int *ip)
{
   return nc4_get_varm_tc(ncid, varid, NC_UINT, 0, startp, 
                          countp, stridep, imapp, ip);
}

int
nc_put_varm_longlong(int ncid, int varid, const size_t *startp, 
                  const size_t *countp, const ptrdiff_t *stridep, 
                  const ptrdiff_t * imapp, const long long *op)
{
   return nc4_put_varm_tc(ncid, varid, NC_INT64, 0, startp, 
                          countp, stridep, imapp, op);
}

int
nc_get_varm_longlong(int ncid, int varid, const size_t *startp, 
                  const size_t *countp, const ptrdiff_t *stridep, 
                  const ptrdiff_t * imapp, long long *ip)
{
   return nc4_get_varm_tc(ncid, varid, NC_INT64, 0, startp, 
                          countp, stridep, imapp, ip);
}

int
nc_put_varm_string(int ncid, int varid, const size_t *startp, 
                  const size_t *countp, const ptrdiff_t *stridep, 
                  const ptrdiff_t * imapp, const char **op)
{
   return nc4_put_varm_tc(ncid, varid, NC_STRING, 0, startp, 
                          countp, stridep, imapp, op);
}

int
nc_get_varm_string(int ncid, int varid, const size_t *startp, 
                  const size_t *countp, const ptrdiff_t *stridep, 
                  const ptrdiff_t * imapp, char **ip)
{
   return nc4_get_varm_tc(ncid, varid, NC_STRING, 0, startp, 
                          countp, stridep, imapp, ip);
}

int
nc_put_varm_ulonglong(int ncid, int varid, const size_t *startp, 
                   const size_t *countp, const ptrdiff_t *stridep, 
                   const ptrdiff_t * imapp, const unsigned long long *op)
{
   return nc4_put_varm_tc(ncid, varid, NC_UINT64, 0, startp, 
                          countp, stridep, imapp, op);
}

int
nc_get_varm_ulonglong(int ncid, int varid, const size_t *startp, 
                   const size_t *countp, const ptrdiff_t *stridep, 
                   const ptrdiff_t * imapp, unsigned long long *ip)
{
   return nc4_get_varm_tc(ncid, varid, NC_UINT64, 0, startp, 
                          countp, stridep, imapp, ip);
}

int
nc_put_var_ubyte(int ncid, int varid, const unsigned char *op)
{
   return nc4_put_var_tc(ncid, varid, NC_UBYTE, 0, op);
}

int
nc_get_var_ubyte(int ncid, int varid, unsigned char *ip)
{
   return nc4_get_var_tc(ncid, varid, NC_UBYTE, 0, ip);
}

int
nc_put_var_ushort(int ncid, int varid, const unsigned short *op)
{
   return nc4_put_var_tc(ncid, varid, NC_USHORT, 0, op);
}

int
nc_get_var_ushort(int ncid, int varid, unsigned short *ip)
{
   return nc4_get_var_tc(ncid, varid, NC_USHORT, 0, ip);
}

int
nc_put_var_uint(int ncid, int varid, const unsigned int *op)
{
   return nc4_put_var_tc(ncid, varid, NC_UINT, 0, op);
}

int
nc_get_var_uint(int ncid, int varid, unsigned int *ip)
{
   return nc4_get_var_tc(ncid, varid, NC_UINT, 0, ip);
}

int
nc_put_var_longlong(int ncid, int varid, const long long *op)
{
   return nc4_put_var_tc(ncid, varid, NC_INT64, 0, op);
}

int
nc_get_var_longlong(int ncid, int varid, long long *ip)
{
   return nc4_get_var_tc(ncid, varid, NC_INT64, 0, ip);
}

int
nc_put_var_ulonglong(int ncid, int varid, const unsigned long long *op)
{
   return nc4_put_var_tc(ncid, varid, NC_UINT64, 0, op);
}

int
nc_get_var_ulonglong(int ncid, int varid, unsigned long long *ip)
{
   return nc4_get_var_tc(ncid, varid, NC_UINT64, 0, ip);
}

int
nc_put_var_string(int ncid, int varid, const char **op)
{
   return nc4_put_var_tc(ncid, varid, NC_STRING, 0, op);
}

int
nc_get_var_string(int ncid, int varid, char **ip)
{
   return nc4_get_var_tc(ncid, varid, NC_STRING, 0, ip);
}

int
nc_put_var(int ncid, int varid, const void *op)
{
   return nc4_put_var_tc(ncid, varid, NC_NAT, 0, op);
}

int
nc_get_var(int ncid, int varid, void *ip)
{
   return nc4_get_var_tc(ncid, varid, NC_NAT, 0, ip);
}

/* Write an array of values. */
int
nc_put_vara(int ncid, int varid, const size_t *startp, 
            const size_t *countp, const void *op)
{
   return nc4_put_vara_tc(ncid, varid, NC_NAT, 0, startp, countp, op);
}

/* Read an array of values. */
int
nc_get_vara(int ncid, int varid, const size_t *startp, 
            const size_t *countp, void *ip)
{
   return nc4_get_vara_tc(ncid, varid, NC_NAT, 0, startp, countp, ip);
}

