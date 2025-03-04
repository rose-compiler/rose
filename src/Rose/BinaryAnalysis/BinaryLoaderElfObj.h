#ifndef ROSE_BinaryAnalysis_BinaryLoaderElfObj_H
#define ROSE_BinaryAnalysis_BinaryLoaderElfObj_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/BinaryLoaderElf.h>

namespace Rose {
namespace BinaryAnalysis {

/** A loader suitable for ELF object files.
 *
 *  An ELF object file typically contains one section per function and each section has a preferred virtual address of zero.
 *  This loader will map these sections so they do not overlap in virtual memory. */
class BinaryLoaderElfObj: public BinaryLoaderElf {
public:
    /** Reference counting pointer to @ref BinaryLoaderElfObj. */
    using Ptr = BinaryLoaderElfObjPtr;

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

    virtual BinaryLoaderPtr clone() const override {
        return BinaryLoaderPtr(new BinaryLoaderElfObj(*this));
    }

    virtual bool canLoad(SgAsmGenericHeader*) const override;

    // Same as super class but appends those sections that are not mapped but which contain code.
    virtual SgAsmGenericSectionPtrList getRemapSections(SgAsmGenericHeader*) override;

    // Same as super class but relaxes alignment constraints for sections that are ELF Sections but not ELF Segments.
    virtual MappingContribution alignValues(SgAsmGenericSection*, const MemoryMap::Ptr&,
                                            Address *malign_lo, Address *malign_hi,
                                            Address *va, Address *mem_size,
                                            Address *offset, Address *file_size, bool *map_private,
                                            Address *va_offset, bool *anon_lo, bool *anon_hi,
                                            ConflictResolution *resolve) override;

    // choose permissions based on the section content.
    virtual unsigned mappingPermissions(SgAsmGenericSection*) const override;
};

} // namespace
} // namespace

#endif
#endif
