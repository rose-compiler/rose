///
/// \file  rted_iface_structs.h
/// \brief defines structs that are used in RTED's runtime system interface
/// \note  These structs are kept separately from RuntimeSystem.h b/c they
///        are used throughout the CppRuntimeSystem implementation.
///        (no need to pull in the entire RuntimeSystem.h when only
///        a few types are needed).
/// \email peter.pirkelbauer@llnl.gov

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
  akUndefined       = 0,

  /* allocation categories */
  akGlobal          = 8,    // Global memory (allocated on the file scope)
  akStack           = 16,   // Stack memory (allocated during function execution)
  akCHeap           = 32,   // C style heap allocation: malloc and friends
  akCxxHeap         = 64,   // C++ new/delete
  akUpcShared       = 128,  // set for UPC allocations (and upc_free)

  /* C++ */
  akCxxNew          = 1 | akCxxHeap,   // for new and delete
  akCxxArrayNew     = 2 | akCxxHeap,   // for new T[] and delete[]

  /* UPC */
  akUpcAlloc        = 1 | akUpcShared, // and upc_local_alloc
  akUpcGlobalAlloc  = 2 | akUpcShared,
  akUpcAllAlloc     = 3 | akUpcShared,

  akUpcSharedGlobal = akGlobal | akUpcShared
};


#ifndef __cplusplus

typedef struct rted_TypeDesc   rted_TypeDesc;
typedef struct rted_SourceInfo rted_SourceInfo;
typedef enum rted_AllocKind    rted_AllocKind;

#endif /* __cplusplus */
#endif /* _RTED_IFACE_STRUCTS_H */
