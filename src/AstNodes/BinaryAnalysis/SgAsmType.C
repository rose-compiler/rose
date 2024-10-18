#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <SgAsmType.h>

using namespace Rose;

// All known types
// FIXME[Robb P. Matzke 2014-07-21]: deleting a type should remove the type from the registry
Sawyer::Container::Map<std::string, SgAsmType*> SgAsmType::p_typeRegistry;

/** Check internal consistency of a type. */
void
SgAsmType::check() const {}

/** Returns the number of bytes require to represent an instance of this type.  Types are allowed to have a size in bits which
 *  is not a multiple of eight; this method will round such sizes up to the next byte boundary. */
size_t
SgAsmType::get_nBytes() const {
    return (get_nBits() + 7) / 8;
}

#endif
