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

    virtual void updateExecutablePath() override;
    virtual SgAsmInterpretation* parseMainExecutable(RSIM_Process*) override;
    virtual bool isSupportedArch(SgAsmGenericHeader*) override;
    virtual PtRegs initialRegistersArch(RSIM_Process*) override;
    virtual void loadSpecimenNative(RSIM_Process*, Rose::BinaryAnalysis::Disassembler*, int existingPid=-1) override;
    virtual void loadSpecimenArch(RSIM_Process*, SgAsmInterpretation*, const std::string &interpName) override;
    virtual void initializeStackArch(RSIM_Thread*, SgAsmGenericHeader *) override;
    virtual void initializeSimulatedOs(RSIM_Process*, SgAsmGenericHeader*) override;
    virtual void threadCreated(RSIM_Thread*) override;

private:
    void init();
};

#endif
