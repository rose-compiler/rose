
#ifndef _PTROPS_H

#define _PTROPS_H

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define MAX_INDIRECTIONS (sizeof(size_t) * CHAR_BIT)
#define SIZEOF_MEMORY_ADDRESS (sizeof(char*))

static const size_t MASK_SHARED = 1;

struct AddressDesc
{
  size_t levels;         ///< levels of indirections before a non pointer/reference type is reached
  size_t shared_mask;    ///< stores whether an indirection is shared for each level of indirections
                         ///  shared_mask & (1 << l) == 1 when an indirection is shared
                         ///  shared_mask & (1 << l) == 0 otherwise
};

union Address
{
  char          raw[SIZEOF_MEMORY_ADDRESS];  ///< must be >= the largest field in the union
                                             ///  this field is not directly accessed, but let
                                             ///  the non-UPC compiler generate code without
                                             ///  potentially truncating shared pointers

  char *        local;                       ///< ordinary local pointer

#ifdef __UPC__
  char shared * global;                      ///< UPC shared ptr (not sure if really needed)
#endif /* __UPC__ */
};

#ifndef __cplusplus

typedef union Address      Address;
typedef struct AddressDesc AddressDesc;

#endif /* __cplusplus */

/// \brief  removes levels of indirections from desc
/// \return a new Indirection Descriptor
AddressDesc pd_deref(AddressDesc desc);

/// \brief  removes levels of indirections from desc
/// \return a new Indirection Descriptor
Address ptr_deref(Address addr, AddressDesc desc);

/// \brief returns the size of the Address abstractions
/// \note  in particular if this is compiled with UPC we need to make sure that
///        sizeof(Address) is consistent across all compilers (g++, C,
///        UPC) This is done in the Rted-Runtimesystem, where we check that
///        sizeof_Address_UPC() == sizeof(Address) at startup-time.
size_t sizeof_Address_UPC(void);

AddressDesc pd_ptr(void);
AddressDesc pd_obj(void);

AddressDesc pd_addrof(AddressDesc);
AddressDesc pd_upc_addrof(AddressDesc desc, size_t shared_mask);

int pd_isPtr(AddressDesc addr);

#ifdef __cplusplus
}
#endif /* __cplusplus -- extern */


#endif /* _PTROPS_H */
