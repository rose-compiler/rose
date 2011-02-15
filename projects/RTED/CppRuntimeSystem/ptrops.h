
#ifndef _PTROPS_H

#define _PTROPS_H

#include <stdlib.h>

// \todo remove the define from here and make it a define
//       in the ROSE configuration
#define WITH_UPC 1

#if __UPC__

#include <upc.h>

#endif /* __UPC__ */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */



#if __UPC__

static inline
size_t rted_ThisThread(void)
{
  return MYTHREAD;
}

#else /* __UPC__ */

static inline
size_t rted_ThisThread(void)
{
  return 0;
}

#endif /* __UPC__ */

static const size_t MASK_SHARED = 1;

struct rted_AddressDesc
{
  size_t levels;         ///< levels of indirections before a non pointer/reference type is reached
  size_t shared_mask;    ///< stores whether an indirection is shared for each level of indirections
                         ///  shared_mask & (1 << l) == 1 when an indirection is shared
                         ///  shared_mask & (1 << l) == 0 otherwise
};

struct rted_Address
{
#if WITH_UPC
  size_t              thread_id;                   ///< owning thread
#endif

  const char *        local;                       ///< ordinary local pointer
};

#ifndef __cplusplus

typedef struct rted_Address     rted_Address;
typedef struct rted_AddressDesc rted_AddressDesc;

#endif /* __cplusplus */

/// \brief  removes levels of indirections from desc
/// \return a new Indirection Descriptor
rted_AddressDesc rted_deref_desc(rted_AddressDesc desc);

/// \brief  removes levels of indirections from desc
/// \return a new Indirection Descriptor
rted_Address rted_deref(rted_Address addr, rted_AddressDesc desc);

rted_AddressDesc rted_ptr(void);

rted_AddressDesc rted_upc_address_of(rted_AddressDesc desc, size_t shared_mask);

int rted_isPtr(rted_AddressDesc addr);

void rted_setIntVal(rted_Address addr, int v);

static inline
rted_Address rted_Addr(const void* ptr)
{
  return (rted_Address) { rted_ThisThread(), (const char*)ptr };
}

static inline
rted_AddressDesc rted_obj(void)
{
  rted_AddressDesc pd;

  pd.levels = 0;
  pd.shared_mask = 0;

  return pd;
}

static inline
rted_AddressDesc rted_address_of(rted_AddressDesc desc)
{
  ++desc.levels;
  desc.shared_mask <<= 1;

  return desc;
}

void rted_exit(int exitcode);

#if WITH_UPC

static inline
int rted_isLocal(rted_Address addr)
{
  return addr.thread_id == rted_ThisThread();
}

#else /* WITH_UPC */

static inline
int rted_isLocal(rted_Address addr)
{
  return 1;
}

#endif /* WITH_UPC */

#ifdef __UPC__

/// \brief get's the lower boundary for this thread's shared memory
const char* rted_ThisShmemBase(void);

/// \brief get's the upper boundary for this thread's shared memory
const char* rted_ThisShmemLimit(void);

/// \brief takes a shared Address and convert it into the rted internal representation
rted_Address rted_AddrSh(shared const char* ptr);

#endif /* __UPC__ */



#ifdef __cplusplus
}
#endif /* __cplusplus -- extern */


#endif /* _PTROPS_H */
