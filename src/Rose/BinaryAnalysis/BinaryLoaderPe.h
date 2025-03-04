#ifndef ROSE_BinaryAnalysis_BinaryLoaderPe_H
#define ROSE_BinaryAnalysis_BinaryLoaderPe_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/BinaryLoader.h>

namespace Rose {
namespace BinaryAnalysis {

/** Loader for Windows PE files. */
class BinaryLoaderPe: public BinaryLoader {
public:
    /** Reference counting pointer to @ref BinaryLoaderPe. */
    using Ptr = BinaryLoaderPePtr;

protected:
    BinaryLoaderPe() {
        performingDynamicLinking(true);
        performingRelocations(true);
        performingRemap(true);
    }

    BinaryLoaderPe(const BinaryLoaderPe &other)
        : BinaryLoader(other){
        performingDynamicLinking(true);
        performingRelocations(true);
        performingRemap(true);
    }

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

    // documented in superclass
    virtual void link(SgAsmInterpretation *interp) override;

    /** Returns a new, temporary base address which is greater than everything that's been mapped already. */
    virtual Address rebase(const MemoryMap::Ptr&, SgAsmGenericHeader*, const SgAsmGenericSectionPtrList&) override;

    // Resolve import section to DLL files.
    virtual void fixup(SgAsmInterpretation *interp, FixupErrors *errors=NULL) override;
    
    // Returns sections in order of their definition in the PE Section Table.
    virtual SgAsmGenericSectionPtrList getRemapSections(SgAsmGenericHeader*) override;

    /** Windows-specific PE section alignment. */
    virtual MappingContribution alignValues(SgAsmGenericSection*, const MemoryMap::Ptr&,
                                            Address *malign_lo, Address *malign_hi,
                                            Address *va, Address *mem_size,
                                            Address *offset, Address *file_size, bool *map_private,
                                            Address *va_offset, bool *anon_lo, bool *anon_hi,
                                            ConflictResolution *resolve) override;

    /** Returns true if the specified file name is already linked into the AST.
     *
     * @{ */
    virtual bool isLinked(SgBinaryComposite *composite, const std::string &filename) override;
    virtual bool isLinked(SgAsmInterpretation *interp, const std::string &filename) override;
    /** @} */

     /** Sets up library search paths and preloads from the environment.  The search paths and preloads are added to the end of
     *  the lists.  If a PE file header is provided, then only LD_PRELOAD and LD_LIBRARY_PATH are used. */
    void addLibDefaults(SgAsmGenericHeader *header=NULL);
                                            
    /** Convert name to fully qualified name.
     *
     *  Given the name of a shared object, return the fully qualified name where the library is located in the file system.
     *  Throws a BinaryLoader::Exception if the library cannot be found. */
    virtual std::string findSoFile(const std::string &libname) const override;
                                                
};

} // namespace
} // namespace

#endif
#endif
