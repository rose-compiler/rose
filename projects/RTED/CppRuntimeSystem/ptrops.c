
#include "ptrops.h"

#ifdef __cplusplus
#error "Compile this file ptrops.c with C or UPC"
#endif

Indirections remove(Indirections desc, size_t levels);
{
  assert(desc.levels <= levels);

  desc.shared_mask >>= levels;
  desc.levels -= levels;

  return desc;
}

MemoryAddress deref(MemoryAddress addr, Indirections desc);
{
  if (desc & MASK_SHARED == 0)
  {
    addr = *((MemoryAddress*)addr.local);
  }
  else
  {
    addr = *((MemoryAddress shared*)addr.global);
  }
}

size_t sizeof_MemoryAddress_UPC()
{
  return sizeof(MemoryAddress);
}
