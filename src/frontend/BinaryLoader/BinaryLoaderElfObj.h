#ifndef ROSE_BINARYLOADERELFOBJ_H
#define ROSE_BINARYLOADERELFOBJ_H

#include "BinaryLoaderElf.h"

/** A loader suitable for ELF object files.
 *
 *  An ELF object file typically contains one section per function and each section has a preferred virtual address of zero.
 *  This loader will map these sections so they do not overlap in virtual memory. */
class BinaryLoaderElfObj: public BinaryLoaderElf {
public:
    BinaryLoaderElfObj() {}

    BinaryLoaderElfObj(const BinaryLoaderElfObj &other)
        : BinaryLoaderElf(other)
        {}

    virtual ~BinaryLoaderElfObj() {}

    /* Override virtual methods from BinaryLoader */
    virtual BinaryLoaderElfObj *clone() const {
        return new BinaryLoaderElfObj(*this);
    }

    virtual bool can_load(SgAsmGenericHeader*) const;

    /** Same as super class but appends those sections that are not mapped but which contain code. */
    virtual SgAsmGenericSectionPtrList get_remap_sections(SgAsmGenericHeader*);

    /** Same as super class but relaxes alignment constraints for sections that are ELF Sections but not ELF Segments. */
    virtual MappingContribution align_values(SgAsmGenericSection*, MemoryMap*,
                                             rose_addr_t *malign_lo, rose_addr_t *malign_hi,
                                             rose_addr_t *va, rose_addr_t *mem_size,
                                             rose_addr_t *offset, rose_addr_t *file_size,
                                             rose_addr_t *va_offset, bool *anon_lo, bool *anon_hi, 
                                             ConflictResolution *resolve);
};

#endif /*ROSE_BINARYLOADERELFOBJ_H*/
