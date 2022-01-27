#ifndef ROSE_BinaryAnalysis_BinaryLoaderPe_H
#define ROSE_BinaryAnalysis_BinaryLoaderPe_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/BinaryLoader.h>

namespace Rose {
namespace BinaryAnalysis {

/** Reference counting pointer to @ref BinaryLoaderPe. */
typedef Sawyer::SharedPointer<class BinaryLoaderPe> BinaryLoaderPePtr;

/** Loader for Windows PE files. */
class BinaryLoaderPe: public BinaryLoader {
public:
    /** Reference counting pointer to @ref BinaryLoaderPe. */
    typedef Sawyer::SharedPointer<BinaryLoaderPe> Ptr;

protected:
    BinaryLoaderPe() {}

    BinaryLoaderPe(const BinaryLoaderPe &other)
        : BinaryLoader(other)
        {}

public:
    /** Allocating constructor. */
    static Ptr instance() {
        return Ptr(new BinaryLoaderPe);
    }

    virtual BinaryLoaderPtr clone() const override {
        return BinaryLoaderPtr(new BinaryLoaderPe(*this));
    }

    virtual ~BinaryLoaderPe() {}

    /* Override virtual methods from BinaryLoader */
public:

    virtual bool canLoad(SgAsmGenericHeader*) const override;

    /** Returns a new, temporary base address which is greater than everything that's been mapped already. */
    virtual rose_addr_t rebase(const MemoryMap::Ptr&, SgAsmGenericHeader*, const SgAsmGenericSectionPtrList&) override;

    // Returns sections in order of their definition in the PE Section Table.
    virtual SgAsmGenericSectionPtrList getRemapSections(SgAsmGenericHeader*) override;

    /** Windows-specific PE section alignment. */
    virtual MappingContribution alignValues(SgAsmGenericSection*, const MemoryMap::Ptr&,
                                            rose_addr_t *malign_lo, rose_addr_t *malign_hi,
                                            rose_addr_t *va, rose_addr_t *mem_size,
                                            rose_addr_t *offset, rose_addr_t *file_size, bool *map_private,
                                            rose_addr_t *va_offset, bool *anon_lo, bool *anon_hi,
                                            ConflictResolution *resolve) override;
};

} // namespace
} // namespace

#endif
#endif
