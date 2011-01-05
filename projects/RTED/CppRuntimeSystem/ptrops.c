
#include <assert.h>

#include "ptrops.h"

size_t sizeof_Address_UPC(void)
{
  return sizeof(Address);
}

AddressDesc pd_deref(AddressDesc desc)
{
  assert(desc.levels >= 1);

  desc.shared_mask >>= 1;
  --desc.levels;

  return desc;
}

AddressDesc pd_upc_addrof(AddressDesc desc, size_t shared_mask)
{
  desc.shared_mask <<= 1;
  desc.shared_mask &= shared_mask;

  ++desc.levels;

  return desc;
}

AddressDesc pd_addrof(AddressDesc desc)
{
  return pd_upc_addrof(desc, 1);
}


AddressDesc pd_obj(void)
{
  AddressDesc pd;

  pd.levels = 0;
  pd.shared_mask = 0;

  return pd;
}

AddressDesc pd_ptr(void)
{
  AddressDesc pd;

  pd.levels = 1;
  pd.shared_mask = 0;

  return pd;
}

int pd_isPtr(AddressDesc desc)
{
  return (desc.levels != 0);
}

#ifdef __UPC__

Address ptr_deref(Address addr, AddressDesc desc)
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

Address ptr_deref(Address addr, AddressDesc unused)
{
  addr = *((Address*)addr.local);

  return addr;
}

#endif /* __UPC__ */
