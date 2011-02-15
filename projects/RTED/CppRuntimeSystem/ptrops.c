//
// Abstracts pointer accesses
//
//   When RTED is used on UPC systems this file HAS TO be compiled
//   with UPC (otherwise shared pointers cannot be derefed).
//   Note, the only UPC compiler supported is GCCUPC with the following
//     command-line define: upc -DIN_TARGET_LIBS
//
//   In non-UPC context, this file can be compiled with a C/C++ compiler
//
// \email peter.pirkelbauer@llnl.gov

#include <upc.h>
#include <assert.h>
#include <stdio.h>

#include "ptrops.h"


#ifdef __UPC__

union rted_SharedPtrCast
{
  shared const char* shptr;
  upc_shared_ptr_t   shmem;
};

static shared[1] char rted_base_hack[THREADS];

#endif /* __UPC__ */

rted_AddressDesc rted_deref_desc(rted_AddressDesc desc)
{
  assert(desc.levels >= 1);

  desc.shared_mask >>= 1;
  --desc.levels;

  return desc;
}

rted_AddressDesc rted_upc_address_of(rted_AddressDesc desc, size_t shared_mask)
{
  desc.shared_mask <<= 1;
  desc.shared_mask &= shared_mask;

  ++desc.levels;

  return desc;
}

rted_AddressDesc rted_ptr(void)
{
  rted_AddressDesc pd;

  pd.levels = 1;
  pd.shared_mask = 0;

  return pd;
}

int rted_isPtr(rted_AddressDesc desc)
{
  return (desc.levels != 0);
}

#if __UPC__

const char* rted_ThisShmemBase(void)
{
  // \todo this can be called once at startup
  union rted_SharedPtrCast ptrcast;

  // get a pointer to the entry for this thread
  ptrcast.shptr = rted_base_hack + rted_ThisThread();

  // by subtracting the offset of this thread's entry we get a pointer
  //   to this thread's shared memory base
  return ((const char*) ptrcast.shptr) - GUPCR_PTS_OFFSET(ptrcast.shmem);
}

const char* rted_ThisShmemLimit(void)
{
  // \pp \todo get the real limit
  return (const char*) ~0;
}

static inline
shared const char* rted_asSharedPtr(rted_Address addr)
{
  union rted_SharedPtrCast ptrcast;
  const long               ofs = addr.local - rted_ThisShmemBase();

  assert(ofs > 0);

  GUPCR_PTS_SET_NULL_SHARED(ptrcast.shmem);

  GUPCR_PTS_SET_THREAD(ptrcast.shmem, addr.thread_id);
  GUPCR_PTS_SET_VADDR(ptrcast.shmem, __upc_shared_start + ofs);

  return (shared const char*)ptrcast.shptr;
}

static inline
void rted_setIntValSh(rted_Address addr, int val)
{
  *((shared int*)(rted_asSharedPtr(addr))) = val;
}

rted_Address rted_AddrSh(shared const char* ptr)
{
  // We convert the shared pointer into a local pointer
  // Standard UPC allows this conversion only if the shared pointer
  //   points to a region that is owned by this thread. To convert
  //   all pointers we utilize the GCCUPC internal GUPCR_PTS* macros.
  //   The resulting pointer can ONLY be dereferenced safely IFF this
  //   thread REALLY OWNS that memory region. See also the implementation
  //   of rted_deref.

  //   \todo check if using a union is a permissable implementation method.
  union rted_SharedPtrCast ptrcast;

  ptrcast.shptr = ptr;
  GUPCR_PTS_SET_THREAD(ptrcast.shmem, rted_ThisThread());

  return (rted_Address) { upc_threadof((shared void*) ptr),
                          (char*) ptrcast.shptr
                        };
}

#else /* __UPC__ */

static
void rted_setIntValSh(rted_Address addr, int val)
{
  assert(0);
}

#endif /* __UPC__ */

void rted_setIntVal(rted_Address addr, int val)
{
  if (rted_isLocal(addr))
  {
    *((int*)addr.local) = val;
  }
  else
  {
    rted_setIntValSh(addr, val);
  }
}

#ifdef __UPC__

static inline
shared const char* shared* rted_asSharedDoublePtr(rted_Address addr)
{
  return (shared const char* shared*) (rted_asSharedPtr(addr));
}

rted_Address rted_deref(rted_Address addr, rted_AddressDesc desc)
{
  // derefencing an adddress
  //   we distinguish three (four) cases depending on the two
  //   lower bits in the shared mask.
  //   This cases are:
  //   - 00, private to private: just deref
  //   - 10, private to shared: the derefed value contains a shared address
  //   - 11, shared to shared:
  //   - 01, shared to private: USE IS GENERALLY UNSAFE by UPC definition
  enum { privPriv = 0, privShar = 2, sharShar = 3 };

  // either a shared address, or the thread_id belongs to this thread
  printf(" %i shared = %lu\n", MYTHREAD, desc.shared_mask);
  assert((desc.shared_mask & 1) || rted_isLocal(addr));

  switch (desc.shared_mask & sharShar)
  {
    case privPriv:
      {
        addr.local = *((const char**)addr.local);
        addr.thread_id = rted_ThisThread();
        break;
      }

    case privShar:
      {
        addr = rted_AddrSh( *((shared const char**)addr.local) );
        break;
      }

    case sharShar:
      {
        // create a shared pointer from the address abstraction
        //  so that we can dereference it regardless of the address'
        //  actual location
        addr = rted_AddrSh(*rted_asSharedDoublePtr(addr));
        break;
      }

    default:
      assert(0);
  }

  assert((desc.shared_mask & 2) || rted_isLocal(addr));
  return addr;
}

void rted_exit(int exitcode)
{
  upc_global_exit(exitcode);
}

#else /* __UPC__ */

rted_Address rted_deref(rted_Address addr, rted_AddressDesc unused)
{
  // for shared memory this file has to be compiled with UPC
  assert(unused.shared_mask == 0);

  addr = *((rted_Address*)addr.local);

  return addr;
}

void rted_exit(int exitcode)
{
  exit(exitcode);
}


#endif /* __UPC__ */
