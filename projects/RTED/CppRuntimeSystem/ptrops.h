
#ifndef _PTROPS_H

#define _PTROPS_H

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

static const size_t SIZEOF_MEMORY_ADDRESS = sizeof(char*); ///< should be MAX(sizeof(char*), sizeof(char shared*)));
static const size_t MAX_INDIRECTIONS = sizeof(size_t) * CHAR_BIT;
static const size_t MASK_SHARED = 1;

struct Indirections
{
  size_t levels;
  size_t shared_mask;
};

union MemoryAddress
{
  char          raw[SIZEOF_MEMORY_ADDRESS];  ///< must be >= the largest field in the union
                                             ///  this field is not directly accessed, but let
                                             ///  the non-UPC compiler generate code without
                                             ///  potentially truncating shared pointers
  char *        local;

#ifdef __UPC__
  char shared * global;
#endif /* __UPC__ */
};

#ifndef __cplusplus

typedef struct Indirections Indirections;
typedef union MemoryAddress MemoryAddress;

#endif /* __cplusplus */

/// \brief  removes levels of indirections from desc
/// \return a new Indirection Descriptor
// Indirections remove(Indirections desc, size_t levels);

/// \brief  removes levels of indirections from desc
/// \return a new Indirection Descriptor
MemoryAddress deref(MemoryAddress addr, Indirections desc);

/// \brief returns the size of the MemoryAddress abstractions
/// \note  in particular if this is compiled with UPC we need to make sure that
///        sizeof(MemoryAddress) is consistent across all compilers (g++, C,
///        UPC) This is done in the Rted-Runtimesystem, where we check that
///        sizeof_MemoryAddress_UPC() == sizeof(MemoryAddress) at startup-time.
size_t sizeof_MemoryAddress_UPC();

#ifdef __cplusplus
}
#endif /* __cplusplus -- extern */


#endif /* _PTROPS_H */
