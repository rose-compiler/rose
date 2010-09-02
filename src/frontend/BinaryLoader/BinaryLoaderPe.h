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
protected:
    virtual SgAsmGenericSectionPtrList order_sections(const SgAsmGenericSectionPtrList&);
    virtual rose_addr_t align_values(SgAsmGenericSection*, Contribution, rose_addr_t *va, rose_addr_t *mem_size,
                                     rose_addr_t *offset, rose_addr_t *file_size, const MemoryMap *current);
};

#endif /*ROSE_BINARYLOADERPE_H*/
