#ifndef ROSE_LOADER_ELF_H
#define ROSE_LOADER_ELF_H

/** An executable loader suitable for ELF binaries. */
class LoaderELF: public Loader {
public:
    virtual ~LoaderELF() {}
    virtual bool can_handle(SgAsmGenericHeader *header) {
        return NULL!=isSgAsmElfFileHeader(header);
    }
    virtual SgAsmGenericSectionPtrList order_sections(const SgAsmGenericSectionPtrList&);
    virtual rose_addr_t align_values(SgAsmGenericSection*, Contribution, rose_addr_t *va, rose_addr_t *mem_size,
                                     rose_addr_t *offset, rose_addr_t *file_size, const MemoryMap *current);
    virtual MemoryMap *load_code_sections(MemoryMap*, const SgAsmGenericSectionPtrList&, bool allow_overmap=true);
};

#endif
