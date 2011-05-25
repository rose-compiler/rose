#ifndef DISPATCH4_H
#define DISPATCH4_H 1

/**************************************************/
/* Wish the netcdf-3 dispatcher was this short*/

typedef struct NC_Dispatch4 {
int (*sync)(int);
int (*abort)(int);
int (*close)(int);
int (*get_vara)(NC_FILE_INFO_T*, int, int, const size_t*, const size_t*, nc_type, int, void*);
int (*put_vara)(NC_FILE_INFO_T*, int, int, const size_t*, const size_t*, nc_type, int, void*);
} NC_Dispatch4;

/* The two defined dispatch tables*/
extern NC_Dispatch4 netcdf4lib;
extern NC_Dispatch4 ncdap4lib;

#endif /*DISPATCH_H*/

