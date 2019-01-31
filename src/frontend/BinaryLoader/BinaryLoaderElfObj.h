#ifndef ROSE_BinaryAnalysis_BinaryLoaderElfObj_H
#define ROSE_BinaryAnalysis_BinaryLoaderElfObj_H

#include "BinaryLoaderElf.h"

namespace Rose {
namespace BinaryAnalysis {

/** Reference counting pointer to @ref BinaryLoaderElfObj. */
typedef Sawyer::SharedPointer<class BinaryLoaderElfObj> BinaryLoaderElfObjPtr;

/** A loader suitable for ELF object files.
 *
 *  An ELF object file typically contains one section per function and each section has a preferred virtual address of zero.
 *  This loader will map these sections so they do not overlap in virtual memory. */
class BinaryLoaderElfObj: public BinaryLoaderElf {
public:
    /** Reference counting pointer to @ref BinaryLoaderElfObj. */
    typedef Sawyer::SharedPointer<class BinaryLoaderElfObj> Ptr;

protected:
    BinaryLoaderElfObj() {}

    BinaryLoaderElfObj(const BinaryLoaderElfObj &other)
        : BinaryLoaderElf(other)
        {}

public:
    /** Allocating constructor. */
    static Ptr instance() {
        return Ptr(new BinaryLoaderElfObj);
    }

    virtual ~BinaryLoaderElfObj() {}

    virtual BinaryLoaderPtr clone() const ROSE_OVERRIDE {
        return BinaryLoaderPtr(new BinaryLoaderElfObj(*this));
    }

    virtual bool canLoad(SgAsmGenericHeader*) const ROSE_OVERRIDE;

    // Same as super class but appends those sections that are not mapped but which contain code.
    virtual SgAsmGenericSectionPtrList getRemapSections(SgAsmGenericHeader*) ROSE_OVERRIDE;

    // Same as super class but relaxes alignment constraints for sections that are ELF Sections but not ELF Segments.
    virtual MappingContribution alignValues(SgAsmGenericSection*, const MemoryMap::Ptr&,
                                            rose_addr_t *malign_lo, rose_addr_t *malign_hi,
                                            rose_addr_t *va, rose_addr_t *mem_size,
                                            rose_addr_t *offset, rose_addr_t *file_size, bool *map_private,
                                            rose_addr_t *va_offset, bool *anon_lo, bool *anon_hi,
                                            ConflictResolution *resolve) ROSE_OVERRIDE;

    // choose permissions based on the section content.
    virtual unsigned mappingPermissions(SgAsmGenericSection*) const ROSE_OVERRIDE;
};

} // namespace
} // namespace

#endif
