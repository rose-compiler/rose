#ifndef ROSE_RSIM_ColdFire_H
#define ROSE_RSIM_ColdFire_H

#include "RSIM_Simulator.h"

class RSIM_ColdFire: public RSIM_Simulator {
public:
    RSIM_ColdFire() {
        init();
    }

    virtual void updateExecutablePath() override;
    virtual SgAsmInterpretation* parseMainExecutable(RSIM_Process*) override;
    virtual bool isSupportedArch(SgAsmGenericHeader*) override;
    virtual PtRegs initialRegistersArch(RSIM_Process*) override;
    virtual void loadSpecimenNative(RSIM_Process*, Rose::BinaryAnalysis::Disassembler::Base*, int existingPid=-1) override;
    virtual void loadSpecimenArch(RSIM_Process*, SgAsmInterpretation*, const std::string &interpName) override;
    virtual void initializeStackArch(RSIM_Thread*, SgAsmGenericHeader *) override;
    virtual void initializeSimulatedOs(RSIM_Process*, SgAsmGenericHeader*) override;

private:
    void init();
};

#endif
