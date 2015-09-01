#ifndef ROSE_RSIM_ColdFire_H
#define ROSE_RSIM_ColdFire_H

#include "RSIM_Simulator.h"

class RSIM_ColdFire: public RSIM_Simulator {
public:
    RSIM_ColdFire() {
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

private:
    void init();
};

#endif
