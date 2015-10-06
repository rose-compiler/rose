#ifndef ROSE_RSIM_Pentium4_H
#define ROSE_RSIM_Pentium4_H

#include "RSIM_Simulator.h"

class RSIM_Pentium4: public RSIM_Simulator {
    rose_addr_t initialEip_;                            // Where program starts executing
    rose_addr_t initialEsp_;                            // Initial stack pointer
public:
    RSIM_Pentium4()
        : initialEip_(0), initialEsp_(0x80000000) {
        init();
    }

    virtual void updateExecutablePath() ROSE_OVERRIDE;
    virtual SgAsmInterpretation* parseMainExecutable(RSIM_Process*) ROSE_OVERRIDE;
    virtual bool isSupportedArch(SgAsmGenericHeader*) ROSE_OVERRIDE;
    virtual PtRegs initialRegistersArch(RSIM_Process*) ROSE_OVERRIDE;
    virtual void loadSpecimenNative(RSIM_Process*, rose::BinaryAnalysis::Disassembler*, int existingPid=-1) ROSE_OVERRIDE;
    virtual void loadSpecimenArch(RSIM_Process*, SgAsmInterpretation*, const std::string &interpName) ROSE_OVERRIDE;
    virtual void initializeStackArch(RSIM_Thread*, SgAsmGenericHeader *) ROSE_OVERRIDE;
    virtual void initializeSimulatedOs(RSIM_Process*, SgAsmGenericHeader*) ROSE_OVERRIDE;
    virtual void threadCreated(RSIM_Thread*) ROSE_OVERRIDE;

private:
    void init();
};

#endif
