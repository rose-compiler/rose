
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

rted_AddressDesc rted_address_of(rted_AddressDesc desc)
{
  return rted_upc_address_of(desc, 0);
}


rted_AddressDesc rted_obj(void)
{
  rted_AddressDesc pd;

  pd.levels = 0;
  pd.shared_mask = 0;

  return pd;
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

rted_Address rted_deref(rted_Address addr, rted_AddressDesc desc)
{
  if (desc & MASK_SHARED == 0)
  {
    addr = *((Address*)addr.local);
  }
  else
  {
    addr = *((Address shared*)addr.global);
  }

  return addr;
}

#else /* __UPC__ */

rted_Address rted_deref(rted_Address addr, rted_AddressDesc unused)
{
  assert(unused.shared_mask == 0);

  addr = *((rted_Address*)addr.local);

  return addr;
}

#endif /* __UPC__ */
