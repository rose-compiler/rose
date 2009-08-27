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
    virtual MemoryMap *load_code_sections(const SgAsmGenericSectionPtrList&, bool allow_overmap=true);
};

#endif
