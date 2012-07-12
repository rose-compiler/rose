#ifndef NCDAP4_H
#define NCDAP4_H

#undef READCHECK
#undef ALIGNCHECK

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>

#include "config.h"
#include "netcdf.h"
#include "nc.h"
#include "nc4internal.h"
#include "ncdap.h"
#include "getvara.h"
#include "constraints3.h"
#include "constraints4.h"

extern int lnc4_redef(int ncid);
extern int lnc4_enddef(int ncid);
extern int lnc4_sync(int ncid);
extern int lnc4_abort(int ncid);
extern int lnc4_close(int ncid);

extern int lnc4_open_file(const char *path, int mode, int basepe,
                          size_t *chunksizehintp, int use_parallel,
                          MPI_Comm comm, MPI_Info info, int *ncidp);

extern int l4nc4_get_vara(NC_FILE_INFO_T *nc, int ncid, int varid, const size_t *startp, const size_t *countp, nc_type mem_nc_type, int is_long, void *data);

extern int l4nc4_put_vara(NC_FILE_INFO_T *nc, int ncid, int varid, const size_t *startp, const size_t *countp, nc_type mem_nc_type, int is_long, void *data);

extern int nc4_nc4f_list_add(NC_FILE_INFO_T *nc, const char *path, int mode);
extern void nc4_file_list_del(NC_FILE_INFO_T *nc);
extern int nc4_nc4f_list_add(NC_FILE_INFO_T *nc, const char *path, int mode);
extern int close_netcdf4_file(NC_HDF5_FILE_INFO_T *h5, int abort);

extern short drno_delta_file_id(short);
extern int drno_delta_numfiles(int);

/**********************************************************/
extern int ncceparse(char*, int, NClist**, NClist**, char**);

extern NCerror computecdfnodesets4(NCDRNO* drno);
extern NCerror fixgrids4(NCDRNO* drno);
extern NCerror computecdfdimnames4(NCDRNO* drno);
extern NCerror computetypenames4(NCDRNO* drno, CDFnode* tnode);
extern NCerror computeusertypes4(NCDRNO* drno);
extern int singletonsequence(CDFnode* node);
extern CDFnode* getsingletonfield(NClist* list);
extern void setvarbasetype(NCDRNO* drno, CDFnode* field);
extern NCerror shortentypenames4(NCDRNO* drno);

extern ptrdiff_t dapsinglestride4[NC_MAX_DIMS];

#endif /*NCDAP4_H*/
