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

    /** Linux-specific ELF Segment and Section alignment. */
    virtual MappingContribution align_values(SgAsmGenericSection*, MemoryMap*,
                                             rose_addr_t *malign_lo, rose_addr_t *malign_hi,
                                             rose_addr_t *va, rose_addr_t *mem_size,
                                             rose_addr_t *offset, rose_addr_t *file_size,
                                             rose_addr_t *va_offset, bool *anon_lo, bool *anon_hi, 
                                             ConflictResolution *resolve);
};

#endif /*ROSE_BINARYLOADERELF_H*/
