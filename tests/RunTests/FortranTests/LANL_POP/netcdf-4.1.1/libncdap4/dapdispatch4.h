#ifndef DAPDISPATCH4_H
#define DAPDISPATCH4_H

extern int nc4d_sync(int ncid);
extern int nc4d_abort(int ncid);
extern int nc4d_close(int ncid);

extern int nc4d_get_vara(NC_FILE_INFO_T* nc, int ncid, int varid, 
			const size_t* startp, const size_t* countp, 
			nc_type mem_nc_type, int is_long, void* data);

extern int nc4d_put_vara(NC_FILE_INFO_T* nc, int ncid, int varid, 
			const size_t* startp, const size_t* countp, 
			nc_type mem_nc_type, int is_long, void* data);

extern int nc4d_open_file(const char *path, int mode,
                          int basepe, size_t *chunksizehintp, 
                          int use_parallel, MPI_Comm comm, MPI_Info info,
                          int *ncidp);


#endif /*DAPDISPATCH4_H*/
