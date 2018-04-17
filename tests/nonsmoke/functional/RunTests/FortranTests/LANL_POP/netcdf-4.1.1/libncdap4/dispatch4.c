#include "ncdap4.h"

#include "dispatch4.h"
#include "dapdispatch4.h"

#include "netcdf4l.h"

NC_Dispatch4 netcdf4lib = {
l4nc_sync,
l4nc_abort,
l4nc_close,
l4nc4_get_vara,
l4nc4_put_vara
};

NC_Dispatch4 ncdap4lib = {
nc4d_sync,
nc4d_close, /* (Turn abort into close) nc4d_abort */
nc4d_close,
nc4d_get_vara,
nc4d_put_vara
};

int
nc_sync(int ncid)
{
    NC_FILE_INFO_T* nc;
    if(!(nc = nc4_find_nc_file(ncid))) return NC_EBADID;
    return nc->dispatch->sync(ncid);
}

int
nc_abort(int ncid)
{
    NC_FILE_INFO_T* nc;
    if(!(nc = nc4_find_nc_file(ncid))) return NC_EBADID;
    return nc->dispatch->abort(ncid);
}

int
nc_close(int ncid)
{
    NC_FILE_INFO_T* nc;
    if(!(nc = nc4_find_nc_file(ncid))) return NC_EBADID;
    return nc->dispatch->close(ncid);
}

int 
nc4_get_vara(NC_FILE_INFO_T* nc, int ncid, int varid, 
			const size_t* startp, const size_t* countp, 
			nc_type mem_nc_type, int is_long, void* data)
{
    return nc->dispatch->get_vara(nc, ncid, varid, startp, countp, mem_nc_type, is_long, data);
}

int 
nc4_put_vara(NC_FILE_INFO_T* nc, int ncid, int varid, 
			const size_t* startp, const size_t* countp, 
			nc_type mem_nc_type, int is_long, void* data)
{
    return nc->dispatch->put_vara(nc, ncid, varid, startp, countp, mem_nc_type, is_long, data);
}

int
nc_open_file(const char *path, int mode, int basepe, size_t *chunksizehintp, 
	     int use_parallel, MPI_Comm comm, MPI_Info info, int *ncidp)
{
    int ncstat;
    ncstat = nc4d_open_file(path, mode, basepe, chunksizehintp, use_parallel, comm, info,ncidp);
    return ncstat;
}

