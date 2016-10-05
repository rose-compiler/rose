/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright by the Board of Trustees of the University of Illinois.         *
 * All rights reserved.                                                      *
 *                                                                           *
 * This file is part of HDF5.  The full HDF5 copyright notice, including     *
 * terms governing use, modification, and redistribution, is contained in    *
 * the files COPYING and Copyright.html.  COPYING can be found at the root   *
 * of the source code distribution tree; Copyright.html can be found at the  *
 * root level of an installed copy of the electronic HDF5 document set and   *
 * is linked from the top-level documents page.  It can also be found at     *
 * http://hdf.ncsa.uiuc.edu/HDF5/doc/Copyright.html.  If you do not have     *
 * access to either file, you may request a copy from hdfhelp@ncsa.uiuc.edu. *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/*
 * Main driver of the Parallel NetCDF4 tests
 *
 */

#include "tests.h"

#define FILE_NAME "tst_parallel3.nc"

/*2,3,4 dimensional test, the first dimension is unlimited, time.
 */

#define NDIMS1 2
#define NDIMS2 4
#define DIMSIZE /*4 */ 768*2 
#define DIMSIZE2 4
#define DIMSIZE3 4
#define TIMELEN 1

/*BIGFILE, >2G, >4G, >8G file
  big file is created but no actually data is written
  Dimensional size is defined inside the function
*/

#define ATTRNAME1 "valid_range"
#define ATTRNAME2 "scale_factor"
#define ATTRNAME3 "title"

/* The number of processors should be a good number for the
   dimension to be divided evenly, the best set of number of processor
   should be 2 power n. However, for NetCDF4 tests, the following numbers
   are generally treated as good numbers:
   1,2,3,4,6,8,12,16,24,32,48,64,96,128,192,256

   The maximum number of processor is 256.*/

int test_pio(int);
int test_pio_attr(int);
int test_pio_big(int);
int test_pio_hyper(int);

char* getenv_all(MPI_Comm comm, int root, const char* name);
int facc_type;
int facc_type_open;
char file_name[NC_MAX_NAME + 1];

int main(int argc, char **argv)
{
   int mpi_size, mpi_rank;				/* mpi variables */
   int i;
   int NUMP[16] ={1,2,3,4,6,8,12,16,24,32,48,64,96,128,192,256};
   int size_flag = 0;

   /* Un-buffer the stdout and stderr */
   setbuf(stderr, NULL);
   setbuf(stdout, NULL);

   MPI_Init(&argc, &argv);
   MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);
   MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);

   if (mpi_rank == 1)
      printf("\n*** Testing more advanced parallel access.\n");

   for (i = 0; i < 16; i++){
      if(mpi_size == NUMP[i])
      {
	 size_flag = 1;
	 break;
      }
   }
   if(!size_flag){
      printf("mpi_size is wrong\n");
      printf(" The number of processor must be chosen from\n");
      printf(" 1,2,3,4,6,8,12,16,24,32,48,64,96,128,192,256 \n");
      return -1;
   }

   if (facc_type != NC_MPIPOSIX)
      facc_type = NC_MPIIO;

   facc_type = NC_NETCDF4|NC_MPIIO;
   facc_type_open = NC_MPIIO;

   /* Create file name. */
   sprintf(file_name, "%s/%s", TEMP_LARGE, FILE_NAME);

   /* Test NetCDF4 with MPI-IO driver */
   if (mpi_rank == 1)
      printf("*** Testing parallel IO for raw-data with MPI-IO (driver)...");
   if(test_pio(NC_INDEPENDENT)!=0) ERR;
   if(test_pio(NC_COLLECTIVE)!=0) ERR;
   if (mpi_rank == 1)
      SUMMARIZE_ERR;

   if (mpi_rank == 1)
      printf("*** Testing parallel IO for meta-data with MPI-IO (driver)...");
   if(test_pio_attr(NC_INDEPENDENT)!=0) ERR;
   if(test_pio_attr(NC_COLLECTIVE)!=0) ERR;
   if (mpi_rank == 1)
      SUMMARIZE_ERR;

   if (mpi_rank == 1)
      printf("*** Testing parallel IO for different hyperslab selections with MPI-IO (driver)...");
   if(test_pio_hyper(NC_INDEPENDENT)!=0)ERR;
   if(test_pio_hyper(NC_COLLECTIVE)!=0) ERR;
   if (mpi_rank == 1)
      SUMMARIZE_ERR;

   if (mpi_rank == 1)
      printf("*** Testing parallel IO for raw-data with MPIPOSIX-IO (driver)...");
   facc_type = NC_NETCDF4|NC_MPIPOSIX;
   facc_type_open = NC_MPIPOSIX;
   if(test_pio(NC_INDEPENDENT)!=0) ERR;
   if(test_pio(NC_COLLECTIVE)!=0) ERR;
   if (mpi_rank == 1)
      SUMMARIZE_ERR;

   if (mpi_rank == 1)
      printf("*** Testing parallel IO for meta-data with MPIPOSIX-IO (driver)...");
   if(test_pio_attr(NC_INDEPENDENT)!=0) ERR;
   if(test_pio_attr(NC_COLLECTIVE)!=0) ERR;
   if (mpi_rank == 1)
      SUMMARIZE_ERR;

   if (mpi_rank == 1)
      printf("*** Testing parallel IO for different hyperslab selections "
	     "with MPIPOSIX-IO (driver)...");
   if(test_pio_hyper(NC_INDEPENDENT)!=0)ERR;
   if(test_pio_hyper(NC_COLLECTIVE)!=0) ERR;
   if (mpi_rank == 1)
      SUMMARIZE_ERR;

/*     if(!getenv_all(MPI_COMM_WORLD,0,"NETCDF4_NOCLEANUP")) */
   remove(file_name); 
   MPI_Finalize();

   if (mpi_rank == 1)
      FINAL_RESULTS;
   return 0;
}

/* Both read and write will be tested */
int test_pio(int flag) 
{
   /* MPI stuff. */
   int mpi_size, mpi_rank;
   int res = NC_NOERR;
   MPI_Comm comm = MPI_COMM_WORLD;
   MPI_Info info = MPI_INFO_NULL;

   /* Netcdf-4 stuff. */
   int ncid;
   int nvid,uvid;
   int rvid;
   unsigned m,k,j,i;

   /* two dimensional integer data test */
   int dimids[NDIMS1];
   size_t start[NDIMS1];
   size_t count[NDIMS1];

   int *data;
   int *tempdata;
   int *rdata;
   int *temprdata;

   /* four dimensional integer data test,
      time dimension is unlimited.*/
   int  dimuids[NDIMS2];
   size_t ustart[NDIMS2];
   size_t ucount[NDIMS2];

   int *udata;
   int *tempudata;
   int *rudata;
   int *temprudata;

   /* Initialize MPI. */
   MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);
   MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);

   /* Create a parallel netcdf-4 file. */
   if ((res = nc_create_par(file_name, facc_type, comm, info, &ncid)))
      BAIL(res);

   /* The first case is two dimensional variables, no unlimited dimension */

   /* Create two dimensions. */
   if ((res = nc_def_dim(ncid, "d1", DIMSIZE2, dimids)))
      BAIL(res);
   if ((res = nc_def_dim(ncid, "d2", DIMSIZE, &dimids[1])))
      BAIL(res);

   /* Create one var. */
   if ((res = nc_def_var(ncid, "v1", NC_INT, NDIMS1, dimids, &nvid)))
      BAIL(res);

   if ((res = nc_enddef(ncid)))
      BAIL(res);

   /* Set up slab for this process. */
   start[0] = 0;
   start[1] = mpi_rank * DIMSIZE/mpi_size;
   count[0] = DIMSIZE2;
   count[1] = DIMSIZE/mpi_size;

   /* start parallel netcdf4 */
   if ((res = nc_var_par_access(ncid, nvid, flag)))
      BAIL(res);

   data = malloc(sizeof(int)*count[1]*count[0]);
   tempdata = data;
   for (j=0; j<count[0];j++){
      for (i=0; i<count[1]; i++)
      {
	 *tempdata = mpi_rank*(j+1);
	 tempdata++;
      }
   }

   /* Write two dimensional integer data */
   if ((res = nc_put_vara_int(ncid, nvid, start, count, 
			      data))){
      free(data);
      BAIL(res);
   }

   free(data);

   /* Case 2: create four dimensional integer data,
      one dimension is unlimited. */

   /* Create four dimensions. */
   if ((res = nc_def_dim(ncid, "ud1", NC_UNLIMITED, dimuids)))
      BAIL(res);
   if ((res = nc_def_dim(ncid, "ud2", DIMSIZE3, &dimuids[1])))
      BAIL(res);
   if ((res = nc_def_dim(ncid, "ud3", DIMSIZE2, &dimuids[2])))
      BAIL(res);
   if ((res = nc_def_dim(ncid, "ud4", DIMSIZE, &dimuids[3])))
      BAIL(res);

   /* Create one var. */
   if ((res = nc_def_var(ncid, "uv1", NC_INT, NDIMS2, dimuids, &uvid)))
      BAIL(res);

   if ((res = nc_enddef(ncid)))
      BAIL(res);
 
   /* Set up selection parameters */
   ustart[0] = 0;
   ustart[1] = 0;
   ustart[2] = 0;
   ustart[3] = DIMSIZE*mpi_rank/mpi_size;
   ucount[0] = TIMELEN;
   ucount[1] = DIMSIZE3;
   ucount[2] = DIMSIZE2;
   ucount[3] = DIMSIZE/mpi_size;

   /* Access parallel */
   if ((res = nc_var_par_access(ncid, uvid, flag))) 
      BAIL(res);
    
   /* Create phony data. */
   udata = malloc(ucount[0]*ucount[1]*ucount[2]*ucount[3]*sizeof(int));
   tempudata = udata;
   for( m=0; m<ucount[0];m++)
      for( k=0; k<ucount[1];k++)
	 for (j=0; j<ucount[2];j++)
	    for (i=0; i<ucount[3]; i++)
	    {
	       *tempudata = (1+mpi_rank)*2*(j+1)*(k+1)*(m+1);
	       tempudata++;
	    }

   /* Write slabs of phoney data. */
   if ((res = nc_put_vara_int(ncid, uvid, ustart, ucount, 
			      udata))){
      free(udata);
      BAIL(res);
   }
   free(udata);

   /* Close the netcdf file. */
   if ((res = nc_close(ncid)))
      BAIL(res);

   if ((res = nc_open_par(file_name, facc_type_open, comm, info, &ncid)))
      BAIL(res);

   /* Case 1: read two-dimensional variables, no unlimited dimension */
   /* Set up slab for this process. */
   start[0] = 0;
   start[1] = mpi_rank * DIMSIZE/mpi_size;
   count[0] = DIMSIZE2;
   count[1] = DIMSIZE/mpi_size;

   if ((res = nc_inq_varid(ncid, "v1", &rvid)))
      BAIL(res);

   if ((res = nc_var_par_access(ncid, rvid, flag)))
      BAIL(res);
    
   rdata = malloc(sizeof(int)*count[1]*count[0]);
   if ((res = nc_get_vara_int(ncid, rvid, start, count, rdata)))
   {
      free(rdata);
      BAIL(res);
   }

   temprdata = rdata;
   for (j=0; j<count[0];j++){
      for (i=0; i<count[1]; i++){
	 if(*temprdata != mpi_rank*(j+1)) 
	 {
	    res = -1;
	    break;
	 }
	 temprdata++;
      }
   }

   free(rdata);

   /* If reading wrong value, 
      return with an error message */
   if(res == -1) BAIL(res);

   /* Case 2: read four dimensional data, one dimension is unlimited. */

   /* set up selection parameters */
   ustart[0] = 0;
   ustart[1] = 0;
   ustart[2] = 0;
   ustart[3] = DIMSIZE*mpi_rank/mpi_size;
   ucount[0] = TIMELEN;
   ucount[1] = DIMSIZE3;
   ucount[2] = DIMSIZE2;
   ucount[3] = DIMSIZE/mpi_size;
 
   /* Inquiry the data */
   if ((res = nc_inq_varid(ncid, "uv1", &rvid)))
      BAIL(res);
    
   /* Access the parallel */
   if ((res = nc_var_par_access(ncid, rvid, flag)))
      BAIL(res);

   rudata = malloc(ucount[0]*ucount[1]*ucount[2]*ucount[3]*sizeof(int));
   temprudata = rudata;

   /* Read data */
   if ((res = nc_get_vara_int(ncid, rvid, ustart, ucount, rudata)))
   {
      free(rudata);
      BAIL(res);
   }

   for( m=0; m<ucount[0];m++)
      for( k=0; k<ucount[1];k++)
	 for (j=0; j<ucount[2];j++)
	    for (i=0; i<ucount[3]; i++){
	       if(*temprudata != (1+mpi_rank)*2*(j+1)*(k+1)*(m+1)){
		  res = -1;
		  break;
	       }
	       temprudata++;
	    }

   free(rudata);
   if(res == -1) BAIL(res);


   /* Close the netcdf file. */
   if ((res = nc_close(ncid)))
      BAIL(res);

   return 0;
}


/* Attributes: both read and write will be tested for parallel NetCDF*/

int test_pio_attr(int flag) 
{
   /* MPI stuff. */
   int mpi_size, mpi_rank;
   int res = NC_NOERR;
   MPI_Comm comm = MPI_COMM_WORLD;
   MPI_Info info = MPI_INFO_NULL;

   /* Netcdf-4 stuff. */
   int ncid;
   int nvid;
   int j, i;

   double rh_range[2];
   static char title[] = "parallel attr to netCDF";
   nc_type    st_type,vr_type;
   size_t     vr_len,st_len;
   size_t     orivr_len;
   double *vr_val;
   char   *st_val;

   /* two dimensional integer data*/
   int dimids[NDIMS1];
   size_t start[NDIMS1];
   size_t count[NDIMS1];
   int *data;
   int *tempdata;

   /* Initialize MPI. */
   MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);
   MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);

   /* Create a parallel netcdf-4 file. */
/*    nc_set_log_level(NC_TURN_OFF_LOGGING); */
   /*    nc_set_log_level(3);*/

   if ((res = nc_create_par(file_name, facc_type, comm, 
			    info, &ncid)))
      BAIL(res);


   /* Create a 2-D variable so that an attribute can be added. */
   if ((res = nc_def_dim(ncid, "d1", DIMSIZE2, dimids)))
      BAIL(res);
   if ((res = nc_def_dim(ncid, "d2", DIMSIZE, &dimids[1])))
      BAIL(res);

   /* Create one var. */
   if ((res = nc_def_var(ncid, "v1", NC_INT, NDIMS1, dimids, &nvid)))
      BAIL(res);

   orivr_len = 2;
   rh_range[0] = 1.0;
   rh_range[1] = 1000.0;

   /* Write attributes of a variable */

   if ((res = nc_put_att_double (ncid, nvid, "valid_range",
                                 NC_DOUBLE, orivr_len, rh_range)))
      BAIL(res);

   if ((res = nc_put_att_text (ncid, nvid, "title",
			       strlen(title), title)))
      BAIL(res);

   /* Write global attributes */
   if ((res = nc_put_att_double (ncid, NC_GLOBAL, "g_valid_range",
                                 NC_DOUBLE, orivr_len, rh_range)))
      BAIL(res);

   if ((res = nc_put_att_text (ncid, NC_GLOBAL, "g_title",
			       strlen(title), title)))
      BAIL(res);

   if ((res = nc_enddef(ncid)))
      BAIL(res);


   /* Set up slab for this process. */
   start[0] = 0;
   start[1] = mpi_rank * DIMSIZE/mpi_size;
   count[0] = DIMSIZE2;
   count[1] = DIMSIZE/mpi_size;

   /* Access parallel */
   if ((res = nc_var_par_access(ncid, nvid, flag)))
      BAIL(res);

   /* Allocating data */
   data      = malloc(sizeof(int)*count[1]*count[0]);
   tempdata  = data;
   for (j=0; j<count[0];j++){
      for (i=0; i<count[1]; i++){
	 *tempdata = mpi_rank*(j+1);
	 tempdata ++;
      }
   }
   if ((res = nc_put_vara_int(ncid, nvid, start, count, 
			      data))){
      free(data);
      BAIL(res);
   }

   free(data);
   /* Close the netcdf file. */
   if ((res = nc_close(ncid)))
      BAIL(res);


   /* Read attributes */
   if ((res = nc_open_par(file_name, facc_type_open, comm, info, &ncid)))
      BAIL(res);

   /* Set up slab for this process. */
   start[0] = 0;
   start[1] = mpi_rank * DIMSIZE/mpi_size;
   count[0] = DIMSIZE2;
   count[1] = DIMSIZE/mpi_size;

   /* Inquiry variable */
   if ((res = nc_inq_varid(ncid, "v1", &nvid)))
      BAIL(res);

   /* Access parallel */
   if ((res = nc_var_par_access(ncid, nvid, flag)))
      BAIL(res);

   /* Inquiry attribute */
   if ((res = nc_inq_att (ncid, nvid, "valid_range", &vr_type, &vr_len)))
      BAIL(res);

   /* check variable attribute type */
   if(vr_type != NC_DOUBLE) BAIL(-1);

   /*check variable attribute length */
   if(vr_len != orivr_len) BAIL(-1);

   vr_val = (double *) malloc(vr_len * sizeof(double));
     
   /* Get variable attribute values */
   if ((res = nc_get_att_double(ncid, nvid, "valid_range", vr_val))){
      free(vr_val);
      BAIL(res);
   }

   /* Check variable attribute value */
   for( i = 0; i <vr_len; i++){
      if (vr_val[i] != rh_range[i]) BAIL(-1);
   }
   free(vr_val);
 

   /* Inquiry global attribute */
   if ((res = nc_inq_att (ncid, NC_GLOBAL, "g_valid_range", &vr_type, &vr_len)))
      BAIL(res);

   /* Check global attribute type */
   if(vr_type != NC_DOUBLE) BAIL(-1);
    
   /* Check global attribute length */
   if(vr_len != orivr_len) BAIL(-1);
    
   /* Obtain global attribute value */
   vr_val = (double *) malloc(vr_len * sizeof(double));
   if ((res = nc_get_att_double(ncid, NC_GLOBAL, "g_valid_range", vr_val))){
      free(vr_val);
      BAIL(res);
   }

   /* Check global attribute value */
   for( i = 0; i <vr_len; i++){
      if (vr_val[i] != rh_range[i]) BAIL(-1);
   }

   free(vr_val);

   /* Inquiry string attribute of a variable */
   if ((res = nc_inq_att (ncid, nvid, "title", &st_type, &st_len)))
      BAIL(res);

 
   /* check string attribute length */
   if(st_len != strlen(title)) BAIL(-1);

   /* Check string attribute type */
   if(st_type != NC_CHAR) BAIL(-1);

   /* Allocate meory for string attribute */
   st_val = (char *) malloc(st_len * (sizeof(char)));
 
   /* Obtain variable string attribute value */
   if ((res = nc_get_att_text(ncid, nvid,"title", st_val))){
      free(st_val);
      BAIL(res);
   }

   /*check string value */
   if(strncmp(st_val,title,st_len)) {
      free(st_val);
      BAIL(-1);
   }
   free(st_val);

   /*Inquiry global attribute */
   if ((res = nc_inq_att (ncid, NC_GLOBAL, "g_title", &st_type, &st_len)))
      BAIL(res);

   /* check attribute length*/
   if(st_len != strlen(title)) BAIL(-1);

   /*check attribute type*/
   if(st_type != NC_CHAR) BAIL(-1);

   /* obtain global string attribute value */
   st_val = (char*)malloc(st_len*sizeof(char));
   if ((res = nc_get_att_text(ncid, NC_GLOBAL,"g_title", st_val))){
      free(st_val);
      BAIL(res);
   }

   /* check attribute value */
   if(strncmp(st_val,title,st_len)){
      free(st_val);
      BAIL(-1);
   }
   free(st_val);

   /* Close the netcdf file. */
   if ((res = nc_close(ncid)))
      BAIL(res);

   return 0;
    
}


/* test different hyperslab settings */

int test_pio_hyper(int flag){
  
   /* MPI stuff. */
   int mpi_size, mpi_rank;
   int res = NC_NOERR;
   MPI_Comm comm = MPI_COMM_WORLD;
   MPI_Info info = MPI_INFO_NULL;

   /* Netcdf-4 stuff. */
   int ncid;
   int nvid;
   int rvid;
   int j, i;

   /* two dimensional integer data test */
   int dimids[NDIMS1];
   size_t start[NDIMS1], count[NDIMS1];
   int *data;
   int *tempdata;
   int *rdata;
   int *temprdata;
   int count_atom;


   /* Initialize MPI. */
   MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);
   MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);

   if(mpi_size == 1) return 0;

   /* Create a parallel netcdf-4 file. */
/*      nc_set_log_level(NC_TURN_OFF_LOGGING); */
/*      nc_set_log_level(4);*/

   if ((res = nc_create_par(file_name, facc_type, comm, 
			    info, &ncid)))
      BAIL(res);


   /* The case is two dimensional variables, no unlimited dimension */

   /* Create two dimensions. */
   if ((res = nc_def_dim(ncid, "d1", DIMSIZE2, dimids)))
      BAIL(res);
   if ((res = nc_def_dim(ncid, "d2", DIMSIZE, &dimids[1])))
      BAIL(res);

   /* Create one var. */
   if ((res = nc_def_var(ncid, "v1", NC_INT, NDIMS1, dimids, &nvid)))
      BAIL(res);

   if ((res = nc_enddef(ncid)))
      BAIL(res);


   /* hyperslab illustration for 3-processor case 

      --------
      |aaaacccc|
      |aaaacccc|
      |bbbb    |
      |bbbb    |
      --------
   */

   /* odd number of processors should be treated differently */
   if(mpi_size%2 != 0) {
      
      count_atom = DIMSIZE*2/(mpi_size+1);
      if(mpi_rank <= mpi_size/2) {
         start[0] = 0;
         start[1] = mpi_rank*count_atom;
         count[0] = DIMSIZE2/2;
         count[1] = count_atom;
      }
      else {
         start[0] = DIMSIZE2/2;
         start[1] = (mpi_rank-mpi_size/2-1)*count_atom;
         count[0] = DIMSIZE2/2;
         count[1] = count_atom;
      }  
   }
   else  {
    
      count_atom = DIMSIZE*2/mpi_size;
      if(mpi_rank < mpi_size/2) {
         start[0] = 0;
         start[1] = mpi_rank*count_atom;
         count[0] = DIMSIZE2/2;
         count[1] = count_atom;
      }
      else {
         start[0] = DIMSIZE2/2;
         start[1] = (mpi_rank-mpi_size/2)*count_atom;
         count[0] = DIMSIZE2/2;
         count[1] = count_atom;
      }  
   }

   if ((res = nc_var_par_access(ncid, nvid, flag)))
      BAIL(res);
   data      = malloc(sizeof(int)*count[1]*count[0]);
   tempdata  = data;
   for (j=0; j<count[0];j++){
      for (i=0; i<count[1]; i++){
	 *tempdata = mpi_rank*(j+1);
	 tempdata ++;
      }
   }


   if ((res = nc_put_vara_int(ncid, nvid, start, count, 
			      data))){
      free(data);
      BAIL(res);

   }
   free(data);

   /* Close the netcdf file. */
   if ((res = nc_close(ncid)))
      BAIL(res);

   if ((res = nc_open_par(file_name, facc_type_open, comm, info, &ncid)))
      BAIL(res);
  
   /* Inquiry the variable */
   if ((res = nc_inq_varid(ncid, "v1", &rvid)))
      BAIL(res);

   if ((res = nc_var_par_access(ncid, rvid, flag)))
      BAIL(res);
    
   rdata      = malloc(sizeof(int)*count[1]*count[0]);
   /* Read the data with the same slab settings */
   if ((res = nc_get_vara_int(ncid, rvid, start, count, 
			      rdata))){
      free(rdata);
      BAIL(res);
   }

   temprdata = rdata;
   for (j=0; j<count[0];j++){
      for (i=0; i<count[1]; i++){
	 if(*temprdata != mpi_rank*(j+1)) 
	 {
	    res = -1;
	    break;
	 }
	 temprdata++;
      }
   }

   free(rdata);
   if(res == -1) BAIL(res);

   /* Close the netcdf file. */
   if ((res = nc_close(ncid)))
      BAIL(res);
    
   return 0;
}

/*-------------------------------------------------------------------------
 * Function:	getenv_all
 *
 * Purpose:	Used to get the environment that the root MPI task has.
 * 		name specifies which environment variable to look for
 * 		val is the string to which the value of that environment
 * 		variable will be copied.
 *
 * 		NOTE: The pointer returned by this function is only
 * 		valid until the next call to getenv_all and the data
 * 		stored there must be copied somewhere else before any
 * 		further calls to getenv_all take place.
 *
 * Return:	pointer to a string containing the value of the environment variable
 * 		NULL if the varialbe doesn't exist in task 'root's environment.
 *
 * Programmer:	Leon Arber
 *              4/4/05
 *
 * Modifications:
 *
 *-------------------------------------------------------------------------
 */

char* getenv_all(MPI_Comm comm, int root, const char* name)
{
   int nID;
   int len = -1;
   static char* env = NULL;

   assert(name);

   MPI_Comm_rank(comm, &nID);

   /* The root task does the getenv call
    * and sends the result to the other tasks */
   if(nID == root)
   {
      env = getenv(name);
      if(env)
      {
	 len = strlen(env);
	 MPI_Bcast(&len, 1, MPI_INT, root, comm);
	 MPI_Bcast(env, len, MPI_CHAR, root, comm);
      }
      /* len -1 indicates that the variable was not in the environment */
      else
	 MPI_Bcast(&len, 1, MPI_INT, root, comm);
   }
   else
   {
      MPI_Bcast(&len, 1, MPI_INT, root, comm);
      if(len >= 0)
      {
	 if(env == NULL)
	    env = (char*) malloc(len+1);
	 else if(strlen(env) < len)
	    env = (char*) realloc(env, len+1);

	 MPI_Bcast(env, len, MPI_CHAR, root, comm);
	 env[len] = '\0';
      }
      else
      {
	 if(env)
	    free(env);
	 env = NULL;
      }
   }

   MPI_Barrier(comm);

   return env;
}


