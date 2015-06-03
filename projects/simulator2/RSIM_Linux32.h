#ifndef ROSE_RSIM_Linux32_H
#define ROSE_RSIM_Linux32_H

#include "RSIM_Linux.h"

/** Simulator for 32-bit Linux guests. */
class RSIM_Linux32: public RSIM_Linux {
    std::vector<uint32_t> auxv_;                       // auxv vector pushed onto initial stack; also used when dumping core
public:
    RSIM_Linux32() {
        init();
    }

    virtual bool isSupportedArch(SgAsmGenericHeader*) ROSE_OVERRIDE;
    virtual void loadSpecimenNative(RSIM_Process*, rose::BinaryAnalysis::Disassembler*) ROSE_OVERRIDE;
    virtual PtRegs initialRegistersArch() ROSE_OVERRIDE;
    virtual rose_addr_t pushAuxVector(RSIM_Process*, rose_addr_t sp, rose_addr_t execfn_va, SgAsmElfFileHeader*,
                                      FILE *trace) ROSE_OVERRIDE;

private:
    void init();
};

#endif /* ROSE_RSIM_Linux32_H */




