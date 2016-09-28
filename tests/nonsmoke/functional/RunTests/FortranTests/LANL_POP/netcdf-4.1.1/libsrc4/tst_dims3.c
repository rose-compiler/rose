/* This is part of the netCDF package. Copyright 2008 University
   Corporation for Atmospheric Research/Unidata See COPYRIGHT file for
   conditions of use. See www.unidata.ucar.edu for more info.

   Test netcdf-4 dimensions inheritance. 

   $Id: tst_dims3.c,v 1.5 2009/10/22 16:40:34 ed Exp $
*/

#include <config.h>
#include "netcdf.h"
#include <nc_tests.h>

int
main(int argc, char **argv)
{
   printf("\n*** Testing netcdf-4 dimension inheritance.\n");
   {
#define FILE_NAME "tst_dims3.nc"
#define RANK_time 1
#define GRP_NAME  "G"
#define TIME_NAME "time"
#define VAR2_NAME "z"
#define TIME_RANK 1
#define NUM_TIMES 2
      int ncid, grpid;
      int time_dim, time_dim_in;
      int time_var, z_var;
      size_t len;
      int time_data[NUM_TIMES] = {1, 2} ;
      size_t time_startset[TIME_RANK] = {0} ;
      size_t time_countset[TIME_RANK] = {NUM_TIMES} ;

      /* Create file with unlimited dim and associated coordinate
       * variable in root group, another variable that uses unlimited
       * dim in subgroup. */
      if (nc_create(FILE_NAME, NC_NETCDF4, &ncid)) ERR;
      if (nc_def_grp(ncid, GRP_NAME, &grpid)) ERR;
      if (nc_def_dim(ncid, TIME_NAME, NC_UNLIMITED, &time_dim)) ERR;
      if (nc_def_var(ncid, TIME_NAME, NC_INT, TIME_RANK, &time_dim, 
		     &time_var)) ERR;
      if (nc_def_var(grpid, VAR2_NAME, NC_INT, TIME_RANK, &time_dim, 
		     &z_var)) ERR;
      if (nc_enddef(ncid)) ERR;

      /* Assign data to time variable, creating two times */
      if (nc_put_vara(ncid, time_dim, time_startset, time_countset, 
		      time_data)) ERR;

      /* Check the dim len from the root group */
      if (nc_inq_dimlen(ncid, time_dim, &len)) ERR;
      if (len != NUM_TIMES) ERR;

      /* Check the dim len from the sub group */
      if (nc_inq_dimlen(grpid, time_dim, &len)) ERR;
      if (len != NUM_TIMES) ERR;
      if (nc_close(ncid)) ERR;

      /* Now check how many times there are from the subgroup */
      if (nc_open(FILE_NAME, NC_NOWRITE, &ncid)) ERR;
      if (nc_inq_ncid(ncid, GRP_NAME, &grpid)) ERR;
      if (nc_inq_dimid(ncid, TIME_NAME, &time_dim)) ERR;

      /* Check the dim len from the root group */
      if (nc_inq_dimlen(ncid, time_dim, &len)) ERR;
      if (len != NUM_TIMES) ERR;

      /* Check the dim len from the sub group */
      if (nc_inq_dimlen(grpid, time_dim, &len)) ERR;
      if (len != NUM_TIMES) ERR;

      /* Find the dimension by name. */
      if (nc_inq_dimid(grpid, TIME_NAME, &time_dim_in)) ERR;
      if (time_dim_in != time_dim) ERR;

      if (nc_close(ncid)) ERR;
   }
   SUMMARIZE_ERR;
   FINAL_RESULTS;
}
