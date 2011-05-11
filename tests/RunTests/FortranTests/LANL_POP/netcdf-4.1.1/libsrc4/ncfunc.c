/*

This file is part of netcdf-4, a netCDF-like interface for HDF5, or a
HDF5 backend for netCDF, depending on your point of view.

This file handles the nc_ calls, calling the appropriate nc3 or nc4
function, depending on ncid.

Copyright 2003, University Corporation for Atmospheric Research. See
netcdf-4/docs/COPYRIGHT file for copying and redistribution
conditions.

$Id: ncfunc.c,v 1.45 2009/08/14 15:05:08 ed Exp $
*/

#include "nc4internal.h"

/* Keep a linked list of file info objects. */
extern NC_FILE_INFO_T *nc_file;

/* Tell the user the version of netCDF. */
static const char nc_libvers[] = PACKAGE_VERSION " of "__DATE__" "__TIME__" $";

const char *
nc_inq_libvers(void)
{
   return nc_libvers;
}

/* Given an error number, return an error message. */
const char *
nc_strerror(int ncerr1)
{
#define MAX_NETCDF4_ERRORS (50)

   /* If this is an error code greater than NC3_LAST_ERROR, the nc3
      library can give us the text. Don't forget that all error
      numbers are negative. */
   if (ncerr1 > NC4_FIRST_ERROR || ncerr1 < NC4_LAST_ERROR)
      return nc3_strerror(ncerr1);

   /* If we're here, this is a netcdf-4 error code. */
   switch(ncerr1)
   {
      case NC_EHDFERR:
	 return "HDF error";
      case NC_ECANTREAD:
	 return "Can't read file";
      case NC_ECANTWRITE:
	 return "Can't write file";
      case NC_ECANTCREATE:
	 return "Can't create file";
      case NC_EFILEMETA:
	 return "Can't add HDF5 file metadata";
      case NC_EDIMMETA:      
	 return "Can't define dimensional metadata";
      case NC_EATTMETA:
	 return "Can't open HDF5 attribute";
      case NC_EVARMETA:
	 return "Problem with variable metadata.";
      case NC_ENOCOMPOUND:
	 return "Can't create HDF5 compound type";
      case NC_EATTEXISTS:
	 return "Attempt to create attribute that alread exists";
      case NC_ENOTNC4:
	 return "Attempting netcdf-4 operation on netcdf-3 file";
      case NC_ESTRICTNC3:
	 return "Attempting netcdf-4 operation on strict nc3 netcdf-4 file";
      case NC_ENOTNC3:
	 return "Attempting netcdf-3 operation on netcdf-4 file";
      case NC_ENOPAR:
	 return "Parallel operation on file opened for non-parallel access";
      case NC_EPARINIT:
	 return "Error initializing for parallel access";
      case NC_EBADGRPID:
	 return "Bad group ID";
      case NC_EBADTYPID:
	 return "Bad type ID";
      case NC_ETYPDEFINED:
	 return "Type has already been defined and may not be edited";
      case NC_EBADFIELD:
	 return "Bad field ID";
      case NC_EBADCLASS:
	 return "Bad class";
      case NC_EMAPTYPE:
	 return "Mapped access for atomic types only";
      case NC_ELATEFILL:
	 return "Attempt to define fill value when data already exists.";
      case NC_ELATEDEF:
	 return "Attempt to define var properties, like deflate, after enddef.";
      case NC_EDIMSCALE:
	 return "Probem with HDF5 dimscales.";
      case NC_ENOGRP:
	 return "No group found.";
      case NC_ESTORAGE:
	 return "Cannot specify both contiguous and chunking.";
      case NC_EBADCHUNK:
	 return "Bad chunk sizes.";
      case NC_ENOTBUILT:
	 return "Attempt to use feature that was not turned on "
	    "when netCDF was built.";
      default:
	 return "Unknown error";
   }
}

/* This function deletes a member of parliment. Be careful! Last time
 * this function was used, Labor got in! This function only does
 * anything for netcdf-3 files. */
int
nc_delete_mp(const char *path, int basepe)
{
   return nc3_delete_mp(path, basepe);
}

/* This will return the length of a netcdf data type in bytes. Since
   we haven't added any new types, I just call the v3 function.
   Ed Hartnett 10/43/03
*/
/* This function only does anything for netcdf-3 files. */
int
nc_set_base_pe(int ncid, int pe)
{
   NC_FILE_INFO_T *nc;
   if (!(nc = nc4_find_nc_file(ncid)))
      return NC_EBADID;
   if (nc->nc4_info)
      return NC_ENOTNC3;
   return nc3_set_base_pe(nc->int_ncid,  pe);
}

/* This function only does anything for netcdf-3 files. */
int
nc_inq_base_pe(int ncid, int *pe)
{
   NC_FILE_INFO_T *nc;
   if (!(nc = nc4_find_nc_file(ncid)))
      return NC_EBADID;
   if (nc->nc4_info)
      return NC_ENOTNC3;
   return nc3_inq_base_pe(nc->int_ncid, pe);
}

/*  This copies the contents of one netcdf file to another, all dims,
 *  vars, and atts.  Assumes new dataset is freshly created, with no
 *  dimensions already defined, because it uses same dimension ids as
 *  in input dataset. */
int 
copy_dataset(int ncid_in, int ncid_out)
{
   int ndims, nvars, ngatts, unlimdimid;
   int d, v, a;
   int res;

   /* Get info from source file. */
   if ((res = nc_inq(ncid_in, &ndims, &nvars, &ngatts, &unlimdimid)))
      return res;

   /* Copy dimensions. */
   for (d=0; d<ndims; d++)
   {
      char name[NC_MAX_NAME + 1];
      size_t len;
      int new_dimid;
      if ((res = nc_inq_dim(ncid_in, d, name, &len)))
	 return res;
      if (d == unlimdimid)
	 len = NC_UNLIMITED;
      if ((res = nc_def_dim(ncid_out, name, len, &new_dimid)))
	 return res;
   }

   /* Copy global attributes. */
   for (a=0; a<ngatts; a++)
   {
      char name[NC_MAX_NAME + 1];
      if((res = nc_inq_attname(ncid_in, NC_GLOBAL, a, name)))
	 return res;
      if ((res = nc_copy_att(ncid_in, NC_GLOBAL, name, 
			     ncid_out, NC_GLOBAL)))
	 return res;
   }

   /* Copy vars. We need to start out of define mode. */
   if ((res = nc_enddef(ncid_out)))
      return res;
   for (v=0; v<nvars; v++)
      if ((res = nc_copy_var(ncid_in, v, ncid_out)))
	 return res;

   return NC_NOERR;
}

/* Get the format (i.e. classic, 64-bit-offset, or netcdf-4) of an
 * open file. */
int
nc_inq_format(int ncid, int *formatp)
{
   NC_FILE_INFO_T *nc;

   LOG((2, "nc_inq_format: ncid 0x%x", ncid));

   if (!formatp)
      return NC_NOERR;

   /* Find the file metadata. */
   if (!(nc = nc4_find_nc_file(ncid)))
      return NC_EBADID;

   /* If this isn't a netcdf-4 file, pass this call on to the netcdf-3
    * library. */
   if (!nc->nc4_info)
      return nc3_inq_format(nc->int_ncid, formatp);
   
   /* Otherwise, this is a netcdf-4 file. Check if classic NC3 rules
    * are in effect for this file. */
   if (nc->nc4_info->cmode & NC_CLASSIC_MODEL)
      *formatp = NC_FORMAT_NETCDF4_CLASSIC;
   else
      *formatp = NC_FORMAT_NETCDF4;

   return NC_NOERR;
}



