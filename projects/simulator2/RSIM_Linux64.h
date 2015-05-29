#ifndef ROSE_RSIM_Linux64_H
#define ROSE_RSIM_Linux64_H

#include "RSIM_Linux.h"

/** Simulator for 64-bit Linux guests. */
class RSIM_Linux64: public RSIM_Linux {
public:
    RSIM_Linux64() {
        init();
    }

    virtual bool isSupportedArch(SgAsmGenericHeader*) ROSE_OVERRIDE;
    virtual rose_addr_t pushAuxVector(RSIM_Process*, rose_addr_t sp, rose_addr_t execfn_va, SgAsmElfFileHeader*,
                                      FILE *trace) ROSE_OVERRIDE;

private:
    void init();
};

#endif
