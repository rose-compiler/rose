#ifndef ROSE_BINARYLOADERELF_H
#define ROSE_BINARYLOADERELF_H

#include "BinaryLoader.h"

class BinaryLoaderElf: public BinaryLoader {
public:
    BinaryLoaderElf() {}

    BinaryLoaderElf(const BinaryLoaderElf &other)
        : BinaryLoader(other)
        {}

    virtual ~BinaryLoaderElf() {}

    /** Sets up library search paths and preloads from the environment.  The search paths and preloads are added to the end of
     *  the lists.  If an ELF file header is provided, then the DT_RPATH and DT_RUNPATH from the ".dynamic" section are also
     *  used.
     *
     *  Caveats:
     *  <ul>
     *    <li>The LD_PRELOAD and LD_LIBRARY_PATH environment variables are always consulted, even if the specimen is
     *        setuid.</li>
     *    <li>The library cache files (/etc/ld.so.*) are never consulted.</li>
     *    <li>No special behavior for specimens linked with "-z nodeflib" (not sure how to detect this.)</li>
     *    <li>The virtual dynamic shared object (vdso, linux-gate.so, etc) is not loaded.</li>
     *    <li>Since the environment variables that are consulted by this method are the very same ones used by the
     *        real loader-linker, it's not possible to fully control this method without also affecting the loading
     *        of ROSE itself.</li>
     *  </ul> */
    void add_lib_defaults(SgAsmGenericHeader *header=NULL);

    /** Returns the strings associated with certain variables in the ".dynamic" section. */
    static void get_dynamic_vars(SgAsmGenericHeader*, std::string &rpath/*out*/, std::string &runpath/*out*/);

    /* Override virtual methods from BinaryLoader */
public:
    virtual BinaryLoaderElf *clone() const {
        return new BinaryLoaderElf(*this);
    }
    virtual bool can_load(SgAsmGenericHeader*) const;
    virtual void fixup(SgAsmInterpretation *interp);
protected:
    /** Returns mappable sections in a particular order.  Returns ELF Segments in the order they are defined in the segment
     *  table, followed by ELF Sections in the order they are defined in the section table but excluding those sections that
     *  were already added to the list as ELF Segments. */
    virtual SgAsmGenericSectionPtrList get_remap_sections(SgAsmGenericHeader*);

    /** Returns a new, temporary base address which is greater than everything that's been mapped already. */
    virtual rose_addr_t rebase(MemoryMap*, SgAsmGenericHeader*, const SgAsmGenericSectionPtrList&);

    /** Linux-specific ELF Segment and Section alignment. */
    virtual MappingContribution align_values(SgAsmGenericSection*, MemoryMap*,
                                             rose_addr_t *malign_lo, rose_addr_t *malign_hi,
                                             rose_addr_t *va, rose_addr_t *mem_size,
                                             rose_addr_t *offset, rose_addr_t *file_size,
                                             rose_addr_t *va_offset, bool *anon_lo, bool *anon_hi, 
                                             ConflictResolution *resolve);
};

#endif /*ROSE_BINARYLOADERELF_H*/
