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
    virtual void loadVsyscalls(RSIM_Process*) ROSE_OVERRIDE;
    virtual void loadSpecimenNative(RSIM_Process*, rose::BinaryAnalysis::Disassembler*, int existingPid=-1) ROSE_OVERRIDE;
    virtual void initializeSimulatedOs(RSIM_Process*, SgAsmGenericHeader*) ROSE_OVERRIDE;
    virtual PtRegs initialRegistersArch(RSIM_Process*) ROSE_OVERRIDE;
    virtual rose_addr_t pushAuxVector(RSIM_Process*, rose_addr_t sp, rose_addr_t execfn_va, SgAsmElfFileHeader*,
                                      FILE *trace) ROSE_OVERRIDE;

private:
    void init();

protected:
    // System calls specific to 64-bit Linux
    static void syscall_accept_enter(RSIM_Thread*, int callno);
    static void syscall_accept_body(RSIM_Thread*, int callno);
    static void syscall_accept_leave(RSIM_Thread*, int callno);

    static void syscall_arch_prctl_enter(RSIM_Thread*, int);
    static void syscall_arch_prctl_body(RSIM_Thread*, int);
    static void syscall_arch_prctl_leave(RSIM_Thread*, int);

    static void syscall_bind_enter(RSIM_Thread*, int callno);
    static void syscall_bind_body(RSIM_Thread*, int callno);

    static void syscall_connect_enter(RSIM_Thread*, int callno);
    static void syscall_connect_body(RSIM_Thread*, int callno);

    static void syscall_fstatfs_enter(RSIM_Thread *t, int callno);
    static void syscall_fstatfs_body(RSIM_Thread *t, int callno);
    static void syscall_fstatfs_leave(RSIM_Thread *t, int callno);

    static void syscall_futex_enter(RSIM_Thread *t, int callno);
    static void syscall_futex_body(RSIM_Thread *t, int callno);
    static void syscall_futex_leave(RSIM_Thread *t, int callno);

    static void syscall_getdents_enter(RSIM_Thread *t, int callno);
    static void syscall_getdents_body(RSIM_Thread *t, int callno);
    static void syscall_getdents_leave(RSIM_Thread *t, int callno);

    static void syscall_getrlimit_enter(RSIM_Thread *t, int callno);
    static void syscall_getrlimit_body(RSIM_Thread *t, int callno);
    static void syscall_getrlimit_leave(RSIM_Thread *t, int callno);

    static void syscall_gettimeofday_enter(RSIM_Thread *t, int callno);
    static void syscall_gettimeofday_body(RSIM_Thread *t, int callno);
    static void syscall_gettimeofday_leave(RSIM_Thread *t, int callno);

    static void syscall_ioctl_enter(RSIM_Thread*, int);
    static void syscall_ioctl_body(RSIM_Thread*, int);

    static void syscall_listen_enter(RSIM_Thread *t, int callno);
    static void syscall_listen_body(RSIM_Thread *t, int callno);

    static void syscall_mmap_enter(RSIM_Thread*, int);
    static void syscall_mmap_body(RSIM_Thread*, int);
    static void syscall_mmap_leave(RSIM_Thread*, int);

    static void syscall_setsockopt_enter(RSIM_Thread*, int callno);
    static void syscall_setsockopt_body(RSIM_Thread*, int callno);

    static void syscall_socket_enter(RSIM_Thread*, int callno);
    static void syscall_socket_body(RSIM_Thread*, int callno);

    static void syscall_stat_enter(RSIM_Thread*, int);
    static void syscall_stat_body(RSIM_Thread*, int);
    static void syscall_stat_leave(RSIM_Thread*, int);

    static void syscall_sysinfo_enter(RSIM_Thread *t, int callno);
    static void syscall_sysinfo_body(RSIM_Thread *t, int callno);
    static void syscall_sysinfo_leave(RSIM_Thread *t, int callno);

    static void syscall_time_enter(RSIM_Thread *t, int callno);
    static void syscall_time_body(RSIM_Thread *t, int callno);
    static void syscall_time_leave(RSIM_Thread *t, int callno);

    static void syscall_uname_enter(RSIM_Thread *t, int callno);
    static void syscall_uname_body(RSIM_Thread *t, int callno);
    static void syscall_uname_leave(RSIM_Thread *t, int callno);

    static void syscall_wait4_enter(RSIM_Thread *t, int callno);
    static void syscall_wait4_body(RSIM_Thread *t, int callno);
    static void syscall_wait4_leave(RSIM_Thread *t, int callno);

    static void syscall_writev_enter(RSIM_Thread*, int);
    static void syscall_writev_body(RSIM_Thread*, int);
};

#endif
