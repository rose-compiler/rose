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
public:
    virtual BinaryLoaderElfObj *clone() const {
        return new BinaryLoaderElfObj(*this);
    }
    virtual bool can_load(SgAsmGenericHeader*) const;
protected:
    virtual rose_addr_t align_values(SgAsmGenericSection*, Contribution, rose_addr_t *va, rose_addr_t *mem_size,
                                     rose_addr_t *offset, rose_addr_t *file_size, const MemoryMap *current);
};

#endif /*ROSE_BINARYLOADERELFOBJ_H*/
