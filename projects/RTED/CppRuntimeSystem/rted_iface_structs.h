

#ifndef _RTED_IFACE_STRUCTS_H
#define _RTED_IFACE_STRUCTS_H

#include "ptrops.h"

struct TypeDesc
{
	const char* name;
	const char* base;
	AddressDesc desc;
};

struct SourceInfo
{
	const char* file;
	size_t      src_line;
	size_t      rted_line;
};

#endif /* _RTED_IFACE_STRUCTS_H */
