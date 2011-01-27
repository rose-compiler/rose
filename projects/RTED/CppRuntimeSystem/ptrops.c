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

#include <assert.h>

#include "ptrops.h"

size_t sizeof_Address_UPC(void)
{
  return sizeof(rted_Address);
}

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

#ifdef __UPC__

union PtrCast
{
  shared const char *     shptr;
  upc_shared_ptr_t  shmem;
};


rted_Address rted_deref(rted_Address addr, rted_AddressDesc desc)
{
  if ((desc.shared_mask & MASK_SHARED) == 0)
  {
    addr.local = *((const char**)addr.local);
  }
  else
  {
    addr.global = *((shared const char* shared*)addr.global);
  }

  return addr;
}

const char*
rted_system_addr(rted_Address addr, rted_AddressDesc desc)
{
  union PtrCast ptrcast;

  if ((desc.shared_mask & MASK_SHARED) == 0) { return addr.local; }

  ptrcast.shptr = addr.global;
  return (const char*) __upc_vm_map_addr(ptrcast.shmem);
}

#else /* __UPC__ */

rted_Address rted_deref(rted_Address addr, rted_AddressDesc unused)
{
  // for shared memory this file has to be compiled with UPC
  assert(unused.shared_mask == 0);

  addr = *((rted_Address*)addr.local);

  return addr;
}

const char*
rted_system_addr(rted_Address addr, rted_AddressDesc unused)
{
  assert(unused.shared_mask == 0);

  return addr.local;
}

#endif /* __UPC__ */
