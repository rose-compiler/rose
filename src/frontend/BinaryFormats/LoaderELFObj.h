#ifndef ROSE_LOADER_ELFOBJ_H
#define ROSE_LOADER_ELFOBJ_H
#include "LoaderELF.h"
/** A loader suitable for ELF object files.
 *
 *  An ELF object file typically contains one section per function and each section has a preferred virtual address of zero.
 *  This loader will map these sections so they do not overlap in virtual memory. */
class LoaderELFObj: public LoaderELF {
public:
    virtual ~LoaderELFObj() {}
    virtual bool can_handle(SgAsmGenericHeader*);

    /* Identical to parent class but with this minor difference:  Any section that has no mapping information but is known to
     * contain code (SgAsmGenericSection::get_contains_code() is true) is mapped to an otherwise unused area of the virtual
     * memory. */
    virtual rose_addr_t align_values(SgAsmGenericSection*, Contribution, rose_addr_t *va, rose_addr_t *mem_size,
                                     rose_addr_t *offset, rose_addr_t *file_size, const MemoryMap *current);
};

#endif
