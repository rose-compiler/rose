#ifndef RENAME4_H
#define RENAME4_H

#ifndef USE_DAP
#define RENAME4(name) name
#define EXPORT4 static
#endif

#ifdef USE_DAP
#include "netcdf3.h"
#include "netcdf4l.h"
#define RENAME4(name) l4##name
#define EXPORT4
/* Add a declaration for each use of EXPORT4 */
extern int
nc_create_file(const char*,int,size_t,int,size_t*,MPI_Comm,MPI_Info,int*);
extern int
nc_open_file(const char*,int,int,size_t*,int,MPI_Comm,MPI_Info,int*);
#endif


#endif /*RENAME4_H*/
