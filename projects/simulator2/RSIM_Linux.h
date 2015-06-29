#ifndef ROSE_RSIM_Linux_H
#define ROSE_RSIM_Linux_H

#include "RSIM_Simulator.h"

class RSIM_Linux: public RSIM_Simulator {
    rose_addr_t interpreterBaseVa_;                     // base address for dynamic linker
    std::string vdsoName_;                              // Optional base name of virtual dynamic shared object from kernel
    rose_addr_t vdsoMappedVa_;                          // Address where vdso is mapped into specimen, or zero
    rose_addr_t vdsoEntryVa_;                           // Entry address for vdso, or zero
public:
    RSIM_Linux(): interpreterBaseVa_(0), vdsoName_("x86vdso"), vdsoMappedVa_(0), vdsoEntryVa_(0) {
        init();
    }

    rose_addr_t interpreterBaseVa() const { return interpreterBaseVa_; }
    void interpreterBaseVa(rose_addr_t va) { interpreterBaseVa_ = va; }
    rose_addr_t vdsoMappedVa() const { return vdsoMappedVa_; }
    rose_addr_t vdsoEntryVa() const { return vdsoEntryVa_; }

    virtual void loadSpecimenArch(RSIM_Process*, SgAsmInterpretation*, const std::string &interpName) ROSE_OVERRIDE;
    virtual void initializeStackArch(RSIM_Thread*, SgAsmGenericHeader *) ROSE_OVERRIDE;
    virtual void initializeSimulatedOs(RSIM_Process*, SgAsmGenericHeader*) ROSE_OVERRIDE;

    /** Push auxv onto the stack. */
    virtual rose_addr_t pushAuxVector(RSIM_Process*, rose_addr_t sp, rose_addr_t execfn_va, SgAsmElfFileHeader*,
                                      FILE *trace) = 0;

private:
    void init();

    template<typename Word>
    rose_addr_t pushArgcArgvEnvAuxv(RSIM_Process*, FILE *trace, SgAsmElfFileHeader*, rose_addr_t sp, rose_addr_t execfn_va);

protected:
    rose_addr_t segmentTableVa(SgAsmElfFileHeader *fhdr) const;

    // System calls that are common to both Linux 32- and 64-bit.
    static void syscall_default_leave(RSIM_Thread*, int);

    static void syscall_accept_enter(RSIM_Thread*, int callno);
    static void syscall_accept_body(RSIM_Thread*, int callno);
    static void syscall_accept_helper(RSIM_Thread*, int guestSrcFd, rose_addr_t addrVa, rose_addr_t addrLenVa, unsigned flags);

    static void syscall_access_enter(RSIM_Thread *t, int callno);
    static void syscall_access_body(RSIM_Thread *t, int callno);

    static void syscall_alarm_enter(RSIM_Thread *t, int callno);
    static void syscall_alarm_body(RSIM_Thread *t, int callno);

    static void syscall_bind_enter(RSIM_Thread*, int callno);
    static void syscall_bind_body(RSIM_Thread*, int callno);
    static void syscall_bind_helper(RSIM_Thread*, int guestFd, rose_addr_t addr_va, size_t addrlen);

    static void syscall_brk_enter(RSIM_Thread*, int);
    static void syscall_brk_body(RSIM_Thread*, int);
    static void syscall_brk_leave(RSIM_Thread*, int);

    static void syscall_chdir_enter(RSIM_Thread *t, int callno);
    static void syscall_chdir_body(RSIM_Thread *t, int callno);

    static void syscall_chmod_enter(RSIM_Thread *t, int callno);
    static void syscall_chmod_body(RSIM_Thread *t, int callno);

    static void syscall_chown_enter(RSIM_Thread *t, int callno);
    static void syscall_chown_body(RSIM_Thread *t, int callno);

    static void syscall_close_enter(RSIM_Thread *t, int callno);
    static void syscall_close_body(RSIM_Thread *t, int callno);

    static void syscall_connect_enter(RSIM_Thread*, int callno);
    static void syscall_connect_body(RSIM_Thread*, int callno);
    static void syscall_connect_helper(RSIM_Thread *t, int guestFd, rose_addr_t addr_va, size_t addrlen);

    static void syscall_creat_enter(RSIM_Thread *t, int callno);
    static void syscall_creat_body(RSIM_Thread *t, int callno);

    static void syscall_dup_enter(RSIM_Thread *t, int callno);
    static void syscall_dup_body(RSIM_Thread *t, int callno);

    static void syscall_dup2_enter(RSIM_Thread *t, int callno);
    static void syscall_dup2_body(RSIM_Thread *t, int callno);

    static void syscall_execve_enter(RSIM_Thread *t, int callno);
    static void syscall_execve_body(RSIM_Thread *t, int callno);

    static void syscall_exit_enter(RSIM_Thread *t, int callno);
    static void syscall_exit_body(RSIM_Thread *t, int callno);
    static void syscall_exit_leave(RSIM_Thread *t, int callno);

    static void syscall_exit_group_enter(RSIM_Thread *t, int callno);
    static void syscall_exit_group_body(RSIM_Thread *t, int callno);
    static void syscall_exit_group_leave(RSIM_Thread *t, int callno);

    static void syscall_fchdir_enter(RSIM_Thread *t, int callno);
    static void syscall_fchdir_body(RSIM_Thread *t, int callno);

    static void syscall_fchmod_enter(RSIM_Thread *t, int callno);
    static void syscall_fchmod_body(RSIM_Thread *t, int callno);

    static void syscall_fchmodat_enter(RSIM_Thread *t, int callno);
    static void syscall_fchmodat_body(RSIM_Thread *t, int callno);

    static void syscall_fchown_enter(RSIM_Thread *t, int callno);
    static void syscall_fchown_body(RSIM_Thread *t, int callno);

    static void syscall_fcntl_enter(RSIM_Thread *t, int callno);
    static void syscall_fcntl_body(RSIM_Thread *t, int callno);
    static void syscall_fcntl_leave(RSIM_Thread *t, int callno);

    static void syscall_fsync_enter(RSIM_Thread *t, int callno);
    static void syscall_fsync_body(RSIM_Thread *t, int callno);

    static void syscall_ftruncate_enter(RSIM_Thread *t, int callno);
    static void syscall_ftruncate_body(RSIM_Thread *t, int callno);

    static void syscall_getcwd_enter(RSIM_Thread *t, int callno);
    static void syscall_getcwd_body(RSIM_Thread *t, int callno);
    static void syscall_getcwd_leave(RSIM_Thread *t, int callno);

    static void syscall_getegid_enter(RSIM_Thread *t, int callno);
    static void syscall_getegid_body(RSIM_Thread *t, int callno);

    static void syscall_geteuid_enter(RSIM_Thread *t, int callno);
    static void syscall_geteuid_body(RSIM_Thread *t, int callno);

    static void syscall_getgid_enter(RSIM_Thread *t, int callno);
    static void syscall_getgid_body(RSIM_Thread *t, int callno);

    static void syscall_getpgrp_enter(RSIM_Thread *t, int callno);
    static void syscall_getpgrp_body(RSIM_Thread *t, int callno);

    static void syscall_getpid_enter(RSIM_Thread *t, int callno);
    static void syscall_getpid_body(RSIM_Thread *t, int callno);

    static void syscall_getppid_enter(RSIM_Thread *t, int callno);
    static void syscall_getppid_body(RSIM_Thread *t, int callno);

    static void syscall_gettid_enter(RSIM_Thread *t, int callno);
    static void syscall_gettid_body(RSIM_Thread *t, int callno);

    static void syscall_getuid_enter(RSIM_Thread *t, int callno);
    static void syscall_getuid_body(RSIM_Thread *t, int callno);

    static void syscall_kill_enter(RSIM_Thread *t, int callno);
    static void syscall_kill_body(RSIM_Thread *t, int callno);

    static void syscall_link_enter(RSIM_Thread *t, int callno);
    static void syscall_link_body(RSIM_Thread *t, int callno);

    static void syscall_listen_enter(RSIM_Thread *t, int callno);
    static void syscall_listen_body(RSIM_Thread *t, int callno);
    static void syscall_listen_helper(RSIM_Thread *t, int guestFd, int backlog);
    
    static void syscall_lseek_enter(RSIM_Thread *t, int callno);
    static void syscall_lseek_body(RSIM_Thread *t, int callno);

    static void syscall_madvise_enter(RSIM_Thread *t, int callno);
    static void syscall_madvise_body(RSIM_Thread *t, int callno);

    static void syscall_mkdir_enter(RSIM_Thread *t, int callno);
    static void syscall_mkdir_body(RSIM_Thread *t, int callno);

    static void syscall_mknod_enter(RSIM_Thread *t, int callno);
    static void syscall_mknod_body(RSIM_Thread *t, int callno);

    static void syscall_mprotect_enter(RSIM_Thread *t, int callno);
    static void syscall_mprotect_body(RSIM_Thread *t, int callno);
    static void syscall_mprotect_leave(RSIM_Thread *t, int callno);

    static void syscall_munmap_enter(RSIM_Thread *t, int callno);
    static void syscall_munmap_body(RSIM_Thread *t, int callno);

    static void syscall_nanosleep_enter(RSIM_Thread *t, int callno);
    static void syscall_nanosleep_body(RSIM_Thread *t, int callno);
    static void syscall_nanosleep_leave(RSIM_Thread *t, int callno);

    static void syscall_open_enter(RSIM_Thread *t, int callno);
    static void syscall_open_body(RSIM_Thread *t, int callno);

    static void syscall_pause_enter(RSIM_Thread *t, int callno);
    static void syscall_pause_body(RSIM_Thread *t, int callno);
    static void syscall_pause_leave(RSIM_Thread *t, int callno);

    static void syscall_pipe_enter(RSIM_Thread *t, int callno);
    static void syscall_pipe_body(RSIM_Thread *t, int callno);
    static void syscall_pipe_leave(RSIM_Thread *t, int callno);

    static void syscall_pipe2_enter(RSIM_Thread *t, int callno);
    static void syscall_pipe2_body(RSIM_Thread *t, int callno);
    static void syscall_pipe2_leave(RSIM_Thread *t, int callno);

    static void syscall_read_enter(RSIM_Thread *t, int callno);
    static void syscall_read_body(RSIM_Thread *t, int callno);
    static void syscall_read_leave(RSIM_Thread*, int callno);

    static void syscall_readlink_enter(RSIM_Thread *t, int callno);
    static void syscall_readlink_body(RSIM_Thread *t, int callno);

    static void syscall_rename_enter(RSIM_Thread *t, int callno);
    static void syscall_rename_body(RSIM_Thread *t, int callno);

    static void syscall_rmdir_enter(RSIM_Thread *t, int callno);
    static void syscall_rmdir_body(RSIM_Thread *t, int callno);

    static void syscall_rt_sigaction_enter(RSIM_Thread *t, int callno);
    static void syscall_rt_sigaction_body(RSIM_Thread *t, int callno);
    static void syscall_rt_sigaction_leave(RSIM_Thread *t, int callno);

    static void syscall_rt_sigprocmask_enter(RSIM_Thread *t, int callno);
    static void syscall_rt_sigprocmask_body(RSIM_Thread *t, int callno);
    static void syscall_rt_sigprocmask_leave(RSIM_Thread *t, int callno);

    static void syscall_sched_get_priority_max_enter(RSIM_Thread *t, int callno);
    static void syscall_sched_get_priority_max_body(RSIM_Thread *t, int callno);

    static void syscall_sched_get_priority_min_enter(RSIM_Thread *t, int callno);
    static void syscall_sched_get_priority_min_body(RSIM_Thread *t, int callno);

    static void syscall_sched_getscheduler_enter(RSIM_Thread *t, int callno);
    static void syscall_sched_getscheduler_body(RSIM_Thread *t, int callno);
    static void syscall_sched_getscheduler_leave(RSIM_Thread *t, int callno);

    static void syscall_sched_yield_enter(RSIM_Thread *t, int callno);
    static void syscall_sched_yield_body(RSIM_Thread *t, int callno);

    static void syscall_setpgid_enter(RSIM_Thread *t, int callno);
    static void syscall_setpgid_body(RSIM_Thread *t, int callno);

    static void syscall_setsockopt_enter(RSIM_Thread*, int callno);
    static void syscall_setsockopt_body(RSIM_Thread*, int callno);
    static void syscall_setsockopt_helper(RSIM_Thread*, int guestFd, int level, int optname, rose_addr_t optval_va, size_t optsz);

    static void syscall_socket_enter(RSIM_Thread*, int callno);
    static void syscall_socket_body(RSIM_Thread*, int callno);
    static void syscall_socket_helper(RSIM_Thread*, int family, int type, int protocol);

    static void syscall_symlink_enter(RSIM_Thread *t, int callno);
    static void syscall_symlink_body(RSIM_Thread *t, int callno);

    static void syscall_sync_enter(RSIM_Thread *t, int callno);
    static void syscall_sync_body(RSIM_Thread *t, int callno);

    static void syscall_tgkill_enter(RSIM_Thread *t, int callno);
    static void syscall_tgkill_body(RSIM_Thread *t, int callno);

    static void syscall_umask_enter(RSIM_Thread *t, int callno);
    static void syscall_umask_body(RSIM_Thread *t, int callno);

    static void syscall_unlink_enter(RSIM_Thread *t, int callno);
    static void syscall_unlink_body(RSIM_Thread *t, int callno);

    static void syscall_write_enter(RSIM_Thread *t, int callno);
    static void syscall_write_body(RSIM_Thread *t, int callno);
};



#endif
