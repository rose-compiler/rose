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

    static void syscall_access_enter(RSIM_Thread *t, int callno);
    static void syscall_access_body(RSIM_Thread *t, int callno);

    static void syscall_brk_enter(RSIM_Thread*, int);
    static void syscall_brk_body(RSIM_Thread*, int);
    static void syscall_brk_leave(RSIM_Thread*, int);

    static void syscall_close_enter(RSIM_Thread *t, int callno);
    static void syscall_close_body(RSIM_Thread *t, int callno);

    static void syscall_creat_enter(RSIM_Thread *t, int callno);
    static void syscall_creat_body(RSIM_Thread *t, int callno);

    static void syscall_dup_enter(RSIM_Thread *t, int callno);
    static void syscall_dup_body(RSIM_Thread *t, int callno);

    static void syscall_dup2_enter(RSIM_Thread *t, int callno);
    static void syscall_dup2_body(RSIM_Thread *t, int callno);

    static void syscall_exit_enter(RSIM_Thread *t, int callno);
    static void syscall_exit_body(RSIM_Thread *t, int callno);
    static void syscall_exit_leave(RSIM_Thread *t, int callno);

    static void syscall_exit_group_enter(RSIM_Thread *t, int callno);
    static void syscall_exit_group_body(RSIM_Thread *t, int callno);
    static void syscall_exit_group_leave(RSIM_Thread *t, int callno);

    static void syscall_mprotect_enter(RSIM_Thread *t, int callno);
    static void syscall_mprotect_body(RSIM_Thread *t, int callno);
    static void syscall_mprotect_leave(RSIM_Thread *t, int callno);

    static void syscall_munmap_enter(RSIM_Thread *t, int callno);
    static void syscall_munmap_body(RSIM_Thread *t, int callno);

    static void syscall_open_enter(RSIM_Thread *t, int callno);
    static void syscall_open_body(RSIM_Thread *t, int callno);

    static void syscall_pipe_enter(RSIM_Thread *t, int callno);
    static void syscall_pipe_body(RSIM_Thread *t, int callno);
    static void syscall_pipe_leave(RSIM_Thread *t, int callno);

    static void syscall_pipe2_enter(RSIM_Thread *t, int callno);
    static void syscall_pipe2_body(RSIM_Thread *t, int callno);
    static void syscall_pipe2_leave(RSIM_Thread *t, int callno);

    static void syscall_read_enter(RSIM_Thread *t, int callno);
    static void syscall_read_body(RSIM_Thread *t, int callno);
    static void syscall_read_leave(RSIM_Thread*, int callno);

    static void syscall_write_enter(RSIM_Thread *t, int callno);
    static void syscall_write_body(RSIM_Thread *t, int callno);
};



#endif
