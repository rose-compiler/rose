///
/// Abstracts concepts from UPC and provides
///   implementations that work both for C and UPC
/// Note, any UPC dependent implementation (that requires
/// compilation with a UPC compiler cannot be put in this header file
/// b/c in mixed source code projects, compilation of non-UPC code
/// would get non-UPC aware implementation.


#ifndef _PTROPS_H

#define _PTROPS_H

#include <stdlib.h>

#if __UPC__

#include <upc.h>

#endif /* __UPC__ */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


typedef unsigned int rted_thread_id;

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
#ifdef WITH_UPC
  rted_thread_id      thread_id; ///< owning thread
#endif

  const char *        local;     ///< ordinary local pointer
};

#ifndef __cplusplus

typedef struct rted_Address     rted_Address;
typedef struct rted_AddressDesc rted_AddressDesc;

#endif /* __cplusplus */

/// \brief  removes one level of indirections from desc
/// \return a new Indirection Descriptor
rted_AddressDesc rted_deref_desc(rted_AddressDesc desc);

/// \brief  removes levels of indirections from desc
/// \return a new Indirection Descriptor
rted_Address rted_deref(rted_Address addr, rted_AddressDesc desc);

/// \brief returns the description of a regular single-level, non-shared pointer
rted_AddressDesc rted_ptr(void);

/// /brief             takes the address of an rted_AddressDesc and returns the result
/// /param desc        the current descriptor
/// /param shared_mask 0, if the resulting address is non-shared, 1 if shared
rted_AddressDesc rted_upc_address_of(rted_AddressDesc desc, int shared_mask);

/// /brief returns true, iff addr describes a pointer
int rted_isPtr(rted_AddressDesc addr);

/// /brief sets the value at addr to value v
void rted_setIntVal(rted_Address addr, int v);

/// /brief exits a program (calls exit in C/C++; upc_global_exit in UPC)
void rted_exit(int exitcode);

/// /brief returns the current thread number; 0 for single-threaded code
rted_thread_id rted_ThisThread(void);

/// /brief returns the current thread number; 0 for single-threaded code
rted_thread_id rted_Threads(void);

/// /brief returns the description of non-shared regular object
static inline
rted_AddressDesc rted_obj(void)
{
  rted_AddressDesc pd;

  pd.levels = 0;
  pd.shared_mask = 0;

  return pd;
}

/// /brief takes the address of an rted_AddressDesc and returns the new descriptor
/// /note  see also rted_upc_address_of
static inline
rted_AddressDesc rted_address_of(rted_AddressDesc desc)
{
  ++desc.levels;
  desc.shared_mask <<= 1;

  return desc;
}

#ifdef WITH_UPC

/// \brief returns true, iff addr is owned by this thread
static inline
int rted_isLocal(rted_Address addr)
{
  return addr.thread_id == rted_ThisThread();
}

/// /brief converts a regular pointer into an rted_Address
static inline
rted_Address rted_Addr(const void* ptr)
{
  return (rted_Address) { rted_ThisThread(), (const char*)ptr };
}

#else /* WITH_UPC */

/// \brief returns true in single threaded code
static inline
int rted_isLocal(rted_Address addr)
{
  ((void) (&addr)); // no compiler warning

  return (1);
}

/// /brief converts a regular pointer into an rted_Address
static inline
rted_Address rted_Addr(const void* ptr)
{
  return (rted_Address) { (const char*)ptr };
}

#endif /* WITH_UPC */

#ifdef __UPC__

/// \brief get's the lower boundary for this thread's shared memory
const char* rted_ThisShmemBase(void);

/// \brief get's the upper boundary for this thread's shared memory
/// \todo  currently this function returns the maximum heap address
const char* rted_ThisShmemLimit(void);

/// \brief takes a shared Address and convert it into the rted internal representation
rted_Address rted_AddrSh(shared const char* ptr);

#endif /* __UPC__ */

#ifdef __cplusplus
}
#endif /* __cplusplus -- extern */


#endif /* _PTROPS_H */
