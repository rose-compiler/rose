

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


#ifndef __cplusplus

typedef struct rted_TypeDesc   rted_TypeDesc;
typedef struct rted_SourceInfo rted_SourceInfo;

#endif /* __cplusplus */
#endif /* _RTED_IFACE_STRUCTS_H */
