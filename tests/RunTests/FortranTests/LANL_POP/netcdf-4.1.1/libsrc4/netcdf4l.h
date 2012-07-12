#ifndef NETCDF4L_H
#define NETCDF4L_H 1

#if defined(__cplusplus)
extern "C" {
#endif

#ifndef EXTERNAL
# define EXTERNL extern MSC_EXTRA
#else
# define EXTERNL extern
#endif

EXTERNL int l4nc_sync(int ncid);
EXTERNL int l4nc_abort(int ncid);
EXTERNL int l4nc_close(int ncid);

EXTERNL int l4nc_create_file(const char *path,
               int cmode, size_t initialsz, 
	       int basepe, size_t *chunksizehintp, MPI_Comm comm, 
	       MPI_Info info, int *ncidp);

EXTERNL int l4nc_open_file(const char *path, int mode, int basepe, size_t *chunksizehintp, 
	     int use_parallel, MPI_Comm comm, MPI_Info info, int *ncidp);

EXTERNL int l4nc_get_vara(NC_FILE_INFO_T*, int ncid, int varid,
                         const size_t *startp, const size_t *countp,
                         nc_type mem_nc_type, int is_long, void *data);
EXTERNL int l4nc_put_vara(NC_FILE_INFO_T*, int ncid, int varid,
                         const size_t *startp, const size_t *countp,
			 nc_type mem_nc_type, int is_long, void *data);

EXTERNL int l4nc_put_att_tc(int ncid, int varid, const char *name,
                            nc_type file_type, nc_type mem_type,
                            int mem_type_is_long, size_t len, const void *op);

EXTERNL int l4nc_get_att_tc(int ncid, int varid, const char *name,
                            nc_type mem_type, int mem_type_is_long, void *ip);

#if defined(__cplusplus)
}
#endif

#endif /*NETCDF4L_H*/

