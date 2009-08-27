#ifndef ROSE_LOADER_PE_H
#define ROSE_LOADER_PE_H

class LoaderPE: public Loader {
public:
    virtual ~LoaderPE() {}
    virtual bool can_handle(SgAsmGenericHeader *header) {
        return NULL!=isSgAsmPEFileHeader(header);
    }
    virtual SgAsmGenericSectionPtrList order_sections(const SgAsmGenericSectionPtrList&);
    virtual void align_values(SgAsmGenericSection*, rose_addr_t *va, rose_addr_t *size, rose_addr_t *offset);
};

#endif
