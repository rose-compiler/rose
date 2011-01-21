

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
	akStack          = 1,
	akCHeap          = 2,

	/* language groups */
	akCxxHeap        = 32,  /* C++ new/delete */
	akUpcSharedHeap  = 64,  /* UPC, but also used for upc_free */

  /* C++ */
	akCxxNew         = 3 | akCxxHeap,
	akCxxArrayNew    = 4 | akCxxHeap,

  /* UPC */
	akUpcAlloc       = 5 | akUpcSharedHeap,
	akUpcGlobalAlloc = 6 | akUpcSharedHeap,
	akUpcAllAlloc    = 7 | akUpcSharedHeap
};


#ifndef __cplusplus

typedef struct rted_TypeDesc   rted_TypeDesc;
typedef struct rted_SourceInfo rted_SourceInfo;
typedef enum rted_AllocKind    rted_AllocKind;

#endif /* __cplusplus */
#endif /* _RTED_IFACE_STRUCTS_H */
