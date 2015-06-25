#ifndef ROSE_RSIM_Linux64_H
#define ROSE_RSIM_Linux64_H

#include "RSIM_Linux.h"

/** Simulator for 64-bit Linux guests. */
class RSIM_Linux64: public RSIM_Linux {
    std::vector<uint64_t> auxv_;
    PtRegs initialRegs_;
public:
    RSIM_Linux64() {
        init();
    }

    virtual bool isSupportedArch(SgAsmGenericHeader*) ROSE_OVERRIDE;
    virtual void loadSpecimenNative(RSIM_Process*, rose::BinaryAnalysis::Disassembler*) ROSE_OVERRIDE;
    virtual void initializeSimulatedOs(RSIM_Process*, SgAsmGenericHeader*) ROSE_OVERRIDE;
    virtual PtRegs initialRegistersArch() ROSE_OVERRIDE;
    virtual rose_addr_t pushAuxVector(RSIM_Process*, rose_addr_t sp, rose_addr_t execfn_va, SgAsmElfFileHeader*,
                                      FILE *trace) ROSE_OVERRIDE;

private:
    void init();

protected:
    // System calls specific to 64-bit Linux
    static void syscall_arch_prctl_enter(RSIM_Thread*, int);
    static void syscall_arch_prctl_body(RSIM_Thread*, int);
    static void syscall_arch_prctl_leave(RSIM_Thread*, int);

    static void syscall_ioctl_enter(RSIM_Thread*, int);
    static void syscall_ioctl_body(RSIM_Thread*, int);

    static void syscall_mmap_enter(RSIM_Thread*, int);
    static void syscall_mmap_body(RSIM_Thread*, int);
    static void syscall_mmap_leave(RSIM_Thread*, int);

    static void syscall_stat_enter(RSIM_Thread*, int);
    static void syscall_stat_body(RSIM_Thread*, int);
    static void syscall_stat_leave(RSIM_Thread*, int);

    static void syscall_writev_enter(RSIM_Thread*, int);
    static void syscall_writev_body(RSIM_Thread*, int);
};

#endif
