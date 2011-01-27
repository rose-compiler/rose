

#ifndef _RTED_IFACE_STRUCTS_H
#define _RTED_IFACE_STRUCTS_H

#include "ptrops.h"

struct rted_TypeDesc
{
	const char*      name;
	const char*      base;
	rted_AddressDesc desc;
};

struct rted_SourceInfo
{
	const char* file;
	size_t      src_line;
	size_t      rted_line;
};

enum rted_AllocKind
{
	akUndefined      = 0,

	/* allocation categories */
	akStack          = 16,
	akCHeap          = 32,
	akCxxHeap        = 64,   /* C++ new/delete */
	akUpcSharedHeap  = 128,  /* UPC, but also used for upc_free */

  /* C++ */
	akCxxNew         = 1 | akCxxHeap, /* for new and delete */
	akCxxArrayNew    = 2 | akCxxHeap, /* for new T[] and delete[] */

  /* UPC */
	akUpcAlloc       = 1 | akUpcSharedHeap, /* and upc_local_alloc */
	akUpcGlobalAlloc = 2 | akUpcSharedHeap,
	akUpcAllAlloc    = 3 | akUpcSharedHeap
};


#ifndef __cplusplus

typedef struct rted_TypeDesc   rted_TypeDesc;
typedef struct rted_SourceInfo rted_SourceInfo;
typedef enum rted_AllocKind    rted_AllocKind;

#endif /* __cplusplus */
#endif /* _RTED_IFACE_STRUCTS_H */
