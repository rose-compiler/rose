#ifndef ROSE_RSIM_Linux_H
#define ROSE_RSIM_Linux_H

#include "RSIM_Simulator.h"

class RSIM_Linux: public RSIM_Simulator {
    rose_addr_t interpreterBaseVa_;                    // base address for dynamic linker
    std::string vdsoName_;                             // Optional base name of virtual dynamic shared object from kernel
    rose_addr_t vdsoMappedVa_;                         // Address where vdso is mapped into specimen, or zero
    rose_addr_t vdsoEntryVa_;                          // Entry address for vdso, or zero
public:
    RSIM_Linux(): interpreterBaseVa_(0x40000000), vdsoName_("x86vdso"), vdsoMappedVa_(0), vdsoEntryVa_(0) {
        init();
    }

    rose_addr_t interpreterBaseVa() const { return interpreterBaseVa_; }
    rose_addr_t vdsoMappedVa() const { return vdsoMappedVa_; }
    rose_addr_t vdsoEntryVa() const { return vdsoEntryVa_; }

    virtual void loadSpecimenArch(RSIM_Process*, SgAsmInterpretation*, const std::string &interpName) ROSE_OVERRIDE;
    virtual void initializeStackArch(RSIM_Thread*, SgAsmGenericHeader *, int argc, char *argv[]) ROSE_OVERRIDE;

    /** Push auxv onto the stack. */
    virtual rose_addr_t pushAuxVector(RSIM_Process*, rose_addr_t sp, rose_addr_t execfn_va, SgAsmElfFileHeader*,
                                      FILE *trace) = 0;

private:
    void init();
};

#endif
