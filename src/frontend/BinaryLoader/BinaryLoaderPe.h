#ifndef ROSE_BINARYLOADERPE_H
#define ROSE_BINARYLOADERPE_H

#include "BinaryLoader.h"

class BinaryLoaderPe: public BinaryLoader {
public:
    BinaryLoaderPe() {}

    BinaryLoaderPe(const BinaryLoaderPe &other)
        : BinaryLoader(other)
        {}

    virtual ~BinaryLoaderPe() {}

    /* Override virtual methods from BinaryLoader */
public:
    virtual BinaryLoaderPe *clone() const {
        return new BinaryLoaderPe(*this);
    }

    virtual bool can_load(SgAsmGenericHeader*) const;

    /** Returns sections in order of their definition in the PE Section Table. */
    virtual SgAsmGenericSectionPtrList get_remap_sections(SgAsmGenericHeader*);

    /** Windows-specific PE section alignment. */
    virtual MappingContribution align_values(SgAsmGenericSection*, MemoryMap*,
                                             rose_addr_t *malign_lo, rose_addr_t *malign_hi,
                                             rose_addr_t *va, rose_addr_t *mem_size,
                                             rose_addr_t *offset, rose_addr_t *file_size,
                                             rose_addr_t *va_offset, bool *anon_lo, bool *anon_hi,
                                             ConflictResolution *resolve);
};

#endif /*ROSE_BINARYLOADERPE_H*/
