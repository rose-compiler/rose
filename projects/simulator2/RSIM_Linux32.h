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
    virtual void loadSpecimenNative(RSIM_Process*, rose::BinaryAnalysis::Disassembler*, int existingPid=-1) ROSE_OVERRIDE;
    virtual void initializeSimulatedOs(RSIM_Process*, SgAsmGenericHeader*) ROSE_OVERRIDE;
    virtual PtRegs initialRegistersArch() ROSE_OVERRIDE;
    virtual rose_addr_t pushAuxVector(RSIM_Process*, rose_addr_t sp, rose_addr_t execfn_va, SgAsmElfFileHeader*,
                                      FILE *trace) ROSE_OVERRIDE;

private:
    void init();

protected:
    // System calls specific to 32-bit Linux
    static void syscall_clock_getres_body(RSIM_Thread *t, int callno);
    static void syscall_clock_getres_enter(RSIM_Thread *t, int callno);
    static void syscall_clock_getres_leave(RSIM_Thread *t, int callno);
    static void syscall_clock_gettime_body(RSIM_Thread *t, int callno);
    static void syscall_clock_gettime_enter(RSIM_Thread *t, int callno);
    static void syscall_clock_gettime_leave(RSIM_Thread *t, int callno);
    static void syscall_clock_settime_body(RSIM_Thread *t, int callno);
    static void syscall_clock_settime_enter(RSIM_Thread *t, int callno);
    static void syscall_clone_body(RSIM_Thread *t, int callno);
    static void syscall_clone_enter(RSIM_Thread *t, int callno);
    static void syscall_clone_leave(RSIM_Thread *t, int callno);
    static void syscall_fchown32_body(RSIM_Thread *t, int callno);
    static void syscall_fchown32_enter(RSIM_Thread *t, int callno);
    static void syscall_fstatfs_body(RSIM_Thread *t, int callno);
    static void syscall_fstatfs_enter(RSIM_Thread *t, int callno);
    static void syscall_fstatfs_leave(RSIM_Thread *t, int callno);
    static void syscall_fstatfs64_body(RSIM_Thread *t, int callno);
    static void syscall_fstatfs64_enter(RSIM_Thread *t, int callno);
    static void syscall_fstatfs64_leave(RSIM_Thread *t, int callno);
    static void syscall_futex_body(RSIM_Thread *t, int callno);
    static void syscall_futex_enter(RSIM_Thread *t, int callno);
    static void syscall_futex_leave(RSIM_Thread *t, int callno);
    static void syscall_getdents_body(RSIM_Thread *t, int callno);
    static void syscall_getdents_enter(RSIM_Thread *t, int callno);
    static void syscall_getdents_leave(RSIM_Thread *t, int callno);
    static void syscall_getdents64_body(RSIM_Thread *t, int callno);
    static void syscall_getdents64_enter(RSIM_Thread *t, int callno);
    static void syscall_getdents64_leave(RSIM_Thread *t, int callno);
    static void syscall_getegid32_body(RSIM_Thread *t, int callno);
    static void syscall_getegid32_enter(RSIM_Thread *t, int callno);
    static void syscall_geteuid32_body(RSIM_Thread *t, int callno);
    static void syscall_geteuid32_enter(RSIM_Thread *t, int callno);
    static void syscall_getgid32_body(RSIM_Thread *t, int callno);
    static void syscall_getgid32_enter(RSIM_Thread *t, int callno);
    static void syscall_getgroups32_body(RSIM_Thread *t, int callno);
    static void syscall_getgroups32_enter(RSIM_Thread *t, int callno);
    static void syscall_getgroups32_leave(RSIM_Thread *t, int callno);
    static void syscall_getrlimit_body(RSIM_Thread *t, int callno);
    static void syscall_getrlimit_enter(RSIM_Thread *t, int callno);
    static void syscall_getrlimit_leave(RSIM_Thread *t, int callno);
    static void syscall_gettimeofday_body(RSIM_Thread *t, int callno);
    static void syscall_gettimeofday_enter(RSIM_Thread *t, int callno);
    static void syscall_gettimeofday_leave(RSIM_Thread *t, int callno);
    static void syscall_getuid32_body(RSIM_Thread *t, int callno);
    static void syscall_getuid32_enter(RSIM_Thread *t, int callno);
    static void syscall_ioctl_body(RSIM_Thread *t, int callno);
    static void syscall_ioctl_enter(RSIM_Thread *t, int callno);
    static void syscall_ioctl_leave(RSIM_Thread *t, int callno);
    static void syscall_ipc_body(RSIM_Thread *t, int callno);
    static void syscall_ipc_enter(RSIM_Thread *t, int callno);
    static void syscall_ipc_leave(RSIM_Thread *t, int callno);
    static void syscall_llseek_body(RSIM_Thread *t, int callno);
    static void syscall_llseek_enter(RSIM_Thread *t, int callno);
    static void syscall_mmap_body(RSIM_Thread *t, int callno);
    static void syscall_mmap_enter(RSIM_Thread *t, int callno);
    static void syscall_mmap_leave(RSIM_Thread *t, int callno);
    static void syscall_mmap2_body(RSIM_Thread *t, int callno);
    static void syscall_mmap2_enter(RSIM_Thread *t, int callno);
    static void syscall_mmap2_leave(RSIM_Thread *t, int callno);
    static void syscall_modify_ldt_body(RSIM_Thread *t, int callno);
    static void syscall_modify_ldt_enter(RSIM_Thread *t, int callno);
    static void syscall_modify_ldt_leave(RSIM_Thread *t, int callno);
    static void syscall_msync_body(RSIM_Thread *t, int callno);
    static void syscall_msync_enter(RSIM_Thread *t, int callno);
    static void syscall_prctl_body(RSIM_Thread *t, int callno);
    static void syscall_prctl_enter(RSIM_Thread *t, int callno);
    static void syscall_pread64_body(RSIM_Thread *t, int callno);
    static void syscall_pread64_enter(RSIM_Thread *t, int callno);
    static void syscall_pread64_leave(RSIM_Thread *t, int callno);
    static void syscall_rt_sigpending_body(RSIM_Thread *t, int callno);
    static void syscall_rt_sigpending_enter(RSIM_Thread *t, int callno);
    static void syscall_rt_sigpending_leave(RSIM_Thread *t, int callno);
    static void syscall_rt_sigreturn_body(RSIM_Thread *t, int callno);
    static void syscall_rt_sigreturn_enter(RSIM_Thread *t, int callno);
    static void syscall_rt_sigreturn_leave(RSIM_Thread *t, int callno);
    static void syscall_rt_sigsuspend_body(RSIM_Thread *t, int callno);
    static void syscall_rt_sigsuspend_enter(RSIM_Thread *t, int callno);
    static void syscall_rt_sigsuspend_leave(RSIM_Thread *t, int callno);
    static void syscall_sched_getaffinity_body(RSIM_Thread *t, int callno);
    static void syscall_sched_getaffinity_enter(RSIM_Thread *t, int callno);
    static void syscall_sched_getaffinity_leave(RSIM_Thread *t, int callno);
    static void syscall_sched_setparam_body(RSIM_Thread *t, int callno);
    static void syscall_sched_setparam_enter(RSIM_Thread *t, int callno);
    static void syscall_sched_setscheduler_body(RSIM_Thread *t, int callno);
    static void syscall_sched_setscheduler_enter(RSIM_Thread *t, int callno);
    static void syscall_select_body(RSIM_Thread *t, int callno);
    static void syscall_select_enter(RSIM_Thread *t, int callno);
    static void syscall_select_leave(RSIM_Thread *t, int callno);
    static void syscall_setgroups32_body(RSIM_Thread *t, int callno);
    static void syscall_setgroups32_enter(RSIM_Thread *t, int callno);
    static void syscall_set_thread_area_body(RSIM_Thread *t, int callno);
    static void syscall_set_thread_area_enter(RSIM_Thread *t, int callno);
    static void syscall_set_thread_area_leave(RSIM_Thread *t, int callno);
    static void syscall_setrlimit_body(RSIM_Thread *t, int callno);
    static void syscall_setrlimit_enter(RSIM_Thread *t, int callno);
    static void syscall_sigaltstack_body(RSIM_Thread *t, int callno);
    static void syscall_sigaltstack_enter(RSIM_Thread *t, int callno);
    static void syscall_sigaltstack_leave(RSIM_Thread *t, int callno);
    static void syscall_sigreturn_body(RSIM_Thread *t, int callno);
    static void syscall_sigreturn_enter(RSIM_Thread *t, int callno);
    static void syscall_sigreturn_leave(RSIM_Thread *t, int callno);
    static void syscall_socketcall_body(RSIM_Thread *t, int callno);
    static void syscall_socketcall_enter(RSIM_Thread *t, int callno);
    static void syscall_socketcall_leave(RSIM_Thread *t, int callno);
    static void syscall_stat64_body(RSIM_Thread *t, int callno);
    static void syscall_stat64_enter(RSIM_Thread *t, int callno);
    static void syscall_stat64_leave(RSIM_Thread *t, int callno);
    static void syscall_statfs_body(RSIM_Thread *t, int callno);
    static void syscall_statfs_enter(RSIM_Thread *t, int callno);
    static void syscall_statfs_leave(RSIM_Thread *t, int callno);
    static void syscall_statfs64_body(RSIM_Thread *t, int callno);
    static void syscall_statfs64_enter(RSIM_Thread *t, int callno);
    static void syscall_statfs64_leave(RSIM_Thread *t, int callno);
    static void syscall_sysinfo_body(RSIM_Thread *t, int callno);
    static void syscall_sysinfo_enter(RSIM_Thread *t, int callno);
    static void syscall_time_body(RSIM_Thread *t, int callno);
    static void syscall_time_enter(RSIM_Thread *t, int callno);
    static void syscall_time_leave(RSIM_Thread *t, int callno);
    static void syscall_ugetrlimit_body(RSIM_Thread *t, int callno);
    static void syscall_ugetrlimit_enter(RSIM_Thread *t, int callno);
    static void syscall_ugetrlimit_leave(RSIM_Thread *t, int callno);
    static void syscall_uname_body(RSIM_Thread *t, int callno);
    static void syscall_uname_enter(RSIM_Thread *t, int callno);
    static void syscall_uname_leave(RSIM_Thread *t, int callno);
    static void syscall_utime_body(RSIM_Thread *t, int callno);
    static void syscall_utime_enter(RSIM_Thread *t, int callno);
    static void syscall_utimes_body(RSIM_Thread *t, int callno);
    static void syscall_utimes_enter(RSIM_Thread *t, int callno);
    static void syscall_wait4_body(RSIM_Thread *t, int callno);
    static void syscall_wait4_enter(RSIM_Thread *t, int callno);
    static void syscall_wait4_leave(RSIM_Thread *t, int callno);
    static void syscall_waitpid_body(RSIM_Thread *t, int callno);
    static void syscall_waitpid_enter(RSIM_Thread *t, int callno);
    static void syscall_waitpid_leave(RSIM_Thread *t, int callno);
    static void syscall_writev_body(RSIM_Thread *t, int callno);
    static void syscall_writev_enter(RSIM_Thread *t, int callno);

};

#endif /* ROSE_RSIM_Linux32_H */




